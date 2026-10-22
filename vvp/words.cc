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

using নামাকাশ std;

স্থির শূন্য __compile_var_real(বর্ণ*label, বর্ণ*name,
			       vvp_array_t array, unsigned long array_addr,
			       তার্কিক local_flag)
{
      vvp_net_t*net = নূতন vvp_net_t;

      যদি (vpip_peek_current_scope()->is_automatic()) {
	    vvp_fun_signal_real_aa*tmp = নূতন vvp_fun_signal_real_aa;
	    net->fil = tmp;
	    net->fun = tmp;
      } অন্যথা {
	    net->fil = নূতন vvp_wire_real;
	    net->fun = নূতন vvp_fun_signal_real_sa;
      }

      define_functor_প্রতীক(label, net);

      vpiHandle obj = vpip_make_real_var(name, net);
      compile_vpi_প্রতীক(label, obj);

      যদি (name) {
	    প্রতিজ্ঞা(!array);
	    যদি (!local_flag)
		  vpip_attach_to_current_scope(obj);
            যদি (!vpip_peek_current_scope()->is_automatic())
                  schedule_init_vector(vvp_net_ptr_t(net,0), 0.0);
      }
      যদি (array) {
	    প্রতিজ্ঞা(!name);
	    array->attach_word(array_addr, obj);
      }
      মুক্ত(label);
      delete[] name;
}

শূন্য compile_var_real(বর্ণ*label, বর্ণ*name, তার্কিক local_flag)
{
      __compile_var_real(label, name, 0, 0, local_flag);
}

শূন্য compile_var_string(বর্ণ*label, বর্ণ*name)
{
      vvp_net_t*net = নূতন vvp_net_t;

      যদি (vpip_peek_current_scope()->is_automatic()) {
	    vvp_fun_signal_string_aa*tmp = নূতন vvp_fun_signal_string_aa;
	    net->fil = tmp;
	    net->fun = tmp;
      } অন্যথা {
	    net->fil = 0;
	    net->fun = নূতন vvp_fun_signal_string_sa;
      }

      define_functor_প্রতীক(label, net);

      vpiHandle obj = vpip_make_string_var(name, net);
      compile_vpi_প্রতীক(label, obj);

      vpip_attach_to_current_scope(obj);
      মুক্ত(label);
      delete[] name;
}

শূন্য compile_var_darray(বর্ণ*label, বর্ণ*name, unsigned size)
{
      vvp_net_t*net = নূতন vvp_net_t;

      যদি (vpip_peek_current_scope()->is_automatic()) {
	    vvp_fun_signal_object_aa*tmp = নূতন vvp_fun_signal_object_aa(size);
	    net->fil = tmp;
	    net->fun = tmp;
      } অন্যথা {
	    net->fil = 0;
	    net->fun = নূতন vvp_fun_signal_object_sa(size);
      }

      define_functor_প্রতীক(label, net);

      vpiHandle obj = vpip_make_darray_var(name, net);
      compile_vpi_প্রতীক(label, obj);

      vpip_attach_to_current_scope(obj);
      মুক্ত(label);
      delete[] name;
}

শূন্য compile_var_queue(বর্ণ*label, বর্ণ*name, unsigned size)
{
      vvp_net_t*net = নূতন vvp_net_t;

      যদি (vpip_peek_current_scope()->is_automatic()) {
	    vvp_fun_signal_object_aa*tmp = নূতন vvp_fun_signal_object_aa(size);
	    net->fil = tmp;
	    net->fun = tmp;
      } অন্যথা {
	    net->fil = 0;
	    net->fun = নূতন vvp_fun_signal_object_sa(size);
      }

      define_functor_প্রতীক(label, net);

      vpiHandle obj = vpip_make_queue_var(name, net);
      compile_vpi_প্রতীক(label, obj);

      vpip_attach_to_current_scope(obj);
      মুক্ত(label);
      delete[] name;
}

