/*
 * Copyright (c) 2006-2012 Stephen Williams (steve@icarus.com)
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
# include  "config.h"
#اگر_تعریف_شدہ CHECK_WITH_VALGRIND
# include "vvp_cleanup.h"
#اگر_ختم
# include  "sfunc.h"
# include  <cstdlib>
# include  <cstring>
# include  <iostream>
# include  <cassert>


sfunc_core::sfunc_core(vvp_net_t*net, vpiHandle sys,
		       غیرعلامتی argc, vpiHandle*argv)
: vvp_wide_fun_core(net, argc)
{
      sys_  = sys;
      argc_ = argc;
      argv_ = argv;
}

sfunc_core::~sfunc_core()
{
      حذف sys_;
#اگر_تعریف_شدہ CHECK_WITH_VALGRIND
      برائے (غیرعلامتی i = 0; i < argc_; i += 1) {
	    constant_delete(argv_[i]);
      }
#اگر_ختم
      حذف [] argv_;
}

/*
 * This method is only called when a trigger event occurs. Just arrange for
 * the function to be called.
 */
خالی sfunc_core::recv_vec4(vvp_net_ptr_t, ثابت vvp_vector4_t&/*ثنائی_ہندسہ*/,
                           vvp_context_t)
{
      جدول_وقت_functor(یہ);
}

خالی sfunc_core::recv_vec4_from_inputs(غیرعلامتی پورٹ)
{
      vpiHandle vpi = argv_[پورٹ];
      ساخت __vpiBinaryConst*obj = dynamic_cast<__vpiBinaryConst*>(vpi);
      أكد(obj);

      obj->بٹ = value(پورٹ);

        /* Schedule the actual call after this finishes. */
      جدول_وقت_functor(یہ);
}

خالی sfunc_core::recv_real_from_inputs(غیرعلامتی پورٹ)
{
      vpiHandle vpi = argv_[پورٹ];
      __vpiRealConst*obj = dynamic_cast<__vpiRealConst*>(vpi);
      أكد(obj);

      obj->value = قدر_r(پورٹ);

        /* Schedule the actual call after this finishes. */
      جدول_وقت_functor(یہ);
}

خالی sfunc_core::تسلسل_تسلسل()
{
      vpip_execute_vpi_call(0, sys_);
}

ساکن صحیح make_vpi_argv(غیرعلامتی argc, vpiHandle*vpi_argv,
			ثابت حرف*arg_string)
{
      غیرعلامتی idx = 0;
      ثابت حرف*cp = arg_string;
      صحیح return_type = 0;

      انتخاب (*cp) {
	  حالت 'r': // real result
	    cp += 1;
	    return_type = -vpiRealVal;
	    توڑو;

	  حالت 'v': // vector4_t
	    cp += 1;
	    return_type = strtoul(cp, 0, 10);
	    cp += strspn(cp, "0123456789");
	    توڑو;

	  طےشدہ:
	    fprintf(stderr, "Unsupported type %c(%d).\n", *cp, *cp);
	    أكد(0);
	    توڑو;
      }

      جب (*cp) {
	    أكد(idx < argc);

	    انتخاب (*cp) {
		حالت 'r': // real
		  cp += 1;
		  vpi_argv[idx] = vpip_make_real_const(0.0);
		  توڑو;

		حالت 'v': { // vector4_t (v<n>)
		      cp += 1;
		      غیرعلامتی wid = strtoul(cp, 0, 10);
		      cp += strspn(cp, "0123456789");
		      vpi_argv[idx] = vpip_make_ثنائی_const(wid, "x");
		      توڑو;
		}

		طےشدہ:
		  fprintf(stderr, "Unsupported type %c(%d).\n", *cp, *cp);
		  أكد(0);
	    }
	    idx += 1;
      }

      أكد(idx == argc);
      واپس return_type;
}


خالی compile_sfunc(حرف*label, حرف*name,  حرف*format_string,
		   طویل file_idx, طویل lineno,
		   غیرعلامتی argc, ساخت symb_s*argv,
                   حرف*trigger_label)
{
      غیرعلامتی vec4_ڈھیر = 0;
      غیرعلامتی real_ڈھیر = 0;
      غیرعلامتی string_ڈھیر = 0;
      vpiHandle*vpi_argv = نیا vpiHandle[argc];
      صحیح val_رمز = make_vpi_argv(argc, vpi_argv, format_string);
      غیرعلامتی val_چوڑائی = 0;
      حذف[] format_string;

	// The make_vpi_argv returns for the function return value a
	// >0 value for the vector width if this is a vector. Convert
	// it to the form that the vpip_build_vpi_call uses.
      اگر (val_رمز > 0) {
	    val_چوڑائی = val_رمز;
	    val_رمز = -vpi_سمتیہ_Val;
      }

      vvp_net_t*ptr = نیا vvp_net_t;

      vpiHandle sys = vpip_build_vpi_call(name, val_رمز, val_چوڑائی, ptr,
                                          سچ, جھوٹ, argc, vpi_argv,
					  vec4_ڈھیر, real_ڈھیر, string_ڈھیر,
                                          file_idx, lineno);
      أكد(sys);

	/* Create and connect the functor to the label. */
      sfunc_core*score = نیا sfunc_core(ptr, sys, argc, vpi_argv);
      ptr->fun = score;
      define_functor_علامت(label, ptr);
      آزاد(label);

	/* Link the inputs to the functor. */
      wide_inputs_connect(score, argc, argv);
      آزاد(argv);

        /* If this function has a trigger event, connect the functor to
           that event. */
      اگر (trigger_label)
            داخلہ_connect(ptr, 0, trigger_label);
      حذف[] name;
}
