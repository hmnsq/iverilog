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
#اگر_تعریف_شدہ CHECK_WITH_VALGRIND
# include  "vvp_cleanup.h"
#اگر_ختم
# include  <vector>
# include  <cstring>
# include  <cstdlib>
# include  <cassert>
# include  "ivl_alloc.h"

استعمال نامخلا std;

ساکن vector<vpiHandle> vpip_جڑ_جدول;

ساکن vpiHandle make_subset_iterator_(صحیح type_رمز, ثابت vector<vpiHandle>&جدول);

vpiHandle vpip_make_جڑ_iterator(صحیح type_رمز)
{
      واپس make_subset_iterator_(type_رمز, vpip_جڑ_جدول);
}

خالی vpip_make_جڑ_iterator(__vpiHandle**&جدول, غیرعلامتی&ntable)
{
      جدول = &vpip_جڑ_جدول[0];
      ntable = vpip_جڑ_جدول.size();
}

#اگر_تعریف_شدہ CHECK_WITH_VALGRIND
خالی پورٹ_delete(__vpiHandle*handle);
خالی پورٹ_ثنائی_ہندسہ_delete(__vpiHandle*handle);

/* Class definitions need to be cleaned up at the end. */
ساکن class_type **class_list = 0;
ساکن غیرعلامتی class_list_گنتی = 0;

ساکن خالی delete_sub_scopes(__vpiScope *scope)
{
      برائے (غیرعلامتی idx = 0; idx < scope->intern.size(); idx += 1) {
	    vpiHandle شے = (scope->intern)[idx];
	    __vpiScope*lscope = static_cast<__vpiScope*>(شے);
	    انتخاب(شے->get_type_رمز()) {
		حالت vpiFunction:
		حالت vpiTask:
		  contexts_delete(lscope);
		  // fallthrough
		حالت vpiModule:
		حالت vpiGenScope:
		حالت vpiNamedBegin:
		حالت vpiNamedFork:
		حالت vpiClassTypespec:
		  delete_sub_scopes(lscope);
		  vthreads_delete(lscope);
		  حذف شے;
		  توڑو;
		حالت vpi_حافظہ:
		حالت vpiNetArray:
		  حافظہ_delete(شے);
		  توڑو;
		حالت vpi_Mod_راستہ:
		    /* The destination ModPath is cleaned up later. */
		  حذف شے;
		  توڑو;
		حالت vpi_Inter_Mod_راستہ:
		  حذف شے;
		  توڑو;
		حالت vpiNamedEvent:
		  named_event_delete(شے);
		  توڑو;
		حالت vpiNet:
		حالت vpiReg:
		حالت vpiIntegerVar:
		حالت vpiLongIntVar:
		حالت vpiShortIntVar:
		حالت vpiIntVar:
		حالت vpiByteVar:
		حالت vpi_ثنائی_ہندسہ_Var:
		  signal_delete(شے);
		  توڑو;
		حالت vpiParameter:
		  parameter_delete(شے);
		  توڑو;
		حالت vpiRealVar:
		  real_delete(شے);
		  توڑو;
		حالت vpiEnumTypespec:
		  enum_delete(شے);
		  توڑو;
		حالت vpi_پورٹ:
		  پورٹ_delete(شے);
		  توڑو;
		حالت vpi_پورٹ_ثنائی_ہندسہ:
		  پورٹ_ثنائی_ہندسہ_delete(شے);
		  توڑو;
		حالت vpiStringVar:
		  string_delete(شے);
		  توڑو;
		حالت vpiClassVar:
		  class_delete(شے);
		  توڑو;
		حالت vpiArrayVar:
		  array_delete(شے);
		  توڑو;
		طےشدہ:
		  fprintf(stderr, "Need support for type: %d\n",
		          شے->get_type_رمز());
		  أكد(0);
		  توڑو;
	    }
      }
      scope->intern.clear();

	/* Save any class definitions to clean up later. */
      map<std::string, class_type*>::iterator citer;
      برائے (citer = scope->classes.begin();
           citer != scope->classes.انتہا(); ++ citer ) {
	    class_list_گنتی += 1;
	    class_list = static_cast<class_type **>
	                 (realloc(class_list,
	                  class_list_گنتی*حجم(class_type **)));
	    class_list[class_list_گنتی-1] = citer->second;
      }
}

