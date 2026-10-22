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
#నిర్వచితమైతే CHECK_WITH_VALGRIND
# include  "vvp_cleanup.h"
#అయితే_ముగింపు
# include  <cstdio>
# include  <cstdlib>
# include  <cstring>
# include  <iostream>
# include  <cassert>

ఉపయోగించి నామాకాశ std;

స్థిర శూన్య __compile_var_real(అక్షరం*label, అక్షరం*name,
			       vvp_array_t array, గుర్తులేని దీర్ఘ array_addr,
			       తార్కిక local_flag)
{
      vvp_net_t*net = కొత్త vvp_net_t;

      ఒకవేళ (vpip_peek_current_scope()->is_automatic()) {
	    vvp_fun_signal_real_aa*tmp = కొత్త vvp_fun_signal_real_aa;
	    net->fil = tmp;
	    net->fun = tmp;
      } లేకపోతే {
	    net->fil = కొత్త vvp_wire_real;
	    net->fun = కొత్త vvp_fun_signal_real_sa;
      }

      define_functor_చిహ్నం(label, net);

      vpiHandle obj = vpip_make_real_var(name, net);
      compile_vpi_చిహ్నం(label, obj);

      ఒకవేళ (name) {
	    ప్రతిజ్ఞా(!array);
	    ఒకవేళ (!local_flag)
		  vpip_attach_గమ్యం_current_scope(obj);
            ఒకవేళ (!vpip_peek_current_scope()->is_automatic())
                  కాలపట్టిక_init_సదిశ(vvp_net_ptr_t(net,0), 0.0);
      }
      ఒకవేళ (array) {
	    ప్రతిజ్ఞా(!name);
	    array->attach_మాట(array_addr, obj);
      }
      విడుదల(label);
      తొలగించు[] name;
}

శూన్య compile_var_real(అక్షరం*label, అక్షరం*name, తార్కిక local_flag)
{
      __compile_var_real(label, name, 0, 0, local_flag);
}

శూన్య compile_var_string(అక్షరం*label, అక్షరం*name)
{
      vvp_net_t*net = కొత్త vvp_net_t;

      ఒకవేళ (vpip_peek_current_scope()->is_automatic()) {
	    vvp_fun_signal_string_aa*tmp = కొత్త vvp_fun_signal_string_aa;
	    net->fil = tmp;
	    net->fun = tmp;
      } లేకపోతే {
	    net->fil = 0;
	    net->fun = కొత్త vvp_fun_signal_string_sa;
      }

      define_functor_చిహ్నం(label, net);

      vpiHandle obj = vpip_make_string_var(name, net);
      compile_vpi_చిహ్నం(label, obj);

      vpip_attach_గమ్యం_current_scope(obj);
      విడుదల(label);
      తొలగించు[] name;
}

శూన్య compile_var_darray(అక్షరం*label, అక్షరం*name, గుర్తులేని size)
{
      vvp_net_t*net = కొత్త vvp_net_t;

      ఒకవేళ (vpip_peek_current_scope()->is_automatic()) {
	    vvp_fun_signal_object_aa*tmp = కొత్త vvp_fun_signal_object_aa(size);
	    net->fil = tmp;
	    net->fun = tmp;
      } లేకపోతే {
	    net->fil = 0;
	    net->fun = కొత్త vvp_fun_signal_object_sa(size);
      }

      define_functor_చిహ్నం(label, net);

      vpiHandle obj = vpip_make_darray_var(name, net);
      compile_vpi_చిహ్నం(label, obj);

      vpip_attach_గమ్యం_current_scope(obj);
      విడుదల(label);
      తొలగించు[] name;
}

శూన్య compile_var_వరుస(అక్షరం*label, అక్షరం*name, గుర్తులేని size)
{
      vvp_net_t*net = కొత్త vvp_net_t;

      ఒకవేళ (vpip_peek_current_scope()->is_automatic()) {
	    vvp_fun_signal_object_aa*tmp = కొత్త vvp_fun_signal_object_aa(size);
	    net->fil = tmp;
	    net->fun = tmp;
      } లేకపోతే {
	    net->fil = 0;
	    net->fun = కొత్త vvp_fun_signal_object_sa(size);
      }

      define_functor_చిహ్నం(label, net);

      vpiHandle obj = vpip_make_వరుస_var(name, net);
      compile_vpi_చిహ్నం(label, obj);

      vpip_attach_గమ్యం_current_scope(obj);
      విడుదల(label);
      తొలగించు[] name;
}

