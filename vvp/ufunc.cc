/*
 * Copyright (c) 2002-2025 Stephen Williams (steve@icarus.com)
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

# include  "vvp_net.h"
# include  "compile.h"
# include  "symbols.h"
# include  "codes.h"
# include  "ufunc.h"
# include  "vvp_net_sig.h"
# include  "vthread.h"
# include  "schedule.h"
#నిర్వచితమైతే CHECK_WITH_VALGRIND
# include  "vvp_cleanup.h"
#అయితే_ముగింపు
# include  <cstdlib>
# include  <cstring>
# include  <iostream>
# include  <cassert>
# include  <map>

#నిర్వచితమైతే __MINGW32__
#అన్తర్భావ <windows.h>
#అయితే_ముగింపు

తరగతి ufunc_real : బహిరంగ ufunc_core {
   బహిరంగ:
      ufunc_real(గుర్తులేని ow, vvp_net_t*ptr,
		 గుర్తులేని nports, vvp_net_t**ports,
		 vvp_సంకేతం_t ప్రారంభం_చిరునామా,
		 __vpiScope*call_scope,
		 అక్షరం*scope_label);
      ~ufunc_real() override;

      శూన్య ముగింపు_thread() override;
};

తరగతి ufunc_vec4 : బహిరంగ ufunc_core {
   బహిరంగ:
      ufunc_vec4(గుర్తులేని ow, vvp_net_t*ptr,
		 గుర్తులేని nports, vvp_net_t**ports,
		 vvp_సంకేతం_t ప్రారంభం_చిరునామా,
		 __vpiScope*call_scope,
		 అక్షరం*scope_label);
      ~ufunc_vec4() override;

      శూన్య ముగింపు_thread() override;
};

ufunc_core::ufunc_core(గుర్తులేని owid, vvp_net_t*ptr,
		       గుర్తులేని nports, vvp_net_t**ports,
		       vvp_సంకేతం_t sa, __vpiScope*call_scope__,
		       అక్షరం*scope_label)
: vvp_wide_fun_core(ptr, nports)
{
      owid_ = owid;
      ports_ = ports;
      సంకేతం_3 = sa;
      thread_ = 0;
      call_scope_ = call_scope__;

	/* A __vpiScope starts with a __vpiHandle structure so this is
	   a safe cast. We need the (void*) to avoid a dereferenced
	   type punned pointer warning from some gcc compilers. */
      compile_vpi_lookup((vpiHandle*)(శూన్య*)(&func_scope_), scope_label);
}

ufunc_core::~ufunc_core()
{
      తొలగించు [] ports_;
}

/*
 * This method is called by the %exec_ufunc function to prepare the
 * input variables of the function for execution. The method copies
 * the input values collected by the core to the variables.
 */
శూన్య ufunc_core::కేటాయించు_బిట్లు_గమ్యం_ports(vvp_context_t context)
{
      కొరకు (గుర్తులేని idx = 0 ; idx < పోర్టు_లెక్క() ;  idx += 1) {
	    vvp_net_t*net = ports_[idx];
	    vvp_net_ptr_t pp (net, 0);

	      // If the port is a real variable, then simply copy the
	      // propagated input to the port variable.
	    ఒకవేళ (vvp_fun_signal_real*tmp = dynamic_cast<vvp_fun_signal_real*>(net->fun))
		  tmp->recv_real(pp, విలువ_r(idx), context);

	      // If the port is a bit4 vector, then copy the
	      // propagated input to the port variable. Detect the
	      // special case that the input vector is nil, and
	      // convert that to an 'bx vector that matches the width
	      // of the port variable. This is to handle the uncommon
	      // startup case where the input values have not
	      // propagated useful values yet.
	    ఒకవేళ (vvp_fun_signal_vec*tmp = dynamic_cast<vvp_fun_signal_vec*>(net->fun)) {
		  స్థిరరాశి vvp_vector4_t&tmp_val = value(idx);
		  ఒకవేళ (tmp_val.size() == 0) {
			స్థిరరాశి vvp_vector4_t&ref = tmp->vec4_unfiltered_విలువ();
			vvp_vector4_t xxx (ref.size(), BIT4_X);
			tmp->recv_vec4(pp, xxx, context);
		  } లేకపోతే {
			tmp->recv_vec4(pp, tmp_val, context);
		  }
	    }
      }
}

/*
 * This method is called by the %exec_ufunc instruction to copy the
 * result from the return code variable and deliver it to the output
 * of the functor, back into the netlist.
 */
శూన్య ufunc_core::ముగింపు_thread_real()
{
      ప్రతిజ్ఞా(thread_);

      ద్విగుణ val = vthread_get_real_దొంతర(thread_, 0);
      vthread_తీయి_real(thread_, 1);

      propagate_real(val);

      thread_ = 0;
}

