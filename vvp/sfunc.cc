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
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ CHECK_WITH_VALGRIND
# include "vvp_cleanup.h"
#ਜੇ_ਅੰਤ
# include  "sfunc.h"
# include  <cstdlib>
# include  <cstring>
# include  <iostream>
# include  <cassert>


sfunc_core::sfunc_core(vvp_net_t*net, vpiHandle sys,
		       ਬਿਨਾਨਿਸ਼ਾਨੀ argc, vpiHandle*argv)
: vvp_wide_fun_core(net, argc)
{
      sys_  = sys;
      argc_ = argc;
      argv_ = argv;
}

sfunc_core::~sfunc_core()
{
      ਮਿਟਾਓ sys_;
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ CHECK_WITH_VALGRIND
      ਲਈ (ਬਿਨਾਨਿਸ਼ਾਨੀ i = 0; i < argc_; i += 1) {
	    constant_delete(argv_[i]);
      }
#ਜੇ_ਅੰਤ
      ਮਿਟਾਓ [] argv_;
}

/*
 * This method is only called when a trigger event occurs. Just arrange for
 * the function to be called.
 */
ਖਾਲੀ sfunc_core::recv_vec4(vvp_net_ptr_t, ਅਚਲ vvp_vector4_t&/*ਦੋਆਧਾਰੀ_ਅੰਕ*/,
                           vvp_context_t)
{
      ਸਮਾਂ_ਸਾਰਣੀ_functor(ਇਹ);
}

ਖਾਲੀ sfunc_core::recv_vec4_from_inputs(ਬਿਨਾਨਿਸ਼ਾਨੀ ਪੋਰਟ)
{
      vpiHandle vpi = argv_[ਪੋਰਟ];
      ਬਣਤਰ __vpiBinaryConst*obj = dynamic_cast<__vpiBinaryConst*>(vpi);
      ਪ੍ਰਤਿਜ੍ਞਾ(obj);

      obj->ਬਿੱਟ = value(ਪੋਰਟ);

        /* Schedule the actual call after this finishes. */
      ਸਮਾਂ_ਸਾਰਣੀ_functor(ਇਹ);
}

ਖਾਲੀ sfunc_core::recv_real_from_inputs(ਬਿਨਾਨਿਸ਼ਾਨੀ ਪੋਰਟ)
{
      vpiHandle vpi = argv_[ਪੋਰਟ];
      __vpiRealConst*obj = dynamic_cast<__vpiRealConst*>(vpi);
      ਪ੍ਰਤਿਜ੍ਞਾ(obj);

      obj->value = ਮੁੱਲ_r(ਪੋਰਟ);

        /* Schedule the actual call after this finishes. */
      ਸਮਾਂ_ਸਾਰਣੀ_functor(ਇਹ);
}

ਖਾਲੀ sfunc_core::ਲੜੀਵਾਰ_ਲੜੀਵਾਰ()
{
      vpip_execute_vpi_call(0, sys_);
}

