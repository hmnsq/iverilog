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

using فضاینام std;

ایستا تهی __compile_var_real(نویسه*label, نویسه*name,
			       vvp_array_t array, unsigned long array_addr,
			       منطقی local_flag)
{
      vvp_net_t*net = جدید vvp_net_t;

      اگر (vpip_peek_current_scope()->is_automatic()) {
	    vvp_fun_signal_real_aa*tmp = جدید vvp_fun_signal_real_aa;
	    net->fil = tmp;
	    net->fun = tmp;
      } وگرنه {
	    net->fil = جدید vvp_wire_real;
	    net->fun = جدید vvp_fun_signal_real_sa;
      }

      define_functor_نماد(label, net);

      vpiHandle obj = vpip_make_real_var(name, net);
      compile_vpi_نماد(label, obj);

      اگر (name) {
	    أكد(!array);
	    اگر (!local_flag)
		  vpip_attach_to_current_scope(obj);
            اگر (!vpip_peek_current_scope()->is_automatic())
                  schedule_init_vector(vvp_net_ptr_t(net,0), 0.0);
      }
      اگر (array) {
	    أكد(!name);
	    array->attach_word(array_addr, obj);
      }
      آزاد(label);
      delete[] name;
}

تهی compile_var_real(نویسه*label, نویسه*name, منطقی local_flag)
{
      __compile_var_real(label, name, 0, 0, local_flag);
}

تهی compile_var_string(نویسه*label, نویسه*name)
{
      vvp_net_t*net = جدید vvp_net_t;

      اگر (vpip_peek_current_scope()->is_automatic()) {
	    vvp_fun_signal_string_aa*tmp = جدید vvp_fun_signal_string_aa;
	    net->fil = tmp;
	    net->fun = tmp;
      } وگرنه {
	    net->fil = 0;
	    net->fun = جدید vvp_fun_signal_string_sa;
      }

      define_functor_نماد(label, net);

      vpiHandle obj = vpip_make_string_var(name, net);
      compile_vpi_نماد(label, obj);

      vpip_attach_to_current_scope(obj);
      آزاد(label);
      delete[] name;
}

تهی compile_var_darray(نویسه*label, نویسه*name, unsigned size)
{
      vvp_net_t*net = جدید vvp_net_t;

      اگر (vpip_peek_current_scope()->is_automatic()) {
	    vvp_fun_signal_object_aa*tmp = جدید vvp_fun_signal_object_aa(size);
	    net->fil = tmp;
	    net->fun = tmp;
      } وگرنه {
	    net->fil = 0;
	    net->fun = جدید vvp_fun_signal_object_sa(size);
      }

      define_functor_نماد(label, net);

      vpiHandle obj = vpip_make_darray_var(name, net);
      compile_vpi_نماد(label, obj);

      vpip_attach_to_current_scope(obj);
      آزاد(label);
      delete[] name;
}

تهی compile_var_queue(نویسه*label, نویسه*name, unsigned size)
{
      vvp_net_t*net = جدید vvp_net_t;

      اگر (vpip_peek_current_scope()->is_automatic()) {
	    vvp_fun_signal_object_aa*tmp = جدید vvp_fun_signal_object_aa(size);
	    net->fil = tmp;
	    net->fun = tmp;
      } وگرنه {
	    net->fil = 0;
	    net->fun = جدید vvp_fun_signal_object_sa(size);
      }

      define_functor_نماد(label, net);

      vpiHandle obj = vpip_make_queue_var(name, net);
      compile_vpi_نماد(label, obj);

      vpip_attach_to_current_scope(obj);
      آزاد(label);
      delete[] name;
}

تهی compile_var_cobject(نویسه*label, نویسه*name)
{
      vvp_net_t*net = جدید vvp_net_t;

      اگر (vpip_peek_current_scope()->is_automatic()) {
	    vvp_fun_signal_object_aa*tmp = جدید vvp_fun_signal_object_aa(1);
	    net->fil = tmp;
	    net->fun = tmp;
      } وگرنه {
	    net->fil = 0;
	    net->fun = جدید vvp_fun_signal_object_sa(1);
      }

      define_functor_نماد(label, net);

      vpiHandle obj = vpip_make_cobject_var(name, net);
      compile_vpi_نماد(label, obj);

      vpip_attach_to_current_scope(obj);
      آزاد(label);
      delete[] name;
}

/*
 * A variable is a special functor, so we allocate that functor and
 * write the label into the symbol table.
 */
