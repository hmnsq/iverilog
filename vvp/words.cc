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

स्थिर शून्य __compile_var_real(अक्षर*label, अक्षर*name,
			       vvp_array_t array, अचिन्हित दीर्घ array_addr,
			       तार्किक local_flag)
{
      vvp_net_t*net = नवीन vvp_net_t;

      जर (vpip_peek_current_scope()->is_automatic()) {
	    vvp_fun_signal_real_aa*tmp = नवीन vvp_fun_signal_real_aa;
	    net->fil = tmp;
	    net->fun = tmp;
      } अन्यथा {
	    net->fil = नवीन vvp_wire_real;
	    net->fun = नवीन vvp_fun_signal_real_sa;
      }

      define_functor_चिन्ह(label, net);

      vpiHandle obj = vpip_make_real_var(name, net);
      compile_vpi_चिन्ह(label, obj);

      जर (name) {
	    प्रतिज्ञा(!array);
	    जर (!local_flag)
		  vpip_attach_to_current_scope(obj);
            जर (!vpip_peek_current_scope()->is_automatic())
                  schedule_init_vector(vvp_net_ptr_t(net,0), 0.0);
      }
      जर (array) {
	    प्रतिज्ञा(!name);
	    array->attach_word(array_addr, obj);
      }
      मुक्त(label);
      delete[] name;
}

शून्य compile_var_real(अक्षर*label, अक्षर*name, तार्किक local_flag)
{
      __compile_var_real(label, name, 0, 0, local_flag);
}

शून्य compile_var_string(अक्षर*label, अक्षर*name)
{
      vvp_net_t*net = नवीन vvp_net_t;

      जर (vpip_peek_current_scope()->is_automatic()) {
	    vvp_fun_signal_string_aa*tmp = नवीन vvp_fun_signal_string_aa;
	    net->fil = tmp;
	    net->fun = tmp;
      } अन्यथा {
	    net->fil = 0;
	    net->fun = नवीन vvp_fun_signal_string_sa;
      }

      define_functor_चिन्ह(label, net);

      vpiHandle obj = vpip_make_string_var(name, net);
      compile_vpi_चिन्ह(label, obj);

      vpip_attach_to_current_scope(obj);
      मुक्त(label);
      delete[] name;
}

शून्य compile_var_darray(अक्षर*label, अक्षर*name, अचिन्हित size)
{
      vvp_net_t*net = नवीन vvp_net_t;

      जर (vpip_peek_current_scope()->is_automatic()) {
	    vvp_fun_signal_object_aa*tmp = नवीन vvp_fun_signal_object_aa(size);
	    net->fil = tmp;
	    net->fun = tmp;
      } अन्यथा {
	    net->fil = 0;
	    net->fun = नवीन vvp_fun_signal_object_sa(size);
      }

      define_functor_चिन्ह(label, net);

      vpiHandle obj = vpip_make_darray_var(name, net);
      compile_vpi_चिन्ह(label, obj);

      vpip_attach_to_current_scope(obj);
      मुक्त(label);
      delete[] name;
}

शून्य compile_var_queue(अक्षर*label, अक्षर*name, अचिन्हित size)
{
      vvp_net_t*net = नवीन vvp_net_t;

      जर (vpip_peek_current_scope()->is_automatic()) {
	    vvp_fun_signal_object_aa*tmp = नवीन vvp_fun_signal_object_aa(size);
	    net->fil = tmp;
	    net->fun = tmp;
      } अन्यथा {
	    net->fil = 0;
	    net->fun = नवीन vvp_fun_signal_object_sa(size);
      }

      define_functor_चिन्ह(label, net);

      vpiHandle obj = vpip_make_queue_var(name, net);
      compile_vpi_चिन्ह(label, obj);

      vpip_attach_to_current_scope(obj);
      मुक्त(label);
      delete[] name;
}

