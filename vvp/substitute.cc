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


ਵਰਗ vvp_fun_substitute : ਜਨਤਕ vvp_net_fun_t {

    ਜਨਤਕ:
      vvp_fun_substitute(ਬਿਨਾਨਿਸ਼ਾਨੀ wid, ਬਿਨਾਨਿਸ਼ਾਨੀ soff, ਬਿਨਾਨਿਸ਼ਾਨੀ swid);
      ~vvp_fun_substitute() override;

      ਖਾਲੀ recv_vec4(vvp_net_ptr_t ਪੋਰਟ, ਅਚਲ vvp_vector4_t&ਦੋਆਧਾਰੀ_ਅੰਕ, vvp_context_t) override;

      ਖਾਲੀ recv_vec4_pv(vvp_net_ptr_t ptr, ਅਚਲ vvp_vector4_t&ਦੋਆਧਾਰੀ_ਅੰਕ,
			ਬਿਨਾਨਿਸ਼ਾਨੀ ਅਧਾਰ, ਬਿਨਾਨਿਸ਼ਾਨੀ vwid, vvp_context_t ctx) override;

    ਨਿਜੀ:
      ਬਿਨਾਨਿਸ਼ਾਨੀ wid_;
      ਬਿਨਾਨਿਸ਼ਾਨੀ soff_;
      ਬਿਨਾਨਿਸ਼ਾਨੀ swid_;

      vvp_vector4_t val_;
};

vvp_fun_substitute::vvp_fun_substitute(ਬਿਨਾਨਿਸ਼ਾਨੀ wid, ਬਿਨਾਨਿਸ਼ਾਨੀ soff, ਬਿਨਾਨਿਸ਼ਾਨੀ swid)
: wid_(wid), soff_(soff), swid_(swid), val_(wid)
{
      ਲਈ (ਬਿਨਾਨਿਸ਼ਾਨੀ idx = 0 ; idx < val_.size() ; idx += 1)
	    val_.ਸੈੱਟ_ਦੋਆਧਾਰੀ_ਅੰਕ(idx, BIT4_Z);
}

vvp_fun_substitute::~vvp_fun_substitute()
{
}

ਖਾਲੀ vvp_fun_substitute::recv_vec4(vvp_net_ptr_t ਪੋਰਟ, ਅਚਲ vvp_vector4_t&ਦੋਆਧਾਰੀ_ਅੰਕ,
				   vvp_context_t)
{
      ਬਿਨਾਨਿਸ਼ਾਨੀ pdx = ਪੋਰਟ.ਪੋਰਟ();
      ਪ੍ਰਤਿਜ੍ਞਾ(pdx <= 1);

      ਜੇ (pdx == 0) {
	    ਪ੍ਰਤਿਜ੍ਞਾ(ਦੋਆਧਾਰੀ_ਅੰਕ.size() == wid_);

	    ਲਈ (ਬਿਨਾਨਿਸ਼ਾਨੀ idx = 0 ; idx < wid_ ; idx += 1) {
		  ਜੇ (idx >= soff_ && idx < (soff_+swid_))
			ਜਾਰੀ;

		  val_.ਸੈੱਟ_ਦੋਆਧਾਰੀ_ਅੰਕ(idx, ਦੋਆਧਾਰੀ_ਅੰਕ.value(idx));
	    }

      } ਨਹੀਂਤਾਂ {
	    ਪ੍ਰਤਿਜ੍ਞਾ(ਦੋਆਧਾਰੀ_ਅੰਕ.size() == swid_);

	    ਲਈ (ਬਿਨਾਨਿਸ਼ਾਨੀ idx = 0 ; idx < swid_ ; idx += 1)
		  val_.ਸੈੱਟ_ਦੋਆਧਾਰੀ_ਅੰਕ(idx+soff_, ਦੋਆਧਾਰੀ_ਅੰਕ.value(idx));
      }

      ਪੋਰਟ.ptr()->send_vec4(val_, 0);
}

ਖਾਲੀ vvp_fun_substitute::recv_vec4_pv(vvp_net_ptr_t ptr, ਅਚਲ vvp_vector4_t&ਦੋਆਧਾਰੀ_ਅੰਕ,
				      ਬਿਨਾਨਿਸ਼ਾਨੀ ਅਧਾਰ, ਬਿਨਾਨਿਸ਼ਾਨੀ vwid, vvp_context_t ctx)
{
      recv_vec4_pv_(ptr, ਦੋਆਧਾਰੀ_ਅੰਕ, ਅਧਾਰ, vwid, ctx);
}

ਖਾਲੀ compile_substitute(ਅੱਖਰ*label, ਬਿਨਾਨਿਸ਼ਾਨੀ width,
			ਬਿਨਾਨਿਸ਼ਾਨੀ soff, ਬਿਨਾਨਿਸ਼ਾਨੀ swidth,
			ਬਿਨਾਨਿਸ਼ਾਨੀ argc, ਬਣਤਰ symb_s*argv)
{
      vvp_fun_substitute*fun = ਨਵਾਂ vvp_fun_substitute(width, soff, swidth);

      vvp_net_t*net = ਨਵਾਂ vvp_net_t;
      net->fun = fun;

      define_functor_ਚਿੰਨ੍ਹ(label, net);
      ਮੁਕਤ(label);

      inputs_connect(net, argc, argv);
      ਮੁਕਤ(argv);
}
