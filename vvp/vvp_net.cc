/*
 * Copyright (c) 2004-2026 Stephen Williams (steve@icarus.com)
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

# include  "config.h"
# include  "vvp_net.h"
# include  "vvp_net_sig.h"
# include  "vvp_island.h"
# include  "vpi_priv.h"
# include  "resolv.h"
# include  "schedule.h"
# include  "statistics.h"
# include  <cstdio>
# include  <cstring>
# include  <cstdlib>
# include  <iostream>
# include  <typeinfo>
# include  <climits>
# include  <cmath>
# include  <cassert>
#ifdef CHECK_WITH_VALGRIND
# include  <valgrind/memcheck.h>
# include  <map>
# include  "sfunc.h"
# include  "udp.h"
# include  "ivl_alloc.h"
#endif

using നാമാകാശ std;

/* This is the size of an unsigned long in bits. This is just a
   convenience macro. */
# define CPU_WORD_BITS (8*വലുപ്പം(unsigned long))
# define മുകൾ_BIT (1UL << (CPU_WORD_BITS-1))

permaheap vvp_net_fun_t::heap_;
permaheap vvp_net_fil_t::heap_;

// Allocate around 1Megabyte/chunk.
സ്ഥിതം സ്ഥിരം size_t VVP_NET_CHUNK = 1024*1024/വലുപ്പം(vvp_net_t);
സ്ഥിതം vvp_net_t*vvp_net_alloc_table = ശൂന്യ;
#ifdef CHECK_WITH_VALGRIND
സ്ഥിതം vvp_net_t **vvp_net_pool = ശൂന്യ;
സ്ഥിതം unsigned vvp_net_pool_എണ്ണം = 0;
#endif
സ്ഥിതം size_t vvp_net_alloc_remaining = 0;
// For statistics, count the vvp_nets allocated and the bytes of alloc
// chunks allocated.
unsigned long എണ്ണം_vvp_nets = 0;
size_t size_vvp_nets = 0;

ശൂന്യം* vvp_net_t::operator പുതിയ (size_t size)
{
      പ്രതിജ്ഞാ(size == വലുപ്പം(vvp_net_t));
      എങ്കിൽ (vvp_net_alloc_remaining == 0) {
	    vvp_net_alloc_table = ::പുതിയ vvp_net_t[VVP_NET_CHUNK];
	    vvp_net_alloc_remaining = VVP_NET_CHUNK;
	    size_vvp_nets += size*VVP_NET_CHUNK;
#ifdef CHECK_WITH_VALGRIND
	    VALGRIND_MAKE_MEM_NOACCESS(vvp_net_alloc_table, size*VVP_NET_CHUNK);
	    VALGRIND_CREATE_MEMPOOL(vvp_net_alloc_table, 0, 0);
	    vvp_net_pool_എണ്ണം += 1;
	    vvp_net_pool = static_cast<vvp_net_t **>(realloc(vvp_net_pool,
	                   vvp_net_pool_എണ്ണം*വലുപ്പം(vvp_net_t **)));
	    vvp_net_pool[vvp_net_pool_എണ്ണം-1] = vvp_net_alloc_table;
#endif
      }

      vvp_net_t*return_this = vvp_net_alloc_table;
#ifdef CHECK_WITH_VALGRIND
      VALGRIND_MEMPOOL_ALLOC(vvp_net_pool[vvp_net_pool_എണ്ണം-1],
                             return_this, size);
      return_this->pool = vvp_net_pool[vvp_net_pool_എണ്ണം-1];
#endif
      vvp_net_alloc_table += 1;
      vvp_net_alloc_remaining -= 1;
      എണ്ണം_vvp_nets += 1;
      തിരികെ return_this;
}

#ifdef CHECK_WITH_VALGRIND
സ്ഥിതം map<vvp_net_t*, തർക്കം> vvp_net_map;
സ്ഥിതം map<sfunc_core*, തർക്കം> sfunc_map;
സ്ഥിതം map<vvp_udp_fun_core*, തർക്കം> udp_map;
സ്ഥിതം map<resolv_core*, തർക്കം> resolv_map;
സ്ഥിതം vvp_net_t **local_net_pool = 0;
സ്ഥിതം unsigned local_net_pool_എണ്ണം = 0;

ശൂന്യം pool_local_net(vvp_net_t*net)
{
      local_net_pool_എണ്ണം += 1;
      local_net_pool = static_cast<vvp_net_t **>(realloc(local_net_pool,
                       local_net_pool_എണ്ണം*വലുപ്പം(vvp_net_t **)));
      local_net_pool[local_net_pool_എണ്ണം-1] = net;
}

ശൂന്യം vvp_net_delete(vvp_net_t *item)
{
      vvp_net_map[item] = സത്യം;
      എങ്കിൽ (sfunc_core*tmp = dynamic_cast<sfunc_core*> (item->fun)) {
	    sfunc_map[tmp] = സത്യം;
      }
      എങ്കിൽ (vvp_udp_fun_core*tmp = dynamic_cast<vvp_udp_fun_core*> (item->fun)) {
	    udp_map[tmp] = സത്യം;
      }
      എങ്കിൽ (resolv_core*tmp = dynamic_cast<resolv_core*> (item->fun)) {
	    resolv_map[tmp] = സത്യം;
      }
	/* Only delete static object variables. */
      എങ്കിൽ (vvp_fun_signal_object_sa*tmp = dynamic_cast<vvp_fun_signal_object_sa*> (item->fun)) {
	    delete tmp;
      }
	/* Only delete static string variables. */
      എങ്കിൽ (vvp_fun_signal_string_sa*tmp = dynamic_cast<vvp_fun_signal_string_sa*> (item->fun)) {
	    delete tmp;
      }
}

ശൂന്യം vvp_net_pool_delete()
{
      unsigned long vvp_nets_del = 0;

      വേണ്ടി (unsigned idx = 0; idx < local_net_pool_എണ്ണം; idx += 1) {
	    vvp_net_delete(local_net_pool[idx]);
      }
      സ്വതന്ത്രം(local_net_pool);
      local_net_pool = 0;
      local_net_pool_എണ്ണം = 0;

      map<vvp_net_t*, തർക്കം>::iterator iter;
      വേണ്ടി (iter = vvp_net_map.begin(); iter != vvp_net_map.end(); ++ iter ) {
	    vvp_nets_del += 1;
	    VALGRIND_MEMPOOL_FREE(iter->first->pool, iter->first);
      }
      vvp_net_map.clear();

      map<sfunc_core*, തർക്കം>::iterator siter;
      വേണ്ടി (siter = sfunc_map.begin(); siter != sfunc_map.end(); ++ siter ) {
	    delete siter->first;
      }
      sfunc_map.clear();

      map<vvp_udp_fun_core*, തർക്കം>::iterator uiter;
      വേണ്ടി (uiter = udp_map.begin(); uiter != udp_map.end(); ++ uiter ) {
	    delete uiter->first;
      }
      udp_map.clear();

      map<resolv_core*, തർക്കം>::iterator riter;
      വേണ്ടി (riter = resolv_map.begin(); riter != resolv_map.end(); ++ riter ) {
	    delete riter->first;
      }
      resolv_map.clear();

      എങ്കിൽ (RUNNING_ON_VALGRIND && (vvp_nets_del != എണ്ണം_vvp_nets)) {
	    fflush(ശൂന്യ);
	    VALGRIND_PRINTF("Error: vvp missed deleting %ld of %lu net(s).",
	                    (long) എണ്ണം_vvp_nets - vvp_nets_del,
	                    എണ്ണം_vvp_nets);
      }

      വേണ്ടി (unsigned idx = 0; idx < vvp_net_pool_എണ്ണം; idx += 1) {
	    VALGRIND_DESTROY_MEMPOOL(vvp_net_pool[idx]);
	    ::delete [] vvp_net_pool[idx];
      }
      സ്വതന്ത്രം(vvp_net_pool);
      vvp_net_pool = ശൂന്യ;
      vvp_net_pool_എണ്ണം = 0;
}
#endif

ശൂന്യം vvp_net_t::operator delete(ശൂന്യം*)
{
      പ്രതിജ്ഞാ(0);
}

vvp_net_t::vvp_net_t()
: out_(vvp_net_ptr_t(0,0))
{
      fun = 0;
      fil = 0;
}

ശൂന്യം vvp_net_t::link(vvp_net_ptr_t port_to_link)
{
      vvp_net_t*net = port_to_link.ptr();

	// Connect nodes with vvp_fun_modpath_src to the head of the
	// linked list, so that vvp_fun_modpath_src are always evaluated
	// before their respective vvp_fun_modpath
      എങ്കിൽ (dynamic_cast<vvp_fun_modpath_src*>(net->fun)) {
	    net->port[port_to_link.port()] = out_;
	    out_ = port_to_link;
	// All other nodes connect after the last vvp_fun_modpath_src
      } അല്ലെങ്കിൽ {
	      // List is empty or first node has no vvp_fun_modpath_src
	      // then just insert the node into the head of linked list
	    എങ്കിൽ (out_.ptr() == nullptr || ! dynamic_cast<vvp_fun_modpath_src*>(out_.ptr()->fun)) {
		  net->port[port_to_link.port()] = out_;
		  out_ = port_to_link;
		  തിരികെ;
	    }

	    vvp_net_ptr_t cur = out_;
	    vvp_net_ptr_t prev = vvp_net_ptr_t(nullptr,0);

	      // Traverse through linked list
	    ആകുമ്പോൾ (dynamic_cast<vvp_fun_modpath_src*>(cur.ptr()->fun)) {
		  prev = cur;
		  cur = cur.ptr()->port[cur.port()];
		  എങ്കിൽ (! cur.ptr()) നിർത്തുക; // End of list
	    }

	    പ്രതിജ്ഞാ(prev.ptr());

	      // Insert after last vvp_fun_modpath_src (or end of list)
	    net->port[port_to_link.port()] = cur;
	    prev.ptr()->port[prev.port()] = port_to_link;
      }
}

/*
 * Unlink a ptr object from the driver. The input is the driver in the
 * form of a vvp_net_t pointer. The .out member of that object is the
 * driver. The dst_ptr argument is the receiver pin to be located and
 * removed from the fan-out list.
 */
ശൂന്യം vvp_net_t::unlink(vvp_net_ptr_t dst_ptr)
{
      vvp_net_t*net = dst_ptr.ptr();
      unsigned net_port = dst_ptr.port();

      എങ്കിൽ (out_ == dst_ptr) {
	      /* If the drive fan-out list starts with this pointer,
		 then the unlink is easy. Pull the list forward. */
	    out_ = net->port[net_port];
      } അല്ലെങ്കിൽ എങ്കിൽ (! out_.nil()) {
	      /* Scan the linked list, looking for the net_ptr_t
		 pointer *before* the one we wish to remove. */
	    vvp_net_ptr_t cur = out_;
	    പ്രതിജ്ഞാ(!cur.nil());
	    vvp_net_t*cur_net = cur.ptr();
	    unsigned cur_port = cur.port();
	    ആകുമ്പോൾ (cur_net && cur_net->port[cur_port] != dst_ptr) {
		  cur = cur_net->port[cur_port];
		  cur_net = cur.ptr();
		  cur_port = cur.port();
	    }
	      /* Unlink. */
	    എങ്കിൽ (cur_net) cur_net->port[cur_port] = net->port[net_port];
      }

      net->port[net_port] = vvp_net_ptr_t(0,0);
}

ശൂന്യം vvp_net_t::എണ്ണം_drivers(unsigned idx, unsigned counts[4])
{
      counts[0] = 0;
      counts[1] = 0;
      counts[2] = 0;
      counts[3] = 0;

        /* $countdrivers can only be used on wires. */
      vvp_wire_base*wire=dynamic_cast<vvp_wire_base*>(fil);
      പ്രതിജ്ഞാ(wire);

      എങ്കിൽ (wire->is_forced(idx))
            counts[3] = 1;

        /* If the net has multiple drivers, we need to interrogate the
           resolver network to get the driven values. */
      എങ്കിൽ (resolv_core*resolver = dynamic_cast<resolv_core*>(fun)) {
            resolver->എണ്ണം_drivers(idx, counts);
            തിരികെ;
      }
      എങ്കിൽ (vvp_island_port*resolver = dynamic_cast<vvp_island_port*>(fun)) {
            resolver->എണ്ണം_drivers(idx, counts);
            തിരികെ;
      }

        /* If the functor is not a resolver, there is only one driver, so
           we can just interrogate the filter to get the driven value. */
      update_driver_counts(wire->driven_മൂല്യം(idx), counts);
}

ശൂന്യം vvp_net_fun_t::operator delete(ശൂന്യം*)
{
#ifndef CHECK_WITH_VALGRIND
      പ്രതിജ്ഞാ(0);
#endif
}


vvp_net_fil_t::vvp_net_fil_t()
{
      force_link_ = 0;
      force_propagate_ = അസത്യം;
      എണ്ണം_filters += 1;
}

vvp_net_fil_t::~vvp_net_fil_t()
{
      പ്രതിജ്ഞാ(force_link_ == 0);
}

vvp_net_fil_t::prop_t vvp_net_fil_t::filter_vec4(സ്ഥിരം vvp_vector4_t&,
                                                 vvp_vector4_t&,
                                                 unsigned, unsigned)
{
      തിരികെ PROP;
}

vvp_net_fil_t::prop_t vvp_net_fil_t::filter_vec8(സ്ഥിരം vvp_vector8_t&,
                                                 vvp_vector8_t&,
                                                 unsigned, unsigned)
{
      തിരികെ PROP;
}

vvp_net_fil_t::prop_t vvp_net_fil_t::filter_real(ഇരട്ട&)
{
      തിരികെ PROP;
}

vvp_net_fil_t::prop_t vvp_net_fil_t::filter_string(സ്ഥിരം string&)
{
      തിരികെ PROP;
}

vvp_net_fil_t::prop_t vvp_net_fil_t::filter_object(vvp_object_t&)
{
      തിരികെ PROP;
}

ശൂന്യം vvp_net_fil_t::force_mask(സ്ഥിരം vvp_vector2_t&mask)
{
      എങ്കിൽ (force_mask_.size() == 0)
	    force_mask_ = vvp_vector2_t(vvp_vector2_t::FILL0, mask.size());

      പ്രതിജ്ഞാ(force_mask_.size() == mask.size());
      വേണ്ടി (unsigned idx = 0 ; idx < mask.size() ; idx += 1) {
	    എങ്കിൽ (mask.value(idx) == 0)
		  തുടരുക;

	    force_mask_.set_bit(idx, 1);
	    force_propagate_ = സത്യം;
      }
}

ശൂന്യം vvp_net_fil_t::release_mask(സ്ഥിരം vvp_vector2_t&mask)
{
      എങ്കിൽ (force_mask_.size() == 0)
	    തിരികെ;

      പ്രതിജ്ഞാ(force_mask_.size() == mask.size());
      വേണ്ടി (unsigned idx = 0 ; idx < mask.size() ; idx += 1) {
	    എങ്കിൽ (mask.value(idx))
		  force_mask_.set_bit(idx, 0);
      }

      എങ്കിൽ (force_mask_.is_zero())
	    force_mask_ = vvp_vector2_t();
}

/*
 * Force link/unlink uses a thunk vvp_net_t node with a vvp_fun_force
 * functor to translate the net values to filter commands. The ports
 * of this vvp_net_t object are use a little differently:
 *
 *     port[3]  - Point to the destination node where the forced
 *                filter resides.
 *
 *     port[2]  - Point to the input node that drives port[0] for use
 *                by the unlink method.
 *
 *     port[0]  - This is the normal input.
 */
ശൂന്യം vvp_net_fil_t::force_link(vvp_net_t*dst, vvp_net_t*src)
{
      പ്രതിജ്ഞാ(dst->fil == ഇത്);

      എങ്കിൽ (force_link_ == 0) {
	    force_link_ = പുതിയ vvp_net_t;
	      // Use port[3] to hold the force destination.
	    force_link_->port[3] = vvp_net_ptr_t(dst, 0);
	    force_link_->port[2] = vvp_net_ptr_t(0,0);
	    force_link_->fun = പുതിയ vvp_fun_force;
      }

      force_unlink();
      പ്രതിജ്ഞാ(force_link_->port[2] == vvp_net_ptr_t(0,0));

	// Use port[2] to hold the force source.
      force_link_->port[2] = vvp_net_ptr_t(src,0);

      vvp_net_ptr_t dst_ptr(force_link_, 0);
      src->link(dst_ptr);
}

ശൂന്യം vvp_net_fil_t::operator delete(ശൂന്യം*)
{
      പ്രതിജ്ഞാ(0);
}

ശൂന്യം vvp_net_fil_t::force_unlink(ശൂന്യം)
{
      എങ്കിൽ (force_link_ == 0) തിരികെ;
      vvp_net_t*src = force_link_->port[2].ptr();
      എങ്കിൽ (src == 0) തിരികെ;

      src->unlink(vvp_net_ptr_t(force_link_,0));
      force_link_->port[2] = vvp_net_ptr_t(0,0);
}

/* *** BIT operations *** */
vvp_bit4_t add_with_carry(vvp_bit4_t a, vvp_bit4_t b, vvp_bit4_t&c)
{
      എങ്കിൽ (bit4_is_xz(a) || bit4_is_xz(b) || bit4_is_xz(c)) {
	    c = BIT4_X;
	    തിരികെ BIT4_X;
      }

	// NOTE: This relies on the facts that XZ values have been
	// weeded out, and that BIT4_1 is 1 and BIT4_0 is 0.
      പൂർണ്ണസംഖ്യ sum = (പൂർണ്ണസംഖ്യ)a + (പൂർണ്ണസംഖ്യ)b + (പൂർണ്ണസംഖ്യ)c;

      തിരഞ്ഞെടുപ്പ് (sum) {
	  അവസ്ഥ 0:
	      // c must already be 0.
	    തിരികെ BIT4_0;
	  അവസ്ഥ 1:
	    c = BIT4_0;
	    തിരികെ BIT4_1;
	  അവസ്ഥ 2:
	    c = BIT4_1;
	    തിരികെ BIT4_0;
	  അവസ്ഥ 3:
	    c = BIT4_1;
	    തിരികെ BIT4_1;
      }
      fprintf(stderr, "Incorrect result %d.\n", sum);
      പ്രതിജ്ഞാ(0);
      തിരികെ BIT4_X;
}

vvp_bit4_t scalar_to_bit4(PLI_INT32 scalar)
{
      തിരഞ്ഞെടുപ്പ്(scalar) {
	  അവസ്ഥ vpi0:
	    തിരികെ BIT4_0;
	  അവസ്ഥ vpi1:
	    തിരികെ BIT4_1;
	  അവസ്ഥ vpiX:
	    തിരികെ BIT4_X;
	  അവസ്ഥ vpiZ:
	    തിരികെ BIT4_Z;
      }
      fprintf(stderr, "Unsupported scalar value %d.\n", (പൂർണ്ണസംഖ്യ)scalar);
      പ്രതിജ്ഞാ(0);
      തിരികെ BIT4_X;
}

vvp_bit4_t operator ^ (vvp_bit4_t a, vvp_bit4_t b)
{
      എങ്കിൽ (bit4_is_xz(a))
	    തിരികെ BIT4_X;
      എങ്കിൽ (bit4_is_xz(b))
	    തിരികെ BIT4_X;
      എങ്കിൽ (a == BIT4_0)
	    തിരികെ b;
      എങ്കിൽ (b == BIT4_0)
	    തിരികെ a;
      തിരികെ BIT4_0;
}

ostream& operator<<(ostream&out, vvp_bit4_t bit)
{
      തിരഞ്ഞെടുപ്പ് (bit) {
	  അവസ്ഥ BIT4_0:
	    out << "0";
	    നിർത്തുക;
	  അവസ്ഥ BIT4_1:
	    out << "1";
	    നിർത്തുക;
	  അവസ്ഥ BIT4_X:
	    out << "X";
	    നിർത്തുക;
	  അവസ്ഥ BIT4_Z:
	    out << "Z";
	    നിർത്തുക;
	  സ്ഥിരസ്ഥിതി:
	    out << "?";
	    നിർത്തുക;
      }
      തിരികെ out;
}

തരനിർവചനം unsigned short edge_t;

വരിയിൽ edge_t VVP_EDGE(vvp_bit4_t from, vvp_bit4_t to)
{
      തിരികെ 1 << ((from << 2) | to);
}

