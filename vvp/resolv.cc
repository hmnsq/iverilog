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

ఉపయోగించి నామాకాశ std;

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


resolv_core::resolv_core(గుర్తులేని nports, vvp_net_t*net)
: nports_(nports), net_(net)
{
      లెక్క_functors_resolv += 1;
}

resolv_core::~resolv_core()
{
}

శూన్య resolv_core::recv_vec4_pv_(గుర్తులేని పోర్టు, స్థిరరాశి vvp_vector4_t&బిట్టు,
				గుర్తులేని పునాది, గుర్తులేని vwid)
{
      గుర్తులేని wid = బిట్టు.size();
      vvp_vector4_t res (vwid);

      కొరకు (గుర్తులేని idx = 0 ;  idx < పునాది ;  idx += 1)
	    res.సమితి_బిట్టు(idx, BIT4_Z);

      కొరకు (గుర్తులేని idx = 0 ;  idx < wid && idx+పునాది < vwid;  idx += 1)
	    res.సమితి_బిట్టు(idx+పునాది, బిట్టు.value(idx));

      కొరకు (గుర్తులేని idx = పునాది+wid ;  idx < vwid ;  idx += 1)
	    res.సమితి_బిట్టు(idx, BIT4_Z);

      recv_vec4_(పోర్టు, res);
}

శూన్య resolv_core::recv_vec8_pv_(గుర్తులేని పోర్టు, స్థిరరాశి vvp_vector8_t&బిట్టు,
				గుర్తులేని పునాది, గుర్తులేని vwid)
{
      గుర్తులేని wid = బిట్టు.size();
      vvp_vector8_t res (vwid);

      కొరకు (గుర్తులేని idx = 0 ;  idx < పునాది ;  idx += 1)
	    res.సమితి_బిట్టు(idx, vvp_scalar_t());

      కొరకు (గుర్తులేని idx = 0 ;  idx < wid && idx+పునాది < vwid;  idx += 1)
	    res.సమితి_బిట్టు(idx+పునాది, బిట్టు.value(idx));

      కొరకు (గుర్తులేని idx = పునాది+wid ;  idx < vwid ;  idx += 1)
	    res.సమితి_బిట్టు(idx, vvp_scalar_t());

      recv_vec8_(పోర్టు, res);
}


resolv_extend::resolv_extend(resolv_core*core, గుర్తులేని పోర్టు_పునాది)
: core_(core), పోర్టు_పునాది_2(పోర్టు_పునాది)
{
}

resolv_extend::~resolv_extend()
{
}


resolv_tri::resolv_tri(గుర్తులేని nports, vvp_net_t*net, vvp_scalar_t hiz_విలువ)
: resolv_core(nports, net), hiz_విలువ_2(hiz_విలువ)
{
        // count the input (leaf) nodes
      గుర్తులేని nnodes = nports;

        // add in the intermediate branch nodes
      అయితే (nports > 4) {
            nports = (nports + 3) / 4;
            nnodes += nports;
      }
        // add one more node for storing the output value
        // (not needed if there is only one input)
      ఒకవేళ (nnodes > 1)
            nnodes += 1;

      val_ = కొత్త vvp_vector8_t [nnodes];
}

resolv_tri::~resolv_tri()
{
      తొలగించు[] val_;
}

శూన్య resolv_tri::recv_vec4_(గుర్తులేని పోర్టు, స్థిరరాశి vvp_vector4_t&బిట్టు)
{
      recv_vec8_(పోర్టు, vvp_vector8_t(బిట్టు, 6,6 /* STRONG */));
}

