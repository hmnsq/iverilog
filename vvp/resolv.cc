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

ਵਰਤੋਂ ਨਾਮਾਕਾਸ਼ std;

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


resolv_core::resolv_core(ਬਿਨਾਨਿਸ਼ਾਨੀ nports, vvp_net_t*net)
: nports_(nports), net_(net)
{
      ਗਿਣਤੀ_functors_resolv += 1;
}

resolv_core::~resolv_core()
{
}

ਖਾਲੀ resolv_core::recv_vec4_pv_(ਬਿਨਾਨਿਸ਼ਾਨੀ ਪੋਰਟ, ਅਚਲ vvp_vector4_t&ਦੋਆਧਾਰੀ_ਅੰਕ,
				ਬਿਨਾਨਿਸ਼ਾਨੀ ਅਧਾਰ, ਬਿਨਾਨਿਸ਼ਾਨੀ vwid)
{
      ਬਿਨਾਨਿਸ਼ਾਨੀ wid = ਦੋਆਧਾਰੀ_ਅੰਕ.size();
      vvp_vector4_t res (vwid);

      ਲਈ (ਬਿਨਾਨਿਸ਼ਾਨੀ idx = 0 ;  idx < ਅਧਾਰ ;  idx += 1)
	    res.ਸੈੱਟ_ਦੋਆਧਾਰੀ_ਅੰਕ(idx, BIT4_Z);

      ਲਈ (ਬਿਨਾਨਿਸ਼ਾਨੀ idx = 0 ;  idx < wid && idx+ਅਧਾਰ < vwid;  idx += 1)
	    res.ਸੈੱਟ_ਦੋਆਧਾਰੀ_ਅੰਕ(idx+ਅਧਾਰ, ਦੋਆਧਾਰੀ_ਅੰਕ.value(idx));

      ਲਈ (ਬਿਨਾਨਿਸ਼ਾਨੀ idx = ਅਧਾਰ+wid ;  idx < vwid ;  idx += 1)
	    res.ਸੈੱਟ_ਦੋਆਧਾਰੀ_ਅੰਕ(idx, BIT4_Z);

      recv_vec4_(ਪੋਰਟ, res);
}

ਖਾਲੀ resolv_core::recv_vec8_pv_(ਬਿਨਾਨਿਸ਼ਾਨੀ ਪੋਰਟ, ਅਚਲ vvp_vector8_t&ਦੋਆਧਾਰੀ_ਅੰਕ,
				ਬਿਨਾਨਿਸ਼ਾਨੀ ਅਧਾਰ, ਬਿਨਾਨਿਸ਼ਾਨੀ vwid)
{
      ਬਿਨਾਨਿਸ਼ਾਨੀ wid = ਦੋਆਧਾਰੀ_ਅੰਕ.size();
      vvp_vector8_t res (vwid);

      ਲਈ (ਬਿਨਾਨਿਸ਼ਾਨੀ idx = 0 ;  idx < ਅਧਾਰ ;  idx += 1)
	    res.ਸੈੱਟ_ਦੋਆਧਾਰੀ_ਅੰਕ(idx, vvp_scalar_t());

      ਲਈ (ਬਿਨਾਨਿਸ਼ਾਨੀ idx = 0 ;  idx < wid && idx+ਅਧਾਰ < vwid;  idx += 1)
	    res.ਸੈੱਟ_ਦੋਆਧਾਰੀ_ਅੰਕ(idx+ਅਧਾਰ, ਦੋਆਧਾਰੀ_ਅੰਕ.value(idx));

      ਲਈ (ਬਿਨਾਨਿਸ਼ਾਨੀ idx = ਅਧਾਰ+wid ;  idx < vwid ;  idx += 1)
	    res.ਸੈੱਟ_ਦੋਆਧਾਰੀ_ਅੰਕ(idx, vvp_scalar_t());

      recv_vec8_(ਪੋਰਟ, res);
}


resolv_extend::resolv_extend(resolv_core*core, ਬਿਨਾਨਿਸ਼ਾਨੀ ਪੋਰਟ_ਅਧਾਰ)
: core_(core), ਪੋਰਟ_ਅਧਾਰ_2(ਪੋਰਟ_ਅਧਾਰ)
{
}

resolv_extend::~resolv_extend()
{
}


