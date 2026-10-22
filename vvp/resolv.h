#که_نه_تعریف_شوی IVL_resolv_H
#تعریف_کړه IVL_resolv_H
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

class resolv_core : عام vvp_net_fun_t {

    عام:
      explicit resolv_core(بینښه nports, vvp_net_t*net);
      مجازي ~resolv_core() override;

      فراغ recv_vec4(vvp_net_ptr_t پورټ, ثابت vvp_vector4_t&بټ,
                     vvp_context_t) override
            { recv_vec4_(پورټ.پورټ(), بټ); }

      فراغ recv_vec8(vvp_net_ptr_t پورټ, ثابت vvp_vector8_t&بټ) override
            { recv_vec8_(پورټ.پورټ(), بټ); }

      فراغ recv_vec4_pv(vvp_net_ptr_t پورټ, ثابت vvp_vector4_t&بټ,
			بینښه بنسټ, بینښه vwid, vvp_context_t) override
            { recv_vec4_pv_(پورټ.پورټ(), بټ, بنسټ, vwid); }

      فراغ recv_vec8_pv(vvp_net_ptr_t پورټ, ثابت vvp_vector8_t&بټ,
			بینښه بنسټ, بینښه vwid) override
            { recv_vec8_pv_(پورټ.پورټ(), بټ, بنسټ, vwid); }

      مجازي فراغ شمېر_drivers(بینښه بټ_idx, بینښه counts[3]) =0;

    خاص:
      ملګری class resolv_extend;
      مجازي فراغ recv_vec4_(بینښه پورټ, ثابت vvp_vector4_t&بټ) =0;
      مجازي فراغ recv_vec8_(بینښه پورټ, ثابت vvp_vector8_t&بټ) =0;

      فراغ recv_vec4_pv_(بینښه پورټ, ثابت vvp_vector4_t&بټ,
			 بینښه بنسټ, بینښه vwid);
      فراغ recv_vec8_pv_(بینښه پورټ, ثابت vvp_vector8_t&بټ,
			 بینښه بنسټ, بینښه vwid);

    محمي:
      بینښه nports_;
      vvp_net_t*net_;
};

class resolv_extend : عام vvp_net_fun_t {

    عام:
      resolv_extend(resolv_core*core, بینښه پورټ_بنسټ);
      ~resolv_extend() override;

      فراغ recv_vec4(vvp_net_ptr_t پورټ, ثابت vvp_vector4_t&بټ,
                     vvp_context_t) override
            { core_->recv_vec4_(پورټ_بنسټ_2 + پورټ.پورټ(), بټ); }

      فراغ recv_vec8(vvp_net_ptr_t پورټ, ثابت vvp_vector8_t&بټ) override
            { core_->recv_vec8_(پورټ_بنسټ_2 + پورټ.پورټ(), بټ); }

      فراغ recv_vec4_pv(vvp_net_ptr_t پورټ, ثابت vvp_vector4_t&بټ,
			بینښه بنسټ, بینښه vwid, vvp_context_t) override
            { core_->recv_vec4_pv_(پورټ_بنسټ_2 + پورټ.پورټ(), بټ,
                                   بنسټ, vwid); }

      فراغ recv_vec8_pv(vvp_net_ptr_t پورټ, ثابت vvp_vector8_t&بټ,
			بینښه بنسټ, بینښه vwid) override
            { core_->recv_vec8_pv_(پورټ_بنسټ_2 + پورټ.پورټ(), بټ,
                                   بنسټ, vwid); }

    خاص:
      resolv_core*core_;
      بینښه پورټ_بنسټ_2;
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
class resolv_tri : عام resolv_core {

    عام:
      explicit resolv_tri(بینښه nports, vvp_net_t*net,
                          vvp_scalar_t hiz_ارزښت);
      ~resolv_tri() override;

      فراغ شمېر_drivers(بینښه بټ_idx, بینښه counts[3]) override;

    خاص:
      فراغ recv_vec4_(بینښه پورټ, ثابت vvp_vector4_t&بټ) override;
      فراغ recv_vec8_(بینښه پورټ, ثابت vvp_vector8_t&بټ) override;

    خاص:
        // The puller value to be used when a bit is not driven.
      vvp_scalar_t hiz_ارزښت_2;
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
class resolv_wired_logic : عام resolv_core {

    عام:
      explicit resolv_wired_logic(بینښه nports, vvp_net_t*net);
      مجازي ~resolv_wired_logic() override;

      فراغ شمېر_drivers(بینښه بټ_idx, بینښه counts[3]) override;

    محمي:
      مجازي vvp_vector4_t wired_logic_math_(vvp_vector4_t&a, vvp_vector4_t&b) =0;

    خاص:
      فراغ recv_vec4_(بینښه پورټ, ثابت vvp_vector4_t&بټ) override;
      فراغ recv_vec8_(بینښه پورټ, ثابت vvp_vector8_t&بټ) override;

    خاص:
        // The array of input values.
      vvp_vector4_t*val_;
};

class resolv_triand : عام resolv_wired_logic {

    عام:
      explicit resolv_triand(بینښه nports, vvp_net_t*net);
      ~resolv_triand() override;

    خاص:
      مجازي vvp_vector4_t wired_logic_math_(vvp_vector4_t&a, vvp_vector4_t&b) override;
};

class resolv_trior : عام resolv_wired_logic {

    عام:
      explicit resolv_trior(بینښه nports, vvp_net_t*net);
      ~resolv_trior() override;

    خاص:
      مجازي vvp_vector4_t wired_logic_math_(vvp_vector4_t&a, vvp_vector4_t&b) override;
};

#د_که_پای /* IVL_resolv_H */
