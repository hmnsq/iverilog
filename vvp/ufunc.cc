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
#جيڪڏهن_وصف_ٿيل CHECK_WITH_VALGRIND
# include  "vvp_cleanup.h"
#جيڪڏهن_پورو
# include  <cstdlib>
# include  <cstring>
# include  <iostream>
# include  <cassert>
# include  <map>

#جيڪڏهن_وصف_ٿيل __MINGW32__
#ضمن <windows.h>
#جيڪڏهن_پورو

صنف ufunc_real : عام ufunc_core {
   عام:
      ufunc_real(بنانشان ow, vvp_net_t*ptr,
		 بنانشان nports, vvp_net_t**ports,
		 vvp_ڪوڊ_t شروعات_پتو,
		 __vpiScope*call_scope,
		 حرف*scope_label);
      ~ufunc_real() override;

      فراغ پڄاڻي_thread() override;
};

صنف ufunc_vec4 : عام ufunc_core {
   عام:
      ufunc_vec4(بنانشان ow, vvp_net_t*ptr,
		 بنانشان nports, vvp_net_t**ports,
		 vvp_ڪوڊ_t شروعات_پتو,
		 __vpiScope*call_scope,
		 حرف*scope_label);
      ~ufunc_vec4() override;

      فراغ پڄاڻي_thread() override;
};

ufunc_core::ufunc_core(بنانشان owid, vvp_net_t*ptr,
		       بنانشان nports, vvp_net_t**ports,
		       vvp_ڪوڊ_t sa, __vpiScope*call_scope__,
		       حرف*scope_label)
: vvp_wide_fun_core(ptr, nports)
{
      owid_ = owid;
      ports_ = ports;
      ڪوڊ_2 = sa;
      thread_ = 0;
      call_scope_ = call_scope__;

	/* A __vpiScope starts with a __vpiHandle structure so this is
	   a safe cast. We need the (void*) to avoid a dereferenced
	   type punned pointer warning from some gcc compilers. */
      compile_vpi_lookup((vpiHandle*)(فراغ*)(&func_scope_), scope_label);
}

ufunc_core::~ufunc_core()
{
      ميٽيو [] ports_;
}

/*
 * This method is called by the %exec_ufunc function to prepare the
 * input variables of the function for execution. The method copies
 * the input values collected by the core to the variables.
 */
