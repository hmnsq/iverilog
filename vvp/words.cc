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
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ CHECK_WITH_VALGRIND
# include  "vvp_cleanup.h"
#ਜੇ_ਅੰਤ
# include  <cstdio>
# include  <cstdlib>
# include  <cstring>
# include  <iostream>
# include  <cassert>

ਵਰਤੋਂ ਨਾਮਾਕਾਸ਼ std;

ਸਥਿਰ ਖਾਲੀ __compile_var_real(ਅੱਖਰ*label, ਅੱਖਰ*name,
			       vvp_array_t array, ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ array_addr,
			       ਤਾਰਕਿਕ local_flag)
{
      vvp_net_t*net = ਨਵਾਂ vvp_net_t;

      ਜੇ (vpip_peek_current_scope()->is_automatic()) {
	    vvp_fun_signal_real_aa*tmp = ਨਵਾਂ vvp_fun_signal_real_aa;
	    net->fil = tmp;
	    net->fun = tmp;
      } ਨਹੀਂਤਾਂ {
	    net->fil = ਨਵਾਂ vvp_wire_real;
	    net->fun = ਨਵਾਂ vvp_fun_signal_real_sa;
      }

      define_functor_ਚਿੰਨ੍ਹ(label, net);

      vpiHandle obj = vpip_make_real_var(name, net);
      compile_vpi_ਚਿੰਨ੍ਹ(label, obj);

      ਜੇ (name) {
	    ਪ੍ਰਤਿਜ੍ਞਾ(!array);
	    ਜੇ (!local_flag)
		  vpip_attach_ਮੰਜ਼ਿਲ_current_scope(obj);
            ਜੇ (!vpip_peek_current_scope()->is_automatic())
                  ਸਮਾਂ_ਸਾਰਣੀ_init_ਸਦਿਸ਼(vvp_net_ptr_t(net,0), 0.0);
      }
      ਜੇ (array) {
	    ਪ੍ਰਤਿਜ੍ਞਾ(!name);
	    array->attach_ਸ਼ਬਦ(array_addr, obj);
      }
      ਮੁਕਤ(label);
      ਮਿਟਾਓ[] name;
}

ਖਾਲੀ compile_var_real(ਅੱਖਰ*label, ਅੱਖਰ*name, ਤਾਰਕਿਕ local_flag)
{
      __compile_var_real(label, name, 0, 0, local_flag);
}

ਖਾਲੀ compile_var_string(ਅੱਖਰ*label, ਅੱਖਰ*name)
{
      vvp_net_t*net = ਨਵਾਂ vvp_net_t;

      ਜੇ (vpip_peek_current_scope()->is_automatic()) {
	    vvp_fun_signal_string_aa*tmp = ਨਵਾਂ vvp_fun_signal_string_aa;
	    net->fil = tmp;
	    net->fun = tmp;
      } ਨਹੀਂਤਾਂ {
	    net->fil = 0;
	    net->fun = ਨਵਾਂ vvp_fun_signal_string_sa;
      }

      define_functor_ਚਿੰਨ੍ਹ(label, net);

      vpiHandle obj = vpip_make_string_var(name, net);
      compile_vpi_ਚਿੰਨ੍ਹ(label, obj);

      vpip_attach_ਮੰਜ਼ਿਲ_current_scope(obj);
      ਮੁਕਤ(label);
      ਮਿਟਾਓ[] name;
}

ਖਾਲੀ compile_var_darray(ਅੱਖਰ*label, ਅੱਖਰ*name, ਬਿਨਾਨਿਸ਼ਾਨੀ size)
{
      vvp_net_t*net = ਨਵਾਂ vvp_net_t;

      ਜੇ (vpip_peek_current_scope()->is_automatic()) {
	    vvp_fun_signal_object_aa*tmp = ਨਵਾਂ vvp_fun_signal_object_aa(size);
	    net->fil = tmp;
	    net->fun = tmp;
      } ਨਹੀਂਤਾਂ {
	    net->fil = 0;
	    net->fun = ਨਵਾਂ vvp_fun_signal_object_sa(size);
      }

      define_functor_ਚਿੰਨ੍ਹ(label, net);

      vpiHandle obj = vpip_make_darray_var(name, net);
      compile_vpi_ਚਿੰਨ੍ਹ(label, obj);

      vpip_attach_ਮੰਜ਼ਿਲ_current_scope(obj);
      ਮੁਕਤ(label);
      ਮਿਟਾਓ[] name;
}