സ്ഥിരം edge_t vvp_edge_posedge
      = VVP_EDGE(BIT4_0,BIT4_1)
      | VVP_EDGE(BIT4_0,BIT4_X)
      | VVP_EDGE(BIT4_0,BIT4_Z)
      | VVP_EDGE(BIT4_X,BIT4_1)
      | VVP_EDGE(BIT4_Z,BIT4_1)
      ;

സ്ഥിരം edge_t vvp_edge_negedge
      = VVP_EDGE(BIT4_1,BIT4_0)
      | VVP_EDGE(BIT4_1,BIT4_X)
      | VVP_EDGE(BIT4_1,BIT4_Z)
      | VVP_EDGE(BIT4_X,BIT4_0)
      | VVP_EDGE(BIT4_Z,BIT4_0)
      ;

പൂർണ്ണസംഖ്യ edge(vvp_bit4_t from, vvp_bit4_t to)
{
      edge_t mask = VVP_EDGE(from, to);
      എങ്കിൽ (mask & vvp_edge_posedge)
	    തിരികെ 1;
      എങ്കിൽ (mask & vvp_edge_negedge)
	    തിരികെ -1;
      തിരികെ 0;
}

unsigned long multiply_with_carry(unsigned long a, unsigned long b,
				  unsigned long&carry)
{
      സ്ഥിരം unsigned long mask = (1UL << (CPU_WORD_BITS/2)) - 1;
      unsigned long a0 = a & mask;
      unsigned long a1 = (a >> (CPU_WORD_BITS/2)) & mask;
      unsigned long b0 = b & mask;
      unsigned long b1 = (b >> (CPU_WORD_BITS/2)) & mask;

      unsigned long tmp = a0 * b0;

      unsigned long r00 = tmp & mask;
      unsigned long c00 = (tmp >> (CPU_WORD_BITS/2)) & mask;

      tmp = a0 * b1;

      unsigned long r01 = tmp & mask;
      unsigned long c01 = (tmp >> (CPU_WORD_BITS/2)) & mask;

      tmp = a1 * b0;

      unsigned long r10 = tmp & mask;
      unsigned long c10 = (tmp >> (CPU_WORD_BITS/2)) & mask;

      tmp = a1 * b1;

      unsigned long r11 = tmp & mask;
      unsigned long c11 = (tmp >> (CPU_WORD_BITS/2)) & mask;

      unsigned long r1 = c00 + r01 + r10;
      unsigned long r2 = (r1 >> (CPU_WORD_BITS/2)) & mask;
      r1 &= mask;
      r2 += c01 + c10 + r11;
      unsigned long r3 = (r2 >> (CPU_WORD_BITS/2)) & mask;
      r2 &= mask;
      r3 += c11;
      r3 &= mask;

      carry = (r3 << (CPU_WORD_BITS/2)) + r2;
      തിരികെ (r1 << (CPU_WORD_BITS/2)) + r00;
}


ശൂന്യം vvp_send_vec8(vvp_net_ptr_t ptr, സ്ഥിരം vvp_vector8_t&val)
{
      ആകുമ്പോൾ (vvp_net_t*cur = ptr.ptr()) {
	    vvp_net_ptr_t next_val = cur->port[ptr.port()];

	    എങ്കിൽ (cur->fun)
		  cur->fun->recv_vec8(ptr, val);

	    ptr = next_val;
      }
}

ശൂന്യം vvp_send_real(vvp_net_ptr_t ptr, ഇരട്ട val, vvp_context_t context)
{
      ആകുമ്പോൾ (vvp_net_t*cur = ptr.ptr()) {
	    vvp_net_ptr_t next_val = cur->port[ptr.port()];

	    എങ്കിൽ (cur->fun)
		  cur->fun->recv_real(ptr, val, context);

	    ptr = next_val;
      }
}

സ്ഥിരം vvp_vector4_t vvp_vector4_t::nil;

ശൂന്യം vvp_vector4_t::copy_bits(സ്ഥിരം vvp_vector4_t&that)
{

      എങ്കിൽ (size_ == that.size_) {
	    എങ്കിൽ (size_ > BITS_PER_WORD) {
		  unsigned വാക്കുകൾ = (size_+BITS_PER_WORD-1) / BITS_PER_WORD;
		  വേണ്ടി (unsigned idx = 0 ;  idx < വാക്കുകൾ ;  idx += 1)
			abits_ptr_[idx] = that.abits_ptr_[idx];
		  വേണ്ടി (unsigned idx = 0 ;  idx < വാക്കുകൾ ;  idx += 1)
			bbits_ptr_[idx] = that.bbits_ptr_[idx];
	    } അല്ലെങ്കിൽ {
		  abits_val_ = that.abits_val_;
		  bbits_val_ = that.bbits_val_;
	    }
	    തിരികെ;
      }

	/* Now we know that the sizes of this and that are definitely
	   different. We can use that in code below. In any case, we
	   need to copy only the smaller of the sizes. */

	/* If source and destination are both short, then mask/copy
	   the bit values. */
      എങ്കിൽ (size_ <= BITS_PER_WORD && that.size_ <= BITS_PER_WORD) {
	    unsigned bits_to_copy = (that.size_ < size_) ? that.size_ : size_;
	    unsigned long mask = ((bits_to_copy < BITS_PER_WORD) ? (1UL << bits_to_copy) :
	                                                           (0UL)) - 1UL;
	    abits_val_ &= ~mask;
	    bbits_val_ &= ~mask;
	    abits_val_ |= that.abits_val_&mask;
	    bbits_val_ |= that.bbits_val_&mask;
	    തിരികെ;
      }

	/* Now we know that either source or destination are long. If
	   the destination is short, then mask/copy from the low word
	   of the long source. */
      എങ്കിൽ (size_ <= BITS_PER_WORD) {
	    abits_val_ = that.abits_ptr_[0];
	    bbits_val_ = that.bbits_ptr_[0];
	    എങ്കിൽ (size_ < BITS_PER_WORD) {
		  unsigned long mask = (1UL << size_) - 1UL;
		  abits_val_ &= mask;
		  bbits_val_ &= mask;
	    }
	    തിരികെ;
      }

	/* Now we know that the destination must be long. If the
	   source is short, then mask/copy from its value. */
      എങ്കിൽ (that.size_ <= BITS_PER_WORD) {
	    unsigned long mask;
	    എങ്കിൽ (that.size_ < BITS_PER_WORD) {
		  mask = (1UL << that.size_) - 1UL;
		  abits_ptr_[0] &= ~mask;
		  bbits_ptr_[0] &= ~mask;
	    } അല്ലെങ്കിൽ {
		  mask = -1UL;
	    }
	    abits_ptr_[0] |= that.abits_val_&mask;
	    bbits_ptr_[0] |= that.bbits_val_&mask;
	    തിരികെ;
      }

	/* Finally, we know that source and destination are long. copy
	   words until we get to the last. */
      unsigned bits_to_copy = (that.size_ < size_) ? that.size_ : size_;
      unsigned word = 0;
      ആകുമ്പോൾ (bits_to_copy >= BITS_PER_WORD) {
	    abits_ptr_[word] = that.abits_ptr_[word];
	    bbits_ptr_[word] = that.bbits_ptr_[word];
	    bits_to_copy -= BITS_PER_WORD;
	    word += 1;
      }
      എങ്കിൽ (bits_to_copy > 0) {
	    unsigned long mask = (1UL << bits_to_copy) - 1UL;
	    abits_ptr_[word] &= ~mask;
	    bbits_ptr_[word] &= ~mask;
	    abits_ptr_[word] |= that.abits_ptr_[word] & mask;
	    bbits_ptr_[word] |= that.bbits_ptr_[word] & mask;
      }
}

/*
 * This function should ONLY BE CALLED FROM vvp_vector4_t::copy_from_,
 * as it performs part of that functions tasks.
 */
ശൂന്യം vvp_vector4_t::copy_from_big_(സ്ഥിരം vvp_vector4_t&that)
{
      unsigned വാക്കുകൾ = (size_+BITS_PER_WORD-1) / BITS_PER_WORD;
      abits_ptr_ = പുതിയ unsigned long[2*വാക്കുകൾ];
      bbits_ptr_ = abits_ptr_ + വാക്കുകൾ;

      വേണ്ടി (unsigned idx = 0 ;  idx < വാക്കുകൾ ;  idx += 1)
	    abits_ptr_[idx] = that.abits_ptr_[idx];
      വേണ്ടി (unsigned idx = 0 ;  idx < വാക്കുകൾ ;  idx += 1)
	    bbits_ptr_[idx] = that.bbits_ptr_[idx];
}

/*
 * The copy_inverted_from_ method is just like the copy_from_ method,
 * except that we combine that with an invert. This allows the ~ and
 * the assignment to be blended in many common cases.
 */
ശൂന്യം vvp_vector4_t::copy_inverted_from_(സ്ഥിരം vvp_vector4_t&that)
{
      size_ = that.size_;
      എങ്കിൽ (size_ > BITS_PER_WORD) {
	    unsigned വാക്കുകൾ = (size_+BITS_PER_WORD-1) / BITS_PER_WORD;
	    abits_ptr_ = പുതിയ unsigned long[2*വാക്കുകൾ];
	    bbits_ptr_ = abits_ptr_ + വാക്കുകൾ;

	    unsigned remaining = size_;
	    unsigned idx = 0;
	    ആകുമ്പോൾ (remaining >= BITS_PER_WORD) {
		  abits_ptr_[idx] = that.bbits_ptr_[idx] | ~that.abits_ptr_[idx];
		  idx += 1;
		  remaining -= BITS_PER_WORD;
	    }
	    എങ്കിൽ (remaining > 0) {
		  unsigned long mask = (1UL<<remaining) - 1UL;
		  abits_ptr_[idx] = mask & (that.bbits_ptr_[idx] | ~that.abits_ptr_[idx]);
	    }

	    വേണ്ടി (idx = 0 ;  idx < വാക്കുകൾ ;  idx += 1)
		  bbits_ptr_[idx] = that.bbits_ptr_[idx];

      } അല്ലെങ്കിൽ {
	    unsigned long mask = (size_<BITS_PER_WORD)? (1UL<<size_)-1UL : -1UL;
	    abits_val_ = mask & (that.bbits_val_ | ~that.abits_val_);
	    bbits_val_ = that.bbits_val_;
      }
}

/* Make sure to set size_ before calling this routine. */
ശൂന്യം vvp_vector4_t::allocate_വാക്കുകൾ(unsigned long inita, unsigned long initb)
{
      എങ്കിൽ (size_ > BITS_PER_WORD) {
	    unsigned cnt = (size_ + BITS_PER_WORD - 1) / BITS_PER_WORD;
	    abits_ptr_ = പുതിയ unsigned long[2*cnt];
	    bbits_ptr_ = abits_ptr_ + cnt;
	    വേണ്ടി (unsigned idx = 0 ;  idx < cnt ;  idx += 1)
		  abits_ptr_[idx] = inita;
	    വേണ്ടി (unsigned idx = 0 ;  idx < cnt ;  idx += 1)
		  bbits_ptr_[idx] = initb;

      } അല്ലെങ്കിൽ {
	    abits_val_ = inita;
	    bbits_val_ = initb;
      }
}

vvp_vector4_t::vvp_vector4_t(unsigned size__, ഇരട്ട val)
: size_(size__)
{
      തർക്കം is_neg = അസത്യം;
      ഇരട്ട fraction;
      പൂർണ്ണസംഖ്യ exponent;

	/* We return 'bx for a NaN or +/- infinity. */
      എങ്കിൽ (val != val || (val && (val == 0.5*val)))  {
	    allocate_വാക്കുകൾ(WORD_X_ABITS, WORD_X_BBITS);
	    തിരികെ;
      }

	/* Convert to a positive result. */
      എങ്കിൽ (val < 0.0) {
	    is_neg = സത്യം;
	    val = -val;
      }
      allocate_വാക്കുകൾ(WORD_0_ABITS, WORD_0_BBITS);

	/* Get the exponent and fractional part of the number. */
      fraction = frexp(val, &exponent);

	/* If the value is small enough just use lround(). */
      എങ്കിൽ (exponent < BITS_PER_WORD-2) {
	    എങ്കിൽ (is_neg) ഇത്->invert();  // Invert the bits if negative.
	    long sval = lround(val);
	    എങ്കിൽ (is_neg) sval = -sval;
	      /* This requires that 0 and 1 have the same bbit value. */
	    എങ്കിൽ (size_ > BITS_PER_WORD) {
		  abits_ptr_[0] = sval;
	    } അല്ലെങ്കിൽ {
		  abits_val_ = sval;
	    }
	    തിരികെ;
      }

      unsigned nwords = (exponent-1) / BITS_PER_WORD;
      unsigned my_വാക്കുകൾ = (size_ + BITS_PER_WORD - 1) / BITS_PER_WORD - 1;

      fraction = ldexp(fraction, (exponent-1) % BITS_PER_WORD + 1);

	/* Skip any leading bits. */
      വേണ്ടി (പൂർണ്ണസംഖ്യ idx = (signed) nwords; idx > (signed) my_വാക്കുകൾ; idx -=1) {
	    unsigned long bits = (unsigned long) fraction;
	    fraction = fraction - (ഇരട്ട) bits;
	    fraction = ldexp(fraction, BITS_PER_WORD);
      }

	/* Convert the remaining bits as appropriate. */
      എങ്കിൽ (my_വാക്കുകൾ == 0) {
		  unsigned long bits = (unsigned long) fraction;
		  abits_val_ = bits;
		  fraction = fraction - (ഇരട്ട) bits;
		    /* Round any fractional part up. */
		  എങ്കിൽ (fraction >= 0.5) *ഇത് += (int64_t) 1;
      } അല്ലെങ്കിൽ {
	    എങ്കിൽ (nwords < my_വാക്കുകൾ) my_വാക്കുകൾ = nwords;
	    വേണ്ടി (പൂർണ്ണസംഖ്യ idx = (signed)my_വാക്കുകൾ; idx >= 0; idx -= 1) {
		  unsigned long bits = (unsigned long) fraction;
		  abits_ptr_[idx] = bits;
		  fraction = fraction - (ഇരട്ട) bits;
		  fraction = ldexp(fraction, BITS_PER_WORD);
	    }
	      /* Round any fractional part up. */
	    എങ്കിൽ (fraction >= ldexp(0.5, BITS_PER_WORD)) *ഇത് += (int64_t) 1;
       }

	/* Convert to a negative number if needed. */
      എങ്കിൽ (is_neg) {
	    ഇത്->invert();
	    *ഇത് += (int64_t) 1;
      }
}

vvp_vector4_t::vvp_vector4_t(സ്ഥിരം vvp_vector4_t&that,
			    unsigned adr, unsigned wid)
{
	// Set up and initialize the destination.
      size_ = wid;
      allocate_വാക്കുകൾ(WORD_X_ABITS, WORD_X_BBITS);

	// Special case: selecting from far beyond the source vector,
	// to the result is all X bits. We're done.
      എങ്കിൽ (adr >= that.size_)
	    തിരികെ;

	// Special case: The source is not quite big enough to supply
	// all bits, so get the bits that we can. The remainder will
	// be left at BIT4_X.
      എങ്കിൽ ((adr + wid) > that.size_) {
	    unsigned use_wid = that.size_ - adr;
	    വേണ്ടി (unsigned idx = 0 ; idx < use_wid ; idx += 1)
		  set_bit(idx, that.value(adr+idx));

	    തിരികെ;
      }

	// At the point, we know that the source part is entirely
	// contained in the source vector.
	// assert((adr + wid) <= that.size_);

      എങ്കിൽ (wid > BITS_PER_WORD) {
	      /* In this case, the subvector and the source vector are
		 long. Do the transfer reasonably efficiently. */
	    unsigned ptr = adr / BITS_PER_WORD;
	    unsigned long off = adr % BITS_PER_WORD;
	    unsigned long noff = BITS_PER_WORD - off;
	    unsigned long lmask = (1UL << off) - 1UL;
	    unsigned trans = 0;
	    unsigned dst = 0;
	    ആകുമ്പോൾ (trans < wid) {
		    // The low bits of the result.
		  abits_ptr_[dst] = (that.abits_ptr_[ptr] & ~lmask) >> off;
		  bbits_ptr_[dst] = (that.bbits_ptr_[ptr] & ~lmask) >> off;
		  trans += noff;

		  എങ്കിൽ (trans >= wid)
			നിർത്തുക;

		  ptr += 1;

		    // The high bits of the result. Skip this if the
		    // source and destination are perfectly aligned.
		  എങ്കിൽ (noff != BITS_PER_WORD) {
			abits_ptr_[dst] |= (that.abits_ptr_[ptr]&lmask) << noff;
			bbits_ptr_[dst] |= (that.bbits_ptr_[ptr]&lmask) << noff;
			trans += off;
		  }

		  dst += 1;
	    }

      } അല്ലെങ്കിൽ എങ്കിൽ (that.size_ > BITS_PER_WORD) {
	      /* In this case, the subvector fits in a single word,
		 but the source is large. */
	    unsigned ptr = adr / BITS_PER_WORD;
	    unsigned long off = adr % BITS_PER_WORD;
	    unsigned trans = BITS_PER_WORD - off;
	    എങ്കിൽ (trans > wid)
		  trans = wid;

	    എങ്കിൽ (trans == BITS_PER_WORD) {
		    // Very special case: Copy exactly 1 perfectly
		    // aligned word.
		  abits_val_ = that.abits_ptr_[ptr];
		  bbits_val_ = that.bbits_ptr_[ptr];

	    } അല്ലെങ്കിൽ {
		    // lmask is the low bits of the destination,
		    // masked into the source.
		  unsigned long lmask = (1UL<<trans) - 1UL;
		  lmask <<= off;

		    // The low bits of the result.
		  abits_val_ = (that.abits_ptr_[ptr] & lmask) >> off;
		  bbits_val_ = (that.bbits_ptr_[ptr] & lmask) >> off;

		  എങ്കിൽ (trans < wid) {
			  // If there are more bits, then get them
			  // from the bottom of the next word of the
			  // source.
			unsigned long hmask = (1UL << (wid-trans)) - 1UL;

			  // The high bits of the result.
			abits_val_ |= (that.abits_ptr_[ptr+1]&hmask) << trans;
			bbits_val_ |= (that.bbits_ptr_[ptr+1]&hmask) << trans;
		  }
	    }

      } അല്ലെങ്കിൽ എങ്കിൽ (size_ == BITS_PER_WORD) {
	      /* We know that source and destination are short. If the
		 destination is a full word, then we know the copy is
		 aligned and complete. */
	    abits_val_ = that.abits_val_;
	    bbits_val_ = that.bbits_val_;

      } അല്ലെങ്കിൽ {
	      /* Finally, the source and destination vectors are both
		 short, so there is a single mask/shift/copy. */
	    unsigned long mask = (1UL << size_) - 1UL;
	    mask <<= adr;

	    abits_val_ = (that.abits_val_ & mask) >> adr;
	    bbits_val_ = (that.bbits_val_ & mask) >> adr;
      }

}

/*
 * Change the size of the vvp_vector4_t vector to the new size. Copy
 * the old values, as many as well fit, into the new vector.
 */
