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

using நாமாகாஶ std;

நிலையான வெறுமை __compile_var_real(எழுத்து*label, எழுத்து*name,
			       vvp_array_t array, unsigned long array_addr,
			       தருக்க local_flag)
{
      vvp_net_t*net = புதிய vvp_net_t;

      எனில் (vpip_peek_current_scope()->is_automatic()) {
	    vvp_fun_signal_real_aa*tmp = புதிய vvp_fun_signal_real_aa;
	    net->fil = tmp;
	    net->fun = tmp;
      } இல்லையெனில் {
	    net->fil = புதிய vvp_wire_real;
	    net->fun = புதிய vvp_fun_signal_real_sa;
      }

      define_functor_குறியீடு(label, net);

      vpiHandle obj = vpip_make_real_var(name, net);
      compile_vpi_குறியீடு(label, obj);

      எனில் (name) {
	    ப்ரதிஜ்ஞா(!array);
	    எனில் (!local_flag)
		  vpip_attach_to_current_scope(obj);
            எனில் (!vpip_peek_current_scope()->is_automatic())
                  schedule_init_vector(vvp_net_ptr_t(net,0), 0.0);
      }
      எனில் (array) {
	    ப்ரதிஜ்ஞா(!name);
	    array->attach_word(array_addr, obj);
      }
      விடுவி(label);
      delete[] name;
}

வெறுமை compile_var_real(எழுத்து*label, எழுத்து*name, தருக்க local_flag)
{
      __compile_var_real(label, name, 0, 0, local_flag);
}

வெறுமை compile_var_string(எழுத்து*label, எழுத்து*name)
{
      vvp_net_t*net = புதிய vvp_net_t;

      எனில் (vpip_peek_current_scope()->is_automatic()) {
	    vvp_fun_signal_string_aa*tmp = புதிய vvp_fun_signal_string_aa;
	    net->fil = tmp;
	    net->fun = tmp;
      } இல்லையெனில் {
	    net->fil = 0;
	    net->fun = புதிய vvp_fun_signal_string_sa;
      }

      define_functor_குறியீடு(label, net);

      vpiHandle obj = vpip_make_string_var(name, net);
      compile_vpi_குறியீடு(label, obj);

      vpip_attach_to_current_scope(obj);
      விடுவி(label);
      delete[] name;
}

வெறுமை compile_var_darray(எழுத்து*label, எழுத்து*name, unsigned size)
{
      vvp_net_t*net = புதிய vvp_net_t;

      எனில் (vpip_peek_current_scope()->is_automatic()) {
	    vvp_fun_signal_object_aa*tmp = புதிய vvp_fun_signal_object_aa(size);
	    net->fil = tmp;
	    net->fun = tmp;
      } இல்லையெனில் {
	    net->fil = 0;
	    net->fun = புதிய vvp_fun_signal_object_sa(size);
      }

      define_functor_குறியீடு(label, net);

      vpiHandle obj = vpip_make_darray_var(name, net);
      compile_vpi_குறியீடு(label, obj);

      vpip_attach_to_current_scope(obj);
      விடுவி(label);
      delete[] name;
}

வெறுமை compile_var_queue(எழுத்து*label, எழுத்து*name, unsigned size)
{
      vvp_net_t*net = புதிய vvp_net_t;

      எனில் (vpip_peek_current_scope()->is_automatic()) {
	    vvp_fun_signal_object_aa*tmp = புதிய vvp_fun_signal_object_aa(size);
	    net->fil = tmp;
	    net->fun = tmp;
      } இல்லையெனில் {
	    net->fil = 0;
	    net->fun = புதிய vvp_fun_signal_object_sa(size);
      }

      define_functor_குறியீடு(label, net);

      vpiHandle obj = vpip_make_queue_var(name, net);
      compile_vpi_குறியீடு(label, obj);

      vpip_attach_to_current_scope(obj);
      விடுவி(label);
      delete[] name;
}

