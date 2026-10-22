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
ವರ್ಗ vvp_reduce_base : ಸಾರ್ವಜನಿಕ vvp_net_fun_t {

    ಸಾರ್ವಜನಿಕ:
      vvp_reduce_base();
      ಆಭಾಸ ~vvp_reduce_base() override;

      ಶೂನ್ಯ recv_vec4(vvp_net_ptr_t prt, ಧ್ರುವ vvp_vector4_t&bit,
                     vvp_context_t context) override;
      ಶೂನ್ಯ recv_vec4_pv(vvp_net_ptr_t ptr, ಧ್ರುವ vvp_vector4_t&bit,
			unsigned base, unsigned vwid, vvp_context_t context) override;

      ಆಭಾಸ vvp_bit4_t calculate_result() ಧ್ರುವ =0;

    ರಕ್ಷಿತ:
      vvp_vector4_t bits_;
};

vvp_reduce_base::vvp_reduce_base()
{
}

vvp_reduce_base::~vvp_reduce_base()
{
}

ಶೂನ್ಯ vvp_reduce_base::recv_vec4(vvp_net_ptr_t prt, ಧ್ರುವ vvp_vector4_t&bit,
                                vvp_context_t context)
{
      bits_ = bit;
      vvp_bit4_t res =  calculate_result();
      vvp_vector4_t rv (1, res);
      prt.ptr()->send_vec4(rv, context);
}

ಶೂನ್ಯ vvp_reduce_base::recv_vec4_pv(vvp_net_ptr_t prt, ಧ್ರುವ vvp_vector4_t&bit,
				   unsigned base, unsigned vwid, vvp_context_t context)
{
      ಯದಿ (bits_.size() == 0) {
	    bits_ = vvp_vector4_t(vwid);
      }
      ಪ್ರತಿಜ್ಞಾ(bits_.size() == vwid);

      bits_.set_vec(base, bit);
      vvp_bit4_t res = calculate_result();
      vvp_vector4_t rv (1, res);
      prt.ptr()->send_vec4(rv, context);
}

ವರ್ಗ vvp_reduce_and  : ಸಾರ್ವಜನಿಕ vvp_reduce_base {

    ಸಾರ್ವಜನಿಕ:
      vvp_reduce_and();
      ~vvp_reduce_and() override;
      vvp_bit4_t calculate_result() ಧ್ರುವ override;
};

vvp_reduce_and::vvp_reduce_and()
{
}

vvp_reduce_and::~vvp_reduce_and()
{
}

vvp_bit4_t vvp_reduce_and::calculate_result() ಧ್ರುವ
{
      ಪ್ರತ್ಯಾಗಮ bits_.reduce_and();
}

ವರ್ಗ vvp_reduce_or  : ಸಾರ್ವಜನಿಕ vvp_reduce_base {

    ಸಾರ್ವಜನಿಕ:
      vvp_reduce_or();
      ~vvp_reduce_or() override;
      vvp_bit4_t calculate_result() ಧ್ರುವ override;
};

vvp_reduce_or::vvp_reduce_or()
{
}

vvp_reduce_or::~vvp_reduce_or()
{
}

vvp_bit4_t vvp_reduce_or::calculate_result() ಧ್ರುವ
{
      ಪ್ರತ್ಯಾಗಮ bits_.reduce_or();
}

ವರ್ಗ vvp_reduce_xor  : ಸಾರ್ವಜನಿಕ vvp_reduce_base {

    ಸಾರ್ವಜನಿಕ:
      vvp_reduce_xor();
      ~vvp_reduce_xor() override;
      vvp_bit4_t calculate_result() ಧ್ರುವ override;
};

vvp_reduce_xor::vvp_reduce_xor()
{
}

vvp_reduce_xor::~vvp_reduce_xor()
{
}

vvp_bit4_t vvp_reduce_xor::calculate_result() ಧ್ರುವ
{
      ಪ್ರತ್ಯಾಗಮ bits_.reduce_xor();
}

ವರ್ಗ vvp_reduce_nand  : ಸಾರ್ವಜನಿಕ vvp_reduce_base {

    ಸಾರ್ವಜನಿಕ:
      vvp_reduce_nand();
      ~vvp_reduce_nand() override;
      vvp_bit4_t calculate_result() ಧ್ರುವ override;
};

