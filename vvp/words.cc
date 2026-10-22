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
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ CHECK_WITH_VALGRIND
# include  "vvp_cleanup.h"
#ಆದರೆ_ಅಂತ್ಯ
# include  <cstdio>
# include  <cstdlib>
# include  <cstring>
# include  <iostream>
# include  <cassert>

ಬಳಸಿ ನಾಮಾಕಾಶ std;

ಸ್ಥಿರ ಶೂನ್ಯ __compile_var_real(ಅಕ್ಷರ*label, ಅಕ್ಷರ*name,
			       vvp_array_t array, ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ array_addr,
			       ತಾರ್ಕಿಕ local_flag)
{
      vvp_net_t*net = ಹೊಸ vvp_net_t;

      ಆದರೆ (vpip_peek_current_scope()->is_automatic()) {
	    vvp_fun_signal_real_aa*tmp = ಹೊಸ vvp_fun_signal_real_aa;
	    net->fil = tmp;
	    net->fun = tmp;
      } ಇಲ್ಲದಿದ್ದರೆ {
	    net->fil = ಹೊಸ vvp_wire_real;
	    net->fun = ಹೊಸ vvp_fun_signal_real_sa;
      }

      define_functor_ಚಿಹ್ನೆ(label, net);

      vpiHandle obj = vpip_make_real_var(name, net);
      compile_vpi_ಚಿಹ್ನೆ(label, obj);

      ಆದರೆ (name) {
	    ಪ್ರತಿಜ್ಞಾ(!array);
	    ಆದರೆ (!local_flag)
		  vpip_attach_ಗಮ್ಯ_current_scope(obj);
            ಆದರೆ (!vpip_peek_current_scope()->is_automatic())
                  ವೇಳಾಪಟ್ಟಿ_init_ಸದಿಶ(vvp_net_ptr_t(net,0), 0.0);
      }
      ಆದರೆ (array) {
	    ಪ್ರತಿಜ್ಞಾ(!name);
	    array->attach_ಶಬ್ದ(array_addr, obj);
      }
      ಬಿಡುಗಡೆ(label);
      ಅಳಿಸು[] name;
}

ಶೂನ್ಯ compile_var_real(ಅಕ್ಷರ*label, ಅಕ್ಷರ*name, ತಾರ್ಕಿಕ local_flag)
{
      __compile_var_real(label, name, 0, 0, local_flag);
}

ಶೂನ್ಯ compile_var_string(ಅಕ್ಷರ*label, ಅಕ್ಷರ*name)
{
      vvp_net_t*net = ಹೊಸ vvp_net_t;

      ಆದರೆ (vpip_peek_current_scope()->is_automatic()) {
	    vvp_fun_signal_string_aa*tmp = ಹೊಸ vvp_fun_signal_string_aa;
	    net->fil = tmp;
	    net->fun = tmp;
      } ಇಲ್ಲದಿದ್ದರೆ {
	    net->fil = 0;
	    net->fun = ಹೊಸ vvp_fun_signal_string_sa;
      }

      define_functor_ಚಿಹ್ನೆ(label, net);

      vpiHandle obj = vpip_make_string_var(name, net);
      compile_vpi_ಚಿಹ್ನೆ(label, obj);

      vpip_attach_ಗಮ್ಯ_current_scope(obj);
      ಬಿಡುಗಡೆ(label);
      ಅಳಿಸು[] name;
}

ಶೂನ್ಯ compile_var_darray(ಅಕ್ಷರ*label, ಅಕ್ಷರ*name, ಚಿಹ್ನೆರಹಿತ size)
{
      vvp_net_t*net = ಹೊಸ vvp_net_t;

      ಆದರೆ (vpip_peek_current_scope()->is_automatic()) {
	    vvp_fun_signal_object_aa*tmp = ಹೊಸ vvp_fun_signal_object_aa(size);
	    net->fil = tmp;
	    net->fun = tmp;
      } ಇಲ್ಲದಿದ್ದರೆ {
	    net->fil = 0;
	    net->fun = ಹೊಸ vvp_fun_signal_object_sa(size);
      }

      define_functor_ಚಿಹ್ನೆ(label, net);

      vpiHandle obj = vpip_make_darray_var(name, net);
      compile_vpi_ಚಿಹ್ನೆ(label, obj);

      vpip_attach_ಗಮ್ಯ_current_scope(obj);
      ಬಿಡುಗಡೆ(label);
      ಅಳಿಸು[] name;
}