வெறுமை compile_var_cobject(எழுத்து*label, எழுத்து*name)
{
      vvp_net_t*net = புதிய vvp_net_t;

      எனில் (vpip_peek_current_scope()->is_automatic()) {
	    vvp_fun_signal_object_aa*tmp = புதிய vvp_fun_signal_object_aa(1);
	    net->fil = tmp;
	    net->fun = tmp;
      } இல்லையெனில் {
	    net->fil = 0;
	    net->fun = புதிய vvp_fun_signal_object_sa(1);
      }

      define_functor_குறியீடு(label, net);

      vpiHandle obj = vpip_make_cobject_var(name, net);
      compile_vpi_குறியீடு(label, obj);

      vpip_attach_to_current_scope(obj);
      விடுவி(label);
      delete[] name;
}

/*
 * A variable is a special functor, so we allocate that functor and
 * write the label into the symbol table.
 */
வெறுமை compile_variable(எழுத்து*label, எழுத்து*name,
		      முழு msb, முழு lsb, முழு vpi_type_code,
		      தருக்க signed_flag, தருக்க local_flag)
{
      unsigned wid = ((msb > lsb)? msb-lsb : lsb-msb) + 1;

      vvp_net_t*net = புதிய vvp_net_t;

      vvp_bit4_t init;
      எனில் (vpi_type_code == vpiIntVar)
	    init = BIT4_0;
      இல்லையெனில்
	    init = BIT4_X;

      எனில் (vpip_peek_current_scope()->is_automatic()) {
	    vvp_fun_signal4_aa*tmp = புதிய vvp_fun_signal4_aa(wid, init);
	    net->fil = tmp;
            net->fun = tmp;
      } இல்லையெனில் {
	    net->fil = புதிய vvp_wire_vec4(wid, init);
            net->fun = புதிய vvp_fun_signal4_sa(wid);
      }
      மாறிலி vvp_signal_மதிப்பு*vfil = dynamic_cast<vvp_signal_மதிப்பு*>(net->fil);

      define_functor_குறியீடு(label, net);

      vpiHandle obj = 0;
      எனில் (! local_flag) {
	      /* Make the vpiHandle for the reg. */
	    தேர்வு (vpi_type_code) {
		நிலை vpiLogicVar:
		  obj = vpip_make_var4(name, msb, lsb, signed_flag, net);
		  நிறுத்து;
		நிலை vpiIntegerVar:
		  obj = vpip_make_int4(name, msb, lsb, net);
		  நிறுத்து;
		நிலை vpiIntVar: // This handles all the atom2 int types
		  obj = vpip_make_int2(name, msb, lsb, signed_flag, net);
		  நிறுத்து;
		இயல்பு:
		  fprintf(stderr, "internal error: %s: vpi_type_code=%d\n", name, vpi_type_code);
		  நிறுத்து;
	    }
	    ப்ரதிஜ்ஞா(obj);
	    compile_vpi_குறியீடு(label, obj);
      }
	// If the signal has a name, then it goes into the current
	// scope as a signal.
      எனில் (name) {
	    எனில் (obj) vpip_attach_to_current_scope(obj);
            எனில் (!vpip_peek_current_scope()->is_automatic()) {
		  vvp_vector4_t tmp;
		  vfil->vec4_மதிப்பு(tmp);
	          schedule_init_vector(vvp_net_ptr_t(net,0), tmp);
	    }
      }

      விடுவி(label);
      delete[] name;
}


vvp_net_t* create_constant_முனை(மாறிலி எழுத்து*val_str)
{
      எனில் (c4string_test(val_str)) {
	    vvp_net_t*net = புதிய vvp_net_t;
	    net->fun = புதிய vvp_fun_bufz;
	    schedule_init_vector(vvp_net_ptr_t(net,0), c4string_to_vector4(val_str));
	    திரும்பு net;
      }

      எனில் (c8string_test(val_str)) {
	    vvp_net_t*net = புதிய vvp_net_t;
	    net->fun = புதிய vvp_fun_bufz;
	    schedule_init_vector(vvp_net_ptr_t(net,0), c8string_to_vector8(val_str));
	    திரும்பு net;
      }

      எனில் (crstring_test(val_str)) {
	    vvp_net_t*net = புதிய vvp_net_t;
	    net->fun = புதிய vvp_fun_bufz;
	    schedule_init_vector(vvp_net_ptr_t(net,0), crstring_to_double(val_str));
	    திரும்பு net;
      }

      திரும்பு 0;
}