ശൂന്യം vvp_vector4_t::resize(unsigned newsize, vvp_bit4_t pad_bit)
{
      എങ്കിൽ (size_ == newsize)
	    തിരികെ;

      unsigned cnt = (size_ + BITS_PER_WORD - 1) / BITS_PER_WORD;
      unsigned long word_pad_abits = 0;
      unsigned long word_pad_bbits = 0;
      തിരഞ്ഞെടുപ്പ് (pad_bit) {
	  അവസ്ഥ BIT4_0:
	    word_pad_abits = WORD_0_ABITS;
	    word_pad_bbits = WORD_0_BBITS;
	    നിർത്തുക;
	  അവസ്ഥ BIT4_1:
	    word_pad_abits = WORD_1_ABITS;
	    word_pad_bbits = WORD_1_BBITS;
	    നിർത്തുക;
	  അവസ്ഥ BIT4_X:
	    word_pad_abits = WORD_X_ABITS;
	    word_pad_bbits = WORD_X_BBITS;
	    നിർത്തുക;
	  അവസ്ഥ BIT4_Z:
	    word_pad_abits = WORD_Z_ABITS;
	    word_pad_bbits = WORD_Z_BBITS;
	    നിർത്തുക;
      }

      എങ്കിൽ (newsize > BITS_PER_WORD) {
	    unsigned newcnt = (newsize + BITS_PER_WORD - 1) / BITS_PER_WORD;
	    എങ്കിൽ (newcnt == cnt) {
		    // If the word count doesn't change, then there is
		    // no need for re-allocation so we are done now.
		  എങ്കിൽ (newsize > size_) {
			എങ്കിൽ (unsigned fill = size_ % BITS_PER_WORD) {
			      abits_ptr_[cnt-1] &= ~((-1UL) << fill);
			      bbits_ptr_[cnt-1] &= ~((-1UL) << fill);
			      abits_ptr_[cnt-1] |= word_pad_abits << fill;
			      bbits_ptr_[cnt-1] |= word_pad_bbits << fill;
			}
		  }
		  size_ = newsize;
		  തിരികെ;
	    }

	    unsigned long*newbits = പുതിയ unsigned long[2*newcnt];

	    എങ്കിൽ (cnt > 1) {
		  unsigned trans = cnt;
		  എങ്കിൽ (trans > newcnt)
			trans = newcnt;

		  വേണ്ടി (unsigned idx = 0 ;  idx < trans ;  idx += 1)
			newbits[idx] = abits_ptr_[idx];
		  വേണ്ടി (unsigned idx = 0 ;  idx < trans ;  idx += 1)
			newbits[newcnt+idx] = bbits_ptr_[idx];

		  delete[]abits_ptr_;

	    } അല്ലെങ്കിൽ {
		  newbits[0] = abits_val_;
		  newbits[newcnt] = bbits_val_;
	    }

	    എങ്കിൽ (newsize > size_) {
		  എങ്കിൽ (unsigned fill = size_ % BITS_PER_WORD) {
			newbits[cnt-1] &= ~((-1UL) << fill);
			newbits[cnt-1] |= word_pad_abits << fill;
			newbits[newcnt+cnt-1] &= ~((-1UL) << fill);
			newbits[newcnt+cnt-1] |= word_pad_bbits << fill;
		  }
		  വേണ്ടി (unsigned idx = cnt ;  idx < newcnt ;  idx += 1)
			newbits[idx] = word_pad_abits;
		  വേണ്ടി (unsigned idx = cnt ;  idx < newcnt ;  idx += 1)
			newbits[newcnt+idx] = word_pad_bbits;
	    }

	    size_ = newsize;
	    abits_ptr_ = newbits;
	    bbits_ptr_ = newbits + newcnt;

      } അല്ലെങ്കിൽ {
	    എങ്കിൽ (cnt > 1) {
		  unsigned long newvala = abits_ptr_[0];
		  unsigned long newvalb = bbits_ptr_[0];
		  delete[]abits_ptr_;
		  abits_val_ = newvala;
		  bbits_val_ = newvalb;
	    }

	    എങ്കിൽ (newsize > size_) {
		  abits_val_ &= ~((-1UL) << size_);
		  bbits_val_ &= ~((-1UL) << size_);
		  abits_val_ |= word_pad_abits << size_;
		  bbits_val_ |= word_pad_bbits << size_;
	    }

	    size_ = newsize;
      }
}


unsigned long* vvp_vector4_t::subarray(unsigned adr, unsigned wid, തർക്കം xz_to_0) സ്ഥിരം
{
      സ്ഥിരം unsigned BIT2_PER_WORD = 8*വലുപ്പം(unsigned long);
      unsigned awid = (wid + BIT2_PER_WORD - 1) / (BIT2_PER_WORD);
      unsigned long*val = പുതിയ unsigned long[awid];

      വേണ്ടി (unsigned idx = 0 ;  idx < awid ;  idx += 1)
	    val[idx] = 0;

      എങ്കിൽ (size_ <= BITS_PER_WORD) {
	      /* Handle the special case that the array is small. The
		 entire value of the vector4 is within the xbits_val_
		 so we know that the result is a single word, the
		 source is a single word, and we just have to loop
		 through that word. */
	    unsigned long atmp = abits_val_ >> adr;
	    unsigned long btmp = bbits_val_ >> adr;
	    എങ്കിൽ (wid < BIT2_PER_WORD) {
		  atmp &= (1UL << wid) - 1;
		  btmp &= (1UL << wid) - 1;
	    }
	    എങ്കിൽ (btmp) {
		  എങ്കിൽ (xz_to_0)
			atmp &= ~btmp;
		  അല്ലെങ്കിൽ
			പോകുക x_out;
	    }

	    val[0] = atmp;

      } അല്ലെങ്കിൽ {

	    unsigned val_ptr = 0;
	    unsigned val_off = 0;

	      /* Get the first word we are scanning. We may in fact be
		 somewhere in the middle of that word. */
	    ആകുമ്പോൾ (wid > 0) {
		  unsigned long atmp = abits_ptr_[adr/BITS_PER_WORD];
		  unsigned long btmp = bbits_ptr_[adr/BITS_PER_WORD];
		  unsigned long off = adr%BITS_PER_WORD;
		  atmp >>= off;
		  btmp >>= off;

		  unsigned long trans = BITS_PER_WORD - off;
		  എങ്കിൽ (trans > (BIT2_PER_WORD - val_off))
			trans = BIT2_PER_WORD - val_off;
		  എങ്കിൽ (wid < trans)
			trans = wid;
		  എങ്കിൽ (trans < BIT2_PER_WORD) {
			atmp &= (1UL << trans) - 1;
			btmp &= (1UL << trans) - 1;
		  }
		  എങ്കിൽ (btmp) {
			എങ്കിൽ (xz_to_0)
			      atmp &= ~btmp;
			അല്ലെങ്കിൽ
			      പോകുക x_out;
		  }

		  val[val_ptr] |= atmp << val_off;
		  adr += trans;
		  wid -= trans;
		  val_off += trans;
		  എങ്കിൽ (val_off == BIT2_PER_WORD) {
			val_ptr += 1;
			val_off = 0;
		  }
	    }
      }

      തിരികെ val;

 x_out:
      delete[]val;
      തിരികെ 0;
}

ശൂന്യം vvp_vector4_t::setarray(unsigned adr, unsigned wid, സ്ഥിരം unsigned long*val)
{
      പ്രതിജ്ഞാ(adr+wid <= size_);

      സ്ഥിരം unsigned BIT2_PER_WORD = 8*വലുപ്പം(unsigned long);

      എങ്കിൽ (size_ <= BITS_PER_WORD) {
	      // We know here that both the source and the target are
	      // within a single word. Write the bits into the
	      // abits_val_ directly.

	    പ്രതിജ്ഞാ(BIT2_PER_WORD <= BITS_PER_WORD);
	    unsigned long lmask = (1UL << adr) - 1UL;
	    unsigned long hmask = ((adr+wid) < BITS_PER_WORD)
		  ? -1UL << (adr+wid)
		  : 0;
	    unsigned long mask = ~(hmask | lmask);

	    abits_val_ &= ~mask;
	    bbits_val_ &= ~mask;

	    abits_val_ |= mask & (val[0] << adr);

      } അല്ലെങ്കിൽ {
	      // The general case, there are multiple words of
	      // destination, and possibly multiple words of source
	      // data. Shift and mask as we go.
	    unsigned off = adr % BITS_PER_WORD;
	    unsigned ptr = adr / BITS_PER_WORD;
	    unsigned val_off = 0;
	    unsigned val_ptr = 0;
	    ആകുമ്പോൾ (wid > 0) {
		  unsigned trans = wid;
		  എങ്കിൽ (trans > (BIT2_PER_WORD-val_off))
			trans = BIT2_PER_WORD-val_off;
		  എങ്കിൽ (trans > (BITS_PER_WORD-off))
			trans = BITS_PER_WORD-off;

		  unsigned long lmask = (1UL << off) - 1UL;
		  unsigned long hmask = ((off+trans) < BITS_PER_WORD)
			? -1UL << (off+trans)
			: 0;
		  unsigned long mask = ~(hmask | lmask);

		  abits_ptr_[ptr] &= ~mask;
		  bbits_ptr_[ptr] &= ~mask;
		  എങ്കിൽ (val_off >= off)
			abits_ptr_[ptr] |= mask & (val[val_ptr] >> (val_off-off));
		  അല്ലെങ്കിൽ
			abits_ptr_[ptr] |= mask & (val[val_ptr] << (off-val_off));

		  wid -= trans;
		  val_off += trans;
		  എങ്കിൽ (val_off == BIT2_PER_WORD) {
			val_off = 0;
			val_ptr += 1;
		  }
		  off += trans;
		  എങ്കിൽ (off == BITS_PER_WORD) {
			off = 0;
			ptr += 1;
		  }
	    }
      }
}

/*
 * Set the bits of that vector, which must be a subset of this vector,
 * into the addressed part of this vector. Use bit masking and word
 * copies to go as fast as reasonably possible.
 */
തർക്കം vvp_vector4_t::set_vec(unsigned adr, സ്ഥിരം vvp_vector4_t&that)
{
      പ്രതിജ്ഞാ(adr+that.size_  <= size_);
      തർക്കം diff_flag = അസത്യം;

      എങ്കിൽ (size_ <= BITS_PER_WORD) {

	      /* The destination vector (me!) is within a bits_val_
		 word, so the subvector is certainly within a
		 bits_val_ word. Therefore, the entire operation is a
		 matter of writing the bits of that into the addressed
		 bits of this. The mask below is calculated to be 1
		 for all the bits that are to come from that. Do the
		 job by some shifting, masking and OR. */

	    unsigned long lmask = (1UL << adr) - 1;
	    unsigned long hmask;
	    unsigned long hshift = adr+that.size_;
	    എങ്കിൽ (hshift >= BITS_PER_WORD)
		  hmask = -1UL;
	    അല്ലെങ്കിൽ
		  hmask = (1UL << (adr+that.size_)) - 1;
	    unsigned long mask = hmask & ~lmask;

	    unsigned long tmp = (that.abits_val_<<adr)&mask;
	    എങ്കിൽ ((abits_val_&mask) != tmp) {
		  diff_flag = സത്യം;
		  abits_val_ = (abits_val_ & ~mask) | tmp;
	    }
	    tmp = (that.bbits_val_<<adr) & mask;
	    എങ്കിൽ ((bbits_val_&mask) != tmp) {
		  diff_flag = സത്യം;
		  bbits_val_ = (bbits_val_ & ~mask) | tmp;
	    }

      } അല്ലെങ്കിൽ എങ്കിൽ (that.size_ <= BITS_PER_WORD) {

	      /* This vector is more than a word, but that vector is
		 still small. Write into the destination, possibly
		 spanning two destination words, depending on whether
		 the source vector spans a word transition. */
	    unsigned long dptr = adr / BITS_PER_WORD;
	    unsigned long doff = adr % BITS_PER_WORD;

	    unsigned long lmask = (1UL << doff) - 1;
	    unsigned long hshift = doff+that.size_;
	    unsigned long hmask;
	    എങ്കിൽ (hshift >= BITS_PER_WORD)
		  hmask = -1UL;
	    അല്ലെങ്കിൽ
		  hmask = (1UL << hshift) - 1UL;

	    unsigned long mask = hmask & ~lmask;
	    unsigned long tmp;

	    tmp = (that.abits_val_ << doff) & mask;
	    എങ്കിൽ ((abits_ptr_[dptr] & mask) != tmp) {
		  diff_flag = സത്യം;
		  abits_ptr_[dptr] = (abits_ptr_[dptr] & ~mask) | tmp;
	    }
	    tmp = (that.bbits_val_ << doff) & mask;
	    എങ്കിൽ ((bbits_ptr_[dptr] & mask) != tmp) {
		  diff_flag = സത്യം;
		  bbits_ptr_[dptr] = (bbits_ptr_[dptr] & ~mask) | tmp;
	    }

	    എങ്കിൽ ((doff + that.size_) > BITS_PER_WORD) {
		  unsigned tail = doff + that.size_ - BITS_PER_WORD;
		  mask = (1UL << tail) - 1;

		  dptr += 1;
		  tmp = (that.abits_val_ >> (that.size_-tail)) & mask;
		  എങ്കിൽ ((abits_ptr_[dptr] & mask) != tmp) {
			diff_flag = സത്യം;
			abits_ptr_[dptr] = (abits_ptr_[dptr] & ~mask) | tmp;
		  }
		  tmp = (that.bbits_val_ >> (that.size_-tail)) & mask;
		  എങ്കിൽ ((bbits_ptr_[dptr] & mask) != tmp) {
			diff_flag = സത്യം;
			bbits_ptr_[dptr] = (bbits_ptr_[dptr] & ~mask) | tmp;
		  }
	    }

      } അല്ലെങ്കിൽ എങ്കിൽ (adr%BITS_PER_WORD == 0) {

	      /* In this case, both vectors are long, but the
		 destination is neatly aligned. That means all but the
		 last word can be simply copied with no masking. */

	    unsigned remain = that.size_;
	    unsigned sptr = 0;
	    unsigned dptr = adr / BITS_PER_WORD;
	    ആകുമ്പോൾ (remain >= BITS_PER_WORD) {
		  എങ്കിൽ (abits_ptr_[dptr] != that.abits_ptr_[sptr]) {
			diff_flag = സത്യം;
			abits_ptr_[dptr] = that.abits_ptr_[sptr];
		  }
		  എങ്കിൽ (bbits_ptr_[dptr] != that.bbits_ptr_[sptr]) {
			diff_flag = സത്യം;
			bbits_ptr_[dptr] = that.bbits_ptr_[sptr];
		  }
		  dptr += 1;
		  sptr += 1;
		  remain -= BITS_PER_WORD;
	    }

	    എങ്കിൽ (remain > 0) {
		  unsigned long mask = (1UL << remain) - 1;
		  unsigned long tmp;

		  tmp = that.abits_ptr_[sptr] & mask;
		  എങ്കിൽ ((abits_ptr_[dptr] & mask) != tmp) {
			diff_flag = സത്യം;
			abits_ptr_[dptr] = (abits_ptr_[dptr] & ~mask) | tmp;
		  }
		  tmp = that.bbits_ptr_[sptr] & mask;
		  എങ്കിൽ ((bbits_ptr_[dptr] & mask) != tmp) {
			diff_flag = സത്യം;
			bbits_ptr_[dptr] = (bbits_ptr_[dptr] & ~mask) | tmp;
		  }
	    }

      } അല്ലെങ്കിൽ {

	      /* We know that there are two long vectors, and we know
		 that the destination is definitely NOT aligned. */

	    unsigned remain = that.size_;
	    unsigned sptr = 0;
	    unsigned dptr = adr / BITS_PER_WORD;
	    unsigned doff = adr % BITS_PER_WORD;
	    unsigned long lmask = (1UL << doff) - 1;
	    unsigned ndoff = BITS_PER_WORD - doff;
	    ആകുമ്പോൾ (remain >= BITS_PER_WORD) {
		  unsigned long tmp;

		  tmp = (that.abits_ptr_[sptr] << doff) & ~lmask;
		  എങ്കിൽ ((abits_ptr_[dptr] & ~lmask) != tmp) {
			diff_flag = സത്യം;
			abits_ptr_[dptr] = (abits_ptr_[dptr] & lmask) | tmp;
		  }
		  tmp = (that.bbits_ptr_[sptr] << doff) & ~lmask;
		  എങ്കിൽ ((bbits_ptr_[dptr] & ~lmask) != tmp) {
			diff_flag = സത്യം;
			bbits_ptr_[dptr] = (bbits_ptr_[dptr] & lmask) | tmp;
		  }
		  dptr += 1;

		  tmp = (that.abits_ptr_[sptr] >> ndoff) & lmask;
		  എങ്കിൽ ((abits_ptr_[dptr] & lmask) != tmp) {
			diff_flag = സത്യം;
			abits_ptr_[dptr] = (abits_ptr_[dptr] & ~lmask) | tmp;
		  }
		  tmp = (that.bbits_ptr_[sptr] >> ndoff) & lmask;
		  എങ്കിൽ ((bbits_ptr_[dptr] & lmask) != tmp) {
			diff_flag = സത്യം;
			bbits_ptr_[dptr] = (bbits_ptr_[dptr] & ~lmask) | tmp;
		  }

		  remain -= BITS_PER_WORD;
		  sptr += 1;
	    }

	    എങ്കിൽ (remain > 0) {
		  unsigned long hshift = doff+remain;
		  unsigned long hmask;
		  എങ്കിൽ (hshift >= BITS_PER_WORD)
			hmask = -1UL;
		  അല്ലെങ്കിൽ
			hmask = (1UL << (doff+remain)) - 1;

		  unsigned long mask = hmask & ~lmask;
		  unsigned long tmp;

		  tmp = (that.abits_ptr_[sptr] << doff) & mask;
		  എങ്കിൽ ((abits_ptr_[dptr] & mask) != tmp) {
			diff_flag = സത്യം;
			abits_ptr_[dptr] = (abits_ptr_[dptr] & ~mask) | tmp;
		  }
		  tmp = (that.bbits_ptr_[sptr] << doff) & mask;
		  എങ്കിൽ ((bbits_ptr_[dptr] & mask) != tmp) {
			diff_flag = സത്യം;
			bbits_ptr_[dptr] = (bbits_ptr_[dptr] & ~mask) | tmp;
		  }

		  എങ്കിൽ ((doff + remain) > BITS_PER_WORD) {
			unsigned tail = doff + remain - BITS_PER_WORD;
			എങ്കിൽ (tail >= BITS_PER_WORD)
			      mask = -1UL;
			അല്ലെങ്കിൽ
			      mask = (1UL << tail) - 1;

			dptr += 1;

			tmp = (that.abits_ptr_[sptr] >> (remain-tail))&mask;
			എങ്കിൽ ((abits_ptr_[dptr] & mask) != tmp) {
			      diff_flag = സത്യം;
			      abits_ptr_[dptr] = (abits_ptr_[dptr] & ~mask) | tmp;
			}
			tmp = (that.bbits_ptr_[sptr] >> (remain-tail))&mask;
			എങ്കിൽ ((bbits_ptr_[dptr] & mask) != tmp) {
			      diff_flag = സത്യം;
			      bbits_ptr_[dptr] = (bbits_ptr_[dptr] & ~mask) | tmp;
			}
		  }
	    }
      }

      തിരികെ diff_flag;
}

/*
 * Add that vector to this vector. Do it in the Verilog way, which
 * means if we detect any X or Z bits, change the entire results to
 * all X.
 *
 * Assume both vectors are the same size.
 */
ശൂന്യം vvp_vector4_t::add(സ്ഥിരം vvp_vector4_t&that)
{
      പ്രതിജ്ഞാ(size_ == that.size_);

      എങ്കിൽ (size_ < BITS_PER_WORD) {
	    unsigned long mask = ~(-1UL << size_);
	    എങ്കിൽ ((bbits_val_|that.bbits_val_) & mask) {
		  abits_val_ |= mask;
		  bbits_val_ |= mask;
		  തിരികെ;
	    }

	    abits_val_ += that.abits_val_;
	    abits_val_ &= mask;
	    തിരികെ;
      }

      എങ്കിൽ (size_ == BITS_PER_WORD) {
	    എങ്കിൽ (bbits_val_ | that.bbits_val_) {
		  abits_val_ = WORD_X_ABITS;
		  bbits_val_ = WORD_X_BBITS;
	    } അല്ലെങ്കിൽ {
		  abits_val_ += that.abits_val_;
	    }
	    തിരികെ;
      }

      പൂർണ്ണസംഖ്യ cnt = size_ / BITS_PER_WORD;
      unsigned long carry = 0;
      വേണ്ടി (പൂർണ്ണസംഖ്യ idx = 0 ; idx < cnt ; idx += 1) {
	    എങ്കിൽ (bbits_ptr_[idx] | that.bbits_ptr_[idx])
		  പോകുക x_out;

	    abits_ptr_[idx] = add_with_carry(abits_ptr_[idx], that.abits_ptr_[idx], carry);
      }

      എങ്കിൽ (unsigned tail = size_ % BITS_PER_WORD) {
	    unsigned long mask = ~( -1UL << tail );
	    എങ്കിൽ ((bbits_ptr_[cnt] | that.bbits_ptr_[cnt])&mask)
		  പോകുക x_out;

	    abits_ptr_[cnt] = add_with_carry(abits_ptr_[cnt], that.abits_ptr_[cnt], carry);
	    abits_ptr_[cnt] &= mask;
      }

      തിരികെ;

 x_out:
      വേണ്ടി (പൂർണ്ണസംഖ്യ idx = 0 ; idx < cnt ; idx += 1) {
	    abits_ptr_[idx] = WORD_X_ABITS;
	    bbits_ptr_[idx] = WORD_X_BBITS;
      }
      എങ്കിൽ (unsigned tail = size_%BITS_PER_WORD) {
	    unsigned long mask = ~( -1UL << tail );
	    abits_ptr_[cnt] = WORD_X_ABITS&mask;
	    bbits_ptr_[cnt] = WORD_X_BBITS&mask;
      }
}

