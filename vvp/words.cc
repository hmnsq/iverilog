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
#ifdef CHECK_WITH_VALGRIND
# include  "vvp_cleanup.h"
#endif
# include  <cstdio>
# include  <cstdlib>
# include  <cstring>
# include  <iostream>
# include  <cassert>

using نطاق std;

ساكن فراغ __compile_var_real(حرف*label, حرف*name,
			       vvp_array_t array, unsigned long array_addr,
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

      define_functor_بەلگە(label, net);

      vpiHandle obj = vpip_make_real_var(name, net);
      compile_vpi_بەلگە(label, obj);

      إذا (name) {
	    أكد(!array);
	    إذا (!local_flag)
		  vpip_attach_to_current_scope(obj);
            إذا (!vpip_peek_current_scope()->is_automatic())
                  schedule_init_vector(vvp_net_ptr_t(net,0), 0.0);
      }
      إذا (array) {
	    أكد(!name);
	    array->attach_word(array_addr, obj);
      }
      حرر(label);
      delete[] name;
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

      define_functor_بەلگە(label, net);

      vpiHandle obj = vpip_make_string_var(name, net);
      compile_vpi_بەلگە(label, obj);

      vpip_attach_to_current_scope(obj);
      حرر(label);
      delete[] name;
}

فراغ compile_var_darray(حرف*label, حرف*name, unsigned size)
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

      define_functor_بەلگە(label, net);

      vpiHandle obj = vpip_make_darray_var(name, net);
      compile_vpi_بەلگە(label, obj);

      vpip_attach_to_current_scope(obj);
      حرر(label);
      delete[] name;
}

فراغ compile_var_queue(حرف*label, حرف*name, unsigned size)
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

      define_functor_بەلگە(label, net);

      vpiHandle obj = vpip_make_queue_var(name, net);
      compile_vpi_بەلگە(label, obj);

      vpip_attach_to_current_scope(obj);
      حرر(label);
      delete[] name;
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

      define_functor_بەلگە(label, net);

      vpiHandle obj = vpip_make_cobject_var(name, net);
      compile_vpi_بەلگە(label, obj);

      vpip_attach_to_current_scope(obj);
      حرر(label);
      delete[] name;
}

/*
 * A variable is a special functor, so we allocate that functor and
 * write the label into the symbol table.
 */
فراغ compile_variable(حرف*label, حرف*name,
		      صحيح msb, صحيح lsb, صحيح vpi_type_code,
		      منطقي signed_flag, منطقي local_flag)
{
      unsigned wid = ((msb > lsb)? msb-lsb : lsb-msb) + 1;

      vvp_net_t*net = جديد vvp_net_t;

      vvp_bit4_t init;
      إذا (vpi_type_code == vpiIntVar)
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
      ثابت vvp_signal_قىممەت*vfil = dynamic_cast<vvp_signal_قىممەت*>(net->fil);

      define_functor_بەلگە(label, net);

      vpiHandle obj = 0;
      إذا (! local_flag) {
	      /* Make the vpiHandle for the reg. */
	    بدل (vpi_type_code) {
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
		  fprintf(stderr, "internal error: %s: vpi_type_code=%d\n", name, vpi_type_code);
		  اكسر;
	    }
	    أكد(obj);
	    compile_vpi_بەلگە(label, obj);
      }
	// If the signal has a name, then it goes into the current
	// scope as a signal.
      إذا (name) {
	    إذا (obj) vpip_attach_to_current_scope(obj);
            إذا (!vpip_peek_current_scope()->is_automatic()) {
		  vvp_vector4_t tmp;
		  vfil->vec4_قىممەت(tmp);
	          schedule_init_vector(vvp_net_ptr_t(net,0), tmp);
	    }
      }

      حرر(label);
      delete[] name;
}


vvp_net_t* create_constant_تۈگۈن(ثابت حرف*val_str)
{
      إذا (c4string_test(val_str)) {
	    vvp_net_t*net = جديد vvp_net_t;
	    net->fun = جديد vvp_fun_bufz;
	    schedule_init_vector(vvp_net_ptr_t(net,0), c4string_to_vector4(val_str));
	    أرجع net;
      }

      إذا (c8string_test(val_str)) {
	    vvp_net_t*net = جديد vvp_net_t;
	    net->fun = جديد vvp_fun_bufz;
	    schedule_init_vector(vvp_net_ptr_t(net,0), c8string_to_vector8(val_str));
	    أرجع net;
      }

      إذا (crstring_test(val_str)) {
	    vvp_net_t*net = جديد vvp_net_t;
	    net->fun = جديد vvp_fun_bufz;
	    schedule_init_vector(vvp_net_ptr_t(net,0), crstring_to_double(val_str));
	    أرجع net;
      }

      أرجع 0;
}

صنف base_net_resolv : عام resolv_list_s {
    عام:
      explicit base_net_resolv(حرف*ref_label, vvp_array_t array,
			       __vpiScope*scope,
			       حرف*my_label, حرف*name,
			       unsigned array_addr, منطقي local_flag)
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
      unsigned array_addr_;
      منطقي local_flag_;
};

