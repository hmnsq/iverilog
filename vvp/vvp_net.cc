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

using নামাকাশ std;

/* This is the size of an unsigned long in bits. This is just a
   convenience macro. */
# define CPU_WORD_BITS (8*পরিমাণ(unsigned long))
# define শীৰ্ষ_BIT (1UL << (CPU_WORD_BITS-1))

permaheap vvp_net_fun_t::heap_;
permaheap vvp_net_fil_t::heap_;

// Allocate around 1Megabyte/chunk.
স্থির ধ্রুৱ size_t VVP_NET_CHUNK = 1024*1024/পরিমাণ(vvp_net_t);
স্থির vvp_net_t*vvp_net_alloc_table = রিক্ত;
#ifdef CHECK_WITH_VALGRIND
স্থির vvp_net_t **vvp_net_pool = রিক্ত;
স্থির unsigned vvp_net_pool_গুনতি = 0;
#endif
স্থির size_t vvp_net_alloc_remaining = 0;
// For statistics, count the vvp_nets allocated and the bytes of alloc
// chunks allocated.
unsigned long গুনতি_vvp_nets = 0;
size_t size_vvp_nets = 0;

শূন্য* vvp_net_t::operator নূতন (size_t size)
{
      প্রতিজ্ঞা(size == পরিমাণ(vvp_net_t));
      যদি (vvp_net_alloc_remaining == 0) {
	    vvp_net_alloc_table = ::নূতন vvp_net_t[VVP_NET_CHUNK];
	    vvp_net_alloc_remaining = VVP_NET_CHUNK;
	    size_vvp_nets += size*VVP_NET_CHUNK;
#ifdef CHECK_WITH_VALGRIND
	    VALGRIND_MAKE_MEM_NOACCESS(vvp_net_alloc_table, size*VVP_NET_CHUNK);
	    VALGRIND_CREATE_MEMPOOL(vvp_net_alloc_table, 0, 0);
	    vvp_net_pool_গুনতি += 1;
	    vvp_net_pool = static_cast<vvp_net_t **>(realloc(vvp_net_pool,
	                   vvp_net_pool_গুনতি*পরিমাণ(vvp_net_t **)));
	    vvp_net_pool[vvp_net_pool_গুনতি-1] = vvp_net_alloc_table;
#endif
      }

      vvp_net_t*return_this = vvp_net_alloc_table;
#ifdef CHECK_WITH_VALGRIND
      VALGRIND_MEMPOOL_ALLOC(vvp_net_pool[vvp_net_pool_গুনতি-1],
                             return_this, size);
      return_this->pool = vvp_net_pool[vvp_net_pool_গুনতি-1];
#endif
      vvp_net_alloc_table += 1;
      vvp_net_alloc_remaining -= 1;
      গুনতি_vvp_nets += 1;
      ঘূৰাই return_this;
}

#ifdef CHECK_WITH_VALGRIND
স্থির map<vvp_net_t*, তার্কিক> vvp_net_map;
স্থির map<sfunc_core*, তার্কিক> sfunc_map;
স্থির map<vvp_udp_fun_core*, তার্কিক> udp_map;
স্থির map<resolv_core*, তার্কিক> resolv_map;
স্থির vvp_net_t **local_net_pool = 0;
স্থির unsigned local_net_pool_গুনতি = 0;

শূন্য pool_local_net(vvp_net_t*net)
{
      local_net_pool_গুনতি += 1;
      local_net_pool = static_cast<vvp_net_t **>(realloc(local_net_pool,
                       local_net_pool_গুনতি*পরিমাণ(vvp_net_t **)));
      local_net_pool[local_net_pool_গুনতি-1] = net;
}

শূন্য vvp_net_delete(vvp_net_t *item)
{
      vvp_net_map[item] = সঁচা;
      যদি (sfunc_core*tmp = dynamic_cast<sfunc_core*> (item->fun)) {
	    sfunc_map[tmp] = সঁচা;
      }
      যদি (vvp_udp_fun_core*tmp = dynamic_cast<vvp_udp_fun_core*> (item->fun)) {
	    udp_map[tmp] = সঁচা;
      }
      যদি (resolv_core*tmp = dynamic_cast<resolv_core*> (item->fun)) {
	    resolv_map[tmp] = সঁচা;
      }
	/* Only delete static object variables. */
      যদি (vvp_fun_signal_object_sa*tmp = dynamic_cast<vvp_fun_signal_object_sa*> (item->fun)) {
	    delete tmp;
      }
	/* Only delete static string variables. */
      যদি (vvp_fun_signal_string_sa*tmp = dynamic_cast<vvp_fun_signal_string_sa*> (item->fun)) {
	    delete tmp;
      }
}

শূন্য vvp_net_pool_delete()
{
      unsigned long vvp_nets_del = 0;

      বাবে (unsigned idx = 0; idx < local_net_pool_গুনতি; idx += 1) {
	    vvp_net_delete(local_net_pool[idx]);
      }
      মুক্ত(local_net_pool);
      local_net_pool = 0;
      local_net_pool_গুনতি = 0;

      map<vvp_net_t*, তার্কিক>::iterator iter;
      বাবে (iter = vvp_net_map.begin(); iter != vvp_net_map.end(); ++ iter ) {
	    vvp_nets_del += 1;
	    VALGRIND_MEMPOOL_FREE(iter->first->pool, iter->first);
      }
      vvp_net_map.clear();

      map<sfunc_core*, তার্কিক>::iterator siter;
      বাবে (siter = sfunc_map.begin(); siter != sfunc_map.end(); ++ siter ) {
	    delete siter->first;
      }
      sfunc_map.clear();

      map<vvp_udp_fun_core*, তার্কিক>::iterator uiter;
      বাবে (uiter = udp_map.begin(); uiter != udp_map.end(); ++ uiter ) {
	    delete uiter->first;
      }
      udp_map.clear();

      map<resolv_core*, তার্কিক>::iterator riter;
      বাবে (riter = resolv_map.begin(); riter != resolv_map.end(); ++ riter ) {
	    delete riter->first;
      }
      resolv_map.clear();

      যদি (RUNNING_ON_VALGRIND && (vvp_nets_del != গুনতি_vvp_nets)) {
	    fflush(রিক্ত);
	    VALGRIND_PRINTF("Error: vvp missed deleting %ld of %lu net(s).",
	                    (long) গুনতি_vvp_nets - vvp_nets_del,
	                    গুনতি_vvp_nets);
      }

      বাবে (unsigned idx = 0; idx < vvp_net_pool_গুনতি; idx += 1) {
	    VALGRIND_DESTROY_MEMPOOL(vvp_net_pool[idx]);
	    ::delete [] vvp_net_pool[idx];
      }
      মুক্ত(vvp_net_pool);
      vvp_net_pool = রিক্ত;
      vvp_net_pool_গুনতি = 0;
}
#endif

শূন্য vvp_net_t::operator delete(শূন্য*)
{
      প্রতিজ্ঞা(0);
}

vvp_net_t::vvp_net_t()
: out_(vvp_net_ptr_t(0,0))
{
      fun = 0;
      fil = 0;
}

