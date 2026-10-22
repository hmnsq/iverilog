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
વર્ગ vvp_reduce_base : જાહેર vvp_net_fun_t {

    જાહેર:
      vvp_reduce_base();
      આભાસી ~vvp_reduce_base() override;

      શૂન્ય recv_vec4(vvp_net_ptr_t prt, અચલ vvp_vector4_t&bit,
                     vvp_context_t context) override;
      શૂન્ય recv_vec4_pv(vvp_net_ptr_t ptr, અચલ vvp_vector4_t&bit,
			unsigned base, unsigned vwid, vvp_context_t context) override;

      આભાસી vvp_bit4_t calculate_result() અચલ =0;

    સુરક્ષિત:
      vvp_vector4_t bits_;
};

vvp_reduce_base::vvp_reduce_base()
{
}

vvp_reduce_base::~vvp_reduce_base()
{
}

શૂન્ય vvp_reduce_base::recv_vec4(vvp_net_ptr_t prt, અચલ vvp_vector4_t&bit,
                                vvp_context_t context)
{
      bits_ = bit;
      vvp_bit4_t res =  calculate_result();
      vvp_vector4_t rv (1, res);
      prt.ptr()->send_vec4(rv, context);
}

શૂન્ય vvp_reduce_base::recv_vec4_pv(vvp_net_ptr_t prt, અચલ vvp_vector4_t&bit,
				   unsigned base, unsigned vwid, vvp_context_t context)
{
      જો (bits_.size() == 0) {
	    bits_ = vvp_vector4_t(vwid);
      }
      પ્રતિજ્ઞા(bits_.size() == vwid);

      bits_.set_vec(base, bit);
      vvp_bit4_t res = calculate_result();
      vvp_vector4_t rv (1, res);
      prt.ptr()->send_vec4(rv, context);
}

વર્ગ vvp_reduce_and  : જાહેર vvp_reduce_base {

    જાહેર:
      vvp_reduce_and();
      ~vvp_reduce_and() override;
      vvp_bit4_t calculate_result() અચલ override;
};

vvp_reduce_and::vvp_reduce_and()
{
}

vvp_reduce_and::~vvp_reduce_and()
{
}

vvp_bit4_t vvp_reduce_and::calculate_result() અચલ
{
      પાછા bits_.reduce_and();
}

વર્ગ vvp_reduce_or  : જાહેર vvp_reduce_base {

    જાહેર:
      vvp_reduce_or();
      ~vvp_reduce_or() override;
      vvp_bit4_t calculate_result() અચલ override;
};

vvp_reduce_or::vvp_reduce_or()
{
}

vvp_reduce_or::~vvp_reduce_or()
{
}

vvp_bit4_t vvp_reduce_or::calculate_result() અચલ
{
      પાછા bits_.reduce_or();
}

વર્ગ vvp_reduce_xor  : જાહેર vvp_reduce_base {

    જાહેર:
      vvp_reduce_xor();
      ~vvp_reduce_xor() override;
      vvp_bit4_t calculate_result() અચલ override;
};

vvp_reduce_xor::vvp_reduce_xor()
{
}

vvp_reduce_xor::~vvp_reduce_xor()
{
}

vvp_bit4_t vvp_reduce_xor::calculate_result() અચલ
{
      પાછા bits_.reduce_xor();
}

વર્ગ vvp_reduce_nand  : જાહેર vvp_reduce_base {

    જાહેર:
      vvp_reduce_nand();
      ~vvp_reduce_nand() override;
      vvp_bit4_t calculate_result() અચલ override;
};

vvp_reduce_nand::vvp_reduce_nand()
{
}

vvp_reduce_nand::~vvp_reduce_nand()
{
}

vvp_bit4_t vvp_reduce_nand::calculate_result() અચલ
{
      પાછા ~bits_.reduce_and();
}

વર્ગ vvp_reduce_nor  : જાહેર vvp_reduce_base {

    જાહેર:
      vvp_reduce_nor();
      ~vvp_reduce_nor() override;
      vvp_bit4_t calculate_result() અચલ override;
};

vvp_reduce_nor::vvp_reduce_nor()
{
}

vvp_reduce_nor::~vvp_reduce_nor()
{
}

vvp_bit4_t vvp_reduce_nor::calculate_result() અચલ
{
      પાછા ~bits_.reduce_or();
}

વર્ગ vvp_reduce_xnor  : જાહેર vvp_reduce_base {

    જાહેર:
      vvp_reduce_xnor();
      ~vvp_reduce_xnor() override;
      vvp_bit4_t calculate_result() અચલ override;
};

vvp_reduce_xnor::vvp_reduce_xnor()
{
}

vvp_reduce_xnor::~vvp_reduce_xnor()
{
}

vvp_bit4_t vvp_reduce_xnor::calculate_result() અચલ
{
      પાછા ~bits_.reduce_xor();
}

સ્થિર શૂન્ય make_reduce(અક્ષર*label, vvp_net_fun_t*red, અચલ સંરચના symb_s&arg)
{
      vvp_net_t*ptr = નવું vvp_net_t;
      ptr->fun = red;

      define_functor_પ્રતીક(label, ptr);
      મુક્ત(label);

      આગત_connect(ptr, 0, arg.text);
}

શૂન્ય compile_reduce_and(અક્ષર*label, અચલ સંરચના symb_s&arg)
{
      vvp_reduce_and* reduce = નવું vvp_reduce_and;
      make_reduce(label, reduce, arg);
}

શૂન્ય compile_reduce_or(અક્ષર*label, અચલ સંરચના symb_s&arg)
{
      vvp_reduce_or* reduce = નવું vvp_reduce_or;
      make_reduce(label, reduce, arg);
}

શૂન્ય compile_reduce_xor(અક્ષર*label, અચલ સંરચના symb_s&arg)
{
      vvp_reduce_xor* reduce = નવું vvp_reduce_xor;
      make_reduce(label, reduce, arg);
}

શૂન્ય compile_reduce_nand(અક્ષર*label, અચલ સંરચના symb_s&arg)
{
      vvp_reduce_nand* reduce = નવું vvp_reduce_nand;
      make_reduce(label, reduce, arg);
}

શૂન્ય compile_reduce_nor(અક્ષર*label, અચલ સંરચના symb_s&arg)
{
      vvp_reduce_nor* reduce = નવું vvp_reduce_nor;
      make_reduce(label, reduce, arg);
}

શૂન્ય compile_reduce_xnor(અક્ષર*label, અચલ સંરચના symb_s&arg)
{
      vvp_reduce_xnor* reduce = નવું vvp_reduce_xnor;
      make_reduce(label, reduce, arg);
}
