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

using ನಾಮಾಕಾಶ std;

ಸ್ಥಿರ ಶೂನ್ಯ __compile_var_real(ವರ್ಣ*label, ವರ್ಣ*name,
			       vvp_array_t array, unsigned long array_addr,
			       ತಾರ್ಕಿಕ local_flag)
{
      vvp_net_t*net = ನೂತನ vvp_net_t;

      ಯದಿ (vpip_peek_current_scope()->is_automatic()) {
	    vvp_fun_signal_real_aa*tmp = ನೂತನ vvp_fun_signal_real_aa;
	    net->fil = tmp;
	    net->fun = tmp;
      } ಅನ್ಯಥಾ {
	    net->fil = ನೂತನ vvp_wire_real;
	    net->fun = ನೂತನ vvp_fun_signal_real_sa;
      }

      define_functor_ಚಿಹ್ನೆ(label, net);

      vpiHandle obj = vpip_make_real_var(name, net);
      compile_vpi_ಚಿಹ್ನೆ(label, obj);

      ಯದಿ (name) {
	    ಪ್ರತಿಜ್ಞಾ(!array);
	    ಯದಿ (!local_flag)
		  vpip_attach_to_current_scope(obj);
            ಯದಿ (!vpip_peek_current_scope()->is_automatic())
                  schedule_init_vector(vvp_net_ptr_t(net,0), 0.0);
      }
      ಯದಿ (array) {
	    ಪ್ರತಿಜ್ಞಾ(!name);
	    array->attach_word(array_addr, obj);
      }
      ಮುಕ್ತ(label);
      delete[] name;
}

ಶೂನ್ಯ compile_var_real(ವರ್ಣ*label, ವರ್ಣ*name, ತಾರ್ಕಿಕ local_flag)
{
      __compile_var_real(label, name, 0, 0, local_flag);
}

ಶೂನ್ಯ compile_var_string(ವರ್ಣ*label, ವರ್ಣ*name)
{
      vvp_net_t*net = ನೂತನ vvp_net_t;

      ಯದಿ (vpip_peek_current_scope()->is_automatic()) {
	    vvp_fun_signal_string_aa*tmp = ನೂತನ vvp_fun_signal_string_aa;
	    net->fil = tmp;
	    net->fun = tmp;
      } ಅನ್ಯಥಾ {
	    net->fil = 0;
	    net->fun = ನೂತನ vvp_fun_signal_string_sa;
      }

      define_functor_ಚಿಹ್ನೆ(label, net);

      vpiHandle obj = vpip_make_string_var(name, net);
      compile_vpi_ಚಿಹ್ನೆ(label, obj);

      vpip_attach_to_current_scope(obj);
      ಮುಕ್ತ(label);
      delete[] name;
}

ಶೂನ್ಯ compile_var_darray(ವರ್ಣ*label, ವರ್ಣ*name, unsigned size)
{
      vvp_net_t*net = ನೂತನ vvp_net_t;

      ಯದಿ (vpip_peek_current_scope()->is_automatic()) {
	    vvp_fun_signal_object_aa*tmp = ನೂತನ vvp_fun_signal_object_aa(size);
	    net->fil = tmp;
	    net->fun = tmp;
      } ಅನ್ಯಥಾ {
	    net->fil = 0;
	    net->fun = ನೂತನ vvp_fun_signal_object_sa(size);
      }

      define_functor_ಚಿಹ್ನೆ(label, net);

      vpiHandle obj = vpip_make_darray_var(name, net);
      compile_vpi_ಚಿಹ್ನೆ(label, obj);

      vpip_attach_to_current_scope(obj);
      ಮುಕ್ತ(label);
      delete[] name;
}

ಶೂನ್ಯ compile_var_queue(ವರ್ಣ*label, ವರ್ಣ*name, unsigned size)
{
      vvp_net_t*net = ನೂತನ vvp_net_t;

      ಯದಿ (vpip_peek_current_scope()->is_automatic()) {
	    vvp_fun_signal_object_aa*tmp = ನೂತನ vvp_fun_signal_object_aa(size);
	    net->fil = tmp;
	    net->fun = tmp;
      } ಅನ್ಯಥಾ {
	    net->fil = 0;
	    net->fun = ನೂತನ vvp_fun_signal_object_sa(size);
      }

      define_functor_ಚಿಹ್ನೆ(label, net);

      vpiHandle obj = vpip_make_queue_var(name, net);
      compile_vpi_ಚಿಹ್ನೆ(label, obj);

      vpip_attach_to_current_scope(obj);
      ಮುಕ್ತ(label);
      delete[] name;
}

