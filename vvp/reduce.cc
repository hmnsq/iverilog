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
جماعت vvp_reduce_بنیاد : عوامی vvp_net_fun_t {

    عوامی:
      vvp_reduce_بنیاد();
      مجازی ~vvp_reduce_بنیاد() override;

      خالی recv_vec4(vvp_net_ptr_t prt, ثابت vvp_vector4_t&ثنائی_ہندسہ,
                     vvp_context_t context) override;
      خالی recv_vec4_pv(vvp_net_ptr_t ptr, ثابت vvp_vector4_t&ثنائی_ہندسہ,
			غیرعلامتی بنیاد, غیرعلامتی vwid, vvp_context_t context) override;

      مجازی vvp_bit4_t calculate_نتیجہ() ثابت =0;

    محفوظ:
      vvp_vector4_t بٹ_2;
};

vvp_reduce_بنیاد::vvp_reduce_بنیاد()
{
}

vvp_reduce_بنیاد::~vvp_reduce_بنیاد()
{
}

خالی vvp_reduce_بنیاد::recv_vec4(vvp_net_ptr_t prt, ثابت vvp_vector4_t&ثنائی_ہندسہ,
                                vvp_context_t context)
{
      بٹ_2 = ثنائی_ہندسہ;
      vvp_bit4_t res =  calculate_نتیجہ();
      vvp_vector4_t rv (1, res);
      prt.ptr()->send_vec4(rv, context);
}

خالی vvp_reduce_بنیاد::recv_vec4_pv(vvp_net_ptr_t prt, ثابت vvp_vector4_t&ثنائی_ہندسہ,
				   غیرعلامتی بنیاد, غیرعلامتی vwid, vvp_context_t context)
{
      اگر (بٹ_2.size() == 0) {
	    بٹ_2 = vvp_vector4_t(vwid);
      }
      أكد(بٹ_2.size() == vwid);

      بٹ_2.سیٹ_vec(بنیاد, ثنائی_ہندسہ);
      vvp_bit4_t res = calculate_نتیجہ();
      vvp_vector4_t rv (1, res);
      prt.ptr()->send_vec4(rv, context);
}

جماعت vvp_reduce_and  : عوامی vvp_reduce_بنیاد {

    عوامی:
      vvp_reduce_and();
      ~vvp_reduce_and() override;
      vvp_bit4_t calculate_نتیجہ() ثابت override;
};

vvp_reduce_and::vvp_reduce_and()
{
}

vvp_reduce_and::~vvp_reduce_and()
{
}

vvp_bit4_t vvp_reduce_and::calculate_نتیجہ() ثابت
{
      واپس بٹ_2.reduce_and();
}

جماعت vvp_reduce_or  : عوامی vvp_reduce_بنیاد {

    عوامی:
      vvp_reduce_or();
      ~vvp_reduce_or() override;
      vvp_bit4_t calculate_نتیجہ() ثابت override;
};

vvp_reduce_or::vvp_reduce_or()
{
}

vvp_reduce_or::~vvp_reduce_or()
{
}

vvp_bit4_t vvp_reduce_or::calculate_نتیجہ() ثابت
{
      واپس بٹ_2.reduce_or();
}

جماعت vvp_reduce_xor  : عوامی vvp_reduce_بنیاد {

    عوامی:
      vvp_reduce_xor();
      ~vvp_reduce_xor() override;
      vvp_bit4_t calculate_نتیجہ() ثابت override;
};

vvp_reduce_xor::vvp_reduce_xor()
{
}

vvp_reduce_xor::~vvp_reduce_xor()
{
}

vvp_bit4_t vvp_reduce_xor::calculate_نتیجہ() ثابت
{
      واپس بٹ_2.reduce_xor();
}

جماعت vvp_reduce_nand  : عوامی vvp_reduce_بنیاد {

    عوامی:
      vvp_reduce_nand();
      ~vvp_reduce_nand() override;
      vvp_bit4_t calculate_نتیجہ() ثابت override;
};

vvp_reduce_nand::vvp_reduce_nand()
{
}

vvp_reduce_nand::~vvp_reduce_nand()
{
}

vvp_bit4_t vvp_reduce_nand::calculate_نتیجہ() ثابت
{
      واپس ~بٹ_2.reduce_and();
}

جماعت vvp_reduce_nor  : عوامی vvp_reduce_بنیاد {

    عوامی:
      vvp_reduce_nor();
      ~vvp_reduce_nor() override;
      vvp_bit4_t calculate_نتیجہ() ثابت override;
};

vvp_reduce_nor::vvp_reduce_nor()
{
}

vvp_reduce_nor::~vvp_reduce_nor()
{
}

vvp_bit4_t vvp_reduce_nor::calculate_نتیجہ() ثابت
{
      واپس ~بٹ_2.reduce_or();
}

جماعت vvp_reduce_xnor  : عوامی vvp_reduce_بنیاد {

    عوامی:
      vvp_reduce_xnor();
      ~vvp_reduce_xnor() override;
      vvp_bit4_t calculate_نتیجہ() ثابت override;
};

vvp_reduce_xnor::vvp_reduce_xnor()
{
}

vvp_reduce_xnor::~vvp_reduce_xnor()
{
}

vvp_bit4_t vvp_reduce_xnor::calculate_نتیجہ() ثابت
{
      واپس ~بٹ_2.reduce_xor();
}

ساکن خالی make_reduce(حرف*label, vvp_net_fun_t*red, ثابت ساخت symb_s&arg)
{
      vvp_net_t*ptr = نیا vvp_net_t;
      ptr->fun = red;

      define_functor_علامت(label, ptr);
      آزاد(label);

      داخلہ_connect(ptr, 0, arg.متن);
}

خالی compile_reduce_and(حرف*label, ثابت ساخت symb_s&arg)
{
      vvp_reduce_and* reduce = نیا vvp_reduce_and;
      make_reduce(label, reduce, arg);
}

خالی compile_reduce_or(حرف*label, ثابت ساخت symb_s&arg)
{
      vvp_reduce_or* reduce = نیا vvp_reduce_or;
      make_reduce(label, reduce, arg);
}

خالی compile_reduce_xor(حرف*label, ثابت ساخت symb_s&arg)
{
      vvp_reduce_xor* reduce = نیا vvp_reduce_xor;
      make_reduce(label, reduce, arg);
}

خالی compile_reduce_nand(حرف*label, ثابت ساخت symb_s&arg)
{
      vvp_reduce_nand* reduce = نیا vvp_reduce_nand;
      make_reduce(label, reduce, arg);
}

خالی compile_reduce_nor(حرف*label, ثابت ساخت symb_s&arg)
{
      vvp_reduce_nor* reduce = نیا vvp_reduce_nor;
      make_reduce(label, reduce, arg);
}

خالی compile_reduce_xnor(حرف*label, ثابت ساخت symb_s&arg)
{
      vvp_reduce_xnor* reduce = نیا vvp_reduce_xnor;
      make_reduce(label, reduce, arg);
}
