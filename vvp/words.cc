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

using നാമാകാശ std;

സ്ഥിതം ശൂന്യം __compile_var_real(അക്ഷരം*label, അക്ഷരം*name,
			       vvp_array_t array, unsigned long array_addr,
			       തർക്കം local_flag)
{
      vvp_net_t*net = പുതിയ vvp_net_t;

      എങ്കിൽ (vpip_peek_current_scope()->is_automatic()) {
	    vvp_fun_signal_real_aa*tmp = പുതിയ vvp_fun_signal_real_aa;
	    net->fil = tmp;
	    net->fun = tmp;
      } അല്ലെങ്കിൽ {
	    net->fil = പുതിയ vvp_wire_real;
	    net->fun = പുതിയ vvp_fun_signal_real_sa;
      }

      define_functor_ചിഹ്നം(label, net);

      vpiHandle obj = vpip_make_real_var(name, net);
      compile_vpi_ചിഹ്നം(label, obj);

      എങ്കിൽ (name) {
	    പ്രതിജ്ഞാ(!array);
	    എങ്കിൽ (!local_flag)
		  vpip_attach_to_current_scope(obj);
            എങ്കിൽ (!vpip_peek_current_scope()->is_automatic())
                  schedule_init_vector(vvp_net_ptr_t(net,0), 0.0);
      }
      എങ്കിൽ (array) {
	    പ്രതിജ്ഞാ(!name);
	    array->attach_word(array_addr, obj);
      }
      സ്വതന്ത്രം(label);
      delete[] name;
}

ശൂന്യം compile_var_real(അക്ഷരം*label, അക്ഷരം*name, തർക്കം local_flag)
{
      __compile_var_real(label, name, 0, 0, local_flag);
}

ശൂന്യം compile_var_string(അക്ഷരം*label, അക്ഷരം*name)
{
      vvp_net_t*net = പുതിയ vvp_net_t;

      എങ്കിൽ (vpip_peek_current_scope()->is_automatic()) {
	    vvp_fun_signal_string_aa*tmp = പുതിയ vvp_fun_signal_string_aa;
	    net->fil = tmp;
	    net->fun = tmp;
      } അല്ലെങ്കിൽ {
	    net->fil = 0;
	    net->fun = പുതിയ vvp_fun_signal_string_sa;
      }

      define_functor_ചിഹ്നം(label, net);

      vpiHandle obj = vpip_make_string_var(name, net);
      compile_vpi_ചിഹ്നം(label, obj);

      vpip_attach_to_current_scope(obj);
      സ്വതന്ത്രം(label);
      delete[] name;
}

ശൂന്യം compile_var_darray(അക്ഷരം*label, അക്ഷരം*name, unsigned size)
{
      vvp_net_t*net = പുതിയ vvp_net_t;

      എങ്കിൽ (vpip_peek_current_scope()->is_automatic()) {
	    vvp_fun_signal_object_aa*tmp = പുതിയ vvp_fun_signal_object_aa(size);
	    net->fil = tmp;
	    net->fun = tmp;
      } അല്ലെങ്കിൽ {
	    net->fil = 0;
	    net->fun = പുതിയ vvp_fun_signal_object_sa(size);
      }

      define_functor_ചിഹ്നം(label, net);

      vpiHandle obj = vpip_make_darray_var(name, net);
      compile_vpi_ചിഹ്നം(label, obj);

      vpip_attach_to_current_scope(obj);
      സ്വതന്ത്രം(label);
      delete[] name;
}

ശൂന്യം compile_var_queue(അക്ഷരം*label, അക്ഷരം*name, unsigned size)
{
      vvp_net_t*net = പുതിയ vvp_net_t;

      എങ്കിൽ (vpip_peek_current_scope()->is_automatic()) {
	    vvp_fun_signal_object_aa*tmp = പുതിയ vvp_fun_signal_object_aa(size);
	    net->fil = tmp;
	    net->fun = tmp;
      } അല്ലെങ്കിൽ {
	    net->fil = 0;
	    net->fun = പുതിയ vvp_fun_signal_object_sa(size);
      }

      define_functor_ചിഹ്നം(label, net);

      vpiHandle obj = vpip_make_queue_var(name, net);
      compile_vpi_ചിഹ്നം(label, obj);

      vpip_attach_to_current_scope(obj);
      സ്വതന്ത്രം(label);
      delete[] name;
}