శూన్య ufunc_core::ముగింపు_thread_vec4()
{
      ప్రతిజ్ఞా(thread_);

      vvp_vector4_t val = vthread_get_vec4_దొంతర(thread_, 0);
      vthread_తీయి_vec4(thread_, 1);

      propagate_vec4(val);

      thread_ = 0;
}

/*
 * This method is only called when a trigger event occurs. Just arrange for
 * the function to be called.
 */
శూన్య ufunc_core::recv_vec4(vvp_net_ptr_t, స్థిరరాశి vvp_vector4_t&,
                           vvp_context_t)
{
      invoke_thread_();
}

/*
 * The recv_vec4 methods of the input functors call this to assign the
 * input value to the port of the functor. I save the input value and
 * arrange for the function to be called.
 */
శూన్య ufunc_core::recv_vec4_from_inputs(గుర్తులేని)
{
      invoke_thread_();
}

శూన్య ufunc_core::recv_real_from_inputs(గుర్తులేని)
{
      invoke_thread_();
}

శూన్య ufunc_core::invoke_thread_()
{
      ఒకవేళ (thread_ == 0) {
	    thread_ = vthread_new(సంకేతం_3, call_scope_);
	    కాలపట్టిక_vthread(thread_, 0);
      }
}

ufunc_vec4::ufunc_vec4(గుర్తులేని ow, vvp_net_t*ptr,
		       గుర్తులేని nports, vvp_net_t**ports,
		       vvp_సంకేతం_t ప్రారంభం_చిరునామా,
		       __vpiScope*call_scope_లోపల,
		       అక్షరం*scope_label)
: ufunc_core(ow, ptr, nports, ports, ప్రారంభం_చిరునామా, call_scope_లోపల, scope_label)
{
}

ufunc_vec4::~ufunc_vec4()
{
}

శూన్య ufunc_vec4::ముగింపు_thread()
{
      ముగింపు_thread_vec4();
}

ufunc_real::ufunc_real(గుర్తులేని ow, vvp_net_t*ptr,
		       గుర్తులేని nports, vvp_net_t**ports,
		       vvp_సంకేతం_t ప్రారంభం_చిరునామా,
		       __vpiScope*call_scope_లోపల,
		       అక్షరం*scope_label)
: ufunc_core(ow, ptr, nports, ports, ప్రారంభం_చిరునామా, call_scope_లోపల, scope_label)
{
}

ufunc_real::~ufunc_real()
{
}

శూన్య ufunc_real::ముగింపు_thread()
{
      ముగింపు_thread_real();
}

/*
 * This function compiles the .ufunc statement that is discovered in
 * the source file. Create all the functors and the thread, and
 * connect them all up.
 *
 * The argv list is a list of the inputs to the function.
 *
 * The portv list is a list of variables that the function reads as
 * inputs. The core assigns values to these nets as part of the startup.
 */
శూన్య compile_ufunc_real(అక్షరం*label, అక్షరం*code, గుర్తులేని wid,
		   గుర్తులేని argc,  నిర్మాణం symb_s*argv,
		   గుర్తులేని portc, నిర్మాణం symb_s*portv,
		   అక్షరం*scope_label, అక్షరం*trigger_label)
{
	/* The input argument list and port list must have the same
	   sizes, since internally we will be mapping the inputs list
	   to the ports list. */
      ప్రతిజ్ఞా(argc == portc);

      __vpiScope*call_scope = vpip_peek_current_scope();
      ప్రతిజ్ఞా(call_scope);

	/* Construct some phantom code that is the thread of the
	   function call. The first instruction, at the start_address
	   of the function, loads the ports and calls the function.
	   The second instruction collects the function result. The
	   last instruction is the usual %end. So the thread looks
	   like this:

	      %exec_ufunc/real <core>;
	      %reap_ufunc;
	      %end;

	   The %exec_ufunc copies the input values into local regs
           and runs the function code. The %reap_ufunc then copies
	   the output value to the destination net functor. */

      vvp_సంకేతం_t exec_సంకేతం = codespace_allocate();
      exec_సంకేతం->opcode = యొక్క_EXEC_UFUNC_REAL;
      సంకేతం_label_lookup(exec_సంకేతం, code, అబద్ధం);

      vvp_సంకేతం_t reap_సంకేతం = codespace_allocate();
      reap_సంకేతం->opcode = యొక్క_REAP_UFUNC;

      vvp_సంకేతం_t చివర_సంకేతం = codespace_allocate();
      చివర_సంకేతం->opcode = &యొక్క_చివర;

	/* Run through the function ports (which are related to but
	   not the same as the input ports) and arrange for their
	   binding. */
      vvp_net_t**ports = కొత్త vvp_net_t*[portc];
      కొరకు (గుర్తులేని idx = 0 ;  idx < portc ;  idx += 1) {
	    functor_ref_lookup(&ports[idx], portv[idx].పాఠ్యం);
      }

	/* Create the output functor and attach it to the label. Tell
	   it about the start address of the code stub, and the scope
	   that will contain the execution. */
      vvp_net_t*ptr = కొత్త vvp_net_t;
      ufunc_core*fcore = కొత్త ufunc_real(wid, ptr, portc, ports,
					exec_సంకేతం, call_scope,
					scope_label);
      ptr->fun = fcore;
      define_functor_చిహ్నం(label, ptr);
      విడుదల(label);

      exec_సంకేతం->ufunc_core_ptr = fcore;
      reap_సంకేతం->ufunc_core_ptr = fcore;

      wide_inputs_connect(fcore, argc, argv);

        /* If this function has a trigger event, connect the functor to
           that event. */
      ఒకవేళ (trigger_label)
            ఆదానం_connect(ptr, 0, trigger_label);

      విడుదల(argv);
      విడుదల(portv);
}

