#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ IVL_resolv_H
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ IVL_resolv_H
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

class resolv_core : ਜਨਤਕ vvp_net_fun_t {

    ਜਨਤਕ:
      explicit resolv_core(ਬਿਨਾਨਿਸ਼ਾਨੀ nports, vvp_net_t*net);
      ਆਭਾਸੀ ~resolv_core() override;

      ਖਾਲੀ recv_vec4(vvp_net_ptr_t ਪੋਰਟ, ਅਚਲ vvp_vector4_t&ਦੋਆਧਾਰੀ_ਅੰਕ,
                     vvp_context_t) override
            { recv_vec4_(ਪੋਰਟ.ਪੋਰਟ(), ਦੋਆਧਾਰੀ_ਅੰਕ); }

      ਖਾਲੀ recv_vec8(vvp_net_ptr_t ਪੋਰਟ, ਅਚਲ vvp_vector8_t&ਦੋਆਧਾਰੀ_ਅੰਕ) override
            { recv_vec8_(ਪੋਰਟ.ਪੋਰਟ(), ਦੋਆਧਾਰੀ_ਅੰਕ); }

      ਖਾਲੀ recv_vec4_pv(vvp_net_ptr_t ਪੋਰਟ, ਅਚਲ vvp_vector4_t&ਦੋਆਧਾਰੀ_ਅੰਕ,
			ਬਿਨਾਨਿਸ਼ਾਨੀ ਅਧਾਰ, ਬਿਨਾਨਿਸ਼ਾਨੀ vwid, vvp_context_t) override
            { recv_vec4_pv_(ਪੋਰਟ.ਪੋਰਟ(), ਦੋਆਧਾਰੀ_ਅੰਕ, ਅਧਾਰ, vwid); }

      ਖਾਲੀ recv_vec8_pv(vvp_net_ptr_t ਪੋਰਟ, ਅਚਲ vvp_vector8_t&ਦੋਆਧਾਰੀ_ਅੰਕ,
			ਬਿਨਾਨਿਸ਼ਾਨੀ ਅਧਾਰ, ਬਿਨਾਨਿਸ਼ਾਨੀ vwid) override
            { recv_vec8_pv_(ਪੋਰਟ.ਪੋਰਟ(), ਦੋਆਧਾਰੀ_ਅੰਕ, ਅਧਾਰ, vwid); }

      ਆਭਾਸੀ ਖਾਲੀ ਗਿਣਤੀ_drivers(ਬਿਨਾਨਿਸ਼ਾਨੀ ਦੋਆਧਾਰੀ_ਅੰਕ_idx, ਬਿਨਾਨਿਸ਼ਾਨੀ counts[3]) =0;

    ਨਿਜੀ:
      ਮਿੱਤਰ class resolv_extend;
      ਆਭਾਸੀ ਖਾਲੀ recv_vec4_(ਬਿਨਾਨਿਸ਼ਾਨੀ ਪੋਰਟ, ਅਚਲ vvp_vector4_t&ਦੋਆਧਾਰੀ_ਅੰਕ) =0;
      ਆਭਾਸੀ ਖਾਲੀ recv_vec8_(ਬਿਨਾਨਿਸ਼ਾਨੀ ਪੋਰਟ, ਅਚਲ vvp_vector8_t&ਦੋਆਧਾਰੀ_ਅੰਕ) =0;

      ਖਾਲੀ recv_vec4_pv_(ਬਿਨਾਨਿਸ਼ਾਨੀ ਪੋਰਟ, ਅਚਲ vvp_vector4_t&ਦੋਆਧਾਰੀ_ਅੰਕ,
			 ਬਿਨਾਨਿਸ਼ਾਨੀ ਅਧਾਰ, ਬਿਨਾਨਿਸ਼ਾਨੀ vwid);
      ਖਾਲੀ recv_vec8_pv_(ਬਿਨਾਨਿਸ਼ਾਨੀ ਪੋਰਟ, ਅਚਲ vvp_vector8_t&ਦੋਆਧਾਰੀ_ਅੰਕ,
			 ਬਿਨਾਨਿਸ਼ਾਨੀ ਅਧਾਰ, ਬਿਨਾਨਿਸ਼ਾਨੀ vwid);

    ਸੁਰੱਖਿਅਤ:
      ਬਿਨਾਨਿਸ਼ਾਨੀ nports_;
      vvp_net_t*net_;
};

class resolv_extend : ਜਨਤਕ vvp_net_fun_t {

    ਜਨਤਕ:
      resolv_extend(resolv_core*core, ਬਿਨਾਨਿਸ਼ਾਨੀ ਪੋਰਟ_ਅਧਾਰ);
      ~resolv_extend() override;

      ਖਾਲੀ recv_vec4(vvp_net_ptr_t ਪੋਰਟ, ਅਚਲ vvp_vector4_t&ਦੋਆਧਾਰੀ_ਅੰਕ,
                     vvp_context_t) override
            { core_->recv_vec4_(ਪੋਰਟ_ਅਧਾਰ_2 + ਪੋਰਟ.ਪੋਰਟ(), ਦੋਆਧਾਰੀ_ਅੰਕ); }

      ਖਾਲੀ recv_vec8(vvp_net_ptr_t ਪੋਰਟ, ਅਚਲ vvp_vector8_t&ਦੋਆਧਾਰੀ_ਅੰਕ) override
            { core_->recv_vec8_(ਪੋਰਟ_ਅਧਾਰ_2 + ਪੋਰਟ.ਪੋਰਟ(), ਦੋਆਧਾਰੀ_ਅੰਕ); }

