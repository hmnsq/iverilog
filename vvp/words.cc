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

using नामाकाश std;

स्थिर शून्य __compile_var_real(वर्ण*label, वर्ण*name,
			       vvp_array_t array, unsigned long array_addr,
			       तार्किक local_flag)
{
      vvp_net_t*net = नूतन vvp_net_t;

      सचे (vpip_peek_current_scope()->is_automatic()) {
	    vvp_fun_signal_real_aa*tmp = नूतन vvp_fun_signal_real_aa;
	    net->fil = tmp;
	    net->fun = tmp;
      } अञ्ञथा {
	    net->fil = नूतन vvp_wire_real;
	    net->fun = नूतन vvp_fun_signal_real_sa;
      }

      define_functor_चिह्नम्(label, net);

      vpiHandle obj = vpip_make_real_var(name, net);
      compile_vpi_चिह्नम्(label, obj);

      सचे (name) {
	    प्रतिज्ञा(!array);
	    सचे (!local_flag)
		  vpip_attach_to_current_scope(obj);
            सचे (!vpip_peek_current_scope()->is_automatic())
                  schedule_init_vector(vvp_net_ptr_t(net,0), 0.0);
      }
      सचे (array) {
	    प्रतिज्ञा(!name);
	    array->attach_word(array_addr, obj);
      }
      मुक्त(label);
      delete[] name;
}

शून्य compile_var_real(वर्ण*label, वर्ण*name, तार्किक local_flag)
{
      __compile_var_real(label, name, 0, 0, local_flag);
}

शून्य compile_var_string(वर्ण*label, वर्ण*name)
{
      vvp_net_t*net = नूतन vvp_net_t;

      सचे (vpip_peek_current_scope()->is_automatic()) {
	    vvp_fun_signal_string_aa*tmp = नूतन vvp_fun_signal_string_aa;
	    net->fil = tmp;
	    net->fun = tmp;
      } अञ्ञथा {
	    net->fil = 0;
	    net->fun = नूतन vvp_fun_signal_string_sa;
      }

      define_functor_चिह्नम्(label, net);

      vpiHandle obj = vpip_make_string_var(name, net);
      compile_vpi_चिह्नम्(label, obj);

      vpip_attach_to_current_scope(obj);
      मुक्त(label);
      delete[] name;
}

शून्य compile_var_darray(वर्ण*label, वर्ण*name, unsigned size)
{
      vvp_net_t*net = नूतन vvp_net_t;

      सचे (vpip_peek_current_scope()->is_automatic()) {
	    vvp_fun_signal_object_aa*tmp = नूतन vvp_fun_signal_object_aa(size);
	    net->fil = tmp;
	    net->fun = tmp;
      } अञ्ञथा {
	    net->fil = 0;
	    net->fun = नूतन vvp_fun_signal_object_sa(size);
      }

      define_functor_चिह्नम्(label, net);

      vpiHandle obj = vpip_make_darray_var(name, net);
      compile_vpi_चिह्नम्(label, obj);

      vpip_attach_to_current_scope(obj);
      मुक्त(label);
      delete[] name;
}

शून्य compile_var_queue(वर्ण*label, वर्ण*name, unsigned size)
{
      vvp_net_t*net = नूतन vvp_net_t;

      सचे (vpip_peek_current_scope()->is_automatic()) {
	    vvp_fun_signal_object_aa*tmp = नूतन vvp_fun_signal_object_aa(size);
	    net->fil = tmp;
	    net->fun = tmp;
      } अञ्ञथा {
	    net->fil = 0;
	    net->fun = नूतन vvp_fun_signal_object_sa(size);
      }

      define_functor_चिह्नम्(label, net);

      vpiHandle obj = vpip_make_queue_var(name, net);
      compile_vpi_चिह्नम्(label, obj);

      vpip_attach_to_current_scope(obj);
      मुक्त(label);
      delete[] name;
}