ಶೂನ್ಯ compile_var_ಸರತಿ(ಅಕ್ಷರ*label, ಅಕ್ಷರ*name, ಚಿಹ್ನೆರಹಿತ size)
{
      vvp_net_t*net = ಹೊಸ vvp_net_t;

      ಆದರೆ (vpip_peek_current_scope()->is_automatic()) {
	    vvp_fun_signal_object_aa*tmp = ಹೊಸ vvp_fun_signal_object_aa(size);
	    net->fil = tmp;
	    net->fun = tmp;
      } ಇಲ್ಲದಿದ್ದರೆ {
	    net->fil = 0;
	    net->fun = ಹೊಸ vvp_fun_signal_object_sa(size);
      }

      define_functor_ಚಿಹ್ನೆ(label, net);

      vpiHandle obj = vpip_make_ಸರತಿ_var(name, net);
      compile_vpi_ಚಿಹ್ನೆ(label, obj);

      vpip_attach_ಗಮ್ಯ_current_scope(obj);
      ಬಿಡುಗಡೆ(label);
      ಅಳಿಸು[] name;
}

ಶೂನ್ಯ compile_var_cobject(ಅಕ್ಷರ*label, ಅಕ್ಷರ*name)
{
      vvp_net_t*net = ಹೊಸ vvp_net_t;

      ಆದರೆ (vpip_peek_current_scope()->is_automatic()) {
	    vvp_fun_signal_object_aa*tmp = ಹೊಸ vvp_fun_signal_object_aa(1);
	    net->fil = tmp;
	    net->fun = tmp;
      } ಇಲ್ಲದಿದ್ದರೆ {
	    net->fil = 0;
	    net->fun = ಹೊಸ vvp_fun_signal_object_sa(1);
      }

      define_functor_ಚಿಹ್ನೆ(label, net);

      vpiHandle obj = vpip_make_cobject_var(name, net);
      compile_vpi_ಚಿಹ್ನೆ(label, obj);

      vpip_attach_ಗಮ್ಯ_current_scope(obj);
      ಬಿಡುಗಡೆ(label);
      ಅಳಿಸು[] name;
}

/*
 * A variable is a special functor, so we allocate that functor and
 * write the label into the symbol table.
 */