resolv_tri::resolv_tri(ਬਿਨਾਨਿਸ਼ਾਨੀ nports, vvp_net_t*net, vvp_scalar_t hiz_ਮੁੱਲ)
: resolv_core(nports, net), hiz_ਮੁੱਲ_2(hiz_ਮੁੱਲ)
{
        // count the input (leaf) nodes
      ਬਿਨਾਨਿਸ਼ਾਨੀ nnodes = nports;

        // add in the intermediate branch nodes
      ਜਦੋਂ (nports > 4) {
            nports = (nports + 3) / 4;
            nnodes += nports;
      }
        // add one more node for storing the output value
        // (not needed if there is only one input)
      ਜੇ (nnodes > 1)
            nnodes += 1;

      val_ = ਨਵਾਂ vvp_vector8_t [nnodes];
}

resolv_tri::~resolv_tri()
{
      ਮਿਟਾਓ[] val_;
}

ਖਾਲੀ resolv_tri::recv_vec4_(ਬਿਨਾਨਿਸ਼ਾਨੀ ਪੋਰਟ, ਅਚਲ vvp_vector4_t&ਦੋਆਧਾਰੀ_ਅੰਕ)
{
      recv_vec8_(ਪੋਰਟ, vvp_vector8_t(ਦੋਆਧਾਰੀ_ਅੰਕ, 6,6 /* STRONG */));
}

ਖਾਲੀ resolv_tri::recv_vec8_(ਬਿਨਾਨਿਸ਼ਾਨੀ ਪੋਰਟ, ਅਚਲ vvp_vector8_t&ਦੋਆਧਾਰੀ_ਅੰਕ)
{
      ਪ੍ਰਤਿਜ੍ਞਾ(ਪੋਰਟ < nports_);

      ਜੇ (val_[ਪੋਰਟ].eeq(ਦੋਆਧਾਰੀ_ਅੰਕ))
	    ਵਾਪਸ;

      val_[ਪੋਰਟ] = ਦੋਆਧਾਰੀ_ਅੰਕ;

        // Starting at the leaf level, work down the tree, resolving
        // the changed values. base is the first node in the current
        // level and span is the number of nodes at that level. ip
        // is the first node in the group of four nodes at that level
        // that include the node that has changed, and op is the node
        // at the next level that stores the resolved value from that
        // group.
      ਬਿਨਾਨਿਸ਼ਾਨੀ ਅਧਾਰ = 0;
      ਬਿਨਾਨਿਸ਼ਾਨੀ ਵਿਸਤਾਰ = nports_;
      ਜਦੋਂ (ਵਿਸਤਾਰ > 1) {
            ਬਿਨਾਨਿਸ਼ਾਨੀ ਅਗਲਾ_ਅਧਾਰ = ਅਧਾਰ + ਵਿਸਤਾਰ;
            ਬਿਨਾਨਿਸ਼ਾਨੀ ip = ਅਧਾਰ + (ਪੋਰਟ & ~0x3);
            ਬਿਨਾਨਿਸ਼ਾਨੀ op = ਅਗਲਾ_ਅਧਾਰ + (ਪੋਰਟ / 4);
            ਬਿਨਾਨਿਸ਼ਾਨੀ ll = min(ip + 4, ਅਗਲਾ_ਅਧਾਰ);

            vvp_vector8_t out = val_[ip];
            ਲਈ (ip = ip + 1; ip < ll; ip += 1) {
                  ਜੇ (val_[ip].size() == 0)
                        ਜਾਰੀ;
                  ਜੇ (out.size() == 0)
                        out = val_[ip];
                  ਨਹੀਂਤਾਂ
                        out = resolve(out, val_[ip]);
            }
            ਜੇ (val_[op].eeq(out))
                  ਵਾਪਸ;
            val_[op] = out;

            ਅਧਾਰ = ਅਗਲਾ_ਅਧਾਰ;
            ਵਿਸਤਾਰ = (ਵਿਸਤਾਰ + 3) / 4;
            ਪੋਰਟ = ਪੋਰਟ / 4;
      }

      ਜੇ (! hiz_ਮੁੱਲ_2.is_hiz()) {
	    ਲਈ (ਬਿਨਾਨਿਸ਼ਾਨੀ idx = 0 ;  idx < val_[ਅਧਾਰ].size() ;  idx += 1) {
		  val_[ਅਧਾਰ].ਸੈੱਟ_ਦੋਆਧਾਰੀ_ਅੰਕ(idx, resolve(val_[ਅਧਾਰ].value(idx),
						  hiz_ਮੁੱਲ_2));
	    }
      }

      net_->send_vec8(val_[ਅਧਾਰ]);
}