vvp_reduce_nand::vvp_reduce_nand()
{
}

vvp_reduce_nand::~vvp_reduce_nand()
{
}

vvp_bit4_t vvp_reduce_nand::calculate_result() ಧ್ರುವ
{
      ಪ್ರತ್ಯಾಗಮ ~bits_.reduce_and();
}

ವರ್ಗ vvp_reduce_nor  : ಸಾರ್ವಜನಿಕ vvp_reduce_base {

    ಸಾರ್ವಜನಿಕ:
      vvp_reduce_nor();
      ~vvp_reduce_nor() override;
      vvp_bit4_t calculate_result() ಧ್ರುವ override;
};

vvp_reduce_nor::vvp_reduce_nor()
{
}

vvp_reduce_nor::~vvp_reduce_nor()
{
}

vvp_bit4_t vvp_reduce_nor::calculate_result() ಧ್ರುವ
{
      ಪ್ರತ್ಯಾಗಮ ~bits_.reduce_or();
}

ವರ್ಗ vvp_reduce_xnor  : ಸಾರ್ವಜನಿಕ vvp_reduce_base {

    ಸಾರ್ವಜನಿಕ:
      vvp_reduce_xnor();
      ~vvp_reduce_xnor() override;
      vvp_bit4_t calculate_result() ಧ್ರುವ override;
};

vvp_reduce_xnor::vvp_reduce_xnor()
{
}

vvp_reduce_xnor::~vvp_reduce_xnor()
{
}

vvp_bit4_t vvp_reduce_xnor::calculate_result() ಧ್ರುವ
{
      ಪ್ರತ್ಯಾಗಮ ~bits_.reduce_xor();
}

ಸ್ಥಿರ ಶೂನ್ಯ make_reduce(ವರ್ಣ*label, vvp_net_fun_t*red, ಧ್ರುವ ಸಂರಚನಾ symb_s&arg)
{
      vvp_net_t*ptr = ನೂತನ vvp_net_t;
      ptr->fun = red;

      define_functor_ಚಿಹ್ನೆ(label, ptr);
      ಮುಕ್ತ(label);

      ಒಳಹರಿವು_connect(ptr, 0, arg.text);
}

ಶೂನ್ಯ compile_reduce_and(ವರ್ಣ*label, ಧ್ರುವ ಸಂರಚನಾ symb_s&arg)
{
      vvp_reduce_and* reduce = ನೂತನ vvp_reduce_and;
      make_reduce(label, reduce, arg);
}

ಶೂನ್ಯ compile_reduce_or(ವರ್ಣ*label, ಧ್ರುವ ಸಂರಚನಾ symb_s&arg)
{
      vvp_reduce_or* reduce = ನೂತನ vvp_reduce_or;
      make_reduce(label, reduce, arg);
}

ಶೂನ್ಯ compile_reduce_xor(ವರ್ಣ*label, ಧ್ರುವ ಸಂರಚನಾ symb_s&arg)
{
      vvp_reduce_xor* reduce = ನೂತನ vvp_reduce_xor;
      make_reduce(label, reduce, arg);
}

ಶೂನ್ಯ compile_reduce_nand(ವರ್ಣ*label, ಧ್ರುವ ಸಂರಚನಾ symb_s&arg)
{
      vvp_reduce_nand* reduce = ನೂತನ vvp_reduce_nand;
      make_reduce(label, reduce, arg);
}

ಶೂನ್ಯ compile_reduce_nor(ವರ್ಣ*label, ಧ್ರುವ ಸಂರಚನಾ symb_s&arg)
{
      vvp_reduce_nor* reduce = ನೂತನ vvp_reduce_nor;
      make_reduce(label, reduce, arg);
}

ಶೂನ್ಯ compile_reduce_xnor(ವರ್ಣ*label, ಧ್ರುವ ಸಂರಚನಾ symb_s&arg)
{
      vvp_reduce_xnor* reduce = ನೂತನ vvp_reduce_xnor;
      make_reduce(label, reduce, arg);
}
