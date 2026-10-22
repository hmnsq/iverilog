#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ IVL_ufunc_H
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ IVL_ufunc_H
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

# include  "config.h"

class __vpiScope;

/*
 * The .ufunc statement creates functors to represent user defined
 * functions within the netlist (as opposed to within behavioral
 * code.) The function device itself is implemented as a thread with a
 * bunch of functors to receive input and a functor to deliver the
 * output. (Functions have exactly one output.) The input functors
 * detect that a change has occurred, and invoke the thread to process
 * the new values. The thread then uses the output functor to deliver
 * the result. The relationships look like this:
 *
 *  ufunc_input_functor --+--> ufunc_core --> ...
 *                        |
 *  ufunc_input_functor --+
 *                        |
 *  ufunc_input_functor --+
 *
 * There are enough input functors to take all the function inputs, 4
 * per functor. These inputs deliver the changed input value to the
 * ufunc_core, which carries the infrastructure for the thread. The
 * ufunc_core is also a functor whose output is connected to the rest
 * of the netlist. This is where the result is delivered back to the
 * netlist.
 *
 * This class relies to the vvp_wide_fun_* classes in vvp_net.h.
 */

class ufunc_core : ਜਨਤਕ vvp_wide_fun_core {

    ਜਨਤਕ:
      ufunc_core(ਬਿਨਾਨਿਸ਼ਾਨੀ ow, vvp_net_t*ptr,
		 ਬਿਨਾਨਿਸ਼ਾਨੀ nports, vvp_net_t**ports,
		 vvp_ਕੋਡ_t ਸ਼ੁਰੂ_ਪਤਾ,
		 __vpiScope*call_scope,
		 ਅੱਖਰ*scope_label);
      ਆਭਾਸੀ ~ufunc_core() override =0;

      __vpiScope*call_scope() { ਵਾਪਸ call_scope_; }
      __vpiScope*func_scope() { ਵਾਪਸ func_scope_; }

      ਖਾਲੀ ਸੌਂਪੋ_ਬਿੱਟ_ਮੰਜ਼ਿਲ_ports(vvp_context_t context);
      ਆਭਾਸੀ ਖਾਲੀ ਅੰਤ_thread() =0;

      ਖਾਲੀ recv_vec4(vvp_net_ptr_t ਪੋਰਟ, ਅਚਲ vvp_vector4_t&ਦੋਆਧਾਰੀ_ਅੰਕ,
                     vvp_context_t context) override;

    ਸੁਰੱਖਿਅਤ:
      ਖਾਲੀ ਅੰਤ_thread_real();
      ਖਾਲੀ ਅੰਤ_thread_vec4();

    ਨਿਜੀ:
      ਖਾਲੀ recv_vec4_from_inputs(ਬਿਨਾਨਿਸ਼ਾਨੀ ਪੋਰਟ) override;
      ਖਾਲੀ recv_real_from_inputs(ਬਿਨਾਨਿਸ਼ਾਨੀ ਪੋਰਟ) override;

      ਖਾਲੀ invoke_thread_(ਖਾਲੀ);


    ਨਿਜੀ:
	// output width of the function node.
      ਬਿਨਾਨਿਸ਼ਾਨੀ owid_;
	// The vvp_net_t* objects for the function input ports. We use
	// these to write the input values to the reg input variable
	// functors for the thread.
      vvp_net_t**ports_;

	// This is a thread to execute the behavioral portion of the
	// function.
      vthread_t thread_;
      __vpiScope*call_scope_;
      __vpiScope*func_scope_;
      vvp_ਕੋਡ_t ਕੋਡ_2;
};

#ਜੇ_ਅੰਤ /* IVL_ufunc_H */
