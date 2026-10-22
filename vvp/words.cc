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

using ନାମାକାଶ std;

ସ୍ଥିର ଶୂନ୍ୟ __compile_var_real(ଅକ୍ଷର*label, ଅକ୍ଷର*name,
			       vvp_array_t array, unsigned long array_addr,
			       ତାର୍କିକ local_flag)
{
      vvp_net_t*net = ନୂତନ vvp_net_t;

      ଯଦି (vpip_peek_current_scope()->is_automatic()) {
	    vvp_fun_signal_real_aa*tmp = ନୂତନ vvp_fun_signal_real_aa;
	    net->fil = tmp;
	    net->fun = tmp;
      } ଅନ୍ୟଥା {
	    net->fil = ନୂତନ vvp_wire_real;
	    net->fun = ନୂତନ vvp_fun_signal_real_sa;
      }

      define_functor_ପ୍ରତୀକ(label, net);

      vpiHandle obj = vpip_make_real_var(name, net);
      compile_vpi_ପ୍ରତୀକ(label, obj);

      ଯଦି (name) {
	    ପ୍ରତିଜ୍ଞା(!array);
	    ଯଦି (!local_flag)
		  vpip_attach_to_current_scope(obj);
            ଯଦି (!vpip_peek_current_scope()->is_automatic())
                  schedule_init_vector(vvp_net_ptr_t(net,0), 0.0);
      }
      ଯଦି (array) {
	    ପ୍ରତିଜ୍ଞା(!name);
	    array->attach_word(array_addr, obj);
      }
      ମୁକ୍ତ(label);
      delete[] name;
}

ଶୂନ୍ୟ compile_var_real(ଅକ୍ଷର*label, ଅକ୍ଷର*name, ତାର୍କିକ local_flag)
{
      __compile_var_real(label, name, 0, 0, local_flag);
}

ଶୂନ୍ୟ compile_var_string(ଅକ୍ଷର*label, ଅକ୍ଷର*name)
{
      vvp_net_t*net = ନୂତନ vvp_net_t;

      ଯଦି (vpip_peek_current_scope()->is_automatic()) {
	    vvp_fun_signal_string_aa*tmp = ନୂତନ vvp_fun_signal_string_aa;
	    net->fil = tmp;
	    net->fun = tmp;
      } ଅନ୍ୟଥା {
	    net->fil = 0;
	    net->fun = ନୂତନ vvp_fun_signal_string_sa;
      }

      define_functor_ପ୍ରତୀକ(label, net);

      vpiHandle obj = vpip_make_string_var(name, net);
      compile_vpi_ପ୍ରତୀକ(label, obj);

      vpip_attach_to_current_scope(obj);
      ମୁକ୍ତ(label);
      delete[] name;
}

ଶୂନ୍ୟ compile_var_darray(ଅକ୍ଷର*label, ଅକ୍ଷର*name, unsigned size)
{
      vvp_net_t*net = ନୂତନ vvp_net_t;

      ଯଦି (vpip_peek_current_scope()->is_automatic()) {
	    vvp_fun_signal_object_aa*tmp = ନୂତନ vvp_fun_signal_object_aa(size);
	    net->fil = tmp;
	    net->fun = tmp;
      } ଅନ୍ୟଥା {
	    net->fil = 0;
	    net->fun = ନୂତନ vvp_fun_signal_object_sa(size);
      }

      define_functor_ପ୍ରତୀକ(label, net);

      vpiHandle obj = vpip_make_darray_var(name, net);
      compile_vpi_ପ୍ରତୀକ(label, obj);

      vpip_attach_to_current_scope(obj);
      ମୁକ୍ତ(label);
      delete[] name;
}

ଶୂନ୍ୟ compile_var_queue(ଅକ୍ଷର*label, ଅକ୍ଷର*name, unsigned size)
{
      vvp_net_t*net = ନୂତନ vvp_net_t;

      ଯଦି (vpip_peek_current_scope()->is_automatic()) {
	    vvp_fun_signal_object_aa*tmp = ନୂତନ vvp_fun_signal_object_aa(size);
	    net->fil = tmp;
	    net->fun = tmp;
      } ଅନ୍ୟଥା {
	    net->fil = 0;
	    net->fun = ନୂତନ vvp_fun_signal_object_sa(size);
      }

      define_functor_ପ୍ରତୀକ(label, net);

      vpiHandle obj = vpip_make_queue_var(name, net);
      compile_vpi_ପ୍ରତୀକ(label, obj);

      vpip_attach_to_current_scope(obj);
      ମୁକ୍ତ(label);
      delete[] name;
}

