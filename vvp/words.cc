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
#که_تعریف_شوی CHECK_WITH_VALGRIND
# include  "vvp_cleanup.h"
#د_که_پای
# include  <cstdio>
# include  <cstdlib>
# include  <cstring>
# include  <iostream>
# include  <cassert>

کارول نطاق std;

ساكن فراغ __compile_var_real(حرف*label, حرف*name,
			       vvp_array_t array, بینښه اوږد array_addr,
			       منطقي local_flag)
{
      vvp_net_t*net = جديد vvp_net_t;

      إذا (vpip_peek_current_scope()->is_automatic()) {
	    vvp_fun_signal_real_aa*tmp = جديد vvp_fun_signal_real_aa;
	    net->fil = tmp;
	    net->fun = tmp;
      } وإلا {
	    net->fil = جديد vvp_wire_real;
	    net->fun = جديد vvp_fun_signal_real_sa;
      }

      define_functor_نښه(label, net);

      vpiHandle obj = vpip_make_real_var(name, net);
      compile_vpi_نښه(label, obj);

      إذا (name) {
	    أكد(!array);
	    إذا (!local_flag)
		  vpip_attach_منزل_current_scope(obj);
            إذا (!vpip_peek_current_scope()->is_automatic())
                  مهالوېش_init_ویکټور(vvp_net_ptr_t(net,0), 0.0);
      }
      إذا (array) {
	    أكد(!name);
	    array->attach_کلمه(array_addr, obj);
      }
      حرر(label);
      ړنګول[] name;
}

فراغ compile_var_real(حرف*label, حرف*name, منطقي local_flag)
{
      __compile_var_real(label, name, 0, 0, local_flag);
}

فراغ compile_var_string(حرف*label, حرف*name)
{
      vvp_net_t*net = جديد vvp_net_t;

      إذا (vpip_peek_current_scope()->is_automatic()) {
	    vvp_fun_signal_string_aa*tmp = جديد vvp_fun_signal_string_aa;
	    net->fil = tmp;
	    net->fun = tmp;
      } وإلا {
	    net->fil = 0;
	    net->fun = جديد vvp_fun_signal_string_sa;
      }

      define_functor_نښه(label, net);

      vpiHandle obj = vpip_make_string_var(name, net);
      compile_vpi_نښه(label, obj);

      vpip_attach_منزل_current_scope(obj);
      حرر(label);
      ړنګول[] name;
}

فراغ compile_var_darray(حرف*label, حرف*name, بینښه size)
{
      vvp_net_t*net = جديد vvp_net_t;

      إذا (vpip_peek_current_scope()->is_automatic()) {
	    vvp_fun_signal_object_aa*tmp = جديد vvp_fun_signal_object_aa(size);
	    net->fil = tmp;
	    net->fun = tmp;
      } وإلا {
	    net->fil = 0;
	    net->fun = جديد vvp_fun_signal_object_sa(size);
      }

      define_functor_نښه(label, net);

      vpiHandle obj = vpip_make_darray_var(name, net);
      compile_vpi_نښه(label, obj);

      vpip_attach_منزل_current_scope(obj);
      حرر(label);
      ړنګول[] name;
}

فراغ compile_var_کتار(حرف*label, حرف*name, بینښه size)
{
      vvp_net_t*net = جديد vvp_net_t;

      إذا (vpip_peek_current_scope()->is_automatic()) {
	    vvp_fun_signal_object_aa*tmp = جديد vvp_fun_signal_object_aa(size);
	    net->fil = tmp;
	    net->fun = tmp;
      } وإلا {
	    net->fil = 0;
	    net->fun = جديد vvp_fun_signal_object_sa(size);
      }

      define_functor_نښه(label, net);

      vpiHandle obj = vpip_make_کتار_var(name, net);
      compile_vpi_نښه(label, obj);

      vpip_attach_منزل_current_scope(obj);
      حرر(label);
      ړنګول[] name;
}

فراغ compile_var_cobject(حرف*label, حرف*name)
{
      vvp_net_t*net = جديد vvp_net_t;

      إذا (vpip_peek_current_scope()->is_automatic()) {
	    vvp_fun_signal_object_aa*tmp = جديد vvp_fun_signal_object_aa(1);
	    net->fil = tmp;
	    net->fun = tmp;
      } وإلا {
	    net->fil = 0;
	    net->fun = جديد vvp_fun_signal_object_sa(1);
      }

      define_functor_نښه(label, net);

      vpiHandle obj = vpip_make_cobject_var(name, net);
      compile_vpi_نښه(label, obj);

      vpip_attach_منزل_current_scope(obj);
      حرر(label);
      ړنګول[] name;
}

/*
 * A variable is a special functor, so we allocate that functor and
 * write the label into the symbol table.
 */
