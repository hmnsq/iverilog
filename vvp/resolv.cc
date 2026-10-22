/*
 * Copyright (c) 2001-2021 Stephen Williams (steve@icarus.com)
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

# include  "resolv.h"
# include  "schedule.h"
# include  "compile.h"
# include  "statistics.h"
# include  <iostream>
# include  <algorithm>
# include  <cassert>

کارول نطاق std;

/*
 * The core functor for a resolver node stores all the input values
 * received by that node. This provides the necessary information
 * for implementing the $countdrivers system call. For efficiency,
 * the resolver is implemented using a balanced quaternary tree, so
 * the core functor also stores the current value for each branch
 * of the tree, to eliminate the need to re-evaluate branches whose
 * inputs haven't changed. The tree values are flattened into a linear
 * array, with the input values stored at the start of the array, then
 * the next level of branch values, and so on, down to the final array
 * array element which stores the current output value.
 */


resolv_core::resolv_core(بینښه nports, vvp_net_t*net)
: nports_(nports), net_(net)
{
      شمېر_functors_resolv += 1;
}

resolv_core::~resolv_core()
{
}

فراغ resolv_core::recv_vec4_pv_(بینښه پورټ, ثابت vvp_vector4_t&بټ,
				بینښه بنسټ, بینښه vwid)
{
      بینښه wid = بټ.size();
      vvp_vector4_t res (vwid);

      لكل (بینښه idx = 0 ;  idx < بنسټ ;  idx += 1)
	    res.سېټ_بټ(idx, BIT4_Z);

      لكل (بینښه idx = 0 ;  idx < wid && idx+بنسټ < vwid;  idx += 1)
	    res.سېټ_بټ(idx+بنسټ, بټ.value(idx));

      لكل (بینښه idx = بنسټ+wid ;  idx < vwid ;  idx += 1)
	    res.سېټ_بټ(idx, BIT4_Z);

      recv_vec4_(پورټ, res);
}

فراغ resolv_core::recv_vec8_pv_(بینښه پورټ, ثابت vvp_vector8_t&بټ,
				بینښه بنسټ, بینښه vwid)
{
      بینښه wid = بټ.size();
      vvp_vector8_t res (vwid);

      لكل (بینښه idx = 0 ;  idx < بنسټ ;  idx += 1)
	    res.سېټ_بټ(idx, vvp_scalar_t());

      لكل (بینښه idx = 0 ;  idx < wid && idx+بنسټ < vwid;  idx += 1)
	    res.سېټ_بټ(idx+بنسټ, بټ.value(idx));

      لكل (بینښه idx = بنسټ+wid ;  idx < vwid ;  idx += 1)
	    res.سېټ_بټ(idx, vvp_scalar_t());

      recv_vec8_(پورټ, res);
}


resolv_extend::resolv_extend(resolv_core*core, بینښه پورټ_بنسټ)
: core_(core), پورټ_بنسټ_2(پورټ_بنسټ)
{
}

resolv_extend::~resolv_extend()
{
}


resolv_tri::resolv_tri(بینښه nports, vvp_net_t*net, vvp_scalar_t hiz_ارزښت)
: resolv_core(nports, net), hiz_ارزښت_2(hiz_ارزښت)
{
        // count the input (leaf) nodes
      بینښه nnodes = nports;

        // add in the intermediate branch nodes
      طالما (nports > 4) {
            nports = (nports + 3) / 4;
            nnodes += nports;
      }
        // add one more node for storing the output value
        // (not needed if there is only one input)
      إذا (nnodes > 1)
            nnodes += 1;

      val_ = جديد vvp_vector8_t [nnodes];
}

resolv_tri::~resolv_tri()
{
      ړنګول[] val_;
}

فراغ resolv_tri::recv_vec4_(بینښه پورټ, ثابت vvp_vector4_t&بټ)
{
      recv_vec8_(پورټ, vvp_vector8_t(بټ, 6,6 /* STRONG */));
}

فراغ resolv_tri::recv_vec8_(بینښه پورټ, ثابت vvp_vector8_t&بټ)
{
      أكد(پورټ < nports_);

      إذا (val_[پورټ].eeq(بټ))
	    أرجع;

      val_[پورټ] = بټ;

        // Starting at the leaf level, work down the tree, resolving
        // the changed values. base is the first node in the current
        // level and span is the number of nodes at that level. ip
        // is the first node in the group of four nodes at that level
        // that include the node that has changed, and op is the node
        // at the next level that stores the resolved value from that
        // group.
      بینښه بنسټ = 0;
      بینښه پراختیا = nports_;
      طالما (پراختیا > 1) {
            بینښه بل_بنسټ = بنسټ + پراختیا;
            بینښه ip = بنسټ + (پورټ & ~0x3);
            بینښه op = بل_بنسټ + (پورټ / 4);
            بینښه ll = min(ip + 4, بل_بنسټ);

            vvp_vector8_t out = val_[ip];
            لكل (ip = ip + 1; ip < ll; ip += 1) {
                  إذا (val_[ip].size() == 0)
                        استمر;
                  إذا (out.size() == 0)
                        out = val_[ip];
                  وإلا
                        out = resolve(out, val_[ip]);
            }
            إذا (val_[op].eeq(out))
                  أرجع;
            val_[op] = out;

            بنسټ = بل_بنسټ;
            پراختیا = (پراختیا + 3) / 4;
            پورټ = پورټ / 4;
      }

      إذا (! hiz_ارزښت_2.is_hiz()) {
	    لكل (بینښه idx = 0 ;  idx < val_[بنسټ].size() ;  idx += 1) {
		  val_[بنسټ].سېټ_بټ(idx, resolve(val_[بنسټ].value(idx),
						  hiz_ارزښت_2));
	    }
      }

      net_->send_vec8(val_[بنسټ]);
}

