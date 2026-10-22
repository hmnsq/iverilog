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
#ifdef CHECK_WITH_VALGRIND
# include  "vvp_cleanup.h"
#endif
# include  <cstdlib>
# include  <cstring>
# include  <iostream>
# include  <cassert>
# include  <map>

#ifdef __MINGW32__
#अन्तर्भाव <windows.h>
#endif

वग्ग ufunc_real : सार्वजनिक ufunc_core {
   सार्वजनिक:
      ufunc_real(unsigned ow, vvp_net_t*ptr,
		 unsigned nports, vvp_net_t**ports,
		 vvp_code_t start_address,
		 __vpiScope*call_scope,
		 वर्ण*scope_label);
      ~ufunc_real() override;

      शून्य finish_thread() override;
};

वग्ग ufunc_vec4 : सार्वजनिक ufunc_core {
   सार्वजनिक:
      ufunc_vec4(unsigned ow, vvp_net_t*ptr,
		 unsigned nports, vvp_net_t**ports,
		 vvp_code_t start_address,
		 __vpiScope*call_scope,
		 वर्ण*scope_label);
      ~ufunc_vec4() override;

      शून्य finish_thread() override;
};

ufunc_core::ufunc_core(unsigned owid, vvp_net_t*ptr,
		       unsigned nports, vvp_net_t**ports,
		       vvp_code_t sa, __vpiScope*call_scope__,
		       वर्ण*scope_label)
: vvp_wide_fun_core(ptr, nports)
{
      owid_ = owid;
      ports_ = ports;
      code_ = sa;
      thread_ = 0;
      call_scope_ = call_scope__;

	/* A __vpiScope starts with a __vpiHandle structure so this is
	   a safe cast. We need the (void*) to avoid a dereferenced
	   type punned pointer warning from some gcc compilers. */
      compile_vpi_lookup((vpiHandle*)(शून्य*)(&func_scope_), scope_label);
}

ufunc_core::~ufunc_core()
{
      delete [] ports_;
}

/*
 * This method is called by the %exec_ufunc function to prepare the
 * input variables of the function for execution. The method copies
 * the input values collected by the core to the variables.
 */