শূন্য compile_var_cobject(বর্ণ*label, বর্ণ*name)
{
      vvp_net_t*net = নূতন vvp_net_t;

      যদি (vpip_peek_current_scope()->is_automatic()) {
	    vvp_fun_signal_object_aa*tmp = নূতন vvp_fun_signal_object_aa(1);
	    net->fil = tmp;
	    net->fun = tmp;
      } অন্যথা {
	    net->fil = 0;
	    net->fun = নূতন vvp_fun_signal_object_sa(1);
      }

      define_functor_প্রতীক(label, net);

      vpiHandle obj = vpip_make_cobject_var(name, net);
      compile_vpi_প্রতীক(label, obj);

      vpip_attach_to_current_scope(obj);
      মুক্ত(label);
      delete[] name;
}

/*
 * A variable is a special functor, so we allocate that functor and
 * write the label into the symbol table.
 */
শূন্য compile_variable(বর্ণ*label, বর্ণ*name,
		      পূর্ণাঙ্ক msb, পূর্ণাঙ্ক lsb, পূর্ণাঙ্ক vpi_type_code,
		      তার্কিক signed_flag, তার্কিক local_flag)
{
      unsigned wid = ((msb > lsb)? msb-lsb : lsb-msb) + 1;

      vvp_net_t*net = নূতন vvp_net_t;

      vvp_bit4_t init;
      যদি (vpi_type_code == vpiIntVar)
	    init = BIT4_0;
      অন্যথা
	    init = BIT4_X;

      যদি (vpip_peek_current_scope()->is_automatic()) {
	    vvp_fun_signal4_aa*tmp = নূতন vvp_fun_signal4_aa(wid, init);
	    net->fil = tmp;
            net->fun = tmp;
      } অন্যথা {
	    net->fil = নূতন vvp_wire_vec4(wid, init);
            net->fun = নূতন vvp_fun_signal4_sa(wid);
      }
      ধ্রুব vvp_signal_মান*vfil = dynamic_cast<vvp_signal_মান*>(net->fil);

      define_functor_প্রতীক(label, net);

      vpiHandle obj = 0;
      যদি (! local_flag) {
	      /* Make the vpiHandle for the reg. */
	    বিকল্প (vpi_type_code) {
		পক্ষ vpiLogicVar:
		  obj = vpip_make_var4(name, msb, lsb, signed_flag, net);
		  ভঙ্গ;
		পক্ষ vpiIntegerVar:
		  obj = vpip_make_int4(name, msb, lsb, net);
		  ভঙ্গ;
		পক্ষ vpiIntVar: // This handles all the atom2 int types
		  obj = vpip_make_int2(name, msb, lsb, signed_flag, net);
		  ভঙ্গ;
		স্বভাব:
		  fprintf(stderr, "internal error: %s: vpi_type_code=%d\n", name, vpi_type_code);
		  ভঙ্গ;
	    }
	    প্রতিজ্ঞা(obj);
	    compile_vpi_প্রতীক(label, obj);
      }
	// If the signal has a name, then it goes into the current
	// scope as a signal.
      যদি (name) {
	    যদি (obj) vpip_attach_to_current_scope(obj);
            যদি (!vpip_peek_current_scope()->is_automatic()) {
		  vvp_vector4_t tmp;
		  vfil->vec4_মান(tmp);
	          schedule_init_vector(vvp_net_ptr_t(net,0), tmp);
	    }
      }

      মুক্ত(label);
      delete[] name;
}


vvp_net_t* create_constant_গ্রন্থি(ধ্রুব বর্ণ*val_str)
{
      যদি (c4string_test(val_str)) {
	    vvp_net_t*net = নূতন vvp_net_t;
	    net->fun = নূতন vvp_fun_bufz;
	    schedule_init_vector(vvp_net_ptr_t(net,0), c4string_to_vector4(val_str));
	    প্রত্যাগম net;
      }

      যদি (c8string_test(val_str)) {
	    vvp_net_t*net = নূতন vvp_net_t;
	    net->fun = নূতন vvp_fun_bufz;
	    schedule_init_vector(vvp_net_ptr_t(net,0), c8string_to_vector8(val_str));
	    প্রত্যাগম net;
      }

      যদি (crstring_test(val_str)) {
	    vvp_net_t*net = নূতন vvp_net_t;
	    net->fun = নূতন vvp_fun_bufz;
	    schedule_init_vector(vvp_net_ptr_t(net,0), crstring_to_double(val_str));
	    প্রত্যাগম net;
      }

      প্রত্যাগম 0;
}