خالی جڑ_جدول_delete(خالی)
{
      برائے (غیرعلامتی idx = 0; idx < vpip_جڑ_جدول.size(); idx += 1) {
	    __vpiScope *scope = static_cast<__vpiScope *>
	          (vpip_جڑ_جدول[idx]);
	    vthreads_delete(scope);
	    delete_sub_scopes(scope);
	    حذف scope;
      }
      vpip_جڑ_جدول.clear();

	/* Clean up all the class definitions. */
      برائے (غیرعلامتی idx = 0; idx < class_list_گنتی; idx += 1) {
            class_def_delete(class_list[idx]);
      }
      آزاد(class_list);
      class_list = 0;
      class_list_گنتی = 0;
}
#اگر_ختم

ساکن خالی construct_scope_fullname(__vpiScope*ref, حرف*buf)
{
      اگر (ref->scope) {
	    construct_scope_fullname(ref->scope, buf);
	      // Add a "." separator, unless this is for a package
	    اگر (ref->scope->get_type_رمز() != vpiPackage) {
		  strcat(buf, ".");
	    }
      }

      strcat(buf, ref->scope_name());
	// For a package, add a "::" to the end
      اگر (ref->get_type_رمز() == vpiPackage) strcat(buf, "::");
}

ساکن ثابت حرف* scope_get_type(صحیح code)
{
      انتخاب (code) {
          حالت vpiModule:
            واپس "vpiModule";
          حالت vpiGenScope:
            واپس "vpiGenScope";
          حالت vpiFunction:
            واپس "vpiFunction";
          حالت vpiTask:
            واپس "vpiTask";
          حالت vpiNamedBegin:
            واپس "vpiNamedBegin";
          حالت vpiNamedFork:
            واپس "vpiNamedFork";
          طےشدہ:
            fprintf(stderr, "VPI error: invalid scope type code %d.\n", code);
            واپس تہی;
      }
}

ساکن حرف* scope_get_str(صحیح code, vpiHandle obj)
{
      __vpiScope*ref = dynamic_cast<__vpiScope*>(obj);
      أكد(ref);

      حرف buf[4096];  // XXX is a fixed buffer size really reliable?
      ثابت حرف *p=0;
      انتخاب (code) {
	  حالت vpiDefFile:
	    p = file_نام[ref->def_file_idx];
	    توڑو;

	  حالت vpiFile:
	    p = file_نام[ref->file_idx];
	    توڑو;

	  حالت vpi_بھرا_Name:
	    buf[0] = 0;
	    construct_scope_fullname(ref, buf);
	    p = buf;
	    توڑو;

	  حالت vpiName:
	    p = ref->scope_name();
	    توڑو;

	  حالت vpiDefName:
	    p = ref->scope_def_name();
	    توڑو;

	  حالت vpiType:
	    p = scope_get_type(code);
	    توڑو;

	  طےشدہ:
	    fprintf(stderr, "VPI error: invalid scope string code %d.\n", code);
	    واپس تہی;
      }
      واپس simple_سیٹ_rbuf_str(p);
}

ساکن vpiHandle scope_get_handle(صحیح code, vpiHandle obj)
{
      __vpiScope*rfp = dynamic_cast<__vpiScope*>(obj);
      أكد(rfp);

      انتخاب (code) {

	  حالت vpiScope:
	    واپس rfp->scope;

	  حالت vpiModule:
	    واپس rfp->scope;
      }

      واپس 0;
}