صنف __compile_net_resolv : عام base_net_resolv {

    عام:
      explicit __compile_net_resolv(حرف*ref_label, vvp_array_t array,
				    __vpiScope*scope,
				    حرف*my_label, حرف*name,
				    صحيح msb, صحيح lsb, unsigned array_addr,
				    صحيح vpi_type_code, منطقي signed_flag, منطقي local_flag)
      : base_net_resolv(ref_label, array, scope, my_label, name, array_addr, local_flag)
      { msb_ = msb;
	lsb_ = lsb;
	vpi_type_code_ = vpi_type_code;
	signed_flag_ = signed_flag;
      }

      ~__compile_net_resolv() override { }

      منطقي resolve(منطقي message_flag) override;

    خاص:
      صحيح msb_, lsb_;
      صحيح vpi_type_code_;
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

ساكن فراغ do_compile_net(vvp_net_t*تۈگۈن, vvp_array_t array,
			   __vpiScope*scope,
			   حرف*my_label, حرف*name,
			   صحيح msb, صحيح lsb, unsigned array_addr,
			   صحيح vpi_type_code, منطقي signed_flag, منطقي local_flag)
{
      unsigned wid = ((msb > lsb)? msb-lsb : lsb-msb) + 1;
      أكد(تۈگۈن);

      vvp_wire_base*vsig = dynamic_cast<vvp_wire_base*>(تۈگۈن->fil);

      إذا (vsig == 0) {
	    بدل (vpi_type_code) {
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
	    تۈگۈن->fil = vsig;
      }

      vpiHandle obj = 0;
      إذا (! local_flag) {
	      /* Make the vpiHandle for the reg. */
	    obj = vpip_make_net4(scope, name, msb, lsb, signed_flag, تۈگۈن);
	      /* This attaches the label to the vpiHandle */
	    compile_vpi_بەلگە(my_label, obj);
      }
#ifdef CHECK_WITH_VALGRIND
      وإلا pool_local_net(تۈگۈن);
#endif

	// REMOVE ME! Giving the net a label is a legacy of the times
	// when the .net was a functor of its own. In the long run, we
	// must fix the code generator to not rely on the label of the
	// .net, then we will remove that label.
      define_functor_بەلگە(my_label, تۈگۈن);

      إذا (array)
	    array->attach_word(array_addr, obj);
      وإلا إذا (obj)
	    vpip_attach_to_scope(scope,obj);

      حرر(my_label);
      delete[] name;
}

ساكن فراغ __compile_net(حرف*label,
			  حرف*name, حرف*array_label, unsigned long array_addr,
			  صحيح msb, صحيح lsb,
			  صحيح vpi_type_code, منطقي signed_flag, منطقي local_flag,
			  unsigned argc, بنية symb_s*argv)
{
      vvp_array_t array = array_label? array_find(array_label) : عدم;
      أكد(array_label ? array!=0 : true);

      حرر(array_label);

      أكد(argc == 1);
      vvp_net_t*تۈگۈن = vvp_net_lookup(argv[0].text);
#إذا 1
      إذا (تۈگۈن == 0) {
	      /* No existing net, but the string value may be a
		 constant. In that case, we will wind up generating a
		 bufz node that can carry the constant value.

	         NOTE: This is a hack! The code generator should be
	         fixed so that this is no longer needed. */
	    تۈگۈن = create_constant_تۈگۈن(argv[0].text);
      }
#endif
      إذا (تۈگۈن == 0) {
	    __vpiScope*scope = vpip_peek_current_scope();
	    __compile_net_resolv*res
		  = جديد __compile_net_resolv(argv[0].text,
					     array, scope, label, name,
					     msb, lsb, array_addr,
					     vpi_type_code, signed_flag, local_flag);
	    resolv_submit(res);
	    حرر(argv);
	    أرجع;
      }
      أكد(تۈگۈن);

      __vpiScope*scope = vpip_peek_current_scope();
      do_compile_net(تۈگۈن, array, scope, label, name, msb, lsb, array_addr,
		     vpi_type_code, signed_flag, local_flag);

      حرر(argv[0].text);
      حرر(argv);
}

منطقي __compile_net_resolv::resolve(منطقي ئۇچۇر_flag)
{
      vvp_net_t*تۈگۈن = vvp_net_lookup(label());
      إذا (تۈگۈن == 0) {
	    إذا (ئۇچۇر_flag)
		  cerr << "Unable to resolve label " << label() << endl;
	    أرجع خطأ;
      }

      do_compile_net(تۈگۈن, array_, scope_, my_label_, name_, msb_, lsb_, array_addr_, vpi_type_code_, signed_flag_, local_flag_);
      أرجع true;
}

فراغ compile_net(حرف*label, حرف*name, صحيح msb, صحيح lsb,
		 صحيح vpi_type_code, منطقي signed_flag, منطقي local_flag,
		 unsigned argc, بنية symb_s*argv)
{
      __compile_net(label, name, 0, 0, msb, lsb,
		    vpi_type_code, signed_flag, local_flag,
		    argc, argv);
}

فراغ compile_netw(حرف*label, حرف*array_label, unsigned long array_addr,
		  صحيح msb, صحيح lsb, صحيح vpi_type_code, منطقي signed_flag,
		  unsigned argc, بنية symb_s*argv)
{
      __compile_net(label, 0, array_label, array_addr,
		    msb, lsb, vpi_type_code, signed_flag, خطأ,
		    argc, argv);
}

صنف __compile_real_net_resolv : عام base_net_resolv {

    عام:
      explicit __compile_real_net_resolv(حرف*ref_label, vvp_array_t array,
					 __vpiScope*scope,
					 حرف*my_label, حرف*name,
					 unsigned array_addr, منطقي local_flag)
      : base_net_resolv(ref_label, array, scope, my_label, name, array_addr, local_flag)
      {
      }

      ~__compile_real_net_resolv() override { }

      منطقي resolve(منطقي message_flag) override;

    خاص:
};

ساكن فراغ __compile_real_net2(vvp_net_t*تۈگۈن, vvp_array_t array,
				__vpiScope*scope,
				حرف*my_label, حرف*name,
				unsigned array_addr, منطقي local_flag)
{
      vvp_wire_base*fil = dynamic_cast<vvp_wire_base*> (تۈگۈن->fil);

      إذا (fil == 0) {
	    fil = جديد vvp_wire_real;
	    تۈگۈن->fil = fil;
      }

      vpiHandle obj = 0;
      إذا (!local_flag) {
	    obj = vpip_make_real_net(scope, name, تۈگۈن);
	    compile_vpi_بەلگە(my_label, obj);
      }
#ifdef CHECK_WITH_VALGRIND
      وإلا pool_local_net(تۈگۈن);
#endif

	// REMOVE ME! Giving the net a label is a legacy of the times
	// when the .net was a functor of its own. In the long run, we
	// must fix the code generator to not rely on the label of the
	// .net, then we will remove that label.
      define_functor_بەلگە(my_label, تۈگۈن);

     إذا (array)
	    array->attach_word(array_addr, obj);
      وإلا إذا (obj)
	    vpip_attach_to_scope(scope, obj);

      حرر(my_label);
      delete[] name;
}

ساكن فراغ __compile_real(حرف*label, حرف*name,
                           حرف*array_label, unsigned long array_addr,
                           صحيح msb, صحيح lsb, منطقي local_flag,
                           unsigned argc, بنية symb_s*argv)
{
      أكد(msb == 0 && lsb == 0);
      vvp_array_t array = array_label ? array_find(array_label) : عدم;
      أكد(array_label ? array!=0 : true);

      حرر(array_label);

      أكد(argc == 1);
      vvp_net_t*تۈگۈن = vvp_net_lookup(argv[0].text);
      إذا (تۈگۈن == 0) {
	      /* No existing net, but the string value may be a
		 constant. In that case, we will wind up generating a
		 bufz node that can carry the constant value. */
	    تۈگۈن = create_constant_تۈگۈن(argv[0].text);
      }
      __vpiScope*scope = vpip_peek_current_scope();
      إذا (تۈگۈن == 0) {
	    __compile_real_net_resolv*res
		  = جديد __compile_real_net_resolv(argv[0].text, array,
						  scope, label, name,
						  array_addr, local_flag);
	    resolv_submit(res);
	    حرر(argv);
	    أرجع;
      }

      أكد(تۈگۈن);
      __compile_real_net2(تۈگۈن, array, scope, label, name, array_addr,
                          local_flag);
      حرر(argv[0].text);
      حرر(argv);
}

منطقي __compile_real_net_resolv::resolve(منطقي ئۇچۇر_flag)
{
      vvp_net_t*تۈگۈن = vvp_net_lookup(label());
      إذا (تۈگۈن == 0) {
	    إذا (ئۇچۇر_flag)
		  cerr << "Unable to resolve label " << label() << endl;
	    أرجع خطأ;
      }

      __compile_real_net2(تۈگۈن, array_, scope_, my_label_, name_, array_addr_, local_flag_);
      أرجع true;
}

فراغ compile_net_real(حرف*label, حرف*name, صحيح msb, صحيح lsb, منطقي local_flag,
		      unsigned argc, بنية symb_s*argv)
{
      __compile_real(label, name, 0, 0,
                     msb, lsb, local_flag, argc, argv);
}

فراغ compile_netw_real(حرف*label, حرف*array_label, unsigned long array_addr,
                       صحيح msb, صحيح lsb,
                       unsigned argc, بنية symb_s*argv)
{
      __compile_real(label, 0, array_label, array_addr,
                     msb, lsb, خطأ, argc, argv);
}
