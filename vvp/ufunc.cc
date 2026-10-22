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
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ CHECK_WITH_VALGRIND
# include  "vvp_cleanup.h"
#ਜੇ_ਅੰਤ
# include  <cstdlib>
# include  <cstring>
# include  <iostream>
# include  <cassert>
# include  <map>

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ __MINGW32__
#ਅਨ੍ਤਰ੍ਭਾਵ <windows.h>
#ਜੇ_ਅੰਤ

ਵਰਗ ufunc_real : ਜਨਤਕ ufunc_core {
   ਜਨਤਕ:
      ufunc_real(ਬਿਨਾਨਿਸ਼ਾਨੀ ow, vvp_net_t*ptr,
		 ਬਿਨਾਨਿਸ਼ਾਨੀ nports, vvp_net_t**ports,
		 vvp_ਕੋਡ_t ਸ਼ੁਰੂ_ਪਤਾ,
		 __vpiScope*call_scope,
		 ਅੱਖਰ*scope_label);
      ~ufunc_real() override;

      ਖਾਲੀ ਅੰਤ_thread() override;
};

ਵਰਗ ufunc_vec4 : ਜਨਤਕ ufunc_core {
   ਜਨਤਕ:
      ufunc_vec4(ਬਿਨਾਨਿਸ਼ਾਨੀ ow, vvp_net_t*ptr,
		 ਬਿਨਾਨਿਸ਼ਾਨੀ nports, vvp_net_t**ports,
		 vvp_ਕੋਡ_t ਸ਼ੁਰੂ_ਪਤਾ,
		 __vpiScope*call_scope,
		 ਅੱਖਰ*scope_label);
      ~ufunc_vec4() override;

      ਖਾਲੀ ਅੰਤ_thread() override;
};

ufunc_core::ufunc_core(ਬਿਨਾਨਿਸ਼ਾਨੀ owid, vvp_net_t*ptr,
		       ਬਿਨਾਨਿਸ਼ਾਨੀ nports, vvp_net_t**ports,
		       vvp_ਕੋਡ_t sa, __vpiScope*call_scope__,
		       ਅੱਖਰ*scope_label)
: vvp_wide_fun_core(ptr, nports)
{
      owid_ = owid;
      ports_ = ports;
      ਕੋਡ_2 = sa;
      thread_ = 0;
      call_scope_ = call_scope__;

	/* A __vpiScope starts with a __vpiHandle structure so this is
	   a safe cast. We need the (void*) to avoid a dereferenced
	   type punned pointer warning from some gcc compilers. */
      compile_vpi_lookup((vpiHandle*)(ਖਾਲੀ*)(&func_scope_), scope_label);
}

ufunc_core::~ufunc_core()
{
      ਮਿਟਾਓ [] ports_;
}

/*
 * This method is called by the %exec_ufunc function to prepare the
 * input variables of the function for execution. The method copies
 * the input values collected by the core to the variables.
 */
