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
ଶ୍ରେଣୀ vvp_reduce_base : ସର୍ବସାଧାରଣ vvp_net_fun_t {

    ସର୍ବସାଧାରଣ:
      vvp_reduce_base();
      ଆଭାସୀ ~vvp_reduce_base() override;

      ଶୂନ୍ୟ recv_vec4(vvp_net_ptr_t prt, ଅଚଳ vvp_vector4_t&bit,
                     vvp_context_t context) override;
      ଶୂନ୍ୟ recv_vec4_pv(vvp_net_ptr_t ptr, ଅଚଳ vvp_vector4_t&bit,
			unsigned base, unsigned vwid, vvp_context_t context) override;

      ଆଭାସୀ vvp_bit4_t calculate_result() ଅଚଳ =0;

    ସୁରକ୍ଷିତ:
      vvp_vector4_t bits_;
};

vvp_reduce_base::vvp_reduce_base()
{
}

vvp_reduce_base::~vvp_reduce_base()
{
}

ଶୂନ୍ୟ vvp_reduce_base::recv_vec4(vvp_net_ptr_t prt, ଅଚଳ vvp_vector4_t&bit,
                                vvp_context_t context)
{
      bits_ = bit;
      vvp_bit4_t res =  calculate_result();
      vvp_vector4_t rv (1, res);
      prt.ptr()->send_vec4(rv, context);
}

ଶୂନ୍ୟ vvp_reduce_base::recv_vec4_pv(vvp_net_ptr_t prt, ଅଚଳ vvp_vector4_t&bit,
				   unsigned base, unsigned vwid, vvp_context_t context)
{
      ଯଦି (bits_.size() == 0) {
	    bits_ = vvp_vector4_t(vwid);
      }
      ପ୍ରତିଜ୍ଞା(bits_.size() == vwid);

      bits_.set_vec(base, bit);
      vvp_bit4_t res = calculate_result();
      vvp_vector4_t rv (1, res);
      prt.ptr()->send_vec4(rv, context);
}

ଶ୍ରେଣୀ vvp_reduce_and  : ସର୍ବସାଧାରଣ vvp_reduce_base {

    ସର୍ବସାଧାରଣ:
      vvp_reduce_and();
      ~vvp_reduce_and() override;
      vvp_bit4_t calculate_result() ଅଚଳ override;
};

vvp_reduce_and::vvp_reduce_and()
{
}

vvp_reduce_and::~vvp_reduce_and()
{
}

vvp_bit4_t vvp_reduce_and::calculate_result() ଅଚଳ
{
      ଫେରନ୍ତୁ bits_.reduce_and();
}

ଶ୍ରେଣୀ vvp_reduce_or  : ସର୍ବସାଧାରଣ vvp_reduce_base {

    ସର୍ବସାଧାରଣ:
      vvp_reduce_or();
      ~vvp_reduce_or() override;
      vvp_bit4_t calculate_result() ଅଚଳ override;
};

vvp_reduce_or::vvp_reduce_or()
{
}

vvp_reduce_or::~vvp_reduce_or()
{
}

vvp_bit4_t vvp_reduce_or::calculate_result() ଅଚଳ
{
      ଫେରନ୍ତୁ bits_.reduce_or();
}

ଶ୍ରେଣୀ vvp_reduce_xor  : ସର୍ବସାଧାରଣ vvp_reduce_base {

    ସର୍ବସାଧାରଣ:
      vvp_reduce_xor();
      ~vvp_reduce_xor() override;
      vvp_bit4_t calculate_result() ଅଚଳ override;
};

vvp_reduce_xor::vvp_reduce_xor()
{
}

vvp_reduce_xor::~vvp_reduce_xor()
{
}

vvp_bit4_t vvp_reduce_xor::calculate_result() ଅଚଳ
{
      ଫେରନ୍ତୁ bits_.reduce_xor();
}

ଶ୍ରେଣୀ vvp_reduce_nand  : ସର୍ବସାଧାରଣ vvp_reduce_base {

    ସର୍ବସାଧାରଣ:
      vvp_reduce_nand();
      ~vvp_reduce_nand() override;
      vvp_bit4_t calculate_result() ଅଚଳ override;
};