శూన్య compile_var_cobject(అక్షరం*label, అక్షరం*name)
{
      vvp_net_t*net = కొత్త vvp_net_t;

      ఒకవేళ (vpip_peek_current_scope()->is_automatic()) {
	    vvp_fun_signal_object_aa*tmp = కొత్త vvp_fun_signal_object_aa(1);
	    net->fil = tmp;
	    net->fun = tmp;
      } లేకపోతే {
	    net->fil = 0;
	    net->fun = కొత్త vvp_fun_signal_object_sa(1);
      }

      define_functor_చిహ్నం(label, net);

      vpiHandle obj = vpip_make_cobject_var(name, net);
      compile_vpi_చిహ్నం(label, obj);

      vpip_attach_గమ్యం_current_scope(obj);
      విడుదల(label);
      తొలగించు[] name;
}

/*
 * A variable is a special functor, so we allocate that functor and
 * write the label into the symbol table.
 */
శూన్య compile_variable(అక్షరం*label, అక్షరం*name,
		      పూర్ణాంకం msb, పూర్ణాంకం lsb, పూర్ణాంకం vpi_type_సంకేతం,
		      తార్కిక signed_flag, తార్కిక local_flag)
{
      గుర్తులేని wid = ((msb > lsb)? msb-lsb : lsb-msb) + 1;

      vvp_net_t*net = కొత్త vvp_net_t;

      vvp_bit4_t init;
      ఒకవేళ (vpi_type_సంకేతం == vpiIntVar)
	    init = BIT4_0;
      లేకపోతే
	    init = BIT4_X;

      ఒకవేళ (vpip_peek_current_scope()->is_automatic()) {
	    vvp_fun_signal4_aa*tmp = కొత్త vvp_fun_signal4_aa(wid, init);
	    net->fil = tmp;
            net->fun = tmp;
      } లేకపోతే {
	    net->fil = కొత్త vvp_wire_vec4(wid, init);
            net->fun = కొత్త vvp_fun_signal4_sa(wid);
      }
      స్థిరరాశి vvp_signal_విలువ*vfil = dynamic_cast<vvp_signal_విలువ*>(net->fil);

      define_functor_చిహ్నం(label, net);

      vpiHandle obj = 0;
      ఒకవేళ (! local_flag) {
	      /* Make the vpiHandle for the reg. */
	    ఎంపిక (vpi_type_సంకేతం) {
		స్థితి vpiLogicVar:
		  obj = vpip_make_var4(name, msb, lsb, signed_flag, net);
		  ఆపు;
		స్థితి vpiIntegerVar:
		  obj = vpip_make_int4(name, msb, lsb, net);
		  ఆపు;
		స్థితి vpiIntVar: // This handles all the atom2 int types
		  obj = vpip_make_int2(name, msb, lsb, signed_flag, net);
		  ఆపు;
		అప్రమేయ:
		  fprintf(stderr, "internal error: %s: vpi_type_code=%d\n", name, vpi_type_సంకేతం);
		  ఆపు;
	    }
	    ప్రతిజ్ఞా(obj);
	    compile_vpi_చిహ్నం(label, obj);
      }
	// If the signal has a name, then it goes into the current
	// scope as a signal.
      ఒకవేళ (name) {
	    ఒకవేళ (obj) vpip_attach_గమ్యం_current_scope(obj);
            ఒకవేళ (!vpip_peek_current_scope()->is_automatic()) {
		  vvp_vector4_t tmp;
		  vfil->vec4_విలువ(tmp);
	          కాలపట్టిక_init_సదిశ(vvp_net_ptr_t(net,0), tmp);
	    }
      }

      విడుదల(label);
      తొలగించు[] name;
}


vvp_net_t* create_constant_కణుపు(స్థిరరాశి అక్షరం*val_str)
{
      ఒకవేళ (c4string_test(val_str)) {
	    vvp_net_t*net = కొత్త vvp_net_t;
	    net->fun = కొత్త vvp_fun_bufz;
	    కాలపట్టిక_init_సదిశ(vvp_net_ptr_t(net,0), c4string_గమ్యం_vector4(val_str));
	    తిరిగి net;
      }

      ఒకవేళ (c8string_test(val_str)) {
	    vvp_net_t*net = కొత్త vvp_net_t;
	    net->fun = కొత్త vvp_fun_bufz;
	    కాలపట్టిక_init_సదిశ(vvp_net_ptr_t(net,0), c8string_గమ్యం_vector8(val_str));
	    తిరిగి net;
      }

      ఒకవేళ (crstring_test(val_str)) {
	    vvp_net_t*net = కొత్త vvp_net_t;
	    net->fun = కొత్త vvp_fun_bufz;
	    కాలపట్టిక_init_సదిశ(vvp_net_ptr_t(net,0), crstring_గమ్యం_double(val_str));
	    తిరిగి net;
      }

      తిరిగి 0;
}