تهی compile_variable(نویسه*label, نویسه*name,
		      صحیح msb, صحیح lsb, صحیح vpi_type_code,
		      منطقی signed_flag, منطقی local_flag)
{
      unsigned wid = ((msb > lsb)? msb-lsb : lsb-msb) + 1;

      vvp_net_t*net = جدید vvp_net_t;

      vvp_bit4_t init;
      اگر (vpi_type_code == vpiIntVar)
	    init = BIT4_0;
      وگرنه
	    init = BIT4_X;

      اگر (vpip_peek_current_scope()->is_automatic()) {
	    vvp_fun_signal4_aa*tmp = جدید vvp_fun_signal4_aa(wid, init);
	    net->fil = tmp;
            net->fun = tmp;
      } وگرنه {
	    net->fil = جدید vvp_wire_vec4(wid, init);
            net->fun = جدید vvp_fun_signal4_sa(wid);
      }
      ثابت vvp_signal_مقدار*vfil = dynamic_cast<vvp_signal_مقدار*>(net->fil);

      define_functor_نماد(label, net);

      vpiHandle obj = 0;
      اگر (! local_flag) {
	      /* Make the vpiHandle for the reg. */
	    انتخاب (vpi_type_code) {
		حالت vpiLogicVar:
		  obj = vpip_make_var4(name, msb, lsb, signed_flag, net);
		  بشکن;
		حالت vpiIntegerVar:
		  obj = vpip_make_int4(name, msb, lsb, net);
		  بشکن;
		حالت vpiIntVar: // This handles all the atom2 int types
		  obj = vpip_make_int2(name, msb, lsb, signed_flag, net);
		  بشکن;
		پیشفرض:
		  fprintf(stderr, "internal error: %s: vpi_type_code=%d\n", name, vpi_type_code);
		  بشکن;
	    }
	    أكد(obj);
	    compile_vpi_نماد(label, obj);
      }
	// If the signal has a name, then it goes into the current
	// scope as a signal.
      اگر (name) {
	    اگر (obj) vpip_attach_to_current_scope(obj);
            اگر (!vpip_peek_current_scope()->is_automatic()) {
		  vvp_vector4_t tmp;
		  vfil->vec4_مقدار(tmp);
	          schedule_init_vector(vvp_net_ptr_t(net,0), tmp);
	    }
      }

      آزاد(label);
      delete[] name;
}


vvp_net_t* create_constant_گره(ثابت نویسه*val_str)
{
      اگر (c4string_test(val_str)) {
	    vvp_net_t*net = جدید vvp_net_t;
	    net->fun = جدید vvp_fun_bufz;
	    schedule_init_vector(vvp_net_ptr_t(net,0), c4string_to_vector4(val_str));
	    بازگشت net;
      }

      اگر (c8string_test(val_str)) {
	    vvp_net_t*net = جدید vvp_net_t;
	    net->fun = جدید vvp_fun_bufz;
	    schedule_init_vector(vvp_net_ptr_t(net,0), c8string_to_vector8(val_str));
	    بازگشت net;
      }

      اگر (crstring_test(val_str)) {
	    vvp_net_t*net = جدید vvp_net_t;
	    net->fun = جدید vvp_fun_bufz;
	    schedule_init_vector(vvp_net_ptr_t(net,0), crstring_to_double(val_str));
	    بازگشت net;
      }

      بازگشت 0;
}

رده base_net_resolv : عمومی resolv_list_s {
    عمومی:
      explicit base_net_resolv(نویسه*ref_label, vvp_array_t array,
			       __vpiScope*scope,
			       نویسه*my_label, نویسه*name,
			       unsigned array_addr, منطقی local_flag)
      : resolv_list_s(ref_label)
      { my_label_ = my_label;
	array_ = array;
	name_ = name;
	scope_ = scope;
	array_addr_ = array_addr;
	local_flag_ = local_flag;
      }

    محافظتشده:
      نویسه*my_label_;
      vvp_array_t array_;
      نویسه*name_;
      __vpiScope*scope_;
      unsigned array_addr_;
      منطقی local_flag_;
};