ശൂന്യം compile_var_cobject(അക്ഷരം*label, അക്ഷരം*name)
{
      vvp_net_t*net = പുതിയ vvp_net_t;

      എങ്കിൽ (vpip_peek_current_scope()->is_automatic()) {
	    vvp_fun_signal_object_aa*tmp = പുതിയ vvp_fun_signal_object_aa(1);
	    net->fil = tmp;
	    net->fun = tmp;
      } അല്ലെങ്കിൽ {
	    net->fil = 0;
	    net->fun = പുതിയ vvp_fun_signal_object_sa(1);
      }

      define_functor_ചിഹ്നം(label, net);

      vpiHandle obj = vpip_make_cobject_var(name, net);
      compile_vpi_ചിഹ്നം(label, obj);

      vpip_attach_to_current_scope(obj);
      സ്വതന്ത്രം(label);
      delete[] name;
}

/*
 * A variable is a special functor, so we allocate that functor and
 * write the label into the symbol table.
 */
ശൂന്യം compile_variable(അക്ഷരം*label, അക്ഷരം*name,
		      പൂർണ്ണസംഖ്യ msb, പൂർണ്ണസംഖ്യ lsb, പൂർണ്ണസംഖ്യ vpi_type_code,
		      തർക്കം signed_flag, തർക്കം local_flag)
{
      unsigned wid = ((msb > lsb)? msb-lsb : lsb-msb) + 1;

      vvp_net_t*net = പുതിയ vvp_net_t;

      vvp_bit4_t init;
      എങ്കിൽ (vpi_type_code == vpiIntVar)
	    init = BIT4_0;
      അല്ലെങ്കിൽ
	    init = BIT4_X;

      എങ്കിൽ (vpip_peek_current_scope()->is_automatic()) {
	    vvp_fun_signal4_aa*tmp = പുതിയ vvp_fun_signal4_aa(wid, init);
	    net->fil = tmp;
            net->fun = tmp;
      } അല്ലെങ്കിൽ {
	    net->fil = പുതിയ vvp_wire_vec4(wid, init);
            net->fun = പുതിയ vvp_fun_signal4_sa(wid);
      }
      സ്ഥിരം vvp_signal_മൂല്യം*vfil = dynamic_cast<vvp_signal_മൂല്യം*>(net->fil);

      define_functor_ചിഹ്നം(label, net);

      vpiHandle obj = 0;
      എങ്കിൽ (! local_flag) {
	      /* Make the vpiHandle for the reg. */
	    തിരഞ്ഞെടുപ്പ് (vpi_type_code) {
		അവസ്ഥ vpiLogicVar:
		  obj = vpip_make_var4(name, msb, lsb, signed_flag, net);
		  നിർത്തുക;
		അവസ്ഥ vpiIntegerVar:
		  obj = vpip_make_int4(name, msb, lsb, net);
		  നിർത്തുക;
		അവസ്ഥ vpiIntVar: // This handles all the atom2 int types
		  obj = vpip_make_int2(name, msb, lsb, signed_flag, net);
		  നിർത്തുക;
		സ്ഥിരസ്ഥിതി:
		  fprintf(stderr, "internal error: %s: vpi_type_code=%d\n", name, vpi_type_code);
		  നിർത്തുക;
	    }
	    പ്രതിജ്ഞാ(obj);
	    compile_vpi_ചിഹ്നം(label, obj);
      }
	// If the signal has a name, then it goes into the current
	// scope as a signal.
      എങ്കിൽ (name) {
	    എങ്കിൽ (obj) vpip_attach_to_current_scope(obj);
            എങ്കിൽ (!vpip_peek_current_scope()->is_automatic()) {
		  vvp_vector4_t tmp;
		  vfil->vec4_മൂല്യം(tmp);
	          schedule_init_vector(vvp_net_ptr_t(net,0), tmp);
	    }
      }

      സ്വതന്ത്രം(label);
      delete[] name;
}


vvp_net_t* create_constant_കണ്ണി(സ്ഥിരം അക്ഷരം*val_str)
{
      എങ്കിൽ (c4string_test(val_str)) {
	    vvp_net_t*net = പുതിയ vvp_net_t;
	    net->fun = പുതിയ vvp_fun_bufz;
	    schedule_init_vector(vvp_net_ptr_t(net,0), c4string_to_vector4(val_str));
	    തിരികെ net;
      }

      എങ്കിൽ (c8string_test(val_str)) {
	    vvp_net_t*net = പുതിയ vvp_net_t;
	    net->fun = പുതിയ vvp_fun_bufz;
	    schedule_init_vector(vvp_net_ptr_t(net,0), c8string_to_vector8(val_str));
	    തിരികെ net;
      }

      എങ്കിൽ (crstring_test(val_str)) {
	    vvp_net_t*net = പുതിയ vvp_net_t;
	    net->fun = പുതിയ vvp_fun_bufz;
	    schedule_init_vector(vvp_net_ptr_t(net,0), crstring_to_double(val_str));
	    തിരികെ net;
      }

      തിരികെ 0;
}

