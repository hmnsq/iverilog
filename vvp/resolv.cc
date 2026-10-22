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

استفاده نطاق std;

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


resolv_core::resolv_core(بیعلامت nports, vvp_net_t*net)
: nports_(nports), net_(net)
{
      شمار_functors_resolv += 1;
}

resolv_core::~resolv_core()
{
}

فراغ resolv_core::recv_vec4_pv_(بیعلامت پورت, ثابت vvp_vector4_t&بیت,
				بیعلامت پایه, بیعلامت vwid)
{
      بیعلامت wid = بیت.size();
      vvp_vector4_t res (vwid);

      لكل (بیعلامت idx = 0 ;  idx < پایه ;  idx += 1)
	    res.مجموعه_بیت(idx, BIT4_Z);

      لكل (بیعلامت idx = 0 ;  idx < wid && idx+پایه < vwid;  idx += 1)
	    res.مجموعه_بیت(idx+پایه, بیت.value(idx));

      لكل (بیعلامت idx = پایه+wid ;  idx < vwid ;  idx += 1)
	    res.مجموعه_بیت(idx, BIT4_Z);

      recv_vec4_(پورت, res);
}

فراغ resolv_core::recv_vec8_pv_(بیعلامت پورت, ثابت vvp_vector8_t&بیت,
				بیعلامت پایه, بیعلامت vwid)
{
      بیعلامت wid = بیت.size();
      vvp_vector8_t res (vwid);

      لكل (بیعلامت idx = 0 ;  idx < پایه ;  idx += 1)
	    res.مجموعه_بیت(idx, vvp_scalar_t());

      لكل (بیعلامت idx = 0 ;  idx < wid && idx+پایه < vwid;  idx += 1)
	    res.مجموعه_بیت(idx+پایه, بیت.value(idx));

      لكل (بیعلامت idx = پایه+wid ;  idx < vwid ;  idx += 1)
	    res.مجموعه_بیت(idx, vvp_scalar_t());

      recv_vec8_(پورت, res);
}


resolv_extend::resolv_extend(resolv_core*core, بیعلامت پورت_پایه)
: core_(core), پورت_پایه_2(پورت_پایه)
{
}

resolv_extend::~resolv_extend()
{
}


resolv_tri::resolv_tri(بیعلامت nports, vvp_net_t*net, vvp_scalar_t hiz_مقدار)
: resolv_core(nports, net), hiz_مقدار_2(hiz_مقدار)
{
        // count the input (leaf) nodes
      بیعلامت nnodes = nports;

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
      حذف[] val_;
}

فراغ resolv_tri::recv_vec4_(بیعلامت پورت, ثابت vvp_vector4_t&بیت)
{
      recv_vec8_(پورت, vvp_vector8_t(بیت, 6,6 /* STRONG */));
}

فراغ resolv_tri::recv_vec8_(بیعلامت پورت, ثابت vvp_vector8_t&بیت)
{
      أكد(پورت < nports_);

      إذا (val_[پورت].eeq(بیت))
	    أرجع;

      val_[پورت] = بیت;

        // Starting at the leaf level, work down the tree, resolving
        // the changed values. base is the first node in the current
        // level and span is the number of nodes at that level. ip
        // is the first node in the group of four nodes at that level
        // that include the node that has changed, and op is the node
        // at the next level that stores the resolved value from that
        // group.
      بیعلامت پایه = 0;
      بیعلامت بازه = nports_;
      طالما (بازه > 1) {
            بیعلامت بعدی_پایه = پایه + بازه;
            بیعلامت ip = پایه + (پورت & ~0x3);
            بیعلامت op = بعدی_پایه + (پورت / 4);
            بیعلامت ll = min(ip + 4, بعدی_پایه);

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

            پایه = بعدی_پایه;
            بازه = (بازه + 3) / 4;
            پورت = پورت / 4;
      }

      إذا (! hiz_مقدار_2.is_hiz()) {
	    لكل (بیعلامت idx = 0 ;  idx < val_[پایه].size() ;  idx += 1) {
		  val_[پایه].مجموعه_بیت(idx, resolve(val_[پایه].value(idx),
						  hiz_مقدار_2));
	    }
      }

      net_->send_vec8(val_[پایه]);
}

