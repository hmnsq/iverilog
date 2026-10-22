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

ಬಳಸಿ ನಾಮಾಕಾಶ std;

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


resolv_core::resolv_core(ಚಿಹ್ನೆರಹಿತ nports, vvp_net_t*net)
: nports_(nports), net_(net)
{
      ಎಣಿಕೆ_functors_resolv += 1;
}

resolv_core::~resolv_core()
{
}

ಶೂನ್ಯ resolv_core::recv_vec4_pv_(ಚಿಹ್ನೆರಹಿತ ಪೋರ್ಟ್, ಸ್ಥಿರಾಂಕ vvp_vector4_t&ಬಿಟ್,
				ಚಿಹ್ನೆರಹಿತ ತಳ, ಚಿಹ್ನೆರಹಿತ vwid)
{
      ಚಿಹ್ನೆರಹಿತ wid = ಬಿಟ್.size();
      vvp_vector4_t res (vwid);

      ಗಾಗಿ (ಚಿಹ್ನೆರಹಿತ idx = 0 ;  idx < ತಳ ;  idx += 1)
	    res.ಗಣ_ಬಿಟ್(idx, BIT4_Z);

      ಗಾಗಿ (ಚಿಹ್ನೆರಹಿತ idx = 0 ;  idx < wid && idx+ತಳ < vwid;  idx += 1)
	    res.ಗಣ_ಬಿಟ್(idx+ತಳ, ಬಿಟ್.value(idx));

      ಗಾಗಿ (ಚಿಹ್ನೆರಹಿತ idx = ತಳ+wid ;  idx < vwid ;  idx += 1)
	    res.ಗಣ_ಬಿಟ್(idx, BIT4_Z);

      recv_vec4_(ಪೋರ್ಟ್, res);
}

ಶೂನ್ಯ resolv_core::recv_vec8_pv_(ಚಿಹ್ನೆರಹಿತ ಪೋರ್ಟ್, ಸ್ಥಿರಾಂಕ vvp_vector8_t&ಬಿಟ್,
				ಚಿಹ್ನೆರಹಿತ ತಳ, ಚಿಹ್ನೆರಹಿತ vwid)
{
      ಚಿಹ್ನೆರಹಿತ wid = ಬಿಟ್.size();
      vvp_vector8_t res (vwid);

      ಗಾಗಿ (ಚಿಹ್ನೆರಹಿತ idx = 0 ;  idx < ತಳ ;  idx += 1)
	    res.ಗಣ_ಬಿಟ್(idx, vvp_scalar_t());

      ಗಾಗಿ (ಚಿಹ್ನೆರಹಿತ idx = 0 ;  idx < wid && idx+ತಳ < vwid;  idx += 1)
	    res.ಗಣ_ಬಿಟ್(idx+ತಳ, ಬಿಟ್.value(idx));

      ಗಾಗಿ (ಚಿಹ್ನೆರಹಿತ idx = ತಳ+wid ;  idx < vwid ;  idx += 1)
	    res.ಗಣ_ಬಿಟ್(idx, vvp_scalar_t());

      recv_vec8_(ಪೋರ್ಟ್, res);
}


resolv_extend::resolv_extend(resolv_core*core, ಚಿಹ್ನೆರಹಿತ ಪೋರ್ಟ್_ತಳ)
: core_(core), ಪೋರ್ಟ್_ತಳ_2(ಪೋರ್ಟ್_ತಳ)
{
}

resolv_extend::~resolv_extend()
{
}


resolv_tri::resolv_tri(ಚಿಹ್ನೆರಹಿತ nports, vvp_net_t*net, vvp_scalar_t hiz_ಮೌಲ್ಯ)
: resolv_core(nports, net), hiz_ಮೌಲ್ಯ_2(hiz_ಮೌಲ್ಯ)
{
        // count the input (leaf) nodes
      ಚಿಹ್ನೆರಹಿತ nnodes = nports;

        // add in the intermediate branch nodes
      ಆಗ (nports > 4) {
            nports = (nports + 3) / 4;
            nnodes += nports;
      }
        // add one more node for storing the output value
        // (not needed if there is only one input)
      ಆದರೆ (nnodes > 1)
            nnodes += 1;

      val_ = ಹೊಸ vvp_vector8_t [nnodes];
}

