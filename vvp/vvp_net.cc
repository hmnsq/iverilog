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

using நாமாகாஶ std;

/* This is the size of an unsigned long in bits. This is just a
   convenience macro. */
# define CPU_WORD_BITS (8*அளவு(unsigned long))
# define உச்சி_BIT (1UL << (CPU_WORD_BITS-1))

permaheap vvp_net_fun_t::heap_;
permaheap vvp_net_fil_t::heap_;

// Allocate around 1Megabyte/chunk.
நிலையான மாறிலி size_t VVP_NET_CHUNK = 1024*1024/அளவு(vvp_net_t);
நிலையான vvp_net_t*vvp_net_alloc_table = வெற்று;
#ifdef CHECK_WITH_VALGRIND
நிலையான vvp_net_t **vvp_net_pool = வெற்று;
நிலையான unsigned vvp_net_pool_மொத்தம் = 0;
#endif
நிலையான size_t vvp_net_alloc_remaining = 0;
// For statistics, count the vvp_nets allocated and the bytes of alloc
// chunks allocated.
unsigned long மொத்தம்_vvp_nets = 0;
size_t size_vvp_nets = 0;

வெறுமை* vvp_net_t::operator புதிய (size_t size)
{
      ப்ரதிஜ்ஞா(size == அளவு(vvp_net_t));
      எனில் (vvp_net_alloc_remaining == 0) {
	    vvp_net_alloc_table = ::புதிய vvp_net_t[VVP_NET_CHUNK];
	    vvp_net_alloc_remaining = VVP_NET_CHUNK;
	    size_vvp_nets += size*VVP_NET_CHUNK;
#ifdef CHECK_WITH_VALGRIND
	    VALGRIND_MAKE_MEM_NOACCESS(vvp_net_alloc_table, size*VVP_NET_CHUNK);
	    VALGRIND_CREATE_MEMPOOL(vvp_net_alloc_table, 0, 0);
	    vvp_net_pool_மொத்தம் += 1;
	    vvp_net_pool = static_cast<vvp_net_t **>(realloc(vvp_net_pool,
	                   vvp_net_pool_மொத்தம்*அளவு(vvp_net_t **)));
	    vvp_net_pool[vvp_net_pool_மொத்தம்-1] = vvp_net_alloc_table;
#endif
      }

      vvp_net_t*return_this = vvp_net_alloc_table;
#ifdef CHECK_WITH_VALGRIND
      VALGRIND_MEMPOOL_ALLOC(vvp_net_pool[vvp_net_pool_மொத்தம்-1],
                             return_this, size);
      return_this->pool = vvp_net_pool[vvp_net_pool_மொத்தம்-1];
#endif
      vvp_net_alloc_table += 1;
      vvp_net_alloc_remaining -= 1;
      மொத்தம்_vvp_nets += 1;
      திரும்பு return_this;
}

#ifdef CHECK_WITH_VALGRIND
நிலையான map<vvp_net_t*, தருக்க> vvp_net_map;
நிலையான map<sfunc_core*, தருக்க> sfunc_map;
நிலையான map<vvp_udp_fun_core*, தருக்க> udp_map;
நிலையான map<resolv_core*, தருக்க> resolv_map;
நிலையான vvp_net_t **local_net_pool = 0;
நிலையான unsigned local_net_pool_மொத்தம் = 0;

வெறுமை pool_local_net(vvp_net_t*net)
{
      local_net_pool_மொத்தம் += 1;
      local_net_pool = static_cast<vvp_net_t **>(realloc(local_net_pool,
                       local_net_pool_மொத்தம்*அளவு(vvp_net_t **)));
      local_net_pool[local_net_pool_மொத்தம்-1] = net;
}

வெறுமை vvp_net_delete(vvp_net_t *item)
{
      vvp_net_map[item] = உண்மை;
      எனில் (sfunc_core*tmp = dynamic_cast<sfunc_core*> (item->fun)) {
	    sfunc_map[tmp] = உண்மை;
      }
      எனில் (vvp_udp_fun_core*tmp = dynamic_cast<vvp_udp_fun_core*> (item->fun)) {
	    udp_map[tmp] = உண்மை;
      }
      எனில் (resolv_core*tmp = dynamic_cast<resolv_core*> (item->fun)) {
	    resolv_map[tmp] = உண்மை;
      }
	/* Only delete static object variables. */
      எனில் (vvp_fun_signal_object_sa*tmp = dynamic_cast<vvp_fun_signal_object_sa*> (item->fun)) {
	    delete tmp;
      }
	/* Only delete static string variables. */
      எனில் (vvp_fun_signal_string_sa*tmp = dynamic_cast<vvp_fun_signal_string_sa*> (item->fun)) {
	    delete tmp;
      }
}

வெறுமை vvp_net_pool_delete()
{
      unsigned long vvp_nets_del = 0;

      க்கு (unsigned idx = 0; idx < local_net_pool_மொத்தம்; idx += 1) {
	    vvp_net_delete(local_net_pool[idx]);
      }
      விடுவி(local_net_pool);
      local_net_pool = 0;
      local_net_pool_மொத்தம் = 0;

      map<vvp_net_t*, தருக்க>::iterator iter;
      க்கு (iter = vvp_net_map.begin(); iter != vvp_net_map.end(); ++ iter ) {
	    vvp_nets_del += 1;
	    VALGRIND_MEMPOOL_FREE(iter->first->pool, iter->first);
      }
      vvp_net_map.clear();

      map<sfunc_core*, தருக்க>::iterator siter;
      க்கு (siter = sfunc_map.begin(); siter != sfunc_map.end(); ++ siter ) {
	    delete siter->first;
      }
      sfunc_map.clear();

      map<vvp_udp_fun_core*, தருக்க>::iterator uiter;
      க்கு (uiter = udp_map.begin(); uiter != udp_map.end(); ++ uiter ) {
	    delete uiter->first;
      }
      udp_map.clear();

      map<resolv_core*, தருக்க>::iterator riter;
      க்கு (riter = resolv_map.begin(); riter != resolv_map.end(); ++ riter ) {
	    delete riter->first;
      }
      resolv_map.clear();

      எனில் (RUNNING_ON_VALGRIND && (vvp_nets_del != மொத்தம்_vvp_nets)) {
	    fflush(வெற்று);
	    VALGRIND_PRINTF("Error: vvp missed deleting %ld of %lu net(s).",
	                    (long) மொத்தம்_vvp_nets - vvp_nets_del,
	                    மொத்தம்_vvp_nets);
      }

      க்கு (unsigned idx = 0; idx < vvp_net_pool_மொத்தம்; idx += 1) {
	    VALGRIND_DESTROY_MEMPOOL(vvp_net_pool[idx]);
	    ::delete [] vvp_net_pool[idx];
      }
      விடுவி(vvp_net_pool);
      vvp_net_pool = வெற்று;
      vvp_net_pool_மொத்தம் = 0;
}
#endif

வெறுமை vvp_net_t::operator delete(வெறுமை*)
{
      ப்ரதிஜ்ஞா(0);
}

vvp_net_t::vvp_net_t()
: out_(vvp_net_ptr_t(0,0))
{
      fun = 0;
      fil = 0;
}