ಶೂನ್ಯ compile_var_cobject(ವರ್ಣ*label, ವರ್ಣ*name)
{
      vvp_net_t*net = ನೂತನ vvp_net_t;

      ಯದಿ (vpip_peek_current_scope()->is_automatic()) {
	    vvp_fun_signal_object_aa*tmp = ನೂತನ vvp_fun_signal_object_aa(1);
	    net->fil = tmp;
	    net->fun = tmp;
      } ಅನ್ಯಥಾ {
	    net->fil = 0;
	    net->fun = ನೂತನ vvp_fun_signal_object_sa(1);
      }

      define_functor_ಚಿಹ್ನೆ(label, net);

      vpiHandle obj = vpip_make_cobject_var(name, net);
      compile_vpi_ಚಿಹ್ನೆ(label, obj);

      vpip_attach_to_current_scope(obj);
      ಮುಕ್ತ(label);
      delete[] name;
}

/*
 * A variable is a special functor, so we allocate that functor and
 * write the label into the symbol table.
 */
ಶೂನ್ಯ compile_variable(ವರ್ಣ*label, ವರ್ಣ*name,
		      ಪೂರ್ಣಾಙ್ಕ msb, ಪೂರ್ಣಾಙ್ಕ lsb, ಪೂರ್ಣಾಙ್ಕ vpi_type_code,
		      ತಾರ್ಕಿಕ signed_flag, ತಾರ್ಕಿಕ local_flag)
{
      unsigned wid = ((msb > lsb)? msb-lsb : lsb-msb) + 1;

      vvp_net_t*net = ನೂತನ vvp_net_t;

      vvp_bit4_t init;
      ಯದಿ (vpi_type_code == vpiIntVar)
	    init = BIT4_0;
      ಅನ್ಯಥಾ
	    init = BIT4_X;

      ಯದಿ (vpip_peek_current_scope()->is_automatic()) {
	    vvp_fun_signal4_aa*tmp = ನೂತನ vvp_fun_signal4_aa(wid, init);
	    net->fil = tmp;
            net->fun = tmp;
      } ಅನ್ಯಥಾ {
	    net->fil = ನೂತನ vvp_wire_vec4(wid, init);
            net->fun = ನೂತನ vvp_fun_signal4_sa(wid);
      }
      ಧ್ರುವ vvp_signal_ಮೌಲ್ಯ*vfil = dynamic_cast<vvp_signal_ಮೌಲ್ಯ*>(net->fil);

      define_functor_ಚಿಹ್ನೆ(label, net);

      vpiHandle obj = 0;
      ಯದಿ (! local_flag) {
	      /* Make the vpiHandle for the reg. */
	    ವಿಕಲ್ಪ (vpi_type_code) {
		ಪಕ್ಷ vpiLogicVar:
		  obj = vpip_make_var4(name, msb, lsb, signed_flag, net);
		  ಭಙ್ಗ;
		ಪಕ್ಷ vpiIntegerVar:
		  obj = vpip_make_int4(name, msb, lsb, net);
		  ಭಙ್ಗ;
		ಪಕ್ಷ vpiIntVar: // This handles all the atom2 int types
		  obj = vpip_make_int2(name, msb, lsb, signed_flag, net);
		  ಭಙ್ಗ;
		ಸ್ವಭಾವ:
		  fprintf(stderr, "internal error: %s: vpi_type_code=%d\n", name, vpi_type_code);
		  ಭಙ್ಗ;
	    }
	    ಪ್ರತಿಜ್ಞಾ(obj);
	    compile_vpi_ಚಿಹ್ನೆ(label, obj);
      }
	// If the signal has a name, then it goes into the current
	// scope as a signal.
      ಯದಿ (name) {
	    ಯದಿ (obj) vpip_attach_to_current_scope(obj);
            ಯದಿ (!vpip_peek_current_scope()->is_automatic()) {
		  vvp_vector4_t tmp;
		  vfil->vec4_ಮೌಲ್ಯ(tmp);
	          schedule_init_vector(vvp_net_ptr_t(net,0), tmp);
	    }
      }

      ಮುಕ್ತ(label);
      delete[] name;
}


