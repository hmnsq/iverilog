/*
 * Copyright (c) 2003-2025 Stephen Williams (steve@icarus.com)
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
# include  "array.h"
# include  "vvp_net_sig.h"
# include  "logic.h"
# include  "schedule.h"
#اگر_تعریف_شدہ CHECK_WITH_VALGRIND
# include  "vvp_cleanup.h"
#اگر_ختم
# include  <cstdio>
# include  <cstdlib>
# include  <cstring>
# include  <iostream>
# include  <cassert>

استعمال نامخلا std;

ساکن خالی __compile_var_real(حرف*label, حرف*name,
			       vvp_array_t array, غیرعلامتی طویل array_addr,
			       منطقی local_flag)
{
      vvp_net_t*net = نیا vvp_net_t;

      اگر (vpip_peek_current_scope()->is_automatic()) {
	    vvp_fun_signal_real_aa*tmp = نیا vvp_fun_signal_real_aa;
	    net->fil = tmp;
	    net->fun = tmp;
      } ورنہ {
	    net->fil = نیا vvp_wire_real;
	    net->fun = نیا vvp_fun_signal_real_sa;
      }

      define_functor_علامت(label, net);

      vpiHandle obj = vpip_make_real_var(name, net);
      compile_vpi_علامت(label, obj);

      اگر (name) {
	    أكد(!array);
	    اگر (!local_flag)
		  vpip_attach_مقصد_current_scope(obj);
            اگر (!vpip_peek_current_scope()->is_automatic())
                  جدول_وقت_init_سمتیہ(vvp_net_ptr_t(net,0), 0.0);
      }
      اگر (array) {
	    أكد(!name);
	    array->attach_لفظ(array_addr, obj);
      }
      آزاد(label);
      حذف[] name;
}

خالی compile_var_real(حرف*label, حرف*name, منطقی local_flag)
{
      __compile_var_real(label, name, 0, 0, local_flag);
}

خالی compile_var_string(حرف*label, حرف*name)
{
      vvp_net_t*net = نیا vvp_net_t;

      اگر (vpip_peek_current_scope()->is_automatic()) {
	    vvp_fun_signal_string_aa*tmp = نیا vvp_fun_signal_string_aa;
	    net->fil = tmp;
	    net->fun = tmp;
      } ورنہ {
	    net->fil = 0;
	    net->fun = نیا vvp_fun_signal_string_sa;
      }

      define_functor_علامت(label, net);

      vpiHandle obj = vpip_make_string_var(name, net);
      compile_vpi_علامت(label, obj);

      vpip_attach_مقصد_current_scope(obj);
      آزاد(label);
      حذف[] name;
}

خالی compile_var_darray(حرف*label, حرف*name, غیرعلامتی size)
{
      vvp_net_t*net = نیا vvp_net_t;

      اگر (vpip_peek_current_scope()->is_automatic()) {
	    vvp_fun_signal_object_aa*tmp = نیا vvp_fun_signal_object_aa(size);
	    net->fil = tmp;
	    net->fun = tmp;
      } ورنہ {
	    net->fil = 0;
	    net->fun = نیا vvp_fun_signal_object_sa(size);
      }

      define_functor_علامت(label, net);

      vpiHandle obj = vpip_make_darray_var(name, net);
      compile_vpi_علامت(label, obj);

      vpip_attach_مقصد_current_scope(obj);
      آزاد(label);
      حذف[] name;
}

خالی compile_var_قطار(حرف*label, حرف*name, غیرعلامتی size)
{
      vvp_net_t*net = نیا vvp_net_t;

      اگر (vpip_peek_current_scope()->is_automatic()) {
	    vvp_fun_signal_object_aa*tmp = نیا vvp_fun_signal_object_aa(size);
	    net->fil = tmp;
	    net->fun = tmp;
      } ورنہ {
	    net->fil = 0;
	    net->fun = نیا vvp_fun_signal_object_sa(size);
      }

      define_functor_علامت(label, net);

      vpiHandle obj = vpip_make_قطار_var(name, net);
      compile_vpi_علامت(label, obj);

      vpip_attach_مقصد_current_scope(obj);
      آزاد(label);
      حذف[] name;
}

خالی compile_var_cobject(حرف*label, حرف*name)
{
      vvp_net_t*net = نیا vvp_net_t;

      اگر (vpip_peek_current_scope()->is_automatic()) {
	    vvp_fun_signal_object_aa*tmp = نیا vvp_fun_signal_object_aa(1);
	    net->fil = tmp;
	    net->fun = tmp;
      } ورنہ {
	    net->fil = 0;
	    net->fun = نیا vvp_fun_signal_object_sa(1);
      }

      define_functor_علامت(label, net);

      vpiHandle obj = vpip_make_cobject_var(name, net);
      compile_vpi_علامت(label, obj);

      vpip_attach_مقصد_current_scope(obj);
      آزاد(label);
      حذف[] name;
}

/*
 * A variable is a special functor, so we allocate that functor and
 * write the label into the symbol table.
 */