ശൂന്യം vvp_vector4_t::sub(സ്ഥിരം vvp_vector4_t&that)
{
      പ്രതിജ്ഞാ(size_ == that.size_);

      എങ്കിൽ (size_ < BITS_PER_WORD) {
	    unsigned long mask = ~(-1UL << size_);
	    എങ്കിൽ ((bbits_val_|that.bbits_val_) & mask) {
		  abits_val_ |= mask;
		  bbits_val_ |= mask;
		  തിരികെ;
	    }

	    abits_val_ -= that.abits_val_;
	    abits_val_ &= mask;
	    തിരികെ;
      }

      എങ്കിൽ (size_ == BITS_PER_WORD) {
	    എങ്കിൽ (bbits_val_ | that.bbits_val_) {
		  abits_val_ = WORD_X_ABITS;
		  bbits_val_ = WORD_X_BBITS;
	    } അല്ലെങ്കിൽ {
		  abits_val_ -= that.abits_val_;
	    }
	    തിരികെ;
      }

      പൂർണ്ണസംഖ്യ cnt = size_ / BITS_PER_WORD;
      unsigned long carry = 1;
      വേണ്ടി (പൂർണ്ണസംഖ്യ idx = 0 ; idx < cnt ; idx += 1) {
	    എങ്കിൽ (bbits_ptr_[idx] | that.bbits_ptr_[idx])
		  പോകുക x_out;

	    abits_ptr_[idx] = add_with_carry(abits_ptr_[idx], ~that.abits_ptr_[idx], carry);
      }

      എങ്കിൽ (unsigned tail = size_ % BITS_PER_WORD) {
	    unsigned long mask = ~( -1UL << tail );
	    എങ്കിൽ ((bbits_ptr_[cnt] | that.bbits_ptr_[cnt])&mask)
		  പോകുക x_out;

	    abits_ptr_[cnt] = add_with_carry(abits_ptr_[cnt], ~that.abits_ptr_[cnt], carry);
	    abits_ptr_[cnt] &= mask;
      }

      തിരികെ;

 x_out:
      വേണ്ടി (പൂർണ്ണസംഖ്യ idx = 0 ; idx < cnt ; idx += 1) {
	    abits_ptr_[idx] = WORD_X_ABITS;
	    bbits_ptr_[idx] = WORD_X_BBITS;
      }
      എങ്കിൽ (unsigned tail = size_%BITS_PER_WORD) {
	    unsigned long mask = ~( -1UL << tail );
	    abits_ptr_[cnt] = WORD_X_ABITS&mask;
	    bbits_ptr_[cnt] = WORD_X_BBITS&mask;
      }

}

ശൂന്യം vvp_vector4_t::mov(unsigned dst, unsigned src, unsigned cnt)
{
      പ്രതിജ്ഞാ(dst+cnt <= size_);
      പ്രതിജ്ഞാ(src+cnt <= size_);

      എങ്കിൽ (size_ <= BITS_PER_WORD) {
	    unsigned long vmask = (1UL << cnt) - 1;
	    unsigned long tmp;

	    tmp = (abits_val_ >> src) & vmask;
	    abits_val_ &= ~ (vmask << dst);
	    abits_val_ |= tmp << dst;

	    tmp = (bbits_val_ >> src) & vmask;
	    bbits_val_ &= ~ (vmask << dst);
	    bbits_val_ |= tmp << dst;

      } അല്ലെങ്കിൽ {
	    unsigned sptr = src / BITS_PER_WORD;
	    unsigned dptr = dst / BITS_PER_WORD;
	    unsigned soff = src % BITS_PER_WORD;
	    unsigned doff = dst % BITS_PER_WORD;

	    ആകുമ്പോൾ (cnt > 0) {
		  unsigned trans = cnt;
		  എങ്കിൽ ((soff+trans) > BITS_PER_WORD)
			trans = BITS_PER_WORD - soff;

		  എങ്കിൽ ((doff+trans) > BITS_PER_WORD)
			trans = BITS_PER_WORD - doff;

		  എങ്കിൽ (trans == BITS_PER_WORD) {
			  // Special case: the transfer count is
			  // exactly an entire word. For this to be
			  // true, it must also be true that the
			  // pointers are aligned. The work is easy,
			abits_ptr_[dptr] = abits_ptr_[sptr];
			bbits_ptr_[dptr] = bbits_ptr_[sptr];
			dptr += 1;
			sptr += 1;
			cnt -= BITS_PER_WORD;
			തുടരുക;
		  }

		    // Here we know that either the source or
		    // destination is unaligned, and also we know that
		    // the count is less than a full word.
		  unsigned long vmask = (1UL << trans) - 1;
		  unsigned long tmp;

		  tmp = (abits_ptr_[sptr] >> soff) & vmask;
		  abits_ptr_[dptr] &= ~ (vmask << doff);
		  abits_ptr_[dptr] |= tmp << doff;

		  tmp = (bbits_ptr_[sptr] >> soff) & vmask;
		  bbits_ptr_[dptr] &= ~ (vmask << doff);
		  bbits_ptr_[dptr] |= tmp << doff;

		  cnt -= trans;
		  soff += trans;
		  എങ്കിൽ (soff >= BITS_PER_WORD) {
			soff = 0;
			sptr += 1;
		  }
		  doff += trans;
		  എങ്കിൽ (doff >= BITS_PER_WORD) {
			doff = 0;
			dptr += 1;
		  }
	    }
      }
}

ശൂന്യം vvp_vector4_t::mul(സ്ഥിരം vvp_vector4_t&that)
{
      പ്രതിജ്ഞാ(size_ == that.size_);

      എങ്കിൽ (size_ < BITS_PER_WORD) {
	    unsigned long mask = ~(-1UL << size_);
	    എങ്കിൽ ((bbits_val_|that.bbits_val_) & mask) {
		  abits_val_ |= mask;
		  bbits_val_ |= mask;
		  തിരികെ;
	    }

	    abits_val_ *= that.abits_val_;
	    abits_val_ &= mask;
	    തിരികെ;
      }

      എങ്കിൽ (size_ == BITS_PER_WORD) {
	    എങ്കിൽ (bbits_val_ || that.bbits_val_) {
		  abits_val_ = WORD_X_ABITS;
		  bbits_val_ = WORD_X_BBITS;
	    } അല്ലെങ്കിൽ {
		  abits_val_ *= that.abits_val_;
	    }
	    തിരികെ;
      }

      സ്ഥിരം പൂർണ്ണസംഖ്യ cnt = (size_+BITS_PER_WORD-1) / BITS_PER_WORD;

      unsigned long mask;
      എങ്കിൽ (unsigned tail = size_%BITS_PER_WORD) {
	    mask = ~( -1UL << tail );
      } അല്ലെങ്കിൽ {
	    mask = ~0UL;
      }

	// Check for any XZ values ahead of time in a first pass. If
	// we find any, then force the entire result to be X and be
	// done.
      വേണ്ടി (പൂർണ്ണസംഖ്യ idx = 0 ; idx < cnt ; idx += 1) {
	    unsigned long lval = bbits_ptr_[idx];
	    unsigned long rval = that.bbits_ptr_[idx];
	    എങ്കിൽ (idx == (cnt-1)) {
		  lval &= mask;
		  rval &= mask;
	    }
	    എങ്കിൽ (lval || rval) {
		  വേണ്ടി (പൂർണ്ണസംഖ്യ xdx = 0 ; xdx < cnt-1 ; xdx += 1) {
			abits_ptr_[xdx] = WORD_X_ABITS;
			bbits_ptr_[xdx] = WORD_X_BBITS;
		  }
		  abits_ptr_[cnt-1] = WORD_X_ABITS & mask;
		  bbits_ptr_[cnt-1] = WORD_X_BBITS & mask;
		  തിരികെ;
	    }
      }

	// Calculate the result into a res array. We need to keep is
	// separate from the "this" array because we are making
	// multiple passes.
      unsigned long*res = പുതിയ unsigned long[cnt];
      വേണ്ടി (പൂർണ്ണസംഖ്യ idx = 0 ; idx < cnt ; idx += 1)
	    res[idx] = 0;

      വേണ്ടി (പൂർണ്ണസംഖ്യ mul_a = 0 ; mul_a < cnt ; mul_a += 1) {
	    unsigned long lval = abits_ptr_[mul_a];
	    എങ്കിൽ (mul_a == (cnt-1))
		  lval &= mask;

	    വേണ്ടി (പൂർണ്ണസംഖ്യ mul_b = 0 ; mul_b < (cnt-mul_a) ; mul_b += 1) {
		  unsigned long rval = that.abits_ptr_[mul_b];
		  എങ്കിൽ (mul_b == (cnt-1))
			rval &= mask;

		  unsigned long sum;
		  unsigned long tmp = multiply_with_carry(lval, rval, sum);
		  പൂർണ്ണസംഖ്യ base = mul_a + mul_b;
		  unsigned long carry = 0;
		  res[base] = add_with_carry(res[base], tmp, carry);
		  വേണ്ടി (പൂർണ്ണസംഖ്യ add_idx = base+1 ; add_idx < cnt ; add_idx += 1) {
			res[add_idx] = add_with_carry(res[add_idx], sum, carry);
			sum = 0;
		  }
	    }
      }

	// Replace the "this" value with the calculated result. We
	// know a-priori that the bbits are zero and unchanged.
      res[cnt-1] &= mask;
      വേണ്ടി (പൂർണ്ണസംഖ്യ idx = 0 ; idx < cnt ; idx += 1)
	    abits_ptr_[idx] = res[idx];

      delete[]res;
      തിരികെ;


}

തർക്കം vvp_vector4_t::eeq(സ്ഥിരം vvp_vector4_t&that) സ്ഥിരം
{
      എങ്കിൽ (size_ != that.size_)
	    തിരികെ അസത്യം;

      എങ്കിൽ (size_ < BITS_PER_WORD) {
	    unsigned long mask = (1UL << size_) - 1;
	    തിരികെ (abits_val_&mask) == (that.abits_val_&mask)
		  && (bbits_val_&mask) == (that.bbits_val_&mask);
      }

      എങ്കിൽ (size_ == BITS_PER_WORD) {
	    തിരികെ (abits_val_ == that.abits_val_)
		  && (bbits_val_ == that.bbits_val_);
      }

      unsigned വാക്കുകൾ = size_ / BITS_PER_WORD;
      വേണ്ടി (unsigned idx = 0 ;  idx < വാക്കുകൾ ;  idx += 1) {
	    എങ്കിൽ (abits_ptr_[idx] != that.abits_ptr_[idx])
		  തിരികെ അസത്യം;
	    എങ്കിൽ (bbits_ptr_[idx] != that.bbits_ptr_[idx])
		  തിരികെ അസത്യം;
      }

      unsigned long mask = size_%BITS_PER_WORD;
      എങ്കിൽ (mask > 0) {
	    mask = (1UL << mask) - 1;
	    തിരികെ (abits_ptr_[വാക്കുകൾ]&mask) == (that.abits_ptr_[വാക്കുകൾ]&mask)
		  && (bbits_ptr_[വാക്കുകൾ]&mask) == (that.bbits_ptr_[വാക്കുകൾ]&mask);
      }

      തിരികെ സത്യം;
}

തർക്കം vvp_vector4_t::eq_xz(സ്ഥിരം vvp_vector4_t&that) സ്ഥിരം
{
      എങ്കിൽ (size_ != that.size_)
	    തിരികെ അസത്യം;

      എങ്കിൽ (size_ < BITS_PER_WORD) {
	    unsigned long mask = (1UL << size_) - 1;
	    തിരികെ ((abits_val_|bbits_val_)&mask) == ((that.abits_val_|that.bbits_val_)&mask)
		  && (bbits_val_&mask) == (that.bbits_val_&mask);
      }

      എങ്കിൽ (size_ == BITS_PER_WORD) {
	    തിരികെ ((abits_val_|bbits_val_) == (that.abits_val_|that.bbits_val_))
		  && (bbits_val_ == that.bbits_val_);
      }

      unsigned വാക്കുകൾ = size_ / BITS_PER_WORD;
      വേണ്ടി (unsigned idx = 0 ;  idx < വാക്കുകൾ ;  idx += 1) {
	    എങ്കിൽ ((abits_ptr_[idx]|bbits_ptr_[idx]) != (that.abits_ptr_[idx]|that.bbits_ptr_[idx]))
		  തിരികെ അസത്യം;
	    എങ്കിൽ (bbits_ptr_[idx] != that.bbits_ptr_[idx])
		  തിരികെ അസത്യം;
      }

      unsigned long mask = size_%BITS_PER_WORD;
      എങ്കിൽ (mask > 0) {
	    mask = (1UL << mask) - 1;
	    തിരികെ ((abits_ptr_[വാക്കുകൾ]|bbits_ptr_[വാക്കുകൾ])&mask) == ((that.abits_ptr_[വാക്കുകൾ]|that.bbits_ptr_[വാക്കുകൾ])&mask)
		  && (bbits_ptr_[വാക്കുകൾ]&mask) == (that.bbits_ptr_[വാക്കുകൾ]&mask);
      }

      തിരികെ സത്യം;
}

തർക്കം vvp_vector4_t::has_xz() സ്ഥിരം
{
      എങ്കിൽ (size_ < BITS_PER_WORD) {
	    unsigned long mask = -1UL >> (BITS_PER_WORD - size_);
	    തിരികെ bbits_val_&mask;
      }

      എങ്കിൽ (size_ == BITS_PER_WORD) {
	    തിരികെ bbits_val_;
      }

      unsigned വാക്കുകൾ = size_ / BITS_PER_WORD;
      വേണ്ടി (unsigned idx = 0 ; idx < വാക്കുകൾ ; idx += 1) {
	    എങ്കിൽ (bbits_ptr_[idx])
		  തിരികെ സത്യം;
      }

      unsigned long mask = size_%BITS_PER_WORD;
      എങ്കിൽ (mask > 0) {
	    mask = -1UL >> (BITS_PER_WORD - mask);
	    തിരികെ bbits_ptr_[വാക്കുകൾ]&mask;
      }

      തിരികെ അസത്യം;
}

ശൂന്യം vvp_vector4_t::change_z2x()
{
	// This method relies on the fact that both BIT4_X and BIT4_Z
	// have the bbit set in the vector4 encoding, and also that
	// the BIT4_X has abit set in the vector4 encoding. By simply
	// or-ing the bbit into the abit, BIT4_X and BIT4_Z both
	// become BIT4_X.

      എങ്കിൽ (size_ <= BITS_PER_WORD) {
	    abits_val_ |= bbits_val_;
      } അല്ലെങ്കിൽ {
	    unsigned വാക്കുകൾ = (size_+BITS_PER_WORD-1) / BITS_PER_WORD;
	    വേണ്ടി (unsigned idx = 0 ;  idx < വാക്കുകൾ ;  idx += 1)
		  abits_ptr_[idx] |= bbits_ptr_[idx];
      }
}

ശൂന്യം vvp_vector4_t::fill_bits(vvp_bit4_t bit)
{
	/* note: this relies on the bit encoding for the vvp_bit4_t. */
      സ്ഥിതം സ്ഥിരം unsigned long init_atable[4] = {
	    WORD_0_ABITS,
	    WORD_1_ABITS,
	    WORD_Z_ABITS,
	    WORD_X_ABITS };
      സ്ഥിതം സ്ഥിരം unsigned long init_btable[4] = {
	    WORD_0_BBITS,
	    WORD_1_BBITS,
	    WORD_Z_BBITS,
	    WORD_X_BBITS };

      എങ്കിൽ (size_ <= BITS_PER_WORD) {
	    abits_val_ = init_atable[bit];
	    bbits_val_ = init_btable[bit];
      } അല്ലെങ്കിൽ {
	    unsigned വാക്കുകൾ = (size_ + BITS_PER_WORD - 1) / BITS_PER_WORD;
	    വേണ്ടി (unsigned idx = 0; idx < വാക്കുകൾ; idx += 1) {
		  abits_ptr_[idx] = init_atable[bit];
		  bbits_ptr_[idx] = init_btable[bit];
	    }
      }
}

അക്ഷരം* vvp_vector4_t::as_string(അക്ഷരം*buf, size_t buf_len) സ്ഥിരം
{
      അക്ഷരം*res = buf;
      *buf++ = 'C';
      *buf++ = '4';
      *buf++ = '<';
      buf_len -= 3;

      വേണ്ടി (unsigned idx = 0 ;  idx < size() && buf_len >= 2 ;  idx += 1) {
	    തിരഞ്ഞെടുപ്പ് (value(size()-idx-1)) {
		അവസ്ഥ BIT4_0:
		  *buf++ = '0';
		  നിർത്തുക;
		അവസ്ഥ BIT4_1:
		  *buf++ = '1';
		  നിർത്തുക;
		അവസ്ഥ BIT4_X:
		  *buf++ = 'x';
		  നിർത്തുക;
		അവസ്ഥ BIT4_Z:
		  *buf++ = 'z';
	    }
	    buf_len -= 1;
      }

      *buf++ = '>';
      *buf++ = 0;
      തിരികെ res;
}

ശൂന്യം vvp_vector4_t::invert()
{
      എങ്കിൽ (size_ <= BITS_PER_WORD) {
	    unsigned long mask = (size_<BITS_PER_WORD)? (1UL<<size_)-1UL : -1UL;
	    abits_val_ = mask & ~abits_val_;
	    abits_val_ |= bbits_val_;
      } അല്ലെങ്കിൽ {
	    unsigned remaining = size_;
	    unsigned idx = 0;
	    ആകുമ്പോൾ (remaining >= BITS_PER_WORD) {
		  abits_ptr_[idx] = ~abits_ptr_[idx];
		  abits_ptr_[idx] |= bbits_ptr_[idx];
		  idx += 1;
		  remaining -= BITS_PER_WORD;
	    }
	    എങ്കിൽ (remaining > 0) {
		  unsigned long mask = (1UL<<remaining) - 1UL;
		  abits_ptr_[idx] = mask & ~abits_ptr_[idx];
		  abits_ptr_[idx] |= bbits_ptr_[idx];
	    }
      }
}

#define BIT_MASK(n) ((n) ? ((~0UL) >> (BITS_PER_WORD - (n))) : ~0UL)

vvp_bit4_t vvp_vector4_t::reduce_or() സ്ഥിരം
{
      unsigned long mask = BIT_MASK(size_ % BITS_PER_WORD);
      vvp_bit4_t res = BIT4_0;

      എങ്കിൽ (size_ <= BITS_PER_WORD) {
	    എങ്കിൽ ((abits_val_ & ~bbits_val_ & mask) != 0UL)
		  തിരികെ BIT4_1;
	    എങ്കിൽ ((bbits_val_ & mask) != 0UL)
		  തിരികെ BIT4_X;
      } അല്ലെങ്കിൽ {
	    unsigned വാക്കുകൾ = (size_ + BITS_PER_WORD - 1) / BITS_PER_WORD;
	    unsigned idx;
	    വേണ്ടി (idx = 0; idx < വാക്കുകൾ - 1; idx += 1) {
		  എങ്കിൽ ((abits_ptr_[idx] & ~bbits_ptr_[idx]) != 0UL)
			തിരികെ BIT4_1;
		  എങ്കിൽ (bbits_ptr_[idx] != 0UL)
			res = BIT4_X;
	    }
	    എങ്കിൽ ((abits_ptr_[idx] & ~bbits_ptr_[idx] & mask) != 0UL)
		  തിരികെ BIT4_1;
	    എങ്കിൽ ((bbits_ptr_[idx] & mask) != 0UL)
		  res = BIT4_X;
      }

      തിരികെ res;
}

vvp_bit4_t vvp_vector4_t::reduce_and() സ്ഥിരം
{
      unsigned long mask = BIT_MASK(size_ % BITS_PER_WORD);
      vvp_bit4_t res = BIT4_1;

      എങ്കിൽ (size_ <= BITS_PER_WORD) {
	    എങ്കിൽ ((abits_val_ | bbits_val_ | ~mask) != ~0UL)
		  തിരികെ BIT4_0;
	    എങ്കിൽ ((bbits_val_ & mask) != 0UL)
		  തിരികെ BIT4_X;
	} അല്ലെങ്കിൽ {
	    unsigned വാക്കുകൾ = (size_ + BITS_PER_WORD - 1) / BITS_PER_WORD;
	    unsigned idx;
	    വേണ്ടി (idx = 0; idx < വാക്കുകൾ - 1; idx += 1) {
		  എങ്കിൽ ((abits_ptr_[idx] | bbits_ptr_[idx]) != ~0UL)
			തിരികെ BIT4_0;
		  എങ്കിൽ (bbits_ptr_[idx] != 0UL)
			res = BIT4_X;
	    }
	    എങ്കിൽ ((abits_ptr_[idx] | bbits_ptr_[idx] | ~mask) != ~0UL)
		  തിരികെ BIT4_0;
	    എങ്കിൽ ((bbits_ptr_[idx] & mask) != 0UL)
		  res = BIT4_X;
      }

      തിരികെ res;
}