ଶୂନ୍ୟ compile_var_cobject(ଅକ୍ଷର*label, ଅକ୍ଷର*name)
{
      vvp_net_t*net = ନୂତନ vvp_net_t;

      ଯଦି (vpip_peek_current_scope()->is_automatic()) {
	    vvp_fun_signal_object_aa*tmp = ନୂତନ vvp_fun_signal_object_aa(1);
	    net->fil = tmp;
	    net->fun = tmp;
      } ଅନ୍ୟଥା {
	    net->fil = 0;
	    net->fun = ନୂତନ vvp_fun_signal_object_sa(1);
      }

      define_functor_ପ୍ରତୀକ(label, net);

      vpiHandle obj = vpip_make_cobject_var(name, net);
      compile_vpi_ପ୍ରତୀକ(label, obj);

      vpip_attach_to_current_scope(obj);
      ମୁକ୍ତ(label);
      delete[] name;
}

/*
 * A variable is a special functor, so we allocate that functor and
 * write the label into the symbol table.
 */
ଶୂନ୍ୟ compile_variable(ଅକ୍ଷର*label, ଅକ୍ଷର*name,
		      ପୂର୍ଣ୍ଣସଂଖ୍ୟା msb, ପୂର୍ଣ୍ଣସଂଖ୍ୟା lsb, ପୂର୍ଣ୍ଣସଂଖ୍ୟା vpi_type_code,
		      ତାର୍କିକ signed_flag, ତାର୍କିକ local_flag)
{
      unsigned wid = ((msb > lsb)? msb-lsb : lsb-msb) + 1;

      vvp_net_t*net = ନୂତନ vvp_net_t;

      vvp_bit4_t init;
      ଯଦି (vpi_type_code == vpiIntVar)
	    init = BIT4_0;
      ଅନ୍ୟଥା
	    init = BIT4_X;

      ଯଦି (vpip_peek_current_scope()->is_automatic()) {
	    vvp_fun_signal4_aa*tmp = ନୂତନ vvp_fun_signal4_aa(wid, init);
	    net->fil = tmp;
            net->fun = tmp;
      } ଅନ୍ୟଥା {
	    net->fil = ନୂତନ vvp_wire_vec4(wid, init);
            net->fun = ନୂତନ vvp_fun_signal4_sa(wid);
      }
      ଅଚଳ vvp_signal_ମୂଲ୍ୟ*vfil = dynamic_cast<vvp_signal_ମୂଲ୍ୟ*>(net->fil);

      define_functor_ପ୍ରତୀକ(label, net);

      vpiHandle obj = 0;
      ଯଦି (! local_flag) {
	      /* Make the vpiHandle for the reg. */
	    ବାଛ (vpi_type_code) {
		ସ୍ଥିତି vpiLogicVar:
		  obj = vpip_make_var4(name, msb, lsb, signed_flag, net);
		  ଭାଙ୍ଗ;
		ସ୍ଥିତି vpiIntegerVar:
		  obj = vpip_make_int4(name, msb, lsb, net);
		  ଭାଙ୍ଗ;
		ସ୍ଥିତି vpiIntVar: // This handles all the atom2 int types
		  obj = vpip_make_int2(name, msb, lsb, signed_flag, net);
		  ଭାଙ୍ଗ;
		ପୂର୍ବନିର୍ଦ୍ଧାରିତ:
		  fprintf(stderr, "internal error: %s: vpi_type_code=%d\n", name, vpi_type_code);
		  ଭାଙ୍ଗ;
	    }
	    ପ୍ରତିଜ୍ଞା(obj);
	    compile_vpi_ପ୍ରତୀକ(label, obj);
      }
	// If the signal has a name, then it goes into the current
	// scope as a signal.
      ଯଦି (name) {
	    ଯଦି (obj) vpip_attach_to_current_scope(obj);
            ଯଦି (!vpip_peek_current_scope()->is_automatic()) {
		  vvp_vector4_t tmp;
		  vfil->vec4_ମୂଲ୍ୟ(tmp);
	          schedule_init_vector(vvp_net_ptr_t(net,0), tmp);
	    }
      }

      ମୁକ୍ତ(label);
      delete[] name;
}


