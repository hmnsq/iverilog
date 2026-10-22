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
#अगर_घोषित CHECK_WITH_VALGRIND
# include  "vvp_cleanup.h"
#पूर्ण_अगर
# include  <cstdio>
# include  <cstdlib>
# include  <cstring>
# include  <iostream>
# include  <cassert>

using namespace std;

अटल व्योम __compile_var_real(अक्षर*label, अक्षर*name,
			       vvp_array_t array, अचिन्हित दीर्घ array_addr,
			       bool local_flag)
{
      vvp_net_t*net = नया vvp_net_t;

      अगर (vpip_peek_current_scope()->is_automatic()) {
	    vvp_fun_signal_real_aa*tmp = नया vvp_fun_signal_real_aa;
	    net->fil = tmp;
	    net->fun = tmp;
      } अन्यथा {
	    net->fil = नया vvp_wire_real;
	    net->fun = नया vvp_fun_signal_real_sa;
      }

      define_functor_प्रतीक(label, net);

      vpiHandle obj = vpip_make_real_var(name, net);
      compile_vpi_प्रतीक(label, obj);

      अगर (name) {
	    निश्चित(!array);
	    अगर (!local_flag)
		  vpip_attach_to_current_scope(obj);
            अगर (!vpip_peek_current_scope()->is_automatic())
                  schedule_init_vector(vvp_net_ptr_t(net,0), 0.0);
      }
      अगर (array) {
	    निश्चित(!name);
	    array->attach_word(array_addr, obj);
      }
      मुक्त(label);
      मिटाओ[] name;
}

व्योम compile_var_real(अक्षर*label, अक्षर*name, bool local_flag)
{
      __compile_var_real(label, name, 0, 0, local_flag);
}

व्योम compile_var_string(अक्षर*label, अक्षर*name)
{
      vvp_net_t*net = नया vvp_net_t;

      अगर (vpip_peek_current_scope()->is_automatic()) {
	    vvp_fun_signal_string_aa*tmp = नया vvp_fun_signal_string_aa;
	    net->fil = tmp;
	    net->fun = tmp;
      } अन्यथा {
	    net->fil = 0;
	    net->fun = नया vvp_fun_signal_string_sa;
      }

      define_functor_प्रतीक(label, net);

      vpiHandle obj = vpip_make_string_var(name, net);
      compile_vpi_प्रतीक(label, obj);

      vpip_attach_to_current_scope(obj);
      मुक्त(label);
      मिटाओ[] name;
}

व्योम compile_var_darray(अक्षर*label, अक्षर*name, अचिन्हित size)
{
      vvp_net_t*net = नया vvp_net_t;

      अगर (vpip_peek_current_scope()->is_automatic()) {
	    vvp_fun_signal_object_aa*tmp = नया vvp_fun_signal_object_aa(size);
	    net->fil = tmp;
	    net->fun = tmp;
      } अन्यथा {
	    net->fil = 0;
	    net->fun = नया vvp_fun_signal_object_sa(size);
      }

      define_functor_प्रतीक(label, net);

      vpiHandle obj = vpip_make_darray_var(name, net);
      compile_vpi_प्रतीक(label, obj);

      vpip_attach_to_current_scope(obj);
      मुक्त(label);
      मिटाओ[] name;
}

व्योम compile_var_queue(अक्षर*label, अक्षर*name, अचिन्हित size)
{
      vvp_net_t*net = नया vvp_net_t;

      अगर (vpip_peek_current_scope()->is_automatic()) {
	    vvp_fun_signal_object_aa*tmp = नया vvp_fun_signal_object_aa(size);
	    net->fil = tmp;
	    net->fun = tmp;
      } अन्यथा {
	    net->fil = 0;
	    net->fun = नया vvp_fun_signal_object_sa(size);
      }

      define_functor_प्रतीक(label, net);

      vpiHandle obj = vpip_make_queue_var(name, net);
      compile_vpi_प्रतीक(label, obj);

      vpip_attach_to_current_scope(obj);
      मुक्त(label);
      मिटाओ[] name;
}

