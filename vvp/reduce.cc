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
ਵਰਗ vvp_reduce_ਅਧਾਰ : ਜਨਤਕ vvp_net_fun_t {

    ਜਨਤਕ:
      vvp_reduce_ਅਧਾਰ();
      ਆਭਾਸੀ ~vvp_reduce_ਅਧਾਰ() override;

      ਖਾਲੀ recv_vec4(vvp_net_ptr_t prt, ਅਚਲ vvp_vector4_t&ਦੋਆਧਾਰੀ_ਅੰਕ,
                     vvp_context_t context) override;
      ਖਾਲੀ recv_vec4_pv(vvp_net_ptr_t ptr, ਅਚਲ vvp_vector4_t&ਦੋਆਧਾਰੀ_ਅੰਕ,
			ਬਿਨਾਨਿਸ਼ਾਨੀ ਅਧਾਰ, ਬਿਨਾਨਿਸ਼ਾਨੀ vwid, vvp_context_t context) override;

      ਆਭਾਸੀ vvp_bit4_t calculate_ਨਤੀਜਾ() ਅਚਲ =0;

    ਸੁਰੱਖਿਅਤ:
      vvp_vector4_t ਬਿੱਟ_2;
};

vvp_reduce_ਅਧਾਰ::vvp_reduce_ਅਧਾਰ()
{
}

vvp_reduce_ਅਧਾਰ::~vvp_reduce_ਅਧਾਰ()
{
}

ਖਾਲੀ vvp_reduce_ਅਧਾਰ::recv_vec4(vvp_net_ptr_t prt, ਅਚਲ vvp_vector4_t&ਦੋਆਧਾਰੀ_ਅੰਕ,
                                vvp_context_t context)
{
      ਬਿੱਟ_2 = ਦੋਆਧਾਰੀ_ਅੰਕ;
      vvp_bit4_t res =  calculate_ਨਤੀਜਾ();
      vvp_vector4_t rv (1, res);
      prt.ptr()->send_vec4(rv, context);
}

ਖਾਲੀ vvp_reduce_ਅਧਾਰ::recv_vec4_pv(vvp_net_ptr_t prt, ਅਚਲ vvp_vector4_t&ਦੋਆਧਾਰੀ_ਅੰਕ,
				   ਬਿਨਾਨਿਸ਼ਾਨੀ ਅਧਾਰ, ਬਿਨਾਨਿਸ਼ਾਨੀ vwid, vvp_context_t context)
{
      ਜੇ (ਬਿੱਟ_2.size() == 0) {
	    ਬਿੱਟ_2 = vvp_vector4_t(vwid);
      }
      ਪ੍ਰਤਿਜ੍ਞਾ(ਬਿੱਟ_2.size() == vwid);

      ਬਿੱਟ_2.ਸੈੱਟ_vec(ਅਧਾਰ, ਦੋਆਧਾਰੀ_ਅੰਕ);
      vvp_bit4_t res = calculate_ਨਤੀਜਾ();
      vvp_vector4_t rv (1, res);
      prt.ptr()->send_vec4(rv, context);
}

ਵਰਗ vvp_reduce_and  : ਜਨਤਕ vvp_reduce_ਅਧਾਰ {

    ਜਨਤਕ:
      vvp_reduce_and();
      ~vvp_reduce_and() override;
      vvp_bit4_t calculate_ਨਤੀਜਾ() ਅਚਲ override;
};

vvp_reduce_and::vvp_reduce_and()
{
}

vvp_reduce_and::~vvp_reduce_and()
{
}

vvp_bit4_t vvp_reduce_and::calculate_ਨਤੀਜਾ() ਅਚਲ
{
      ਵਾਪਸ ਬਿੱਟ_2.reduce_and();
}

ਵਰਗ vvp_reduce_or  : ਜਨਤਕ vvp_reduce_ਅਧਾਰ {

    ਜਨਤਕ:
      vvp_reduce_or();
      ~vvp_reduce_or() override;
      vvp_bit4_t calculate_ਨਤੀਜਾ() ਅਚਲ override;
};

vvp_reduce_or::vvp_reduce_or()
{
}

vvp_reduce_or::~vvp_reduce_or()
{
}

vvp_bit4_t vvp_reduce_or::calculate_ਨਤੀਜਾ() ਅਚਲ
{
      ਵਾਪਸ ਬਿੱਟ_2.reduce_or();
}

ਵਰਗ vvp_reduce_xor  : ਜਨਤਕ vvp_reduce_ਅਧਾਰ {

    ਜਨਤਕ:
      vvp_reduce_xor();
      ~vvp_reduce_xor() override;
      vvp_bit4_t calculate_ਨਤੀਜਾ() ਅਚਲ override;
};