فراغ resolv_tri::شمېر_drivers(بینښه بټ_idx, بینښه counts[3])
{
      لكل (بینښه idx = 0 ; idx < nports_ ; idx += 1) {
	    إذا (val_[idx].size() == 0)
	          استمر;

            update_driver_counts(val_[idx].value(بټ_idx).value(), counts);
      }
}


resolv_wired_logic::resolv_wired_logic(بینښه nports, vvp_net_t*net)
: resolv_core(nports, net)
{
        // count the input (leaf) nodes
      بینښه nnodes = nports;

        // add in the intermediate branch nodes
      طالما (nports > 4) {
            nports = (nports + 3) / 4;
            nnodes += nports;
      }
        // add one more node for storing the output value
        // (not needed if there is only one input)
      إذا (nnodes > 1)
            nnodes += 1;

      val_ = جديد vvp_vector4_t [nnodes];
}

resolv_wired_logic::~resolv_wired_logic()
{
      ړنګول[] val_;
}

فراغ resolv_wired_logic::recv_vec4_(بینښه پورټ, ثابت vvp_vector4_t&بټ)
{
      أكد(پورټ < nports_);

      إذا (val_[پورټ].eeq(بټ))
	    أرجع;

      val_[پورټ] = بټ;

        // Starting at the leaf level, work down the tree, resolving
        // the changed values. base is the first node in the current
        // level and span is the number of nodes at that level. ip
        // is the first node in the group of four nodes at that level
        // that include the node that has changed, and op is the node
        // at the next level that stores the resolved value from that
        // group.
      بینښه بنسټ = 0;
      بینښه پراختیا = nports_;
      طالما (پراختیا > 1) {
            بینښه بل_بنسټ = بنسټ + پراختیا;
            بینښه ip = بنسټ + (پورټ & ~0x3);
            بینښه op = بل_بنسټ + (پورټ / 4);
            بینښه ll = min(ip + 4, بل_بنسټ);

            vvp_vector4_t out = val_[ip];
            لكل (ip = ip + 1; ip < ll; ip += 1) {
                  إذا (val_[ip].size() == 0)
                        استمر;
                  إذا (out.size() == 0)
                        out = val_[ip];
                  وإلا
                        out = wired_logic_math_(out, val_[ip]);
            }
            إذا (val_[op].eeq(out))
                  أرجع;
            val_[op] = out;

            بنسټ = بل_بنسټ;
            پراختیا = (پراختیا + 3) / 4;
            پورټ = پورټ / 4;
      }

      net_->send_vec4(val_[بنسټ], 0);
}

فراغ resolv_wired_logic::recv_vec8_(بینښه پورټ, ثابت vvp_vector8_t&بټ)
{
      recv_vec4_(پورټ, reduce4(بټ));
}

فراغ resolv_wired_logic::شمېر_drivers(بینښه بټ_idx, بینښه counts[3])
{
      لكل (بینښه idx = 0 ; idx < nports_ ; idx += 1) {
	    إذا (val_[idx].size() == 0)
	          استمر;

            update_driver_counts(val_[idx].value(بټ_idx), counts);
      }
}


resolv_triand::resolv_triand(بینښه nports, vvp_net_t*net)
: resolv_wired_logic(nports, net)
{
}

resolv_triand::~resolv_triand()
{
}

vvp_vector4_t resolv_triand::wired_logic_math_(vvp_vector4_t&a, vvp_vector4_t&b)
{
      أكد(a.size() == b.size());

      vvp_vector4_t out (a.size());

      لكل (بینښه idx = 0 ; idx < out.size() ; idx += 1) {
	    vvp_bit4_t abit = a.value(idx);
	    vvp_bit4_t bbit = b.value(idx);
	    إذا (abit == BIT4_Z) {
		  out.سېټ_بټ(idx, bbit);
	    } وإلا إذا (bbit == BIT4_Z) {
		  out.سېټ_بټ(idx, abit);
	    } وإلا إذا (abit == BIT4_0 || bbit == BIT4_0) {
		  out.سېټ_بټ(idx, BIT4_0);
	    } وإلا إذا (abit == BIT4_X || bbit == BIT4_X) {
		  out.سېټ_بټ(idx, BIT4_X);
	    } وإلا {
		  out.سېټ_بټ(idx, BIT4_1);
	    }
      }

      أرجع out;
}


resolv_trior::resolv_trior(بینښه nports, vvp_net_t*net)
: resolv_wired_logic(nports, net)
{
}

resolv_trior::~resolv_trior()
{
}

vvp_vector4_t resolv_trior::wired_logic_math_(vvp_vector4_t&a, vvp_vector4_t&b)
{
      أكد(a.size() == b.size());

      vvp_vector4_t out (a.size());

      لكل (بینښه idx = 0 ; idx < out.size() ; idx += 1) {
	    vvp_bit4_t abit = a.value(idx);
	    vvp_bit4_t bbit = b.value(idx);
	    إذا (abit == BIT4_Z) {
		  out.سېټ_بټ(idx, bbit);
	    } وإلا إذا (bbit == BIT4_Z) {
		  out.سېټ_بټ(idx, abit);
	    } وإلا إذا (abit == BIT4_1 || bbit == BIT4_1) {
		  out.سېټ_بټ(idx, BIT4_1);
	    } وإلا إذا (abit == BIT4_X || bbit == BIT4_X) {
		  out.سېټ_بټ(idx, BIT4_X);
	    } وإلا {
		  out.سېټ_بټ(idx, BIT4_0);
	    }
      }

      أرجع out;
}