خالی compile_variable(حرف*label, حرف*name,
		      صحیح msb, صحیح lsb, صحیح vpi_type_رمز,
		      منطقی signed_flag, منطقی local_flag)
{
      غیرعلامتی wid = ((msb > lsb)? msb-lsb : lsb-msb) + 1;

      vvp_net_t*net = نیا vvp_net_t;

      vvp_bit4_t init;
      اگر (vpi_type_رمز == vpiIntVar)
	    init = BIT4_0;
      ورنہ
	    init = BIT4_X;

      اگر (vpip_peek_current_scope()->is_automatic()) {
	    vvp_fun_signal4_aa*tmp = نیا vvp_fun_signal4_aa(wid, init);
	    net->fil = tmp;
            net->fun = tmp;
      } ورنہ {
	    net->fil = نیا vvp_wire_vec4(wid, init);
            net->fun = نیا vvp_fun_signal4_sa(wid);
      }
      ثابت vvp_signal_قدر*vfil = dynamic_cast<vvp_signal_قدر*>(net->fil);

      define_functor_علامت(label, net);

      vpiHandle obj = 0;
      اگر (! local_flag) {
	      /* Make the vpiHandle for the reg. */
	    انتخاب (vpi_type_رمز) {
		حالت vpiLogicVar:
		  obj = vpip_make_var4(name, msb, lsb, signed_flag, net);
		  توڑو;
		حالت vpiIntegerVar:
		  obj = vpip_make_int4(name, msb, lsb, net);
		  توڑو;
		حالت vpiIntVar: // This handles all the atom2 int types
		  obj = vpip_make_int2(name, msb, lsb, signed_flag, net);
		  توڑو;
		طےشدہ:
		  fprintf(stderr, "internal error: %s: vpi_type_code=%d\n", name, vpi_type_رمز);
		  توڑو;
	    }
	    أكد(obj);
	    compile_vpi_علامت(label, obj);
      }
	// If the signal has a name, then it goes into the current
	// scope as a signal.
      اگر (name) {
	    اگر (obj) vpip_attach_مقصد_current_scope(obj);
            اگر (!vpip_peek_current_scope()->is_automatic()) {
		  vvp_vector4_t tmp;
		  vfil->vec4_قدر(tmp);
	          جدول_وقت_init_سمتیہ(vvp_net_ptr_t(net,0), tmp);
	    }
      }

      آزاد(label);
      حذف[] name;
}


vvp_net_t* create_constant_گرہ(ثابت حرف*val_str)
{
      اگر (c4string_test(val_str)) {
	    vvp_net_t*net = نیا vvp_net_t;
	    net->fun = نیا vvp_fun_bufz;
	    جدول_وقت_init_سمتیہ(vvp_net_ptr_t(net,0), c4string_مقصد_vector4(val_str));
	    واپس net;
      }

      اگر (c8string_test(val_str)) {
	    vvp_net_t*net = نیا vvp_net_t;
	    net->fun = نیا vvp_fun_bufz;
	    جدول_وقت_init_سمتیہ(vvp_net_ptr_t(net,0), c8string_مقصد_vector8(val_str));
	    واپس net;
      }

      اگر (crstring_test(val_str)) {
	    vvp_net_t*net = نیا vvp_net_t;
	    net->fun = نیا vvp_fun_bufz;
	    جدول_وقت_init_سمتیہ(vvp_net_ptr_t(net,0), crstring_مقصد_double(val_str));
	    واپس net;
      }

      واپس 0;
}