resolv_tri::~resolv_tri()
{
      ಅಳಿಸು[] val_;
}

ಶೂನ್ಯ resolv_tri::recv_vec4_(ಚಿಹ್ನೆರಹಿತ ಪೋರ್ಟ್, ಸ್ಥಿರಾಂಕ vvp_vector4_t&ಬಿಟ್)
{
      recv_vec8_(ಪೋರ್ಟ್, vvp_vector8_t(ಬಿಟ್, 6,6 /* STRONG */));
}

ಶೂನ್ಯ resolv_tri::recv_vec8_(ಚಿಹ್ನೆರಹಿತ ಪೋರ್ಟ್, ಸ್ಥಿರಾಂಕ vvp_vector8_t&ಬಿಟ್)
{
      ಪ್ರತಿಜ್ಞಾ(ಪೋರ್ಟ್ < nports_);

      ಆದರೆ (val_[ಪೋರ್ಟ್].eeq(ಬಿಟ್))
	    ಹಿಂತಿರುಗಿ;

      val_[ಪೋರ್ಟ್] = ಬಿಟ್;

        // Starting at the leaf level, work down the tree, resolving
        // the changed values. base is the first node in the current
        // level and span is the number of nodes at that level. ip
        // is the first node in the group of four nodes at that level
        // that include the node that has changed, and op is the node
        // at the next level that stores the resolved value from that
        // group.
      ಚಿಹ್ನೆರಹಿತ ತಳ = 0;
      ಚಿಹ್ನೆರಹಿತ ಹರವು = nports_;
      ಆಗ (ಹರವು > 1) {
            ಚಿಹ್ನೆರಹಿತ ಮುಂದಿನ_ತಳ = ತಳ + ಹರವು;
            ಚಿಹ್ನೆರಹಿತ ip = ತಳ + (ಪೋರ್ಟ್ & ~0x3);
            ಚಿಹ್ನೆರಹಿತ op = ಮುಂದಿನ_ತಳ + (ಪೋರ್ಟ್ / 4);
            ಚಿಹ್ನೆರಹಿತ ll = min(ip + 4, ಮುಂದಿನ_ತಳ);

            vvp_vector8_t out = val_[ip];
            ಗಾಗಿ (ip = ip + 1; ip < ll; ip += 1) {
                  ಆದರೆ (val_[ip].size() == 0)
                        ಮುಂದುವರಿ;
                  ಆದರೆ (out.size() == 0)
                        out = val_[ip];
                  ಇಲ್ಲದಿದ್ದರೆ
                        out = resolve(out, val_[ip]);
            }
            ಆದರೆ (val_[op].eeq(out))
                  ಹಿಂತಿರುಗಿ;
            val_[op] = out;

            ತಳ = ಮುಂದಿನ_ತಳ;
            ಹರವು = (ಹರವು + 3) / 4;
            ಪೋರ್ಟ್ = ಪೋರ್ಟ್ / 4;
      }

      ಆದರೆ (! hiz_ಮೌಲ್ಯ_2.is_hiz()) {
	    ಗಾಗಿ (ಚಿಹ್ನೆರಹಿತ idx = 0 ;  idx < val_[ತಳ].size() ;  idx += 1) {
		  val_[ತಳ].ಗಣ_ಬಿಟ್(idx, resolve(val_[ತಳ].value(idx),
						  hiz_ಮೌಲ್ಯ_2));
	    }
      }

      net_->send_vec8(val_[ತಳ]);
}