शून्य ufunc_core::assign_bits_to_ports(vvp_context_t context)
{
      अत्थाय (unsigned idx = 0 ; idx < port_गणनम्() ;  idx += 1) {
	    vvp_net_t*net = ports_[idx];
	    vvp_net_ptr_t pp (net, 0);

	      // If the port is a real variable, then simply copy the
	      // propagated input to the port variable.
	    सचे (vvp_fun_signal_real*tmp = dynamic_cast<vvp_fun_signal_real*>(net->fun))
		  tmp->recv_real(pp, मानम्_r(idx), context);

	      // If the port is a bit4 vector, then copy the
	      // propagated input to the port variable. Detect the
	      // special case that the input vector is nil, and
	      // convert that to an 'bx vector that matches the width
	      // of the port variable. This is to handle the uncommon
	      // startup case where the input values have not
	      // propagated useful values yet.
	    सचे (vvp_fun_signal_vec*tmp = dynamic_cast<vvp_fun_signal_vec*>(net->fun)) {
		  ध्रुव vvp_vector4_t&tmp_val = value(idx);
		  सचे (tmp_val.size() == 0) {
			ध्रुव vvp_vector4_t&ref = tmp->vec4_unfiltered_मानम्();
			vvp_vector4_t xxx (ref.size(), BIT4_X);
			tmp->recv_vec4(pp, xxx, context);
		  } अञ्ञथा {
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
शून्य ufunc_core::finish_thread_real_()
{
      प्रतिज्ञा(thread_);

      द्विगुण val = vthread_get_real_राशिः(thread_, 0);
      vthread_निष्कासय_real(thread_, 1);

      propagate_real(val);

      thread_ = 0;
}

शून्य ufunc_core::finish_thread_vec4_()
{
      प्रतिज्ञा(thread_);

      vvp_vector4_t val = vthread_get_vec4_राशिः(thread_, 0);
      vthread_निष्कासय_vec4(thread_, 1);

      propagate_vec4(val);

      thread_ = 0;
}

/*
 * This method is only called when a trigger event occurs. Just arrange for
 * the function to be called.
 */
शून्य ufunc_core::recv_vec4(vvp_net_ptr_t, ध्रुव vvp_vector4_t&,
                           vvp_context_t)
{
      invoke_thread_();
}

/*
 * The recv_vec4 methods of the input functors call this to assign the
 * input value to the port of the functor. I save the input value and
 * arrange for the function to be called.
 */
शून्य ufunc_core::recv_vec4_from_inputs(unsigned)
{
      invoke_thread_();
}

शून्य ufunc_core::recv_real_from_inputs(unsigned)
{
      invoke_thread_();
}

शून्य ufunc_core::invoke_thread_()
{
      सचे (thread_ == 0) {
	    thread_ = vthread_new(code_, call_scope_);
	    schedule_vthread(thread_, 0);
      }
}

ufunc_vec4::ufunc_vec4(unsigned ow, vvp_net_t*ptr,
		       unsigned nports, vvp_net_t**ports,
		       vvp_code_t start_address,
		       __vpiScope*call_scope_in,
		       वर्ण*scope_label)
: ufunc_core(ow, ptr, nports, ports, start_address, call_scope_in, scope_label)
{
}

ufunc_vec4::~ufunc_vec4()
{
}

शून्य ufunc_vec4::finish_thread()
{
      finish_thread_vec4_();
}

ufunc_real::ufunc_real(unsigned ow, vvp_net_t*ptr,
		       unsigned nports, vvp_net_t**ports,
		       vvp_code_t start_address,
		       __vpiScope*call_scope_in,
		       वर्ण*scope_label)
: ufunc_core(ow, ptr, nports, ports, start_address, call_scope_in, scope_label)
{
}

ufunc_real::~ufunc_real()
{
}

शून्य ufunc_real::finish_thread()
{
      finish_thread_real_();
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
शून्य compile_ufunc_real(वर्ण*label, वर्ण*code, unsigned wid,
		   unsigned argc,  संरचना symb_s*argv,
		   unsigned portc, संरचना symb_s*portv,
		   वर्ण*scope_label, वर्ण*trigger_label)
{
	/* The input argument list and port list must have the same
	   sizes, since internally we will be mapping the inputs list
	   to the ports list. */
      प्रतिज्ञा(argc == portc);

      __vpiScope*call_scope = vpip_peek_current_scope();
      प्रतिज्ञा(call_scope);

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

      vvp_code_t exec_code = codespace_allocate();
      exec_code->opcode = of_EXEC_UFUNC_REAL;
      code_label_lookup(exec_code, code, मिच्छा);

      vvp_code_t reap_code = codespace_allocate();
      reap_code->opcode = of_REAP_UFUNC;

      vvp_code_t end_code = codespace_allocate();
      end_code->opcode = &of_END;

	/* Run through the function ports (which are related to but
	   not the same as the input ports) and arrange for their
	   binding. */
      vvp_net_t**ports = नूतन vvp_net_t*[portc];
      अत्थाय (unsigned idx = 0 ;  idx < portc ;  idx += 1) {
	    functor_ref_lookup(&ports[idx], portv[idx].text);
      }

	/* Create the output functor and attach it to the label. Tell
	   it about the start address of the code stub, and the scope
	   that will contain the execution. */
      vvp_net_t*ptr = नूतन vvp_net_t;
      ufunc_core*fcore = नूतन ufunc_real(wid, ptr, portc, ports,
					exec_code, call_scope,
					scope_label);
      ptr->fun = fcore;
      define_functor_चिह्नम्(label, ptr);
      मुक्त(label);

      exec_code->ufunc_core_ptr = fcore;
      reap_code->ufunc_core_ptr = fcore;

      wide_inputs_connect(fcore, argc, argv);

        /* If this function has a trigger event, connect the functor to
           that event. */
      सचे (trigger_label)
            निवेशः_connect(ptr, 0, trigger_label);

      मुक्त(argv);
      मुक्त(portv);
}

शून्य compile_ufunc_vec4(वर्ण*label, वर्ण*code, unsigned wid,
		   unsigned argc,  संरचना symb_s*argv,
		   unsigned portc, संरचना symb_s*portv,
		   वर्ण*scope_label, वर्ण*trigger_label)
{
	/* The input argument list and port list must have the same
	   sizes, since internally we will be mapping the inputs list
	   to the ports list. */
      प्रतिज्ञा(argc == portc);

      __vpiScope*call_scope = vpip_peek_current_scope();
      प्रतिज्ञा(call_scope);

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

      vvp_code_t exec_code = codespace_allocate();
      exec_code->opcode = of_EXEC_UFUNC_VEC4;
      code_label_lookup(exec_code, code, मिच्छा);

      vvp_code_t reap_code = codespace_allocate();
      reap_code->opcode = of_REAP_UFUNC;

      vvp_code_t end_code = codespace_allocate();
      end_code->opcode = &of_END;

	/* Run through the function ports (which are related to but
	   not the same as the input ports) and arrange for their
	   binding. */
      vvp_net_t**ports = नूतन vvp_net_t*[portc];
      अत्थाय (unsigned idx = 0 ;  idx < portc ;  idx += 1) {
	    functor_ref_lookup(&ports[idx], portv[idx].text);
      }

	/* Create the output functor and attach it to the label. Tell
	   it about the start address of the code stub, and the scope
	   that will contain the execution. */
      vvp_net_t*ptr = नूतन vvp_net_t;
      ufunc_core*fcore = नूतन ufunc_vec4(wid, ptr, portc, ports,
					exec_code, call_scope,
					scope_label);
      ptr->fun = fcore;
      define_functor_चिह्नम्(label, ptr);
      मुक्त(label);

      exec_code->ufunc_core_ptr = fcore;
      reap_code->ufunc_core_ptr = fcore;

      wide_inputs_connect(fcore, argc, argv);

        /* If this function has a trigger event, connect the functor to
           that event. */
      सचे (trigger_label)
            निवेशः_connect(ptr, 0, trigger_label);

      मुक्त(argv);
      मुक्त(portv);
}
#ifdef CHECK_WITH_VALGRIND
स्थिर std::map<ufunc_core*, तार्किक> ufunc_map;

शून्य exec_ufunc_delete(vvp_code_t euf_code)
{
      ufunc_map[euf_code->ufunc_core_ptr] = सच्च;
}

शून्य ufunc_pool_delete(शून्य)
{
      std::map<ufunc_core*, तार्किक>::iterator iter;
      अत्थाय (iter = ufunc_map.begin(); iter != ufunc_map.end(); ++ iter ) {
	    delete iter->first;
      }
}
#endif