/* compares vpiType's considering object classes */
ساکن صحیح compare_types(صحیح code, صحیح type)
{
	/* NOTE: The Verilog VPI does not for any object support
	   vpiScope as an iterator parameter, so it is used here as a
	   means to scan everything in the *current* scope. */
      اگر (code == vpiScope)
	    واپس 1;

      اگر (code == type)
	    واپس 1;

      اگر ( code == vpi_اندرونی_Scope &&
	     (type == vpiModule ||
	     type == vpiGenScope ||
	     type == vpiFunction ||
	     type == vpiTask ||
	     type == vpiNamedBegin ||
	     type == vpiNamedFork) )
	    واپس 1;

      اگر ( code == vpiInstance &&
	    (type == vpiModule ||
	     type == vpi_پروگرام ||
	     type == vpiInterface ||
	     type == vpiPackage) )
	    واپس 1;

      اگر ( code == vpiVariables &&
	     (type == vpiIntegerVar  ||
	      type == vpi_ثنائی_ہندسہ_Var      ||
	      type == vpiByteVar     ||
	      type == vpiShortIntVar ||
	      type == vpiIntVar      ||
	      type == vpiLongIntVar  ||
	      type == vpi_وقت_Var     ||
	      type == vpiRealVar))
	    واپس 1;

      واپس 0;
}

ساکن vpiHandle make_subset_iterator_(صحیح type_رمز, ثابت vector<vpiHandle>&جدول)
{
      غیرعلامتی mcnt = 0, ncnt = 0;
      vpiHandle*دلائل;

      برائے (غیرعلامتی idx = 0; idx < جدول.size(); idx += 1)
	    اگر (compare_types(type_رمز, جدول[idx]->get_type_رمز()))
		  mcnt += 1;

      اگر (mcnt == 0)
	    واپس 0;

      دلائل = static_cast<vpiHandle*>(صفر_حافظہ_دو(mcnt, حجم(vpiHandle)));
      برائے (غیرعلامتی idx = 0; idx < جدول.size(); idx += 1)
	    اگر (compare_types(type_رمز, جدول[idx]->get_type_رمز()))
		  دلائل[ncnt++] = جدول[idx];

      أكد(ncnt == mcnt);

      واپس vpip_make_iterator(mcnt, دلائل, سچ);
}

/*
 * This function implements the vpi_iterate method for vpiModule and
 * similar objects. The vpi_iterate allows the user to iterate over
 * things that are contained in the scope object, by generating an
 * iterator for the requested set of items.
 */
ساکن vpiHandle module_iter(صحیح code, vpiHandle obj)
{
      __vpiScope*ref = dynamic_cast<__vpiScope*>(obj);
      أكد(ref);

      واپس make_subset_iterator_(code, ref->intern);
}


__vpiScope::__vpiScope(ثابت حرف*nam, ثابت حرف*tnam, منطقی auto_flag)
: is_automatic_(auto_flag)
{
      name_ = vpip_name_string(nam);
      tname_ = vpip_name_string(tnam? tnam : "");
}

صحیح __vpiScope::vpi_get(صحیح code)
{
      انتخاب (code) {
	  حالت vpiCellInstance:
	    واپس is_cell? 1 : 0;

	  حالت vpi_Def_سطر_No:
	    واپس def_lineno;

	  حالت vpi_سطر_No:
	    واپس lineno;

	  حالت vpi_وقت_Unit:
	    واپس وقت_units;

	  حالت vpi_وقت_Precision:
	    واپس وقت_precision;

	  حالت vpi_چوٹی_Module:
	    واپس 0x0 == scope;

          حالت vpiAutomatic:
	    واپس is_automatic_? 1 : 0;
      }

      واپس vpiUndefined;
}


حرف*__vpiScope::vpi_get_str(صحیح code)
{ واپس scope_get_str(code, یہ); }

vpiHandle __vpiScope::vpi_handle(صحیح code)
{ واپس scope_get_handle(code, یہ); }

vpiHandle __vpiScope::vpi_iterate(صحیح code)
{ واپس module_iter(code, یہ); }


جماعت vpiScopeModule  : عوامی __vpiScope {
    عوامی:
      اندرونی vpiScopeModule(ثابت حرف*nam, ثابت حرف*tnam)
      : __vpiScope(nam,tnam,جھوٹ) { }
      صحیح get_type_رمز(خالی) ثابت override { واپس vpiModule; }
};

ساخت vpiScopePackage  : عوامی __vpiScope {
      اندرونی vpiScopePackage(ثابت حرف*nam, ثابت حرف*tnam)
      : __vpiScope(nam,tnam) { }
      صحیح get_type_رمز(خالی) ثابت override { واپس vpiPackage; }
};