शून्य compile_var_cobject(अक्षर*label, अक्षर*name)
{
      vvp_net_t*net = नवीन vvp_net_t;

      जर (vpip_peek_current_scope()->is_automatic()) {
	    vvp_fun_signal_object_aa*tmp = नवीन vvp_fun_signal_object_aa(1);
	    net->fil = tmp;
	    net->fun = tmp;
      } अन्यथा {
	    net->fil = 0;
	    net->fun = नवीन vvp_fun_signal_object_sa(1);
      }

      define_functor_चिन्ह(label, net);

      vpiHandle obj = vpip_make_cobject_var(name, net);
      compile_vpi_चिन्ह(label, obj);

      vpip_attach_to_current_scope(obj);
      मुक्त(label);
      delete[] name;
}

/*
 * A variable is a special functor, so we allocate that functor and
 * write the label into the symbol table.
 */
शून्य compile_variable(अक्षर*label, अक्षर*name,
		      पूर्णांक msb, पूर्णांक lsb, पूर्णांक vpi_type_code,
		      तार्किक signed_flag, तार्किक local_flag)
{
      अचिन्हित wid = ((msb > lsb)? msb-lsb : lsb-msb) + 1;

      vvp_net_t*net = नवीन vvp_net_t;

      vvp_bit4_t init;
      जर (vpi_type_code == vpiIntVar)
	    init = BIT4_0;
      अन्यथा
	    init = BIT4_X;

      जर (vpip_peek_current_scope()->is_automatic()) {
	    vvp_fun_signal4_aa*tmp = नवीन vvp_fun_signal4_aa(wid, init);
	    net->fil = tmp;
            net->fun = tmp;
      } अन्यथा {
	    net->fil = नवीन vvp_wire_vec4(wid, init);
            net->fun = नवीन vvp_fun_signal4_sa(wid);
      }
      स्थिरांक vvp_signal_मूल्य*vfil = dynamic_cast<vvp_signal_मूल्य*>(net->fil);

      define_functor_चिन्ह(label, net);

      vpiHandle obj = 0;
      जर (! local_flag) {
	      /* Make the vpiHandle for the reg. */
	    निवड (vpi_type_code) {
		स्थिती vpiLogicVar:
		  obj = vpip_make_var4(name, msb, lsb, signed_flag, net);
		  थांबा;
		स्थिती vpiIntegerVar:
		  obj = vpip_make_int4(name, msb, lsb, net);
		  थांबा;
		स्थिती vpiIntVar: // This handles all the atom2 int types
		  obj = vpip_make_int2(name, msb, lsb, signed_flag, net);
		  थांबा;
		पूर्वनिर्धारित:
		  ख_लिखो(मानक_त्रुटि, "internal error: %s: vpi_type_code=%d\n", name, vpi_type_code);
		  थांबा;
	    }
	    प्रतिज्ञा(obj);
	    compile_vpi_चिन्ह(label, obj);
      }
	// If the signal has a name, then it goes into the current
	// scope as a signal.
      जर (name) {
	    जर (obj) vpip_attach_to_current_scope(obj);
            जर (!vpip_peek_current_scope()->is_automatic()) {
		  vvp_vector4_t tmp;
		  vfil->vec4_मूल्य(tmp);
	          schedule_init_vector(vvp_net_ptr_t(net,0), tmp);
	    }
      }

      मुक्त(label);
      delete[] name;
}


vvp_net_t* create_constant_गाठ(स्थिरांक अक्षर*val_str)
{
      जर (c4string_test(val_str)) {
	    vvp_net_t*net = नवीन vvp_net_t;
	    net->fun = नवीन vvp_fun_bufz;
	    schedule_init_vector(vvp_net_ptr_t(net,0), c4string_to_vector4(val_str));
	    परत net;
      }

      जर (c8string_test(val_str)) {
	    vvp_net_t*net = नवीन vvp_net_t;
	    net->fun = नवीन vvp_fun_bufz;
	    schedule_init_vector(vvp_net_ptr_t(net,0), c8string_to_vector8(val_str));
	    परत net;
      }

      जर (crstring_test(val_str)) {
	    vvp_net_t*net = नवीन vvp_net_t;
	    net->fun = नवीन vvp_fun_bufz;
	    schedule_init_vector(vvp_net_ptr_t(net,0), crstring_to_double(val_str));
	    परत net;
      }

      परत 0;
}