vvp_reduce_xor::vvp_reduce_xor()
{
}

vvp_reduce_xor::~vvp_reduce_xor()
{
}

vvp_bit4_t vvp_reduce_xor::calculate_ਨਤੀਜਾ() ਅਚਲ
{
      ਵਾਪਸ ਬਿੱਟ_2.reduce_xor();
}

ਵਰਗ vvp_reduce_nand  : ਜਨਤਕ vvp_reduce_ਅਧਾਰ {

    ਜਨਤਕ:
      vvp_reduce_nand();
      ~vvp_reduce_nand() override;
      vvp_bit4_t calculate_ਨਤੀਜਾ() ਅਚਲ override;
};

vvp_reduce_nand::vvp_reduce_nand()
{
}

vvp_reduce_nand::~vvp_reduce_nand()
{
}

vvp_bit4_t vvp_reduce_nand::calculate_ਨਤੀਜਾ() ਅਚਲ
{
      ਵਾਪਸ ~ਬਿੱਟ_2.reduce_and();
}

ਵਰਗ vvp_reduce_nor  : ਜਨਤਕ vvp_reduce_ਅਧਾਰ {

    ਜਨਤਕ:
      vvp_reduce_nor();
      ~vvp_reduce_nor() override;
      vvp_bit4_t calculate_ਨਤੀਜਾ() ਅਚਲ override;
};

vvp_reduce_nor::vvp_reduce_nor()
{
}

vvp_reduce_nor::~vvp_reduce_nor()
{
}

vvp_bit4_t vvp_reduce_nor::calculate_ਨਤੀਜਾ() ਅਚਲ
{
      ਵਾਪਸ ~ਬਿੱਟ_2.reduce_or();
}

ਵਰਗ vvp_reduce_xnor  : ਜਨਤਕ vvp_reduce_ਅਧਾਰ {

    ਜਨਤਕ:
      vvp_reduce_xnor();
      ~vvp_reduce_xnor() override;
      vvp_bit4_t calculate_ਨਤੀਜਾ() ਅਚਲ override;
};

vvp_reduce_xnor::vvp_reduce_xnor()
{
}

vvp_reduce_xnor::~vvp_reduce_xnor()
{
}

vvp_bit4_t vvp_reduce_xnor::calculate_ਨਤੀਜਾ() ਅਚਲ
{
      ਵਾਪਸ ~ਬਿੱਟ_2.reduce_xor();
}

ਸਥਿਰ ਖਾਲੀ make_reduce(ਅੱਖਰ*label, vvp_net_fun_t*red, ਅਚਲ ਬਣਤਰ symb_s&arg)
{
      vvp_net_t*ptr = ਨਵਾਂ vvp_net_t;
      ptr->fun = red;

      define_functor_ਚਿੰਨ੍ਹ(label, ptr);
      ਮੁਕਤ(label);

      ਨਿਵੇਸ਼_connect(ptr, 0, arg.ਪਾਠ);
}

ਖਾਲੀ compile_reduce_and(ਅੱਖਰ*label, ਅਚਲ ਬਣਤਰ symb_s&arg)
{
      vvp_reduce_and* reduce = ਨਵਾਂ vvp_reduce_and;
      make_reduce(label, reduce, arg);
}

ਖਾਲੀ compile_reduce_or(ਅੱਖਰ*label, ਅਚਲ ਬਣਤਰ symb_s&arg)
{
      vvp_reduce_or* reduce = ਨਵਾਂ vvp_reduce_or;
      make_reduce(label, reduce, arg);
}

ਖਾਲੀ compile_reduce_xor(ਅੱਖਰ*label, ਅਚਲ ਬਣਤਰ symb_s&arg)
{
      vvp_reduce_xor* reduce = ਨਵਾਂ vvp_reduce_xor;
      make_reduce(label, reduce, arg);
}

ਖਾਲੀ compile_reduce_nand(ਅੱਖਰ*label, ਅਚਲ ਬਣਤਰ symb_s&arg)
{
      vvp_reduce_nand* reduce = ਨਵਾਂ vvp_reduce_nand;
      make_reduce(label, reduce, arg);
}

ਖਾਲੀ compile_reduce_nor(ਅੱਖਰ*label, ਅਚਲ ਬਣਤਰ symb_s&arg)
{
      vvp_reduce_nor* reduce = ਨਵਾਂ vvp_reduce_nor;
      make_reduce(label, reduce, arg);
}

ਖਾਲੀ compile_reduce_xnor(ਅੱਖਰ*label, ਅਚਲ ਬਣਤਰ symb_s&arg)
{
      vvp_reduce_xnor* reduce = ਨਵਾਂ vvp_reduce_xnor;
      make_reduce(label, reduce, arg);
}
