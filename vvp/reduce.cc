/*
 * Copyright (c) 2005-2026 Stephen Williams (steve@icarus.com)
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
# include  "schedule.h"
# include  <climits>
# include  <cstdio>
# include  <cassert>
# include  <cstdlib>

/*
 * All the reduction operations take a single vector input and produce
 * a scalar result. The vvp_reduce_base class codifies these general
 * characteristics, leaving only the calculation of the result for the
 * base class. This can be used in both statically and automatically
 * allocated scopes, as bits_ is only used for temporary storage.
 */
வகுப்பு vvp_reduce_base : பொது vvp_net_fun_t {

    பொது:
      vvp_reduce_base();
      மெய்நிகர் ~vvp_reduce_base() override;

      வெறுமை recv_vec4(vvp_net_ptr_t prt, மாறிலி vvp_vector4_t&bit,
                     vvp_context_t context) override;
      வெறுமை recv_vec4_pv(vvp_net_ptr_t ptr, மாறிலி vvp_vector4_t&bit,
			unsigned base, unsigned vwid, vvp_context_t context) override;

      மெய்நிகர் vvp_bit4_t calculate_result() மாறிலி =0;

    பாதுகாக்கப்பட்ட:
      vvp_vector4_t bits_;
};

vvp_reduce_base::vvp_reduce_base()
{
}

vvp_reduce_base::~vvp_reduce_base()
{
}

வெறுமை vvp_reduce_base::recv_vec4(vvp_net_ptr_t prt, மாறிலி vvp_vector4_t&bit,
                                vvp_context_t context)
{
      bits_ = bit;
      vvp_bit4_t res =  calculate_result();
      vvp_vector4_t rv (1, res);
      prt.ptr()->send_vec4(rv, context);
}

வெறுமை vvp_reduce_base::recv_vec4_pv(vvp_net_ptr_t prt, மாறிலி vvp_vector4_t&bit,
				   unsigned base, unsigned vwid, vvp_context_t context)
{
      எனில் (bits_.size() == 0) {
	    bits_ = vvp_vector4_t(vwid);
      }
      ப்ரதிஜ்ஞா(bits_.size() == vwid);

      bits_.set_vec(base, bit);
      vvp_bit4_t res = calculate_result();
      vvp_vector4_t rv (1, res);
      prt.ptr()->send_vec4(rv, context);
}

வகுப்பு vvp_reduce_and  : பொது vvp_reduce_base {

    பொது:
      vvp_reduce_and();
      ~vvp_reduce_and() override;
      vvp_bit4_t calculate_result() மாறிலி override;
};

vvp_reduce_and::vvp_reduce_and()
{
}

vvp_reduce_and::~vvp_reduce_and()
{
}

vvp_bit4_t vvp_reduce_and::calculate_result() மாறிலி
{
      திரும்பு bits_.reduce_and();
}

வகுப்பு vvp_reduce_or  : பொது vvp_reduce_base {

    பொது:
      vvp_reduce_or();
      ~vvp_reduce_or() override;
      vvp_bit4_t calculate_result() மாறிலி override;
};

vvp_reduce_or::vvp_reduce_or()
{
}

vvp_reduce_or::~vvp_reduce_or()
{
}

vvp_bit4_t vvp_reduce_or::calculate_result() மாறிலி
{
      திரும்பு bits_.reduce_or();
}

வகுப்பு vvp_reduce_xor  : பொது vvp_reduce_base {

    பொது:
      vvp_reduce_xor();
      ~vvp_reduce_xor() override;
      vvp_bit4_t calculate_result() மாறிலி override;
};

vvp_reduce_xor::vvp_reduce_xor()
{
}

vvp_reduce_xor::~vvp_reduce_xor()
{
}

vvp_bit4_t vvp_reduce_xor::calculate_result() மாறிலி
{
      திரும்பு bits_.reduce_xor();
}

வகுப்பு vvp_reduce_nand  : பொது vvp_reduce_base {

    பொது:
      vvp_reduce_nand();
      ~vvp_reduce_nand() override;
      vvp_bit4_t calculate_result() மாறிலி override;
};

vvp_reduce_nand::vvp_reduce_nand()
{
}

vvp_reduce_nand::~vvp_reduce_nand()
{
}

vvp_bit4_t vvp_reduce_nand::calculate_result() மாறிலி
{
      திரும்பு ~bits_.reduce_and();
}

வகுப்பு vvp_reduce_nor  : பொது vvp_reduce_base {

    பொது:
      vvp_reduce_nor();
      ~vvp_reduce_nor() override;
      vvp_bit4_t calculate_result() மாறிலி override;
};

vvp_reduce_nor::vvp_reduce_nor()
{
}

vvp_reduce_nor::~vvp_reduce_nor()
{
}

vvp_bit4_t vvp_reduce_nor::calculate_result() மாறிலி
{
      திரும்பு ~bits_.reduce_or();
}

வகுப்பு vvp_reduce_xnor  : பொது vvp_reduce_base {

    பொது:
      vvp_reduce_xnor();
      ~vvp_reduce_xnor() override;
      vvp_bit4_t calculate_result() மாறிலி override;
};

vvp_reduce_xnor::vvp_reduce_xnor()
{
}

vvp_reduce_xnor::~vvp_reduce_xnor()
{
}

vvp_bit4_t vvp_reduce_xnor::calculate_result() மாறிலி
{
      திரும்பு ~bits_.reduce_xor();
}

நிலையான வெறுமை make_reduce(எழுத்து*label, vvp_net_fun_t*red, மாறிலி கட்டமைப்பு symb_s&arg)
{
      vvp_net_t*ptr = புதிய vvp_net_t;
      ptr->fun = red;

      define_functor_குறியீடு(label, ptr);
      விடுவி(label);

      உள்ளீடு_connect(ptr, 0, arg.text);
}

வெறுமை compile_reduce_and(எழுத்து*label, மாறிலி கட்டமைப்பு symb_s&arg)
{
      vvp_reduce_and* reduce = புதிய vvp_reduce_and;
      make_reduce(label, reduce, arg);
}

வெறுமை compile_reduce_or(எழுத்து*label, மாறிலி கட்டமைப்பு symb_s&arg)
{
      vvp_reduce_or* reduce = புதிய vvp_reduce_or;
      make_reduce(label, reduce, arg);
}

வெறுமை compile_reduce_xor(எழுத்து*label, மாறிலி கட்டமைப்பு symb_s&arg)
{
      vvp_reduce_xor* reduce = புதிய vvp_reduce_xor;
      make_reduce(label, reduce, arg);
}

வெறுமை compile_reduce_nand(எழுத்து*label, மாறிலி கட்டமைப்பு symb_s&arg)
{
      vvp_reduce_nand* reduce = புதிய vvp_reduce_nand;
      make_reduce(label, reduce, arg);
}

வெறுமை compile_reduce_nor(எழுத்து*label, மாறிலி கட்டமைப்பு symb_s&arg)
{
      vvp_reduce_nor* reduce = புதிய vvp_reduce_nor;
      make_reduce(label, reduce, arg);
}

வெறுமை compile_reduce_xnor(எழுத்து*label, மாறிலி கட்டமைப்பு symb_s&arg)
{
      vvp_reduce_xnor* reduce = புதிய vvp_reduce_xnor;
      make_reduce(label, reduce, arg);
}
