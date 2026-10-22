/*
 * Copyright (c) 2016-2025 Stephen Williams (steve@icarus.com)
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
# include  "vvp_net.h"
# include  <cstdlib>
# include  <iostream>
# include  <cassert>


વર્ગ vvp_fun_substitute : જાહેર vvp_net_fun_t {

    જાહેર:
      vvp_fun_substitute(unsigned wid, unsigned soff, unsigned swid);
      ~vvp_fun_substitute() override;

      શૂન્ય recv_vec4(vvp_net_ptr_t port, અચલ vvp_vector4_t&bit, vvp_context_t) override;

      શૂન્ય recv_vec4_pv(vvp_net_ptr_t ptr, અચલ vvp_vector4_t&bit,
			unsigned base, unsigned vwid, vvp_context_t ctx) override;

    ખાનગી:
      unsigned wid_;
      unsigned soff_;
      unsigned swid_;

      vvp_vector4_t val_;
};

vvp_fun_substitute::vvp_fun_substitute(unsigned wid, unsigned soff, unsigned swid)
: wid_(wid), soff_(soff), swid_(swid), val_(wid)
{
      માટે (unsigned idx = 0 ; idx < val_.size() ; idx += 1)
	    val_.set_bit(idx, BIT4_Z);
}

vvp_fun_substitute::~vvp_fun_substitute()
{
}

શૂન્ય vvp_fun_substitute::recv_vec4(vvp_net_ptr_t port, અચલ vvp_vector4_t&bit,
				   vvp_context_t)
{
      unsigned pdx = port.port();
      પ્રતિજ્ઞા(pdx <= 1);

      જો (pdx == 0) {
	    પ્રતિજ્ઞા(bit.size() == wid_);

	    માટે (unsigned idx = 0 ; idx < wid_ ; idx += 1) {
		  જો (idx >= soff_ && idx < (soff_+swid_))
			ચાલુ;

		  val_.set_bit(idx, bit.value(idx));
	    }

      } નહીંતર {
	    પ્રતિજ્ઞા(bit.size() == swid_);

	    માટે (unsigned idx = 0 ; idx < swid_ ; idx += 1)
		  val_.set_bit(idx+soff_, bit.value(idx));
      }

      port.ptr()->send_vec4(val_, 0);
}

શૂન્ય vvp_fun_substitute::recv_vec4_pv(vvp_net_ptr_t ptr, અચલ vvp_vector4_t&bit,
				      unsigned base, unsigned vwid, vvp_context_t ctx)
{
      recv_vec4_pv_(ptr, bit, base, vwid, ctx);
}

શૂન્ય compile_substitute(અક્ષર*label, unsigned width,
			unsigned soff, unsigned swidth,
			unsigned argc, સંરચના symb_s*argv)
{
      vvp_fun_substitute*fun = નવું vvp_fun_substitute(width, soff, swidth);

      vvp_net_t*net = નવું vvp_net_t;
      net->fun = fun;

      define_functor_પ્રતીક(label, net);
      મુક્ત(label);

      inputs_connect(net, argc, argv);
      મુક્ત(argv);
}