فراغ compile_variable(حرف*label, حرف*name,
		      صحيح msb, صحيح lsb, صحيح vpi_type_کوډ,
		      منطقي signed_flag, منطقي local_flag)
{
      بینښه wid = ((msb > lsb)? msb-lsb : lsb-msb) + 1;

      vvp_net_t*net = جديد vvp_net_t;

      vvp_bit4_t init;
      إذا (vpi_type_کوډ == vpiIntVar)
	    init = BIT4_0;
      وإلا
	    init = BIT4_X;

      إذا (vpip_peek_current_scope()->is_automatic()) {
	    vvp_fun_signal4_aa*tmp = جديد vvp_fun_signal4_aa(wid, init);
	    net->fil = tmp;
            net->fun = tmp;
      } وإلا {
	    net->fil = جديد vvp_wire_vec4(wid, init);
            net->fun = جديد vvp_fun_signal4_sa(wid);
      }
      ثابت vvp_signal_ارزښت*vfil = dynamic_cast<vvp_signal_ارزښت*>(net->fil);

      define_functor_نښه(label, net);

      vpiHandle obj = 0;
      إذا (! local_flag) {
	      /* Make the vpiHandle for the reg. */
	    بدل (vpi_type_کوډ) {
		حالة vpiLogicVar:
		  obj = vpip_make_var4(name, msb, lsb, signed_flag, net);
		  اكسر;
		حالة vpiIntegerVar:
		  obj = vpip_make_int4(name, msb, lsb, net);
		  اكسر;
		حالة vpiIntVar: // This handles all the atom2 int types
		  obj = vpip_make_int2(name, msb, lsb, signed_flag, net);
		  اكسر;
		افتراضي:
		  fprintf(stderr, "internal error: %s: vpi_type_code=%d\n", name, vpi_type_کوډ);
		  اكسر;
	    }
	    أكد(obj);
	    compile_vpi_نښه(label, obj);
      }
	// If the signal has a name, then it goes into the current
	// scope as a signal.
      إذا (name) {
	    إذا (obj) vpip_attach_منزل_current_scope(obj);
            إذا (!vpip_peek_current_scope()->is_automatic()) {
		  vvp_vector4_t tmp;
		  vfil->vec4_ارزښت(tmp);
	          مهالوېش_init_ویکټور(vvp_net_ptr_t(net,0), tmp);
	    }
      }

      حرر(label);
      ړنګول[] name;
}


vvp_net_t* create_constant_غوټه(ثابت حرف*val_str)
{
      إذا (c4string_test(val_str)) {
	    vvp_net_t*net = جديد vvp_net_t;
	    net->fun = جديد vvp_fun_bufz;
	    مهالوېش_init_ویکټور(vvp_net_ptr_t(net,0), c4string_منزل_vector4(val_str));
	    أرجع net;
      }

      إذا (c8string_test(val_str)) {
	    vvp_net_t*net = جديد vvp_net_t;
	    net->fun = جديد vvp_fun_bufz;
	    مهالوېش_init_ویکټور(vvp_net_ptr_t(net,0), c8string_منزل_vector8(val_str));
	    أرجع net;
      }

      إذا (crstring_test(val_str)) {
	    vvp_net_t*net = جديد vvp_net_t;
	    net->fun = جديد vvp_fun_bufz;
	    مهالوېش_init_ویکټور(vvp_net_ptr_t(net,0), crstring_منزل_double(val_str));
	    أرجع net;
      }

      أرجع 0;
}

صنف بنسټ_net_resolv : عام resolv_list_s {
    عام:
      څرګند بنسټ_net_resolv(حرف*ref_label, vvp_array_t array,
			       __vpiScope*scope,
			       حرف*my_label, حرف*name,
			       بینښه array_addr, منطقي local_flag)
      : resolv_list_s(ref_label)
      { my_label_ = my_label;
	array_ = array;
	name_ = name;
	scope_ = scope;
	array_addr_ = array_addr;
	local_flag_ = local_flag;
      }

    محمي:
      حرف*my_label_;
      vvp_array_t array_;
      حرف*name_;
      __vpiScope*scope_;
      بینښه array_addr_;
      منطقي local_flag_;
};