ਖਾਲੀ compile_var_ਕਤਾਰ(ਅੱਖਰ*label, ਅੱਖਰ*name, ਬਿਨਾਨਿਸ਼ਾਨੀ size)
{
      vvp_net_t*net = ਨਵਾਂ vvp_net_t;

      ਜੇ (vpip_peek_current_scope()->is_automatic()) {
	    vvp_fun_signal_object_aa*tmp = ਨਵਾਂ vvp_fun_signal_object_aa(size);
	    net->fil = tmp;
	    net->fun = tmp;
      } ਨਹੀਂਤਾਂ {
	    net->fil = 0;
	    net->fun = ਨਵਾਂ vvp_fun_signal_object_sa(size);
      }

      define_functor_ਚਿੰਨ੍ਹ(label, net);

      vpiHandle obj = vpip_make_ਕਤਾਰ_var(name, net);
      compile_vpi_ਚਿੰਨ੍ਹ(label, obj);

      vpip_attach_ਮੰਜ਼ਿਲ_current_scope(obj);
      ਮੁਕਤ(label);
      ਮਿਟਾਓ[] name;
}

ਖਾਲੀ compile_var_cobject(ਅੱਖਰ*label, ਅੱਖਰ*name)
{
      vvp_net_t*net = ਨਵਾਂ vvp_net_t;

      ਜੇ (vpip_peek_current_scope()->is_automatic()) {
	    vvp_fun_signal_object_aa*tmp = ਨਵਾਂ vvp_fun_signal_object_aa(1);
	    net->fil = tmp;
	    net->fun = tmp;
      } ਨਹੀਂਤਾਂ {
	    net->fil = 0;
	    net->fun = ਨਵਾਂ vvp_fun_signal_object_sa(1);
      }

      define_functor_ਚਿੰਨ੍ਹ(label, net);

      vpiHandle obj = vpip_make_cobject_var(name, net);
      compile_vpi_ਚਿੰਨ੍ਹ(label, obj);

      vpip_attach_ਮੰਜ਼ਿਲ_current_scope(obj);
      ਮੁਕਤ(label);
      ਮਿਟਾਓ[] name;
}

/*
 * A variable is a special functor, so we allocate that functor and
 * write the label into the symbol table.
 */