শূন্য vvp_net_t::link(vvp_net_ptr_t port_to_link)
{
      vvp_net_t*net = port_to_link.ptr();

	// Connect nodes with vvp_fun_modpath_src to the head of the
	// linked list, so that vvp_fun_modpath_src are always evaluated
	// before their respective vvp_fun_modpath
      যদি (dynamic_cast<vvp_fun_modpath_src*>(net->fun)) {
	    net->port[port_to_link.port()] = out_;
	    out_ = port_to_link;
	// All other nodes connect after the last vvp_fun_modpath_src
      } অন্যথা {
	      // List is empty or first node has no vvp_fun_modpath_src
	      // then just insert the node into the head of linked list
	    যদি (out_.ptr() == nullptr || ! dynamic_cast<vvp_fun_modpath_src*>(out_.ptr()->fun)) {
		  net->port[port_to_link.port()] = out_;
		  out_ = port_to_link;
		  ঘূৰাই;
	    }

	    vvp_net_ptr_t cur = out_;
	    vvp_net_ptr_t prev = vvp_net_ptr_t(nullptr,0);

	      // Traverse through linked list
	    যেতিয়ালৈ (dynamic_cast<vvp_fun_modpath_src*>(cur.ptr()->fun)) {
		  prev = cur;
		  cur = cur.ptr()->port[cur.port()];
		  যদি (! cur.ptr()) ভঙ্গ; // End of list
	    }

	    প্রতিজ্ঞা(prev.ptr());

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
শূন্য vvp_net_t::unlink(vvp_net_ptr_t dst_ptr)
{
      vvp_net_t*net = dst_ptr.ptr();
      unsigned net_port = dst_ptr.port();

      যদি (out_ == dst_ptr) {
	      /* If the drive fan-out list starts with this pointer,
		 then the unlink is easy. Pull the list forward. */
	    out_ = net->port[net_port];
      } অন্যথা যদি (! out_.nil()) {
	      /* Scan the linked list, looking for the net_ptr_t
		 pointer *before* the one we wish to remove. */
	    vvp_net_ptr_t cur = out_;
	    প্রতিজ্ঞা(!cur.nil());
	    vvp_net_t*cur_net = cur.ptr();
	    unsigned cur_port = cur.port();
	    যেতিয়ালৈ (cur_net && cur_net->port[cur_port] != dst_ptr) {
		  cur = cur_net->port[cur_port];
		  cur_net = cur.ptr();
		  cur_port = cur.port();
	    }
	      /* Unlink. */
	    যদি (cur_net) cur_net->port[cur_port] = net->port[net_port];
      }

      net->port[net_port] = vvp_net_ptr_t(0,0);
}

শূন্য vvp_net_t::গুনতি_drivers(unsigned idx, unsigned counts[4])
{
      counts[0] = 0;
      counts[1] = 0;
      counts[2] = 0;
      counts[3] = 0;

        /* $countdrivers can only be used on wires. */
      vvp_wire_base*wire=dynamic_cast<vvp_wire_base*>(fil);
      প্রতিজ্ঞা(wire);

      যদি (wire->is_forced(idx))
            counts[3] = 1;

        /* If the net has multiple drivers, we need to interrogate the
           resolver network to get the driven values. */
      যদি (resolv_core*resolver = dynamic_cast<resolv_core*>(fun)) {
            resolver->গুনতি_drivers(idx, counts);
            ঘূৰাই;
      }
      যদি (vvp_island_port*resolver = dynamic_cast<vvp_island_port*>(fun)) {
            resolver->গুনতি_drivers(idx, counts);
            ঘূৰাই;
      }

        /* If the functor is not a resolver, there is only one driver, so
           we can just interrogate the filter to get the driven value. */
      update_driver_counts(wire->driven_মান(idx), counts);
}

শূন্য vvp_net_fun_t::operator delete(শূন্য*)
{
#ifndef CHECK_WITH_VALGRIND
      প্রতিজ্ঞা(0);
#endif
}


vvp_net_fil_t::vvp_net_fil_t()
{
      force_link_ = 0;
      force_propagate_ = মিছা;
      গুনতি_filters += 1;
}

vvp_net_fil_t::~vvp_net_fil_t()
{
      প্রতিজ্ঞা(force_link_ == 0);
}

vvp_net_fil_t::prop_t vvp_net_fil_t::filter_vec4(ধ্রুৱ vvp_vector4_t&,
                                                 vvp_vector4_t&,
                                                 unsigned, unsigned)
{
      ঘূৰাই PROP;
}

vvp_net_fil_t::prop_t vvp_net_fil_t::filter_vec8(ধ্রুৱ vvp_vector8_t&,
                                                 vvp_vector8_t&,
                                                 unsigned, unsigned)
{
      ঘূৰাই PROP;
}

vvp_net_fil_t::prop_t vvp_net_fil_t::filter_real(দ্ৱিগুণ&)
{
      ঘূৰাই PROP;
}

vvp_net_fil_t::prop_t vvp_net_fil_t::filter_string(ধ্রুৱ string&)
{
      ঘূৰাই PROP;
}

vvp_net_fil_t::prop_t vvp_net_fil_t::filter_object(vvp_object_t&)
{
      ঘূৰাই PROP;
}

শূন্য vvp_net_fil_t::force_mask(ধ্রুৱ vvp_vector2_t&mask)
{
      যদি (force_mask_.size() == 0)
	    force_mask_ = vvp_vector2_t(vvp_vector2_t::FILL0, mask.size());

      প্রতিজ্ঞা(force_mask_.size() == mask.size());
      বাবে (unsigned idx = 0 ; idx < mask.size() ; idx += 1) {
	    যদি (mask.value(idx) == 0)
		  অনুৱর্ত;

	    force_mask_.set_bit(idx, 1);
	    force_propagate_ = সঁচা;
      }
}

শূন্য vvp_net_fil_t::release_mask(ধ্রুৱ vvp_vector2_t&mask)
{
      যদি (force_mask_.size() == 0)
	    ঘূৰাই;

      প্রতিজ্ঞা(force_mask_.size() == mask.size());
      বাবে (unsigned idx = 0 ; idx < mask.size() ; idx += 1) {
	    যদি (mask.value(idx))
		  force_mask_.set_bit(idx, 0);
      }

      যদি (force_mask_.is_zero())
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
শূন্য vvp_net_fil_t::force_link(vvp_net_t*dst, vvp_net_t*src)
{
      প্রতিজ্ঞা(dst->fil == এতত্);

      যদি (force_link_ == 0) {
	    force_link_ = নূতন vvp_net_t;
	      // Use port[3] to hold the force destination.
	    force_link_->port[3] = vvp_net_ptr_t(dst, 0);
	    force_link_->port[2] = vvp_net_ptr_t(0,0);
	    force_link_->fun = নূতন vvp_fun_force;
      }

      force_unlink();
      প্রতিজ্ঞা(force_link_->port[2] == vvp_net_ptr_t(0,0));

	// Use port[2] to hold the force source.
      force_link_->port[2] = vvp_net_ptr_t(src,0);

      vvp_net_ptr_t dst_ptr(force_link_, 0);
      src->link(dst_ptr);
}

শূন্য vvp_net_fil_t::operator delete(শূন্য*)
{
      প্রতিজ্ঞা(0);
}

শূন্য vvp_net_fil_t::force_unlink(শূন্য)
{
      যদি (force_link_ == 0) ঘূৰাই;
      vvp_net_t*src = force_link_->port[2].ptr();
      যদি (src == 0) ঘূৰাই;

      src->unlink(vvp_net_ptr_t(force_link_,0));
      force_link_->port[2] = vvp_net_ptr_t(0,0);
}

/* *** BIT operations *** */
vvp_bit4_t add_with_carry(vvp_bit4_t a, vvp_bit4_t b, vvp_bit4_t&c)
{
      যদি (bit4_is_xz(a) || bit4_is_xz(b) || bit4_is_xz(c)) {
	    c = BIT4_X;
	    ঘূৰাই BIT4_X;
      }

	// NOTE: This relies on the facts that XZ values have been
	// weeded out, and that BIT4_1 is 1 and BIT4_0 is 0.
      পূর্ণাঙ্ক sum = (পূর্ণাঙ্ক)a + (পূর্ণাঙ্ক)b + (পূর্ণাঙ্ক)c;

      ৱিকল্প (sum) {
	  পক্ষ 0:
	      // c must already be 0.
	    ঘূৰাই BIT4_0;
	  পক্ষ 1:
	    c = BIT4_0;
	    ঘূৰাই BIT4_1;
	  পক্ষ 2:
	    c = BIT4_1;
	    ঘূৰাই BIT4_0;
	  পক্ষ 3:
	    c = BIT4_1;
	    ঘূৰাই BIT4_1;
      }
      fprintf(stderr, "Incorrect result %d.\n", sum);
      প্রতিজ্ঞা(0);
      ঘূৰাই BIT4_X;
}

vvp_bit4_t scalar_to_bit4(PLI_INT32 scalar)
{
      ৱিকল্প(scalar) {
	  পক্ষ vpi0:
	    ঘূৰাই BIT4_0;
	  পক্ষ vpi1:
	    ঘূৰাই BIT4_1;
	  পক্ষ vpiX:
	    ঘূৰাই BIT4_X;
	  পক্ষ vpiZ:
	    ঘূৰাই BIT4_Z;
      }
      fprintf(stderr, "Unsupported scalar value %d.\n", (পূর্ণাঙ্ক)scalar);
      প্রতিজ্ঞা(0);
      ঘূৰাই BIT4_X;
}

vvp_bit4_t operator ^ (vvp_bit4_t a, vvp_bit4_t b)
{
      যদি (bit4_is_xz(a))
	    ঘূৰাই BIT4_X;
      যদি (bit4_is_xz(b))
	    ঘূৰাই BIT4_X;
      যদি (a == BIT4_0)
	    ঘূৰাই b;
      যদি (b == BIT4_0)
	    ঘূৰাই a;
      ঘূৰাই BIT4_0;
}

ostream& operator<<(ostream&out, vvp_bit4_t bit)
{
      ৱিকল্প (bit) {
	  পক্ষ BIT4_0:
	    out << "0";
	    ভঙ্গ;
	  পক্ষ BIT4_1:
	    out << "1";
	    ভঙ্গ;
	  পক্ষ BIT4_X:
	    out << "X";
	    ভঙ্গ;
	  পক্ষ BIT4_Z:
	    out << "Z";
	    ভঙ্গ;
	  স্ৱভাৱ:
	    out << "?";
	    ভঙ্গ;
      }
      ঘূৰাই out;
}

প্রকারনির্দেশ unsigned short edge_t;

অন্তঃস্থ edge_t VVP_EDGE(vvp_bit4_t from, vvp_bit4_t to)
{
      ঘূৰাই 1 << ((from << 2) | to);
}

ধ্রুৱ edge_t vvp_edge_posedge
      = VVP_EDGE(BIT4_0,BIT4_1)
      | VVP_EDGE(BIT4_0,BIT4_X)
      | VVP_EDGE(BIT4_0,BIT4_Z)
      | VVP_EDGE(BIT4_X,BIT4_1)
      | VVP_EDGE(BIT4_Z,BIT4_1)
      ;

ধ্রুৱ edge_t vvp_edge_negedge
      = VVP_EDGE(BIT4_1,BIT4_0)
      | VVP_EDGE(BIT4_1,BIT4_X)
      | VVP_EDGE(BIT4_1,BIT4_Z)
      | VVP_EDGE(BIT4_X,BIT4_0)
      | VVP_EDGE(BIT4_Z,BIT4_0)
      ;

পূর্ণাঙ্ক edge(vvp_bit4_t from, vvp_bit4_t to)
{
      edge_t mask = VVP_EDGE(from, to);
      যদি (mask & vvp_edge_posedge)
	    ঘূৰাই 1;
      যদি (mask & vvp_edge_negedge)
	    ঘূৰাই -1;
      ঘূৰাই 0;
}

unsigned long multiply_with_carry(unsigned long a, unsigned long b,
				  unsigned long&carry)
{
      ধ্রুৱ unsigned long mask = (1UL << (CPU_WORD_BITS/2)) - 1;
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
      ঘূৰাই (r1 << (CPU_WORD_BITS/2)) + r00;
}


শূন্য vvp_send_vec8(vvp_net_ptr_t ptr, ধ্রুৱ vvp_vector8_t&val)
{
      যেতিয়ালৈ (vvp_net_t*cur = ptr.ptr()) {
	    vvp_net_ptr_t next_val = cur->port[ptr.port()];

	    যদি (cur->fun)
		  cur->fun->recv_vec8(ptr, val);

	    ptr = next_val;
      }
}

শূন্য vvp_send_real(vvp_net_ptr_t ptr, দ্ৱিগুণ val, vvp_context_t context)
{
      যেতিয়ালৈ (vvp_net_t*cur = ptr.ptr()) {
	    vvp_net_ptr_t next_val = cur->port[ptr.port()];

	    যদি (cur->fun)
		  cur->fun->recv_real(ptr, val, context);

	    ptr = next_val;
      }
}

ধ্রুৱ vvp_vector4_t vvp_vector4_t::nil;

শূন্য vvp_vector4_t::copy_bits(ধ্রুৱ vvp_vector4_t&that)
{

      যদি (size_ == that.size_) {
	    যদি (size_ > BITS_PER_WORD) {
		  unsigned শব্দগুলি = (size_+BITS_PER_WORD-1) / BITS_PER_WORD;
		  বাবে (unsigned idx = 0 ;  idx < শব্দগুলি ;  idx += 1)
			abits_ptr_[idx] = that.abits_ptr_[idx];
		  বাবে (unsigned idx = 0 ;  idx < শব্দগুলি ;  idx += 1)
			bbits_ptr_[idx] = that.bbits_ptr_[idx];
	    } অন্যথা {
		  abits_val_ = that.abits_val_;
		  bbits_val_ = that.bbits_val_;
	    }
	    ঘূৰাই;
      }

	/* Now we know that the sizes of this and that are definitely
	   different. We can use that in code below. In any case, we
	   need to copy only the smaller of the sizes. */

	/* If source and destination are both short, then mask/copy
	   the bit values. */
      যদি (size_ <= BITS_PER_WORD && that.size_ <= BITS_PER_WORD) {
	    unsigned bits_to_copy = (that.size_ < size_) ? that.size_ : size_;
	    unsigned long mask = ((bits_to_copy < BITS_PER_WORD) ? (1UL << bits_to_copy) :
	                                                           (0UL)) - 1UL;
	    abits_val_ &= ~mask;
	    bbits_val_ &= ~mask;
	    abits_val_ |= that.abits_val_&mask;
	    bbits_val_ |= that.bbits_val_&mask;
	    ঘূৰাই;
      }

	/* Now we know that either source or destination are long. If
	   the destination is short, then mask/copy from the low word
	   of the long source. */
      যদি (size_ <= BITS_PER_WORD) {
	    abits_val_ = that.abits_ptr_[0];
	    bbits_val_ = that.bbits_ptr_[0];
	    যদি (size_ < BITS_PER_WORD) {
		  unsigned long mask = (1UL << size_) - 1UL;
		  abits_val_ &= mask;
		  bbits_val_ &= mask;
	    }
	    ঘূৰাই;
      }

	/* Now we know that the destination must be long. If the
	   source is short, then mask/copy from its value. */
      যদি (that.size_ <= BITS_PER_WORD) {
	    unsigned long mask;
	    যদি (that.size_ < BITS_PER_WORD) {
		  mask = (1UL << that.size_) - 1UL;
		  abits_ptr_[0] &= ~mask;
		  bbits_ptr_[0] &= ~mask;
	    } অন্যথা {
		  mask = -1UL;
	    }
	    abits_ptr_[0] |= that.abits_val_&mask;
	    bbits_ptr_[0] |= that.bbits_val_&mask;
	    ঘূৰাই;
      }

	/* Finally, we know that source and destination are long. copy
	   words until we get to the last. */
      unsigned bits_to_copy = (that.size_ < size_) ? that.size_ : size_;
      unsigned word = 0;
      যেতিয়ালৈ (bits_to_copy >= BITS_PER_WORD) {
	    abits_ptr_[word] = that.abits_ptr_[word];
	    bbits_ptr_[word] = that.bbits_ptr_[word];
	    bits_to_copy -= BITS_PER_WORD;
	    word += 1;
      }
      যদি (bits_to_copy > 0) {
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
শূন্য vvp_vector4_t::copy_from_big_(ধ্রুৱ vvp_vector4_t&that)
{
      unsigned শব্দগুলি = (size_+BITS_PER_WORD-1) / BITS_PER_WORD;
      abits_ptr_ = নূতন unsigned long[2*শব্দগুলি];
      bbits_ptr_ = abits_ptr_ + শব্দগুলি;

      বাবে (unsigned idx = 0 ;  idx < শব্দগুলি ;  idx += 1)
	    abits_ptr_[idx] = that.abits_ptr_[idx];
      বাবে (unsigned idx = 0 ;  idx < শব্দগুলি ;  idx += 1)
	    bbits_ptr_[idx] = that.bbits_ptr_[idx];
}

/*
 * The copy_inverted_from_ method is just like the copy_from_ method,
 * except that we combine that with an invert. This allows the ~ and
 * the assignment to be blended in many common cases.
 */
শূন্য vvp_vector4_t::copy_inverted_from_(ধ্রুৱ vvp_vector4_t&that)
{
      size_ = that.size_;
      যদি (size_ > BITS_PER_WORD) {
	    unsigned শব্দগুলি = (size_+BITS_PER_WORD-1) / BITS_PER_WORD;
	    abits_ptr_ = নূতন unsigned long[2*শব্দগুলি];
	    bbits_ptr_ = abits_ptr_ + শব্দগুলি;

	    unsigned remaining = size_;
	    unsigned idx = 0;
	    যেতিয়ালৈ (remaining >= BITS_PER_WORD) {
		  abits_ptr_[idx] = that.bbits_ptr_[idx] | ~that.abits_ptr_[idx];
		  idx += 1;
		  remaining -= BITS_PER_WORD;
	    }
	    যদি (remaining > 0) {
		  unsigned long mask = (1UL<<remaining) - 1UL;
		  abits_ptr_[idx] = mask & (that.bbits_ptr_[idx] | ~that.abits_ptr_[idx]);
	    }

	    বাবে (idx = 0 ;  idx < শব্দগুলি ;  idx += 1)
		  bbits_ptr_[idx] = that.bbits_ptr_[idx];

      } অন্যথা {
	    unsigned long mask = (size_<BITS_PER_WORD)? (1UL<<size_)-1UL : -1UL;
	    abits_val_ = mask & (that.bbits_val_ | ~that.abits_val_);
	    bbits_val_ = that.bbits_val_;
      }
}

/* Make sure to set size_ before calling this routine. */
শূন্য vvp_vector4_t::allocate_শব্দগুলি(unsigned long inita, unsigned long initb)
{
      যদি (size_ > BITS_PER_WORD) {
	    unsigned cnt = (size_ + BITS_PER_WORD - 1) / BITS_PER_WORD;
	    abits_ptr_ = নূতন unsigned long[2*cnt];
	    bbits_ptr_ = abits_ptr_ + cnt;
	    বাবে (unsigned idx = 0 ;  idx < cnt ;  idx += 1)
		  abits_ptr_[idx] = inita;
	    বাবে (unsigned idx = 0 ;  idx < cnt ;  idx += 1)
		  bbits_ptr_[idx] = initb;

      } অন্যথা {
	    abits_val_ = inita;
	    bbits_val_ = initb;
      }
}

vvp_vector4_t::vvp_vector4_t(unsigned size__, দ্ৱিগুণ val)
: size_(size__)
{
      তার্কিক is_neg = মিছা;
      দ্ৱিগুণ fraction;
      পূর্ণাঙ্ক exponent;

	/* We return 'bx for a NaN or +/- infinity. */
      যদি (val != val || (val && (val == 0.5*val)))  {
	    allocate_শব্দগুলি(WORD_X_ABITS, WORD_X_BBITS);
	    ঘূৰাই;
      }

	/* Convert to a positive result. */
      যদি (val < 0.0) {
	    is_neg = সঁচা;
	    val = -val;
      }
      allocate_শব্দগুলি(WORD_0_ABITS, WORD_0_BBITS);

	/* Get the exponent and fractional part of the number. */
      fraction = frexp(val, &exponent);

	/* If the value is small enough just use lround(). */
      যদি (exponent < BITS_PER_WORD-2) {
	    যদি (is_neg) এতত্->invert();  // Invert the bits if negative.
	    long sval = lround(val);
	    যদি (is_neg) sval = -sval;
	      /* This requires that 0 and 1 have the same bbit value. */
	    যদি (size_ > BITS_PER_WORD) {
		  abits_ptr_[0] = sval;
	    } অন্যথা {
		  abits_val_ = sval;
	    }
	    ঘূৰাই;
      }

      unsigned nwords = (exponent-1) / BITS_PER_WORD;
      unsigned my_শব্দগুলি = (size_ + BITS_PER_WORD - 1) / BITS_PER_WORD - 1;

      fraction = ldexp(fraction, (exponent-1) % BITS_PER_WORD + 1);

	/* Skip any leading bits. */
      বাবে (পূর্ণাঙ্ক idx = (signed) nwords; idx > (signed) my_শব্দগুলি; idx -=1) {
	    unsigned long bits = (unsigned long) fraction;
	    fraction = fraction - (দ্ৱিগুণ) bits;
	    fraction = ldexp(fraction, BITS_PER_WORD);
      }

	/* Convert the remaining bits as appropriate. */
      যদি (my_শব্দগুলি == 0) {
		  unsigned long bits = (unsigned long) fraction;
		  abits_val_ = bits;
		  fraction = fraction - (দ্ৱিগুণ) bits;
		    /* Round any fractional part up. */
		  যদি (fraction >= 0.5) *এতত্ += (int64_t) 1;
      } অন্যথা {
	    যদি (nwords < my_শব্দগুলি) my_শব্দগুলি = nwords;
	    বাবে (পূর্ণাঙ্ক idx = (signed)my_শব্দগুলি; idx >= 0; idx -= 1) {
		  unsigned long bits = (unsigned long) fraction;
		  abits_ptr_[idx] = bits;
		  fraction = fraction - (দ্ৱিগুণ) bits;
		  fraction = ldexp(fraction, BITS_PER_WORD);
	    }
	      /* Round any fractional part up. */
	    যদি (fraction >= ldexp(0.5, BITS_PER_WORD)) *এতত্ += (int64_t) 1;
       }

	/* Convert to a negative number if needed. */
      যদি (is_neg) {
	    এতত্->invert();
	    *এতত্ += (int64_t) 1;
      }
}

vvp_vector4_t::vvp_vector4_t(ধ্রুৱ vvp_vector4_t&that,
			    unsigned adr, unsigned wid)
{
	// Set up and initialize the destination.
      size_ = wid;
      allocate_শব্দগুলি(WORD_X_ABITS, WORD_X_BBITS);

	// Special case: selecting from far beyond the source vector,
	// to the result is all X bits. We're done.
      যদি (adr >= that.size_)
	    ঘূৰাই;

	// Special case: The source is not quite big enough to supply
	// all bits, so get the bits that we can. The remainder will
	// be left at BIT4_X.
      যদি ((adr + wid) > that.size_) {
	    unsigned use_wid = that.size_ - adr;
	    বাবে (unsigned idx = 0 ; idx < use_wid ; idx += 1)
		  set_bit(idx, that.value(adr+idx));

	    ঘূৰাই;
      }

	// At the point, we know that the source part is entirely
	// contained in the source vector.
	// assert((adr + wid) <= that.size_);

      যদি (wid > BITS_PER_WORD) {
	      /* In this case, the subvector and the source vector are
		 long. Do the transfer reasonably efficiently. */
	    unsigned ptr = adr / BITS_PER_WORD;
	    unsigned long off = adr % BITS_PER_WORD;
	    unsigned long noff = BITS_PER_WORD - off;
	    unsigned long lmask = (1UL << off) - 1UL;
	    unsigned trans = 0;
	    unsigned dst = 0;
	    যেতিয়ালৈ (trans < wid) {
		    // The low bits of the result.
		  abits_ptr_[dst] = (that.abits_ptr_[ptr] & ~lmask) >> off;
		  bbits_ptr_[dst] = (that.bbits_ptr_[ptr] & ~lmask) >> off;
		  trans += noff;

		  যদি (trans >= wid)
			ভঙ্গ;

		  ptr += 1;

		    // The high bits of the result. Skip this if the
		    // source and destination are perfectly aligned.
		  যদি (noff != BITS_PER_WORD) {
			abits_ptr_[dst] |= (that.abits_ptr_[ptr]&lmask) << noff;
			bbits_ptr_[dst] |= (that.bbits_ptr_[ptr]&lmask) << noff;
			trans += off;
		  }

		  dst += 1;
	    }

      } অন্যথা যদি (that.size_ > BITS_PER_WORD) {
	      /* In this case, the subvector fits in a single word,
		 but the source is large. */
	    unsigned ptr = adr / BITS_PER_WORD;
	    unsigned long off = adr % BITS_PER_WORD;
	    unsigned trans = BITS_PER_WORD - off;
	    যদি (trans > wid)
		  trans = wid;

	    যদি (trans == BITS_PER_WORD) {
		    // Very special case: Copy exactly 1 perfectly
		    // aligned word.
		  abits_val_ = that.abits_ptr_[ptr];
		  bbits_val_ = that.bbits_ptr_[ptr];

	    } অন্যথা {
		    // lmask is the low bits of the destination,
		    // masked into the source.
		  unsigned long lmask = (1UL<<trans) - 1UL;
		  lmask <<= off;

		    // The low bits of the result.
		  abits_val_ = (that.abits_ptr_[ptr] & lmask) >> off;
		  bbits_val_ = (that.bbits_ptr_[ptr] & lmask) >> off;

		  যদি (trans < wid) {
			  // If there are more bits, then get them
			  // from the bottom of the next word of the
			  // source.
			unsigned long hmask = (1UL << (wid-trans)) - 1UL;

			  // The high bits of the result.
			abits_val_ |= (that.abits_ptr_[ptr+1]&hmask) << trans;
			bbits_val_ |= (that.bbits_ptr_[ptr+1]&hmask) << trans;
		  }
	    }

      } অন্যথা যদি (size_ == BITS_PER_WORD) {
	      /* We know that source and destination are short. If the
		 destination is a full word, then we know the copy is
		 aligned and complete. */
	    abits_val_ = that.abits_val_;
	    bbits_val_ = that.bbits_val_;

      } অন্যথা {
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
শূন্য vvp_vector4_t::resize(unsigned newsize, vvp_bit4_t pad_bit)
{
      যদি (size_ == newsize)
	    ঘূৰাই;

      unsigned cnt = (size_ + BITS_PER_WORD - 1) / BITS_PER_WORD;
      unsigned long word_pad_abits = 0;
      unsigned long word_pad_bbits = 0;
      ৱিকল্প (pad_bit) {
	  পক্ষ BIT4_0:
	    word_pad_abits = WORD_0_ABITS;
	    word_pad_bbits = WORD_0_BBITS;
	    ভঙ্গ;
	  পক্ষ BIT4_1:
	    word_pad_abits = WORD_1_ABITS;
	    word_pad_bbits = WORD_1_BBITS;
	    ভঙ্গ;
	  পক্ষ BIT4_X:
	    word_pad_abits = WORD_X_ABITS;
	    word_pad_bbits = WORD_X_BBITS;
	    ভঙ্গ;
	  পক্ষ BIT4_Z:
	    word_pad_abits = WORD_Z_ABITS;
	    word_pad_bbits = WORD_Z_BBITS;
	    ভঙ্গ;
      }

      যদি (newsize > BITS_PER_WORD) {
	    unsigned newcnt = (newsize + BITS_PER_WORD - 1) / BITS_PER_WORD;
	    যদি (newcnt == cnt) {
		    // If the word count doesn't change, then there is
		    // no need for re-allocation so we are done now.
		  যদি (newsize > size_) {
			যদি (unsigned fill = size_ % BITS_PER_WORD) {
			      abits_ptr_[cnt-1] &= ~((-1UL) << fill);
			      bbits_ptr_[cnt-1] &= ~((-1UL) << fill);
			      abits_ptr_[cnt-1] |= word_pad_abits << fill;
			      bbits_ptr_[cnt-1] |= word_pad_bbits << fill;
			}
		  }
		  size_ = newsize;
		  ঘূৰাই;
	    }

	    unsigned long*newbits = নূতন unsigned long[2*newcnt];

	    যদি (cnt > 1) {
		  unsigned trans = cnt;
		  যদি (trans > newcnt)
			trans = newcnt;

		  বাবে (unsigned idx = 0 ;  idx < trans ;  idx += 1)
			newbits[idx] = abits_ptr_[idx];
		  বাবে (unsigned idx = 0 ;  idx < trans ;  idx += 1)
			newbits[newcnt+idx] = bbits_ptr_[idx];

		  delete[]abits_ptr_;

	    } অন্যথা {
		  newbits[0] = abits_val_;
		  newbits[newcnt] = bbits_val_;
	    }

	    যদি (newsize > size_) {
		  যদি (unsigned fill = size_ % BITS_PER_WORD) {
			newbits[cnt-1] &= ~((-1UL) << fill);
			newbits[cnt-1] |= word_pad_abits << fill;
			newbits[newcnt+cnt-1] &= ~((-1UL) << fill);
			newbits[newcnt+cnt-1] |= word_pad_bbits << fill;
		  }
		  বাবে (unsigned idx = cnt ;  idx < newcnt ;  idx += 1)
			newbits[idx] = word_pad_abits;
		  বাবে (unsigned idx = cnt ;  idx < newcnt ;  idx += 1)
			newbits[newcnt+idx] = word_pad_bbits;
	    }

	    size_ = newsize;
	    abits_ptr_ = newbits;
	    bbits_ptr_ = newbits + newcnt;

      } অন্যথা {
	    যদি (cnt > 1) {
		  unsigned long newvala = abits_ptr_[0];
		  unsigned long newvalb = bbits_ptr_[0];
		  delete[]abits_ptr_;
		  abits_val_ = newvala;
		  bbits_val_ = newvalb;
	    }

	    যদি (newsize > size_) {
		  abits_val_ &= ~((-1UL) << size_);
		  bbits_val_ &= ~((-1UL) << size_);
		  abits_val_ |= word_pad_abits << size_;
		  bbits_val_ |= word_pad_bbits << size_;
	    }

	    size_ = newsize;
      }
}


unsigned long* vvp_vector4_t::subarray(unsigned adr, unsigned wid, তার্কিক xz_to_0) ধ্রুৱ
{
      ধ্রুৱ unsigned BIT2_PER_WORD = 8*পরিমাণ(unsigned long);
      unsigned awid = (wid + BIT2_PER_WORD - 1) / (BIT2_PER_WORD);
      unsigned long*val = নূতন unsigned long[awid];

      বাবে (unsigned idx = 0 ;  idx < awid ;  idx += 1)
	    val[idx] = 0;

      যদি (size_ <= BITS_PER_WORD) {
	      /* Handle the special case that the array is small. The
		 entire value of the vector4 is within the xbits_val_
		 so we know that the result is a single word, the
		 source is a single word, and we just have to loop
		 through that word. */
	    unsigned long atmp = abits_val_ >> adr;
	    unsigned long btmp = bbits_val_ >> adr;
	    যদি (wid < BIT2_PER_WORD) {
		  atmp &= (1UL << wid) - 1;
		  btmp &= (1UL << wid) - 1;
	    }
	    যদি (btmp) {
		  যদি (xz_to_0)
			atmp &= ~btmp;
		  অন্যথা
			গচ্ছ x_out;
	    }

	    val[0] = atmp;

      } অন্যথা {

	    unsigned val_ptr = 0;
	    unsigned val_off = 0;

	      /* Get the first word we are scanning. We may in fact be
		 somewhere in the middle of that word. */
	    যেতিয়ালৈ (wid > 0) {
		  unsigned long atmp = abits_ptr_[adr/BITS_PER_WORD];
		  unsigned long btmp = bbits_ptr_[adr/BITS_PER_WORD];
		  unsigned long off = adr%BITS_PER_WORD;
		  atmp >>= off;
		  btmp >>= off;

		  unsigned long trans = BITS_PER_WORD - off;
		  যদি (trans > (BIT2_PER_WORD - val_off))
			trans = BIT2_PER_WORD - val_off;
		  যদি (wid < trans)
			trans = wid;
		  যদি (trans < BIT2_PER_WORD) {
			atmp &= (1UL << trans) - 1;
			btmp &= (1UL << trans) - 1;
		  }
		  যদি (btmp) {
			যদি (xz_to_0)
			      atmp &= ~btmp;
			অন্যথা
			      গচ্ছ x_out;
		  }

		  val[val_ptr] |= atmp << val_off;
		  adr += trans;
		  wid -= trans;
		  val_off += trans;
		  যদি (val_off == BIT2_PER_WORD) {
			val_ptr += 1;
			val_off = 0;
		  }
	    }
      }

      ঘূৰাই val;

 x_out:
      delete[]val;
      ঘূৰাই 0;
}

শূন্য vvp_vector4_t::setarray(unsigned adr, unsigned wid, ধ্রুৱ unsigned long*val)
{
      প্রতিজ্ঞা(adr+wid <= size_);

      ধ্রুৱ unsigned BIT2_PER_WORD = 8*পরিমাণ(unsigned long);

      যদি (size_ <= BITS_PER_WORD) {
	      // We know here that both the source and the target are
	      // within a single word. Write the bits into the
	      // abits_val_ directly.

	    প্রতিজ্ঞা(BIT2_PER_WORD <= BITS_PER_WORD);
	    unsigned long lmask = (1UL << adr) - 1UL;
	    unsigned long hmask = ((adr+wid) < BITS_PER_WORD)
		  ? -1UL << (adr+wid)
		  : 0;
	    unsigned long mask = ~(hmask | lmask);

	    abits_val_ &= ~mask;
	    bbits_val_ &= ~mask;

	    abits_val_ |= mask & (val[0] << adr);

      } অন্যথা {
	      // The general case, there are multiple words of
	      // destination, and possibly multiple words of source
	      // data. Shift and mask as we go.
	    unsigned off = adr % BITS_PER_WORD;
	    unsigned ptr = adr / BITS_PER_WORD;
	    unsigned val_off = 0;
	    unsigned val_ptr = 0;
	    যেতিয়ালৈ (wid > 0) {
		  unsigned trans = wid;
		  যদি (trans > (BIT2_PER_WORD-val_off))
			trans = BIT2_PER_WORD-val_off;
		  যদি (trans > (BITS_PER_WORD-off))
			trans = BITS_PER_WORD-off;

		  unsigned long lmask = (1UL << off) - 1UL;
		  unsigned long hmask = ((off+trans) < BITS_PER_WORD)
			? -1UL << (off+trans)
			: 0;
		  unsigned long mask = ~(hmask | lmask);

		  abits_ptr_[ptr] &= ~mask;
		  bbits_ptr_[ptr] &= ~mask;
		  যদি (val_off >= off)
			abits_ptr_[ptr] |= mask & (val[val_ptr] >> (val_off-off));
		  অন্যথা
			abits_ptr_[ptr] |= mask & (val[val_ptr] << (off-val_off));

		  wid -= trans;
		  val_off += trans;
		  যদি (val_off == BIT2_PER_WORD) {
			val_off = 0;
			val_ptr += 1;
		  }
		  off += trans;
		  যদি (off == BITS_PER_WORD) {
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
তার্কিক vvp_vector4_t::set_vec(unsigned adr, ধ্রুৱ vvp_vector4_t&that)
{
      প্রতিজ্ঞা(adr+that.size_  <= size_);
      তার্কিক diff_flag = মিছা;

      যদি (size_ <= BITS_PER_WORD) {

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
	    যদি (hshift >= BITS_PER_WORD)
		  hmask = -1UL;
	    অন্যথা
		  hmask = (1UL << (adr+that.size_)) - 1;
	    unsigned long mask = hmask & ~lmask;

	    unsigned long tmp = (that.abits_val_<<adr)&mask;
	    যদি ((abits_val_&mask) != tmp) {
		  diff_flag = সঁচা;
		  abits_val_ = (abits_val_ & ~mask) | tmp;
	    }
	    tmp = (that.bbits_val_<<adr) & mask;
	    যদি ((bbits_val_&mask) != tmp) {
		  diff_flag = সঁচা;
		  bbits_val_ = (bbits_val_ & ~mask) | tmp;
	    }

      } অন্যথা যদি (that.size_ <= BITS_PER_WORD) {

	      /* This vector is more than a word, but that vector is
		 still small. Write into the destination, possibly
		 spanning two destination words, depending on whether
		 the source vector spans a word transition. */
	    unsigned long dptr = adr / BITS_PER_WORD;
	    unsigned long doff = adr % BITS_PER_WORD;

	    unsigned long lmask = (1UL << doff) - 1;
	    unsigned long hshift = doff+that.size_;
	    unsigned long hmask;
	    যদি (hshift >= BITS_PER_WORD)
		  hmask = -1UL;
	    অন্যথা
		  hmask = (1UL << hshift) - 1UL;

	    unsigned long mask = hmask & ~lmask;
	    unsigned long tmp;

	    tmp = (that.abits_val_ << doff) & mask;
	    যদি ((abits_ptr_[dptr] & mask) != tmp) {
		  diff_flag = সঁচা;
		  abits_ptr_[dptr] = (abits_ptr_[dptr] & ~mask) | tmp;
	    }
	    tmp = (that.bbits_val_ << doff) & mask;
	    যদি ((bbits_ptr_[dptr] & mask) != tmp) {
		  diff_flag = সঁচা;
		  bbits_ptr_[dptr] = (bbits_ptr_[dptr] & ~mask) | tmp;
	    }

	    যদি ((doff + that.size_) > BITS_PER_WORD) {
		  unsigned tail = doff + that.size_ - BITS_PER_WORD;
		  mask = (1UL << tail) - 1;

		  dptr += 1;
		  tmp = (that.abits_val_ >> (that.size_-tail)) & mask;
		  যদি ((abits_ptr_[dptr] & mask) != tmp) {
			diff_flag = সঁচা;
			abits_ptr_[dptr] = (abits_ptr_[dptr] & ~mask) | tmp;
		  }
		  tmp = (that.bbits_val_ >> (that.size_-tail)) & mask;
		  যদি ((bbits_ptr_[dptr] & mask) != tmp) {
			diff_flag = সঁচা;
			bbits_ptr_[dptr] = (bbits_ptr_[dptr] & ~mask) | tmp;
		  }
	    }

      } অন্যথা যদি (adr%BITS_PER_WORD == 0) {

	      /* In this case, both vectors are long, but the
		 destination is neatly aligned. That means all but the
		 last word can be simply copied with no masking. */

	    unsigned remain = that.size_;
	    unsigned sptr = 0;
	    unsigned dptr = adr / BITS_PER_WORD;
	    যেতিয়ালৈ (remain >= BITS_PER_WORD) {
		  যদি (abits_ptr_[dptr] != that.abits_ptr_[sptr]) {
			diff_flag = সঁচা;
			abits_ptr_[dptr] = that.abits_ptr_[sptr];
		  }
		  যদি (bbits_ptr_[dptr] != that.bbits_ptr_[sptr]) {
			diff_flag = সঁচা;
			bbits_ptr_[dptr] = that.bbits_ptr_[sptr];
		  }
		  dptr += 1;
		  sptr += 1;
		  remain -= BITS_PER_WORD;
	    }

	    যদি (remain > 0) {
		  unsigned long mask = (1UL << remain) - 1;
		  unsigned long tmp;

		  tmp = that.abits_ptr_[sptr] & mask;
		  যদি ((abits_ptr_[dptr] & mask) != tmp) {
			diff_flag = সঁচা;
			abits_ptr_[dptr] = (abits_ptr_[dptr] & ~mask) | tmp;
		  }
		  tmp = that.bbits_ptr_[sptr] & mask;
		  যদি ((bbits_ptr_[dptr] & mask) != tmp) {
			diff_flag = সঁচা;
			bbits_ptr_[dptr] = (bbits_ptr_[dptr] & ~mask) | tmp;
		  }
	    }

      } অন্যথা {

	      /* We know that there are two long vectors, and we know
		 that the destination is definitely NOT aligned. */

	    unsigned remain = that.size_;
	    unsigned sptr = 0;
	    unsigned dptr = adr / BITS_PER_WORD;
	    unsigned doff = adr % BITS_PER_WORD;
	    unsigned long lmask = (1UL << doff) - 1;
	    unsigned ndoff = BITS_PER_WORD - doff;
	    যেতিয়ালৈ (remain >= BITS_PER_WORD) {
		  unsigned long tmp;

		  tmp = (that.abits_ptr_[sptr] << doff) & ~lmask;
		  যদি ((abits_ptr_[dptr] & ~lmask) != tmp) {
			diff_flag = সঁচা;
			abits_ptr_[dptr] = (abits_ptr_[dptr] & lmask) | tmp;
		  }
		  tmp = (that.bbits_ptr_[sptr] << doff) & ~lmask;
		  যদি ((bbits_ptr_[dptr] & ~lmask) != tmp) {
			diff_flag = সঁচা;
			bbits_ptr_[dptr] = (bbits_ptr_[dptr] & lmask) | tmp;
		  }
		  dptr += 1;

		  tmp = (that.abits_ptr_[sptr] >> ndoff) & lmask;
		  যদি ((abits_ptr_[dptr] & lmask) != tmp) {
			diff_flag = সঁচা;
			abits_ptr_[dptr] = (abits_ptr_[dptr] & ~lmask) | tmp;
		  }
		  tmp = (that.bbits_ptr_[sptr] >> ndoff) & lmask;
		  যদি ((bbits_ptr_[dptr] & lmask) != tmp) {
			diff_flag = সঁচা;
			bbits_ptr_[dptr] = (bbits_ptr_[dptr] & ~lmask) | tmp;
		  }

		  remain -= BITS_PER_WORD;
		  sptr += 1;
	    }

	    যদি (remain > 0) {
		  unsigned long hshift = doff+remain;
		  unsigned long hmask;
		  যদি (hshift >= BITS_PER_WORD)
			hmask = -1UL;
		  অন্যথা
			hmask = (1UL << (doff+remain)) - 1;

		  unsigned long mask = hmask & ~lmask;
		  unsigned long tmp;

		  tmp = (that.abits_ptr_[sptr] << doff) & mask;
		  যদি ((abits_ptr_[dptr] & mask) != tmp) {
			diff_flag = সঁচা;
			abits_ptr_[dptr] = (abits_ptr_[dptr] & ~mask) | tmp;
		  }
		  tmp = (that.bbits_ptr_[sptr] << doff) & mask;
		  যদি ((bbits_ptr_[dptr] & mask) != tmp) {
			diff_flag = সঁচা;
			bbits_ptr_[dptr] = (bbits_ptr_[dptr] & ~mask) | tmp;
		  }

		  যদি ((doff + remain) > BITS_PER_WORD) {
			unsigned tail = doff + remain - BITS_PER_WORD;
			যদি (tail >= BITS_PER_WORD)
			      mask = -1UL;
			অন্যথা
			      mask = (1UL << tail) - 1;

			dptr += 1;

			tmp = (that.abits_ptr_[sptr] >> (remain-tail))&mask;
			যদি ((abits_ptr_[dptr] & mask) != tmp) {
			      diff_flag = সঁচা;
			      abits_ptr_[dptr] = (abits_ptr_[dptr] & ~mask) | tmp;
			}
			tmp = (that.bbits_ptr_[sptr] >> (remain-tail))&mask;
			যদি ((bbits_ptr_[dptr] & mask) != tmp) {
			      diff_flag = সঁচা;
			      bbits_ptr_[dptr] = (bbits_ptr_[dptr] & ~mask) | tmp;
			}
		  }
	    }
      }

      ঘূৰাই diff_flag;
}

/*
 * Add that vector to this vector. Do it in the Verilog way, which
 * means if we detect any X or Z bits, change the entire results to
 * all X.
 *
 * Assume both vectors are the same size.
 */
শূন্য vvp_vector4_t::add(ধ্রুৱ vvp_vector4_t&that)
{
      প্রতিজ্ঞা(size_ == that.size_);

      যদি (size_ < BITS_PER_WORD) {
	    unsigned long mask = ~(-1UL << size_);
	    যদি ((bbits_val_|that.bbits_val_) & mask) {
		  abits_val_ |= mask;
		  bbits_val_ |= mask;
		  ঘূৰাই;
	    }

	    abits_val_ += that.abits_val_;
	    abits_val_ &= mask;
	    ঘূৰাই;
      }

      যদি (size_ == BITS_PER_WORD) {
	    যদি (bbits_val_ | that.bbits_val_) {
		  abits_val_ = WORD_X_ABITS;
		  bbits_val_ = WORD_X_BBITS;
	    } অন্যথা {
		  abits_val_ += that.abits_val_;
	    }
	    ঘূৰাই;
      }

      পূর্ণাঙ্ক cnt = size_ / BITS_PER_WORD;
      unsigned long carry = 0;
      বাবে (পূর্ণাঙ্ক idx = 0 ; idx < cnt ; idx += 1) {
	    যদি (bbits_ptr_[idx] | that.bbits_ptr_[idx])
		  গচ্ছ x_out;

	    abits_ptr_[idx] = add_with_carry(abits_ptr_[idx], that.abits_ptr_[idx], carry);
      }

      যদি (unsigned tail = size_ % BITS_PER_WORD) {
	    unsigned long mask = ~( -1UL << tail );
	    যদি ((bbits_ptr_[cnt] | that.bbits_ptr_[cnt])&mask)
		  গচ্ছ x_out;

	    abits_ptr_[cnt] = add_with_carry(abits_ptr_[cnt], that.abits_ptr_[cnt], carry);
	    abits_ptr_[cnt] &= mask;
      }

      ঘূৰাই;

 x_out:
      বাবে (পূর্ণাঙ্ক idx = 0 ; idx < cnt ; idx += 1) {
	    abits_ptr_[idx] = WORD_X_ABITS;
	    bbits_ptr_[idx] = WORD_X_BBITS;
      }
      যদি (unsigned tail = size_%BITS_PER_WORD) {
	    unsigned long mask = ~( -1UL << tail );
	    abits_ptr_[cnt] = WORD_X_ABITS&mask;
	    bbits_ptr_[cnt] = WORD_X_BBITS&mask;
      }
}

শূন্য vvp_vector4_t::sub(ধ্রুৱ vvp_vector4_t&that)
{
      প্রতিজ্ঞা(size_ == that.size_);

      যদি (size_ < BITS_PER_WORD) {
	    unsigned long mask = ~(-1UL << size_);
	    যদি ((bbits_val_|that.bbits_val_) & mask) {
		  abits_val_ |= mask;
		  bbits_val_ |= mask;
		  ঘূৰাই;
	    }

	    abits_val_ -= that.abits_val_;
	    abits_val_ &= mask;
	    ঘূৰাই;
      }

      যদি (size_ == BITS_PER_WORD) {
	    যদি (bbits_val_ | that.bbits_val_) {
		  abits_val_ = WORD_X_ABITS;
		  bbits_val_ = WORD_X_BBITS;
	    } অন্যথা {
		  abits_val_ -= that.abits_val_;
	    }
	    ঘূৰাই;
      }

      পূর্ণাঙ্ক cnt = size_ / BITS_PER_WORD;
      unsigned long carry = 1;
      বাবে (পূর্ণাঙ্ক idx = 0 ; idx < cnt ; idx += 1) {
	    যদি (bbits_ptr_[idx] | that.bbits_ptr_[idx])
		  গচ্ছ x_out;

	    abits_ptr_[idx] = add_with_carry(abits_ptr_[idx], ~that.abits_ptr_[idx], carry);
      }

      যদি (unsigned tail = size_ % BITS_PER_WORD) {
	    unsigned long mask = ~( -1UL << tail );
	    যদি ((bbits_ptr_[cnt] | that.bbits_ptr_[cnt])&mask)
		  গচ্ছ x_out;

	    abits_ptr_[cnt] = add_with_carry(abits_ptr_[cnt], ~that.abits_ptr_[cnt], carry);
	    abits_ptr_[cnt] &= mask;
      }

      ঘূৰাই;

 x_out:
      বাবে (পূর্ণাঙ্ক idx = 0 ; idx < cnt ; idx += 1) {
	    abits_ptr_[idx] = WORD_X_ABITS;
	    bbits_ptr_[idx] = WORD_X_BBITS;
      }
      যদি (unsigned tail = size_%BITS_PER_WORD) {
	    unsigned long mask = ~( -1UL << tail );
	    abits_ptr_[cnt] = WORD_X_ABITS&mask;
	    bbits_ptr_[cnt] = WORD_X_BBITS&mask;
      }

}

শূন্য vvp_vector4_t::mov(unsigned dst, unsigned src, unsigned cnt)
{
      প্রতিজ্ঞা(dst+cnt <= size_);
      প্রতিজ্ঞা(src+cnt <= size_);

      যদি (size_ <= BITS_PER_WORD) {
	    unsigned long vmask = (1UL << cnt) - 1;
	    unsigned long tmp;

	    tmp = (abits_val_ >> src) & vmask;
	    abits_val_ &= ~ (vmask << dst);
	    abits_val_ |= tmp << dst;

	    tmp = (bbits_val_ >> src) & vmask;
	    bbits_val_ &= ~ (vmask << dst);
	    bbits_val_ |= tmp << dst;

      } অন্যথা {
	    unsigned sptr = src / BITS_PER_WORD;
	    unsigned dptr = dst / BITS_PER_WORD;
	    unsigned soff = src % BITS_PER_WORD;
	    unsigned doff = dst % BITS_PER_WORD;

	    যেতিয়ালৈ (cnt > 0) {
		  unsigned trans = cnt;
		  যদি ((soff+trans) > BITS_PER_WORD)
			trans = BITS_PER_WORD - soff;

		  যদি ((doff+trans) > BITS_PER_WORD)
			trans = BITS_PER_WORD - doff;

		  যদি (trans == BITS_PER_WORD) {
			  // Special case: the transfer count is
			  // exactly an entire word. For this to be
			  // true, it must also be true that the
			  // pointers are aligned. The work is easy,
			abits_ptr_[dptr] = abits_ptr_[sptr];
			bbits_ptr_[dptr] = bbits_ptr_[sptr];
			dptr += 1;
			sptr += 1;
			cnt -= BITS_PER_WORD;
			অনুৱর্ত;
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
		  যদি (soff >= BITS_PER_WORD) {
			soff = 0;
			sptr += 1;
		  }
		  doff += trans;
		  যদি (doff >= BITS_PER_WORD) {
			doff = 0;
			dptr += 1;
		  }
	    }
      }
}

শূন্য vvp_vector4_t::mul(ধ্রুৱ vvp_vector4_t&that)
{
      প্রতিজ্ঞা(size_ == that.size_);

      যদি (size_ < BITS_PER_WORD) {
	    unsigned long mask = ~(-1UL << size_);
	    যদি ((bbits_val_|that.bbits_val_) & mask) {
		  abits_val_ |= mask;
		  bbits_val_ |= mask;
		  ঘূৰাই;
	    }

	    abits_val_ *= that.abits_val_;
	    abits_val_ &= mask;
	    ঘূৰাই;
      }

      যদি (size_ == BITS_PER_WORD) {
	    যদি (bbits_val_ || that.bbits_val_) {
		  abits_val_ = WORD_X_ABITS;
		  bbits_val_ = WORD_X_BBITS;
	    } অন্যথা {
		  abits_val_ *= that.abits_val_;
	    }
	    ঘূৰাই;
      }

      ধ্রুৱ পূর্ণাঙ্ক cnt = (size_+BITS_PER_WORD-1) / BITS_PER_WORD;

      unsigned long mask;
      যদি (unsigned tail = size_%BITS_PER_WORD) {
	    mask = ~( -1UL << tail );
      } অন্যথা {
	    mask = ~0UL;
      }

	// Check for any XZ values ahead of time in a first pass. If
	// we find any, then force the entire result to be X and be
	// done.
      বাবে (পূর্ণাঙ্ক idx = 0 ; idx < cnt ; idx += 1) {
	    unsigned long lval = bbits_ptr_[idx];
	    unsigned long rval = that.bbits_ptr_[idx];
	    যদি (idx == (cnt-1)) {
		  lval &= mask;
		  rval &= mask;
	    }
	    যদি (lval || rval) {
		  বাবে (পূর্ণাঙ্ক xdx = 0 ; xdx < cnt-1 ; xdx += 1) {
			abits_ptr_[xdx] = WORD_X_ABITS;
			bbits_ptr_[xdx] = WORD_X_BBITS;
		  }
		  abits_ptr_[cnt-1] = WORD_X_ABITS & mask;
		  bbits_ptr_[cnt-1] = WORD_X_BBITS & mask;
		  ঘূৰাই;
	    }
      }

	// Calculate the result into a res array. We need to keep is
	// separate from the "this" array because we are making
	// multiple passes.
      unsigned long*res = নূতন unsigned long[cnt];
      বাবে (পূর্ণাঙ্ক idx = 0 ; idx < cnt ; idx += 1)
	    res[idx] = 0;

      বাবে (পূর্ণাঙ্ক mul_a = 0 ; mul_a < cnt ; mul_a += 1) {
	    unsigned long lval = abits_ptr_[mul_a];
	    যদি (mul_a == (cnt-1))
		  lval &= mask;

	    বাবে (পূর্ণাঙ্ক mul_b = 0 ; mul_b < (cnt-mul_a) ; mul_b += 1) {
		  unsigned long rval = that.abits_ptr_[mul_b];
		  যদি (mul_b == (cnt-1))
			rval &= mask;

		  unsigned long sum;
		  unsigned long tmp = multiply_with_carry(lval, rval, sum);
		  পূর্ণাঙ্ক base = mul_a + mul_b;
		  unsigned long carry = 0;
		  res[base] = add_with_carry(res[base], tmp, carry);
		  বাবে (পূর্ণাঙ্ক add_idx = base+1 ; add_idx < cnt ; add_idx += 1) {
			res[add_idx] = add_with_carry(res[add_idx], sum, carry);
			sum = 0;
		  }
	    }
      }

	// Replace the "this" value with the calculated result. We
	// know a-priori that the bbits are zero and unchanged.
      res[cnt-1] &= mask;
      বাবে (পূর্ণাঙ্ক idx = 0 ; idx < cnt ; idx += 1)
	    abits_ptr_[idx] = res[idx];

      delete[]res;
      ঘূৰাই;


}

তার্কিক vvp_vector4_t::eeq(ধ্রুৱ vvp_vector4_t&that) ধ্রুৱ
{
      যদি (size_ != that.size_)
	    ঘূৰাই মিছা;

      যদি (size_ < BITS_PER_WORD) {
	    unsigned long mask = (1UL << size_) - 1;
	    ঘূৰাই (abits_val_&mask) == (that.abits_val_&mask)
		  && (bbits_val_&mask) == (that.bbits_val_&mask);
      }

      যদি (size_ == BITS_PER_WORD) {
	    ঘূৰাই (abits_val_ == that.abits_val_)
		  && (bbits_val_ == that.bbits_val_);
      }

      unsigned শব্দগুলি = size_ / BITS_PER_WORD;
      বাবে (unsigned idx = 0 ;  idx < শব্দগুলি ;  idx += 1) {
	    যদি (abits_ptr_[idx] != that.abits_ptr_[idx])
		  ঘূৰাই মিছা;
	    যদি (bbits_ptr_[idx] != that.bbits_ptr_[idx])
		  ঘূৰাই মিছা;
      }

      unsigned long mask = size_%BITS_PER_WORD;
      যদি (mask > 0) {
	    mask = (1UL << mask) - 1;
	    ঘূৰাই (abits_ptr_[শব্দগুলি]&mask) == (that.abits_ptr_[শব্দগুলি]&mask)
		  && (bbits_ptr_[শব্দগুলি]&mask) == (that.bbits_ptr_[শব্দগুলি]&mask);
      }

      ঘূৰাই সঁচা;
}

তার্কিক vvp_vector4_t::eq_xz(ধ্রুৱ vvp_vector4_t&that) ধ্রুৱ
{
      যদি (size_ != that.size_)
	    ঘূৰাই মিছা;

      যদি (size_ < BITS_PER_WORD) {
	    unsigned long mask = (1UL << size_) - 1;
	    ঘূৰাই ((abits_val_|bbits_val_)&mask) == ((that.abits_val_|that.bbits_val_)&mask)
		  && (bbits_val_&mask) == (that.bbits_val_&mask);
      }

      যদি (size_ == BITS_PER_WORD) {
	    ঘূৰাই ((abits_val_|bbits_val_) == (that.abits_val_|that.bbits_val_))
		  && (bbits_val_ == that.bbits_val_);
      }

      unsigned শব্দগুলি = size_ / BITS_PER_WORD;
      বাবে (unsigned idx = 0 ;  idx < শব্দগুলি ;  idx += 1) {
	    যদি ((abits_ptr_[idx]|bbits_ptr_[idx]) != (that.abits_ptr_[idx]|that.bbits_ptr_[idx]))
		  ঘূৰাই মিছা;
	    যদি (bbits_ptr_[idx] != that.bbits_ptr_[idx])
		  ঘূৰাই মিছা;
      }

      unsigned long mask = size_%BITS_PER_WORD;
      যদি (mask > 0) {
	    mask = (1UL << mask) - 1;
	    ঘূৰাই ((abits_ptr_[শব্দগুলি]|bbits_ptr_[শব্দগুলি])&mask) == ((that.abits_ptr_[শব্দগুলি]|that.bbits_ptr_[শব্দগুলি])&mask)
		  && (bbits_ptr_[শব্দগুলি]&mask) == (that.bbits_ptr_[শব্দগুলি]&mask);
      }

      ঘূৰাই সঁচা;
}

তার্কিক vvp_vector4_t::has_xz() ধ্রুৱ
{
      যদি (size_ < BITS_PER_WORD) {
	    unsigned long mask = -1UL >> (BITS_PER_WORD - size_);
	    ঘূৰাই bbits_val_&mask;
      }

      যদি (size_ == BITS_PER_WORD) {
	    ঘূৰাই bbits_val_;
      }

      unsigned শব্দগুলি = size_ / BITS_PER_WORD;
      বাবে (unsigned idx = 0 ; idx < শব্দগুলি ; idx += 1) {
	    যদি (bbits_ptr_[idx])
		  ঘূৰাই সঁচা;
      }

      unsigned long mask = size_%BITS_PER_WORD;
      যদি (mask > 0) {
	    mask = -1UL >> (BITS_PER_WORD - mask);
	    ঘূৰাই bbits_ptr_[শব্দগুলি]&mask;
      }

      ঘূৰাই মিছা;
}

শূন্য vvp_vector4_t::change_z2x()
{
	// This method relies on the fact that both BIT4_X and BIT4_Z
	// have the bbit set in the vector4 encoding, and also that
	// the BIT4_X has abit set in the vector4 encoding. By simply
	// or-ing the bbit into the abit, BIT4_X and BIT4_Z both
	// become BIT4_X.

      যদি (size_ <= BITS_PER_WORD) {
	    abits_val_ |= bbits_val_;
      } অন্যথা {
	    unsigned শব্দগুলি = (size_+BITS_PER_WORD-1) / BITS_PER_WORD;
	    বাবে (unsigned idx = 0 ;  idx < শব্দগুলি ;  idx += 1)
		  abits_ptr_[idx] |= bbits_ptr_[idx];
      }
}

শূন্য vvp_vector4_t::fill_bits(vvp_bit4_t bit)
{
	/* note: this relies on the bit encoding for the vvp_bit4_t. */
      স্থির ধ্রুৱ unsigned long init_atable[4] = {
	    WORD_0_ABITS,
	    WORD_1_ABITS,
	    WORD_Z_ABITS,
	    WORD_X_ABITS };
      স্থির ধ্রুৱ unsigned long init_btable[4] = {
	    WORD_0_BBITS,
	    WORD_1_BBITS,
	    WORD_Z_BBITS,
	    WORD_X_BBITS };

      যদি (size_ <= BITS_PER_WORD) {
	    abits_val_ = init_atable[bit];
	    bbits_val_ = init_btable[bit];
      } অন্যথা {
	    unsigned শব্দগুলি = (size_ + BITS_PER_WORD - 1) / BITS_PER_WORD;
	    বাবে (unsigned idx = 0; idx < শব্দগুলি; idx += 1) {
		  abits_ptr_[idx] = init_atable[bit];
		  bbits_ptr_[idx] = init_btable[bit];
	    }
      }
}

ৱর্ণ* vvp_vector4_t::as_string(ৱর্ণ*buf, size_t buf_len) ধ্রুৱ
{
      ৱর্ণ*res = buf;
      *buf++ = 'C';
      *buf++ = '4';
      *buf++ = '<';
      buf_len -= 3;

      বাবে (unsigned idx = 0 ;  idx < size() && buf_len >= 2 ;  idx += 1) {
	    ৱিকল্প (value(size()-idx-1)) {
		পক্ষ BIT4_0:
		  *buf++ = '0';
		  ভঙ্গ;
		পক্ষ BIT4_1:
		  *buf++ = '1';
		  ভঙ্গ;
		পক্ষ BIT4_X:
		  *buf++ = 'x';
		  ভঙ্গ;
		পক্ষ BIT4_Z:
		  *buf++ = 'z';
	    }
	    buf_len -= 1;
      }

      *buf++ = '>';
      *buf++ = 0;
      ঘূৰাই res;
}

শূন্য vvp_vector4_t::invert()
{
      যদি (size_ <= BITS_PER_WORD) {
	    unsigned long mask = (size_<BITS_PER_WORD)? (1UL<<size_)-1UL : -1UL;
	    abits_val_ = mask & ~abits_val_;
	    abits_val_ |= bbits_val_;
      } অন্যথা {
	    unsigned remaining = size_;
	    unsigned idx = 0;
	    যেতিয়ালৈ (remaining >= BITS_PER_WORD) {
		  abits_ptr_[idx] = ~abits_ptr_[idx];
		  abits_ptr_[idx] |= bbits_ptr_[idx];
		  idx += 1;
		  remaining -= BITS_PER_WORD;
	    }
	    যদি (remaining > 0) {
		  unsigned long mask = (1UL<<remaining) - 1UL;
		  abits_ptr_[idx] = mask & ~abits_ptr_[idx];
		  abits_ptr_[idx] |= bbits_ptr_[idx];
	    }
      }
}

#define BIT_MASK(n) ((n) ? ((~0UL) >> (BITS_PER_WORD - (n))) : ~0UL)

vvp_bit4_t vvp_vector4_t::reduce_or() ধ্রুৱ
{
      unsigned long mask = BIT_MASK(size_ % BITS_PER_WORD);
      vvp_bit4_t res = BIT4_0;

      যদি (size_ <= BITS_PER_WORD) {
	    যদি ((abits_val_ & ~bbits_val_ & mask) != 0UL)
		  ঘূৰাই BIT4_1;
	    যদি ((bbits_val_ & mask) != 0UL)
		  ঘূৰাই BIT4_X;
      } অন্যথা {
	    unsigned শব্দগুলি = (size_ + BITS_PER_WORD - 1) / BITS_PER_WORD;
	    unsigned idx;
	    বাবে (idx = 0; idx < শব্দগুলি - 1; idx += 1) {
		  যদি ((abits_ptr_[idx] & ~bbits_ptr_[idx]) != 0UL)
			ঘূৰাই BIT4_1;
		  যদি (bbits_ptr_[idx] != 0UL)
			res = BIT4_X;
	    }
	    যদি ((abits_ptr_[idx] & ~bbits_ptr_[idx] & mask) != 0UL)
		  ঘূৰাই BIT4_1;
	    যদি ((bbits_ptr_[idx] & mask) != 0UL)
		  res = BIT4_X;
      }

      ঘূৰাই res;
}

vvp_bit4_t vvp_vector4_t::reduce_and() ধ্রুৱ
{
      unsigned long mask = BIT_MASK(size_ % BITS_PER_WORD);
      vvp_bit4_t res = BIT4_1;

      যদি (size_ <= BITS_PER_WORD) {
	    যদি ((abits_val_ | bbits_val_ | ~mask) != ~0UL)
		  ঘূৰাই BIT4_0;
	    যদি ((bbits_val_ & mask) != 0UL)
		  ঘূৰাই BIT4_X;
	} অন্যথা {
	    unsigned শব্দগুলি = (size_ + BITS_PER_WORD - 1) / BITS_PER_WORD;
	    unsigned idx;
	    বাবে (idx = 0; idx < শব্দগুলি - 1; idx += 1) {
		  যদি ((abits_ptr_[idx] | bbits_ptr_[idx]) != ~0UL)
			ঘূৰাই BIT4_0;
		  যদি (bbits_ptr_[idx] != 0UL)
			res = BIT4_X;
	    }
	    যদি ((abits_ptr_[idx] | bbits_ptr_[idx] | ~mask) != ~0UL)
		  ঘূৰাই BIT4_0;
	    যদি ((bbits_ptr_[idx] & mask) != 0UL)
		  res = BIT4_X;
      }

      ঘূৰাই res;
}

স্থির unsigned long parity(unsigned long val)
{
#যদি defined(__GNUC__)
      // The compiler builtin can use target-specific CPU instructions.
      ঘূৰাই __builtin_parityl(val);
#অন্যথা
#যদি ULONG_MAX > 0xffffffffUL
	val ^= val >> 32;
#endif
	val ^= val >> 16;
	val ^= val >> 8;
	val ^= val >> 4;

      ঘূৰাই (0x6996 >> (val & 0xf)) & 1;
#endif
}

vvp_bit4_t vvp_vector4_t::reduce_xor() ধ্রুৱ
{
      unsigned long mask = BIT_MASK(size_ % BITS_PER_WORD);

      যদি (size_ <= BITS_PER_WORD) {
	    যদি ((bbits_val_ & mask) != 0UL)
		  ঘূৰাই BIT4_X;
	    ঘূৰাই parity(abits_val_ & mask) ? BIT4_1 : BIT4_0;
      } অন্যথা {
	    unsigned শব্দগুলি = (size_ + BITS_PER_WORD - 1) / BITS_PER_WORD;
	    unsigned long val_a = 0UL;
	    unsigned idx;
	    বাবে (idx = 0; idx < শব্দগুলি - 1; idx += 1) {
		  যদি (bbits_ptr_[idx] != 0UL)
			ঘূৰাই BIT4_X;
		  val_a ^= abits_ptr_[idx];
	    }
	    যদি ((bbits_ptr_[idx] & mask) != 0UL)
		  ঘূৰাই BIT4_X;
	    val_a ^= abits_ptr_[idx] & mask;
	    ঘূৰাই parity(val_a) ? BIT4_1 : BIT4_0;
      }
}

vvp_vector4_t& vvp_vector4_t::operator &= (ধ্রুৱ vvp_vector4_t&that)
{
	// The truth table is:
	//     00 01 11 10
	//  00 00 00 00 00
	//  01 00 01 11 11
	//  11 00 11 11 11
	//  10 00 11 11 11
      যদি (size_ <= BITS_PER_WORD) {
	    unsigned long tmp1 = abits_val_ | bbits_val_;
	    unsigned long tmp2 = that.abits_val_ | that.bbits_val_;
	    abits_val_ = tmp1 & tmp2;
	    bbits_val_ = (tmp1 & that.bbits_val_) | (tmp2 & bbits_val_);
      } অন্যথা {
	    unsigned শব্দগুলি = (size_ + BITS_PER_WORD - 1) / BITS_PER_WORD;
	    বাবে (unsigned idx = 0; idx < শব্দগুলি ; idx += 1) {
		  unsigned long tmp1 = abits_ptr_[idx] | bbits_ptr_[idx];
		  unsigned long tmp2 = that.abits_ptr_[idx] |
		                       that.bbits_ptr_[idx];
		  abits_ptr_[idx] = tmp1 & tmp2;
		  bbits_ptr_[idx] = (tmp1 & that.bbits_ptr_[idx]) |
		                    (tmp2 & bbits_ptr_[idx]);
	    }
      }

      ঘূৰাই *এতত্;
}

vvp_vector4_t& vvp_vector4_t::operator |= (ধ্রুৱ vvp_vector4_t&that)
{
	// The truth table is:
	//     00 01 11 10
	//  00 00 01 11 11
	//  01 01 01 01 01
	//  11 11 01 11 11
	//  10 11 01 11 11
      যদি (size_ <= BITS_PER_WORD) {
	    unsigned long tmp = abits_val_ | bbits_val_ |
	                        that.abits_val_ | that.bbits_val_;
	    bbits_val_ = ((~abits_val_ | bbits_val_) & that.bbits_val_) |
	                 ((~that.abits_val_ | that.bbits_val_) & bbits_val_);
	    abits_val_ = tmp;

      } অন্যথা {
	    unsigned শব্দগুলি = (size_ + BITS_PER_WORD - 1) / BITS_PER_WORD;
	    বাবে (unsigned idx = 0; idx < শব্দগুলি ; idx += 1) {
		  unsigned long tmp = abits_ptr_[idx] | bbits_ptr_[idx] |
	                        that.abits_ptr_[idx] | that.bbits_ptr_[idx];
		  bbits_ptr_[idx] = ((~abits_ptr_[idx] | bbits_ptr_[idx]) &
		                     that.bbits_ptr_[idx]) |
		                    ((~that.abits_ptr_[idx] |
		                      that.bbits_ptr_[idx]) & bbits_ptr_[idx]);
		  abits_ptr_[idx] = tmp;
	    }
      }

      ঘূৰাই *এতত্;
}

vvp_vector4_t& vvp_vector4_t::operator ^= (ধ্রুৱ vvp_vector4_t&that)
{
	// The truth table is:
	//     00 01 11 10
	//  00 00 01 11 11
	//  01 01 00 11 11
	//  11 11 11 11 11
	//  10 11 11 11 11
      যদি (size_ <= BITS_PER_WORD) {
	    unsigned long bval = bbits_val_ | that.bbits_val_;
	    bbits_val_ = bval;
	    abits_val_ = (abits_val_ ^ that.abits_val_) | bval;
      } অন্যথা {
	    unsigned শব্দগুলি = (size_ + BITS_PER_WORD - 1) / BITS_PER_WORD;
	    বাবে (unsigned idx = 0; idx < শব্দগুলি ; idx += 1) {
		  unsigned long bval = bbits_ptr_[idx] | that.bbits_ptr_[idx];
		  bbits_ptr_[idx] = bval;
		  abits_ptr_[idx] = (abits_ptr_[idx] ^ that.abits_ptr_[idx]) | bval;
	    }
      }

      ঘূৰাই *এতত্;
}

/*
* Add an integer to the vvp_vector4_t in place, bit by bit so that
* there is no size limitations.
*/
vvp_vector4_t& vvp_vector4_t::operator += (int64_t that)
{
      vvp_bit4_t carry = BIT4_0;
      unsigned idx;

      যদি (has_xz()) {
	    vvp_vector4_t xxx (size(), BIT4_X);
	    *এতত্ = xxx;
	    ঘূৰাই *এতত্;
      }

      বাবে (idx = 0 ; idx < size() ; idx += 1) {
	    যদি (that == 0 && carry==BIT4_0)
		  ভঙ্গ;

	    vvp_bit4_t that_bit = (that&1)? BIT4_1 : BIT4_0;
	    that >>= 1;

	    যদি (that_bit==BIT4_0 && carry==BIT4_0)
		  অনুৱর্ত;

	    vvp_bit4_t bit = value(idx);
	    bit = add_with_carry(bit, that_bit, carry);

	    set_bit(idx, bit);
      }

      ঘূৰাই *এতত্;
}

ostream& operator<< (ostream&out, ধ্রুৱ vvp_vector4_t&that)
{
      out << that.size() << "'b";
      বাবে (unsigned idx = 0 ;  idx < that.size() ;  idx += 1)
	    out << that.value(that.size()-idx-1);
      ঘূৰাই out;
}

template <শ্ৰেণী INT>তার্কিক vector4_to_মান(ধ্রুৱ vvp_vector4_t&vec, INT&val,
					  তার্কিক is_signed, তার্কিক is_arithmetic)
{
      INT res = 0;
      INT msk = 1;
      তার্কিক rc_flag = সঁচা;

      unsigned size = vec.size();
      যদি (size > 8*পরিমাণ(val)) size = 8*পরিমাণ(val);
      বাবে (unsigned idx = 0 ;  idx < size ;  idx += 1) {
	    ৱিকল্প (vec.value(idx)) {
		পক্ষ BIT4_0:
		  ভঙ্গ;
		পক্ষ BIT4_1:
		  res |= msk;
		  ভঙ্গ;
		স্ৱভাৱ:
		  যদি (is_arithmetic)
			ঘূৰাই মিছা;
		  অন্যথা
			rc_flag = মিছা;
	    }

	    msk <<= 1;
      }

      যদি (is_signed && vec.value(vec.size()-1) == BIT4_1) {
	    যদি (vec.size() < 8*পরিমাণ(val))
		  res |= static_cast<INT>(-1ULL << vec.size());
      }

      val = res;
      ঘূৰাই rc_flag;
}

template তার্কিক vector4_to_মান(ধ্রুৱ vvp_vector4_t&vec, int8_t&val,
			       তার্কিক is_signed, তার্কিক is_arithmetic);
template তার্কিক vector4_to_মান(ধ্রুৱ vvp_vector4_t&vec, int16_t&val,
			       তার্কিক is_signed, তার্কিক is_arithmetic);
template তার্কিক vector4_to_মান(ধ্রুৱ vvp_vector4_t&vec, int32_t&val,
			       তার্কিক is_signed, তার্কিক is_arithmetic);
template তার্কিক vector4_to_মান(ধ্রুৱ vvp_vector4_t&vec, int64_t&val,
			       তার্কিক is_signed, তার্কিক is_arithmetic);
template তার্কিক vector4_to_মান(ধ্রুৱ vvp_vector4_t&vec, uint8_t&val,
			       তার্কিক is_signed, তার্কিক is_arithmetic);
template তার্কিক vector4_to_মান(ধ্রুৱ vvp_vector4_t&vec, uint16_t&val,
			       তার্কিক is_signed, তার্কিক is_arithmetic);
template তার্কিক vector4_to_মান(ধ্রুৱ vvp_vector4_t&vec, uint32_t&val,
			       তার্কিক is_signed, তার্কিক is_arithmetic);
template তার্কিক vector4_to_মান(ধ্রুৱ vvp_vector4_t&vec, uint64_t&val,
			       তার্কিক is_signed, তার্কিক is_arithmetic);

template <শ্ৰেণী T> তার্কিক vector4_to_মান(ধ্রুৱ vvp_vector4_t&vec,
                                         তার্কিক&overflow_flag, T&val)
{
      T res = 0;
      T msk = 1;

      overflow_flag = মিছা;
      unsigned size = vec.size();
      বাবে (unsigned idx = 0 ;  idx < size ;  idx += 1) {
	    ৱিকল্প (vec.value(idx)) {
		পক্ষ BIT4_0:
		  ভঙ্গ;
		পক্ষ BIT4_1:
		  যদি (msk == 0)
			overflow_flag = সঁচা;
		  অন্যথা
			res |= msk;
		  ভঙ্গ;
		স্ৱভাৱ:
		  ঘূৰাই মিছা;
	    }

	    msk <<= static_cast<T>(1);
      }

      val = res;
      ঘূৰাই সঁচা;
}

template তার্কিক vector4_to_মান(ধ্রুৱ vvp_vector4_t&vec, তার্কিক&overflow_flag,
                               unsigned long&val);
#ifndef UL_AND_TIME64_SAME
template তার্কিক vector4_to_মান(ধ্রুৱ vvp_vector4_t&vec, তার্কিক&overflow_flag,
                               vvp_time64_t&val);
#endif

তার্কিক vector4_to_মান(ধ্রুৱ vvp_vector4_t&vec, দ্ৱিগুণ&val, তার্কিক signed_flag)
{

      যদি (vec.size() == 0) {
	    val = 0.0;
	    ঘূৰাই সঁচা;
      }

      তার্কিক flag = সঁচা;

      যদি (vec.value(vec.size()-1) != BIT4_1) {
	    signed_flag = মিছা;
      }

      দ্ৱিগুণ res = 0.0;
      যদি (signed_flag) {
	    vvp_bit4_t carry = BIT4_1;
	    বাবে (unsigned idx = 0 ;  idx < vec.size() ;  idx += 1) {
		  vvp_bit4_t a = ~vec.value(idx);
		  vvp_bit4_t x = add_with_carry(a, BIT4_0, carry);
		  ৱিকল্প (x) {
		      পক্ষ BIT4_0:
			ভঙ্গ;
		      পক্ষ BIT4_1:
			res += pow(2.0, (পূর্ণাঙ্ক)idx);
			ভঙ্গ;
		      স্ৱভাৱ:
			flag = মিছা;
		  }
	    }
	    res *= -1.0;
      } অন্যথা {
	    বাবে (unsigned idx = 0 ;  idx < vec.size() ;  idx += 1) {
		  ৱিকল্প (vec.value(idx)) {
		      পক্ষ BIT4_0:
			ভঙ্গ;
		      পক্ষ BIT4_1:
			res += pow(2.0, (পূর্ণাঙ্ক)idx);
			ভঙ্গ;
		      স্ৱভাৱ:
			flag = মিছা;
		  }
	    }
      }
      val = res;
      ঘূৰাই flag;
}

তার্কিক vector2_to_মান(ধ্রুৱ vvp_vector2_t&a, int32_t&val, তার্কিক is_signed)
{
      val = 0;
      unsigned idx;
      uint32_t mask;
      বাবে (idx = 0, mask = 1 ; idx < a.size() && idx < 32 ; idx += 1, mask <<= 1) {
	    যদি (a.value(idx)) val |= mask;
      }

      যদি (is_signed && a.size() < 32 && a.value(a.size()-1)) {
	    mask = -1;
	    mask <<= a.size();
	    val |= mask;
      }

      ঘূৰাই a.size() <= 32;
}

vvp_vector4array_t::vvp_vector4array_t(unsigned width__, unsigned শব্দগুলি_2)
: width_(width__), শব্দগুলি_3(শব্দগুলি_2)
{
}

vvp_vector4array_t::~vvp_vector4array_t()
{
}

শূন্য vvp_vector4array_t::set_word_(v4cell*cell, ধ্রুৱ vvp_vector4_t&that)
{
      প্রতিজ্ঞা(that.size_ == width_);

      যদি (width_ <= vvp_vector4_t::BITS_PER_WORD) {
	    cell->abits_val_ = that.abits_val_;
	    cell->bbits_val_ = that.bbits_val_;
	    ঘূৰাই;
      }

      unsigned cnt = (width_ + vvp_vector4_t::BITS_PER_WORD-1)/vvp_vector4_t::BITS_PER_WORD;

      যদি (cell->abits_ptr_ == 0) {
	    cell->abits_ptr_ = নূতন unsigned long[2*cnt];
	    cell->bbits_ptr_ = cell->abits_ptr_ + cnt;
      }

      বাবে (unsigned idx = 0 ; idx < cnt ; idx += 1)
	    cell->abits_ptr_[idx] = that.abits_ptr_[idx];
      বাবে (unsigned idx = 0 ; idx < cnt ; idx += 1)
	    cell->bbits_ptr_[idx] = that.bbits_ptr_[idx];
}

vvp_vector4_t vvp_vector4array_t::get_word_(v4cell*cell) ধ্রুৱ
{
      যদি (width_ <= vvp_vector4_t::BITS_PER_WORD) {
	    vvp_vector4_t res;
	    res.size_ = width_;
	    res.abits_val_ = cell->abits_val_;
	    res.bbits_val_ = cell->bbits_val_;
	    ঘূৰাই res;
      }

      vvp_vector4_t res (width_, BIT4_X);
      যদি (cell->abits_ptr_ == 0)
	    ঘূৰাই res;

      unsigned cnt = (width_ + vvp_vector4_t::BITS_PER_WORD-1)/vvp_vector4_t::BITS_PER_WORD;

      বাবে (unsigned idx = 0 ; idx < cnt ; idx += 1)
	    res.abits_ptr_[idx] = cell->abits_ptr_[idx];
      বাবে (unsigned idx = 0 ; idx < cnt ; idx += 1)
	    res.bbits_ptr_[idx] = cell->bbits_ptr_[idx];

      ঘূৰাই res;
}

vvp_vector4array_sa::vvp_vector4array_sa(unsigned width__, unsigned শব্দগুলি_2)
: vvp_vector4array_t(width__, শব্দগুলি_2)
{
      array_ = নূতন v4cell[শব্দগুলি_3];

      যদি (width_ <= vvp_vector4_t::BITS_PER_WORD) {
	    বাবে (unsigned idx = 0 ; idx < শব্দগুলি_3 ; idx += 1) {
		  array_[idx].abits_val_ = vvp_vector4_t::WORD_X_ABITS;
		  array_[idx].bbits_val_ = vvp_vector4_t::WORD_X_BBITS;
	    }
      } অন্যথা {
	    বাবে (unsigned idx = 0 ; idx < শব্দগুলি_3 ; idx += 1) {
		  array_[idx].abits_ptr_ = 0;
		  array_[idx].bbits_ptr_ = 0;
	    }
      }
}

vvp_vector4array_sa::~vvp_vector4array_sa()
{
      যদি (array_) {
	    যদি (width_ > vvp_vector4_t::BITS_PER_WORD) {
		  বাবে (unsigned idx = 0 ; idx < শব্দগুলি_3 ; idx += 1)
			যদি (array_[idx].abits_ptr_)
			      delete[]array_[idx].abits_ptr_;
	    }
	    delete[]array_;
      }
}

শূন্য vvp_vector4array_sa::set_word(unsigned index, ধ্রুৱ vvp_vector4_t&that)
{
      প্রতিজ্ঞা(index < শব্দগুলি_3);

      v4cell*cell = &array_[index];

      set_word_(cell, that);
}

vvp_vector4_t vvp_vector4array_sa::get_word(unsigned index) ধ্রুৱ
{
      যদি (index >= শব্দগুলি_3)
	    ঘূৰাই vvp_vector4_t(width_, BIT4_X);

      প্রতিজ্ঞা(index < শব্দগুলি_3);

      v4cell*cell = &array_[index];

      ঘূৰাই get_word_(cell);
}

vvp_vector4array_aa::vvp_vector4array_aa(unsigned width__, unsigned শব্দগুলি_2)
: vvp_vector4array_t(width__, শব্দগুলি_2)
{
      context_idx_ = vpip_add_item_to_context(এতত্, vpip_peek_context_scope());
}

vvp_vector4array_aa::~vvp_vector4array_aa()
{
}

শূন্য vvp_vector4array_aa::alloc_instance(vvp_context_t context)
{
      v4cell*array = নূতন v4cell[শব্দগুলি_3];

      যদি (width_ <= vvp_vector4_t::BITS_PER_WORD) {
	    বাবে (unsigned idx = 0 ; idx < শব্দগুলি_3 ; idx += 1) {
		  array[idx].abits_val_ = vvp_vector4_t::WORD_X_ABITS;
		  array[idx].bbits_val_ = vvp_vector4_t::WORD_X_BBITS;
	    }
      } অন্যথা {
	    বাবে (unsigned idx = 0 ; idx < শব্দগুলি_3 ; idx += 1) {
		  array[idx].abits_ptr_ = 0;
		  array[idx].bbits_ptr_ = 0;
	    }
      }

      vvp_set_context_item(context, context_idx_, array);
}

শূন্য vvp_vector4array_aa::reset_instance(vvp_context_t context)
{
      v4cell*cell = static_cast<v4cell*>
            (vvp_get_context_item(context, context_idx_));

      যদি (width_ <= vvp_vector4_t::BITS_PER_WORD) {
	    বাবে (unsigned idx = 0 ; idx < শব্দগুলি_3 ; idx += 1) {
		  cell->abits_val_ = vvp_vector4_t::WORD_X_ABITS;
		  cell->bbits_val_ = vvp_vector4_t::WORD_X_BBITS;
                  cell++;
	    }
      } অন্যথা {
            unsigned cnt = (width_ + vvp_vector4_t::BITS_PER_WORD-1)/vvp_vector4_t::BITS_PER_WORD;
	    বাবে (unsigned idx = 0 ; idx < শব্দগুলি_3 ; idx += 1) {
		  যদি (cell->abits_ptr_) {
                        বাবে (unsigned n = 0 ; n < cnt ; n += 1) {
			      cell->abits_ptr_[n] = vvp_vector4_t::WORD_X_ABITS;
			      cell->bbits_ptr_[n] = vvp_vector4_t::WORD_X_BBITS;
                        }
                  }
                  cell++;
	    }
      }
}

#ifdef CHECK_WITH_VALGRIND
শূন্য vvp_vector4array_aa::free_instance(vvp_context_t context)
{
      v4cell*cell = static_cast<v4cell*>
            (vvp_get_context_item(context, context_idx_));
      delete [] cell;
}
#endif

শূন্য vvp_vector4array_aa::set_word(unsigned index, ধ্রুৱ vvp_vector4_t&that)
{
      প্রতিজ্ঞা(index < শব্দগুলি_3);

      v4cell*cell = static_cast<v4cell*>
            (vthread_get_wt_context_item(context_idx_)) + index;

      set_word_(cell, that);
}

vvp_vector4_t vvp_vector4array_aa::get_word(unsigned index) ধ্রুৱ
{
      যদি (index >= শব্দগুলি_3)
	    ঘূৰাই vvp_vector4_t(width_, BIT4_X);

      প্রতিজ্ঞা(index < শব্দগুলি_3);

      v4cell*cell = static_cast<v4cell*>
            (vthread_get_rd_context_item(context_idx_)) + index;

      ঘূৰাই get_word_(cell);
}

vvp_vector2_t::vvp_vector2_t()
{
      vec_ = 0;
      wid_ = 0;
}

vvp_vector2_t::vvp_vector2_t(uint64_t v, unsigned wid)
{
      wid_ = wid;
      ধ্রুৱ unsigned bits_per_word = 8 * পরিমাণ(vec_[0]);
      ধ্রুৱ unsigned শব্দগুলি = (wid_ + bits_per_word-1) / bits_per_word;

      vec_ = নূতন unsigned long[শব্দগুলি];
      বাবে (unsigned idx = 0 ; idx < শব্দগুলি ; idx += 1) {
	    ধ্রুৱ unsigned shift = idx * bits_per_word;
	    vec_[idx] = shift < 64 ? v >> shift : 0;
      }
}

vvp_vector2_t::vvp_vector2_t(vvp_vector2_t::fill_t fill, unsigned wid)
{
      wid_ = wid;
      ধ্রুৱ unsigned bits_per_word = 8 * পরিমাণ(vec_[0]);
      ধ্রুৱ unsigned শব্দগুলি = (wid_ + bits_per_word-1) / bits_per_word;

      vec_ = নূতন unsigned long[শব্দগুলি];
      বাবে (unsigned idx = 0 ;  idx < শব্দগুলি ;  idx += 1)
	    vec_[idx] = fill? -1 : 0;
}

vvp_vector2_t::vvp_vector2_t(ধ্রুৱ vvp_vector2_t&that, unsigned base, unsigned wid)
{
      wid_ = wid;
      ধ্রুৱ unsigned শব্দগুলি = (wid_ + BITS_PER_WORD-1) / BITS_PER_WORD;

      vec_ = নূতন unsigned long[শব্দগুলি];

      বাবে (unsigned idx = 0 ; idx < wid ; idx += 1) {
	    পূর্ণাঙ্ক bit = that.value(base+idx);
	    যদি (bit == 0)
		  অনুৱর্ত;
	    unsigned word = idx / BITS_PER_WORD;
	    unsigned long mask = 1UL << (idx % BITS_PER_WORD);
	    vec_[word] |= mask;
      }
}

শূন্য vvp_vector2_t::copy_from_that_(ধ্রুৱ vvp_vector4_t&that)
{
      wid_ = that.size();
      ধ্রুৱ unsigned শব্দগুলি = (that.size() + BITS_PER_WORD-1) / BITS_PER_WORD;

      যদি (শব্দগুলি == 0) {
	    vec_ = 0;
	    wid_ = 0;
	    ঘূৰাই;
      }

	// Use the subarray method with the xz_to_0 flag set so that
	// we get values even when there are xz bits.
      vec_ = that.subarray(0, wid_, সঁচা);
}

শূন্য vvp_vector2_t::copy_from_that_(ধ্রুৱ vvp_vector2_t&that)
{
      wid_ = that.wid_;
      ধ্রুৱ unsigned শব্দগুলি = (wid_ + BITS_PER_WORD-1) / BITS_PER_WORD;

      যদি (শব্দগুলি == 0) {
	    vec_ = 0;
	    wid_ = 0;
	    ঘূৰাই;
      }

      vec_ = নূতন unsigned long[শব্দগুলি];
      বাবে (unsigned idx = 0 ;  idx < শব্দগুলি ;  idx += 1)
	    vec_[idx] = that.vec_[idx];
}

vvp_vector2_t::vvp_vector2_t(ধ্রুৱ vvp_vector2_t&that, unsigned newsize)
{
      wid_ = newsize;
      যদি (newsize == 0) {
	    vec_ = 0;
	    ঘূৰাই;
      }

      ধ্রুৱ unsigned শব্দগুলি = (wid_ + BITS_PER_WORD-1) / BITS_PER_WORD;
      ধ্রুৱ unsigned twords = (that.wid_ + BITS_PER_WORD-1) / BITS_PER_WORD;

      vec_ = নূতন unsigned long[শব্দগুলি];
      বাবে (unsigned idx = 0 ;  idx < শব্দগুলি ;  idx += 1) {
	    যদি (idx < twords)
		  vec_[idx] = that.vec_[idx];
	    অন্যথা
		  vec_[idx] = 0;
      }
}

vvp_vector2_t& vvp_vector2_t::operator= (ধ্রুৱ vvp_vector2_t&that)
{
      যদি (এতত্ == &that)
	    ঘূৰাই *এতত্;

      delete[] vec_;
      vec_ = 0;

      copy_from_that_(that);
      ঘূৰাই *এতত্;
}

vvp_vector2_t& vvp_vector2_t::operator= (ধ্রুৱ vvp_vector4_t&that)
{
      delete[]vec_;
      vec_ = 0;
      copy_from_that_(that);
      ঘূৰাই *এতত্;
}

vvp_vector2_t& vvp_vector2_t::operator <<= (unsigned পূর্ণাঙ্ক shift)
{
      যদি (wid_ == 0)
	    ঘূৰাই *এতত্;

      ধ্রুৱ unsigned শব্দগুলি = (wid_ + BITS_PER_WORD-1) / BITS_PER_WORD;

	// Number of words to shift
      ধ্রুৱ unsigned wshift = shift / BITS_PER_WORD;
	// bits to shift within each word.
      ধ্রুৱ unsigned long oshift = shift % BITS_PER_WORD;

	// If shifting the entire value away, then return zeros.
      যদি (wshift >= শব্দগুলি) {
	    বাবে (unsigned idx = 0 ;  idx < শব্দগুলি ;  idx += 1)
		  vec_[idx] = 0;

	    ঘূৰাই *এতত্;
      }

	// Do the word shift first.
      যদি (wshift > 0) {
	    বাবে (unsigned idx = 0 ;  idx < শব্দগুলি-wshift ;  idx += 1) {
		  unsigned sel = শব্দগুলি - idx - 1;
		  vec_[sel] = vec_[sel-wshift];
	    }

	    বাবে (unsigned idx = 0 ;  idx < wshift ;  idx += 1)
		  vec_[idx] = 0;
      }

	// Do the fine shift.
      যদি (oshift != 0) {
	    unsigned long pad = 0;
	    বাবে (unsigned idx = 0 ;  idx < শব্দগুলি ;  idx += 1) {
		  unsigned long next_pad = vec_[idx] >> (BITS_PER_WORD-oshift);
		  vec_[idx] = (vec_[idx] << oshift) | pad;
		  pad = next_pad;
	    }

	      // Cleanup the tail bits.
	    unsigned long mask = -1UL >> (BITS_PER_WORD - wid_%BITS_PER_WORD);
	    vec_[শব্দগুলি-1] &= mask;
      }

      ঘূৰাই *এতত্;
}

vvp_vector2_t& vvp_vector2_t::operator >>= (unsigned shift)
{
      যদি (wid_ == 0)
	    ঘূৰাই *এতত্;

      ধ্রুৱ unsigned শব্দগুলি = (wid_ + BITS_PER_WORD-1) / BITS_PER_WORD;

	// Number of words to shift
      ধ্রুৱ unsigned wshift = shift / BITS_PER_WORD;
	// bits to shift within each word.
      ধ্রুৱ unsigned long oshift = shift % BITS_PER_WORD;

	// If shifting the entire value away, then return zeros.
      যদি (wshift >= শব্দগুলি) {
	    বাবে (unsigned idx = 0 ;  idx < শব্দগুলি ;  idx += 1)
		  vec_[idx] = 0;

	    ঘূৰাই *এতত্;
      }

      যদি (wshift > 0) {
	    বাবে (unsigned idx = 0 ;  idx < শব্দগুলি-wshift ;  idx += 1)
		  vec_[idx] = vec_[idx+wshift];

	    বাবে (unsigned idx = শব্দগুলি-wshift ;  idx < শব্দগুলি ;  idx += 1)
		  vec_[idx] = 0;
      }

      যদি (oshift > 0) {
	    unsigned long pad = 0;
	    বাবে (unsigned idx = শব্দগুলি ;  idx > 0 ;  idx -= 1) {
		  unsigned long new_pad = vec_[idx-1] <<(BITS_PER_WORD-oshift);
		  vec_[idx-1] = pad | (vec_[idx-1] >> oshift);
		  pad = new_pad;
	    }

	      // Cleanup the tail bits.

	    unsigned use_শব্দগুলি = শব্দগুলি;
	      // Mask_shift is the number of high bits of the top word
	      // that are to be masked off. We start with the number
	      // of bits that are not included even in the original,
	      // then we include the bits of the shift, that are to be
	      // masked to zero.
	    unsigned long mask_shift = BITS_PER_WORD - wid_%BITS_PER_WORD;
	    mask_shift %= BITS_PER_WORD;
	    mask_shift += oshift;
	    যেতিয়ালৈ (mask_shift >= BITS_PER_WORD) {
		  vec_[use_শব্দগুলি-1] = 0;
		  use_শব্দগুলি -= 1;
		  mask_shift -= BITS_PER_WORD;
	    }
	    যদি (mask_shift > 0) {
		  প্রতিজ্ঞা(use_শব্দগুলি > 0);
		  unsigned long mask = -1UL >> mask_shift;
		  vec_[use_শব্দগুলি-1] &= mask;
	    }
      }

      ঘূৰাই *এতত্;
}

স্থির unsigned long add_carry(unsigned long a, unsigned long b,
			       unsigned long&carry)
{
      unsigned long out = carry;
      carry = 0;

      যদি ((ULONG_MAX - out) < a)
	    carry += 1;
      out += a;

      যদি ((ULONG_MAX - out) < b)
	    carry += 1;
      out += b;

      ঘূৰাই out;
}

vvp_vector2_t& vvp_vector2_t::operator += (ধ্রুৱ vvp_vector2_t&that)
{
      প্রতিজ্ঞা(wid_ == that.wid_);
      যদি (wid_ == 0)
	    ঘূৰাই *এতত্;

      ধ্রুৱ unsigned শব্দগুলি = (wid_ + BITS_PER_WORD-1) / BITS_PER_WORD;

      unsigned long carry = 0;
      বাবে (unsigned idx = 0 ;  idx < শব্দগুলি ;  idx += 1) {
	    vec_[idx] = add_carry(vec_[idx], that.vec_[idx], carry);
      }


	// Cleanup the tail bits.
      unsigned long mask = -1UL >> (BITS_PER_WORD - wid_%BITS_PER_WORD);
      vec_[শব্দগুলি-1] &= mask;

      ঘূৰাই *এতত্;
}

vvp_vector2_t& vvp_vector2_t::operator -= (ধ্রুৱ vvp_vector2_t&that)
{
      প্রতিজ্ঞা(wid_ == that.wid_);
      যদি (wid_ == 0)
	    ঘূৰাই *এতত্;

      ধ্রুৱ unsigned শব্দগুলি = (wid_ + BITS_PER_WORD-1) / BITS_PER_WORD;

      unsigned long carry = 1;
      বাবে (unsigned idx = 0 ;  idx < শব্দগুলি ;  idx += 1) {
	    vec_[idx] = add_carry(vec_[idx], ~that.vec_[idx], carry);
      }

      ঘূৰাই *এতত্;
}

শূন্য vvp_vector2_t::trim()
{
      যেতিয়ালৈ (value(wid_-1) == 0 && wid_ > 1) wid_ -= 1;
}

/* This is a special trim that is used on numbers we know represent a
 * negative signed value (they came from a negative real value). */
শূন্য vvp_vector2_t::trim_neg()
{
      যদি (value(wid_-1) == 1 && wid_ > 32) {
	    যেতিয়ালৈ (value(wid_-2) == 1 && wid_ > 32) wid_ -= 1;
      }
}

পূর্ণাঙ্ক vvp_vector2_t::value(unsigned idx) ধ্রুৱ
{
      যদি (idx >= wid_)
	    ঘূৰাই 0;

      ধ্রুৱ unsigned bits_per_word = 8 * পরিমাণ(vec_[0]);
      unsigned addr = idx/bits_per_word;
      unsigned mask = idx%bits_per_word;

      যদি (vec_[addr] & (1UL<<mask))
	    ঘূৰাই 1;
      অন্যথা
	    ঘূৰাই 0;
}

শূন্য vvp_vector2_t::set_bit(unsigned idx, পূর্ণাঙ্ক bit)
{
      প্রতিজ্ঞা(idx < wid_);

      ধ্রুৱ unsigned bits_per_word = 8 * পরিমাণ(vec_[0]);
      unsigned addr = idx/bits_per_word;
      unsigned long mask = idx%bits_per_word;

      যদি (bit)
	    vec_[addr] |= 1UL << mask;
      অন্যথা
	    vec_[addr] &= ~(1UL << mask);
}

শূন্য vvp_vector2_t::set_vec(unsigned adr, ধ্রুৱ vvp_vector2_t&that)
{
      প্রতিজ্ঞা((adr + that.wid_) <= wid_);

      বাবে (unsigned idx = 0 ; idx < that.wid_ ; idx += 1)
	    set_bit(adr+idx, that.value(idx));
}

তার্কিক vvp_vector2_t::is_NaN() ধ্রুৱ
{
      ঘূৰাই wid_ == 0;
}

তার্কিক vvp_vector2_t::is_zero() ধ্রুৱ
{
      ধ্রুৱ unsigned শব্দগুলি = (wid_ + BITS_PER_WORD-1) / BITS_PER_WORD;

      বাবে (unsigned idx = 0; idx < শব্দগুলি; idx += 1) {
	    যদি (vec_[idx] == 0) অনুৱর্ত;
	    ঘূৰাই মিছা;
      }

      ঘূৰাই সঁচা;
}

/*
 * Basic idea from "Introduction to Programming using SML" by
 * Michael R. Hansen and Hans Rischel page 261 and "Seminumerical
 * Algorithms, Third Edition" by Donald E. Knuth section 4.6.3.
 */
vvp_vector2_t pow(ধ্রুৱ vvp_vector2_t&x, vvp_vector2_t&y)
{
        /* If we have a zero exponent just return 1. */
      যদি (y == vvp_vector2_t(0L, 1)) {
	    ঘূৰাই vvp_vector2_t(1L, x.size());
      }

        /* Is the value odd? */
      যদি (y.value(0) == 1) {
	    y.set_bit(0, 0);  // A quick subtract by 1.
	    vvp_vector2_t res = x * pow(x, y);
	    ঘূৰাই res;
      }

      y >>= 1;  // A fast divide by two. We know the LSB is zero.
      vvp_vector2_t z = pow(x, y);
      vvp_vector2_t res = z * z;
      ঘূৰাই res;
}

স্থির শূন্য multiply_long(unsigned long a, unsigned long b,
			  unsigned long&নিচেৰ, unsigned long&উপৰেৰ)
{
      প্রতিজ্ঞা(পরিমাণ(unsigned long) %2 == 0);

      ধ্রুৱ unsigned long word_mask = (1UL << 4UL*পরিমাণ(a)) - 1UL;
      unsigned long tmpa;
      unsigned long tmpb;
      unsigned long res[4];

      tmpa = a & word_mask;
      tmpb = b & word_mask;
      res[0] = tmpa * tmpb;
      res[1] = res[0] >> 4UL*পরিমাণ(unsigned long);
      res[0] &= word_mask;

      tmpa = (a >> 4UL*পরিমাণ(unsigned long)) & word_mask;
      res[1] += tmpa * tmpb;
      res[2] = res[1] >> 4UL*পরিমাণ(unsigned long);
      res[1] &= word_mask;

      tmpa = a & word_mask;
      tmpb = (b >> 4UL*পরিমাণ(unsigned long)) & word_mask;
      res[1] += tmpa * tmpb;
      res[2] += res[1] >> 4UL*পরিমাণ(unsigned long);
      res[3]  = res[2] >> 4UL*পরিমাণ(unsigned long);
      res[1] &= word_mask;
      res[2] &= word_mask;

      tmpa = (a >> 4UL*পরিমাণ(unsigned long)) & word_mask;
      res[2] += tmpa * tmpb;
      res[3] += res[2] >> 4UL*পরিমাণ(unsigned long);
      res[2] &= word_mask;

      উপৰেৰ = (res[3] << 4UL*পরিমাণ(unsigned long)) | res[2];
      নিচেৰ  = (res[1] << 4UL*পরিমাণ(unsigned long)) | res[0];
}

vvp_vector2_t operator * (ধ্রুৱ vvp_vector2_t&a, ধ্রুৱ vvp_vector2_t&b)
{
      ধ্রুৱ unsigned bits_per_word = 8 * পরিমাণ(a.vec_[0]);

	// The compiler ensures that the two operands are of equal size.
      প্রতিজ্ঞা(a.size() == b.size());
      vvp_vector2_t r (0, a.size());

      unsigned শব্দগুলি = (r.wid_ + bits_per_word - 1) / bits_per_word;

      বাবে (unsigned bdx = 0 ;  bdx < শব্দগুলি ;  bdx += 1) {
	    unsigned long tmpb = b.vec_[bdx];
	    যদি (tmpb == 0)
		  অনুৱর্ত;

	    বাবে (unsigned adx = 0 ;  adx < শব্দগুলি ;  adx += 1) {
		  unsigned long tmpa = a.vec_[adx];
		  যদি (tmpa == 0)
			অনুৱর্ত;

		  unsigned long নিচেৰ, hig;
		  multiply_long(tmpa, tmpb, নিচেৰ, hig);

		  unsigned long carry = 0;
		  বাবে (unsigned sdx = 0
			     ; (adx+bdx+sdx) < শব্দগুলি
			     ;  sdx += 1) {

			r.vec_[adx+bdx+sdx] = add_carry(r.vec_[adx+bdx+sdx],
							নিচেৰ, carry);
			নিচেৰ = hig;
			hig = 0;
		  }
	    }
      }

      ঘূৰাই r;
}

স্থির শূন্য div_mod (vvp_vector2_t dividend, vvp_vector2_t ভাজক,
		     vvp_vector2_t&quotient, vvp_vector2_t&remainder)
{

      quotient = vvp_vector2_t(0, dividend.size());

      যদি (ভাজক == quotient) {
	    cerr << "ERROR: division by zero, exiting." << endl;
	    exit(255);
      }

      যদি (dividend < ভাজক) {
	    remainder = dividend;
	    ঘূৰাই;
      }

      vvp_vector2_t mask (1, dividend.size());

	// Make the dividend 1 bit larger to prevent overflow of
	// divtmp in startup.
      dividend = vvp_vector2_t(dividend, dividend.size()+1);
      vvp_vector2_t divtmp (ভাজক, dividend.size());

      যেতিয়ালৈ (divtmp < dividend) {
	    divtmp <<= 1;
	    mask <<= 1;
      }

      যেতিয়ালৈ (dividend >= ভাজক) {
	    যদি (divtmp <= dividend) {
		  dividend -= divtmp;
		  quotient += mask;
	    }

	    divtmp >>= 1;
	    mask >>= 1;
      }

      remainder = vvp_vector2_t(dividend, mask.size());
}

vvp_vector2_t operator - (ধ্রুৱ vvp_vector2_t&that)
{
      vvp_vector2_t neg(that);
      যদি (neg.wid_ == 0) ঘূৰাই neg;

      ধ্রুৱ unsigned শব্দগুলি = (neg.wid_ + neg.BITS_PER_WORD-1) /
                                         neg.BITS_PER_WORD;
      বাবে (unsigned idx = 0 ;  idx < শব্দগুলি ;  idx += 1) {
	    neg.vec_[idx] = ~neg.vec_[idx];
      }
      neg += vvp_vector2_t(1, neg.wid_);

      ঘূৰাই neg;
}

vvp_vector2_t operator / (ধ্রুৱ vvp_vector2_t&dividend,
			  ধ্রুৱ vvp_vector2_t&ভাজক)
{
      vvp_vector2_t quot, rem;
      div_mod(dividend, ভাজক, quot, rem);
      ঘূৰাই quot;
}

vvp_vector2_t operator % (ধ্রুৱ vvp_vector2_t&dividend,
			  ধ্রুৱ vvp_vector2_t&ভাজক)
{
      vvp_vector2_t quot, rem;
      div_mod(dividend, ভাজক, quot, rem);
      ঘূৰাই rem;
}

তার্কিক operator > (ধ্রুৱ vvp_vector2_t&a, ধ্রুৱ vvp_vector2_t&b)
{
      ধ্রুৱ unsigned awords = (a.wid_ + vvp_vector2_t::BITS_PER_WORD-1) / vvp_vector2_t::BITS_PER_WORD;
      ধ্রুৱ unsigned bwords = (b.wid_ + vvp_vector2_t::BITS_PER_WORD-1) / vvp_vector2_t::BITS_PER_WORD;

      ধ্রুৱ unsigned শব্দগুলি = awords > bwords? awords : bwords;

      বাবে (unsigned idx = শব্দগুলি ;  idx > 0 ;  idx -= 1) {
	    unsigned long aw = (idx <= awords)? a.vec_[idx-1] : 0;
	    unsigned long bw = (idx <= bwords)? b.vec_[idx-1] : 0;

	    যদি (aw > bw)
		  ঘূৰাই সঁচা;
	    যদি (aw < bw)
		  ঘূৰাই মিছা;
      }

	// If the above loop finishes, then the vectors are equal.
      ঘূৰাই মিছা;
}

তার্কিক operator >= (ধ্রুৱ vvp_vector2_t&a, ধ্রুৱ vvp_vector2_t&b)
{
      ধ্রুৱ unsigned awords = (a.wid_ + vvp_vector2_t::BITS_PER_WORD-1) / vvp_vector2_t::BITS_PER_WORD;
      ধ্রুৱ unsigned bwords = (b.wid_ + vvp_vector2_t::BITS_PER_WORD-1) / vvp_vector2_t::BITS_PER_WORD;

      ধ্রুৱ unsigned শব্দগুলি = awords > bwords? awords : bwords;

      বাবে (unsigned idx = শব্দগুলি ;  idx > 0 ;  idx -= 1) {
	    unsigned long aw = (idx <= awords)? a.vec_[idx-1] : 0;
	    unsigned long bw = (idx <= bwords)? b.vec_[idx-1] : 0;

	    যদি (aw > bw)
		  ঘূৰাই সঁচা;
	    যদি (aw < bw)
		  ঘূৰাই মিছা;
      }

	// If the above loop finishes, then the vectors are equal.
      ঘূৰাই সঁচা;
}

তার্কিক operator < (ধ্রুৱ vvp_vector2_t&a, ধ্রুৱ vvp_vector2_t&b)
{
      ধ্রুৱ unsigned awords = (a.wid_ + vvp_vector2_t::BITS_PER_WORD-1) / vvp_vector2_t::BITS_PER_WORD;
      ধ্রুৱ unsigned bwords = (b.wid_ + vvp_vector2_t::BITS_PER_WORD-1) / vvp_vector2_t::BITS_PER_WORD;

      unsigned শব্দগুলি = awords;
      যদি (bwords > শব্দগুলি)
	    শব্দগুলি = bwords;

      বাবে (unsigned idx = শব্দগুলি ;  idx > 0 ;  idx -= 1) {
	    unsigned long aw = (idx <= awords)? a.vec_[idx-1] : 0;
	    unsigned long bw = (idx <= bwords)? b.vec_[idx-1] : 0;

	    যদি (aw < bw)
		  ঘূৰাই সঁচা;
	    যদি (aw > bw)
		  ঘূৰাই মিছা;
      }

	// If the above loop finishes, then the vectors are equal.
      ঘূৰাই মিছা;
}

তার্কিক operator <= (ধ্রুৱ vvp_vector2_t&a, ধ্রুৱ vvp_vector2_t&b)
{
	// XXXX For now, only support equal width vectors.
      প্রতিজ্ঞা(a.wid_ == b.wid_);

      ধ্রুৱ unsigned awords = (a.wid_ + vvp_vector2_t::BITS_PER_WORD-1) / vvp_vector2_t::BITS_PER_WORD;

      বাবে (unsigned idx = awords ;  idx > 0 ;  idx -= 1) {
	    যদি (a.vec_[idx-1] < b.vec_[idx-1])
		  ঘূৰাই সঁচা;
	    যদি (a.vec_[idx-1] > b.vec_[idx-1])
		  ঘূৰাই মিছা;
      }

	// If the above loop finishes, then the vectors are equal.
      ঘূৰাই সঁচা;
}

তার্কিক operator == (ধ্রুৱ vvp_vector2_t&a, ধ্রুৱ vvp_vector2_t&b)
{
      ধ্রুৱ unsigned awords = (a.wid_ + vvp_vector2_t::BITS_PER_WORD-1) / vvp_vector2_t::BITS_PER_WORD;
      ধ্রুৱ unsigned bwords = (b.wid_ + vvp_vector2_t::BITS_PER_WORD-1) / vvp_vector2_t::BITS_PER_WORD;

      ধ্রুৱ unsigned শব্দগুলি = awords > bwords? awords : bwords;

      বাবে (unsigned idx = শব্দগুলি ;  idx > 0 ;  idx -= 1) {
	    unsigned long aw = (idx <= awords)? a.vec_[idx-1] : 0;
	    unsigned long bw = (idx <= bwords)? b.vec_[idx-1] : 0;

	    যদি (aw > bw)
		  ঘূৰাই মিছা;
	    যদি (aw < bw)
		  ঘূৰাই মিছা;
      }

	// If the above loop finishes, then the vectors are equal.
      ঘূৰাই সঁচা;
}


vvp_vector4_t vector2_to_vector4(ধ্রুৱ vvp_vector2_t&that, unsigned wid)
{
      vvp_vector4_t res (wid);

      বাবে (unsigned idx = 0 ;  idx < res.size() ;  idx += 1) {
	    vvp_bit4_t bit = BIT4_0;

	    যদি (that.value(idx))
		  bit = BIT4_1;

	    res.set_bit(idx, bit);
      }

      ঘূৰাই res;
}

স্থির তার্কিক c4string_header_test(ধ্রুৱ ৱর্ণ*str)
{
      যদি ((str[0] != 'C') && (str[0] != 'c'))
	    ঘূৰাই মিছা;
      যদি ((str[1] != '4') || (str[2] != '<'))
	    ঘূৰাই মিছা;
      ঘূৰাই সঁচা;
}

তার্কিক c4string_test(ধ্রুৱ ৱর্ণ*str)
{
      যদি (!c4string_header_test(str))
	    ঘূৰাই মিছা;
      size_t মান_size = strspn(str+3, "01xz");
      যদি (str[3+মান_size] != '>')
	    ঘূৰাই মিছা;
      যদি (str[3+মান_size+1] != 0)
	    ঘূৰাই মিছা;

      ঘূৰাই সঁচা;
}

vvp_vector4_t c4string_to_vector4(ধ্রুৱ ৱর্ণ*str)
{
      প্রতিজ্ঞা(c4string_header_test(str));

      str += 3;
      ধ্রুৱ ৱর্ণ*tp = str + strspn(str,"01xz");
      প্রতিজ্ঞা(tp[0] == '>');

      vvp_vector4_t tmp (tp-str);

      বাবে (unsigned idx = 0 ;  idx < tmp.size() ;  idx += 1) {
	    vvp_bit4_t bit;
	    ৱিকল্প (str[idx]) {
		পক্ষ '0':
		  bit = BIT4_0;
		  ভঙ্গ;
		পক্ষ '1':
		  bit = BIT4_1;
		  ভঙ্গ;
		পক্ষ 'x':
		  bit = BIT4_X;
		  ভঙ্গ;
		পক্ষ 'z':
		  bit = BIT4_Z;
		  ভঙ্গ;
		স্ৱভাৱ:
		  fprintf(stderr, "Unsupported bit value %c(%d).\n", str[idx],
		          str[idx]);
		  প্রতিজ্ঞা(0);
		  bit = BIT4_0;
		  ভঙ্গ;
	    }
	    tmp.set_bit(tmp.size()-idx-1, bit);
      }

      ঘূৰাই tmp;
}

ostream& operator<< (ostream&out, ধ্রুৱ vvp_vector2_t&that)
{
      যদি (that.is_NaN()) {
	    out << "NaN";

      } অন্যথা {
	    out << vector2_to_vector4(that, that.size());
      }
      ঘূৰাই out;
}

vvp_vector8_t::vvp_vector8_t(ধ্রুৱ vvp_vector8_t&that)
{
      size_ = that.size_;
      যদি (size_ <= পরিমাণ(val_)) {
	    memcpy(val_, that.val_, পরিমাণ val_);
      } অন্যথা {
	    ptr_ = নূতন unsigned ৱর্ণ[size_];
	    memcpy(ptr_, that.ptr_, size_);
      }
}

vvp_vector8_t::vvp_vector8_t(ধ্রুৱ vvp_vector4_t&that,
			     unsigned str0, unsigned str1)
: size_(that.size())
{
      যদি (size_ == 0)
	    ঘূৰাই;

      যদি (size_ <= পরিমাণ(val_)) {
	    ptr_ = 0; // Prefill all val_ bytes
	    বাবে (unsigned idx = 0 ; idx < size_ ; idx += 1)
		  val_[idx] = vvp_scalar_t(that.value(idx),str0, str1).raw();
      } অন্যথা {
	    ptr_ = নূতন unsigned ৱর্ণ[size_];
	    বাবে (unsigned idx = 0 ;  idx < size_ ;  idx += 1)
		  ptr_[idx] = vvp_scalar_t(that.value(idx), str0, str1).raw();
      }
}

vvp_vector8_t::vvp_vector8_t(ধ্রুৱ vvp_vector2_t&that,
			     unsigned str0, unsigned str1)
: size_(that.size())
{
      যদি (size_ == 0)
	    ঘূৰাই;

      যদি (size_ <= পরিমাণ(val_)) {
	    ptr_ = 0;
	    বাবে (unsigned idx = 0 ; idx < size_ ; idx += 1)
		  val_[idx] = vvp_scalar_t(that.value(idx)? BIT4_1:BIT4_0, str0, str1).raw();
      } অন্যথা {
	    ptr_ = নূতন unsigned ৱর্ণ[size_];
	    বাবে (unsigned idx = 0 ; idx < size_ ; idx += 1)
		  ptr_[idx] = vvp_scalar_t(that.value(idx)? BIT4_1:BIT4_0, str0, str1).raw();
      }
}

ধ্রুৱ vvp_vector8_t vvp_vector8_t::nil;

vvp_vector8_t& vvp_vector8_t::operator= (ধ্রুৱ vvp_vector8_t&that)
{
	// Assign to self.
      যদি (এতত্ == &that)
	    ঘূৰাই *এতত্;

      যদি (size_ != that.size_) {
	    যদি (size_ > পরিমাণ(val_))
		  delete[]ptr_;
	    size_ = 0;
      }

      যদি (that.size_ == 0) {
	    প্রতিজ্ঞা(size_ == 0);
	    ঘূৰাই *এতত্;
      }

      যদি (that.size_ <= পরিমাণ(val_)) {
	    size_ = that.size_;
	    memcpy(val_, that.val_, পরিমাণ(val_));
	    ঘূৰাই *এতত্;
      }

      যদি (size_ == 0) {
	    size_ = that.size_;
	    ptr_ = নূতন unsigned ৱর্ণ[size_];
      }

      memcpy(ptr_, that.ptr_, size_);

      ঘূৰাই *এতত্;
}

vvp_vector8_t vvp_vector8_t::subvalue(unsigned base, unsigned wid) ধ্রুৱ
{
      vvp_vector8_t tmp (wid);

      unsigned ৱর্ণ*tmp_ptr = tmp.size_ <= পরিমাণ(val_) ? tmp.val_ : tmp.ptr_;
      ধ্রুৱ unsigned ৱর্ণ*use_ptr = size_ <= পরিমাণ(val_) ? val_ : ptr_;

      unsigned idx = 0;
      যেতিয়ালৈ ((idx < wid) && (base+idx < size_)) {
	    tmp_ptr[idx] = use_ptr[base+idx];
	    idx += 1;
      }

      ঘূৰাই tmp;
}

শূন্য vvp_vector8_t::set_vec(unsigned base, ধ্রুৱ vvp_vector8_t&that)
{
      প্রতিজ্ঞা((base+that.size()) <= size());
      বাবে (unsigned idx = 0 ; idx < that.size() ; idx += 1)
	    set_bit(base+idx, that.value(idx));
}

vvp_vector8_t part_expand(ধ্রুৱ vvp_vector8_t&that, unsigned wid, unsigned off)
{
      প্রতিজ্ঞা(off < wid);
      vvp_vector8_t tmp (wid);

      unsigned ৱর্ণ* tmp_ptr = tmp.size_<= পরিমাণ(tmp.val_) ?
                               tmp.val_ : tmp.ptr_;
      ধ্রুৱ unsigned ৱর্ণ* that_ptr = that.size_<= পরিমাণ(that.val_) ?
                                      that.val_ : that.ptr_;

      unsigned idx = off;

      যেতিয়ালৈ (idx < wid && that.size_ > (idx-off)) {
	    tmp_ptr[idx] = that_ptr[idx-off];
	    idx += 1;
      }

      ঘূৰাই tmp;
}

স্থির তার্কিক c8string_header_test(ধ্রুৱ ৱর্ণ*str)
{
      যদি ((str[0] != 'C') && (str[0] != 'c'))
	    ঘূৰাই মিছা;
      যদি ((str[1] != '8') || (str[2] != '<'))
	    ঘূৰাই মিছা;
      ঘূৰাই সঁচা;
}

তার্কিক c8string_test(ধ্রুৱ ৱর্ণ*str)
{
      যদি (!c8string_header_test(str))
	    ঘূৰাই মিছা;

      ধ্রুৱ ৱর্ণ*cp = str+3;
      বাবে (;; cp += 1) {
	    যদি (cp[0] == '>' && cp[1] == 0) ঘূৰাই সঁচা;
	    যদি (cp[0] >= '0' && cp[0] <= '9') অনুৱর্ত;
	    যদি (cp[0] == 'x') অনুৱর্ত;
	    যদি (cp[0] == 'z') অনুৱর্ত;
	    ঘূৰাই মিছা;
      }
      ঘূৰাই মিছা;
}
/*
 * The format of a C8<> string is:
 *   C8<aaabbbccc...>
 * where aaa... is a 3 character bit descriptor.
 */
vvp_vector8_t c8string_to_vector8(ধ্রুৱ ৱর্ণ*str)
{
      প্রতিজ্ঞা(c8string_header_test(str));

      size_t vsize = strlen(str)-4;
      প্রতিজ্ঞা(vsize%3 == 0);
      vsize /= 3;
      vvp_vector8_t tmp (vsize);

      বাবে (size_t idx = 0 ; idx < vsize ; idx += 1) {
	    ধ্রুৱ ৱর্ণ*cp = str+3+3*idx;
	    vvp_bit4_t bit = BIT4_X;
	    unsigned dr0 = cp[0]-'0';
	    unsigned dr1 = cp[1]-'0';
	    ৱিকল্প (cp[2]) {
		পক্ষ '0':
		  bit = BIT4_0;
		  ভঙ্গ;
		পক্ষ '1':
		  bit = BIT4_1;
		  ভঙ্গ;
		পক্ষ 'x':
		  bit = BIT4_X;
		  ভঙ্গ;
		পক্ষ 'z':
		  bit = BIT4_Z;
		  ভঙ্গ;
	    }
	    tmp.set_bit(vsize-idx-1, vvp_scalar_t(bit, dr0, dr1));
      }

      ঘূৰাই tmp;
}

ostream& operator<<(ostream&out, ধ্রুৱ vvp_vector8_t&that)
{
      out << "C8<";
      বাবে (unsigned idx = 0 ;  idx < that.size() ; idx += 1)
	    out << that.value(that.size()-idx-1);

      out << ">";
      ঘূৰাই out;
}

vvp_net_fun_t::vvp_net_fun_t()
{
      গুনতি_functors += 1;
}

vvp_net_fun_t::~vvp_net_fun_t()
{
}

শূন্য vvp_net_fun_t::recv_vec4(vvp_net_ptr_t, ধ্রুৱ vvp_vector4_t&,
                              vvp_context_t)
{
      fprintf(stderr, "internal error: %s: recv_vec4 not implemented\n",
	      typeid(*এতত্).name());
      প্রতিজ্ঞা(0);
}

শূন্য vvp_net_fun_t::recv_vec4_pv_(vvp_net_ptr_t p, ধ্রুৱ vvp_vector4_t&bit,
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

      প্রতিজ্ঞা(base + bit.size() <= vwid);

      vvp_vector4_t tmp(vwid, BIT4_Z);
      tmp.set_vec(base, bit);
      recv_vec4(p, tmp, 0);
}

শূন্য vvp_net_fun_t::recv_vec4_pv(vvp_net_ptr_t, ধ্রুৱ vvp_vector4_t&bit,
                                 unsigned base, unsigned vwid, vvp_context_t)
{
      cerr << "internal error: " << typeid(*এতত্).name() << ": "
	   << "recv_vec4_pv(" << bit << ", " << base
	   << ", " << vwid << ") not implemented" << endl;
      প্রতিজ্ঞা(0);
}

শূন্য vvp_net_fun_t::recv_vec8(vvp_net_ptr_t port, ধ্রুৱ vvp_vector8_t&bit)
{
      recv_vec4(port, reduce4(bit), 0);
}

শূন্য vvp_net_fun_t::recv_vec8_pv_(vvp_net_ptr_t p, ধ্রুৱ vvp_vector8_t&bit,
				  unsigned base, unsigned vwid)
{
	// This is the strength-aware version of recv_vec4_pv_.

      প্রতিজ্ঞা(base + bit.size() <= vwid);

      vvp_vector8_t tmp(vwid);
      tmp.set_vec(base, bit);
      recv_vec8(p, tmp);
}

শূন্য vvp_net_fun_t::recv_vec8_pv(vvp_net_ptr_t port, ধ্রুৱ vvp_vector8_t&bit,
				 unsigned base, unsigned vwid)
{
      recv_vec4_pv(port, reduce4(bit), base, vwid, 0);
}

শূন্য vvp_net_fun_t::recv_real(vvp_net_ptr_t, দ্ৱিগুণ bit, vvp_context_t)
{
      fprintf(stderr, "internal error: %s: recv_real(%f) not implemented\n",
	      typeid(*এতত্).name(), bit);
      প্রতিজ্ঞা(0);
}

শূন্য vvp_net_fun_t::recv_string(vvp_net_ptr_t, ধ্রুৱ std::string&bit, vvp_context_t)
{
      fprintf(stderr, "internal error: %s: recv_string(%s) not implemented\n",
	      typeid(*এতত্).name(), bit.c_str());
      প্রতিজ্ঞা(0);
}

শূন্য vvp_net_fun_t::recv_object(vvp_net_ptr_t, vvp_object_t, vvp_context_t)
{
      fprintf(stderr, "internal error: %s: recv_object(...) not implemented\n",
	      typeid(*এতত্).name());
      প্রতিজ্ঞা(0);
}

শূন্য vvp_net_fun_t::force_flag(তার্কিক)
{
}

/* **** vvp_fun_drive methods **** */

vvp_fun_drive::vvp_fun_drive(unsigned str0, unsigned str1)
{
      প্রতিজ্ঞা(str0 < 8);
      প্রতিজ্ঞা(str1 < 8);

      drive0_ = str0;
      drive1_ = str1;
}

vvp_fun_drive::~vvp_fun_drive()
{
}

শূন্য vvp_fun_drive::recv_vec4(vvp_net_ptr_t port, ধ্রুৱ vvp_vector4_t&bit,
                              vvp_context_t)
{
      প্রতিজ্ঞা(port.port() == 0);
      port.ptr()->send_vec8(vvp_vector8_t(bit, drive0_, drive1_));
}

শূন্য vvp_fun_drive::recv_vec4_pv(vvp_net_ptr_t ptr, ধ্রুৱ vvp_vector4_t&bit,
				 unsigned base, unsigned vwid, vvp_context_t ctx)
{
      recv_vec4_pv_(ptr, bit, base, vwid, ctx);
}

/* **** vvp_wide_fun_* methods **** */

vvp_wide_fun_core::vvp_wide_fun_core(vvp_net_t*net, unsigned nports)
{
      ptr_ = net;
      nports_ = nports;
      port_মানসমূহ = 0;
      port_rvalues_ = 0;
}

vvp_wide_fun_core::~vvp_wide_fun_core()
{
      delete[] port_মানসমূহ;
      delete[] port_rvalues_;
}

শূন্য vvp_wide_fun_core::propagate_vec4(ধ্রুৱ vvp_vector4_t&bit,
				       vvp_time64_t delay)
{
      যদি (delay)
	    schedule_propagate_vector(ptr_, delay, bit);
      অন্যথা
	    ptr_->send_vec4(bit, 0);
}

শূন্য vvp_wide_fun_core::propagate_real(দ্ৱিগুণ bit,
				       vvp_time64_t delay)
{
      যদি (delay) {
	      // schedule_assign_vector(ptr_->out, bit, delay);
	    প্রতিজ্ঞা(0); // Need a real-value version of assign_vector.
      } অন্যথা {
	    ptr_->send_real(bit, 0);
      }
}


unsigned vvp_wide_fun_core::port_গুনতি() ধ্রুৱ
{
      ঘূৰাই nports_;
}

vvp_vector4_t& vvp_wide_fun_core::value(unsigned idx)
{
      প্রতিজ্ঞা(idx < nports_);
      যদি (port_মানসমূহ == 0)
	    port_মানসমূহ = নূতন vvp_vector4_t [nports_];
      ঘূৰাই port_মানসমূহ[idx];
}

দ্ৱিগুণ vvp_wide_fun_core::মান_r(unsigned idx)
{
      প্রতিজ্ঞা(idx < nports_);
      ঘূৰাই port_rvalues_? port_rvalues_[idx] : 0.0;
}

শূন্য vvp_wide_fun_core::recv_real_from_inputs(unsigned)
{
      প্রতিজ্ঞা(0);
}

শূন্য vvp_wide_fun_core::dispatch_vec4_from_প্ৰবেশ(unsigned port,
						  ধ্রুৱ vvp_vector4_t&bit)
{
      প্রতিজ্ঞা(port < nports_);
      যদি (port_মানসমূহ == 0) port_মানসমূহ = নূতন vvp_vector4_t [nports_];
      port_মানসমূহ[port] = bit;
      recv_vec4_from_inputs(port);
}

শূন্য vvp_wide_fun_core::dispatch_real_from_প্ৰবেশ(unsigned port,
						  দ্ৱিগুণ bit)
{
      প্রতিজ্ঞা(port < nports_);
      যদি (port_rvalues_ == 0) port_rvalues_ = নূতন দ্ৱিগুণ[nports_];
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

শূন্য vvp_wide_fun_t::recv_vec4(vvp_net_ptr_t port, ধ্রুৱ vvp_vector4_t&bit,
                               vvp_context_t)
{
      unsigned pidx = port_base_ + port.port();
      core_->dispatch_vec4_from_প্ৰবেশ(pidx, bit);
}

শূন্য vvp_wide_fun_t::recv_vec4_pv(vvp_net_ptr_t ptr, ধ্রুৱ vvp_vector4_t&bit,
				  unsigned base, unsigned vwid, vvp_context_t ctx)
{
      recv_vec4_pv_(ptr, bit, base, vwid, ctx);
}

শূন্য vvp_wide_fun_t::recv_real(vvp_net_ptr_t port, দ্ৱিগুণ bit,
                               vvp_context_t)
{
      unsigned pidx = port_base_ + port.port();
      core_->dispatch_real_from_প্ৰবেশ(pidx, bit);
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

unsigned vvp_scalar_t::strength0() ধ্রুৱ
{
      ঘূৰাই STREN0(মান_2);
}

unsigned vvp_scalar_t::strength1() ধ্রুৱ
{
      ঘূৰাই STREN1(মান_2);
}

ostream& operator <<(ostream&out, vvp_scalar_t a)
{
      out << a.strength0() << a.strength1();
      ৱিকল্প (a.value()) {
	  পক্ষ BIT4_0:
	    out << "0";
	    ভঙ্গ;
	  পক্ষ BIT4_1:
	    out << "1";
	    ভঙ্গ;
	  পক্ষ BIT4_X:
	    out << "X";
	    ভঙ্গ;
	  পক্ষ BIT4_Z:
	    out << "Z";
	    ভঙ্গ;
      }
      ঘূৰাই out;
}

/*
 * This function is only called if the actual interface function rules
 * out some of the easy cases. If we get here, we can assume that
 * neither of the values is HiZ, and the values are not exactly equal.
 */
vvp_scalar_t fully_featured_resolv_(vvp_scalar_t a, vvp_scalar_t b)
{

      যদি (UNAMBIG(a.মান_2) && UNAMBIG(b.মান_2)) {

	      /* If both signals are unambiguous, simply choose
		 the stronger. If they have the same strength
		 but different values, then this becomes
		 ambiguous. */

	    যদি ((b.মান_2&0x07) > (a.মান_2&0x07)) {

		    /* b value is stronger. Take it. */
		  ঘূৰাই b;

	    } অন্যথা যদি ((b.মান_2&0x77) == (a.মান_2&0x77)) {

		    // Strengths are the same. Since we know already
		    // that the values are not the same, Make value
		    // into "x".
		  vvp_scalar_t tmp (a);
		  tmp.মান_2 = (tmp.মান_2&0x77) | 0x80;
		  ঘূৰাই tmp;

	    } অন্যথা {

		    /* Must be "a" is the stronger one. */
		  ঘূৰাই a;
	    }

      }

	/* If one of the signals is unambiguous, then it
	   will sweep up the weaker parts of the ambiguous
	   signal. The result may be ambiguous, or maybe not. */

      যদি (UNAMBIG(a.মান_2)) {
	    vvp_scalar_t res;

	    যদি ((a.মান_2&0x70) > (b.মান_2&0x70))
		  res.মান_2 |= a.মান_2&0xf0;
	    অন্যথা
		  res.মান_2 |= b.মান_2&0xf0;

	    যদি ((a.মান_2&0x07) > (b.মান_2&0x07))
		  res.মান_2 |= a.মান_2&0x0f;
	    অন্যথা
		  res.মান_2 |= b.মান_2&0x0f;

	    ঘূৰাই res;

      } অন্যথা যদি (UNAMBIG(b.মান_2)) {

	    vvp_scalar_t res;

	    যদি ((b.মান_2&0x70) > (a.মান_2&0x70))
		  res.মান_2 |= b.মান_2&0xf0;
	    অন্যথা
		  res.মান_2 |= a.মান_2&0xf0;

	    যদি ((b.মান_2&0x07) > (a.মান_2&0x07))
		  res.মান_2 |= b.মান_2&0x0f;
	    অন্যথা
		  res.মান_2 |= a.মান_2&0x0f;

	    ঘূৰাই res;

      }


	/* If both signals are ambiguous, then the result
	   has an even wider ambiguity. */

      unsigned tmp = 0;
      পূর্ণাঙ্ক sv1a = (a.মান_2 & 0x80) ? STREN1(a.মান_2) : - STREN1(a.মান_2);
      পূর্ণাঙ্ক sv0a = (a.মান_2 & 0x08) ? STREN0(a.মান_2) : - STREN0(a.মান_2);
      পূর্ণাঙ্ক sv1b = (b.মান_2 & 0x80) ? STREN1(b.মান_2) : - STREN1(b.মান_2);
      পূর্ণাঙ্ক sv0b = (b.মান_2 & 0x08) ? STREN0(b.মান_2) : - STREN0(b.মান_2);

      পূর্ণাঙ্ক sv1 = sv1a;
      পূর্ণাঙ্ক sv0 = sv0a;

      যদি (sv0a > sv1)
	    sv1 = sv0a;
      যদি (sv1b > sv1)
	    sv1 = sv1b;
      যদি (sv0b > sv1)
	    sv1 = sv0b;

      যদি (sv1a < sv0)
	    sv0 = sv1a;
      যদি (sv1b < sv0)
	    sv0 = sv1b;
      যদি (sv0b < sv0)
	    sv0 = sv0b;

      যদি (sv1 > 0) {
	    tmp |= 0x80;
	    tmp |= sv1 << 4;
      } অন্যথা {
	      /* Set the MSB when both arguments MSBs are set. This
		 can only happen if both one strengths are zero. */
	    tmp |= (a.মান_2&b.মান_2)&0x80;
	    tmp |= (-sv1) << 4;
      }

      যদি (sv0 > 0) {
	    tmp |= 0x08;
	    tmp |= sv0;
      } অন্যথা {
	    tmp |= (-sv0);
      }

      vvp_scalar_t res;
      res.মান_2 = tmp;

	/* Canonicalize the HiZ value. */
      যদি ((res.মান_2&0x77) == 0)
	    res.মান_2 = 0;

      ঘূৰাই res;
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

vvp_vector4_t reduce4(ধ্রুৱ vvp_vector8_t&that)
{
      vvp_vector4_t out (that.size());
      বাবে (unsigned idx = 0 ;  idx < out.size() ;  idx += 1)
	    out.set_bit(idx, that.value(idx).value());

      ঘূৰাই out;
}

vvp_bit4_t compare_gtge(ধ্রুৱ vvp_vector4_t&lef, ধ্রুৱ vvp_vector4_t&rig,
			vvp_bit4_t out_if_equal)
{
      unsigned min_size = lef.size();
      যদি (rig.size() < min_size)
	    min_size = rig.size();

	// If one of the inputs is nil, treat is as all X values, and
	// that makes the result BIT4_X.
      যদি (min_size == 0)
	    ঘূৰাই BIT4_X;

	// As per the IEEE1364 definition of >, >=, < and <=, if there
	// are any X or Z values in either of the operand vectors,
	// then the result of the compare is BIT4_X.

	// Check for X/Z in the left operand
      যদি (lef.has_xz())
	    ঘূৰাই BIT4_X;

	// Check for X/Z in the right operand
      যদি (rig.has_xz())
	    ঘূৰাই BIT4_X;

      বাবে (unsigned idx = lef.size() ; idx > rig.size() ;  idx -= 1) {
	    যদি (lef.value(idx-1) == BIT4_1)
		  ঘূৰাই BIT4_1;
      }

      বাবে (unsigned idx = rig.size() ; idx > lef.size() ;  idx -= 1) {
	    যদি (rig.value(idx-1) == BIT4_1)
		  ঘূৰাই BIT4_0;
      }

      বাবে (unsigned idx = min_size ; idx > 0 ;  idx -= 1) {
	    vvp_bit4_t lv = lef.value(idx-1);
	    vvp_bit4_t rv = rig.value(idx-1);

	    যদি (lv == rv)
		  অনুৱর্ত;

	    যদি (lv == BIT4_1)
		  ঘূৰাই BIT4_1;
	    অন্যথা
		  ঘূৰাই BIT4_0;
      }

      ঘূৰাই out_if_equal;
}

vvp_bit4_t compare_gtge_signed(ধ্রুৱ vvp_vector4_t&a,
			       ধ্রুৱ vvp_vector4_t&b,
			       vvp_bit4_t out_if_equal)
{
      প্রতিজ্ঞা(a.size() == b.size());

      unsigned sign_idx = a.size()-1;
      vvp_bit4_t a_sign = a.value(sign_idx);
      vvp_bit4_t b_sign = b.value(sign_idx);

      যদি (bit4_is_xz(a_sign))
	    ঘূৰাই BIT4_X;
      যদি (bit4_is_xz(b_sign))
	    ঘূৰাই BIT4_X;

      যদি (a_sign == b_sign)
	    ঘূৰাই compare_gtge(a, b, out_if_equal);

      যদি (a.has_xz())
	    ঘূৰাই BIT4_X;

      যদি (b.has_xz())
	    ঘূৰাই BIT4_X;

      যদি(a_sign == BIT4_0)
	    ঘূৰাই BIT4_1;
      অন্যথা
	    ঘূৰাই BIT4_0;
}
