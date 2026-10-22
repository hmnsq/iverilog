#جيڪڏهن_وصف_نه_ٿيل IVL_ufunc_H
#وصف_ڪريو IVL_ufunc_H
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

class ufunc_core : عام vvp_wide_fun_core {

    عام:
      ufunc_core(بنانشان ow, vvp_net_t*ptr,
		 بنانشان nports, vvp_net_t**ports,
		 vvp_ڪوڊ_t شروعات_پتو,
		 __vpiScope*call_scope,
		 حرف*scope_label);
      مجازي ~ufunc_core() override =0;

      __vpiScope*call_scope() { أرجع call_scope_; }
      __vpiScope*func_scope() { أرجع func_scope_; }

      فراغ سونپيو_بٽ_منزل_ports(vvp_context_t context);
      مجازي فراغ پڄاڻي_thread() =0;

      فراغ recv_vec4(vvp_net_ptr_t پورٽ, ثابت vvp_vector4_t&ٻٽو_انگ,
                     vvp_context_t context) override;

    محمي:
      فراغ پڄاڻي_thread_real();
      فراغ پڄاڻي_thread_vec4();

    خاص:
      فراغ recv_vec4_from_inputs(بنانشان پورٽ) override;
      فراغ recv_real_from_inputs(بنانشان پورٽ) override;

      فراغ invoke_thread_(فراغ);


    خاص:
	// output width of the function node.
      بنانشان owid_;
	// The vvp_net_t* objects for the function input ports. We use
	// these to write the input values to the reg input variable
	// functors for the thread.
      vvp_net_t**ports_;

	// This is a thread to execute the behavioral portion of the
	// function.
      vthread_t thread_;
      __vpiScope*call_scope_;
      __vpiScope*func_scope_;
      vvp_ڪوڊ_t ڪوڊ_2;
};

#جيڪڏهن_پورو /* IVL_ufunc_H */