vvp_net_t* create_constant_ଗଣ୍ଠି(ଅଚଳ ଅକ୍ଷର*val_str)
{
      ଯଦି (c4string_test(val_str)) {
	    vvp_net_t*net = ନୂତନ vvp_net_t;
	    net->fun = ନୂତନ vvp_fun_bufz;
	    schedule_init_vector(vvp_net_ptr_t(net,0), c4string_to_vector4(val_str));
	    ଫେରନ୍ତୁ net;
      }

      ଯଦି (c8string_test(val_str)) {
	    vvp_net_t*net = ନୂତନ vvp_net_t;
	    net->fun = ନୂତନ vvp_fun_bufz;
	    schedule_init_vector(vvp_net_ptr_t(net,0), c8string_to_vector8(val_str));
	    ଫେରନ୍ତୁ net;
      }

      ଯଦି (crstring_test(val_str)) {
	    vvp_net_t*net = ନୂତନ vvp_net_t;
	    net->fun = ନୂତନ vvp_fun_bufz;
	    schedule_init_vector(vvp_net_ptr_t(net,0), crstring_to_double(val_str));
	    ଫେରନ୍ତୁ net;
      }

      ଫେରନ୍ତୁ 0;
}

ଶ୍ରେଣୀ base_net_resolv : ସର୍ବସାଧାରଣ resolv_list_s {
    ସର୍ବସାଧାରଣ:
      explicit base_net_resolv(ଅକ୍ଷର*ref_label, vvp_array_t array,
			       __vpiScope*scope,
			       ଅକ୍ଷର*my_label, ଅକ୍ଷର*name,
			       unsigned array_addr, ତାର୍କିକ local_flag)
      : resolv_list_s(ref_label)
      { my_label_ = my_label;
	array_ = array;
	name_ = name;
	scope_ = scope;
	array_addr_ = array_addr;
	local_flag_ = local_flag;
      }

    ସୁରକ୍ଷିତ:
      ଅକ୍ଷର*my_label_;
      vvp_array_t array_;
      ଅକ୍ଷର*name_;
      __vpiScope*scope_;
      unsigned array_addr_;
      ତାର୍କିକ local_flag_;
};

