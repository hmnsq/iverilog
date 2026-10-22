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
      માટે (unsigned i = 0; i < argc_; i += 1) {
	    constant_delete(argv_[i]);
      }
#endif
      delete [] argv_;
}

/*
 * This method is only called when a trigger event occurs. Just arrange for
 * the function to be called.
 */
શૂન્ય sfunc_core::recv_vec4(vvp_net_ptr_t, અચલ vvp_vector4_t&/*bit*/,
                           vvp_context_t)
{
      schedule_functor(આ);
}

શૂન્ય sfunc_core::recv_vec4_from_inputs(unsigned port)
{
      vpiHandle vpi = argv_[port];
      સંરચના __vpiBinaryConst*obj = dynamic_cast<__vpiBinaryConst*>(vpi);
      પ્રતિજ્ઞા(obj);

      obj->bits = value(port);

        /* Schedule the actual call after this finishes. */
      schedule_functor(આ);
}

શૂન્ય sfunc_core::recv_real_from_inputs(unsigned port)
{
      vpiHandle vpi = argv_[port];
      __vpiRealConst*obj = dynamic_cast<__vpiRealConst*>(vpi);
      પ્રતિજ્ઞા(obj);

      obj->value = કિંમત_r(port);

        /* Schedule the actual call after this finishes. */
      schedule_functor(આ);
}

શૂન્ય sfunc_core::run_run()
{
      vpip_execute_vpi_call(0, sys_);
}

સ્થિર પૂર્ણાંક make_vpi_argv(unsigned argc, vpiHandle*vpi_argv,
			અચલ અક્ષર*arg_string)
{
      unsigned idx = 0;
      અચલ અક્ષર*cp = arg_string;
      પૂર્ણાંક return_type = 0;

      પસંદગી (*cp) {
	  સ્થિતિ 'r': // real result
	    cp += 1;
	    return_type = -vpiRealVal;
	    તોડો;

	  સ્થિતિ 'v': // vector4_t
	    cp += 1;
	    return_type = strtoul(cp, 0, 10);
	    cp += strspn(cp, "0123456789");
	    તોડો;

	  મૂળભૂત:
	    fprintf(stderr, "Unsupported type %c(%d).\n", *cp, *cp);
	    પ્રતિજ્ઞા(0);
	    તોડો;
      }

      જ્યારે (*cp) {
	    પ્રતિજ્ઞા(idx < argc);

	    પસંદગી (*cp) {
		સ્થિતિ 'r': // real
		  cp += 1;
		  vpi_argv[idx] = vpip_make_real_const(0.0);
		  તોડો;

		સ્થિતિ 'v': { // vector4_t (v<n>)
		      cp += 1;
		      unsigned wid = strtoul(cp, 0, 10);
		      cp += strspn(cp, "0123456789");
		      vpi_argv[idx] = vpip_make_દ્વિઅંકી_const(wid, "x");
		      તોડો;
		}

		મૂળભૂત:
		  fprintf(stderr, "Unsupported type %c(%d).\n", *cp, *cp);
		  પ્રતિજ્ઞા(0);
	    }
	    idx += 1;
      }

      પ્રતિજ્ઞા(idx == argc);
      પાછા return_type;
}


શૂન્ય compile_sfunc(અક્ષર*label, અક્ષર*name,  અક્ષર*format_string,
		   long file_idx, long lineno,
		   unsigned argc, સંરચના symb_s*argv,
                   અક્ષર*trigger_label)
{
      unsigned vec4_ઢગલો = 0;
      unsigned real_ઢગલો = 0;
      unsigned string_ઢગલો = 0;
      vpiHandle*vpi_argv = નવું vpiHandle[argc];
      પૂર્ણાંક val_code = make_vpi_argv(argc, vpi_argv, format_string);
      unsigned val_width = 0;
      delete[] format_string;

	// The make_vpi_argv returns for the function return value a
	// >0 value for the vector width if this is a vector. Convert
	// it to the form that the vpip_build_vpi_call uses.
      જો (val_code > 0) {
	    val_width = val_code;
	    val_code = -vpiVectorVal;
      }

      vvp_net_t*ptr = નવું vvp_net_t;

      vpiHandle sys = vpip_build_vpi_call(name, val_code, val_width, ptr,
                                          સાચું, ખોટું, argc, vpi_argv,
					  vec4_ઢગલો, real_ઢગલો, string_ઢગલો,
                                          file_idx, lineno);
      પ્રતિજ્ઞા(sys);

	/* Create and connect the functor to the label. */
      sfunc_core*score = નવું sfunc_core(ptr, sys, argc, vpi_argv);
      ptr->fun = score;
      define_functor_પ્રતીક(label, ptr);
      મુક્ત(label);

	/* Link the inputs to the functor. */
      wide_inputs_connect(score, argc, argv);
      મુક્ત(argv);

        /* If this function has a trigger event, connect the functor to
           that event. */
      જો (trigger_label)
            આગત_connect(ptr, 0, trigger_label);
      delete[] name;
}
