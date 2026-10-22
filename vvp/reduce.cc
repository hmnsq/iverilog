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
वर्ग vvp_reduce_base : सार्वजनिक vvp_net_fun_t {

    सार्वजनिक:
      vvp_reduce_base();
      आभास ~vvp_reduce_base() override;

      शून्य recv_vec4(vvp_net_ptr_t prt, अचर vvp_vector4_t&bit,
                     vvp_context_t context) override;
      शून्य recv_vec4_pv(vvp_net_ptr_t ptr, अचर vvp_vector4_t&bit,
			अचिन्हित base, अचिन्हित vwid, vvp_context_t context) override;

      आभास vvp_bit4_t calculate_result() अचर =0;

    रक्षित:
      vvp_vector4_t bits_;
};

vvp_reduce_base::vvp_reduce_base()
{
}

vvp_reduce_base::~vvp_reduce_base()
{
}

शून्य vvp_reduce_base::recv_vec4(vvp_net_ptr_t prt, अचर vvp_vector4_t&bit,
                                vvp_context_t context)
{
      bits_ = bit;
      vvp_bit4_t res =  calculate_result();
      vvp_vector4_t rv (1, res);
      prt.ptr()->send_vec4(rv, context);
}

शून्य vvp_reduce_base::recv_vec4_pv(vvp_net_ptr_t prt, अचर vvp_vector4_t&bit,
				   अचिन्हित base, अचिन्हित vwid, vvp_context_t context)
{
      यदि (bits_.size() == 0) {
	    bits_ = vvp_vector4_t(vwid);
      }
      प्रतिज्ञा(bits_.size() == vwid);

      bits_.set_vec(base, bit);
      vvp_bit4_t res = calculate_result();
      vvp_vector4_t rv (1, res);
      prt.ptr()->send_vec4(rv, context);
}

वर्ग vvp_reduce_and  : सार्वजनिक vvp_reduce_base {

    सार्वजनिक:
      vvp_reduce_and();
      ~vvp_reduce_and() override;
      vvp_bit4_t calculate_result() अचर override;
};

vvp_reduce_and::vvp_reduce_and()
{
}

vvp_reduce_and::~vvp_reduce_and()
{
}

vvp_bit4_t vvp_reduce_and::calculate_result() अचर
{
      फर्काऊ bits_.reduce_and();
}

वर्ग vvp_reduce_or  : सार्वजनिक vvp_reduce_base {

    सार्वजनिक:
      vvp_reduce_or();
      ~vvp_reduce_or() override;
      vvp_bit4_t calculate_result() अचर override;
};

vvp_reduce_or::vvp_reduce_or()
{
}

vvp_reduce_or::~vvp_reduce_or()
{
}

vvp_bit4_t vvp_reduce_or::calculate_result() अचर
{
      फर्काऊ bits_.reduce_or();
}

वर्ग vvp_reduce_xor  : सार्वजनिक vvp_reduce_base {

    सार्वजनिक:
      vvp_reduce_xor();
      ~vvp_reduce_xor() override;
      vvp_bit4_t calculate_result() अचर override;
};

vvp_reduce_xor::vvp_reduce_xor()
{
}

vvp_reduce_xor::~vvp_reduce_xor()
{
}

vvp_bit4_t vvp_reduce_xor::calculate_result() अचर
{
      फर्काऊ bits_.reduce_xor();
}

वर्ग vvp_reduce_nand  : सार्वजनिक vvp_reduce_base {

    सार्वजनिक:
      vvp_reduce_nand();
      ~vvp_reduce_nand() override;
      vvp_bit4_t calculate_result() अचर override;
};

vvp_reduce_nand::vvp_reduce_nand()
{
}

vvp_reduce_nand::~vvp_reduce_nand()
{
}

vvp_bit4_t vvp_reduce_nand::calculate_result() अचर
{
      फर्काऊ ~bits_.reduce_and();
}

वर्ग vvp_reduce_nor  : सार्वजनिक vvp_reduce_base {

    सार्वजनिक:
      vvp_reduce_nor();
      ~vvp_reduce_nor() override;
      vvp_bit4_t calculate_result() अचर override;
};

vvp_reduce_nor::vvp_reduce_nor()
{
}

vvp_reduce_nor::~vvp_reduce_nor()
{
}

vvp_bit4_t vvp_reduce_nor::calculate_result() अचर
{
      फर्काऊ ~bits_.reduce_or();
}

वर्ग vvp_reduce_xnor  : सार्वजनिक vvp_reduce_base {

    सार्वजनिक:
      vvp_reduce_xnor();
      ~vvp_reduce_xnor() override;
      vvp_bit4_t calculate_result() अचर override;
};

vvp_reduce_xnor::vvp_reduce_xnor()
{
}

vvp_reduce_xnor::~vvp_reduce_xnor()
{
}

vvp_bit4_t vvp_reduce_xnor::calculate_result() अचर
{
      फर्काऊ ~bits_.reduce_xor();
}

स्थिर शून्य make_reduce(वर्ण*label, vvp_net_fun_t*red, अचर संरचना symb_s&arg)
{
      vvp_net_t*ptr = नूतन vvp_net_t;
      ptr->fun = red;

      define_functor_चिह्न(label, ptr);
      मुक्त(label);

      आगत_connect(ptr, 0, arg.text);
}

शून्य compile_reduce_and(वर्ण*label, अचर संरचना symb_s&arg)
{
      vvp_reduce_and* reduce = नूतन vvp_reduce_and;
      make_reduce(label, reduce, arg);
}

शून्य compile_reduce_or(वर्ण*label, अचर संरचना symb_s&arg)
{
      vvp_reduce_or* reduce = नूतन vvp_reduce_or;
      make_reduce(label, reduce, arg);
}

शून्य compile_reduce_xor(वर्ण*label, अचर संरचना symb_s&arg)
{
      vvp_reduce_xor* reduce = नूतन vvp_reduce_xor;
      make_reduce(label, reduce, arg);
}

शून्य compile_reduce_nand(वर्ण*label, अचर संरचना symb_s&arg)
{
      vvp_reduce_nand* reduce = नूतन vvp_reduce_nand;
      make_reduce(label, reduce, arg);
}

शून्य compile_reduce_nor(वर्ण*label, अचर संरचना symb_s&arg)
{
      vvp_reduce_nor* reduce = नूतन vvp_reduce_nor;
      make_reduce(label, reduce, arg);
}

शून्य compile_reduce_xnor(वर्ण*label, अचर संरचना symb_s&arg)
{
      vvp_reduce_xnor* reduce = नूतन vvp_reduce_xnor;
      make_reduce(label, reduce, arg);
}