വർഗ്ഗം base_net_resolv : പൊതു resolv_list_s {
    പൊതു:
      explicit base_net_resolv(അക്ഷരം*ref_label, vvp_array_t array,
			       __vpiScope*scope,
			       അക്ഷരം*my_label, അക്ഷരം*name,
			       unsigned array_addr, തർക്കം local_flag)
      : resolv_list_s(ref_label)
      { my_label_ = my_label;
	array_ = array;
	name_ = name;
	scope_ = scope;
	array_addr_ = array_addr;
	local_flag_ = local_flag;
      }

    സംരക്ഷിത:
      അക്ഷരം*my_label_;
      vvp_array_t array_;
      അക്ഷരം*name_;
      __vpiScope*scope_;
      unsigned array_addr_;
      തർക്കം local_flag_;
};

വർഗ്ഗം __compile_net_resolv : പൊതു base_net_resolv {

    പൊതു:
      explicit __compile_net_resolv(അക്ഷരം*ref_label, vvp_array_t array,
				    __vpiScope*scope,
				    അക്ഷരം*my_label, അക്ഷരം*name,
				    പൂർണ്ണസംഖ്യ msb, പൂർണ്ണസംഖ്യ lsb, unsigned array_addr,
				    പൂർണ്ണസംഖ്യ vpi_type_code, തർക്കം signed_flag, തർക്കം local_flag)
      : base_net_resolv(ref_label, array, scope, my_label, name, array_addr, local_flag)
      { msb_ = msb;
	lsb_ = lsb;
	vpi_type_code_ = vpi_type_code;
	signed_flag_ = signed_flag;
      }

      ~__compile_net_resolv() override { }

      തർക്കം resolve(തർക്കം message_flag) override;

    സ്വകാര്യ:
      പൂർണ്ണസംഖ്യ msb_, lsb_;
      പൂർണ്ണസംഖ്യ vpi_type_code_;
      തർക്കം signed_flag_;
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

സ്ഥിതം ശൂന്യം do_compile_net(vvp_net_t*കണ്ണി, vvp_array_t array,
			   __vpiScope*scope,
			   അക്ഷരം*my_label, അക്ഷരം*name,
			   പൂർണ്ണസംഖ്യ msb, പൂർണ്ണസംഖ്യ lsb, unsigned array_addr,
			   പൂർണ്ണസംഖ്യ vpi_type_code, തർക്കം signed_flag, തർക്കം local_flag)
{
      unsigned wid = ((msb > lsb)? msb-lsb : lsb-msb) + 1;
      പ്രതിജ്ഞാ(കണ്ണി);

      vvp_wire_base*vsig = dynamic_cast<vvp_wire_base*>(കണ്ണി->fil);

      എങ്കിൽ (vsig == 0) {
	    തിരഞ്ഞെടുപ്പ് (vpi_type_code) {
		അവസ്ഥ vpiIntVar:
		  vsig = പുതിയ vvp_wire_vec4(wid,BIT4_0);
		  നിർത്തുക;
		അവസ്ഥ vpiLogicVar:
		  vsig = പുതിയ vvp_wire_vec4(wid,BIT4_Z);
		  നിർത്തുക;
		അവസ്ഥ -vpiLogicVar:
		  vsig = പുതിയ vvp_wire_vec8(wid);
		  നിർത്തുക;
	    }
	    പ്രതിജ്ഞാ(vsig);
	    കണ്ണി->fil = vsig;
      }

      vpiHandle obj = 0;
      എങ്കിൽ (! local_flag) {
	      /* Make the vpiHandle for the reg. */
	    obj = vpip_make_net4(scope, name, msb, lsb, signed_flag, കണ്ണി);
	      /* This attaches the label to the vpiHandle */
	    compile_vpi_ചിഹ്നം(my_label, obj);
      }
#ifdef CHECK_WITH_VALGRIND
      അല്ലെങ്കിൽ pool_local_net(കണ്ണി);
#endif

	// REMOVE ME! Giving the net a label is a legacy of the times
	// when the .net was a functor of its own. In the long run, we
	// must fix the code generator to not rely on the label of the
	// .net, then we will remove that label.
      define_functor_ചിഹ്നം(my_label, കണ്ണി);

      എങ്കിൽ (array)
	    array->attach_word(array_addr, obj);
      അല്ലെങ്കിൽ എങ്കിൽ (obj)
	    vpip_attach_to_scope(scope,obj);

      സ്വതന്ത്രം(my_label);
      delete[] name;
}

സ്ഥിതം ശൂന്യം __compile_net(അക്ഷരം*label,
			  അക്ഷരം*name, അക്ഷരം*array_label, unsigned long array_addr,
			  പൂർണ്ണസംഖ്യ msb, പൂർണ്ണസംഖ്യ lsb,
			  പൂർണ്ണസംഖ്യ vpi_type_code, തർക്കം signed_flag, തർക്കം local_flag,
			  unsigned argc, ഘടന symb_s*argv)
{
      vvp_array_t array = array_label? array_find(array_label) : ശൂന്യ;
      പ്രതിജ്ഞാ(array_label ? array!=0 : സത്യം);

      സ്വതന്ത്രം(array_label);

      പ്രതിജ്ഞാ(argc == 1);
      vvp_net_t*കണ്ണി = vvp_net_lookup(argv[0].text);
#എങ്കിൽ 1
      എങ്കിൽ (കണ്ണി == 0) {
	      /* No existing net, but the string value may be a
		 constant. In that case, we will wind up generating a
		 bufz node that can carry the constant value.

	         NOTE: This is a hack! The code generator should be
	         fixed so that this is no longer needed. */
	    കണ്ണി = create_constant_കണ്ണി(argv[0].text);
      }
#endif
      എങ്കിൽ (കണ്ണി == 0) {
	    __vpiScope*scope = vpip_peek_current_scope();
	    __compile_net_resolv*res
		  = പുതിയ __compile_net_resolv(argv[0].text,
					     array, scope, label, name,
					     msb, lsb, array_addr,
					     vpi_type_code, signed_flag, local_flag);
	    resolv_submit(res);
	    സ്വതന്ത്രം(argv);
	    തിരികെ;
      }
      പ്രതിജ്ഞാ(കണ്ണി);

      __vpiScope*scope = vpip_peek_current_scope();
      do_compile_net(കണ്ണി, array, scope, label, name, msb, lsb, array_addr,
		     vpi_type_code, signed_flag, local_flag);

      സ്വതന്ത്രം(argv[0].text);
      സ്വതന്ത്രം(argv);
}

തർക്കം __compile_net_resolv::resolve(തർക്കം സന്ദേശം_flag)
{
      vvp_net_t*കണ്ണി = vvp_net_lookup(label());
      എങ്കിൽ (കണ്ണി == 0) {
	    എങ്കിൽ (സന്ദേശം_flag)
		  cerr << "Unable to resolve label " << label() << endl;
	    തിരികെ അസത്യം;
      }

      do_compile_net(കണ്ണി, array_, scope_, my_label_, name_, msb_, lsb_, array_addr_, vpi_type_code_, signed_flag_, local_flag_);
      തിരികെ സത്യം;
}

ശൂന്യം compile_net(അക്ഷരം*label, അക്ഷരം*name, പൂർണ്ണസംഖ്യ msb, പൂർണ്ണസംഖ്യ lsb,
		 പൂർണ്ണസംഖ്യ vpi_type_code, തർക്കം signed_flag, തർക്കം local_flag,
		 unsigned argc, ഘടന symb_s*argv)
{
      __compile_net(label, name, 0, 0, msb, lsb,
		    vpi_type_code, signed_flag, local_flag,
		    argc, argv);
}

ശൂന്യം compile_netw(അക്ഷരം*label, അക്ഷരം*array_label, unsigned long array_addr,
		  പൂർണ്ണസംഖ്യ msb, പൂർണ്ണസംഖ്യ lsb, പൂർണ്ണസംഖ്യ vpi_type_code, തർക്കം signed_flag,
		  unsigned argc, ഘടന symb_s*argv)
{
      __compile_net(label, 0, array_label, array_addr,
		    msb, lsb, vpi_type_code, signed_flag, അസത്യം,
		    argc, argv);
}

വർഗ്ഗം __compile_real_net_resolv : പൊതു base_net_resolv {

    പൊതു:
      explicit __compile_real_net_resolv(അക്ഷരം*ref_label, vvp_array_t array,
					 __vpiScope*scope,
					 അക്ഷരം*my_label, അക്ഷരം*name,
					 unsigned array_addr, തർക്കം local_flag)
      : base_net_resolv(ref_label, array, scope, my_label, name, array_addr, local_flag)
      {
      }

      ~__compile_real_net_resolv() override { }

      തർക്കം resolve(തർക്കം message_flag) override;

    സ്വകാര്യ:
};

സ്ഥിതം ശൂന്യം __compile_real_net2(vvp_net_t*കണ്ണി, vvp_array_t array,
				__vpiScope*scope,
				അക്ഷരം*my_label, അക്ഷരം*name,
				unsigned array_addr, തർക്കം local_flag)
{
      vvp_wire_base*fil = dynamic_cast<vvp_wire_base*> (കണ്ണി->fil);

      എങ്കിൽ (fil == 0) {
	    fil = പുതിയ vvp_wire_real;
	    കണ്ണി->fil = fil;
      }

      vpiHandle obj = 0;
      എങ്കിൽ (!local_flag) {
	    obj = vpip_make_real_net(scope, name, കണ്ണി);
	    compile_vpi_ചിഹ്നം(my_label, obj);
      }
#ifdef CHECK_WITH_VALGRIND
      അല്ലെങ്കിൽ pool_local_net(കണ്ണി);
#endif

	// REMOVE ME! Giving the net a label is a legacy of the times
	// when the .net was a functor of its own. In the long run, we
	// must fix the code generator to not rely on the label of the
	// .net, then we will remove that label.
      define_functor_ചിഹ്നം(my_label, കണ്ണി);

     എങ്കിൽ (array)
	    array->attach_word(array_addr, obj);
      അല്ലെങ്കിൽ എങ്കിൽ (obj)
	    vpip_attach_to_scope(scope, obj);

      സ്വതന്ത്രം(my_label);
      delete[] name;
}

സ്ഥിതം ശൂന്യം __compile_real(അക്ഷരം*label, അക്ഷരം*name,
                           അക്ഷരം*array_label, unsigned long array_addr,
                           പൂർണ്ണസംഖ്യ msb, പൂർണ്ണസംഖ്യ lsb, തർക്കം local_flag,
                           unsigned argc, ഘടന symb_s*argv)
{
      പ്രതിജ്ഞാ(msb == 0 && lsb == 0);
      vvp_array_t array = array_label ? array_find(array_label) : ശൂന്യ;
      പ്രതിജ്ഞാ(array_label ? array!=0 : സത്യം);

      സ്വതന്ത്രം(array_label);

      പ്രതിജ്ഞാ(argc == 1);
      vvp_net_t*കണ്ണി = vvp_net_lookup(argv[0].text);
      എങ്കിൽ (കണ്ണി == 0) {
	      /* No existing net, but the string value may be a
		 constant. In that case, we will wind up generating a
		 bufz node that can carry the constant value. */
	    കണ്ണി = create_constant_കണ്ണി(argv[0].text);
      }
      __vpiScope*scope = vpip_peek_current_scope();
      എങ്കിൽ (കണ്ണി == 0) {
	    __compile_real_net_resolv*res
		  = പുതിയ __compile_real_net_resolv(argv[0].text, array,
						  scope, label, name,
						  array_addr, local_flag);
	    resolv_submit(res);
	    സ്വതന്ത്രം(argv);
	    തിരികെ;
      }

      പ്രതിജ്ഞാ(കണ്ണി);
      __compile_real_net2(കണ്ണി, array, scope, label, name, array_addr,
                          local_flag);
      സ്വതന്ത്രം(argv[0].text);
      സ്വതന്ത്രം(argv);
}

തർക്കം __compile_real_net_resolv::resolve(തർക്കം സന്ദേശം_flag)
{
      vvp_net_t*കണ്ണി = vvp_net_lookup(label());
      എങ്കിൽ (കണ്ണി == 0) {
	    എങ്കിൽ (സന്ദേശം_flag)
		  cerr << "Unable to resolve label " << label() << endl;
	    തിരികെ അസത്യം;
      }

      __compile_real_net2(കണ്ണി, array_, scope_, my_label_, name_, array_addr_, local_flag_);
      തിരികെ സത്യം;
}

ശൂന്യം compile_net_real(അക്ഷരം*label, അക്ഷരം*name, പൂർണ്ണസംഖ്യ msb, പൂർണ്ണസംഖ്യ lsb, തർക്കം local_flag,
		      unsigned argc, ഘടന symb_s*argv)
{
      __compile_real(label, name, 0, 0,
                     msb, lsb, local_flag, argc, argv);
}

ശൂന്യം compile_netw_real(അക്ഷരം*label, അക്ഷരം*array_label, unsigned long array_addr,
                       പൂർണ്ണസംഖ്യ msb, പൂർണ്ണസംഖ്യ lsb,
                       unsigned argc, ഘടന symb_s*argv)
{
      __compile_real(label, 0, array_label, array_addr,
                     msb, lsb, അസത്യം, argc, argv);
}
