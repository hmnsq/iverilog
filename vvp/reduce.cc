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
വർഗ്ഗം vvp_reduce_base : പൊതു vvp_net_fun_t {

    പൊതു:
      vvp_reduce_base();
      മായാ ~vvp_reduce_base() override;

      ശൂന്യം recv_vec4(vvp_net_ptr_t prt, സ്ഥിരം vvp_vector4_t&bit,
                     vvp_context_t context) override;
      ശൂന്യം recv_vec4_pv(vvp_net_ptr_t ptr, സ്ഥിരം vvp_vector4_t&bit,
			unsigned base, unsigned vwid, vvp_context_t context) override;

      മായാ vvp_bit4_t calculate_result() സ്ഥിരം =0;

    സംരക്ഷിത:
      vvp_vector4_t bits_;
};

vvp_reduce_base::vvp_reduce_base()
{
}

vvp_reduce_base::~vvp_reduce_base()
{
}

ശൂന്യം vvp_reduce_base::recv_vec4(vvp_net_ptr_t prt, സ്ഥിരം vvp_vector4_t&bit,
                                vvp_context_t context)
{
      bits_ = bit;
      vvp_bit4_t res =  calculate_result();
      vvp_vector4_t rv (1, res);
      prt.ptr()->send_vec4(rv, context);
}

ശൂന്യം vvp_reduce_base::recv_vec4_pv(vvp_net_ptr_t prt, സ്ഥിരം vvp_vector4_t&bit,
				   unsigned base, unsigned vwid, vvp_context_t context)
{
      എങ്കിൽ (bits_.size() == 0) {
	    bits_ = vvp_vector4_t(vwid);
      }
      പ്രതിജ്ഞാ(bits_.size() == vwid);

      bits_.set_vec(base, bit);
      vvp_bit4_t res = calculate_result();
      vvp_vector4_t rv (1, res);
      prt.ptr()->send_vec4(rv, context);
}

വർഗ്ഗം vvp_reduce_and  : പൊതു vvp_reduce_base {

    പൊതു:
      vvp_reduce_and();
      ~vvp_reduce_and() override;
      vvp_bit4_t calculate_result() സ്ഥിരം override;
};

vvp_reduce_and::vvp_reduce_and()
{
}

vvp_reduce_and::~vvp_reduce_and()
{
}

vvp_bit4_t vvp_reduce_and::calculate_result() സ്ഥിരം
{
      തിരികെ bits_.reduce_and();
}

വർഗ്ഗം vvp_reduce_or  : പൊതു vvp_reduce_base {

    പൊതു:
      vvp_reduce_or();
      ~vvp_reduce_or() override;
      vvp_bit4_t calculate_result() സ്ഥിരം override;
};

vvp_reduce_or::vvp_reduce_or()
{
}

vvp_reduce_or::~vvp_reduce_or()
{
}

vvp_bit4_t vvp_reduce_or::calculate_result() സ്ഥിരം
{
      തിരികെ bits_.reduce_or();
}

വർഗ്ഗം vvp_reduce_xor  : പൊതു vvp_reduce_base {

    പൊതു:
      vvp_reduce_xor();
      ~vvp_reduce_xor() override;
      vvp_bit4_t calculate_result() സ്ഥിരം override;
};

vvp_reduce_xor::vvp_reduce_xor()
{
}

vvp_reduce_xor::~vvp_reduce_xor()
{
}

vvp_bit4_t vvp_reduce_xor::calculate_result() സ്ഥിരം
{
      തിരികെ bits_.reduce_xor();
}

വർഗ്ഗം vvp_reduce_nand  : പൊതു vvp_reduce_base {

    പൊതു:
      vvp_reduce_nand();
      ~vvp_reduce_nand() override;
      vvp_bit4_t calculate_result() സ്ഥിരം override;
};

vvp_reduce_nand::vvp_reduce_nand()
{
}

vvp_reduce_nand::~vvp_reduce_nand()
{
}

vvp_bit4_t vvp_reduce_nand::calculate_result() സ്ഥിരം
{
      തിരികെ ~bits_.reduce_and();
}

വർഗ്ഗം vvp_reduce_nor  : പൊതു vvp_reduce_base {

    പൊതു:
      vvp_reduce_nor();
      ~vvp_reduce_nor() override;
      vvp_bit4_t calculate_result() സ്ഥിരം override;
};

vvp_reduce_nor::vvp_reduce_nor()
{
}

vvp_reduce_nor::~vvp_reduce_nor()
{
}

vvp_bit4_t vvp_reduce_nor::calculate_result() സ്ഥിരം
{
      തിരികെ ~bits_.reduce_or();
}

വർഗ്ഗം vvp_reduce_xnor  : പൊതു vvp_reduce_base {

    പൊതു:
      vvp_reduce_xnor();
      ~vvp_reduce_xnor() override;
      vvp_bit4_t calculate_result() സ്ഥിരം override;
};

vvp_reduce_xnor::vvp_reduce_xnor()
{
}

vvp_reduce_xnor::~vvp_reduce_xnor()
{
}

vvp_bit4_t vvp_reduce_xnor::calculate_result() സ്ഥിരം
{
      തിരികെ ~bits_.reduce_xor();
}

സ്ഥിതം ശൂന്യം make_reduce(അക്ഷരം*label, vvp_net_fun_t*red, സ്ഥിരം ഘടന symb_s&arg)
{
      vvp_net_t*ptr = പുതിയ vvp_net_t;
      ptr->fun = red;

      define_functor_ചിഹ്നം(label, ptr);
      സ്വതന്ത്രം(label);

      നിവേശം_connect(ptr, 0, arg.text);
}

ശൂന്യം compile_reduce_and(അക്ഷരം*label, സ്ഥിരം ഘടന symb_s&arg)
{
      vvp_reduce_and* reduce = പുതിയ vvp_reduce_and;
      make_reduce(label, reduce, arg);
}

ശൂന്യം compile_reduce_or(അക്ഷരം*label, സ്ഥിരം ഘടന symb_s&arg)
{
      vvp_reduce_or* reduce = പുതിയ vvp_reduce_or;
      make_reduce(label, reduce, arg);
}

ശൂന്യം compile_reduce_xor(അക്ഷരം*label, സ്ഥിരം ഘടന symb_s&arg)
{
      vvp_reduce_xor* reduce = പുതിയ vvp_reduce_xor;
      make_reduce(label, reduce, arg);
}

ശൂന്യം compile_reduce_nand(അക്ഷരം*label, സ്ഥിരം ഘടന symb_s&arg)
{
      vvp_reduce_nand* reduce = പുതിയ vvp_reduce_nand;
      make_reduce(label, reduce, arg);
}

ശൂന്യം compile_reduce_nor(അക്ഷരം*label, സ്ഥിരം ഘടന symb_s&arg)
{
      vvp_reduce_nor* reduce = പുതിയ vvp_reduce_nor;
      make_reduce(label, reduce, arg);
}

ശൂന്യം compile_reduce_xnor(അക്ഷരം*label, സ്ഥിരം ഘടന symb_s&arg)
{
      vvp_reduce_xnor* reduce = പുതിയ vvp_reduce_xnor;
      make_reduce(label, reduce, arg);
}