സ്ഥിതം unsigned long parity(unsigned long val)
{
#എങ്കിൽ defined(__GNUC__)
      // The compiler builtin can use target-specific CPU instructions.
      തിരികെ __builtin_parityl(val);
#അല്ലെങ്കിൽ
#എങ്കിൽ ULONG_MAX > 0xffffffffUL
	val ^= val >> 32;
#endif
	val ^= val >> 16;
	val ^= val >> 8;
	val ^= val >> 4;

      തിരികെ (0x6996 >> (val & 0xf)) & 1;
#endif
}

vvp_bit4_t vvp_vector4_t::reduce_xor() സ്ഥിരം
{
      unsigned long mask = BIT_MASK(size_ % BITS_PER_WORD);

      എങ്കിൽ (size_ <= BITS_PER_WORD) {
	    എങ്കിൽ ((bbits_val_ & mask) != 0UL)
		  തിരികെ BIT4_X;
	    തിരികെ parity(abits_val_ & mask) ? BIT4_1 : BIT4_0;
      } അല്ലെങ്കിൽ {
	    unsigned വാക്കുകൾ = (size_ + BITS_PER_WORD - 1) / BITS_PER_WORD;
	    unsigned long val_a = 0UL;
	    unsigned idx;
	    വേണ്ടി (idx = 0; idx < വാക്കുകൾ - 1; idx += 1) {
		  എങ്കിൽ (bbits_ptr_[idx] != 0UL)
			തിരികെ BIT4_X;
		  val_a ^= abits_ptr_[idx];
	    }
	    എങ്കിൽ ((bbits_ptr_[idx] & mask) != 0UL)
		  തിരികെ BIT4_X;
	    val_a ^= abits_ptr_[idx] & mask;
	    തിരികെ parity(val_a) ? BIT4_1 : BIT4_0;
      }
}

vvp_vector4_t& vvp_vector4_t::operator &= (സ്ഥിരം vvp_vector4_t&that)
{
	// The truth table is:
	//     00 01 11 10
	//  00 00 00 00 00
	//  01 00 01 11 11
	//  11 00 11 11 11
	//  10 00 11 11 11
      എങ്കിൽ (size_ <= BITS_PER_WORD) {
	    unsigned long tmp1 = abits_val_ | bbits_val_;
	    unsigned long tmp2 = that.abits_val_ | that.bbits_val_;
	    abits_val_ = tmp1 & tmp2;
	    bbits_val_ = (tmp1 & that.bbits_val_) | (tmp2 & bbits_val_);
      } അല്ലെങ്കിൽ {
	    unsigned വാക്കുകൾ = (size_ + BITS_PER_WORD - 1) / BITS_PER_WORD;
	    വേണ്ടി (unsigned idx = 0; idx < വാക്കുകൾ ; idx += 1) {
		  unsigned long tmp1 = abits_ptr_[idx] | bbits_ptr_[idx];
		  unsigned long tmp2 = that.abits_ptr_[idx] |
		                       that.bbits_ptr_[idx];
		  abits_ptr_[idx] = tmp1 & tmp2;
		  bbits_ptr_[idx] = (tmp1 & that.bbits_ptr_[idx]) |
		                    (tmp2 & bbits_ptr_[idx]);
	    }
      }

      തിരികെ *ഇത്;
}

vvp_vector4_t& vvp_vector4_t::operator |= (സ്ഥിരം vvp_vector4_t&that)
{
	// The truth table is:
	//     00 01 11 10
	//  00 00 01 11 11
	//  01 01 01 01 01
	//  11 11 01 11 11
	//  10 11 01 11 11
      എങ്കിൽ (size_ <= BITS_PER_WORD) {
	    unsigned long tmp = abits_val_ | bbits_val_ |
	                        that.abits_val_ | that.bbits_val_;
	    bbits_val_ = ((~abits_val_ | bbits_val_) & that.bbits_val_) |
	                 ((~that.abits_val_ | that.bbits_val_) & bbits_val_);
	    abits_val_ = tmp;

      } അല്ലെങ്കിൽ {
	    unsigned വാക്കുകൾ = (size_ + BITS_PER_WORD - 1) / BITS_PER_WORD;
	    വേണ്ടി (unsigned idx = 0; idx < വാക്കുകൾ ; idx += 1) {
		  unsigned long tmp = abits_ptr_[idx] | bbits_ptr_[idx] |
	                        that.abits_ptr_[idx] | that.bbits_ptr_[idx];
		  bbits_ptr_[idx] = ((~abits_ptr_[idx] | bbits_ptr_[idx]) &
		                     that.bbits_ptr_[idx]) |
		                    ((~that.abits_ptr_[idx] |
		                      that.bbits_ptr_[idx]) & bbits_ptr_[idx]);
		  abits_ptr_[idx] = tmp;
	    }
      }

      തിരികെ *ഇത്;
}

vvp_vector4_t& vvp_vector4_t::operator ^= (സ്ഥിരം vvp_vector4_t&that)
{
	// The truth table is:
	//     00 01 11 10
	//  00 00 01 11 11
	//  01 01 00 11 11
	//  11 11 11 11 11
	//  10 11 11 11 11
      എങ്കിൽ (size_ <= BITS_PER_WORD) {
	    unsigned long bval = bbits_val_ | that.bbits_val_;
	    bbits_val_ = bval;
	    abits_val_ = (abits_val_ ^ that.abits_val_) | bval;
      } അല്ലെങ്കിൽ {
	    unsigned വാക്കുകൾ = (size_ + BITS_PER_WORD - 1) / BITS_PER_WORD;
	    വേണ്ടി (unsigned idx = 0; idx < വാക്കുകൾ ; idx += 1) {
		  unsigned long bval = bbits_ptr_[idx] | that.bbits_ptr_[idx];
		  bbits_ptr_[idx] = bval;
		  abits_ptr_[idx] = (abits_ptr_[idx] ^ that.abits_ptr_[idx]) | bval;
	    }
      }

      തിരികെ *ഇത്;
}

/*
* Add an integer to the vvp_vector4_t in place, bit by bit so that
* there is no size limitations.
*/
vvp_vector4_t& vvp_vector4_t::operator += (int64_t that)
{
      vvp_bit4_t carry = BIT4_0;
      unsigned idx;

      എങ്കിൽ (has_xz()) {
	    vvp_vector4_t xxx (size(), BIT4_X);
	    *ഇത് = xxx;
	    തിരികെ *ഇത്;
      }

      വേണ്ടി (idx = 0 ; idx < size() ; idx += 1) {
	    എങ്കിൽ (that == 0 && carry==BIT4_0)
		  നിർത്തുക;

	    vvp_bit4_t that_bit = (that&1)? BIT4_1 : BIT4_0;
	    that >>= 1;

	    എങ്കിൽ (that_bit==BIT4_0 && carry==BIT4_0)
		  തുടരുക;

	    vvp_bit4_t bit = value(idx);
	    bit = add_with_carry(bit, that_bit, carry);

	    set_bit(idx, bit);
      }

      തിരികെ *ഇത്;
}

ostream& operator<< (ostream&out, സ്ഥിരം vvp_vector4_t&that)
{
      out << that.size() << "'b";
      വേണ്ടി (unsigned idx = 0 ;  idx < that.size() ;  idx += 1)
	    out << that.value(that.size()-idx-1);
      തിരികെ out;
}

template <വർഗ്ഗം INT>തർക്കം vector4_to_മൂല്യം(സ്ഥിരം vvp_vector4_t&vec, INT&val,
					  തർക്കം is_signed, തർക്കം is_arithmetic)
{
      INT res = 0;
      INT msk = 1;
      തർക്കം rc_flag = സത്യം;

      unsigned size = vec.size();
      എങ്കിൽ (size > 8*വലുപ്പം(val)) size = 8*വലുപ്പം(val);
      വേണ്ടി (unsigned idx = 0 ;  idx < size ;  idx += 1) {
	    തിരഞ്ഞെടുപ്പ് (vec.value(idx)) {
		അവസ്ഥ BIT4_0:
		  നിർത്തുക;
		അവസ്ഥ BIT4_1:
		  res |= msk;
		  നിർത്തുക;
		സ്ഥിരസ്ഥിതി:
		  എങ്കിൽ (is_arithmetic)
			തിരികെ അസത്യം;
		  അല്ലെങ്കിൽ
			rc_flag = അസത്യം;
	    }

	    msk <<= 1;
      }

      എങ്കിൽ (is_signed && vec.value(vec.size()-1) == BIT4_1) {
	    എങ്കിൽ (vec.size() < 8*വലുപ്പം(val))
		  res |= static_cast<INT>(-1ULL << vec.size());
      }

      val = res;
      തിരികെ rc_flag;
}

template തർക്കം vector4_to_മൂല്യം(സ്ഥിരം vvp_vector4_t&vec, int8_t&val,
			       തർക്കം is_signed, തർക്കം is_arithmetic);
template തർക്കം vector4_to_മൂല്യം(സ്ഥിരം vvp_vector4_t&vec, int16_t&val,
			       തർക്കം is_signed, തർക്കം is_arithmetic);
template തർക്കം vector4_to_മൂല്യം(സ്ഥിരം vvp_vector4_t&vec, int32_t&val,
			       തർക്കം is_signed, തർക്കം is_arithmetic);
template തർക്കം vector4_to_മൂല്യം(സ്ഥിരം vvp_vector4_t&vec, int64_t&val,
			       തർക്കം is_signed, തർക്കം is_arithmetic);
template തർക്കം vector4_to_മൂല്യം(സ്ഥിരം vvp_vector4_t&vec, uint8_t&val,
			       തർക്കം is_signed, തർക്കം is_arithmetic);
template തർക്കം vector4_to_മൂല്യം(സ്ഥിരം vvp_vector4_t&vec, uint16_t&val,
			       തർക്കം is_signed, തർക്കം is_arithmetic);
template തർക്കം vector4_to_മൂല്യം(സ്ഥിരം vvp_vector4_t&vec, uint32_t&val,
			       തർക്കം is_signed, തർക്കം is_arithmetic);
template തർക്കം vector4_to_മൂല്യം(സ്ഥിരം vvp_vector4_t&vec, uint64_t&val,
			       തർക്കം is_signed, തർക്കം is_arithmetic);

template <വർഗ്ഗം T> തർക്കം vector4_to_മൂല്യം(സ്ഥിരം vvp_vector4_t&vec,
                                         തർക്കം&overflow_flag, T&val)
{
      T res = 0;
      T msk = 1;

      overflow_flag = അസത്യം;
      unsigned size = vec.size();
      വേണ്ടി (unsigned idx = 0 ;  idx < size ;  idx += 1) {
	    തിരഞ്ഞെടുപ്പ് (vec.value(idx)) {
		അവസ്ഥ BIT4_0:
		  നിർത്തുക;
		അവസ്ഥ BIT4_1:
		  എങ്കിൽ (msk == 0)
			overflow_flag = സത്യം;
		  അല്ലെങ്കിൽ
			res |= msk;
		  നിർത്തുക;
		സ്ഥിരസ്ഥിതി:
		  തിരികെ അസത്യം;
	    }

	    msk <<= static_cast<T>(1);
      }

      val = res;
      തിരികെ സത്യം;
}

template തർക്കം vector4_to_മൂല്യം(സ്ഥിരം vvp_vector4_t&vec, തർക്കം&overflow_flag,
                               unsigned long&val);
#ifndef UL_AND_TIME64_SAME
template തർക്കം vector4_to_മൂല്യം(സ്ഥിരം vvp_vector4_t&vec, തർക്കം&overflow_flag,
                               vvp_time64_t&val);
#endif

തർക്കം vector4_to_മൂല്യം(സ്ഥിരം vvp_vector4_t&vec, ഇരട്ട&val, തർക്കം signed_flag)
{

      എങ്കിൽ (vec.size() == 0) {
	    val = 0.0;
	    തിരികെ സത്യം;
      }

      തർക്കം flag = സത്യം;

      എങ്കിൽ (vec.value(vec.size()-1) != BIT4_1) {
	    signed_flag = അസത്യം;
      }

      ഇരട്ട res = 0.0;
      എങ്കിൽ (signed_flag) {
	    vvp_bit4_t carry = BIT4_1;
	    വേണ്ടി (unsigned idx = 0 ;  idx < vec.size() ;  idx += 1) {
		  vvp_bit4_t a = ~vec.value(idx);
		  vvp_bit4_t x = add_with_carry(a, BIT4_0, carry);
		  തിരഞ്ഞെടുപ്പ് (x) {
		      അവസ്ഥ BIT4_0:
			നിർത്തുക;
		      അവസ്ഥ BIT4_1:
			res += pow(2.0, (പൂർണ്ണസംഖ്യ)idx);
			നിർത്തുക;
		      സ്ഥിരസ്ഥിതി:
			flag = അസത്യം;
		  }
	    }
	    res *= -1.0;
      } അല്ലെങ്കിൽ {
	    വേണ്ടി (unsigned idx = 0 ;  idx < vec.size() ;  idx += 1) {
		  തിരഞ്ഞെടുപ്പ് (vec.value(idx)) {
		      അവസ്ഥ BIT4_0:
			നിർത്തുക;
		      അവസ്ഥ BIT4_1:
			res += pow(2.0, (പൂർണ്ണസംഖ്യ)idx);
			നിർത്തുക;
		      സ്ഥിരസ്ഥിതി:
			flag = അസത്യം;
		  }
	    }
      }
      val = res;
      തിരികെ flag;
}

തർക്കം vector2_to_മൂല്യം(സ്ഥിരം vvp_vector2_t&a, int32_t&val, തർക്കം is_signed)
{
      val = 0;
      unsigned idx;
      uint32_t mask;
      വേണ്ടി (idx = 0, mask = 1 ; idx < a.size() && idx < 32 ; idx += 1, mask <<= 1) {
	    എങ്കിൽ (a.value(idx)) val |= mask;
      }

      എങ്കിൽ (is_signed && a.size() < 32 && a.value(a.size()-1)) {
	    mask = -1;
	    mask <<= a.size();
	    val |= mask;
      }

      തിരികെ a.size() <= 32;
}

vvp_vector4array_t::vvp_vector4array_t(unsigned width__, unsigned വാക്കുകൾ_2)
: width_(width__), വാക്കുകൾ_3(വാക്കുകൾ_2)
{
}

vvp_vector4array_t::~vvp_vector4array_t()
{
}

ശൂന്യം vvp_vector4array_t::set_word_(v4cell*cell, സ്ഥിരം vvp_vector4_t&that)
{
      പ്രതിജ്ഞാ(that.size_ == width_);

      എങ്കിൽ (width_ <= vvp_vector4_t::BITS_PER_WORD) {
	    cell->abits_val_ = that.abits_val_;
	    cell->bbits_val_ = that.bbits_val_;
	    തിരികെ;
      }

      unsigned cnt = (width_ + vvp_vector4_t::BITS_PER_WORD-1)/vvp_vector4_t::BITS_PER_WORD;

      എങ്കിൽ (cell->abits_ptr_ == 0) {
	    cell->abits_ptr_ = പുതിയ unsigned long[2*cnt];
	    cell->bbits_ptr_ = cell->abits_ptr_ + cnt;
      }

      വേണ്ടി (unsigned idx = 0 ; idx < cnt ; idx += 1)
	    cell->abits_ptr_[idx] = that.abits_ptr_[idx];
      വേണ്ടി (unsigned idx = 0 ; idx < cnt ; idx += 1)
	    cell->bbits_ptr_[idx] = that.bbits_ptr_[idx];
}

vvp_vector4_t vvp_vector4array_t::get_word_(v4cell*cell) സ്ഥിരം
{
      എങ്കിൽ (width_ <= vvp_vector4_t::BITS_PER_WORD) {
	    vvp_vector4_t res;
	    res.size_ = width_;
	    res.abits_val_ = cell->abits_val_;
	    res.bbits_val_ = cell->bbits_val_;
	    തിരികെ res;
      }

      vvp_vector4_t res (width_, BIT4_X);
      എങ്കിൽ (cell->abits_ptr_ == 0)
	    തിരികെ res;

      unsigned cnt = (width_ + vvp_vector4_t::BITS_PER_WORD-1)/vvp_vector4_t::BITS_PER_WORD;

      വേണ്ടി (unsigned idx = 0 ; idx < cnt ; idx += 1)
	    res.abits_ptr_[idx] = cell->abits_ptr_[idx];
      വേണ്ടി (unsigned idx = 0 ; idx < cnt ; idx += 1)
	    res.bbits_ptr_[idx] = cell->bbits_ptr_[idx];

      തിരികെ res;
}

vvp_vector4array_sa::vvp_vector4array_sa(unsigned width__, unsigned വാക്കുകൾ_2)
: vvp_vector4array_t(width__, വാക്കുകൾ_2)
{
      array_ = പുതിയ v4cell[വാക്കുകൾ_3];

      എങ്കിൽ (width_ <= vvp_vector4_t::BITS_PER_WORD) {
	    വേണ്ടി (unsigned idx = 0 ; idx < വാക്കുകൾ_3 ; idx += 1) {
		  array_[idx].abits_val_ = vvp_vector4_t::WORD_X_ABITS;
		  array_[idx].bbits_val_ = vvp_vector4_t::WORD_X_BBITS;
	    }
      } അല്ലെങ്കിൽ {
	    വേണ്ടി (unsigned idx = 0 ; idx < വാക്കുകൾ_3 ; idx += 1) {
		  array_[idx].abits_ptr_ = 0;
		  array_[idx].bbits_ptr_ = 0;
	    }
      }
}

vvp_vector4array_sa::~vvp_vector4array_sa()
{
      എങ്കിൽ (array_) {
	    എങ്കിൽ (width_ > vvp_vector4_t::BITS_PER_WORD) {
		  വേണ്ടി (unsigned idx = 0 ; idx < വാക്കുകൾ_3 ; idx += 1)
			എങ്കിൽ (array_[idx].abits_ptr_)
			      delete[]array_[idx].abits_ptr_;
	    }
	    delete[]array_;
      }
}

ശൂന്യം vvp_vector4array_sa::set_word(unsigned index, സ്ഥിരം vvp_vector4_t&that)
{
      പ്രതിജ്ഞാ(index < വാക്കുകൾ_3);

      v4cell*cell = &array_[index];

      set_word_(cell, that);
}

vvp_vector4_t vvp_vector4array_sa::get_word(unsigned index) സ്ഥിരം
{
      എങ്കിൽ (index >= വാക്കുകൾ_3)
	    തിരികെ vvp_vector4_t(width_, BIT4_X);

      പ്രതിജ്ഞാ(index < വാക്കുകൾ_3);

      v4cell*cell = &array_[index];

      തിരികെ get_word_(cell);
}

vvp_vector4array_aa::vvp_vector4array_aa(unsigned width__, unsigned വാക്കുകൾ_2)
: vvp_vector4array_t(width__, വാക്കുകൾ_2)
{
      context_idx_ = vpip_add_item_to_context(ഇത്, vpip_peek_context_scope());
}

vvp_vector4array_aa::~vvp_vector4array_aa()
{
}

ശൂന്യം vvp_vector4array_aa::alloc_instance(vvp_context_t context)
{
      v4cell*array = പുതിയ v4cell[വാക്കുകൾ_3];

      എങ്കിൽ (width_ <= vvp_vector4_t::BITS_PER_WORD) {
	    വേണ്ടി (unsigned idx = 0 ; idx < വാക്കുകൾ_3 ; idx += 1) {
		  array[idx].abits_val_ = vvp_vector4_t::WORD_X_ABITS;
		  array[idx].bbits_val_ = vvp_vector4_t::WORD_X_BBITS;
	    }
      } അല്ലെങ്കിൽ {
	    വേണ്ടി (unsigned idx = 0 ; idx < വാക്കുകൾ_3 ; idx += 1) {
		  array[idx].abits_ptr_ = 0;
		  array[idx].bbits_ptr_ = 0;
	    }
      }

      vvp_set_context_item(context, context_idx_, array);
}