வெறுமை vvp_net_t::link(vvp_net_ptr_t port_to_link)
{
      vvp_net_t*net = port_to_link.ptr();

	// Connect nodes with vvp_fun_modpath_src to the head of the
	// linked list, so that vvp_fun_modpath_src are always evaluated
	// before their respective vvp_fun_modpath
      எனில் (dynamic_cast<vvp_fun_modpath_src*>(net->fun)) {
	    net->port[port_to_link.port()] = out_;
	    out_ = port_to_link;
	// All other nodes connect after the last vvp_fun_modpath_src
      } இல்லையெனில் {
	      // List is empty or first node has no vvp_fun_modpath_src
	      // then just insert the node into the head of linked list
	    எனில் (out_.ptr() == nullptr || ! dynamic_cast<vvp_fun_modpath_src*>(out_.ptr()->fun)) {
		  net->port[port_to_link.port()] = out_;
		  out_ = port_to_link;
		  திரும்பு;
	    }

	    vvp_net_ptr_t cur = out_;
	    vvp_net_ptr_t prev = vvp_net_ptr_t(nullptr,0);

	      // Traverse through linked list
	    போது (dynamic_cast<vvp_fun_modpath_src*>(cur.ptr()->fun)) {
		  prev = cur;
		  cur = cur.ptr()->port[cur.port()];
		  எனில் (! cur.ptr()) நிறுத்து; // End of list
	    }

	    ப்ரதிஜ்ஞா(prev.ptr());

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
வெறுமை vvp_net_t::unlink(vvp_net_ptr_t dst_ptr)
{
      vvp_net_t*net = dst_ptr.ptr();
      unsigned net_port = dst_ptr.port();

      எனில் (out_ == dst_ptr) {
	      /* If the drive fan-out list starts with this pointer,
		 then the unlink is easy. Pull the list forward. */
	    out_ = net->port[net_port];
      } இல்லையெனில் எனில் (! out_.nil()) {
	      /* Scan the linked list, looking for the net_ptr_t
		 pointer *before* the one we wish to remove. */
	    vvp_net_ptr_t cur = out_;
	    ப்ரதிஜ்ஞா(!cur.nil());
	    vvp_net_t*cur_net = cur.ptr();
	    unsigned cur_port = cur.port();
	    போது (cur_net && cur_net->port[cur_port] != dst_ptr) {
		  cur = cur_net->port[cur_port];
		  cur_net = cur.ptr();
		  cur_port = cur.port();
	    }
	      /* Unlink. */
	    எனில் (cur_net) cur_net->port[cur_port] = net->port[net_port];
      }

      net->port[net_port] = vvp_net_ptr_t(0,0);
}

வெறுமை vvp_net_t::மொத்தம்_drivers(unsigned idx, unsigned counts[4])
{
      counts[0] = 0;
      counts[1] = 0;
      counts[2] = 0;
      counts[3] = 0;

        /* $countdrivers can only be used on wires. */
      vvp_wire_base*wire=dynamic_cast<vvp_wire_base*>(fil);
      ப்ரதிஜ்ஞா(wire);

      எனில் (wire->is_forced(idx))
            counts[3] = 1;

        /* If the net has multiple drivers, we need to interrogate the
           resolver network to get the driven values. */
      எனில் (resolv_core*resolver = dynamic_cast<resolv_core*>(fun)) {
            resolver->மொத்தம்_drivers(idx, counts);
            திரும்பு;
      }
      எனில் (vvp_island_port*resolver = dynamic_cast<vvp_island_port*>(fun)) {
            resolver->மொத்தம்_drivers(idx, counts);
            திரும்பு;
      }

        /* If the functor is not a resolver, there is only one driver, so
           we can just interrogate the filter to get the driven value. */
      update_driver_counts(wire->driven_மதிப்பு(idx), counts);
}

வெறுமை vvp_net_fun_t::operator delete(வெறுமை*)
{
#ifndef CHECK_WITH_VALGRIND
      ப்ரதிஜ்ஞா(0);
#endif
}


vvp_net_fil_t::vvp_net_fil_t()
{
      force_link_ = 0;
      force_propagate_ = பொய்;
      மொத்தம்_filters += 1;
}

vvp_net_fil_t::~vvp_net_fil_t()
{
      ப்ரதிஜ்ஞா(force_link_ == 0);
}

vvp_net_fil_t::prop_t vvp_net_fil_t::filter_vec4(மாறிலி vvp_vector4_t&,
                                                 vvp_vector4_t&,
                                                 unsigned, unsigned)
{
      திரும்பு PROP;
}

vvp_net_fil_t::prop_t vvp_net_fil_t::filter_vec8(மாறிலி vvp_vector8_t&,
                                                 vvp_vector8_t&,
                                                 unsigned, unsigned)
{
      திரும்பு PROP;
}

vvp_net_fil_t::prop_t vvp_net_fil_t::filter_real(இரட்டை&)
{
      திரும்பு PROP;
}

vvp_net_fil_t::prop_t vvp_net_fil_t::filter_string(மாறிலி string&)
{
      திரும்பு PROP;
}

vvp_net_fil_t::prop_t vvp_net_fil_t::filter_object(vvp_object_t&)
{
      திரும்பு PROP;
}

வெறுமை vvp_net_fil_t::force_mask(மாறிலி vvp_vector2_t&mask)
{
      எனில் (force_mask_.size() == 0)
	    force_mask_ = vvp_vector2_t(vvp_vector2_t::FILL0, mask.size());

      ப்ரதிஜ்ஞா(force_mask_.size() == mask.size());
      க்கு (unsigned idx = 0 ; idx < mask.size() ; idx += 1) {
	    எனில் (mask.value(idx) == 0)
		  தொடர்;

	    force_mask_.set_bit(idx, 1);
	    force_propagate_ = உண்மை;
      }
}

வெறுமை vvp_net_fil_t::release_mask(மாறிலி vvp_vector2_t&mask)
{
      எனில் (force_mask_.size() == 0)
	    திரும்பு;

      ப்ரதிஜ்ஞா(force_mask_.size() == mask.size());
      க்கு (unsigned idx = 0 ; idx < mask.size() ; idx += 1) {
	    எனில் (mask.value(idx))
		  force_mask_.set_bit(idx, 0);
      }

      எனில் (force_mask_.is_zero())
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
வெறுமை vvp_net_fil_t::force_link(vvp_net_t*dst, vvp_net_t*src)
{
      ப்ரதிஜ்ஞா(dst->fil == இது);

      எனில் (force_link_ == 0) {
	    force_link_ = புதிய vvp_net_t;
	      // Use port[3] to hold the force destination.
	    force_link_->port[3] = vvp_net_ptr_t(dst, 0);
	    force_link_->port[2] = vvp_net_ptr_t(0,0);
	    force_link_->fun = புதிய vvp_fun_force;
      }

      force_unlink();
      ப்ரதிஜ்ஞா(force_link_->port[2] == vvp_net_ptr_t(0,0));

	// Use port[2] to hold the force source.
      force_link_->port[2] = vvp_net_ptr_t(src,0);

      vvp_net_ptr_t dst_ptr(force_link_, 0);
      src->link(dst_ptr);
}

வெறுமை vvp_net_fil_t::operator delete(வெறுமை*)
{
      ப்ரதிஜ்ஞா(0);
}

வெறுமை vvp_net_fil_t::force_unlink(வெறுமை)
{
      எனில் (force_link_ == 0) திரும்பு;
      vvp_net_t*src = force_link_->port[2].ptr();
      எனில் (src == 0) திரும்பு;

      src->unlink(vvp_net_ptr_t(force_link_,0));
      force_link_->port[2] = vvp_net_ptr_t(0,0);
}

/* *** BIT operations *** */
vvp_bit4_t add_with_carry(vvp_bit4_t a, vvp_bit4_t b, vvp_bit4_t&c)
{
      எனில் (bit4_is_xz(a) || bit4_is_xz(b) || bit4_is_xz(c)) {
	    c = BIT4_X;
	    திரும்பு BIT4_X;
      }

	// NOTE: This relies on the facts that XZ values have been
	// weeded out, and that BIT4_1 is 1 and BIT4_0 is 0.
      முழு sum = (முழு)a + (முழு)b + (முழு)c;

      தேர்வு (sum) {
	  நிலை 0:
	      // c must already be 0.
	    திரும்பு BIT4_0;
	  நிலை 1:
	    c = BIT4_0;
	    திரும்பு BIT4_1;
	  நிலை 2:
	    c = BIT4_1;
	    திரும்பு BIT4_0;
	  நிலை 3:
	    c = BIT4_1;
	    திரும்பு BIT4_1;
      }
      fprintf(stderr, "Incorrect result %d.\n", sum);
      ப்ரதிஜ்ஞா(0);
      திரும்பு BIT4_X;
}

vvp_bit4_t scalar_to_bit4(PLI_INT32 scalar)
{
      தேர்வு(scalar) {
	  நிலை vpi0:
	    திரும்பு BIT4_0;
	  நிலை vpi1:
	    திரும்பு BIT4_1;
	  நிலை vpiX:
	    திரும்பு BIT4_X;
	  நிலை vpiZ:
	    திரும்பு BIT4_Z;
      }
      fprintf(stderr, "Unsupported scalar value %d.\n", (முழு)scalar);
      ப்ரதிஜ்ஞா(0);
      திரும்பு BIT4_X;
}

vvp_bit4_t operator ^ (vvp_bit4_t a, vvp_bit4_t b)
{
      எனில் (bit4_is_xz(a))
	    திரும்பு BIT4_X;
      எனில் (bit4_is_xz(b))
	    திரும்பு BIT4_X;
      எனில் (a == BIT4_0)
	    திரும்பு b;
      எனில் (b == BIT4_0)
	    திரும்பு a;
      திரும்பு BIT4_0;
}

ostream& operator<<(ostream&out, vvp_bit4_t bit)
{
      தேர்வு (bit) {
	  நிலை BIT4_0:
	    out << "0";
	    நிறுத்து;
	  நிலை BIT4_1:
	    out << "1";
	    நிறுத்து;
	  நிலை BIT4_X:
	    out << "X";
	    நிறுத்து;
	  நிலை BIT4_Z:
	    out << "Z";
	    நிறுத்து;
	  இயல்பு:
	    out << "?";
	    நிறுத்து;
      }
      திரும்பு out;
}

வகைவரை unsigned short edge_t;

உள்வரி edge_t VVP_EDGE(vvp_bit4_t from, vvp_bit4_t to)
{
      திரும்பு 1 << ((from << 2) | to);
}

மாறிலி edge_t vvp_edge_posedge
      = VVP_EDGE(BIT4_0,BIT4_1)
      | VVP_EDGE(BIT4_0,BIT4_X)
      | VVP_EDGE(BIT4_0,BIT4_Z)
      | VVP_EDGE(BIT4_X,BIT4_1)
      | VVP_EDGE(BIT4_Z,BIT4_1)
      ;

மாறிலி edge_t vvp_edge_negedge
      = VVP_EDGE(BIT4_1,BIT4_0)
      | VVP_EDGE(BIT4_1,BIT4_X)
      | VVP_EDGE(BIT4_1,BIT4_Z)
      | VVP_EDGE(BIT4_X,BIT4_0)
      | VVP_EDGE(BIT4_Z,BIT4_0)
      ;

முழு edge(vvp_bit4_t from, vvp_bit4_t to)
{
      edge_t mask = VVP_EDGE(from, to);
      எனில் (mask & vvp_edge_posedge)
	    திரும்பு 1;
      எனில் (mask & vvp_edge_negedge)
	    திரும்பு -1;
      திரும்பு 0;
}

unsigned long multiply_with_carry(unsigned long a, unsigned long b,
				  unsigned long&carry)
{
      மாறிலி unsigned long mask = (1UL << (CPU_WORD_BITS/2)) - 1;
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
      திரும்பு (r1 << (CPU_WORD_BITS/2)) + r00;
}


வெறுமை vvp_send_vec8(vvp_net_ptr_t ptr, மாறிலி vvp_vector8_t&val)
{
      போது (vvp_net_t*cur = ptr.ptr()) {
	    vvp_net_ptr_t next_val = cur->port[ptr.port()];

	    எனில் (cur->fun)
		  cur->fun->recv_vec8(ptr, val);

	    ptr = next_val;
      }
}

வெறுமை vvp_send_real(vvp_net_ptr_t ptr, இரட்டை val, vvp_context_t context)
{
      போது (vvp_net_t*cur = ptr.ptr()) {
	    vvp_net_ptr_t next_val = cur->port[ptr.port()];

	    எனில் (cur->fun)
		  cur->fun->recv_real(ptr, val, context);

	    ptr = next_val;
      }
}

மாறிலி vvp_vector4_t vvp_vector4_t::nil;

வெறுமை vvp_vector4_t::copy_bits(மாறிலி vvp_vector4_t&that)
{

      எனில் (size_ == that.size_) {
	    எனில் (size_ > BITS_PER_WORD) {
		  unsigned சொற்கள் = (size_+BITS_PER_WORD-1) / BITS_PER_WORD;
		  க்கு (unsigned idx = 0 ;  idx < சொற்கள் ;  idx += 1)
			abits_ptr_[idx] = that.abits_ptr_[idx];
		  க்கு (unsigned idx = 0 ;  idx < சொற்கள் ;  idx += 1)
			bbits_ptr_[idx] = that.bbits_ptr_[idx];
	    } இல்லையெனில் {
		  abits_val_ = that.abits_val_;
		  bbits_val_ = that.bbits_val_;
	    }
	    திரும்பு;
      }

	/* Now we know that the sizes of this and that are definitely
	   different. We can use that in code below. In any case, we
	   need to copy only the smaller of the sizes. */

	/* If source and destination are both short, then mask/copy
	   the bit values. */
      எனில் (size_ <= BITS_PER_WORD && that.size_ <= BITS_PER_WORD) {
	    unsigned bits_to_copy = (that.size_ < size_) ? that.size_ : size_;
	    unsigned long mask = ((bits_to_copy < BITS_PER_WORD) ? (1UL << bits_to_copy) :
	                                                           (0UL)) - 1UL;
	    abits_val_ &= ~mask;
	    bbits_val_ &= ~mask;
	    abits_val_ |= that.abits_val_&mask;
	    bbits_val_ |= that.bbits_val_&mask;
	    திரும்பு;
      }

	/* Now we know that either source or destination are long. If
	   the destination is short, then mask/copy from the low word
	   of the long source. */
      எனில் (size_ <= BITS_PER_WORD) {
	    abits_val_ = that.abits_ptr_[0];
	    bbits_val_ = that.bbits_ptr_[0];
	    எனில் (size_ < BITS_PER_WORD) {
		  unsigned long mask = (1UL << size_) - 1UL;
		  abits_val_ &= mask;
		  bbits_val_ &= mask;
	    }
	    திரும்பு;
      }

	/* Now we know that the destination must be long. If the
	   source is short, then mask/copy from its value. */
      எனில் (that.size_ <= BITS_PER_WORD) {
	    unsigned long mask;
	    எனில் (that.size_ < BITS_PER_WORD) {
		  mask = (1UL << that.size_) - 1UL;
		  abits_ptr_[0] &= ~mask;
		  bbits_ptr_[0] &= ~mask;
	    } இல்லையெனில் {
		  mask = -1UL;
	    }
	    abits_ptr_[0] |= that.abits_val_&mask;
	    bbits_ptr_[0] |= that.bbits_val_&mask;
	    திரும்பு;
      }

	/* Finally, we know that source and destination are long. copy
	   words until we get to the last. */
      unsigned bits_to_copy = (that.size_ < size_) ? that.size_ : size_;
      unsigned word = 0;
      போது (bits_to_copy >= BITS_PER_WORD) {
	    abits_ptr_[word] = that.abits_ptr_[word];
	    bbits_ptr_[word] = that.bbits_ptr_[word];
	    bits_to_copy -= BITS_PER_WORD;
	    word += 1;
      }
      எனில் (bits_to_copy > 0) {
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
வெறுமை vvp_vector4_t::copy_from_big_(மாறிலி vvp_vector4_t&that)
{
      unsigned சொற்கள் = (size_+BITS_PER_WORD-1) / BITS_PER_WORD;
      abits_ptr_ = புதிய unsigned long[2*சொற்கள்];
      bbits_ptr_ = abits_ptr_ + சொற்கள்;

      க்கு (unsigned idx = 0 ;  idx < சொற்கள் ;  idx += 1)
	    abits_ptr_[idx] = that.abits_ptr_[idx];
      க்கு (unsigned idx = 0 ;  idx < சொற்கள் ;  idx += 1)
	    bbits_ptr_[idx] = that.bbits_ptr_[idx];
}

/*
 * The copy_inverted_from_ method is just like the copy_from_ method,
 * except that we combine that with an invert. This allows the ~ and
 * the assignment to be blended in many common cases.
 */
வெறுமை vvp_vector4_t::copy_inverted_from_(மாறிலி vvp_vector4_t&that)
{
      size_ = that.size_;
      எனில் (size_ > BITS_PER_WORD) {
	    unsigned சொற்கள் = (size_+BITS_PER_WORD-1) / BITS_PER_WORD;
	    abits_ptr_ = புதிய unsigned long[2*சொற்கள்];
	    bbits_ptr_ = abits_ptr_ + சொற்கள்;

	    unsigned remaining = size_;
	    unsigned idx = 0;
	    போது (remaining >= BITS_PER_WORD) {
		  abits_ptr_[idx] = that.bbits_ptr_[idx] | ~that.abits_ptr_[idx];
		  idx += 1;
		  remaining -= BITS_PER_WORD;
	    }
	    எனில் (remaining > 0) {
		  unsigned long mask = (1UL<<remaining) - 1UL;
		  abits_ptr_[idx] = mask & (that.bbits_ptr_[idx] | ~that.abits_ptr_[idx]);
	    }

	    க்கு (idx = 0 ;  idx < சொற்கள் ;  idx += 1)
		  bbits_ptr_[idx] = that.bbits_ptr_[idx];

      } இல்லையெனில் {
	    unsigned long mask = (size_<BITS_PER_WORD)? (1UL<<size_)-1UL : -1UL;
	    abits_val_ = mask & (that.bbits_val_ | ~that.abits_val_);
	    bbits_val_ = that.bbits_val_;
      }
}

/* Make sure to set size_ before calling this routine. */
வெறுமை vvp_vector4_t::allocate_சொற்கள்(unsigned long inita, unsigned long initb)
{
      எனில் (size_ > BITS_PER_WORD) {
	    unsigned cnt = (size_ + BITS_PER_WORD - 1) / BITS_PER_WORD;
	    abits_ptr_ = புதிய unsigned long[2*cnt];
	    bbits_ptr_ = abits_ptr_ + cnt;
	    க்கு (unsigned idx = 0 ;  idx < cnt ;  idx += 1)
		  abits_ptr_[idx] = inita;
	    க்கு (unsigned idx = 0 ;  idx < cnt ;  idx += 1)
		  bbits_ptr_[idx] = initb;

      } இல்லையெனில் {
	    abits_val_ = inita;
	    bbits_val_ = initb;
      }
}

vvp_vector4_t::vvp_vector4_t(unsigned size__, இரட்டை val)
: size_(size__)
{
      தருக்க is_neg = பொய்;
      இரட்டை fraction;
      முழு exponent;

	/* We return 'bx for a NaN or +/- infinity. */
      எனில் (val != val || (val && (val == 0.5*val)))  {
	    allocate_சொற்கள்(WORD_X_ABITS, WORD_X_BBITS);
	    திரும்பு;
      }

	/* Convert to a positive result. */
      எனில் (val < 0.0) {
	    is_neg = உண்மை;
	    val = -val;
      }
      allocate_சொற்கள்(WORD_0_ABITS, WORD_0_BBITS);

	/* Get the exponent and fractional part of the number. */
      fraction = frexp(val, &exponent);

	/* If the value is small enough just use lround(). */
      எனில் (exponent < BITS_PER_WORD-2) {
	    எனில் (is_neg) இது->invert();  // Invert the bits if negative.
	    long sval = lround(val);
	    எனில் (is_neg) sval = -sval;
	      /* This requires that 0 and 1 have the same bbit value. */
	    எனில் (size_ > BITS_PER_WORD) {
		  abits_ptr_[0] = sval;
	    } இல்லையெனில் {
		  abits_val_ = sval;
	    }
	    திரும்பு;
      }

      unsigned nwords = (exponent-1) / BITS_PER_WORD;
      unsigned my_சொற்கள் = (size_ + BITS_PER_WORD - 1) / BITS_PER_WORD - 1;

      fraction = ldexp(fraction, (exponent-1) % BITS_PER_WORD + 1);

	/* Skip any leading bits. */
      க்கு (முழு idx = (signed) nwords; idx > (signed) my_சொற்கள்; idx -=1) {
	    unsigned long bits = (unsigned long) fraction;
	    fraction = fraction - (இரட்டை) bits;
	    fraction = ldexp(fraction, BITS_PER_WORD);
      }

	/* Convert the remaining bits as appropriate. */
      எனில் (my_சொற்கள் == 0) {
		  unsigned long bits = (unsigned long) fraction;
		  abits_val_ = bits;
		  fraction = fraction - (இரட்டை) bits;
		    /* Round any fractional part up. */
		  எனில் (fraction >= 0.5) *இது += (int64_t) 1;
      } இல்லையெனில் {
	    எனில் (nwords < my_சொற்கள்) my_சொற்கள் = nwords;
	    க்கு (முழு idx = (signed)my_சொற்கள்; idx >= 0; idx -= 1) {
		  unsigned long bits = (unsigned long) fraction;
		  abits_ptr_[idx] = bits;
		  fraction = fraction - (இரட்டை) bits;
		  fraction = ldexp(fraction, BITS_PER_WORD);
	    }
	      /* Round any fractional part up. */
	    எனில் (fraction >= ldexp(0.5, BITS_PER_WORD)) *இது += (int64_t) 1;
       }

	/* Convert to a negative number if needed. */
      எனில் (is_neg) {
	    இது->invert();
	    *இது += (int64_t) 1;
      }
}

vvp_vector4_t::vvp_vector4_t(மாறிலி vvp_vector4_t&that,
			    unsigned adr, unsigned wid)
{
	// Set up and initialize the destination.
      size_ = wid;
      allocate_சொற்கள்(WORD_X_ABITS, WORD_X_BBITS);

	// Special case: selecting from far beyond the source vector,
	// to the result is all X bits. We're done.
      எனில் (adr >= that.size_)
	    திரும்பு;

	// Special case: The source is not quite big enough to supply
	// all bits, so get the bits that we can. The remainder will
	// be left at BIT4_X.
      எனில் ((adr + wid) > that.size_) {
	    unsigned use_wid = that.size_ - adr;
	    க்கு (unsigned idx = 0 ; idx < use_wid ; idx += 1)
		  set_bit(idx, that.value(adr+idx));

	    திரும்பு;
      }

	// At the point, we know that the source part is entirely
	// contained in the source vector.
	// assert((adr + wid) <= that.size_);

      எனில் (wid > BITS_PER_WORD) {
	      /* In this case, the subvector and the source vector are
		 long. Do the transfer reasonably efficiently. */
	    unsigned ptr = adr / BITS_PER_WORD;
	    unsigned long off = adr % BITS_PER_WORD;
	    unsigned long noff = BITS_PER_WORD - off;
	    unsigned long lmask = (1UL << off) - 1UL;
	    unsigned trans = 0;
	    unsigned dst = 0;
	    போது (trans < wid) {
		    // The low bits of the result.
		  abits_ptr_[dst] = (that.abits_ptr_[ptr] & ~lmask) >> off;
		  bbits_ptr_[dst] = (that.bbits_ptr_[ptr] & ~lmask) >> off;
		  trans += noff;

		  எனில் (trans >= wid)
			நிறுத்து;

		  ptr += 1;

		    // The high bits of the result. Skip this if the
		    // source and destination are perfectly aligned.
		  எனில் (noff != BITS_PER_WORD) {
			abits_ptr_[dst] |= (that.abits_ptr_[ptr]&lmask) << noff;
			bbits_ptr_[dst] |= (that.bbits_ptr_[ptr]&lmask) << noff;
			trans += off;
		  }

		  dst += 1;
	    }

      } இல்லையெனில் எனில் (that.size_ > BITS_PER_WORD) {
	      /* In this case, the subvector fits in a single word,
		 but the source is large. */
	    unsigned ptr = adr / BITS_PER_WORD;
	    unsigned long off = adr % BITS_PER_WORD;
	    unsigned trans = BITS_PER_WORD - off;
	    எனில் (trans > wid)
		  trans = wid;

	    எனில் (trans == BITS_PER_WORD) {
		    // Very special case: Copy exactly 1 perfectly
		    // aligned word.
		  abits_val_ = that.abits_ptr_[ptr];
		  bbits_val_ = that.bbits_ptr_[ptr];

	    } இல்லையெனில் {
		    // lmask is the low bits of the destination,
		    // masked into the source.
		  unsigned long lmask = (1UL<<trans) - 1UL;
		  lmask <<= off;

		    // The low bits of the result.
		  abits_val_ = (that.abits_ptr_[ptr] & lmask) >> off;
		  bbits_val_ = (that.bbits_ptr_[ptr] & lmask) >> off;

		  எனில் (trans < wid) {
			  // If there are more bits, then get them
			  // from the bottom of the next word of the
			  // source.
			unsigned long hmask = (1UL << (wid-trans)) - 1UL;

			  // The high bits of the result.
			abits_val_ |= (that.abits_ptr_[ptr+1]&hmask) << trans;
			bbits_val_ |= (that.bbits_ptr_[ptr+1]&hmask) << trans;
		  }
	    }

      } இல்லையெனில் எனில் (size_ == BITS_PER_WORD) {
	      /* We know that source and destination are short. If the
		 destination is a full word, then we know the copy is
		 aligned and complete. */
	    abits_val_ = that.abits_val_;
	    bbits_val_ = that.bbits_val_;

      } இல்லையெனில் {
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
வெறுமை vvp_vector4_t::resize(unsigned newsize, vvp_bit4_t pad_bit)
{
      எனில் (size_ == newsize)
	    திரும்பு;

      unsigned cnt = (size_ + BITS_PER_WORD - 1) / BITS_PER_WORD;
      unsigned long word_pad_abits = 0;
      unsigned long word_pad_bbits = 0;
      தேர்வு (pad_bit) {
	  நிலை BIT4_0:
	    word_pad_abits = WORD_0_ABITS;
	    word_pad_bbits = WORD_0_BBITS;
	    நிறுத்து;
	  நிலை BIT4_1:
	    word_pad_abits = WORD_1_ABITS;
	    word_pad_bbits = WORD_1_BBITS;
	    நிறுத்து;
	  நிலை BIT4_X:
	    word_pad_abits = WORD_X_ABITS;
	    word_pad_bbits = WORD_X_BBITS;
	    நிறுத்து;
	  நிலை BIT4_Z:
	    word_pad_abits = WORD_Z_ABITS;
	    word_pad_bbits = WORD_Z_BBITS;
	    நிறுத்து;
      }

      எனில் (newsize > BITS_PER_WORD) {
	    unsigned newcnt = (newsize + BITS_PER_WORD - 1) / BITS_PER_WORD;
	    எனில் (newcnt == cnt) {
		    // If the word count doesn't change, then there is
		    // no need for re-allocation so we are done now.
		  எனில் (newsize > size_) {
			எனில் (unsigned fill = size_ % BITS_PER_WORD) {
			      abits_ptr_[cnt-1] &= ~((-1UL) << fill);
			      bbits_ptr_[cnt-1] &= ~((-1UL) << fill);
			      abits_ptr_[cnt-1] |= word_pad_abits << fill;
			      bbits_ptr_[cnt-1] |= word_pad_bbits << fill;
			}
		  }
		  size_ = newsize;
		  திரும்பு;
	    }

	    unsigned long*newbits = புதிய unsigned long[2*newcnt];

	    எனில் (cnt > 1) {
		  unsigned trans = cnt;
		  எனில் (trans > newcnt)
			trans = newcnt;

		  க்கு (unsigned idx = 0 ;  idx < trans ;  idx += 1)
			newbits[idx] = abits_ptr_[idx];
		  க்கு (unsigned idx = 0 ;  idx < trans ;  idx += 1)
			newbits[newcnt+idx] = bbits_ptr_[idx];

		  delete[]abits_ptr_;

	    } இல்லையெனில் {
		  newbits[0] = abits_val_;
		  newbits[newcnt] = bbits_val_;
	    }

	    எனில் (newsize > size_) {
		  எனில் (unsigned fill = size_ % BITS_PER_WORD) {
			newbits[cnt-1] &= ~((-1UL) << fill);
			newbits[cnt-1] |= word_pad_abits << fill;
			newbits[newcnt+cnt-1] &= ~((-1UL) << fill);
			newbits[newcnt+cnt-1] |= word_pad_bbits << fill;
		  }
		  க்கு (unsigned idx = cnt ;  idx < newcnt ;  idx += 1)
			newbits[idx] = word_pad_abits;
		  க்கு (unsigned idx = cnt ;  idx < newcnt ;  idx += 1)
			newbits[newcnt+idx] = word_pad_bbits;
	    }

	    size_ = newsize;
	    abits_ptr_ = newbits;
	    bbits_ptr_ = newbits + newcnt;

      } இல்லையெனில் {
	    எனில் (cnt > 1) {
		  unsigned long newvala = abits_ptr_[0];
		  unsigned long newvalb = bbits_ptr_[0];
		  delete[]abits_ptr_;
		  abits_val_ = newvala;
		  bbits_val_ = newvalb;
	    }

	    எனில் (newsize > size_) {
		  abits_val_ &= ~((-1UL) << size_);
		  bbits_val_ &= ~((-1UL) << size_);
		  abits_val_ |= word_pad_abits << size_;
		  bbits_val_ |= word_pad_bbits << size_;
	    }

	    size_ = newsize;
      }
}


unsigned long* vvp_vector4_t::subarray(unsigned adr, unsigned wid, தருக்க xz_to_0) மாறிலி
{
      மாறிலி unsigned BIT2_PER_WORD = 8*அளவு(unsigned long);
      unsigned awid = (wid + BIT2_PER_WORD - 1) / (BIT2_PER_WORD);
      unsigned long*val = புதிய unsigned long[awid];

      க்கு (unsigned idx = 0 ;  idx < awid ;  idx += 1)
	    val[idx] = 0;

      எனில் (size_ <= BITS_PER_WORD) {
	      /* Handle the special case that the array is small. The
		 entire value of the vector4 is within the xbits_val_
		 so we know that the result is a single word, the
		 source is a single word, and we just have to loop
		 through that word. */
	    unsigned long atmp = abits_val_ >> adr;
	    unsigned long btmp = bbits_val_ >> adr;
	    எனில் (wid < BIT2_PER_WORD) {
		  atmp &= (1UL << wid) - 1;
		  btmp &= (1UL << wid) - 1;
	    }
	    எனில் (btmp) {
		  எனில் (xz_to_0)
			atmp &= ~btmp;
		  இல்லையெனில்
			செல் x_out;
	    }

	    val[0] = atmp;

      } இல்லையெனில் {

	    unsigned val_ptr = 0;
	    unsigned val_off = 0;

	      /* Get the first word we are scanning. We may in fact be
		 somewhere in the middle of that word. */
	    போது (wid > 0) {
		  unsigned long atmp = abits_ptr_[adr/BITS_PER_WORD];
		  unsigned long btmp = bbits_ptr_[adr/BITS_PER_WORD];
		  unsigned long off = adr%BITS_PER_WORD;
		  atmp >>= off;
		  btmp >>= off;

		  unsigned long trans = BITS_PER_WORD - off;
		  எனில் (trans > (BIT2_PER_WORD - val_off))
			trans = BIT2_PER_WORD - val_off;
		  எனில் (wid < trans)
			trans = wid;
		  எனில் (trans < BIT2_PER_WORD) {
			atmp &= (1UL << trans) - 1;
			btmp &= (1UL << trans) - 1;
		  }
		  எனில் (btmp) {
			எனில் (xz_to_0)
			      atmp &= ~btmp;
			இல்லையெனில்
			      செல் x_out;
		  }

		  val[val_ptr] |= atmp << val_off;
		  adr += trans;
		  wid -= trans;
		  val_off += trans;
		  எனில் (val_off == BIT2_PER_WORD) {
			val_ptr += 1;
			val_off = 0;
		  }
	    }
      }

      திரும்பு val;

 x_out:
      delete[]val;
      திரும்பு 0;
}

வெறுமை vvp_vector4_t::setarray(unsigned adr, unsigned wid, மாறிலி unsigned long*val)
{
      ப்ரதிஜ்ஞா(adr+wid <= size_);

      மாறிலி unsigned BIT2_PER_WORD = 8*அளவு(unsigned long);

      எனில் (size_ <= BITS_PER_WORD) {
	      // We know here that both the source and the target are
	      // within a single word. Write the bits into the
	      // abits_val_ directly.

	    ப்ரதிஜ்ஞா(BIT2_PER_WORD <= BITS_PER_WORD);
	    unsigned long lmask = (1UL << adr) - 1UL;
	    unsigned long hmask = ((adr+wid) < BITS_PER_WORD)
		  ? -1UL << (adr+wid)
		  : 0;
	    unsigned long mask = ~(hmask | lmask);

	    abits_val_ &= ~mask;
	    bbits_val_ &= ~mask;

	    abits_val_ |= mask & (val[0] << adr);

      } இல்லையெனில் {
	      // The general case, there are multiple words of
	      // destination, and possibly multiple words of source
	      // data. Shift and mask as we go.
	    unsigned off = adr % BITS_PER_WORD;
	    unsigned ptr = adr / BITS_PER_WORD;
	    unsigned val_off = 0;
	    unsigned val_ptr = 0;
	    போது (wid > 0) {
		  unsigned trans = wid;
		  எனில் (trans > (BIT2_PER_WORD-val_off))
			trans = BIT2_PER_WORD-val_off;
		  எனில் (trans > (BITS_PER_WORD-off))
			trans = BITS_PER_WORD-off;

		  unsigned long lmask = (1UL << off) - 1UL;
		  unsigned long hmask = ((off+trans) < BITS_PER_WORD)
			? -1UL << (off+trans)
			: 0;
		  unsigned long mask = ~(hmask | lmask);

		  abits_ptr_[ptr] &= ~mask;
		  bbits_ptr_[ptr] &= ~mask;
		  எனில் (val_off >= off)
			abits_ptr_[ptr] |= mask & (val[val_ptr] >> (val_off-off));
		  இல்லையெனில்
			abits_ptr_[ptr] |= mask & (val[val_ptr] << (off-val_off));

		  wid -= trans;
		  val_off += trans;
		  எனில் (val_off == BIT2_PER_WORD) {
			val_off = 0;
			val_ptr += 1;
		  }
		  off += trans;
		  எனில் (off == BITS_PER_WORD) {
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
தருக்க vvp_vector4_t::set_vec(unsigned adr, மாறிலி vvp_vector4_t&that)
{
      ப்ரதிஜ்ஞா(adr+that.size_  <= size_);
      தருக்க diff_flag = பொய்;

      எனில் (size_ <= BITS_PER_WORD) {

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
	    எனில் (hshift >= BITS_PER_WORD)
		  hmask = -1UL;
	    இல்லையெனில்
		  hmask = (1UL << (adr+that.size_)) - 1;
	    unsigned long mask = hmask & ~lmask;

	    unsigned long tmp = (that.abits_val_<<adr)&mask;
	    எனில் ((abits_val_&mask) != tmp) {
		  diff_flag = உண்மை;
		  abits_val_ = (abits_val_ & ~mask) | tmp;
	    }
	    tmp = (that.bbits_val_<<adr) & mask;
	    எனில் ((bbits_val_&mask) != tmp) {
		  diff_flag = உண்மை;
		  bbits_val_ = (bbits_val_ & ~mask) | tmp;
	    }

      } இல்லையெனில் எனில் (that.size_ <= BITS_PER_WORD) {

	      /* This vector is more than a word, but that vector is
		 still small. Write into the destination, possibly
		 spanning two destination words, depending on whether
		 the source vector spans a word transition. */
	    unsigned long dptr = adr / BITS_PER_WORD;
	    unsigned long doff = adr % BITS_PER_WORD;

	    unsigned long lmask = (1UL << doff) - 1;
	    unsigned long hshift = doff+that.size_;
	    unsigned long hmask;
	    எனில் (hshift >= BITS_PER_WORD)
		  hmask = -1UL;
	    இல்லையெனில்
		  hmask = (1UL << hshift) - 1UL;

	    unsigned long mask = hmask & ~lmask;
	    unsigned long tmp;

	    tmp = (that.abits_val_ << doff) & mask;
	    எனில் ((abits_ptr_[dptr] & mask) != tmp) {
		  diff_flag = உண்மை;
		  abits_ptr_[dptr] = (abits_ptr_[dptr] & ~mask) | tmp;
	    }
	    tmp = (that.bbits_val_ << doff) & mask;
	    எனில் ((bbits_ptr_[dptr] & mask) != tmp) {
		  diff_flag = உண்மை;
		  bbits_ptr_[dptr] = (bbits_ptr_[dptr] & ~mask) | tmp;
	    }

	    எனில் ((doff + that.size_) > BITS_PER_WORD) {
		  unsigned tail = doff + that.size_ - BITS_PER_WORD;
		  mask = (1UL << tail) - 1;

		  dptr += 1;
		  tmp = (that.abits_val_ >> (that.size_-tail)) & mask;
		  எனில் ((abits_ptr_[dptr] & mask) != tmp) {
			diff_flag = உண்மை;
			abits_ptr_[dptr] = (abits_ptr_[dptr] & ~mask) | tmp;
		  }
		  tmp = (that.bbits_val_ >> (that.size_-tail)) & mask;
		  எனில் ((bbits_ptr_[dptr] & mask) != tmp) {
			diff_flag = உண்மை;
			bbits_ptr_[dptr] = (bbits_ptr_[dptr] & ~mask) | tmp;
		  }
	    }

      } இல்லையெனில் எனில் (adr%BITS_PER_WORD == 0) {

	      /* In this case, both vectors are long, but the
		 destination is neatly aligned. That means all but the
		 last word can be simply copied with no masking. */

	    unsigned remain = that.size_;
	    unsigned sptr = 0;
	    unsigned dptr = adr / BITS_PER_WORD;
	    போது (remain >= BITS_PER_WORD) {
		  எனில் (abits_ptr_[dptr] != that.abits_ptr_[sptr]) {
			diff_flag = உண்மை;
			abits_ptr_[dptr] = that.abits_ptr_[sptr];
		  }
		  எனில் (bbits_ptr_[dptr] != that.bbits_ptr_[sptr]) {
			diff_flag = உண்மை;
			bbits_ptr_[dptr] = that.bbits_ptr_[sptr];
		  }
		  dptr += 1;
		  sptr += 1;
		  remain -= BITS_PER_WORD;
	    }

	    எனில் (remain > 0) {
		  unsigned long mask = (1UL << remain) - 1;
		  unsigned long tmp;

		  tmp = that.abits_ptr_[sptr] & mask;
		  எனில் ((abits_ptr_[dptr] & mask) != tmp) {
			diff_flag = உண்மை;
			abits_ptr_[dptr] = (abits_ptr_[dptr] & ~mask) | tmp;
		  }
		  tmp = that.bbits_ptr_[sptr] & mask;
		  எனில் ((bbits_ptr_[dptr] & mask) != tmp) {
			diff_flag = உண்மை;
			bbits_ptr_[dptr] = (bbits_ptr_[dptr] & ~mask) | tmp;
		  }
	    }

      } இல்லையெனில் {

	      /* We know that there are two long vectors, and we know
		 that the destination is definitely NOT aligned. */

	    unsigned remain = that.size_;
	    unsigned sptr = 0;
	    unsigned dptr = adr / BITS_PER_WORD;
	    unsigned doff = adr % BITS_PER_WORD;
	    unsigned long lmask = (1UL << doff) - 1;
	    unsigned ndoff = BITS_PER_WORD - doff;
	    போது (remain >= BITS_PER_WORD) {
		  unsigned long tmp;

		  tmp = (that.abits_ptr_[sptr] << doff) & ~lmask;
		  எனில் ((abits_ptr_[dptr] & ~lmask) != tmp) {
			diff_flag = உண்மை;
			abits_ptr_[dptr] = (abits_ptr_[dptr] & lmask) | tmp;
		  }
		  tmp = (that.bbits_ptr_[sptr] << doff) & ~lmask;
		  எனில் ((bbits_ptr_[dptr] & ~lmask) != tmp) {
			diff_flag = உண்மை;
			bbits_ptr_[dptr] = (bbits_ptr_[dptr] & lmask) | tmp;
		  }
		  dptr += 1;

		  tmp = (that.abits_ptr_[sptr] >> ndoff) & lmask;
		  எனில் ((abits_ptr_[dptr] & lmask) != tmp) {
			diff_flag = உண்மை;
			abits_ptr_[dptr] = (abits_ptr_[dptr] & ~lmask) | tmp;
		  }
		  tmp = (that.bbits_ptr_[sptr] >> ndoff) & lmask;
		  எனில் ((bbits_ptr_[dptr] & lmask) != tmp) {
			diff_flag = உண்மை;
			bbits_ptr_[dptr] = (bbits_ptr_[dptr] & ~lmask) | tmp;
		  }

		  remain -= BITS_PER_WORD;
		  sptr += 1;
	    }

	    எனில் (remain > 0) {
		  unsigned long hshift = doff+remain;
		  unsigned long hmask;
		  எனில் (hshift >= BITS_PER_WORD)
			hmask = -1UL;
		  இல்லையெனில்
			hmask = (1UL << (doff+remain)) - 1;

		  unsigned long mask = hmask & ~lmask;
		  unsigned long tmp;

		  tmp = (that.abits_ptr_[sptr] << doff) & mask;
		  எனில் ((abits_ptr_[dptr] & mask) != tmp) {
			diff_flag = உண்மை;
			abits_ptr_[dptr] = (abits_ptr_[dptr] & ~mask) | tmp;
		  }
		  tmp = (that.bbits_ptr_[sptr] << doff) & mask;
		  எனில் ((bbits_ptr_[dptr] & mask) != tmp) {
			diff_flag = உண்மை;
			bbits_ptr_[dptr] = (bbits_ptr_[dptr] & ~mask) | tmp;
		  }

		  எனில் ((doff + remain) > BITS_PER_WORD) {
			unsigned tail = doff + remain - BITS_PER_WORD;
			எனில் (tail >= BITS_PER_WORD)
			      mask = -1UL;
			இல்லையெனில்
			      mask = (1UL << tail) - 1;

			dptr += 1;

			tmp = (that.abits_ptr_[sptr] >> (remain-tail))&mask;
			எனில் ((abits_ptr_[dptr] & mask) != tmp) {
			      diff_flag = உண்மை;
			      abits_ptr_[dptr] = (abits_ptr_[dptr] & ~mask) | tmp;
			}
			tmp = (that.bbits_ptr_[sptr] >> (remain-tail))&mask;
			எனில் ((bbits_ptr_[dptr] & mask) != tmp) {
			      diff_flag = உண்மை;
			      bbits_ptr_[dptr] = (bbits_ptr_[dptr] & ~mask) | tmp;
			}
		  }
	    }
      }

      திரும்பு diff_flag;
}

/*
 * Add that vector to this vector. Do it in the Verilog way, which
 * means if we detect any X or Z bits, change the entire results to
 * all X.
 *
 * Assume both vectors are the same size.
 */
வெறுமை vvp_vector4_t::add(மாறிலி vvp_vector4_t&that)
{
      ப்ரதிஜ்ஞா(size_ == that.size_);

      எனில் (size_ < BITS_PER_WORD) {
	    unsigned long mask = ~(-1UL << size_);
	    எனில் ((bbits_val_|that.bbits_val_) & mask) {
		  abits_val_ |= mask;
		  bbits_val_ |= mask;
		  திரும்பு;
	    }

	    abits_val_ += that.abits_val_;
	    abits_val_ &= mask;
	    திரும்பு;
      }

      எனில் (size_ == BITS_PER_WORD) {
	    எனில் (bbits_val_ | that.bbits_val_) {
		  abits_val_ = WORD_X_ABITS;
		  bbits_val_ = WORD_X_BBITS;
	    } இல்லையெனில் {
		  abits_val_ += that.abits_val_;
	    }
	    திரும்பு;
      }

      முழு cnt = size_ / BITS_PER_WORD;
      unsigned long carry = 0;
      க்கு (முழு idx = 0 ; idx < cnt ; idx += 1) {
	    எனில் (bbits_ptr_[idx] | that.bbits_ptr_[idx])
		  செல் x_out;

	    abits_ptr_[idx] = add_with_carry(abits_ptr_[idx], that.abits_ptr_[idx], carry);
      }

      எனில் (unsigned tail = size_ % BITS_PER_WORD) {
	    unsigned long mask = ~( -1UL << tail );
	    எனில் ((bbits_ptr_[cnt] | that.bbits_ptr_[cnt])&mask)
		  செல் x_out;

	    abits_ptr_[cnt] = add_with_carry(abits_ptr_[cnt], that.abits_ptr_[cnt], carry);
	    abits_ptr_[cnt] &= mask;
      }

      திரும்பு;

 x_out:
      க்கு (முழு idx = 0 ; idx < cnt ; idx += 1) {
	    abits_ptr_[idx] = WORD_X_ABITS;
	    bbits_ptr_[idx] = WORD_X_BBITS;
      }
      எனில் (unsigned tail = size_%BITS_PER_WORD) {
	    unsigned long mask = ~( -1UL << tail );
	    abits_ptr_[cnt] = WORD_X_ABITS&mask;
	    bbits_ptr_[cnt] = WORD_X_BBITS&mask;
      }
}

வெறுமை vvp_vector4_t::sub(மாறிலி vvp_vector4_t&that)
{
      ப்ரதிஜ்ஞா(size_ == that.size_);

      எனில் (size_ < BITS_PER_WORD) {
	    unsigned long mask = ~(-1UL << size_);
	    எனில் ((bbits_val_|that.bbits_val_) & mask) {
		  abits_val_ |= mask;
		  bbits_val_ |= mask;
		  திரும்பு;
	    }

	    abits_val_ -= that.abits_val_;
	    abits_val_ &= mask;
	    திரும்பு;
      }

      எனில் (size_ == BITS_PER_WORD) {
	    எனில் (bbits_val_ | that.bbits_val_) {
		  abits_val_ = WORD_X_ABITS;
		  bbits_val_ = WORD_X_BBITS;
	    } இல்லையெனில் {
		  abits_val_ -= that.abits_val_;
	    }
	    திரும்பு;
      }

      முழு cnt = size_ / BITS_PER_WORD;
      unsigned long carry = 1;
      க்கு (முழு idx = 0 ; idx < cnt ; idx += 1) {
	    எனில் (bbits_ptr_[idx] | that.bbits_ptr_[idx])
		  செல் x_out;

	    abits_ptr_[idx] = add_with_carry(abits_ptr_[idx], ~that.abits_ptr_[idx], carry);
      }

      எனில் (unsigned tail = size_ % BITS_PER_WORD) {
	    unsigned long mask = ~( -1UL << tail );
	    எனில் ((bbits_ptr_[cnt] | that.bbits_ptr_[cnt])&mask)
		  செல் x_out;

	    abits_ptr_[cnt] = add_with_carry(abits_ptr_[cnt], ~that.abits_ptr_[cnt], carry);
	    abits_ptr_[cnt] &= mask;
      }

      திரும்பு;

 x_out:
      க்கு (முழு idx = 0 ; idx < cnt ; idx += 1) {
	    abits_ptr_[idx] = WORD_X_ABITS;
	    bbits_ptr_[idx] = WORD_X_BBITS;
      }
      எனில் (unsigned tail = size_%BITS_PER_WORD) {
	    unsigned long mask = ~( -1UL << tail );
	    abits_ptr_[cnt] = WORD_X_ABITS&mask;
	    bbits_ptr_[cnt] = WORD_X_BBITS&mask;
      }

}

வெறுமை vvp_vector4_t::mov(unsigned dst, unsigned src, unsigned cnt)
{
      ப்ரதிஜ்ஞா(dst+cnt <= size_);
      ப்ரதிஜ்ஞா(src+cnt <= size_);

      எனில் (size_ <= BITS_PER_WORD) {
	    unsigned long vmask = (1UL << cnt) - 1;
	    unsigned long tmp;

	    tmp = (abits_val_ >> src) & vmask;
	    abits_val_ &= ~ (vmask << dst);
	    abits_val_ |= tmp << dst;

	    tmp = (bbits_val_ >> src) & vmask;
	    bbits_val_ &= ~ (vmask << dst);
	    bbits_val_ |= tmp << dst;

      } இல்லையெனில் {
	    unsigned sptr = src / BITS_PER_WORD;
	    unsigned dptr = dst / BITS_PER_WORD;
	    unsigned soff = src % BITS_PER_WORD;
	    unsigned doff = dst % BITS_PER_WORD;

	    போது (cnt > 0) {
		  unsigned trans = cnt;
		  எனில் ((soff+trans) > BITS_PER_WORD)
			trans = BITS_PER_WORD - soff;

		  எனில் ((doff+trans) > BITS_PER_WORD)
			trans = BITS_PER_WORD - doff;

		  எனில் (trans == BITS_PER_WORD) {
			  // Special case: the transfer count is
			  // exactly an entire word. For this to be
			  // true, it must also be true that the
			  // pointers are aligned. The work is easy,
			abits_ptr_[dptr] = abits_ptr_[sptr];
			bbits_ptr_[dptr] = bbits_ptr_[sptr];
			dptr += 1;
			sptr += 1;
			cnt -= BITS_PER_WORD;
			தொடர்;
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
		  எனில் (soff >= BITS_PER_WORD) {
			soff = 0;
			sptr += 1;
		  }
		  doff += trans;
		  எனில் (doff >= BITS_PER_WORD) {
			doff = 0;
			dptr += 1;
		  }
	    }
      }
}

வெறுமை vvp_vector4_t::mul(மாறிலி vvp_vector4_t&that)
{
      ப்ரதிஜ்ஞா(size_ == that.size_);

      எனில் (size_ < BITS_PER_WORD) {
	    unsigned long mask = ~(-1UL << size_);
	    எனில் ((bbits_val_|that.bbits_val_) & mask) {
		  abits_val_ |= mask;
		  bbits_val_ |= mask;
		  திரும்பு;
	    }

	    abits_val_ *= that.abits_val_;
	    abits_val_ &= mask;
	    திரும்பு;
      }

      எனில் (size_ == BITS_PER_WORD) {
	    எனில் (bbits_val_ || that.bbits_val_) {
		  abits_val_ = WORD_X_ABITS;
		  bbits_val_ = WORD_X_BBITS;
	    } இல்லையெனில் {
		  abits_val_ *= that.abits_val_;
	    }
	    திரும்பு;
      }

      மாறிலி முழு cnt = (size_+BITS_PER_WORD-1) / BITS_PER_WORD;

      unsigned long mask;
      எனில் (unsigned tail = size_%BITS_PER_WORD) {
	    mask = ~( -1UL << tail );
      } இல்லையெனில் {
	    mask = ~0UL;
      }

	// Check for any XZ values ahead of time in a first pass. If
	// we find any, then force the entire result to be X and be
	// done.
      க்கு (முழு idx = 0 ; idx < cnt ; idx += 1) {
	    unsigned long lval = bbits_ptr_[idx];
	    unsigned long rval = that.bbits_ptr_[idx];
	    எனில் (idx == (cnt-1)) {
		  lval &= mask;
		  rval &= mask;
	    }
	    எனில் (lval || rval) {
		  க்கு (முழு xdx = 0 ; xdx < cnt-1 ; xdx += 1) {
			abits_ptr_[xdx] = WORD_X_ABITS;
			bbits_ptr_[xdx] = WORD_X_BBITS;
		  }
		  abits_ptr_[cnt-1] = WORD_X_ABITS & mask;
		  bbits_ptr_[cnt-1] = WORD_X_BBITS & mask;
		  திரும்பு;
	    }
      }

	// Calculate the result into a res array. We need to keep is
	// separate from the "this" array because we are making
	// multiple passes.
      unsigned long*res = புதிய unsigned long[cnt];
      க்கு (முழு idx = 0 ; idx < cnt ; idx += 1)
	    res[idx] = 0;

      க்கு (முழு mul_a = 0 ; mul_a < cnt ; mul_a += 1) {
	    unsigned long lval = abits_ptr_[mul_a];
	    எனில் (mul_a == (cnt-1))
		  lval &= mask;

	    க்கு (முழு mul_b = 0 ; mul_b < (cnt-mul_a) ; mul_b += 1) {
		  unsigned long rval = that.abits_ptr_[mul_b];
		  எனில் (mul_b == (cnt-1))
			rval &= mask;

		  unsigned long sum;
		  unsigned long tmp = multiply_with_carry(lval, rval, sum);
		  முழு base = mul_a + mul_b;
		  unsigned long carry = 0;
		  res[base] = add_with_carry(res[base], tmp, carry);
		  க்கு (முழு add_idx = base+1 ; add_idx < cnt ; add_idx += 1) {
			res[add_idx] = add_with_carry(res[add_idx], sum, carry);
			sum = 0;
		  }
	    }
      }

	// Replace the "this" value with the calculated result. We
	// know a-priori that the bbits are zero and unchanged.
      res[cnt-1] &= mask;
      க்கு (முழு idx = 0 ; idx < cnt ; idx += 1)
	    abits_ptr_[idx] = res[idx];

      delete[]res;
      திரும்பு;


}

தருக்க vvp_vector4_t::eeq(மாறிலி vvp_vector4_t&that) மாறிலி
{
      எனில் (size_ != that.size_)
	    திரும்பு பொய்;

      எனில் (size_ < BITS_PER_WORD) {
	    unsigned long mask = (1UL << size_) - 1;
	    திரும்பு (abits_val_&mask) == (that.abits_val_&mask)
		  && (bbits_val_&mask) == (that.bbits_val_&mask);
      }

      எனில் (size_ == BITS_PER_WORD) {
	    திரும்பு (abits_val_ == that.abits_val_)
		  && (bbits_val_ == that.bbits_val_);
      }

      unsigned சொற்கள் = size_ / BITS_PER_WORD;
      க்கு (unsigned idx = 0 ;  idx < சொற்கள் ;  idx += 1) {
	    எனில் (abits_ptr_[idx] != that.abits_ptr_[idx])
		  திரும்பு பொய்;
	    எனில் (bbits_ptr_[idx] != that.bbits_ptr_[idx])
		  திரும்பு பொய்;
      }

      unsigned long mask = size_%BITS_PER_WORD;
      எனில் (mask > 0) {
	    mask = (1UL << mask) - 1;
	    திரும்பு (abits_ptr_[சொற்கள்]&mask) == (that.abits_ptr_[சொற்கள்]&mask)
		  && (bbits_ptr_[சொற்கள்]&mask) == (that.bbits_ptr_[சொற்கள்]&mask);
      }

      திரும்பு உண்மை;
}

தருக்க vvp_vector4_t::eq_xz(மாறிலி vvp_vector4_t&that) மாறிலி
{
      எனில் (size_ != that.size_)
	    திரும்பு பொய்;

      எனில் (size_ < BITS_PER_WORD) {
	    unsigned long mask = (1UL << size_) - 1;
	    திரும்பு ((abits_val_|bbits_val_)&mask) == ((that.abits_val_|that.bbits_val_)&mask)
		  && (bbits_val_&mask) == (that.bbits_val_&mask);
      }

      எனில் (size_ == BITS_PER_WORD) {
	    திரும்பு ((abits_val_|bbits_val_) == (that.abits_val_|that.bbits_val_))
		  && (bbits_val_ == that.bbits_val_);
      }

      unsigned சொற்கள் = size_ / BITS_PER_WORD;
      க்கு (unsigned idx = 0 ;  idx < சொற்கள் ;  idx += 1) {
	    எனில் ((abits_ptr_[idx]|bbits_ptr_[idx]) != (that.abits_ptr_[idx]|that.bbits_ptr_[idx]))
		  திரும்பு பொய்;
	    எனில் (bbits_ptr_[idx] != that.bbits_ptr_[idx])
		  திரும்பு பொய்;
      }

      unsigned long mask = size_%BITS_PER_WORD;
      எனில் (mask > 0) {
	    mask = (1UL << mask) - 1;
	    திரும்பு ((abits_ptr_[சொற்கள்]|bbits_ptr_[சொற்கள்])&mask) == ((that.abits_ptr_[சொற்கள்]|that.bbits_ptr_[சொற்கள்])&mask)
		  && (bbits_ptr_[சொற்கள்]&mask) == (that.bbits_ptr_[சொற்கள்]&mask);
      }

      திரும்பு உண்மை;
}

தருக்க vvp_vector4_t::has_xz() மாறிலி
{
      எனில் (size_ < BITS_PER_WORD) {
	    unsigned long mask = -1UL >> (BITS_PER_WORD - size_);
	    திரும்பு bbits_val_&mask;
      }

      எனில் (size_ == BITS_PER_WORD) {
	    திரும்பு bbits_val_;
      }

      unsigned சொற்கள் = size_ / BITS_PER_WORD;
      க்கு (unsigned idx = 0 ; idx < சொற்கள் ; idx += 1) {
	    எனில் (bbits_ptr_[idx])
		  திரும்பு உண்மை;
      }

      unsigned long mask = size_%BITS_PER_WORD;
      எனில் (mask > 0) {
	    mask = -1UL >> (BITS_PER_WORD - mask);
	    திரும்பு bbits_ptr_[சொற்கள்]&mask;
      }

      திரும்பு பொய்;
}

வெறுமை vvp_vector4_t::change_z2x()
{
	// This method relies on the fact that both BIT4_X and BIT4_Z
	// have the bbit set in the vector4 encoding, and also that
	// the BIT4_X has abit set in the vector4 encoding. By simply
	// or-ing the bbit into the abit, BIT4_X and BIT4_Z both
	// become BIT4_X.

      எனில் (size_ <= BITS_PER_WORD) {
	    abits_val_ |= bbits_val_;
      } இல்லையெனில் {
	    unsigned சொற்கள் = (size_+BITS_PER_WORD-1) / BITS_PER_WORD;
	    க்கு (unsigned idx = 0 ;  idx < சொற்கள் ;  idx += 1)
		  abits_ptr_[idx] |= bbits_ptr_[idx];
      }
}

வெறுமை vvp_vector4_t::fill_bits(vvp_bit4_t bit)
{
	/* note: this relies on the bit encoding for the vvp_bit4_t. */
      நிலையான மாறிலி unsigned long init_atable[4] = {
	    WORD_0_ABITS,
	    WORD_1_ABITS,
	    WORD_Z_ABITS,
	    WORD_X_ABITS };
      நிலையான மாறிலி unsigned long init_btable[4] = {
	    WORD_0_BBITS,
	    WORD_1_BBITS,
	    WORD_Z_BBITS,
	    WORD_X_BBITS };

      எனில் (size_ <= BITS_PER_WORD) {
	    abits_val_ = init_atable[bit];
	    bbits_val_ = init_btable[bit];
      } இல்லையெனில் {
	    unsigned சொற்கள் = (size_ + BITS_PER_WORD - 1) / BITS_PER_WORD;
	    க்கு (unsigned idx = 0; idx < சொற்கள்; idx += 1) {
		  abits_ptr_[idx] = init_atable[bit];
		  bbits_ptr_[idx] = init_btable[bit];
	    }
      }
}

எழுத்து* vvp_vector4_t::as_string(எழுத்து*buf, size_t buf_len) மாறிலி
{
      எழுத்து*res = buf;
      *buf++ = 'C';
      *buf++ = '4';
      *buf++ = '<';
      buf_len -= 3;

      க்கு (unsigned idx = 0 ;  idx < size() && buf_len >= 2 ;  idx += 1) {
	    தேர்வு (value(size()-idx-1)) {
		நிலை BIT4_0:
		  *buf++ = '0';
		  நிறுத்து;
		நிலை BIT4_1:
		  *buf++ = '1';
		  நிறுத்து;
		நிலை BIT4_X:
		  *buf++ = 'x';
		  நிறுத்து;
		நிலை BIT4_Z:
		  *buf++ = 'z';
	    }
	    buf_len -= 1;
      }

      *buf++ = '>';
      *buf++ = 0;
      திரும்பு res;
}

வெறுமை vvp_vector4_t::invert()
{
      எனில் (size_ <= BITS_PER_WORD) {
	    unsigned long mask = (size_<BITS_PER_WORD)? (1UL<<size_)-1UL : -1UL;
	    abits_val_ = mask & ~abits_val_;
	    abits_val_ |= bbits_val_;
      } இல்லையெனில் {
	    unsigned remaining = size_;
	    unsigned idx = 0;
	    போது (remaining >= BITS_PER_WORD) {
		  abits_ptr_[idx] = ~abits_ptr_[idx];
		  abits_ptr_[idx] |= bbits_ptr_[idx];
		  idx += 1;
		  remaining -= BITS_PER_WORD;
	    }
	    எனில் (remaining > 0) {
		  unsigned long mask = (1UL<<remaining) - 1UL;
		  abits_ptr_[idx] = mask & ~abits_ptr_[idx];
		  abits_ptr_[idx] |= bbits_ptr_[idx];
	    }
      }
}

#define BIT_MASK(n) ((n) ? ((~0UL) >> (BITS_PER_WORD - (n))) : ~0UL)

vvp_bit4_t vvp_vector4_t::reduce_or() மாறிலி
{
      unsigned long mask = BIT_MASK(size_ % BITS_PER_WORD);
      vvp_bit4_t res = BIT4_0;

      எனில் (size_ <= BITS_PER_WORD) {
	    எனில் ((abits_val_ & ~bbits_val_ & mask) != 0UL)
		  திரும்பு BIT4_1;
	    எனில் ((bbits_val_ & mask) != 0UL)
		  திரும்பு BIT4_X;
      } இல்லையெனில் {
	    unsigned சொற்கள் = (size_ + BITS_PER_WORD - 1) / BITS_PER_WORD;
	    unsigned idx;
	    க்கு (idx = 0; idx < சொற்கள் - 1; idx += 1) {
		  எனில் ((abits_ptr_[idx] & ~bbits_ptr_[idx]) != 0UL)
			திரும்பு BIT4_1;
		  எனில் (bbits_ptr_[idx] != 0UL)
			res = BIT4_X;
	    }
	    எனில் ((abits_ptr_[idx] & ~bbits_ptr_[idx] & mask) != 0UL)
		  திரும்பு BIT4_1;
	    எனில் ((bbits_ptr_[idx] & mask) != 0UL)
		  res = BIT4_X;
      }

      திரும்பு res;
}

vvp_bit4_t vvp_vector4_t::reduce_and() மாறிலி
{
      unsigned long mask = BIT_MASK(size_ % BITS_PER_WORD);
      vvp_bit4_t res = BIT4_1;

      எனில் (size_ <= BITS_PER_WORD) {
	    எனில் ((abits_val_ | bbits_val_ | ~mask) != ~0UL)
		  திரும்பு BIT4_0;
	    எனில் ((bbits_val_ & mask) != 0UL)
		  திரும்பு BIT4_X;
	} இல்லையெனில் {
	    unsigned சொற்கள் = (size_ + BITS_PER_WORD - 1) / BITS_PER_WORD;
	    unsigned idx;
	    க்கு (idx = 0; idx < சொற்கள் - 1; idx += 1) {
		  எனில் ((abits_ptr_[idx] | bbits_ptr_[idx]) != ~0UL)
			திரும்பு BIT4_0;
		  எனில் (bbits_ptr_[idx] != 0UL)
			res = BIT4_X;
	    }
	    எனில் ((abits_ptr_[idx] | bbits_ptr_[idx] | ~mask) != ~0UL)
		  திரும்பு BIT4_0;
	    எனில் ((bbits_ptr_[idx] & mask) != 0UL)
		  res = BIT4_X;
      }

      திரும்பு res;
}

நிலையான unsigned long parity(unsigned long val)
{
#எனில் defined(__GNUC__)
      // The compiler builtin can use target-specific CPU instructions.
      திரும்பு __builtin_parityl(val);
#இல்லையெனில்
#எனில் ULONG_MAX > 0xffffffffUL
	val ^= val >> 32;
#endif
	val ^= val >> 16;
	val ^= val >> 8;
	val ^= val >> 4;

      திரும்பு (0x6996 >> (val & 0xf)) & 1;
#endif
}

vvp_bit4_t vvp_vector4_t::reduce_xor() மாறிலி
{
      unsigned long mask = BIT_MASK(size_ % BITS_PER_WORD);

      எனில் (size_ <= BITS_PER_WORD) {
	    எனில் ((bbits_val_ & mask) != 0UL)
		  திரும்பு BIT4_X;
	    திரும்பு parity(abits_val_ & mask) ? BIT4_1 : BIT4_0;
      } இல்லையெனில் {
	    unsigned சொற்கள் = (size_ + BITS_PER_WORD - 1) / BITS_PER_WORD;
	    unsigned long val_a = 0UL;
	    unsigned idx;
	    க்கு (idx = 0; idx < சொற்கள் - 1; idx += 1) {
		  எனில் (bbits_ptr_[idx] != 0UL)
			திரும்பு BIT4_X;
		  val_a ^= abits_ptr_[idx];
	    }
	    எனில் ((bbits_ptr_[idx] & mask) != 0UL)
		  திரும்பு BIT4_X;
	    val_a ^= abits_ptr_[idx] & mask;
	    திரும்பு parity(val_a) ? BIT4_1 : BIT4_0;
      }
}

vvp_vector4_t& vvp_vector4_t::operator &= (மாறிலி vvp_vector4_t&that)
{
	// The truth table is:
	//     00 01 11 10
	//  00 00 00 00 00
	//  01 00 01 11 11
	//  11 00 11 11 11
	//  10 00 11 11 11
      எனில் (size_ <= BITS_PER_WORD) {
	    unsigned long tmp1 = abits_val_ | bbits_val_;
	    unsigned long tmp2 = that.abits_val_ | that.bbits_val_;
	    abits_val_ = tmp1 & tmp2;
	    bbits_val_ = (tmp1 & that.bbits_val_) | (tmp2 & bbits_val_);
      } இல்லையெனில் {
	    unsigned சொற்கள் = (size_ + BITS_PER_WORD - 1) / BITS_PER_WORD;
	    க்கு (unsigned idx = 0; idx < சொற்கள் ; idx += 1) {
		  unsigned long tmp1 = abits_ptr_[idx] | bbits_ptr_[idx];
		  unsigned long tmp2 = that.abits_ptr_[idx] |
		                       that.bbits_ptr_[idx];
		  abits_ptr_[idx] = tmp1 & tmp2;
		  bbits_ptr_[idx] = (tmp1 & that.bbits_ptr_[idx]) |
		                    (tmp2 & bbits_ptr_[idx]);
	    }
      }

      திரும்பு *இது;
}

vvp_vector4_t& vvp_vector4_t::operator |= (மாறிலி vvp_vector4_t&that)
{
	// The truth table is:
	//     00 01 11 10
	//  00 00 01 11 11
	//  01 01 01 01 01
	//  11 11 01 11 11
	//  10 11 01 11 11
      எனில் (size_ <= BITS_PER_WORD) {
	    unsigned long tmp = abits_val_ | bbits_val_ |
	                        that.abits_val_ | that.bbits_val_;
	    bbits_val_ = ((~abits_val_ | bbits_val_) & that.bbits_val_) |
	                 ((~that.abits_val_ | that.bbits_val_) & bbits_val_);
	    abits_val_ = tmp;

      } இல்லையெனில் {
	    unsigned சொற்கள் = (size_ + BITS_PER_WORD - 1) / BITS_PER_WORD;
	    க்கு (unsigned idx = 0; idx < சொற்கள் ; idx += 1) {
		  unsigned long tmp = abits_ptr_[idx] | bbits_ptr_[idx] |
	                        that.abits_ptr_[idx] | that.bbits_ptr_[idx];
		  bbits_ptr_[idx] = ((~abits_ptr_[idx] | bbits_ptr_[idx]) &
		                     that.bbits_ptr_[idx]) |
		                    ((~that.abits_ptr_[idx] |
		                      that.bbits_ptr_[idx]) & bbits_ptr_[idx]);
		  abits_ptr_[idx] = tmp;
	    }
      }

      திரும்பு *இது;
}

vvp_vector4_t& vvp_vector4_t::operator ^= (மாறிலி vvp_vector4_t&that)
{
	// The truth table is:
	//     00 01 11 10
	//  00 00 01 11 11
	//  01 01 00 11 11
	//  11 11 11 11 11
	//  10 11 11 11 11
      எனில் (size_ <= BITS_PER_WORD) {
	    unsigned long bval = bbits_val_ | that.bbits_val_;
	    bbits_val_ = bval;
	    abits_val_ = (abits_val_ ^ that.abits_val_) | bval;
      } இல்லையெனில் {
	    unsigned சொற்கள் = (size_ + BITS_PER_WORD - 1) / BITS_PER_WORD;
	    க்கு (unsigned idx = 0; idx < சொற்கள் ; idx += 1) {
		  unsigned long bval = bbits_ptr_[idx] | that.bbits_ptr_[idx];
		  bbits_ptr_[idx] = bval;
		  abits_ptr_[idx] = (abits_ptr_[idx] ^ that.abits_ptr_[idx]) | bval;
	    }
      }

      திரும்பு *இது;
}

/*
* Add an integer to the vvp_vector4_t in place, bit by bit so that
* there is no size limitations.
*/
vvp_vector4_t& vvp_vector4_t::operator += (int64_t that)
{
      vvp_bit4_t carry = BIT4_0;
      unsigned idx;

      எனில் (has_xz()) {
	    vvp_vector4_t xxx (size(), BIT4_X);
	    *இது = xxx;
	    திரும்பு *இது;
      }

      க்கு (idx = 0 ; idx < size() ; idx += 1) {
	    எனில் (that == 0 && carry==BIT4_0)
		  நிறுத்து;

	    vvp_bit4_t that_bit = (that&1)? BIT4_1 : BIT4_0;
	    that >>= 1;

	    எனில் (that_bit==BIT4_0 && carry==BIT4_0)
		  தொடர்;

	    vvp_bit4_t bit = value(idx);
	    bit = add_with_carry(bit, that_bit, carry);

	    set_bit(idx, bit);
      }

      திரும்பு *இது;
}

ostream& operator<< (ostream&out, மாறிலி vvp_vector4_t&that)
{
      out << that.size() << "'b";
      க்கு (unsigned idx = 0 ;  idx < that.size() ;  idx += 1)
	    out << that.value(that.size()-idx-1);
      திரும்பு out;
}

template <வகுப்பு INT>தருக்க vector4_to_மதிப்பு(மாறிலி vvp_vector4_t&vec, INT&val,
					  தருக்க is_signed, தருக்க is_arithmetic)
{
      INT res = 0;
      INT msk = 1;
      தருக்க rc_flag = உண்மை;

      unsigned size = vec.size();
      எனில் (size > 8*அளவு(val)) size = 8*அளவு(val);
      க்கு (unsigned idx = 0 ;  idx < size ;  idx += 1) {
	    தேர்வு (vec.value(idx)) {
		நிலை BIT4_0:
		  நிறுத்து;
		நிலை BIT4_1:
		  res |= msk;
		  நிறுத்து;
		இயல்பு:
		  எனில் (is_arithmetic)
			திரும்பு பொய்;
		  இல்லையெனில்
			rc_flag = பொய்;
	    }

	    msk <<= 1;
      }

      எனில் (is_signed && vec.value(vec.size()-1) == BIT4_1) {
	    எனில் (vec.size() < 8*அளவு(val))
		  res |= static_cast<INT>(-1ULL << vec.size());
      }

      val = res;
      திரும்பு rc_flag;
}

template தருக்க vector4_to_மதிப்பு(மாறிலி vvp_vector4_t&vec, int8_t&val,
			       தருக்க is_signed, தருக்க is_arithmetic);
template தருக்க vector4_to_மதிப்பு(மாறிலி vvp_vector4_t&vec, int16_t&val,
			       தருக்க is_signed, தருக்க is_arithmetic);
template தருக்க vector4_to_மதிப்பு(மாறிலி vvp_vector4_t&vec, int32_t&val,
			       தருக்க is_signed, தருக்க is_arithmetic);
template தருக்க vector4_to_மதிப்பு(மாறிலி vvp_vector4_t&vec, int64_t&val,
			       தருக்க is_signed, தருக்க is_arithmetic);
template தருக்க vector4_to_மதிப்பு(மாறிலி vvp_vector4_t&vec, uint8_t&val,
			       தருக்க is_signed, தருக்க is_arithmetic);
template தருக்க vector4_to_மதிப்பு(மாறிலி vvp_vector4_t&vec, uint16_t&val,
			       தருக்க is_signed, தருக்க is_arithmetic);
template தருக்க vector4_to_மதிப்பு(மாறிலி vvp_vector4_t&vec, uint32_t&val,
			       தருக்க is_signed, தருக்க is_arithmetic);
template தருக்க vector4_to_மதிப்பு(மாறிலி vvp_vector4_t&vec, uint64_t&val,
			       தருக்க is_signed, தருக்க is_arithmetic);

template <வகுப்பு T> தருக்க vector4_to_மதிப்பு(மாறிலி vvp_vector4_t&vec,
                                         தருக்க&overflow_flag, T&val)
{
      T res = 0;
      T msk = 1;

      overflow_flag = பொய்;
      unsigned size = vec.size();
      க்கு (unsigned idx = 0 ;  idx < size ;  idx += 1) {
	    தேர்வு (vec.value(idx)) {
		நிலை BIT4_0:
		  நிறுத்து;
		நிலை BIT4_1:
		  எனில் (msk == 0)
			overflow_flag = உண்மை;
		  இல்லையெனில்
			res |= msk;
		  நிறுத்து;
		இயல்பு:
		  திரும்பு பொய்;
	    }

	    msk <<= static_cast<T>(1);
      }

      val = res;
      திரும்பு உண்மை;
}

template தருக்க vector4_to_மதிப்பு(மாறிலி vvp_vector4_t&vec, தருக்க&overflow_flag,
                               unsigned long&val);
#ifndef UL_AND_TIME64_SAME
template தருக்க vector4_to_மதிப்பு(மாறிலி vvp_vector4_t&vec, தருக்க&overflow_flag,
                               vvp_time64_t&val);
#endif

தருக்க vector4_to_மதிப்பு(மாறிலி vvp_vector4_t&vec, இரட்டை&val, தருக்க signed_flag)
{

      எனில் (vec.size() == 0) {
	    val = 0.0;
	    திரும்பு உண்மை;
      }

      தருக்க flag = உண்மை;

      எனில் (vec.value(vec.size()-1) != BIT4_1) {
	    signed_flag = பொய்;
      }

      இரட்டை res = 0.0;
      எனில் (signed_flag) {
	    vvp_bit4_t carry = BIT4_1;
	    க்கு (unsigned idx = 0 ;  idx < vec.size() ;  idx += 1) {
		  vvp_bit4_t a = ~vec.value(idx);
		  vvp_bit4_t x = add_with_carry(a, BIT4_0, carry);
		  தேர்வு (x) {
		      நிலை BIT4_0:
			நிறுத்து;
		      நிலை BIT4_1:
			res += pow(2.0, (முழு)idx);
			நிறுத்து;
		      இயல்பு:
			flag = பொய்;
		  }
	    }
	    res *= -1.0;
      } இல்லையெனில் {
	    க்கு (unsigned idx = 0 ;  idx < vec.size() ;  idx += 1) {
		  தேர்வு (vec.value(idx)) {
		      நிலை BIT4_0:
			நிறுத்து;
		      நிலை BIT4_1:
			res += pow(2.0, (முழு)idx);
			நிறுத்து;
		      இயல்பு:
			flag = பொய்;
		  }
	    }
      }
      val = res;
      திரும்பு flag;
}

தருக்க vector2_to_மதிப்பு(மாறிலி vvp_vector2_t&a, int32_t&val, தருக்க is_signed)
{
      val = 0;
      unsigned idx;
      uint32_t mask;
      க்கு (idx = 0, mask = 1 ; idx < a.size() && idx < 32 ; idx += 1, mask <<= 1) {
	    எனில் (a.value(idx)) val |= mask;
      }

      எனில் (is_signed && a.size() < 32 && a.value(a.size()-1)) {
	    mask = -1;
	    mask <<= a.size();
	    val |= mask;
      }

      திரும்பு a.size() <= 32;
}

vvp_vector4array_t::vvp_vector4array_t(unsigned width__, unsigned சொற்கள்_2)
: width_(width__), சொற்கள்_3(சொற்கள்_2)
{
}

vvp_vector4array_t::~vvp_vector4array_t()
{
}

வெறுமை vvp_vector4array_t::set_word_(v4cell*cell, மாறிலி vvp_vector4_t&that)
{
      ப்ரதிஜ்ஞா(that.size_ == width_);

      எனில் (width_ <= vvp_vector4_t::BITS_PER_WORD) {
	    cell->abits_val_ = that.abits_val_;
	    cell->bbits_val_ = that.bbits_val_;
	    திரும்பு;
      }

      unsigned cnt = (width_ + vvp_vector4_t::BITS_PER_WORD-1)/vvp_vector4_t::BITS_PER_WORD;

      எனில் (cell->abits_ptr_ == 0) {
	    cell->abits_ptr_ = புதிய unsigned long[2*cnt];
	    cell->bbits_ptr_ = cell->abits_ptr_ + cnt;
      }

      க்கு (unsigned idx = 0 ; idx < cnt ; idx += 1)
	    cell->abits_ptr_[idx] = that.abits_ptr_[idx];
      க்கு (unsigned idx = 0 ; idx < cnt ; idx += 1)
	    cell->bbits_ptr_[idx] = that.bbits_ptr_[idx];
}

vvp_vector4_t vvp_vector4array_t::get_word_(v4cell*cell) மாறிலி
{
      எனில் (width_ <= vvp_vector4_t::BITS_PER_WORD) {
	    vvp_vector4_t res;
	    res.size_ = width_;
	    res.abits_val_ = cell->abits_val_;
	    res.bbits_val_ = cell->bbits_val_;
	    திரும்பு res;
      }

      vvp_vector4_t res (width_, BIT4_X);
      எனில் (cell->abits_ptr_ == 0)
	    திரும்பு res;

      unsigned cnt = (width_ + vvp_vector4_t::BITS_PER_WORD-1)/vvp_vector4_t::BITS_PER_WORD;

      க்கு (unsigned idx = 0 ; idx < cnt ; idx += 1)
	    res.abits_ptr_[idx] = cell->abits_ptr_[idx];
      க்கு (unsigned idx = 0 ; idx < cnt ; idx += 1)
	    res.bbits_ptr_[idx] = cell->bbits_ptr_[idx];

      திரும்பு res;
}

vvp_vector4array_sa::vvp_vector4array_sa(unsigned width__, unsigned சொற்கள்_2)
: vvp_vector4array_t(width__, சொற்கள்_2)
{
      array_ = புதிய v4cell[சொற்கள்_3];

      எனில் (width_ <= vvp_vector4_t::BITS_PER_WORD) {
	    க்கு (unsigned idx = 0 ; idx < சொற்கள்_3 ; idx += 1) {
		  array_[idx].abits_val_ = vvp_vector4_t::WORD_X_ABITS;
		  array_[idx].bbits_val_ = vvp_vector4_t::WORD_X_BBITS;
	    }
      } இல்லையெனில் {
	    க்கு (unsigned idx = 0 ; idx < சொற்கள்_3 ; idx += 1) {
		  array_[idx].abits_ptr_ = 0;
		  array_[idx].bbits_ptr_ = 0;
	    }
      }
}

vvp_vector4array_sa::~vvp_vector4array_sa()
{
      எனில் (array_) {
	    எனில் (width_ > vvp_vector4_t::BITS_PER_WORD) {
		  க்கு (unsigned idx = 0 ; idx < சொற்கள்_3 ; idx += 1)
			எனில் (array_[idx].abits_ptr_)
			      delete[]array_[idx].abits_ptr_;
	    }
	    delete[]array_;
      }
}

வெறுமை vvp_vector4array_sa::set_word(unsigned index, மாறிலி vvp_vector4_t&that)
{
      ப்ரதிஜ்ஞா(index < சொற்கள்_3);

      v4cell*cell = &array_[index];

      set_word_(cell, that);
}

vvp_vector4_t vvp_vector4array_sa::get_word(unsigned index) மாறிலி
{
      எனில் (index >= சொற்கள்_3)
	    திரும்பு vvp_vector4_t(width_, BIT4_X);

      ப்ரதிஜ்ஞா(index < சொற்கள்_3);

      v4cell*cell = &array_[index];

      திரும்பு get_word_(cell);
}

vvp_vector4array_aa::vvp_vector4array_aa(unsigned width__, unsigned சொற்கள்_2)
: vvp_vector4array_t(width__, சொற்கள்_2)
{
      context_idx_ = vpip_add_item_to_context(இது, vpip_peek_context_scope());
}

vvp_vector4array_aa::~vvp_vector4array_aa()
{
}

வெறுமை vvp_vector4array_aa::alloc_instance(vvp_context_t context)
{
      v4cell*array = புதிய v4cell[சொற்கள்_3];

      எனில் (width_ <= vvp_vector4_t::BITS_PER_WORD) {
	    க்கு (unsigned idx = 0 ; idx < சொற்கள்_3 ; idx += 1) {
		  array[idx].abits_val_ = vvp_vector4_t::WORD_X_ABITS;
		  array[idx].bbits_val_ = vvp_vector4_t::WORD_X_BBITS;
	    }
      } இல்லையெனில் {
	    க்கு (unsigned idx = 0 ; idx < சொற்கள்_3 ; idx += 1) {
		  array[idx].abits_ptr_ = 0;
		  array[idx].bbits_ptr_ = 0;
	    }
      }

      vvp_set_context_item(context, context_idx_, array);
}

வெறுமை vvp_vector4array_aa::reset_instance(vvp_context_t context)
{
      v4cell*cell = static_cast<v4cell*>
            (vvp_get_context_item(context, context_idx_));

      எனில் (width_ <= vvp_vector4_t::BITS_PER_WORD) {
	    க்கு (unsigned idx = 0 ; idx < சொற்கள்_3 ; idx += 1) {
		  cell->abits_val_ = vvp_vector4_t::WORD_X_ABITS;
		  cell->bbits_val_ = vvp_vector4_t::WORD_X_BBITS;
                  cell++;
	    }
      } இல்லையெனில் {
            unsigned cnt = (width_ + vvp_vector4_t::BITS_PER_WORD-1)/vvp_vector4_t::BITS_PER_WORD;
	    க்கு (unsigned idx = 0 ; idx < சொற்கள்_3 ; idx += 1) {
		  எனில் (cell->abits_ptr_) {
                        க்கு (unsigned n = 0 ; n < cnt ; n += 1) {
			      cell->abits_ptr_[n] = vvp_vector4_t::WORD_X_ABITS;
			      cell->bbits_ptr_[n] = vvp_vector4_t::WORD_X_BBITS;
                        }
                  }
                  cell++;
	    }
      }
}

#ifdef CHECK_WITH_VALGRIND
வெறுமை vvp_vector4array_aa::free_instance(vvp_context_t context)
{
      v4cell*cell = static_cast<v4cell*>
            (vvp_get_context_item(context, context_idx_));
      delete [] cell;
}
#endif

வெறுமை vvp_vector4array_aa::set_word(unsigned index, மாறிலி vvp_vector4_t&that)
{
      ப்ரதிஜ்ஞா(index < சொற்கள்_3);

      v4cell*cell = static_cast<v4cell*>
            (vthread_get_wt_context_item(context_idx_)) + index;

      set_word_(cell, that);
}

vvp_vector4_t vvp_vector4array_aa::get_word(unsigned index) மாறிலி
{
      எனில் (index >= சொற்கள்_3)
	    திரும்பு vvp_vector4_t(width_, BIT4_X);

      ப்ரதிஜ்ஞா(index < சொற்கள்_3);

      v4cell*cell = static_cast<v4cell*>
            (vthread_get_rd_context_item(context_idx_)) + index;

      திரும்பு get_word_(cell);
}

vvp_vector2_t::vvp_vector2_t()
{
      vec_ = 0;
      wid_ = 0;
}

vvp_vector2_t::vvp_vector2_t(uint64_t v, unsigned wid)
{
      wid_ = wid;
      மாறிலி unsigned bits_per_word = 8 * அளவு(vec_[0]);
      மாறிலி unsigned சொற்கள் = (wid_ + bits_per_word-1) / bits_per_word;

      vec_ = புதிய unsigned long[சொற்கள்];
      க்கு (unsigned idx = 0 ; idx < சொற்கள் ; idx += 1) {
	    மாறிலி unsigned shift = idx * bits_per_word;
	    vec_[idx] = shift < 64 ? v >> shift : 0;
      }
}

vvp_vector2_t::vvp_vector2_t(vvp_vector2_t::fill_t fill, unsigned wid)
{
      wid_ = wid;
      மாறிலி unsigned bits_per_word = 8 * அளவு(vec_[0]);
      மாறிலி unsigned சொற்கள் = (wid_ + bits_per_word-1) / bits_per_word;

      vec_ = புதிய unsigned long[சொற்கள்];
      க்கு (unsigned idx = 0 ;  idx < சொற்கள் ;  idx += 1)
	    vec_[idx] = fill? -1 : 0;
}

vvp_vector2_t::vvp_vector2_t(மாறிலி vvp_vector2_t&that, unsigned base, unsigned wid)
{
      wid_ = wid;
      மாறிலி unsigned சொற்கள் = (wid_ + BITS_PER_WORD-1) / BITS_PER_WORD;

      vec_ = புதிய unsigned long[சொற்கள்];

      க்கு (unsigned idx = 0 ; idx < wid ; idx += 1) {
	    முழு bit = that.value(base+idx);
	    எனில் (bit == 0)
		  தொடர்;
	    unsigned word = idx / BITS_PER_WORD;
	    unsigned long mask = 1UL << (idx % BITS_PER_WORD);
	    vec_[word] |= mask;
      }
}

வெறுமை vvp_vector2_t::copy_from_that_(மாறிலி vvp_vector4_t&that)
{
      wid_ = that.size();
      மாறிலி unsigned சொற்கள் = (that.size() + BITS_PER_WORD-1) / BITS_PER_WORD;

      எனில் (சொற்கள் == 0) {
	    vec_ = 0;
	    wid_ = 0;
	    திரும்பு;
      }

	// Use the subarray method with the xz_to_0 flag set so that
	// we get values even when there are xz bits.
      vec_ = that.subarray(0, wid_, உண்மை);
}

வெறுமை vvp_vector2_t::copy_from_that_(மாறிலி vvp_vector2_t&that)
{
      wid_ = that.wid_;
      மாறிலி unsigned சொற்கள் = (wid_ + BITS_PER_WORD-1) / BITS_PER_WORD;

      எனில் (சொற்கள் == 0) {
	    vec_ = 0;
	    wid_ = 0;
	    திரும்பு;
      }

      vec_ = புதிய unsigned long[சொற்கள்];
      க்கு (unsigned idx = 0 ;  idx < சொற்கள் ;  idx += 1)
	    vec_[idx] = that.vec_[idx];
}

vvp_vector2_t::vvp_vector2_t(மாறிலி vvp_vector2_t&that, unsigned newsize)
{
      wid_ = newsize;
      எனில் (newsize == 0) {
	    vec_ = 0;
	    திரும்பு;
      }

      மாறிலி unsigned சொற்கள் = (wid_ + BITS_PER_WORD-1) / BITS_PER_WORD;
      மாறிலி unsigned twords = (that.wid_ + BITS_PER_WORD-1) / BITS_PER_WORD;

      vec_ = புதிய unsigned long[சொற்கள்];
      க்கு (unsigned idx = 0 ;  idx < சொற்கள் ;  idx += 1) {
	    எனில் (idx < twords)
		  vec_[idx] = that.vec_[idx];
	    இல்லையெனில்
		  vec_[idx] = 0;
      }
}

vvp_vector2_t& vvp_vector2_t::operator= (மாறிலி vvp_vector2_t&that)
{
      எனில் (இது == &that)
	    திரும்பு *இது;

      delete[] vec_;
      vec_ = 0;

      copy_from_that_(that);
      திரும்பு *இது;
}

vvp_vector2_t& vvp_vector2_t::operator= (மாறிலி vvp_vector4_t&that)
{
      delete[]vec_;
      vec_ = 0;
      copy_from_that_(that);
      திரும்பு *இது;
}

vvp_vector2_t& vvp_vector2_t::operator <<= (unsigned முழு shift)
{
      எனில் (wid_ == 0)
	    திரும்பு *இது;

      மாறிலி unsigned சொற்கள் = (wid_ + BITS_PER_WORD-1) / BITS_PER_WORD;

	// Number of words to shift
      மாறிலி unsigned wshift = shift / BITS_PER_WORD;
	// bits to shift within each word.
      மாறிலி unsigned long oshift = shift % BITS_PER_WORD;

	// If shifting the entire value away, then return zeros.
      எனில் (wshift >= சொற்கள்) {
	    க்கு (unsigned idx = 0 ;  idx < சொற்கள் ;  idx += 1)
		  vec_[idx] = 0;

	    திரும்பு *இது;
      }

	// Do the word shift first.
      எனில் (wshift > 0) {
	    க்கு (unsigned idx = 0 ;  idx < சொற்கள்-wshift ;  idx += 1) {
		  unsigned sel = சொற்கள் - idx - 1;
		  vec_[sel] = vec_[sel-wshift];
	    }

	    க்கு (unsigned idx = 0 ;  idx < wshift ;  idx += 1)
		  vec_[idx] = 0;
      }

	// Do the fine shift.
      எனில் (oshift != 0) {
	    unsigned long pad = 0;
	    க்கு (unsigned idx = 0 ;  idx < சொற்கள் ;  idx += 1) {
		  unsigned long next_pad = vec_[idx] >> (BITS_PER_WORD-oshift);
		  vec_[idx] = (vec_[idx] << oshift) | pad;
		  pad = next_pad;
	    }

	      // Cleanup the tail bits.
	    unsigned long mask = -1UL >> (BITS_PER_WORD - wid_%BITS_PER_WORD);
	    vec_[சொற்கள்-1] &= mask;
      }

      திரும்பு *இது;
}

vvp_vector2_t& vvp_vector2_t::operator >>= (unsigned shift)
{
      எனில் (wid_ == 0)
	    திரும்பு *இது;

      மாறிலி unsigned சொற்கள் = (wid_ + BITS_PER_WORD-1) / BITS_PER_WORD;

	// Number of words to shift
      மாறிலி unsigned wshift = shift / BITS_PER_WORD;
	// bits to shift within each word.
      மாறிலி unsigned long oshift = shift % BITS_PER_WORD;

	// If shifting the entire value away, then return zeros.
      எனில் (wshift >= சொற்கள்) {
	    க்கு (unsigned idx = 0 ;  idx < சொற்கள் ;  idx += 1)
		  vec_[idx] = 0;

	    திரும்பு *இது;
      }

      எனில் (wshift > 0) {
	    க்கு (unsigned idx = 0 ;  idx < சொற்கள்-wshift ;  idx += 1)
		  vec_[idx] = vec_[idx+wshift];

	    க்கு (unsigned idx = சொற்கள்-wshift ;  idx < சொற்கள் ;  idx += 1)
		  vec_[idx] = 0;
      }

      எனில் (oshift > 0) {
	    unsigned long pad = 0;
	    க்கு (unsigned idx = சொற்கள் ;  idx > 0 ;  idx -= 1) {
		  unsigned long new_pad = vec_[idx-1] <<(BITS_PER_WORD-oshift);
		  vec_[idx-1] = pad | (vec_[idx-1] >> oshift);
		  pad = new_pad;
	    }

	      // Cleanup the tail bits.

	    unsigned use_சொற்கள் = சொற்கள்;
	      // Mask_shift is the number of high bits of the top word
	      // that are to be masked off. We start with the number
	      // of bits that are not included even in the original,
	      // then we include the bits of the shift, that are to be
	      // masked to zero.
	    unsigned long mask_shift = BITS_PER_WORD - wid_%BITS_PER_WORD;
	    mask_shift %= BITS_PER_WORD;
	    mask_shift += oshift;
	    போது (mask_shift >= BITS_PER_WORD) {
		  vec_[use_சொற்கள்-1] = 0;
		  use_சொற்கள் -= 1;
		  mask_shift -= BITS_PER_WORD;
	    }
	    எனில் (mask_shift > 0) {
		  ப்ரதிஜ்ஞா(use_சொற்கள் > 0);
		  unsigned long mask = -1UL >> mask_shift;
		  vec_[use_சொற்கள்-1] &= mask;
	    }
      }

      திரும்பு *இது;
}

நிலையான unsigned long add_carry(unsigned long a, unsigned long b,
			       unsigned long&carry)
{
      unsigned long out = carry;
      carry = 0;

      எனில் ((ULONG_MAX - out) < a)
	    carry += 1;
      out += a;

      எனில் ((ULONG_MAX - out) < b)
	    carry += 1;
      out += b;

      திரும்பு out;
}

vvp_vector2_t& vvp_vector2_t::operator += (மாறிலி vvp_vector2_t&that)
{
      ப்ரதிஜ்ஞா(wid_ == that.wid_);
      எனில் (wid_ == 0)
	    திரும்பு *இது;

      மாறிலி unsigned சொற்கள் = (wid_ + BITS_PER_WORD-1) / BITS_PER_WORD;

      unsigned long carry = 0;
      க்கு (unsigned idx = 0 ;  idx < சொற்கள் ;  idx += 1) {
	    vec_[idx] = add_carry(vec_[idx], that.vec_[idx], carry);
      }


	// Cleanup the tail bits.
      unsigned long mask = -1UL >> (BITS_PER_WORD - wid_%BITS_PER_WORD);
      vec_[சொற்கள்-1] &= mask;

      திரும்பு *இது;
}

vvp_vector2_t& vvp_vector2_t::operator -= (மாறிலி vvp_vector2_t&that)
{
      ப்ரதிஜ்ஞா(wid_ == that.wid_);
      எனில் (wid_ == 0)
	    திரும்பு *இது;

      மாறிலி unsigned சொற்கள் = (wid_ + BITS_PER_WORD-1) / BITS_PER_WORD;

      unsigned long carry = 1;
      க்கு (unsigned idx = 0 ;  idx < சொற்கள் ;  idx += 1) {
	    vec_[idx] = add_carry(vec_[idx], ~that.vec_[idx], carry);
      }

      திரும்பு *இது;
}

வெறுமை vvp_vector2_t::trim()
{
      போது (value(wid_-1) == 0 && wid_ > 1) wid_ -= 1;
}

/* This is a special trim that is used on numbers we know represent a
 * negative signed value (they came from a negative real value). */
வெறுமை vvp_vector2_t::trim_neg()
{
      எனில் (value(wid_-1) == 1 && wid_ > 32) {
	    போது (value(wid_-2) == 1 && wid_ > 32) wid_ -= 1;
      }
}

முழு vvp_vector2_t::value(unsigned idx) மாறிலி
{
      எனில் (idx >= wid_)
	    திரும்பு 0;

      மாறிலி unsigned bits_per_word = 8 * அளவு(vec_[0]);
      unsigned addr = idx/bits_per_word;
      unsigned mask = idx%bits_per_word;

      எனில் (vec_[addr] & (1UL<<mask))
	    திரும்பு 1;
      இல்லையெனில்
	    திரும்பு 0;
}

வெறுமை vvp_vector2_t::set_bit(unsigned idx, முழு bit)
{
      ப்ரதிஜ்ஞா(idx < wid_);

      மாறிலி unsigned bits_per_word = 8 * அளவு(vec_[0]);
      unsigned addr = idx/bits_per_word;
      unsigned long mask = idx%bits_per_word;

      எனில் (bit)
	    vec_[addr] |= 1UL << mask;
      இல்லையெனில்
	    vec_[addr] &= ~(1UL << mask);
}

வெறுமை vvp_vector2_t::set_vec(unsigned adr, மாறிலி vvp_vector2_t&that)
{
      ப்ரதிஜ்ஞா((adr + that.wid_) <= wid_);

      க்கு (unsigned idx = 0 ; idx < that.wid_ ; idx += 1)
	    set_bit(adr+idx, that.value(idx));
}

தருக்க vvp_vector2_t::is_NaN() மாறிலி
{
      திரும்பு wid_ == 0;
}

தருக்க vvp_vector2_t::is_zero() மாறிலி
{
      மாறிலி unsigned சொற்கள் = (wid_ + BITS_PER_WORD-1) / BITS_PER_WORD;

      க்கு (unsigned idx = 0; idx < சொற்கள்; idx += 1) {
	    எனில் (vec_[idx] == 0) தொடர்;
	    திரும்பு பொய்;
      }

      திரும்பு உண்மை;
}

/*
 * Basic idea from "Introduction to Programming using SML" by
 * Michael R. Hansen and Hans Rischel page 261 and "Seminumerical
 * Algorithms, Third Edition" by Donald E. Knuth section 4.6.3.
 */
vvp_vector2_t pow(மாறிலி vvp_vector2_t&x, vvp_vector2_t&y)
{
        /* If we have a zero exponent just return 1. */
      எனில் (y == vvp_vector2_t(0L, 1)) {
	    திரும்பு vvp_vector2_t(1L, x.size());
      }

        /* Is the value odd? */
      எனில் (y.value(0) == 1) {
	    y.set_bit(0, 0);  // A quick subtract by 1.
	    vvp_vector2_t res = x * pow(x, y);
	    திரும்பு res;
      }

      y >>= 1;  // A fast divide by two. We know the LSB is zero.
      vvp_vector2_t z = pow(x, y);
      vvp_vector2_t res = z * z;
      திரும்பு res;
}

நிலையான வெறுமை multiply_long(unsigned long a, unsigned long b,
			  unsigned long&கீழ், unsigned long&உயர்வு)
{
      ப்ரதிஜ்ஞா(அளவு(unsigned long) %2 == 0);

      மாறிலி unsigned long word_mask = (1UL << 4UL*அளவு(a)) - 1UL;
      unsigned long tmpa;
      unsigned long tmpb;
      unsigned long res[4];

      tmpa = a & word_mask;
      tmpb = b & word_mask;
      res[0] = tmpa * tmpb;
      res[1] = res[0] >> 4UL*அளவு(unsigned long);
      res[0] &= word_mask;

      tmpa = (a >> 4UL*அளவு(unsigned long)) & word_mask;
      res[1] += tmpa * tmpb;
      res[2] = res[1] >> 4UL*அளவு(unsigned long);
      res[1] &= word_mask;

      tmpa = a & word_mask;
      tmpb = (b >> 4UL*அளவு(unsigned long)) & word_mask;
      res[1] += tmpa * tmpb;
      res[2] += res[1] >> 4UL*அளவு(unsigned long);
      res[3]  = res[2] >> 4UL*அளவு(unsigned long);
      res[1] &= word_mask;
      res[2] &= word_mask;

      tmpa = (a >> 4UL*அளவு(unsigned long)) & word_mask;
      res[2] += tmpa * tmpb;
      res[3] += res[2] >> 4UL*அளவு(unsigned long);
      res[2] &= word_mask;

      உயர்வு = (res[3] << 4UL*அளவு(unsigned long)) | res[2];
      கீழ்  = (res[1] << 4UL*அளவு(unsigned long)) | res[0];
}

vvp_vector2_t operator * (மாறிலி vvp_vector2_t&a, மாறிலி vvp_vector2_t&b)
{
      மாறிலி unsigned bits_per_word = 8 * அளவு(a.vec_[0]);

	// The compiler ensures that the two operands are of equal size.
      ப்ரதிஜ்ஞா(a.size() == b.size());
      vvp_vector2_t r (0, a.size());

      unsigned சொற்கள் = (r.wid_ + bits_per_word - 1) / bits_per_word;

      க்கு (unsigned bdx = 0 ;  bdx < சொற்கள் ;  bdx += 1) {
	    unsigned long tmpb = b.vec_[bdx];
	    எனில் (tmpb == 0)
		  தொடர்;

	    க்கு (unsigned adx = 0 ;  adx < சொற்கள் ;  adx += 1) {
		  unsigned long tmpa = a.vec_[adx];
		  எனில் (tmpa == 0)
			தொடர்;

		  unsigned long கீழ், hig;
		  multiply_long(tmpa, tmpb, கீழ், hig);

		  unsigned long carry = 0;
		  க்கு (unsigned sdx = 0
			     ; (adx+bdx+sdx) < சொற்கள்
			     ;  sdx += 1) {

			r.vec_[adx+bdx+sdx] = add_carry(r.vec_[adx+bdx+sdx],
							கீழ், carry);
			கீழ் = hig;
			hig = 0;
		  }
	    }
      }

      திரும்பு r;
}

நிலையான வெறுமை div_mod (vvp_vector2_t dividend, vvp_vector2_t வகுஎண்,
		     vvp_vector2_t&quotient, vvp_vector2_t&remainder)
{

      quotient = vvp_vector2_t(0, dividend.size());

      எனில் (வகுஎண் == quotient) {
	    cerr << "ERROR: division by zero, exiting." << endl;
	    exit(255);
      }

      எனில் (dividend < வகுஎண்) {
	    remainder = dividend;
	    திரும்பு;
      }

      vvp_vector2_t mask (1, dividend.size());

	// Make the dividend 1 bit larger to prevent overflow of
	// divtmp in startup.
      dividend = vvp_vector2_t(dividend, dividend.size()+1);
      vvp_vector2_t divtmp (வகுஎண், dividend.size());

      போது (divtmp < dividend) {
	    divtmp <<= 1;
	    mask <<= 1;
      }

      போது (dividend >= வகுஎண்) {
	    எனில் (divtmp <= dividend) {
		  dividend -= divtmp;
		  quotient += mask;
	    }

	    divtmp >>= 1;
	    mask >>= 1;
      }

      remainder = vvp_vector2_t(dividend, mask.size());
}

vvp_vector2_t operator - (மாறிலி vvp_vector2_t&that)
{
      vvp_vector2_t neg(that);
      எனில் (neg.wid_ == 0) திரும்பு neg;

      மாறிலி unsigned சொற்கள் = (neg.wid_ + neg.BITS_PER_WORD-1) /
                                         neg.BITS_PER_WORD;
      க்கு (unsigned idx = 0 ;  idx < சொற்கள் ;  idx += 1) {
	    neg.vec_[idx] = ~neg.vec_[idx];
      }
      neg += vvp_vector2_t(1, neg.wid_);

      திரும்பு neg;
}

vvp_vector2_t operator / (மாறிலி vvp_vector2_t&dividend,
			  மாறிலி vvp_vector2_t&வகுஎண்)
{
      vvp_vector2_t quot, rem;
      div_mod(dividend, வகுஎண், quot, rem);
      திரும்பு quot;
}

vvp_vector2_t operator % (மாறிலி vvp_vector2_t&dividend,
			  மாறிலி vvp_vector2_t&வகுஎண்)
{
      vvp_vector2_t quot, rem;
      div_mod(dividend, வகுஎண், quot, rem);
      திரும்பு rem;
}

தருக்க operator > (மாறிலி vvp_vector2_t&a, மாறிலி vvp_vector2_t&b)
{
      மாறிலி unsigned awords = (a.wid_ + vvp_vector2_t::BITS_PER_WORD-1) / vvp_vector2_t::BITS_PER_WORD;
      மாறிலி unsigned bwords = (b.wid_ + vvp_vector2_t::BITS_PER_WORD-1) / vvp_vector2_t::BITS_PER_WORD;

      மாறிலி unsigned சொற்கள் = awords > bwords? awords : bwords;

      க்கு (unsigned idx = சொற்கள் ;  idx > 0 ;  idx -= 1) {
	    unsigned long aw = (idx <= awords)? a.vec_[idx-1] : 0;
	    unsigned long bw = (idx <= bwords)? b.vec_[idx-1] : 0;

	    எனில் (aw > bw)
		  திரும்பு உண்மை;
	    எனில் (aw < bw)
		  திரும்பு பொய்;
      }

	// If the above loop finishes, then the vectors are equal.
      திரும்பு பொய்;
}

தருக்க operator >= (மாறிலி vvp_vector2_t&a, மாறிலி vvp_vector2_t&b)
{
      மாறிலி unsigned awords = (a.wid_ + vvp_vector2_t::BITS_PER_WORD-1) / vvp_vector2_t::BITS_PER_WORD;
      மாறிலி unsigned bwords = (b.wid_ + vvp_vector2_t::BITS_PER_WORD-1) / vvp_vector2_t::BITS_PER_WORD;

      மாறிலி unsigned சொற்கள் = awords > bwords? awords : bwords;

      க்கு (unsigned idx = சொற்கள் ;  idx > 0 ;  idx -= 1) {
	    unsigned long aw = (idx <= awords)? a.vec_[idx-1] : 0;
	    unsigned long bw = (idx <= bwords)? b.vec_[idx-1] : 0;

	    எனில் (aw > bw)
		  திரும்பு உண்மை;
	    எனில் (aw < bw)
		  திரும்பு பொய்;
      }

	// If the above loop finishes, then the vectors are equal.
      திரும்பு உண்மை;
}

தருக்க operator < (மாறிலி vvp_vector2_t&a, மாறிலி vvp_vector2_t&b)
{
      மாறிலி unsigned awords = (a.wid_ + vvp_vector2_t::BITS_PER_WORD-1) / vvp_vector2_t::BITS_PER_WORD;
      மாறிலி unsigned bwords = (b.wid_ + vvp_vector2_t::BITS_PER_WORD-1) / vvp_vector2_t::BITS_PER_WORD;

      unsigned சொற்கள் = awords;
      எனில் (bwords > சொற்கள்)
	    சொற்கள் = bwords;

      க்கு (unsigned idx = சொற்கள் ;  idx > 0 ;  idx -= 1) {
	    unsigned long aw = (idx <= awords)? a.vec_[idx-1] : 0;
	    unsigned long bw = (idx <= bwords)? b.vec_[idx-1] : 0;

	    எனில் (aw < bw)
		  திரும்பு உண்மை;
	    எனில் (aw > bw)
		  திரும்பு பொய்;
      }

	// If the above loop finishes, then the vectors are equal.
      திரும்பு பொய்;
}

தருக்க operator <= (மாறிலி vvp_vector2_t&a, மாறிலி vvp_vector2_t&b)
{
	// XXXX For now, only support equal width vectors.
      ப்ரதிஜ்ஞா(a.wid_ == b.wid_);

      மாறிலி unsigned awords = (a.wid_ + vvp_vector2_t::BITS_PER_WORD-1) / vvp_vector2_t::BITS_PER_WORD;

      க்கு (unsigned idx = awords ;  idx > 0 ;  idx -= 1) {
	    எனில் (a.vec_[idx-1] < b.vec_[idx-1])
		  திரும்பு உண்மை;
	    எனில் (a.vec_[idx-1] > b.vec_[idx-1])
		  திரும்பு பொய்;
      }

	// If the above loop finishes, then the vectors are equal.
      திரும்பு உண்மை;
}

தருக்க operator == (மாறிலி vvp_vector2_t&a, மாறிலி vvp_vector2_t&b)
{
      மாறிலி unsigned awords = (a.wid_ + vvp_vector2_t::BITS_PER_WORD-1) / vvp_vector2_t::BITS_PER_WORD;
      மாறிலி unsigned bwords = (b.wid_ + vvp_vector2_t::BITS_PER_WORD-1) / vvp_vector2_t::BITS_PER_WORD;

      மாறிலி unsigned சொற்கள் = awords > bwords? awords : bwords;

      க்கு (unsigned idx = சொற்கள் ;  idx > 0 ;  idx -= 1) {
	    unsigned long aw = (idx <= awords)? a.vec_[idx-1] : 0;
	    unsigned long bw = (idx <= bwords)? b.vec_[idx-1] : 0;

	    எனில் (aw > bw)
		  திரும்பு பொய்;
	    எனில் (aw < bw)
		  திரும்பு பொய்;
      }

	// If the above loop finishes, then the vectors are equal.
      திரும்பு உண்மை;
}


vvp_vector4_t vector2_to_vector4(மாறிலி vvp_vector2_t&that, unsigned wid)
{
      vvp_vector4_t res (wid);

      க்கு (unsigned idx = 0 ;  idx < res.size() ;  idx += 1) {
	    vvp_bit4_t bit = BIT4_0;

	    எனில் (that.value(idx))
		  bit = BIT4_1;

	    res.set_bit(idx, bit);
      }

      திரும்பு res;
}

நிலையான தருக்க c4string_header_test(மாறிலி எழுத்து*str)
{
      எனில் ((str[0] != 'C') && (str[0] != 'c'))
	    திரும்பு பொய்;
      எனில் ((str[1] != '4') || (str[2] != '<'))
	    திரும்பு பொய்;
      திரும்பு உண்மை;
}

தருக்க c4string_test(மாறிலி எழுத்து*str)
{
      எனில் (!c4string_header_test(str))
	    திரும்பு பொய்;
      size_t மதிப்பு_size = strspn(str+3, "01xz");
      எனில் (str[3+மதிப்பு_size] != '>')
	    திரும்பு பொய்;
      எனில் (str[3+மதிப்பு_size+1] != 0)
	    திரும்பு பொய்;

      திரும்பு உண்மை;
}

vvp_vector4_t c4string_to_vector4(மாறிலி எழுத்து*str)
{
      ப்ரதிஜ்ஞா(c4string_header_test(str));

      str += 3;
      மாறிலி எழுத்து*tp = str + strspn(str,"01xz");
      ப்ரதிஜ்ஞா(tp[0] == '>');

      vvp_vector4_t tmp (tp-str);

      க்கு (unsigned idx = 0 ;  idx < tmp.size() ;  idx += 1) {
	    vvp_bit4_t bit;
	    தேர்வு (str[idx]) {
		நிலை '0':
		  bit = BIT4_0;
		  நிறுத்து;
		நிலை '1':
		  bit = BIT4_1;
		  நிறுத்து;
		நிலை 'x':
		  bit = BIT4_X;
		  நிறுத்து;
		நிலை 'z':
		  bit = BIT4_Z;
		  நிறுத்து;
		இயல்பு:
		  fprintf(stderr, "Unsupported bit value %c(%d).\n", str[idx],
		          str[idx]);
		  ப்ரதிஜ்ஞா(0);
		  bit = BIT4_0;
		  நிறுத்து;
	    }
	    tmp.set_bit(tmp.size()-idx-1, bit);
      }

      திரும்பு tmp;
}

ostream& operator<< (ostream&out, மாறிலி vvp_vector2_t&that)
{
      எனில் (that.is_NaN()) {
	    out << "NaN";

      } இல்லையெனில் {
	    out << vector2_to_vector4(that, that.size());
      }
      திரும்பு out;
}

vvp_vector8_t::vvp_vector8_t(மாறிலி vvp_vector8_t&that)
{
      size_ = that.size_;
      எனில் (size_ <= அளவு(val_)) {
	    memcpy(val_, that.val_, அளவு val_);
      } இல்லையெனில் {
	    ptr_ = புதிய unsigned எழுத்து[size_];
	    memcpy(ptr_, that.ptr_, size_);
      }
}

vvp_vector8_t::vvp_vector8_t(மாறிலி vvp_vector4_t&that,
			     unsigned str0, unsigned str1)
: size_(that.size())
{
      எனில் (size_ == 0)
	    திரும்பு;

      எனில் (size_ <= அளவு(val_)) {
	    ptr_ = 0; // Prefill all val_ bytes
	    க்கு (unsigned idx = 0 ; idx < size_ ; idx += 1)
		  val_[idx] = vvp_scalar_t(that.value(idx),str0, str1).raw();
      } இல்லையெனில் {
	    ptr_ = புதிய unsigned எழுத்து[size_];
	    க்கு (unsigned idx = 0 ;  idx < size_ ;  idx += 1)
		  ptr_[idx] = vvp_scalar_t(that.value(idx), str0, str1).raw();
      }
}

vvp_vector8_t::vvp_vector8_t(மாறிலி vvp_vector2_t&that,
			     unsigned str0, unsigned str1)
: size_(that.size())
{
      எனில் (size_ == 0)
	    திரும்பு;

      எனில் (size_ <= அளவு(val_)) {
	    ptr_ = 0;
	    க்கு (unsigned idx = 0 ; idx < size_ ; idx += 1)
		  val_[idx] = vvp_scalar_t(that.value(idx)? BIT4_1:BIT4_0, str0, str1).raw();
      } இல்லையெனில் {
	    ptr_ = புதிய unsigned எழுத்து[size_];
	    க்கு (unsigned idx = 0 ; idx < size_ ; idx += 1)
		  ptr_[idx] = vvp_scalar_t(that.value(idx)? BIT4_1:BIT4_0, str0, str1).raw();
      }
}

மாறிலி vvp_vector8_t vvp_vector8_t::nil;

vvp_vector8_t& vvp_vector8_t::operator= (மாறிலி vvp_vector8_t&that)
{
	// Assign to self.
      எனில் (இது == &that)
	    திரும்பு *இது;

      எனில் (size_ != that.size_) {
	    எனில் (size_ > அளவு(val_))
		  delete[]ptr_;
	    size_ = 0;
      }

      எனில் (that.size_ == 0) {
	    ப்ரதிஜ்ஞா(size_ == 0);
	    திரும்பு *இது;
      }

      எனில் (that.size_ <= அளவு(val_)) {
	    size_ = that.size_;
	    memcpy(val_, that.val_, அளவு(val_));
	    திரும்பு *இது;
      }

      எனில் (size_ == 0) {
	    size_ = that.size_;
	    ptr_ = புதிய unsigned எழுத்து[size_];
      }

      memcpy(ptr_, that.ptr_, size_);

      திரும்பு *இது;
}

vvp_vector8_t vvp_vector8_t::subvalue(unsigned base, unsigned wid) மாறிலி
{
      vvp_vector8_t tmp (wid);

      unsigned எழுத்து*tmp_ptr = tmp.size_ <= அளவு(val_) ? tmp.val_ : tmp.ptr_;
      மாறிலி unsigned எழுத்து*use_ptr = size_ <= அளவு(val_) ? val_ : ptr_;

      unsigned idx = 0;
      போது ((idx < wid) && (base+idx < size_)) {
	    tmp_ptr[idx] = use_ptr[base+idx];
	    idx += 1;
      }

      திரும்பு tmp;
}

வெறுமை vvp_vector8_t::set_vec(unsigned base, மாறிலி vvp_vector8_t&that)
{
      ப்ரதிஜ்ஞா((base+that.size()) <= size());
      க்கு (unsigned idx = 0 ; idx < that.size() ; idx += 1)
	    set_bit(base+idx, that.value(idx));
}

vvp_vector8_t part_expand(மாறிலி vvp_vector8_t&that, unsigned wid, unsigned off)
{
      ப்ரதிஜ்ஞா(off < wid);
      vvp_vector8_t tmp (wid);

      unsigned எழுத்து* tmp_ptr = tmp.size_<= அளவு(tmp.val_) ?
                               tmp.val_ : tmp.ptr_;
      மாறிலி unsigned எழுத்து* that_ptr = that.size_<= அளவு(that.val_) ?
                                      that.val_ : that.ptr_;

      unsigned idx = off;

      போது (idx < wid && that.size_ > (idx-off)) {
	    tmp_ptr[idx] = that_ptr[idx-off];
	    idx += 1;
      }

      திரும்பு tmp;
}

நிலையான தருக்க c8string_header_test(மாறிலி எழுத்து*str)
{
      எனில் ((str[0] != 'C') && (str[0] != 'c'))
	    திரும்பு பொய்;
      எனில் ((str[1] != '8') || (str[2] != '<'))
	    திரும்பு பொய்;
      திரும்பு உண்மை;
}

தருக்க c8string_test(மாறிலி எழுத்து*str)
{
      எனில் (!c8string_header_test(str))
	    திரும்பு பொய்;

      மாறிலி எழுத்து*cp = str+3;
      க்கு (;; cp += 1) {
	    எனில் (cp[0] == '>' && cp[1] == 0) திரும்பு உண்மை;
	    எனில் (cp[0] >= '0' && cp[0] <= '9') தொடர்;
	    எனில் (cp[0] == 'x') தொடர்;
	    எனில் (cp[0] == 'z') தொடர்;
	    திரும்பு பொய்;
      }
      திரும்பு பொய்;
}
/*
 * The format of a C8<> string is:
 *   C8<aaabbbccc...>
 * where aaa... is a 3 character bit descriptor.
 */
vvp_vector8_t c8string_to_vector8(மாறிலி எழுத்து*str)
{
      ப்ரதிஜ்ஞா(c8string_header_test(str));

      size_t vsize = strlen(str)-4;
      ப்ரதிஜ்ஞா(vsize%3 == 0);
      vsize /= 3;
      vvp_vector8_t tmp (vsize);

      க்கு (size_t idx = 0 ; idx < vsize ; idx += 1) {
	    மாறிலி எழுத்து*cp = str+3+3*idx;
	    vvp_bit4_t bit = BIT4_X;
	    unsigned dr0 = cp[0]-'0';
	    unsigned dr1 = cp[1]-'0';
	    தேர்வு (cp[2]) {
		நிலை '0':
		  bit = BIT4_0;
		  நிறுத்து;
		நிலை '1':
		  bit = BIT4_1;
		  நிறுத்து;
		நிலை 'x':
		  bit = BIT4_X;
		  நிறுத்து;
		நிலை 'z':
		  bit = BIT4_Z;
		  நிறுத்து;
	    }
	    tmp.set_bit(vsize-idx-1, vvp_scalar_t(bit, dr0, dr1));
      }

      திரும்பு tmp;
}

ostream& operator<<(ostream&out, மாறிலி vvp_vector8_t&that)
{
      out << "C8<";
      க்கு (unsigned idx = 0 ;  idx < that.size() ; idx += 1)
	    out << that.value(that.size()-idx-1);

      out << ">";
      திரும்பு out;
}

vvp_net_fun_t::vvp_net_fun_t()
{
      மொத்தம்_functors += 1;
}

vvp_net_fun_t::~vvp_net_fun_t()
{
}

வெறுமை vvp_net_fun_t::recv_vec4(vvp_net_ptr_t, மாறிலி vvp_vector4_t&,
                              vvp_context_t)
{
      fprintf(stderr, "internal error: %s: recv_vec4 not implemented\n",
	      typeid(*இது).name());
      ப்ரதிஜ்ஞா(0);
}

வெறுமை vvp_net_fun_t::recv_vec4_pv_(vvp_net_ptr_t p, மாறிலி vvp_vector4_t&bit,
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

      ப்ரதிஜ்ஞா(base + bit.size() <= vwid);

      vvp_vector4_t tmp(vwid, BIT4_Z);
      tmp.set_vec(base, bit);
      recv_vec4(p, tmp, 0);
}

வெறுமை vvp_net_fun_t::recv_vec4_pv(vvp_net_ptr_t, மாறிலி vvp_vector4_t&bit,
                                 unsigned base, unsigned vwid, vvp_context_t)
{
      cerr << "internal error: " << typeid(*இது).name() << ": "
	   << "recv_vec4_pv(" << bit << ", " << base
	   << ", " << vwid << ") not implemented" << endl;
      ப்ரதிஜ்ஞா(0);
}

வெறுமை vvp_net_fun_t::recv_vec8(vvp_net_ptr_t port, மாறிலி vvp_vector8_t&bit)
{
      recv_vec4(port, reduce4(bit), 0);
}

வெறுமை vvp_net_fun_t::recv_vec8_pv_(vvp_net_ptr_t p, மாறிலி vvp_vector8_t&bit,
				  unsigned base, unsigned vwid)
{
	// This is the strength-aware version of recv_vec4_pv_.

      ப்ரதிஜ்ஞா(base + bit.size() <= vwid);

      vvp_vector8_t tmp(vwid);
      tmp.set_vec(base, bit);
      recv_vec8(p, tmp);
}

வெறுமை vvp_net_fun_t::recv_vec8_pv(vvp_net_ptr_t port, மாறிலி vvp_vector8_t&bit,
				 unsigned base, unsigned vwid)
{
      recv_vec4_pv(port, reduce4(bit), base, vwid, 0);
}

வெறுமை vvp_net_fun_t::recv_real(vvp_net_ptr_t, இரட்டை bit, vvp_context_t)
{
      fprintf(stderr, "internal error: %s: recv_real(%f) not implemented\n",
	      typeid(*இது).name(), bit);
      ப்ரதிஜ்ஞா(0);
}

வெறுமை vvp_net_fun_t::recv_string(vvp_net_ptr_t, மாறிலி std::string&bit, vvp_context_t)
{
      fprintf(stderr, "internal error: %s: recv_string(%s) not implemented\n",
	      typeid(*இது).name(), bit.c_str());
      ப்ரதிஜ்ஞா(0);
}

வெறுமை vvp_net_fun_t::recv_object(vvp_net_ptr_t, vvp_object_t, vvp_context_t)
{
      fprintf(stderr, "internal error: %s: recv_object(...) not implemented\n",
	      typeid(*இது).name());
      ப்ரதிஜ்ஞா(0);
}

வெறுமை vvp_net_fun_t::force_flag(தருக்க)
{
}

/* **** vvp_fun_drive methods **** */

vvp_fun_drive::vvp_fun_drive(unsigned str0, unsigned str1)
{
      ப்ரதிஜ்ஞா(str0 < 8);
      ப்ரதிஜ்ஞா(str1 < 8);

      drive0_ = str0;
      drive1_ = str1;
}

vvp_fun_drive::~vvp_fun_drive()
{
}

வெறுமை vvp_fun_drive::recv_vec4(vvp_net_ptr_t port, மாறிலி vvp_vector4_t&bit,
                              vvp_context_t)
{
      ப்ரதிஜ்ஞா(port.port() == 0);
      port.ptr()->send_vec8(vvp_vector8_t(bit, drive0_, drive1_));
}

வெறுமை vvp_fun_drive::recv_vec4_pv(vvp_net_ptr_t ptr, மாறிலி vvp_vector4_t&bit,
				 unsigned base, unsigned vwid, vvp_context_t ctx)
{
      recv_vec4_pv_(ptr, bit, base, vwid, ctx);
}

/* **** vvp_wide_fun_* methods **** */

vvp_wide_fun_core::vvp_wide_fun_core(vvp_net_t*net, unsigned nports)
{
      ptr_ = net;
      nports_ = nports;
      port_மதிப்புகள் = 0;
      port_rvalues_ = 0;
}

vvp_wide_fun_core::~vvp_wide_fun_core()
{
      delete[] port_மதிப்புகள்;
      delete[] port_rvalues_;
}

வெறுமை vvp_wide_fun_core::propagate_vec4(மாறிலி vvp_vector4_t&bit,
				       vvp_time64_t delay)
{
      எனில் (delay)
	    schedule_propagate_vector(ptr_, delay, bit);
      இல்லையெனில்
	    ptr_->send_vec4(bit, 0);
}

வெறுமை vvp_wide_fun_core::propagate_real(இரட்டை bit,
				       vvp_time64_t delay)
{
      எனில் (delay) {
	      // schedule_assign_vector(ptr_->out, bit, delay);
	    ப்ரதிஜ்ஞா(0); // Need a real-value version of assign_vector.
      } இல்லையெனில் {
	    ptr_->send_real(bit, 0);
      }
}


unsigned vvp_wide_fun_core::port_மொத்தம்() மாறிலி
{
      திரும்பு nports_;
}

vvp_vector4_t& vvp_wide_fun_core::value(unsigned idx)
{
      ப்ரதிஜ்ஞா(idx < nports_);
      எனில் (port_மதிப்புகள் == 0)
	    port_மதிப்புகள் = புதிய vvp_vector4_t [nports_];
      திரும்பு port_மதிப்புகள்[idx];
}

இரட்டை vvp_wide_fun_core::மதிப்பு_r(unsigned idx)
{
      ப்ரதிஜ்ஞா(idx < nports_);
      திரும்பு port_rvalues_? port_rvalues_[idx] : 0.0;
}

வெறுமை vvp_wide_fun_core::recv_real_from_inputs(unsigned)
{
      ப்ரதிஜ்ஞா(0);
}

வெறுமை vvp_wide_fun_core::dispatch_vec4_from_உள்ளீடு(unsigned port,
						  மாறிலி vvp_vector4_t&bit)
{
      ப்ரதிஜ்ஞா(port < nports_);
      எனில் (port_மதிப்புகள் == 0) port_மதிப்புகள் = புதிய vvp_vector4_t [nports_];
      port_மதிப்புகள்[port] = bit;
      recv_vec4_from_inputs(port);
}

வெறுமை vvp_wide_fun_core::dispatch_real_from_உள்ளீடு(unsigned port,
						  இரட்டை bit)
{
      ப்ரதிஜ்ஞா(port < nports_);
      எனில் (port_rvalues_ == 0) port_rvalues_ = புதிய இரட்டை[nports_];
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

வெறுமை vvp_wide_fun_t::recv_vec4(vvp_net_ptr_t port, மாறிலி vvp_vector4_t&bit,
                               vvp_context_t)
{
      unsigned pidx = port_base_ + port.port();
      core_->dispatch_vec4_from_உள்ளீடு(pidx, bit);
}

வெறுமை vvp_wide_fun_t::recv_vec4_pv(vvp_net_ptr_t ptr, மாறிலி vvp_vector4_t&bit,
				  unsigned base, unsigned vwid, vvp_context_t ctx)
{
      recv_vec4_pv_(ptr, bit, base, vwid, ctx);
}

வெறுமை vvp_wide_fun_t::recv_real(vvp_net_ptr_t port, இரட்டை bit,
                               vvp_context_t)
{
      unsigned pidx = port_base_ + port.port();
      core_->dispatch_real_from_உள்ளீடு(pidx, bit);
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

unsigned vvp_scalar_t::strength0() மாறிலி
{
      திரும்பு STREN0(மதிப்பு_2);
}

unsigned vvp_scalar_t::strength1() மாறிலி
{
      திரும்பு STREN1(மதிப்பு_2);
}

ostream& operator <<(ostream&out, vvp_scalar_t a)
{
      out << a.strength0() << a.strength1();
      தேர்வு (a.value()) {
	  நிலை BIT4_0:
	    out << "0";
	    நிறுத்து;
	  நிலை BIT4_1:
	    out << "1";
	    நிறுத்து;
	  நிலை BIT4_X:
	    out << "X";
	    நிறுத்து;
	  நிலை BIT4_Z:
	    out << "Z";
	    நிறுத்து;
      }
      திரும்பு out;
}

/*
 * This function is only called if the actual interface function rules
 * out some of the easy cases. If we get here, we can assume that
 * neither of the values is HiZ, and the values are not exactly equal.
 */
vvp_scalar_t fully_featured_resolv_(vvp_scalar_t a, vvp_scalar_t b)
{

      எனில் (UNAMBIG(a.மதிப்பு_2) && UNAMBIG(b.மதிப்பு_2)) {

	      /* If both signals are unambiguous, simply choose
		 the stronger. If they have the same strength
		 but different values, then this becomes
		 ambiguous. */

	    எனில் ((b.மதிப்பு_2&0x07) > (a.மதிப்பு_2&0x07)) {

		    /* b value is stronger. Take it. */
		  திரும்பு b;

	    } இல்லையெனில் எனில் ((b.மதிப்பு_2&0x77) == (a.மதிப்பு_2&0x77)) {

		    // Strengths are the same. Since we know already
		    // that the values are not the same, Make value
		    // into "x".
		  vvp_scalar_t tmp (a);
		  tmp.மதிப்பு_2 = (tmp.மதிப்பு_2&0x77) | 0x80;
		  திரும்பு tmp;

	    } இல்லையெனில் {

		    /* Must be "a" is the stronger one. */
		  திரும்பு a;
	    }

      }

	/* If one of the signals is unambiguous, then it
	   will sweep up the weaker parts of the ambiguous
	   signal. The result may be ambiguous, or maybe not. */

      எனில் (UNAMBIG(a.மதிப்பு_2)) {
	    vvp_scalar_t res;

	    எனில் ((a.மதிப்பு_2&0x70) > (b.மதிப்பு_2&0x70))
		  res.மதிப்பு_2 |= a.மதிப்பு_2&0xf0;
	    இல்லையெனில்
		  res.மதிப்பு_2 |= b.மதிப்பு_2&0xf0;

	    எனில் ((a.மதிப்பு_2&0x07) > (b.மதிப்பு_2&0x07))
		  res.மதிப்பு_2 |= a.மதிப்பு_2&0x0f;
	    இல்லையெனில்
		  res.மதிப்பு_2 |= b.மதிப்பு_2&0x0f;

	    திரும்பு res;

      } இல்லையெனில் எனில் (UNAMBIG(b.மதிப்பு_2)) {

	    vvp_scalar_t res;

	    எனில் ((b.மதிப்பு_2&0x70) > (a.மதிப்பு_2&0x70))
		  res.மதிப்பு_2 |= b.மதிப்பு_2&0xf0;
	    இல்லையெனில்
		  res.மதிப்பு_2 |= a.மதிப்பு_2&0xf0;

	    எனில் ((b.மதிப்பு_2&0x07) > (a.மதிப்பு_2&0x07))
		  res.மதிப்பு_2 |= b.மதிப்பு_2&0x0f;
	    இல்லையெனில்
		  res.மதிப்பு_2 |= a.மதிப்பு_2&0x0f;

	    திரும்பு res;

      }


	/* If both signals are ambiguous, then the result
	   has an even wider ambiguity. */

      unsigned tmp = 0;
      முழு sv1a = (a.மதிப்பு_2 & 0x80) ? STREN1(a.மதிப்பு_2) : - STREN1(a.மதிப்பு_2);
      முழு sv0a = (a.மதிப்பு_2 & 0x08) ? STREN0(a.மதிப்பு_2) : - STREN0(a.மதிப்பு_2);
      முழு sv1b = (b.மதிப்பு_2 & 0x80) ? STREN1(b.மதிப்பு_2) : - STREN1(b.மதிப்பு_2);
      முழு sv0b = (b.மதிப்பு_2 & 0x08) ? STREN0(b.மதிப்பு_2) : - STREN0(b.மதிப்பு_2);

      முழு sv1 = sv1a;
      முழு sv0 = sv0a;

      எனில் (sv0a > sv1)
	    sv1 = sv0a;
      எனில் (sv1b > sv1)
	    sv1 = sv1b;
      எனில் (sv0b > sv1)
	    sv1 = sv0b;

      எனில் (sv1a < sv0)
	    sv0 = sv1a;
      எனில் (sv1b < sv0)
	    sv0 = sv1b;
      எனில் (sv0b < sv0)
	    sv0 = sv0b;

      எனில் (sv1 > 0) {
	    tmp |= 0x80;
	    tmp |= sv1 << 4;
      } இல்லையெனில் {
	      /* Set the MSB when both arguments MSBs are set. This
		 can only happen if both one strengths are zero. */
	    tmp |= (a.மதிப்பு_2&b.மதிப்பு_2)&0x80;
	    tmp |= (-sv1) << 4;
      }

      எனில் (sv0 > 0) {
	    tmp |= 0x08;
	    tmp |= sv0;
      } இல்லையெனில் {
	    tmp |= (-sv0);
      }

      vvp_scalar_t res;
      res.மதிப்பு_2 = tmp;

	/* Canonicalize the HiZ value. */
      எனில் ((res.மதிப்பு_2&0x77) == 0)
	    res.மதிப்பு_2 = 0;

      திரும்பு res;
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

vvp_vector4_t reduce4(மாறிலி vvp_vector8_t&that)
{
      vvp_vector4_t out (that.size());
      க்கு (unsigned idx = 0 ;  idx < out.size() ;  idx += 1)
	    out.set_bit(idx, that.value(idx).value());

      திரும்பு out;
}

vvp_bit4_t compare_gtge(மாறிலி vvp_vector4_t&lef, மாறிலி vvp_vector4_t&rig,
			vvp_bit4_t out_if_equal)
{
      unsigned min_size = lef.size();
      எனில் (rig.size() < min_size)
	    min_size = rig.size();

	// If one of the inputs is nil, treat is as all X values, and
	// that makes the result BIT4_X.
      எனில் (min_size == 0)
	    திரும்பு BIT4_X;

	// As per the IEEE1364 definition of >, >=, < and <=, if there
	// are any X or Z values in either of the operand vectors,
	// then the result of the compare is BIT4_X.

	// Check for X/Z in the left operand
      எனில் (lef.has_xz())
	    திரும்பு BIT4_X;

	// Check for X/Z in the right operand
      எனில் (rig.has_xz())
	    திரும்பு BIT4_X;

      க்கு (unsigned idx = lef.size() ; idx > rig.size() ;  idx -= 1) {
	    எனில் (lef.value(idx-1) == BIT4_1)
		  திரும்பு BIT4_1;
      }

      க்கு (unsigned idx = rig.size() ; idx > lef.size() ;  idx -= 1) {
	    எனில் (rig.value(idx-1) == BIT4_1)
		  திரும்பு BIT4_0;
      }

      க்கு (unsigned idx = min_size ; idx > 0 ;  idx -= 1) {
	    vvp_bit4_t lv = lef.value(idx-1);
	    vvp_bit4_t rv = rig.value(idx-1);

	    எனில் (lv == rv)
		  தொடர்;

	    எனில் (lv == BIT4_1)
		  திரும்பு BIT4_1;
	    இல்லையெனில்
		  திரும்பு BIT4_0;
      }

      திரும்பு out_if_equal;
}

vvp_bit4_t compare_gtge_signed(மாறிலி vvp_vector4_t&a,
			       மாறிலி vvp_vector4_t&b,
			       vvp_bit4_t out_if_equal)
{
      ப்ரதிஜ்ஞா(a.size() == b.size());

      unsigned sign_idx = a.size()-1;
      vvp_bit4_t a_sign = a.value(sign_idx);
      vvp_bit4_t b_sign = b.value(sign_idx);

      எனில் (bit4_is_xz(a_sign))
	    திரும்பு BIT4_X;
      எனில் (bit4_is_xz(b_sign))
	    திரும்பு BIT4_X;

      எனில் (a_sign == b_sign)
	    திரும்பு compare_gtge(a, b, out_if_equal);

      எனில் (a.has_xz())
	    திரும்பு BIT4_X;

      எனில் (b.has_xz())
	    திரும்பு BIT4_X;

      எனில்(a_sign == BIT4_0)
	    திரும்பு BIT4_1;
      இல்லையெனில்
	    திரும்பு BIT4_0;
}