శూన్య compile_ufunc_vec4(అక్షరం*label, అక్షరం*code, గుర్తులేని wid,
		   గుర్తులేని argc,  నిర్మాణం symb_s*argv,
		   గుర్తులేని portc, నిర్మాణం symb_s*portv,
		   అక్షరం*scope_label, అక్షరం*trigger_label)
{
	/* The input argument list and port list must have the same
	   sizes, since internally we will be mapping the inputs list
	   to the ports list. */
      ప్రతిజ్ఞా(argc == portc);

      __vpiScope*call_scope = vpip_peek_current_scope();
      ప్రతిజ్ఞా(call_scope);

	/* Construct some phantom code that is the thread of the
	   function call. The first instruction, at the start_address
	   of the function, loads the ports and calls the function.
	   The second instruction collects the function result. The
	   last instruction is the usual %end. So the thread looks
	   like this:

	      %exec_ufunc/vec4 <core>;
	      %reap_ufunc;
	      %end;

	   The %exec_ufunc copies the input values into local regs
           and runs the function code. The %reap_ufunc then copies
	   the output value to the destination net functor. */

      vvp_సంకేతం_t exec_సంకేతం = codespace_allocate();
      exec_సంకేతం->opcode = యొక్క_EXEC_UFUNC_VEC4;
      సంకేతం_label_lookup(exec_సంకేతం, code, అబద్ధం);

      vvp_సంకేతం_t reap_సంకేతం = codespace_allocate();
      reap_సంకేతం->opcode = యొక్క_REAP_UFUNC;

      vvp_సంకేతం_t చివర_సంకేతం = codespace_allocate();
      చివర_సంకేతం->opcode = &యొక్క_చివర;

	/* Run through the function ports (which are related to but
	   not the same as the input ports) and arrange for their
	   binding. */
      vvp_net_t**ports = కొత్త vvp_net_t*[portc];
      కొరకు (గుర్తులేని idx = 0 ;  idx < portc ;  idx += 1) {
	    functor_ref_lookup(&ports[idx], portv[idx].పాఠ్యం);
      }

	/* Create the output functor and attach it to the label. Tell
	   it about the start address of the code stub, and the scope
	   that will contain the execution. */
      vvp_net_t*ptr = కొత్త vvp_net_t;
      ufunc_core*fcore = కొత్త ufunc_vec4(wid, ptr, portc, ports,
					exec_సంకేతం, call_scope,
					scope_label);
      ptr->fun = fcore;
      define_functor_చిహ్నం(label, ptr);
      విడుదల(label);

      exec_సంకేతం->ufunc_core_ptr = fcore;
      reap_సంకేతం->ufunc_core_ptr = fcore;

      wide_inputs_connect(fcore, argc, argv);

        /* If this function has a trigger event, connect the functor to
           that event. */
      ఒకవేళ (trigger_label)
            ఆదానం_connect(ptr, 0, trigger_label);

      విడుదల(argv);
      విడుదల(portv);
}
#నిర్వచితమైతే CHECK_WITH_VALGRIND
స్థిర std::map<ufunc_core*, తార్కిక> ufunc_map;

శూన్య exec_ufunc_delete(vvp_సంకేతం_t euf_సంకేతం)
{
      ufunc_map[euf_సంకేతం->ufunc_core_ptr] = నిజం;
}

శూన్య ufunc_pool_delete(శూన్య)
{
      std::map<ufunc_core*, తార్కిక>::iterator iter;
      కొరకు (iter = ufunc_map.begin(); iter != ufunc_map.చివర(); ++ iter ) {
	    తొలగించు iter->మొదటి;
      }
}
#అయితే_ముగింపు