व्योम compile_var_cobject(अक्षर*label, अक्षर*name)
{
      vvp_net_t*net = नया vvp_net_t;

      अगर (vpip_peek_current_scope()->is_automatic()) {
	    vvp_fun_signal_object_aa*tmp = नया vvp_fun_signal_object_aa(1);
	    net->fil = tmp;
	    net->fun = tmp;
      } अन्यथा {
	    net->fil = 0;
	    net->fun = नया vvp_fun_signal_object_sa(1);
      }

      define_functor_प्रतीक(label, net);

      vpiHandle obj = vpip_make_cobject_var(name, net);
      compile_vpi_प्रतीक(label, obj);

      vpip_attach_to_current_scope(obj);
      मुक्त(label);
      मिटाओ[] name;
}

/*
 * A variable is a special functor, so we allocate that functor and
 * write the label into the symbol table.
 */
व्योम compile_variable(अक्षर*label, अक्षर*name,
		      पूर्णांक msb, पूर्णांक lsb, पूर्णांक vpi_type_code,
		      bool signed_flag, bool local_flag)
{
      अचिन्हित wid = ((msb > lsb)? msb-lsb : lsb-msb) + 1;

      vvp_net_t*net = नया vvp_net_t;

      vvp_bit4_t init;
      अगर (vpi_type_code == vpiIntVar)
	    init = BIT4_0;
      अन्यथा
	    init = BIT4_X;

      अगर (vpip_peek_current_scope()->is_automatic()) {
	    vvp_fun_signal4_aa*tmp = नया vvp_fun_signal4_aa(wid, init);
	    net->fil = tmp;
            net->fun = tmp;
      } अन्यथा {
	    net->fil = नया vvp_wire_vec4(wid, init);
            net->fun = नया vvp_fun_signal4_sa(wid);
      }
      स्थिर vvp_signal_मूल्य*vfil = dynamic_cast<vvp_signal_मूल्य*>(net->fil);

      define_functor_प्रतीक(label, net);

      vpiHandle obj = 0;
      अगर (! local_flag) {
	      /* Make the vpiHandle for the reg. */
	    चयन (vpi_type_code) {
		हाल vpiLogicVar:
		  obj = vpip_make_var4(name, msb, lsb, signed_flag, net);
		  अवरोध;
		हाल vpiIntegerVar:
		  obj = vpip_make_int4(name, msb, lsb, net);
		  अवरोध;
		हाल vpiIntVar: // This handles all the atom2 int types
		  obj = vpip_make_int2(name, msb, lsb, signed_flag, net);
		  अवरोध;
		शेष:
		  ख_लिखो(मानक_त्रुटि, "internal error: %s: vpi_type_code=%d\n", name, vpi_type_code);
		  अवरोध;
	    }
	    निश्चित(obj);
	    compile_vpi_प्रतीक(label, obj);
      }
	// If the signal has a name, then it goes into the current
	// scope as a signal.
      अगर (name) {
	    अगर (obj) vpip_attach_to_current_scope(obj);
            अगर (!vpip_peek_current_scope()->is_automatic()) {
		  vvp_vector4_t tmp;
		  vfil->vec4_मूल्य(tmp);
	          schedule_init_vector(vvp_net_ptr_t(net,0), tmp);
	    }
      }

      मुक्त(label);
      मिटाओ[] name;
}


vvp_net_t* create_constant_गाँठ(स्थिर अक्षर*val_str)
{
      अगर (c4string_test(val_str)) {
	    vvp_net_t*net = नया vvp_net_t;
	    net->fun = नया vvp_fun_bufz;
	    schedule_init_vector(vvp_net_ptr_t(net,0), c4string_to_vector4(val_str));
	    वापस net;
      }

      अगर (c8string_test(val_str)) {
	    vvp_net_t*net = नया vvp_net_t;
	    net->fun = नया vvp_fun_bufz;
	    schedule_init_vector(vvp_net_ptr_t(net,0), c8string_to_vector8(val_str));
	    वापस net;
      }

      अगर (crstring_test(val_str)) {
	    vvp_net_t*net = नया vvp_net_t;
	    net->fun = नया vvp_fun_bufz;
	    schedule_init_vector(vvp_net_ptr_t(net,0), crstring_to_double(val_str));
	    वापस net;
      }

      वापस 0;
}