ਖਾਲੀ resolv_tri::ਗਿਣਤੀ_drivers(ਬਿਨਾਨਿਸ਼ਾਨੀ ਦੋਆਧਾਰੀ_ਅੰਕ_idx, ਬਿਨਾਨਿਸ਼ਾਨੀ counts[3])
{
      ਲਈ (ਬਿਨਾਨਿਸ਼ਾਨੀ idx = 0 ; idx < nports_ ; idx += 1) {
	    ਜੇ (val_[idx].size() == 0)
	          ਜਾਰੀ;

            update_driver_counts(val_[idx].value(ਦੋਆਧਾਰੀ_ਅੰਕ_idx).value(), counts);
      }
}


resolv_wired_logic::resolv_wired_logic(ਬਿਨਾਨਿਸ਼ਾਨੀ nports, vvp_net_t*net)
: resolv_core(nports, net)
{
        // count the input (leaf) nodes
      ਬਿਨਾਨਿਸ਼ਾਨੀ nnodes = nports;

        // add in the intermediate branch nodes
      ਜਦੋਂ (nports > 4) {
            nports = (nports + 3) / 4;
            nnodes += nports;
      }
        // add one more node for storing the output value
        // (not needed if there is only one input)
      ਜੇ (nnodes > 1)
            nnodes += 1;

      val_ = ਨਵਾਂ vvp_vector4_t [nnodes];
}

resolv_wired_logic::~resolv_wired_logic()
{
      ਮਿਟਾਓ[] val_;
}

ਖਾਲੀ resolv_wired_logic::recv_vec4_(ਬਿਨਾਨਿਸ਼ਾਨੀ ਪੋਰਟ, ਅਚਲ vvp_vector4_t&ਦੋਆਧਾਰੀ_ਅੰਕ)
{
      ਪ੍ਰਤਿਜ੍ਞਾ(ਪੋਰਟ < nports_);

      ਜੇ (val_[ਪੋਰਟ].eeq(ਦੋਆਧਾਰੀ_ਅੰਕ))
	    ਵਾਪਸ;

      val_[ਪੋਰਟ] = ਦੋਆਧਾਰੀ_ਅੰਕ;

        // Starting at the leaf level, work down the tree, resolving
        // the changed values. base is the first node in the current
        // level and span is the number of nodes at that level. ip
        // is the first node in the group of four nodes at that level
        // that include the node that has changed, and op is the node
        // at the next level that stores the resolved value from that
        // group.
      ਬਿਨਾਨਿਸ਼ਾਨੀ ਅਧਾਰ = 0;
      ਬਿਨਾਨਿਸ਼ਾਨੀ ਵਿਸਤਾਰ = nports_;
      ਜਦੋਂ (ਵਿਸਤਾਰ > 1) {
            ਬਿਨਾਨਿਸ਼ਾਨੀ ਅਗਲਾ_ਅਧਾਰ = ਅਧਾਰ + ਵਿਸਤਾਰ;
            ਬਿਨਾਨਿਸ਼ਾਨੀ ip = ਅਧਾਰ + (ਪੋਰਟ & ~0x3);
            ਬਿਨਾਨਿਸ਼ਾਨੀ op = ਅਗਲਾ_ਅਧਾਰ + (ਪੋਰਟ / 4);
            ਬਿਨਾਨਿਸ਼ਾਨੀ ll = min(ip + 4, ਅਗਲਾ_ਅਧਾਰ);

            vvp_vector4_t out = val_[ip];
            ਲਈ (ip = ip + 1; ip < ll; ip += 1) {
                  ਜੇ (val_[ip].size() == 0)
                        ਜਾਰੀ;
                  ਜੇ (out.size() == 0)
                        out = val_[ip];
                  ਨਹੀਂਤਾਂ
                        out = wired_logic_math_(out, val_[ip]);
            }
            ਜੇ (val_[op].eeq(out))
                  ਵਾਪਸ;
            val_[op] = out;

            ਅਧਾਰ = ਅਗਲਾ_ਅਧਾਰ;
            ਵਿਸਤਾਰ = (ਵਿਸਤਾਰ + 3) / 4;
            ਪੋਰਟ = ਪੋਰਟ / 4;
      }

      net_->send_vec4(val_[ਅਧਾਰ], 0);
}

ਖਾਲੀ resolv_wired_logic::recv_vec8_(ਬਿਨਾਨਿਸ਼ਾਨੀ ਪੋਰਟ, ਅਚਲ vvp_vector8_t&ਦੋਆਧਾਰੀ_ਅੰਕ)
{
      recv_vec4_(ਪੋਰਟ, reduce4(ਦੋਆਧਾਰੀ_ਅੰਕ));
}