शून्य compile_var_cobject(वर्ण*label, वर्ण*name)
{
      vvp_net_t*net = नूतन vvp_net_t;

      सचे (vpip_peek_current_scope()->is_automatic()) {
	    vvp_fun_signal_object_aa*tmp = नूतन vvp_fun_signal_object_aa(1);
	    net->fil = tmp;
	    net->fun = tmp;
      } अञ्ञथा {
	    net->fil = 0;
	    net->fun = नूतन vvp_fun_signal_object_sa(1);
      }

      define_functor_चिह्नम्(label, net);

      vpiHandle obj = vpip_make_cobject_var(name, net);
      compile_vpi_चिह्नम्(label, obj);

      vpip_attach_to_current_scope(obj);
      मुक्त(label);
      delete[] name;
}

/*
 * A variable is a special functor, so we allocate that functor and
 * write the label into the symbol table.
 */
शून्य compile_variable(वर्ण*label, वर्ण*name,
		      पूर्णाङ्क msb, पूर्णाङ्क lsb, पूर्णाङ्क vpi_type_code,
		      तार्किक signed_flag, तार्किक local_flag)
{
      unsigned wid = ((msb > lsb)? msb-lsb : lsb-msb) + 1;

      vvp_net_t*net = नूतन vvp_net_t;

      vvp_bit4_t init;
      सचे (vpi_type_code == vpiIntVar)
	    init = BIT4_0;
      अञ्ञथा
	    init = BIT4_X;

      सचे (vpip_peek_current_scope()->is_automatic()) {
	    vvp_fun_signal4_aa*tmp = नूतन vvp_fun_signal4_aa(wid, init);
	    net->fil = tmp;
            net->fun = tmp;
      } अञ्ञथा {
	    net->fil = नूतन vvp_wire_vec4(wid, init);
            net->fun = नूतन vvp_fun_signal4_sa(wid);
      }
      ध्रुव vvp_signal_मानम्*vfil = dynamic_cast<vvp_signal_मानम्*>(net->fil);

      define_functor_चिह्नम्(label, net);

      vpiHandle obj = 0;
      सचे (! local_flag) {
	      /* Make the vpiHandle for the reg. */
	    विकल्प (vpi_type_code) {
		पक्ष vpiLogicVar:
		  obj = vpip_make_var4(name, msb, lsb, signed_flag, net);
		  भङ्ग;
		पक्ष vpiIntegerVar:
		  obj = vpip_make_int4(name, msb, lsb, net);
		  भङ्ग;
		पक्ष vpiIntVar: // This handles all the atom2 int types
		  obj = vpip_make_int2(name, msb, lsb, signed_flag, net);
		  भङ्ग;
		स्वभाव:
		  fprintf(stderr, "internal error: %s: vpi_type_code=%d\n", name, vpi_type_code);
		  भङ्ग;
	    }
	    प्रतिज्ञा(obj);
	    compile_vpi_चिह्नम्(label, obj);
      }
	// If the signal has a name, then it goes into the current
	// scope as a signal.
      सचे (name) {
	    सचे (obj) vpip_attach_to_current_scope(obj);
            सचे (!vpip_peek_current_scope()->is_automatic()) {
		  vvp_vector4_t tmp;
		  vfil->vec4_मानम्(tmp);
	          schedule_init_vector(vvp_net_ptr_t(net,0), tmp);
	    }
      }

      मुक्त(label);
      delete[] name;
}


vvp_net_t* create_constant_ग्रन्थिः(ध्रुव वर्ण*val_str)
{
      सचे (c4string_test(val_str)) {
	    vvp_net_t*net = नूतन vvp_net_t;
	    net->fun = नूतन vvp_fun_bufz;
	    schedule_init_vector(vvp_net_ptr_t(net,0), c4string_to_vector4(val_str));
	    पटिनिवत्त net;
      }

      सचे (c8string_test(val_str)) {
	    vvp_net_t*net = नूतन vvp_net_t;
	    net->fun = नूतन vvp_fun_bufz;
	    schedule_init_vector(vvp_net_ptr_t(net,0), c8string_to_vector8(val_str));
	    पटिनिवत्त net;
      }

      सचे (crstring_test(val_str)) {
	    vvp_net_t*net = नूतन vvp_net_t;
	    net->fun = नूतन vvp_fun_bufz;
	    schedule_init_vector(vvp_net_ptr_t(net,0), crstring_to_double(val_str));
	    पटिनिवत्त net;
      }

      पटिनिवत्त 0;
}