جماعت بنیاد_net_resolv : عوامی resolv_list_s {
    عوامی:
      صریح بنیاد_net_resolv(حرف*ref_label, vvp_array_t array,
			       __vpiScope*scope,
			       حرف*my_label, حرف*name,
			       غیرعلامتی array_addr, منطقی local_flag)
      : resolv_list_s(ref_label)
      { my_label_ = my_label;
	array_ = array;
	name_ = name;
	scope_ = scope;
	array_addr_ = array_addr;
	local_flag_ = local_flag;
      }

    محفوظ:
      حرف*my_label_;
      vvp_array_t array_;
      حرف*name_;
      __vpiScope*scope_;
      غیرعلامتی array_addr_;
      منطقی local_flag_;
};

جماعت __compile_net_resolv : عوامی بنیاد_net_resolv {

    عوامی:
      صریح __compile_net_resolv(حرف*ref_label, vvp_array_t array,
				    __vpiScope*scope,
				    حرف*my_label, حرف*name,
				    صحیح msb, صحیح lsb, غیرعلامتی array_addr,
				    صحیح vpi_type_رمز, منطقی signed_flag, منطقی local_flag)
      : بنیاد_net_resolv(ref_label, array, scope, my_label, name, array_addr, local_flag)
      { msb_ = msb;
	lsb_ = lsb;
	vpi_type_رمز_2 = vpi_type_رمز;
	signed_flag_ = signed_flag;
      }

      ~__compile_net_resolv() override { }

      منطقی resolve(منطقی پیغام_flag) override;

    نجی:
      صحیح msb_, lsb_;
      صحیح vpi_type_رمز_2;
      منطقی signed_flag_;
};

/*
 * Here we handle .net records from the vvp source:
 *
 *    .net   <name>, <msb>, <lsb>, <input> ;
 *    .net/s <name>, <msb>, <lsb>, <input> ;
 *    .net8   <name>, <msb>, <lsb>, <input> ;
 *    .net8/s <name>, <msb>, <lsb>, <input> ;
 *
 * Create a VPI handle to represent it, and fill that handle in with
 * references into the net.
 */

ساکن خالی do_compile_net(vvp_net_t*گرہ, vvp_array_t array,
			   __vpiScope*scope,
			   حرف*my_label, حرف*name,
			   صحیح msb, صحیح lsb, غیرعلامتی array_addr,
			   صحیح vpi_type_رمز, منطقی signed_flag, منطقی local_flag)
{
      غیرعلامتی wid = ((msb > lsb)? msb-lsb : lsb-msb) + 1;
      أكد(گرہ);

      vvp_wire_بنیاد*vsig = dynamic_cast<vvp_wire_بنیاد*>(گرہ->fil);

      اگر (vsig == 0) {
	    انتخاب (vpi_type_رمز) {
		حالت vpiIntVar:
		  vsig = نیا vvp_wire_vec4(wid,BIT4_0);
		  توڑو;
		حالت vpiLogicVar:
		  vsig = نیا vvp_wire_vec4(wid,BIT4_Z);
		  توڑو;
		حالت -vpiLogicVar:
		  vsig = نیا vvp_wire_vec8(wid);
		  توڑو;
	    }
	    أكد(vsig);
	    گرہ->fil = vsig;
      }

      vpiHandle obj = 0;
      اگر (! local_flag) {
	      /* Make the vpiHandle for the reg. */
	    obj = vpip_make_net4(scope, name, msb, lsb, signed_flag, گرہ);
	      /* This attaches the label to the vpiHandle */
	    compile_vpi_علامت(my_label, obj);
      }
#اگر_تعریف_شدہ CHECK_WITH_VALGRIND
      ورنہ pool_local_net(گرہ);
#اگر_ختم

	// REMOVE ME! Giving the net a label is a legacy of the times
	// when the .net was a functor of its own. In the long run, we
	// must fix the code generator to not rely on the label of the
	// .net, then we will remove that label.
      define_functor_علامت(my_label, گرہ);

      اگر (array)
	    array->attach_لفظ(array_addr, obj);
      ورنہ اگر (obj)
	    vpip_attach_مقصد_scope(scope,obj);

      آزاد(my_label);
      حذف[] name;
}

