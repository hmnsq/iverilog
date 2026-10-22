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

using നാമാകാശ std;

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


resolv_core::resolv_core(unsigned nports, vvp_net_t*net)
: nports_(nports), net_(net)
{
      എണ്ണം_functors_resolv += 1;
}

resolv_core::~resolv_core()
{
}

ശൂന്യം resolv_core::recv_vec4_pv_(unsigned port, സ്ഥിരം vvp_vector4_t&bit,
				unsigned base, unsigned vwid)
{
      unsigned wid = bit.size();
      vvp_vector4_t res (vwid);

      വേണ്ടി (unsigned idx = 0 ;  idx < base ;  idx += 1)
	    res.set_bit(idx, BIT4_Z);

      വേണ്ടി (unsigned idx = 0 ;  idx < wid && idx+base < vwid;  idx += 1)
	    res.set_bit(idx+base, bit.value(idx));

      വേണ്ടി (unsigned idx = base+wid ;  idx < vwid ;  idx += 1)
	    res.set_bit(idx, BIT4_Z);

      recv_vec4_(port, res);
}

ശൂന്യം resolv_core::recv_vec8_pv_(unsigned port, സ്ഥിരം vvp_vector8_t&bit,
				unsigned base, unsigned vwid)
{
      unsigned wid = bit.size();
      vvp_vector8_t res (vwid);

      വേണ്ടി (unsigned idx = 0 ;  idx < base ;  idx += 1)
	    res.set_bit(idx, vvp_scalar_t());

      വേണ്ടി (unsigned idx = 0 ;  idx < wid && idx+base < vwid;  idx += 1)
	    res.set_bit(idx+base, bit.value(idx));

      വേണ്ടി (unsigned idx = base+wid ;  idx < vwid ;  idx += 1)
	    res.set_bit(idx, vvp_scalar_t());

      recv_vec8_(port, res);
}


resolv_extend::resolv_extend(resolv_core*core, unsigned port_base)
: core_(core), port_base_(port_base)
{
}

resolv_extend::~resolv_extend()
{
}


resolv_tri::resolv_tri(unsigned nports, vvp_net_t*net, vvp_scalar_t hiz_മൂല്യം)
: resolv_core(nports, net), hiz_മൂല്യം_2(hiz_മൂല്യം)
{
        // count the input (leaf) nodes
      unsigned nnodes = nports;

        // add in the intermediate branch nodes
      ആകുമ്പോൾ (nports > 4) {
            nports = (nports + 3) / 4;
            nnodes += nports;
      }
        // add one more node for storing the output value
        // (not needed if there is only one input)
      എങ്കിൽ (nnodes > 1)
            nnodes += 1;

      val_ = പുതിയ vvp_vector8_t [nnodes];
}

resolv_tri::~resolv_tri()
{
      delete[] val_;
}

ശൂന്യം resolv_tri::recv_vec4_(unsigned port, സ്ഥിരം vvp_vector4_t&bit)
{
      recv_vec8_(port, vvp_vector8_t(bit, 6,6 /* STRONG */));
}

ശൂന്യം resolv_tri::recv_vec8_(unsigned port, സ്ഥിരം vvp_vector8_t&bit)
{
      പ്രതിജ്ഞാ(port < nports_);

      എങ്കിൽ (val_[port].eeq(bit))
	    തിരികെ;

      val_[port] = bit;

        // Starting at the leaf level, work down the tree, resolving
        // the changed values. base is the first node in the current
        // level and span is the number of nodes at that level. ip
        // is the first node in the group of four nodes at that level
        // that include the node that has changed, and op is the node
        // at the next level that stores the resolved value from that
        // group.
      unsigned base = 0;
      unsigned span = nports_;
      ആകുമ്പോൾ (span > 1) {
            unsigned next_base = base + span;
            unsigned ip = base + (port & ~0x3);
            unsigned op = next_base + (port / 4);
            unsigned ll = min(ip + 4, next_base);

            vvp_vector8_t out = val_[ip];
            വേണ്ടി (ip = ip + 1; ip < ll; ip += 1) {
                  എങ്കിൽ (val_[ip].size() == 0)
                        തുടരുക;
                  എങ്കിൽ (out.size() == 0)
                        out = val_[ip];
                  അല്ലെങ്കിൽ
                        out = resolve(out, val_[ip]);
            }
            എങ്കിൽ (val_[op].eeq(out))
                  തിരികെ;
            val_[op] = out;

            base = next_base;
            span = (span + 3) / 4;
            port = port / 4;
      }

      എങ്കിൽ (! hiz_മൂല്യം_2.is_hiz()) {
	    വേണ്ടി (unsigned idx = 0 ;  idx < val_[base].size() ;  idx += 1) {
		  val_[base].set_bit(idx, resolve(val_[base].value(idx),
						  hiz_മൂല്യം_2));
	    }
      }

      net_->send_vec8(val_[base]);
}

ശൂന്യം resolv_tri::എണ്ണം_drivers(unsigned bit_idx, unsigned counts[3])
{
      വേണ്ടി (unsigned idx = 0 ; idx < nports_ ; idx += 1) {
	    എങ്കിൽ (val_[idx].size() == 0)
	          തുടരുക;

            update_driver_counts(val_[idx].value(bit_idx).value(), counts);
      }
}


