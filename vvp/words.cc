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

using નામાકાશ std;

સ્થિર શૂન્ય __compile_var_real(અક્ષર*label, અક્ષર*name,
			       vvp_array_t array, unsigned long array_addr,
			       તાર્કિક local_flag)
{
      vvp_net_t*net = નવું vvp_net_t;

      જો (vpip_peek_current_scope()->is_automatic()) {
	    vvp_fun_signal_real_aa*tmp = નવું vvp_fun_signal_real_aa;
	    net->fil = tmp;
	    net->fun = tmp;
      } નહીંતર {
	    net->fil = નવું vvp_wire_real;
	    net->fun = નવું vvp_fun_signal_real_sa;
      }

      define_functor_પ્રતીક(label, net);

      vpiHandle obj = vpip_make_real_var(name, net);
      compile_vpi_પ્રતીક(label, obj);

      જો (name) {
	    પ્રતિજ્ઞા(!array);
	    જો (!local_flag)
		  vpip_attach_to_current_scope(obj);
            જો (!vpip_peek_current_scope()->is_automatic())
                  schedule_init_vector(vvp_net_ptr_t(net,0), 0.0);
      }
      જો (array) {
	    પ્રતિજ્ઞા(!name);
	    array->attach_word(array_addr, obj);
      }
      મુક્ત(label);
      delete[] name;
}

શૂન્ય compile_var_real(અક્ષર*label, અક્ષર*name, તાર્કિક local_flag)
{
      __compile_var_real(label, name, 0, 0, local_flag);
}

શૂન્ય compile_var_string(અક્ષર*label, અક્ષર*name)
{
      vvp_net_t*net = નવું vvp_net_t;

      જો (vpip_peek_current_scope()->is_automatic()) {
	    vvp_fun_signal_string_aa*tmp = નવું vvp_fun_signal_string_aa;
	    net->fil = tmp;
	    net->fun = tmp;
      } નહીંતર {
	    net->fil = 0;
	    net->fun = નવું vvp_fun_signal_string_sa;
      }

      define_functor_પ્રતીક(label, net);

      vpiHandle obj = vpip_make_string_var(name, net);
      compile_vpi_પ્રતીક(label, obj);

      vpip_attach_to_current_scope(obj);
      મુક્ત(label);
      delete[] name;
}

શૂન્ય compile_var_darray(અક્ષર*label, અક્ષર*name, unsigned size)
{
      vvp_net_t*net = નવું vvp_net_t;

      જો (vpip_peek_current_scope()->is_automatic()) {
	    vvp_fun_signal_object_aa*tmp = નવું vvp_fun_signal_object_aa(size);
	    net->fil = tmp;
	    net->fun = tmp;
      } નહીંતર {
	    net->fil = 0;
	    net->fun = નવું vvp_fun_signal_object_sa(size);
      }

      define_functor_પ્રતીક(label, net);

      vpiHandle obj = vpip_make_darray_var(name, net);
      compile_vpi_પ્રતીક(label, obj);

      vpip_attach_to_current_scope(obj);
      મુક્ત(label);
      delete[] name;
}

શૂન્ય compile_var_queue(અક્ષર*label, અક્ષર*name, unsigned size)
{
      vvp_net_t*net = નવું vvp_net_t;

      જો (vpip_peek_current_scope()->is_automatic()) {
	    vvp_fun_signal_object_aa*tmp = નવું vvp_fun_signal_object_aa(size);
	    net->fil = tmp;
	    net->fun = tmp;
      } નહીંતર {
	    net->fil = 0;
	    net->fun = નવું vvp_fun_signal_object_sa(size);
      }

      define_functor_પ્રતીક(label, net);

      vpiHandle obj = vpip_make_queue_var(name, net);
      compile_vpi_પ્રતીક(label, obj);

      vpip_attach_to_current_scope(obj);
      મુક્ત(label);
      delete[] name;
}

