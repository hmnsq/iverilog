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
      प्रति (unsigned i = 0; i < argc_; i += 1) {
	    constant_delete(argv_[i]);
      }
#endif
      delete [] argv_;
}

/*
 * This method is only called when a trigger event occurs. Just arrange for
 * the function to be called.
 */
शून्य sfunc_core::recv_vec4(vvp_net_ptr_t, ध्रुव vvp_vector4_t&/*bit*/,
                           vvp_context_t)
{
      schedule_functor(एतत्);
}

शून्य sfunc_core::recv_vec4_from_inputs(unsigned port)
{
      vpiHandle vpi = argv_[port];
      संरचना __vpiBinaryConst*obj = dynamic_cast<__vpiBinaryConst*>(vpi);
      प्रतिज्ञा(obj);

      obj->bits = value(port);

        /* Schedule the actual call after this finishes. */
      schedule_functor(एतत्);
}

शून्य sfunc_core::recv_real_from_inputs(unsigned port)
{
      vpiHandle vpi = argv_[port];
      __vpiRealConst*obj = dynamic_cast<__vpiRealConst*>(vpi);
      प्रतिज्ञा(obj);

      obj->value = मानम्_r(port);

        /* Schedule the actual call after this finishes. */
      schedule_functor(एतत्);
}

शून्य sfunc_core::run_run()
{
      vpip_execute_vpi_call(0, sys_);
}

स्थिर पूर्णाङ्क make_vpi_argv(unsigned argc, vpiHandle*vpi_argv,
			ध्रुव वर्ण*arg_string)
{
      unsigned idx = 0;
      ध्रुव वर्ण*cp = arg_string;
      पूर्णाङ्क return_type = 0;

      विकल्प (*cp) {
	  पक्ष 'r': // real result
	    cp += 1;
	    return_type = -vpiRealVal;
	    भङ्ग;

	  पक्ष 'v': // vector4_t
	    cp += 1;
	    return_type = strtoul(cp, 0, 10);
	    cp += strspn(cp, "0123456789");
	    भङ्ग;

	  स्वभाव:
	    fprintf(stderr, "Unsupported type %c(%d).\n", *cp, *cp);
	    प्रतिज्ञा(0);
	    भङ्ग;
      }

      जाव (*cp) {
	    प्रतिज्ञा(idx < argc);

	    विकल्प (*cp) {
		पक्ष 'r': // real
		  cp += 1;
		  vpi_argv[idx] = vpip_make_real_const(0.0);
		  भङ्ग;

		पक्ष 'v': { // vector4_t (v<n>)
		      cp += 1;
		      unsigned wid = strtoul(cp, 0, 10);
		      cp += strspn(cp, "0123456789");
		      vpi_argv[idx] = vpip_make_द्विमान_const(wid, "x");
		      भङ्ग;
		}

		स्वभाव:
		  fprintf(stderr, "Unsupported type %c(%d).\n", *cp, *cp);
		  प्रतिज्ञा(0);
	    }
	    idx += 1;
      }

      प्रतिज्ञा(idx == argc);
      प्रत्यागम return_type;
}


शून्य compile_sfunc(वर्ण*label, वर्ण*name,  वर्ण*format_string,
		   long file_idx, long lineno,
		   unsigned argc, संरचना symb_s*argv,
                   वर्ण*trigger_label)
{
      unsigned vec4_राशिः = 0;
      unsigned real_राशिः = 0;
      unsigned string_राशिः = 0;
      vpiHandle*vpi_argv = नूतन vpiHandle[argc];
      पूर्णाङ्क val_code = make_vpi_argv(argc, vpi_argv, format_string);
      unsigned val_width = 0;
      delete[] format_string;

	// The make_vpi_argv returns for the function return value a
	// >0 value for the vector width if this is a vector. Convert
	// it to the form that the vpip_build_vpi_call uses.
      जइ (val_code > 0) {
	    val_width = val_code;
	    val_code = -vpiVectorVal;
      }

      vvp_net_t*ptr = नूतन vvp_net_t;

      vpiHandle sys = vpip_build_vpi_call(name, val_code, val_width, ptr,
                                          सच्च, मिच्छ, argc, vpi_argv,
					  vec4_राशिः, real_राशिः, string_राशिः,
                                          file_idx, lineno);
      प्रतिज्ञा(sys);

	/* Create and connect the functor to the label. */
      sfunc_core*score = नूतन sfunc_core(ptr, sys, argc, vpi_argv);
      ptr->fun = score;
      define_functor_चिह्नम्(label, ptr);
      मुक्त(label);

	/* Link the inputs to the functor. */
      wide_inputs_connect(score, argc, argv);
      मुक्त(argv);

        /* If this function has a trigger event, connect the functor to
           that event. */
      जइ (trigger_label)
            निवेशः_connect(ptr, 0, trigger_label);
      delete[] name;
}