ಶೂನ್ಯ compile_variable(ಅಕ್ಷರ*label, ಅಕ್ಷರ*name,
		      ಪೂರ್ಣಾಂಕ msb, ಪೂರ್ಣಾಂಕ lsb, ಪೂರ್ಣಾಂಕ vpi_type_ಸಂಕೇತ,
		      ತಾರ್ಕಿಕ signed_flag, ತಾರ್ಕಿಕ local_flag)
{
      ಚಿಹ್ನೆರಹಿತ wid = ((msb > lsb)? msb-lsb : lsb-msb) + 1;

      vvp_net_t*net = ಹೊಸ vvp_net_t;

      vvp_bit4_t init;
      ಆದರೆ (vpi_type_ಸಂಕೇತ == vpiIntVar)
	    init = BIT4_0;
      ಇಲ್ಲದಿದ್ದರೆ
	    init = BIT4_X;

      ಆದರೆ (vpip_peek_current_scope()->is_automatic()) {
	    vvp_fun_signal4_aa*tmp = ಹೊಸ vvp_fun_signal4_aa(wid, init);
	    net->fil = tmp;
            net->fun = tmp;
      } ಇಲ್ಲದಿದ್ದರೆ {
	    net->fil = ಹೊಸ vvp_wire_vec4(wid, init);
            net->fun = ಹೊಸ vvp_fun_signal4_sa(wid);
      }
      ಸ್ಥಿರಾಂಕ vvp_signal_ಮೌಲ್ಯ*vfil = dynamic_cast<vvp_signal_ಮೌಲ್ಯ*>(net->fil);

      define_functor_ಚಿಹ್ನೆ(label, net);

      vpiHandle obj = 0;
      ಆದರೆ (! local_flag) {
	      /* Make the vpiHandle for the reg. */
	    ಆಯ್ಕೆ (vpi_type_ಸಂಕೇತ) {
		ಸ್ಥಿತಿ vpiLogicVar:
		  obj = vpip_make_var4(name, msb, lsb, signed_flag, net);
		  ನಿಲ್ಲಿಸು;
		ಸ್ಥಿತಿ vpiIntegerVar:
		  obj = vpip_make_int4(name, msb, lsb, net);
		  ನಿಲ್ಲಿಸು;
		ಸ್ಥಿತಿ vpiIntVar: // This handles all the atom2 int types
		  obj = vpip_make_int2(name, msb, lsb, signed_flag, net);
		  ನಿಲ್ಲಿಸು;
		ಪೂರ್ವನಿಯೋಜಿತ:
		  fprintf(stderr, "internal error: %s: vpi_type_code=%d\n", name, vpi_type_ಸಂಕೇತ);
		  ನಿಲ್ಲಿಸು;
	    }
	    ಪ್ರತಿಜ್ಞಾ(obj);
	    compile_vpi_ಚಿಹ್ನೆ(label, obj);
      }
	// If the signal has a name, then it goes into the current
	// scope as a signal.
      ಆದರೆ (name) {
	    ಆದರೆ (obj) vpip_attach_ಗಮ್ಯ_current_scope(obj);
            ಆದರೆ (!vpip_peek_current_scope()->is_automatic()) {
		  vvp_vector4_t tmp;
		  vfil->vec4_ಮೌಲ್ಯ(tmp);
	          ವೇಳಾಪಟ್ಟಿ_init_ಸದಿಶ(vvp_net_ptr_t(net,0), tmp);
	    }
      }

      ಬಿಡುಗಡೆ(label);
      ಅಳಿಸು[] name;
}


vvp_net_t* create_constant_ಗಂಟು(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ*val_str)
{
      ಆದರೆ (c4string_test(val_str)) {
	    vvp_net_t*net = ಹೊಸ vvp_net_t;
	    net->fun = ಹೊಸ vvp_fun_bufz;
	    ವೇಳಾಪಟ್ಟಿ_init_ಸದಿಶ(vvp_net_ptr_t(net,0), c4string_ಗಮ್ಯ_vector4(val_str));
	    ಹಿಂತಿರುಗಿ net;
      }

      ಆದರೆ (c8string_test(val_str)) {
	    vvp_net_t*net = ಹೊಸ vvp_net_t;
	    net->fun = ಹೊಸ vvp_fun_bufz;
	    ವೇಳಾಪಟ್ಟಿ_init_ಸದಿಶ(vvp_net_ptr_t(net,0), c8string_ಗಮ್ಯ_vector8(val_str));
	    ಹಿಂತಿರುಗಿ net;
      }

      ಆದರೆ (crstring_test(val_str)) {
	    vvp_net_t*net = ಹೊಸ vvp_net_t;
	    net->fun = ಹೊಸ vvp_fun_bufz;
	    ವೇಳಾಪಟ್ಟಿ_init_ಸದಿಶ(vvp_net_ptr_t(net,0), crstring_ಗಮ್ಯ_double(val_str));
	    ಹಿಂತಿರುಗಿ net;
      }

      ಹಿಂತಿರುಗಿ 0;
}

ವರ್ಗ ತಳ_net_resolv : ಸಾರ್ವಜನಿಕ resolv_list_s {
    ಸಾರ್ವಜನಿಕ:
      ಸ್ಪಷ್ಟ ತಳ_net_resolv(ಅಕ್ಷರ*ref_label, vvp_array_t array,
			       __vpiScope*scope,
			       ಅಕ್ಷರ*my_label, ಅಕ್ಷರ*name,
			       ಚಿಹ್ನೆರಹಿತ array_addr, ತಾರ್ಕಿಕ local_flag)
      : resolv_list_s(ref_label)
      { my_label_ = my_label;
	array_ = array;
	name_ = name;
	scope_ = scope;
	array_addr_ = array_addr;
	local_flag_ = local_flag;
      }

    ಸಂರಕ್ಷಿತ:
      ಅಕ್ಷರ*my_label_;
      vvp_array_t array_;
      ಅಕ್ಷರ*name_;
      __vpiScope*scope_;
      ಚಿಹ್ನೆರಹಿತ array_addr_;
      ತಾರ್ಕಿಕ local_flag_;
};

