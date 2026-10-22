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
#अगर_घोषित CHECK_WITH_VALGRIND
# include "vvp_cleanup.h"
#पूर्ण_अगर
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
      मिटाओ sys_;
#अगर_घोषित CHECK_WITH_VALGRIND
      क्रम (अचिन्हित i = 0; i < argc_; i += 1) {
	    constant_delete(argv_[i]);
      }
#पूर्ण_अगर
      मिटाओ [] argv_;
}

/*
 * This method is only called when a trigger event occurs. Just arrange for
 * the function to be called.
 */
व्योम sfunc_core::recv_vec4(vvp_net_ptr_t, स्थिर vvp_vector4_t&/*bit*/,
                           vvp_context_t)
{
      schedule_functor(यह);
}

व्योम sfunc_core::recv_vec4_from_inputs(अचिन्हित port)
{
      vpiHandle vpi = argv_[port];
      काष्ठा __vpiBinaryConst*obj = dynamic_cast<__vpiBinaryConst*>(vpi);
      निश्चित(obj);

      obj->bits = value(port);

        /* Schedule the actual call after this finishes. */
      schedule_functor(यह);
}

व्योम sfunc_core::recv_real_from_inputs(अचिन्हित port)
{
      vpiHandle vpi = argv_[port];
      __vpiRealConst*obj = dynamic_cast<__vpiRealConst*>(vpi);
      निश्चित(obj);

      obj->value = मूल्य_r(port);

        /* Schedule the actual call after this finishes. */
      schedule_functor(यह);
}

व्योम sfunc_core::run_run()
{
      vpip_execute_vpi_call(0, sys_);
}

अटल पूर्णांक make_vpi_argv(अचिन्हित argc, vpiHandle*vpi_argv,
			स्थिर अक्षर*arg_string)
{
      अचिन्हित idx = 0;
      स्थिर अक्षर*cp = arg_string;
      पूर्णांक return_type = 0;

      चयन (*cp) {
	  हाल 'r': // real result
	    cp += 1;
	    return_type = -vpiRealVal;
	    अवरोध;

	  हाल 'v': // vector4_t
	    cp += 1;
	    return_type = म_से_अदीर्घ(cp, 0, 10);
	    cp += म_अखोज(cp, "0123456789");
	    अवरोध;

	  शेष:
	    ख_लिखो(मानक_त्रुटि, "Unsupported type %c(%d).\n", *cp, *cp);
	    निश्चित(0);
	    अवरोध;
      }

      जबतक (*cp) {
	    निश्चित(idx < argc);

	    चयन (*cp) {
		हाल 'r': // real
		  cp += 1;
		  vpi_argv[idx] = vpip_make_real_const(0.0);
		  अवरोध;

		हाल 'v': { // vector4_t (v<n>)
		      cp += 1;
		      अचिन्हित wid = म_से_अदीर्घ(cp, 0, 10);
		      cp += म_अखोज(cp, "0123456789");
		      vpi_argv[idx] = vpip_make_द्विआधारी_const(wid, "x");
		      अवरोध;
		}

		शेष:
		  ख_लिखो(मानक_त्रुटि, "Unsupported type %c(%d).\n", *cp, *cp);
		  निश्चित(0);
	    }
	    idx += 1;
      }

      निश्चित(idx == argc);
      वापस return_type;
}


व्योम compile_sfunc(अक्षर*label, अक्षर*name,  अक्षर*format_string,
		   दीर्घ file_idx, दीर्घ lineno,
		   अचिन्हित argc, काष्ठा symb_s*argv,
                   अक्षर*trigger_label)
{
      अचिन्हित vec4_ढेर = 0;
      अचिन्हित real_ढेर = 0;
      अचिन्हित string_ढेर = 0;
      vpiHandle*vpi_argv = नया vpiHandle[argc];
      पूर्णांक val_code = make_vpi_argv(argc, vpi_argv, format_string);
      अचिन्हित val_width = 0;
      मिटाओ[] format_string;

	// The make_vpi_argv returns for the function return value a
	// >0 value for the vector width if this is a vector. Convert
	// it to the form that the vpip_build_vpi_call uses.
      अगर (val_code > 0) {
	    val_width = val_code;
	    val_code = -vpiVectorVal;
      }

      vvp_net_t*ptr = नया vvp_net_t;

      vpiHandle sys = vpip_build_vpi_call(name, val_code, val_width, ptr,
                                          true, false, argc, vpi_argv,
					  vec4_ढेर, real_ढेर, string_ढेर,
                                          file_idx, lineno);
      निश्चित(sys);

	/* Create and connect the functor to the label. */
      sfunc_core*score = नया sfunc_core(ptr, sys, argc, vpi_argv);
      ptr->fun = score;
      define_functor_प्रतीक(label, ptr);
      मुक्त(label);

	/* Link the inputs to the functor. */
      wide_inputs_connect(score, argc, argv);
      मुक्त(argv);

        /* If this function has a trigger event, connect the functor to
           that event. */
      अगर (trigger_label)
            निवेश_connect(ptr, 0, trigger_label);
      मिटाओ[] name;
}