ਖਾਲੀ ufunc_core::ਸੌਂਪੋ_ਬਿੱਟ_ਮੰਜ਼ਿਲ_ports(vvp_context_t context)
{
      ਲਈ (ਬਿਨਾਨਿਸ਼ਾਨੀ idx = 0 ; idx < ਪੋਰਟ_ਗਿਣਤੀ() ;  idx += 1) {
	    vvp_net_t*net = ports_[idx];
	    vvp_net_ptr_t pp (net, 0);

	      // If the port is a real variable, then simply copy the
	      // propagated input to the port variable.
	    ਜੇ (vvp_fun_signal_real*tmp = dynamic_cast<vvp_fun_signal_real*>(net->fun))
		  tmp->recv_real(pp, ਮੁੱਲ_r(idx), context);

	      // If the port is a bit4 vector, then copy the
	      // propagated input to the port variable. Detect the
	      // special case that the input vector is nil, and
	      // convert that to an 'bx vector that matches the width
	      // of the port variable. This is to handle the uncommon
	      // startup case where the input values have not
	      // propagated useful values yet.
	    ਜੇ (vvp_fun_signal_vec*tmp = dynamic_cast<vvp_fun_signal_vec*>(net->fun)) {
		  ਅਚਲ vvp_vector4_t&tmp_val = value(idx);
		  ਜੇ (tmp_val.size() == 0) {
			ਅਚਲ vvp_vector4_t&ref = tmp->vec4_unfiltered_ਮੁੱਲ();
			vvp_vector4_t xxx (ref.size(), BIT4_X);
			tmp->recv_vec4(pp, xxx, context);
		  } ਨਹੀਂਤਾਂ {
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
ਖਾਲੀ ufunc_core::ਅੰਤ_thread_real()
{
      ਪ੍ਰਤਿਜ੍ਞਾ(thread_);

      ਦੁੱਗਣਾ val = vthread_get_real_ਢੇਰ(thread_, 0);
      vthread_ਕੱਢੋ_real(thread_, 1);

      propagate_real(val);

      thread_ = 0;
}

ਖਾਲੀ ufunc_core::ਅੰਤ_thread_vec4()
{
      ਪ੍ਰਤਿਜ੍ਞਾ(thread_);

      vvp_vector4_t val = vthread_get_vec4_ਢੇਰ(thread_, 0);
      vthread_ਕੱਢੋ_vec4(thread_, 1);

      propagate_vec4(val);

      thread_ = 0;
}

/*
 * This method is only called when a trigger event occurs. Just arrange for
 * the function to be called.
 */
ਖਾਲੀ ufunc_core::recv_vec4(vvp_net_ptr_t, ਅਚਲ vvp_vector4_t&,
                           vvp_context_t)
{
      invoke_thread_();
}

/*
 * The recv_vec4 methods of the input functors call this to assign the
 * input value to the port of the functor. I save the input value and
 * arrange for the function to be called.
 */
ਖਾਲੀ ufunc_core::recv_vec4_from_inputs(ਬਿਨਾਨਿਸ਼ਾਨੀ)
{
      invoke_thread_();
}

ਖਾਲੀ ufunc_core::recv_real_from_inputs(ਬਿਨਾਨਿਸ਼ਾਨੀ)
{
      invoke_thread_();
}

ਖਾਲੀ ufunc_core::invoke_thread_()
{
      ਜੇ (thread_ == 0) {
	    thread_ = vthread_new(ਕੋਡ_2, call_scope_);
	    ਸਮਾਂ_ਸਾਰਣੀ_vthread(thread_, 0);
      }
}

ufunc_vec4::ufunc_vec4(ਬਿਨਾਨਿਸ਼ਾਨੀ ow, vvp_net_t*ptr,
		       ਬਿਨਾਨਿਸ਼ਾਨੀ nports, vvp_net_t**ports,
		       vvp_ਕੋਡ_t ਸ਼ੁਰੂ_ਪਤਾ,
		       __vpiScope*call_scope_ਵਿੱਚ,
		       ਅੱਖਰ*scope_label)
: ufunc_core(ow, ptr, nports, ports, ਸ਼ੁਰੂ_ਪਤਾ, call_scope_ਵਿੱਚ, scope_label)
{
}

ufunc_vec4::~ufunc_vec4()
{
}

ਖਾਲੀ ufunc_vec4::ਅੰਤ_thread()
{
      ਅੰਤ_thread_vec4();
}

ufunc_real::ufunc_real(ਬਿਨਾਨਿਸ਼ਾਨੀ ow, vvp_net_t*ptr,
		       ਬਿਨਾਨਿਸ਼ਾਨੀ nports, vvp_net_t**ports,
		       vvp_ਕੋਡ_t ਸ਼ੁਰੂ_ਪਤਾ,
		       __vpiScope*call_scope_ਵਿੱਚ,
		       ਅੱਖਰ*scope_label)
: ufunc_core(ow, ptr, nports, ports, ਸ਼ੁਰੂ_ਪਤਾ, call_scope_ਵਿੱਚ, scope_label)
{
}

ufunc_real::~ufunc_real()
{
}

ਖਾਲੀ ufunc_real::ਅੰਤ_thread()
{
      ਅੰਤ_thread_real();
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
ਖਾਲੀ compile_ufunc_real(ਅੱਖਰ*label, ਅੱਖਰ*code, ਬਿਨਾਨਿਸ਼ਾਨੀ wid,
		   ਬਿਨਾਨਿਸ਼ਾਨੀ argc,  ਬਣਤਰ symb_s*argv,
		   ਬਿਨਾਨਿਸ਼ਾਨੀ portc, ਬਣਤਰ symb_s*portv,
		   ਅੱਖਰ*scope_label, ਅੱਖਰ*trigger_label)
{
	/* The input argument list and port list must have the same
	   sizes, since internally we will be mapping the inputs list
	   to the ports list. */
      ਪ੍ਰਤਿਜ੍ਞਾ(argc == portc);

      __vpiScope*call_scope = vpip_peek_current_scope();
      ਪ੍ਰਤਿਜ੍ਞਾ(call_scope);

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

      vvp_ਕੋਡ_t exec_ਕੋਡ = codespace_allocate();
      exec_ਕੋਡ->opcode = ਦਾ_EXEC_UFUNC_REAL;
      ਕੋਡ_label_lookup(exec_ਕੋਡ, code, ਝੂਠ);

      vvp_ਕੋਡ_t reap_ਕੋਡ = codespace_allocate();
      reap_ਕੋਡ->opcode = ਦਾ_REAP_UFUNC;

      vvp_ਕੋਡ_t ਸਿਰਾ_ਕੋਡ = codespace_allocate();
      ਸਿਰਾ_ਕੋਡ->opcode = &ਦਾ_ਸਿਰਾ;

	/* Run through the function ports (which are related to but
	   not the same as the input ports) and arrange for their
	   binding. */
      vvp_net_t**ports = ਨਵਾਂ vvp_net_t*[portc];
      ਲਈ (ਬਿਨਾਨਿਸ਼ਾਨੀ idx = 0 ;  idx < portc ;  idx += 1) {
	    functor_ref_lookup(&ports[idx], portv[idx].ਪਾਠ);
      }

	/* Create the output functor and attach it to the label. Tell
	   it about the start address of the code stub, and the scope
	   that will contain the execution. */
      vvp_net_t*ptr = ਨਵਾਂ vvp_net_t;
      ufunc_core*fcore = ਨਵਾਂ ufunc_real(wid, ptr, portc, ports,
					exec_ਕੋਡ, call_scope,
					scope_label);
      ptr->fun = fcore;
      define_functor_ਚਿੰਨ੍ਹ(label, ptr);
      ਮੁਕਤ(label);

      exec_ਕੋਡ->ufunc_core_ptr = fcore;
      reap_ਕੋਡ->ufunc_core_ptr = fcore;

      wide_inputs_connect(fcore, argc, argv);

        /* If this function has a trigger event, connect the functor to
           that event. */
      ਜੇ (trigger_label)
            ਨਿਵੇਸ਼_connect(ptr, 0, trigger_label);

      ਮੁਕਤ(argv);
      ਮੁਕਤ(portv);
}

ਖਾਲੀ compile_ufunc_vec4(ਅੱਖਰ*label, ਅੱਖਰ*code, ਬਿਨਾਨਿਸ਼ਾਨੀ wid,
		   ਬਿਨਾਨਿਸ਼ਾਨੀ argc,  ਬਣਤਰ symb_s*argv,
		   ਬਿਨਾਨਿਸ਼ਾਨੀ portc, ਬਣਤਰ symb_s*portv,
		   ਅੱਖਰ*scope_label, ਅੱਖਰ*trigger_label)
{
	/* The input argument list and port list must have the same
	   sizes, since internally we will be mapping the inputs list
	   to the ports list. */
      ਪ੍ਰਤਿਜ੍ਞਾ(argc == portc);

      __vpiScope*call_scope = vpip_peek_current_scope();
      ਪ੍ਰਤਿਜ੍ਞਾ(call_scope);

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

      vvp_ਕੋਡ_t exec_ਕੋਡ = codespace_allocate();
      exec_ਕੋਡ->opcode = ਦਾ_EXEC_UFUNC_VEC4;
      ਕੋਡ_label_lookup(exec_ਕੋਡ, code, ਝੂਠ);

      vvp_ਕੋਡ_t reap_ਕੋਡ = codespace_allocate();
      reap_ਕੋਡ->opcode = ਦਾ_REAP_UFUNC;

      vvp_ਕੋਡ_t ਸਿਰਾ_ਕੋਡ = codespace_allocate();
      ਸਿਰਾ_ਕੋਡ->opcode = &ਦਾ_ਸਿਰਾ;

	/* Run through the function ports (which are related to but
	   not the same as the input ports) and arrange for their
	   binding. */
      vvp_net_t**ports = ਨਵਾਂ vvp_net_t*[portc];
      ਲਈ (ਬਿਨਾਨਿਸ਼ਾਨੀ idx = 0 ;  idx < portc ;  idx += 1) {
	    functor_ref_lookup(&ports[idx], portv[idx].ਪਾਠ);
      }

	/* Create the output functor and attach it to the label. Tell
	   it about the start address of the code stub, and the scope
	   that will contain the execution. */
      vvp_net_t*ptr = ਨਵਾਂ vvp_net_t;
      ufunc_core*fcore = ਨਵਾਂ ufunc_vec4(wid, ptr, portc, ports,
					exec_ਕੋਡ, call_scope,
					scope_label);
      ptr->fun = fcore;
      define_functor_ਚਿੰਨ੍ਹ(label, ptr);
      ਮੁਕਤ(label);

      exec_ਕੋਡ->ufunc_core_ptr = fcore;
      reap_ਕੋਡ->ufunc_core_ptr = fcore;

      wide_inputs_connect(fcore, argc, argv);

        /* If this function has a trigger event, connect the functor to
           that event. */
      ਜੇ (trigger_label)
            ਨਿਵੇਸ਼_connect(ptr, 0, trigger_label);

      ਮੁਕਤ(argv);
      ਮੁਕਤ(portv);
}
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ CHECK_WITH_VALGRIND
ਸਥਿਰ std::map<ufunc_core*, ਤਾਰਕਿਕ> ufunc_map;

ਖਾਲੀ exec_ufunc_delete(vvp_ਕੋਡ_t euf_ਕੋਡ)
{
      ufunc_map[euf_ਕੋਡ->ufunc_core_ptr] = ਸੱਚ;
}

ਖਾਲੀ ufunc_pool_delete(ਖਾਲੀ)
{
      std::map<ufunc_core*, ਤਾਰਕਿਕ>::iterator iter;
      ਲਈ (iter = ufunc_map.begin(); iter != ufunc_map.ਸਿਰਾ(); ++ iter ) {
	    ਮਿਟਾਓ iter->ਪਹਿਲਾ;
      }
}
#ਜੇ_ਅੰਤ