ವರ್ಗ __compile_net_resolv : ಸಾರ್ವಜನಿಕ ತಳ_net_resolv {

    ಸಾರ್ವಜನಿಕ:
      ಸ್ಪಷ್ಟ __compile_net_resolv(ಅಕ್ಷರ*ref_label, vvp_array_t array,
				    __vpiScope*scope,
				    ಅಕ್ಷರ*my_label, ಅಕ್ಷರ*name,
				    ಪೂರ್ಣಾಂಕ msb, ಪೂರ್ಣಾಂಕ lsb, ಚಿಹ್ನೆರಹಿತ array_addr,
				    ಪೂರ್ಣಾಂಕ vpi_type_ಸಂಕೇತ, ತಾರ್ಕಿಕ signed_flag, ತಾರ್ಕಿಕ local_flag)
      : ತಳ_net_resolv(ref_label, array, scope, my_label, name, array_addr, local_flag)
      { msb_ = msb;
	lsb_ = lsb;
	vpi_type_ಸಂಕೇತ_2 = vpi_type_ಸಂಕೇತ;
	signed_flag_ = signed_flag;
      }

      ~__compile_net_resolv() override { }

      ತಾರ್ಕಿಕ resolve(ತಾರ್ಕಿಕ ಸಂದೇಶ_flag) override;

    ಖಾಸಗಿ:
      ಪೂರ್ಣಾಂಕ msb_, lsb_;
      ಪೂರ್ಣಾಂಕ vpi_type_ಸಂಕೇತ_2;
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
			   ಅಕ್ಷರ*my_label, ಅಕ್ಷರ*name,
			   ಪೂರ್ಣಾಂಕ msb, ಪೂರ್ಣಾಂಕ lsb, ಚಿಹ್ನೆರಹಿತ array_addr,
			   ಪೂರ್ಣಾಂಕ vpi_type_ಸಂಕೇತ, ತಾರ್ಕಿಕ signed_flag, ತಾರ್ಕಿಕ local_flag)
{
      ಚಿಹ್ನೆರಹಿತ wid = ((msb > lsb)? msb-lsb : lsb-msb) + 1;
      ಪ್ರತಿಜ್ಞಾ(ಗಂಟು);

      vvp_wire_ತಳ*vsig = dynamic_cast<vvp_wire_ತಳ*>(ಗಂಟು->fil);

      ಆದರೆ (vsig == 0) {
	    ಆಯ್ಕೆ (vpi_type_ಸಂಕೇತ) {
		ಸ್ಥಿತಿ vpiIntVar:
		  vsig = ಹೊಸ vvp_wire_vec4(wid,BIT4_0);
		  ನಿಲ್ಲಿಸು;
		ಸ್ಥಿತಿ vpiLogicVar:
		  vsig = ಹೊಸ vvp_wire_vec4(wid,BIT4_Z);
		  ನಿಲ್ಲಿಸು;
		ಸ್ಥಿತಿ -vpiLogicVar:
		  vsig = ಹೊಸ vvp_wire_vec8(wid);
		  ನಿಲ್ಲಿಸು;
	    }
	    ಪ್ರತಿಜ್ಞಾ(vsig);
	    ಗಂಟು->fil = vsig;
      }

      vpiHandle obj = 0;
      ಆದರೆ (! local_flag) {
	      /* Make the vpiHandle for the reg. */
	    obj = vpip_make_net4(scope, name, msb, lsb, signed_flag, ಗಂಟು);
	      /* This attaches the label to the vpiHandle */
	    compile_vpi_ಚಿಹ್ನೆ(my_label, obj);
      }
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ CHECK_WITH_VALGRIND
      ಇಲ್ಲದಿದ್ದರೆ pool_local_net(ಗಂಟು);
#ಆದರೆ_ಅಂತ್ಯ

	// REMOVE ME! Giving the net a label is a legacy of the times
	// when the .net was a functor of its own. In the long run, we
	// must fix the code generator to not rely on the label of the
	// .net, then we will remove that label.
      define_functor_ಚಿಹ್ನೆ(my_label, ಗಂಟು);

      ಆದರೆ (array)
	    array->attach_ಶಬ್ದ(array_addr, obj);
      ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (obj)
	    vpip_attach_ಗಮ್ಯ_scope(scope,obj);

      ಬಿಡುಗಡೆ(my_label);
      ಅಳಿಸು[] name;
}

