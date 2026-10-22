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


শ্রেণী vvp_fun_substitute : সর্বজনীন vvp_net_fun_t {

    সর্বজনীন:
      vvp_fun_substitute(unsigned wid, unsigned soff, unsigned swid);
      ~vvp_fun_substitute() override;

      শূন্য recv_vec4(vvp_net_ptr_t port, ধ্রুবক vvp_vector4_t&bit, vvp_context_t) override;

      শূন্য recv_vec4_pv(vvp_net_ptr_t ptr, ধ্রুবক vvp_vector4_t&bit,
			unsigned base, unsigned vwid, vvp_context_t ctx) override;

    ব্যক্তিগত:
      unsigned wid_;
      unsigned soff_;
      unsigned swid_;

      vvp_vector4_t val_;
};

vvp_fun_substitute::vvp_fun_substitute(unsigned wid, unsigned soff, unsigned swid)
: wid_(wid), soff_(soff), swid_(swid), val_(wid)
{
      জন্য (unsigned idx = 0 ; idx < val_.size() ; idx += 1)
	    val_.set_bit(idx, BIT4_Z);
}

vvp_fun_substitute::~vvp_fun_substitute()
{
}

শূন্য vvp_fun_substitute::recv_vec4(vvp_net_ptr_t port, ধ্রুবক vvp_vector4_t&bit,
				   vvp_context_t)
{
      unsigned pdx = port.port();
      প্রতিজ্ঞা(pdx <= 1);

      যদি (pdx == 0) {
	    প্রতিজ্ঞা(bit.size() == wid_);

	    জন্য (unsigned idx = 0 ; idx < wid_ ; idx += 1) {
		  যদি (idx >= soff_ && idx < (soff_+swid_))
			চালু;

		  val_.set_bit(idx, bit.value(idx));
	    }

      } অন্যথা {
	    প্রতিজ্ঞা(bit.size() == swid_);

	    জন্য (unsigned idx = 0 ; idx < swid_ ; idx += 1)
		  val_.set_bit(idx+soff_, bit.value(idx));
      }

      port.ptr()->send_vec4(val_, 0);
}

শূন্য vvp_fun_substitute::recv_vec4_pv(vvp_net_ptr_t ptr, ধ্রুবক vvp_vector4_t&bit,
				      unsigned base, unsigned vwid, vvp_context_t ctx)
{
      recv_vec4_pv_(ptr, bit, base, vwid, ctx);
}

শূন্য compile_substitute(অক্ষর*label, unsigned width,
			unsigned soff, unsigned swidth,
			unsigned argc, কাঠামো symb_s*argv)
{
      vvp_fun_substitute*fun = নতুন vvp_fun_substitute(width, soff, swidth);

      vvp_net_t*net = নতুন vvp_net_t;
      net->fun = fun;

      define_functor_প্রতীক(label, net);
      মুক্ত(label);

      inputs_connect(net, argc, argv);
      মুক্ত(argv);
}
