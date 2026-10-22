#اگر_غیر_تعریف_شدہ IVL_resolv_H
#تعریف_کرو IVL_resolv_H
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

class resolv_core : عوامی vvp_net_fun_t {

    عوامی:
      explicit resolv_core(غیرعلامتی nports, vvp_net_t*net);
      مجازی ~resolv_core() override;

      خالی recv_vec4(vvp_net_ptr_t پورٹ, ثابت vvp_vector4_t&ثنائی_ہندسہ,
                     vvp_context_t) override
            { recv_vec4_(پورٹ.پورٹ(), ثنائی_ہندسہ); }

      خالی recv_vec8(vvp_net_ptr_t پورٹ, ثابت vvp_vector8_t&ثنائی_ہندسہ) override
            { recv_vec8_(پورٹ.پورٹ(), ثنائی_ہندسہ); }

      خالی recv_vec4_pv(vvp_net_ptr_t پورٹ, ثابت vvp_vector4_t&ثنائی_ہندسہ,
			غیرعلامتی بنیاد, غیرعلامتی vwid, vvp_context_t) override
            { recv_vec4_pv_(پورٹ.پورٹ(), ثنائی_ہندسہ, بنیاد, vwid); }

      خالی recv_vec8_pv(vvp_net_ptr_t پورٹ, ثابت vvp_vector8_t&ثنائی_ہندسہ,
			غیرعلامتی بنیاد, غیرعلامتی vwid) override
            { recv_vec8_pv_(پورٹ.پورٹ(), ثنائی_ہندسہ, بنیاد, vwid); }

      مجازی خالی گنتی_drivers(غیرعلامتی ثنائی_ہندسہ_idx, غیرعلامتی counts[3]) =0;

    نجی:
      دوست class resolv_extend;
      مجازی خالی recv_vec4_(غیرعلامتی پورٹ, ثابت vvp_vector4_t&ثنائی_ہندسہ) =0;
      مجازی خالی recv_vec8_(غیرعلامتی پورٹ, ثابت vvp_vector8_t&ثنائی_ہندسہ) =0;

      خالی recv_vec4_pv_(غیرعلامتی پورٹ, ثابت vvp_vector4_t&ثنائی_ہندسہ,
			 غیرعلامتی بنیاد, غیرعلامتی vwid);
      خالی recv_vec8_pv_(غیرعلامتی پورٹ, ثابت vvp_vector8_t&ثنائی_ہندسہ,
			 غیرعلامتی بنیاد, غیرعلامتی vwid);

    محفوظ:
      غیرعلامتی nports_;
      vvp_net_t*net_;
};

class resolv_extend : عوامی vvp_net_fun_t {

    عوامی:
      resolv_extend(resolv_core*core, غیرعلامتی پورٹ_بنیاد);
      ~resolv_extend() override;

      خالی recv_vec4(vvp_net_ptr_t پورٹ, ثابت vvp_vector4_t&ثنائی_ہندسہ,
                     vvp_context_t) override
            { core_->recv_vec4_(پورٹ_بنیاد_2 + پورٹ.پورٹ(), ثنائی_ہندسہ); }

      خالی recv_vec8(vvp_net_ptr_t پورٹ, ثابت vvp_vector8_t&ثنائی_ہندسہ) override
            { core_->recv_vec8_(پورٹ_بنیاد_2 + پورٹ.پورٹ(), ثنائی_ہندسہ); }

      خالی recv_vec4_pv(vvp_net_ptr_t پورٹ, ثابت vvp_vector4_t&ثنائی_ہندسہ,
			غیرعلامتی بنیاد, غیرعلامتی vwid, vvp_context_t) override
            { core_->recv_vec4_pv_(پورٹ_بنیاد_2 + پورٹ.پورٹ(), ثنائی_ہندسہ,
                                   بنیاد, vwid); }

      خالی recv_vec8_pv(vvp_net_ptr_t پورٹ, ثابت vvp_vector8_t&ثنائی_ہندسہ,
			غیرعلامتی بنیاد, غیرعلامتی vwid) override
            { core_->recv_vec8_pv_(پورٹ_بنیاد_2 + پورٹ.پورٹ(), ثنائی_ہندسہ,
                                   بنیاد, vwid); }

    نجی:
      resolv_core*core_;
      غیرعلامتی پورٹ_بنیاد_2;
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
class resolv_tri : عوامی resolv_core {

    عوامی:
      explicit resolv_tri(غیرعلامتی nports, vvp_net_t*net,
                          vvp_scalar_t hiz_قدر);
      ~resolv_tri() override;

      خالی گنتی_drivers(غیرعلامتی ثنائی_ہندسہ_idx, غیرعلامتی counts[3]) override;

    نجی:
      خالی recv_vec4_(غیرعلامتی پورٹ, ثابت vvp_vector4_t&ثنائی_ہندسہ) override;
      خالی recv_vec8_(غیرعلامتی پورٹ, ثابت vvp_vector8_t&ثنائی_ہندسہ) override;

    نجی:
        // The puller value to be used when a bit is not driven.
      vvp_scalar_t hiz_قدر_2;
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
class resolv_wired_logic : عوامی resolv_core {

    عوامی:
      explicit resolv_wired_logic(غیرعلامتی nports, vvp_net_t*net);
      مجازی ~resolv_wired_logic() override;

      خالی گنتی_drivers(غیرعلامتی ثنائی_ہندسہ_idx, غیرعلامتی counts[3]) override;

    محفوظ:
      مجازی vvp_vector4_t wired_logic_math_(vvp_vector4_t&a, vvp_vector4_t&b) =0;

    نجی:
      خالی recv_vec4_(غیرعلامتی پورٹ, ثابت vvp_vector4_t&ثنائی_ہندسہ) override;
      خالی recv_vec8_(غیرعلامتی پورٹ, ثابت vvp_vector8_t&ثنائی_ہندسہ) override;

    نجی:
        // The array of input values.
      vvp_vector4_t*val_;
};

class resolv_triand : عوامی resolv_wired_logic {

    عوامی:
      explicit resolv_triand(غیرعلامتی nports, vvp_net_t*net);
      ~resolv_triand() override;

    نجی:
      مجازی vvp_vector4_t wired_logic_math_(vvp_vector4_t&a, vvp_vector4_t&b) override;
};

class resolv_trior : عوامی resolv_wired_logic {

    عوامی:
      explicit resolv_trior(غیرعلامتی nports, vvp_net_t*net);
      ~resolv_trior() override;

    نجی:
      مجازی vvp_vector4_t wired_logic_math_(vvp_vector4_t&a, vvp_vector4_t&b) override;
};

#اگر_ختم /* IVL_resolv_H */
