#ifndef IVL_ufunc_H
#define IVL_ufunc_H
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

class ufunc_core : सार्वजनिक vvp_wide_fun_core {

    सार्वजनिक:
      ufunc_core(अचिन्हित ow, vvp_net_t*ptr,
		 अचिन्हित nports, vvp_net_t**ports,
		 vvp_code_t start_address,
		 __vpiScope*call_scope,
		 वर्ण*scope_label);
      आभास ~ufunc_core() override =0;

      __vpiScope*call_scope() { प्रत्यागम call_scope_; }
      __vpiScope*func_scope() { प्रत्यागम func_scope_; }

      शून्य assign_bits_to_ports(vvp_context_t context);
      आभास शून्य finish_thread() =0;

      शून्य recv_vec4(vvp_net_ptr_t port, ध्रुव vvp_vector4_t&bit,
                     vvp_context_t context) override;

    रक्षित:
      शून्य finish_thread_real_();
      शून्य finish_thread_vec4_();

    गुप्त:
      शून्य recv_vec4_from_inputs(अचिन्हित port) override;
      शून्य recv_real_from_inputs(अचिन्हित port) override;

      शून्य invoke_thread_(शून्य);


    गुप्त:
	// output width of the function node.
      अचिन्हित owid_;
	// The vvp_net_t* objects for the function input ports. We use
	// these to write the input values to the reg input variable
	// functors for the thread.
      vvp_net_t**ports_;

	// This is a thread to execute the behavioral portion of the
	// function.
      vthread_t thread_;
      __vpiScope*call_scope_;
      __vpiScope*func_scope_;
      vvp_code_t code_;
};

#endif /* IVL_ufunc_H */
