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

using નામાકાશ std;

/* This is the size of an unsigned long in bits. This is just a
   convenience macro. */
# define CPU_WORD_BITS (8*કદ(unsigned long))
# define ટોચ_BIT (1UL << (CPU_WORD_BITS-1))

permaheap vvp_net_fun_t::heap_;
permaheap vvp_net_fil_t::heap_;

// Allocate around 1Megabyte/chunk.
સ્થિર અચલ size_t VVP_NET_CHUNK = 1024*1024/કદ(vvp_net_t);
સ્થિર vvp_net_t*vvp_net_alloc_table = ખાલી;
#ifdef CHECK_WITH_VALGRIND
સ્થિર vvp_net_t **vvp_net_pool = ખાલી;
સ્થિર unsigned vvp_net_pool_ગણતરી = 0;
#endif
સ્થિર size_t vvp_net_alloc_remaining = 0;
// For statistics, count the vvp_nets allocated and the bytes of alloc
// chunks allocated.
unsigned long ગણતરી_vvp_nets = 0;
size_t size_vvp_nets = 0;

શૂન્ય* vvp_net_t::operator નવું (size_t size)
{
      પ્રતિજ્ઞા(size == કદ(vvp_net_t));
      જો (vvp_net_alloc_remaining == 0) {
	    vvp_net_alloc_table = ::નવું vvp_net_t[VVP_NET_CHUNK];
	    vvp_net_alloc_remaining = VVP_NET_CHUNK;
	    size_vvp_nets += size*VVP_NET_CHUNK;
#ifdef CHECK_WITH_VALGRIND
	    VALGRIND_MAKE_MEM_NOACCESS(vvp_net_alloc_table, size*VVP_NET_CHUNK);
	    VALGRIND_CREATE_MEMPOOL(vvp_net_alloc_table, 0, 0);
	    vvp_net_pool_ગણતરી += 1;
	    vvp_net_pool = static_cast<vvp_net_t **>(realloc(vvp_net_pool,
	                   vvp_net_pool_ગણતરી*કદ(vvp_net_t **)));
	    vvp_net_pool[vvp_net_pool_ગણતરી-1] = vvp_net_alloc_table;
#endif
      }

      vvp_net_t*return_this = vvp_net_alloc_table;
#ifdef CHECK_WITH_VALGRIND
      VALGRIND_MEMPOOL_ALLOC(vvp_net_pool[vvp_net_pool_ગણતરી-1],
                             return_this, size);
      return_this->pool = vvp_net_pool[vvp_net_pool_ગણતરી-1];
#endif
      vvp_net_alloc_table += 1;
      vvp_net_alloc_remaining -= 1;
      ગણતરી_vvp_nets += 1;
      પાછા return_this;
}

#ifdef CHECK_WITH_VALGRIND
સ્થિર map<vvp_net_t*, તાર્કિક> vvp_net_map;
સ્થિર map<sfunc_core*, તાર્કિક> sfunc_map;
સ્થિર map<vvp_udp_fun_core*, તાર્કિક> udp_map;
સ્થિર map<resolv_core*, તાર્કિક> resolv_map;
સ્થિર vvp_net_t **local_net_pool = 0;
સ્થિર unsigned local_net_pool_ગણતરી = 0;

શૂન્ય pool_local_net(vvp_net_t*net)
{
      local_net_pool_ગણતરી += 1;
      local_net_pool = static_cast<vvp_net_t **>(realloc(local_net_pool,
                       local_net_pool_ગણતરી*કદ(vvp_net_t **)));
      local_net_pool[local_net_pool_ગણતરી-1] = net;
}

શૂન્ય vvp_net_delete(vvp_net_t *item)
{
      vvp_net_map[item] = સાચું;
      જો (sfunc_core*tmp = dynamic_cast<sfunc_core*> (item->fun)) {
	    sfunc_map[tmp] = સાચું;
      }
      જો (vvp_udp_fun_core*tmp = dynamic_cast<vvp_udp_fun_core*> (item->fun)) {
	    udp_map[tmp] = સાચું;
      }
      જો (resolv_core*tmp = dynamic_cast<resolv_core*> (item->fun)) {
	    resolv_map[tmp] = સાચું;
      }
	/* Only delete static object variables. */
      જો (vvp_fun_signal_object_sa*tmp = dynamic_cast<vvp_fun_signal_object_sa*> (item->fun)) {
	    delete tmp;
      }
	/* Only delete static string variables. */
      જો (vvp_fun_signal_string_sa*tmp = dynamic_cast<vvp_fun_signal_string_sa*> (item->fun)) {
	    delete tmp;
      }
}

શૂન્ય vvp_net_pool_delete()
{
      unsigned long vvp_nets_del = 0;

      માટે (unsigned idx = 0; idx < local_net_pool_ગણતરી; idx += 1) {
	    vvp_net_delete(local_net_pool[idx]);
      }
      મુક્ત(local_net_pool);
      local_net_pool = 0;
      local_net_pool_ગણતરી = 0;

      map<vvp_net_t*, તાર્કિક>::iterator iter;
      માટે (iter = vvp_net_map.begin(); iter != vvp_net_map.end(); ++ iter ) {
	    vvp_nets_del += 1;
	    VALGRIND_MEMPOOL_FREE(iter->first->pool, iter->first);
      }
      vvp_net_map.clear();

      map<sfunc_core*, તાર્કિક>::iterator siter;
      માટે (siter = sfunc_map.begin(); siter != sfunc_map.end(); ++ siter ) {
	    delete siter->first;
      }
      sfunc_map.clear();

      map<vvp_udp_fun_core*, તાર્કિક>::iterator uiter;
      માટે (uiter = udp_map.begin(); uiter != udp_map.end(); ++ uiter ) {
	    delete uiter->first;
      }
      udp_map.clear();

      map<resolv_core*, તાર્કિક>::iterator riter;
      માટે (riter = resolv_map.begin(); riter != resolv_map.end(); ++ riter ) {
	    delete riter->first;
      }
      resolv_map.clear();

      જો (RUNNING_ON_VALGRIND && (vvp_nets_del != ગણતરી_vvp_nets)) {
	    fflush(ખાલી);
	    VALGRIND_PRINTF("Error: vvp missed deleting %ld of %lu net(s).",
	                    (long) ગણતરી_vvp_nets - vvp_nets_del,
	                    ગણતરી_vvp_nets);
      }

      માટે (unsigned idx = 0; idx < vvp_net_pool_ગણતરી; idx += 1) {
	    VALGRIND_DESTROY_MEMPOOL(vvp_net_pool[idx]);
	    ::delete [] vvp_net_pool[idx];
      }
      મુક્ત(vvp_net_pool);
      vvp_net_pool = ખાલી;
      vvp_net_pool_ગણતરી = 0;
}
#endif

શૂન્ય vvp_net_t::operator delete(શૂન્ય*)
{
      પ્રતિજ્ઞા(0);
}

vvp_net_t::vvp_net_t()
: out_(vvp_net_ptr_t(0,0))
{
      fun = 0;
      fil = 0;
}