ساخت vpiScopeTask  : عوامی __vpiScope {
      اندرونی vpiScopeTask(ثابت حرف*nam, ثابت حرف*tnam)
      : __vpiScope(nam,tnam) { }
      صحیح get_type_رمز(خالی) ثابت override { واپس vpiTask; }
};

ساخت vpiScopeTaskAuto  : عوامی __vpiScope {
      اندرونی vpiScopeTaskAuto(ثابت حرف*nam, ثابت حرف*tnam)
      : __vpiScope(nam,tnam,سچ) {  }
      صحیح get_type_رمز(خالی) ثابت override { واپس vpiTask; }
};

ساخت vpiScopeBegin  : عوامی __vpiScope {
      اندرونی vpiScopeBegin(ثابت حرف*nam, ثابت حرف*tnam)
      : __vpiScope(nam,tnam,جھوٹ) { }
      صحیح get_type_رمز(خالی) ثابت override { واپس vpiNamedBegin; }
};

جماعت vpiScopeBeginAuto  : عوامی __vpiScope {
    عوامی:
      اندرونی vpiScopeBeginAuto(ثابت حرف*nam, ثابت حرف*tnam)
      : __vpiScope(nam,tnam,سچ) {  }
      صحیح get_type_رمز(خالی) ثابت override { واپس vpiNamedBegin; }
};

ساخت vpiScopeGenerate  : عوامی __vpiScope {
      اندرونی vpiScopeGenerate(ثابت حرف*nam, ثابت حرف*tnam)
      : __vpiScope(nam,tnam) { }
      صحیح get_type_رمز(خالی) ثابت override { واپس vpiGenScope; }
};

ساخت vpiScopeFork  : عوامی __vpiScope {
      اندرونی vpiScopeFork(ثابت حرف*nam, ثابت حرف*tnam)
      : __vpiScope(nam,tnam,جھوٹ) { }
      صحیح get_type_رمز(خالی) ثابت override { واپس vpiNamedFork; }
};

جماعت vpiScopeForkAuto  : عوامی __vpiScope {
    عوامی:
      اندرونی vpiScopeForkAuto(ثابت حرف*nam, ثابت حرف*tnam)
      : __vpiScope(nam,tnam,سچ) {  }
      صحیح get_type_رمز(خالی) ثابت override { واپس vpiNamedFork; }
};

ساخت vpiScopeClass  : عوامی __vpiScope {
      اندرونی vpiScopeClass(ثابت حرف*nam, ثابت حرف*tnam)
      : __vpiScope(nam,tnam) { }
      صحیح get_type_رمز(خالی) ثابت override { واپس vpiClassTypespec; }
};

/*
 * The current_scope is a compile time concept. As the vvp source is
 * compiled, items that have scope are placed in the current
 * scope. The ".scope" directives select the scope that is current.
 */
ساکن __vpiScope*current_scope = 0;

