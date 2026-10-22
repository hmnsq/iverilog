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

vvp_fun_extend_signed::vvp_fun_extend_signed(غیرعلامتی wid)
: چوڑائی_2(wid)
{
}

vvp_fun_extend_signed::~vvp_fun_extend_signed()
{
}

خالی vvp_fun_extend_signed::recv_vec4(vvp_net_ptr_t پورٹ, ثابت vvp_vector4_t&ثنائی_ہندسہ,
                                      vvp_context_t)
{
      اگر (ثنائی_ہندسہ.size() >= چوڑائی_2) {
	    پورٹ.ptr()->send_vec4(ثنائی_ہندسہ, 0);
	    واپس;
      }

      vvp_vector4_t res (چوڑائی_2);

      برائے (غیرعلامتی idx = 0 ;  idx < ثنائی_ہندسہ.size() ;  idx += 1)
	    res.سیٹ_ثنائی_ہندسہ(idx, ثنائی_ہندسہ.value(idx));

      vvp_bit4_t pad = ثنائی_ہندسہ.size() > 0? ثنائی_ہندسہ.value(ثنائی_ہندسہ.size()-1) : BIT4_0;
      برائے (غیرعلامتی idx = ثنائی_ہندسہ.size() ;  idx < res.size() ;  idx += 1)
	    res.سیٹ_ثنائی_ہندسہ(idx, pad);

      پورٹ.ptr()->send_vec4(res, 0);
}

خالی vvp_fun_extend_signed::recv_vec4_pv(vvp_net_ptr_t ptr, ثابت vvp_vector4_t&ثنائی_ہندسہ,
					 غیرعلامتی بنیاد, غیرعلامتی vwid, vvp_context_t ctx)
{
      recv_vec4_pv_(ptr, ثنائی_ہندسہ, بنیاد, vwid, ctx);
}
