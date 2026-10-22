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

استعمال نطاق std;

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


resolv_core::resolv_core(بنانشان nports, vvp_net_t*net)
: nports_(nports), net_(net)
{
      ڳڻپ_functors_resolv += 1;
}

resolv_core::~resolv_core()
{
}

فراغ resolv_core::recv_vec4_pv_(بنانشان پورٽ, ثابت vvp_vector4_t&ٻٽو_انگ,
				بنانشان بنياد, بنانشان vwid)
{
      بنانشان wid = ٻٽو_انگ.size();
      vvp_vector4_t res (vwid);

      لكل (بنانشان idx = 0 ;  idx < بنياد ;  idx += 1)
	    res.سيٽ_ٻٽو_انگ(idx, BIT4_Z);

      لكل (بنانشان idx = 0 ;  idx < wid && idx+بنياد < vwid;  idx += 1)
	    res.سيٽ_ٻٽو_انگ(idx+بنياد, ٻٽو_انگ.value(idx));

      لكل (بنانشان idx = بنياد+wid ;  idx < vwid ;  idx += 1)
	    res.سيٽ_ٻٽو_انگ(idx, BIT4_Z);

      recv_vec4_(پورٽ, res);
}

فراغ resolv_core::recv_vec8_pv_(بنانشان پورٽ, ثابت vvp_vector8_t&ٻٽو_انگ,
				بنانشان بنياد, بنانشان vwid)
{
      بنانشان wid = ٻٽو_انگ.size();
      vvp_vector8_t res (vwid);

      لكل (بنانشان idx = 0 ;  idx < بنياد ;  idx += 1)
	    res.سيٽ_ٻٽو_انگ(idx, vvp_scalar_t());

      لكل (بنانشان idx = 0 ;  idx < wid && idx+بنياد < vwid;  idx += 1)
	    res.سيٽ_ٻٽو_انگ(idx+بنياد, ٻٽو_انگ.value(idx));

      لكل (بنانشان idx = بنياد+wid ;  idx < vwid ;  idx += 1)
	    res.سيٽ_ٻٽو_انگ(idx, vvp_scalar_t());

      recv_vec8_(پورٽ, res);
}


resolv_extend::resolv_extend(resolv_core*core, بنانشان پورٽ_بنياد)
: core_(core), پورٽ_بنياد_2(پورٽ_بنياد)
{
}

resolv_extend::~resolv_extend()
{
}


resolv_tri::resolv_tri(بنانشان nports, vvp_net_t*net, vvp_scalar_t hiz_قدر)
: resolv_core(nports, net), hiz_قدر_2(hiz_قدر)
{
        // count the input (leaf) nodes
      بنانشان nnodes = nports;

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
      ميٽيو[] val_;
}

فراغ resolv_tri::recv_vec4_(بنانشان پورٽ, ثابت vvp_vector4_t&ٻٽو_انگ)
{
      recv_vec8_(پورٽ, vvp_vector8_t(ٻٽو_انگ, 6,6 /* STRONG */));
}

فراغ resolv_tri::recv_vec8_(بنانشان پورٽ, ثابت vvp_vector8_t&ٻٽو_انگ)
{
      أكد(پورٽ < nports_);

      إذا (val_[پورٽ].eeq(ٻٽو_انگ))
	    أرجع;

      val_[پورٽ] = ٻٽو_انگ;

        // Starting at the leaf level, work down the tree, resolving
        // the changed values. base is the first node in the current
        // level and span is the number of nodes at that level. ip
        // is the first node in the group of four nodes at that level
        // that include the node that has changed, and op is the node
        // at the next level that stores the resolved value from that
        // group.
      بنانشان بنياد = 0;
      بنانشان وسعت = nports_;
      طالما (وسعت > 1) {
            بنانشان ايندڙ_بنياد = بنياد + وسعت;
            بنانشان ip = بنياد + (پورٽ & ~0x3);
            بنانشان op = ايندڙ_بنياد + (پورٽ / 4);
            بنانشان ll = min(ip + 4, ايندڙ_بنياد);

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

            بنياد = ايندڙ_بنياد;
            وسعت = (وسعت + 3) / 4;
            پورٽ = پورٽ / 4;
      }

      إذا (! hiz_قدر_2.is_hiz()) {
	    لكل (بنانشان idx = 0 ;  idx < val_[بنياد].size() ;  idx += 1) {
		  val_[بنياد].سيٽ_ٻٽو_انگ(idx, resolve(val_[بنياد].value(idx),
						  hiz_قدر_2));
	    }
      }

      net_->send_vec8(val_[بنياد]);
}