वग्ग base_net_resolv : सार्वजनिक resolv_list_s {
    सार्वजनिक:
      explicit base_net_resolv(वर्ण*ref_label, vvp_array_t array,
			       __vpiScope*scope,
			       वर्ण*my_label, वर्ण*name,
			       unsigned array_addr, तार्किक local_flag)
      : resolv_list_s(ref_label)
      { my_label_ = my_label;
	array_ = array;
	name_ = name;
	scope_ = scope;
	array_addr_ = array_addr;
	local_flag_ = local_flag;
      }

    रक्षित:
      वर्ण*my_label_;
      vvp_array_t array_;
      वर्ण*name_;
      __vpiScope*scope_;
      unsigned array_addr_;
      तार्किक local_flag_;
};

वग्ग __compile_net_resolv : सार्वजनिक base_net_resolv {

    सार्वजनिक:
      explicit __compile_net_resolv(वर्ण*ref_label, vvp_array_t array,
				    __vpiScope*scope,
				    वर्ण*my_label, वर्ण*name,
				    पूर्णाङ्क msb, पूर्णाङ्क lsb, unsigned array_addr,
				    पूर्णाङ्क vpi_type_code, तार्किक signed_flag, तार्किक local_flag)
      : base_net_resolv(ref_label, array, scope, my_label, name, array_addr, local_flag)
      { msb_ = msb;
	lsb_ = lsb;
	vpi_type_code_ = vpi_type_code;
	signed_flag_ = signed_flag;
      }

      ~__compile_net_resolv() override { }

      तार्किक resolve(तार्किक message_flag) override;

    गुप्त:
      पूर्णाङ्क msb_, lsb_;
      पूर्णाङ्क vpi_type_code_;
      तार्किक signed_flag_;
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

स्थिर शून्य do_compile_net(vvp_net_t*ग्रन्थिः, vvp_array_t array,
			   __vpiScope*scope,
			   वर्ण*my_label, वर्ण*name,
			   पूर्णाङ्क msb, पूर्णाङ्क lsb, unsigned array_addr,
			   पूर्णाङ्क vpi_type_code, तार्किक signed_flag, तार्किक local_flag)
{
      unsigned wid = ((msb > lsb)? msb-lsb : lsb-msb) + 1;
      प्रतिज्ञा(ग्रन्थिः);

      vvp_wire_base*vsig = dynamic_cast<vvp_wire_base*>(ग्रन्थिः->fil);

      सचे (vsig == 0) {
	    विकल्प (vpi_type_code) {
		पक्ष vpiIntVar:
		  vsig = नूतन vvp_wire_vec4(wid,BIT4_0);
		  भङ्ग;
		पक्ष vpiLogicVar:
		  vsig = नूतन vvp_wire_vec4(wid,BIT4_Z);
		  भङ्ग;
		पक्ष -vpiLogicVar:
		  vsig = नूतन vvp_wire_vec8(wid);
		  भङ्ग;
	    }
	    प्रतिज्ञा(vsig);
	    ग्रन्थिः->fil = vsig;
      }

      vpiHandle obj = 0;
      सचे (! local_flag) {
	      /* Make the vpiHandle for the reg. */
	    obj = vpip_make_net4(scope, name, msb, lsb, signed_flag, ग्रन्थिः);
	      /* This attaches the label to the vpiHandle */
	    compile_vpi_चिह्नम्(my_label, obj);
      }
#ifdef CHECK_WITH_VALGRIND
      अञ्ञथा pool_local_net(ग्रन्थिः);
#endif

	// REMOVE ME! Giving the net a label is a legacy of the times
	// when the .net was a functor of its own. In the long run, we
	// must fix the code generator to not rely on the label of the
	// .net, then we will remove that label.
      define_functor_चिह्नम्(my_label, ग्रन्थिः);

      सचे (array)
	    array->attach_word(array_addr, obj);
      अञ्ञथा सचे (obj)
	    vpip_attach_to_scope(scope,obj);

      मुक्त(my_label);
      delete[] name;
}

स्थिर शून्य __compile_net(वर्ण*label,
			  वर्ण*name, वर्ण*array_label, unsigned long array_addr,
			  पूर्णाङ्क msb, पूर्णाङ्क lsb,
			  पूर्णाङ्क vpi_type_code, तार्किक signed_flag, तार्किक local_flag,
			  unsigned argc, संरचना symb_s*argv)
{
      vvp_array_t array = array_label? array_find(array_label) : रिक्त;
      प्रतिज्ञा(array_label ? array!=0 : सच्च);

      मुक्त(array_label);

      प्रतिज्ञा(argc == 1);
      vvp_net_t*ग्रन्थिः = vvp_net_lookup(argv[0].text);
#सचे 1
      सचे (ग्रन्थिः == 0) {
	      /* No existing net, but the string value may be a
		 constant. In that case, we will wind up generating a
		 bufz node that can carry the constant value.

	         NOTE: This is a hack! The code generator should be
	         fixed so that this is no longer needed. */
	    ग्रन्थिः = create_constant_ग्रन्थिः(argv[0].text);
      }
#endif
      सचे (ग्रन्थिः == 0) {
	    __vpiScope*scope = vpip_peek_current_scope();
	    __compile_net_resolv*res
		  = नूतन __compile_net_resolv(argv[0].text,
					     array, scope, label, name,
					     msb, lsb, array_addr,
					     vpi_type_code, signed_flag, local_flag);
	    resolv_submit(res);
	    मुक्त(argv);
	    पटिनिवत्त;
      }
      प्रतिज्ञा(ग्रन्थिः);

      __vpiScope*scope = vpip_peek_current_scope();
      do_compile_net(ग्रन्थिः, array, scope, label, name, msb, lsb, array_addr,
		     vpi_type_code, signed_flag, local_flag);

      मुक्त(argv[0].text);
      मुक्त(argv);
}

तार्किक __compile_net_resolv::resolve(तार्किक सन्देशः_flag)
{
      vvp_net_t*ग्रन्थिः = vvp_net_lookup(label());
      सचे (ग्रन्थिः == 0) {
	    सचे (सन्देशः_flag)
		  cerr << "Unable to resolve label " << label() << endl;
	    पटिनिवत्त मिच्छा;
      }

      do_compile_net(ग्रन्थिः, array_, scope_, my_label_, name_, msb_, lsb_, array_addr_, vpi_type_code_, signed_flag_, local_flag_);
      पटिनिवत्त सच्च;
}

शून्य compile_net(वर्ण*label, वर्ण*name, पूर्णाङ्क msb, पूर्णाङ्क lsb,
		 पूर्णाङ्क vpi_type_code, तार्किक signed_flag, तार्किक local_flag,
		 unsigned argc, संरचना symb_s*argv)
{
      __compile_net(label, name, 0, 0, msb, lsb,
		    vpi_type_code, signed_flag, local_flag,
		    argc, argv);
}

शून्य compile_netw(वर्ण*label, वर्ण*array_label, unsigned long array_addr,
		  पूर्णाङ्क msb, पूर्णाङ्क lsb, पूर्णाङ्क vpi_type_code, तार्किक signed_flag,
		  unsigned argc, संरचना symb_s*argv)
{
      __compile_net(label, 0, array_label, array_addr,
		    msb, lsb, vpi_type_code, signed_flag, मिच्छा,
		    argc, argv);
}

वग्ग __compile_real_net_resolv : सार्वजनिक base_net_resolv {

    सार्वजनिक:
      explicit __compile_real_net_resolv(वर्ण*ref_label, vvp_array_t array,
					 __vpiScope*scope,
					 वर्ण*my_label, वर्ण*name,
					 unsigned array_addr, तार्किक local_flag)
      : base_net_resolv(ref_label, array, scope, my_label, name, array_addr, local_flag)
      {
      }

      ~__compile_real_net_resolv() override { }

      तार्किक resolve(तार्किक message_flag) override;

    गुप्त:
};

स्थिर शून्य __compile_real_net2(vvp_net_t*ग्रन्थिः, vvp_array_t array,
				__vpiScope*scope,
				वर्ण*my_label, वर्ण*name,
				unsigned array_addr, तार्किक local_flag)
{
      vvp_wire_base*fil = dynamic_cast<vvp_wire_base*> (ग्रन्थिः->fil);

      सचे (fil == 0) {
	    fil = नूतन vvp_wire_real;
	    ग्रन्थिः->fil = fil;
      }

      vpiHandle obj = 0;
      सचे (!local_flag) {
	    obj = vpip_make_real_net(scope, name, ग्रन्थिः);
	    compile_vpi_चिह्नम्(my_label, obj);
      }
#ifdef CHECK_WITH_VALGRIND
      अञ्ञथा pool_local_net(ग्रन्थिः);
#endif

	// REMOVE ME! Giving the net a label is a legacy of the times
	// when the .net was a functor of its own. In the long run, we
	// must fix the code generator to not rely on the label of the
	// .net, then we will remove that label.
      define_functor_चिह्नम्(my_label, ग्रन्थिः);

     सचे (array)
	    array->attach_word(array_addr, obj);
      अञ्ञथा सचे (obj)
	    vpip_attach_to_scope(scope, obj);

      मुक्त(my_label);
      delete[] name;
}

स्थिर शून्य __compile_real(वर्ण*label, वर्ण*name,
                           वर्ण*array_label, unsigned long array_addr,
                           पूर्णाङ्क msb, पूर्णाङ्क lsb, तार्किक local_flag,
                           unsigned argc, संरचना symb_s*argv)
{
      प्रतिज्ञा(msb == 0 && lsb == 0);
      vvp_array_t array = array_label ? array_find(array_label) : रिक्त;
      प्रतिज्ञा(array_label ? array!=0 : सच्च);

      मुक्त(array_label);

      प्रतिज्ञा(argc == 1);
      vvp_net_t*ग्रन्थिः = vvp_net_lookup(argv[0].text);
      सचे (ग्रन्थिः == 0) {
	      /* No existing net, but the string value may be a
		 constant. In that case, we will wind up generating a
		 bufz node that can carry the constant value. */
	    ग्रन्थिः = create_constant_ग्रन्थिः(argv[0].text);
      }
      __vpiScope*scope = vpip_peek_current_scope();
      सचे (ग्रन्थिः == 0) {
	    __compile_real_net_resolv*res
		  = नूतन __compile_real_net_resolv(argv[0].text, array,
						  scope, label, name,
						  array_addr, local_flag);
	    resolv_submit(res);
	    मुक्त(argv);
	    पटिनिवत्त;
      }

      प्रतिज्ञा(ग्रन्थिः);
      __compile_real_net2(ग्रन्थिः, array, scope, label, name, array_addr,
                          local_flag);
      मुक्त(argv[0].text);
      मुक्त(argv);
}

तार्किक __compile_real_net_resolv::resolve(तार्किक सन्देशः_flag)
{
      vvp_net_t*ग्रन्थिः = vvp_net_lookup(label());
      सचे (ग्रन्थिः == 0) {
	    सचे (सन्देशः_flag)
		  cerr << "Unable to resolve label " << label() << endl;
	    पटिनिवत्त मिच्छा;
      }

      __compile_real_net2(ग्रन्थिः, array_, scope_, my_label_, name_, array_addr_, local_flag_);
      पटिनिवत्त सच्च;
}

शून्य compile_net_real(वर्ण*label, वर्ण*name, पूर्णाङ्क msb, पूर्णाङ्क lsb, तार्किक local_flag,
		      unsigned argc, संरचना symb_s*argv)
{
      __compile_real(label, name, 0, 0,
                     msb, lsb, local_flag, argc, argv);
}

शून्य compile_netw_real(वर्ण*label, वर्ण*array_label, unsigned long array_addr,
                       पूर्णाङ्क msb, पूर्णाङ्क lsb,
                       unsigned argc, संरचना symb_s*argv)
{
      __compile_real(label, 0, array_label, array_addr,
                     msb, lsb, मिच्छा, argc, argv);
}