ಸ್ಥಿರ ಶೂನ್ಯ __compile_net(ಅಕ್ಷರ*label,
			  ಅಕ್ಷರ*name, ಅಕ್ಷರ*array_label, ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ array_addr,
			  ಪೂರ್ಣಾಂಕ msb, ಪೂರ್ಣಾಂಕ lsb,
			  ಪೂರ್ಣಾಂಕ vpi_type_ಸಂಕೇತ, ತಾರ್ಕಿಕ signed_flag, ತಾರ್ಕಿಕ local_flag,
			  ಚಿಹ್ನೆರಹಿತ argc, ರಚನೆ symb_s*argv)
{
      vvp_array_t array = array_label? array_ಕಂಡುಹಿಡಿ(array_label) : ಖಾಲಿ;
      ಪ್ರತಿಜ್ಞಾ(array_label ? array!=0 : ನಿಜ);

      ಬಿಡುಗಡೆ(array_label);

      ಪ್ರತಿಜ್ಞಾ(argc == 1);
      vvp_net_t*ಗಂಟು = vvp_net_lookup(argv[0].ಪಠ್ಯ);
#ಆದರೆ 1
      ಆದರೆ (ಗಂಟು == 0) {
	      /* No existing net, but the string value may be a
		 constant. In that case, we will wind up generating a
		 bufz node that can carry the constant value.

	         NOTE: This is a hack! The code generator should be
	         fixed so that this is no longer needed. */
	    ಗಂಟು = create_constant_ಗಂಟು(argv[0].ಪಠ್ಯ);
      }
#ಆದರೆ_ಅಂತ್ಯ
      ಆದರೆ (ಗಂಟು == 0) {
	    __vpiScope*scope = vpip_peek_current_scope();
	    __compile_net_resolv*res
		  = ಹೊಸ __compile_net_resolv(argv[0].ಪಠ್ಯ,
					     array, scope, label, name,
					     msb, lsb, array_addr,
					     vpi_type_ಸಂಕೇತ, signed_flag, local_flag);
	    resolv_submit(res);
	    ಬಿಡುಗಡೆ(argv);
	    ಹಿಂತಿರುಗಿ;
      }
      ಪ್ರತಿಜ್ಞಾ(ಗಂಟು);

      __vpiScope*scope = vpip_peek_current_scope();
      do_compile_net(ಗಂಟು, array, scope, label, name, msb, lsb, array_addr,
		     vpi_type_ಸಂಕೇತ, signed_flag, local_flag);

      ಬಿಡುಗಡೆ(argv[0].ಪಠ್ಯ);
      ಬಿಡುಗಡೆ(argv);
}

ತಾರ್ಕಿಕ __compile_net_resolv::resolve(ತಾರ್ಕಿಕ ಸಂದೇಶ_flag_2)
{
      vvp_net_t*ಗಂಟು = vvp_net_lookup(label());
      ಆದರೆ (ಗಂಟು == 0) {
	    ಆದರೆ (ಸಂದೇಶ_flag_2)
		  cerr << "Unable to resolve label " << label() << ಸಾಲು_ಅಂತ್ಯ;
	    ಹಿಂತಿರುಗಿ ಸುಳ್ಳು;
      }

      do_compile_net(ಗಂಟು, array_, scope_, my_label_, name_, msb_, lsb_, array_addr_, vpi_type_ಸಂಕೇತ_2, signed_flag_, local_flag_);
      ಹಿಂತಿರುಗಿ ನಿಜ;
}