رده __compile_net_resolv : عمومی base_net_resolv {

    عمومی:
      explicit __compile_net_resolv(نویسه*ref_label, vvp_array_t array,
				    __vpiScope*scope,
				    نویسه*my_label, نویسه*name,
				    صحیح msb, صحیح lsb, unsigned array_addr,
				    صحیح vpi_type_code, منطقی signed_flag, منطقی local_flag)
      : base_net_resolv(ref_label, array, scope, my_label, name, array_addr, local_flag)
      { msb_ = msb;
	lsb_ = lsb;
	vpi_type_code_ = vpi_type_code;
	signed_flag_ = signed_flag;
      }

      ~__compile_net_resolv() override { }

      منطقی resolve(منطقی message_flag) override;

    خصوصی:
      صحیح msb_, lsb_;
      صحیح vpi_type_code_;
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

ایستا تهی do_compile_net(vvp_net_t*گره, vvp_array_t array,
			   __vpiScope*scope,
			   نویسه*my_label, نویسه*name,
			   صحیح msb, صحیح lsb, unsigned array_addr,
			   صحیح vpi_type_code, منطقی signed_flag, منطقی local_flag)
{
      unsigned wid = ((msb > lsb)? msb-lsb : lsb-msb) + 1;
      أكد(گره);

      vvp_wire_base*vsig = dynamic_cast<vvp_wire_base*>(گره->fil);

      اگر (vsig == 0) {
	    انتخاب (vpi_type_code) {
		حالت vpiIntVar:
		  vsig = جدید vvp_wire_vec4(wid,BIT4_0);
		  بشکن;
		حالت vpiLogicVar:
		  vsig = جدید vvp_wire_vec4(wid,BIT4_Z);
		  بشکن;
		حالت -vpiLogicVar:
		  vsig = جدید vvp_wire_vec8(wid);
		  بشکن;
	    }
	    أكد(vsig);
	    گره->fil = vsig;
      }

      vpiHandle obj = 0;
      اگر (! local_flag) {
	      /* Make the vpiHandle for the reg. */
	    obj = vpip_make_net4(scope, name, msb, lsb, signed_flag, گره);
	      /* This attaches the label to the vpiHandle */
	    compile_vpi_نماد(my_label, obj);
      }
#ifdef CHECK_WITH_VALGRIND
      وگرنه pool_local_net(گره);
#endif

	// REMOVE ME! Giving the net a label is a legacy of the times
	// when the .net was a functor of its own. In the long run, we
	// must fix the code generator to not rely on the label of the
	// .net, then we will remove that label.
      define_functor_نماد(my_label, گره);

      اگر (array)
	    array->attach_word(array_addr, obj);
      وگرنه اگر (obj)
	    vpip_attach_to_scope(scope,obj);

      آزاد(my_label);
      delete[] name;
}

ایستا تهی __compile_net(نویسه*label,
			  نویسه*name, نویسه*array_label, unsigned long array_addr,
			  صحیح msb, صحیح lsb,
			  صحیح vpi_type_code, منطقی signed_flag, منطقی local_flag,
			  unsigned argc, ساختار symb_s*argv)
{
      vvp_array_t array = array_label? array_find(array_label) : پوچ;
      أكد(array_label ? array!=0 : درست);

      آزاد(array_label);

      أكد(argc == 1);
      vvp_net_t*گره = vvp_net_lookup(argv[0].text);
#اگر 1
      اگر (گره == 0) {
	      /* No existing net, but the string value may be a
		 constant. In that case, we will wind up generating a
		 bufz node that can carry the constant value.

	         NOTE: This is a hack! The code generator should be
	         fixed so that this is no longer needed. */
	    گره = create_constant_گره(argv[0].text);
      }
#endif
      اگر (گره == 0) {
	    __vpiScope*scope = vpip_peek_current_scope();
	    __compile_net_resolv*res
		  = جدید __compile_net_resolv(argv[0].text,
					     array, scope, label, name,
					     msb, lsb, array_addr,
					     vpi_type_code, signed_flag, local_flag);
	    resolv_submit(res);
	    آزاد(argv);
	    بازگشت;
      }
      أكد(گره);

      __vpiScope*scope = vpip_peek_current_scope();
      do_compile_net(گره, array, scope, label, name, msb, lsb, array_addr,
		     vpi_type_code, signed_flag, local_flag);

      آزاد(argv[0].text);
      آزاد(argv);
}

منطقی __compile_net_resolv::resolve(منطقی پیام_flag)
{
      vvp_net_t*گره = vvp_net_lookup(label());
      اگر (گره == 0) {
	    اگر (پیام_flag)
		  cerr << "Unable to resolve label " << label() << endl;
	    بازگشت نادرست;
      }

      do_compile_net(گره, array_, scope_, my_label_, name_, msb_, lsb_, array_addr_, vpi_type_code_, signed_flag_, local_flag_);
      بازگشت درست;
}

تهی compile_net(نویسه*label, نویسه*name, صحیح msb, صحیح lsb,
		 صحیح vpi_type_code, منطقی signed_flag, منطقی local_flag,
		 unsigned argc, ساختار symb_s*argv)
{
      __compile_net(label, name, 0, 0, msb, lsb,
		    vpi_type_code, signed_flag, local_flag,
		    argc, argv);
}

تهی compile_netw(نویسه*label, نویسه*array_label, unsigned long array_addr,
		  صحیح msb, صحیح lsb, صحیح vpi_type_code, منطقی signed_flag,
		  unsigned argc, ساختار symb_s*argv)
{
      __compile_net(label, 0, array_label, array_addr,
		    msb, lsb, vpi_type_code, signed_flag, نادرست,
		    argc, argv);
}

رده __compile_real_net_resolv : عمومی base_net_resolv {

    عمومی:
      explicit __compile_real_net_resolv(نویسه*ref_label, vvp_array_t array,
					 __vpiScope*scope,
					 نویسه*my_label, نویسه*name,
					 unsigned array_addr, منطقی local_flag)
      : base_net_resolv(ref_label, array, scope, my_label, name, array_addr, local_flag)
      {
      }

      ~__compile_real_net_resolv() override { }

      منطقی resolve(منطقی message_flag) override;

    خصوصی:
};

ایستا تهی __compile_real_net2(vvp_net_t*گره, vvp_array_t array,
				__vpiScope*scope,
				نویسه*my_label, نویسه*name,
				unsigned array_addr, منطقی local_flag)
{
      vvp_wire_base*fil = dynamic_cast<vvp_wire_base*> (گره->fil);

      اگر (fil == 0) {
	    fil = جدید vvp_wire_real;
	    گره->fil = fil;
      }

      vpiHandle obj = 0;
      اگر (!local_flag) {
	    obj = vpip_make_real_net(scope, name, گره);
	    compile_vpi_نماد(my_label, obj);
      }
#ifdef CHECK_WITH_VALGRIND
      وگرنه pool_local_net(گره);
#endif

	// REMOVE ME! Giving the net a label is a legacy of the times
	// when the .net was a functor of its own. In the long run, we
	// must fix the code generator to not rely on the label of the
	// .net, then we will remove that label.
      define_functor_نماد(my_label, گره);

     اگر (array)
	    array->attach_word(array_addr, obj);
      وگرنه اگر (obj)
	    vpip_attach_to_scope(scope, obj);

      آزاد(my_label);
      delete[] name;
}

ایستا تهی __compile_real(نویسه*label, نویسه*name,
                           نویسه*array_label, unsigned long array_addr,
                           صحیح msb, صحیح lsb, منطقی local_flag,
                           unsigned argc, ساختار symb_s*argv)
{
      أكد(msb == 0 && lsb == 0);
      vvp_array_t array = array_label ? array_find(array_label) : پوچ;
      أكد(array_label ? array!=0 : درست);

      آزاد(array_label);

      أكد(argc == 1);
      vvp_net_t*گره = vvp_net_lookup(argv[0].text);
      اگر (گره == 0) {
	      /* No existing net, but the string value may be a
		 constant. In that case, we will wind up generating a
		 bufz node that can carry the constant value. */
	    گره = create_constant_گره(argv[0].text);
      }
      __vpiScope*scope = vpip_peek_current_scope();
      اگر (گره == 0) {
	    __compile_real_net_resolv*res
		  = جدید __compile_real_net_resolv(argv[0].text, array,
						  scope, label, name,
						  array_addr, local_flag);
	    resolv_submit(res);
	    آزاد(argv);
	    بازگشت;
      }

      أكد(گره);
      __compile_real_net2(گره, array, scope, label, name, array_addr,
                          local_flag);
      آزاد(argv[0].text);
      آزاد(argv);
}

منطقی __compile_real_net_resolv::resolve(منطقی پیام_flag)
{
      vvp_net_t*گره = vvp_net_lookup(label());
      اگر (گره == 0) {
	    اگر (پیام_flag)
		  cerr << "Unable to resolve label " << label() << endl;
	    بازگشت نادرست;
      }

      __compile_real_net2(گره, array_, scope_, my_label_, name_, array_addr_, local_flag_);
      بازگشت درست;
}

تهی compile_net_real(نویسه*label, نویسه*name, صحیح msb, صحیح lsb, منطقی local_flag,
		      unsigned argc, ساختار symb_s*argv)
{
      __compile_real(label, name, 0, 0,
                     msb, lsb, local_flag, argc, argv);
}

تهی compile_netw_real(نویسه*label, نویسه*array_label, unsigned long array_addr,
                       صحیح msb, صحیح lsb,
                       unsigned argc, ساختار symb_s*argv)
{
      __compile_real(label, 0, array_label, array_addr,
                     msb, lsb, نادرست, argc, argv);
}
