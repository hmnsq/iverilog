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
      বাবে (unsigned i = 0; i < argc_; i += 1) {
	    constant_delete(argv_[i]);
      }
#endif
      delete [] argv_;
}

/*
 * This method is only called when a trigger event occurs. Just arrange for
 * the function to be called.
 */
শূন্য sfunc_core::recv_vec4(vvp_net_ptr_t, ধ্রুৱ vvp_vector4_t&/*bit*/,
                           vvp_context_t)
{
      schedule_functor(এতত্);
}

শূন্য sfunc_core::recv_vec4_from_inputs(unsigned port)
{
      vpiHandle vpi = argv_[port];
      সংরচনা __vpiBinaryConst*obj = dynamic_cast<__vpiBinaryConst*>(vpi);
      প্রতিজ্ঞা(obj);

      obj->bits = value(port);

        /* Schedule the actual call after this finishes. */
      schedule_functor(এতত্);
}

শূন্য sfunc_core::recv_real_from_inputs(unsigned port)
{
      vpiHandle vpi = argv_[port];
      __vpiRealConst*obj = dynamic_cast<__vpiRealConst*>(vpi);
      প্রতিজ্ঞা(obj);

      obj->value = মান_r(port);

        /* Schedule the actual call after this finishes. */
      schedule_functor(এতত্);
}

শূন্য sfunc_core::run_run()
{
      vpip_execute_vpi_call(0, sys_);
}

স্থির পূর্ণাঙ্ক make_vpi_argv(unsigned argc, vpiHandle*vpi_argv,
			ধ্রুৱ ৱর্ণ*arg_string)
{
      unsigned idx = 0;
      ধ্রুৱ ৱর্ণ*cp = arg_string;
      পূর্ণাঙ্ক return_type = 0;

      ৱিকল্প (*cp) {
	  পক্ষ 'r': // real result
	    cp += 1;
	    return_type = -vpiRealVal;
	    ভঙ্গ;

	  পক্ষ 'v': // vector4_t
	    cp += 1;
	    return_type = strtoul(cp, 0, 10);
	    cp += strspn(cp, "0123456789");
	    ভঙ্গ;

	  স্ৱভাৱ:
	    fprintf(stderr, "Unsupported type %c(%d).\n", *cp, *cp);
	    প্রতিজ্ঞা(0);
	    ভঙ্গ;
      }

      যেতিয়ালৈ (*cp) {
	    প্রতিজ্ঞা(idx < argc);

	    ৱিকল্প (*cp) {
		পক্ষ 'r': // real
		  cp += 1;
		  vpi_argv[idx] = vpip_make_real_const(0.0);
		  ভঙ্গ;

		পক্ষ 'v': { // vector4_t (v<n>)
		      cp += 1;
		      unsigned wid = strtoul(cp, 0, 10);
		      cp += strspn(cp, "0123456789");
		      vpi_argv[idx] = vpip_make_দ্বিমিক_const(wid, "x");
		      ভঙ্গ;
		}

		স্ৱভাৱ:
		  fprintf(stderr, "Unsupported type %c(%d).\n", *cp, *cp);
		  প্রতিজ্ঞা(0);
	    }
	    idx += 1;
      }

      প্রতিজ্ঞা(idx == argc);
      ঘূৰাই return_type;
}


শূন্য compile_sfunc(ৱর্ণ*label, ৱর্ণ*name,  ৱর্ণ*format_string,
		   long file_idx, long lineno,
		   unsigned argc, সংরচনা symb_s*argv,
                   ৱর্ণ*trigger_label)
{
      unsigned vec4_স্তূপ = 0;
      unsigned real_স্তূপ = 0;
      unsigned string_স্তূপ = 0;
      vpiHandle*vpi_argv = নূতন vpiHandle[argc];
      পূর্ণাঙ্ক val_code = make_vpi_argv(argc, vpi_argv, format_string);
      unsigned val_width = 0;
      delete[] format_string;

	// The make_vpi_argv returns for the function return value a
	// >0 value for the vector width if this is a vector. Convert
	// it to the form that the vpip_build_vpi_call uses.
      যদি (val_code > 0) {
	    val_width = val_code;
	    val_code = -vpiVectorVal;
      }

      vvp_net_t*ptr = নূতন vvp_net_t;

      vpiHandle sys = vpip_build_vpi_call(name, val_code, val_width, ptr,
                                          সঁচা, মিছা, argc, vpi_argv,
					  vec4_স্তূপ, real_স্তূপ, string_স্তূপ,
                                          file_idx, lineno);
      প্রতিজ্ঞা(sys);

	/* Create and connect the functor to the label. */
      sfunc_core*score = নূতন sfunc_core(ptr, sys, argc, vpi_argv);
      ptr->fun = score;
      define_functor_প্ৰতীক(label, ptr);
      মুক্ত(label);

	/* Link the inputs to the functor. */
      wide_inputs_connect(score, argc, argv);
      মুক্ত(argv);

        /* If this function has a trigger event, connect the functor to
           that event. */
      যদি (trigger_label)
            প্ৰবেশ_connect(ptr, 0, trigger_label);
      delete[] name;
}
