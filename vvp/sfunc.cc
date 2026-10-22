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
#جيڪڏهن_وصف_ٿيل CHECK_WITH_VALGRIND
# include "vvp_cleanup.h"
#جيڪڏهن_پورو
# include  "sfunc.h"
# include  <cstdlib>
# include  <cstring>
# include  <iostream>
# include  <cassert>


sfunc_core::sfunc_core(vvp_net_t*net, vpiHandle sys,
		       بنانشان argc, vpiHandle*argv)
: vvp_wide_fun_core(net, argc)
{
      sys_  = sys;
      argc_ = argc;
      argv_ = argv;
}

sfunc_core::~sfunc_core()
{
      ميٽيو sys_;
#جيڪڏهن_وصف_ٿيل CHECK_WITH_VALGRIND
      لكل (بنانشان i = 0; i < argc_; i += 1) {
	    constant_delete(argv_[i]);
      }
#جيڪڏهن_پورو
      ميٽيو [] argv_;
}

/*
 * This method is only called when a trigger event occurs. Just arrange for
 * the function to be called.
 */
فراغ sfunc_core::recv_vec4(vvp_net_ptr_t, ثابت vvp_vector4_t&/*ٻٽو_انگ*/,
                           vvp_context_t)
{
      وقت_جدول_functor(هذا);
}

فراغ sfunc_core::recv_vec4_from_inputs(بنانشان پورٽ)
{
      vpiHandle vpi = argv_[پورٽ];
      بنية __vpiBinaryConst*obj = dynamic_cast<__vpiBinaryConst*>(vpi);
      أكد(obj);

      obj->بٽ = value(پورٽ);

        /* Schedule the actual call after this finishes. */
      وقت_جدول_functor(هذا);
}

فراغ sfunc_core::recv_real_from_inputs(بنانشان پورٽ)
{
      vpiHandle vpi = argv_[پورٽ];
      __vpiRealConst*obj = dynamic_cast<__vpiRealConst*>(vpi);
      أكد(obj);

      obj->value = قدر_r(پورٽ);

        /* Schedule the actual call after this finishes. */
      وقت_جدول_functor(هذا);
}

فراغ sfunc_core::لاڳيتو_لاڳيتو()
{
      vpip_execute_vpi_call(0, sys_);
}

ساكن صحيح make_vpi_argv(بنانشان argc, vpiHandle*vpi_argv,
			ثابت حرف*arg_string)
{
      بنانشان idx = 0;
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
		      بنانشان wid = strtoul(cp, 0, 10);
		      cp += strspn(cp, "0123456789");
		      vpi_argv[idx] = vpip_make_ٻٽو_const(wid, "x");
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
		   ڊگهو file_idx, ڊگهو lineno,
		   بنانشان argc, بنية symb_s*argv,
                   حرف*trigger_label)
{
      بنانشان vec4_ڍير = 0;
      بنانشان real_ڍير = 0;
      بنانشان string_ڍير = 0;
      vpiHandle*vpi_argv = جديد vpiHandle[argc];
      صحيح val_ڪوڊ = make_vpi_argv(argc, vpi_argv, format_string);
      بنانشان val_ويڪر = 0;
      ميٽيو[] format_string;

	// The make_vpi_argv returns for the function return value a
	// >0 value for the vector width if this is a vector. Convert
	// it to the form that the vpip_build_vpi_call uses.
      إذا (val_ڪوڊ > 0) {
	    val_ويڪر = val_ڪوڊ;
	    val_ڪوڊ = -vpi_ويڪٽر_Val;
      }

      vvp_net_t*ptr = جديد vvp_net_t;

      vpiHandle sys = vpip_build_vpi_call(name, val_ڪوڊ, val_ويڪر, ptr,
                                          سچ, خطأ, argc, vpi_argv,
					  vec4_ڍير, real_ڍير, string_ڍير,
                                          file_idx, lineno);
      أكد(sys);

	/* Create and connect the functor to the label. */
      sfunc_core*score = جديد sfunc_core(ptr, sys, argc, vpi_argv);
      ptr->fun = score;
      define_functor_نشان(label, ptr);
      حرر(label);

	/* Link the inputs to the functor. */
      wide_inputs_connect(score, argc, argv);
      حرر(argv);

        /* If this function has a trigger event, connect the functor to
           that event. */
      إذا (trigger_label)
            داخلا_connect(ptr, 0, trigger_label);
      ميٽيو[] name;
}