ശൂന്യം vvp_vector4array_aa::reset_instance(vvp_context_t context)
{
      v4cell*cell = static_cast<v4cell*>
            (vvp_get_context_item(context, context_idx_));

      എങ്കിൽ (width_ <= vvp_vector4_t::BITS_PER_WORD) {
	    വേണ്ടി (unsigned idx = 0 ; idx < വാക്കുകൾ_3 ; idx += 1) {
		  cell->abits_val_ = vvp_vector4_t::WORD_X_ABITS;
		  cell->bbits_val_ = vvp_vector4_t::WORD_X_BBITS;
                  cell++;
	    }
      } അല്ലെങ്കിൽ {
            unsigned cnt = (width_ + vvp_vector4_t::BITS_PER_WORD-1)/vvp_vector4_t::BITS_PER_WORD;
	    വേണ്ടി (unsigned idx = 0 ; idx < വാക്കുകൾ_3 ; idx += 1) {
		  എങ്കിൽ (cell->abits_ptr_) {
                        വേണ്ടി (unsigned n = 0 ; n < cnt ; n += 1) {
			      cell->abits_ptr_[n] = vvp_vector4_t::WORD_X_ABITS;
			      cell->bbits_ptr_[n] = vvp_vector4_t::WORD_X_BBITS;
                        }
                  }
                  cell++;
	    }
      }
}

#ifdef CHECK_WITH_VALGRIND
ശൂന്യം vvp_vector4array_aa::free_instance(vvp_context_t context)
{
      v4cell*cell = static_cast<v4cell*>
            (vvp_get_context_item(context, context_idx_));
      delete [] cell;
}
#endif

ശൂന്യം vvp_vector4array_aa::set_word(unsigned index, സ്ഥിരം vvp_vector4_t&that)
{
      പ്രതിജ്ഞാ(index < വാക്കുകൾ_3);

      v4cell*cell = static_cast<v4cell*>
            (vthread_get_wt_context_item(context_idx_)) + index;

      set_word_(cell, that);
}

vvp_vector4_t vvp_vector4array_aa::get_word(unsigned index) സ്ഥിരം
{
      എങ്കിൽ (index >= വാക്കുകൾ_3)
	    തിരികെ vvp_vector4_t(width_, BIT4_X);

      പ്രതിജ്ഞാ(index < വാക്കുകൾ_3);

      v4cell*cell = static_cast<v4cell*>
            (vthread_get_rd_context_item(context_idx_)) + index;

      തിരികെ get_word_(cell);
}

vvp_vector2_t::vvp_vector2_t()
{
      vec_ = 0;
      wid_ = 0;
}

vvp_vector2_t::vvp_vector2_t(uint64_t v, unsigned wid)
{
      wid_ = wid;
      സ്ഥിരം unsigned bits_per_word = 8 * വലുപ്പം(vec_[0]);
      സ്ഥിരം unsigned വാക്കുകൾ = (wid_ + bits_per_word-1) / bits_per_word;

      vec_ = പുതിയ unsigned long[വാക്കുകൾ];
      വേണ്ടി (unsigned idx = 0 ; idx < വാക്കുകൾ ; idx += 1) {
	    സ്ഥിരം unsigned shift = idx * bits_per_word;
	    vec_[idx] = shift < 64 ? v >> shift : 0;
      }
}

vvp_vector2_t::vvp_vector2_t(vvp_vector2_t::fill_t fill, unsigned wid)
{
      wid_ = wid;
      സ്ഥിരം unsigned bits_per_word = 8 * വലുപ്പം(vec_[0]);
      സ്ഥിരം unsigned വാക്കുകൾ = (wid_ + bits_per_word-1) / bits_per_word;

      vec_ = പുതിയ unsigned long[വാക്കുകൾ];
      വേണ്ടി (unsigned idx = 0 ;  idx < വാക്കുകൾ ;  idx += 1)
	    vec_[idx] = fill? -1 : 0;
}

vvp_vector2_t::vvp_vector2_t(സ്ഥിരം vvp_vector2_t&that, unsigned base, unsigned wid)
{
      wid_ = wid;
      സ്ഥിരം unsigned വാക്കുകൾ = (wid_ + BITS_PER_WORD-1) / BITS_PER_WORD;

      vec_ = പുതിയ unsigned long[വാക്കുകൾ];

      വേണ്ടി (unsigned idx = 0 ; idx < wid ; idx += 1) {
	    പൂർണ്ണസംഖ്യ bit = that.value(base+idx);
	    എങ്കിൽ (bit == 0)
		  തുടരുക;
	    unsigned word = idx / BITS_PER_WORD;
	    unsigned long mask = 1UL << (idx % BITS_PER_WORD);
	    vec_[word] |= mask;
      }
}

ശൂന്യം vvp_vector2_t::copy_from_that_(സ്ഥിരം vvp_vector4_t&that)
{
      wid_ = that.size();
      സ്ഥിരം unsigned വാക്കുകൾ = (that.size() + BITS_PER_WORD-1) / BITS_PER_WORD;

      എങ്കിൽ (വാക്കുകൾ == 0) {
	    vec_ = 0;
	    wid_ = 0;
	    തിരികെ;
      }

	// Use the subarray method with the xz_to_0 flag set so that
	// we get values even when there are xz bits.
      vec_ = that.subarray(0, wid_, സത്യം);
}

ശൂന്യം vvp_vector2_t::copy_from_that_(സ്ഥിരം vvp_vector2_t&that)
{
      wid_ = that.wid_;
      സ്ഥിരം unsigned വാക്കുകൾ = (wid_ + BITS_PER_WORD-1) / BITS_PER_WORD;

      എങ്കിൽ (വാക്കുകൾ == 0) {
	    vec_ = 0;
	    wid_ = 0;
	    തിരികെ;
      }

      vec_ = പുതിയ unsigned long[വാക്കുകൾ];
      വേണ്ടി (unsigned idx = 0 ;  idx < വാക്കുകൾ ;  idx += 1)
	    vec_[idx] = that.vec_[idx];
}

vvp_vector2_t::vvp_vector2_t(സ്ഥിരം vvp_vector2_t&that, unsigned newsize)
{
      wid_ = newsize;
      എങ്കിൽ (newsize == 0) {
	    vec_ = 0;
	    തിരികെ;
      }

      സ്ഥിരം unsigned വാക്കുകൾ = (wid_ + BITS_PER_WORD-1) / BITS_PER_WORD;
      സ്ഥിരം unsigned twords = (that.wid_ + BITS_PER_WORD-1) / BITS_PER_WORD;

      vec_ = പുതിയ unsigned long[വാക്കുകൾ];
      വേണ്ടി (unsigned idx = 0 ;  idx < വാക്കുകൾ ;  idx += 1) {
	    എങ്കിൽ (idx < twords)
		  vec_[idx] = that.vec_[idx];
	    അല്ലെങ്കിൽ
		  vec_[idx] = 0;
      }
}

vvp_vector2_t& vvp_vector2_t::operator= (സ്ഥിരം vvp_vector2_t&that)
{
      എങ്കിൽ (ഇത് == &that)
	    തിരികെ *ഇത്;

      delete[] vec_;
      vec_ = 0;

      copy_from_that_(that);
      തിരികെ *ഇത്;
}

vvp_vector2_t& vvp_vector2_t::operator= (സ്ഥിരം vvp_vector4_t&that)
{
      delete[]vec_;
      vec_ = 0;
      copy_from_that_(that);
      തിരികെ *ഇത്;
}

vvp_vector2_t& vvp_vector2_t::operator <<= (unsigned പൂർണ്ണസംഖ്യ shift)
{
      എങ്കിൽ (wid_ == 0)
	    തിരികെ *ഇത്;

      സ്ഥിരം unsigned വാക്കുകൾ = (wid_ + BITS_PER_WORD-1) / BITS_PER_WORD;

	// Number of words to shift
      സ്ഥിരം unsigned wshift = shift / BITS_PER_WORD;
	// bits to shift within each word.
      സ്ഥിരം unsigned long oshift = shift % BITS_PER_WORD;

	// If shifting the entire value away, then return zeros.
      എങ്കിൽ (wshift >= വാക്കുകൾ) {
	    വേണ്ടി (unsigned idx = 0 ;  idx < വാക്കുകൾ ;  idx += 1)
		  vec_[idx] = 0;

	    തിരികെ *ഇത്;
      }

	// Do the word shift first.
      എങ്കിൽ (wshift > 0) {
	    വേണ്ടി (unsigned idx = 0 ;  idx < വാക്കുകൾ-wshift ;  idx += 1) {
		  unsigned sel = വാക്കുകൾ - idx - 1;
		  vec_[sel] = vec_[sel-wshift];
	    }

	    വേണ്ടി (unsigned idx = 0 ;  idx < wshift ;  idx += 1)
		  vec_[idx] = 0;
      }

	// Do the fine shift.
      എങ്കിൽ (oshift != 0) {
	    unsigned long pad = 0;
	    വേണ്ടി (unsigned idx = 0 ;  idx < വാക്കുകൾ ;  idx += 1) {
		  unsigned long next_pad = vec_[idx] >> (BITS_PER_WORD-oshift);
		  vec_[idx] = (vec_[idx] << oshift) | pad;
		  pad = next_pad;
	    }

	      // Cleanup the tail bits.
	    unsigned long mask = -1UL >> (BITS_PER_WORD - wid_%BITS_PER_WORD);
	    vec_[വാക്കുകൾ-1] &= mask;
      }

      തിരികെ *ഇത്;
}

vvp_vector2_t& vvp_vector2_t::operator >>= (unsigned shift)
{
      എങ്കിൽ (wid_ == 0)
	    തിരികെ *ഇത്;

      സ്ഥിരം unsigned വാക്കുകൾ = (wid_ + BITS_PER_WORD-1) / BITS_PER_WORD;

	// Number of words to shift
      സ്ഥിരം unsigned wshift = shift / BITS_PER_WORD;
	// bits to shift within each word.
      സ്ഥിരം unsigned long oshift = shift % BITS_PER_WORD;

	// If shifting the entire value away, then return zeros.
      എങ്കിൽ (wshift >= വാക്കുകൾ) {
	    വേണ്ടി (unsigned idx = 0 ;  idx < വാക്കുകൾ ;  idx += 1)
		  vec_[idx] = 0;

	    തിരികെ *ഇത്;
      }

      എങ്കിൽ (wshift > 0) {
	    വേണ്ടി (unsigned idx = 0 ;  idx < വാക്കുകൾ-wshift ;  idx += 1)
		  vec_[idx] = vec_[idx+wshift];

	    വേണ്ടി (unsigned idx = വാക്കുകൾ-wshift ;  idx < വാക്കുകൾ ;  idx += 1)
		  vec_[idx] = 0;
      }

      എങ്കിൽ (oshift > 0) {
	    unsigned long pad = 0;
	    വേണ്ടി (unsigned idx = വാക്കുകൾ ;  idx > 0 ;  idx -= 1) {
		  unsigned long new_pad = vec_[idx-1] <<(BITS_PER_WORD-oshift);
		  vec_[idx-1] = pad | (vec_[idx-1] >> oshift);
		  pad = new_pad;
	    }

	      // Cleanup the tail bits.

	    unsigned use_വാക്കുകൾ = വാക്കുകൾ;
	      // Mask_shift is the number of high bits of the top word
	      // that are to be masked off. We start with the number
	      // of bits that are not included even in the original,
	      // then we include the bits of the shift, that are to be
	      // masked to zero.
	    unsigned long mask_shift = BITS_PER_WORD - wid_%BITS_PER_WORD;
	    mask_shift %= BITS_PER_WORD;
	    mask_shift += oshift;
	    ആകുമ്പോൾ (mask_shift >= BITS_PER_WORD) {
		  vec_[use_വാക്കുകൾ-1] = 0;
		  use_വാക്കുകൾ -= 1;
		  mask_shift -= BITS_PER_WORD;
	    }
	    എങ്കിൽ (mask_shift > 0) {
		  പ്രതിജ്ഞാ(use_വാക്കുകൾ > 0);
		  unsigned long mask = -1UL >> mask_shift;
		  vec_[use_വാക്കുകൾ-1] &= mask;
	    }
      }

      തിരികെ *ഇത്;
}

സ്ഥിതം unsigned long add_carry(unsigned long a, unsigned long b,
			       unsigned long&carry)
{
      unsigned long out = carry;
      carry = 0;

      എങ്കിൽ ((ULONG_MAX - out) < a)
	    carry += 1;
      out += a;

      എങ്കിൽ ((ULONG_MAX - out) < b)
	    carry += 1;
      out += b;

      തിരികെ out;
}

vvp_vector2_t& vvp_vector2_t::operator += (സ്ഥിരം vvp_vector2_t&that)
{
      പ്രതിജ്ഞാ(wid_ == that.wid_);
      എങ്കിൽ (wid_ == 0)
	    തിരികെ *ഇത്;

      സ്ഥിരം unsigned വാക്കുകൾ = (wid_ + BITS_PER_WORD-1) / BITS_PER_WORD;

      unsigned long carry = 0;
      വേണ്ടി (unsigned idx = 0 ;  idx < വാക്കുകൾ ;  idx += 1) {
	    vec_[idx] = add_carry(vec_[idx], that.vec_[idx], carry);
      }


	// Cleanup the tail bits.
      unsigned long mask = -1UL >> (BITS_PER_WORD - wid_%BITS_PER_WORD);
      vec_[വാക്കുകൾ-1] &= mask;

      തിരികെ *ഇത്;
}

vvp_vector2_t& vvp_vector2_t::operator -= (സ്ഥിരം vvp_vector2_t&that)
{
      പ്രതിജ്ഞാ(wid_ == that.wid_);
      എങ്കിൽ (wid_ == 0)
	    തിരികെ *ഇത്;

      സ്ഥിരം unsigned വാക്കുകൾ = (wid_ + BITS_PER_WORD-1) / BITS_PER_WORD;

      unsigned long carry = 1;
      വേണ്ടി (unsigned idx = 0 ;  idx < വാക്കുകൾ ;  idx += 1) {
	    vec_[idx] = add_carry(vec_[idx], ~that.vec_[idx], carry);
      }

      തിരികെ *ഇത്;
}

ശൂന്യം vvp_vector2_t::trim()
{
      ആകുമ്പോൾ (value(wid_-1) == 0 && wid_ > 1) wid_ -= 1;
}

/* This is a special trim that is used on numbers we know represent a
 * negative signed value (they came from a negative real value). */
ശൂന്യം vvp_vector2_t::trim_neg()
{
      എങ്കിൽ (value(wid_-1) == 1 && wid_ > 32) {
	    ആകുമ്പോൾ (value(wid_-2) == 1 && wid_ > 32) wid_ -= 1;
      }
}

പൂർണ്ണസംഖ്യ vvp_vector2_t::value(unsigned idx) സ്ഥിരം
{
      എങ്കിൽ (idx >= wid_)
	    തിരികെ 0;

      സ്ഥിരം unsigned bits_per_word = 8 * വലുപ്പം(vec_[0]);
      unsigned addr = idx/bits_per_word;
      unsigned mask = idx%bits_per_word;

      എങ്കിൽ (vec_[addr] & (1UL<<mask))
	    തിരികെ 1;
      അല്ലെങ്കിൽ
	    തിരികെ 0;
}

ശൂന്യം vvp_vector2_t::set_bit(unsigned idx, പൂർണ്ണസംഖ്യ bit)
{
      പ്രതിജ്ഞാ(idx < wid_);

      സ്ഥിരം unsigned bits_per_word = 8 * വലുപ്പം(vec_[0]);
      unsigned addr = idx/bits_per_word;
      unsigned long mask = idx%bits_per_word;

      എങ്കിൽ (bit)
	    vec_[addr] |= 1UL << mask;
      അല്ലെങ്കിൽ
	    vec_[addr] &= ~(1UL << mask);
}

ശൂന്യം vvp_vector2_t::set_vec(unsigned adr, സ്ഥിരം vvp_vector2_t&that)
{
      പ്രതിജ്ഞാ((adr + that.wid_) <= wid_);

      വേണ്ടി (unsigned idx = 0 ; idx < that.wid_ ; idx += 1)
	    set_bit(adr+idx, that.value(idx));
}

തർക്കം vvp_vector2_t::is_NaN() സ്ഥിരം
{
      തിരികെ wid_ == 0;
}

തർക്കം vvp_vector2_t::is_zero() സ്ഥിരം
{
      സ്ഥിരം unsigned വാക്കുകൾ = (wid_ + BITS_PER_WORD-1) / BITS_PER_WORD;

      വേണ്ടി (unsigned idx = 0; idx < വാക്കുകൾ; idx += 1) {
	    എങ്കിൽ (vec_[idx] == 0) തുടരുക;
	    തിരികെ അസത്യം;
      }

      തിരികെ സത്യം;
}

/*
 * Basic idea from "Introduction to Programming using SML" by
 * Michael R. Hansen and Hans Rischel page 261 and "Seminumerical
 * Algorithms, Third Edition" by Donald E. Knuth section 4.6.3.
 */
vvp_vector2_t pow(സ്ഥിരം vvp_vector2_t&x, vvp_vector2_t&y)
{
        /* If we have a zero exponent just return 1. */
      എങ്കിൽ (y == vvp_vector2_t(0L, 1)) {
	    തിരികെ vvp_vector2_t(1L, x.size());
      }

        /* Is the value odd? */
      എങ്കിൽ (y.value(0) == 1) {
	    y.set_bit(0, 0);  // A quick subtract by 1.
	    vvp_vector2_t res = x * pow(x, y);
	    തിരികെ res;
      }

      y >>= 1;  // A fast divide by two. We know the LSB is zero.
      vvp_vector2_t z = pow(x, y);
      vvp_vector2_t res = z * z;
      തിരികെ res;
}

സ്ഥിതം ശൂന്യം multiply_long(unsigned long a, unsigned long b,
			  unsigned long&താഴെയുള്ള, unsigned long&മുകളിലെ)
{
      പ്രതിജ്ഞാ(വലുപ്പം(unsigned long) %2 == 0);

      സ്ഥിരം unsigned long word_mask = (1UL << 4UL*വലുപ്പം(a)) - 1UL;
      unsigned long tmpa;
      unsigned long tmpb;
      unsigned long res[4];

      tmpa = a & word_mask;
      tmpb = b & word_mask;
      res[0] = tmpa * tmpb;
      res[1] = res[0] >> 4UL*വലുപ്പം(unsigned long);
      res[0] &= word_mask;

      tmpa = (a >> 4UL*വലുപ്പം(unsigned long)) & word_mask;
      res[1] += tmpa * tmpb;
      res[2] = res[1] >> 4UL*വലുപ്പം(unsigned long);
      res[1] &= word_mask;

      tmpa = a & word_mask;
      tmpb = (b >> 4UL*വലുപ്പം(unsigned long)) & word_mask;
      res[1] += tmpa * tmpb;
      res[2] += res[1] >> 4UL*വലുപ്പം(unsigned long);
      res[3]  = res[2] >> 4UL*വലുപ്പം(unsigned long);
      res[1] &= word_mask;
      res[2] &= word_mask;

      tmpa = (a >> 4UL*വലുപ്പം(unsigned long)) & word_mask;
      res[2] += tmpa * tmpb;
      res[3] += res[2] >> 4UL*വലുപ്പം(unsigned long);
      res[2] &= word_mask;

      മുകളിലെ = (res[3] << 4UL*വലുപ്പം(unsigned long)) | res[2];
      താഴെയുള്ള  = (res[1] << 4UL*വലുപ്പം(unsigned long)) | res[0];
}

vvp_vector2_t operator * (സ്ഥിരം vvp_vector2_t&a, സ്ഥിരം vvp_vector2_t&b)
{
      സ്ഥിരം unsigned bits_per_word = 8 * വലുപ്പം(a.vec_[0]);

	// The compiler ensures that the two operands are of equal size.
      പ്രതിജ്ഞാ(a.size() == b.size());
      vvp_vector2_t r (0, a.size());

      unsigned വാക്കുകൾ = (r.wid_ + bits_per_word - 1) / bits_per_word;

      വേണ്ടി (unsigned bdx = 0 ;  bdx < വാക്കുകൾ ;  bdx += 1) {
	    unsigned long tmpb = b.vec_[bdx];
	    എങ്കിൽ (tmpb == 0)
		  തുടരുക;

	    വേണ്ടി (unsigned adx = 0 ;  adx < വാക്കുകൾ ;  adx += 1) {
		  unsigned long tmpa = a.vec_[adx];
		  എങ്കിൽ (tmpa == 0)
			തുടരുക;

		  unsigned long താഴെയുള്ള, hig;
		  multiply_long(tmpa, tmpb, താഴെയുള്ള, hig);

		  unsigned long carry = 0;
		  വേണ്ടി (unsigned sdx = 0
			     ; (adx+bdx+sdx) < വാക്കുകൾ
			     ;  sdx += 1) {

			r.vec_[adx+bdx+sdx] = add_carry(r.vec_[adx+bdx+sdx],
							താഴെയുള്ള, carry);
			താഴെയുള്ള = hig;
			hig = 0;
		  }
	    }
      }

      തിരികെ r;
}

