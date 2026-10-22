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
#اگر_تعریف_شده CHECK_WITH_VALGRIND
# include "vvp_cleanup.h"
#پایان_اگر
# include  "sfunc.h"
# include  <cstdlib>
# include  <cstring>
# include  <iostream>
# include  <cassert>


sfunc_core::sfunc_core(vvp_net_t*net, vpiHandle sys,
		       بیعلامت argc, vpiHandle*argv)
: vvp_wide_fun_core(net, argc)
{
      sys_  = sys;
      argc_ = argc;
      argv_ = argv;
}

sfunc_core::~sfunc_core()
{
      حذف sys_;
#اگر_تعریف_شده CHECK_WITH_VALGRIND
      لكل (بیعلامت i = 0; i < argc_; i += 1) {
	    constant_delete(argv_[i]);
      }
#پایان_اگر
      حذف [] argv_;
}

/*
 * This method is only called when a trigger event occurs. Just arrange for
 * the function to be called.
 */
فراغ sfunc_core::recv_vec4(vvp_net_ptr_t, ثابت vvp_vector4_t&/*بیت*/,
                           vvp_context_t)
{
      زمانبندی_functor(هذا);
}

فراغ sfunc_core::recv_vec4_from_inputs(بیعلامت پورت)
{
      vpiHandle vpi = argv_[پورت];
      بنية __vpiBinaryConst*obj = dynamic_cast<__vpiBinaryConst*>(vpi);
      أكد(obj);

      obj->بیتها = value(پورت);

        /* Schedule the actual call after this finishes. */
      زمانبندی_functor(هذا);
}

فراغ sfunc_core::recv_real_from_inputs(بیعلامت پورت)
{
      vpiHandle vpi = argv_[پورت];
      __vpiRealConst*obj = dynamic_cast<__vpiRealConst*>(vpi);
      أكد(obj);

      obj->value = مقدار_r(پورت);

        /* Schedule the actual call after this finishes. */
      زمانبندی_functor(هذا);
}

فراغ sfunc_core::پیاپی_پیاپی()
{
      vpip_execute_vpi_call(0, sys_);
}

ساكن صحيح make_vpi_argv(بیعلامت argc, vpiHandle*vpi_argv,
			ثابت حرف*arg_string)
{
      بیعلامت idx = 0;
      ثابت حرف*cp = arg_string;
      صحيح return_type = 0;

      بدل (*cp) {
	  حالة 'r': // real result
	    cp += 1;
	    return_type = -vpiRealVal;
	    اكسر;

	  حالة 'v': // vector4_t
	    cp += 1;
	    return_type = strtoul(cp, 0, 10);
	    cp += strspn(cp, "0123456789");
	    اكسر;

	  افتراضي:
	    fprintf(stderr, "Unsupported type %c(%d).\n", *cp, *cp);
	    أكد(0);
	    اكسر;
      }

      طالما (*cp) {
	    أكد(idx < argc);

	    بدل (*cp) {
		حالة 'r': // real
		  cp += 1;
		  vpi_argv[idx] = vpip_make_real_const(0.0);
		  اكسر;

		حالة 'v': { // vector4_t (v<n>)
		      cp += 1;
		      بیعلامت wid = strtoul(cp, 0, 10);
		      cp += strspn(cp, "0123456789");
		      vpi_argv[idx] = vpip_make_دودویی_const(wid, "x");
		      اكسر;
		}

		افتراضي:
		  fprintf(stderr, "Unsupported type %c(%d).\n", *cp, *cp);
		  أكد(0);
	    }
	    idx += 1;
      }

      أكد(idx == argc);
      أرجع return_type;
}


فراغ compile_sfunc(حرف*label, حرف*name,  حرف*format_string,
		   دراز file_idx, دراز lineno,
		   بیعلامت argc, بنية symb_s*argv,
                   حرف*trigger_label)
{
      بیعلامت vec4_پشته = 0;
      بیعلامت real_پشته = 0;
      بیعلامت string_پشته = 0;
      vpiHandle*vpi_argv = جديد vpiHandle[argc];
      صحيح val_کد = make_vpi_argv(argc, vpi_argv, format_string);
      بیعلامت val_عرض = 0;
      حذف[] format_string;

	// The make_vpi_argv returns for the function return value a
	// >0 value for the vector width if this is a vector. Convert
	// it to the form that the vpip_build_vpi_call uses.
      إذا (val_کد > 0) {
	    val_عرض = val_کد;
	    val_کد = -vpi_وکتور_Val;
      }

      vvp_net_t*ptr = جديد vvp_net_t;

      vpiHandle sys = vpip_build_vpi_call(name, val_کد, val_عرض, ptr,
                                          درست, خطأ, argc, vpi_argv,
					  vec4_پشته, real_پشته, string_پشته,
                                          file_idx, lineno);
      أكد(sys);

	/* Create and connect the functor to the label. */
      sfunc_core*score = جديد sfunc_core(ptr, sys, argc, vpi_argv);
      ptr->fun = score;
      define_functor_نماد(label, ptr);
      حرر(label);

	/* Link the inputs to the functor. */
      wide_inputs_connect(score, argc, argv);
      حرر(argv);

        /* If this function has a trigger event, connect the functor to
           that event. */
      إذا (trigger_label)
            درون_داد_connect(ptr, 0, trigger_label);
      حذف[] name;
}