বর্গ base_net_resolv : সার্বজনিক resolv_list_s {
    সার্বজনিক:
      explicit base_net_resolv(বর্ণ*ref_label, vvp_array_t array,
			       __vpiScope*scope,
			       বর্ণ*my_label, বর্ণ*name,
			       unsigned array_addr, তার্কিক local_flag)
      : resolv_list_s(ref_label)
      { my_label_ = my_label;
	array_ = array;
	name_ = name;
	scope_ = scope;
	array_addr_ = array_addr;
	local_flag_ = local_flag;
      }

    রক্ষিত:
      বর্ণ*my_label_;
      vvp_array_t array_;
      বর্ণ*name_;
      __vpiScope*scope_;
      unsigned array_addr_;
      তার্কিক local_flag_;
};

বর্গ __compile_net_resolv : সার্বজনিক base_net_resolv {

    সার্বজনিক:
      explicit __compile_net_resolv(বর্ণ*ref_label, vvp_array_t array,
				    __vpiScope*scope,
				    বর্ণ*my_label, বর্ণ*name,
				    পূর্ণাঙ্ক msb, পূর্ণাঙ্ক lsb, unsigned array_addr,
				    পূর্ণাঙ্ক vpi_type_code, তার্কিক signed_flag, তার্কিক local_flag)
      : base_net_resolv(ref_label, array, scope, my_label, name, array_addr, local_flag)
      { msb_ = msb;
	lsb_ = lsb;
	vpi_type_code_ = vpi_type_code;
	signed_flag_ = signed_flag;
      }

      ~__compile_net_resolv() override { }

      তার্কিক resolve(তার্কিক message_flag) override;

    গুপ্ত:
      পূর্ণাঙ্ক msb_, lsb_;
      পূর্ণাঙ্ক vpi_type_code_;
      তার্কিক signed_flag_;
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

স্থির শূন্য do_compile_net(vvp_net_t*গ্রন্থি, vvp_array_t array,
			   __vpiScope*scope,
			   বর্ণ*my_label, বর্ণ*name,
			   পূর্ণাঙ্ক msb, পূর্ণাঙ্ক lsb, unsigned array_addr,
			   পূর্ণাঙ্ক vpi_type_code, তার্কিক signed_flag, তার্কিক local_flag)
{
      unsigned wid = ((msb > lsb)? msb-lsb : lsb-msb) + 1;
      প্রতিজ্ঞা(গ্রন্থি);

      vvp_wire_base*vsig = dynamic_cast<vvp_wire_base*>(গ্রন্থি->fil);

      যদি (vsig == 0) {
	    বিকল্প (vpi_type_code) {
		পক্ষ vpiIntVar:
		  vsig = নূতন vvp_wire_vec4(wid,BIT4_0);
		  ভঙ্গ;
		পক্ষ vpiLogicVar:
		  vsig = নূতন vvp_wire_vec4(wid,BIT4_Z);
		  ভঙ্গ;
		পক্ষ -vpiLogicVar:
		  vsig = নূতন vvp_wire_vec8(wid);
		  ভঙ্গ;
	    }
	    প্রতিজ্ঞা(vsig);
	    গ্রন্থি->fil = vsig;
      }

      vpiHandle obj = 0;
      যদি (! local_flag) {
	      /* Make the vpiHandle for the reg. */
	    obj = vpip_make_net4(scope, name, msb, lsb, signed_flag, গ্রন্থি);
	      /* This attaches the label to the vpiHandle */
	    compile_vpi_প্রতীক(my_label, obj);
      }
#ifdef CHECK_WITH_VALGRIND
      অন্যথা pool_local_net(গ্রন্থি);
#endif

	// REMOVE ME! Giving the net a label is a legacy of the times
	// when the .net was a functor of its own. In the long run, we
	// must fix the code generator to not rely on the label of the
	// .net, then we will remove that label.
      define_functor_প্রতীক(my_label, গ্রন্থি);

      যদি (array)
	    array->attach_word(array_addr, obj);
      অন্যথা যদি (obj)
	    vpip_attach_to_scope(scope,obj);

      মুক্ত(my_label);
      delete[] name;
}

স্থির শূন্য __compile_net(বর্ণ*label,
			  বর্ণ*name, বর্ণ*array_label, unsigned long array_addr,
			  পূর্ণাঙ্ক msb, পূর্ণাঙ্ক lsb,
			  পূর্ণাঙ্ক vpi_type_code, তার্কিক signed_flag, তার্কিক local_flag,
			  unsigned argc, সংরচনা symb_s*argv)
{
      vvp_array_t array = array_label? array_find(array_label) : রিক্ত;
      প্রতিজ্ঞা(array_label ? array!=0 : সত্য);

      মুক্ত(array_label);

      প্রতিজ্ঞা(argc == 1);
      vvp_net_t*গ্রন্থি = vvp_net_lookup(argv[0].text);
#যদি 1
      যদি (গ্রন্থি == 0) {
	      /* No existing net, but the string value may be a
		 constant. In that case, we will wind up generating a
		 bufz node that can carry the constant value.

	         NOTE: This is a hack! The code generator should be
	         fixed so that this is no longer needed. */
	    গ্রন্থি = create_constant_গ্রন্থি(argv[0].text);
      }
#endif
      যদি (গ্রন্থি == 0) {
	    __vpiScope*scope = vpip_peek_current_scope();
	    __compile_net_resolv*res
		  = নূতন __compile_net_resolv(argv[0].text,
					     array, scope, label, name,
					     msb, lsb, array_addr,
					     vpi_type_code, signed_flag, local_flag);
	    resolv_submit(res);
	    মুক্ত(argv);
	    প্রত্যাগম;
      }
      প্রতিজ্ঞা(গ্রন্থি);

      __vpiScope*scope = vpip_peek_current_scope();
      do_compile_net(গ্রন্থি, array, scope, label, name, msb, lsb, array_addr,
		     vpi_type_code, signed_flag, local_flag);

      মুক্ত(argv[0].text);
      মুক্ত(argv);
}

তার্কিক __compile_net_resolv::resolve(তার্কিক বার্তা_flag)
{
      vvp_net_t*গ্রন্থি = vvp_net_lookup(label());
      যদি (গ্রন্থি == 0) {
	    যদি (বার্তা_flag)
		  cerr << "Unable to resolve label " << label() << endl;
	    প্রত্যাগম অসত্য;
      }

      do_compile_net(গ্রন্থি, array_, scope_, my_label_, name_, msb_, lsb_, array_addr_, vpi_type_code_, signed_flag_, local_flag_);
      প্রত্যাগম সত্য;
}

শূন্য compile_net(বর্ণ*label, বর্ণ*name, পূর্ণাঙ্ক msb, পূর্ণাঙ্ক lsb,
		 পূর্ণাঙ্ক vpi_type_code, তার্কিক signed_flag, তার্কিক local_flag,
		 unsigned argc, সংরচনা symb_s*argv)
{
      __compile_net(label, name, 0, 0, msb, lsb,
		    vpi_type_code, signed_flag, local_flag,
		    argc, argv);
}

শূন্য compile_netw(বর্ণ*label, বর্ণ*array_label, unsigned long array_addr,
		  পূর্ণাঙ্ক msb, পূর্ণাঙ্ক lsb, পূর্ণাঙ্ক vpi_type_code, তার্কিক signed_flag,
		  unsigned argc, সংরচনা symb_s*argv)
{
      __compile_net(label, 0, array_label, array_addr,
		    msb, lsb, vpi_type_code, signed_flag, অসত্য,
		    argc, argv);
}

বর্গ __compile_real_net_resolv : সার্বজনিক base_net_resolv {

    সার্বজনিক:
      explicit __compile_real_net_resolv(বর্ণ*ref_label, vvp_array_t array,
					 __vpiScope*scope,
					 বর্ণ*my_label, বর্ণ*name,
					 unsigned array_addr, তার্কিক local_flag)
      : base_net_resolv(ref_label, array, scope, my_label, name, array_addr, local_flag)
      {
      }

      ~__compile_real_net_resolv() override { }

      তার্কিক resolve(তার্কিক message_flag) override;

    গুপ্ত:
};

স্থির শূন্য __compile_real_net2(vvp_net_t*গ্রন্থি, vvp_array_t array,
				__vpiScope*scope,
				বর্ণ*my_label, বর্ণ*name,
				unsigned array_addr, তার্কিক local_flag)
{
      vvp_wire_base*fil = dynamic_cast<vvp_wire_base*> (গ্রন্থি->fil);

      যদি (fil == 0) {
	    fil = নূতন vvp_wire_real;
	    গ্রন্থি->fil = fil;
      }

      vpiHandle obj = 0;
      যদি (!local_flag) {
	    obj = vpip_make_real_net(scope, name, গ্রন্থি);
	    compile_vpi_প্রতীক(my_label, obj);
      }
#ifdef CHECK_WITH_VALGRIND
      অন্যথা pool_local_net(গ্রন্থি);
#endif

	// REMOVE ME! Giving the net a label is a legacy of the times
	// when the .net was a functor of its own. In the long run, we
	// must fix the code generator to not rely on the label of the
	// .net, then we will remove that label.
      define_functor_প্রতীক(my_label, গ্রন্থি);

     যদি (array)
	    array->attach_word(array_addr, obj);
      অন্যথা যদি (obj)
	    vpip_attach_to_scope(scope, obj);

      মুক্ত(my_label);
      delete[] name;
}

স্থির শূন্য __compile_real(বর্ণ*label, বর্ণ*name,
                           বর্ণ*array_label, unsigned long array_addr,
                           পূর্ণাঙ্ক msb, পূর্ণাঙ্ক lsb, তার্কিক local_flag,
                           unsigned argc, সংরচনা symb_s*argv)
{
      প্রতিজ্ঞা(msb == 0 && lsb == 0);
      vvp_array_t array = array_label ? array_find(array_label) : রিক্ত;
      প্রতিজ্ঞা(array_label ? array!=0 : সত্য);

      মুক্ত(array_label);

      প্রতিজ্ঞা(argc == 1);
      vvp_net_t*গ্রন্থি = vvp_net_lookup(argv[0].text);
      যদি (গ্রন্থি == 0) {
	      /* No existing net, but the string value may be a
		 constant. In that case, we will wind up generating a
		 bufz node that can carry the constant value. */
	    গ্রন্থি = create_constant_গ্রন্থি(argv[0].text);
      }
      __vpiScope*scope = vpip_peek_current_scope();
      যদি (গ্রন্থি == 0) {
	    __compile_real_net_resolv*res
		  = নূতন __compile_real_net_resolv(argv[0].text, array,
						  scope, label, name,
						  array_addr, local_flag);
	    resolv_submit(res);
	    মুক্ত(argv);
	    প্রত্যাগম;
      }

      প্রতিজ্ঞা(গ্রন্থি);
      __compile_real_net2(গ্রন্থি, array, scope, label, name, array_addr,
                          local_flag);
      মুক্ত(argv[0].text);
      মুক্ত(argv);
}

তার্কিক __compile_real_net_resolv::resolve(তার্কিক বার্তা_flag)
{
      vvp_net_t*গ্রন্থি = vvp_net_lookup(label());
      যদি (গ্রন্থি == 0) {
	    যদি (বার্তা_flag)
		  cerr << "Unable to resolve label " << label() << endl;
	    প্রত্যাগম অসত্য;
      }

      __compile_real_net2(গ্রন্থি, array_, scope_, my_label_, name_, array_addr_, local_flag_);
      প্রত্যাগম সত্য;
}

শূন্য compile_net_real(বর্ণ*label, বর্ণ*name, পূর্ণাঙ্ক msb, পূর্ণাঙ্ক lsb, তার্কিক local_flag,
		      unsigned argc, সংরচনা symb_s*argv)
{
      __compile_real(label, name, 0, 0,
                     msb, lsb, local_flag, argc, argv);
}

শূন্য compile_netw_real(বর্ণ*label, বর্ণ*array_label, unsigned long array_addr,
                       পূর্ণাঙ্ক msb, পূর্ণাঙ্ক lsb,
                       unsigned argc, সংরচনা symb_s*argv)
{
      __compile_real(label, 0, array_label, array_addr,
                     msb, lsb, অসত্য, argc, argv);
}