സ്ഥിതം ശൂന്യം div_mod (vvp_vector2_t dividend, vvp_vector2_t ഹാരകം,
		     vvp_vector2_t&quotient, vvp_vector2_t&remainder)
{

      quotient = vvp_vector2_t(0, dividend.size());

      എങ്കിൽ (ഹാരകം == quotient) {
	    cerr << "ERROR: division by zero, exiting." << endl;
	    exit(255);
      }

      എങ്കിൽ (dividend < ഹാരകം) {
	    remainder = dividend;
	    തിരികെ;
      }

      vvp_vector2_t mask (1, dividend.size());

	// Make the dividend 1 bit larger to prevent overflow of
	// divtmp in startup.
      dividend = vvp_vector2_t(dividend, dividend.size()+1);
      vvp_vector2_t divtmp (ഹാരകം, dividend.size());

      ആകുമ്പോൾ (divtmp < dividend) {
	    divtmp <<= 1;
	    mask <<= 1;
      }

      ആകുമ്പോൾ (dividend >= ഹാരകം) {
	    എങ്കിൽ (divtmp <= dividend) {
		  dividend -= divtmp;
		  quotient += mask;
	    }

	    divtmp >>= 1;
	    mask >>= 1;
      }

      remainder = vvp_vector2_t(dividend, mask.size());
}

vvp_vector2_t operator - (സ്ഥിരം vvp_vector2_t&that)
{
      vvp_vector2_t neg(that);
      എങ്കിൽ (neg.wid_ == 0) തിരികെ neg;

      സ്ഥിരം unsigned വാക്കുകൾ = (neg.wid_ + neg.BITS_PER_WORD-1) /
                                         neg.BITS_PER_WORD;
      വേണ്ടി (unsigned idx = 0 ;  idx < വാക്കുകൾ ;  idx += 1) {
	    neg.vec_[idx] = ~neg.vec_[idx];
      }
      neg += vvp_vector2_t(1, neg.wid_);

      തിരികെ neg;
}

vvp_vector2_t operator / (സ്ഥിരം vvp_vector2_t&dividend,
			  സ്ഥിരം vvp_vector2_t&ഹാരകം)
{
      vvp_vector2_t quot, rem;
      div_mod(dividend, ഹാരകം, quot, rem);
      തിരികെ quot;
}

vvp_vector2_t operator % (സ്ഥിരം vvp_vector2_t&dividend,
			  സ്ഥിരം vvp_vector2_t&ഹാരകം)
{
      vvp_vector2_t quot, rem;
      div_mod(dividend, ഹാരകം, quot, rem);
      തിരികെ rem;
}

തർക്കം operator > (സ്ഥിരം vvp_vector2_t&a, സ്ഥിരം vvp_vector2_t&b)
{
      സ്ഥിരം unsigned awords = (a.wid_ + vvp_vector2_t::BITS_PER_WORD-1) / vvp_vector2_t::BITS_PER_WORD;
      സ്ഥിരം unsigned bwords = (b.wid_ + vvp_vector2_t::BITS_PER_WORD-1) / vvp_vector2_t::BITS_PER_WORD;

      സ്ഥിരം unsigned വാക്കുകൾ = awords > bwords? awords : bwords;

      വേണ്ടി (unsigned idx = വാക്കുകൾ ;  idx > 0 ;  idx -= 1) {
	    unsigned long aw = (idx <= awords)? a.vec_[idx-1] : 0;
	    unsigned long bw = (idx <= bwords)? b.vec_[idx-1] : 0;

	    എങ്കിൽ (aw > bw)
		  തിരികെ സത്യം;
	    എങ്കിൽ (aw < bw)
		  തിരികെ അസത്യം;
      }

	// If the above loop finishes, then the vectors are equal.
      തിരികെ അസത്യം;
}

തർക്കം operator >= (സ്ഥിരം vvp_vector2_t&a, സ്ഥിരം vvp_vector2_t&b)
{
      സ്ഥിരം unsigned awords = (a.wid_ + vvp_vector2_t::BITS_PER_WORD-1) / vvp_vector2_t::BITS_PER_WORD;
      സ്ഥിരം unsigned bwords = (b.wid_ + vvp_vector2_t::BITS_PER_WORD-1) / vvp_vector2_t::BITS_PER_WORD;

      സ്ഥിരം unsigned വാക്കുകൾ = awords > bwords? awords : bwords;

      വേണ്ടി (unsigned idx = വാക്കുകൾ ;  idx > 0 ;  idx -= 1) {
	    unsigned long aw = (idx <= awords)? a.vec_[idx-1] : 0;
	    unsigned long bw = (idx <= bwords)? b.vec_[idx-1] : 0;

	    എങ്കിൽ (aw > bw)
		  തിരികെ സത്യം;
	    എങ്കിൽ (aw < bw)
		  തിരികെ അസത്യം;
      }

	// If the above loop finishes, then the vectors are equal.
      തിരികെ സത്യം;
}

തർക്കം operator < (സ്ഥിരം vvp_vector2_t&a, സ്ഥിരം vvp_vector2_t&b)
{
      സ്ഥിരം unsigned awords = (a.wid_ + vvp_vector2_t::BITS_PER_WORD-1) / vvp_vector2_t::BITS_PER_WORD;
      സ്ഥിരം unsigned bwords = (b.wid_ + vvp_vector2_t::BITS_PER_WORD-1) / vvp_vector2_t::BITS_PER_WORD;

      unsigned വാക്കുകൾ = awords;
      എങ്കിൽ (bwords > വാക്കുകൾ)
	    വാക്കുകൾ = bwords;

      വേണ്ടി (unsigned idx = വാക്കുകൾ ;  idx > 0 ;  idx -= 1) {
	    unsigned long aw = (idx <= awords)? a.vec_[idx-1] : 0;
	    unsigned long bw = (idx <= bwords)? b.vec_[idx-1] : 0;

	    എങ്കിൽ (aw < bw)
		  തിരികെ സത്യം;
	    എങ്കിൽ (aw > bw)
		  തിരികെ അസത്യം;
      }

	// If the above loop finishes, then the vectors are equal.
      തിരികെ അസത്യം;
}

തർക്കം operator <= (സ്ഥിരം vvp_vector2_t&a, സ്ഥിരം vvp_vector2_t&b)
{
	// XXXX For now, only support equal width vectors.
      പ്രതിജ്ഞാ(a.wid_ == b.wid_);

      സ്ഥിരം unsigned awords = (a.wid_ + vvp_vector2_t::BITS_PER_WORD-1) / vvp_vector2_t::BITS_PER_WORD;

      വേണ്ടി (unsigned idx = awords ;  idx > 0 ;  idx -= 1) {
	    എങ്കിൽ (a.vec_[idx-1] < b.vec_[idx-1])
		  തിരികെ സത്യം;
	    എങ്കിൽ (a.vec_[idx-1] > b.vec_[idx-1])
		  തിരികെ അസത്യം;
      }

	// If the above loop finishes, then the vectors are equal.
      തിരികെ സത്യം;
}

തർക്കം operator == (സ്ഥിരം vvp_vector2_t&a, സ്ഥിരം vvp_vector2_t&b)
{
      സ്ഥിരം unsigned awords = (a.wid_ + vvp_vector2_t::BITS_PER_WORD-1) / vvp_vector2_t::BITS_PER_WORD;
      സ്ഥിരം unsigned bwords = (b.wid_ + vvp_vector2_t::BITS_PER_WORD-1) / vvp_vector2_t::BITS_PER_WORD;

      സ്ഥിരം unsigned വാക്കുകൾ = awords > bwords? awords : bwords;

      വേണ്ടി (unsigned idx = വാക്കുകൾ ;  idx > 0 ;  idx -= 1) {
	    unsigned long aw = (idx <= awords)? a.vec_[idx-1] : 0;
	    unsigned long bw = (idx <= bwords)? b.vec_[idx-1] : 0;

	    എങ്കിൽ (aw > bw)
		  തിരികെ അസത്യം;
	    എങ്കിൽ (aw < bw)
		  തിരികെ അസത്യം;
      }

	// If the above loop finishes, then the vectors are equal.
      തിരികെ സത്യം;
}


vvp_vector4_t vector2_to_vector4(സ്ഥിരം vvp_vector2_t&that, unsigned wid)
{
      vvp_vector4_t res (wid);

      വേണ്ടി (unsigned idx = 0 ;  idx < res.size() ;  idx += 1) {
	    vvp_bit4_t bit = BIT4_0;

	    എങ്കിൽ (that.value(idx))
		  bit = BIT4_1;

	    res.set_bit(idx, bit);
      }

      തിരികെ res;
}

സ്ഥിതം തർക്കം c4string_header_test(സ്ഥിരം അക്ഷരം*str)
{
      എങ്കിൽ ((str[0] != 'C') && (str[0] != 'c'))
	    തിരികെ അസത്യം;
      എങ്കിൽ ((str[1] != '4') || (str[2] != '<'))
	    തിരികെ അസത്യം;
      തിരികെ സത്യം;
}

തർക്കം c4string_test(സ്ഥിരം അക്ഷരം*str)
{
      എങ്കിൽ (!c4string_header_test(str))
	    തിരികെ അസത്യം;
      size_t മൂല്യം_size = strspn(str+3, "01xz");
      എങ്കിൽ (str[3+മൂല്യം_size] != '>')
	    തിരികെ അസത്യം;
      എങ്കിൽ (str[3+മൂല്യം_size+1] != 0)
	    തിരികെ അസത്യം;

      തിരികെ സത്യം;
}

vvp_vector4_t c4string_to_vector4(സ്ഥിരം അക്ഷരം*str)
{
      പ്രതിജ്ഞാ(c4string_header_test(str));

      str += 3;
      സ്ഥിരം അക്ഷരം*tp = str + strspn(str,"01xz");
      പ്രതിജ്ഞാ(tp[0] == '>');

      vvp_vector4_t tmp (tp-str);

      വേണ്ടി (unsigned idx = 0 ;  idx < tmp.size() ;  idx += 1) {
	    vvp_bit4_t bit;
	    തിരഞ്ഞെടുപ്പ് (str[idx]) {
		അവസ്ഥ '0':
		  bit = BIT4_0;
		  നിർത്തുക;
		അവസ്ഥ '1':
		  bit = BIT4_1;
		  നിർത്തുക;
		അവസ്ഥ 'x':
		  bit = BIT4_X;
		  നിർത്തുക;
		അവസ്ഥ 'z':
		  bit = BIT4_Z;
		  നിർത്തുക;
		സ്ഥിരസ്ഥിതി:
		  fprintf(stderr, "Unsupported bit value %c(%d).\n", str[idx],
		          str[idx]);
		  പ്രതിജ്ഞാ(0);
		  bit = BIT4_0;
		  നിർത്തുക;
	    }
	    tmp.set_bit(tmp.size()-idx-1, bit);
      }

      തിരികെ tmp;
}

ostream& operator<< (ostream&out, സ്ഥിരം vvp_vector2_t&that)
{
      എങ്കിൽ (that.is_NaN()) {
	    out << "NaN";

      } അല്ലെങ്കിൽ {
	    out << vector2_to_vector4(that, that.size());
      }
      തിരികെ out;
}

vvp_vector8_t::vvp_vector8_t(സ്ഥിരം vvp_vector8_t&that)
{
      size_ = that.size_;
      എങ്കിൽ (size_ <= വലുപ്പം(val_)) {
	    memcpy(val_, that.val_, വലുപ്പം val_);
      } അല്ലെങ്കിൽ {
	    ptr_ = പുതിയ unsigned അക്ഷരം[size_];
	    memcpy(ptr_, that.ptr_, size_);
      }
}

vvp_vector8_t::vvp_vector8_t(സ്ഥിരം vvp_vector4_t&that,
			     unsigned str0, unsigned str1)
: size_(that.size())
{
      എങ്കിൽ (size_ == 0)
	    തിരികെ;

      എങ്കിൽ (size_ <= വലുപ്പം(val_)) {
	    ptr_ = 0; // Prefill all val_ bytes
	    വേണ്ടി (unsigned idx = 0 ; idx < size_ ; idx += 1)
		  val_[idx] = vvp_scalar_t(that.value(idx),str0, str1).raw();
      } അല്ലെങ്കിൽ {
	    ptr_ = പുതിയ unsigned അക്ഷരം[size_];
	    വേണ്ടി (unsigned idx = 0 ;  idx < size_ ;  idx += 1)
		  ptr_[idx] = vvp_scalar_t(that.value(idx), str0, str1).raw();
      }
}

vvp_vector8_t::vvp_vector8_t(സ്ഥിരം vvp_vector2_t&that,
			     unsigned str0, unsigned str1)
: size_(that.size())
{
      എങ്കിൽ (size_ == 0)
	    തിരികെ;

      എങ്കിൽ (size_ <= വലുപ്പം(val_)) {
	    ptr_ = 0;
	    വേണ്ടി (unsigned idx = 0 ; idx < size_ ; idx += 1)
		  val_[idx] = vvp_scalar_t(that.value(idx)? BIT4_1:BIT4_0, str0, str1).raw();
      } അല്ലെങ്കിൽ {
	    ptr_ = പുതിയ unsigned അക്ഷരം[size_];
	    വേണ്ടി (unsigned idx = 0 ; idx < size_ ; idx += 1)
		  ptr_[idx] = vvp_scalar_t(that.value(idx)? BIT4_1:BIT4_0, str0, str1).raw();
      }
}

സ്ഥിരം vvp_vector8_t vvp_vector8_t::nil;

vvp_vector8_t& vvp_vector8_t::operator= (സ്ഥിരം vvp_vector8_t&that)
{
	// Assign to self.
      എങ്കിൽ (ഇത് == &that)
	    തിരികെ *ഇത്;

      എങ്കിൽ (size_ != that.size_) {
	    എങ്കിൽ (size_ > വലുപ്പം(val_))
		  delete[]ptr_;
	    size_ = 0;
      }

      എങ്കിൽ (that.size_ == 0) {
	    പ്രതിജ്ഞാ(size_ == 0);
	    തിരികെ *ഇത്;
      }

      എങ്കിൽ (that.size_ <= വലുപ്പം(val_)) {
	    size_ = that.size_;
	    memcpy(val_, that.val_, വലുപ്പം(val_));
	    തിരികെ *ഇത്;
      }

      എങ്കിൽ (size_ == 0) {
	    size_ = that.size_;
	    ptr_ = പുതിയ unsigned അക്ഷരം[size_];
      }

      memcpy(ptr_, that.ptr_, size_);

      തിരികെ *ഇത്;
}

vvp_vector8_t vvp_vector8_t::subvalue(unsigned base, unsigned wid) സ്ഥിരം
{
      vvp_vector8_t tmp (wid);

      unsigned അക്ഷരം*tmp_ptr = tmp.size_ <= വലുപ്പം(val_) ? tmp.val_ : tmp.ptr_;
      സ്ഥിരം unsigned അക്ഷരം*use_ptr = size_ <= വലുപ്പം(val_) ? val_ : ptr_;

      unsigned idx = 0;
      ആകുമ്പോൾ ((idx < wid) && (base+idx < size_)) {
	    tmp_ptr[idx] = use_ptr[base+idx];
	    idx += 1;
      }

      തിരികെ tmp;
}

ശൂന്യം vvp_vector8_t::set_vec(unsigned base, സ്ഥിരം vvp_vector8_t&that)
{
      പ്രതിജ്ഞാ((base+that.size()) <= size());
      വേണ്ടി (unsigned idx = 0 ; idx < that.size() ; idx += 1)
	    set_bit(base+idx, that.value(idx));
}

vvp_vector8_t part_expand(സ്ഥിരം vvp_vector8_t&that, unsigned wid, unsigned off)
{
      പ്രതിജ്ഞാ(off < wid);
      vvp_vector8_t tmp (wid);

      unsigned അക്ഷരം* tmp_ptr = tmp.size_<= വലുപ്പം(tmp.val_) ?
                               tmp.val_ : tmp.ptr_;
      സ്ഥിരം unsigned അക്ഷരം* that_ptr = that.size_<= വലുപ്പം(that.val_) ?
                                      that.val_ : that.ptr_;

      unsigned idx = off;

      ആകുമ്പോൾ (idx < wid && that.size_ > (idx-off)) {
	    tmp_ptr[idx] = that_ptr[idx-off];
	    idx += 1;
      }

      തിരികെ tmp;
}

സ്ഥിതം തർക്കം c8string_header_test(സ്ഥിരം അക്ഷരം*str)
{
      എങ്കിൽ ((str[0] != 'C') && (str[0] != 'c'))
	    തിരികെ അസത്യം;
      എങ്കിൽ ((str[1] != '8') || (str[2] != '<'))
	    തിരികെ അസത്യം;
      തിരികെ സത്യം;
}

തർക്കം c8string_test(സ്ഥിരം അക്ഷരം*str)
{
      എങ്കിൽ (!c8string_header_test(str))
	    തിരികെ അസത്യം;

      സ്ഥിരം അക്ഷരം*cp = str+3;
      വേണ്ടി (;; cp += 1) {
	    എങ്കിൽ (cp[0] == '>' && cp[1] == 0) തിരികെ സത്യം;
	    എങ്കിൽ (cp[0] >= '0' && cp[0] <= '9') തുടരുക;
	    എങ്കിൽ (cp[0] == 'x') തുടരുക;
	    എങ്കിൽ (cp[0] == 'z') തുടരുക;
	    തിരികെ അസത്യം;
      }
      തിരികെ അസത്യം;
}
/*
 * The format of a C8<> string is:
 *   C8<aaabbbccc...>
 * where aaa... is a 3 character bit descriptor.
 */
vvp_vector8_t c8string_to_vector8(സ്ഥിരം അക്ഷരം*str)
{
      പ്രതിജ്ഞാ(c8string_header_test(str));

      size_t vsize = strlen(str)-4;
      പ്രതിജ്ഞാ(vsize%3 == 0);
      vsize /= 3;
      vvp_vector8_t tmp (vsize);

      വേണ്ടി (size_t idx = 0 ; idx < vsize ; idx += 1) {
	    സ്ഥിരം അക്ഷരം*cp = str+3+3*idx;
	    vvp_bit4_t bit = BIT4_X;
	    unsigned dr0 = cp[0]-'0';
	    unsigned dr1 = cp[1]-'0';
	    തിരഞ്ഞെടുപ്പ് (cp[2]) {
		അവസ്ഥ '0':
		  bit = BIT4_0;
		  നിർത്തുക;
		അവസ്ഥ '1':
		  bit = BIT4_1;
		  നിർത്തുക;
		അവസ്ഥ 'x':
		  bit = BIT4_X;
		  നിർത്തുക;
		അവസ്ഥ 'z':
		  bit = BIT4_Z;
		  നിർത്തുക;
	    }
	    tmp.set_bit(vsize-idx-1, vvp_scalar_t(bit, dr0, dr1));
      }

      തിരികെ tmp;
}

ostream& operator<<(ostream&out, സ്ഥിരം vvp_vector8_t&that)
{
      out << "C8<";
      വേണ്ടി (unsigned idx = 0 ;  idx < that.size() ; idx += 1)
	    out << that.value(that.size()-idx-1);

      out << ">";
      തിരികെ out;
}

vvp_net_fun_t::vvp_net_fun_t()
{
      എണ്ണം_functors += 1;
}

vvp_net_fun_t::~vvp_net_fun_t()
{
}

ശൂന്യം vvp_net_fun_t::recv_vec4(vvp_net_ptr_t, സ്ഥിരം vvp_vector4_t&,
                              vvp_context_t)
{
      fprintf(stderr, "internal error: %s: recv_vec4 not implemented\n",
	      typeid(*ഇത്).name());
      പ്രതിജ്ഞാ(0);
}