resolv_wired_logic::resolv_wired_logic(unsigned nports, vvp_net_t*net)
: resolv_core(nports, net)
{
        // count the input (leaf) nodes
      unsigned nnodes = nports;

        // add in the intermediate branch nodes
      ആകുമ്പോൾ (nports > 4) {
            nports = (nports + 3) / 4;
            nnodes += nports;
      }
        // add one more node for storing the output value
        // (not needed if there is only one input)
      എങ്കിൽ (nnodes > 1)
            nnodes += 1;

      val_ = പുതിയ vvp_vector4_t [nnodes];
}

resolv_wired_logic::~resolv_wired_logic()
{
      delete[] val_;
}

ശൂന്യം resolv_wired_logic::recv_vec4_(unsigned port, സ്ഥിരം vvp_vector4_t&bit)
{
      പ്രതിജ്ഞാ(port < nports_);

      എങ്കിൽ (val_[port].eeq(bit))
	    തിരികെ;

      val_[port] = bit;

        // Starting at the leaf level, work down the tree, resolving
        // the changed values. base is the first node in the current
        // level and span is the number of nodes at that level. ip
        // is the first node in the group of four nodes at that level
        // that include the node that has changed, and op is the node
        // at the next level that stores the resolved value from that
        // group.
      unsigned base = 0;
      unsigned span = nports_;
      ആകുമ്പോൾ (span > 1) {
            unsigned next_base = base + span;
            unsigned ip = base + (port & ~0x3);
            unsigned op = next_base + (port / 4);
            unsigned ll = min(ip + 4, next_base);

            vvp_vector4_t out = val_[ip];
            വേണ്ടി (ip = ip + 1; ip < ll; ip += 1) {
                  എങ്കിൽ (val_[ip].size() == 0)
                        തുടരുക;
                  എങ്കിൽ (out.size() == 0)
                        out = val_[ip];
                  അല്ലെങ്കിൽ
                        out = wired_logic_math_(out, val_[ip]);
            }
            എങ്കിൽ (val_[op].eeq(out))
                  തിരികെ;
            val_[op] = out;

            base = next_base;
            span = (span + 3) / 4;
            port = port / 4;
      }

      net_->send_vec4(val_[base], 0);
}

ശൂന്യം resolv_wired_logic::recv_vec8_(unsigned port, സ്ഥിരം vvp_vector8_t&bit)
{
      recv_vec4_(port, reduce4(bit));
}

ശൂന്യം resolv_wired_logic::എണ്ണം_drivers(unsigned bit_idx, unsigned counts[3])
{
      വേണ്ടി (unsigned idx = 0 ; idx < nports_ ; idx += 1) {
	    എങ്കിൽ (val_[idx].size() == 0)
	          തുടരുക;

            update_driver_counts(val_[idx].value(bit_idx), counts);
      }
}


resolv_triand::resolv_triand(unsigned nports, vvp_net_t*net)
: resolv_wired_logic(nports, net)
{
}

resolv_triand::~resolv_triand()
{
}

vvp_vector4_t resolv_triand::wired_logic_math_(vvp_vector4_t&a, vvp_vector4_t&b)
{
      പ്രതിജ്ഞാ(a.size() == b.size());

      vvp_vector4_t out (a.size());

      വേണ്ടി (unsigned idx = 0 ; idx < out.size() ; idx += 1) {
	    vvp_bit4_t abit = a.value(idx);
	    vvp_bit4_t bbit = b.value(idx);
	    എങ്കിൽ (abit == BIT4_Z) {
		  out.set_bit(idx, bbit);
	    } അല്ലെങ്കിൽ എങ്കിൽ (bbit == BIT4_Z) {
		  out.set_bit(idx, abit);
	    } അല്ലെങ്കിൽ എങ്കിൽ (abit == BIT4_0 || bbit == BIT4_0) {
		  out.set_bit(idx, BIT4_0);
	    } അല്ലെങ്കിൽ എങ്കിൽ (abit == BIT4_X || bbit == BIT4_X) {
		  out.set_bit(idx, BIT4_X);
	    } അല്ലെങ്കിൽ {
		  out.set_bit(idx, BIT4_1);
	    }
      }

      തിരികെ out;
}


resolv_trior::resolv_trior(unsigned nports, vvp_net_t*net)
: resolv_wired_logic(nports, net)
{
}

resolv_trior::~resolv_trior()
{
}

vvp_vector4_t resolv_trior::wired_logic_math_(vvp_vector4_t&a, vvp_vector4_t&b)
{
      പ്രതിജ്ഞാ(a.size() == b.size());

      vvp_vector4_t out (a.size());

      വേണ്ടി (unsigned idx = 0 ; idx < out.size() ; idx += 1) {
	    vvp_bit4_t abit = a.value(idx);
	    vvp_bit4_t bbit = b.value(idx);
	    എങ്കിൽ (abit == BIT4_Z) {
		  out.set_bit(idx, bbit);
	    } അല്ലെങ്കിൽ എങ്കിൽ (bbit == BIT4_Z) {
		  out.set_bit(idx, abit);
	    } അല്ലെങ്കിൽ എങ്കിൽ (abit == BIT4_1 || bbit == BIT4_1) {
		  out.set_bit(idx, BIT4_1);
	    } അല്ലെങ്കിൽ എങ്കിൽ (abit == BIT4_X || bbit == BIT4_X) {
		  out.set_bit(idx, BIT4_X);
	    } അല്ലെങ്കിൽ {
		  out.set_bit(idx, BIT4_0);
	    }
      }

      തിരികെ out;
}
