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
తరగతి vvp_reduce_పునాది : బహిరంగ vvp_net_fun_t {

    బహిరంగ:
      vvp_reduce_పునాది();
      మాయా ~vvp_reduce_పునాది() override;

      శూన్య recv_vec4(vvp_net_ptr_t prt, స్థిరరాశి vvp_vector4_t&బిట్టు,
                     vvp_context_t context) override;
      శూన్య recv_vec4_pv(vvp_net_ptr_t ptr, స్థిరరాశి vvp_vector4_t&బిట్టు,
			గుర్తులేని పునాది, గుర్తులేని vwid, vvp_context_t context) override;

      మాయా vvp_bit4_t calculate_ఫలితం() స్థిరరాశి =0;

    రక్షిత:
      vvp_vector4_t బిట్లు_2;
};

vvp_reduce_పునాది::vvp_reduce_పునాది()
{
}

vvp_reduce_పునాది::~vvp_reduce_పునాది()
{
}

శూన్య vvp_reduce_పునాది::recv_vec4(vvp_net_ptr_t prt, స్థిరరాశి vvp_vector4_t&బిట్టు,
                                vvp_context_t context)
{
      బిట్లు_2 = బిట్టు;
      vvp_bit4_t res =  calculate_ఫలితం();
      vvp_vector4_t rv (1, res);
      prt.ptr()->send_vec4(rv, context);
}

శూన్య vvp_reduce_పునాది::recv_vec4_pv(vvp_net_ptr_t prt, స్థిరరాశి vvp_vector4_t&బిట్టు,
				   గుర్తులేని పునాది, గుర్తులేని vwid, vvp_context_t context)
{
      ఒకవేళ (బిట్లు_2.size() == 0) {
	    బిట్లు_2 = vvp_vector4_t(vwid);
      }
      ప్రతిజ్ఞా(బిట్లు_2.size() == vwid);

      బిట్లు_2.సమితి_vec(పునాది, బిట్టు);
      vvp_bit4_t res = calculate_ఫలితం();
      vvp_vector4_t rv (1, res);
      prt.ptr()->send_vec4(rv, context);
}

తరగతి vvp_reduce_and  : బహిరంగ vvp_reduce_పునాది {

    బహిరంగ:
      vvp_reduce_and();
      ~vvp_reduce_and() override;
      vvp_bit4_t calculate_ఫలితం() స్థిరరాశి override;
};

vvp_reduce_and::vvp_reduce_and()
{
}

vvp_reduce_and::~vvp_reduce_and()
{
}

vvp_bit4_t vvp_reduce_and::calculate_ఫలితం() స్థిరరాశి
{
      తిరిగి బిట్లు_2.reduce_and();
}

తరగతి vvp_reduce_or  : బహిరంగ vvp_reduce_పునాది {

    బహిరంగ:
      vvp_reduce_or();
      ~vvp_reduce_or() override;
      vvp_bit4_t calculate_ఫలితం() స్థిరరాశి override;
};

vvp_reduce_or::vvp_reduce_or()
{
}

vvp_reduce_or::~vvp_reduce_or()
{
}

vvp_bit4_t vvp_reduce_or::calculate_ఫలితం() స్థిరరాశి
{
      తిరిగి బిట్లు_2.reduce_or();
}

తరగతి vvp_reduce_xor  : బహిరంగ vvp_reduce_పునాది {

    బహిరంగ:
      vvp_reduce_xor();
      ~vvp_reduce_xor() override;
      vvp_bit4_t calculate_ఫలితం() స్థిరరాశి override;
};

vvp_reduce_xor::vvp_reduce_xor()
{
}

vvp_reduce_xor::~vvp_reduce_xor()
{
}

vvp_bit4_t vvp_reduce_xor::calculate_ఫలితం() స్థిరరాశి
{
      తిరిగి బిట్లు_2.reduce_xor();
}

తరగతి vvp_reduce_nand  : బహిరంగ vvp_reduce_పునాది {

    బహిరంగ:
      vvp_reduce_nand();
      ~vvp_reduce_nand() override;
      vvp_bit4_t calculate_ఫలితం() స్థిరరాశి override;
};

vvp_reduce_nand::vvp_reduce_nand()
{
}

vvp_reduce_nand::~vvp_reduce_nand()
{
}

vvp_bit4_t vvp_reduce_nand::calculate_ఫలితం() స్థిరరాశి
{
      తిరిగి ~బిట్లు_2.reduce_and();
}

తరగతి vvp_reduce_nor  : బహిరంగ vvp_reduce_పునాది {

    బహిరంగ:
      vvp_reduce_nor();
      ~vvp_reduce_nor() override;
      vvp_bit4_t calculate_ఫలితం() స్థిరరాశి override;
};

vvp_reduce_nor::vvp_reduce_nor()
{
}

vvp_reduce_nor::~vvp_reduce_nor()
{
}

vvp_bit4_t vvp_reduce_nor::calculate_ఫలితం() స్థిరరాశి
{
      తిరిగి ~బిట్లు_2.reduce_or();
}

తరగతి vvp_reduce_xnor  : బహిరంగ vvp_reduce_పునాది {

    బహిరంగ:
      vvp_reduce_xnor();
      ~vvp_reduce_xnor() override;
      vvp_bit4_t calculate_ఫలితం() స్థిరరాశి override;
};

vvp_reduce_xnor::vvp_reduce_xnor()
{
}

vvp_reduce_xnor::~vvp_reduce_xnor()
{
}

vvp_bit4_t vvp_reduce_xnor::calculate_ఫలితం() స్థిరరాశి
{
      తిరిగి ~బిట్లు_2.reduce_xor();
}

స్థిర శూన్య make_reduce(అక్షరం*label, vvp_net_fun_t*red, స్థిరరాశి నిర్మాణం symb_s&arg)
{
      vvp_net_t*ptr = కొత్త vvp_net_t;
      ptr->fun = red;

      define_functor_చిహ్నం(label, ptr);
      విడుదల(label);

      ఆదానం_connect(ptr, 0, arg.పాఠ్యం);
}

శూన్య compile_reduce_and(అక్షరం*label, స్థిరరాశి నిర్మాణం symb_s&arg)
{
      vvp_reduce_and* reduce = కొత్త vvp_reduce_and;
      make_reduce(label, reduce, arg);
}

శూన్య compile_reduce_or(అక్షరం*label, స్థిరరాశి నిర్మాణం symb_s&arg)
{
      vvp_reduce_or* reduce = కొత్త vvp_reduce_or;
      make_reduce(label, reduce, arg);
}

శూన్య compile_reduce_xor(అక్షరం*label, స్థిరరాశి నిర్మాణం symb_s&arg)
{
      vvp_reduce_xor* reduce = కొత్త vvp_reduce_xor;
      make_reduce(label, reduce, arg);
}

శూన్య compile_reduce_nand(అక్షరం*label, స్థిరరాశి నిర్మాణం symb_s&arg)
{
      vvp_reduce_nand* reduce = కొత్త vvp_reduce_nand;
      make_reduce(label, reduce, arg);
}

శూన్య compile_reduce_nor(అక్షరం*label, స్థిరరాశి నిర్మాణం symb_s&arg)
{
      vvp_reduce_nor* reduce = కొత్త vvp_reduce_nor;
      make_reduce(label, reduce, arg);
}

శూన్య compile_reduce_xnor(అక్షరం*label, స్థిరరాశి నిర్మాణం symb_s&arg)
{
      vvp_reduce_xnor* reduce = కొత్త vvp_reduce_xnor;
      make_reduce(label, reduce, arg);
}