శూన్య resolv_tri::recv_vec8_(గుర్తులేని పోర్టు, స్థిరరాశి vvp_vector8_t&బిట్టు)
{
      ప్రతిజ్ఞా(పోర్టు < nports_);

      ఒకవేళ (val_[పోర్టు].eeq(బిట్టు))
	    తిరిగి;

      val_[పోర్టు] = బిట్టు;

        // Starting at the leaf level, work down the tree, resolving
        // the changed values. base is the first node in the current
        // level and span is the number of nodes at that level. ip
        // is the first node in the group of four nodes at that level
        // that include the node that has changed, and op is the node
        // at the next level that stores the resolved value from that
        // group.
      గుర్తులేని పునాది = 0;
      గుర్తులేని పరిధి = nports_;
      అయితే (పరిధి > 1) {
            గుర్తులేని తదుపరి_పునాది = పునాది + పరిధి;
            గుర్తులేని ip = పునాది + (పోర్టు & ~0x3);
            గుర్తులేని op = తదుపరి_పునాది + (పోర్టు / 4);
            గుర్తులేని ll = min(ip + 4, తదుపరి_పునాది);

            vvp_vector8_t out = val_[ip];
            కొరకు (ip = ip + 1; ip < ll; ip += 1) {
                  ఒకవేళ (val_[ip].size() == 0)
                        కొనసాగు;
                  ఒకవేళ (out.size() == 0)
                        out = val_[ip];
                  లేకపోతే
                        out = resolve(out, val_[ip]);
            }
            ఒకవేళ (val_[op].eeq(out))
                  తిరిగి;
            val_[op] = out;

            పునాది = తదుపరి_పునాది;
            పరిధి = (పరిధి + 3) / 4;
            పోర్టు = పోర్టు / 4;
      }

      ఒకవేళ (! hiz_విలువ_2.is_hiz()) {
	    కొరకు (గుర్తులేని idx = 0 ;  idx < val_[పునాది].size() ;  idx += 1) {
		  val_[పునాది].సమితి_బిట్టు(idx, resolve(val_[పునాది].value(idx),
						  hiz_విలువ_2));
	    }
      }

      net_->send_vec8(val_[పునాది]);
}

శూన్య resolv_tri::లెక్క_drivers(గుర్తులేని బిట్టు_idx, గుర్తులేని counts[3])
{
      కొరకు (గుర్తులేని idx = 0 ; idx < nports_ ; idx += 1) {
	    ఒకవేళ (val_[idx].size() == 0)
	          కొనసాగు;

            update_driver_counts(val_[idx].value(బిట్టు_idx).value(), counts);
      }
}


resolv_wired_logic::resolv_wired_logic(గుర్తులేని nports, vvp_net_t*net)
: resolv_core(nports, net)
{
        // count the input (leaf) nodes
      గుర్తులేని nnodes = nports;

        // add in the intermediate branch nodes
      అయితే (nports > 4) {
            nports = (nports + 3) / 4;
            nnodes += nports;
      }
        // add one more node for storing the output value
        // (not needed if there is only one input)
      ఒకవేళ (nnodes > 1)
            nnodes += 1;

      val_ = కొత్త vvp_vector4_t [nnodes];
}

resolv_wired_logic::~resolv_wired_logic()
{
      తొలగించు[] val_;
}

శూన్య resolv_wired_logic::recv_vec4_(గుర్తులేని పోర్టు, స్థిరరాశి vvp_vector4_t&బిట్టు)
{
      ప్రతిజ్ఞా(పోర్టు < nports_);

      ఒకవేళ (val_[పోర్టు].eeq(బిట్టు))
	    తిరిగి;

      val_[పోర్టు] = బిట్టు;

        // Starting at the leaf level, work down the tree, resolving
        // the changed values. base is the first node in the current
        // level and span is the number of nodes at that level. ip
        // is the first node in the group of four nodes at that level
        // that include the node that has changed, and op is the node
        // at the next level that stores the resolved value from that
        // group.
      గుర్తులేని పునాది = 0;
      గుర్తులేని పరిధి = nports_;
      అయితే (పరిధి > 1) {
            గుర్తులేని తదుపరి_పునాది = పునాది + పరిధి;
            గుర్తులేని ip = పునాది + (పోర్టు & ~0x3);
            గుర్తులేని op = తదుపరి_పునాది + (పోర్టు / 4);
            గుర్తులేని ll = min(ip + 4, తదుపరి_పునాది);

            vvp_vector4_t out = val_[ip];
            కొరకు (ip = ip + 1; ip < ll; ip += 1) {
                  ఒకవేళ (val_[ip].size() == 0)
                        కొనసాగు;
                  ఒకవేళ (out.size() == 0)
                        out = val_[ip];
                  లేకపోతే
                        out = wired_logic_math_(out, val_[ip]);
            }
            ఒకవేళ (val_[op].eeq(out))
                  తిరిగి;
            val_[op] = out;

            పునాది = తదుపరి_పునాది;
            పరిధి = (పరిధి + 3) / 4;
            పోర్టు = పోర్టు / 4;
      }

      net_->send_vec4(val_[పునాది], 0);
}