શૂન્ય compile_var_cobject(અક્ષર*label, અક્ષર*name)
{
      vvp_net_t*net = નવું vvp_net_t;

      જો (vpip_peek_current_scope()->is_automatic()) {
	    vvp_fun_signal_object_aa*tmp = નવું vvp_fun_signal_object_aa(1);
	    net->fil = tmp;
	    net->fun = tmp;
      } નહીંતર {
	    net->fil = 0;
	    net->fun = નવું vvp_fun_signal_object_sa(1);
      }

      define_functor_પ્રતીક(label, net);

      vpiHandle obj = vpip_make_cobject_var(name, net);
      compile_vpi_પ્રતીક(label, obj);

      vpip_attach_to_current_scope(obj);
      મુક્ત(label);
      delete[] name;
}

/*
 * A variable is a special functor, so we allocate that functor and
 * write the label into the symbol table.
 */
શૂન્ય compile_variable(અક્ષર*label, અક્ષર*name,
		      પૂર્ણાંક msb, પૂર્ણાંક lsb, પૂર્ણાંક vpi_type_code,
		      તાર્કિક signed_flag, તાર્કિક local_flag)
{
      unsigned wid = ((msb > lsb)? msb-lsb : lsb-msb) + 1;

      vvp_net_t*net = નવું vvp_net_t;

      vvp_bit4_t init;
      જો (vpi_type_code == vpiIntVar)
	    init = BIT4_0;
      નહીંતર
	    init = BIT4_X;

      જો (vpip_peek_current_scope()->is_automatic()) {
	    vvp_fun_signal4_aa*tmp = નવું vvp_fun_signal4_aa(wid, init);
	    net->fil = tmp;
            net->fun = tmp;
      } નહીંતર {
	    net->fil = નવું vvp_wire_vec4(wid, init);
            net->fun = નવું vvp_fun_signal4_sa(wid);
      }
      અચલ vvp_signal_કિંમત*vfil = dynamic_cast<vvp_signal_કિંમત*>(net->fil);

      define_functor_પ્રતીક(label, net);

      vpiHandle obj = 0;
      જો (! local_flag) {
	      /* Make the vpiHandle for the reg. */
	    પસંદગી (vpi_type_code) {
		સ્થિતિ vpiLogicVar:
		  obj = vpip_make_var4(name, msb, lsb, signed_flag, net);
		  તોડો;
		સ્થિતિ vpiIntegerVar:
		  obj = vpip_make_int4(name, msb, lsb, net);
		  તોડો;
		સ્થિતિ vpiIntVar: // This handles all the atom2 int types
		  obj = vpip_make_int2(name, msb, lsb, signed_flag, net);
		  તોડો;
		મૂળભૂત:
		  fprintf(stderr, "internal error: %s: vpi_type_code=%d\n", name, vpi_type_code);
		  તોડો;
	    }
	    પ્રતિજ્ઞા(obj);
	    compile_vpi_પ્રતીક(label, obj);
      }
	// If the signal has a name, then it goes into the current
	// scope as a signal.
      જો (name) {
	    જો (obj) vpip_attach_to_current_scope(obj);
            જો (!vpip_peek_current_scope()->is_automatic()) {
		  vvp_vector4_t tmp;
		  vfil->vec4_કિંમત(tmp);
	          schedule_init_vector(vvp_net_ptr_t(net,0), tmp);
	    }
      }

      મુક્ત(label);
      delete[] name;
}


vvp_net_t* create_constant_ગાંઠ(અચલ અક્ષર*val_str)
{
      જો (c4string_test(val_str)) {
	    vvp_net_t*net = નવું vvp_net_t;
	    net->fun = નવું vvp_fun_bufz;
	    schedule_init_vector(vvp_net_ptr_t(net,0), c4string_to_vector4(val_str));
	    પાછા net;
      }

      જો (c8string_test(val_str)) {
	    vvp_net_t*net = નવું vvp_net_t;
	    net->fun = નવું vvp_fun_bufz;
	    schedule_init_vector(vvp_net_ptr_t(net,0), c8string_to_vector8(val_str));
	    પાછા net;
      }

      જો (crstring_test(val_str)) {
	    vvp_net_t*net = નવું vvp_net_t;
	    net->fun = નવું vvp_fun_bufz;
	    schedule_init_vector(vvp_net_ptr_t(net,0), crstring_to_double(val_str));
	    પાછા net;
      }

      પાછા 0;
}

