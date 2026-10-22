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
বর্গ vvp_reduce_base : সার্বজনিক vvp_net_fun_t {

    সার্বজনিক:
      vvp_reduce_base();
      আভাস ~vvp_reduce_base() override;

      শূন্য recv_vec4(vvp_net_ptr_t prt, ধ্রুব vvp_vector4_t&bit,
                     vvp_context_t context) override;
      শূন্য recv_vec4_pv(vvp_net_ptr_t ptr, ধ্রুব vvp_vector4_t&bit,
			unsigned base, unsigned vwid, vvp_context_t context) override;

      আভাস vvp_bit4_t calculate_result() ধ্রুব =0;

    রক্ষিত:
      vvp_vector4_t bits_;
};

vvp_reduce_base::vvp_reduce_base()
{
}

vvp_reduce_base::~vvp_reduce_base()
{
}

শূন্য vvp_reduce_base::recv_vec4(vvp_net_ptr_t prt, ধ্রুব vvp_vector4_t&bit,
                                vvp_context_t context)
{
      bits_ = bit;
      vvp_bit4_t res =  calculate_result();
      vvp_vector4_t rv (1, res);
      prt.ptr()->send_vec4(rv, context);
}

শূন্য vvp_reduce_base::recv_vec4_pv(vvp_net_ptr_t prt, ধ্রুব vvp_vector4_t&bit,
				   unsigned base, unsigned vwid, vvp_context_t context)
{
      যদি (bits_.size() == 0) {
	    bits_ = vvp_vector4_t(vwid);
      }
      প্রতিজ্ঞা(bits_.size() == vwid);

      bits_.set_vec(base, bit);
      vvp_bit4_t res = calculate_result();
      vvp_vector4_t rv (1, res);
      prt.ptr()->send_vec4(rv, context);
}

বর্গ vvp_reduce_and  : সার্বজনিক vvp_reduce_base {

    সার্বজনিক:
      vvp_reduce_and();
      ~vvp_reduce_and() override;
      vvp_bit4_t calculate_result() ধ্রুব override;
};

vvp_reduce_and::vvp_reduce_and()
{
}

vvp_reduce_and::~vvp_reduce_and()
{
}

vvp_bit4_t vvp_reduce_and::calculate_result() ধ্রুব
{
      প্রত্যাগম bits_.reduce_and();
}

বর্গ vvp_reduce_or  : সার্বজনিক vvp_reduce_base {

    সার্বজনিক:
      vvp_reduce_or();
      ~vvp_reduce_or() override;
      vvp_bit4_t calculate_result() ধ্রুব override;
};

vvp_reduce_or::vvp_reduce_or()
{
}

vvp_reduce_or::~vvp_reduce_or()
{
}

vvp_bit4_t vvp_reduce_or::calculate_result() ধ্রুব
{
      প্রত্যাগম bits_.reduce_or();
}

বর্গ vvp_reduce_xor  : সার্বজনিক vvp_reduce_base {

    সার্বজনিক:
      vvp_reduce_xor();
      ~vvp_reduce_xor() override;
      vvp_bit4_t calculate_result() ধ্রুব override;
};

vvp_reduce_xor::vvp_reduce_xor()
{
}

vvp_reduce_xor::~vvp_reduce_xor()
{
}

vvp_bit4_t vvp_reduce_xor::calculate_result() ধ্রুব
{
      প্রত্যাগম bits_.reduce_xor();
}

বর্গ vvp_reduce_nand  : সার্বজনিক vvp_reduce_base {

    সার্বজনিক:
      vvp_reduce_nand();
      ~vvp_reduce_nand() override;
      vvp_bit4_t calculate_result() ধ্রুব override;
};

vvp_reduce_nand::vvp_reduce_nand()
{
}

vvp_reduce_nand::~vvp_reduce_nand()
{
}

vvp_bit4_t vvp_reduce_nand::calculate_result() ধ্রুব
{
      প্রত্যাগম ~bits_.reduce_and();
}

বর্গ vvp_reduce_nor  : সার্বজনিক vvp_reduce_base {

    সার্বজনিক:
      vvp_reduce_nor();
      ~vvp_reduce_nor() override;
      vvp_bit4_t calculate_result() ধ্রুব override;
};

vvp_reduce_nor::vvp_reduce_nor()
{
}

vvp_reduce_nor::~vvp_reduce_nor()
{
}

vvp_bit4_t vvp_reduce_nor::calculate_result() ধ্রুব
{
      প্রত্যাগম ~bits_.reduce_or();
}

বর্গ vvp_reduce_xnor  : সার্বজনিক vvp_reduce_base {

    সার্বজনিক:
      vvp_reduce_xnor();
      ~vvp_reduce_xnor() override;
      vvp_bit4_t calculate_result() ধ্রুব override;
};

vvp_reduce_xnor::vvp_reduce_xnor()
{
}

vvp_reduce_xnor::~vvp_reduce_xnor()
{
}

vvp_bit4_t vvp_reduce_xnor::calculate_result() ধ্রুব
{
      প্রত্যাগম ~bits_.reduce_xor();
}

স্থির শূন্য make_reduce(বর্ণ*label, vvp_net_fun_t*red, ধ্রুব সংরচনা symb_s&arg)
{
      vvp_net_t*ptr = নূতন vvp_net_t;
      ptr->fun = red;

      define_functor_প্রতীক(label, ptr);
      মুক্ত(label);

      প্রবেশ_connect(ptr, 0, arg.text);
}

শূন্য compile_reduce_and(বর্ণ*label, ধ্রুব সংরচনা symb_s&arg)
{
      vvp_reduce_and* reduce = নূতন vvp_reduce_and;
      make_reduce(label, reduce, arg);
}

শূন্য compile_reduce_or(বর্ণ*label, ধ্রুব সংরচনা symb_s&arg)
{
      vvp_reduce_or* reduce = নূতন vvp_reduce_or;
      make_reduce(label, reduce, arg);
}

শূন্য compile_reduce_xor(বর্ণ*label, ধ্রুব সংরচনা symb_s&arg)
{
      vvp_reduce_xor* reduce = নূতন vvp_reduce_xor;
      make_reduce(label, reduce, arg);
}

শূন্য compile_reduce_nand(বর্ণ*label, ধ্রুব সংরচনা symb_s&arg)
{
      vvp_reduce_nand* reduce = নূতন vvp_reduce_nand;
      make_reduce(label, reduce, arg);
}

শূন্য compile_reduce_nor(বর্ণ*label, ধ্রুব সংরচনা symb_s&arg)
{
      vvp_reduce_nor* reduce = নূতন vvp_reduce_nor;
      make_reduce(label, reduce, arg);
}

শূন্য compile_reduce_xnor(বর্ণ*label, ধ্রুব সংরচনা symb_s&arg)
{
      vvp_reduce_xnor* reduce = নূতন vvp_reduce_xnor;
      make_reduce(label, reduce, arg);
}