      ਖਾਲੀ recv_vec4_pv(vvp_net_ptr_t ਪੋਰਟ, ਅਚਲ vvp_vector4_t&ਦੋਆਧਾਰੀ_ਅੰਕ,
			ਬਿਨਾਨਿਸ਼ਾਨੀ ਅਧਾਰ, ਬਿਨਾਨਿਸ਼ਾਨੀ vwid, vvp_context_t) override
            { core_->recv_vec4_pv_(ਪੋਰਟ_ਅਧਾਰ_2 + ਪੋਰਟ.ਪੋਰਟ(), ਦੋਆਧਾਰੀ_ਅੰਕ,
                                   ਅਧਾਰ, vwid); }

      ਖਾਲੀ recv_vec8_pv(vvp_net_ptr_t ਪੋਰਟ, ਅਚਲ vvp_vector8_t&ਦੋਆਧਾਰੀ_ਅੰਕ,
			ਬਿਨਾਨਿਸ਼ਾਨੀ ਅਧਾਰ, ਬਿਨਾਨਿਸ਼ਾਨੀ vwid) override
            { core_->recv_vec8_pv_(ਪੋਰਟ_ਅਧਾਰ_2 + ਪੋਰਟ.ਪੋਰਟ(), ਦੋਆਧਾਰੀ_ਅੰਕ,
                                   ਅਧਾਰ, vwid); }

    ਨਿਜੀ:
      resolv_core*core_;
      ਬਿਨਾਨਿਸ਼ਾਨੀ ਪੋਰਟ_ਅਧਾਰ_2;
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
class resolv_tri : ਜਨਤਕ resolv_core {

    ਜਨਤਕ:
      explicit resolv_tri(ਬਿਨਾਨਿਸ਼ਾਨੀ nports, vvp_net_t*net,
                          vvp_scalar_t hiz_ਮੁੱਲ);
      ~resolv_tri() override;

      ਖਾਲੀ ਗਿਣਤੀ_drivers(ਬਿਨਾਨਿਸ਼ਾਨੀ ਦੋਆਧਾਰੀ_ਅੰਕ_idx, ਬਿਨਾਨਿਸ਼ਾਨੀ counts[3]) override;

    ਨਿਜੀ:
      ਖਾਲੀ recv_vec4_(ਬਿਨਾਨਿਸ਼ਾਨੀ ਪੋਰਟ, ਅਚਲ vvp_vector4_t&ਦੋਆਧਾਰੀ_ਅੰਕ) override;
      ਖਾਲੀ recv_vec8_(ਬਿਨਾਨਿਸ਼ਾਨੀ ਪੋਰਟ, ਅਚਲ vvp_vector8_t&ਦੋਆਧਾਰੀ_ਅੰਕ) override;

    ਨਿਜੀ:
        // The puller value to be used when a bit is not driven.
      vvp_scalar_t hiz_ਮੁੱਲ_2;
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
class resolv_wired_logic : ਜਨਤਕ resolv_core {

    ਜਨਤਕ:
      explicit resolv_wired_logic(ਬਿਨਾਨਿਸ਼ਾਨੀ nports, vvp_net_t*net);
      ਆਭਾਸੀ ~resolv_wired_logic() override;

      ਖਾਲੀ ਗਿਣਤੀ_drivers(ਬਿਨਾਨਿਸ਼ਾਨੀ ਦੋਆਧਾਰੀ_ਅੰਕ_idx, ਬਿਨਾਨਿਸ਼ਾਨੀ counts[3]) override;

    ਸੁਰੱਖਿਅਤ:
      ਆਭਾਸੀ vvp_vector4_t wired_logic_math_(vvp_vector4_t&a, vvp_vector4_t&b) =0;

    ਨਿਜੀ:
      ਖਾਲੀ recv_vec4_(ਬਿਨਾਨਿਸ਼ਾਨੀ ਪੋਰਟ, ਅਚਲ vvp_vector4_t&ਦੋਆਧਾਰੀ_ਅੰਕ) override;
      ਖਾਲੀ recv_vec8_(ਬਿਨਾਨਿਸ਼ਾਨੀ ਪੋਰਟ, ਅਚਲ vvp_vector8_t&ਦੋਆਧਾਰੀ_ਅੰਕ) override;

    ਨਿਜੀ:
        // The array of input values.
      vvp_vector4_t*val_;
};

class resolv_triand : ਜਨਤਕ resolv_wired_logic {

    ਜਨਤਕ:
      explicit resolv_triand(ਬਿਨਾਨਿਸ਼ਾਨੀ nports, vvp_net_t*net);
      ~resolv_triand() override;

    ਨਿਜੀ:
      ਆਭਾਸੀ vvp_vector4_t wired_logic_math_(vvp_vector4_t&a, vvp_vector4_t&b) override;
};

class resolv_trior : ਜਨਤਕ resolv_wired_logic {

    ਜਨਤਕ:
      explicit resolv_trior(ਬਿਨਾਨਿਸ਼ਾਨੀ nports, vvp_net_t*net);
      ~resolv_trior() override;

    ਨਿਜੀ:
      ਆਭਾਸੀ vvp_vector4_t wired_logic_math_(vvp_vector4_t&a, vvp_vector4_t&b) override;
};

#ਜੇ_ਅੰਤ /* IVL_resolv_H */