श्रेणी base_net_resolv : खुला resolv_list_s {
    खुला:
      explicit base_net_resolv(अक्षर*ref_label, vvp_array_t array,
			       __vpiScope*scope,
			       अक्षर*my_label, अक्षर*name,
			       अचिन्हित array_addr, bool local_flag)
      : resolv_list_s(ref_label)
      { my_label_ = my_label;
	array_ = array;
	name_ = name;
	scope_ = scope;
	array_addr_ = array_addr;
	local_flag_ = local_flag;
      }

    रक्षित:
      अक्षर*my_label_;
      vvp_array_t array_;
      अक्षर*name_;
      __vpiScope*scope_;
      अचिन्हित array_addr_;
      bool local_flag_;
};

श्रेणी __compile_net_resolv : खुला base_net_resolv {

    खुला:
      explicit __compile_net_resolv(अक्षर*ref_label, vvp_array_t array,
				    __vpiScope*scope,
				    अक्षर*my_label, अक्षर*name,
				    पूर्णांक msb, पूर्णांक lsb, अचिन्हित array_addr,
				    पूर्णांक vpi_type_code, bool signed_flag, bool local_flag)
      : base_net_resolv(ref_label, array, scope, my_label, name, array_addr, local_flag)
      { msb_ = msb;
	lsb_ = lsb;
	vpi_type_code_ = vpi_type_code;
	signed_flag_ = signed_flag;
      }

      ~__compile_net_resolv() override { }

      bool resolve(bool message_flag) override;

    निजी:
      पूर्णांक msb_, lsb_;
      पूर्णांक vpi_type_code_;
      bool signed_flag_;
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

अटल व्योम do_compile_net(vvp_net_t*गाँठ, vvp_array_t array,
			   __vpiScope*scope,
			   अक्षर*my_label, अक्षर*name,
			   पूर्णांक msb, पूर्णांक lsb, अचिन्हित array_addr,
			   पूर्णांक vpi_type_code, bool signed_flag, bool local_flag)
{
      अचिन्हित wid = ((msb > lsb)? msb-lsb : lsb-msb) + 1;
      निश्चित(गाँठ);

      vvp_wire_base*vsig = dynamic_cast<vvp_wire_base*>(गाँठ->fil);

      अगर (vsig == 0) {
	    चयन (vpi_type_code) {
		हाल vpiIntVar:
		  vsig = नया vvp_wire_vec4(wid,BIT4_0);
		  अवरोध;
		हाल vpiLogicVar:
		  vsig = नया vvp_wire_vec4(wid,BIT4_Z);
		  अवरोध;
		हाल -vpiLogicVar:
		  vsig = नया vvp_wire_vec8(wid);
		  अवरोध;
	    }
	    निश्चित(vsig);
	    गाँठ->fil = vsig;
      }

      vpiHandle obj = 0;
      अगर (! local_flag) {
	      /* Make the vpiHandle for the reg. */
	    obj = vpip_make_net4(scope, name, msb, lsb, signed_flag, गाँठ);
	      /* This attaches the label to the vpiHandle */
	    compile_vpi_प्रतीक(my_label, obj);
      }
#अगर_घोषित CHECK_WITH_VALGRIND
      अन्यथा pool_local_net(गाँठ);
#पूर्ण_अगर

	// REMOVE ME! Giving the net a label is a legacy of the times
	// when the .net was a functor of its own. In the long run, we
	// must fix the code generator to not rely on the label of the
	// .net, then we will remove that label.
      define_functor_प्रतीक(my_label, गाँठ);

      अगर (array)
	    array->attach_word(array_addr, obj);
      अन्यथा अगर (obj)
	    vpip_attach_to_scope(scope,obj);

      मुक्त(my_label);
      मिटाओ[] name;
}

अटल व्योम __compile_net(अक्षर*label,
			  अक्षर*name, अक्षर*array_label, अचिन्हित दीर्घ array_addr,
			  पूर्णांक msb, पूर्णांक lsb,
			  पूर्णांक vpi_type_code, bool signed_flag, bool local_flag,
			  अचिन्हित argc, काष्ठा symb_s*argv)
{
      vvp_array_t array = array_label? array_find(array_label) : शून्य;
      निश्चित(array_label ? array!=0 : true);

      मुक्त(array_label);

      निश्चित(argc == 1);
      vvp_net_t*गाँठ = vvp_net_lookup(argv[0].text);
#अगर 1
      अगर (गाँठ == 0) {
	      /* No existing net, but the string value may be a
		 constant. In that case, we will wind up generating a
		 bufz node that can carry the constant value.

	         NOTE: This is a hack! The code generator should be
	         fixed so that this is no longer needed. */
	    गाँठ = create_constant_गाँठ(argv[0].text);
      }
#पूर्ण_अगर
      अगर (गाँठ == 0) {
	    __vpiScope*scope = vpip_peek_current_scope();
	    __compile_net_resolv*res
		  = नया __compile_net_resolv(argv[0].text,
					     array, scope, label, name,
					     msb, lsb, array_addr,
					     vpi_type_code, signed_flag, local_flag);
	    resolv_submit(res);
	    मुक्त(argv);
	    वापस;
      }
      निश्चित(गाँठ);

      __vpiScope*scope = vpip_peek_current_scope();
      do_compile_net(गाँठ, array, scope, label, name, msb, lsb, array_addr,
		     vpi_type_code, signed_flag, local_flag);

      मुक्त(argv[0].text);
      मुक्त(argv);
}

bool __compile_net_resolv::resolve(bool संदेश_flag)
{
      vvp_net_t*गाँठ = vvp_net_lookup(label());
      अगर (गाँठ == 0) {
	    अगर (संदेश_flag)
		  अ_त्रुटि << "Unable to resolve label " << label() << प_पूर्ण;
	    वापस false;
      }

      do_compile_net(गाँठ, array_, scope_, my_label_, name_, msb_, lsb_, array_addr_, vpi_type_code_, signed_flag_, local_flag_);
      वापस true;
}

व्योम compile_net(अक्षर*label, अक्षर*name, पूर्णांक msb, पूर्णांक lsb,
		 पूर्णांक vpi_type_code, bool signed_flag, bool local_flag,
		 अचिन्हित argc, काष्ठा symb_s*argv)
{
      __compile_net(label, name, 0, 0, msb, lsb,
		    vpi_type_code, signed_flag, local_flag,
		    argc, argv);
}

व्योम compile_netw(अक्षर*label, अक्षर*array_label, अचिन्हित दीर्घ array_addr,
		  पूर्णांक msb, पूर्णांक lsb, पूर्णांक vpi_type_code, bool signed_flag,
		  अचिन्हित argc, काष्ठा symb_s*argv)
{
      __compile_net(label, 0, array_label, array_addr,
		    msb, lsb, vpi_type_code, signed_flag, false,
		    argc, argv);
}

श्रेणी __compile_real_net_resolv : खुला base_net_resolv {

    खुला:
      explicit __compile_real_net_resolv(अक्षर*ref_label, vvp_array_t array,
					 __vpiScope*scope,
					 अक्षर*my_label, अक्षर*name,
					 अचिन्हित array_addr, bool local_flag)
      : base_net_resolv(ref_label, array, scope, my_label, name, array_addr, local_flag)
      {
      }

      ~__compile_real_net_resolv() override { }

      bool resolve(bool message_flag) override;

    निजी:
};

अटल व्योम __compile_real_net2(vvp_net_t*गाँठ, vvp_array_t array,
				__vpiScope*scope,
				अक्षर*my_label, अक्षर*name,
				अचिन्हित array_addr, bool local_flag)
{
      vvp_wire_base*fil = dynamic_cast<vvp_wire_base*> (गाँठ->fil);

      अगर (fil == 0) {
	    fil = नया vvp_wire_real;
	    गाँठ->fil = fil;
      }

      vpiHandle obj = 0;
      अगर (!local_flag) {
	    obj = vpip_make_real_net(scope, name, गाँठ);
	    compile_vpi_प्रतीक(my_label, obj);
      }
#अगर_घोषित CHECK_WITH_VALGRIND
      अन्यथा pool_local_net(गाँठ);
#पूर्ण_अगर

	// REMOVE ME! Giving the net a label is a legacy of the times
	// when the .net was a functor of its own. In the long run, we
	// must fix the code generator to not rely on the label of the
	// .net, then we will remove that label.
      define_functor_प्रतीक(my_label, गाँठ);

     अगर (array)
	    array->attach_word(array_addr, obj);
      अन्यथा अगर (obj)
	    vpip_attach_to_scope(scope, obj);

      मुक्त(my_label);
      मिटाओ[] name;
}

अटल व्योम __compile_real(अक्षर*label, अक्षर*name,
                           अक्षर*array_label, अचिन्हित दीर्घ array_addr,
                           पूर्णांक msb, पूर्णांक lsb, bool local_flag,
                           अचिन्हित argc, काष्ठा symb_s*argv)
{
      निश्चित(msb == 0 && lsb == 0);
      vvp_array_t array = array_label ? array_find(array_label) : शून्य;
      निश्चित(array_label ? array!=0 : true);

      मुक्त(array_label);

      निश्चित(argc == 1);
      vvp_net_t*गाँठ = vvp_net_lookup(argv[0].text);
      अगर (गाँठ == 0) {
	      /* No existing net, but the string value may be a
		 constant. In that case, we will wind up generating a
		 bufz node that can carry the constant value. */
	    गाँठ = create_constant_गाँठ(argv[0].text);
      }
      __vpiScope*scope = vpip_peek_current_scope();
      अगर (गाँठ == 0) {
	    __compile_real_net_resolv*res
		  = नया __compile_real_net_resolv(argv[0].text, array,
						  scope, label, name,
						  array_addr, local_flag);
	    resolv_submit(res);
	    मुक्त(argv);
	    वापस;
      }

      निश्चित(गाँठ);
      __compile_real_net2(गाँठ, array, scope, label, name, array_addr,
                          local_flag);
      मुक्त(argv[0].text);
      मुक्त(argv);
}

bool __compile_real_net_resolv::resolve(bool संदेश_flag)
{
      vvp_net_t*गाँठ = vvp_net_lookup(label());
      अगर (गाँठ == 0) {
	    अगर (संदेश_flag)
		  अ_त्रुटि << "Unable to resolve label " << label() << प_पूर्ण;
	    वापस false;
      }

      __compile_real_net2(गाँठ, array_, scope_, my_label_, name_, array_addr_, local_flag_);
      वापस true;
}

व्योम compile_net_real(अक्षर*label, अक्षर*name, पूर्णांक msb, पूर्णांक lsb, bool local_flag,
		      अचिन्हित argc, काष्ठा symb_s*argv)
{
      __compile_real(label, name, 0, 0,
                     msb, lsb, local_flag, argc, argv);
}

व्योम compile_netw_real(अक्षर*label, अक्षर*array_label, अचिन्हित दीर्घ array_addr,
                       पूर्णांक msb, पूर्णांक lsb,
                       अचिन्हित argc, काष्ठा symb_s*argv)
{
      __compile_real(label, 0, array_label, array_addr,
                     msb, lsb, false, argc, argv);
}