વર્ગ base_net_resolv : જાહેર resolv_list_s {
    જાહેર:
      explicit base_net_resolv(અક્ષર*ref_label, vvp_array_t array,
			       __vpiScope*scope,
			       અક્ષર*my_label, અક્ષર*name,
			       unsigned array_addr, તાર્કિક local_flag)
      : resolv_list_s(ref_label)
      { my_label_ = my_label;
	array_ = array;
	name_ = name;
	scope_ = scope;
	array_addr_ = array_addr;
	local_flag_ = local_flag;
      }

    સુરક્ષિત:
      અક્ષર*my_label_;
      vvp_array_t array_;
      અક્ષર*name_;
      __vpiScope*scope_;
      unsigned array_addr_;
      તાર્કિક local_flag_;
};

વર્ગ __compile_net_resolv : જાહેર base_net_resolv {

    જાહેર:
      explicit __compile_net_resolv(અક્ષર*ref_label, vvp_array_t array,
				    __vpiScope*scope,
				    અક્ષર*my_label, અક્ષર*name,
				    પૂર્ણાંક msb, પૂર્ણાંક lsb, unsigned array_addr,
				    પૂર્ણાંક vpi_type_code, તાર્કિક signed_flag, તાર્કિક local_flag)
      : base_net_resolv(ref_label, array, scope, my_label, name, array_addr, local_flag)
      { msb_ = msb;
	lsb_ = lsb;
	vpi_type_code_ = vpi_type_code;
	signed_flag_ = signed_flag;
      }

      ~__compile_net_resolv() override { }

      તાર્કિક resolve(તાર્કિક message_flag) override;

    ખાનગી:
      પૂર્ણાંક msb_, lsb_;
      પૂર્ણાંક vpi_type_code_;
      તાર્કિક signed_flag_;
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

સ્થિર શૂન્ય do_compile_net(vvp_net_t*ગાંઠ, vvp_array_t array,
			   __vpiScope*scope,
			   અક્ષર*my_label, અક્ષર*name,
			   પૂર્ણાંક msb, પૂર્ણાંક lsb, unsigned array_addr,
			   પૂર્ણાંક vpi_type_code, તાર્કિક signed_flag, તાર્કિક local_flag)
{
      unsigned wid = ((msb > lsb)? msb-lsb : lsb-msb) + 1;
      પ્રતિજ્ઞા(ગાંઠ);

      vvp_wire_base*vsig = dynamic_cast<vvp_wire_base*>(ગાંઠ->fil);

      જો (vsig == 0) {
	    પસંદગી (vpi_type_code) {
		સ્થિતિ vpiIntVar:
		  vsig = નવું vvp_wire_vec4(wid,BIT4_0);
		  તોડો;
		સ્થિતિ vpiLogicVar:
		  vsig = નવું vvp_wire_vec4(wid,BIT4_Z);
		  તોડો;
		સ્થિતિ -vpiLogicVar:
		  vsig = નવું vvp_wire_vec8(wid);
		  તોડો;
	    }
	    પ્રતિજ્ઞા(vsig);
	    ગાંઠ->fil = vsig;
      }

      vpiHandle obj = 0;
      જો (! local_flag) {
	      /* Make the vpiHandle for the reg. */
	    obj = vpip_make_net4(scope, name, msb, lsb, signed_flag, ગાંઠ);
	      /* This attaches the label to the vpiHandle */
	    compile_vpi_પ્રતીક(my_label, obj);
      }
#ifdef CHECK_WITH_VALGRIND
      નહીંતર pool_local_net(ગાંઠ);
#endif

	// REMOVE ME! Giving the net a label is a legacy of the times
	// when the .net was a functor of its own. In the long run, we
	// must fix the code generator to not rely on the label of the
	// .net, then we will remove that label.
      define_functor_પ્રતીક(my_label, ગાંઠ);

      જો (array)
	    array->attach_word(array_addr, obj);
      નહીંતર જો (obj)
	    vpip_attach_to_scope(scope,obj);

      મુક્ત(my_label);
      delete[] name;
}

સ્થિર શૂન્ય __compile_net(અક્ષર*label,
			  અક્ષર*name, અક્ષર*array_label, unsigned long array_addr,
			  પૂર્ણાંક msb, પૂર્ણાંક lsb,
			  પૂર્ણાંક vpi_type_code, તાર્કિક signed_flag, તાર્કિક local_flag,
			  unsigned argc, સંરચના symb_s*argv)
{
      vvp_array_t array = array_label? array_find(array_label) : ખાલી;
      પ્રતિજ્ઞા(array_label ? array!=0 : સાચું);

      મુક્ત(array_label);

      પ્રતિજ્ઞા(argc == 1);
      vvp_net_t*ગાંઠ = vvp_net_lookup(argv[0].text);
#જો 1
      જો (ગાંઠ == 0) {
	      /* No existing net, but the string value may be a
		 constant. In that case, we will wind up generating a
		 bufz node that can carry the constant value.

	         NOTE: This is a hack! The code generator should be
	         fixed so that this is no longer needed. */
	    ગાંઠ = create_constant_ગાંઠ(argv[0].text);
      }
#endif
      જો (ગાંઠ == 0) {
	    __vpiScope*scope = vpip_peek_current_scope();
	    __compile_net_resolv*res
		  = નવું __compile_net_resolv(argv[0].text,
					     array, scope, label, name,
					     msb, lsb, array_addr,
					     vpi_type_code, signed_flag, local_flag);
	    resolv_submit(res);
	    મુક્ત(argv);
	    પાછા;
      }
      પ્રતિજ્ઞા(ગાંઠ);

      __vpiScope*scope = vpip_peek_current_scope();
      do_compile_net(ગાંઠ, array, scope, label, name, msb, lsb, array_addr,
		     vpi_type_code, signed_flag, local_flag);

      મુક્ત(argv[0].text);
      મુક્ત(argv);
}

તાર્કિક __compile_net_resolv::resolve(તાર્કિક સંદેશ_flag)
{
      vvp_net_t*ગાંઠ = vvp_net_lookup(label());
      જો (ગાંઠ == 0) {
	    જો (સંદેશ_flag)
		  cerr << "Unable to resolve label " << label() << endl;
	    પાછા ખોટું;
      }

      do_compile_net(ગાંઠ, array_, scope_, my_label_, name_, msb_, lsb_, array_addr_, vpi_type_code_, signed_flag_, local_flag_);
      પાછા સાચું;
}

શૂન્ય compile_net(અક્ષર*label, અક્ષર*name, પૂર્ણાંક msb, પૂર્ણાંક lsb,
		 પૂર્ણાંક vpi_type_code, તાર્કિક signed_flag, તાર્કિક local_flag,
		 unsigned argc, સંરચના symb_s*argv)
{
      __compile_net(label, name, 0, 0, msb, lsb,
		    vpi_type_code, signed_flag, local_flag,
		    argc, argv);
}

શૂન્ય compile_netw(અક્ષર*label, અક્ષર*array_label, unsigned long array_addr,
		  પૂર્ણાંક msb, પૂર્ણાંક lsb, પૂર્ણાંક vpi_type_code, તાર્કિક signed_flag,
		  unsigned argc, સંરચના symb_s*argv)
{
      __compile_net(label, 0, array_label, array_addr,
		    msb, lsb, vpi_type_code, signed_flag, ખોટું,
		    argc, argv);
}

વર્ગ __compile_real_net_resolv : જાહેર base_net_resolv {

    જાહેર:
      explicit __compile_real_net_resolv(અક્ષર*ref_label, vvp_array_t array,
					 __vpiScope*scope,
					 અક્ષર*my_label, અક્ષર*name,
					 unsigned array_addr, તાર્કિક local_flag)
      : base_net_resolv(ref_label, array, scope, my_label, name, array_addr, local_flag)
      {
      }

      ~__compile_real_net_resolv() override { }

      તાર્કિક resolve(તાર્કિક message_flag) override;

    ખાનગી:
};

સ્થિર શૂન્ય __compile_real_net2(vvp_net_t*ગાંઠ, vvp_array_t array,
				__vpiScope*scope,
				અક્ષર*my_label, અક્ષર*name,
				unsigned array_addr, તાર્કિક local_flag)
{
      vvp_wire_base*fil = dynamic_cast<vvp_wire_base*> (ગાંઠ->fil);

      જો (fil == 0) {
	    fil = નવું vvp_wire_real;
	    ગાંઠ->fil = fil;
      }

      vpiHandle obj = 0;
      જો (!local_flag) {
	    obj = vpip_make_real_net(scope, name, ગાંઠ);
	    compile_vpi_પ્રતીક(my_label, obj);
      }
#ifdef CHECK_WITH_VALGRIND
      નહીંતર pool_local_net(ગાંઠ);
#endif

	// REMOVE ME! Giving the net a label is a legacy of the times
	// when the .net was a functor of its own. In the long run, we
	// must fix the code generator to not rely on the label of the
	// .net, then we will remove that label.
      define_functor_પ્રતીક(my_label, ગાંઠ);

     જો (array)
	    array->attach_word(array_addr, obj);
      નહીંતર જો (obj)
	    vpip_attach_to_scope(scope, obj);

      મુક્ત(my_label);
      delete[] name;
}

સ્થિર શૂન્ય __compile_real(અક્ષર*label, અક્ષર*name,
                           અક્ષર*array_label, unsigned long array_addr,
                           પૂર્ણાંક msb, પૂર્ણાંક lsb, તાર્કિક local_flag,
                           unsigned argc, સંરચના symb_s*argv)
{
      પ્રતિજ્ઞા(msb == 0 && lsb == 0);
      vvp_array_t array = array_label ? array_find(array_label) : ખાલી;
      પ્રતિજ્ઞા(array_label ? array!=0 : સાચું);

      મુક્ત(array_label);

      પ્રતિજ્ઞા(argc == 1);
      vvp_net_t*ગાંઠ = vvp_net_lookup(argv[0].text);
      જો (ગાંઠ == 0) {
	      /* No existing net, but the string value may be a
		 constant. In that case, we will wind up generating a
		 bufz node that can carry the constant value. */
	    ગાંઠ = create_constant_ગાંઠ(argv[0].text);
      }
      __vpiScope*scope = vpip_peek_current_scope();
      જો (ગાંઠ == 0) {
	    __compile_real_net_resolv*res
		  = નવું __compile_real_net_resolv(argv[0].text, array,
						  scope, label, name,
						  array_addr, local_flag);
	    resolv_submit(res);
	    મુક્ત(argv);
	    પાછા;
      }

      પ્રતિજ્ઞા(ગાંઠ);
      __compile_real_net2(ગાંઠ, array, scope, label, name, array_addr,
                          local_flag);
      મુક્ત(argv[0].text);
      મુક્ત(argv);
}

તાર્કિક __compile_real_net_resolv::resolve(તાર્કિક સંદેશ_flag)
{
      vvp_net_t*ગાંઠ = vvp_net_lookup(label());
      જો (ગાંઠ == 0) {
	    જો (સંદેશ_flag)
		  cerr << "Unable to resolve label " << label() << endl;
	    પાછા ખોટું;
      }

      __compile_real_net2(ગાંઠ, array_, scope_, my_label_, name_, array_addr_, local_flag_);
      પાછા સાચું;
}

શૂન્ય compile_net_real(અક્ષર*label, અક્ષર*name, પૂર્ણાંક msb, પૂર્ણાંક lsb, તાર્કિક local_flag,
		      unsigned argc, સંરચના symb_s*argv)
{
      __compile_real(label, name, 0, 0,
                     msb, lsb, local_flag, argc, argv);
}

શૂન્ય compile_netw_real(અક્ષર*label, અક્ષર*array_label, unsigned long array_addr,
                       પૂર્ણાંક msb, પૂર્ણાંક lsb,
                       unsigned argc, સંરચના symb_s*argv)
{
      __compile_real(label, 0, array_label, array_addr,
                     msb, lsb, ખોટું, argc, argv);
}