فراغ resolv_tri::ڳڻپ_drivers(بنانشان ٻٽو_انگ_idx, بنانشان counts[3])
{
      لكل (بنانشان idx = 0 ; idx < nports_ ; idx += 1) {
	    إذا (val_[idx].size() == 0)
	          استمر;

            update_driver_counts(val_[idx].value(ٻٽو_انگ_idx).value(), counts);
      }
}


resolv_wired_logic::resolv_wired_logic(بنانشان nports, vvp_net_t*net)
: resolv_core(nports, net)
{
        // count the input (leaf) nodes
      بنانشان nnodes = nports;

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
      ميٽيو[] val_;
}

فراغ resolv_wired_logic::recv_vec4_(بنانشان پورٽ, ثابت vvp_vector4_t&ٻٽو_انگ)
{
      أكد(پورٽ < nports_);

      إذا (val_[پورٽ].eeq(ٻٽو_انگ))
	    أرجع;

      val_[پورٽ] = ٻٽو_انگ;

        // Starting at the leaf level, work down the tree, resolving
        // the changed values. base is the first node in the current
        // level and span is the number of nodes at that level. ip
        // is the first node in the group of four nodes at that level
        // that include the node that has changed, and op is the node
        // at the next level that stores the resolved value from that
        // group.
      بنانشان بنياد = 0;
      بنانشان وسعت = nports_;
      طالما (وسعت > 1) {
            بنانشان ايندڙ_بنياد = بنياد + وسعت;
            بنانشان ip = بنياد + (پورٽ & ~0x3);
            بنانشان op = ايندڙ_بنياد + (پورٽ / 4);
            بنانشان ll = min(ip + 4, ايندڙ_بنياد);

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

            بنياد = ايندڙ_بنياد;
            وسعت = (وسعت + 3) / 4;
            پورٽ = پورٽ / 4;
      }

      net_->send_vec4(val_[بنياد], 0);
}

فراغ resolv_wired_logic::recv_vec8_(بنانشان پورٽ, ثابت vvp_vector8_t&ٻٽو_انگ)
{
      recv_vec4_(پورٽ, reduce4(ٻٽو_انگ));
}

فراغ resolv_wired_logic::ڳڻپ_drivers(بنانشان ٻٽو_انگ_idx, بنانشان counts[3])
{
      لكل (بنانشان idx = 0 ; idx < nports_ ; idx += 1) {
	    إذا (val_[idx].size() == 0)
	          استمر;

            update_driver_counts(val_[idx].value(ٻٽو_انگ_idx), counts);
      }
}


resolv_triand::resolv_triand(بنانشان nports, vvp_net_t*net)
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

      لكل (بنانشان idx = 0 ; idx < out.size() ; idx += 1) {
	    vvp_bit4_t abit = a.value(idx);
	    vvp_bit4_t bbit = b.value(idx);
	    إذا (abit == BIT4_Z) {
		  out.سيٽ_ٻٽو_انگ(idx, bbit);
	    } وإلا إذا (bbit == BIT4_Z) {
		  out.سيٽ_ٻٽو_انگ(idx, abit);
	    } وإلا إذا (abit == BIT4_0 || bbit == BIT4_0) {
		  out.سيٽ_ٻٽو_انگ(idx, BIT4_0);
	    } وإلا إذا (abit == BIT4_X || bbit == BIT4_X) {
		  out.سيٽ_ٻٽو_انگ(idx, BIT4_X);
	    } وإلا {
		  out.سيٽ_ٻٽو_انگ(idx, BIT4_1);
	    }
      }

      أرجع out;
}


resolv_trior::resolv_trior(بنانشان nports, vvp_net_t*net)
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

      لكل (بنانشان idx = 0 ; idx < out.size() ; idx += 1) {
	    vvp_bit4_t abit = a.value(idx);
	    vvp_bit4_t bbit = b.value(idx);
	    إذا (abit == BIT4_Z) {
		  out.سيٽ_ٻٽو_انگ(idx, bbit);
	    } وإلا إذا (bbit == BIT4_Z) {
		  out.سيٽ_ٻٽو_انگ(idx, abit);
	    } وإلا إذا (abit == BIT4_1 || bbit == BIT4_1) {
		  out.سيٽ_ٻٽو_انگ(idx, BIT4_1);
	    } وإلا إذا (abit == BIT4_X || bbit == BIT4_X) {
		  out.سيٽ_ٻٽو_انگ(idx, BIT4_X);
	    } وإلا {
		  out.سيٽ_ٻٽو_انگ(idx, BIT4_0);
	    }
      }

      أرجع out;
}