ساکن خالی __compile_net(حرف*label,
			  حرف*name, حرف*array_label, غیرعلامتی طویل array_addr,
			  صحیح msb, صحیح lsb,
			  صحیح vpi_type_رمز, منطقی signed_flag, منطقی local_flag,
			  غیرعلامتی argc, ساخت symb_s*argv)
{
      vvp_array_t array = array_label? array_ڈھونڈو(array_label) : تہی;
      أكد(array_label ? array!=0 : سچ);

      آزاد(array_label);

      أكد(argc == 1);
      vvp_net_t*گرہ = vvp_net_lookup(argv[0].متن);
#اگر 1
      اگر (گرہ == 0) {
	      /* No existing net, but the string value may be a
		 constant. In that case, we will wind up generating a
		 bufz node that can carry the constant value.

	         NOTE: This is a hack! The code generator should be
	         fixed so that this is no longer needed. */
	    گرہ = create_constant_گرہ(argv[0].متن);
      }
#اگر_ختم
      اگر (گرہ == 0) {
	    __vpiScope*scope = vpip_peek_current_scope();
	    __compile_net_resolv*res
		  = نیا __compile_net_resolv(argv[0].متن,
					     array, scope, label, name,
					     msb, lsb, array_addr,
					     vpi_type_رمز, signed_flag, local_flag);
	    resolv_submit(res);
	    آزاد(argv);
	    واپس;
      }
      أكد(گرہ);

      __vpiScope*scope = vpip_peek_current_scope();
      do_compile_net(گرہ, array, scope, label, name, msb, lsb, array_addr,
		     vpi_type_رمز, signed_flag, local_flag);

      آزاد(argv[0].متن);
      آزاد(argv);
}

منطقی __compile_net_resolv::resolve(منطقی پیغام_flag_2)
{
      vvp_net_t*گرہ = vvp_net_lookup(label());
      اگر (گرہ == 0) {
	    اگر (پیغام_flag_2)
		  cerr << "Unable to resolve label " << label() << سطر_ختم;
	    واپس جھوٹ;
      }

      do_compile_net(گرہ, array_, scope_, my_label_, name_, msb_, lsb_, array_addr_, vpi_type_رمز_2, signed_flag_, local_flag_);
      واپس سچ;
}

خالی compile_net(حرف*label, حرف*name, صحیح msb, صحیح lsb,
		 صحیح vpi_type_رمز, منطقی signed_flag, منطقی local_flag,
		 غیرعلامتی argc, ساخت symb_s*argv)
{
      __compile_net(label, name, 0, 0, msb, lsb,
		    vpi_type_رمز, signed_flag, local_flag,
		    argc, argv);
}

خالی compile_netw(حرف*label, حرف*array_label, غیرعلامتی طویل array_addr,
		  صحیح msb, صحیح lsb, صحیح vpi_type_رمز, منطقی signed_flag,
		  غیرعلامتی argc, ساخت symb_s*argv)
{
      __compile_net(label, 0, array_label, array_addr,
		    msb, lsb, vpi_type_رمز, signed_flag, جھوٹ,
		    argc, argv);
}

جماعت __compile_real_net_resolv : عوامی بنیاد_net_resolv {

    عوامی:
      صریح __compile_real_net_resolv(حرف*ref_label, vvp_array_t array,
					 __vpiScope*scope,
					 حرف*my_label, حرف*name,
					 غیرعلامتی array_addr, منطقی local_flag)
      : بنیاد_net_resolv(ref_label, array, scope, my_label, name, array_addr, local_flag)
      {
      }

      ~__compile_real_net_resolv() override { }

      منطقی resolve(منطقی پیغام_flag) override;

    نجی:
};

