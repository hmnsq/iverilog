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
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ CHECK_WITH_VALGRIND
# include "vvp_cleanup.h"
#ಆದರೆ_ಅಂತ್ಯ
# include  "sfunc.h"
# include  <cstdlib>
# include  <cstring>
# include  <iostream>
# include  <cassert>


sfunc_core::sfunc_core(vvp_net_t*net, vpiHandle sys,
		       ಚಿಹ್ನೆರಹಿತ argc, vpiHandle*argv)
: vvp_wide_fun_core(net, argc)
{
      sys_  = sys;
      argc_ = argc;
      argv_ = argv;
}

sfunc_core::~sfunc_core()
{
      ಅಳಿಸು sys_;
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ CHECK_WITH_VALGRIND
      ಗಾಗಿ (ಚಿಹ್ನೆರಹಿತ i = 0; i < argc_; i += 1) {
	    constant_delete(argv_[i]);
      }
#ಆದರೆ_ಅಂತ್ಯ
      ಅಳಿಸು [] argv_;
}

/*
 * This method is only called when a trigger event occurs. Just arrange for
 * the function to be called.
 */
ಶೂನ್ಯ sfunc_core::recv_vec4(vvp_net_ptr_t, ಸ್ಥಿರಾಂಕ vvp_vector4_t&/*ಬಿಟ್*/,
                           vvp_context_t)
{
      ವೇಳಾಪಟ್ಟಿ_functor(ಇದು);
}

ಶೂನ್ಯ sfunc_core::recv_vec4_from_inputs(ಚಿಹ್ನೆರಹಿತ ಪೋರ್ಟ್)
{
      vpiHandle vpi = argv_[ಪೋರ್ಟ್];
      ರಚನೆ __vpiBinaryConst*obj = dynamic_cast<__vpiBinaryConst*>(vpi);
      ಪ್ರತಿಜ್ಞಾ(obj);

      obj->ಬಿಟ್ಟುಗಳು = value(ಪೋರ್ಟ್);

        /* Schedule the actual call after this finishes. */
      ವೇಳಾಪಟ್ಟಿ_functor(ಇದು);
}

ಶೂನ್ಯ sfunc_core::recv_real_from_inputs(ಚಿಹ್ನೆರಹಿತ ಪೋರ್ಟ್)
{
      vpiHandle vpi = argv_[ಪೋರ್ಟ್];
      __vpiRealConst*obj = dynamic_cast<__vpiRealConst*>(vpi);
      ಪ್ರತಿಜ್ಞಾ(obj);

      obj->value = ಮೌಲ್ಯ_r(ಪೋರ್ಟ್);

        /* Schedule the actual call after this finishes. */
      ವೇಳಾಪಟ್ಟಿ_functor(ಇದು);
}