خالی vpip_attach_مقصد_scope(__vpiScope*scope, vpiHandle obj)
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
خالی
compile_scope_decl(حرف*label, حرف*type, حرف*name, حرف*tname,
                   حرف*والد, طویل file_idx, طویل lineno,
                   طویل def_file_idx, طویل def_lineno, طویل is_cell)
{
      گنتی_vpi_scopes += 1;
      حرف vec_type;
      حرف sign_flag;
      غیرعلامتی wid;

      __vpiScope*scope;
      اگر (strcmp(type,"module") == 0) {
	    scope = نیا vpiScopeModule(name, tname);
      } ورنہ اگر ( لڑی_سے_پڑھو(type, "function.vec%c.%c%u", &vec_type, &sign_flag, &wid) == 3 ) {
	    صحیح type_رمز;
	    اگر (sign_flag=='s') {
		  type_رمز = vpiSizedSignedFunc;
	    } ورنہ اگر (sign_flag=='u') {
		  type_رمز = vpiSizedFunc;
	    } ورنہ اگر (sign_flag=='i') {
		  type_رمز = vpiIntFunc;
	    } ورنہ {
		  أكد(0);
		  type_رمز = vpiSizedFunc;
	    }
	    vvp_bit4_t init_val = vec_type == '4' ? BIT4_X : BIT4_0;
	    scope = نیا vpiScopeFunction(name, tname, جھوٹ, type_رمز, wid, init_val);

      } ورنہ اگر ( لڑی_سے_پڑھو(type, "autofunction.vec%c.%c%u", &vec_type, &sign_flag, &wid) == 3 ) {
	    صحیح type_رمز;
	    انتخاب (sign_flag) {
		حالت 's':
		  type_رمز = vpiSizedSignedFunc;
		  توڑو;
		حالت 'u':
		  type_رمز = vpiSizedFunc;
		  توڑو;
		طےشدہ:
		  أكد(0);
		  type_رمز = vpiSizedFunc;
		  توڑو;
	    }
	    vvp_bit4_t init_val = vec_type == '4' ? BIT4_X : BIT4_0;
	    scope = نیا vpiScopeFunction(name, tname, سچ, type_رمز, wid, init_val);

      } ورنہ اگر (strcmp(type,"function.obj") == 0) {
	    scope = نیا vpiScopeFunction(name, tname, جھوٹ, vpiSizedFunc, 0, BIT4_0);
      } ورنہ اگر (strcmp(type,"autofunction.obj") == 0) {
	    scope = نیا vpiScopeFunction(name, tname, سچ, vpiSizedFunc, 0, BIT4_0);
      } ورنہ اگر (strcmp(type,"function.real") == 0) {
	    scope = نیا vpiScopeFunction(name, tname, جھوٹ, vpiRealFunc, 0, BIT4_0);
      } ورنہ اگر (strcmp(type,"autofunction.real") == 0) {
	    scope = نیا vpiScopeFunction(name, tname, سچ, vpiRealFunc, 0, BIT4_0);
      } ورنہ اگر (strcmp(type,"function.str") == 0) {
	    scope = نیا vpiScopeFunction(name, tname, جھوٹ, vpiOtherFunc, 0, BIT4_0);
      } ورنہ اگر (strcmp(type,"autofunction.str") == 0) {
	    scope = نیا vpiScopeFunction(name, tname, سچ, vpiOtherFunc, 0, BIT4_0);
      } ورنہ اگر (strcmp(type,"function.void") == 0) {
	    scope = نیا vpiScopeFunction(name, tname, جھوٹ, vpiOtherFunc, 0, BIT4_0);
      } ورنہ اگر (strcmp(type,"autofunction.void") == 0) {
	    scope = نیا vpiScopeFunction(name, tname, سچ, vpiOtherFunc, 0, BIT4_0);
      } ورنہ اگر (strcmp(type,"task") == 0) {
	    scope = نیا vpiScopeTask(name, tname);
      } ورنہ اگر (strcmp(type,"autotask") == 0) {
	    scope = نیا vpiScopeTaskAuto(name, tname);
      } ورنہ اگر (strcmp(type,"fork") == 0) {
	    scope = نیا vpiScopeFork(name, tname);
      } ورنہ اگر (strcmp(type,"autofork") == 0) {
	    scope = نیا vpiScopeForkAuto(name, tname);
      } ورنہ اگر (strcmp(type,"begin") == 0) {
	    scope = نیا vpiScopeBegin(name, tname);
      } ورنہ اگر (strcmp(type,"autobegin") == 0) {
	    scope = نیا vpiScopeBeginAuto(name, tname);
      } ورنہ اگر (strcmp(type,"generate") == 0) {
	    scope = نیا vpiScopeGenerate(name, tname);
      } ورنہ اگر (strcmp(type,"package") == 0) {
	    scope = نیا vpiScopePackage(name, tname);
      } ورنہ اگر (strcmp(type,"class") == 0) {
	    scope = نیا vpiScopeClass(name, tname);
      } ورنہ {
	    scope = نیا vpiScopeModule(name, tname);
	    أكد(0);
      }

      scope->file_idx = (غیرعلامتی) file_idx;
      scope->lineno  = (غیرعلامتی) lineno;
      scope->def_file_idx = (غیرعلامتی) def_file_idx;
      scope->def_lineno  = (غیرعلامتی) def_lineno;
      scope->شے = 0;
      scope->nitem = 0;
      scope->زندہ_contexts = 0;
      scope->free_contexts = 0;

      اگر (is_cell) scope->is_cell = سچ;
      ورنہ scope->is_cell = جھوٹ;

      current_scope = scope;

      compile_vpi_علامت(label, scope);

      آزاد(label);
      آزاد(type);
      حذف[] name;
      حذف[] tname;

      اگر (والد) {
	    ساکن vpiHandle obj;
	    compile_vpi_lookup(&obj, والد);
	    أكد(obj);
	    __vpiScope*sp = dynamic_cast<__vpiScope*>(obj);
	    vpip_attach_مقصد_scope(sp, scope);
	    scope->scope = dynamic_cast<__vpiScope*>(obj);

	      /* Inherit time units and precision from the parent scope. */
	    scope->وقت_units = sp->وقت_units;
	    scope->وقت_precision = sp->وقت_precision;

      } ورنہ {
	    scope->scope = 0x0;

	    vpip_جڑ_جدول.push_back(scope);

	      /* Root scopes inherit time_units and precision from the
	         system precision. */
	    scope->وقت_units = vpip_get_وقت_precision();
	    scope->وقت_precision = vpip_get_وقت_precision();
      }
}