ശൂന്യം vvp_net_fun_t::recv_vec4_pv_(vvp_net_ptr_t p, സ്ഥിരം vvp_vector4_t&bit,
                                  unsigned base, unsigned vwid, vvp_context_t)
{
	// The majority of functors don't normally expect to receive part
	// values, because the primary operands of an expression will be
	// extended to the expression width. But in the case that a primary
	// operand is a wire that is only partly driven by a single driver,
	// the part value driven onto the wire propagates directly to the
	// inputs of any functors connected to that wire. In this case we
	// know that the remaining bits are undriven, so can simply build
	// the full width value from the part we have received.
	//
	// Note that this case is almost certainly a bug in the user's
	// code, but we still need to handle it correctly. See GitHub
	// issue #99 and br_gh99*.v in the test suite for examples.

      പ്രതിജ്ഞാ(base + bit.size() <= vwid);

      vvp_vector4_t tmp(vwid, BIT4_Z);
      tmp.set_vec(base, bit);
      recv_vec4(p, tmp, 0);
}

ശൂന്യം vvp_net_fun_t::recv_vec4_pv(vvp_net_ptr_t, സ്ഥിരം vvp_vector4_t&bit,
                                 unsigned base, unsigned vwid, vvp_context_t)
{
      cerr << "internal error: " << typeid(*ഇത്).name() << ": "
	   << "recv_vec4_pv(" << bit << ", " << base
	   << ", " << vwid << ") not implemented" << endl;
      പ്രതിജ്ഞാ(0);
}

ശൂന്യം vvp_net_fun_t::recv_vec8(vvp_net_ptr_t port, സ്ഥിരം vvp_vector8_t&bit)
{
      recv_vec4(port, reduce4(bit), 0);
}

ശൂന്യം vvp_net_fun_t::recv_vec8_pv_(vvp_net_ptr_t p, സ്ഥിരം vvp_vector8_t&bit,
				  unsigned base, unsigned vwid)
{
	// This is the strength-aware version of recv_vec4_pv_.

      പ്രതിജ്ഞാ(base + bit.size() <= vwid);

      vvp_vector8_t tmp(vwid);
      tmp.set_vec(base, bit);
      recv_vec8(p, tmp);
}

ശൂന്യം vvp_net_fun_t::recv_vec8_pv(vvp_net_ptr_t port, സ്ഥിരം vvp_vector8_t&bit,
				 unsigned base, unsigned vwid)
{
      recv_vec4_pv(port, reduce4(bit), base, vwid, 0);
}

ശൂന്യം vvp_net_fun_t::recv_real(vvp_net_ptr_t, ഇരട്ട bit, vvp_context_t)
{
      fprintf(stderr, "internal error: %s: recv_real(%f) not implemented\n",
	      typeid(*ഇത്).name(), bit);
      പ്രതിജ്ഞാ(0);
}

ശൂന്യം vvp_net_fun_t::recv_string(vvp_net_ptr_t, സ്ഥിരം std::string&bit, vvp_context_t)
{
      fprintf(stderr, "internal error: %s: recv_string(%s) not implemented\n",
	      typeid(*ഇത്).name(), bit.c_str());
      പ്രതിജ്ഞാ(0);
}

ശൂന്യം vvp_net_fun_t::recv_object(vvp_net_ptr_t, vvp_object_t, vvp_context_t)
{
      fprintf(stderr, "internal error: %s: recv_object(...) not implemented\n",
	      typeid(*ഇത്).name());
      പ്രതിജ്ഞാ(0);
}

ശൂന്യം vvp_net_fun_t::force_flag(തർക്കം)
{
}

/* **** vvp_fun_drive methods **** */

vvp_fun_drive::vvp_fun_drive(unsigned str0, unsigned str1)
{
      പ്രതിജ്ഞാ(str0 < 8);
      പ്രതിജ്ഞാ(str1 < 8);

      drive0_ = str0;
      drive1_ = str1;
}

vvp_fun_drive::~vvp_fun_drive()
{
}

ശൂന്യം vvp_fun_drive::recv_vec4(vvp_net_ptr_t port, സ്ഥിരം vvp_vector4_t&bit,
                              vvp_context_t)
{
      പ്രതിജ്ഞാ(port.port() == 0);
      port.ptr()->send_vec8(vvp_vector8_t(bit, drive0_, drive1_));
}

ശൂന്യം vvp_fun_drive::recv_vec4_pv(vvp_net_ptr_t ptr, സ്ഥിരം vvp_vector4_t&bit,
				 unsigned base, unsigned vwid, vvp_context_t ctx)
{
      recv_vec4_pv_(ptr, bit, base, vwid, ctx);
}

/* **** vvp_wide_fun_* methods **** */

vvp_wide_fun_core::vvp_wide_fun_core(vvp_net_t*net, unsigned nports)
{
      ptr_ = net;
      nports_ = nports;
      port_മൂല്യങ്ങൾ = 0;
      port_rvalues_ = 0;
}

vvp_wide_fun_core::~vvp_wide_fun_core()
{
      delete[] port_മൂല്യങ്ങൾ;
      delete[] port_rvalues_;
}

ശൂന്യം vvp_wide_fun_core::propagate_vec4(സ്ഥിരം vvp_vector4_t&bit,
				       vvp_time64_t delay)
{
      എങ്കിൽ (delay)
	    schedule_propagate_vector(ptr_, delay, bit);
      അല്ലെങ്കിൽ
	    ptr_->send_vec4(bit, 0);
}

ശൂന്യം vvp_wide_fun_core::propagate_real(ഇരട്ട bit,
				       vvp_time64_t delay)
{
      എങ്കിൽ (delay) {
	      // schedule_assign_vector(ptr_->out, bit, delay);
	    പ്രതിജ്ഞാ(0); // Need a real-value version of assign_vector.
      } അല്ലെങ്കിൽ {
	    ptr_->send_real(bit, 0);
      }
}


unsigned vvp_wide_fun_core::port_എണ്ണം() സ്ഥിരം
{
      തിരികെ nports_;
}

vvp_vector4_t& vvp_wide_fun_core::value(unsigned idx)
{
      പ്രതിജ്ഞാ(idx < nports_);
      എങ്കിൽ (port_മൂല്യങ്ങൾ == 0)
	    port_മൂല്യങ്ങൾ = പുതിയ vvp_vector4_t [nports_];
      തിരികെ port_മൂല്യങ്ങൾ[idx];
}

ഇരട്ട vvp_wide_fun_core::മൂല്യം_r(unsigned idx)
{
      പ്രതിജ്ഞാ(idx < nports_);
      തിരികെ port_rvalues_? port_rvalues_[idx] : 0.0;
}

ശൂന്യം vvp_wide_fun_core::recv_real_from_inputs(unsigned)
{
      പ്രതിജ്ഞാ(0);
}

ശൂന്യം vvp_wide_fun_core::dispatch_vec4_from_നിവേശം(unsigned port,
						  സ്ഥിരം vvp_vector4_t&bit)
{
      പ്രതിജ്ഞാ(port < nports_);
      എങ്കിൽ (port_മൂല്യങ്ങൾ == 0) port_മൂല്യങ്ങൾ = പുതിയ vvp_vector4_t [nports_];
      port_മൂല്യങ്ങൾ[port] = bit;
      recv_vec4_from_inputs(port);
}

ശൂന്യം vvp_wide_fun_core::dispatch_real_from_നിവേശം(unsigned port,
						  ഇരട്ട bit)
{
      പ്രതിജ്ഞാ(port < nports_);
      എങ്കിൽ (port_rvalues_ == 0) port_rvalues_ = പുതിയ ഇരട്ട[nports_];
      port_rvalues_[port] = bit;
      recv_real_from_inputs(port);
}

vvp_wide_fun_t::vvp_wide_fun_t(vvp_wide_fun_core*c, unsigned base)
: core_(c), port_base_(base)
{
}

vvp_wide_fun_t::~vvp_wide_fun_t()
{
}

ശൂന്യം vvp_wide_fun_t::recv_vec4(vvp_net_ptr_t port, സ്ഥിരം vvp_vector4_t&bit,
                               vvp_context_t)
{
      unsigned pidx = port_base_ + port.port();
      core_->dispatch_vec4_from_നിവേശം(pidx, bit);
}

ശൂന്യം vvp_wide_fun_t::recv_vec4_pv(vvp_net_ptr_t ptr, സ്ഥിരം vvp_vector4_t&bit,
				  unsigned base, unsigned vwid, vvp_context_t ctx)
{
      recv_vec4_pv_(ptr, bit, base, vwid, ctx);
}

ശൂന്യം vvp_wide_fun_t::recv_real(vvp_net_ptr_t port, ഇരട്ട bit,
                               vvp_context_t)
{
      unsigned pidx = port_base_ + port.port();
      core_->dispatch_real_from_നിവേശം(pidx, bit);
}

/* **** vvp_scalar_t methods **** */

/*
 * DRIVE STRENGTHS:
 *
 * The normal functor is not aware of strengths. It generates strength
 * simply by virtue of having strength specifications. The drive
 * strength specification includes a drive0 and drive1 strength, each
 * with 8 possible values (that can be represented in 3 bits) as given
 * in this table:
 *
 *    HiZ    = 0,
 *    SMALL  = 1,
 *    MEDIUM = 2,
 *    WEAK   = 3,
 *    LARGE  = 4,
 *    PULL   = 5,
 *    STRONG = 6,
 *    SUPPLY = 7
 *
 * The vvp_scalar_t value, however, is a combination of value and
 * strength, used in strength-aware contexts.
 *
 * OUTPUT STRENGTHS:
 *
 * The strength-aware values are specified as an 8 bit value, that is
 * two 4 bit numbers. The value is encoded with two drive strengths (0-7)
 * and two drive values (0 or 1). Each nibble contains three bits of
 * strength and one bit of value, like so: VSSS. The high nibble has
 * the strength-value closest to supply1, and the low nibble has the
 * strength-value closest to supply0.
 */

/*
 * A signal value is unambiguous if the top 4 bits and the bottom 4
 * bits are identical. This means that the VSSSvsss bits of the 8bit
 * value have V==v and SSS==sss.
 */
# define UNAMBIG(v)  (((v) & 0x0f) == (((v) >> 4) & 0x0f))

# define STREN1(v) (((v)&0x70) >> 4)
# define STREN0(v) ((v)&0x07)

unsigned vvp_scalar_t::strength0() സ്ഥിരം
{
      തിരികെ STREN0(മൂല്യം_2);
}

unsigned vvp_scalar_t::strength1() സ്ഥിരം
{
      തിരികെ STREN1(മൂല്യം_2);
}

ostream& operator <<(ostream&out, vvp_scalar_t a)
{
      out << a.strength0() << a.strength1();
      തിരഞ്ഞെടുപ്പ് (a.value()) {
	  അവസ്ഥ BIT4_0:
	    out << "0";
	    നിർത്തുക;
	  അവസ്ഥ BIT4_1:
	    out << "1";
	    നിർത്തുക;
	  അവസ്ഥ BIT4_X:
	    out << "X";
	    നിർത്തുക;
	  അവസ്ഥ BIT4_Z:
	    out << "Z";
	    നിർത്തുക;
      }
      തിരികെ out;
}

/*
 * This function is only called if the actual interface function rules
 * out some of the easy cases. If we get here, we can assume that
 * neither of the values is HiZ, and the values are not exactly equal.
 */
vvp_scalar_t fully_featured_resolv_(vvp_scalar_t a, vvp_scalar_t b)
{

      എങ്കിൽ (UNAMBIG(a.മൂല്യം_2) && UNAMBIG(b.മൂല്യം_2)) {

	      /* If both signals are unambiguous, simply choose
		 the stronger. If they have the same strength
		 but different values, then this becomes
		 ambiguous. */

	    എങ്കിൽ ((b.മൂല്യം_2&0x07) > (a.മൂല്യം_2&0x07)) {

		    /* b value is stronger. Take it. */
		  തിരികെ b;

	    } അല്ലെങ്കിൽ എങ്കിൽ ((b.മൂല്യം_2&0x77) == (a.മൂല്യം_2&0x77)) {

		    // Strengths are the same. Since we know already
		    // that the values are not the same, Make value
		    // into "x".
		  vvp_scalar_t tmp (a);
		  tmp.മൂല്യം_2 = (tmp.മൂല്യം_2&0x77) | 0x80;
		  തിരികെ tmp;

	    } അല്ലെങ്കിൽ {

		    /* Must be "a" is the stronger one. */
		  തിരികെ a;
	    }

      }

	/* If one of the signals is unambiguous, then it
	   will sweep up the weaker parts of the ambiguous
	   signal. The result may be ambiguous, or maybe not. */

      എങ്കിൽ (UNAMBIG(a.മൂല്യം_2)) {
	    vvp_scalar_t res;

	    എങ്കിൽ ((a.മൂല്യം_2&0x70) > (b.മൂല്യം_2&0x70))
		  res.മൂല്യം_2 |= a.മൂല്യം_2&0xf0;
	    അല്ലെങ്കിൽ
		  res.മൂല്യം_2 |= b.മൂല്യം_2&0xf0;

	    എങ്കിൽ ((a.മൂല്യം_2&0x07) > (b.മൂല്യം_2&0x07))
		  res.മൂല്യം_2 |= a.മൂല്യം_2&0x0f;
	    അല്ലെങ്കിൽ
		  res.മൂല്യം_2 |= b.മൂല്യം_2&0x0f;

	    തിരികെ res;

      } അല്ലെങ്കിൽ എങ്കിൽ (UNAMBIG(b.മൂല്യം_2)) {

	    vvp_scalar_t res;

	    എങ്കിൽ ((b.മൂല്യം_2&0x70) > (a.മൂല്യം_2&0x70))
		  res.മൂല്യം_2 |= b.മൂല്യം_2&0xf0;
	    അല്ലെങ്കിൽ
		  res.മൂല്യം_2 |= a.മൂല്യം_2&0xf0;

	    എങ്കിൽ ((b.മൂല്യം_2&0x07) > (a.മൂല്യം_2&0x07))
		  res.മൂല്യം_2 |= b.മൂല്യം_2&0x0f;
	    അല്ലെങ്കിൽ
		  res.മൂല്യം_2 |= a.മൂല്യം_2&0x0f;

	    തിരികെ res;

      }


	/* If both signals are ambiguous, then the result
	   has an even wider ambiguity. */

      unsigned tmp = 0;
      പൂർണ്ണസംഖ്യ sv1a = (a.മൂല്യം_2 & 0x80) ? STREN1(a.മൂല്യം_2) : - STREN1(a.മൂല്യം_2);
      പൂർണ്ണസംഖ്യ sv0a = (a.മൂല്യം_2 & 0x08) ? STREN0(a.മൂല്യം_2) : - STREN0(a.മൂല്യം_2);
      പൂർണ്ണസംഖ്യ sv1b = (b.മൂല്യം_2 & 0x80) ? STREN1(b.മൂല്യം_2) : - STREN1(b.മൂല്യം_2);
      പൂർണ്ണസംഖ്യ sv0b = (b.മൂല്യം_2 & 0x08) ? STREN0(b.മൂല്യം_2) : - STREN0(b.മൂല്യം_2);

      പൂർണ്ണസംഖ്യ sv1 = sv1a;
      പൂർണ്ണസംഖ്യ sv0 = sv0a;

      എങ്കിൽ (sv0a > sv1)
	    sv1 = sv0a;
      എങ്കിൽ (sv1b > sv1)
	    sv1 = sv1b;
      എങ്കിൽ (sv0b > sv1)
	    sv1 = sv0b;

      എങ്കിൽ (sv1a < sv0)
	    sv0 = sv1a;
      എങ്കിൽ (sv1b < sv0)
	    sv0 = sv1b;
      എങ്കിൽ (sv0b < sv0)
	    sv0 = sv0b;

      എങ്കിൽ (sv1 > 0) {
	    tmp |= 0x80;
	    tmp |= sv1 << 4;
      } അല്ലെങ്കിൽ {
	      /* Set the MSB when both arguments MSBs are set. This
		 can only happen if both one strengths are zero. */
	    tmp |= (a.മൂല്യം_2&b.മൂല്യം_2)&0x80;
	    tmp |= (-sv1) << 4;
      }

      എങ്കിൽ (sv0 > 0) {
	    tmp |= 0x08;
	    tmp |= sv0;
      } അല്ലെങ്കിൽ {
	    tmp |= (-sv0);
      }

      vvp_scalar_t res;
      res.മൂല്യം_2 = tmp;

	/* Canonicalize the HiZ value. */
      എങ്കിൽ ((res.മൂല്യം_2&0x77) == 0)
	    res.മൂല്യം_2 = 0;

      തിരികെ res;
}

unsigned vvp_switch_strength_map[2][8] = {
      {  // non-resistive
	    0, /* High impedance   --> High impedance   */
	    1, /* Small capacitor  --> Small capacitor  */
	    2, /* Medium capacitor --> Medium capacitor */
	    3, /* Weak drive       --> Weak drive       */
	    4, /* Large capacitor  --> Large capacitor  */
	    5, /* Pull drive       --> Pull drive       */
	    6, /* Strong drive     --> Strong drive     */
	    6  /* Supply drive     --> Strong drive     */
      },
      {  // resistive
	    0, /* High impedance   --> High impedance   */
	    1, /* Small capacitor  --> Small capacitor  */
	    1, /* Medium capacitor --> Small capacitor  */
	    2, /* Weak drive       --> Medium capacitor */
	    2, /* Large capacitor  --> Medium capacitor */
	    3, /* Pull drive       --> Weak drive       */
	    5, /* Strong drive     --> Pull drive       */
	    5  /* Supply drive     --> Pull drive       */
      }
};

vvp_vector4_t reduce4(സ്ഥിരം vvp_vector8_t&that)
{
      vvp_vector4_t out (that.size());
      വേണ്ടി (unsigned idx = 0 ;  idx < out.size() ;  idx += 1)
	    out.set_bit(idx, that.value(idx).value());

      തിരികെ out;
}

vvp_bit4_t compare_gtge(സ്ഥിരം vvp_vector4_t&lef, സ്ഥിരം vvp_vector4_t&rig,
			vvp_bit4_t out_if_equal)
{
      unsigned min_size = lef.size();
      എങ്കിൽ (rig.size() < min_size)
	    min_size = rig.size();

	// If one of the inputs is nil, treat is as all X values, and
	// that makes the result BIT4_X.
      എങ്കിൽ (min_size == 0)
	    തിരികെ BIT4_X;

	// As per the IEEE1364 definition of >, >=, < and <=, if there
	// are any X or Z values in either of the operand vectors,
	// then the result of the compare is BIT4_X.

	// Check for X/Z in the left operand
      എങ്കിൽ (lef.has_xz())
	    തിരികെ BIT4_X;

	// Check for X/Z in the right operand
      എങ്കിൽ (rig.has_xz())
	    തിരികെ BIT4_X;

      വേണ്ടി (unsigned idx = lef.size() ; idx > rig.size() ;  idx -= 1) {
	    എങ്കിൽ (lef.value(idx-1) == BIT4_1)
		  തിരികെ BIT4_1;
      }

      വേണ്ടി (unsigned idx = rig.size() ; idx > lef.size() ;  idx -= 1) {
	    എങ്കിൽ (rig.value(idx-1) == BIT4_1)
		  തിരികെ BIT4_0;
      }

      വേണ്ടി (unsigned idx = min_size ; idx > 0 ;  idx -= 1) {
	    vvp_bit4_t lv = lef.value(idx-1);
	    vvp_bit4_t rv = rig.value(idx-1);

	    എങ്കിൽ (lv == rv)
		  തുടരുക;

	    എങ്കിൽ (lv == BIT4_1)
		  തിരികെ BIT4_1;
	    അല്ലെങ്കിൽ
		  തിരികെ BIT4_0;
      }

      തിരികെ out_if_equal;
}

vvp_bit4_t compare_gtge_signed(സ്ഥിരം vvp_vector4_t&a,
			       സ്ഥിരം vvp_vector4_t&b,
			       vvp_bit4_t out_if_equal)
{
      പ്രതിജ്ഞാ(a.size() == b.size());

      unsigned sign_idx = a.size()-1;
      vvp_bit4_t a_sign = a.value(sign_idx);
      vvp_bit4_t b_sign = b.value(sign_idx);

      എങ്കിൽ (bit4_is_xz(a_sign))
	    തിരികെ BIT4_X;
      എങ്കിൽ (bit4_is_xz(b_sign))
	    തിരികെ BIT4_X;

      എങ്കിൽ (a_sign == b_sign)
	    തിരികെ compare_gtge(a, b, out_if_equal);

      എങ്കിൽ (a.has_xz())
	    തിരികെ BIT4_X;

      എങ്കിൽ (b.has_xz())
	    തിരികെ BIT4_X;

      എങ്കിൽ(a_sign == BIT4_0)
	    തിരികെ BIT4_1;
      അല്ലെങ്കിൽ
	    തിരികെ BIT4_0;
}
