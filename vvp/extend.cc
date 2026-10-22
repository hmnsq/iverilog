/*
 * Copyright (c) 2005-2016 Stephen Williams <steve@icarus.com>
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
# include  "compile.h"
# include  <cstring>
# include  <iostream>
# include  <cassert>

vvp_fun_extend_signed::vvp_fun_extend_signed(ਬਿਨਾਨਿਸ਼ਾਨੀ wid)
: ਚੌੜਾਈ_2(wid)
{
}

vvp_fun_extend_signed::~vvp_fun_extend_signed()
{
}

ਖਾਲੀ vvp_fun_extend_signed::recv_vec4(vvp_net_ptr_t ਪੋਰਟ, ਅਚਲ vvp_vector4_t&ਦੋਆਧਾਰੀ_ਅੰਕ,
                                      vvp_context_t)
{
      ਜੇ (ਦੋਆਧਾਰੀ_ਅੰਕ.size() >= ਚੌੜਾਈ_2) {
	    ਪੋਰਟ.ptr()->send_vec4(ਦੋਆਧਾਰੀ_ਅੰਕ, 0);
	    ਵਾਪਸ;
      }

      vvp_vector4_t res (ਚੌੜਾਈ_2);

      ਲਈ (ਬਿਨਾਨਿਸ਼ਾਨੀ idx = 0 ;  idx < ਦੋਆਧਾਰੀ_ਅੰਕ.size() ;  idx += 1)
	    res.ਸੈੱਟ_ਦੋਆਧਾਰੀ_ਅੰਕ(idx, ਦੋਆਧਾਰੀ_ਅੰਕ.value(idx));

      vvp_bit4_t pad = ਦੋਆਧਾਰੀ_ਅੰਕ.size() > 0? ਦੋਆਧਾਰੀ_ਅੰਕ.value(ਦੋਆਧਾਰੀ_ਅੰਕ.size()-1) : BIT4_0;
      ਲਈ (ਬਿਨਾਨਿਸ਼ਾਨੀ idx = ਦੋਆਧਾਰੀ_ਅੰਕ.size() ;  idx < res.size() ;  idx += 1)
	    res.ਸੈੱਟ_ਦੋਆਧਾਰੀ_ਅੰਕ(idx, pad);

      ਪੋਰਟ.ptr()->send_vec4(res, 0);
}

ਖਾਲੀ vvp_fun_extend_signed::recv_vec4_pv(vvp_net_ptr_t ptr, ਅਚਲ vvp_vector4_t&ਦੋਆਧਾਰੀ_ਅੰਕ,
					 ਬਿਨਾਨਿਸ਼ਾਨੀ ਅਧਾਰ, ਬਿਨਾਨਿਸ਼ਾਨੀ vwid, vvp_context_t ctx)
{
      recv_vec4_pv_(ptr, ਦੋਆਧਾਰੀ_ਅੰਕ, ਅਧਾਰ, vwid, ctx);
}
