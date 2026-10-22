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
श्रेणी vvp_reduce_base : खुला vvp_net_fun_t {

    खुला:
      vvp_reduce_base();
      भव ~vvp_reduce_base() override;

      व्योम recv_vec4(vvp_net_ptr_t prt, स्थिर vvp_vector4_t&bit,
                     vvp_context_t context) override;
      व्योम recv_vec4_pv(vvp_net_ptr_t ptr, स्थिर vvp_vector4_t&bit,
			अचिन्हित base, अचिन्हित vwid, vvp_context_t context) override;

      भव vvp_bit4_t calculate_परिणाम() स्थिर =0;

    रक्षित:
      vvp_vector4_t bits_;
};

vvp_reduce_base::vvp_reduce_base()
{
}

vvp_reduce_base::~vvp_reduce_base()
{
}

व्योम vvp_reduce_base::recv_vec4(vvp_net_ptr_t prt, स्थिर vvp_vector4_t&bit,
                                vvp_context_t context)
{
      bits_ = bit;
      vvp_bit4_t res =  calculate_परिणाम();
      vvp_vector4_t rv (1, res);
      prt.ptr()->send_vec4(rv, context);
}

व्योम vvp_reduce_base::recv_vec4_pv(vvp_net_ptr_t prt, स्थिर vvp_vector4_t&bit,
				   अचिन्हित base, अचिन्हित vwid, vvp_context_t context)
{
      अगर (bits_.size() == 0) {
	    bits_ = vvp_vector4_t(vwid);
      }
      निश्चित(bits_.size() == vwid);

      bits_.set_vec(base, bit);
      vvp_bit4_t res = calculate_परिणाम();
      vvp_vector4_t rv (1, res);
      prt.ptr()->send_vec4(rv, context);
}

श्रेणी vvp_reduce_and  : खुला vvp_reduce_base {

    खुला:
      vvp_reduce_and();
      ~vvp_reduce_and() override;
      vvp_bit4_t calculate_परिणाम() स्थिर override;
};

vvp_reduce_and::vvp_reduce_and()
{
}

vvp_reduce_and::~vvp_reduce_and()
{
}

vvp_bit4_t vvp_reduce_and::calculate_परिणाम() स्थिर
{
      वापस bits_.reduce_and();
}

श्रेणी vvp_reduce_or  : खुला vvp_reduce_base {

    खुला:
      vvp_reduce_or();
      ~vvp_reduce_or() override;
      vvp_bit4_t calculate_परिणाम() स्थिर override;
};

vvp_reduce_or::vvp_reduce_or()
{
}

vvp_reduce_or::~vvp_reduce_or()
{
}

vvp_bit4_t vvp_reduce_or::calculate_परिणाम() स्थिर
{
      वापस bits_.reduce_or();
}

श्रेणी vvp_reduce_xor  : खुला vvp_reduce_base {

    खुला:
      vvp_reduce_xor();
      ~vvp_reduce_xor() override;
      vvp_bit4_t calculate_परिणाम() स्थिर override;
};

vvp_reduce_xor::vvp_reduce_xor()
{
}

vvp_reduce_xor::~vvp_reduce_xor()
{
}

vvp_bit4_t vvp_reduce_xor::calculate_परिणाम() स्थिर
{
      वापस bits_.reduce_xor();
}

श्रेणी vvp_reduce_nand  : खुला vvp_reduce_base {

    खुला:
      vvp_reduce_nand();
      ~vvp_reduce_nand() override;
      vvp_bit4_t calculate_परिणाम() स्थिर override;
};

vvp_reduce_nand::vvp_reduce_nand()
{
}

vvp_reduce_nand::~vvp_reduce_nand()
{
}

vvp_bit4_t vvp_reduce_nand::calculate_परिणाम() स्थिर
{
      वापस ~bits_.reduce_and();
}

श्रेणी vvp_reduce_nor  : खुला vvp_reduce_base {

    खुला:
      vvp_reduce_nor();
      ~vvp_reduce_nor() override;
      vvp_bit4_t calculate_परिणाम() स्थिर override;
};

vvp_reduce_nor::vvp_reduce_nor()
{
}

vvp_reduce_nor::~vvp_reduce_nor()
{
}

vvp_bit4_t vvp_reduce_nor::calculate_परिणाम() स्थिर
{
      वापस ~bits_.reduce_or();
}

श्रेणी vvp_reduce_xnor  : खुला vvp_reduce_base {

    खुला:
      vvp_reduce_xnor();
      ~vvp_reduce_xnor() override;
      vvp_bit4_t calculate_परिणाम() स्थिर override;
};

vvp_reduce_xnor::vvp_reduce_xnor()
{
}

vvp_reduce_xnor::~vvp_reduce_xnor()
{
}

vvp_bit4_t vvp_reduce_xnor::calculate_परिणाम() स्थिर
{
      वापस ~bits_.reduce_xor();
}

अटल व्योम make_reduce(अक्षर*label, vvp_net_fun_t*red, स्थिर काष्ठा symb_s&arg)
{
      vvp_net_t*ptr = नया vvp_net_t;
      ptr->fun = red;

      define_functor_प्रतीक(label, ptr);
      मुक्त(label);

      निवेश_connect(ptr, 0, arg.text);
}

व्योम compile_reduce_and(अक्षर*label, स्थिर काष्ठा symb_s&arg)
{
      vvp_reduce_and* reduce = नया vvp_reduce_and;
      make_reduce(label, reduce, arg);
}

व्योम compile_reduce_or(अक्षर*label, स्थिर काष्ठा symb_s&arg)
{
      vvp_reduce_or* reduce = नया vvp_reduce_or;
      make_reduce(label, reduce, arg);
}

व्योम compile_reduce_xor(अक्षर*label, स्थिर काष्ठा symb_s&arg)
{
      vvp_reduce_xor* reduce = नया vvp_reduce_xor;
      make_reduce(label, reduce, arg);
}

व्योम compile_reduce_nand(अक्षर*label, स्थिर काष्ठा symb_s&arg)
{
      vvp_reduce_nand* reduce = नया vvp_reduce_nand;
      make_reduce(label, reduce, arg);
}

व्योम compile_reduce_nor(अक्षर*label, स्थिर काष्ठा symb_s&arg)
{
      vvp_reduce_nor* reduce = नया vvp_reduce_nor;
      make_reduce(label, reduce, arg);
}

व्योम compile_reduce_xnor(अक्षर*label, स्थिर काष्ठा symb_s&arg)
{
      vvp_reduce_xnor* reduce = नया vvp_reduce_xnor;
      make_reduce(label, reduce, arg);
}