فراغ resolv_tri::شمار_drivers(بیعلامت بیت_idx, بیعلامت counts[3])
{
      لكل (بیعلامت idx = 0 ; idx < nports_ ; idx += 1) {
	    إذا (val_[idx].size() == 0)
	          استمر;

            update_driver_counts(val_[idx].value(بیت_idx).value(), counts);
      }
}


resolv_wired_logic::resolv_wired_logic(بیعلامت nports, vvp_net_t*net)
: resolv_core(nports, net)
{
        // count the input (leaf) nodes
      بیعلامت nnodes = nports;

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
      حذف[] val_;
}

فراغ resolv_wired_logic::recv_vec4_(بیعلامت پورت, ثابت vvp_vector4_t&بیت)
{
      أكد(پورت < nports_);

      إذا (val_[پورت].eeq(بیت))
	    أرجع;

      val_[پورت] = بیت;

        // Starting at the leaf level, work down the tree, resolving
        // the changed values. base is the first node in the current
        // level and span is the number of nodes at that level. ip
        // is the first node in the group of four nodes at that level
        // that include the node that has changed, and op is the node
        // at the next level that stores the resolved value from that
        // group.
      بیعلامت پایه = 0;
      بیعلامت بازه = nports_;
      طالما (بازه > 1) {
            بیعلامت بعدی_پایه = پایه + بازه;
            بیعلامت ip = پایه + (پورت & ~0x3);
            بیعلامت op = بعدی_پایه + (پورت / 4);
            بیعلامت ll = min(ip + 4, بعدی_پایه);

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

            پایه = بعدی_پایه;
            بازه = (بازه + 3) / 4;
            پورت = پورت / 4;
      }

      net_->send_vec4(val_[پایه], 0);
}

فراغ resolv_wired_logic::recv_vec8_(بیعلامت پورت, ثابت vvp_vector8_t&بیت)
{
      recv_vec4_(پورت, reduce4(بیت));
}

فراغ resolv_wired_logic::شمار_drivers(بیعلامت بیت_idx, بیعلامت counts[3])
{
      لكل (بیعلامت idx = 0 ; idx < nports_ ; idx += 1) {
	    إذا (val_[idx].size() == 0)
	          استمر;

            update_driver_counts(val_[idx].value(بیت_idx), counts);
      }
}


resolv_triand::resolv_triand(بیعلامت nports, vvp_net_t*net)
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

      لكل (بیعلامت idx = 0 ; idx < out.size() ; idx += 1) {
	    vvp_bit4_t abit = a.value(idx);
	    vvp_bit4_t bbit = b.value(idx);
	    إذا (abit == BIT4_Z) {
		  out.مجموعه_بیت(idx, bbit);
	    } وإلا إذا (bbit == BIT4_Z) {
		  out.مجموعه_بیت(idx, abit);
	    } وإلا إذا (abit == BIT4_0 || bbit == BIT4_0) {
		  out.مجموعه_بیت(idx, BIT4_0);
	    } وإلا إذا (abit == BIT4_X || bbit == BIT4_X) {
		  out.مجموعه_بیت(idx, BIT4_X);
	    } وإلا {
		  out.مجموعه_بیت(idx, BIT4_1);
	    }
      }

      أرجع out;
}


resolv_trior::resolv_trior(بیعلامت nports, vvp_net_t*net)
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

      لكل (بیعلامت idx = 0 ; idx < out.size() ; idx += 1) {
	    vvp_bit4_t abit = a.value(idx);
	    vvp_bit4_t bbit = b.value(idx);
	    إذا (abit == BIT4_Z) {
		  out.مجموعه_بیت(idx, bbit);
	    } وإلا إذا (bbit == BIT4_Z) {
		  out.مجموعه_بیت(idx, abit);
	    } وإلا إذا (abit == BIT4_1 || bbit == BIT4_1) {
		  out.مجموعه_بیت(idx, BIT4_1);
	    } وإلا إذا (abit == BIT4_X || bbit == BIT4_X) {
		  out.مجموعه_بیت(idx, BIT4_X);
	    } وإلا {
		  out.مجموعه_بیت(idx, BIT4_0);
	    }
      }

      أرجع out;
}