ଶ୍ରେଣୀ __compile_net_resolv : ସର୍ବସାଧାରଣ base_net_resolv {

    ସର୍ବସାଧାରଣ:
      explicit __compile_net_resolv(ଅକ୍ଷର*ref_label, vvp_array_t array,
				    __vpiScope*scope,
				    ଅକ୍ଷର*my_label, ଅକ୍ଷର*name,
				    ପୂର୍ଣ୍ଣସଂଖ୍ୟା msb, ପୂର୍ଣ୍ଣସଂଖ୍ୟା lsb, unsigned array_addr,
				    ପୂର୍ଣ୍ଣସଂଖ୍ୟା vpi_type_code, ତାର୍କିକ signed_flag, ତାର୍କିକ local_flag)
      : base_net_resolv(ref_label, array, scope, my_label, name, array_addr, local_flag)
      { msb_ = msb;
	lsb_ = lsb;
	vpi_type_code_ = vpi_type_code;
	signed_flag_ = signed_flag;
      }

      ~__compile_net_resolv() override { }

      ତାର୍କିକ resolve(ତାର୍କିକ message_flag) override;

    ବ୍ୟକ୍ତିଗତ:
      ପୂର୍ଣ୍ଣସଂଖ୍ୟା msb_, lsb_;
      ପୂର୍ଣ୍ଣସଂଖ୍ୟା vpi_type_code_;
      ତାର୍କିକ signed_flag_;
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

ସ୍ଥିର ଶୂନ୍ୟ do_compile_net(vvp_net_t*ଗଣ୍ଠି, vvp_array_t array,
			   __vpiScope*scope,
			   ଅକ୍ଷର*my_label, ଅକ୍ଷର*name,
			   ପୂର୍ଣ୍ଣସଂଖ୍ୟା msb, ପୂର୍ଣ୍ଣସଂଖ୍ୟା lsb, unsigned array_addr,
			   ପୂର୍ଣ୍ଣସଂଖ୍ୟା vpi_type_code, ତାର୍କିକ signed_flag, ତାର୍କିକ local_flag)
{
      unsigned wid = ((msb > lsb)? msb-lsb : lsb-msb) + 1;
      ପ୍ରତିଜ୍ଞା(ଗଣ୍ଠି);

      vvp_wire_base*vsig = dynamic_cast<vvp_wire_base*>(ଗଣ୍ଠି->fil);

      ଯଦି (vsig == 0) {
	    ବାଛ (vpi_type_code) {
		ସ୍ଥିତି vpiIntVar:
		  vsig = ନୂତନ vvp_wire_vec4(wid,BIT4_0);
		  ଭାଙ୍ଗ;
		ସ୍ଥିତି vpiLogicVar:
		  vsig = ନୂତନ vvp_wire_vec4(wid,BIT4_Z);
		  ଭାଙ୍ଗ;
		ସ୍ଥିତି -vpiLogicVar:
		  vsig = ନୂତନ vvp_wire_vec8(wid);
		  ଭାଙ୍ଗ;
	    }
	    ପ୍ରତିଜ୍ଞା(vsig);
	    ଗଣ୍ଠି->fil = vsig;
      }

      vpiHandle obj = 0;
      ଯଦି (! local_flag) {
	      /* Make the vpiHandle for the reg. */
	    obj = vpip_make_net4(scope, name, msb, lsb, signed_flag, ଗଣ୍ଠି);
	      /* This attaches the label to the vpiHandle */
	    compile_vpi_ପ୍ରତୀକ(my_label, obj);
      }
#ifdef CHECK_WITH_VALGRIND
      ଅନ୍ୟଥା pool_local_net(ଗଣ୍ଠି);
#endif

	// REMOVE ME! Giving the net a label is a legacy of the times
	// when the .net was a functor of its own. In the long run, we
	// must fix the code generator to not rely on the label of the
	// .net, then we will remove that label.
      define_functor_ପ୍ରତୀକ(my_label, ଗଣ୍ଠି);

      ଯଦି (array)
	    array->attach_word(array_addr, obj);
      ଅନ୍ୟଥା ଯଦି (obj)
	    vpip_attach_to_scope(scope,obj);

      ମୁକ୍ତ(my_label);
      delete[] name;
}

ସ୍ଥିର ଶୂନ୍ୟ __compile_net(ଅକ୍ଷର*label,
			  ଅକ୍ଷର*name, ଅକ୍ଷର*array_label, unsigned long array_addr,
			  ପୂର୍ଣ୍ଣସଂଖ୍ୟା msb, ପୂର୍ଣ୍ଣସଂଖ୍ୟା lsb,
			  ପୂର୍ଣ୍ଣସଂଖ୍ୟା vpi_type_code, ତାର୍କିକ signed_flag, ତାର୍କିକ local_flag,
			  unsigned argc, ସଂରଚନା symb_s*argv)
{
      vvp_array_t array = array_label? array_find(array_label) : ଖାଲି;
      ପ୍ରତିଜ୍ଞା(array_label ? array!=0 : ସତ୍ୟ);

      ମୁକ୍ତ(array_label);

      ପ୍ରତିଜ୍ଞା(argc == 1);
      vvp_net_t*ଗଣ୍ଠି = vvp_net_lookup(argv[0].text);
#ଯଦି 1
      ଯଦି (ଗଣ୍ଠି == 0) {
	      /* No existing net, but the string value may be a
		 constant. In that case, we will wind up generating a
		 bufz node that can carry the constant value.

	         NOTE: This is a hack! The code generator should be
	         fixed so that this is no longer needed. */
	    ଗଣ୍ଠି = create_constant_ଗଣ୍ଠି(argv[0].text);
      }
#endif
      ଯଦି (ଗଣ୍ଠି == 0) {
	    __vpiScope*scope = vpip_peek_current_scope();
	    __compile_net_resolv*res
		  = ନୂତନ __compile_net_resolv(argv[0].text,
					     array, scope, label, name,
					     msb, lsb, array_addr,
					     vpi_type_code, signed_flag, local_flag);
	    resolv_submit(res);
	    ମୁକ୍ତ(argv);
	    ଫେରନ୍ତୁ;
      }
      ପ୍ରତିଜ୍ଞା(ଗଣ୍ଠି);

      __vpiScope*scope = vpip_peek_current_scope();
      do_compile_net(ଗଣ୍ଠି, array, scope, label, name, msb, lsb, array_addr,
		     vpi_type_code, signed_flag, local_flag);

      ମୁକ୍ତ(argv[0].text);
      ମୁକ୍ତ(argv);
}

ତାର୍କିକ __compile_net_resolv::resolve(ତାର୍କିକ ସନ୍ଦେଶ_flag)
{
      vvp_net_t*ଗଣ୍ଠି = vvp_net_lookup(label());
      ଯଦି (ଗଣ୍ଠି == 0) {
	    ଯଦି (ସନ୍ଦେଶ_flag)
		  cerr << "Unable to resolve label " << label() << endl;
	    ଫେରନ୍ତୁ ମିଥ୍ୟା;
      }

      do_compile_net(ଗଣ୍ଠି, array_, scope_, my_label_, name_, msb_, lsb_, array_addr_, vpi_type_code_, signed_flag_, local_flag_);
      ଫେରନ୍ତୁ ସତ୍ୟ;
}

ଶୂନ୍ୟ compile_net(ଅକ୍ଷର*label, ଅକ୍ଷର*name, ପୂର୍ଣ୍ଣସଂଖ୍ୟା msb, ପୂର୍ଣ୍ଣସଂଖ୍ୟା lsb,
		 ପୂର୍ଣ୍ଣସଂଖ୍ୟା vpi_type_code, ତାର୍କିକ signed_flag, ତାର୍କିକ local_flag,
		 unsigned argc, ସଂରଚନା symb_s*argv)
{
      __compile_net(label, name, 0, 0, msb, lsb,
		    vpi_type_code, signed_flag, local_flag,
		    argc, argv);
}

ଶୂନ୍ୟ compile_netw(ଅକ୍ଷର*label, ଅକ୍ଷର*array_label, unsigned long array_addr,
		  ପୂର୍ଣ୍ଣସଂଖ୍ୟା msb, ପୂର୍ଣ୍ଣସଂଖ୍ୟା lsb, ପୂର୍ଣ୍ଣସଂଖ୍ୟା vpi_type_code, ତାର୍କିକ signed_flag,
		  unsigned argc, ସଂରଚନା symb_s*argv)
{
      __compile_net(label, 0, array_label, array_addr,
		    msb, lsb, vpi_type_code, signed_flag, ମିଥ୍ୟା,
		    argc, argv);
}

ଶ୍ରେଣୀ __compile_real_net_resolv : ସର୍ବସାଧାରଣ base_net_resolv {

    ସର୍ବସାଧାରଣ:
      explicit __compile_real_net_resolv(ଅକ୍ଷର*ref_label, vvp_array_t array,
					 __vpiScope*scope,
					 ଅକ୍ଷର*my_label, ଅକ୍ଷର*name,
					 unsigned array_addr, ତାର୍କିକ local_flag)
      : base_net_resolv(ref_label, array, scope, my_label, name, array_addr, local_flag)
      {
      }

      ~__compile_real_net_resolv() override { }

      ତାର୍କିକ resolve(ତାର୍କିକ message_flag) override;

    ବ୍ୟକ୍ତିଗତ:
};

ସ୍ଥିର ଶୂନ୍ୟ __compile_real_net2(vvp_net_t*ଗଣ୍ଠି, vvp_array_t array,
				__vpiScope*scope,
				ଅକ୍ଷର*my_label, ଅକ୍ଷର*name,
				unsigned array_addr, ତାର୍କିକ local_flag)
{
      vvp_wire_base*fil = dynamic_cast<vvp_wire_base*> (ଗଣ୍ଠି->fil);

      ଯଦି (fil == 0) {
	    fil = ନୂତନ vvp_wire_real;
	    ଗଣ୍ଠି->fil = fil;
      }

      vpiHandle obj = 0;
      ଯଦି (!local_flag) {
	    obj = vpip_make_real_net(scope, name, ଗଣ୍ଠି);
	    compile_vpi_ପ୍ରତୀକ(my_label, obj);
      }
#ifdef CHECK_WITH_VALGRIND
      ଅନ୍ୟଥା pool_local_net(ଗଣ୍ଠି);
#endif

	// REMOVE ME! Giving the net a label is a legacy of the times
	// when the .net was a functor of its own. In the long run, we
	// must fix the code generator to not rely on the label of the
	// .net, then we will remove that label.
      define_functor_ପ୍ରତୀକ(my_label, ଗଣ୍ଠି);

     ଯଦି (array)
	    array->attach_word(array_addr, obj);
      ଅନ୍ୟଥା ଯଦି (obj)
	    vpip_attach_to_scope(scope, obj);

      ମୁକ୍ତ(my_label);
      delete[] name;
}

ସ୍ଥିର ଶୂନ୍ୟ __compile_real(ଅକ୍ଷର*label, ଅକ୍ଷର*name,
                           ଅକ୍ଷର*array_label, unsigned long array_addr,
                           ପୂର୍ଣ୍ଣସଂଖ୍ୟା msb, ପୂର୍ଣ୍ଣସଂଖ୍ୟା lsb, ତାର୍କିକ local_flag,
                           unsigned argc, ସଂରଚନା symb_s*argv)
{
      ପ୍ରତିଜ୍ଞା(msb == 0 && lsb == 0);
      vvp_array_t array = array_label ? array_find(array_label) : ଖାଲି;
      ପ୍ରତିଜ୍ଞା(array_label ? array!=0 : ସତ୍ୟ);

      ମୁକ୍ତ(array_label);

      ପ୍ରତିଜ୍ଞା(argc == 1);
      vvp_net_t*ଗଣ୍ଠି = vvp_net_lookup(argv[0].text);
      ଯଦି (ଗଣ୍ଠି == 0) {
	      /* No existing net, but the string value may be a
		 constant. In that case, we will wind up generating a
		 bufz node that can carry the constant value. */
	    ଗଣ୍ଠି = create_constant_ଗଣ୍ଠି(argv[0].text);
      }
      __vpiScope*scope = vpip_peek_current_scope();
      ଯଦି (ଗଣ୍ଠି == 0) {
	    __compile_real_net_resolv*res
		  = ନୂତନ __compile_real_net_resolv(argv[0].text, array,
						  scope, label, name,
						  array_addr, local_flag);
	    resolv_submit(res);
	    ମୁକ୍ତ(argv);
	    ଫେରନ୍ତୁ;
      }

      ପ୍ରତିଜ୍ଞା(ଗଣ୍ଠି);
      __compile_real_net2(ଗଣ୍ଠି, array, scope, label, name, array_addr,
                          local_flag);
      ମୁକ୍ତ(argv[0].text);
      ମୁକ୍ତ(argv);
}

ତାର୍କିକ __compile_real_net_resolv::resolve(ତାର୍କିକ ସନ୍ଦେଶ_flag)
{
      vvp_net_t*ଗଣ୍ଠି = vvp_net_lookup(label());
      ଯଦି (ଗଣ୍ଠି == 0) {
	    ଯଦି (ସନ୍ଦେଶ_flag)
		  cerr << "Unable to resolve label " << label() << endl;
	    ଫେରନ୍ତୁ ମିଥ୍ୟା;
      }

      __compile_real_net2(ଗଣ୍ଠି, array_, scope_, my_label_, name_, array_addr_, local_flag_);
      ଫେରନ୍ତୁ ସତ୍ୟ;
}

ଶୂନ୍ୟ compile_net_real(ଅକ୍ଷର*label, ଅକ୍ଷର*name, ପୂର୍ଣ୍ଣସଂଖ୍ୟା msb, ପୂର୍ଣ୍ଣସଂଖ୍ୟା lsb, ତାର୍କିକ local_flag,
		      unsigned argc, ସଂରଚନା symb_s*argv)
{
      __compile_real(label, name, 0, 0,
                     msb, lsb, local_flag, argc, argv);
}

ଶୂନ୍ୟ compile_netw_real(ଅକ୍ଷର*label, ଅକ୍ଷର*array_label, unsigned long array_addr,
                       ପୂର୍ଣ୍ଣସଂଖ୍ୟା msb, ପୂର୍ଣ୍ଣସଂଖ୍ୟା lsb,
                       unsigned argc, ସଂରଚନା symb_s*argv)
{
      __compile_real(label, 0, array_label, array_addr,
                     msb, lsb, ମିଥ୍ୟା, argc, argv);
}