શૂન્ય vvp_net_t::link(vvp_net_ptr_t port_to_link)
{
      vvp_net_t*net = port_to_link.ptr();

	// Connect nodes with vvp_fun_modpath_src to the head of the
	// linked list, so that vvp_fun_modpath_src are always evaluated
	// before their respective vvp_fun_modpath
      જો (dynamic_cast<vvp_fun_modpath_src*>(net->fun)) {
	    net->port[port_to_link.port()] = out_;
	    out_ = port_to_link;
	// All other nodes connect after the last vvp_fun_modpath_src
      } નહીંતર {
	      // List is empty or first node has no vvp_fun_modpath_src
	      // then just insert the node into the head of linked list
	    જો (out_.ptr() == nullptr || ! dynamic_cast<vvp_fun_modpath_src*>(out_.ptr()->fun)) {
		  net->port[port_to_link.port()] = out_;
		  out_ = port_to_link;
		  પાછા;
	    }

	    vvp_net_ptr_t cur = out_;
	    vvp_net_ptr_t prev = vvp_net_ptr_t(nullptr,0);

	      // Traverse through linked list
	    જ્યારે (dynamic_cast<vvp_fun_modpath_src*>(cur.ptr()->fun)) {
		  prev = cur;
		  cur = cur.ptr()->port[cur.port()];
		  જો (! cur.ptr()) તોડો; // End of list
	    }

	    પ્રતિજ્ઞા(prev.ptr());

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
શૂન્ય vvp_net_t::unlink(vvp_net_ptr_t dst_ptr)
{
      vvp_net_t*net = dst_ptr.ptr();
      unsigned net_port = dst_ptr.port();

      જો (out_ == dst_ptr) {
	      /* If the drive fan-out list starts with this pointer,
		 then the unlink is easy. Pull the list forward. */
	    out_ = net->port[net_port];
      } નહીંતર જો (! out_.nil()) {
	      /* Scan the linked list, looking for the net_ptr_t
		 pointer *before* the one we wish to remove. */
	    vvp_net_ptr_t cur = out_;
	    પ્રતિજ્ઞા(!cur.nil());
	    vvp_net_t*cur_net = cur.ptr();
	    unsigned cur_port = cur.port();
	    જ્યારે (cur_net && cur_net->port[cur_port] != dst_ptr) {
		  cur = cur_net->port[cur_port];
		  cur_net = cur.ptr();
		  cur_port = cur.port();
	    }
	      /* Unlink. */
	    જો (cur_net) cur_net->port[cur_port] = net->port[net_port];
      }

      net->port[net_port] = vvp_net_ptr_t(0,0);
}

શૂન્ય vvp_net_t::ગણતરી_drivers(unsigned idx, unsigned counts[4])
{
      counts[0] = 0;
      counts[1] = 0;
      counts[2] = 0;
      counts[3] = 0;

        /* $countdrivers can only be used on wires. */
      vvp_wire_base*wire=dynamic_cast<vvp_wire_base*>(fil);
      પ્રતિજ્ઞા(wire);

      જો (wire->is_forced(idx))
            counts[3] = 1;

        /* If the net has multiple drivers, we need to interrogate the
           resolver network to get the driven values. */
      જો (resolv_core*resolver = dynamic_cast<resolv_core*>(fun)) {
            resolver->ગણતરી_drivers(idx, counts);
            પાછા;
      }
      જો (vvp_island_port*resolver = dynamic_cast<vvp_island_port*>(fun)) {
            resolver->ગણતરી_drivers(idx, counts);
            પાછા;
      }

        /* If the functor is not a resolver, there is only one driver, so
           we can just interrogate the filter to get the driven value. */
      update_driver_counts(wire->driven_કિંમત(idx), counts);
}

શૂન્ય vvp_net_fun_t::operator delete(શૂન્ય*)
{
#ifndef CHECK_WITH_VALGRIND
      પ્રતિજ્ઞા(0);
#endif
}


vvp_net_fil_t::vvp_net_fil_t()
{
      force_link_ = 0;
      force_propagate_ = ખોટું;
      ગણતરી_filters += 1;
}

vvp_net_fil_t::~vvp_net_fil_t()
{
      પ્રતિજ્ઞા(force_link_ == 0);
}

vvp_net_fil_t::prop_t vvp_net_fil_t::filter_vec4(અચલ vvp_vector4_t&,
                                                 vvp_vector4_t&,
                                                 unsigned, unsigned)
{
      પાછા PROP;
}

vvp_net_fil_t::prop_t vvp_net_fil_t::filter_vec8(અચલ vvp_vector8_t&,
                                                 vvp_vector8_t&,
                                                 unsigned, unsigned)
{
      પાછા PROP;
}

vvp_net_fil_t::prop_t vvp_net_fil_t::filter_real(દ્વિગુણ&)
{
      પાછા PROP;
}

vvp_net_fil_t::prop_t vvp_net_fil_t::filter_string(અચલ string&)
{
      પાછા PROP;
}

vvp_net_fil_t::prop_t vvp_net_fil_t::filter_object(vvp_object_t&)
{
      પાછા PROP;
}

શૂન્ય vvp_net_fil_t::force_mask(અચલ vvp_vector2_t&mask)
{
      જો (force_mask_.size() == 0)
	    force_mask_ = vvp_vector2_t(vvp_vector2_t::FILL0, mask.size());

      પ્રતિજ્ઞા(force_mask_.size() == mask.size());
      માટે (unsigned idx = 0 ; idx < mask.size() ; idx += 1) {
	    જો (mask.value(idx) == 0)
		  ચાલુ;

	    force_mask_.set_bit(idx, 1);
	    force_propagate_ = સાચું;
      }
}

શૂન્ય vvp_net_fil_t::release_mask(અચલ vvp_vector2_t&mask)
{
      જો (force_mask_.size() == 0)
	    પાછા;

      પ્રતિજ્ઞા(force_mask_.size() == mask.size());
      માટે (unsigned idx = 0 ; idx < mask.size() ; idx += 1) {
	    જો (mask.value(idx))
		  force_mask_.set_bit(idx, 0);
      }

      જો (force_mask_.is_zero())
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
શૂન્ય vvp_net_fil_t::force_link(vvp_net_t*dst, vvp_net_t*src)
{
      પ્રતિજ્ઞા(dst->fil == આ);

      જો (force_link_ == 0) {
	    force_link_ = નવું vvp_net_t;
	      // Use port[3] to hold the force destination.
	    force_link_->port[3] = vvp_net_ptr_t(dst, 0);
	    force_link_->port[2] = vvp_net_ptr_t(0,0);
	    force_link_->fun = નવું vvp_fun_force;
      }

      force_unlink();
      પ્રતિજ્ઞા(force_link_->port[2] == vvp_net_ptr_t(0,0));

	// Use port[2] to hold the force source.
      force_link_->port[2] = vvp_net_ptr_t(src,0);

      vvp_net_ptr_t dst_ptr(force_link_, 0);
      src->link(dst_ptr);
}

શૂન્ય vvp_net_fil_t::operator delete(શૂન્ય*)
{
      પ્રતિજ્ઞા(0);
}

શૂન્ય vvp_net_fil_t::force_unlink(શૂન્ય)
{
      જો (force_link_ == 0) પાછા;
      vvp_net_t*src = force_link_->port[2].ptr();
      જો (src == 0) પાછા;

      src->unlink(vvp_net_ptr_t(force_link_,0));
      force_link_->port[2] = vvp_net_ptr_t(0,0);
}

/* *** BIT operations *** */
vvp_bit4_t add_with_carry(vvp_bit4_t a, vvp_bit4_t b, vvp_bit4_t&c)
{
      જો (bit4_is_xz(a) || bit4_is_xz(b) || bit4_is_xz(c)) {
	    c = BIT4_X;
	    પાછા BIT4_X;
      }

	// NOTE: This relies on the facts that XZ values have been
	// weeded out, and that BIT4_1 is 1 and BIT4_0 is 0.
      પૂર્ણાંક sum = (પૂર્ણાંક)a + (પૂર્ણાંક)b + (પૂર્ણાંક)c;

      પસંદગી (sum) {
	  સ્થિતિ 0:
	      // c must already be 0.
	    પાછા BIT4_0;
	  સ્થિતિ 1:
	    c = BIT4_0;
	    પાછા BIT4_1;
	  સ્થિતિ 2:
	    c = BIT4_1;
	    પાછા BIT4_0;
	  સ્થિતિ 3:
	    c = BIT4_1;
	    પાછા BIT4_1;
      }
      fprintf(stderr, "Incorrect result %d.\n", sum);
      પ્રતિજ્ઞા(0);
      પાછા BIT4_X;
}

vvp_bit4_t scalar_to_bit4(PLI_INT32 scalar)
{
      પસંદગી(scalar) {
	  સ્થિતિ vpi0:
	    પાછા BIT4_0;
	  સ્થિતિ vpi1:
	    પાછા BIT4_1;
	  સ્થિતિ vpiX:
	    પાછા BIT4_X;
	  સ્થિતિ vpiZ:
	    પાછા BIT4_Z;
      }
      fprintf(stderr, "Unsupported scalar value %d.\n", (પૂર્ણાંક)scalar);
      પ્રતિજ્ઞા(0);
      પાછા BIT4_X;
}

vvp_bit4_t operator ^ (vvp_bit4_t a, vvp_bit4_t b)
{
      જો (bit4_is_xz(a))
	    પાછા BIT4_X;
      જો (bit4_is_xz(b))
	    પાછા BIT4_X;
      જો (a == BIT4_0)
	    પાછા b;
      જો (b == BIT4_0)
	    પાછા a;
      પાછા BIT4_0;
}

ostream& operator<<(ostream&out, vvp_bit4_t bit)
{
      પસંદગી (bit) {
	  સ્થિતિ BIT4_0:
	    out << "0";
	    તોડો;
	  સ્થિતિ BIT4_1:
	    out << "1";
	    તોડો;
	  સ્થિતિ BIT4_X:
	    out << "X";
	    તોડો;
	  સ્થિતિ BIT4_Z:
	    out << "Z";
	    તોડો;
	  મૂળભૂત:
	    out << "?";
	    તોડો;
      }
      પાછા out;
}

પ્રકારવ્યાખ્યા unsigned short edge_t;

અંતઃરેખ edge_t VVP_EDGE(vvp_bit4_t from, vvp_bit4_t to)
{
      પાછા 1 << ((from << 2) | to);
}

અચલ edge_t vvp_edge_posedge
      = VVP_EDGE(BIT4_0,BIT4_1)
      | VVP_EDGE(BIT4_0,BIT4_X)
      | VVP_EDGE(BIT4_0,BIT4_Z)
      | VVP_EDGE(BIT4_X,BIT4_1)
      | VVP_EDGE(BIT4_Z,BIT4_1)
      ;

અચલ edge_t vvp_edge_negedge
      = VVP_EDGE(BIT4_1,BIT4_0)
      | VVP_EDGE(BIT4_1,BIT4_X)
      | VVP_EDGE(BIT4_1,BIT4_Z)
      | VVP_EDGE(BIT4_X,BIT4_0)
      | VVP_EDGE(BIT4_Z,BIT4_0)
      ;

પૂર્ણાંક edge(vvp_bit4_t from, vvp_bit4_t to)
{
      edge_t mask = VVP_EDGE(from, to);
      જો (mask & vvp_edge_posedge)
	    પાછા 1;
      જો (mask & vvp_edge_negedge)
	    પાછા -1;
      પાછા 0;
}

unsigned long multiply_with_carry(unsigned long a, unsigned long b,
				  unsigned long&carry)
{
      અચલ unsigned long mask = (1UL << (CPU_WORD_BITS/2)) - 1;
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
      પાછા (r1 << (CPU_WORD_BITS/2)) + r00;
}


શૂન્ય vvp_send_vec8(vvp_net_ptr_t ptr, અચલ vvp_vector8_t&val)
{
      જ્યારે (vvp_net_t*cur = ptr.ptr()) {
	    vvp_net_ptr_t next_val = cur->port[ptr.port()];

	    જો (cur->fun)
		  cur->fun->recv_vec8(ptr, val);

	    ptr = next_val;
      }
}

શૂન્ય vvp_send_real(vvp_net_ptr_t ptr, દ્વિગુણ val, vvp_context_t context)
{
      જ્યારે (vvp_net_t*cur = ptr.ptr()) {
	    vvp_net_ptr_t next_val = cur->port[ptr.port()];

	    જો (cur->fun)
		  cur->fun->recv_real(ptr, val, context);

	    ptr = next_val;
      }
}

અચલ vvp_vector4_t vvp_vector4_t::nil;

શૂન્ય vvp_vector4_t::copy_bits(અચલ vvp_vector4_t&that)
{

      જો (size_ == that.size_) {
	    જો (size_ > BITS_PER_WORD) {
		  unsigned શબ્દો = (size_+BITS_PER_WORD-1) / BITS_PER_WORD;
		  માટે (unsigned idx = 0 ;  idx < શબ્દો ;  idx += 1)
			abits_ptr_[idx] = that.abits_ptr_[idx];
		  માટે (unsigned idx = 0 ;  idx < શબ્દો ;  idx += 1)
			bbits_ptr_[idx] = that.bbits_ptr_[idx];
	    } નહીંતર {
		  abits_val_ = that.abits_val_;
		  bbits_val_ = that.bbits_val_;
	    }
	    પાછા;
      }

	/* Now we know that the sizes of this and that are definitely
	   different. We can use that in code below. In any case, we
	   need to copy only the smaller of the sizes. */

	/* If source and destination are both short, then mask/copy
	   the bit values. */
      જો (size_ <= BITS_PER_WORD && that.size_ <= BITS_PER_WORD) {
	    unsigned bits_to_copy = (that.size_ < size_) ? that.size_ : size_;
	    unsigned long mask = ((bits_to_copy < BITS_PER_WORD) ? (1UL << bits_to_copy) :
	                                                           (0UL)) - 1UL;
	    abits_val_ &= ~mask;
	    bbits_val_ &= ~mask;
	    abits_val_ |= that.abits_val_&mask;
	    bbits_val_ |= that.bbits_val_&mask;
	    પાછા;
      }

	/* Now we know that either source or destination are long. If
	   the destination is short, then mask/copy from the low word
	   of the long source. */
      જો (size_ <= BITS_PER_WORD) {
	    abits_val_ = that.abits_ptr_[0];
	    bbits_val_ = that.bbits_ptr_[0];
	    જો (size_ < BITS_PER_WORD) {
		  unsigned long mask = (1UL << size_) - 1UL;
		  abits_val_ &= mask;
		  bbits_val_ &= mask;
	    }
	    પાછા;
      }

	/* Now we know that the destination must be long. If the
	   source is short, then mask/copy from its value. */
      જો (that.size_ <= BITS_PER_WORD) {
	    unsigned long mask;
	    જો (that.size_ < BITS_PER_WORD) {
		  mask = (1UL << that.size_) - 1UL;
		  abits_ptr_[0] &= ~mask;
		  bbits_ptr_[0] &= ~mask;
	    } નહીંતર {
		  mask = -1UL;
	    }
	    abits_ptr_[0] |= that.abits_val_&mask;
	    bbits_ptr_[0] |= that.bbits_val_&mask;
	    પાછા;
      }

	/* Finally, we know that source and destination are long. copy
	   words until we get to the last. */
      unsigned bits_to_copy = (that.size_ < size_) ? that.size_ : size_;
      unsigned word = 0;
      જ્યારે (bits_to_copy >= BITS_PER_WORD) {
	    abits_ptr_[word] = that.abits_ptr_[word];
	    bbits_ptr_[word] = that.bbits_ptr_[word];
	    bits_to_copy -= BITS_PER_WORD;
	    word += 1;
      }
      જો (bits_to_copy > 0) {
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
શૂન્ય vvp_vector4_t::copy_from_big_(અચલ vvp_vector4_t&that)
{
      unsigned શબ્દો = (size_+BITS_PER_WORD-1) / BITS_PER_WORD;
      abits_ptr_ = નવું unsigned long[2*શબ્દો];
      bbits_ptr_ = abits_ptr_ + શબ્દો;

      માટે (unsigned idx = 0 ;  idx < શબ્દો ;  idx += 1)
	    abits_ptr_[idx] = that.abits_ptr_[idx];
      માટે (unsigned idx = 0 ;  idx < શબ્દો ;  idx += 1)
	    bbits_ptr_[idx] = that.bbits_ptr_[idx];
}

/*
 * The copy_inverted_from_ method is just like the copy_from_ method,
 * except that we combine that with an invert. This allows the ~ and
 * the assignment to be blended in many common cases.
 */
શૂન્ય vvp_vector4_t::copy_inverted_from_(અચલ vvp_vector4_t&that)
{
      size_ = that.size_;
      જો (size_ > BITS_PER_WORD) {
	    unsigned શબ્દો = (size_+BITS_PER_WORD-1) / BITS_PER_WORD;
	    abits_ptr_ = નવું unsigned long[2*શબ્દો];
	    bbits_ptr_ = abits_ptr_ + શબ્દો;

	    unsigned remaining = size_;
	    unsigned idx = 0;
	    જ્યારે (remaining >= BITS_PER_WORD) {
		  abits_ptr_[idx] = that.bbits_ptr_[idx] | ~that.abits_ptr_[idx];
		  idx += 1;
		  remaining -= BITS_PER_WORD;
	    }
	    જો (remaining > 0) {
		  unsigned long mask = (1UL<<remaining) - 1UL;
		  abits_ptr_[idx] = mask & (that.bbits_ptr_[idx] | ~that.abits_ptr_[idx]);
	    }

	    માટે (idx = 0 ;  idx < શબ્દો ;  idx += 1)
		  bbits_ptr_[idx] = that.bbits_ptr_[idx];

      } નહીંતર {
	    unsigned long mask = (size_<BITS_PER_WORD)? (1UL<<size_)-1UL : -1UL;
	    abits_val_ = mask & (that.bbits_val_ | ~that.abits_val_);
	    bbits_val_ = that.bbits_val_;
      }
}

/* Make sure to set size_ before calling this routine. */
શૂન્ય vvp_vector4_t::allocate_શબ્દો(unsigned long inita, unsigned long initb)
{
      જો (size_ > BITS_PER_WORD) {
	    unsigned cnt = (size_ + BITS_PER_WORD - 1) / BITS_PER_WORD;
	    abits_ptr_ = નવું unsigned long[2*cnt];
	    bbits_ptr_ = abits_ptr_ + cnt;
	    માટે (unsigned idx = 0 ;  idx < cnt ;  idx += 1)
		  abits_ptr_[idx] = inita;
	    માટે (unsigned idx = 0 ;  idx < cnt ;  idx += 1)
		  bbits_ptr_[idx] = initb;

      } નહીંતર {
	    abits_val_ = inita;
	    bbits_val_ = initb;
      }
}

vvp_vector4_t::vvp_vector4_t(unsigned size__, દ્વિગુણ val)
: size_(size__)
{
      તાર્કિક is_neg = ખોટું;
      દ્વિગુણ fraction;
      પૂર્ણાંક exponent;

	/* We return 'bx for a NaN or +/- infinity. */
      જો (val != val || (val && (val == 0.5*val)))  {
	    allocate_શબ્દો(WORD_X_ABITS, WORD_X_BBITS);
	    પાછા;
      }

	/* Convert to a positive result. */
      જો (val < 0.0) {
	    is_neg = સાચું;
	    val = -val;
      }
      allocate_શબ્દો(WORD_0_ABITS, WORD_0_BBITS);

	/* Get the exponent and fractional part of the number. */
      fraction = frexp(val, &exponent);

	/* If the value is small enough just use lround(). */
      જો (exponent < BITS_PER_WORD-2) {
	    જો (is_neg) આ->invert();  // Invert the bits if negative.
	    long sval = lround(val);
	    જો (is_neg) sval = -sval;
	      /* This requires that 0 and 1 have the same bbit value. */
	    જો (size_ > BITS_PER_WORD) {
		  abits_ptr_[0] = sval;
	    } નહીંતર {
		  abits_val_ = sval;
	    }
	    પાછા;
      }

      unsigned nwords = (exponent-1) / BITS_PER_WORD;
      unsigned my_શબ્દો = (size_ + BITS_PER_WORD - 1) / BITS_PER_WORD - 1;

      fraction = ldexp(fraction, (exponent-1) % BITS_PER_WORD + 1);

	/* Skip any leading bits. */
      માટે (પૂર્ણાંક idx = (signed) nwords; idx > (signed) my_શબ્દો; idx -=1) {
	    unsigned long bits = (unsigned long) fraction;
	    fraction = fraction - (દ્વિગુણ) bits;
	    fraction = ldexp(fraction, BITS_PER_WORD);
      }

	/* Convert the remaining bits as appropriate. */
      જો (my_શબ્દો == 0) {
		  unsigned long bits = (unsigned long) fraction;
		  abits_val_ = bits;
		  fraction = fraction - (દ્વિગુણ) bits;
		    /* Round any fractional part up. */
		  જો (fraction >= 0.5) *આ += (int64_t) 1;
      } નહીંતર {
	    જો (nwords < my_શબ્દો) my_શબ્દો = nwords;
	    માટે (પૂર્ણાંક idx = (signed)my_શબ્દો; idx >= 0; idx -= 1) {
		  unsigned long bits = (unsigned long) fraction;
		  abits_ptr_[idx] = bits;
		  fraction = fraction - (દ્વિગુણ) bits;
		  fraction = ldexp(fraction, BITS_PER_WORD);
	    }
	      /* Round any fractional part up. */
	    જો (fraction >= ldexp(0.5, BITS_PER_WORD)) *આ += (int64_t) 1;
       }

	/* Convert to a negative number if needed. */
      જો (is_neg) {
	    આ->invert();
	    *આ += (int64_t) 1;
      }
}

vvp_vector4_t::vvp_vector4_t(અચલ vvp_vector4_t&that,
			    unsigned adr, unsigned wid)
{
	// Set up and initialize the destination.
      size_ = wid;
      allocate_શબ્દો(WORD_X_ABITS, WORD_X_BBITS);

	// Special case: selecting from far beyond the source vector,
	// to the result is all X bits. We're done.
      જો (adr >= that.size_)
	    પાછા;

	// Special case: The source is not quite big enough to supply
	// all bits, so get the bits that we can. The remainder will
	// be left at BIT4_X.
      જો ((adr + wid) > that.size_) {
	    unsigned use_wid = that.size_ - adr;
	    માટે (unsigned idx = 0 ; idx < use_wid ; idx += 1)
		  set_bit(idx, that.value(adr+idx));

	    પાછા;
      }

	// At the point, we know that the source part is entirely
	// contained in the source vector.
	// assert((adr + wid) <= that.size_);

      જો (wid > BITS_PER_WORD) {
	      /* In this case, the subvector and the source vector are
		 long. Do the transfer reasonably efficiently. */
	    unsigned ptr = adr / BITS_PER_WORD;
	    unsigned long off = adr % BITS_PER_WORD;
	    unsigned long noff = BITS_PER_WORD - off;
	    unsigned long lmask = (1UL << off) - 1UL;
	    unsigned trans = 0;
	    unsigned dst = 0;
	    જ્યારે (trans < wid) {
		    // The low bits of the result.
		  abits_ptr_[dst] = (that.abits_ptr_[ptr] & ~lmask) >> off;
		  bbits_ptr_[dst] = (that.bbits_ptr_[ptr] & ~lmask) >> off;
		  trans += noff;

		  જો (trans >= wid)
			તોડો;

		  ptr += 1;

		    // The high bits of the result. Skip this if the
		    // source and destination are perfectly aligned.
		  જો (noff != BITS_PER_WORD) {
			abits_ptr_[dst] |= (that.abits_ptr_[ptr]&lmask) << noff;
			bbits_ptr_[dst] |= (that.bbits_ptr_[ptr]&lmask) << noff;
			trans += off;
		  }

		  dst += 1;
	    }

      } નહીંતર જો (that.size_ > BITS_PER_WORD) {
	      /* In this case, the subvector fits in a single word,
		 but the source is large. */
	    unsigned ptr = adr / BITS_PER_WORD;
	    unsigned long off = adr % BITS_PER_WORD;
	    unsigned trans = BITS_PER_WORD - off;
	    જો (trans > wid)
		  trans = wid;

	    જો (trans == BITS_PER_WORD) {
		    // Very special case: Copy exactly 1 perfectly
		    // aligned word.
		  abits_val_ = that.abits_ptr_[ptr];
		  bbits_val_ = that.bbits_ptr_[ptr];

	    } નહીંતર {
		    // lmask is the low bits of the destination,
		    // masked into the source.
		  unsigned long lmask = (1UL<<trans) - 1UL;
		  lmask <<= off;

		    // The low bits of the result.
		  abits_val_ = (that.abits_ptr_[ptr] & lmask) >> off;
		  bbits_val_ = (that.bbits_ptr_[ptr] & lmask) >> off;

		  જો (trans < wid) {
			  // If there are more bits, then get them
			  // from the bottom of the next word of the
			  // source.
			unsigned long hmask = (1UL << (wid-trans)) - 1UL;

			  // The high bits of the result.
			abits_val_ |= (that.abits_ptr_[ptr+1]&hmask) << trans;
			bbits_val_ |= (that.bbits_ptr_[ptr+1]&hmask) << trans;
		  }
	    }

      } નહીંતર જો (size_ == BITS_PER_WORD) {
	      /* We know that source and destination are short. If the
		 destination is a full word, then we know the copy is
		 aligned and complete. */
	    abits_val_ = that.abits_val_;
	    bbits_val_ = that.bbits_val_;

      } નહીંતર {
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
શૂન્ય vvp_vector4_t::resize(unsigned newsize, vvp_bit4_t pad_bit)
{
      જો (size_ == newsize)
	    પાછા;

      unsigned cnt = (size_ + BITS_PER_WORD - 1) / BITS_PER_WORD;
      unsigned long word_pad_abits = 0;
      unsigned long word_pad_bbits = 0;
      પસંદગી (pad_bit) {
	  સ્થિતિ BIT4_0:
	    word_pad_abits = WORD_0_ABITS;
	    word_pad_bbits = WORD_0_BBITS;
	    તોડો;
	  સ્થિતિ BIT4_1:
	    word_pad_abits = WORD_1_ABITS;
	    word_pad_bbits = WORD_1_BBITS;
	    તોડો;
	  સ્થિતિ BIT4_X:
	    word_pad_abits = WORD_X_ABITS;
	    word_pad_bbits = WORD_X_BBITS;
	    તોડો;
	  સ્થિતિ BIT4_Z:
	    word_pad_abits = WORD_Z_ABITS;
	    word_pad_bbits = WORD_Z_BBITS;
	    તોડો;
      }

      જો (newsize > BITS_PER_WORD) {
	    unsigned newcnt = (newsize + BITS_PER_WORD - 1) / BITS_PER_WORD;
	    જો (newcnt == cnt) {
		    // If the word count doesn't change, then there is
		    // no need for re-allocation so we are done now.
		  જો (newsize > size_) {
			જો (unsigned fill = size_ % BITS_PER_WORD) {
			      abits_ptr_[cnt-1] &= ~((-1UL) << fill);
			      bbits_ptr_[cnt-1] &= ~((-1UL) << fill);
			      abits_ptr_[cnt-1] |= word_pad_abits << fill;
			      bbits_ptr_[cnt-1] |= word_pad_bbits << fill;
			}
		  }
		  size_ = newsize;
		  પાછા;
	    }

	    unsigned long*newbits = નવું unsigned long[2*newcnt];

	    જો (cnt > 1) {
		  unsigned trans = cnt;
		  જો (trans > newcnt)
			trans = newcnt;

		  માટે (unsigned idx = 0 ;  idx < trans ;  idx += 1)
			newbits[idx] = abits_ptr_[idx];
		  માટે (unsigned idx = 0 ;  idx < trans ;  idx += 1)
			newbits[newcnt+idx] = bbits_ptr_[idx];

		  delete[]abits_ptr_;

	    } નહીંતર {
		  newbits[0] = abits_val_;
		  newbits[newcnt] = bbits_val_;
	    }

	    જો (newsize > size_) {
		  જો (unsigned fill = size_ % BITS_PER_WORD) {
			newbits[cnt-1] &= ~((-1UL) << fill);
			newbits[cnt-1] |= word_pad_abits << fill;
			newbits[newcnt+cnt-1] &= ~((-1UL) << fill);
			newbits[newcnt+cnt-1] |= word_pad_bbits << fill;
		  }
		  માટે (unsigned idx = cnt ;  idx < newcnt ;  idx += 1)
			newbits[idx] = word_pad_abits;
		  માટે (unsigned idx = cnt ;  idx < newcnt ;  idx += 1)
			newbits[newcnt+idx] = word_pad_bbits;
	    }

	    size_ = newsize;
	    abits_ptr_ = newbits;
	    bbits_ptr_ = newbits + newcnt;

      } નહીંતર {
	    જો (cnt > 1) {
		  unsigned long newvala = abits_ptr_[0];
		  unsigned long newvalb = bbits_ptr_[0];
		  delete[]abits_ptr_;
		  abits_val_ = newvala;
		  bbits_val_ = newvalb;
	    }

	    જો (newsize > size_) {
		  abits_val_ &= ~((-1UL) << size_);
		  bbits_val_ &= ~((-1UL) << size_);
		  abits_val_ |= word_pad_abits << size_;
		  bbits_val_ |= word_pad_bbits << size_;
	    }

	    size_ = newsize;
      }
}


unsigned long* vvp_vector4_t::subarray(unsigned adr, unsigned wid, તાર્કિક xz_to_0) અચલ
{
      અચલ unsigned BIT2_PER_WORD = 8*કદ(unsigned long);
      unsigned awid = (wid + BIT2_PER_WORD - 1) / (BIT2_PER_WORD);
      unsigned long*val = નવું unsigned long[awid];

      માટે (unsigned idx = 0 ;  idx < awid ;  idx += 1)
	    val[idx] = 0;

      જો (size_ <= BITS_PER_WORD) {
	      /* Handle the special case that the array is small. The
		 entire value of the vector4 is within the xbits_val_
		 so we know that the result is a single word, the
		 source is a single word, and we just have to loop
		 through that word. */
	    unsigned long atmp = abits_val_ >> adr;
	    unsigned long btmp = bbits_val_ >> adr;
	    જો (wid < BIT2_PER_WORD) {
		  atmp &= (1UL << wid) - 1;
		  btmp &= (1UL << wid) - 1;
	    }
	    જો (btmp) {
		  જો (xz_to_0)
			atmp &= ~btmp;
		  નહીંતર
			જાઓ x_out;
	    }

	    val[0] = atmp;

      } નહીંતર {

	    unsigned val_ptr = 0;
	    unsigned val_off = 0;

	      /* Get the first word we are scanning. We may in fact be
		 somewhere in the middle of that word. */
	    જ્યારે (wid > 0) {
		  unsigned long atmp = abits_ptr_[adr/BITS_PER_WORD];
		  unsigned long btmp = bbits_ptr_[adr/BITS_PER_WORD];
		  unsigned long off = adr%BITS_PER_WORD;
		  atmp >>= off;
		  btmp >>= off;

		  unsigned long trans = BITS_PER_WORD - off;
		  જો (trans > (BIT2_PER_WORD - val_off))
			trans = BIT2_PER_WORD - val_off;
		  જો (wid < trans)
			trans = wid;
		  જો (trans < BIT2_PER_WORD) {
			atmp &= (1UL << trans) - 1;
			btmp &= (1UL << trans) - 1;
		  }
		  જો (btmp) {
			જો (xz_to_0)
			      atmp &= ~btmp;
			નહીંતર
			      જાઓ x_out;
		  }

		  val[val_ptr] |= atmp << val_off;
		  adr += trans;
		  wid -= trans;
		  val_off += trans;
		  જો (val_off == BIT2_PER_WORD) {
			val_ptr += 1;
			val_off = 0;
		  }
	    }
      }

      પાછા val;

 x_out:
      delete[]val;
      પાછા 0;
}

શૂન્ય vvp_vector4_t::setarray(unsigned adr, unsigned wid, અચલ unsigned long*val)
{
      પ્રતિજ્ઞા(adr+wid <= size_);

      અચલ unsigned BIT2_PER_WORD = 8*કદ(unsigned long);

      જો (size_ <= BITS_PER_WORD) {
	      // We know here that both the source and the target are
	      // within a single word. Write the bits into the
	      // abits_val_ directly.

	    પ્રતિજ્ઞા(BIT2_PER_WORD <= BITS_PER_WORD);
	    unsigned long lmask = (1UL << adr) - 1UL;
	    unsigned long hmask = ((adr+wid) < BITS_PER_WORD)
		  ? -1UL << (adr+wid)
		  : 0;
	    unsigned long mask = ~(hmask | lmask);

	    abits_val_ &= ~mask;
	    bbits_val_ &= ~mask;

	    abits_val_ |= mask & (val[0] << adr);

      } નહીંતર {
	      // The general case, there are multiple words of
	      // destination, and possibly multiple words of source
	      // data. Shift and mask as we go.
	    unsigned off = adr % BITS_PER_WORD;
	    unsigned ptr = adr / BITS_PER_WORD;
	    unsigned val_off = 0;
	    unsigned val_ptr = 0;
	    જ્યારે (wid > 0) {
		  unsigned trans = wid;
		  જો (trans > (BIT2_PER_WORD-val_off))
			trans = BIT2_PER_WORD-val_off;
		  જો (trans > (BITS_PER_WORD-off))
			trans = BITS_PER_WORD-off;

		  unsigned long lmask = (1UL << off) - 1UL;
		  unsigned long hmask = ((off+trans) < BITS_PER_WORD)
			? -1UL << (off+trans)
			: 0;
		  unsigned long mask = ~(hmask | lmask);

		  abits_ptr_[ptr] &= ~mask;
		  bbits_ptr_[ptr] &= ~mask;
		  જો (val_off >= off)
			abits_ptr_[ptr] |= mask & (val[val_ptr] >> (val_off-off));
		  નહીંતર
			abits_ptr_[ptr] |= mask & (val[val_ptr] << (off-val_off));

		  wid -= trans;
		  val_off += trans;
		  જો (val_off == BIT2_PER_WORD) {
			val_off = 0;
			val_ptr += 1;
		  }
		  off += trans;
		  જો (off == BITS_PER_WORD) {
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
તાર્કિક vvp_vector4_t::set_vec(unsigned adr, અચલ vvp_vector4_t&that)
{
      પ્રતિજ્ઞા(adr+that.size_  <= size_);
      તાર્કિક diff_flag = ખોટું;

      જો (size_ <= BITS_PER_WORD) {

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
	    જો (hshift >= BITS_PER_WORD)
		  hmask = -1UL;
	    નહીંતર
		  hmask = (1UL << (adr+that.size_)) - 1;
	    unsigned long mask = hmask & ~lmask;

	    unsigned long tmp = (that.abits_val_<<adr)&mask;
	    જો ((abits_val_&mask) != tmp) {
		  diff_flag = સાચું;
		  abits_val_ = (abits_val_ & ~mask) | tmp;
	    }
	    tmp = (that.bbits_val_<<adr) & mask;
	    જો ((bbits_val_&mask) != tmp) {
		  diff_flag = સાચું;
		  bbits_val_ = (bbits_val_ & ~mask) | tmp;
	    }

      } નહીંતર જો (that.size_ <= BITS_PER_WORD) {

	      /* This vector is more than a word, but that vector is
		 still small. Write into the destination, possibly
		 spanning two destination words, depending on whether
		 the source vector spans a word transition. */
	    unsigned long dptr = adr / BITS_PER_WORD;
	    unsigned long doff = adr % BITS_PER_WORD;

	    unsigned long lmask = (1UL << doff) - 1;
	    unsigned long hshift = doff+that.size_;
	    unsigned long hmask;
	    જો (hshift >= BITS_PER_WORD)
		  hmask = -1UL;
	    નહીંતર
		  hmask = (1UL << hshift) - 1UL;

	    unsigned long mask = hmask & ~lmask;
	    unsigned long tmp;

	    tmp = (that.abits_val_ << doff) & mask;
	    જો ((abits_ptr_[dptr] & mask) != tmp) {
		  diff_flag = સાચું;
		  abits_ptr_[dptr] = (abits_ptr_[dptr] & ~mask) | tmp;
	    }
	    tmp = (that.bbits_val_ << doff) & mask;
	    જો ((bbits_ptr_[dptr] & mask) != tmp) {
		  diff_flag = સાચું;
		  bbits_ptr_[dptr] = (bbits_ptr_[dptr] & ~mask) | tmp;
	    }

	    જો ((doff + that.size_) > BITS_PER_WORD) {
		  unsigned tail = doff + that.size_ - BITS_PER_WORD;
		  mask = (1UL << tail) - 1;

		  dptr += 1;
		  tmp = (that.abits_val_ >> (that.size_-tail)) & mask;
		  જો ((abits_ptr_[dptr] & mask) != tmp) {
			diff_flag = સાચું;
			abits_ptr_[dptr] = (abits_ptr_[dptr] & ~mask) | tmp;
		  }
		  tmp = (that.bbits_val_ >> (that.size_-tail)) & mask;
		  જો ((bbits_ptr_[dptr] & mask) != tmp) {
			diff_flag = સાચું;
			bbits_ptr_[dptr] = (bbits_ptr_[dptr] & ~mask) | tmp;
		  }
	    }

      } નહીંતર જો (adr%BITS_PER_WORD == 0) {

	      /* In this case, both vectors are long, but the
		 destination is neatly aligned. That means all but the
		 last word can be simply copied with no masking. */

	    unsigned remain = that.size_;
	    unsigned sptr = 0;
	    unsigned dptr = adr / BITS_PER_WORD;
	    જ્યારે (remain >= BITS_PER_WORD) {
		  જો (abits_ptr_[dptr] != that.abits_ptr_[sptr]) {
			diff_flag = સાચું;
			abits_ptr_[dptr] = that.abits_ptr_[sptr];
		  }
		  જો (bbits_ptr_[dptr] != that.bbits_ptr_[sptr]) {
			diff_flag = સાચું;
			bbits_ptr_[dptr] = that.bbits_ptr_[sptr];
		  }
		  dptr += 1;
		  sptr += 1;
		  remain -= BITS_PER_WORD;
	    }

	    જો (remain > 0) {
		  unsigned long mask = (1UL << remain) - 1;
		  unsigned long tmp;

		  tmp = that.abits_ptr_[sptr] & mask;
		  જો ((abits_ptr_[dptr] & mask) != tmp) {
			diff_flag = સાચું;
			abits_ptr_[dptr] = (abits_ptr_[dptr] & ~mask) | tmp;
		  }
		  tmp = that.bbits_ptr_[sptr] & mask;
		  જો ((bbits_ptr_[dptr] & mask) != tmp) {
			diff_flag = સાચું;
			bbits_ptr_[dptr] = (bbits_ptr_[dptr] & ~mask) | tmp;
		  }
	    }

      } નહીંતર {

	      /* We know that there are two long vectors, and we know
		 that the destination is definitely NOT aligned. */

	    unsigned remain = that.size_;
	    unsigned sptr = 0;
	    unsigned dptr = adr / BITS_PER_WORD;
	    unsigned doff = adr % BITS_PER_WORD;
	    unsigned long lmask = (1UL << doff) - 1;
	    unsigned ndoff = BITS_PER_WORD - doff;
	    જ્યારે (remain >= BITS_PER_WORD) {
		  unsigned long tmp;

		  tmp = (that.abits_ptr_[sptr] << doff) & ~lmask;
		  જો ((abits_ptr_[dptr] & ~lmask) != tmp) {
			diff_flag = સાચું;
			abits_ptr_[dptr] = (abits_ptr_[dptr] & lmask) | tmp;
		  }
		  tmp = (that.bbits_ptr_[sptr] << doff) & ~lmask;
		  જો ((bbits_ptr_[dptr] & ~lmask) != tmp) {
			diff_flag = સાચું;
			bbits_ptr_[dptr] = (bbits_ptr_[dptr] & lmask) | tmp;
		  }
		  dptr += 1;

		  tmp = (that.abits_ptr_[sptr] >> ndoff) & lmask;
		  જો ((abits_ptr_[dptr] & lmask) != tmp) {
			diff_flag = સાચું;
			abits_ptr_[dptr] = (abits_ptr_[dptr] & ~lmask) | tmp;
		  }
		  tmp = (that.bbits_ptr_[sptr] >> ndoff) & lmask;
		  જો ((bbits_ptr_[dptr] & lmask) != tmp) {
			diff_flag = સાચું;
			bbits_ptr_[dptr] = (bbits_ptr_[dptr] & ~lmask) | tmp;
		  }

		  remain -= BITS_PER_WORD;
		  sptr += 1;
	    }

	    જો (remain > 0) {
		  unsigned long hshift = doff+remain;
		  unsigned long hmask;
		  જો (hshift >= BITS_PER_WORD)
			hmask = -1UL;
		  નહીંતર
			hmask = (1UL << (doff+remain)) - 1;

		  unsigned long mask = hmask & ~lmask;
		  unsigned long tmp;

		  tmp = (that.abits_ptr_[sptr] << doff) & mask;
		  જો ((abits_ptr_[dptr] & mask) != tmp) {
			diff_flag = સાચું;
			abits_ptr_[dptr] = (abits_ptr_[dptr] & ~mask) | tmp;
		  }
		  tmp = (that.bbits_ptr_[sptr] << doff) & mask;
		  જો ((bbits_ptr_[dptr] & mask) != tmp) {
			diff_flag = સાચું;
			bbits_ptr_[dptr] = (bbits_ptr_[dptr] & ~mask) | tmp;
		  }

		  જો ((doff + remain) > BITS_PER_WORD) {
			unsigned tail = doff + remain - BITS_PER_WORD;
			જો (tail >= BITS_PER_WORD)
			      mask = -1UL;
			નહીંતર
			      mask = (1UL << tail) - 1;

			dptr += 1;

			tmp = (that.abits_ptr_[sptr] >> (remain-tail))&mask;
			જો ((abits_ptr_[dptr] & mask) != tmp) {
			      diff_flag = સાચું;
			      abits_ptr_[dptr] = (abits_ptr_[dptr] & ~mask) | tmp;
			}
			tmp = (that.bbits_ptr_[sptr] >> (remain-tail))&mask;
			જો ((bbits_ptr_[dptr] & mask) != tmp) {
			      diff_flag = સાચું;
			      bbits_ptr_[dptr] = (bbits_ptr_[dptr] & ~mask) | tmp;
			}
		  }
	    }
      }

      પાછા diff_flag;
}

/*
 * Add that vector to this vector. Do it in the Verilog way, which
 * means if we detect any X or Z bits, change the entire results to
 * all X.
 *
 * Assume both vectors are the same size.
 */
શૂન્ય vvp_vector4_t::add(અચલ vvp_vector4_t&that)
{
      પ્રતિજ્ઞા(size_ == that.size_);

      જો (size_ < BITS_PER_WORD) {
	    unsigned long mask = ~(-1UL << size_);
	    જો ((bbits_val_|that.bbits_val_) & mask) {
		  abits_val_ |= mask;
		  bbits_val_ |= mask;
		  પાછા;
	    }

	    abits_val_ += that.abits_val_;
	    abits_val_ &= mask;
	    પાછા;
      }

      જો (size_ == BITS_PER_WORD) {
	    જો (bbits_val_ | that.bbits_val_) {
		  abits_val_ = WORD_X_ABITS;
		  bbits_val_ = WORD_X_BBITS;
	    } નહીંતર {
		  abits_val_ += that.abits_val_;
	    }
	    પાછા;
      }

      પૂર્ણાંક cnt = size_ / BITS_PER_WORD;
      unsigned long carry = 0;
      માટે (પૂર્ણાંક idx = 0 ; idx < cnt ; idx += 1) {
	    જો (bbits_ptr_[idx] | that.bbits_ptr_[idx])
		  જાઓ x_out;

	    abits_ptr_[idx] = add_with_carry(abits_ptr_[idx], that.abits_ptr_[idx], carry);
      }

      જો (unsigned tail = size_ % BITS_PER_WORD) {
	    unsigned long mask = ~( -1UL << tail );
	    જો ((bbits_ptr_[cnt] | that.bbits_ptr_[cnt])&mask)
		  જાઓ x_out;

	    abits_ptr_[cnt] = add_with_carry(abits_ptr_[cnt], that.abits_ptr_[cnt], carry);
	    abits_ptr_[cnt] &= mask;
      }

      પાછા;

 x_out:
      માટે (પૂર્ણાંક idx = 0 ; idx < cnt ; idx += 1) {
	    abits_ptr_[idx] = WORD_X_ABITS;
	    bbits_ptr_[idx] = WORD_X_BBITS;
      }
      જો (unsigned tail = size_%BITS_PER_WORD) {
	    unsigned long mask = ~( -1UL << tail );
	    abits_ptr_[cnt] = WORD_X_ABITS&mask;
	    bbits_ptr_[cnt] = WORD_X_BBITS&mask;
      }
}

શૂન્ય vvp_vector4_t::sub(અચલ vvp_vector4_t&that)
{
      પ્રતિજ્ઞા(size_ == that.size_);

      જો (size_ < BITS_PER_WORD) {
	    unsigned long mask = ~(-1UL << size_);
	    જો ((bbits_val_|that.bbits_val_) & mask) {
		  abits_val_ |= mask;
		  bbits_val_ |= mask;
		  પાછા;
	    }

	    abits_val_ -= that.abits_val_;
	    abits_val_ &= mask;
	    પાછા;
      }

      જો (size_ == BITS_PER_WORD) {
	    જો (bbits_val_ | that.bbits_val_) {
		  abits_val_ = WORD_X_ABITS;
		  bbits_val_ = WORD_X_BBITS;
	    } નહીંતર {
		  abits_val_ -= that.abits_val_;
	    }
	    પાછા;
      }

      પૂર્ણાંક cnt = size_ / BITS_PER_WORD;
      unsigned long carry = 1;
      માટે (પૂર્ણાંક idx = 0 ; idx < cnt ; idx += 1) {
	    જો (bbits_ptr_[idx] | that.bbits_ptr_[idx])
		  જાઓ x_out;

	    abits_ptr_[idx] = add_with_carry(abits_ptr_[idx], ~that.abits_ptr_[idx], carry);
      }

      જો (unsigned tail = size_ % BITS_PER_WORD) {
	    unsigned long mask = ~( -1UL << tail );
	    જો ((bbits_ptr_[cnt] | that.bbits_ptr_[cnt])&mask)
		  જાઓ x_out;

	    abits_ptr_[cnt] = add_with_carry(abits_ptr_[cnt], ~that.abits_ptr_[cnt], carry);
	    abits_ptr_[cnt] &= mask;
      }

      પાછા;

 x_out:
      માટે (પૂર્ણાંક idx = 0 ; idx < cnt ; idx += 1) {
	    abits_ptr_[idx] = WORD_X_ABITS;
	    bbits_ptr_[idx] = WORD_X_BBITS;
      }
      જો (unsigned tail = size_%BITS_PER_WORD) {
	    unsigned long mask = ~( -1UL << tail );
	    abits_ptr_[cnt] = WORD_X_ABITS&mask;
	    bbits_ptr_[cnt] = WORD_X_BBITS&mask;
      }

}

શૂન્ય vvp_vector4_t::mov(unsigned dst, unsigned src, unsigned cnt)
{
      પ્રતિજ્ઞા(dst+cnt <= size_);
      પ્રતિજ્ઞા(src+cnt <= size_);

      જો (size_ <= BITS_PER_WORD) {
	    unsigned long vmask = (1UL << cnt) - 1;
	    unsigned long tmp;

	    tmp = (abits_val_ >> src) & vmask;
	    abits_val_ &= ~ (vmask << dst);
	    abits_val_ |= tmp << dst;

	    tmp = (bbits_val_ >> src) & vmask;
	    bbits_val_ &= ~ (vmask << dst);
	    bbits_val_ |= tmp << dst;

      } નહીંતર {
	    unsigned sptr = src / BITS_PER_WORD;
	    unsigned dptr = dst / BITS_PER_WORD;
	    unsigned soff = src % BITS_PER_WORD;
	    unsigned doff = dst % BITS_PER_WORD;

	    જ્યારે (cnt > 0) {
		  unsigned trans = cnt;
		  જો ((soff+trans) > BITS_PER_WORD)
			trans = BITS_PER_WORD - soff;

		  જો ((doff+trans) > BITS_PER_WORD)
			trans = BITS_PER_WORD - doff;

		  જો (trans == BITS_PER_WORD) {
			  // Special case: the transfer count is
			  // exactly an entire word. For this to be
			  // true, it must also be true that the
			  // pointers are aligned. The work is easy,
			abits_ptr_[dptr] = abits_ptr_[sptr];
			bbits_ptr_[dptr] = bbits_ptr_[sptr];
			dptr += 1;
			sptr += 1;
			cnt -= BITS_PER_WORD;
			ચાલુ;
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
		  જો (soff >= BITS_PER_WORD) {
			soff = 0;
			sptr += 1;
		  }
		  doff += trans;
		  જો (doff >= BITS_PER_WORD) {
			doff = 0;
			dptr += 1;
		  }
	    }
      }
}

શૂન્ય vvp_vector4_t::mul(અચલ vvp_vector4_t&that)
{
      પ્રતિજ્ઞા(size_ == that.size_);

      જો (size_ < BITS_PER_WORD) {
	    unsigned long mask = ~(-1UL << size_);
	    જો ((bbits_val_|that.bbits_val_) & mask) {
		  abits_val_ |= mask;
		  bbits_val_ |= mask;
		  પાછા;
	    }

	    abits_val_ *= that.abits_val_;
	    abits_val_ &= mask;
	    પાછા;
      }

      જો (size_ == BITS_PER_WORD) {
	    જો (bbits_val_ || that.bbits_val_) {
		  abits_val_ = WORD_X_ABITS;
		  bbits_val_ = WORD_X_BBITS;
	    } નહીંતર {
		  abits_val_ *= that.abits_val_;
	    }
	    પાછા;
      }

      અચલ પૂર્ણાંક cnt = (size_+BITS_PER_WORD-1) / BITS_PER_WORD;

      unsigned long mask;
      જો (unsigned tail = size_%BITS_PER_WORD) {
	    mask = ~( -1UL << tail );
      } નહીંતર {
	    mask = ~0UL;
      }

	// Check for any XZ values ahead of time in a first pass. If
	// we find any, then force the entire result to be X and be
	// done.
      માટે (પૂર્ણાંક idx = 0 ; idx < cnt ; idx += 1) {
	    unsigned long lval = bbits_ptr_[idx];
	    unsigned long rval = that.bbits_ptr_[idx];
	    જો (idx == (cnt-1)) {
		  lval &= mask;
		  rval &= mask;
	    }
	    જો (lval || rval) {
		  માટે (પૂર્ણાંક xdx = 0 ; xdx < cnt-1 ; xdx += 1) {
			abits_ptr_[xdx] = WORD_X_ABITS;
			bbits_ptr_[xdx] = WORD_X_BBITS;
		  }
		  abits_ptr_[cnt-1] = WORD_X_ABITS & mask;
		  bbits_ptr_[cnt-1] = WORD_X_BBITS & mask;
		  પાછા;
	    }
      }

	// Calculate the result into a res array. We need to keep is
	// separate from the "this" array because we are making
	// multiple passes.
      unsigned long*res = નવું unsigned long[cnt];
      માટે (પૂર્ણાંક idx = 0 ; idx < cnt ; idx += 1)
	    res[idx] = 0;

      માટે (પૂર્ણાંક mul_a = 0 ; mul_a < cnt ; mul_a += 1) {
	    unsigned long lval = abits_ptr_[mul_a];
	    જો (mul_a == (cnt-1))
		  lval &= mask;

	    માટે (પૂર્ણાંક mul_b = 0 ; mul_b < (cnt-mul_a) ; mul_b += 1) {
		  unsigned long rval = that.abits_ptr_[mul_b];
		  જો (mul_b == (cnt-1))
			rval &= mask;

		  unsigned long sum;
		  unsigned long tmp = multiply_with_carry(lval, rval, sum);
		  પૂર્ણાંક base = mul_a + mul_b;
		  unsigned long carry = 0;
		  res[base] = add_with_carry(res[base], tmp, carry);
		  માટે (પૂર્ણાંક add_idx = base+1 ; add_idx < cnt ; add_idx += 1) {
			res[add_idx] = add_with_carry(res[add_idx], sum, carry);
			sum = 0;
		  }
	    }
      }

	// Replace the "this" value with the calculated result. We
	// know a-priori that the bbits are zero and unchanged.
      res[cnt-1] &= mask;
      માટે (પૂર્ણાંક idx = 0 ; idx < cnt ; idx += 1)
	    abits_ptr_[idx] = res[idx];

      delete[]res;
      પાછા;


}

તાર્કિક vvp_vector4_t::eeq(અચલ vvp_vector4_t&that) અચલ
{
      જો (size_ != that.size_)
	    પાછા ખોટું;

      જો (size_ < BITS_PER_WORD) {
	    unsigned long mask = (1UL << size_) - 1;
	    પાછા (abits_val_&mask) == (that.abits_val_&mask)
		  && (bbits_val_&mask) == (that.bbits_val_&mask);
      }

      જો (size_ == BITS_PER_WORD) {
	    પાછા (abits_val_ == that.abits_val_)
		  && (bbits_val_ == that.bbits_val_);
      }

      unsigned શબ્દો = size_ / BITS_PER_WORD;
      માટે (unsigned idx = 0 ;  idx < શબ્દો ;  idx += 1) {
	    જો (abits_ptr_[idx] != that.abits_ptr_[idx])
		  પાછા ખોટું;
	    જો (bbits_ptr_[idx] != that.bbits_ptr_[idx])
		  પાછા ખોટું;
      }

      unsigned long mask = size_%BITS_PER_WORD;
      જો (mask > 0) {
	    mask = (1UL << mask) - 1;
	    પાછા (abits_ptr_[શબ્દો]&mask) == (that.abits_ptr_[શબ્દો]&mask)
		  && (bbits_ptr_[શબ્દો]&mask) == (that.bbits_ptr_[શબ્દો]&mask);
      }

      પાછા સાચું;
}

તાર્કિક vvp_vector4_t::eq_xz(અચલ vvp_vector4_t&that) અચલ
{
      જો (size_ != that.size_)
	    પાછા ખોટું;

      જો (size_ < BITS_PER_WORD) {
	    unsigned long mask = (1UL << size_) - 1;
	    પાછા ((abits_val_|bbits_val_)&mask) == ((that.abits_val_|that.bbits_val_)&mask)
		  && (bbits_val_&mask) == (that.bbits_val_&mask);
      }

      જો (size_ == BITS_PER_WORD) {
	    પાછા ((abits_val_|bbits_val_) == (that.abits_val_|that.bbits_val_))
		  && (bbits_val_ == that.bbits_val_);
      }

      unsigned શબ્દો = size_ / BITS_PER_WORD;
      માટે (unsigned idx = 0 ;  idx < શબ્દો ;  idx += 1) {
	    જો ((abits_ptr_[idx]|bbits_ptr_[idx]) != (that.abits_ptr_[idx]|that.bbits_ptr_[idx]))
		  પાછા ખોટું;
	    જો (bbits_ptr_[idx] != that.bbits_ptr_[idx])
		  પાછા ખોટું;
      }

      unsigned long mask = size_%BITS_PER_WORD;
      જો (mask > 0) {
	    mask = (1UL << mask) - 1;
	    પાછા ((abits_ptr_[શબ્દો]|bbits_ptr_[શબ્દો])&mask) == ((that.abits_ptr_[શબ્દો]|that.bbits_ptr_[શબ્દો])&mask)
		  && (bbits_ptr_[શબ્દો]&mask) == (that.bbits_ptr_[શબ્દો]&mask);
      }

      પાછા સાચું;
}

તાર્કિક vvp_vector4_t::has_xz() અચલ
{
      જો (size_ < BITS_PER_WORD) {
	    unsigned long mask = -1UL >> (BITS_PER_WORD - size_);
	    પાછા bbits_val_&mask;
      }

      જો (size_ == BITS_PER_WORD) {
	    પાછા bbits_val_;
      }

      unsigned શબ્દો = size_ / BITS_PER_WORD;
      માટે (unsigned idx = 0 ; idx < શબ્દો ; idx += 1) {
	    જો (bbits_ptr_[idx])
		  પાછા સાચું;
      }

      unsigned long mask = size_%BITS_PER_WORD;
      જો (mask > 0) {
	    mask = -1UL >> (BITS_PER_WORD - mask);
	    પાછા bbits_ptr_[શબ્દો]&mask;
      }

      પાછા ખોટું;
}

શૂન્ય vvp_vector4_t::change_z2x()
{
	// This method relies on the fact that both BIT4_X and BIT4_Z
	// have the bbit set in the vector4 encoding, and also that
	// the BIT4_X has abit set in the vector4 encoding. By simply
	// or-ing the bbit into the abit, BIT4_X and BIT4_Z both
	// become BIT4_X.

      જો (size_ <= BITS_PER_WORD) {
	    abits_val_ |= bbits_val_;
      } નહીંતર {
	    unsigned શબ્દો = (size_+BITS_PER_WORD-1) / BITS_PER_WORD;
	    માટે (unsigned idx = 0 ;  idx < શબ્દો ;  idx += 1)
		  abits_ptr_[idx] |= bbits_ptr_[idx];
      }
}

શૂન્ય vvp_vector4_t::fill_bits(vvp_bit4_t bit)
{
	/* note: this relies on the bit encoding for the vvp_bit4_t. */
      સ્થિર અચલ unsigned long init_atable[4] = {
	    WORD_0_ABITS,
	    WORD_1_ABITS,
	    WORD_Z_ABITS,
	    WORD_X_ABITS };
      સ્થિર અચલ unsigned long init_btable[4] = {
	    WORD_0_BBITS,
	    WORD_1_BBITS,
	    WORD_Z_BBITS,
	    WORD_X_BBITS };

      જો (size_ <= BITS_PER_WORD) {
	    abits_val_ = init_atable[bit];
	    bbits_val_ = init_btable[bit];
      } નહીંતર {
	    unsigned શબ્દો = (size_ + BITS_PER_WORD - 1) / BITS_PER_WORD;
	    માટે (unsigned idx = 0; idx < શબ્દો; idx += 1) {
		  abits_ptr_[idx] = init_atable[bit];
		  bbits_ptr_[idx] = init_btable[bit];
	    }
      }
}

અક્ષર* vvp_vector4_t::as_string(અક્ષર*buf, size_t buf_len) અચલ
{
      અક્ષર*res = buf;
      *buf++ = 'C';
      *buf++ = '4';
      *buf++ = '<';
      buf_len -= 3;

      માટે (unsigned idx = 0 ;  idx < size() && buf_len >= 2 ;  idx += 1) {
	    પસંદગી (value(size()-idx-1)) {
		સ્થિતિ BIT4_0:
		  *buf++ = '0';
		  તોડો;
		સ્થિતિ BIT4_1:
		  *buf++ = '1';
		  તોડો;
		સ્થિતિ BIT4_X:
		  *buf++ = 'x';
		  તોડો;
		સ્થિતિ BIT4_Z:
		  *buf++ = 'z';
	    }
	    buf_len -= 1;
      }

      *buf++ = '>';
      *buf++ = 0;
      પાછા res;
}

શૂન્ય vvp_vector4_t::invert()
{
      જો (size_ <= BITS_PER_WORD) {
	    unsigned long mask = (size_<BITS_PER_WORD)? (1UL<<size_)-1UL : -1UL;
	    abits_val_ = mask & ~abits_val_;
	    abits_val_ |= bbits_val_;
      } નહીંતર {
	    unsigned remaining = size_;
	    unsigned idx = 0;
	    જ્યારે (remaining >= BITS_PER_WORD) {
		  abits_ptr_[idx] = ~abits_ptr_[idx];
		  abits_ptr_[idx] |= bbits_ptr_[idx];
		  idx += 1;
		  remaining -= BITS_PER_WORD;
	    }
	    જો (remaining > 0) {
		  unsigned long mask = (1UL<<remaining) - 1UL;
		  abits_ptr_[idx] = mask & ~abits_ptr_[idx];
		  abits_ptr_[idx] |= bbits_ptr_[idx];
	    }
      }
}

#define BIT_MASK(n) ((n) ? ((~0UL) >> (BITS_PER_WORD - (n))) : ~0UL)

vvp_bit4_t vvp_vector4_t::reduce_or() અચલ
{
      unsigned long mask = BIT_MASK(size_ % BITS_PER_WORD);
      vvp_bit4_t res = BIT4_0;

      જો (size_ <= BITS_PER_WORD) {
	    જો ((abits_val_ & ~bbits_val_ & mask) != 0UL)
		  પાછા BIT4_1;
	    જો ((bbits_val_ & mask) != 0UL)
		  પાછા BIT4_X;
      } નહીંતર {
	    unsigned શબ્દો = (size_ + BITS_PER_WORD - 1) / BITS_PER_WORD;
	    unsigned idx;
	    માટે (idx = 0; idx < શબ્દો - 1; idx += 1) {
		  જો ((abits_ptr_[idx] & ~bbits_ptr_[idx]) != 0UL)
			પાછા BIT4_1;
		  જો (bbits_ptr_[idx] != 0UL)
			res = BIT4_X;
	    }
	    જો ((abits_ptr_[idx] & ~bbits_ptr_[idx] & mask) != 0UL)
		  પાછા BIT4_1;
	    જો ((bbits_ptr_[idx] & mask) != 0UL)
		  res = BIT4_X;
      }

      પાછા res;
}

vvp_bit4_t vvp_vector4_t::reduce_and() અચલ
{
      unsigned long mask = BIT_MASK(size_ % BITS_PER_WORD);
      vvp_bit4_t res = BIT4_1;

      જો (size_ <= BITS_PER_WORD) {
	    જો ((abits_val_ | bbits_val_ | ~mask) != ~0UL)
		  પાછા BIT4_0;
	    જો ((bbits_val_ & mask) != 0UL)
		  પાછા BIT4_X;
	} નહીંતર {
	    unsigned શબ્દો = (size_ + BITS_PER_WORD - 1) / BITS_PER_WORD;
	    unsigned idx;
	    માટે (idx = 0; idx < શબ્દો - 1; idx += 1) {
		  જો ((abits_ptr_[idx] | bbits_ptr_[idx]) != ~0UL)
			પાછા BIT4_0;
		  જો (bbits_ptr_[idx] != 0UL)
			res = BIT4_X;
	    }
	    જો ((abits_ptr_[idx] | bbits_ptr_[idx] | ~mask) != ~0UL)
		  પાછા BIT4_0;
	    જો ((bbits_ptr_[idx] & mask) != 0UL)
		  res = BIT4_X;
      }

      પાછા res;
}

સ્થિર unsigned long parity(unsigned long val)
{
#જો defined(__GNUC__)
      // The compiler builtin can use target-specific CPU instructions.
      પાછા __builtin_parityl(val);
#નહીંતર
#જો ULONG_MAX > 0xffffffffUL
	val ^= val >> 32;
#endif
	val ^= val >> 16;
	val ^= val >> 8;
	val ^= val >> 4;

      પાછા (0x6996 >> (val & 0xf)) & 1;
#endif
}

vvp_bit4_t vvp_vector4_t::reduce_xor() અચલ
{
      unsigned long mask = BIT_MASK(size_ % BITS_PER_WORD);

      જો (size_ <= BITS_PER_WORD) {
	    જો ((bbits_val_ & mask) != 0UL)
		  પાછા BIT4_X;
	    પાછા parity(abits_val_ & mask) ? BIT4_1 : BIT4_0;
      } નહીંતર {
	    unsigned શબ્દો = (size_ + BITS_PER_WORD - 1) / BITS_PER_WORD;
	    unsigned long val_a = 0UL;
	    unsigned idx;
	    માટે (idx = 0; idx < શબ્દો - 1; idx += 1) {
		  જો (bbits_ptr_[idx] != 0UL)
			પાછા BIT4_X;
		  val_a ^= abits_ptr_[idx];
	    }
	    જો ((bbits_ptr_[idx] & mask) != 0UL)
		  પાછા BIT4_X;
	    val_a ^= abits_ptr_[idx] & mask;
	    પાછા parity(val_a) ? BIT4_1 : BIT4_0;
      }
}

vvp_vector4_t& vvp_vector4_t::operator &= (અચલ vvp_vector4_t&that)
{
	// The truth table is:
	//     00 01 11 10
	//  00 00 00 00 00
	//  01 00 01 11 11
	//  11 00 11 11 11
	//  10 00 11 11 11
      જો (size_ <= BITS_PER_WORD) {
	    unsigned long tmp1 = abits_val_ | bbits_val_;
	    unsigned long tmp2 = that.abits_val_ | that.bbits_val_;
	    abits_val_ = tmp1 & tmp2;
	    bbits_val_ = (tmp1 & that.bbits_val_) | (tmp2 & bbits_val_);
      } નહીંતર {
	    unsigned શબ્દો = (size_ + BITS_PER_WORD - 1) / BITS_PER_WORD;
	    માટે (unsigned idx = 0; idx < શબ્દો ; idx += 1) {
		  unsigned long tmp1 = abits_ptr_[idx] | bbits_ptr_[idx];
		  unsigned long tmp2 = that.abits_ptr_[idx] |
		                       that.bbits_ptr_[idx];
		  abits_ptr_[idx] = tmp1 & tmp2;
		  bbits_ptr_[idx] = (tmp1 & that.bbits_ptr_[idx]) |
		                    (tmp2 & bbits_ptr_[idx]);
	    }
      }

      પાછા *આ;
}

vvp_vector4_t& vvp_vector4_t::operator |= (અચલ vvp_vector4_t&that)
{
	// The truth table is:
	//     00 01 11 10
	//  00 00 01 11 11
	//  01 01 01 01 01
	//  11 11 01 11 11
	//  10 11 01 11 11
      જો (size_ <= BITS_PER_WORD) {
	    unsigned long tmp = abits_val_ | bbits_val_ |
	                        that.abits_val_ | that.bbits_val_;
	    bbits_val_ = ((~abits_val_ | bbits_val_) & that.bbits_val_) |
	                 ((~that.abits_val_ | that.bbits_val_) & bbits_val_);
	    abits_val_ = tmp;

      } નહીંતર {
	    unsigned શબ્દો = (size_ + BITS_PER_WORD - 1) / BITS_PER_WORD;
	    માટે (unsigned idx = 0; idx < શબ્દો ; idx += 1) {
		  unsigned long tmp = abits_ptr_[idx] | bbits_ptr_[idx] |
	                        that.abits_ptr_[idx] | that.bbits_ptr_[idx];
		  bbits_ptr_[idx] = ((~abits_ptr_[idx] | bbits_ptr_[idx]) &
		                     that.bbits_ptr_[idx]) |
		                    ((~that.abits_ptr_[idx] |
		                      that.bbits_ptr_[idx]) & bbits_ptr_[idx]);
		  abits_ptr_[idx] = tmp;
	    }
      }

      પાછા *આ;
}

vvp_vector4_t& vvp_vector4_t::operator ^= (અચલ vvp_vector4_t&that)
{
	// The truth table is:
	//     00 01 11 10
	//  00 00 01 11 11
	//  01 01 00 11 11
	//  11 11 11 11 11
	//  10 11 11 11 11
      જો (size_ <= BITS_PER_WORD) {
	    unsigned long bval = bbits_val_ | that.bbits_val_;
	    bbits_val_ = bval;
	    abits_val_ = (abits_val_ ^ that.abits_val_) | bval;
      } નહીંતર {
	    unsigned શબ્દો = (size_ + BITS_PER_WORD - 1) / BITS_PER_WORD;
	    માટે (unsigned idx = 0; idx < શબ્દો ; idx += 1) {
		  unsigned long bval = bbits_ptr_[idx] | that.bbits_ptr_[idx];
		  bbits_ptr_[idx] = bval;
		  abits_ptr_[idx] = (abits_ptr_[idx] ^ that.abits_ptr_[idx]) | bval;
	    }
      }

      પાછા *આ;
}

/*
* Add an integer to the vvp_vector4_t in place, bit by bit so that
* there is no size limitations.
*/
vvp_vector4_t& vvp_vector4_t::operator += (int64_t that)
{
      vvp_bit4_t carry = BIT4_0;
      unsigned idx;

      જો (has_xz()) {
	    vvp_vector4_t xxx (size(), BIT4_X);
	    *આ = xxx;
	    પાછા *આ;
      }

      માટે (idx = 0 ; idx < size() ; idx += 1) {
	    જો (that == 0 && carry==BIT4_0)
		  તોડો;

	    vvp_bit4_t that_bit = (that&1)? BIT4_1 : BIT4_0;
	    that >>= 1;

	    જો (that_bit==BIT4_0 && carry==BIT4_0)
		  ચાલુ;

	    vvp_bit4_t bit = value(idx);
	    bit = add_with_carry(bit, that_bit, carry);

	    set_bit(idx, bit);
      }

      પાછા *આ;
}

ostream& operator<< (ostream&out, અચલ vvp_vector4_t&that)
{
      out << that.size() << "'b";
      માટે (unsigned idx = 0 ;  idx < that.size() ;  idx += 1)
	    out << that.value(that.size()-idx-1);
      પાછા out;
}

template <વર્ગ INT>તાર્કિક vector4_to_કિંમત(અચલ vvp_vector4_t&vec, INT&val,
					  તાર્કિક is_signed, તાર્કિક is_arithmetic)
{
      INT res = 0;
      INT msk = 1;
      તાર્કિક rc_flag = સાચું;

      unsigned size = vec.size();
      જો (size > 8*કદ(val)) size = 8*કદ(val);
      માટે (unsigned idx = 0 ;  idx < size ;  idx += 1) {
	    પસંદગી (vec.value(idx)) {
		સ્થિતિ BIT4_0:
		  તોડો;
		સ્થિતિ BIT4_1:
		  res |= msk;
		  તોડો;
		મૂળભૂત:
		  જો (is_arithmetic)
			પાછા ખોટું;
		  નહીંતર
			rc_flag = ખોટું;
	    }

	    msk <<= 1;
      }

      જો (is_signed && vec.value(vec.size()-1) == BIT4_1) {
	    જો (vec.size() < 8*કદ(val))
		  res |= static_cast<INT>(-1ULL << vec.size());
      }

      val = res;
      પાછા rc_flag;
}

template તાર્કિક vector4_to_કિંમત(અચલ vvp_vector4_t&vec, int8_t&val,
			       તાર્કિક is_signed, તાર્કિક is_arithmetic);
template તાર્કિક vector4_to_કિંમત(અચલ vvp_vector4_t&vec, int16_t&val,
			       તાર્કિક is_signed, તાર્કિક is_arithmetic);
template તાર્કિક vector4_to_કિંમત(અચલ vvp_vector4_t&vec, int32_t&val,
			       તાર્કિક is_signed, તાર્કિક is_arithmetic);
template તાર્કિક vector4_to_કિંમત(અચલ vvp_vector4_t&vec, int64_t&val,
			       તાર્કિક is_signed, તાર્કિક is_arithmetic);
template તાર્કિક vector4_to_કિંમત(અચલ vvp_vector4_t&vec, uint8_t&val,
			       તાર્કિક is_signed, તાર્કિક is_arithmetic);
template તાર્કિક vector4_to_કિંમત(અચલ vvp_vector4_t&vec, uint16_t&val,
			       તાર્કિક is_signed, તાર્કિક is_arithmetic);
template તાર્કિક vector4_to_કિંમત(અચલ vvp_vector4_t&vec, uint32_t&val,
			       તાર્કિક is_signed, તાર્કિક is_arithmetic);
template તાર્કિક vector4_to_કિંમત(અચલ vvp_vector4_t&vec, uint64_t&val,
			       તાર્કિક is_signed, તાર્કિક is_arithmetic);

template <વર્ગ T> તાર્કિક vector4_to_કિંમત(અચલ vvp_vector4_t&vec,
                                         તાર્કિક&overflow_flag, T&val)
{
      T res = 0;
      T msk = 1;

      overflow_flag = ખોટું;
      unsigned size = vec.size();
      માટે (unsigned idx = 0 ;  idx < size ;  idx += 1) {
	    પસંદગી (vec.value(idx)) {
		સ્થિતિ BIT4_0:
		  તોડો;
		સ્થિતિ BIT4_1:
		  જો (msk == 0)
			overflow_flag = સાચું;
		  નહીંતર
			res |= msk;
		  તોડો;
		મૂળભૂત:
		  પાછા ખોટું;
	    }

	    msk <<= static_cast<T>(1);
      }

      val = res;
      પાછા સાચું;
}

template તાર્કિક vector4_to_કિંમત(અચલ vvp_vector4_t&vec, તાર્કિક&overflow_flag,
                               unsigned long&val);
#ifndef UL_AND_TIME64_SAME
template તાર્કિક vector4_to_કિંમત(અચલ vvp_vector4_t&vec, તાર્કિક&overflow_flag,
                               vvp_time64_t&val);
#endif

તાર્કિક vector4_to_કિંમત(અચલ vvp_vector4_t&vec, દ્વિગુણ&val, તાર્કિક signed_flag)
{

      જો (vec.size() == 0) {
	    val = 0.0;
	    પાછા સાચું;
      }

      તાર્કિક flag = સાચું;

      જો (vec.value(vec.size()-1) != BIT4_1) {
	    signed_flag = ખોટું;
      }

      દ્વિગુણ res = 0.0;
      જો (signed_flag) {
	    vvp_bit4_t carry = BIT4_1;
	    માટે (unsigned idx = 0 ;  idx < vec.size() ;  idx += 1) {
		  vvp_bit4_t a = ~vec.value(idx);
		  vvp_bit4_t x = add_with_carry(a, BIT4_0, carry);
		  પસંદગી (x) {
		      સ્થિતિ BIT4_0:
			તોડો;
		      સ્થિતિ BIT4_1:
			res += pow(2.0, (પૂર્ણાંક)idx);
			તોડો;
		      મૂળભૂત:
			flag = ખોટું;
		  }
	    }
	    res *= -1.0;
      } નહીંતર {
	    માટે (unsigned idx = 0 ;  idx < vec.size() ;  idx += 1) {
		  પસંદગી (vec.value(idx)) {
		      સ્થિતિ BIT4_0:
			તોડો;
		      સ્થિતિ BIT4_1:
			res += pow(2.0, (પૂર્ણાંક)idx);
			તોડો;
		      મૂળભૂત:
			flag = ખોટું;
		  }
	    }
      }
      val = res;
      પાછા flag;
}

તાર્કિક vector2_to_કિંમત(અચલ vvp_vector2_t&a, int32_t&val, તાર્કિક is_signed)
{
      val = 0;
      unsigned idx;
      uint32_t mask;
      માટે (idx = 0, mask = 1 ; idx < a.size() && idx < 32 ; idx += 1, mask <<= 1) {
	    જો (a.value(idx)) val |= mask;
      }

      જો (is_signed && a.size() < 32 && a.value(a.size()-1)) {
	    mask = -1;
	    mask <<= a.size();
	    val |= mask;
      }

      પાછા a.size() <= 32;
}

vvp_vector4array_t::vvp_vector4array_t(unsigned width__, unsigned શબ્દો_2)
: width_(width__), શબ્દો_3(શબ્દો_2)
{
}

vvp_vector4array_t::~vvp_vector4array_t()
{
}

શૂન્ય vvp_vector4array_t::set_word_(v4cell*cell, અચલ vvp_vector4_t&that)
{
      પ્રતિજ્ઞા(that.size_ == width_);

      જો (width_ <= vvp_vector4_t::BITS_PER_WORD) {
	    cell->abits_val_ = that.abits_val_;
	    cell->bbits_val_ = that.bbits_val_;
	    પાછા;
      }

      unsigned cnt = (width_ + vvp_vector4_t::BITS_PER_WORD-1)/vvp_vector4_t::BITS_PER_WORD;

      જો (cell->abits_ptr_ == 0) {
	    cell->abits_ptr_ = નવું unsigned long[2*cnt];
	    cell->bbits_ptr_ = cell->abits_ptr_ + cnt;
      }

      માટે (unsigned idx = 0 ; idx < cnt ; idx += 1)
	    cell->abits_ptr_[idx] = that.abits_ptr_[idx];
      માટે (unsigned idx = 0 ; idx < cnt ; idx += 1)
	    cell->bbits_ptr_[idx] = that.bbits_ptr_[idx];
}

vvp_vector4_t vvp_vector4array_t::get_word_(v4cell*cell) અચલ
{
      જો (width_ <= vvp_vector4_t::BITS_PER_WORD) {
	    vvp_vector4_t res;
	    res.size_ = width_;
	    res.abits_val_ = cell->abits_val_;
	    res.bbits_val_ = cell->bbits_val_;
	    પાછા res;
      }

      vvp_vector4_t res (width_, BIT4_X);
      જો (cell->abits_ptr_ == 0)
	    પાછા res;

      unsigned cnt = (width_ + vvp_vector4_t::BITS_PER_WORD-1)/vvp_vector4_t::BITS_PER_WORD;

      માટે (unsigned idx = 0 ; idx < cnt ; idx += 1)
	    res.abits_ptr_[idx] = cell->abits_ptr_[idx];
      માટે (unsigned idx = 0 ; idx < cnt ; idx += 1)
	    res.bbits_ptr_[idx] = cell->bbits_ptr_[idx];

      પાછા res;
}

vvp_vector4array_sa::vvp_vector4array_sa(unsigned width__, unsigned શબ્દો_2)
: vvp_vector4array_t(width__, શબ્દો_2)
{
      array_ = નવું v4cell[શબ્દો_3];

      જો (width_ <= vvp_vector4_t::BITS_PER_WORD) {
	    માટે (unsigned idx = 0 ; idx < શબ્દો_3 ; idx += 1) {
		  array_[idx].abits_val_ = vvp_vector4_t::WORD_X_ABITS;
		  array_[idx].bbits_val_ = vvp_vector4_t::WORD_X_BBITS;
	    }
      } નહીંતર {
	    માટે (unsigned idx = 0 ; idx < શબ્દો_3 ; idx += 1) {
		  array_[idx].abits_ptr_ = 0;
		  array_[idx].bbits_ptr_ = 0;
	    }
      }
}

vvp_vector4array_sa::~vvp_vector4array_sa()
{
      જો (array_) {
	    જો (width_ > vvp_vector4_t::BITS_PER_WORD) {
		  માટે (unsigned idx = 0 ; idx < શબ્દો_3 ; idx += 1)
			જો (array_[idx].abits_ptr_)
			      delete[]array_[idx].abits_ptr_;
	    }
	    delete[]array_;
      }
}

શૂન્ય vvp_vector4array_sa::set_word(unsigned index, અચલ vvp_vector4_t&that)
{
      પ્રતિજ્ઞા(index < શબ્દો_3);

      v4cell*cell = &array_[index];

      set_word_(cell, that);
}

vvp_vector4_t vvp_vector4array_sa::get_word(unsigned index) અચલ
{
      જો (index >= શબ્દો_3)
	    પાછા vvp_vector4_t(width_, BIT4_X);

      પ્રતિજ્ઞા(index < શબ્દો_3);

      v4cell*cell = &array_[index];

      પાછા get_word_(cell);
}

vvp_vector4array_aa::vvp_vector4array_aa(unsigned width__, unsigned શબ્દો_2)
: vvp_vector4array_t(width__, શબ્દો_2)
{
      context_idx_ = vpip_add_item_to_context(આ, vpip_peek_context_scope());
}

vvp_vector4array_aa::~vvp_vector4array_aa()
{
}

શૂન્ય vvp_vector4array_aa::alloc_instance(vvp_context_t context)
{
      v4cell*array = નવું v4cell[શબ્દો_3];

      જો (width_ <= vvp_vector4_t::BITS_PER_WORD) {
	    માટે (unsigned idx = 0 ; idx < શબ્દો_3 ; idx += 1) {
		  array[idx].abits_val_ = vvp_vector4_t::WORD_X_ABITS;
		  array[idx].bbits_val_ = vvp_vector4_t::WORD_X_BBITS;
	    }
      } નહીંતર {
	    માટે (unsigned idx = 0 ; idx < શબ્દો_3 ; idx += 1) {
		  array[idx].abits_ptr_ = 0;
		  array[idx].bbits_ptr_ = 0;
	    }
      }

      vvp_set_context_item(context, context_idx_, array);
}

શૂન્ય vvp_vector4array_aa::reset_instance(vvp_context_t context)
{
      v4cell*cell = static_cast<v4cell*>
            (vvp_get_context_item(context, context_idx_));

      જો (width_ <= vvp_vector4_t::BITS_PER_WORD) {
	    માટે (unsigned idx = 0 ; idx < શબ્દો_3 ; idx += 1) {
		  cell->abits_val_ = vvp_vector4_t::WORD_X_ABITS;
		  cell->bbits_val_ = vvp_vector4_t::WORD_X_BBITS;
                  cell++;
	    }
      } નહીંતર {
            unsigned cnt = (width_ + vvp_vector4_t::BITS_PER_WORD-1)/vvp_vector4_t::BITS_PER_WORD;
	    માટે (unsigned idx = 0 ; idx < શબ્દો_3 ; idx += 1) {
		  જો (cell->abits_ptr_) {
                        માટે (unsigned n = 0 ; n < cnt ; n += 1) {
			      cell->abits_ptr_[n] = vvp_vector4_t::WORD_X_ABITS;
			      cell->bbits_ptr_[n] = vvp_vector4_t::WORD_X_BBITS;
                        }
                  }
                  cell++;
	    }
      }
}

#ifdef CHECK_WITH_VALGRIND
શૂન્ય vvp_vector4array_aa::free_instance(vvp_context_t context)
{
      v4cell*cell = static_cast<v4cell*>
            (vvp_get_context_item(context, context_idx_));
      delete [] cell;
}
#endif

શૂન્ય vvp_vector4array_aa::set_word(unsigned index, અચલ vvp_vector4_t&that)
{
      પ્રતિજ્ઞા(index < શબ્દો_3);

      v4cell*cell = static_cast<v4cell*>
            (vthread_get_wt_context_item(context_idx_)) + index;

      set_word_(cell, that);
}

vvp_vector4_t vvp_vector4array_aa::get_word(unsigned index) અચલ
{
      જો (index >= શબ્દો_3)
	    પાછા vvp_vector4_t(width_, BIT4_X);

      પ્રતિજ્ઞા(index < શબ્દો_3);

      v4cell*cell = static_cast<v4cell*>
            (vthread_get_rd_context_item(context_idx_)) + index;

      પાછા get_word_(cell);
}

vvp_vector2_t::vvp_vector2_t()
{
      vec_ = 0;
      wid_ = 0;
}

vvp_vector2_t::vvp_vector2_t(uint64_t v, unsigned wid)
{
      wid_ = wid;
      અચલ unsigned bits_per_word = 8 * કદ(vec_[0]);
      અચલ unsigned શબ્દો = (wid_ + bits_per_word-1) / bits_per_word;

      vec_ = નવું unsigned long[શબ્દો];
      માટે (unsigned idx = 0 ; idx < શબ્દો ; idx += 1) {
	    અચલ unsigned shift = idx * bits_per_word;
	    vec_[idx] = shift < 64 ? v >> shift : 0;
      }
}

vvp_vector2_t::vvp_vector2_t(vvp_vector2_t::fill_t fill, unsigned wid)
{
      wid_ = wid;
      અચલ unsigned bits_per_word = 8 * કદ(vec_[0]);
      અચલ unsigned શબ્દો = (wid_ + bits_per_word-1) / bits_per_word;

      vec_ = નવું unsigned long[શબ્દો];
      માટે (unsigned idx = 0 ;  idx < શબ્દો ;  idx += 1)
	    vec_[idx] = fill? -1 : 0;
}

vvp_vector2_t::vvp_vector2_t(અચલ vvp_vector2_t&that, unsigned base, unsigned wid)
{
      wid_ = wid;
      અચલ unsigned શબ્દો = (wid_ + BITS_PER_WORD-1) / BITS_PER_WORD;

      vec_ = નવું unsigned long[શબ્દો];

      માટે (unsigned idx = 0 ; idx < wid ; idx += 1) {
	    પૂર્ણાંક bit = that.value(base+idx);
	    જો (bit == 0)
		  ચાલુ;
	    unsigned word = idx / BITS_PER_WORD;
	    unsigned long mask = 1UL << (idx % BITS_PER_WORD);
	    vec_[word] |= mask;
      }
}

શૂન્ય vvp_vector2_t::copy_from_that_(અચલ vvp_vector4_t&that)
{
      wid_ = that.size();
      અચલ unsigned શબ્દો = (that.size() + BITS_PER_WORD-1) / BITS_PER_WORD;

      જો (શબ્દો == 0) {
	    vec_ = 0;
	    wid_ = 0;
	    પાછા;
      }

	// Use the subarray method with the xz_to_0 flag set so that
	// we get values even when there are xz bits.
      vec_ = that.subarray(0, wid_, સાચું);
}

શૂન્ય vvp_vector2_t::copy_from_that_(અચલ vvp_vector2_t&that)
{
      wid_ = that.wid_;
      અચલ unsigned શબ્દો = (wid_ + BITS_PER_WORD-1) / BITS_PER_WORD;

      જો (શબ્દો == 0) {
	    vec_ = 0;
	    wid_ = 0;
	    પાછા;
      }

      vec_ = નવું unsigned long[શબ્દો];
      માટે (unsigned idx = 0 ;  idx < શબ્દો ;  idx += 1)
	    vec_[idx] = that.vec_[idx];
}

vvp_vector2_t::vvp_vector2_t(અચલ vvp_vector2_t&that, unsigned newsize)
{
      wid_ = newsize;
      જો (newsize == 0) {
	    vec_ = 0;
	    પાછા;
      }

      અચલ unsigned શબ્દો = (wid_ + BITS_PER_WORD-1) / BITS_PER_WORD;
      અચલ unsigned twords = (that.wid_ + BITS_PER_WORD-1) / BITS_PER_WORD;

      vec_ = નવું unsigned long[શબ્દો];
      માટે (unsigned idx = 0 ;  idx < શબ્દો ;  idx += 1) {
	    જો (idx < twords)
		  vec_[idx] = that.vec_[idx];
	    નહીંતર
		  vec_[idx] = 0;
      }
}

vvp_vector2_t& vvp_vector2_t::operator= (અચલ vvp_vector2_t&that)
{
      જો (આ == &that)
	    પાછા *આ;

      delete[] vec_;
      vec_ = 0;

      copy_from_that_(that);
      પાછા *આ;
}

vvp_vector2_t& vvp_vector2_t::operator= (અચલ vvp_vector4_t&that)
{
      delete[]vec_;
      vec_ = 0;
      copy_from_that_(that);
      પાછા *આ;
}

vvp_vector2_t& vvp_vector2_t::operator <<= (unsigned પૂર્ણાંક shift)
{
      જો (wid_ == 0)
	    પાછા *આ;

      અચલ unsigned શબ્દો = (wid_ + BITS_PER_WORD-1) / BITS_PER_WORD;

	// Number of words to shift
      અચલ unsigned wshift = shift / BITS_PER_WORD;
	// bits to shift within each word.
      અચલ unsigned long oshift = shift % BITS_PER_WORD;

	// If shifting the entire value away, then return zeros.
      જો (wshift >= શબ્દો) {
	    માટે (unsigned idx = 0 ;  idx < શબ્દો ;  idx += 1)
		  vec_[idx] = 0;

	    પાછા *આ;
      }

	// Do the word shift first.
      જો (wshift > 0) {
	    માટે (unsigned idx = 0 ;  idx < શબ્દો-wshift ;  idx += 1) {
		  unsigned sel = શબ્દો - idx - 1;
		  vec_[sel] = vec_[sel-wshift];
	    }

	    માટે (unsigned idx = 0 ;  idx < wshift ;  idx += 1)
		  vec_[idx] = 0;
      }

	// Do the fine shift.
      જો (oshift != 0) {
	    unsigned long pad = 0;
	    માટે (unsigned idx = 0 ;  idx < શબ્દો ;  idx += 1) {
		  unsigned long next_pad = vec_[idx] >> (BITS_PER_WORD-oshift);
		  vec_[idx] = (vec_[idx] << oshift) | pad;
		  pad = next_pad;
	    }

	      // Cleanup the tail bits.
	    unsigned long mask = -1UL >> (BITS_PER_WORD - wid_%BITS_PER_WORD);
	    vec_[શબ્દો-1] &= mask;
      }

      પાછા *આ;
}

vvp_vector2_t& vvp_vector2_t::operator >>= (unsigned shift)
{
      જો (wid_ == 0)
	    પાછા *આ;

      અચલ unsigned શબ્દો = (wid_ + BITS_PER_WORD-1) / BITS_PER_WORD;

	// Number of words to shift
      અચલ unsigned wshift = shift / BITS_PER_WORD;
	// bits to shift within each word.
      અચલ unsigned long oshift = shift % BITS_PER_WORD;

	// If shifting the entire value away, then return zeros.
      જો (wshift >= શબ્દો) {
	    માટે (unsigned idx = 0 ;  idx < શબ્દો ;  idx += 1)
		  vec_[idx] = 0;

	    પાછા *આ;
      }

      જો (wshift > 0) {
	    માટે (unsigned idx = 0 ;  idx < શબ્દો-wshift ;  idx += 1)
		  vec_[idx] = vec_[idx+wshift];

	    માટે (unsigned idx = શબ્દો-wshift ;  idx < શબ્દો ;  idx += 1)
		  vec_[idx] = 0;
      }

      જો (oshift > 0) {
	    unsigned long pad = 0;
	    માટે (unsigned idx = શબ્દો ;  idx > 0 ;  idx -= 1) {
		  unsigned long new_pad = vec_[idx-1] <<(BITS_PER_WORD-oshift);
		  vec_[idx-1] = pad | (vec_[idx-1] >> oshift);
		  pad = new_pad;
	    }

	      // Cleanup the tail bits.

	    unsigned use_શબ્દો = શબ્દો;
	      // Mask_shift is the number of high bits of the top word
	      // that are to be masked off. We start with the number
	      // of bits that are not included even in the original,
	      // then we include the bits of the shift, that are to be
	      // masked to zero.
	    unsigned long mask_shift = BITS_PER_WORD - wid_%BITS_PER_WORD;
	    mask_shift %= BITS_PER_WORD;
	    mask_shift += oshift;
	    જ્યારે (mask_shift >= BITS_PER_WORD) {
		  vec_[use_શબ્દો-1] = 0;
		  use_શબ્દો -= 1;
		  mask_shift -= BITS_PER_WORD;
	    }
	    જો (mask_shift > 0) {
		  પ્રતિજ્ઞા(use_શબ્દો > 0);
		  unsigned long mask = -1UL >> mask_shift;
		  vec_[use_શબ્દો-1] &= mask;
	    }
      }

      પાછા *આ;
}

સ્થિર unsigned long add_carry(unsigned long a, unsigned long b,
			       unsigned long&carry)
{
      unsigned long out = carry;
      carry = 0;

      જો ((ULONG_MAX - out) < a)
	    carry += 1;
      out += a;

      જો ((ULONG_MAX - out) < b)
	    carry += 1;
      out += b;

      પાછા out;
}

vvp_vector2_t& vvp_vector2_t::operator += (અચલ vvp_vector2_t&that)
{
      પ્રતિજ્ઞા(wid_ == that.wid_);
      જો (wid_ == 0)
	    પાછા *આ;

      અચલ unsigned શબ્દો = (wid_ + BITS_PER_WORD-1) / BITS_PER_WORD;

      unsigned long carry = 0;
      માટે (unsigned idx = 0 ;  idx < શબ્દો ;  idx += 1) {
	    vec_[idx] = add_carry(vec_[idx], that.vec_[idx], carry);
      }


	// Cleanup the tail bits.
      unsigned long mask = -1UL >> (BITS_PER_WORD - wid_%BITS_PER_WORD);
      vec_[શબ્દો-1] &= mask;

      પાછા *આ;
}

vvp_vector2_t& vvp_vector2_t::operator -= (અચલ vvp_vector2_t&that)
{
      પ્રતિજ્ઞા(wid_ == that.wid_);
      જો (wid_ == 0)
	    પાછા *આ;

      અચલ unsigned શબ્દો = (wid_ + BITS_PER_WORD-1) / BITS_PER_WORD;

      unsigned long carry = 1;
      માટે (unsigned idx = 0 ;  idx < શબ્દો ;  idx += 1) {
	    vec_[idx] = add_carry(vec_[idx], ~that.vec_[idx], carry);
      }

      પાછા *આ;
}

શૂન્ય vvp_vector2_t::trim()
{
      જ્યારે (value(wid_-1) == 0 && wid_ > 1) wid_ -= 1;
}

/* This is a special trim that is used on numbers we know represent a
 * negative signed value (they came from a negative real value). */
શૂન્ય vvp_vector2_t::trim_neg()
{
      જો (value(wid_-1) == 1 && wid_ > 32) {
	    જ્યારે (value(wid_-2) == 1 && wid_ > 32) wid_ -= 1;
      }
}

પૂર્ણાંક vvp_vector2_t::value(unsigned idx) અચલ
{
      જો (idx >= wid_)
	    પાછા 0;

      અચલ unsigned bits_per_word = 8 * કદ(vec_[0]);
      unsigned addr = idx/bits_per_word;
      unsigned mask = idx%bits_per_word;

      જો (vec_[addr] & (1UL<<mask))
	    પાછા 1;
      નહીંતર
	    પાછા 0;
}

શૂન્ય vvp_vector2_t::set_bit(unsigned idx, પૂર્ણાંક bit)
{
      પ્રતિજ્ઞા(idx < wid_);

      અચલ unsigned bits_per_word = 8 * કદ(vec_[0]);
      unsigned addr = idx/bits_per_word;
      unsigned long mask = idx%bits_per_word;

      જો (bit)
	    vec_[addr] |= 1UL << mask;
      નહીંતર
	    vec_[addr] &= ~(1UL << mask);
}

શૂન્ય vvp_vector2_t::set_vec(unsigned adr, અચલ vvp_vector2_t&that)
{
      પ્રતિજ્ઞા((adr + that.wid_) <= wid_);

      માટે (unsigned idx = 0 ; idx < that.wid_ ; idx += 1)
	    set_bit(adr+idx, that.value(idx));
}

તાર્કિક vvp_vector2_t::is_NaN() અચલ
{
      પાછા wid_ == 0;
}

તાર્કિક vvp_vector2_t::is_zero() અચલ
{
      અચલ unsigned શબ્દો = (wid_ + BITS_PER_WORD-1) / BITS_PER_WORD;

      માટે (unsigned idx = 0; idx < શબ્દો; idx += 1) {
	    જો (vec_[idx] == 0) ચાલુ;
	    પાછા ખોટું;
      }

      પાછા સાચું;
}

/*
 * Basic idea from "Introduction to Programming using SML" by
 * Michael R. Hansen and Hans Rischel page 261 and "Seminumerical
 * Algorithms, Third Edition" by Donald E. Knuth section 4.6.3.
 */
vvp_vector2_t pow(અચલ vvp_vector2_t&x, vvp_vector2_t&y)
{
        /* If we have a zero exponent just return 1. */
      જો (y == vvp_vector2_t(0L, 1)) {
	    પાછા vvp_vector2_t(1L, x.size());
      }

        /* Is the value odd? */
      જો (y.value(0) == 1) {
	    y.set_bit(0, 0);  // A quick subtract by 1.
	    vvp_vector2_t res = x * pow(x, y);
	    પાછા res;
      }

      y >>= 1;  // A fast divide by two. We know the LSB is zero.
      vvp_vector2_t z = pow(x, y);
      vvp_vector2_t res = z * z;
      પાછા res;
}

સ્થિર શૂન્ય multiply_long(unsigned long a, unsigned long b,
			  unsigned long&નીચલું, unsigned long&ઉપલું)
{
      પ્રતિજ્ઞા(કદ(unsigned long) %2 == 0);

      અચલ unsigned long word_mask = (1UL << 4UL*કદ(a)) - 1UL;
      unsigned long tmpa;
      unsigned long tmpb;
      unsigned long res[4];

      tmpa = a & word_mask;
      tmpb = b & word_mask;
      res[0] = tmpa * tmpb;
      res[1] = res[0] >> 4UL*કદ(unsigned long);
      res[0] &= word_mask;

      tmpa = (a >> 4UL*કદ(unsigned long)) & word_mask;
      res[1] += tmpa * tmpb;
      res[2] = res[1] >> 4UL*કદ(unsigned long);
      res[1] &= word_mask;

      tmpa = a & word_mask;
      tmpb = (b >> 4UL*કદ(unsigned long)) & word_mask;
      res[1] += tmpa * tmpb;
      res[2] += res[1] >> 4UL*કદ(unsigned long);
      res[3]  = res[2] >> 4UL*કદ(unsigned long);
      res[1] &= word_mask;
      res[2] &= word_mask;

      tmpa = (a >> 4UL*કદ(unsigned long)) & word_mask;
      res[2] += tmpa * tmpb;
      res[3] += res[2] >> 4UL*કદ(unsigned long);
      res[2] &= word_mask;

      ઉપલું = (res[3] << 4UL*કદ(unsigned long)) | res[2];
      નીચલું  = (res[1] << 4UL*કદ(unsigned long)) | res[0];
}

vvp_vector2_t operator * (અચલ vvp_vector2_t&a, અચલ vvp_vector2_t&b)
{
      અચલ unsigned bits_per_word = 8 * કદ(a.vec_[0]);

	// The compiler ensures that the two operands are of equal size.
      પ્રતિજ્ઞા(a.size() == b.size());
      vvp_vector2_t r (0, a.size());

      unsigned શબ્દો = (r.wid_ + bits_per_word - 1) / bits_per_word;

      માટે (unsigned bdx = 0 ;  bdx < શબ્દો ;  bdx += 1) {
	    unsigned long tmpb = b.vec_[bdx];
	    જો (tmpb == 0)
		  ચાલુ;

	    માટે (unsigned adx = 0 ;  adx < શબ્દો ;  adx += 1) {
		  unsigned long tmpa = a.vec_[adx];
		  જો (tmpa == 0)
			ચાલુ;

		  unsigned long નીચલું, hig;
		  multiply_long(tmpa, tmpb, નીચલું, hig);

		  unsigned long carry = 0;
		  માટે (unsigned sdx = 0
			     ; (adx+bdx+sdx) < શબ્દો
			     ;  sdx += 1) {

			r.vec_[adx+bdx+sdx] = add_carry(r.vec_[adx+bdx+sdx],
							નીચલું, carry);
			નીચલું = hig;
			hig = 0;
		  }
	    }
      }

      પાછા r;
}

સ્થિર શૂન્ય div_mod (vvp_vector2_t dividend, vvp_vector2_t ભાજક,
		     vvp_vector2_t&quotient, vvp_vector2_t&remainder)
{

      quotient = vvp_vector2_t(0, dividend.size());

      જો (ભાજક == quotient) {
	    cerr << "ERROR: division by zero, exiting." << endl;
	    exit(255);
      }

      જો (dividend < ભાજક) {
	    remainder = dividend;
	    પાછા;
      }

      vvp_vector2_t mask (1, dividend.size());

	// Make the dividend 1 bit larger to prevent overflow of
	// divtmp in startup.
      dividend = vvp_vector2_t(dividend, dividend.size()+1);
      vvp_vector2_t divtmp (ભાજક, dividend.size());

      જ્યારે (divtmp < dividend) {
	    divtmp <<= 1;
	    mask <<= 1;
      }

      જ્યારે (dividend >= ભાજક) {
	    જો (divtmp <= dividend) {
		  dividend -= divtmp;
		  quotient += mask;
	    }

	    divtmp >>= 1;
	    mask >>= 1;
      }

      remainder = vvp_vector2_t(dividend, mask.size());
}

vvp_vector2_t operator - (અચલ vvp_vector2_t&that)
{
      vvp_vector2_t neg(that);
      જો (neg.wid_ == 0) પાછા neg;

      અચલ unsigned શબ્દો = (neg.wid_ + neg.BITS_PER_WORD-1) /
                                         neg.BITS_PER_WORD;
      માટે (unsigned idx = 0 ;  idx < શબ્દો ;  idx += 1) {
	    neg.vec_[idx] = ~neg.vec_[idx];
      }
      neg += vvp_vector2_t(1, neg.wid_);

      પાછા neg;
}

vvp_vector2_t operator / (અચલ vvp_vector2_t&dividend,
			  અચલ vvp_vector2_t&ભાજક)
{
      vvp_vector2_t quot, rem;
      div_mod(dividend, ભાજક, quot, rem);
      પાછા quot;
}

vvp_vector2_t operator % (અચલ vvp_vector2_t&dividend,
			  અચલ vvp_vector2_t&ભાજક)
{
      vvp_vector2_t quot, rem;
      div_mod(dividend, ભાજક, quot, rem);
      પાછા rem;
}

તાર્કિક operator > (અચલ vvp_vector2_t&a, અચલ vvp_vector2_t&b)
{
      અચલ unsigned awords = (a.wid_ + vvp_vector2_t::BITS_PER_WORD-1) / vvp_vector2_t::BITS_PER_WORD;
      અચલ unsigned bwords = (b.wid_ + vvp_vector2_t::BITS_PER_WORD-1) / vvp_vector2_t::BITS_PER_WORD;

      અચલ unsigned શબ્દો = awords > bwords? awords : bwords;

      માટે (unsigned idx = શબ્દો ;  idx > 0 ;  idx -= 1) {
	    unsigned long aw = (idx <= awords)? a.vec_[idx-1] : 0;
	    unsigned long bw = (idx <= bwords)? b.vec_[idx-1] : 0;

	    જો (aw > bw)
		  પાછા સાચું;
	    જો (aw < bw)
		  પાછા ખોટું;
      }

	// If the above loop finishes, then the vectors are equal.
      પાછા ખોટું;
}

તાર્કિક operator >= (અચલ vvp_vector2_t&a, અચલ vvp_vector2_t&b)
{
      અચલ unsigned awords = (a.wid_ + vvp_vector2_t::BITS_PER_WORD-1) / vvp_vector2_t::BITS_PER_WORD;
      અચલ unsigned bwords = (b.wid_ + vvp_vector2_t::BITS_PER_WORD-1) / vvp_vector2_t::BITS_PER_WORD;

      અચલ unsigned શબ્દો = awords > bwords? awords : bwords;

      માટે (unsigned idx = શબ્દો ;  idx > 0 ;  idx -= 1) {
	    unsigned long aw = (idx <= awords)? a.vec_[idx-1] : 0;
	    unsigned long bw = (idx <= bwords)? b.vec_[idx-1] : 0;

	    જો (aw > bw)
		  પાછા સાચું;
	    જો (aw < bw)
		  પાછા ખોટું;
      }

	// If the above loop finishes, then the vectors are equal.
      પાછા સાચું;
}

તાર્કિક operator < (અચલ vvp_vector2_t&a, અચલ vvp_vector2_t&b)
{
      અચલ unsigned awords = (a.wid_ + vvp_vector2_t::BITS_PER_WORD-1) / vvp_vector2_t::BITS_PER_WORD;
      અચલ unsigned bwords = (b.wid_ + vvp_vector2_t::BITS_PER_WORD-1) / vvp_vector2_t::BITS_PER_WORD;

      unsigned શબ્દો = awords;
      જો (bwords > શબ્દો)
	    શબ્દો = bwords;

      માટે (unsigned idx = શબ્દો ;  idx > 0 ;  idx -= 1) {
	    unsigned long aw = (idx <= awords)? a.vec_[idx-1] : 0;
	    unsigned long bw = (idx <= bwords)? b.vec_[idx-1] : 0;

	    જો (aw < bw)
		  પાછા સાચું;
	    જો (aw > bw)
		  પાછા ખોટું;
      }

	// If the above loop finishes, then the vectors are equal.
      પાછા ખોટું;
}

તાર્કિક operator <= (અચલ vvp_vector2_t&a, અચલ vvp_vector2_t&b)
{
	// XXXX For now, only support equal width vectors.
      પ્રતિજ્ઞા(a.wid_ == b.wid_);

      અચલ unsigned awords = (a.wid_ + vvp_vector2_t::BITS_PER_WORD-1) / vvp_vector2_t::BITS_PER_WORD;

      માટે (unsigned idx = awords ;  idx > 0 ;  idx -= 1) {
	    જો (a.vec_[idx-1] < b.vec_[idx-1])
		  પાછા સાચું;
	    જો (a.vec_[idx-1] > b.vec_[idx-1])
		  પાછા ખોટું;
      }

	// If the above loop finishes, then the vectors are equal.
      પાછા સાચું;
}

તાર્કિક operator == (અચલ vvp_vector2_t&a, અચલ vvp_vector2_t&b)
{
      અચલ unsigned awords = (a.wid_ + vvp_vector2_t::BITS_PER_WORD-1) / vvp_vector2_t::BITS_PER_WORD;
      અચલ unsigned bwords = (b.wid_ + vvp_vector2_t::BITS_PER_WORD-1) / vvp_vector2_t::BITS_PER_WORD;

      અચલ unsigned શબ્દો = awords > bwords? awords : bwords;

      માટે (unsigned idx = શબ્દો ;  idx > 0 ;  idx -= 1) {
	    unsigned long aw = (idx <= awords)? a.vec_[idx-1] : 0;
	    unsigned long bw = (idx <= bwords)? b.vec_[idx-1] : 0;

	    જો (aw > bw)
		  પાછા ખોટું;
	    જો (aw < bw)
		  પાછા ખોટું;
      }

	// If the above loop finishes, then the vectors are equal.
      પાછા સાચું;
}


vvp_vector4_t vector2_to_vector4(અચલ vvp_vector2_t&that, unsigned wid)
{
      vvp_vector4_t res (wid);

      માટે (unsigned idx = 0 ;  idx < res.size() ;  idx += 1) {
	    vvp_bit4_t bit = BIT4_0;

	    જો (that.value(idx))
		  bit = BIT4_1;

	    res.set_bit(idx, bit);
      }

      પાછા res;
}

સ્થિર તાર્કિક c4string_header_test(અચલ અક્ષર*str)
{
      જો ((str[0] != 'C') && (str[0] != 'c'))
	    પાછા ખોટું;
      જો ((str[1] != '4') || (str[2] != '<'))
	    પાછા ખોટું;
      પાછા સાચું;
}

તાર્કિક c4string_test(અચલ અક્ષર*str)
{
      જો (!c4string_header_test(str))
	    પાછા ખોટું;
      size_t કિંમત_size = strspn(str+3, "01xz");
      જો (str[3+કિંમત_size] != '>')
	    પાછા ખોટું;
      જો (str[3+કિંમત_size+1] != 0)
	    પાછા ખોટું;

      પાછા સાચું;
}

vvp_vector4_t c4string_to_vector4(અચલ અક્ષર*str)
{
      પ્રતિજ્ઞા(c4string_header_test(str));

      str += 3;
      અચલ અક્ષર*tp = str + strspn(str,"01xz");
      પ્રતિજ્ઞા(tp[0] == '>');

      vvp_vector4_t tmp (tp-str);

      માટે (unsigned idx = 0 ;  idx < tmp.size() ;  idx += 1) {
	    vvp_bit4_t bit;
	    પસંદગી (str[idx]) {
		સ્થિતિ '0':
		  bit = BIT4_0;
		  તોડો;
		સ્થિતિ '1':
		  bit = BIT4_1;
		  તોડો;
		સ્થિતિ 'x':
		  bit = BIT4_X;
		  તોડો;
		સ્થિતિ 'z':
		  bit = BIT4_Z;
		  તોડો;
		મૂળભૂત:
		  fprintf(stderr, "Unsupported bit value %c(%d).\n", str[idx],
		          str[idx]);
		  પ્રતિજ્ઞા(0);
		  bit = BIT4_0;
		  તોડો;
	    }
	    tmp.set_bit(tmp.size()-idx-1, bit);
      }

      પાછા tmp;
}

ostream& operator<< (ostream&out, અચલ vvp_vector2_t&that)
{
      જો (that.is_NaN()) {
	    out << "NaN";

      } નહીંતર {
	    out << vector2_to_vector4(that, that.size());
      }
      પાછા out;
}

vvp_vector8_t::vvp_vector8_t(અચલ vvp_vector8_t&that)
{
      size_ = that.size_;
      જો (size_ <= કદ(val_)) {
	    memcpy(val_, that.val_, કદ val_);
      } નહીંતર {
	    ptr_ = નવું unsigned અક્ષર[size_];
	    memcpy(ptr_, that.ptr_, size_);
      }
}

vvp_vector8_t::vvp_vector8_t(અચલ vvp_vector4_t&that,
			     unsigned str0, unsigned str1)
: size_(that.size())
{
      જો (size_ == 0)
	    પાછા;

      જો (size_ <= કદ(val_)) {
	    ptr_ = 0; // Prefill all val_ bytes
	    માટે (unsigned idx = 0 ; idx < size_ ; idx += 1)
		  val_[idx] = vvp_scalar_t(that.value(idx),str0, str1).raw();
      } નહીંતર {
	    ptr_ = નવું unsigned અક્ષર[size_];
	    માટે (unsigned idx = 0 ;  idx < size_ ;  idx += 1)
		  ptr_[idx] = vvp_scalar_t(that.value(idx), str0, str1).raw();
      }
}

vvp_vector8_t::vvp_vector8_t(અચલ vvp_vector2_t&that,
			     unsigned str0, unsigned str1)
: size_(that.size())
{
      જો (size_ == 0)
	    પાછા;

      જો (size_ <= કદ(val_)) {
	    ptr_ = 0;
	    માટે (unsigned idx = 0 ; idx < size_ ; idx += 1)
		  val_[idx] = vvp_scalar_t(that.value(idx)? BIT4_1:BIT4_0, str0, str1).raw();
      } નહીંતર {
	    ptr_ = નવું unsigned અક્ષર[size_];
	    માટે (unsigned idx = 0 ; idx < size_ ; idx += 1)
		  ptr_[idx] = vvp_scalar_t(that.value(idx)? BIT4_1:BIT4_0, str0, str1).raw();
      }
}

અચલ vvp_vector8_t vvp_vector8_t::nil;

vvp_vector8_t& vvp_vector8_t::operator= (અચલ vvp_vector8_t&that)
{
	// Assign to self.
      જો (આ == &that)
	    પાછા *આ;

      જો (size_ != that.size_) {
	    જો (size_ > કદ(val_))
		  delete[]ptr_;
	    size_ = 0;
      }

      જો (that.size_ == 0) {
	    પ્રતિજ્ઞા(size_ == 0);
	    પાછા *આ;
      }

      જો (that.size_ <= કદ(val_)) {
	    size_ = that.size_;
	    memcpy(val_, that.val_, કદ(val_));
	    પાછા *આ;
      }

      જો (size_ == 0) {
	    size_ = that.size_;
	    ptr_ = નવું unsigned અક્ષર[size_];
      }

      memcpy(ptr_, that.ptr_, size_);

      પાછા *આ;
}

vvp_vector8_t vvp_vector8_t::subvalue(unsigned base, unsigned wid) અચલ
{
      vvp_vector8_t tmp (wid);

      unsigned અક્ષર*tmp_ptr = tmp.size_ <= કદ(val_) ? tmp.val_ : tmp.ptr_;
      અચલ unsigned અક્ષર*use_ptr = size_ <= કદ(val_) ? val_ : ptr_;

      unsigned idx = 0;
      જ્યારે ((idx < wid) && (base+idx < size_)) {
	    tmp_ptr[idx] = use_ptr[base+idx];
	    idx += 1;
      }

      પાછા tmp;
}

શૂન્ય vvp_vector8_t::set_vec(unsigned base, અચલ vvp_vector8_t&that)
{
      પ્રતિજ્ઞા((base+that.size()) <= size());
      માટે (unsigned idx = 0 ; idx < that.size() ; idx += 1)
	    set_bit(base+idx, that.value(idx));
}

vvp_vector8_t part_expand(અચલ vvp_vector8_t&that, unsigned wid, unsigned off)
{
      પ્રતિજ્ઞા(off < wid);
      vvp_vector8_t tmp (wid);

      unsigned અક્ષર* tmp_ptr = tmp.size_<= કદ(tmp.val_) ?
                               tmp.val_ : tmp.ptr_;
      અચલ unsigned અક્ષર* that_ptr = that.size_<= કદ(that.val_) ?
                                      that.val_ : that.ptr_;

      unsigned idx = off;

      જ્યારે (idx < wid && that.size_ > (idx-off)) {
	    tmp_ptr[idx] = that_ptr[idx-off];
	    idx += 1;
      }

      પાછા tmp;
}

સ્થિર તાર્કિક c8string_header_test(અચલ અક્ષર*str)
{
      જો ((str[0] != 'C') && (str[0] != 'c'))
	    પાછા ખોટું;
      જો ((str[1] != '8') || (str[2] != '<'))
	    પાછા ખોટું;
      પાછા સાચું;
}

તાર્કિક c8string_test(અચલ અક્ષર*str)
{
      જો (!c8string_header_test(str))
	    પાછા ખોટું;

      અચલ અક્ષર*cp = str+3;
      માટે (;; cp += 1) {
	    જો (cp[0] == '>' && cp[1] == 0) પાછા સાચું;
	    જો (cp[0] >= '0' && cp[0] <= '9') ચાલુ;
	    જો (cp[0] == 'x') ચાલુ;
	    જો (cp[0] == 'z') ચાલુ;
	    પાછા ખોટું;
      }
      પાછા ખોટું;
}
/*
 * The format of a C8<> string is:
 *   C8<aaabbbccc...>
 * where aaa... is a 3 character bit descriptor.
 */
vvp_vector8_t c8string_to_vector8(અચલ અક્ષર*str)
{
      પ્રતિજ્ઞા(c8string_header_test(str));

      size_t vsize = strlen(str)-4;
      પ્રતિજ્ઞા(vsize%3 == 0);
      vsize /= 3;
      vvp_vector8_t tmp (vsize);

      માટે (size_t idx = 0 ; idx < vsize ; idx += 1) {
	    અચલ અક્ષર*cp = str+3+3*idx;
	    vvp_bit4_t bit = BIT4_X;
	    unsigned dr0 = cp[0]-'0';
	    unsigned dr1 = cp[1]-'0';
	    પસંદગી (cp[2]) {
		સ્થિતિ '0':
		  bit = BIT4_0;
		  તોડો;
		સ્થિતિ '1':
		  bit = BIT4_1;
		  તોડો;
		સ્થિતિ 'x':
		  bit = BIT4_X;
		  તોડો;
		સ્થિતિ 'z':
		  bit = BIT4_Z;
		  તોડો;
	    }
	    tmp.set_bit(vsize-idx-1, vvp_scalar_t(bit, dr0, dr1));
      }

      પાછા tmp;
}

ostream& operator<<(ostream&out, અચલ vvp_vector8_t&that)
{
      out << "C8<";
      માટે (unsigned idx = 0 ;  idx < that.size() ; idx += 1)
	    out << that.value(that.size()-idx-1);

      out << ">";
      પાછા out;
}

vvp_net_fun_t::vvp_net_fun_t()
{
      ગણતરી_functors += 1;
}

vvp_net_fun_t::~vvp_net_fun_t()
{
}

શૂન્ય vvp_net_fun_t::recv_vec4(vvp_net_ptr_t, અચલ vvp_vector4_t&,
                              vvp_context_t)
{
      fprintf(stderr, "internal error: %s: recv_vec4 not implemented\n",
	      typeid(*આ).name());
      પ્રતિજ્ઞા(0);
}

શૂન્ય vvp_net_fun_t::recv_vec4_pv_(vvp_net_ptr_t p, અચલ vvp_vector4_t&bit,
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

      પ્રતિજ્ઞા(base + bit.size() <= vwid);

      vvp_vector4_t tmp(vwid, BIT4_Z);
      tmp.set_vec(base, bit);
      recv_vec4(p, tmp, 0);
}

શૂન્ય vvp_net_fun_t::recv_vec4_pv(vvp_net_ptr_t, અચલ vvp_vector4_t&bit,
                                 unsigned base, unsigned vwid, vvp_context_t)
{
      cerr << "internal error: " << typeid(*આ).name() << ": "
	   << "recv_vec4_pv(" << bit << ", " << base
	   << ", " << vwid << ") not implemented" << endl;
      પ્રતિજ્ઞા(0);
}

શૂન્ય vvp_net_fun_t::recv_vec8(vvp_net_ptr_t port, અચલ vvp_vector8_t&bit)
{
      recv_vec4(port, reduce4(bit), 0);
}

શૂન્ય vvp_net_fun_t::recv_vec8_pv_(vvp_net_ptr_t p, અચલ vvp_vector8_t&bit,
				  unsigned base, unsigned vwid)
{
	// This is the strength-aware version of recv_vec4_pv_.

      પ્રતિજ્ઞા(base + bit.size() <= vwid);

      vvp_vector8_t tmp(vwid);
      tmp.set_vec(base, bit);
      recv_vec8(p, tmp);
}

શૂન્ય vvp_net_fun_t::recv_vec8_pv(vvp_net_ptr_t port, અચલ vvp_vector8_t&bit,
				 unsigned base, unsigned vwid)
{
      recv_vec4_pv(port, reduce4(bit), base, vwid, 0);
}

શૂન્ય vvp_net_fun_t::recv_real(vvp_net_ptr_t, દ્વિગુણ bit, vvp_context_t)
{
      fprintf(stderr, "internal error: %s: recv_real(%f) not implemented\n",
	      typeid(*આ).name(), bit);
      પ્રતિજ્ઞા(0);
}

શૂન્ય vvp_net_fun_t::recv_string(vvp_net_ptr_t, અચલ std::string&bit, vvp_context_t)
{
      fprintf(stderr, "internal error: %s: recv_string(%s) not implemented\n",
	      typeid(*આ).name(), bit.c_str());
      પ્રતિજ્ઞા(0);
}

શૂન્ય vvp_net_fun_t::recv_object(vvp_net_ptr_t, vvp_object_t, vvp_context_t)
{
      fprintf(stderr, "internal error: %s: recv_object(...) not implemented\n",
	      typeid(*આ).name());
      પ્રતિજ્ઞા(0);
}

શૂન્ય vvp_net_fun_t::force_flag(તાર્કિક)
{
}

/* **** vvp_fun_drive methods **** */

vvp_fun_drive::vvp_fun_drive(unsigned str0, unsigned str1)
{
      પ્રતિજ્ઞા(str0 < 8);
      પ્રતિજ્ઞા(str1 < 8);

      drive0_ = str0;
      drive1_ = str1;
}

vvp_fun_drive::~vvp_fun_drive()
{
}

શૂન્ય vvp_fun_drive::recv_vec4(vvp_net_ptr_t port, અચલ vvp_vector4_t&bit,
                              vvp_context_t)
{
      પ્રતિજ્ઞા(port.port() == 0);
      port.ptr()->send_vec8(vvp_vector8_t(bit, drive0_, drive1_));
}

શૂન્ય vvp_fun_drive::recv_vec4_pv(vvp_net_ptr_t ptr, અચલ vvp_vector4_t&bit,
				 unsigned base, unsigned vwid, vvp_context_t ctx)
{
      recv_vec4_pv_(ptr, bit, base, vwid, ctx);
}

/* **** vvp_wide_fun_* methods **** */

vvp_wide_fun_core::vvp_wide_fun_core(vvp_net_t*net, unsigned nports)
{
      ptr_ = net;
      nports_ = nports;
      port_કિંમતો = 0;
      port_rvalues_ = 0;
}

vvp_wide_fun_core::~vvp_wide_fun_core()
{
      delete[] port_કિંમતો;
      delete[] port_rvalues_;
}

શૂન્ય vvp_wide_fun_core::propagate_vec4(અચલ vvp_vector4_t&bit,
				       vvp_time64_t delay)
{
      જો (delay)
	    schedule_propagate_vector(ptr_, delay, bit);
      નહીંતર
	    ptr_->send_vec4(bit, 0);
}

શૂન્ય vvp_wide_fun_core::propagate_real(દ્વિગુણ bit,
				       vvp_time64_t delay)
{
      જો (delay) {
	      // schedule_assign_vector(ptr_->out, bit, delay);
	    પ્રતિજ્ઞા(0); // Need a real-value version of assign_vector.
      } નહીંતર {
	    ptr_->send_real(bit, 0);
      }
}


unsigned vvp_wide_fun_core::port_ગણતરી() અચલ
{
      પાછા nports_;
}

vvp_vector4_t& vvp_wide_fun_core::value(unsigned idx)
{
      પ્રતિજ્ઞા(idx < nports_);
      જો (port_કિંમતો == 0)
	    port_કિંમતો = નવું vvp_vector4_t [nports_];
      પાછા port_કિંમતો[idx];
}

દ્વિગુણ vvp_wide_fun_core::કિંમત_r(unsigned idx)
{
      પ્રતિજ્ઞા(idx < nports_);
      પાછા port_rvalues_? port_rvalues_[idx] : 0.0;
}

શૂન્ય vvp_wide_fun_core::recv_real_from_inputs(unsigned)
{
      પ્રતિજ્ઞા(0);
}

શૂન્ય vvp_wide_fun_core::dispatch_vec4_from_આગત(unsigned port,
						  અચલ vvp_vector4_t&bit)
{
      પ્રતિજ્ઞા(port < nports_);
      જો (port_કિંમતો == 0) port_કિંમતો = નવું vvp_vector4_t [nports_];
      port_કિંમતો[port] = bit;
      recv_vec4_from_inputs(port);
}

શૂન્ય vvp_wide_fun_core::dispatch_real_from_આગત(unsigned port,
						  દ્વિગુણ bit)
{
      પ્રતિજ્ઞા(port < nports_);
      જો (port_rvalues_ == 0) port_rvalues_ = નવું દ્વિગુણ[nports_];
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

શૂન્ય vvp_wide_fun_t::recv_vec4(vvp_net_ptr_t port, અચલ vvp_vector4_t&bit,
                               vvp_context_t)
{
      unsigned pidx = port_base_ + port.port();
      core_->dispatch_vec4_from_આગત(pidx, bit);
}

શૂન્ય vvp_wide_fun_t::recv_vec4_pv(vvp_net_ptr_t ptr, અચલ vvp_vector4_t&bit,
				  unsigned base, unsigned vwid, vvp_context_t ctx)
{
      recv_vec4_pv_(ptr, bit, base, vwid, ctx);
}

શૂન્ય vvp_wide_fun_t::recv_real(vvp_net_ptr_t port, દ્વિગુણ bit,
                               vvp_context_t)
{
      unsigned pidx = port_base_ + port.port();
      core_->dispatch_real_from_આગત(pidx, bit);
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

unsigned vvp_scalar_t::strength0() અચલ
{
      પાછા STREN0(કિંમત_2);
}

unsigned vvp_scalar_t::strength1() અચલ
{
      પાછા STREN1(કિંમત_2);
}

ostream& operator <<(ostream&out, vvp_scalar_t a)
{
      out << a.strength0() << a.strength1();
      પસંદગી (a.value()) {
	  સ્થિતિ BIT4_0:
	    out << "0";
	    તોડો;
	  સ્થિતિ BIT4_1:
	    out << "1";
	    તોડો;
	  સ્થિતિ BIT4_X:
	    out << "X";
	    તોડો;
	  સ્થિતિ BIT4_Z:
	    out << "Z";
	    તોડો;
      }
      પાછા out;
}

/*
 * This function is only called if the actual interface function rules
 * out some of the easy cases. If we get here, we can assume that
 * neither of the values is HiZ, and the values are not exactly equal.
 */
vvp_scalar_t fully_featured_resolv_(vvp_scalar_t a, vvp_scalar_t b)
{

      જો (UNAMBIG(a.કિંમત_2) && UNAMBIG(b.કિંમત_2)) {

	      /* If both signals are unambiguous, simply choose
		 the stronger. If they have the same strength
		 but different values, then this becomes
		 ambiguous. */

	    જો ((b.કિંમત_2&0x07) > (a.કિંમત_2&0x07)) {

		    /* b value is stronger. Take it. */
		  પાછા b;

	    } નહીંતર જો ((b.કિંમત_2&0x77) == (a.કિંમત_2&0x77)) {

		    // Strengths are the same. Since we know already
		    // that the values are not the same, Make value
		    // into "x".
		  vvp_scalar_t tmp (a);
		  tmp.કિંમત_2 = (tmp.કિંમત_2&0x77) | 0x80;
		  પાછા tmp;

	    } નહીંતર {

		    /* Must be "a" is the stronger one. */
		  પાછા a;
	    }

      }

	/* If one of the signals is unambiguous, then it
	   will sweep up the weaker parts of the ambiguous
	   signal. The result may be ambiguous, or maybe not. */

      જો (UNAMBIG(a.કિંમત_2)) {
	    vvp_scalar_t res;

	    જો ((a.કિંમત_2&0x70) > (b.કિંમત_2&0x70))
		  res.કિંમત_2 |= a.કિંમત_2&0xf0;
	    નહીંતર
		  res.કિંમત_2 |= b.કિંમત_2&0xf0;

	    જો ((a.કિંમત_2&0x07) > (b.કિંમત_2&0x07))
		  res.કિંમત_2 |= a.કિંમત_2&0x0f;
	    નહીંતર
		  res.કિંમત_2 |= b.કિંમત_2&0x0f;

	    પાછા res;

      } નહીંતર જો (UNAMBIG(b.કિંમત_2)) {

	    vvp_scalar_t res;

	    જો ((b.કિંમત_2&0x70) > (a.કિંમત_2&0x70))
		  res.કિંમત_2 |= b.કિંમત_2&0xf0;
	    નહીંતર
		  res.કિંમત_2 |= a.કિંમત_2&0xf0;

	    જો ((b.કિંમત_2&0x07) > (a.કિંમત_2&0x07))
		  res.કિંમત_2 |= b.કિંમત_2&0x0f;
	    નહીંતર
		  res.કિંમત_2 |= a.કિંમત_2&0x0f;

	    પાછા res;

      }


	/* If both signals are ambiguous, then the result
	   has an even wider ambiguity. */

      unsigned tmp = 0;
      પૂર્ણાંક sv1a = (a.કિંમત_2 & 0x80) ? STREN1(a.કિંમત_2) : - STREN1(a.કિંમત_2);
      પૂર્ણાંક sv0a = (a.કિંમત_2 & 0x08) ? STREN0(a.કિંમત_2) : - STREN0(a.કિંમત_2);
      પૂર્ણાંક sv1b = (b.કિંમત_2 & 0x80) ? STREN1(b.કિંમત_2) : - STREN1(b.કિંમત_2);
      પૂર્ણાંક sv0b = (b.કિંમત_2 & 0x08) ? STREN0(b.કિંમત_2) : - STREN0(b.કિંમત_2);

      પૂર્ણાંક sv1 = sv1a;
      પૂર્ણાંક sv0 = sv0a;

      જો (sv0a > sv1)
	    sv1 = sv0a;
      જો (sv1b > sv1)
	    sv1 = sv1b;
      જો (sv0b > sv1)
	    sv1 = sv0b;

      જો (sv1a < sv0)
	    sv0 = sv1a;
      જો (sv1b < sv0)
	    sv0 = sv1b;
      જો (sv0b < sv0)
	    sv0 = sv0b;

      જો (sv1 > 0) {
	    tmp |= 0x80;
	    tmp |= sv1 << 4;
      } નહીંતર {
	      /* Set the MSB when both arguments MSBs are set. This
		 can only happen if both one strengths are zero. */
	    tmp |= (a.કિંમત_2&b.કિંમત_2)&0x80;
	    tmp |= (-sv1) << 4;
      }

      જો (sv0 > 0) {
	    tmp |= 0x08;
	    tmp |= sv0;
      } નહીંતર {
	    tmp |= (-sv0);
      }

      vvp_scalar_t res;
      res.કિંમત_2 = tmp;

	/* Canonicalize the HiZ value. */
      જો ((res.કિંમત_2&0x77) == 0)
	    res.કિંમત_2 = 0;

      પાછા res;
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

vvp_vector4_t reduce4(અચલ vvp_vector8_t&that)
{
      vvp_vector4_t out (that.size());
      માટે (unsigned idx = 0 ;  idx < out.size() ;  idx += 1)
	    out.set_bit(idx, that.value(idx).value());

      પાછા out;
}

vvp_bit4_t compare_gtge(અચલ vvp_vector4_t&lef, અચલ vvp_vector4_t&rig,
			vvp_bit4_t out_if_equal)
{
      unsigned min_size = lef.size();
      જો (rig.size() < min_size)
	    min_size = rig.size();

	// If one of the inputs is nil, treat is as all X values, and
	// that makes the result BIT4_X.
      જો (min_size == 0)
	    પાછા BIT4_X;

	// As per the IEEE1364 definition of >, >=, < and <=, if there
	// are any X or Z values in either of the operand vectors,
	// then the result of the compare is BIT4_X.

	// Check for X/Z in the left operand
      જો (lef.has_xz())
	    પાછા BIT4_X;

	// Check for X/Z in the right operand
      જો (rig.has_xz())
	    પાછા BIT4_X;

      માટે (unsigned idx = lef.size() ; idx > rig.size() ;  idx -= 1) {
	    જો (lef.value(idx-1) == BIT4_1)
		  પાછા BIT4_1;
      }

      માટે (unsigned idx = rig.size() ; idx > lef.size() ;  idx -= 1) {
	    જો (rig.value(idx-1) == BIT4_1)
		  પાછા BIT4_0;
      }

      માટે (unsigned idx = min_size ; idx > 0 ;  idx -= 1) {
	    vvp_bit4_t lv = lef.value(idx-1);
	    vvp_bit4_t rv = rig.value(idx-1);

	    જો (lv == rv)
		  ચાલુ;

	    જો (lv == BIT4_1)
		  પાછા BIT4_1;
	    નહીંતર
		  પાછા BIT4_0;
      }

      પાછા out_if_equal;
}

vvp_bit4_t compare_gtge_signed(અચલ vvp_vector4_t&a,
			       અચલ vvp_vector4_t&b,
			       vvp_bit4_t out_if_equal)
{
      પ્રતિજ્ઞા(a.size() == b.size());

      unsigned sign_idx = a.size()-1;
      vvp_bit4_t a_sign = a.value(sign_idx);
      vvp_bit4_t b_sign = b.value(sign_idx);

      જો (bit4_is_xz(a_sign))
	    પાછા BIT4_X;
      જો (bit4_is_xz(b_sign))
	    પાછા BIT4_X;

      જો (a_sign == b_sign)
	    પાછા compare_gtge(a, b, out_if_equal);

      જો (a.has_xz())
	    પાછા BIT4_X;

      જો (b.has_xz())
	    પાછા BIT4_X;

      જો(a_sign == BIT4_0)
	    પાછા BIT4_1;
      નહીંતર
	    પાછા BIT4_0;
}