வகுப்பு base_net_resolv : பொது resolv_list_s {
    பொது:
      explicit base_net_resolv(எழுத்து*ref_label, vvp_array_t array,
			       __vpiScope*scope,
			       எழுத்து*my_label, எழுத்து*name,
			       unsigned array_addr, தருக்க local_flag)
      : resolv_list_s(ref_label)
      { my_label_ = my_label;
	array_ = array;
	name_ = name;
	scope_ = scope;
	array_addr_ = array_addr;
	local_flag_ = local_flag;
      }

    பாதுகாக்கப்பட்ட:
      எழுத்து*my_label_;
      vvp_array_t array_;
      எழுத்து*name_;
      __vpiScope*scope_;
      unsigned array_addr_;
      தருக்க local_flag_;
};

வகுப்பு __compile_net_resolv : பொது base_net_resolv {

    பொது:
      explicit __compile_net_resolv(எழுத்து*ref_label, vvp_array_t array,
				    __vpiScope*scope,
				    எழுத்து*my_label, எழுத்து*name,
				    முழு msb, முழு lsb, unsigned array_addr,
				    முழு vpi_type_code, தருக்க signed_flag, தருக்க local_flag)
      : base_net_resolv(ref_label, array, scope, my_label, name, array_addr, local_flag)
      { msb_ = msb;
	lsb_ = lsb;
	vpi_type_code_ = vpi_type_code;
	signed_flag_ = signed_flag;
      }

      ~__compile_net_resolv() override { }

      தருக்க resolve(தருக்க message_flag) override;

    தனிப்பட்ட:
      முழு msb_, lsb_;
      முழு vpi_type_code_;
      தருக்க signed_flag_;
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

நிலையான வெறுமை do_compile_net(vvp_net_t*முனை, vvp_array_t array,
			   __vpiScope*scope,
			   எழுத்து*my_label, எழுத்து*name,
			   முழு msb, முழு lsb, unsigned array_addr,
			   முழு vpi_type_code, தருக்க signed_flag, தருக்க local_flag)
{
      unsigned wid = ((msb > lsb)? msb-lsb : lsb-msb) + 1;
      ப்ரதிஜ்ஞா(முனை);

      vvp_wire_base*vsig = dynamic_cast<vvp_wire_base*>(முனை->fil);

      எனில் (vsig == 0) {
	    தேர்வு (vpi_type_code) {
		நிலை vpiIntVar:
		  vsig = புதிய vvp_wire_vec4(wid,BIT4_0);
		  நிறுத்து;
		நிலை vpiLogicVar:
		  vsig = புதிய vvp_wire_vec4(wid,BIT4_Z);
		  நிறுத்து;
		நிலை -vpiLogicVar:
		  vsig = புதிய vvp_wire_vec8(wid);
		  நிறுத்து;
	    }
	    ப்ரதிஜ்ஞா(vsig);
	    முனை->fil = vsig;
      }

      vpiHandle obj = 0;
      எனில் (! local_flag) {
	      /* Make the vpiHandle for the reg. */
	    obj = vpip_make_net4(scope, name, msb, lsb, signed_flag, முனை);
	      /* This attaches the label to the vpiHandle */
	    compile_vpi_குறியீடு(my_label, obj);
      }
#ifdef CHECK_WITH_VALGRIND
      இல்லையெனில் pool_local_net(முனை);
#endif

	// REMOVE ME! Giving the net a label is a legacy of the times
	// when the .net was a functor of its own. In the long run, we
	// must fix the code generator to not rely on the label of the
	// .net, then we will remove that label.
      define_functor_குறியீடு(my_label, முனை);

      எனில் (array)
	    array->attach_word(array_addr, obj);
      இல்லையெனில் எனில் (obj)
	    vpip_attach_to_scope(scope,obj);

      விடுவி(my_label);
      delete[] name;
}

நிலையான வெறுமை __compile_net(எழுத்து*label,
			  எழுத்து*name, எழுத்து*array_label, unsigned long array_addr,
			  முழு msb, முழு lsb,
			  முழு vpi_type_code, தருக்க signed_flag, தருக்க local_flag,
			  unsigned argc, கட்டமைப்பு symb_s*argv)
{
      vvp_array_t array = array_label? array_find(array_label) : வெற்று;
      ப்ரதிஜ்ஞா(array_label ? array!=0 : உண்மை);

      விடுவி(array_label);

      ப்ரதிஜ்ஞா(argc == 1);
      vvp_net_t*முனை = vvp_net_lookup(argv[0].text);
#எனில் 1
      எனில் (முனை == 0) {
	      /* No existing net, but the string value may be a
		 constant. In that case, we will wind up generating a
		 bufz node that can carry the constant value.

	         NOTE: This is a hack! The code generator should be
	         fixed so that this is no longer needed. */
	    முனை = create_constant_முனை(argv[0].text);
      }
#endif
      எனில் (முனை == 0) {
	    __vpiScope*scope = vpip_peek_current_scope();
	    __compile_net_resolv*res
		  = புதிய __compile_net_resolv(argv[0].text,
					     array, scope, label, name,
					     msb, lsb, array_addr,
					     vpi_type_code, signed_flag, local_flag);
	    resolv_submit(res);
	    விடுவி(argv);
	    திரும்பு;
      }
      ப்ரதிஜ்ஞா(முனை);

      __vpiScope*scope = vpip_peek_current_scope();
      do_compile_net(முனை, array, scope, label, name, msb, lsb, array_addr,
		     vpi_type_code, signed_flag, local_flag);

      விடுவி(argv[0].text);
      விடுவி(argv);
}

தருக்க __compile_net_resolv::resolve(தருக்க செய்தி_flag)
{
      vvp_net_t*முனை = vvp_net_lookup(label());
      எனில் (முனை == 0) {
	    எனில் (செய்தி_flag)
		  cerr << "Unable to resolve label " << label() << endl;
	    திரும்பு பொய்;
      }

      do_compile_net(முனை, array_, scope_, my_label_, name_, msb_, lsb_, array_addr_, vpi_type_code_, signed_flag_, local_flag_);
      திரும்பு உண்மை;
}

வெறுமை compile_net(எழுத்து*label, எழுத்து*name, முழு msb, முழு lsb,
		 முழு vpi_type_code, தருக்க signed_flag, தருக்க local_flag,
		 unsigned argc, கட்டமைப்பு symb_s*argv)
{
      __compile_net(label, name, 0, 0, msb, lsb,
		    vpi_type_code, signed_flag, local_flag,
		    argc, argv);
}

வெறுமை compile_netw(எழுத்து*label, எழுத்து*array_label, unsigned long array_addr,
		  முழு msb, முழு lsb, முழு vpi_type_code, தருக்க signed_flag,
		  unsigned argc, கட்டமைப்பு symb_s*argv)
{
      __compile_net(label, 0, array_label, array_addr,
		    msb, lsb, vpi_type_code, signed_flag, பொய்,
		    argc, argv);
}

வகுப்பு __compile_real_net_resolv : பொது base_net_resolv {

    பொது:
      explicit __compile_real_net_resolv(எழுத்து*ref_label, vvp_array_t array,
					 __vpiScope*scope,
					 எழுத்து*my_label, எழுத்து*name,
					 unsigned array_addr, தருக்க local_flag)
      : base_net_resolv(ref_label, array, scope, my_label, name, array_addr, local_flag)
      {
      }

      ~__compile_real_net_resolv() override { }

      தருக்க resolve(தருக்க message_flag) override;

    தனிப்பட்ட:
};

நிலையான வெறுமை __compile_real_net2(vvp_net_t*முனை, vvp_array_t array,
				__vpiScope*scope,
				எழுத்து*my_label, எழுத்து*name,
				unsigned array_addr, தருக்க local_flag)
{
      vvp_wire_base*fil = dynamic_cast<vvp_wire_base*> (முனை->fil);

      எனில் (fil == 0) {
	    fil = புதிய vvp_wire_real;
	    முனை->fil = fil;
      }

      vpiHandle obj = 0;
      எனில் (!local_flag) {
	    obj = vpip_make_real_net(scope, name, முனை);
	    compile_vpi_குறியீடு(my_label, obj);
      }
#ifdef CHECK_WITH_VALGRIND
      இல்லையெனில் pool_local_net(முனை);
#endif

	// REMOVE ME! Giving the net a label is a legacy of the times
	// when the .net was a functor of its own. In the long run, we
	// must fix the code generator to not rely on the label of the
	// .net, then we will remove that label.
      define_functor_குறியீடு(my_label, முனை);

     எனில் (array)
	    array->attach_word(array_addr, obj);
      இல்லையெனில் எனில் (obj)
	    vpip_attach_to_scope(scope, obj);

      விடுவி(my_label);
      delete[] name;
}

நிலையான வெறுமை __compile_real(எழுத்து*label, எழுத்து*name,
                           எழுத்து*array_label, unsigned long array_addr,
                           முழு msb, முழு lsb, தருக்க local_flag,
                           unsigned argc, கட்டமைப்பு symb_s*argv)
{
      ப்ரதிஜ்ஞா(msb == 0 && lsb == 0);
      vvp_array_t array = array_label ? array_find(array_label) : வெற்று;
      ப்ரதிஜ்ஞா(array_label ? array!=0 : உண்மை);

      விடுவி(array_label);

      ப்ரதிஜ்ஞா(argc == 1);
      vvp_net_t*முனை = vvp_net_lookup(argv[0].text);
      எனில் (முனை == 0) {
	      /* No existing net, but the string value may be a
		 constant. In that case, we will wind up generating a
		 bufz node that can carry the constant value. */
	    முனை = create_constant_முனை(argv[0].text);
      }
      __vpiScope*scope = vpip_peek_current_scope();
      எனில் (முனை == 0) {
	    __compile_real_net_resolv*res
		  = புதிய __compile_real_net_resolv(argv[0].text, array,
						  scope, label, name,
						  array_addr, local_flag);
	    resolv_submit(res);
	    விடுவி(argv);
	    திரும்பு;
      }

      ப்ரதிஜ்ஞா(முனை);
      __compile_real_net2(முனை, array, scope, label, name, array_addr,
                          local_flag);
      விடுவி(argv[0].text);
      விடுவி(argv);
}

தருக்க __compile_real_net_resolv::resolve(தருக்க செய்தி_flag)
{
      vvp_net_t*முனை = vvp_net_lookup(label());
      எனில் (முனை == 0) {
	    எனில் (செய்தி_flag)
		  cerr << "Unable to resolve label " << label() << endl;
	    திரும்பு பொய்;
      }

      __compile_real_net2(முனை, array_, scope_, my_label_, name_, array_addr_, local_flag_);
      திரும்பு உண்மை;
}

வெறுமை compile_net_real(எழுத்து*label, எழுத்து*name, முழு msb, முழு lsb, தருக்க local_flag,
		      unsigned argc, கட்டமைப்பு symb_s*argv)
{
      __compile_real(label, name, 0, 0,
                     msb, lsb, local_flag, argc, argv);
}

வெறுமை compile_netw_real(எழுத்து*label, எழுத்து*array_label, unsigned long array_addr,
                       முழு msb, முழு lsb,
                       unsigned argc, கட்டமைப்பு symb_s*argv)
{
      __compile_real(label, 0, array_label, array_addr,
                     msb, lsb, பொய், argc, argv);
}