ಶೂನ್ಯ compile_net(ಅಕ್ಷರ*label, ಅಕ್ಷರ*name, ಪೂರ್ಣಾಂಕ msb, ಪೂರ್ಣಾಂಕ lsb,
		 ಪೂರ್ಣಾಂಕ vpi_type_ಸಂಕೇತ, ತಾರ್ಕಿಕ signed_flag, ತಾರ್ಕಿಕ local_flag,
		 ಚಿಹ್ನೆರಹಿತ argc, ರಚನೆ symb_s*argv)
{
      __compile_net(label, name, 0, 0, msb, lsb,
		    vpi_type_ಸಂಕೇತ, signed_flag, local_flag,
		    argc, argv);
}

ಶೂನ್ಯ compile_netw(ಅಕ್ಷರ*label, ಅಕ್ಷರ*array_label, ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ array_addr,
		  ಪೂರ್ಣಾಂಕ msb, ಪೂರ್ಣಾಂಕ lsb, ಪೂರ್ಣಾಂಕ vpi_type_ಸಂಕೇತ, ತಾರ್ಕಿಕ signed_flag,
		  ಚಿಹ್ನೆರಹಿತ argc, ರಚನೆ symb_s*argv)
{
      __compile_net(label, 0, array_label, array_addr,
		    msb, lsb, vpi_type_ಸಂಕೇತ, signed_flag, ಸುಳ್ಳು,
		    argc, argv);
}

ವರ್ಗ __compile_real_net_resolv : ಸಾರ್ವಜನಿಕ ತಳ_net_resolv {

    ಸಾರ್ವಜನಿಕ:
      ಸ್ಪಷ್ಟ __compile_real_net_resolv(ಅಕ್ಷರ*ref_label, vvp_array_t array,
					 __vpiScope*scope,
					 ಅಕ್ಷರ*my_label, ಅಕ್ಷರ*name,
					 ಚಿಹ್ನೆರಹಿತ array_addr, ತಾರ್ಕಿಕ local_flag)
      : ತಳ_net_resolv(ref_label, array, scope, my_label, name, array_addr, local_flag)
      {
      }

      ~__compile_real_net_resolv() override { }

      ತಾರ್ಕಿಕ resolve(ತಾರ್ಕಿಕ ಸಂದೇಶ_flag) override;

    ಖಾಸಗಿ:
};

ಸ್ಥಿರ ಶೂನ್ಯ __compile_real_net2(vvp_net_t*ಗಂಟು, vvp_array_t array,
				__vpiScope*scope,
				ಅಕ್ಷರ*my_label, ಅಕ್ಷರ*name,
				ಚಿಹ್ನೆರಹಿತ array_addr, ತಾರ್ಕಿಕ local_flag)
{
      vvp_wire_ತಳ*fil = dynamic_cast<vvp_wire_ತಳ*> (ಗಂಟು->fil);

      ಆದರೆ (fil == 0) {
	    fil = ಹೊಸ vvp_wire_real;
	    ಗಂಟು->fil = fil;
      }

      vpiHandle obj = 0;
      ಆದರೆ (!local_flag) {
	    obj = vpip_make_real_net(scope, name, ಗಂಟು);
	    compile_vpi_ಚಿಹ್ನೆ(my_label, obj);
      }
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ CHECK_WITH_VALGRIND
      ಇಲ್ಲದಿದ್ದರೆ pool_local_net(ಗಂಟು);
#ಆದರೆ_ಅಂತ್ಯ

	// REMOVE ME! Giving the net a label is a legacy of the times
	// when the .net was a functor of its own. In the long run, we
	// must fix the code generator to not rely on the label of the
	// .net, then we will remove that label.
      define_functor_ಚಿಹ್ನೆ(my_label, ಗಂಟು);

     ಆದರೆ (array)
	    array->attach_ಶಬ್ದ(array_addr, obj);
      ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (obj)
	    vpip_attach_ಗಮ್ಯ_scope(scope, obj);

      ಬಿಡುಗಡೆ(my_label);
      ಅಳಿಸು[] name;
}

