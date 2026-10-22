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
ವರ್ಗ vvp_reduce_ತಳ : ಸಾರ್ವಜನಿಕ vvp_net_fun_t {

    ಸಾರ್ವಜನಿಕ:
      vvp_reduce_ತಳ();
      ವಾಸ್ತವಿಕ ~vvp_reduce_ತಳ() override;

      ಶೂನ್ಯ recv_vec4(vvp_net_ptr_t prt, ಸ್ಥಿರಾಂಕ vvp_vector4_t&ಬಿಟ್,
                     vvp_context_t context) override;
      ಶೂನ್ಯ recv_vec4_pv(vvp_net_ptr_t ptr, ಸ್ಥಿರಾಂಕ vvp_vector4_t&ಬಿಟ್,
			ಚಿಹ್ನೆರಹಿತ ತಳ, ಚಿಹ್ನೆರಹಿತ vwid, vvp_context_t context) override;

      ವಾಸ್ತವಿಕ vvp_bit4_t calculate_ಫಲಿತಾಂಶ() ಸ್ಥಿರಾಂಕ =0;

    ಸಂರಕ್ಷಿತ:
      vvp_vector4_t ಬಿಟ್ಟುಗಳು_2;
};

vvp_reduce_ತಳ::vvp_reduce_ತಳ()
{
}

vvp_reduce_ತಳ::~vvp_reduce_ತಳ()
{
}

ಶೂನ್ಯ vvp_reduce_ತಳ::recv_vec4(vvp_net_ptr_t prt, ಸ್ಥಿರಾಂಕ vvp_vector4_t&ಬಿಟ್,
                                vvp_context_t context)
{
      ಬಿಟ್ಟುಗಳು_2 = ಬಿಟ್;
      vvp_bit4_t res =  calculate_ಫಲಿತಾಂಶ();
      vvp_vector4_t rv (1, res);
      prt.ptr()->send_vec4(rv, context);
}

ಶೂನ್ಯ vvp_reduce_ತಳ::recv_vec4_pv(vvp_net_ptr_t prt, ಸ್ಥಿರಾಂಕ vvp_vector4_t&ಬಿಟ್,
				   ಚಿಹ್ನೆರಹಿತ ತಳ, ಚಿಹ್ನೆರಹಿತ vwid, vvp_context_t context)
{
      ಆದರೆ (ಬಿಟ್ಟುಗಳು_2.size() == 0) {
	    ಬಿಟ್ಟುಗಳು_2 = vvp_vector4_t(vwid);
      }
      ಪ್ರತಿಜ್ಞಾ(ಬಿಟ್ಟುಗಳು_2.size() == vwid);

      ಬಿಟ್ಟುಗಳು_2.ಗಣ_vec(ತಳ, ಬಿಟ್);
      vvp_bit4_t res = calculate_ಫಲಿತಾಂಶ();
      vvp_vector4_t rv (1, res);
      prt.ptr()->send_vec4(rv, context);
}

ವರ್ಗ vvp_reduce_and  : ಸಾರ್ವಜನಿಕ vvp_reduce_ತಳ {

    ಸಾರ್ವಜನಿಕ:
      vvp_reduce_and();
      ~vvp_reduce_and() override;
      vvp_bit4_t calculate_ಫಲಿತಾಂಶ() ಸ್ಥಿರಾಂಕ override;
};

vvp_reduce_and::vvp_reduce_and()
{
}

vvp_reduce_and::~vvp_reduce_and()
{
}

vvp_bit4_t vvp_reduce_and::calculate_ಫಲಿತಾಂಶ() ಸ್ಥಿರಾಂಕ
{
      ಹಿಂತಿರುಗಿ ಬಿಟ್ಟುಗಳು_2.reduce_and();
}

ವರ್ಗ vvp_reduce_or  : ಸಾರ್ವಜನಿಕ vvp_reduce_ತಳ {

    ಸಾರ್ವಜನಿಕ:
      vvp_reduce_or();
      ~vvp_reduce_or() override;
      vvp_bit4_t calculate_ಫಲಿತಾಂಶ() ಸ್ಥಿರಾಂಕ override;
};

vvp_reduce_or::vvp_reduce_or()
{
}

vvp_reduce_or::~vvp_reduce_or()
{
}

vvp_bit4_t vvp_reduce_or::calculate_ಫಲಿತಾಂಶ() ಸ್ಥಿರಾಂಕ
{
      ಹಿಂತಿರುಗಿ ಬಿಟ್ಟುಗಳು_2.reduce_or();
}

ವರ್ಗ vvp_reduce_xor  : ಸಾರ್ವಜನಿಕ vvp_reduce_ತಳ {

    ಸಾರ್ವಜನಿಕ:
      vvp_reduce_xor();
      ~vvp_reduce_xor() override;
      vvp_bit4_t calculate_ಫಲಿತಾಂಶ() ಸ್ಥಿರಾಂಕ override;
};

vvp_reduce_xor::vvp_reduce_xor()
{
}

vvp_reduce_xor::~vvp_reduce_xor()
{
}