خالی compile_scope_recall(حرف*علامت)
{
	/* A __vpiScope starts with a __vpiHandle structure so this is
	   a safe cast. We need the (void*) to avoid a dereferenced
	   type punned pointer warning from some gcc compilers. */
      compile_vpi_lookup((vpiHandle*)(خالی*)&current_scope, علامت);
      أكد(current_scope);
}

/*
 * This function handles the ".timescale" directive in the vvp
 * source. It sets in the current scope the specified units value.
 */
خالی compile_timescale(طویل units, طویل precision)
{
      أكد(current_scope);
      current_scope->وقت_units = units;
      current_scope->وقت_precision = precision;
}

__vpiScope* vpip_peek_current_scope(خالی)
{
      واپس current_scope;
}

خالی vpip_attach_مقصد_current_scope(vpiHandle obj)
{
      vpip_attach_مقصد_scope(current_scope, obj);
}

__vpiScope* vpip_peek_context_scope(خالی)
{
      __vpiScope*scope = current_scope;

        /* A context is allocated for each automatic task or function.
           Storage for nested scopes (named blocks) is allocated in
           the parent context. */
      جب (scope->scope && scope->scope->is_automatic())
            scope = scope->scope;

      واپس scope;
}

غیرعلامتی vpip_جوڑو_شے_مقصد_context(automatic_hooks_s*شے,
                                  __vpiScope*scope)
{
      أكد(scope);
      أكد(scope->is_automatic());

      غیرعلامتی idx = scope->nitem++;

      اگر (scope->شے == 0)
	    scope->شے = static_cast<automatic_hooks_s**>
	                  (حافظہ_دو(حجم(automatic_hooks_s*)));
      ورنہ
	    scope->شے = static_cast<automatic_hooks_s**>
	                  (realloc(scope->شے,
	                   حجم(automatic_hooks_s*)*scope->nitem));

      scope->شے[idx] = شے;

        /* Offset the context index by 2 to leave space for the list links. */
      واپس 2 + idx;
}


vpi_پورٹ_Info::vpi_پورٹ_Info( __vpiScope *والد,
              غیرعلامتی اشاریہ,
              صحیح vpi_direction,
              غیرعلامتی width,
              ثابت حرف *name,
              حرف* بفر) :
      والد_2(والد),
      اشاریہ_2(اشاریہ),
      direction_(vpi_direction),
      چوڑائی_2(width),
      name_(name)
{
      اگر (بفر != خالی_اشاریہ) functor_ref_lookup(&ref_, بفر);
      ورنہ ref_ = خالی_اشاریہ;
}

vpi_پورٹ_Info::~vpi_پورٹ_Info()
{
      حذف[] name_;
}

#اگر_تعریف_شدہ CHECK_WITH_VALGRIND
خالی پورٹ_delete(__vpiHandle *handle)
{
      حذف dynamic_cast<vpi_پورٹ_Info *>(handle);
}
#اگر_ختم

صحیح vpi_پورٹ_Info::vpi_get(صحیح code)
{
      انتخاب( code ) {

        حالت vpiDirection :
          واپس direction_;
        حالت vpi_پورٹ_اشاریہ :
          واپس اشاریہ_2;
        حالت vpi_سائز :
          واپس چوڑائی_2;
        طےشدہ :
          واپس vpiUndefined;
      }

}