vvp_reduce_nand::vvp_reduce_nand()
{
}

vvp_reduce_nand::~vvp_reduce_nand()
{
}

vvp_bit4_t vvp_reduce_nand::calculate_result() ଅଚଳ
{
      ଫେରନ୍ତୁ ~bits_.reduce_and();
}

ଶ୍ରେଣୀ vvp_reduce_nor  : ସର୍ବସାଧାରଣ vvp_reduce_base {

    ସର୍ବସାଧାରଣ:
      vvp_reduce_nor();
      ~vvp_reduce_nor() override;
      vvp_bit4_t calculate_result() ଅଚଳ override;
};

vvp_reduce_nor::vvp_reduce_nor()
{
}

vvp_reduce_nor::~vvp_reduce_nor()
{
}

vvp_bit4_t vvp_reduce_nor::calculate_result() ଅଚଳ
{
      ଫେରନ୍ତୁ ~bits_.reduce_or();
}

ଶ୍ରେଣୀ vvp_reduce_xnor  : ସର୍ବସାଧାରଣ vvp_reduce_base {

    ସର୍ବସାଧାରଣ:
      vvp_reduce_xnor();
      ~vvp_reduce_xnor() override;
      vvp_bit4_t calculate_result() ଅଚଳ override;
};

vvp_reduce_xnor::vvp_reduce_xnor()
{
}

vvp_reduce_xnor::~vvp_reduce_xnor()
{
}

vvp_bit4_t vvp_reduce_xnor::calculate_result() ଅଚଳ
{
      ଫେରନ୍ତୁ ~bits_.reduce_xor();
}

ସ୍ଥିର ଶୂନ୍ୟ make_reduce(ଅକ୍ଷର*label, vvp_net_fun_t*red, ଅଚଳ ସଂରଚନା symb_s&arg)
{
      vvp_net_t*ptr = ନୂତନ vvp_net_t;
      ptr->fun = red;

      define_functor_ପ୍ରତୀକ(label, ptr);
      ମୁକ୍ତ(label);

      ନିବେଶ_connect(ptr, 0, arg.text);
}

ଶୂନ୍ୟ compile_reduce_and(ଅକ୍ଷର*label, ଅଚଳ ସଂରଚନା symb_s&arg)
{
      vvp_reduce_and* reduce = ନୂତନ vvp_reduce_and;
      make_reduce(label, reduce, arg);
}

ଶୂନ୍ୟ compile_reduce_or(ଅକ୍ଷର*label, ଅଚଳ ସଂରଚନା symb_s&arg)
{
      vvp_reduce_or* reduce = ନୂତନ vvp_reduce_or;
      make_reduce(label, reduce, arg);
}

ଶୂନ୍ୟ compile_reduce_xor(ଅକ୍ଷର*label, ଅଚଳ ସଂରଚନା symb_s&arg)
{
      vvp_reduce_xor* reduce = ନୂତନ vvp_reduce_xor;
      make_reduce(label, reduce, arg);
}

ଶୂନ୍ୟ compile_reduce_nand(ଅକ୍ଷର*label, ଅଚଳ ସଂରଚନା symb_s&arg)
{
      vvp_reduce_nand* reduce = ନୂତନ vvp_reduce_nand;
      make_reduce(label, reduce, arg);
}

ଶୂନ୍ୟ compile_reduce_nor(ଅକ୍ଷର*label, ଅଚଳ ସଂରଚନା symb_s&arg)
{
      vvp_reduce_nor* reduce = ନୂତନ vvp_reduce_nor;
      make_reduce(label, reduce, arg);
}

ଶୂନ୍ୟ compile_reduce_xnor(ଅକ୍ଷର*label, ଅଚଳ ସଂରଚନା symb_s&arg)
{
      vvp_reduce_xnor* reduce = ନୂତନ vvp_reduce_xnor;
      make_reduce(label, reduce, arg);
}