ਖਾਲੀ resolv_wired_logic::ਗਿਣਤੀ_drivers(ਬਿਨਾਨਿਸ਼ਾਨੀ ਦੋਆਧਾਰੀ_ਅੰਕ_idx, ਬਿਨਾਨਿਸ਼ਾਨੀ counts[3])
{
      ਲਈ (ਬਿਨਾਨਿਸ਼ਾਨੀ idx = 0 ; idx < nports_ ; idx += 1) {
	    ਜੇ (val_[idx].size() == 0)
	          ਜਾਰੀ;

            update_driver_counts(val_[idx].value(ਦੋਆਧਾਰੀ_ਅੰਕ_idx), counts);
      }
}


resolv_triand::resolv_triand(ਬਿਨਾਨਿਸ਼ਾਨੀ nports, vvp_net_t*net)
: resolv_wired_logic(nports, net)
{
}

resolv_triand::~resolv_triand()
{
}

vvp_vector4_t resolv_triand::wired_logic_math_(vvp_vector4_t&a, vvp_vector4_t&b)
{
      ਪ੍ਰਤਿਜ੍ਞਾ(a.size() == b.size());

      vvp_vector4_t out (a.size());

      ਲਈ (ਬਿਨਾਨਿਸ਼ਾਨੀ idx = 0 ; idx < out.size() ; idx += 1) {
	    vvp_bit4_t abit = a.value(idx);
	    vvp_bit4_t bbit = b.value(idx);
	    ਜੇ (abit == BIT4_Z) {
		  out.ਸੈੱਟ_ਦੋਆਧਾਰੀ_ਅੰਕ(idx, bbit);
	    } ਨਹੀਂਤਾਂ ਜੇ (bbit == BIT4_Z) {
		  out.ਸੈੱਟ_ਦੋਆਧਾਰੀ_ਅੰਕ(idx, abit);
	    } ਨਹੀਂਤਾਂ ਜੇ (abit == BIT4_0 || bbit == BIT4_0) {
		  out.ਸੈੱਟ_ਦੋਆਧਾਰੀ_ਅੰਕ(idx, BIT4_0);
	    } ਨਹੀਂਤਾਂ ਜੇ (abit == BIT4_X || bbit == BIT4_X) {
		  out.ਸੈੱਟ_ਦੋਆਧਾਰੀ_ਅੰਕ(idx, BIT4_X);
	    } ਨਹੀਂਤਾਂ {
		  out.ਸੈੱਟ_ਦੋਆਧਾਰੀ_ਅੰਕ(idx, BIT4_1);
	    }
      }

      ਵਾਪਸ out;
}


resolv_trior::resolv_trior(ਬਿਨਾਨਿਸ਼ਾਨੀ nports, vvp_net_t*net)
: resolv_wired_logic(nports, net)
{
}

resolv_trior::~resolv_trior()
{
}

vvp_vector4_t resolv_trior::wired_logic_math_(vvp_vector4_t&a, vvp_vector4_t&b)
{
      ਪ੍ਰਤਿਜ੍ਞਾ(a.size() == b.size());

      vvp_vector4_t out (a.size());

      ਲਈ (ਬਿਨਾਨਿਸ਼ਾਨੀ idx = 0 ; idx < out.size() ; idx += 1) {
	    vvp_bit4_t abit = a.value(idx);
	    vvp_bit4_t bbit = b.value(idx);
	    ਜੇ (abit == BIT4_Z) {
		  out.ਸੈੱਟ_ਦੋਆਧਾਰੀ_ਅੰਕ(idx, bbit);
	    } ਨਹੀਂਤਾਂ ਜੇ (bbit == BIT4_Z) {
		  out.ਸੈੱਟ_ਦੋਆਧਾਰੀ_ਅੰਕ(idx, abit);
	    } ਨਹੀਂਤਾਂ ਜੇ (abit == BIT4_1 || bbit == BIT4_1) {
		  out.ਸੈੱਟ_ਦੋਆਧਾਰੀ_ਅੰਕ(idx, BIT4_1);
	    } ਨਹੀਂਤਾਂ ਜੇ (abit == BIT4_X || bbit == BIT4_X) {
		  out.ਸੈੱਟ_ਦੋਆਧਾਰੀ_ਅੰਕ(idx, BIT4_X);
	    } ਨਹੀਂਤਾਂ {
		  out.ਸੈੱਟ_ਦੋਆਧਾਰੀ_ਅੰਕ(idx, BIT4_0);
	    }
      }

      ਵਾਪਸ out;
}
