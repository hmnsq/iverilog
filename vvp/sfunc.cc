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
#నిర్వచితమైతే CHECK_WITH_VALGRIND
# include "vvp_cleanup.h"
#అయితే_ముగింపు
# include  "sfunc.h"
# include  <cstdlib>
# include  <cstring>
# include  <iostream>
# include  <cassert>


sfunc_core::sfunc_core(vvp_net_t*net, vpiHandle sys,
		       గుర్తులేని argc, vpiHandle*argv)
: vvp_wide_fun_core(net, argc)
{
      sys_  = sys;
      argc_ = argc;
      argv_ = argv;
}

sfunc_core::~sfunc_core()
{
      తొలగించు sys_;
#నిర్వచితమైతే CHECK_WITH_VALGRIND
      కొరకు (గుర్తులేని i = 0; i < argc_; i += 1) {
	    constant_delete(argv_[i]);
      }
#అయితే_ముగింపు
      తొలగించు [] argv_;
}

/*
 * This method is only called when a trigger event occurs. Just arrange for
 * the function to be called.
 */
శూన్య sfunc_core::recv_vec4(vvp_net_ptr_t, స్థిరరాశి vvp_vector4_t&/*బిట్టు*/,
                           vvp_context_t)
{
      కాలపట్టిక_functor(ఇది);
}

శూన్య sfunc_core::recv_vec4_from_inputs(గుర్తులేని పోర్టు)
{
      vpiHandle vpi = argv_[పోర్టు];
      నిర్మాణం __vpiBinaryConst*obj = dynamic_cast<__vpiBinaryConst*>(vpi);
      ప్రతిజ్ఞా(obj);

      obj->బిట్లు = value(పోర్టు);

        /* Schedule the actual call after this finishes. */
      కాలపట్టిక_functor(ఇది);
}

శూన్య sfunc_core::recv_real_from_inputs(గుర్తులేని పోర్టు)
{
      vpiHandle vpi = argv_[పోర్టు];
      __vpiRealConst*obj = dynamic_cast<__vpiRealConst*>(vpi);
      ప్రతిజ్ఞా(obj);

      obj->value = విలువ_r(పోర్టు);

        /* Schedule the actual call after this finishes. */
      కాలపట్టిక_functor(ఇది);
}

శూన్య sfunc_core::నిరంతరం_నిరంతరం()
{
      vpip_execute_vpi_call(0, sys_);
}

స్థిర పూర్ణాంకం make_vpi_argv(గుర్తులేని argc, vpiHandle*vpi_argv,
			స్థిరరాశి అక్షరం*arg_string)
{
      గుర్తులేని idx = 0;
      స్థిరరాశి అక్షరం*cp = arg_string;
      పూర్ణాంకం return_type = 0;

      ఎంపిక (*cp) {
	  స్థితి 'r': // real result
	    cp += 1;
	    return_type = -vpiRealVal;
	    ఆపు;

	  స్థితి 'v': // vector4_t
	    cp += 1;
	    return_type = strtoul(cp, 0, 10);
	    cp += strspn(cp, "0123456789");
	    ఆపు;

	  అప్రమేయ:
	    fprintf(stderr, "Unsupported type %c(%d).\n", *cp, *cp);
	    ప్రతిజ్ఞా(0);
	    ఆపు;
      }

      అయితే (*cp) {
	    ప్రతిజ్ఞా(idx < argc);

	    ఎంపిక (*cp) {
		స్థితి 'r': // real
		  cp += 1;
		  vpi_argv[idx] = vpip_make_real_const(0.0);
		  ఆపు;

		స్థితి 'v': { // vector4_t (v<n>)
		      cp += 1;
		      గుర్తులేని wid = strtoul(cp, 0, 10);
		      cp += strspn(cp, "0123456789");
		      vpi_argv[idx] = vpip_make_ద్విమాన_const(wid, "x");
		      ఆపు;
		}

		అప్రమేయ:
		  fprintf(stderr, "Unsupported type %c(%d).\n", *cp, *cp);
		  ప్రతిజ్ఞా(0);
	    }
	    idx += 1;
      }

      ప్రతిజ్ఞా(idx == argc);
      తిరిగి return_type;
}


శూన్య compile_sfunc(అక్షరం*label, అక్షరం*name,  అక్షరం*format_string,
		   దీర్ఘ file_idx, దీర్ఘ lineno,
		   గుర్తులేని argc, నిర్మాణం symb_s*argv,
                   అక్షరం*trigger_label)
{
      గుర్తులేని vec4_దొంతర = 0;
      గుర్తులేని real_దొంతర = 0;
      గుర్తులేని string_దొంతర = 0;
      vpiHandle*vpi_argv = కొత్త vpiHandle[argc];
      పూర్ణాంకం val_సంకేతం = make_vpi_argv(argc, vpi_argv, format_string);
      గుర్తులేని val_వెడల్పు = 0;
      తొలగించు[] format_string;

	// The make_vpi_argv returns for the function return value a
	// >0 value for the vector width if this is a vector. Convert
	// it to the form that the vpip_build_vpi_call uses.
      ఒకవేళ (val_సంకేతం > 0) {
	    val_వెడల్పు = val_సంకేతం;
	    val_సంకేతం = -vpi_సదిశ_Val;
      }

      vvp_net_t*ptr = కొత్త vvp_net_t;

      vpiHandle sys = vpip_build_vpi_call(name, val_సంకేతం, val_వెడల్పు, ptr,
                                          నిజం, అబద్ధం, argc, vpi_argv,
					  vec4_దొంతర, real_దొంతర, string_దొంతర,
                                          file_idx, lineno);
      ప్రతిజ్ఞా(sys);

	/* Create and connect the functor to the label. */
      sfunc_core*score = కొత్త sfunc_core(ptr, sys, argc, vpi_argv);
      ptr->fun = score;
      define_functor_చిహ్నం(label, ptr);
      విడుదల(label);

	/* Link the inputs to the functor. */
      wide_inputs_connect(score, argc, argv);
      విడుదల(argv);

        /* If this function has a trigger event, connect the functor to
           that event. */
      ఒకవేళ (trigger_label)
            ఆదానం_connect(ptr, 0, trigger_label);
      తొలగించు[] name;
}