vvp_net_t* create_constant_ಗಂಟು(ಧ್ರುವ ವರ್ಣ*val_str)
{
      ಯದಿ (c4string_test(val_str)) {
	    vvp_net_t*net = ನೂತನ vvp_net_t;
	    net->fun = ನೂತನ vvp_fun_bufz;
	    schedule_init_vector(vvp_net_ptr_t(net,0), c4string_to_vector4(val_str));
	    ಪ್ರತ್ಯಾಗಮ net;
      }

      ಯದಿ (c8string_test(val_str)) {
	    vvp_net_t*net = ನೂತನ vvp_net_t;
	    net->fun = ನೂತನ vvp_fun_bufz;
	    schedule_init_vector(vvp_net_ptr_t(net,0), c8string_to_vector8(val_str));
	    ಪ್ರತ್ಯಾಗಮ net;
      }

      ಯದಿ (crstring_test(val_str)) {
	    vvp_net_t*net = ನೂತನ vvp_net_t;
	    net->fun = ನೂತನ vvp_fun_bufz;
	    schedule_init_vector(vvp_net_ptr_t(net,0), crstring_to_double(val_str));
	    ಪ್ರತ್ಯಾಗಮ net;
      }

      ಪ್ರತ್ಯಾಗಮ 0;
}

ವರ್ಗ base_net_resolv : ಸಾರ್ವಜನಿಕ resolv_list_s {
    ಸಾರ್ವಜನಿಕ:
      explicit base_net_resolv(ವರ್ಣ*ref_label, vvp_array_t array,
			       __vpiScope*scope,
			       ವರ್ಣ*my_label, ವರ್ಣ*name,
			       unsigned array_addr, ತಾರ್ಕಿಕ local_flag)
      : resolv_list_s(ref_label)
      { my_label_ = my_label;
	array_ = array;
	name_ = name;
	scope_ = scope;
	array_addr_ = array_addr;
	local_flag_ = local_flag;
      }

    ರಕ್ಷಿತ:
      ವರ್ಣ*my_label_;
      vvp_array_t array_;
      ವರ್ಣ*name_;
      __vpiScope*scope_;
      unsigned array_addr_;
      ತಾರ್ಕಿಕ local_flag_;
};

