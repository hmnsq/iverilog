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
#اگر_تعریف_شدہ CHECK_WITH_VALGRIND
# include  "vvp_cleanup.h"
#اگر_ختم
# include  <cstdlib>
# include  <cstring>
# include  <iostream>
# include  <cassert>
# include  <map>

#اگر_تعریف_شدہ __MINGW32__
#شامل <windows.h>
#اگر_ختم

جماعت ufunc_real : عوامی ufunc_core {
   عوامی:
      ufunc_real(غیرعلامتی ow, vvp_net_t*ptr,
		 غیرعلامتی nports, vvp_net_t**ports,
		 vvp_رمز_t آغاز_پتہ,
		 __vpiScope*call_scope,
		 حرف*scope_label);
      ~ufunc_real() override;

      خالی اختتام_thread() override;
};

جماعت ufunc_vec4 : عوامی ufunc_core {
   عوامی:
      ufunc_vec4(غیرعلامتی ow, vvp_net_t*ptr,
		 غیرعلامتی nports, vvp_net_t**ports,
		 vvp_رمز_t آغاز_پتہ,
		 __vpiScope*call_scope,
		 حرف*scope_label);
      ~ufunc_vec4() override;

      خالی اختتام_thread() override;
};

ufunc_core::ufunc_core(غیرعلامتی owid, vvp_net_t*ptr,
		       غیرعلامتی nports, vvp_net_t**ports,
		       vvp_رمز_t sa, __vpiScope*call_scope__,
		       حرف*scope_label)
: vvp_wide_fun_core(ptr, nports)
{
      owid_ = owid;
      ports_ = ports;
      رمز_2 = sa;
      thread_ = 0;
      call_scope_ = call_scope__;

	/* A __vpiScope starts with a __vpiHandle structure so this is
	   a safe cast. We need the (void*) to avoid a dereferenced
	   type punned pointer warning from some gcc compilers. */
      compile_vpi_lookup((vpiHandle*)(خالی*)(&func_scope_), scope_label);
}

ufunc_core::~ufunc_core()
{
      حذف [] ports_;
}

/*
 * This method is called by the %exec_ufunc function to prepare the
 * input variables of the function for execution. The method copies
 * the input values collected by the core to the variables.
 */