శూన్య resolv_wired_logic::recv_vec8_(గుర్తులేని పోర్టు, స్థిరరాశి vvp_vector8_t&బిట్టు)
{
      recv_vec4_(పోర్టు, reduce4(బిట్టు));
}

శూన్య resolv_wired_logic::లెక్క_drivers(గుర్తులేని బిట్టు_idx, గుర్తులేని counts[3])
{
      కొరకు (గుర్తులేని idx = 0 ; idx < nports_ ; idx += 1) {
	    ఒకవేళ (val_[idx].size() == 0)
	          కొనసాగు;

            update_driver_counts(val_[idx].value(బిట్టు_idx), counts);
      }
}


resolv_triand::resolv_triand(గుర్తులేని nports, vvp_net_t*net)
: resolv_wired_logic(nports, net)
{
}

resolv_triand::~resolv_triand()
{
}

vvp_vector4_t resolv_triand::wired_logic_math_(vvp_vector4_t&a, vvp_vector4_t&b)
{
      ప్రతిజ్ఞా(a.size() == b.size());

      vvp_vector4_t out (a.size());

      కొరకు (గుర్తులేని idx = 0 ; idx < out.size() ; idx += 1) {
	    vvp_bit4_t abit = a.value(idx);
	    vvp_bit4_t bbit = b.value(idx);
	    ఒకవేళ (abit == BIT4_Z) {
		  out.సమితి_బిట్టు(idx, bbit);
	    } లేకపోతే ఒకవేళ (bbit == BIT4_Z) {
		  out.సమితి_బిట్టు(idx, abit);
	    } లేకపోతే ఒకవేళ (abit == BIT4_0 || bbit == BIT4_0) {
		  out.సమితి_బిట్టు(idx, BIT4_0);
	    } లేకపోతే ఒకవేళ (abit == BIT4_X || bbit == BIT4_X) {
		  out.సమితి_బిట్టు(idx, BIT4_X);
	    } లేకపోతే {
		  out.సమితి_బిట్టు(idx, BIT4_1);
	    }
      }

      తిరిగి out;
}


resolv_trior::resolv_trior(గుర్తులేని nports, vvp_net_t*net)
: resolv_wired_logic(nports, net)
{
}

resolv_trior::~resolv_trior()
{
}

vvp_vector4_t resolv_trior::wired_logic_math_(vvp_vector4_t&a, vvp_vector4_t&b)
{
      ప్రతిజ్ఞా(a.size() == b.size());

      vvp_vector4_t out (a.size());

      కొరకు (గుర్తులేని idx = 0 ; idx < out.size() ; idx += 1) {
	    vvp_bit4_t abit = a.value(idx);
	    vvp_bit4_t bbit = b.value(idx);
	    ఒకవేళ (abit == BIT4_Z) {
		  out.సమితి_బిట్టు(idx, bbit);
	    } లేకపోతే ఒకవేళ (bbit == BIT4_Z) {
		  out.సమితి_బిట్టు(idx, abit);
	    } లేకపోతే ఒకవేళ (abit == BIT4_1 || bbit == BIT4_1) {
		  out.సమితి_బిట్టు(idx, BIT4_1);
	    } లేకపోతే ఒకవేళ (abit == BIT4_X || bbit == BIT4_X) {
		  out.సమితి_బిట్టు(idx, BIT4_X);
	    } లేకపోతే {
		  out.సమితి_బిట్టు(idx, BIT4_0);
	    }
      }

      తిరిగి out;
}