తరగతి పునాది_net_resolv : బహిరంగ resolv_list_s {
    బహిరంగ:
      స్పష్టమైన పునాది_net_resolv(అక్షరం*ref_label, vvp_array_t array,
			       __vpiScope*scope,
			       అక్షరం*my_label, అక్షరం*name,
			       గుర్తులేని array_addr, తార్కిక local_flag)
      : resolv_list_s(ref_label)
      { my_label_ = my_label;
	array_ = array;
	name_ = name;
	scope_ = scope;
	array_addr_ = array_addr;
	local_flag_ = local_flag;
      }

    రక్షిత:
      అక్షరం*my_label_;
      vvp_array_t array_;
      అక్షరం*name_;
      __vpiScope*scope_;
      గుర్తులేని array_addr_;
      తార్కిక local_flag_;
};

తరగతి __compile_net_resolv : బహిరంగ పునాది_net_resolv {

    బహిరంగ:
      స్పష్టమైన __compile_net_resolv(అక్షరం*ref_label, vvp_array_t array,
				    __vpiScope*scope,
				    అక్షరం*my_label, అక్షరం*name,
				    పూర్ణాంకం msb, పూర్ణాంకం lsb, గుర్తులేని array_addr,
				    పూర్ణాంకం vpi_type_సంకేతం, తార్కిక signed_flag, తార్కిక local_flag)
      : పునాది_net_resolv(ref_label, array, scope, my_label, name, array_addr, local_flag)
      { msb_ = msb;
	lsb_ = lsb;
	vpi_type_సంకేతం_2 = vpi_type_సంకేతం;
	signed_flag_ = signed_flag;
      }

      ~__compile_net_resolv() override { }

      తార్కిక resolve(తార్కిక సందేశం_flag) override;

    అంతరంగ:
      పూర్ణాంకం msb_, lsb_;
      పూర్ణాంకం vpi_type_సంకేతం_2;
      తార్కిక signed_flag_;
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

స్థిర శూన్య do_compile_net(vvp_net_t*కణుపు, vvp_array_t array,
			   __vpiScope*scope,
			   అక్షరం*my_label, అక్షరం*name,
			   పూర్ణాంకం msb, పూర్ణాంకం lsb, గుర్తులేని array_addr,
			   పూర్ణాంకం vpi_type_సంకేతం, తార్కిక signed_flag, తార్కిక local_flag)
{
      గుర్తులేని wid = ((msb > lsb)? msb-lsb : lsb-msb) + 1;
      ప్రతిజ్ఞా(కణుపు);

      vvp_wire_పునాది*vsig = dynamic_cast<vvp_wire_పునాది*>(కణుపు->fil);

      ఒకవేళ (vsig == 0) {
	    ఎంపిక (vpi_type_సంకేతం) {
		స్థితి vpiIntVar:
		  vsig = కొత్త vvp_wire_vec4(wid,BIT4_0);
		  ఆపు;
		స్థితి vpiLogicVar:
		  vsig = కొత్త vvp_wire_vec4(wid,BIT4_Z);
		  ఆపు;
		స్థితి -vpiLogicVar:
		  vsig = కొత్త vvp_wire_vec8(wid);
		  ఆపు;
	    }
	    ప్రతిజ్ఞా(vsig);
	    కణుపు->fil = vsig;
      }

      vpiHandle obj = 0;
      ఒకవేళ (! local_flag) {
	      /* Make the vpiHandle for the reg. */
	    obj = vpip_make_net4(scope, name, msb, lsb, signed_flag, కణుపు);
	      /* This attaches the label to the vpiHandle */
	    compile_vpi_చిహ్నం(my_label, obj);
      }
#నిర్వచితమైతే CHECK_WITH_VALGRIND
      లేకపోతే pool_local_net(కణుపు);
#అయితే_ముగింపు

	// REMOVE ME! Giving the net a label is a legacy of the times
	// when the .net was a functor of its own. In the long run, we
	// must fix the code generator to not rely on the label of the
	// .net, then we will remove that label.
      define_functor_చిహ్నం(my_label, కణుపు);

      ఒకవేళ (array)
	    array->attach_మాట(array_addr, obj);
      లేకపోతే ఒకవేళ (obj)
	    vpip_attach_గమ్యం_scope(scope,obj);

      విడుదల(my_label);
      తొలగించు[] name;
}

స్థిర శూన్య __compile_net(అక్షరం*label,
			  అక్షరం*name, అక్షరం*array_label, గుర్తులేని దీర్ఘ array_addr,
			  పూర్ణాంకం msb, పూర్ణాంకం lsb,
			  పూర్ణాంకం vpi_type_సంకేతం, తార్కిక signed_flag, తార్కిక local_flag,
			  గుర్తులేని argc, నిర్మాణం symb_s*argv)
{
      vvp_array_t array = array_label? array_కనుగొను(array_label) : ఖాళీ;
      ప్రతిజ్ఞా(array_label ? array!=0 : నిజం);

      విడుదల(array_label);

      ప్రతిజ్ఞా(argc == 1);
      vvp_net_t*కణుపు = vvp_net_lookup(argv[0].పాఠ్యం);
#ఒకవేళ 1
      ఒకవేళ (కణుపు == 0) {
	      /* No existing net, but the string value may be a
		 constant. In that case, we will wind up generating a
		 bufz node that can carry the constant value.

	         NOTE: This is a hack! The code generator should be
	         fixed so that this is no longer needed. */
	    కణుపు = create_constant_కణుపు(argv[0].పాఠ్యం);
      }
#అయితే_ముగింపు
      ఒకవేళ (కణుపు == 0) {
	    __vpiScope*scope = vpip_peek_current_scope();
	    __compile_net_resolv*res
		  = కొత్త __compile_net_resolv(argv[0].పాఠ్యం,
					     array, scope, label, name,
					     msb, lsb, array_addr,
					     vpi_type_సంకేతం, signed_flag, local_flag);
	    resolv_submit(res);
	    విడుదల(argv);
	    తిరిగి;
      }
      ప్రతిజ్ఞా(కణుపు);

      __vpiScope*scope = vpip_peek_current_scope();
      do_compile_net(కణుపు, array, scope, label, name, msb, lsb, array_addr,
		     vpi_type_సంకేతం, signed_flag, local_flag);

      విడుదల(argv[0].పాఠ్యం);
      విడుదల(argv);
}

తార్కిక __compile_net_resolv::resolve(తార్కిక సందేశం_flag_2)
{
      vvp_net_t*కణుపు = vvp_net_lookup(label());
      ఒకవేళ (కణుపు == 0) {
	    ఒకవేళ (సందేశం_flag_2)
		  cerr << "Unable to resolve label " << label() << పంక్తి_ముగింపు;
	    తిరిగి అబద్ధం;
      }

      do_compile_net(కణుపు, array_, scope_, my_label_, name_, msb_, lsb_, array_addr_, vpi_type_సంకేతం_2, signed_flag_, local_flag_);
      తిరిగి నిజం;
}

శూన్య compile_net(అక్షరం*label, అక్షరం*name, పూర్ణాంకం msb, పూర్ణాంకం lsb,
		 పూర్ణాంకం vpi_type_సంకేతం, తార్కిక signed_flag, తార్కిక local_flag,
		 గుర్తులేని argc, నిర్మాణం symb_s*argv)
{
      __compile_net(label, name, 0, 0, msb, lsb,
		    vpi_type_సంకేతం, signed_flag, local_flag,
		    argc, argv);
}

శూన్య compile_netw(అక్షరం*label, అక్షరం*array_label, గుర్తులేని దీర్ఘ array_addr,
		  పూర్ణాంకం msb, పూర్ణాంకం lsb, పూర్ణాంకం vpi_type_సంకేతం, తార్కిక signed_flag,
		  గుర్తులేని argc, నిర్మాణం symb_s*argv)
{
      __compile_net(label, 0, array_label, array_addr,
		    msb, lsb, vpi_type_సంకేతం, signed_flag, అబద్ధం,
		    argc, argv);
}

తరగతి __compile_real_net_resolv : బహిరంగ పునాది_net_resolv {

    బహిరంగ:
      స్పష్టమైన __compile_real_net_resolv(అక్షరం*ref_label, vvp_array_t array,
					 __vpiScope*scope,
					 అక్షరం*my_label, అక్షరం*name,
					 గుర్తులేని array_addr, తార్కిక local_flag)
      : పునాది_net_resolv(ref_label, array, scope, my_label, name, array_addr, local_flag)
      {
      }

      ~__compile_real_net_resolv() override { }

      తార్కిక resolve(తార్కిక సందేశం_flag) override;

    అంతరంగ:
};

స్థిర శూన్య __compile_real_net2(vvp_net_t*కణుపు, vvp_array_t array,
				__vpiScope*scope,
				అక్షరం*my_label, అక్షరం*name,
				గుర్తులేని array_addr, తార్కిక local_flag)
{
      vvp_wire_పునాది*fil = dynamic_cast<vvp_wire_పునాది*> (కణుపు->fil);

      ఒకవేళ (fil == 0) {
	    fil = కొత్త vvp_wire_real;
	    కణుపు->fil = fil;
      }

      vpiHandle obj = 0;
      ఒకవేళ (!local_flag) {
	    obj = vpip_make_real_net(scope, name, కణుపు);
	    compile_vpi_చిహ్నం(my_label, obj);
      }
#నిర్వచితమైతే CHECK_WITH_VALGRIND
      లేకపోతే pool_local_net(కణుపు);
#అయితే_ముగింపు

	// REMOVE ME! Giving the net a label is a legacy of the times
	// when the .net was a functor of its own. In the long run, we
	// must fix the code generator to not rely on the label of the
	// .net, then we will remove that label.
      define_functor_చిహ్నం(my_label, కణుపు);

     ఒకవేళ (array)
	    array->attach_మాట(array_addr, obj);
      లేకపోతే ఒకవేళ (obj)
	    vpip_attach_గమ్యం_scope(scope, obj);

      విడుదల(my_label);
      తొలగించు[] name;
}

స్థిర శూన్య __compile_real(అక్షరం*label, అక్షరం*name,
                           అక్షరం*array_label, గుర్తులేని దీర్ఘ array_addr,
                           పూర్ణాంకం msb, పూర్ణాంకం lsb, తార్కిక local_flag,
                           గుర్తులేని argc, నిర్మాణం symb_s*argv)
{
      ప్రతిజ్ఞా(msb == 0 && lsb == 0);
      vvp_array_t array = array_label ? array_కనుగొను(array_label) : ఖాళీ;
      ప్రతిజ్ఞా(array_label ? array!=0 : నిజం);

      విడుదల(array_label);

      ప్రతిజ్ఞా(argc == 1);
      vvp_net_t*కణుపు = vvp_net_lookup(argv[0].పాఠ్యం);
      ఒకవేళ (కణుపు == 0) {
	      /* No existing net, but the string value may be a
		 constant. In that case, we will wind up generating a
		 bufz node that can carry the constant value. */
	    కణుపు = create_constant_కణుపు(argv[0].పాఠ్యం);
      }
      __vpiScope*scope = vpip_peek_current_scope();
      ఒకవేళ (కణుపు == 0) {
	    __compile_real_net_resolv*res
		  = కొత్త __compile_real_net_resolv(argv[0].పాఠ్యం, array,
						  scope, label, name,
						  array_addr, local_flag);
	    resolv_submit(res);
	    విడుదల(argv);
	    తిరిగి;
      }

      ప్రతిజ్ఞా(కణుపు);
      __compile_real_net2(కణుపు, array, scope, label, name, array_addr,
                          local_flag);
      విడుదల(argv[0].పాఠ్యం);
      విడుదల(argv);
}

తార్కిక __compile_real_net_resolv::resolve(తార్కిక సందేశం_flag_2)
{
      vvp_net_t*కణుపు = vvp_net_lookup(label());
      ఒకవేళ (కణుపు == 0) {
	    ఒకవేళ (సందేశం_flag_2)
		  cerr << "Unable to resolve label " << label() << పంక్తి_ముగింపు;
	    తిరిగి అబద్ధం;
      }

      __compile_real_net2(కణుపు, array_, scope_, my_label_, name_, array_addr_, local_flag_);
      తిరిగి నిజం;
}

శూన్య compile_net_real(అక్షరం*label, అక్షరం*name, పూర్ణాంకం msb, పూర్ణాంకం lsb, తార్కిక local_flag,
		      గుర్తులేని argc, నిర్మాణం symb_s*argv)
{
      __compile_real(label, name, 0, 0,
                     msb, lsb, local_flag, argc, argv);
}

శూన్య compile_netw_real(అక్షరం*label, అక్షరం*array_label, గుర్తులేని దీర్ఘ array_addr,
                       పూర్ణాంకం msb, పూర్ణాంకం lsb,
                       గుర్తులేని argc, నిర్మాణం symb_s*argv)
{
      __compile_real(label, 0, array_label, array_addr,
                     msb, lsb, అబద్ధం, argc, argv);
}
