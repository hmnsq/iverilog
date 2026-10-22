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
		       अचिन्हित argc, vpiHandle*argv)
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
      साठी (अचिन्हित i = 0; i < argc_; i += 1) {
	    constant_delete(argv_[i]);
      }
#endif
      delete [] argv_;
}

/*
 * This method is only called when a trigger event occurs. Just arrange for
 * the function to be called.
 */
शून्य sfunc_core::recv_vec4(vvp_net_ptr_t, स्थिरांक vvp_vector4_t&/*bit*/,
                           vvp_context_t)
{
      schedule_functor(हे);
}

शून्य sfunc_core::recv_vec4_from_inputs(अचिन्हित port)
{
      vpiHandle vpi = argv_[port];
      संरचना __vpiBinaryConst*obj = dynamic_cast<__vpiBinaryConst*>(vpi);
      प्रतिज्ञा(obj);

      obj->bits = value(port);

        /* Schedule the actual call after this finishes. */
      schedule_functor(हे);
}

शून्य sfunc_core::recv_real_from_inputs(अचिन्हित port)
{
      vpiHandle vpi = argv_[port];
      __vpiRealConst*obj = dynamic_cast<__vpiRealConst*>(vpi);
      प्रतिज्ञा(obj);

      obj->value = मूल्य_r(port);

        /* Schedule the actual call after this finishes. */
      schedule_functor(हे);
}

शून्य sfunc_core::run_run()
{
      vpip_execute_vpi_call(0, sys_);
}

स्थिर पूर्णांक make_vpi_argv(अचिन्हित argc, vpiHandle*vpi_argv,
			स्थिरांक अक्षर*arg_string)
{
      अचिन्हित idx = 0;
      स्थिरांक अक्षर*cp = arg_string;
      पूर्णांक return_type = 0;

      निवड (*cp) {
	  स्थिती 'r': // real result
	    cp += 1;
	    return_type = -vpiRealVal;
	    थांबा;

	  स्थिती 'v': // vector4_t
	    cp += 1;
	    return_type = म_से_अदीर्घ(cp, 0, 10);
	    cp += म_अखोज(cp, "0123456789");
	    थांबा;

	  पूर्वनिर्धारित:
	    ख_लिखो(मानक_त्रुटि, "Unsupported type %c(%d).\n", *cp, *cp);
	    प्रतिज्ञा(0);
	    थांबा;
      }

      पर्यंत (*cp) {
	    प्रतिज्ञा(idx < argc);

	    निवड (*cp) {
		स्थिती 'r': // real
		  cp += 1;
		  vpi_argv[idx] = vpip_make_real_const(0.0);
		  थांबा;

		स्थिती 'v': { // vector4_t (v<n>)
		      cp += 1;
		      अचिन्हित wid = म_से_अदीर्घ(cp, 0, 10);
		      cp += म_अखोज(cp, "0123456789");
		      vpi_argv[idx] = vpip_make_द्विमान_const(wid, "x");
		      थांबा;
		}

		पूर्वनिर्धारित:
		  ख_लिखो(मानक_त्रुटि, "Unsupported type %c(%d).\n", *cp, *cp);
		  प्रतिज्ञा(0);
	    }
	    idx += 1;
      }

      प्रतिज्ञा(idx == argc);
      परत return_type;
}


शून्य compile_sfunc(अक्षर*label, अक्षर*name,  अक्षर*format_string,
		   दीर्घ file_idx, दीर्घ lineno,
		   अचिन्हित argc, संरचना symb_s*argv,
                   अक्षर*trigger_label)
{
      अचिन्हित vec4_चळत = 0;
      अचिन्हित real_चळत = 0;
      अचिन्हित string_चळत = 0;
      vpiHandle*vpi_argv = नवीन vpiHandle[argc];
      पूर्णांक val_code = make_vpi_argv(argc, vpi_argv, format_string);
      अचिन्हित val_width = 0;
      delete[] format_string;

	// The make_vpi_argv returns for the function return value a
	// >0 value for the vector width if this is a vector. Convert
	// it to the form that the vpip_build_vpi_call uses.
      जर (val_code > 0) {
	    val_width = val_code;
	    val_code = -vpiVectorVal;
      }

      vvp_net_t*ptr = नवीन vvp_net_t;

      vpiHandle sys = vpip_build_vpi_call(name, val_code, val_width, ptr,
                                          सत्य, असत्य, argc, vpi_argv,
					  vec4_चळत, real_चळत, string_चळत,
                                          file_idx, lineno);
      प्रतिज्ञा(sys);

	/* Create and connect the functor to the label. */
      sfunc_core*score = नवीन sfunc_core(ptr, sys, argc, vpi_argv);
      ptr->fun = score;
      define_functor_चिन्ह(label, ptr);
      मुक्त(label);

	/* Link the inputs to the functor. */
      wide_inputs_connect(score, argc, argv);
      मुक्त(argv);

        /* If this function has a trigger event, connect the functor to
           that event. */
      जर (trigger_label)
            आदान_connect(ptr, 0, trigger_label);
      delete[] name;
}