صنف __compile_net_resolv : عام بنسټ_net_resolv {

    عام:
      څرګند __compile_net_resolv(حرف*ref_label, vvp_array_t array,
				    __vpiScope*scope,
				    حرف*my_label, حرف*name,
				    صحيح msb, صحيح lsb, بینښه array_addr,
				    صحيح vpi_type_کوډ, منطقي signed_flag, منطقي local_flag)
      : بنسټ_net_resolv(ref_label, array, scope, my_label, name, array_addr, local_flag)
      { msb_ = msb;
	lsb_ = lsb;
	vpi_type_کوډ_2 = vpi_type_کوډ;
	signed_flag_ = signed_flag;
      }

      ~__compile_net_resolv() override { }

      منطقي resolve(منطقي پیغام_flag) override;

    خاص:
      صحيح msb_, lsb_;
      صحيح vpi_type_کوډ_2;
      منطقي signed_flag_;
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

ساكن فراغ do_compile_net(vvp_net_t*غوټه, vvp_array_t array,
			   __vpiScope*scope,
			   حرف*my_label, حرف*name,
			   صحيح msb, صحيح lsb, بینښه array_addr,
			   صحيح vpi_type_کوډ, منطقي signed_flag, منطقي local_flag)
{
      بینښه wid = ((msb > lsb)? msb-lsb : lsb-msb) + 1;
      أكد(غوټه);

      vvp_wire_بنسټ*vsig = dynamic_cast<vvp_wire_بنسټ*>(غوټه->fil);

      إذا (vsig == 0) {
	    بدل (vpi_type_کوډ) {
		حالة vpiIntVar:
		  vsig = جديد vvp_wire_vec4(wid,BIT4_0);
		  اكسر;
		حالة vpiLogicVar:
		  vsig = جديد vvp_wire_vec4(wid,BIT4_Z);
		  اكسر;
		حالة -vpiLogicVar:
		  vsig = جديد vvp_wire_vec8(wid);
		  اكسر;
	    }
	    أكد(vsig);
	    غوټه->fil = vsig;
      }

      vpiHandle obj = 0;
      إذا (! local_flag) {
	      /* Make the vpiHandle for the reg. */
	    obj = vpip_make_net4(scope, name, msb, lsb, signed_flag, غوټه);
	      /* This attaches the label to the vpiHandle */
	    compile_vpi_نښه(my_label, obj);
      }
#که_تعریف_شوی CHECK_WITH_VALGRIND
      وإلا pool_local_net(غوټه);
#د_که_پای

	// REMOVE ME! Giving the net a label is a legacy of the times
	// when the .net was a functor of its own. In the long run, we
	// must fix the code generator to not rely on the label of the
	// .net, then we will remove that label.
      define_functor_نښه(my_label, غوټه);

      إذا (array)
	    array->attach_کلمه(array_addr, obj);
      وإلا إذا (obj)
	    vpip_attach_منزل_scope(scope,obj);

      حرر(my_label);
      ړنګول[] name;
}

ساكن فراغ __compile_net(حرف*label,
			  حرف*name, حرف*array_label, بینښه اوږد array_addr,
			  صحيح msb, صحيح lsb,
			  صحيح vpi_type_کوډ, منطقي signed_flag, منطقي local_flag,
			  بینښه argc, بنية symb_s*argv)
{
      vvp_array_t array = array_label? array_پیدا_کړه(array_label) : عدم;
      أكد(array_label ? array!=0 : ریښتیا);

      حرر(array_label);

      أكد(argc == 1);
      vvp_net_t*غوټه = vvp_net_lookup(argv[0].متن);
#إذا 1
      إذا (غوټه == 0) {
	      /* No existing net, but the string value may be a
		 constant. In that case, we will wind up generating a
		 bufz node that can carry the constant value.

	         NOTE: This is a hack! The code generator should be
	         fixed so that this is no longer needed. */
	    غوټه = create_constant_غوټه(argv[0].متن);
      }
#د_که_پای
      إذا (غوټه == 0) {
	    __vpiScope*scope = vpip_peek_current_scope();
	    __compile_net_resolv*res
		  = جديد __compile_net_resolv(argv[0].متن,
					     array, scope, label, name,
					     msb, lsb, array_addr,
					     vpi_type_کوډ, signed_flag, local_flag);
	    resolv_submit(res);
	    حرر(argv);
	    أرجع;
      }
      أكد(غوټه);

      __vpiScope*scope = vpip_peek_current_scope();
      do_compile_net(غوټه, array, scope, label, name, msb, lsb, array_addr,
		     vpi_type_کوډ, signed_flag, local_flag);

      حرر(argv[0].متن);
      حرر(argv);
}

منطقي __compile_net_resolv::resolve(منطقي پیغام_flag_2)
{
      vvp_net_t*غوټه = vvp_net_lookup(label());
      إذا (غوټه == 0) {
	    إذا (پیغام_flag_2)
		  cerr << "Unable to resolve label " << label() << کرښه_پای;
	    أرجع خطأ;
      }

      do_compile_net(غوټه, array_, scope_, my_label_, name_, msb_, lsb_, array_addr_, vpi_type_کوډ_2, signed_flag_, local_flag_);
      أرجع ریښتیا;
}

فراغ compile_net(حرف*label, حرف*name, صحيح msb, صحيح lsb,
		 صحيح vpi_type_کوډ, منطقي signed_flag, منطقي local_flag,
		 بینښه argc, بنية symb_s*argv)
{
      __compile_net(label, name, 0, 0, msb, lsb,
		    vpi_type_کوډ, signed_flag, local_flag,
		    argc, argv);
}

فراغ compile_netw(حرف*label, حرف*array_label, بینښه اوږد array_addr,
		  صحيح msb, صحيح lsb, صحيح vpi_type_کوډ, منطقي signed_flag,
		  بینښه argc, بنية symb_s*argv)
{
      __compile_net(label, 0, array_label, array_addr,
		    msb, lsb, vpi_type_کوډ, signed_flag, خطأ,
		    argc, argv);
}

صنف __compile_real_net_resolv : عام بنسټ_net_resolv {

    عام:
      څرګند __compile_real_net_resolv(حرف*ref_label, vvp_array_t array,
					 __vpiScope*scope,
					 حرف*my_label, حرف*name,
					 بینښه array_addr, منطقي local_flag)
      : بنسټ_net_resolv(ref_label, array, scope, my_label, name, array_addr, local_flag)
      {
      }

      ~__compile_real_net_resolv() override { }

      منطقي resolve(منطقي پیغام_flag) override;

    خاص:
};

ساكن فراغ __compile_real_net2(vvp_net_t*غوټه, vvp_array_t array,
				__vpiScope*scope,
				حرف*my_label, حرف*name,
				بینښه array_addr, منطقي local_flag)
{
      vvp_wire_بنسټ*fil = dynamic_cast<vvp_wire_بنسټ*> (غوټه->fil);

      إذا (fil == 0) {
	    fil = جديد vvp_wire_real;
	    غوټه->fil = fil;
      }

      vpiHandle obj = 0;
      إذا (!local_flag) {
	    obj = vpip_make_real_net(scope, name, غوټه);
	    compile_vpi_نښه(my_label, obj);
      }
#که_تعریف_شوی CHECK_WITH_VALGRIND
      وإلا pool_local_net(غوټه);
#د_که_پای

	// REMOVE ME! Giving the net a label is a legacy of the times
	// when the .net was a functor of its own. In the long run, we
	// must fix the code generator to not rely on the label of the
	// .net, then we will remove that label.
      define_functor_نښه(my_label, غوټه);

     إذا (array)
	    array->attach_کلمه(array_addr, obj);
      وإلا إذا (obj)
	    vpip_attach_منزل_scope(scope, obj);

      حرر(my_label);
      ړنګول[] name;
}

ساكن فراغ __compile_real(حرف*label, حرف*name,
                           حرف*array_label, بینښه اوږد array_addr,
                           صحيح msb, صحيح lsb, منطقي local_flag,
                           بینښه argc, بنية symb_s*argv)
{
      أكد(msb == 0 && lsb == 0);
      vvp_array_t array = array_label ? array_پیدا_کړه(array_label) : عدم;
      أكد(array_label ? array!=0 : ریښتیا);

      حرر(array_label);

      أكد(argc == 1);
      vvp_net_t*غوټه = vvp_net_lookup(argv[0].متن);
      إذا (غوټه == 0) {
	      /* No existing net, but the string value may be a
		 constant. In that case, we will wind up generating a
		 bufz node that can carry the constant value. */
	    غوټه = create_constant_غوټه(argv[0].متن);
      }
      __vpiScope*scope = vpip_peek_current_scope();
      إذا (غوټه == 0) {
	    __compile_real_net_resolv*res
		  = جديد __compile_real_net_resolv(argv[0].متن, array,
						  scope, label, name,
						  array_addr, local_flag);
	    resolv_submit(res);
	    حرر(argv);
	    أرجع;
      }

      أكد(غوټه);
      __compile_real_net2(غوټه, array, scope, label, name, array_addr,
                          local_flag);
      حرر(argv[0].متن);
      حرر(argv);
}

منطقي __compile_real_net_resolv::resolve(منطقي پیغام_flag_2)
{
      vvp_net_t*غوټه = vvp_net_lookup(label());
      إذا (غوټه == 0) {
	    إذا (پیغام_flag_2)
		  cerr << "Unable to resolve label " << label() << کرښه_پای;
	    أرجع خطأ;
      }

      __compile_real_net2(غوټه, array_, scope_, my_label_, name_, array_addr_, local_flag_);
      أرجع ریښتیا;
}

فراغ compile_net_real(حرف*label, حرف*name, صحيح msb, صحيح lsb, منطقي local_flag,
		      بینښه argc, بنية symb_s*argv)
{
      __compile_real(label, name, 0, 0,
                     msb, lsb, local_flag, argc, argv);
}

فراغ compile_netw_real(حرف*label, حرف*array_label, بینښه اوږد array_addr,
                       صحيح msb, صحيح lsb,
                       بینښه argc, بنية symb_s*argv)
{
      __compile_real(label, 0, array_label, array_addr,
                     msb, lsb, خطأ, argc, argv);
}
