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

vvp_fun_extend_signed::vvp_fun_extend_signed(अचिह्नित wid)
: विस्तारः_2(wid)
{
}

vvp_fun_extend_signed::~vvp_fun_extend_signed()
{
}

शून्य vvp_fun_extend_signed::recv_vec4(vvp_net_ptr_t पत्तनम्, ध्रुव vvp_vector4_t&bit,
                                      vvp_context_t)
{
      यदि (bit.size() >= विस्तारः_2) {
	    पत्तनम्.ptr()->send_vec4(bit, 0);
	    प्रत्यागम;
      }

      vvp_vector4_t res (विस्तारः_2);

      प्रति (अचिह्नित idx = 0 ;  idx < bit.size() ;  idx += 1)
	    res.समुच्चयः_bit(idx, bit.value(idx));

      vvp_bit4_t pad = bit.size() > 0? bit.value(bit.size()-1) : BIT4_0;
      प्रति (अचिह्नित idx = bit.size() ;  idx < res.size() ;  idx += 1)
	    res.समुच्चयः_bit(idx, pad);

      पत्तनम्.ptr()->send_vec4(res, 0);
}

शून्य vvp_fun_extend_signed::recv_vec4_pv(vvp_net_ptr_t ptr, ध्रुव vvp_vector4_t&bit,
					 अचिह्नित आधारः, अचिह्नित vwid, vvp_context_t ctx)
{
      recv_vec4_pv_(ptr, bit, आधारः, vwid, ctx);
}