वर्ग base_net_resolv : सार्वजनिक resolv_list_s {
    सार्वजनिक:
      explicit base_net_resolv(अक्षर*ref_label, vvp_array_t array,
			       __vpiScope*scope,
			       अक्षर*my_label, अक्षर*name,
			       अचिन्हित array_addr, तार्किक local_flag)
      : resolv_list_s(ref_label)
      { my_label_ = my_label;
	array_ = array;
	name_ = name;
	scope_ = scope;
	array_addr_ = array_addr;
	local_flag_ = local_flag;
      }

    संरक्षित:
      अक्षर*my_label_;
      vvp_array_t array_;
      अक्षर*name_;
      __vpiScope*scope_;
      अचिन्हित array_addr_;
      तार्किक local_flag_;
};

वर्ग __compile_net_resolv : सार्वजनिक base_net_resolv {

    सार्वजनिक:
      explicit __compile_net_resolv(अक्षर*ref_label, vvp_array_t array,
				    __vpiScope*scope,
				    अक्षर*my_label, अक्षर*name,
				    पूर्णांक msb, पूर्णांक lsb, अचिन्हित array_addr,
				    पूर्णांक vpi_type_code, तार्किक signed_flag, तार्किक local_flag)
      : base_net_resolv(ref_label, array, scope, my_label, name, array_addr, local_flag)
      { msb_ = msb;
	lsb_ = lsb;
	vpi_type_code_ = vpi_type_code;
	signed_flag_ = signed_flag;
      }

      ~__compile_net_resolv() override { }

      तार्किक resolve(तार्किक message_flag) override;

    खाजगी:
      पूर्णांक msb_, lsb_;
      पूर्णांक vpi_type_code_;
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

स्थिर शून्य do_compile_net(vvp_net_t*गाठ, vvp_array_t array,
			   __vpiScope*scope,
			   अक्षर*my_label, अक्षर*name,
			   पूर्णांक msb, पूर्णांक lsb, अचिन्हित array_addr,
			   पूर्णांक vpi_type_code, तार्किक signed_flag, तार्किक local_flag)
{
      अचिन्हित wid = ((msb > lsb)? msb-lsb : lsb-msb) + 1;
      प्रतिज्ञा(गाठ);

      vvp_wire_base*vsig = dynamic_cast<vvp_wire_base*>(गाठ->fil);

      जर (vsig == 0) {
	    निवड (vpi_type_code) {
		स्थिती vpiIntVar:
		  vsig = नवीन vvp_wire_vec4(wid,BIT4_0);
		  थांबा;
		स्थिती vpiLogicVar:
		  vsig = नवीन vvp_wire_vec4(wid,BIT4_Z);
		  थांबा;
		स्थिती -vpiLogicVar:
		  vsig = नवीन vvp_wire_vec8(wid);
		  थांबा;
	    }
	    प्रतिज्ञा(vsig);
	    गाठ->fil = vsig;
      }

      vpiHandle obj = 0;
      जर (! local_flag) {
	      /* Make the vpiHandle for the reg. */
	    obj = vpip_make_net4(scope, name, msb, lsb, signed_flag, गाठ);
	      /* This attaches the label to the vpiHandle */
	    compile_vpi_चिन्ह(my_label, obj);
      }
#ifdef CHECK_WITH_VALGRIND
      अन्यथा pool_local_net(गाठ);
#endif

	// REMOVE ME! Giving the net a label is a legacy of the times
	// when the .net was a functor of its own. In the long run, we
	// must fix the code generator to not rely on the label of the
	// .net, then we will remove that label.
      define_functor_चिन्ह(my_label, गाठ);

      जर (array)
	    array->attach_word(array_addr, obj);
      अन्यथा जर (obj)
	    vpip_attach_to_scope(scope,obj);

      मुक्त(my_label);
      delete[] name;
}

स्थिर शून्य __compile_net(अक्षर*label,
			  अक्षर*name, अक्षर*array_label, अचिन्हित दीर्घ array_addr,
			  पूर्णांक msb, पूर्णांक lsb,
			  पूर्णांक vpi_type_code, तार्किक signed_flag, तार्किक local_flag,
			  अचिन्हित argc, संरचना symb_s*argv)
{
      vvp_array_t array = array_label? array_find(array_label) : रिक्त;
      प्रतिज्ञा(array_label ? array!=0 : सत्य);

      मुक्त(array_label);

      प्रतिज्ञा(argc == 1);
      vvp_net_t*गाठ = vvp_net_lookup(argv[0].text);
#जर 1
      जर (गाठ == 0) {
	      /* No existing net, but the string value may be a
		 constant. In that case, we will wind up generating a
		 bufz node that can carry the constant value.

	         NOTE: This is a hack! The code generator should be
	         fixed so that this is no longer needed. */
	    गाठ = create_constant_गाठ(argv[0].text);
      }
#endif
      जर (गाठ == 0) {
	    __vpiScope*scope = vpip_peek_current_scope();
	    __compile_net_resolv*res
		  = नवीन __compile_net_resolv(argv[0].text,
					     array, scope, label, name,
					     msb, lsb, array_addr,
					     vpi_type_code, signed_flag, local_flag);
	    resolv_submit(res);
	    मुक्त(argv);
	    परत;
      }
      प्रतिज्ञा(गाठ);

      __vpiScope*scope = vpip_peek_current_scope();
      do_compile_net(गाठ, array, scope, label, name, msb, lsb, array_addr,
		     vpi_type_code, signed_flag, local_flag);

      मुक्त(argv[0].text);
      मुक्त(argv);
}

तार्किक __compile_net_resolv::resolve(तार्किक संदेश_flag)
{
      vvp_net_t*गाठ = vvp_net_lookup(label());
      जर (गाठ == 0) {
	    जर (संदेश_flag)
		  cerr << "Unable to resolve label " << label() << endl;
	    परत असत्य;
      }

      do_compile_net(गाठ, array_, scope_, my_label_, name_, msb_, lsb_, array_addr_, vpi_type_code_, signed_flag_, local_flag_);
      परत सत्य;
}

शून्य compile_net(अक्षर*label, अक्षर*name, पूर्णांक msb, पूर्णांक lsb,
		 पूर्णांक vpi_type_code, तार्किक signed_flag, तार्किक local_flag,
		 अचिन्हित argc, संरचना symb_s*argv)
{
      __compile_net(label, name, 0, 0, msb, lsb,
		    vpi_type_code, signed_flag, local_flag,
		    argc, argv);
}

शून्य compile_netw(अक्षर*label, अक्षर*array_label, अचिन्हित दीर्घ array_addr,
		  पूर्णांक msb, पूर्णांक lsb, पूर्णांक vpi_type_code, तार्किक signed_flag,
		  अचिन्हित argc, संरचना symb_s*argv)
{
      __compile_net(label, 0, array_label, array_addr,
		    msb, lsb, vpi_type_code, signed_flag, असत्य,
		    argc, argv);
}

वर्ग __compile_real_net_resolv : सार्वजनिक base_net_resolv {

    सार्वजनिक:
      explicit __compile_real_net_resolv(अक्षर*ref_label, vvp_array_t array,
					 __vpiScope*scope,
					 अक्षर*my_label, अक्षर*name,
					 अचिन्हित array_addr, तार्किक local_flag)
      : base_net_resolv(ref_label, array, scope, my_label, name, array_addr, local_flag)
      {
      }

      ~__compile_real_net_resolv() override { }

      तार्किक resolve(तार्किक message_flag) override;

    खाजगी:
};

स्थिर शून्य __compile_real_net2(vvp_net_t*गाठ, vvp_array_t array,
				__vpiScope*scope,
				अक्षर*my_label, अक्षर*name,
				अचिन्हित array_addr, तार्किक local_flag)
{
      vvp_wire_base*fil = dynamic_cast<vvp_wire_base*> (गाठ->fil);

      जर (fil == 0) {
	    fil = नवीन vvp_wire_real;
	    गाठ->fil = fil;
      }

      vpiHandle obj = 0;
      जर (!local_flag) {
	    obj = vpip_make_real_net(scope, name, गाठ);
	    compile_vpi_चिन्ह(my_label, obj);
      }
#ifdef CHECK_WITH_VALGRIND
      अन्यथा pool_local_net(गाठ);
#endif

	// REMOVE ME! Giving the net a label is a legacy of the times
	// when the .net was a functor of its own. In the long run, we
	// must fix the code generator to not rely on the label of the
	// .net, then we will remove that label.
      define_functor_चिन्ह(my_label, गाठ);

     जर (array)
	    array->attach_word(array_addr, obj);
      अन्यथा जर (obj)
	    vpip_attach_to_scope(scope, obj);

      मुक्त(my_label);
      delete[] name;
}

स्थिर शून्य __compile_real(अक्षर*label, अक्षर*name,
                           अक्षर*array_label, अचिन्हित दीर्घ array_addr,
                           पूर्णांक msb, पूर्णांक lsb, तार्किक local_flag,
                           अचिन्हित argc, संरचना symb_s*argv)
{
      प्रतिज्ञा(msb == 0 && lsb == 0);
      vvp_array_t array = array_label ? array_find(array_label) : रिक्त;
      प्रतिज्ञा(array_label ? array!=0 : सत्य);

      मुक्त(array_label);

      प्रतिज्ञा(argc == 1);
      vvp_net_t*गाठ = vvp_net_lookup(argv[0].text);
      जर (गाठ == 0) {
	      /* No existing net, but the string value may be a
		 constant. In that case, we will wind up generating a
		 bufz node that can carry the constant value. */
	    गाठ = create_constant_गाठ(argv[0].text);
      }
      __vpiScope*scope = vpip_peek_current_scope();
      जर (गाठ == 0) {
	    __compile_real_net_resolv*res
		  = नवीन __compile_real_net_resolv(argv[0].text, array,
						  scope, label, name,
						  array_addr, local_flag);
	    resolv_submit(res);
	    मुक्त(argv);
	    परत;
      }

      प्रतिज्ञा(गाठ);
      __compile_real_net2(गाठ, array, scope, label, name, array_addr,
                          local_flag);
      मुक्त(argv[0].text);
      मुक्त(argv);
}

तार्किक __compile_real_net_resolv::resolve(तार्किक संदेश_flag)
{
      vvp_net_t*गाठ = vvp_net_lookup(label());
      जर (गाठ == 0) {
	    जर (संदेश_flag)
		  cerr << "Unable to resolve label " << label() << endl;
	    परत असत्य;
      }

      __compile_real_net2(गाठ, array_, scope_, my_label_, name_, array_addr_, local_flag_);
      परत सत्य;
}

शून्य compile_net_real(अक्षर*label, अक्षर*name, पूर्णांक msb, पूर्णांक lsb, तार्किक local_flag,
		      अचिन्हित argc, संरचना symb_s*argv)
{
      __compile_real(label, name, 0, 0,
                     msb, lsb, local_flag, argc, argv);
}

शून्य compile_netw_real(अक्षर*label, अक्षर*array_label, अचिन्हित दीर्घ array_addr,
                       पूर्णांक msb, पूर्णांक lsb,
                       अचिन्हित argc, संरचना symb_s*argv)
{
      __compile_real(label, 0, array_label, array_addr,
                     msb, lsb, असत्य, argc, argv);
}