ساکن خالی __compile_real_net2(vvp_net_t*گرہ, vvp_array_t array,
				__vpiScope*scope,
				حرف*my_label, حرف*name,
				غیرعلامتی array_addr, منطقی local_flag)
{
      vvp_wire_بنیاد*fil = dynamic_cast<vvp_wire_بنیاد*> (گرہ->fil);

      اگر (fil == 0) {
	    fil = نیا vvp_wire_real;
	    گرہ->fil = fil;
      }

      vpiHandle obj = 0;
      اگر (!local_flag) {
	    obj = vpip_make_real_net(scope, name, گرہ);
	    compile_vpi_علامت(my_label, obj);
      }
#اگر_تعریف_شدہ CHECK_WITH_VALGRIND
      ورنہ pool_local_net(گرہ);
#اگر_ختم

	// REMOVE ME! Giving the net a label is a legacy of the times
	// when the .net was a functor of its own. In the long run, we
	// must fix the code generator to not rely on the label of the
	// .net, then we will remove that label.
      define_functor_علامت(my_label, گرہ);

     اگر (array)
	    array->attach_لفظ(array_addr, obj);
      ورنہ اگر (obj)
	    vpip_attach_مقصد_scope(scope, obj);

      آزاد(my_label);
      حذف[] name;
}

ساکن خالی __compile_real(حرف*label, حرف*name,
                           حرف*array_label, غیرعلامتی طویل array_addr,
                           صحیح msb, صحیح lsb, منطقی local_flag,
                           غیرعلامتی argc, ساخت symb_s*argv)
{
      أكد(msb == 0 && lsb == 0);
      vvp_array_t array = array_label ? array_ڈھونڈو(array_label) : تہی;
      أكد(array_label ? array!=0 : سچ);

      آزاد(array_label);

      أكد(argc == 1);
      vvp_net_t*گرہ = vvp_net_lookup(argv[0].متن);
      اگر (گرہ == 0) {
	      /* No existing net, but the string value may be a
		 constant. In that case, we will wind up generating a
		 bufz node that can carry the constant value. */
	    گرہ = create_constant_گرہ(argv[0].متن);
      }
      __vpiScope*scope = vpip_peek_current_scope();
      اگر (گرہ == 0) {
	    __compile_real_net_resolv*res
		  = نیا __compile_real_net_resolv(argv[0].متن, array,
						  scope, label, name,
						  array_addr, local_flag);
	    resolv_submit(res);
	    آزاد(argv);
	    واپس;
      }

      أكد(گرہ);
      __compile_real_net2(گرہ, array, scope, label, name, array_addr,
                          local_flag);
      آزاد(argv[0].متن);
      آزاد(argv);
}

منطقی __compile_real_net_resolv::resolve(منطقی پیغام_flag_2)
{
      vvp_net_t*گرہ = vvp_net_lookup(label());
      اگر (گرہ == 0) {
	    اگر (پیغام_flag_2)
		  cerr << "Unable to resolve label " << label() << سطر_ختم;
	    واپس جھوٹ;
      }

      __compile_real_net2(گرہ, array_, scope_, my_label_, name_, array_addr_, local_flag_);
      واپس سچ;
}

خالی compile_net_real(حرف*label, حرف*name, صحیح msb, صحیح lsb, منطقی local_flag,
		      غیرعلامتی argc, ساخت symb_s*argv)
{
      __compile_real(label, name, 0, 0,
                     msb, lsb, local_flag, argc, argv);
}

خالی compile_netw_real(حرف*label, حرف*array_label, غیرعلامتی طویل array_addr,
                       صحیح msb, صحیح lsb,
                       غیرعلامتی argc, ساخت symb_s*argv)
{
      __compile_real(label, 0, array_label, array_addr,
                     msb, lsb, جھوٹ, argc, argv);
}