ಶೂನ್ಯ resolv_tri::ಎಣಿಕೆ_drivers(ಚಿಹ್ನೆರಹಿತ ಬಿಟ್_idx, ಚಿಹ್ನೆರಹಿತ counts[3])
{
      ಗಾಗಿ (ಚಿಹ್ನೆರಹಿತ idx = 0 ; idx < nports_ ; idx += 1) {
	    ಆದರೆ (val_[idx].size() == 0)
	          ಮುಂದುವರಿ;

            update_driver_counts(val_[idx].value(ಬಿಟ್_idx).value(), counts);
      }
}


resolv_wired_logic::resolv_wired_logic(ಚಿಹ್ನೆರಹಿತ nports, vvp_net_t*net)
: resolv_core(nports, net)
{
        // count the input (leaf) nodes
      ಚಿಹ್ನೆರಹಿತ nnodes = nports;

        // add in the intermediate branch nodes
      ಆಗ (nports > 4) {
            nports = (nports + 3) / 4;
            nnodes += nports;
      }
        // add one more node for storing the output value
        // (not needed if there is only one input)
      ಆದರೆ (nnodes > 1)
            nnodes += 1;

      val_ = ಹೊಸ vvp_vector4_t [nnodes];
}

resolv_wired_logic::~resolv_wired_logic()
{
      ಅಳಿಸು[] val_;
}

ಶೂನ್ಯ resolv_wired_logic::recv_vec4_(ಚಿಹ್ನೆರಹಿತ ಪೋರ್ಟ್, ಸ್ಥಿರಾಂಕ vvp_vector4_t&ಬಿಟ್)
{
      ಪ್ರತಿಜ್ಞಾ(ಪೋರ್ಟ್ < nports_);

      ಆದರೆ (val_[ಪೋರ್ಟ್].eeq(ಬಿಟ್))
	    ಹಿಂತಿರುಗಿ;

      val_[ಪೋರ್ಟ್] = ಬಿಟ್;

        // Starting at the leaf level, work down the tree, resolving
        // the changed values. base is the first node in the current
        // level and span is the number of nodes at that level. ip
        // is the first node in the group of four nodes at that level
        // that include the node that has changed, and op is the node
        // at the next level that stores the resolved value from that
        // group.
      ಚಿಹ್ನೆರಹಿತ ತಳ = 0;
      ಚಿಹ್ನೆರಹಿತ ಹರವು = nports_;
      ಆಗ (ಹರವು > 1) {
            ಚಿಹ್ನೆರಹಿತ ಮುಂದಿನ_ತಳ = ತಳ + ಹರವು;
            ಚಿಹ್ನೆರಹಿತ ip = ತಳ + (ಪೋರ್ಟ್ & ~0x3);
            ಚಿಹ್ನೆರಹಿತ op = ಮುಂದಿನ_ತಳ + (ಪೋರ್ಟ್ / 4);
            ಚಿಹ್ನೆರಹಿತ ll = min(ip + 4, ಮುಂದಿನ_ತಳ);

            vvp_vector4_t out = val_[ip];
            ಗಾಗಿ (ip = ip + 1; ip < ll; ip += 1) {
                  ಆದರೆ (val_[ip].size() == 0)
                        ಮುಂದುವರಿ;
                  ಆದರೆ (out.size() == 0)
                        out = val_[ip];
                  ಇಲ್ಲದಿದ್ದರೆ
                        out = wired_logic_math_(out, val_[ip]);
            }
            ಆದರೆ (val_[op].eeq(out))
                  ಹಿಂತಿರುಗಿ;
            val_[op] = out;

            ತಳ = ಮುಂದಿನ_ತಳ;
            ಹರವು = (ಹರವು + 3) / 4;
            ಪೋರ್ಟ್ = ಪೋರ್ಟ್ / 4;
      }

      net_->send_vec4(val_[ತಳ], 0);
}