ಶೂನ್ಯ sfunc_core::ಸತತ_ಸತತ()
{
      vpip_execute_vpi_call(0, sys_);
}

ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ make_vpi_argv(ಚಿಹ್ನೆರಹಿತ argc, vpiHandle*vpi_argv,
			ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ*arg_string)
{
      ಚಿಹ್ನೆರಹಿತ idx = 0;
      ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ*cp = arg_string;
      ಪೂರ್ಣಾಂಕ return_type = 0;

      ಆಯ್ಕೆ (*cp) {
	  ಸ್ಥಿತಿ 'r': // real result
	    cp += 1;
	    return_type = -vpiRealVal;
	    ನಿಲ್ಲಿಸು;

	  ಸ್ಥಿತಿ 'v': // vector4_t
	    cp += 1;
	    return_type = strtoul(cp, 0, 10);
	    cp += strspn(cp, "0123456789");
	    ನಿಲ್ಲಿಸು;

	  ಪೂರ್ವನಿಯೋಜಿತ:
	    fprintf(stderr, "Unsupported type %c(%d).\n", *cp, *cp);
	    ಪ್ರತಿಜ್ಞಾ(0);
	    ನಿಲ್ಲಿಸು;
      }

      ಆಗ (*cp) {
	    ಪ್ರತಿಜ್ಞಾ(idx < argc);

	    ಆಯ್ಕೆ (*cp) {
		ಸ್ಥಿತಿ 'r': // real
		  cp += 1;
		  vpi_argv[idx] = vpip_make_real_const(0.0);
		  ನಿಲ್ಲಿಸು;

		ಸ್ಥಿತಿ 'v': { // vector4_t (v<n>)
		      cp += 1;
		      ಚಿಹ್ನೆರಹಿತ wid = strtoul(cp, 0, 10);
		      cp += strspn(cp, "0123456789");
		      vpi_argv[idx] = vpip_make_ದ್ವಿಮಾನ_const(wid, "x");
		      ನಿಲ್ಲಿಸು;
		}

		ಪೂರ್ವನಿಯೋಜಿತ:
		  fprintf(stderr, "Unsupported type %c(%d).\n", *cp, *cp);
		  ಪ್ರತಿಜ್ಞಾ(0);
	    }
	    idx += 1;
      }

      ಪ್ರತಿಜ್ಞಾ(idx == argc);
      ಹಿಂತಿರುಗಿ return_type;
}


ಶೂನ್ಯ compile_sfunc(ಅಕ್ಷರ*label, ಅಕ್ಷರ*name,  ಅಕ್ಷರ*format_string,
		   ದೀರ್ಘ file_idx, ದೀರ್ಘ lineno,
		   ಚಿಹ್ನೆರಹಿತ argc, ರಚನೆ symb_s*argv,
                   ಅಕ್ಷರ*trigger_label)
{
      ಚಿಹ್ನೆರಹಿತ vec4_ರಾಶಿ = 0;
      ಚಿಹ್ನೆರಹಿತ real_ರಾಶಿ = 0;
      ಚಿಹ್ನೆರಹಿತ string_ರಾಶಿ = 0;
      vpiHandle*vpi_argv = ಹೊಸ vpiHandle[argc];
      ಪೂರ್ಣಾಂಕ val_ಸಂಕೇತ = make_vpi_argv(argc, vpi_argv, format_string);
      ಚಿಹ್ನೆರಹಿತ val_ಅಗಲ = 0;
      ಅಳಿಸು[] format_string;

	// The make_vpi_argv returns for the function return value a
	// >0 value for the vector width if this is a vector. Convert
	// it to the form that the vpip_build_vpi_call uses.
      ಆದರೆ (val_ಸಂಕೇತ > 0) {
	    val_ಅಗಲ = val_ಸಂಕೇತ;
	    val_ಸಂಕೇತ = -vpi_ಸದಿಶ_Val;
      }

      vvp_net_t*ptr = ಹೊಸ vvp_net_t;

      vpiHandle sys = vpip_build_vpi_call(name, val_ಸಂಕೇತ, val_ಅಗಲ, ptr,
                                          ನಿಜ, ಸುಳ್ಳು, argc, vpi_argv,
					  vec4_ರಾಶಿ, real_ರಾಶಿ, string_ರಾಶಿ,
                                          file_idx, lineno);
      ಪ್ರತಿಜ್ಞಾ(sys);

	/* Create and connect the functor to the label. */
      sfunc_core*score = ಹೊಸ sfunc_core(ptr, sys, argc, vpi_argv);
      ptr->fun = score;
      define_functor_ಚಿಹ್ನೆ(label, ptr);
      ಬಿಡುಗಡೆ(label);

	/* Link the inputs to the functor. */
      wide_inputs_connect(score, argc, argv);
      ಬಿಡುಗಡೆ(argv);

        /* If this function has a trigger event, connect the functor to
           that event. */
      ಆದರೆ (trigger_label)
            ಒಳಹರಿವು_connect(ptr, 0, trigger_label);
      ಅಳಿಸು[] name;
}