ਖਾਲੀ compile_variable(ਅੱਖਰ*label, ਅੱਖਰ*name,
		      ਪੂਰਨ msb, ਪੂਰਨ lsb, ਪੂਰਨ vpi_type_ਕੋਡ,
		      ਤਾਰਕਿਕ signed_flag, ਤਾਰਕਿਕ local_flag)
{
      ਬਿਨਾਨਿਸ਼ਾਨੀ wid = ((msb > lsb)? msb-lsb : lsb-msb) + 1;

      vvp_net_t*net = ਨਵਾਂ vvp_net_t;

      vvp_bit4_t init;
      ਜੇ (vpi_type_ਕੋਡ == vpiIntVar)
	    init = BIT4_0;
      ਨਹੀਂਤਾਂ
	    init = BIT4_X;

      ਜੇ (vpip_peek_current_scope()->is_automatic()) {
	    vvp_fun_signal4_aa*tmp = ਨਵਾਂ vvp_fun_signal4_aa(wid, init);
	    net->fil = tmp;
            net->fun = tmp;
      } ਨਹੀਂਤਾਂ {
	    net->fil = ਨਵਾਂ vvp_wire_vec4(wid, init);
            net->fun = ਨਵਾਂ vvp_fun_signal4_sa(wid);
      }
      ਅਚਲ vvp_signal_ਮੁੱਲ*vfil = dynamic_cast<vvp_signal_ਮੁੱਲ*>(net->fil);

      define_functor_ਚਿੰਨ੍ਹ(label, net);

      vpiHandle obj = 0;
      ਜੇ (! local_flag) {
	      /* Make the vpiHandle for the reg. */
	    ਚੋਣ (vpi_type_ਕੋਡ) {
		ਹਾਲਤ vpiLogicVar:
		  obj = vpip_make_var4(name, msb, lsb, signed_flag, net);
		  ਰੋਕੋ;
		ਹਾਲਤ vpiIntegerVar:
		  obj = vpip_make_int4(name, msb, lsb, net);
		  ਰੋਕੋ;
		ਹਾਲਤ vpiIntVar: // This handles all the atom2 int types
		  obj = vpip_make_int2(name, msb, lsb, signed_flag, net);
		  ਰੋਕੋ;
		ਮੂਲ:
		  fprintf(stderr, "internal error: %s: vpi_type_code=%d\n", name, vpi_type_ਕੋਡ);
		  ਰੋਕੋ;
	    }
	    ਪ੍ਰਤਿਜ੍ਞਾ(obj);
	    compile_vpi_ਚਿੰਨ੍ਹ(label, obj);
      }
	// If the signal has a name, then it goes into the current
	// scope as a signal.
      ਜੇ (name) {
	    ਜੇ (obj) vpip_attach_ਮੰਜ਼ਿਲ_current_scope(obj);
            ਜੇ (!vpip_peek_current_scope()->is_automatic()) {
		  vvp_vector4_t tmp;
		  vfil->vec4_ਮੁੱਲ(tmp);
	          ਸਮਾਂ_ਸਾਰਣੀ_init_ਸਦਿਸ਼(vvp_net_ptr_t(net,0), tmp);
	    }
      }

      ਮੁਕਤ(label);
      ਮਿਟਾਓ[] name;
}


vvp_net_t* create_constant_ਗੰਢ(ਅਚਲ ਅੱਖਰ*val_str)
{
      ਜੇ (c4string_test(val_str)) {
	    vvp_net_t*net = ਨਵਾਂ vvp_net_t;
	    net->fun = ਨਵਾਂ vvp_fun_bufz;
	    ਸਮਾਂ_ਸਾਰਣੀ_init_ਸਦਿਸ਼(vvp_net_ptr_t(net,0), c4string_ਮੰਜ਼ਿਲ_vector4(val_str));
	    ਵਾਪਸ net;
      }

      ਜੇ (c8string_test(val_str)) {
	    vvp_net_t*net = ਨਵਾਂ vvp_net_t;
	    net->fun = ਨਵਾਂ vvp_fun_bufz;
	    ਸਮਾਂ_ਸਾਰਣੀ_init_ਸਦਿਸ਼(vvp_net_ptr_t(net,0), c8string_ਮੰਜ਼ਿਲ_vector8(val_str));
	    ਵਾਪਸ net;
      }

      ਜੇ (crstring_test(val_str)) {
	    vvp_net_t*net = ਨਵਾਂ vvp_net_t;
	    net->fun = ਨਵਾਂ vvp_fun_bufz;
	    ਸਮਾਂ_ਸਾਰਣੀ_init_ਸਦਿਸ਼(vvp_net_ptr_t(net,0), crstring_ਮੰਜ਼ਿਲ_double(val_str));
	    ਵਾਪਸ net;
      }

      ਵਾਪਸ 0;
}