vvp_bit4_t vvp_reduce_xor::calculate_ಫಲಿತಾಂಶ() ಸ್ಥಿರಾಂಕ
{
      ಹಿಂತಿರುಗಿ ಬಿಟ್ಟುಗಳು_2.reduce_xor();
}

ವರ್ಗ vvp_reduce_nand  : ಸಾರ್ವಜನಿಕ vvp_reduce_ತಳ {

    ಸಾರ್ವಜನಿಕ:
      vvp_reduce_nand();
      ~vvp_reduce_nand() override;
      vvp_bit4_t calculate_ಫಲಿತಾಂಶ() ಸ್ಥಿರಾಂಕ override;
};

vvp_reduce_nand::vvp_reduce_nand()
{
}

vvp_reduce_nand::~vvp_reduce_nand()
{
}

vvp_bit4_t vvp_reduce_nand::calculate_ಫಲಿತಾಂಶ() ಸ್ಥಿರಾಂಕ
{
      ಹಿಂತಿರುಗಿ ~ಬಿಟ್ಟುಗಳು_2.reduce_and();
}

ವರ್ಗ vvp_reduce_nor  : ಸಾರ್ವಜನಿಕ vvp_reduce_ತಳ {

    ಸಾರ್ವಜನಿಕ:
      vvp_reduce_nor();
      ~vvp_reduce_nor() override;
      vvp_bit4_t calculate_ಫಲಿತಾಂಶ() ಸ್ಥಿರಾಂಕ override;
};

vvp_reduce_nor::vvp_reduce_nor()
{
}

vvp_reduce_nor::~vvp_reduce_nor()
{
}

vvp_bit4_t vvp_reduce_nor::calculate_ಫಲಿತಾಂಶ() ಸ್ಥಿರಾಂಕ
{
      ಹಿಂತಿರುಗಿ ~ಬಿಟ್ಟುಗಳು_2.reduce_or();
}

ವರ್ಗ vvp_reduce_xnor  : ಸಾರ್ವಜನಿಕ vvp_reduce_ತಳ {

    ಸಾರ್ವಜನಿಕ:
      vvp_reduce_xnor();
      ~vvp_reduce_xnor() override;
      vvp_bit4_t calculate_ಫಲಿತಾಂಶ() ಸ್ಥಿರಾಂಕ override;
};

vvp_reduce_xnor::vvp_reduce_xnor()
{
}

vvp_reduce_xnor::~vvp_reduce_xnor()
{
}

vvp_bit4_t vvp_reduce_xnor::calculate_ಫಲಿತಾಂಶ() ಸ್ಥಿರಾಂಕ
{
      ಹಿಂತಿರುಗಿ ~ಬಿಟ್ಟುಗಳು_2.reduce_xor();
}

ಸ್ಥಿರ ಶೂನ್ಯ make_reduce(ಅಕ್ಷರ*label, vvp_net_fun_t*red, ಸ್ಥಿರಾಂಕ ರಚನೆ symb_s&arg)
{
      vvp_net_t*ptr = ಹೊಸ vvp_net_t;
      ptr->fun = red;

      define_functor_ಚಿಹ್ನೆ(label, ptr);
      ಬಿಡುಗಡೆ(label);

      ಒಳಹರಿವು_connect(ptr, 0, arg.ಪಠ್ಯ);
}

ಶೂನ್ಯ compile_reduce_and(ಅಕ್ಷರ*label, ಸ್ಥಿರಾಂಕ ರಚನೆ symb_s&arg)
{
      vvp_reduce_and* reduce = ಹೊಸ vvp_reduce_and;
      make_reduce(label, reduce, arg);
}

ಶೂನ್ಯ compile_reduce_or(ಅಕ್ಷರ*label, ಸ್ಥಿರಾಂಕ ರಚನೆ symb_s&arg)
{
      vvp_reduce_or* reduce = ಹೊಸ vvp_reduce_or;
      make_reduce(label, reduce, arg);
}

ಶೂನ್ಯ compile_reduce_xor(ಅಕ್ಷರ*label, ಸ್ಥಿರಾಂಕ ರಚನೆ symb_s&arg)
{
      vvp_reduce_xor* reduce = ಹೊಸ vvp_reduce_xor;
      make_reduce(label, reduce, arg);
}

ಶೂನ್ಯ compile_reduce_nand(ಅಕ್ಷರ*label, ಸ್ಥಿರಾಂಕ ರಚನೆ symb_s&arg)
{
      vvp_reduce_nand* reduce = ಹೊಸ vvp_reduce_nand;
      make_reduce(label, reduce, arg);
}

ಶೂನ್ಯ compile_reduce_nor(ಅಕ್ಷರ*label, ಸ್ಥಿರಾಂಕ ರಚನೆ symb_s&arg)
{
      vvp_reduce_nor* reduce = ಹೊಸ vvp_reduce_nor;
      make_reduce(label, reduce, arg);
}

ಶೂನ್ಯ compile_reduce_xnor(ಅಕ್ಷರ*label, ಸ್ಥಿರಾಂಕ ರಚನೆ symb_s&arg)
{
      vvp_reduce_xnor* reduce = ಹೊಸ vvp_reduce_xnor;
      make_reduce(label, reduce, arg);
}
