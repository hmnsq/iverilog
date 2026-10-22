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
      க்கு (unsigned i = 0; i < argc_; i += 1) {
	    constant_delete(argv_[i]);
      }
#endif
      delete [] argv_;
}

/*
 * This method is only called when a trigger event occurs. Just arrange for
 * the function to be called.
 */
வெறுமை sfunc_core::recv_vec4(vvp_net_ptr_t, மாறிலி vvp_vector4_t&/*bit*/,
                           vvp_context_t)
{
      schedule_functor(இது);
}

வெறுமை sfunc_core::recv_vec4_from_inputs(unsigned port)
{
      vpiHandle vpi = argv_[port];
      கட்டமைப்பு __vpiBinaryConst*obj = dynamic_cast<__vpiBinaryConst*>(vpi);
      ப்ரதிஜ்ஞா(obj);

      obj->bits = value(port);

        /* Schedule the actual call after this finishes. */
      schedule_functor(இது);
}

வெறுமை sfunc_core::recv_real_from_inputs(unsigned port)
{
      vpiHandle vpi = argv_[port];
      __vpiRealConst*obj = dynamic_cast<__vpiRealConst*>(vpi);
      ப்ரதிஜ்ஞா(obj);

      obj->value = மதிப்பு_r(port);

        /* Schedule the actual call after this finishes. */
      schedule_functor(இது);
}

வெறுமை sfunc_core::run_run()
{
      vpip_execute_vpi_call(0, sys_);
}

நிலையான முழு make_vpi_argv(unsigned argc, vpiHandle*vpi_argv,
			மாறிலி எழுத்து*arg_string)
{
      unsigned idx = 0;
      மாறிலி எழுத்து*cp = arg_string;
      முழு return_type = 0;

      தேர்வு (*cp) {
	  நிலை 'r': // real result
	    cp += 1;
	    return_type = -vpiRealVal;
	    நிறுத்து;

	  நிலை 'v': // vector4_t
	    cp += 1;
	    return_type = strtoul(cp, 0, 10);
	    cp += strspn(cp, "0123456789");
	    நிறுத்து;

	  இயல்பு:
	    fprintf(stderr, "Unsupported type %c(%d).\n", *cp, *cp);
	    ப்ரதிஜ்ஞா(0);
	    நிறுத்து;
      }

      போது (*cp) {
	    ப்ரதிஜ்ஞா(idx < argc);

	    தேர்வு (*cp) {
		நிலை 'r': // real
		  cp += 1;
		  vpi_argv[idx] = vpip_make_real_const(0.0);
		  நிறுத்து;

		நிலை 'v': { // vector4_t (v<n>)
		      cp += 1;
		      unsigned wid = strtoul(cp, 0, 10);
		      cp += strspn(cp, "0123456789");
		      vpi_argv[idx] = vpip_make_இருமம்_const(wid, "x");
		      நிறுத்து;
		}

		இயல்பு:
		  fprintf(stderr, "Unsupported type %c(%d).\n", *cp, *cp);
		  ப்ரதிஜ்ஞா(0);
	    }
	    idx += 1;
      }

      ப்ரதிஜ்ஞா(idx == argc);
      திரும்பு return_type;
}


வெறுமை compile_sfunc(எழுத்து*label, எழுத்து*name,  எழுத்து*format_string,
		   long file_idx, long lineno,
		   unsigned argc, கட்டமைப்பு symb_s*argv,
                   எழுத்து*trigger_label)
{
      unsigned vec4_அடுக்கு = 0;
      unsigned real_அடுக்கு = 0;
      unsigned string_அடுக்கு = 0;
      vpiHandle*vpi_argv = புதிய vpiHandle[argc];
      முழு val_code = make_vpi_argv(argc, vpi_argv, format_string);
      unsigned val_width = 0;
      delete[] format_string;

	// The make_vpi_argv returns for the function return value a
	// >0 value for the vector width if this is a vector. Convert
	// it to the form that the vpip_build_vpi_call uses.
      எனில் (val_code > 0) {
	    val_width = val_code;
	    val_code = -vpiVectorVal;
      }

      vvp_net_t*ptr = புதிய vvp_net_t;

      vpiHandle sys = vpip_build_vpi_call(name, val_code, val_width, ptr,
                                          உண்மை, பொய், argc, vpi_argv,
					  vec4_அடுக்கு, real_அடுக்கு, string_அடுக்கு,
                                          file_idx, lineno);
      ப்ரதிஜ்ஞா(sys);

	/* Create and connect the functor to the label. */
      sfunc_core*score = புதிய sfunc_core(ptr, sys, argc, vpi_argv);
      ptr->fun = score;
      define_functor_குறியீடு(label, ptr);
      விடுவி(label);

	/* Link the inputs to the functor. */
      wide_inputs_connect(score, argc, argv);
      விடுவி(argv);

        /* If this function has a trigger event, connect the functor to
           that event. */
      எனில் (trigger_label)
            உள்ளீடு_connect(ptr, 0, trigger_label);
      delete[] name;
}