ਵਰਗ ਅਧਾਰ_net_resolv : ਜਨਤਕ resolv_list_s {
    ਜਨਤਕ:
      ਸਪਸ਼ਟ ਅਧਾਰ_net_resolv(ਅੱਖਰ*ref_label, vvp_array_t array,
			       __vpiScope*scope,
			       ਅੱਖਰ*my_label, ਅੱਖਰ*name,
			       ਬਿਨਾਨਿਸ਼ਾਨੀ array_addr, ਤਾਰਕਿਕ local_flag)
      : resolv_list_s(ref_label)
      { my_label_ = my_label;
	array_ = array;
	name_ = name;
	scope_ = scope;
	array_addr_ = array_addr;
	local_flag_ = local_flag;
      }

    ਸੁਰੱਖਿਅਤ:
      ਅੱਖਰ*my_label_;
      vvp_array_t array_;
      ਅੱਖਰ*name_;
      __vpiScope*scope_;
      ਬਿਨਾਨਿਸ਼ਾਨੀ array_addr_;
      ਤਾਰਕਿਕ local_flag_;
};

ਵਰਗ __compile_net_resolv : ਜਨਤਕ ਅਧਾਰ_net_resolv {

    ਜਨਤਕ:
      ਸਪਸ਼ਟ __compile_net_resolv(ਅੱਖਰ*ref_label, vvp_array_t array,
				    __vpiScope*scope,
				    ਅੱਖਰ*my_label, ਅੱਖਰ*name,
				    ਪੂਰਨ msb, ਪੂਰਨ lsb, ਬਿਨਾਨਿਸ਼ਾਨੀ array_addr,
				    ਪੂਰਨ vpi_type_ਕੋਡ, ਤਾਰਕਿਕ signed_flag, ਤਾਰਕਿਕ local_flag)
      : ਅਧਾਰ_net_resolv(ref_label, array, scope, my_label, name, array_addr, local_flag)
      { msb_ = msb;
	lsb_ = lsb;
	vpi_type_ਕੋਡ_2 = vpi_type_ਕੋਡ;
	signed_flag_ = signed_flag;
      }

      ~__compile_net_resolv() override { }

      ਤਾਰਕਿਕ resolve(ਤਾਰਕਿਕ ਸੁਨੇਹਾ_flag) override;

    ਨਿਜੀ:
      ਪੂਰਨ msb_, lsb_;
      ਪੂਰਨ vpi_type_ਕੋਡ_2;
      ਤਾਰਕਿਕ signed_flag_;
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

ਸਥਿਰ ਖਾਲੀ do_compile_net(vvp_net_t*ਗੰਢ, vvp_array_t array,
			   __vpiScope*scope,
			   ਅੱਖਰ*my_label, ਅੱਖਰ*name,
			   ਪੂਰਨ msb, ਪੂਰਨ lsb, ਬਿਨਾਨਿਸ਼ਾਨੀ array_addr,
			   ਪੂਰਨ vpi_type_ਕੋਡ, ਤਾਰਕਿਕ signed_flag, ਤਾਰਕਿਕ local_flag)
{
      ਬਿਨਾਨਿਸ਼ਾਨੀ wid = ((msb > lsb)? msb-lsb : lsb-msb) + 1;
      ਪ੍ਰਤਿਜ੍ਞਾ(ਗੰਢ);

      vvp_wire_ਅਧਾਰ*vsig = dynamic_cast<vvp_wire_ਅਧਾਰ*>(ਗੰਢ->fil);

      ਜੇ (vsig == 0) {
	    ਚੋਣ (vpi_type_ਕੋਡ) {
		ਹਾਲਤ vpiIntVar:
		  vsig = ਨਵਾਂ vvp_wire_vec4(wid,BIT4_0);
		  ਰੋਕੋ;
		ਹਾਲਤ vpiLogicVar:
		  vsig = ਨਵਾਂ vvp_wire_vec4(wid,BIT4_Z);
		  ਰੋਕੋ;
		ਹਾਲਤ -vpiLogicVar:
		  vsig = ਨਵਾਂ vvp_wire_vec8(wid);
		  ਰੋਕੋ;
	    }
	    ਪ੍ਰਤਿਜ੍ਞਾ(vsig);
	    ਗੰਢ->fil = vsig;
      }

      vpiHandle obj = 0;
      ਜੇ (! local_flag) {
	      /* Make the vpiHandle for the reg. */
	    obj = vpip_make_net4(scope, name, msb, lsb, signed_flag, ਗੰਢ);
	      /* This attaches the label to the vpiHandle */
	    compile_vpi_ਚਿੰਨ੍ਹ(my_label, obj);
      }
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ CHECK_WITH_VALGRIND
      ਨਹੀਂਤਾਂ pool_local_net(ਗੰਢ);
#ਜੇ_ਅੰਤ

	// REMOVE ME! Giving the net a label is a legacy of the times
	// when the .net was a functor of its own. In the long run, we
	// must fix the code generator to not rely on the label of the
	// .net, then we will remove that label.
      define_functor_ਚਿੰਨ੍ਹ(my_label, ਗੰਢ);

      ਜੇ (array)
	    array->attach_ਸ਼ਬਦ(array_addr, obj);
      ਨਹੀਂਤਾਂ ਜੇ (obj)
	    vpip_attach_ਮੰਜ਼ਿਲ_scope(scope,obj);

      ਮੁਕਤ(my_label);
      ਮਿਟਾਓ[] name;
}

ਸਥਿਰ ਖਾਲੀ __compile_net(ਅੱਖਰ*label,
			  ਅੱਖਰ*name, ਅੱਖਰ*array_label, ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ array_addr,
			  ਪੂਰਨ msb, ਪੂਰਨ lsb,
			  ਪੂਰਨ vpi_type_ਕੋਡ, ਤਾਰਕਿਕ signed_flag, ਤਾਰਕਿਕ local_flag,
			  ਬਿਨਾਨਿਸ਼ਾਨੀ argc, ਬਣਤਰ symb_s*argv)
{
      vvp_array_t array = array_label? array_ਲੱਭੋ(array_label) : ਨਿਰਾ;
      ਪ੍ਰਤਿਜ੍ਞਾ(array_label ? array!=0 : ਸੱਚ);

      ਮੁਕਤ(array_label);

      ਪ੍ਰਤਿਜ੍ਞਾ(argc == 1);
      vvp_net_t*ਗੰਢ = vvp_net_lookup(argv[0].ਪਾਠ);
#ਜੇ 1
      ਜੇ (ਗੰਢ == 0) {
	      /* No existing net, but the string value may be a
		 constant. In that case, we will wind up generating a
		 bufz node that can carry the constant value.

	         NOTE: This is a hack! The code generator should be
	         fixed so that this is no longer needed. */
	    ਗੰਢ = create_constant_ਗੰਢ(argv[0].ਪਾਠ);
      }
#ਜੇ_ਅੰਤ
      ਜੇ (ਗੰਢ == 0) {
	    __vpiScope*scope = vpip_peek_current_scope();
	    __compile_net_resolv*res
		  = ਨਵਾਂ __compile_net_resolv(argv[0].ਪਾਠ,
					     array, scope, label, name,
					     msb, lsb, array_addr,
					     vpi_type_ਕੋਡ, signed_flag, local_flag);
	    resolv_submit(res);
	    ਮੁਕਤ(argv);
	    ਵਾਪਸ;
      }
      ਪ੍ਰਤਿਜ੍ਞਾ(ਗੰਢ);

      __vpiScope*scope = vpip_peek_current_scope();
      do_compile_net(ਗੰਢ, array, scope, label, name, msb, lsb, array_addr,
		     vpi_type_ਕੋਡ, signed_flag, local_flag);

      ਮੁਕਤ(argv[0].ਪਾਠ);
      ਮੁਕਤ(argv);
}

ਤਾਰਕਿਕ __compile_net_resolv::resolve(ਤਾਰਕਿਕ ਸੁਨੇਹਾ_flag_2)
{
      vvp_net_t*ਗੰਢ = vvp_net_lookup(label());
      ਜੇ (ਗੰਢ == 0) {
	    ਜੇ (ਸੁਨੇਹਾ_flag_2)
		  cerr << "Unable to resolve label " << label() << ਸਤਰ_ਅੰਤ;
	    ਵਾਪਸ ਝੂਠ;
      }

      do_compile_net(ਗੰਢ, array_, scope_, my_label_, name_, msb_, lsb_, array_addr_, vpi_type_ਕੋਡ_2, signed_flag_, local_flag_);
      ਵਾਪਸ ਸੱਚ;
}

ਖਾਲੀ compile_net(ਅੱਖਰ*label, ਅੱਖਰ*name, ਪੂਰਨ msb, ਪੂਰਨ lsb,
		 ਪੂਰਨ vpi_type_ਕੋਡ, ਤਾਰਕਿਕ signed_flag, ਤਾਰਕਿਕ local_flag,
		 ਬਿਨਾਨਿਸ਼ਾਨੀ argc, ਬਣਤਰ symb_s*argv)
{
      __compile_net(label, name, 0, 0, msb, lsb,
		    vpi_type_ਕੋਡ, signed_flag, local_flag,
		    argc, argv);
}

ਖਾਲੀ compile_netw(ਅੱਖਰ*label, ਅੱਖਰ*array_label, ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ array_addr,
		  ਪੂਰਨ msb, ਪੂਰਨ lsb, ਪੂਰਨ vpi_type_ਕੋਡ, ਤਾਰਕਿਕ signed_flag,
		  ਬਿਨਾਨਿਸ਼ਾਨੀ argc, ਬਣਤਰ symb_s*argv)
{
      __compile_net(label, 0, array_label, array_addr,
		    msb, lsb, vpi_type_ਕੋਡ, signed_flag, ਝੂਠ,
		    argc, argv);
}

ਵਰਗ __compile_real_net_resolv : ਜਨਤਕ ਅਧਾਰ_net_resolv {

    ਜਨਤਕ:
      ਸਪਸ਼ਟ __compile_real_net_resolv(ਅੱਖਰ*ref_label, vvp_array_t array,
					 __vpiScope*scope,
					 ਅੱਖਰ*my_label, ਅੱਖਰ*name,
					 ਬਿਨਾਨਿਸ਼ਾਨੀ array_addr, ਤਾਰਕਿਕ local_flag)
      : ਅਧਾਰ_net_resolv(ref_label, array, scope, my_label, name, array_addr, local_flag)
      {
      }

      ~__compile_real_net_resolv() override { }

      ਤਾਰਕਿਕ resolve(ਤਾਰਕਿਕ ਸੁਨੇਹਾ_flag) override;

    ਨਿਜੀ:
};

ਸਥਿਰ ਖਾਲੀ __compile_real_net2(vvp_net_t*ਗੰਢ, vvp_array_t array,
				__vpiScope*scope,
				ਅੱਖਰ*my_label, ਅੱਖਰ*name,
				ਬਿਨਾਨਿਸ਼ਾਨੀ array_addr, ਤਾਰਕਿਕ local_flag)
{
      vvp_wire_ਅਧਾਰ*fil = dynamic_cast<vvp_wire_ਅਧਾਰ*> (ਗੰਢ->fil);

      ਜੇ (fil == 0) {
	    fil = ਨਵਾਂ vvp_wire_real;
	    ਗੰਢ->fil = fil;
      }

      vpiHandle obj = 0;
      ਜੇ (!local_flag) {
	    obj = vpip_make_real_net(scope, name, ਗੰਢ);
	    compile_vpi_ਚਿੰਨ੍ਹ(my_label, obj);
      }
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ CHECK_WITH_VALGRIND
      ਨਹੀਂਤਾਂ pool_local_net(ਗੰਢ);
#ਜੇ_ਅੰਤ

	// REMOVE ME! Giving the net a label is a legacy of the times
	// when the .net was a functor of its own. In the long run, we
	// must fix the code generator to not rely on the label of the
	// .net, then we will remove that label.
      define_functor_ਚਿੰਨ੍ਹ(my_label, ਗੰਢ);

     ਜੇ (array)
	    array->attach_ਸ਼ਬਦ(array_addr, obj);
      ਨਹੀਂਤਾਂ ਜੇ (obj)
	    vpip_attach_ਮੰਜ਼ਿਲ_scope(scope, obj);

      ਮੁਕਤ(my_label);
      ਮਿਟਾਓ[] name;
}

ਸਥਿਰ ਖਾਲੀ __compile_real(ਅੱਖਰ*label, ਅੱਖਰ*name,
                           ਅੱਖਰ*array_label, ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ array_addr,
                           ਪੂਰਨ msb, ਪੂਰਨ lsb, ਤਾਰਕਿਕ local_flag,
                           ਬਿਨਾਨਿਸ਼ਾਨੀ argc, ਬਣਤਰ symb_s*argv)
{
      ਪ੍ਰਤਿਜ੍ਞਾ(msb == 0 && lsb == 0);
      vvp_array_t array = array_label ? array_ਲੱਭੋ(array_label) : ਨਿਰਾ;
      ਪ੍ਰਤਿਜ੍ਞਾ(array_label ? array!=0 : ਸੱਚ);

      ਮੁਕਤ(array_label);

      ਪ੍ਰਤਿਜ੍ਞਾ(argc == 1);
      vvp_net_t*ਗੰਢ = vvp_net_lookup(argv[0].ਪਾਠ);
      ਜੇ (ਗੰਢ == 0) {
	      /* No existing net, but the string value may be a
		 constant. In that case, we will wind up generating a
		 bufz node that can carry the constant value. */
	    ਗੰਢ = create_constant_ਗੰਢ(argv[0].ਪਾਠ);
      }
      __vpiScope*scope = vpip_peek_current_scope();
      ਜੇ (ਗੰਢ == 0) {
	    __compile_real_net_resolv*res
		  = ਨਵਾਂ __compile_real_net_resolv(argv[0].ਪਾਠ, array,
						  scope, label, name,
						  array_addr, local_flag);
	    resolv_submit(res);
	    ਮੁਕਤ(argv);
	    ਵਾਪਸ;
      }

      ਪ੍ਰਤਿਜ੍ਞਾ(ਗੰਢ);
      __compile_real_net2(ਗੰਢ, array, scope, label, name, array_addr,
                          local_flag);
      ਮੁਕਤ(argv[0].ਪਾਠ);
      ਮੁਕਤ(argv);
}

ਤਾਰਕਿਕ __compile_real_net_resolv::resolve(ਤਾਰਕਿਕ ਸੁਨੇਹਾ_flag_2)
{
      vvp_net_t*ਗੰਢ = vvp_net_lookup(label());
      ਜੇ (ਗੰਢ == 0) {
	    ਜੇ (ਸੁਨੇਹਾ_flag_2)
		  cerr << "Unable to resolve label " << label() << ਸਤਰ_ਅੰਤ;
	    ਵਾਪਸ ਝੂਠ;
      }

      __compile_real_net2(ਗੰਢ, array_, scope_, my_label_, name_, array_addr_, local_flag_);
      ਵਾਪਸ ਸੱਚ;
}

ਖਾਲੀ compile_net_real(ਅੱਖਰ*label, ਅੱਖਰ*name, ਪੂਰਨ msb, ਪੂਰਨ lsb, ਤਾਰਕਿਕ local_flag,
		      ਬਿਨਾਨਿਸ਼ਾਨੀ argc, ਬਣਤਰ symb_s*argv)
{
      __compile_real(label, name, 0, 0,
                     msb, lsb, local_flag, argc, argv);
}

ਖਾਲੀ compile_netw_real(ਅੱਖਰ*label, ਅੱਖਰ*array_label, ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ array_addr,
                       ਪੂਰਨ msb, ਪੂਰਨ lsb,
                       ਬਿਨਾਨਿਸ਼ਾਨੀ argc, ਬਣਤਰ symb_s*argv)
{
      __compile_real(label, 0, array_label, array_addr,
                     msb, lsb, ਝੂਠ, argc, argv);
}
