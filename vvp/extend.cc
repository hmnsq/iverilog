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

vvp_fun_extend_signed::vvp_fun_extend_signed(بیعلامت wid)
: عرض_2(wid)
{
}

vvp_fun_extend_signed::~vvp_fun_extend_signed()
{
}

فراغ vvp_fun_extend_signed::recv_vec4(vvp_net_ptr_t پورت, ثابت vvp_vector4_t&بیت,
                                      vvp_context_t)
{
      إذا (بیت.size() >= عرض_2) {
	    پورت.ptr()->send_vec4(بیت, 0);
	    أرجع;
      }

      vvp_vector4_t res (عرض_2);

      لكل (بیعلامت idx = 0 ;  idx < بیت.size() ;  idx += 1)
	    res.مجموعه_بیت(idx, بیت.value(idx));

      vvp_bit4_t pad = بیت.size() > 0? بیت.value(بیت.size()-1) : BIT4_0;
      لكل (بیعلامت idx = بیت.size() ;  idx < res.size() ;  idx += 1)
	    res.مجموعه_بیت(idx, pad);

      پورت.ptr()->send_vec4(res, 0);
}

فراغ vvp_fun_extend_signed::recv_vec4_pv(vvp_net_ptr_t ptr, ثابت vvp_vector4_t&بیت,
					 بیعلامت پایه, بیعلامت vwid, vvp_context_t ctx)
{
      recv_vec4_pv_(ptr, بیت, پایه, vwid, ctx);
}