فراغ ufunc_core::سونپيو_بٽ_منزل_ports(vvp_context_t context)
{
      لكل (بنانشان idx = 0 ; idx < پورٽ_ڳڻپ() ;  idx += 1) {
	    vvp_net_t*net = ports_[idx];
	    vvp_net_ptr_t pp (net, 0);

	      // If the port is a real variable, then simply copy the
	      // propagated input to the port variable.
	    إذا (vvp_fun_signal_real*tmp = dynamic_cast<vvp_fun_signal_real*>(net->fun))
		  tmp->recv_real(pp, قدر_r(idx), context);

	      // If the port is a bit4 vector, then copy the
	      // propagated input to the port variable. Detect the
	      // special case that the input vector is nil, and
	      // convert that to an 'bx vector that matches the width
	      // of the port variable. This is to handle the uncommon
	      // startup case where the input values have not
	      // propagated useful values yet.
	    إذا (vvp_fun_signal_vec*tmp = dynamic_cast<vvp_fun_signal_vec*>(net->fun)) {
		  ثابت vvp_vector4_t&tmp_val = value(idx);
		  إذا (tmp_val.size() == 0) {
			ثابت vvp_vector4_t&ref = tmp->vec4_unfiltered_قدر();
			vvp_vector4_t xxx (ref.size(), BIT4_X);
			tmp->recv_vec4(pp, xxx, context);
		  } وإلا {
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
فراغ ufunc_core::پڄاڻي_thread_real()
{
      أكد(thread_);

      مزدوج val = vthread_get_real_ڍير(thread_, 0);
      vthread_ڪڍو_real(thread_, 1);

      propagate_real(val);

      thread_ = 0;
}

فراغ ufunc_core::پڄاڻي_thread_vec4()
{
      أكد(thread_);

      vvp_vector4_t val = vthread_get_vec4_ڍير(thread_, 0);
      vthread_ڪڍو_vec4(thread_, 1);

      propagate_vec4(val);

      thread_ = 0;
}

/*
 * This method is only called when a trigger event occurs. Just arrange for
 * the function to be called.
 */
فراغ ufunc_core::recv_vec4(vvp_net_ptr_t, ثابت vvp_vector4_t&,
                           vvp_context_t)
{
      invoke_thread_();
}

/*
 * The recv_vec4 methods of the input functors call this to assign the
 * input value to the port of the functor. I save the input value and
 * arrange for the function to be called.
 */
فراغ ufunc_core::recv_vec4_from_inputs(بنانشان)
{
      invoke_thread_();
}

فراغ ufunc_core::recv_real_from_inputs(بنانشان)
{
      invoke_thread_();
}

فراغ ufunc_core::invoke_thread_()
{
      إذا (thread_ == 0) {
	    thread_ = vthread_new(ڪوڊ_2, call_scope_);
	    وقت_جدول_vthread(thread_, 0);
      }
}

ufunc_vec4::ufunc_vec4(بنانشان ow, vvp_net_t*ptr,
		       بنانشان nports, vvp_net_t**ports,
		       vvp_ڪوڊ_t شروعات_پتو,
		       __vpiScope*call_scope_اندريون,
		       حرف*scope_label)
: ufunc_core(ow, ptr, nports, ports, شروعات_پتو, call_scope_اندريون, scope_label)
{
}

ufunc_vec4::~ufunc_vec4()
{
}

فراغ ufunc_vec4::پڄاڻي_thread()
{
      پڄاڻي_thread_vec4();
}

ufunc_real::ufunc_real(بنانشان ow, vvp_net_t*ptr,
		       بنانشان nports, vvp_net_t**ports,
		       vvp_ڪوڊ_t شروعات_پتو,
		       __vpiScope*call_scope_اندريون,
		       حرف*scope_label)
: ufunc_core(ow, ptr, nports, ports, شروعات_پتو, call_scope_اندريون, scope_label)
{
}

ufunc_real::~ufunc_real()
{
}

فراغ ufunc_real::پڄاڻي_thread()
{
      پڄاڻي_thread_real();
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
فراغ compile_ufunc_real(حرف*label, حرف*code, بنانشان wid,
		   بنانشان argc,  بنية symb_s*argv,
		   بنانشان portc, بنية symb_s*portv,
		   حرف*scope_label, حرف*trigger_label)
{
	/* The input argument list and port list must have the same
	   sizes, since internally we will be mapping the inputs list
	   to the ports list. */
      أكد(argc == portc);

      __vpiScope*call_scope = vpip_peek_current_scope();
      أكد(call_scope);

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

      vvp_ڪوڊ_t exec_ڪوڊ = codespace_allocate();
      exec_ڪوڊ->opcode = جو_EXEC_UFUNC_REAL;
      ڪوڊ_label_lookup(exec_ڪوڊ, code, خطأ);

      vvp_ڪوڊ_t reap_ڪوڊ = codespace_allocate();
      reap_ڪوڊ->opcode = جو_REAP_UFUNC;

      vvp_ڪوڊ_t پڇاڙي_ڪوڊ = codespace_allocate();
      پڇاڙي_ڪوڊ->opcode = &جو_پڇاڙي;

	/* Run through the function ports (which are related to but
	   not the same as the input ports) and arrange for their
	   binding. */
      vvp_net_t**ports = جديد vvp_net_t*[portc];
      لكل (بنانشان idx = 0 ;  idx < portc ;  idx += 1) {
	    functor_ref_lookup(&ports[idx], portv[idx].متن);
      }

	/* Create the output functor and attach it to the label. Tell
	   it about the start address of the code stub, and the scope
	   that will contain the execution. */
      vvp_net_t*ptr = جديد vvp_net_t;
      ufunc_core*fcore = جديد ufunc_real(wid, ptr, portc, ports,
					exec_ڪوڊ, call_scope,
					scope_label);
      ptr->fun = fcore;
      define_functor_نشان(label, ptr);
      حرر(label);

      exec_ڪوڊ->ufunc_core_ptr = fcore;
      reap_ڪوڊ->ufunc_core_ptr = fcore;

      wide_inputs_connect(fcore, argc, argv);

        /* If this function has a trigger event, connect the functor to
           that event. */
      إذا (trigger_label)
            داخلا_connect(ptr, 0, trigger_label);

      حرر(argv);
      حرر(portv);
}

فراغ compile_ufunc_vec4(حرف*label, حرف*code, بنانشان wid,
		   بنانشان argc,  بنية symb_s*argv,
		   بنانشان portc, بنية symb_s*portv,
		   حرف*scope_label, حرف*trigger_label)
{
	/* The input argument list and port list must have the same
	   sizes, since internally we will be mapping the inputs list
	   to the ports list. */
      أكد(argc == portc);

      __vpiScope*call_scope = vpip_peek_current_scope();
      أكد(call_scope);

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

      vvp_ڪوڊ_t exec_ڪوڊ = codespace_allocate();
      exec_ڪوڊ->opcode = جو_EXEC_UFUNC_VEC4;
      ڪوڊ_label_lookup(exec_ڪوڊ, code, خطأ);

      vvp_ڪوڊ_t reap_ڪوڊ = codespace_allocate();
      reap_ڪوڊ->opcode = جو_REAP_UFUNC;

      vvp_ڪوڊ_t پڇاڙي_ڪوڊ = codespace_allocate();
      پڇاڙي_ڪوڊ->opcode = &جو_پڇاڙي;

	/* Run through the function ports (which are related to but
	   not the same as the input ports) and arrange for their
	   binding. */
      vvp_net_t**ports = جديد vvp_net_t*[portc];
      لكل (بنانشان idx = 0 ;  idx < portc ;  idx += 1) {
	    functor_ref_lookup(&ports[idx], portv[idx].متن);
      }

	/* Create the output functor and attach it to the label. Tell
	   it about the start address of the code stub, and the scope
	   that will contain the execution. */
      vvp_net_t*ptr = جديد vvp_net_t;
      ufunc_core*fcore = جديد ufunc_vec4(wid, ptr, portc, ports,
					exec_ڪوڊ, call_scope,
					scope_label);
      ptr->fun = fcore;
      define_functor_نشان(label, ptr);
      حرر(label);

      exec_ڪوڊ->ufunc_core_ptr = fcore;
      reap_ڪوڊ->ufunc_core_ptr = fcore;

      wide_inputs_connect(fcore, argc, argv);

        /* If this function has a trigger event, connect the functor to
           that event. */
      إذا (trigger_label)
            داخلا_connect(ptr, 0, trigger_label);

      حرر(argv);
      حرر(portv);
}
#جيڪڏهن_وصف_ٿيل CHECK_WITH_VALGRIND
ساكن std::map<ufunc_core*, منطقي> ufunc_map;

فراغ exec_ufunc_delete(vvp_ڪوڊ_t euf_ڪوڊ)
{
      ufunc_map[euf_ڪوڊ->ufunc_core_ptr] = سچ;
}

فراغ ufunc_pool_delete(فراغ)
{
      std::map<ufunc_core*, منطقي>::iterator iter;
      لكل (iter = ufunc_map.begin(); iter != ufunc_map.پڇاڙي(); ++ iter ) {
	    ميٽيو iter->پهريون;
      }
}
#جيڪڏهن_پورو