ಶೂನ್ಯ resolv_wired_logic::recv_vec8_(ಚಿಹ್ನೆರಹಿತ ಪೋರ್ಟ್, ಸ್ಥಿರಾಂಕ vvp_vector8_t&ಬಿಟ್)
{
      recv_vec4_(ಪೋರ್ಟ್, reduce4(ಬಿಟ್));
}

ಶೂನ್ಯ resolv_wired_logic::ಎಣಿಕೆ_drivers(ಚಿಹ್ನೆರಹಿತ ಬಿಟ್_idx, ಚಿಹ್ನೆರಹಿತ counts[3])
{
      ಗಾಗಿ (ಚಿಹ್ನೆರಹಿತ idx = 0 ; idx < nports_ ; idx += 1) {
	    ಆದರೆ (val_[idx].size() == 0)
	          ಮುಂದುವರಿ;

            update_driver_counts(val_[idx].value(ಬಿಟ್_idx), counts);
      }
}


resolv_triand::resolv_triand(ಚಿಹ್ನೆರಹಿತ nports, vvp_net_t*net)
: resolv_wired_logic(nports, net)
{
}

resolv_triand::~resolv_triand()
{
}

vvp_vector4_t resolv_triand::wired_logic_math_(vvp_vector4_t&a, vvp_vector4_t&b)
{
      ಪ್ರತಿಜ್ಞಾ(a.size() == b.size());

      vvp_vector4_t out (a.size());

      ಗಾಗಿ (ಚಿಹ್ನೆರಹಿತ idx = 0 ; idx < out.size() ; idx += 1) {
	    vvp_bit4_t abit = a.value(idx);
	    vvp_bit4_t bbit = b.value(idx);
	    ಆದರೆ (abit == BIT4_Z) {
		  out.ಗಣ_ಬಿಟ್(idx, bbit);
	    } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (bbit == BIT4_Z) {
		  out.ಗಣ_ಬಿಟ್(idx, abit);
	    } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (abit == BIT4_0 || bbit == BIT4_0) {
		  out.ಗಣ_ಬಿಟ್(idx, BIT4_0);
	    } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (abit == BIT4_X || bbit == BIT4_X) {
		  out.ಗಣ_ಬಿಟ್(idx, BIT4_X);
	    } ಇಲ್ಲದಿದ್ದರೆ {
		  out.ಗಣ_ಬಿಟ್(idx, BIT4_1);
	    }
      }

      ಹಿಂತಿರುಗಿ out;
}


resolv_trior::resolv_trior(ಚಿಹ್ನೆರಹಿತ nports, vvp_net_t*net)
: resolv_wired_logic(nports, net)
{
}

resolv_trior::~resolv_trior()
{
}

vvp_vector4_t resolv_trior::wired_logic_math_(vvp_vector4_t&a, vvp_vector4_t&b)
{
      ಪ್ರತಿಜ್ಞಾ(a.size() == b.size());

      vvp_vector4_t out (a.size());

      ಗಾಗಿ (ಚಿಹ್ನೆರಹಿತ idx = 0 ; idx < out.size() ; idx += 1) {
	    vvp_bit4_t abit = a.value(idx);
	    vvp_bit4_t bbit = b.value(idx);
	    ಆದರೆ (abit == BIT4_Z) {
		  out.ಗಣ_ಬಿಟ್(idx, bbit);
	    } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (bbit == BIT4_Z) {
		  out.ಗಣ_ಬಿಟ್(idx, abit);
	    } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (abit == BIT4_1 || bbit == BIT4_1) {
		  out.ಗಣ_ಬಿಟ್(idx, BIT4_1);
	    } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (abit == BIT4_X || bbit == BIT4_X) {
		  out.ಗಣ_ಬಿಟ್(idx, BIT4_X);
	    } ಇಲ್ಲದಿದ್ದರೆ {
		  out.ಗಣ_ಬಿಟ್(idx, BIT4_0);
	    }
      }

      ಹಿಂತಿರುಗಿ out;
}