ವರ್ಗ __compile_net_resolv : ಸಾರ್ವಜನಿಕ base_net_resolv {

    ಸಾರ್ವಜನಿಕ:
      explicit __compile_net_resolv(ವರ್ಣ*ref_label, vvp_array_t array,
				    __vpiScope*scope,
				    ವರ್ಣ*my_label, ವರ್ಣ*name,
				    ಪೂರ್ಣಾಙ್ಕ msb, ಪೂರ್ಣಾಙ್ಕ lsb, unsigned array_addr,
				    ಪೂರ್ಣಾಙ್ಕ vpi_type_code, ತಾರ್ಕಿಕ signed_flag, ತಾರ್ಕಿಕ local_flag)
      : base_net_resolv(ref_label, array, scope, my_label, name, array_addr, local_flag)
      { msb_ = msb;
	lsb_ = lsb;
	vpi_type_code_ = vpi_type_code;
	signed_flag_ = signed_flag;
      }

      ~__compile_net_resolv() override { }

      ತಾರ್ಕಿಕ resolve(ತಾರ್ಕಿಕ message_flag) override;

    ಗುಪ್ತ:
      ಪೂರ್ಣಾಙ್ಕ msb_, lsb_;
      ಪೂರ್ಣಾಙ್ಕ vpi_type_code_;
      ತಾರ್ಕಿಕ signed_flag_;
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

ಸ್ಥಿರ ಶೂನ್ಯ do_compile_net(vvp_net_t*ಗಂಟು, vvp_array_t array,
			   __vpiScope*scope,
			   ವರ್ಣ*my_label, ವರ್ಣ*name,
			   ಪೂರ್ಣಾಙ್ಕ msb, ಪೂರ್ಣಾಙ್ಕ lsb, unsigned array_addr,
			   ಪೂರ್ಣಾಙ್ಕ vpi_type_code, ತಾರ್ಕಿಕ signed_flag, ತಾರ್ಕಿಕ local_flag)
{
      unsigned wid = ((msb > lsb)? msb-lsb : lsb-msb) + 1;
      ಪ್ರತಿಜ್ಞಾ(ಗಂಟು);

      vvp_wire_base*vsig = dynamic_cast<vvp_wire_base*>(ಗಂಟು->fil);

      ಯದಿ (vsig == 0) {
	    ವಿಕಲ್ಪ (vpi_type_code) {
		ಪಕ್ಷ vpiIntVar:
		  vsig = ನೂತನ vvp_wire_vec4(wid,BIT4_0);
		  ಭಙ್ಗ;
		ಪಕ್ಷ vpiLogicVar:
		  vsig = ನೂತನ vvp_wire_vec4(wid,BIT4_Z);
		  ಭಙ್ಗ;
		ಪಕ್ಷ -vpiLogicVar:
		  vsig = ನೂತನ vvp_wire_vec8(wid);
		  ಭಙ್ಗ;
	    }
	    ಪ್ರತಿಜ್ಞಾ(vsig);
	    ಗಂಟು->fil = vsig;
      }

      vpiHandle obj = 0;
      ಯದಿ (! local_flag) {
	      /* Make the vpiHandle for the reg. */
	    obj = vpip_make_net4(scope, name, msb, lsb, signed_flag, ಗಂಟು);
	      /* This attaches the label to the vpiHandle */
	    compile_vpi_ಚಿಹ್ನೆ(my_label, obj);
      }
#ifdef CHECK_WITH_VALGRIND
      ಅನ್ಯಥಾ pool_local_net(ಗಂಟು);
#endif

	// REMOVE ME! Giving the net a label is a legacy of the times
	// when the .net was a functor of its own. In the long run, we
	// must fix the code generator to not rely on the label of the
	// .net, then we will remove that label.
      define_functor_ಚಿಹ್ನೆ(my_label, ಗಂಟು);

      ಯದಿ (array)
	    array->attach_word(array_addr, obj);
      ಅನ್ಯಥಾ ಯದಿ (obj)
	    vpip_attach_to_scope(scope,obj);

      ಮುಕ್ತ(my_label);
      delete[] name;
}

ಸ್ಥಿರ ಶೂನ್ಯ __compile_net(ವರ್ಣ*label,
			  ವರ್ಣ*name, ವರ್ಣ*array_label, unsigned long array_addr,
			  ಪೂರ್ಣಾಙ್ಕ msb, ಪೂರ್ಣಾಙ್ಕ lsb,
			  ಪೂರ್ಣಾಙ್ಕ vpi_type_code, ತಾರ್ಕಿಕ signed_flag, ತಾರ್ಕಿಕ local_flag,
			  unsigned argc, ಸಂರಚನಾ symb_s*argv)
{
      vvp_array_t array = array_label? array_find(array_label) : ರಿಕ್ತ;
      ಪ್ರತಿಜ್ಞಾ(array_label ? array!=0 : ಸತ್ಯ);

      ಮುಕ್ತ(array_label);

      ಪ್ರತಿಜ್ಞಾ(argc == 1);
      vvp_net_t*ಗಂಟು = vvp_net_lookup(argv[0].text);
#ಯದಿ 1
      ಯದಿ (ಗಂಟು == 0) {
	      /* No existing net, but the string value may be a
		 constant. In that case, we will wind up generating a
		 bufz node that can carry the constant value.

	         NOTE: This is a hack! The code generator should be
	         fixed so that this is no longer needed. */
	    ಗಂಟು = create_constant_ಗಂಟು(argv[0].text);
      }
#endif
      ಯದಿ (ಗಂಟು == 0) {
	    __vpiScope*scope = vpip_peek_current_scope();
	    __compile_net_resolv*res
		  = ನೂತನ __compile_net_resolv(argv[0].text,
					     array, scope, label, name,
					     msb, lsb, array_addr,
					     vpi_type_code, signed_flag, local_flag);
	    resolv_submit(res);
	    ಮುಕ್ತ(argv);
	    ಪ್ರತ್ಯಾಗಮ;
      }
      ಪ್ರತಿಜ್ಞಾ(ಗಂಟು);

      __vpiScope*scope = vpip_peek_current_scope();
      do_compile_net(ಗಂಟು, array, scope, label, name, msb, lsb, array_addr,
		     vpi_type_code, signed_flag, local_flag);

      ಮುಕ್ತ(argv[0].text);
      ಮುಕ್ತ(argv);
}

ತಾರ್ಕಿಕ __compile_net_resolv::resolve(ತಾರ್ಕಿಕ ಸಂದೇಶ_flag)
{
      vvp_net_t*ಗಂಟು = vvp_net_lookup(label());
      ಯದಿ (ಗಂಟು == 0) {
	    ಯದಿ (ಸಂದೇಶ_flag)
		  cerr << "Unable to resolve label " << label() << endl;
	    ಪ್ರತ್ಯಾಗಮ ಅಸತ್ಯ;
      }

      do_compile_net(ಗಂಟು, array_, scope_, my_label_, name_, msb_, lsb_, array_addr_, vpi_type_code_, signed_flag_, local_flag_);
      ಪ್ರತ್ಯಾಗಮ ಸತ್ಯ;
}

ಶೂನ್ಯ compile_net(ವರ್ಣ*label, ವರ್ಣ*name, ಪೂರ್ಣಾಙ್ಕ msb, ಪೂರ್ಣಾಙ್ಕ lsb,
		 ಪೂರ್ಣಾಙ್ಕ vpi_type_code, ತಾರ್ಕಿಕ signed_flag, ತಾರ್ಕಿಕ local_flag,
		 unsigned argc, ಸಂರಚನಾ symb_s*argv)
{
      __compile_net(label, name, 0, 0, msb, lsb,
		    vpi_type_code, signed_flag, local_flag,
		    argc, argv);
}

ಶೂನ್ಯ compile_netw(ವರ್ಣ*label, ವರ್ಣ*array_label, unsigned long array_addr,
		  ಪೂರ್ಣಾಙ್ಕ msb, ಪೂರ್ಣಾಙ್ಕ lsb, ಪೂರ್ಣಾಙ್ಕ vpi_type_code, ತಾರ್ಕಿಕ signed_flag,
		  unsigned argc, ಸಂರಚನಾ symb_s*argv)
{
      __compile_net(label, 0, array_label, array_addr,
		    msb, lsb, vpi_type_code, signed_flag, ಅಸತ್ಯ,
		    argc, argv);
}

ವರ್ಗ __compile_real_net_resolv : ಸಾರ್ವಜನಿಕ base_net_resolv {

    ಸಾರ್ವಜನಿಕ:
      explicit __compile_real_net_resolv(ವರ್ಣ*ref_label, vvp_array_t array,
					 __vpiScope*scope,
					 ವರ್ಣ*my_label, ವರ್ಣ*name,
					 unsigned array_addr, ತಾರ್ಕಿಕ local_flag)
      : base_net_resolv(ref_label, array, scope, my_label, name, array_addr, local_flag)
      {
      }

      ~__compile_real_net_resolv() override { }

      ತಾರ್ಕಿಕ resolve(ತಾರ್ಕಿಕ message_flag) override;

    ಗುಪ್ತ:
};

ಸ್ಥಿರ ಶೂನ್ಯ __compile_real_net2(vvp_net_t*ಗಂಟು, vvp_array_t array,
				__vpiScope*scope,
				ವರ್ಣ*my_label, ವರ್ಣ*name,
				unsigned array_addr, ತಾರ್ಕಿಕ local_flag)
{
      vvp_wire_base*fil = dynamic_cast<vvp_wire_base*> (ಗಂಟು->fil);

      ಯದಿ (fil == 0) {
	    fil = ನೂತನ vvp_wire_real;
	    ಗಂಟು->fil = fil;
      }

      vpiHandle obj = 0;
      ಯದಿ (!local_flag) {
	    obj = vpip_make_real_net(scope, name, ಗಂಟು);
	    compile_vpi_ಚಿಹ್ನೆ(my_label, obj);
      }
#ifdef CHECK_WITH_VALGRIND
      ಅನ್ಯಥಾ pool_local_net(ಗಂಟು);
#endif

	// REMOVE ME! Giving the net a label is a legacy of the times
	// when the .net was a functor of its own. In the long run, we
	// must fix the code generator to not rely on the label of the
	// .net, then we will remove that label.
      define_functor_ಚಿಹ್ನೆ(my_label, ಗಂಟು);

     ಯದಿ (array)
	    array->attach_word(array_addr, obj);
      ಅನ್ಯಥಾ ಯದಿ (obj)
	    vpip_attach_to_scope(scope, obj);

      ಮುಕ್ತ(my_label);
      delete[] name;
}

ಸ್ಥಿರ ಶೂನ್ಯ __compile_real(ವರ್ಣ*label, ವರ್ಣ*name,
                           ವರ್ಣ*array_label, unsigned long array_addr,
                           ಪೂರ್ಣಾಙ್ಕ msb, ಪೂರ್ಣಾಙ್ಕ lsb, ತಾರ್ಕಿಕ local_flag,
                           unsigned argc, ಸಂರಚನಾ symb_s*argv)
{
      ಪ್ರತಿಜ್ಞಾ(msb == 0 && lsb == 0);
      vvp_array_t array = array_label ? array_find(array_label) : ರಿಕ್ತ;
      ಪ್ರತಿಜ್ಞಾ(array_label ? array!=0 : ಸತ್ಯ);

      ಮುಕ್ತ(array_label);

      ಪ್ರತಿಜ್ಞಾ(argc == 1);
      vvp_net_t*ಗಂಟು = vvp_net_lookup(argv[0].text);
      ಯದಿ (ಗಂಟು == 0) {
	      /* No existing net, but the string value may be a
		 constant. In that case, we will wind up generating a
		 bufz node that can carry the constant value. */
	    ಗಂಟು = create_constant_ಗಂಟು(argv[0].text);
      }
      __vpiScope*scope = vpip_peek_current_scope();
      ಯದಿ (ಗಂಟು == 0) {
	    __compile_real_net_resolv*res
		  = ನೂತನ __compile_real_net_resolv(argv[0].text, array,
						  scope, label, name,
						  array_addr, local_flag);
	    resolv_submit(res);
	    ಮುಕ್ತ(argv);
	    ಪ್ರತ್ಯಾಗಮ;
      }

      ಪ್ರತಿಜ್ಞಾ(ಗಂಟು);
      __compile_real_net2(ಗಂಟು, array, scope, label, name, array_addr,
                          local_flag);
      ಮುಕ್ತ(argv[0].text);
      ಮುಕ್ತ(argv);
}

ತಾರ್ಕಿಕ __compile_real_net_resolv::resolve(ತಾರ್ಕಿಕ ಸಂದೇಶ_flag)
{
      vvp_net_t*ಗಂಟು = vvp_net_lookup(label());
      ಯದಿ (ಗಂಟು == 0) {
	    ಯದಿ (ಸಂದೇಶ_flag)
		  cerr << "Unable to resolve label " << label() << endl;
	    ಪ್ರತ್ಯಾಗಮ ಅಸತ್ಯ;
      }

      __compile_real_net2(ಗಂಟು, array_, scope_, my_label_, name_, array_addr_, local_flag_);
      ಪ್ರತ್ಯಾಗಮ ಸತ್ಯ;
}

ಶೂನ್ಯ compile_net_real(ವರ್ಣ*label, ವರ್ಣ*name, ಪೂರ್ಣಾಙ್ಕ msb, ಪೂರ್ಣಾಙ್ಕ lsb, ತಾರ್ಕಿಕ local_flag,
		      unsigned argc, ಸಂರಚನಾ symb_s*argv)
{
      __compile_real(label, name, 0, 0,
                     msb, lsb, local_flag, argc, argv);
}

ಶೂನ್ಯ compile_netw_real(ವರ್ಣ*label, ವರ್ಣ*array_label, unsigned long array_addr,
                       ಪೂರ್ಣಾಙ್ಕ msb, ಪೂರ್ಣಾಙ್ಕ lsb,
                       unsigned argc, ಸಂರಚನಾ symb_s*argv)
{
      __compile_real(label, 0, array_label, array_addr,
                     msb, lsb, ಅಸತ್ಯ, argc, argv);
}