ਸਥਿਰ ਪੂਰਨ make_vpi_argv(ਬਿਨਾਨਿਸ਼ਾਨੀ argc, vpiHandle*vpi_argv,
			ਅਚਲ ਅੱਖਰ*arg_string)
{
      ਬਿਨਾਨਿਸ਼ਾਨੀ idx = 0;
      ਅਚਲ ਅੱਖਰ*cp = arg_string;
      ਪੂਰਨ return_type = 0;

      ਚੋਣ (*cp) {
	  ਹਾਲਤ 'r': // real result
	    cp += 1;
	    return_type = -vpiRealVal;
	    ਰੋਕੋ;

	  ਹਾਲਤ 'v': // vector4_t
	    cp += 1;
	    return_type = strtoul(cp, 0, 10);
	    cp += strspn(cp, "0123456789");
	    ਰੋਕੋ;

	  ਮੂਲ:
	    fprintf(stderr, "Unsupported type %c(%d).\n", *cp, *cp);
	    ਪ੍ਰਤਿਜ੍ਞਾ(0);
	    ਰੋਕੋ;
      }

      ਜਦੋਂ (*cp) {
	    ਪ੍ਰਤਿਜ੍ਞਾ(idx < argc);

	    ਚੋਣ (*cp) {
		ਹਾਲਤ 'r': // real
		  cp += 1;
		  vpi_argv[idx] = vpip_make_real_const(0.0);
		  ਰੋਕੋ;

		ਹਾਲਤ 'v': { // vector4_t (v<n>)
		      cp += 1;
		      ਬਿਨਾਨਿਸ਼ਾਨੀ wid = strtoul(cp, 0, 10);
		      cp += strspn(cp, "0123456789");
		      vpi_argv[idx] = vpip_make_ਦੁਆਧਾਰੀ_const(wid, "x");
		      ਰੋਕੋ;
		}

		ਮੂਲ:
		  fprintf(stderr, "Unsupported type %c(%d).\n", *cp, *cp);
		  ਪ੍ਰਤਿਜ੍ਞਾ(0);
	    }
	    idx += 1;
      }

      ਪ੍ਰਤਿਜ੍ਞਾ(idx == argc);
      ਵਾਪਸ return_type;
}


ਖਾਲੀ compile_sfunc(ਅੱਖਰ*label, ਅੱਖਰ*name,  ਅੱਖਰ*format_string,
		   ਲੰਮਾ file_idx, ਲੰਮਾ lineno,
		   ਬਿਨਾਨਿਸ਼ਾਨੀ argc, ਬਣਤਰ symb_s*argv,
                   ਅੱਖਰ*trigger_label)
{
      ਬਿਨਾਨਿਸ਼ਾਨੀ vec4_ਢੇਰ = 0;
      ਬਿਨਾਨਿਸ਼ਾਨੀ real_ਢੇਰ = 0;
      ਬਿਨਾਨਿਸ਼ਾਨੀ string_ਢੇਰ = 0;
      vpiHandle*vpi_argv = ਨਵਾਂ vpiHandle[argc];
      ਪੂਰਨ val_ਕੋਡ = make_vpi_argv(argc, vpi_argv, format_string);
      ਬਿਨਾਨਿਸ਼ਾਨੀ val_ਚੌੜਾਈ = 0;
      ਮਿਟਾਓ[] format_string;

	// The make_vpi_argv returns for the function return value a
	// >0 value for the vector width if this is a vector. Convert
	// it to the form that the vpip_build_vpi_call uses.
      ਜੇ (val_ਕੋਡ > 0) {
	    val_ਚੌੜਾਈ = val_ਕੋਡ;
	    val_ਕੋਡ = -vpi_ਸਦਿਸ਼_Val;
      }

      vvp_net_t*ptr = ਨਵਾਂ vvp_net_t;

      vpiHandle sys = vpip_build_vpi_call(name, val_ਕੋਡ, val_ਚੌੜਾਈ, ptr,
                                          ਸੱਚ, ਝੂਠ, argc, vpi_argv,
					  vec4_ਢੇਰ, real_ਢੇਰ, string_ਢੇਰ,
                                          file_idx, lineno);
      ਪ੍ਰਤਿਜ੍ਞਾ(sys);

	/* Create and connect the functor to the label. */
      sfunc_core*score = ਨਵਾਂ sfunc_core(ptr, sys, argc, vpi_argv);
      ptr->fun = score;
      define_functor_ਚਿੰਨ੍ਹ(label, ptr);
      ਮੁਕਤ(label);

	/* Link the inputs to the functor. */
      wide_inputs_connect(score, argc, argv);
      ਮੁਕਤ(argv);

        /* If this function has a trigger event, connect the functor to
           that event. */
      ਜੇ (trigger_label)
            ਨਿਵੇਸ਼_connect(ptr, 0, trigger_label);
      ਮਿਟਾਓ[] name;
}
