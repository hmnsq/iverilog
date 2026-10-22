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
#ifdef CHECK_WITH_VALGRIND
# include "vvp_cleanup.h"
#endif
# include  "sfunc.h"
# include  <cstdlib>
# include  <cstring>
# include  <iostream>
# include  <cassert>


sfunc_core::sfunc_core(vvp_net_t*net, vpiHandle sys,
		       unsigned argc, vpiHandle*argv)
: vvp_wide_fun_core(net, argc)
{
      sys_  = sys;
      argc_ = argc;
      argv_ = argv;
}

sfunc_core::~sfunc_core()
{
      delete sys_;
#ifdef CHECK_WITH_VALGRIND
      জন্য (unsigned i = 0; i < argc_; i += 1) {
	    constant_delete(argv_[i]);
      }
#endif
      delete [] argv_;
}

/*
 * This method is only called when a trigger event occurs. Just arrange for
 * the function to be called.
 */
শূন্য sfunc_core::recv_vec4(vvp_net_ptr_t, ধ্রুবক vvp_vector4_t&/*bit*/,
                           vvp_context_t)
{
      schedule_functor(এই);
}

শূন্য sfunc_core::recv_vec4_from_inputs(unsigned port)
{
      vpiHandle vpi = argv_[port];
      কাঠামো __vpiBinaryConst*obj = dynamic_cast<__vpiBinaryConst*>(vpi);
      প্রতিজ্ঞা(obj);

      obj->bits = value(port);

        /* Schedule the actual call after this finishes. */
      schedule_functor(এই);
}

শূন্য sfunc_core::recv_real_from_inputs(unsigned port)
{
      vpiHandle vpi = argv_[port];
      __vpiRealConst*obj = dynamic_cast<__vpiRealConst*>(vpi);
      প্রতিজ্ঞা(obj);

      obj->value = মান_r(port);

        /* Schedule the actual call after this finishes. */
      schedule_functor(এই);
}

শূন্য sfunc_core::run_run()
{
      vpip_execute_vpi_call(0, sys_);
}

স্থির পূর্ণসংখ্যা make_vpi_argv(unsigned argc, vpiHandle*vpi_argv,
			ধ্রুবক অক্ষর*arg_string)
{
      unsigned idx = 0;
      ধ্রুবক অক্ষর*cp = arg_string;
      পূর্ণসংখ্যা return_type = 0;

      নির্বাচন (*cp) {
	  ক্ষেত্র 'r': // real result
	    cp += 1;
	    return_type = -vpiRealVal;
	    বিরতি;

	  ক্ষেত্র 'v': // vector4_t
	    cp += 1;
	    return_type = strtoul(cp, 0, 10);
	    cp += strspn(cp, "0123456789");
	    বিরতি;

	  পূর্বনির্ধারিত:
	    fprintf(stderr, "Unsupported type %c(%d).\n", *cp, *cp);
	    প্রতিজ্ঞা(0);
	    বিরতি;
      }

      যখন (*cp) {
	    প্রতিজ্ঞা(idx < argc);

	    নির্বাচন (*cp) {
		ক্ষেত্র 'r': // real
		  cp += 1;
		  vpi_argv[idx] = vpip_make_real_const(0.0);
		  বিরতি;

		ক্ষেত্র 'v': { // vector4_t (v<n>)
		      cp += 1;
		      unsigned wid = strtoul(cp, 0, 10);
		      cp += strspn(cp, "0123456789");
		      vpi_argv[idx] = vpip_make_দ্বিমিক_const(wid, "x");
		      বিরতি;
		}

		পূর্বনির্ধারিত:
		  fprintf(stderr, "Unsupported type %c(%d).\n", *cp, *cp);
		  প্রতিজ্ঞা(0);
	    }
	    idx += 1;
      }

      প্রতিজ্ঞা(idx == argc);
      ফেরত return_type;
}


শূন্য compile_sfunc(অক্ষর*label, অক্ষর*name,  অক্ষর*format_string,
		   long file_idx, long lineno,
		   unsigned argc, কাঠামো symb_s*argv,
                   অক্ষর*trigger_label)
{
      unsigned vec4_স্তূপ = 0;
      unsigned real_স্তূপ = 0;
      unsigned string_স্তূপ = 0;
      vpiHandle*vpi_argv = নতুন vpiHandle[argc];
      পূর্ণসংখ্যা val_code = make_vpi_argv(argc, vpi_argv, format_string);
      unsigned val_width = 0;
      delete[] format_string;

	// The make_vpi_argv returns for the function return value a
	// >0 value for the vector width if this is a vector. Convert
	// it to the form that the vpip_build_vpi_call uses.
      যদি (val_code > 0) {
	    val_width = val_code;
	    val_code = -vpiVectorVal;
      }

      vvp_net_t*ptr = নতুন vvp_net_t;

      vpiHandle sys = vpip_build_vpi_call(name, val_code, val_width, ptr,
                                          সত্য, মিথ্যা, argc, vpi_argv,
					  vec4_স্তূপ, real_স্তূপ, string_স্তূপ,
                                          file_idx, lineno);
      প্রতিজ্ঞা(sys);

	/* Create and connect the functor to the label. */
      sfunc_core*score = নতুন sfunc_core(ptr, sys, argc, vpi_argv);
      ptr->fun = score;
      define_functor_প্রতীক(label, ptr);
      মুক্ত(label);

	/* Link the inputs to the functor. */
      wide_inputs_connect(score, argc, argv);
      মুক্ত(argv);

        /* If this function has a trigger event, connect the functor to
           that event. */
      যদি (trigger_label)
            প্রবেশ_connect(ptr, 0, trigger_label);
      delete[] name;
}