خالی ufunc_core::تفویض_بٹ_مقصد_ports(vvp_context_t context)
{
      برائے (غیرعلامتی idx = 0 ; idx < پورٹ_گنتی() ;  idx += 1) {
	    vvp_net_t*net = ports_[idx];
	    vvp_net_ptr_t pp (net, 0);

	      // If the port is a real variable, then simply copy the
	      // propagated input to the port variable.
	    اگر (vvp_fun_signal_real*tmp = dynamic_cast<vvp_fun_signal_real*>(net->fun))
		  tmp->recv_real(pp, قدر_r(idx), context);

	      // If the port is a bit4 vector, then copy the
	      // propagated input to the port variable. Detect the
	      // special case that the input vector is nil, and
	      // convert that to an 'bx vector that matches the width
	      // of the port variable. This is to handle the uncommon
	      // startup case where the input values have not
	      // propagated useful values yet.
	    اگر (vvp_fun_signal_vec*tmp = dynamic_cast<vvp_fun_signal_vec*>(net->fun)) {
		  ثابت vvp_vector4_t&tmp_val = value(idx);
		  اگر (tmp_val.size() == 0) {
			ثابت vvp_vector4_t&ref = tmp->vec4_unfiltered_قدر();
			vvp_vector4_t xxx (ref.size(), BIT4_X);
			tmp->recv_vec4(pp, xxx, context);
		  } ورنہ {
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
خالی ufunc_core::اختتام_thread_real()
{
      أكد(thread_);

      دوہرا val = vthread_get_real_ڈھیر(thread_, 0);
      vthread_نکالو_real(thread_, 1);

      propagate_real(val);

      thread_ = 0;
}

خالی ufunc_core::اختتام_thread_vec4()
{
      أكد(thread_);

      vvp_vector4_t val = vthread_get_vec4_ڈھیر(thread_, 0);
      vthread_نکالو_vec4(thread_, 1);

      propagate_vec4(val);

      thread_ = 0;
}

/*
 * This method is only called when a trigger event occurs. Just arrange for
 * the function to be called.
 */
خالی ufunc_core::recv_vec4(vvp_net_ptr_t, ثابت vvp_vector4_t&,
                           vvp_context_t)
{
      invoke_thread_();
}

/*
 * The recv_vec4 methods of the input functors call this to assign the
 * input value to the port of the functor. I save the input value and
 * arrange for the function to be called.
 */
خالی ufunc_core::recv_vec4_from_inputs(غیرعلامتی)
{
      invoke_thread_();
}

خالی ufunc_core::recv_real_from_inputs(غیرعلامتی)
{
      invoke_thread_();
}

خالی ufunc_core::invoke_thread_()
{
      اگر (thread_ == 0) {
	    thread_ = vthread_new(رمز_2, call_scope_);
	    جدول_وقت_vthread(thread_, 0);
      }
}

ufunc_vec4::ufunc_vec4(غیرعلامتی ow, vvp_net_t*ptr,
		       غیرعلامتی nports, vvp_net_t**ports,
		       vvp_رمز_t آغاز_پتہ,
		       __vpiScope*call_scope_اندر,
		       حرف*scope_label)
: ufunc_core(ow, ptr, nports, ports, آغاز_پتہ, call_scope_اندر, scope_label)
{
}

ufunc_vec4::~ufunc_vec4()
{
}

خالی ufunc_vec4::اختتام_thread()
{
      اختتام_thread_vec4();
}

ufunc_real::ufunc_real(غیرعلامتی ow, vvp_net_t*ptr,
		       غیرعلامتی nports, vvp_net_t**ports,
		       vvp_رمز_t آغاز_پتہ,
		       __vpiScope*call_scope_اندر,
		       حرف*scope_label)
: ufunc_core(ow, ptr, nports, ports, آغاز_پتہ, call_scope_اندر, scope_label)
{
}

ufunc_real::~ufunc_real()
{
}

خالی ufunc_real::اختتام_thread()
{
      اختتام_thread_real();
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
خالی compile_ufunc_real(حرف*label, حرف*code, غیرعلامتی wid,
		   غیرعلامتی argc,  ساخت symb_s*argv,
		   غیرعلامتی portc, ساخت symb_s*portv,
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

      vvp_رمز_t exec_رمز = codespace_allocate();
      exec_رمز->opcode = کا_EXEC_UFUNC_REAL;
      رمز_label_lookup(exec_رمز, code, جھوٹ);

      vvp_رمز_t reap_رمز = codespace_allocate();
      reap_رمز->opcode = کا_REAP_UFUNC;

      vvp_رمز_t انتہا_رمز = codespace_allocate();
      انتہا_رمز->opcode = &کا_انتہا;

	/* Run through the function ports (which are related to but
	   not the same as the input ports) and arrange for their
	   binding. */
      vvp_net_t**ports = نیا vvp_net_t*[portc];
      برائے (غیرعلامتی idx = 0 ;  idx < portc ;  idx += 1) {
	    functor_ref_lookup(&ports[idx], portv[idx].متن);
      }

	/* Create the output functor and attach it to the label. Tell
	   it about the start address of the code stub, and the scope
	   that will contain the execution. */
      vvp_net_t*ptr = نیا vvp_net_t;
      ufunc_core*fcore = نیا ufunc_real(wid, ptr, portc, ports,
					exec_رمز, call_scope,
					scope_label);
      ptr->fun = fcore;
      define_functor_علامت(label, ptr);
      آزاد(label);

      exec_رمز->ufunc_core_ptr = fcore;
      reap_رمز->ufunc_core_ptr = fcore;

      wide_inputs_connect(fcore, argc, argv);

        /* If this function has a trigger event, connect the functor to
           that event. */
      اگر (trigger_label)
            داخلہ_connect(ptr, 0, trigger_label);

      آزاد(argv);
      آزاد(portv);
}

خالی compile_ufunc_vec4(حرف*label, حرف*code, غیرعلامتی wid,
		   غیرعلامتی argc,  ساخت symb_s*argv,
		   غیرعلامتی portc, ساخت symb_s*portv,
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

      vvp_رمز_t exec_رمز = codespace_allocate();
      exec_رمز->opcode = کا_EXEC_UFUNC_VEC4;
      رمز_label_lookup(exec_رمز, code, جھوٹ);

      vvp_رمز_t reap_رمز = codespace_allocate();
      reap_رمز->opcode = کا_REAP_UFUNC;

      vvp_رمز_t انتہا_رمز = codespace_allocate();
      انتہا_رمز->opcode = &کا_انتہا;

	/* Run through the function ports (which are related to but
	   not the same as the input ports) and arrange for their
	   binding. */
      vvp_net_t**ports = نیا vvp_net_t*[portc];
      برائے (غیرعلامتی idx = 0 ;  idx < portc ;  idx += 1) {
	    functor_ref_lookup(&ports[idx], portv[idx].متن);
      }

	/* Create the output functor and attach it to the label. Tell
	   it about the start address of the code stub, and the scope
	   that will contain the execution. */
      vvp_net_t*ptr = نیا vvp_net_t;
      ufunc_core*fcore = نیا ufunc_vec4(wid, ptr, portc, ports,
					exec_رمز, call_scope,
					scope_label);
      ptr->fun = fcore;
      define_functor_علامت(label, ptr);
      آزاد(label);

      exec_رمز->ufunc_core_ptr = fcore;
      reap_رمز->ufunc_core_ptr = fcore;

      wide_inputs_connect(fcore, argc, argv);

        /* If this function has a trigger event, connect the functor to
           that event. */
      اگر (trigger_label)
            داخلہ_connect(ptr, 0, trigger_label);

      آزاد(argv);
      آزاد(portv);
}
#اگر_تعریف_شدہ CHECK_WITH_VALGRIND
ساکن std::map<ufunc_core*, منطقی> ufunc_map;

خالی exec_ufunc_delete(vvp_رمز_t euf_رمز)
{
      ufunc_map[euf_رمز->ufunc_core_ptr] = سچ;
}

خالی ufunc_pool_delete(خالی)
{
      std::map<ufunc_core*, منطقی>::iterator iter;
      برائے (iter = ufunc_map.begin(); iter != ufunc_map.انتہا(); ++ iter ) {
	    حذف iter->پہلا;
      }
}
#اگر_ختم
