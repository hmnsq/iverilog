#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ IVL_npmos_H
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ IVL_npmos_H
/*
 * Copyright (c) 2005-2025 Stephen Williams (steve@icarus.com)
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

# include  "vvp_net.h"

/*
 * The vvp_fun_pmos functor is similar to the vvp_fun_bufif. The
 * principle difference is that it has no drive of its own, instead
 * taking drive strength from its data input. In other words, it is
 * not a buffer but a uni-directional switch.
 *
 * The truth table for the PMOS device is:
 *
 *     Q = D C  (D is port0, C is port1)
 *     -------
 *     0 | 0 0
 *     Z | 0 1
 *     L | 0 x
 *     1 | 1 0
 *     Z | 1 1
 *     H | 1 x
 *
 * This class also implements the NMOS device, which is the same as
 * the PMOS device, but the Control input inverted. The enable_invert
 * flag to the constructor activates this inversion.
 */

class vvp_fun_pmos_ : ਜਨਤਕ vvp_net_fun_t {

    ਜਨਤਕ:
      explicit vvp_fun_pmos_(bool enable_invert, bool resistive);

      ਆਭਾਸੀ ਖਾਲੀ recv_vec4(vvp_net_ptr_t ਪੋਰਟ, ਅਚਲ vvp_vector4_t&ਦੋਆਧਾਰੀ_ਅੰਕ,
                             vvp_context_t) override;
      ਆਭਾਸੀ ਖਾਲੀ recv_vec4_pv(vvp_net_ptr_t ptr, ਅਚਲ vvp_vector4_t&ਦੋਆਧਾਰੀ_ਅੰਕ,
		                ਬਿਨਾਨਿਸ਼ਾਨੀ ਅਧਾਰ, ਬਿਨਾਨਿਸ਼ਾਨੀ vwid, vvp_context_t ctx) override;
      ਆਭਾਸੀ ਖਾਲੀ recv_vec8_pv(vvp_net_ptr_t ptr, ਅਚਲ vvp_vector8_t&ਦੋਆਧਾਰੀ_ਅੰਕ,
		                ਬਿਨਾਨਿਸ਼ਾਨੀ ਅਧਾਰ, ਬਿਨਾਨਿਸ਼ਾਨੀ vwid) override;
      ਆਭਾਸੀ ਖਾਲੀ recv_real(vvp_net_ptr_t ਪੋਰਟ, ਦੁੱਗਣਾ real,
                             vvp_context_t ctx) override;

    ਸੁਰੱਖਿਅਤ:
      ਖਾਲੀ generate_output_(vvp_net_ptr_t ਪੋਰਟ);

      vvp_vector8_t ਦੋਆਧਾਰੀ_ਅੰਕ_2;
      vvp_vector4_t en_;
      bool inv_en_, resistive_;
};

class vvp_fun_pmos  : ਜਨਤਕ vvp_fun_pmos_ {

    ਜਨਤਕ:
      explicit vvp_fun_pmos(bool enable_invert);

      ਆਭਾਸੀ ਖਾਲੀ recv_vec8(vvp_net_ptr_t ਪੋਰਟ, ਅਚਲ vvp_vector8_t&ਦੋਆਧਾਰੀ_ਅੰਕ) override;
};

/*
 * The vvp_fun_rpmos is a resistive version of the vvp_fun_pmos. The
 * only difference is that the input strength is reduced as it passes
 * through the device.
 */
class vvp_fun_rpmos  : ਜਨਤਕ vvp_fun_pmos_ {

    ਜਨਤਕ:
      explicit vvp_fun_rpmos(bool enable_invert);

      ਆਭਾਸੀ ਖਾਲੀ recv_vec8(vvp_net_ptr_t ਪੋਰਟ, ਅਚਲ vvp_vector8_t&ਦੋਆਧਾਰੀ_ਅੰਕ) override;
};

/*
 * The truth table for the CMOS device is:
 *
 *    Q = D N P  (D is port0, N is port1, P is port2)
 *    ---------
 *    0 | 0 0 0
 *    Z | 0 0 1
 *    0 | 0 1 0
 *    0 | 0 1 1
 *    L | 0 0 x
 *    L | 0 x 1
 *    L | 0 x x
 *    1 | 1 0 0
 *    Z | 1 0 1
 *    1 | 1 1 0
 *    1 | 1 1 1
 *    H | 1 0 x
 *    H | 1 x 1
 *    H | 1 x x
 */

class vvp_fun_cmos_ : ਜਨਤਕ vvp_net_fun_t {
    ਜਨਤਕ:
      explicit vvp_fun_cmos_(bool resistive);

      ਆਭਾਸੀ ਖਾਲੀ recv_vec4(vvp_net_ptr_t ਪੋਰਟ, ਅਚਲ vvp_vector4_t&ਦੋਆਧਾਰੀ_ਅੰਕ,
                             vvp_context_t) override;
      ਆਭਾਸੀ ਖਾਲੀ recv_vec4_pv(vvp_net_ptr_t ptr, ਅਚਲ vvp_vector4_t&ਦੋਆਧਾਰੀ_ਅੰਕ,
                                ਬਿਨਾਨਿਸ਼ਾਨੀ ਅਧਾਰ, ਬਿਨਾਨਿਸ਼ਾਨੀ vwid, vvp_context_t ctx) override;
      ਆਭਾਸੀ ਖਾਲੀ recv_vec8_pv(vvp_net_ptr_t ptr, ਅਚਲ vvp_vector8_t&ਦੋਆਧਾਰੀ_ਅੰਕ,
                              ਬਿਨਾਨਿਸ਼ਾਨੀ ਅਧਾਰ, ਬਿਨਾਨਿਸ਼ਾਨੀ vwid) override;
      ਆਭਾਸੀ ਖਾਲੀ recv_real(vvp_net_ptr_t ਪੋਰਟ, ਦੁੱਗਣਾ real,
                             vvp_context_t) override;

    ਸੁਰੱਖਿਅਤ:
      ਖਾਲੀ generate_output_(vvp_net_ptr_t ਪੋਰਟ);

      vvp_vector8_t ਦੋਆਧਾਰੀ_ਅੰਕ_2;
      vvp_vector4_t n_en_;
      vvp_vector4_t p_en_;
      bool resistive_;
};

class vvp_fun_cmos : ਜਨਤਕ vvp_fun_cmos_ {
    ਜਨਤਕ:
      explicit vvp_fun_cmos();

      ਆਭਾਸੀ ਖਾਲੀ recv_vec8(vvp_net_ptr_t ਪੋਰਟ, ਅਚਲ vvp_vector8_t&ਦੋਆਧਾਰੀ_ਅੰਕ) override;
};

class vvp_fun_rcmos : ਜਨਤਕ vvp_fun_cmos_ {
    ਜਨਤਕ:
      explicit vvp_fun_rcmos();

      ਆਭਾਸੀ ਖਾਲੀ recv_vec8(vvp_net_ptr_t ਪੋਰਟ, ਅਚਲ vvp_vector8_t&ਦੋਆਧਾਰੀ_ਅੰਕ) override;
};

#ਜੇ_ਅੰਤ /* IVL_npmos_H */