ಸ್ಥಿರ ಶೂನ್ಯ __compile_real(ಅಕ್ಷರ*label, ಅಕ್ಷರ*name,
                           ಅಕ್ಷರ*array_label, ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ array_addr,
                           ಪೂರ್ಣಾಂಕ msb, ಪೂರ್ಣಾಂಕ lsb, ತಾರ್ಕಿಕ local_flag,
                           ಚಿಹ್ನೆರಹಿತ argc, ರಚನೆ symb_s*argv)
{
      ಪ್ರತಿಜ್ಞಾ(msb == 0 && lsb == 0);
      vvp_array_t array = array_label ? array_ಕಂಡುಹಿಡಿ(array_label) : ಖಾಲಿ;
      ಪ್ರತಿಜ್ಞಾ(array_label ? array!=0 : ನಿಜ);

      ಬಿಡುಗಡೆ(array_label);

      ಪ್ರತಿಜ್ಞಾ(argc == 1);
      vvp_net_t*ಗಂಟು = vvp_net_lookup(argv[0].ಪಠ್ಯ);
      ಆದರೆ (ಗಂಟು == 0) {
	      /* No existing net, but the string value may be a
		 constant. In that case, we will wind up generating a
		 bufz node that can carry the constant value. */
	    ಗಂಟು = create_constant_ಗಂಟು(argv[0].ಪಠ್ಯ);
      }
      __vpiScope*scope = vpip_peek_current_scope();
      ಆದರೆ (ಗಂಟು == 0) {
	    __compile_real_net_resolv*res
		  = ಹೊಸ __compile_real_net_resolv(argv[0].ಪಠ್ಯ, array,
						  scope, label, name,
						  array_addr, local_flag);
	    resolv_submit(res);
	    ಬಿಡುಗಡೆ(argv);
	    ಹಿಂತಿರುಗಿ;
      }

      ಪ್ರತಿಜ್ಞಾ(ಗಂಟು);
      __compile_real_net2(ಗಂಟು, array, scope, label, name, array_addr,
                          local_flag);
      ಬಿಡುಗಡೆ(argv[0].ಪಠ್ಯ);
      ಬಿಡುಗಡೆ(argv);
}

ತಾರ್ಕಿಕ __compile_real_net_resolv::resolve(ತಾರ್ಕಿಕ ಸಂದೇಶ_flag_2)
{
      vvp_net_t*ಗಂಟು = vvp_net_lookup(label());
      ಆದರೆ (ಗಂಟು == 0) {
	    ಆದರೆ (ಸಂದೇಶ_flag_2)
		  cerr << "Unable to resolve label " << label() << ಸಾಲು_ಅಂತ್ಯ;
	    ಹಿಂತಿರುಗಿ ಸುಳ್ಳು;
      }

      __compile_real_net2(ಗಂಟು, array_, scope_, my_label_, name_, array_addr_, local_flag_);
      ಹಿಂತಿರುಗಿ ನಿಜ;
}

ಶೂನ್ಯ compile_net_real(ಅಕ್ಷರ*label, ಅಕ್ಷರ*name, ಪೂರ್ಣಾಂಕ msb, ಪೂರ್ಣಾಂಕ lsb, ತಾರ್ಕಿಕ local_flag,
		      ಚಿಹ್ನೆರಹಿತ argc, ರಚನೆ symb_s*argv)
{
      __compile_real(label, name, 0, 0,
                     msb, lsb, local_flag, argc, argv);
}

ಶೂನ್ಯ compile_netw_real(ಅಕ್ಷರ*label, ಅಕ್ಷರ*array_label, ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ array_addr,
                       ಪೂರ್ಣಾಂಕ msb, ಪೂರ್ಣಾಂಕ lsb,
                       ಚಿಹ್ನೆರಹಿತ argc, ರಚನೆ symb_s*argv)
{
      __compile_real(label, 0, array_label, array_addr,
                     msb, lsb, ಸುಳ್ಳು, argc, argv);
}