حرف *vpi_پورٹ_Info::vpi_get_str(صحیح code)
{
      انتخاب( code ) {
        حالت vpiName :
          واپس simple_سیٹ_rbuf_str(name_);
        طےشدہ :
          واپس تہی;
      }

}


vpiHandle vpi_پورٹ_Info::vpi_handle(صحیح code)
{

      انتخاب (code) {

          حالت vpi_والد:
          حالت vpiScope:
          حالت vpiModule:
            واپس والد_2;
          طےشدہ :
            توڑو;
      }

      واپس 0;
}

ساکن vpiHandle portinfo_iterate(صحیح code, vpiHandle ref)
{
      vpi_پورٹ_Info*rfp = dynamic_cast<vpi_پورٹ_Info*>(ref);
      أكد(rfp);
      غیرعلامتی width = rfp->get_چوڑائی();

      انتخاب (code) {
	  حالت vpi_ثنائی_ہندسہ: {
	    vpiHandle*دلائل = static_cast<vpiHandle*>(صفر_حافظہ_دو(width, حجم(vpiHandle*)));

	    برائے (غیرعلامتی i = 0; i<rfp->پورٹ_بٹ.size(); i++) {
		  دلائل[i] = rfp->پورٹ_بٹ[i];
	    }

	    واپس vpip_make_iterator(width, دلائل, سچ);
	  }
      }
      واپس 0;
}

vpiHandle vpi_پورٹ_Info::vpi_iterate(صحیح code)
{
      واپس portinfo_iterate(code, یہ);
}


vpi_پورٹ_ثنائی_ہندسہ_Info::vpi_پورٹ_ثنائی_ہندسہ_Info(vpi_پورٹ_Info *والد,
                               غیرعلامتی ثنائی_ہندسہ) :
      والد_2(والد),
      ثنائی_ہندسہ_2(ثنائی_ہندسہ)
{
}

vpi_پورٹ_ثنائی_ہندسہ_Info::~vpi_پورٹ_ثنائی_ہندسہ_Info()
{
}

#اگر_تعریف_شدہ CHECK_WITH_VALGRIND
خالی پورٹ_ثنائی_ہندسہ_delete(__vpiHandle *handle)
{
      حذف dynamic_cast<vpi_پورٹ_ثنائی_ہندسہ_Info *>(handle);
}
#اگر_ختم

vpiHandle vpi_پورٹ_ثنائی_ہندسہ_Info::vpi_handle(صحیح code)
{

      انتخاب (code) {
          حالت vpi_والد:
            واپس والد_2;
          طےشدہ :
            توڑو;
      }

      واپس 0;
}

صحیح vpi_پورٹ_ثنائی_ہندسہ_Info::vpi_get(صحیح code)
{
      انتخاب( code ) {
        حالت vpi_ثنائی_ہندسہ :
          واپس ثنائی_ہندسہ_2;
        طےشدہ :
          توڑو;
      }

      واپس 0;
}

/* Port info is meta-data to allow vpi queries of the port signature of modules for
 * code-generators etc.  There are no actual nets corresponding to instances of module ports
 * as elaboration directly connects nets connected through module ports.
 */
خالی compile_پورٹ_info( غیرعلامتی اشاریہ, صحیح vpi_direction, غیرعلامتی width, ثابت حرف *name, حرف* بفر )
{
      vpi_پورٹ_Info* obj = نیا vpi_پورٹ_Info( vpip_peek_current_scope(),
                                          اشاریہ, vpi_direction, width, name, بفر );
      vpip_attach_مقصد_current_scope(obj);

	// Create vpiPortBit objects
      برائے (غیرعلامتی i=0; i<width; i++) {
	    vpi_پورٹ_ثنائی_ہندسہ_Info* obj_ثنائی_ہندسہ = نیا vpi_پورٹ_ثنائی_ہندسہ_Info(obj, i);
	    obj->جوڑو_پورٹ_ثنائی_ہندسہ(obj_ثنائی_ہندسہ);
	    vpip_attach_مقصد_current_scope(obj_ثنائی_ہندسہ);
      }
}
