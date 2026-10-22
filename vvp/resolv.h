#ifndef IVL_resolv_H
#define IVL_resolv_H
/*
 * Copyright (c) 2001-2025 Stephen Williams (steve@icarus.com)
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

# include  "config.h"
# include  "vvp_net.h"

/*
 * Resolver nodes are similar to wide functors, in that they may have
 * more than 4 inputs. Unlike wide functors, the functor that provides
 * the core functionality and delivers the output is also used to handle
 * the first 4 inputs, thus each resolver node is implemented by N/4
 * actual functors.
 */

class resolv_core : பொது vvp_net_fun_t {

    பொது:
      explicit resolv_core(unsigned nports, vvp_net_t*net);
      மெய்நிகர் ~resolv_core() override;

      வெறுமை recv_vec4(vvp_net_ptr_t port, மாறிலி vvp_vector4_t&bit,
                     vvp_context_t) override
            { recv_vec4_(port.port(), bit); }

      வெறுமை recv_vec8(vvp_net_ptr_t port, மாறிலி vvp_vector8_t&bit) override
            { recv_vec8_(port.port(), bit); }

      வெறுமை recv_vec4_pv(vvp_net_ptr_t port, மாறிலி vvp_vector4_t&bit,
			unsigned base, unsigned vwid, vvp_context_t) override
            { recv_vec4_pv_(port.port(), bit, base, vwid); }

      வெறுமை recv_vec8_pv(vvp_net_ptr_t port, மாறிலி vvp_vector8_t&bit,
			unsigned base, unsigned vwid) override
            { recv_vec8_pv_(port.port(), bit, base, vwid); }

      மெய்நிகர் வெறுமை மொத்தம்_drivers(unsigned bit_idx, unsigned counts[3]) =0;

    தனிப்பட்ட:
      friend class resolv_extend;
      மெய்நிகர் வெறுமை recv_vec4_(unsigned port, மாறிலி vvp_vector4_t&bit) =0;
      மெய்நிகர் வெறுமை recv_vec8_(unsigned port, மாறிலி vvp_vector8_t&bit) =0;

      வெறுமை recv_vec4_pv_(unsigned port, மாறிலி vvp_vector4_t&bit,
			 unsigned base, unsigned vwid);
      வெறுமை recv_vec8_pv_(unsigned port, மாறிலி vvp_vector8_t&bit,
			 unsigned base, unsigned vwid);

    பாதுகாக்கப்பட்ட:
      unsigned nports_;
      vvp_net_t*net_;
};

class resolv_extend : பொது vvp_net_fun_t {

    பொது:
      resolv_extend(resolv_core*core, unsigned port_base);
      ~resolv_extend() override;

      வெறுமை recv_vec4(vvp_net_ptr_t port, மாறிலி vvp_vector4_t&bit,
                     vvp_context_t) override
            { core_->recv_vec4_(port_base_ + port.port(), bit); }

      வெறுமை recv_vec8(vvp_net_ptr_t port, மாறிலி vvp_vector8_t&bit) override
            { core_->recv_vec8_(port_base_ + port.port(), bit); }

      வெறுமை recv_vec4_pv(vvp_net_ptr_t port, மாறிலி vvp_vector4_t&bit,
			unsigned base, unsigned vwid, vvp_context_t) override
            { core_->recv_vec4_pv_(port_base_ + port.port(), bit,
                                   base, vwid); }

      வெறுமை recv_vec8_pv(vvp_net_ptr_t port, மாறிலி vvp_vector8_t&bit,
			unsigned base, unsigned vwid) override
            { core_->recv_vec8_pv_(port_base_ + port.port(), bit,
                                   base, vwid); }

    தனிப்பட்ட:
      resolv_core*core_;
      unsigned port_base_;
};

/*
 * This functor type resolves its inputs using the Verilog method of
 * combining signals, and outputs that resolved value. The puller
 * value is also blended with the result. This helps with the
 * implementation of tri0 and tri1, which have pull constants
 * attached.
 *
 * This node takes in vvp_vector8_t values, and emits a vvp_vector8_t
 * value. It also takes in vvp_vector4_t values, which it treats as
 * strong values (or HiZ) for the sake of resolution. In any case, the
 * propagated value is a vvp_vector8_t value.
 */
class resolv_tri : பொது resolv_core {

    பொது:
      explicit resolv_tri(unsigned nports, vvp_net_t*net,
                          vvp_scalar_t hiz_மதிப்பு);
      ~resolv_tri() override;

      வெறுமை மொத்தம்_drivers(unsigned bit_idx, unsigned counts[3]) override;

    தனிப்பட்ட:
      வெறுமை recv_vec4_(unsigned port, மாறிலி vvp_vector4_t&bit) override;
      வெறுமை recv_vec8_(unsigned port, மாறிலி vvp_vector8_t&bit) override;

    தனிப்பட்ட:
        // The puller value to be used when a bit is not driven.
      vvp_scalar_t hiz_மதிப்பு_2;
        // The array of input values.
      vvp_vector8_t*val_;
};

/*
 * This functor type resolves its inputs using the wired_logic_math_
 * method implemented by each derived class, and outputs that resolved
 * value.
 *
 * This node takes in vvp_vector4_t values, and emits a vvp_vector4_t
 * value. It also takes in vvp_vector8_t values, which it reduces to
 * 4-state values for the sake of resolution.
 */
class resolv_wired_logic : பொது resolv_core {

    பொது:
      explicit resolv_wired_logic(unsigned nports, vvp_net_t*net);
      மெய்நிகர் ~resolv_wired_logic() override;

      வெறுமை மொத்தம்_drivers(unsigned bit_idx, unsigned counts[3]) override;

    பாதுகாக்கப்பட்ட:
      மெய்நிகர் vvp_vector4_t wired_logic_math_(vvp_vector4_t&a, vvp_vector4_t&b) =0;

    தனிப்பட்ட:
      வெறுமை recv_vec4_(unsigned port, மாறிலி vvp_vector4_t&bit) override;
      வெறுமை recv_vec8_(unsigned port, மாறிலி vvp_vector8_t&bit) override;

    தனிப்பட்ட:
        // The array of input values.
      vvp_vector4_t*val_;
};

class resolv_triand : பொது resolv_wired_logic {

    பொது:
      explicit resolv_triand(unsigned nports, vvp_net_t*net);
      ~resolv_triand() override;

    தனிப்பட்ட:
      மெய்நிகர் vvp_vector4_t wired_logic_math_(vvp_vector4_t&a, vvp_vector4_t&b) override;
};

class resolv_trior : பொது resolv_wired_logic {

    பொது:
      explicit resolv_trior(unsigned nports, vvp_net_t*net);
      ~resolv_trior() override;

    தனிப்பட்ட:
      மெய்நிகர் vvp_vector4_t wired_logic_math_(vvp_vector4_t&a, vvp_vector4_t&b) override;
};

#endif /* IVL_resolv_H */
