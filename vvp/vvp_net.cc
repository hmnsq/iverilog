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

using ನಾಮಾಕಾಶ std;

/* This is the size of an unsigned long in bits. This is just a
   convenience macro. */
# define CPU_WORD_BITS (8*ಪರಿಮಾಣ(unsigned long))
# define ತುದಿ_BIT (1UL << (CPU_WORD_BITS-1))

permaheap vvp_net_fun_t::heap_;
permaheap vvp_net_fil_t::heap_;

// Allocate around 1Megabyte/chunk.
ಸ್ಥಿರ ಧ್ರುವ size_t VVP_NET_CHUNK = 1024*1024/ಪರಿಮಾಣ(vvp_net_t);
ಸ್ಥಿರ vvp_net_t*vvp_net_alloc_table = ರಿಕ್ತ;
#ifdef CHECK_WITH_VALGRIND
ಸ್ಥಿರ vvp_net_t **vvp_net_pool = ರಿಕ್ತ;
ಸ್ಥಿರ unsigned vvp_net_pool_ಎಣಿಕೆ = 0;
#endif
ಸ್ಥಿರ size_t vvp_net_alloc_remaining = 0;
// For statistics, count the vvp_nets allocated and the bytes of alloc
// chunks allocated.
unsigned long ಎಣಿಕೆ_vvp_nets = 0;
size_t size_vvp_nets = 0;

ಶೂನ್ಯ* vvp_net_t::operator ನೂತನ (size_t size)
{
      ಪ್ರತಿಜ್ಞಾ(size == ಪರಿಮಾಣ(vvp_net_t));
      ಯದಿ (vvp_net_alloc_remaining == 0) {
	    vvp_net_alloc_table = ::ನೂತನ vvp_net_t[VVP_NET_CHUNK];
	    vvp_net_alloc_remaining = VVP_NET_CHUNK;
	    size_vvp_nets += size*VVP_NET_CHUNK;
#ifdef CHECK_WITH_VALGRIND
	    VALGRIND_MAKE_MEM_NOACCESS(vvp_net_alloc_table, size*VVP_NET_CHUNK);
	    VALGRIND_CREATE_MEMPOOL(vvp_net_alloc_table, 0, 0);
	    vvp_net_pool_ಎಣಿಕೆ += 1;
	    vvp_net_pool = static_cast<vvp_net_t **>(realloc(vvp_net_pool,
	                   vvp_net_pool_ಎಣಿಕೆ*ಪರಿಮಾಣ(vvp_net_t **)));
	    vvp_net_pool[vvp_net_pool_ಎಣಿಕೆ-1] = vvp_net_alloc_table;
#endif
      }

      vvp_net_t*return_this = vvp_net_alloc_table;
#ifdef CHECK_WITH_VALGRIND
      VALGRIND_MEMPOOL_ALLOC(vvp_net_pool[vvp_net_pool_ಎಣಿಕೆ-1],
                             return_this, size);
      return_this->pool = vvp_net_pool[vvp_net_pool_ಎಣಿಕೆ-1];
#endif
      vvp_net_alloc_table += 1;
      vvp_net_alloc_remaining -= 1;
      ಎಣಿಕೆ_vvp_nets += 1;
      ಪ್ರತ್ಯಾಗಮ return_this;
}

#ifdef CHECK_WITH_VALGRIND
ಸ್ಥಿರ map<vvp_net_t*, ತಾರ್ಕಿಕ> vvp_net_map;
ಸ್ಥಿರ map<sfunc_core*, ತಾರ್ಕಿಕ> sfunc_map;
ಸ್ಥಿರ map<vvp_udp_fun_core*, ತಾರ್ಕಿಕ> udp_map;
ಸ್ಥಿರ map<resolv_core*, ತಾರ್ಕಿಕ> resolv_map;
ಸ್ಥಿರ vvp_net_t **local_net_pool = 0;
ಸ್ಥಿರ unsigned local_net_pool_ಎಣಿಕೆ = 0;

ಶೂನ್ಯ pool_local_net(vvp_net_t*net)
{
      local_net_pool_ಎಣಿಕೆ += 1;
      local_net_pool = static_cast<vvp_net_t **>(realloc(local_net_pool,
                       local_net_pool_ಎಣಿಕೆ*ಪರಿಮಾಣ(vvp_net_t **)));
      local_net_pool[local_net_pool_ಎಣಿಕೆ-1] = net;
}

ಶೂನ್ಯ vvp_net_delete(vvp_net_t *item)
{
      vvp_net_map[item] = ಸತ್ಯ;
      ಯದಿ (sfunc_core*tmp = dynamic_cast<sfunc_core*> (item->fun)) {
	    sfunc_map[tmp] = ಸತ್ಯ;
      }
      ಯದಿ (vvp_udp_fun_core*tmp = dynamic_cast<vvp_udp_fun_core*> (item->fun)) {
	    udp_map[tmp] = ಸತ್ಯ;
      }
      ಯದಿ (resolv_core*tmp = dynamic_cast<resolv_core*> (item->fun)) {
	    resolv_map[tmp] = ಸತ್ಯ;
      }
	/* Only delete static object variables. */
      ಯದಿ (vvp_fun_signal_object_sa*tmp = dynamic_cast<vvp_fun_signal_object_sa*> (item->fun)) {
	    delete tmp;
      }
	/* Only delete static string variables. */
      ಯದಿ (vvp_fun_signal_string_sa*tmp = dynamic_cast<vvp_fun_signal_string_sa*> (item->fun)) {
	    delete tmp;
      }
}

ಶೂನ್ಯ vvp_net_pool_delete()
{
      unsigned long vvp_nets_del = 0;

      ಪ್ರತಿ (unsigned idx = 0; idx < local_net_pool_ಎಣಿಕೆ; idx += 1) {
	    vvp_net_delete(local_net_pool[idx]);
      }
      ಮುಕ್ತ(local_net_pool);
      local_net_pool = 0;
      local_net_pool_ಎಣಿಕೆ = 0;

      map<vvp_net_t*, ತಾರ್ಕಿಕ>::iterator iter;
      ಪ್ರತಿ (iter = vvp_net_map.begin(); iter != vvp_net_map.end(); ++ iter ) {
	    vvp_nets_del += 1;
	    VALGRIND_MEMPOOL_FREE(iter->first->pool, iter->first);
      }
      vvp_net_map.clear();

      map<sfunc_core*, ತಾರ್ಕಿಕ>::iterator siter;
      ಪ್ರತಿ (siter = sfunc_map.begin(); siter != sfunc_map.end(); ++ siter ) {
	    delete siter->first;
      }
      sfunc_map.clear();

      map<vvp_udp_fun_core*, ತಾರ್ಕಿಕ>::iterator uiter;
      ಪ್ರತಿ (uiter = udp_map.begin(); uiter != udp_map.end(); ++ uiter ) {
	    delete uiter->first;
      }
      udp_map.clear();

      map<resolv_core*, ತಾರ್ಕಿಕ>::iterator riter;
      ಪ್ರತಿ (riter = resolv_map.begin(); riter != resolv_map.end(); ++ riter ) {
	    delete riter->first;
      }
      resolv_map.clear();

      ಯದಿ (RUNNING_ON_VALGRIND && (vvp_nets_del != ಎಣಿಕೆ_vvp_nets)) {
	    fflush(ರಿಕ್ತ);
	    VALGRIND_PRINTF("Error: vvp missed deleting %ld of %lu net(s).",
	                    (long) ಎಣಿಕೆ_vvp_nets - vvp_nets_del,
	                    ಎಣಿಕೆ_vvp_nets);
      }

      ಪ್ರತಿ (unsigned idx = 0; idx < vvp_net_pool_ಎಣಿಕೆ; idx += 1) {
	    VALGRIND_DESTROY_MEMPOOL(vvp_net_pool[idx]);
	    ::delete [] vvp_net_pool[idx];
      }
      ಮುಕ್ತ(vvp_net_pool);
      vvp_net_pool = ರಿಕ್ತ;
      vvp_net_pool_ಎಣಿಕೆ = 0;
}
#endif

ಶೂನ್ಯ vvp_net_t::operator delete(ಶೂನ್ಯ*)
{
      ಪ್ರತಿಜ್ಞಾ(0);
}

vvp_net_t::vvp_net_t()
: out_(vvp_net_ptr_t(0,0))
{
      fun = 0;
      fil = 0;
}

ಶೂನ್ಯ vvp_net_t::link(vvp_net_ptr_t port_to_link)
{
      vvp_net_t*net = port_to_link.ptr();

	// Connect nodes with vvp_fun_modpath_src to the head of the
	// linked list, so that vvp_fun_modpath_src are always evaluated
	// before their respective vvp_fun_modpath
      ಯದಿ (dynamic_cast<vvp_fun_modpath_src*>(net->fun)) {
	    net->port[port_to_link.port()] = out_;
	    out_ = port_to_link;
	// All other nodes connect after the last vvp_fun_modpath_src
      } ಅನ್ಯಥಾ {
	      // List is empty or first node has no vvp_fun_modpath_src
	      // then just insert the node into the head of linked list
	    ಯದಿ (out_.ptr() == nullptr || ! dynamic_cast<vvp_fun_modpath_src*>(out_.ptr()->fun)) {
		  net->port[port_to_link.port()] = out_;
		  out_ = port_to_link;
		  ಪ್ರತ್ಯಾಗಮ;
	    }

	    vvp_net_ptr_t cur = out_;
	    vvp_net_ptr_t prev = vvp_net_ptr_t(nullptr,0);

	      // Traverse through linked list
	    ಯಾವತ್ (dynamic_cast<vvp_fun_modpath_src*>(cur.ptr()->fun)) {
		  prev = cur;
		  cur = cur.ptr()->port[cur.port()];
		  ಯದಿ (! cur.ptr()) ಭಙ್ಗ; // End of list
	    }

	    ಪ್ರತಿಜ್ಞಾ(prev.ptr());

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
ಶೂನ್ಯ vvp_net_t::unlink(vvp_net_ptr_t dst_ptr)
{
      vvp_net_t*net = dst_ptr.ptr();
      unsigned net_port = dst_ptr.port();

      ಯದಿ (out_ == dst_ptr) {
	      /* If the drive fan-out list starts with this pointer,
		 then the unlink is easy. Pull the list forward. */
	    out_ = net->port[net_port];
      } ಅನ್ಯಥಾ ಯದಿ (! out_.nil()) {
	      /* Scan the linked list, looking for the net_ptr_t
		 pointer *before* the one we wish to remove. */
	    vvp_net_ptr_t cur = out_;
	    ಪ್ರತಿಜ್ಞಾ(!cur.nil());
	    vvp_net_t*cur_net = cur.ptr();
	    unsigned cur_port = cur.port();
	    ಯಾವತ್ (cur_net && cur_net->port[cur_port] != dst_ptr) {
		  cur = cur_net->port[cur_port];
		  cur_net = cur.ptr();
		  cur_port = cur.port();
	    }
	      /* Unlink. */
	    ಯದಿ (cur_net) cur_net->port[cur_port] = net->port[net_port];
      }

      net->port[net_port] = vvp_net_ptr_t(0,0);
}

ಶೂನ್ಯ vvp_net_t::ಎಣಿಕೆ_drivers(unsigned idx, unsigned counts[4])
{
      counts[0] = 0;
      counts[1] = 0;
      counts[2] = 0;
      counts[3] = 0;

        /* $countdrivers can only be used on wires. */
      vvp_wire_base*wire=dynamic_cast<vvp_wire_base*>(fil);
      ಪ್ರತಿಜ್ಞಾ(wire);

      ಯದಿ (wire->is_forced(idx))
            counts[3] = 1;

        /* If the net has multiple drivers, we need to interrogate the
           resolver network to get the driven values. */
      ಯದಿ (resolv_core*resolver = dynamic_cast<resolv_core*>(fun)) {
            resolver->ಎಣಿಕೆ_drivers(idx, counts);
            ಪ್ರತ್ಯಾಗಮ;
      }
      ಯದಿ (vvp_island_port*resolver = dynamic_cast<vvp_island_port*>(fun)) {
            resolver->ಎಣಿಕೆ_drivers(idx, counts);
            ಪ್ರತ್ಯಾಗಮ;
      }

        /* If the functor is not a resolver, there is only one driver, so
           we can just interrogate the filter to get the driven value. */
      update_driver_counts(wire->driven_ಮೌಲ್ಯ(idx), counts);
}

ಶೂನ್ಯ vvp_net_fun_t::operator delete(ಶೂನ್ಯ*)
{
#ifndef CHECK_WITH_VALGRIND
      ಪ್ರತಿಜ್ಞಾ(0);
#endif
}


vvp_net_fil_t::vvp_net_fil_t()
{
      force_link_ = 0;
      force_propagate_ = ಅಸತ್ಯ;
      ಎಣಿಕೆ_filters += 1;
}

vvp_net_fil_t::~vvp_net_fil_t()
{
      ಪ್ರತಿಜ್ಞಾ(force_link_ == 0);
}

vvp_net_fil_t::prop_t vvp_net_fil_t::filter_vec4(ಧ್ರುವ vvp_vector4_t&,
                                                 vvp_vector4_t&,
                                                 unsigned, unsigned)
{
      ಪ್ರತ್ಯಾಗಮ PROP;
}

vvp_net_fil_t::prop_t vvp_net_fil_t::filter_vec8(ಧ್ರುವ vvp_vector8_t&,
                                                 vvp_vector8_t&,
                                                 unsigned, unsigned)
{
      ಪ್ರತ್ಯಾಗಮ PROP;
}

vvp_net_fil_t::prop_t vvp_net_fil_t::filter_real(ದ್ವಿಗುಣ&)
{
      ಪ್ರತ್ಯಾಗಮ PROP;
}

vvp_net_fil_t::prop_t vvp_net_fil_t::filter_string(ಧ್ರುವ string&)
{
      ಪ್ರತ್ಯಾಗಮ PROP;
}

vvp_net_fil_t::prop_t vvp_net_fil_t::filter_object(vvp_object_t&)
{
      ಪ್ರತ್ಯಾಗಮ PROP;
}

ಶೂನ್ಯ vvp_net_fil_t::force_mask(ಧ್ರುವ vvp_vector2_t&mask)
{
      ಯದಿ (force_mask_.size() == 0)
	    force_mask_ = vvp_vector2_t(vvp_vector2_t::FILL0, mask.size());

      ಪ್ರತಿಜ್ಞಾ(force_mask_.size() == mask.size());
      ಪ್ರತಿ (unsigned idx = 0 ; idx < mask.size() ; idx += 1) {
	    ಯದಿ (mask.value(idx) == 0)
		  ಅನುವರ್ತ;

	    force_mask_.set_bit(idx, 1);
	    force_propagate_ = ಸತ್ಯ;
      }
}

ಶೂನ್ಯ vvp_net_fil_t::release_mask(ಧ್ರುವ vvp_vector2_t&mask)
{
      ಯದಿ (force_mask_.size() == 0)
	    ಪ್ರತ್ಯಾಗಮ;

      ಪ್ರತಿಜ್ಞಾ(force_mask_.size() == mask.size());
      ಪ್ರತಿ (unsigned idx = 0 ; idx < mask.size() ; idx += 1) {
	    ಯದಿ (mask.value(idx))
		  force_mask_.set_bit(idx, 0);
      }

      ಯದಿ (force_mask_.is_zero())
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
ಶೂನ್ಯ vvp_net_fil_t::force_link(vvp_net_t*dst, vvp_net_t*src)
{
      ಪ್ರತಿಜ್ಞಾ(dst->fil == ಏತತ್);

      ಯದಿ (force_link_ == 0) {
	    force_link_ = ನೂತನ vvp_net_t;
	      // Use port[3] to hold the force destination.
	    force_link_->port[3] = vvp_net_ptr_t(dst, 0);
	    force_link_->port[2] = vvp_net_ptr_t(0,0);
	    force_link_->fun = ನೂತನ vvp_fun_force;
      }

      force_unlink();
      ಪ್ರತಿಜ್ಞಾ(force_link_->port[2] == vvp_net_ptr_t(0,0));

	// Use port[2] to hold the force source.
      force_link_->port[2] = vvp_net_ptr_t(src,0);

      vvp_net_ptr_t dst_ptr(force_link_, 0);
      src->link(dst_ptr);
}

ಶೂನ್ಯ vvp_net_fil_t::operator delete(ಶೂನ್ಯ*)
{
      ಪ್ರತಿಜ್ಞಾ(0);
}

ಶೂನ್ಯ vvp_net_fil_t::force_unlink(ಶೂನ್ಯ)
{
      ಯದಿ (force_link_ == 0) ಪ್ರತ್ಯಾಗಮ;
      vvp_net_t*src = force_link_->port[2].ptr();
      ಯದಿ (src == 0) ಪ್ರತ್ಯಾಗಮ;

      src->unlink(vvp_net_ptr_t(force_link_,0));
      force_link_->port[2] = vvp_net_ptr_t(0,0);
}

/* *** BIT operations *** */
vvp_bit4_t add_with_carry(vvp_bit4_t a, vvp_bit4_t b, vvp_bit4_t&c)
{
      ಯದಿ (bit4_is_xz(a) || bit4_is_xz(b) || bit4_is_xz(c)) {
	    c = BIT4_X;
	    ಪ್ರತ್ಯಾಗಮ BIT4_X;
      }

	// NOTE: This relies on the facts that XZ values have been
	// weeded out, and that BIT4_1 is 1 and BIT4_0 is 0.
      ಪೂರ್ಣಾಙ್ಕ sum = (ಪೂರ್ಣಾಙ್ಕ)a + (ಪೂರ್ಣಾಙ್ಕ)b + (ಪೂರ್ಣಾಙ್ಕ)c;

      ವಿಕಲ್ಪ (sum) {
	  ಪಕ್ಷ 0:
	      // c must already be 0.
	    ಪ್ರತ್ಯಾಗಮ BIT4_0;
	  ಪಕ್ಷ 1:
	    c = BIT4_0;
	    ಪ್ರತ್ಯಾಗಮ BIT4_1;
	  ಪಕ್ಷ 2:
	    c = BIT4_1;
	    ಪ್ರತ್ಯಾಗಮ BIT4_0;
	  ಪಕ್ಷ 3:
	    c = BIT4_1;
	    ಪ್ರತ್ಯಾಗಮ BIT4_1;
      }
      fprintf(stderr, "Incorrect result %d.\n", sum);
      ಪ್ರತಿಜ್ಞಾ(0);
      ಪ್ರತ್ಯಾಗಮ BIT4_X;
}

vvp_bit4_t scalar_to_bit4(PLI_INT32 scalar)
{
      ವಿಕಲ್ಪ(scalar) {
	  ಪಕ್ಷ vpi0:
	    ಪ್ರತ್ಯಾಗಮ BIT4_0;
	  ಪಕ್ಷ vpi1:
	    ಪ್ರತ್ಯಾಗಮ BIT4_1;
	  ಪಕ್ಷ vpiX:
	    ಪ್ರತ್ಯಾಗಮ BIT4_X;
	  ಪಕ್ಷ vpiZ:
	    ಪ್ರತ್ಯಾಗಮ BIT4_Z;
      }
      fprintf(stderr, "Unsupported scalar value %d.\n", (ಪೂರ್ಣಾಙ್ಕ)scalar);
      ಪ್ರತಿಜ್ಞಾ(0);
      ಪ್ರತ್ಯಾಗಮ BIT4_X;
}

vvp_bit4_t operator ^ (vvp_bit4_t a, vvp_bit4_t b)
{
      ಯದಿ (bit4_is_xz(a))
	    ಪ್ರತ್ಯಾಗಮ BIT4_X;
      ಯದಿ (bit4_is_xz(b))
	    ಪ್ರತ್ಯಾಗಮ BIT4_X;
      ಯದಿ (a == BIT4_0)
	    ಪ್ರತ್ಯಾಗಮ b;
      ಯದಿ (b == BIT4_0)
	    ಪ್ರತ್ಯಾಗಮ a;
      ಪ್ರತ್ಯಾಗಮ BIT4_0;
}

ostream& operator<<(ostream&out, vvp_bit4_t bit)
{
      ವಿಕಲ್ಪ (bit) {
	  ಪಕ್ಷ BIT4_0:
	    out << "0";
	    ಭಙ್ಗ;
	  ಪಕ್ಷ BIT4_1:
	    out << "1";
	    ಭಙ್ಗ;
	  ಪಕ್ಷ BIT4_X:
	    out << "X";
	    ಭಙ್ಗ;
	  ಪಕ್ಷ BIT4_Z:
	    out << "Z";
	    ಭಙ್ಗ;
	  ಸ್ವಭಾವ:
	    out << "?";
	    ಭಙ್ಗ;
      }
      ಪ್ರತ್ಯಾಗಮ out;
}

ಪ್ರಕಾರನಿರ್ದೇಶ unsigned short edge_t;

ಅನ್ತಃಸ್ಥ edge_t VVP_EDGE(vvp_bit4_t from, vvp_bit4_t to)
{
      ಪ್ರತ್ಯಾಗಮ 1 << ((from << 2) | to);
}

ಧ್ರುವ edge_t vvp_edge_posedge
      = VVP_EDGE(BIT4_0,BIT4_1)
      | VVP_EDGE(BIT4_0,BIT4_X)
      | VVP_EDGE(BIT4_0,BIT4_Z)
      | VVP_EDGE(BIT4_X,BIT4_1)
      | VVP_EDGE(BIT4_Z,BIT4_1)
      ;

ಧ್ರುವ edge_t vvp_edge_negedge
      = VVP_EDGE(BIT4_1,BIT4_0)
      | VVP_EDGE(BIT4_1,BIT4_X)
      | VVP_EDGE(BIT4_1,BIT4_Z)
      | VVP_EDGE(BIT4_X,BIT4_0)
      | VVP_EDGE(BIT4_Z,BIT4_0)
      ;

ಪೂರ್ಣಾಙ್ಕ edge(vvp_bit4_t from, vvp_bit4_t to)
{
      edge_t mask = VVP_EDGE(from, to);
      ಯದಿ (mask & vvp_edge_posedge)
	    ಪ್ರತ್ಯಾಗಮ 1;
      ಯದಿ (mask & vvp_edge_negedge)
	    ಪ್ರತ್ಯಾಗಮ -1;
      ಪ್ರತ್ಯಾಗಮ 0;
}

unsigned long multiply_with_carry(unsigned long a, unsigned long b,
				  unsigned long&carry)
{
      ಧ್ರುವ unsigned long mask = (1UL << (CPU_WORD_BITS/2)) - 1;
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
      ಪ್ರತ್ಯಾಗಮ (r1 << (CPU_WORD_BITS/2)) + r00;
}


ಶೂನ್ಯ vvp_send_vec8(vvp_net_ptr_t ptr, ಧ್ರುವ vvp_vector8_t&val)
{
      ಯಾವತ್ (vvp_net_t*cur = ptr.ptr()) {
	    vvp_net_ptr_t next_val = cur->port[ptr.port()];

	    ಯದಿ (cur->fun)
		  cur->fun->recv_vec8(ptr, val);

	    ptr = next_val;
      }
}

ಶೂನ್ಯ vvp_send_real(vvp_net_ptr_t ptr, ದ್ವಿಗುಣ val, vvp_context_t context)
{
      ಯಾವತ್ (vvp_net_t*cur = ptr.ptr()) {
	    vvp_net_ptr_t next_val = cur->port[ptr.port()];

	    ಯದಿ (cur->fun)
		  cur->fun->recv_real(ptr, val, context);

	    ptr = next_val;
      }
}

ಧ್ರುವ vvp_vector4_t vvp_vector4_t::nil;

ಶೂನ್ಯ vvp_vector4_t::copy_bits(ಧ್ರುವ vvp_vector4_t&that)
{

      ಯದಿ (size_ == that.size_) {
	    ಯದಿ (size_ > BITS_PER_WORD) {
		  unsigned ಪದಗಳು = (size_+BITS_PER_WORD-1) / BITS_PER_WORD;
		  ಪ್ರತಿ (unsigned idx = 0 ;  idx < ಪದಗಳು ;  idx += 1)
			abits_ptr_[idx] = that.abits_ptr_[idx];
		  ಪ್ರತಿ (unsigned idx = 0 ;  idx < ಪದಗಳು ;  idx += 1)
			bbits_ptr_[idx] = that.bbits_ptr_[idx];
	    } ಅನ್ಯಥಾ {
		  abits_val_ = that.abits_val_;
		  bbits_val_ = that.bbits_val_;
	    }
	    ಪ್ರತ್ಯಾಗಮ;
      }

	/* Now we know that the sizes of this and that are definitely
	   different. We can use that in code below. In any case, we
	   need to copy only the smaller of the sizes. */

	/* If source and destination are both short, then mask/copy
	   the bit values. */
      ಯದಿ (size_ <= BITS_PER_WORD && that.size_ <= BITS_PER_WORD) {
	    unsigned bits_to_copy = (that.size_ < size_) ? that.size_ : size_;
	    unsigned long mask = ((bits_to_copy < BITS_PER_WORD) ? (1UL << bits_to_copy) :
	                                                           (0UL)) - 1UL;
	    abits_val_ &= ~mask;
	    bbits_val_ &= ~mask;
	    abits_val_ |= that.abits_val_&mask;
	    bbits_val_ |= that.bbits_val_&mask;
	    ಪ್ರತ್ಯಾಗಮ;
      }

	/* Now we know that either source or destination are long. If
	   the destination is short, then mask/copy from the low word
	   of the long source. */
      ಯದಿ (size_ <= BITS_PER_WORD) {
	    abits_val_ = that.abits_ptr_[0];
	    bbits_val_ = that.bbits_ptr_[0];
	    ಯದಿ (size_ < BITS_PER_WORD) {
		  unsigned long mask = (1UL << size_) - 1UL;
		  abits_val_ &= mask;
		  bbits_val_ &= mask;
	    }
	    ಪ್ರತ್ಯಾಗಮ;
      }

	/* Now we know that the destination must be long. If the
	   source is short, then mask/copy from its value. */
      ಯದಿ (that.size_ <= BITS_PER_WORD) {
	    unsigned long mask;
	    ಯದಿ (that.size_ < BITS_PER_WORD) {
		  mask = (1UL << that.size_) - 1UL;
		  abits_ptr_[0] &= ~mask;
		  bbits_ptr_[0] &= ~mask;
	    } ಅನ್ಯಥಾ {
		  mask = -1UL;
	    }
	    abits_ptr_[0] |= that.abits_val_&mask;
	    bbits_ptr_[0] |= that.bbits_val_&mask;
	    ಪ್ರತ್ಯಾಗಮ;
      }

	/* Finally, we know that source and destination are long. copy
	   words until we get to the last. */
      unsigned bits_to_copy = (that.size_ < size_) ? that.size_ : size_;
      unsigned word = 0;
      ಯಾವತ್ (bits_to_copy >= BITS_PER_WORD) {
	    abits_ptr_[word] = that.abits_ptr_[word];
	    bbits_ptr_[word] = that.bbits_ptr_[word];
	    bits_to_copy -= BITS_PER_WORD;
	    word += 1;
      }
      ಯದಿ (bits_to_copy > 0) {
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
ಶೂನ್ಯ vvp_vector4_t::copy_from_big_(ಧ್ರುವ vvp_vector4_t&that)
{
      unsigned ಪದಗಳು = (size_+BITS_PER_WORD-1) / BITS_PER_WORD;
      abits_ptr_ = ನೂತನ unsigned long[2*ಪದಗಳು];
      bbits_ptr_ = abits_ptr_ + ಪದಗಳು;

      ಪ್ರತಿ (unsigned idx = 0 ;  idx < ಪದಗಳು ;  idx += 1)
	    abits_ptr_[idx] = that.abits_ptr_[idx];
      ಪ್ರತಿ (unsigned idx = 0 ;  idx < ಪದಗಳು ;  idx += 1)
	    bbits_ptr_[idx] = that.bbits_ptr_[idx];
}

/*
 * The copy_inverted_from_ method is just like the copy_from_ method,
 * except that we combine that with an invert. This allows the ~ and
 * the assignment to be blended in many common cases.
 */
ಶೂನ್ಯ vvp_vector4_t::copy_inverted_from_(ಧ್ರುವ vvp_vector4_t&that)
{
      size_ = that.size_;
      ಯದಿ (size_ > BITS_PER_WORD) {
	    unsigned ಪದಗಳು = (size_+BITS_PER_WORD-1) / BITS_PER_WORD;
	    abits_ptr_ = ನೂತನ unsigned long[2*ಪದಗಳು];
	    bbits_ptr_ = abits_ptr_ + ಪದಗಳು;

	    unsigned remaining = size_;
	    unsigned idx = 0;
	    ಯಾವತ್ (remaining >= BITS_PER_WORD) {
		  abits_ptr_[idx] = that.bbits_ptr_[idx] | ~that.abits_ptr_[idx];
		  idx += 1;
		  remaining -= BITS_PER_WORD;
	    }
	    ಯದಿ (remaining > 0) {
		  unsigned long mask = (1UL<<remaining) - 1UL;
		  abits_ptr_[idx] = mask & (that.bbits_ptr_[idx] | ~that.abits_ptr_[idx]);
	    }

	    ಪ್ರತಿ (idx = 0 ;  idx < ಪದಗಳು ;  idx += 1)
		  bbits_ptr_[idx] = that.bbits_ptr_[idx];

      } ಅನ್ಯಥಾ {
	    unsigned long mask = (size_<BITS_PER_WORD)? (1UL<<size_)-1UL : -1UL;
	    abits_val_ = mask & (that.bbits_val_ | ~that.abits_val_);
	    bbits_val_ = that.bbits_val_;
      }
}

/* Make sure to set size_ before calling this routine. */
ಶೂನ್ಯ vvp_vector4_t::allocate_ಪದಗಳು(unsigned long inita, unsigned long initb)
{
      ಯದಿ (size_ > BITS_PER_WORD) {
	    unsigned cnt = (size_ + BITS_PER_WORD - 1) / BITS_PER_WORD;
	    abits_ptr_ = ನೂತನ unsigned long[2*cnt];
	    bbits_ptr_ = abits_ptr_ + cnt;
	    ಪ್ರತಿ (unsigned idx = 0 ;  idx < cnt ;  idx += 1)
		  abits_ptr_[idx] = inita;
	    ಪ್ರತಿ (unsigned idx = 0 ;  idx < cnt ;  idx += 1)
		  bbits_ptr_[idx] = initb;

      } ಅನ್ಯಥಾ {
	    abits_val_ = inita;
	    bbits_val_ = initb;
      }
}

vvp_vector4_t::vvp_vector4_t(unsigned size__, ದ್ವಿಗುಣ val)
: size_(size__)
{
      ತಾರ್ಕಿಕ is_neg = ಅಸತ್ಯ;
      ದ್ವಿಗುಣ fraction;
      ಪೂರ್ಣಾಙ್ಕ exponent;

	/* We return 'bx for a NaN or +/- infinity. */
      ಯದಿ (val != val || (val && (val == 0.5*val)))  {
	    allocate_ಪದಗಳು(WORD_X_ABITS, WORD_X_BBITS);
	    ಪ್ರತ್ಯಾಗಮ;
      }

	/* Convert to a positive result. */
      ಯದಿ (val < 0.0) {
	    is_neg = ಸತ್ಯ;
	    val = -val;
      }
      allocate_ಪದಗಳು(WORD_0_ABITS, WORD_0_BBITS);

	/* Get the exponent and fractional part of the number. */
      fraction = frexp(val, &exponent);

	/* If the value is small enough just use lround(). */
      ಯದಿ (exponent < BITS_PER_WORD-2) {
	    ಯದಿ (is_neg) ಏತತ್->invert();  // Invert the bits if negative.
	    long sval = lround(val);
	    ಯದಿ (is_neg) sval = -sval;
	      /* This requires that 0 and 1 have the same bbit value. */
	    ಯದಿ (size_ > BITS_PER_WORD) {
		  abits_ptr_[0] = sval;
	    } ಅನ್ಯಥಾ {
		  abits_val_ = sval;
	    }
	    ಪ್ರತ್ಯಾಗಮ;
      }

      unsigned nwords = (exponent-1) / BITS_PER_WORD;
      unsigned my_ಪದಗಳು = (size_ + BITS_PER_WORD - 1) / BITS_PER_WORD - 1;

      fraction = ldexp(fraction, (exponent-1) % BITS_PER_WORD + 1);

	/* Skip any leading bits. */
      ಪ್ರತಿ (ಪೂರ್ಣಾಙ್ಕ idx = (signed) nwords; idx > (signed) my_ಪದಗಳು; idx -=1) {
	    unsigned long bits = (unsigned long) fraction;
	    fraction = fraction - (ದ್ವಿಗುಣ) bits;
	    fraction = ldexp(fraction, BITS_PER_WORD);
      }

	/* Convert the remaining bits as appropriate. */
      ಯದಿ (my_ಪದಗಳು == 0) {
		  unsigned long bits = (unsigned long) fraction;
		  abits_val_ = bits;
		  fraction = fraction - (ದ್ವಿಗುಣ) bits;
		    /* Round any fractional part up. */
		  ಯದಿ (fraction >= 0.5) *ಏತತ್ += (int64_t) 1;
      } ಅನ್ಯಥಾ {
	    ಯದಿ (nwords < my_ಪದಗಳು) my_ಪದಗಳು = nwords;
	    ಪ್ರತಿ (ಪೂರ್ಣಾಙ್ಕ idx = (signed)my_ಪದಗಳು; idx >= 0; idx -= 1) {
		  unsigned long bits = (unsigned long) fraction;
		  abits_ptr_[idx] = bits;
		  fraction = fraction - (ದ್ವಿಗುಣ) bits;
		  fraction = ldexp(fraction, BITS_PER_WORD);
	    }
	      /* Round any fractional part up. */
	    ಯದಿ (fraction >= ldexp(0.5, BITS_PER_WORD)) *ಏತತ್ += (int64_t) 1;
       }

	/* Convert to a negative number if needed. */
      ಯದಿ (is_neg) {
	    ಏತತ್->invert();
	    *ಏತತ್ += (int64_t) 1;
      }
}

vvp_vector4_t::vvp_vector4_t(ಧ್ರುವ vvp_vector4_t&that,
			    unsigned adr, unsigned wid)
{
	// Set up and initialize the destination.
      size_ = wid;
      allocate_ಪದಗಳು(WORD_X_ABITS, WORD_X_BBITS);

	// Special case: selecting from far beyond the source vector,
	// to the result is all X bits. We're done.
      ಯದಿ (adr >= that.size_)
	    ಪ್ರತ್ಯಾಗಮ;

	// Special case: The source is not quite big enough to supply
	// all bits, so get the bits that we can. The remainder will
	// be left at BIT4_X.
      ಯದಿ ((adr + wid) > that.size_) {
	    unsigned use_wid = that.size_ - adr;
	    ಪ್ರತಿ (unsigned idx = 0 ; idx < use_wid ; idx += 1)
		  set_bit(idx, that.value(adr+idx));

	    ಪ್ರತ್ಯಾಗಮ;
      }

	// At the point, we know that the source part is entirely
	// contained in the source vector.
	// assert((adr + wid) <= that.size_);

      ಯದಿ (wid > BITS_PER_WORD) {
	      /* In this case, the subvector and the source vector are
		 long. Do the transfer reasonably efficiently. */
	    unsigned ptr = adr / BITS_PER_WORD;
	    unsigned long off = adr % BITS_PER_WORD;
	    unsigned long noff = BITS_PER_WORD - off;
	    unsigned long lmask = (1UL << off) - 1UL;
	    unsigned trans = 0;
	    unsigned dst = 0;
	    ಯಾವತ್ (trans < wid) {
		    // The low bits of the result.
		  abits_ptr_[dst] = (that.abits_ptr_[ptr] & ~lmask) >> off;
		  bbits_ptr_[dst] = (that.bbits_ptr_[ptr] & ~lmask) >> off;
		  trans += noff;

		  ಯದಿ (trans >= wid)
			ಭಙ್ಗ;

		  ptr += 1;

		    // The high bits of the result. Skip this if the
		    // source and destination are perfectly aligned.
		  ಯದಿ (noff != BITS_PER_WORD) {
			abits_ptr_[dst] |= (that.abits_ptr_[ptr]&lmask) << noff;
			bbits_ptr_[dst] |= (that.bbits_ptr_[ptr]&lmask) << noff;
			trans += off;
		  }

		  dst += 1;
	    }

      } ಅನ್ಯಥಾ ಯದಿ (that.size_ > BITS_PER_WORD) {
	      /* In this case, the subvector fits in a single word,
		 but the source is large. */
	    unsigned ptr = adr / BITS_PER_WORD;
	    unsigned long off = adr % BITS_PER_WORD;
	    unsigned trans = BITS_PER_WORD - off;
	    ಯದಿ (trans > wid)
		  trans = wid;

	    ಯದಿ (trans == BITS_PER_WORD) {
		    // Very special case: Copy exactly 1 perfectly
		    // aligned word.
		  abits_val_ = that.abits_ptr_[ptr];
		  bbits_val_ = that.bbits_ptr_[ptr];

	    } ಅನ್ಯಥಾ {
		    // lmask is the low bits of the destination,
		    // masked into the source.
		  unsigned long lmask = (1UL<<trans) - 1UL;
		  lmask <<= off;

		    // The low bits of the result.
		  abits_val_ = (that.abits_ptr_[ptr] & lmask) >> off;
		  bbits_val_ = (that.bbits_ptr_[ptr] & lmask) >> off;

		  ಯದಿ (trans < wid) {
			  // If there are more bits, then get them
			  // from the bottom of the next word of the
			  // source.
			unsigned long hmask = (1UL << (wid-trans)) - 1UL;

			  // The high bits of the result.
			abits_val_ |= (that.abits_ptr_[ptr+1]&hmask) << trans;
			bbits_val_ |= (that.bbits_ptr_[ptr+1]&hmask) << trans;
		  }
	    }

      } ಅನ್ಯಥಾ ಯದಿ (size_ == BITS_PER_WORD) {
	      /* We know that source and destination are short. If the
		 destination is a full word, then we know the copy is
		 aligned and complete. */
	    abits_val_ = that.abits_val_;
	    bbits_val_ = that.bbits_val_;

      } ಅನ್ಯಥಾ {
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
ಶೂನ್ಯ vvp_vector4_t::resize(unsigned newsize, vvp_bit4_t pad_bit)
{
      ಯದಿ (size_ == newsize)
	    ಪ್ರತ್ಯಾಗಮ;

      unsigned cnt = (size_ + BITS_PER_WORD - 1) / BITS_PER_WORD;
      unsigned long word_pad_abits = 0;
      unsigned long word_pad_bbits = 0;
      ವಿಕಲ್ಪ (pad_bit) {
	  ಪಕ್ಷ BIT4_0:
	    word_pad_abits = WORD_0_ABITS;
	    word_pad_bbits = WORD_0_BBITS;
	    ಭಙ್ಗ;
	  ಪಕ್ಷ BIT4_1:
	    word_pad_abits = WORD_1_ABITS;
	    word_pad_bbits = WORD_1_BBITS;
	    ಭಙ್ಗ;
	  ಪಕ್ಷ BIT4_X:
	    word_pad_abits = WORD_X_ABITS;
	    word_pad_bbits = WORD_X_BBITS;
	    ಭಙ್ಗ;
	  ಪಕ್ಷ BIT4_Z:
	    word_pad_abits = WORD_Z_ABITS;
	    word_pad_bbits = WORD_Z_BBITS;
	    ಭಙ್ಗ;
      }

      ಯದಿ (newsize > BITS_PER_WORD) {
	    unsigned newcnt = (newsize + BITS_PER_WORD - 1) / BITS_PER_WORD;
	    ಯದಿ (newcnt == cnt) {
		    // If the word count doesn't change, then there is
		    // no need for re-allocation so we are done now.
		  ಯದಿ (newsize > size_) {
			ಯದಿ (unsigned fill = size_ % BITS_PER_WORD) {
			      abits_ptr_[cnt-1] &= ~((-1UL) << fill);
			      bbits_ptr_[cnt-1] &= ~((-1UL) << fill);
			      abits_ptr_[cnt-1] |= word_pad_abits << fill;
			      bbits_ptr_[cnt-1] |= word_pad_bbits << fill;
			}
		  }
		  size_ = newsize;
		  ಪ್ರತ್ಯಾಗಮ;
	    }

	    unsigned long*newbits = ನೂತನ unsigned long[2*newcnt];

	    ಯದಿ (cnt > 1) {
		  unsigned trans = cnt;
		  ಯದಿ (trans > newcnt)
			trans = newcnt;

		  ಪ್ರತಿ (unsigned idx = 0 ;  idx < trans ;  idx += 1)
			newbits[idx] = abits_ptr_[idx];
		  ಪ್ರತಿ (unsigned idx = 0 ;  idx < trans ;  idx += 1)
			newbits[newcnt+idx] = bbits_ptr_[idx];

		  delete[]abits_ptr_;

	    } ಅನ್ಯಥಾ {
		  newbits[0] = abits_val_;
		  newbits[newcnt] = bbits_val_;
	    }

	    ಯದಿ (newsize > size_) {
		  ಯದಿ (unsigned fill = size_ % BITS_PER_WORD) {
			newbits[cnt-1] &= ~((-1UL) << fill);
			newbits[cnt-1] |= word_pad_abits << fill;
			newbits[newcnt+cnt-1] &= ~((-1UL) << fill);
			newbits[newcnt+cnt-1] |= word_pad_bbits << fill;
		  }
		  ಪ್ರತಿ (unsigned idx = cnt ;  idx < newcnt ;  idx += 1)
			newbits[idx] = word_pad_abits;
		  ಪ್ರತಿ (unsigned idx = cnt ;  idx < newcnt ;  idx += 1)
			newbits[newcnt+idx] = word_pad_bbits;
	    }

	    size_ = newsize;
	    abits_ptr_ = newbits;
	    bbits_ptr_ = newbits + newcnt;

      } ಅನ್ಯಥಾ {
	    ಯದಿ (cnt > 1) {
		  unsigned long newvala = abits_ptr_[0];
		  unsigned long newvalb = bbits_ptr_[0];
		  delete[]abits_ptr_;
		  abits_val_ = newvala;
		  bbits_val_ = newvalb;
	    }

	    ಯದಿ (newsize > size_) {
		  abits_val_ &= ~((-1UL) << size_);
		  bbits_val_ &= ~((-1UL) << size_);
		  abits_val_ |= word_pad_abits << size_;
		  bbits_val_ |= word_pad_bbits << size_;
	    }

	    size_ = newsize;
      }
}


unsigned long* vvp_vector4_t::subarray(unsigned adr, unsigned wid, ತಾರ್ಕಿಕ xz_to_0) ಧ್ರುವ
{
      ಧ್ರುವ unsigned BIT2_PER_WORD = 8*ಪರಿಮಾಣ(unsigned long);
      unsigned awid = (wid + BIT2_PER_WORD - 1) / (BIT2_PER_WORD);
      unsigned long*val = ನೂತನ unsigned long[awid];

      ಪ್ರತಿ (unsigned idx = 0 ;  idx < awid ;  idx += 1)
	    val[idx] = 0;

      ಯದಿ (size_ <= BITS_PER_WORD) {
	      /* Handle the special case that the array is small. The
		 entire value of the vector4 is within the xbits_val_
		 so we know that the result is a single word, the
		 source is a single word, and we just have to loop
		 through that word. */
	    unsigned long atmp = abits_val_ >> adr;
	    unsigned long btmp = bbits_val_ >> adr;
	    ಯದಿ (wid < BIT2_PER_WORD) {
		  atmp &= (1UL << wid) - 1;
		  btmp &= (1UL << wid) - 1;
	    }
	    ಯದಿ (btmp) {
		  ಯದಿ (xz_to_0)
			atmp &= ~btmp;
		  ಅನ್ಯಥಾ
			ಗಚ್ಛ x_out;
	    }

	    val[0] = atmp;

      } ಅನ್ಯಥಾ {

	    unsigned val_ptr = 0;
	    unsigned val_off = 0;

	      /* Get the first word we are scanning. We may in fact be
		 somewhere in the middle of that word. */
	    ಯಾವತ್ (wid > 0) {
		  unsigned long atmp = abits_ptr_[adr/BITS_PER_WORD];
		  unsigned long btmp = bbits_ptr_[adr/BITS_PER_WORD];
		  unsigned long off = adr%BITS_PER_WORD;
		  atmp >>= off;
		  btmp >>= off;

		  unsigned long trans = BITS_PER_WORD - off;
		  ಯದಿ (trans > (BIT2_PER_WORD - val_off))
			trans = BIT2_PER_WORD - val_off;
		  ಯದಿ (wid < trans)
			trans = wid;
		  ಯದಿ (trans < BIT2_PER_WORD) {
			atmp &= (1UL << trans) - 1;
			btmp &= (1UL << trans) - 1;
		  }
		  ಯದಿ (btmp) {
			ಯದಿ (xz_to_0)
			      atmp &= ~btmp;
			ಅನ್ಯಥಾ
			      ಗಚ್ಛ x_out;
		  }

		  val[val_ptr] |= atmp << val_off;
		  adr += trans;
		  wid -= trans;
		  val_off += trans;
		  ಯದಿ (val_off == BIT2_PER_WORD) {
			val_ptr += 1;
			val_off = 0;
		  }
	    }
      }

      ಪ್ರತ್ಯಾಗಮ val;

 x_out:
      delete[]val;
      ಪ್ರತ್ಯಾಗಮ 0;
}

ಶೂನ್ಯ vvp_vector4_t::setarray(unsigned adr, unsigned wid, ಧ್ರುವ unsigned long*val)
{
      ಪ್ರತಿಜ್ಞಾ(adr+wid <= size_);

      ಧ್ರುವ unsigned BIT2_PER_WORD = 8*ಪರಿಮಾಣ(unsigned long);

      ಯದಿ (size_ <= BITS_PER_WORD) {
	      // We know here that both the source and the target are
	      // within a single word. Write the bits into the
	      // abits_val_ directly.

	    ಪ್ರತಿಜ್ಞಾ(BIT2_PER_WORD <= BITS_PER_WORD);
	    unsigned long lmask = (1UL << adr) - 1UL;
	    unsigned long hmask = ((adr+wid) < BITS_PER_WORD)
		  ? -1UL << (adr+wid)
		  : 0;
	    unsigned long mask = ~(hmask | lmask);

	    abits_val_ &= ~mask;
	    bbits_val_ &= ~mask;

	    abits_val_ |= mask & (val[0] << adr);

      } ಅನ್ಯಥಾ {
	      // The general case, there are multiple words of
	      // destination, and possibly multiple words of source
	      // data. Shift and mask as we go.
	    unsigned off = adr % BITS_PER_WORD;
	    unsigned ptr = adr / BITS_PER_WORD;
	    unsigned val_off = 0;
	    unsigned val_ptr = 0;
	    ಯಾವತ್ (wid > 0) {
		  unsigned trans = wid;
		  ಯದಿ (trans > (BIT2_PER_WORD-val_off))
			trans = BIT2_PER_WORD-val_off;
		  ಯದಿ (trans > (BITS_PER_WORD-off))
			trans = BITS_PER_WORD-off;

		  unsigned long lmask = (1UL << off) - 1UL;
		  unsigned long hmask = ((off+trans) < BITS_PER_WORD)
			? -1UL << (off+trans)
			: 0;
		  unsigned long mask = ~(hmask | lmask);

		  abits_ptr_[ptr] &= ~mask;
		  bbits_ptr_[ptr] &= ~mask;
		  ಯದಿ (val_off >= off)
			abits_ptr_[ptr] |= mask & (val[val_ptr] >> (val_off-off));
		  ಅನ್ಯಥಾ
			abits_ptr_[ptr] |= mask & (val[val_ptr] << (off-val_off));

		  wid -= trans;
		  val_off += trans;
		  ಯದಿ (val_off == BIT2_PER_WORD) {
			val_off = 0;
			val_ptr += 1;
		  }
		  off += trans;
		  ಯದಿ (off == BITS_PER_WORD) {
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
ತಾರ್ಕಿಕ vvp_vector4_t::set_vec(unsigned adr, ಧ್ರುವ vvp_vector4_t&that)
{
      ಪ್ರತಿಜ್ಞಾ(adr+that.size_  <= size_);
      ತಾರ್ಕಿಕ diff_flag = ಅಸತ್ಯ;

      ಯದಿ (size_ <= BITS_PER_WORD) {

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
	    ಯದಿ (hshift >= BITS_PER_WORD)
		  hmask = -1UL;
	    ಅನ್ಯಥಾ
		  hmask = (1UL << (adr+that.size_)) - 1;
	    unsigned long mask = hmask & ~lmask;

	    unsigned long tmp = (that.abits_val_<<adr)&mask;
	    ಯದಿ ((abits_val_&mask) != tmp) {
		  diff_flag = ಸತ್ಯ;
		  abits_val_ = (abits_val_ & ~mask) | tmp;
	    }
	    tmp = (that.bbits_val_<<adr) & mask;
	    ಯದಿ ((bbits_val_&mask) != tmp) {
		  diff_flag = ಸತ್ಯ;
		  bbits_val_ = (bbits_val_ & ~mask) | tmp;
	    }

      } ಅನ್ಯಥಾ ಯದಿ (that.size_ <= BITS_PER_WORD) {

	      /* This vector is more than a word, but that vector is
		 still small. Write into the destination, possibly
		 spanning two destination words, depending on whether
		 the source vector spans a word transition. */
	    unsigned long dptr = adr / BITS_PER_WORD;
	    unsigned long doff = adr % BITS_PER_WORD;

	    unsigned long lmask = (1UL << doff) - 1;
	    unsigned long hshift = doff+that.size_;
	    unsigned long hmask;
	    ಯದಿ (hshift >= BITS_PER_WORD)
		  hmask = -1UL;
	    ಅನ್ಯಥಾ
		  hmask = (1UL << hshift) - 1UL;

	    unsigned long mask = hmask & ~lmask;
	    unsigned long tmp;

	    tmp = (that.abits_val_ << doff) & mask;
	    ಯದಿ ((abits_ptr_[dptr] & mask) != tmp) {
		  diff_flag = ಸತ್ಯ;
		  abits_ptr_[dptr] = (abits_ptr_[dptr] & ~mask) | tmp;
	    }
	    tmp = (that.bbits_val_ << doff) & mask;
	    ಯದಿ ((bbits_ptr_[dptr] & mask) != tmp) {
		  diff_flag = ಸತ್ಯ;
		  bbits_ptr_[dptr] = (bbits_ptr_[dptr] & ~mask) | tmp;
	    }

	    ಯದಿ ((doff + that.size_) > BITS_PER_WORD) {
		  unsigned tail = doff + that.size_ - BITS_PER_WORD;
		  mask = (1UL << tail) - 1;

		  dptr += 1;
		  tmp = (that.abits_val_ >> (that.size_-tail)) & mask;
		  ಯದಿ ((abits_ptr_[dptr] & mask) != tmp) {
			diff_flag = ಸತ್ಯ;
			abits_ptr_[dptr] = (abits_ptr_[dptr] & ~mask) | tmp;
		  }
		  tmp = (that.bbits_val_ >> (that.size_-tail)) & mask;
		  ಯದಿ ((bbits_ptr_[dptr] & mask) != tmp) {
			diff_flag = ಸತ್ಯ;
			bbits_ptr_[dptr] = (bbits_ptr_[dptr] & ~mask) | tmp;
		  }
	    }

      } ಅನ್ಯಥಾ ಯದಿ (adr%BITS_PER_WORD == 0) {

	      /* In this case, both vectors are long, but the
		 destination is neatly aligned. That means all but the
		 last word can be simply copied with no masking. */

	    unsigned remain = that.size_;
	    unsigned sptr = 0;
	    unsigned dptr = adr / BITS_PER_WORD;
	    ಯಾವತ್ (remain >= BITS_PER_WORD) {
		  ಯದಿ (abits_ptr_[dptr] != that.abits_ptr_[sptr]) {
			diff_flag = ಸತ್ಯ;
			abits_ptr_[dptr] = that.abits_ptr_[sptr];
		  }
		  ಯದಿ (bbits_ptr_[dptr] != that.bbits_ptr_[sptr]) {
			diff_flag = ಸತ್ಯ;
			bbits_ptr_[dptr] = that.bbits_ptr_[sptr];
		  }
		  dptr += 1;
		  sptr += 1;
		  remain -= BITS_PER_WORD;
	    }

	    ಯದಿ (remain > 0) {
		  unsigned long mask = (1UL << remain) - 1;
		  unsigned long tmp;

		  tmp = that.abits_ptr_[sptr] & mask;
		  ಯದಿ ((abits_ptr_[dptr] & mask) != tmp) {
			diff_flag = ಸತ್ಯ;
			abits_ptr_[dptr] = (abits_ptr_[dptr] & ~mask) | tmp;
		  }
		  tmp = that.bbits_ptr_[sptr] & mask;
		  ಯದಿ ((bbits_ptr_[dptr] & mask) != tmp) {
			diff_flag = ಸತ್ಯ;
			bbits_ptr_[dptr] = (bbits_ptr_[dptr] & ~mask) | tmp;
		  }
	    }

      } ಅನ್ಯಥಾ {

	      /* We know that there are two long vectors, and we know
		 that the destination is definitely NOT aligned. */

	    unsigned remain = that.size_;
	    unsigned sptr = 0;
	    unsigned dptr = adr / BITS_PER_WORD;
	    unsigned doff = adr % BITS_PER_WORD;
	    unsigned long lmask = (1UL << doff) - 1;
	    unsigned ndoff = BITS_PER_WORD - doff;
	    ಯಾವತ್ (remain >= BITS_PER_WORD) {
		  unsigned long tmp;

		  tmp = (that.abits_ptr_[sptr] << doff) & ~lmask;
		  ಯದಿ ((abits_ptr_[dptr] & ~lmask) != tmp) {
			diff_flag = ಸತ್ಯ;
			abits_ptr_[dptr] = (abits_ptr_[dptr] & lmask) | tmp;
		  }
		  tmp = (that.bbits_ptr_[sptr] << doff) & ~lmask;
		  ಯದಿ ((bbits_ptr_[dptr] & ~lmask) != tmp) {
			diff_flag = ಸತ್ಯ;
			bbits_ptr_[dptr] = (bbits_ptr_[dptr] & lmask) | tmp;
		  }
		  dptr += 1;

		  tmp = (that.abits_ptr_[sptr] >> ndoff) & lmask;
		  ಯದಿ ((abits_ptr_[dptr] & lmask) != tmp) {
			diff_flag = ಸತ್ಯ;
			abits_ptr_[dptr] = (abits_ptr_[dptr] & ~lmask) | tmp;
		  }
		  tmp = (that.bbits_ptr_[sptr] >> ndoff) & lmask;
		  ಯದಿ ((bbits_ptr_[dptr] & lmask) != tmp) {
			diff_flag = ಸತ್ಯ;
			bbits_ptr_[dptr] = (bbits_ptr_[dptr] & ~lmask) | tmp;
		  }

		  remain -= BITS_PER_WORD;
		  sptr += 1;
	    }

	    ಯದಿ (remain > 0) {
		  unsigned long hshift = doff+remain;
		  unsigned long hmask;
		  ಯದಿ (hshift >= BITS_PER_WORD)
			hmask = -1UL;
		  ಅನ್ಯಥಾ
			hmask = (1UL << (doff+remain)) - 1;

		  unsigned long mask = hmask & ~lmask;
		  unsigned long tmp;

		  tmp = (that.abits_ptr_[sptr] << doff) & mask;
		  ಯದಿ ((abits_ptr_[dptr] & mask) != tmp) {
			diff_flag = ಸತ್ಯ;
			abits_ptr_[dptr] = (abits_ptr_[dptr] & ~mask) | tmp;
		  }
		  tmp = (that.bbits_ptr_[sptr] << doff) & mask;
		  ಯದಿ ((bbits_ptr_[dptr] & mask) != tmp) {
			diff_flag = ಸತ್ಯ;
			bbits_ptr_[dptr] = (bbits_ptr_[dptr] & ~mask) | tmp;
		  }

		  ಯದಿ ((doff + remain) > BITS_PER_WORD) {
			unsigned tail = doff + remain - BITS_PER_WORD;
			ಯದಿ (tail >= BITS_PER_WORD)
			      mask = -1UL;
			ಅನ್ಯಥಾ
			      mask = (1UL << tail) - 1;

			dptr += 1;

			tmp = (that.abits_ptr_[sptr] >> (remain-tail))&mask;
			ಯದಿ ((abits_ptr_[dptr] & mask) != tmp) {
			      diff_flag = ಸತ್ಯ;
			      abits_ptr_[dptr] = (abits_ptr_[dptr] & ~mask) | tmp;
			}
			tmp = (that.bbits_ptr_[sptr] >> (remain-tail))&mask;
			ಯದಿ ((bbits_ptr_[dptr] & mask) != tmp) {
			      diff_flag = ಸತ್ಯ;
			      bbits_ptr_[dptr] = (bbits_ptr_[dptr] & ~mask) | tmp;
			}
		  }
	    }
      }

      ಪ್ರತ್ಯಾಗಮ diff_flag;
}

/*
 * Add that vector to this vector. Do it in the Verilog way, which
 * means if we detect any X or Z bits, change the entire results to
 * all X.
 *
 * Assume both vectors are the same size.
 */
ಶೂನ್ಯ vvp_vector4_t::add(ಧ್ರುವ vvp_vector4_t&that)
{
      ಪ್ರತಿಜ್ಞಾ(size_ == that.size_);

      ಯದಿ (size_ < BITS_PER_WORD) {
	    unsigned long mask = ~(-1UL << size_);
	    ಯದಿ ((bbits_val_|that.bbits_val_) & mask) {
		  abits_val_ |= mask;
		  bbits_val_ |= mask;
		  ಪ್ರತ್ಯಾಗಮ;
	    }

	    abits_val_ += that.abits_val_;
	    abits_val_ &= mask;
	    ಪ್ರತ್ಯಾಗಮ;
      }

      ಯದಿ (size_ == BITS_PER_WORD) {
	    ಯದಿ (bbits_val_ | that.bbits_val_) {
		  abits_val_ = WORD_X_ABITS;
		  bbits_val_ = WORD_X_BBITS;
	    } ಅನ್ಯಥಾ {
		  abits_val_ += that.abits_val_;
	    }
	    ಪ್ರತ್ಯಾಗಮ;
      }

      ಪೂರ್ಣಾಙ್ಕ cnt = size_ / BITS_PER_WORD;
      unsigned long carry = 0;
      ಪ್ರತಿ (ಪೂರ್ಣಾಙ್ಕ idx = 0 ; idx < cnt ; idx += 1) {
	    ಯದಿ (bbits_ptr_[idx] | that.bbits_ptr_[idx])
		  ಗಚ್ಛ x_out;

	    abits_ptr_[idx] = add_with_carry(abits_ptr_[idx], that.abits_ptr_[idx], carry);
      }

      ಯದಿ (unsigned tail = size_ % BITS_PER_WORD) {
	    unsigned long mask = ~( -1UL << tail );
	    ಯದಿ ((bbits_ptr_[cnt] | that.bbits_ptr_[cnt])&mask)
		  ಗಚ್ಛ x_out;

	    abits_ptr_[cnt] = add_with_carry(abits_ptr_[cnt], that.abits_ptr_[cnt], carry);
	    abits_ptr_[cnt] &= mask;
      }

      ಪ್ರತ್ಯಾಗಮ;

 x_out:
      ಪ್ರತಿ (ಪೂರ್ಣಾಙ್ಕ idx = 0 ; idx < cnt ; idx += 1) {
	    abits_ptr_[idx] = WORD_X_ABITS;
	    bbits_ptr_[idx] = WORD_X_BBITS;
      }
      ಯದಿ (unsigned tail = size_%BITS_PER_WORD) {
	    unsigned long mask = ~( -1UL << tail );
	    abits_ptr_[cnt] = WORD_X_ABITS&mask;
	    bbits_ptr_[cnt] = WORD_X_BBITS&mask;
      }
}

ಶೂನ್ಯ vvp_vector4_t::sub(ಧ್ರುವ vvp_vector4_t&that)
{
      ಪ್ರತಿಜ್ಞಾ(size_ == that.size_);

      ಯದಿ (size_ < BITS_PER_WORD) {
	    unsigned long mask = ~(-1UL << size_);
	    ಯದಿ ((bbits_val_|that.bbits_val_) & mask) {
		  abits_val_ |= mask;
		  bbits_val_ |= mask;
		  ಪ್ರತ್ಯಾಗಮ;
	    }

	    abits_val_ -= that.abits_val_;
	    abits_val_ &= mask;
	    ಪ್ರತ್ಯಾಗಮ;
      }

      ಯದಿ (size_ == BITS_PER_WORD) {
	    ಯದಿ (bbits_val_ | that.bbits_val_) {
		  abits_val_ = WORD_X_ABITS;
		  bbits_val_ = WORD_X_BBITS;
	    } ಅನ್ಯಥಾ {
		  abits_val_ -= that.abits_val_;
	    }
	    ಪ್ರತ್ಯಾಗಮ;
      }

      ಪೂರ್ಣಾಙ್ಕ cnt = size_ / BITS_PER_WORD;
      unsigned long carry = 1;
      ಪ್ರತಿ (ಪೂರ್ಣಾಙ್ಕ idx = 0 ; idx < cnt ; idx += 1) {
	    ಯದಿ (bbits_ptr_[idx] | that.bbits_ptr_[idx])
		  ಗಚ್ಛ x_out;

	    abits_ptr_[idx] = add_with_carry(abits_ptr_[idx], ~that.abits_ptr_[idx], carry);
      }

      ಯದಿ (unsigned tail = size_ % BITS_PER_WORD) {
	    unsigned long mask = ~( -1UL << tail );
	    ಯದಿ ((bbits_ptr_[cnt] | that.bbits_ptr_[cnt])&mask)
		  ಗಚ್ಛ x_out;

	    abits_ptr_[cnt] = add_with_carry(abits_ptr_[cnt], ~that.abits_ptr_[cnt], carry);
	    abits_ptr_[cnt] &= mask;
      }

      ಪ್ರತ್ಯಾಗಮ;

 x_out:
      ಪ್ರತಿ (ಪೂರ್ಣಾಙ್ಕ idx = 0 ; idx < cnt ; idx += 1) {
	    abits_ptr_[idx] = WORD_X_ABITS;
	    bbits_ptr_[idx] = WORD_X_BBITS;
      }
      ಯದಿ (unsigned tail = size_%BITS_PER_WORD) {
	    unsigned long mask = ~( -1UL << tail );
	    abits_ptr_[cnt] = WORD_X_ABITS&mask;
	    bbits_ptr_[cnt] = WORD_X_BBITS&mask;
      }

}

ಶೂನ್ಯ vvp_vector4_t::mov(unsigned dst, unsigned src, unsigned cnt)
{
      ಪ್ರತಿಜ್ಞಾ(dst+cnt <= size_);
      ಪ್ರತಿಜ್ಞಾ(src+cnt <= size_);

      ಯದಿ (size_ <= BITS_PER_WORD) {
	    unsigned long vmask = (1UL << cnt) - 1;
	    unsigned long tmp;

	    tmp = (abits_val_ >> src) & vmask;
	    abits_val_ &= ~ (vmask << dst);
	    abits_val_ |= tmp << dst;

	    tmp = (bbits_val_ >> src) & vmask;
	    bbits_val_ &= ~ (vmask << dst);
	    bbits_val_ |= tmp << dst;

      } ಅನ್ಯಥಾ {
	    unsigned sptr = src / BITS_PER_WORD;
	    unsigned dptr = dst / BITS_PER_WORD;
	    unsigned soff = src % BITS_PER_WORD;
	    unsigned doff = dst % BITS_PER_WORD;

	    ಯಾವತ್ (cnt > 0) {
		  unsigned trans = cnt;
		  ಯದಿ ((soff+trans) > BITS_PER_WORD)
			trans = BITS_PER_WORD - soff;

		  ಯದಿ ((doff+trans) > BITS_PER_WORD)
			trans = BITS_PER_WORD - doff;

		  ಯದಿ (trans == BITS_PER_WORD) {
			  // Special case: the transfer count is
			  // exactly an entire word. For this to be
			  // true, it must also be true that the
			  // pointers are aligned. The work is easy,
			abits_ptr_[dptr] = abits_ptr_[sptr];
			bbits_ptr_[dptr] = bbits_ptr_[sptr];
			dptr += 1;
			sptr += 1;
			cnt -= BITS_PER_WORD;
			ಅನುವರ್ತ;
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
		  ಯದಿ (soff >= BITS_PER_WORD) {
			soff = 0;
			sptr += 1;
		  }
		  doff += trans;
		  ಯದಿ (doff >= BITS_PER_WORD) {
			doff = 0;
			dptr += 1;
		  }
	    }
      }
}

ಶೂನ್ಯ vvp_vector4_t::mul(ಧ್ರುವ vvp_vector4_t&that)
{
      ಪ್ರತಿಜ್ಞಾ(size_ == that.size_);

      ಯದಿ (size_ < BITS_PER_WORD) {
	    unsigned long mask = ~(-1UL << size_);
	    ಯದಿ ((bbits_val_|that.bbits_val_) & mask) {
		  abits_val_ |= mask;
		  bbits_val_ |= mask;
		  ಪ್ರತ್ಯಾಗಮ;
	    }

	    abits_val_ *= that.abits_val_;
	    abits_val_ &= mask;
	    ಪ್ರತ್ಯಾಗಮ;
      }

      ಯದಿ (size_ == BITS_PER_WORD) {
	    ಯದಿ (bbits_val_ || that.bbits_val_) {
		  abits_val_ = WORD_X_ABITS;
		  bbits_val_ = WORD_X_BBITS;
	    } ಅನ್ಯಥಾ {
		  abits_val_ *= that.abits_val_;
	    }
	    ಪ್ರತ್ಯಾಗಮ;
      }

      ಧ್ರುವ ಪೂರ್ಣಾಙ್ಕ cnt = (size_+BITS_PER_WORD-1) / BITS_PER_WORD;

      unsigned long mask;
      ಯದಿ (unsigned tail = size_%BITS_PER_WORD) {
	    mask = ~( -1UL << tail );
      } ಅನ್ಯಥಾ {
	    mask = ~0UL;
      }

	// Check for any XZ values ahead of time in a first pass. If
	// we find any, then force the entire result to be X and be
	// done.
      ಪ್ರತಿ (ಪೂರ್ಣಾಙ್ಕ idx = 0 ; idx < cnt ; idx += 1) {
	    unsigned long lval = bbits_ptr_[idx];
	    unsigned long rval = that.bbits_ptr_[idx];
	    ಯದಿ (idx == (cnt-1)) {
		  lval &= mask;
		  rval &= mask;
	    }
	    ಯದಿ (lval || rval) {
		  ಪ್ರತಿ (ಪೂರ್ಣಾಙ್ಕ xdx = 0 ; xdx < cnt-1 ; xdx += 1) {
			abits_ptr_[xdx] = WORD_X_ABITS;
			bbits_ptr_[xdx] = WORD_X_BBITS;
		  }
		  abits_ptr_[cnt-1] = WORD_X_ABITS & mask;
		  bbits_ptr_[cnt-1] = WORD_X_BBITS & mask;
		  ಪ್ರತ್ಯಾಗಮ;
	    }
      }

	// Calculate the result into a res array. We need to keep is
	// separate from the "this" array because we are making
	// multiple passes.
      unsigned long*res = ನೂತನ unsigned long[cnt];
      ಪ್ರತಿ (ಪೂರ್ಣಾಙ್ಕ idx = 0 ; idx < cnt ; idx += 1)
	    res[idx] = 0;

      ಪ್ರತಿ (ಪೂರ್ಣಾಙ್ಕ mul_a = 0 ; mul_a < cnt ; mul_a += 1) {
	    unsigned long lval = abits_ptr_[mul_a];
	    ಯದಿ (mul_a == (cnt-1))
		  lval &= mask;

	    ಪ್ರತಿ (ಪೂರ್ಣಾಙ್ಕ mul_b = 0 ; mul_b < (cnt-mul_a) ; mul_b += 1) {
		  unsigned long rval = that.abits_ptr_[mul_b];
		  ಯದಿ (mul_b == (cnt-1))
			rval &= mask;

		  unsigned long sum;
		  unsigned long tmp = multiply_with_carry(lval, rval, sum);
		  ಪೂರ್ಣಾಙ್ಕ base = mul_a + mul_b;
		  unsigned long carry = 0;
		  res[base] = add_with_carry(res[base], tmp, carry);
		  ಪ್ರತಿ (ಪೂರ್ಣಾಙ್ಕ add_idx = base+1 ; add_idx < cnt ; add_idx += 1) {
			res[add_idx] = add_with_carry(res[add_idx], sum, carry);
			sum = 0;
		  }
	    }
      }

	// Replace the "this" value with the calculated result. We
	// know a-priori that the bbits are zero and unchanged.
      res[cnt-1] &= mask;
      ಪ್ರತಿ (ಪೂರ್ಣಾಙ್ಕ idx = 0 ; idx < cnt ; idx += 1)
	    abits_ptr_[idx] = res[idx];

      delete[]res;
      ಪ್ರತ್ಯಾಗಮ;


}

ತಾರ್ಕಿಕ vvp_vector4_t::eeq(ಧ್ರುವ vvp_vector4_t&that) ಧ್ರುವ
{
      ಯದಿ (size_ != that.size_)
	    ಪ್ರತ್ಯಾಗಮ ಅಸತ್ಯ;

      ಯದಿ (size_ < BITS_PER_WORD) {
	    unsigned long mask = (1UL << size_) - 1;
	    ಪ್ರತ್ಯಾಗಮ (abits_val_&mask) == (that.abits_val_&mask)
		  && (bbits_val_&mask) == (that.bbits_val_&mask);
      }

      ಯದಿ (size_ == BITS_PER_WORD) {
	    ಪ್ರತ್ಯಾಗಮ (abits_val_ == that.abits_val_)
		  && (bbits_val_ == that.bbits_val_);
      }

      unsigned ಪದಗಳು = size_ / BITS_PER_WORD;
      ಪ್ರತಿ (unsigned idx = 0 ;  idx < ಪದಗಳು ;  idx += 1) {
	    ಯದಿ (abits_ptr_[idx] != that.abits_ptr_[idx])
		  ಪ್ರತ್ಯಾಗಮ ಅಸತ್ಯ;
	    ಯದಿ (bbits_ptr_[idx] != that.bbits_ptr_[idx])
		  ಪ್ರತ್ಯಾಗಮ ಅಸತ್ಯ;
      }

      unsigned long mask = size_%BITS_PER_WORD;
      ಯದಿ (mask > 0) {
	    mask = (1UL << mask) - 1;
	    ಪ್ರತ್ಯಾಗಮ (abits_ptr_[ಪದಗಳು]&mask) == (that.abits_ptr_[ಪದಗಳು]&mask)
		  && (bbits_ptr_[ಪದಗಳು]&mask) == (that.bbits_ptr_[ಪದಗಳು]&mask);
      }

      ಪ್ರತ್ಯಾಗಮ ಸತ್ಯ;
}

ತಾರ್ಕಿಕ vvp_vector4_t::eq_xz(ಧ್ರುವ vvp_vector4_t&that) ಧ್ರುವ
{
      ಯದಿ (size_ != that.size_)
	    ಪ್ರತ್ಯಾಗಮ ಅಸತ್ಯ;

      ಯದಿ (size_ < BITS_PER_WORD) {
	    unsigned long mask = (1UL << size_) - 1;
	    ಪ್ರತ್ಯಾಗಮ ((abits_val_|bbits_val_)&mask) == ((that.abits_val_|that.bbits_val_)&mask)
		  && (bbits_val_&mask) == (that.bbits_val_&mask);
      }

      ಯದಿ (size_ == BITS_PER_WORD) {
	    ಪ್ರತ್ಯಾಗಮ ((abits_val_|bbits_val_) == (that.abits_val_|that.bbits_val_))
		  && (bbits_val_ == that.bbits_val_);
      }

      unsigned ಪದಗಳು = size_ / BITS_PER_WORD;
      ಪ್ರತಿ (unsigned idx = 0 ;  idx < ಪದಗಳು ;  idx += 1) {
	    ಯದಿ ((abits_ptr_[idx]|bbits_ptr_[idx]) != (that.abits_ptr_[idx]|that.bbits_ptr_[idx]))
		  ಪ್ರತ್ಯಾಗಮ ಅಸತ್ಯ;
	    ಯದಿ (bbits_ptr_[idx] != that.bbits_ptr_[idx])
		  ಪ್ರತ್ಯಾಗಮ ಅಸತ್ಯ;
      }

      unsigned long mask = size_%BITS_PER_WORD;
      ಯದಿ (mask > 0) {
	    mask = (1UL << mask) - 1;
	    ಪ್ರತ್ಯಾಗಮ ((abits_ptr_[ಪದಗಳು]|bbits_ptr_[ಪದಗಳು])&mask) == ((that.abits_ptr_[ಪದಗಳು]|that.bbits_ptr_[ಪದಗಳು])&mask)
		  && (bbits_ptr_[ಪದಗಳು]&mask) == (that.bbits_ptr_[ಪದಗಳು]&mask);
      }

      ಪ್ರತ್ಯಾಗಮ ಸತ್ಯ;
}

ತಾರ್ಕಿಕ vvp_vector4_t::has_xz() ಧ್ರುವ
{
      ಯದಿ (size_ < BITS_PER_WORD) {
	    unsigned long mask = -1UL >> (BITS_PER_WORD - size_);
	    ಪ್ರತ್ಯಾಗಮ bbits_val_&mask;
      }

      ಯದಿ (size_ == BITS_PER_WORD) {
	    ಪ್ರತ್ಯಾಗಮ bbits_val_;
      }

      unsigned ಪದಗಳು = size_ / BITS_PER_WORD;
      ಪ್ರತಿ (unsigned idx = 0 ; idx < ಪದಗಳು ; idx += 1) {
	    ಯದಿ (bbits_ptr_[idx])
		  ಪ್ರತ್ಯಾಗಮ ಸತ್ಯ;
      }

      unsigned long mask = size_%BITS_PER_WORD;
      ಯದಿ (mask > 0) {
	    mask = -1UL >> (BITS_PER_WORD - mask);
	    ಪ್ರತ್ಯಾಗಮ bbits_ptr_[ಪದಗಳು]&mask;
      }

      ಪ್ರತ್ಯಾಗಮ ಅಸತ್ಯ;
}

ಶೂನ್ಯ vvp_vector4_t::change_z2x()
{
	// This method relies on the fact that both BIT4_X and BIT4_Z
	// have the bbit set in the vector4 encoding, and also that
	// the BIT4_X has abit set in the vector4 encoding. By simply
	// or-ing the bbit into the abit, BIT4_X and BIT4_Z both
	// become BIT4_X.

      ಯದಿ (size_ <= BITS_PER_WORD) {
	    abits_val_ |= bbits_val_;
      } ಅನ್ಯಥಾ {
	    unsigned ಪದಗಳು = (size_+BITS_PER_WORD-1) / BITS_PER_WORD;
	    ಪ್ರತಿ (unsigned idx = 0 ;  idx < ಪದಗಳು ;  idx += 1)
		  abits_ptr_[idx] |= bbits_ptr_[idx];
      }
}

ಶೂನ್ಯ vvp_vector4_t::fill_bits(vvp_bit4_t bit)
{
	/* note: this relies on the bit encoding for the vvp_bit4_t. */
      ಸ್ಥಿರ ಧ್ರುವ unsigned long init_atable[4] = {
	    WORD_0_ABITS,
	    WORD_1_ABITS,
	    WORD_Z_ABITS,
	    WORD_X_ABITS };
      ಸ್ಥಿರ ಧ್ರುವ unsigned long init_btable[4] = {
	    WORD_0_BBITS,
	    WORD_1_BBITS,
	    WORD_Z_BBITS,
	    WORD_X_BBITS };

      ಯದಿ (size_ <= BITS_PER_WORD) {
	    abits_val_ = init_atable[bit];
	    bbits_val_ = init_btable[bit];
      } ಅನ್ಯಥಾ {
	    unsigned ಪದಗಳು = (size_ + BITS_PER_WORD - 1) / BITS_PER_WORD;
	    ಪ್ರತಿ (unsigned idx = 0; idx < ಪದಗಳು; idx += 1) {
		  abits_ptr_[idx] = init_atable[bit];
		  bbits_ptr_[idx] = init_btable[bit];
	    }
      }
}

ವರ್ಣ* vvp_vector4_t::as_string(ವರ್ಣ*buf, size_t buf_len) ಧ್ರುವ
{
      ವರ್ಣ*res = buf;
      *buf++ = 'C';
      *buf++ = '4';
      *buf++ = '<';
      buf_len -= 3;

      ಪ್ರತಿ (unsigned idx = 0 ;  idx < size() && buf_len >= 2 ;  idx += 1) {
	    ವಿಕಲ್ಪ (value(size()-idx-1)) {
		ಪಕ್ಷ BIT4_0:
		  *buf++ = '0';
		  ಭಙ್ಗ;
		ಪಕ್ಷ BIT4_1:
		  *buf++ = '1';
		  ಭಙ್ಗ;
		ಪಕ್ಷ BIT4_X:
		  *buf++ = 'x';
		  ಭಙ್ಗ;
		ಪಕ್ಷ BIT4_Z:
		  *buf++ = 'z';
	    }
	    buf_len -= 1;
      }

      *buf++ = '>';
      *buf++ = 0;
      ಪ್ರತ್ಯಾಗಮ res;
}

ಶೂನ್ಯ vvp_vector4_t::invert()
{
      ಯದಿ (size_ <= BITS_PER_WORD) {
	    unsigned long mask = (size_<BITS_PER_WORD)? (1UL<<size_)-1UL : -1UL;
	    abits_val_ = mask & ~abits_val_;
	    abits_val_ |= bbits_val_;
      } ಅನ್ಯಥಾ {
	    unsigned remaining = size_;
	    unsigned idx = 0;
	    ಯಾವತ್ (remaining >= BITS_PER_WORD) {
		  abits_ptr_[idx] = ~abits_ptr_[idx];
		  abits_ptr_[idx] |= bbits_ptr_[idx];
		  idx += 1;
		  remaining -= BITS_PER_WORD;
	    }
	    ಯದಿ (remaining > 0) {
		  unsigned long mask = (1UL<<remaining) - 1UL;
		  abits_ptr_[idx] = mask & ~abits_ptr_[idx];
		  abits_ptr_[idx] |= bbits_ptr_[idx];
	    }
      }
}

#define BIT_MASK(n) ((n) ? ((~0UL) >> (BITS_PER_WORD - (n))) : ~0UL)

vvp_bit4_t vvp_vector4_t::reduce_or() ಧ್ರುವ
{
      unsigned long mask = BIT_MASK(size_ % BITS_PER_WORD);
      vvp_bit4_t res = BIT4_0;

      ಯದಿ (size_ <= BITS_PER_WORD) {
	    ಯದಿ ((abits_val_ & ~bbits_val_ & mask) != 0UL)
		  ಪ್ರತ್ಯಾಗಮ BIT4_1;
	    ಯದಿ ((bbits_val_ & mask) != 0UL)
		  ಪ್ರತ್ಯಾಗಮ BIT4_X;
      } ಅನ್ಯಥಾ {
	    unsigned ಪದಗಳು = (size_ + BITS_PER_WORD - 1) / BITS_PER_WORD;
	    unsigned idx;
	    ಪ್ರತಿ (idx = 0; idx < ಪದಗಳು - 1; idx += 1) {
		  ಯದಿ ((abits_ptr_[idx] & ~bbits_ptr_[idx]) != 0UL)
			ಪ್ರತ್ಯಾಗಮ BIT4_1;
		  ಯದಿ (bbits_ptr_[idx] != 0UL)
			res = BIT4_X;
	    }
	    ಯದಿ ((abits_ptr_[idx] & ~bbits_ptr_[idx] & mask) != 0UL)
		  ಪ್ರತ್ಯಾಗಮ BIT4_1;
	    ಯದಿ ((bbits_ptr_[idx] & mask) != 0UL)
		  res = BIT4_X;
      }

      ಪ್ರತ್ಯಾಗಮ res;
}

vvp_bit4_t vvp_vector4_t::reduce_and() ಧ್ರುವ
{
      unsigned long mask = BIT_MASK(size_ % BITS_PER_WORD);
      vvp_bit4_t res = BIT4_1;

      ಯದಿ (size_ <= BITS_PER_WORD) {
	    ಯದಿ ((abits_val_ | bbits_val_ | ~mask) != ~0UL)
		  ಪ್ರತ್ಯಾಗಮ BIT4_0;
	    ಯದಿ ((bbits_val_ & mask) != 0UL)
		  ಪ್ರತ್ಯಾಗಮ BIT4_X;
	} ಅನ್ಯಥಾ {
	    unsigned ಪದಗಳು = (size_ + BITS_PER_WORD - 1) / BITS_PER_WORD;
	    unsigned idx;
	    ಪ್ರತಿ (idx = 0; idx < ಪದಗಳು - 1; idx += 1) {
		  ಯದಿ ((abits_ptr_[idx] | bbits_ptr_[idx]) != ~0UL)
			ಪ್ರತ್ಯಾಗಮ BIT4_0;
		  ಯದಿ (bbits_ptr_[idx] != 0UL)
			res = BIT4_X;
	    }
	    ಯದಿ ((abits_ptr_[idx] | bbits_ptr_[idx] | ~mask) != ~0UL)
		  ಪ್ರತ್ಯಾಗಮ BIT4_0;
	    ಯದಿ ((bbits_ptr_[idx] & mask) != 0UL)
		  res = BIT4_X;
      }

      ಪ್ರತ್ಯಾಗಮ res;
}

ಸ್ಥಿರ unsigned long parity(unsigned long val)
{
#ಯದಿ defined(__GNUC__)
      // The compiler builtin can use target-specific CPU instructions.
      ಪ್ರತ್ಯಾಗಮ __builtin_parityl(val);
#ಅನ್ಯಥಾ
#ಯದಿ ULONG_MAX > 0xffffffffUL
	val ^= val >> 32;
#endif
	val ^= val >> 16;
	val ^= val >> 8;
	val ^= val >> 4;

      ಪ್ರತ್ಯಾಗಮ (0x6996 >> (val & 0xf)) & 1;
#endif
}

vvp_bit4_t vvp_vector4_t::reduce_xor() ಧ್ರುವ
{
      unsigned long mask = BIT_MASK(size_ % BITS_PER_WORD);

      ಯದಿ (size_ <= BITS_PER_WORD) {
	    ಯದಿ ((bbits_val_ & mask) != 0UL)
		  ಪ್ರತ್ಯಾಗಮ BIT4_X;
	    ಪ್ರತ್ಯಾಗಮ parity(abits_val_ & mask) ? BIT4_1 : BIT4_0;
      } ಅನ್ಯಥಾ {
	    unsigned ಪದಗಳು = (size_ + BITS_PER_WORD - 1) / BITS_PER_WORD;
	    unsigned long val_a = 0UL;
	    unsigned idx;
	    ಪ್ರತಿ (idx = 0; idx < ಪದಗಳು - 1; idx += 1) {
		  ಯದಿ (bbits_ptr_[idx] != 0UL)
			ಪ್ರತ್ಯಾಗಮ BIT4_X;
		  val_a ^= abits_ptr_[idx];
	    }
	    ಯದಿ ((bbits_ptr_[idx] & mask) != 0UL)
		  ಪ್ರತ್ಯಾಗಮ BIT4_X;
	    val_a ^= abits_ptr_[idx] & mask;
	    ಪ್ರತ್ಯಾಗಮ parity(val_a) ? BIT4_1 : BIT4_0;
      }
}

vvp_vector4_t& vvp_vector4_t::operator &= (ಧ್ರುವ vvp_vector4_t&that)
{
	// The truth table is:
	//     00 01 11 10
	//  00 00 00 00 00
	//  01 00 01 11 11
	//  11 00 11 11 11
	//  10 00 11 11 11
      ಯದಿ (size_ <= BITS_PER_WORD) {
	    unsigned long tmp1 = abits_val_ | bbits_val_;
	    unsigned long tmp2 = that.abits_val_ | that.bbits_val_;
	    abits_val_ = tmp1 & tmp2;
	    bbits_val_ = (tmp1 & that.bbits_val_) | (tmp2 & bbits_val_);
      } ಅನ್ಯಥಾ {
	    unsigned ಪದಗಳು = (size_ + BITS_PER_WORD - 1) / BITS_PER_WORD;
	    ಪ್ರತಿ (unsigned idx = 0; idx < ಪದಗಳು ; idx += 1) {
		  unsigned long tmp1 = abits_ptr_[idx] | bbits_ptr_[idx];
		  unsigned long tmp2 = that.abits_ptr_[idx] |
		                       that.bbits_ptr_[idx];
		  abits_ptr_[idx] = tmp1 & tmp2;
		  bbits_ptr_[idx] = (tmp1 & that.bbits_ptr_[idx]) |
		                    (tmp2 & bbits_ptr_[idx]);
	    }
      }

      ಪ್ರತ್ಯಾಗಮ *ಏತತ್;
}

vvp_vector4_t& vvp_vector4_t::operator |= (ಧ್ರುವ vvp_vector4_t&that)
{
	// The truth table is:
	//     00 01 11 10
	//  00 00 01 11 11
	//  01 01 01 01 01
	//  11 11 01 11 11
	//  10 11 01 11 11
      ಯದಿ (size_ <= BITS_PER_WORD) {
	    unsigned long tmp = abits_val_ | bbits_val_ |
	                        that.abits_val_ | that.bbits_val_;
	    bbits_val_ = ((~abits_val_ | bbits_val_) & that.bbits_val_) |
	                 ((~that.abits_val_ | that.bbits_val_) & bbits_val_);
	    abits_val_ = tmp;

      } ಅನ್ಯಥಾ {
	    unsigned ಪದಗಳು = (size_ + BITS_PER_WORD - 1) / BITS_PER_WORD;
	    ಪ್ರತಿ (unsigned idx = 0; idx < ಪದಗಳು ; idx += 1) {
		  unsigned long tmp = abits_ptr_[idx] | bbits_ptr_[idx] |
	                        that.abits_ptr_[idx] | that.bbits_ptr_[idx];
		  bbits_ptr_[idx] = ((~abits_ptr_[idx] | bbits_ptr_[idx]) &
		                     that.bbits_ptr_[idx]) |
		                    ((~that.abits_ptr_[idx] |
		                      that.bbits_ptr_[idx]) & bbits_ptr_[idx]);
		  abits_ptr_[idx] = tmp;
	    }
      }

      ಪ್ರತ್ಯಾಗಮ *ಏತತ್;
}

vvp_vector4_t& vvp_vector4_t::operator ^= (ಧ್ರುವ vvp_vector4_t&that)
{
	// The truth table is:
	//     00 01 11 10
	//  00 00 01 11 11
	//  01 01 00 11 11
	//  11 11 11 11 11
	//  10 11 11 11 11
      ಯದಿ (size_ <= BITS_PER_WORD) {
	    unsigned long bval = bbits_val_ | that.bbits_val_;
	    bbits_val_ = bval;
	    abits_val_ = (abits_val_ ^ that.abits_val_) | bval;
      } ಅನ್ಯಥಾ {
	    unsigned ಪದಗಳು = (size_ + BITS_PER_WORD - 1) / BITS_PER_WORD;
	    ಪ್ರತಿ (unsigned idx = 0; idx < ಪದಗಳು ; idx += 1) {
		  unsigned long bval = bbits_ptr_[idx] | that.bbits_ptr_[idx];
		  bbits_ptr_[idx] = bval;
		  abits_ptr_[idx] = (abits_ptr_[idx] ^ that.abits_ptr_[idx]) | bval;
	    }
      }

      ಪ್ರತ್ಯಾಗಮ *ಏತತ್;
}

/*
* Add an integer to the vvp_vector4_t in place, bit by bit so that
* there is no size limitations.
*/
vvp_vector4_t& vvp_vector4_t::operator += (int64_t that)
{
      vvp_bit4_t carry = BIT4_0;
      unsigned idx;

      ಯದಿ (has_xz()) {
	    vvp_vector4_t xxx (size(), BIT4_X);
	    *ಏತತ್ = xxx;
	    ಪ್ರತ್ಯಾಗಮ *ಏತತ್;
      }

      ಪ್ರತಿ (idx = 0 ; idx < size() ; idx += 1) {
	    ಯದಿ (that == 0 && carry==BIT4_0)
		  ಭಙ್ಗ;

	    vvp_bit4_t that_bit = (that&1)? BIT4_1 : BIT4_0;
	    that >>= 1;

	    ಯದಿ (that_bit==BIT4_0 && carry==BIT4_0)
		  ಅನುವರ್ತ;

	    vvp_bit4_t bit = value(idx);
	    bit = add_with_carry(bit, that_bit, carry);

	    set_bit(idx, bit);
      }

      ಪ್ರತ್ಯಾಗಮ *ಏತತ್;
}

ostream& operator<< (ostream&out, ಧ್ರುವ vvp_vector4_t&that)
{
      out << that.size() << "'b";
      ಪ್ರತಿ (unsigned idx = 0 ;  idx < that.size() ;  idx += 1)
	    out << that.value(that.size()-idx-1);
      ಪ್ರತ್ಯಾಗಮ out;
}

template <ವರ್ಗ INT>ತಾರ್ಕಿಕ vector4_to_ಮೌಲ್ಯ(ಧ್ರುವ vvp_vector4_t&vec, INT&val,
					  ತಾರ್ಕಿಕ is_signed, ತಾರ್ಕಿಕ is_arithmetic)
{
      INT res = 0;
      INT msk = 1;
      ತಾರ್ಕಿಕ rc_flag = ಸತ್ಯ;

      unsigned size = vec.size();
      ಯದಿ (size > 8*ಪರಿಮಾಣ(val)) size = 8*ಪರಿಮಾಣ(val);
      ಪ್ರತಿ (unsigned idx = 0 ;  idx < size ;  idx += 1) {
	    ವಿಕಲ್ಪ (vec.value(idx)) {
		ಪಕ್ಷ BIT4_0:
		  ಭಙ್ಗ;
		ಪಕ್ಷ BIT4_1:
		  res |= msk;
		  ಭಙ್ಗ;
		ಸ್ವಭಾವ:
		  ಯದಿ (is_arithmetic)
			ಪ್ರತ್ಯಾಗಮ ಅಸತ್ಯ;
		  ಅನ್ಯಥಾ
			rc_flag = ಅಸತ್ಯ;
	    }

	    msk <<= 1;
      }

      ಯದಿ (is_signed && vec.value(vec.size()-1) == BIT4_1) {
	    ಯದಿ (vec.size() < 8*ಪರಿಮಾಣ(val))
		  res |= static_cast<INT>(-1ULL << vec.size());
      }

      val = res;
      ಪ್ರತ್ಯಾಗಮ rc_flag;
}

template ತಾರ್ಕಿಕ vector4_to_ಮೌಲ್ಯ(ಧ್ರುವ vvp_vector4_t&vec, int8_t&val,
			       ತಾರ್ಕಿಕ is_signed, ತಾರ್ಕಿಕ is_arithmetic);
template ತಾರ್ಕಿಕ vector4_to_ಮೌಲ್ಯ(ಧ್ರುವ vvp_vector4_t&vec, int16_t&val,
			       ತಾರ್ಕಿಕ is_signed, ತಾರ್ಕಿಕ is_arithmetic);
template ತಾರ್ಕಿಕ vector4_to_ಮೌಲ್ಯ(ಧ್ರುವ vvp_vector4_t&vec, int32_t&val,
			       ತಾರ್ಕಿಕ is_signed, ತಾರ್ಕಿಕ is_arithmetic);
template ತಾರ್ಕಿಕ vector4_to_ಮೌಲ್ಯ(ಧ್ರುವ vvp_vector4_t&vec, int64_t&val,
			       ತಾರ್ಕಿಕ is_signed, ತಾರ್ಕಿಕ is_arithmetic);
template ತಾರ್ಕಿಕ vector4_to_ಮೌಲ್ಯ(ಧ್ರುವ vvp_vector4_t&vec, uint8_t&val,
			       ತಾರ್ಕಿಕ is_signed, ತಾರ್ಕಿಕ is_arithmetic);
template ತಾರ್ಕಿಕ vector4_to_ಮೌಲ್ಯ(ಧ್ರುವ vvp_vector4_t&vec, uint16_t&val,
			       ತಾರ್ಕಿಕ is_signed, ತಾರ್ಕಿಕ is_arithmetic);
template ತಾರ್ಕಿಕ vector4_to_ಮೌಲ್ಯ(ಧ್ರುವ vvp_vector4_t&vec, uint32_t&val,
			       ತಾರ್ಕಿಕ is_signed, ತಾರ್ಕಿಕ is_arithmetic);
template ತಾರ್ಕಿಕ vector4_to_ಮೌಲ್ಯ(ಧ್ರುವ vvp_vector4_t&vec, uint64_t&val,
			       ತಾರ್ಕಿಕ is_signed, ತಾರ್ಕಿಕ is_arithmetic);

template <ವರ್ಗ T> ತಾರ್ಕಿಕ vector4_to_ಮೌಲ್ಯ(ಧ್ರುವ vvp_vector4_t&vec,
                                         ತಾರ್ಕಿಕ&overflow_flag, T&val)
{
      T res = 0;
      T msk = 1;

      overflow_flag = ಅಸತ್ಯ;
      unsigned size = vec.size();
      ಪ್ರತಿ (unsigned idx = 0 ;  idx < size ;  idx += 1) {
	    ವಿಕಲ್ಪ (vec.value(idx)) {
		ಪಕ್ಷ BIT4_0:
		  ಭಙ್ಗ;
		ಪಕ್ಷ BIT4_1:
		  ಯದಿ (msk == 0)
			overflow_flag = ಸತ್ಯ;
		  ಅನ್ಯಥಾ
			res |= msk;
		  ಭಙ್ಗ;
		ಸ್ವಭಾವ:
		  ಪ್ರತ್ಯಾಗಮ ಅಸತ್ಯ;
	    }

	    msk <<= static_cast<T>(1);
      }

      val = res;
      ಪ್ರತ್ಯಾಗಮ ಸತ್ಯ;
}

template ತಾರ್ಕಿಕ vector4_to_ಮೌಲ್ಯ(ಧ್ರುವ vvp_vector4_t&vec, ತಾರ್ಕಿಕ&overflow_flag,
                               unsigned long&val);
#ifndef UL_AND_TIME64_SAME
template ತಾರ್ಕಿಕ vector4_to_ಮೌಲ್ಯ(ಧ್ರುವ vvp_vector4_t&vec, ತಾರ್ಕಿಕ&overflow_flag,
                               vvp_time64_t&val);
#endif

ತಾರ್ಕಿಕ vector4_to_ಮೌಲ್ಯ(ಧ್ರುವ vvp_vector4_t&vec, ದ್ವಿಗುಣ&val, ತಾರ್ಕಿಕ signed_flag)
{

      ಯದಿ (vec.size() == 0) {
	    val = 0.0;
	    ಪ್ರತ್ಯಾಗಮ ಸತ್ಯ;
      }

      ತಾರ್ಕಿಕ flag = ಸತ್ಯ;

      ಯದಿ (vec.value(vec.size()-1) != BIT4_1) {
	    signed_flag = ಅಸತ್ಯ;
      }

      ದ್ವಿಗುಣ res = 0.0;
      ಯದಿ (signed_flag) {
	    vvp_bit4_t carry = BIT4_1;
	    ಪ್ರತಿ (unsigned idx = 0 ;  idx < vec.size() ;  idx += 1) {
		  vvp_bit4_t a = ~vec.value(idx);
		  vvp_bit4_t x = add_with_carry(a, BIT4_0, carry);
		  ವಿಕಲ್ಪ (x) {
		      ಪಕ್ಷ BIT4_0:
			ಭಙ್ಗ;
		      ಪಕ್ಷ BIT4_1:
			res += pow(2.0, (ಪೂರ್ಣಾಙ್ಕ)idx);
			ಭಙ್ಗ;
		      ಸ್ವಭಾವ:
			flag = ಅಸತ್ಯ;
		  }
	    }
	    res *= -1.0;
      } ಅನ್ಯಥಾ {
	    ಪ್ರತಿ (unsigned idx = 0 ;  idx < vec.size() ;  idx += 1) {
		  ವಿಕಲ್ಪ (vec.value(idx)) {
		      ಪಕ್ಷ BIT4_0:
			ಭಙ್ಗ;
		      ಪಕ್ಷ BIT4_1:
			res += pow(2.0, (ಪೂರ್ಣಾಙ್ಕ)idx);
			ಭಙ್ಗ;
		      ಸ್ವಭಾವ:
			flag = ಅಸತ್ಯ;
		  }
	    }
      }
      val = res;
      ಪ್ರತ್ಯಾಗಮ flag;
}

ತಾರ್ಕಿಕ vector2_to_ಮೌಲ್ಯ(ಧ್ರುವ vvp_vector2_t&a, int32_t&val, ತಾರ್ಕಿಕ is_signed)
{
      val = 0;
      unsigned idx;
      uint32_t mask;
      ಪ್ರತಿ (idx = 0, mask = 1 ; idx < a.size() && idx < 32 ; idx += 1, mask <<= 1) {
	    ಯದಿ (a.value(idx)) val |= mask;
      }

      ಯದಿ (is_signed && a.size() < 32 && a.value(a.size()-1)) {
	    mask = -1;
	    mask <<= a.size();
	    val |= mask;
      }

      ಪ್ರತ್ಯಾಗಮ a.size() <= 32;
}

vvp_vector4array_t::vvp_vector4array_t(unsigned width__, unsigned ಪದಗಳು_2)
: width_(width__), ಪದಗಳು_3(ಪದಗಳು_2)
{
}

vvp_vector4array_t::~vvp_vector4array_t()
{
}

ಶೂನ್ಯ vvp_vector4array_t::set_word_(v4cell*cell, ಧ್ರುವ vvp_vector4_t&that)
{
      ಪ್ರತಿಜ್ಞಾ(that.size_ == width_);

      ಯದಿ (width_ <= vvp_vector4_t::BITS_PER_WORD) {
	    cell->abits_val_ = that.abits_val_;
	    cell->bbits_val_ = that.bbits_val_;
	    ಪ್ರತ್ಯಾಗಮ;
      }

      unsigned cnt = (width_ + vvp_vector4_t::BITS_PER_WORD-1)/vvp_vector4_t::BITS_PER_WORD;

      ಯದಿ (cell->abits_ptr_ == 0) {
	    cell->abits_ptr_ = ನೂತನ unsigned long[2*cnt];
	    cell->bbits_ptr_ = cell->abits_ptr_ + cnt;
      }

      ಪ್ರತಿ (unsigned idx = 0 ; idx < cnt ; idx += 1)
	    cell->abits_ptr_[idx] = that.abits_ptr_[idx];
      ಪ್ರತಿ (unsigned idx = 0 ; idx < cnt ; idx += 1)
	    cell->bbits_ptr_[idx] = that.bbits_ptr_[idx];
}

vvp_vector4_t vvp_vector4array_t::get_word_(v4cell*cell) ಧ್ರುವ
{
      ಯದಿ (width_ <= vvp_vector4_t::BITS_PER_WORD) {
	    vvp_vector4_t res;
	    res.size_ = width_;
	    res.abits_val_ = cell->abits_val_;
	    res.bbits_val_ = cell->bbits_val_;
	    ಪ್ರತ್ಯಾಗಮ res;
      }

      vvp_vector4_t res (width_, BIT4_X);
      ಯದಿ (cell->abits_ptr_ == 0)
	    ಪ್ರತ್ಯಾಗಮ res;

      unsigned cnt = (width_ + vvp_vector4_t::BITS_PER_WORD-1)/vvp_vector4_t::BITS_PER_WORD;

      ಪ್ರತಿ (unsigned idx = 0 ; idx < cnt ; idx += 1)
	    res.abits_ptr_[idx] = cell->abits_ptr_[idx];
      ಪ್ರತಿ (unsigned idx = 0 ; idx < cnt ; idx += 1)
	    res.bbits_ptr_[idx] = cell->bbits_ptr_[idx];

      ಪ್ರತ್ಯಾಗಮ res;
}

vvp_vector4array_sa::vvp_vector4array_sa(unsigned width__, unsigned ಪದಗಳು_2)
: vvp_vector4array_t(width__, ಪದಗಳು_2)
{
      array_ = ನೂತನ v4cell[ಪದಗಳು_3];

      ಯದಿ (width_ <= vvp_vector4_t::BITS_PER_WORD) {
	    ಪ್ರತಿ (unsigned idx = 0 ; idx < ಪದಗಳು_3 ; idx += 1) {
		  array_[idx].abits_val_ = vvp_vector4_t::WORD_X_ABITS;
		  array_[idx].bbits_val_ = vvp_vector4_t::WORD_X_BBITS;
	    }
      } ಅನ್ಯಥಾ {
	    ಪ್ರತಿ (unsigned idx = 0 ; idx < ಪದಗಳು_3 ; idx += 1) {
		  array_[idx].abits_ptr_ = 0;
		  array_[idx].bbits_ptr_ = 0;
	    }
      }
}

vvp_vector4array_sa::~vvp_vector4array_sa()
{
      ಯದಿ (array_) {
	    ಯದಿ (width_ > vvp_vector4_t::BITS_PER_WORD) {
		  ಪ್ರತಿ (unsigned idx = 0 ; idx < ಪದಗಳು_3 ; idx += 1)
			ಯದಿ (array_[idx].abits_ptr_)
			      delete[]array_[idx].abits_ptr_;
	    }
	    delete[]array_;
      }
}

ಶೂನ್ಯ vvp_vector4array_sa::set_word(unsigned index, ಧ್ರುವ vvp_vector4_t&that)
{
      ಪ್ರತಿಜ್ಞಾ(index < ಪದಗಳು_3);

      v4cell*cell = &array_[index];

      set_word_(cell, that);
}

vvp_vector4_t vvp_vector4array_sa::get_word(unsigned index) ಧ್ರುವ
{
      ಯದಿ (index >= ಪದಗಳು_3)
	    ಪ್ರತ್ಯಾಗಮ vvp_vector4_t(width_, BIT4_X);

      ಪ್ರತಿಜ್ಞಾ(index < ಪದಗಳು_3);

      v4cell*cell = &array_[index];

      ಪ್ರತ್ಯಾಗಮ get_word_(cell);
}

vvp_vector4array_aa::vvp_vector4array_aa(unsigned width__, unsigned ಪದಗಳು_2)
: vvp_vector4array_t(width__, ಪದಗಳು_2)
{
      context_idx_ = vpip_add_item_to_context(ಏತತ್, vpip_peek_context_scope());
}

vvp_vector4array_aa::~vvp_vector4array_aa()
{
}

ಶೂನ್ಯ vvp_vector4array_aa::alloc_instance(vvp_context_t context)
{
      v4cell*array = ನೂತನ v4cell[ಪದಗಳು_3];

      ಯದಿ (width_ <= vvp_vector4_t::BITS_PER_WORD) {
	    ಪ್ರತಿ (unsigned idx = 0 ; idx < ಪದಗಳು_3 ; idx += 1) {
		  array[idx].abits_val_ = vvp_vector4_t::WORD_X_ABITS;
		  array[idx].bbits_val_ = vvp_vector4_t::WORD_X_BBITS;
	    }
      } ಅನ್ಯಥಾ {
	    ಪ್ರತಿ (unsigned idx = 0 ; idx < ಪದಗಳು_3 ; idx += 1) {
		  array[idx].abits_ptr_ = 0;
		  array[idx].bbits_ptr_ = 0;
	    }
      }

      vvp_set_context_item(context, context_idx_, array);
}

ಶೂನ್ಯ vvp_vector4array_aa::reset_instance(vvp_context_t context)
{
      v4cell*cell = static_cast<v4cell*>
            (vvp_get_context_item(context, context_idx_));

      ಯದಿ (width_ <= vvp_vector4_t::BITS_PER_WORD) {
	    ಪ್ರತಿ (unsigned idx = 0 ; idx < ಪದಗಳು_3 ; idx += 1) {
		  cell->abits_val_ = vvp_vector4_t::WORD_X_ABITS;
		  cell->bbits_val_ = vvp_vector4_t::WORD_X_BBITS;
                  cell++;
	    }
      } ಅನ್ಯಥಾ {
            unsigned cnt = (width_ + vvp_vector4_t::BITS_PER_WORD-1)/vvp_vector4_t::BITS_PER_WORD;
	    ಪ್ರತಿ (unsigned idx = 0 ; idx < ಪದಗಳು_3 ; idx += 1) {
		  ಯದಿ (cell->abits_ptr_) {
                        ಪ್ರತಿ (unsigned n = 0 ; n < cnt ; n += 1) {
			      cell->abits_ptr_[n] = vvp_vector4_t::WORD_X_ABITS;
			      cell->bbits_ptr_[n] = vvp_vector4_t::WORD_X_BBITS;
                        }
                  }
                  cell++;
	    }
      }
}

#ifdef CHECK_WITH_VALGRIND
ಶೂನ್ಯ vvp_vector4array_aa::free_instance(vvp_context_t context)
{
      v4cell*cell = static_cast<v4cell*>
            (vvp_get_context_item(context, context_idx_));
      delete [] cell;
}
#endif

ಶೂನ್ಯ vvp_vector4array_aa::set_word(unsigned index, ಧ್ರುವ vvp_vector4_t&that)
{
      ಪ್ರತಿಜ್ಞಾ(index < ಪದಗಳು_3);

      v4cell*cell = static_cast<v4cell*>
            (vthread_get_wt_context_item(context_idx_)) + index;

      set_word_(cell, that);
}

vvp_vector4_t vvp_vector4array_aa::get_word(unsigned index) ಧ್ರುವ
{
      ಯದಿ (index >= ಪದಗಳು_3)
	    ಪ್ರತ್ಯಾಗಮ vvp_vector4_t(width_, BIT4_X);

      ಪ್ರತಿಜ್ಞಾ(index < ಪದಗಳು_3);

      v4cell*cell = static_cast<v4cell*>
            (vthread_get_rd_context_item(context_idx_)) + index;

      ಪ್ರತ್ಯಾಗಮ get_word_(cell);
}

vvp_vector2_t::vvp_vector2_t()
{
      vec_ = 0;
      wid_ = 0;
}

vvp_vector2_t::vvp_vector2_t(uint64_t v, unsigned wid)
{
      wid_ = wid;
      ಧ್ರುವ unsigned bits_per_word = 8 * ಪರಿಮಾಣ(vec_[0]);
      ಧ್ರುವ unsigned ಪದಗಳು = (wid_ + bits_per_word-1) / bits_per_word;

      vec_ = ನೂತನ unsigned long[ಪದಗಳು];
      ಪ್ರತಿ (unsigned idx = 0 ; idx < ಪದಗಳು ; idx += 1) {
	    ಧ್ರುವ unsigned shift = idx * bits_per_word;
	    vec_[idx] = shift < 64 ? v >> shift : 0;
      }
}

vvp_vector2_t::vvp_vector2_t(vvp_vector2_t::fill_t fill, unsigned wid)
{
      wid_ = wid;
      ಧ್ರುವ unsigned bits_per_word = 8 * ಪರಿಮಾಣ(vec_[0]);
      ಧ್ರುವ unsigned ಪದಗಳು = (wid_ + bits_per_word-1) / bits_per_word;

      vec_ = ನೂತನ unsigned long[ಪದಗಳು];
      ಪ್ರತಿ (unsigned idx = 0 ;  idx < ಪದಗಳು ;  idx += 1)
	    vec_[idx] = fill? -1 : 0;
}

vvp_vector2_t::vvp_vector2_t(ಧ್ರುವ vvp_vector2_t&that, unsigned base, unsigned wid)
{
      wid_ = wid;
      ಧ್ರುವ unsigned ಪದಗಳು = (wid_ + BITS_PER_WORD-1) / BITS_PER_WORD;

      vec_ = ನೂತನ unsigned long[ಪದಗಳು];

      ಪ್ರತಿ (unsigned idx = 0 ; idx < wid ; idx += 1) {
	    ಪೂರ್ಣಾಙ್ಕ bit = that.value(base+idx);
	    ಯದಿ (bit == 0)
		  ಅನುವರ್ತ;
	    unsigned word = idx / BITS_PER_WORD;
	    unsigned long mask = 1UL << (idx % BITS_PER_WORD);
	    vec_[word] |= mask;
      }
}

ಶೂನ್ಯ vvp_vector2_t::copy_from_that_(ಧ್ರುವ vvp_vector4_t&that)
{
      wid_ = that.size();
      ಧ್ರುವ unsigned ಪದಗಳು = (that.size() + BITS_PER_WORD-1) / BITS_PER_WORD;

      ಯದಿ (ಪದಗಳು == 0) {
	    vec_ = 0;
	    wid_ = 0;
	    ಪ್ರತ್ಯಾಗಮ;
      }

	// Use the subarray method with the xz_to_0 flag set so that
	// we get values even when there are xz bits.
      vec_ = that.subarray(0, wid_, ಸತ್ಯ);
}

ಶೂನ್ಯ vvp_vector2_t::copy_from_that_(ಧ್ರುವ vvp_vector2_t&that)
{
      wid_ = that.wid_;
      ಧ್ರುವ unsigned ಪದಗಳು = (wid_ + BITS_PER_WORD-1) / BITS_PER_WORD;

      ಯದಿ (ಪದಗಳು == 0) {
	    vec_ = 0;
	    wid_ = 0;
	    ಪ್ರತ್ಯಾಗಮ;
      }

      vec_ = ನೂತನ unsigned long[ಪದಗಳು];
      ಪ್ರತಿ (unsigned idx = 0 ;  idx < ಪದಗಳು ;  idx += 1)
	    vec_[idx] = that.vec_[idx];
}

vvp_vector2_t::vvp_vector2_t(ಧ್ರುವ vvp_vector2_t&that, unsigned newsize)
{
      wid_ = newsize;
      ಯದಿ (newsize == 0) {
	    vec_ = 0;
	    ಪ್ರತ್ಯಾಗಮ;
      }

      ಧ್ರುವ unsigned ಪದಗಳು = (wid_ + BITS_PER_WORD-1) / BITS_PER_WORD;
      ಧ್ರುವ unsigned twords = (that.wid_ + BITS_PER_WORD-1) / BITS_PER_WORD;

      vec_ = ನೂತನ unsigned long[ಪದಗಳು];
      ಪ್ರತಿ (unsigned idx = 0 ;  idx < ಪದಗಳು ;  idx += 1) {
	    ಯದಿ (idx < twords)
		  vec_[idx] = that.vec_[idx];
	    ಅನ್ಯಥಾ
		  vec_[idx] = 0;
      }
}

vvp_vector2_t& vvp_vector2_t::operator= (ಧ್ರುವ vvp_vector2_t&that)
{
      ಯದಿ (ಏತತ್ == &that)
	    ಪ್ರತ್ಯಾಗಮ *ಏತತ್;

      delete[] vec_;
      vec_ = 0;

      copy_from_that_(that);
      ಪ್ರತ್ಯಾಗಮ *ಏತತ್;
}

vvp_vector2_t& vvp_vector2_t::operator= (ಧ್ರುವ vvp_vector4_t&that)
{
      delete[]vec_;
      vec_ = 0;
      copy_from_that_(that);
      ಪ್ರತ್ಯಾಗಮ *ಏತತ್;
}

vvp_vector2_t& vvp_vector2_t::operator <<= (unsigned ಪೂರ್ಣಾಙ್ಕ shift)
{
      ಯದಿ (wid_ == 0)
	    ಪ್ರತ್ಯಾಗಮ *ಏತತ್;

      ಧ್ರುವ unsigned ಪದಗಳು = (wid_ + BITS_PER_WORD-1) / BITS_PER_WORD;

	// Number of words to shift
      ಧ್ರುವ unsigned wshift = shift / BITS_PER_WORD;
	// bits to shift within each word.
      ಧ್ರುವ unsigned long oshift = shift % BITS_PER_WORD;

	// If shifting the entire value away, then return zeros.
      ಯದಿ (wshift >= ಪದಗಳು) {
	    ಪ್ರತಿ (unsigned idx = 0 ;  idx < ಪದಗಳು ;  idx += 1)
		  vec_[idx] = 0;

	    ಪ್ರತ್ಯಾಗಮ *ಏತತ್;
      }

	// Do the word shift first.
      ಯದಿ (wshift > 0) {
	    ಪ್ರತಿ (unsigned idx = 0 ;  idx < ಪದಗಳು-wshift ;  idx += 1) {
		  unsigned sel = ಪದಗಳು - idx - 1;
		  vec_[sel] = vec_[sel-wshift];
	    }

	    ಪ್ರತಿ (unsigned idx = 0 ;  idx < wshift ;  idx += 1)
		  vec_[idx] = 0;
      }

	// Do the fine shift.
      ಯದಿ (oshift != 0) {
	    unsigned long pad = 0;
	    ಪ್ರತಿ (unsigned idx = 0 ;  idx < ಪದಗಳು ;  idx += 1) {
		  unsigned long next_pad = vec_[idx] >> (BITS_PER_WORD-oshift);
		  vec_[idx] = (vec_[idx] << oshift) | pad;
		  pad = next_pad;
	    }

	      // Cleanup the tail bits.
	    unsigned long mask = -1UL >> (BITS_PER_WORD - wid_%BITS_PER_WORD);
	    vec_[ಪದಗಳು-1] &= mask;
      }

      ಪ್ರತ್ಯಾಗಮ *ಏತತ್;
}

vvp_vector2_t& vvp_vector2_t::operator >>= (unsigned shift)
{
      ಯದಿ (wid_ == 0)
	    ಪ್ರತ್ಯಾಗಮ *ಏತತ್;

      ಧ್ರುವ unsigned ಪದಗಳು = (wid_ + BITS_PER_WORD-1) / BITS_PER_WORD;

	// Number of words to shift
      ಧ್ರುವ unsigned wshift = shift / BITS_PER_WORD;
	// bits to shift within each word.
      ಧ್ರುವ unsigned long oshift = shift % BITS_PER_WORD;

	// If shifting the entire value away, then return zeros.
      ಯದಿ (wshift >= ಪದಗಳು) {
	    ಪ್ರತಿ (unsigned idx = 0 ;  idx < ಪದಗಳು ;  idx += 1)
		  vec_[idx] = 0;

	    ಪ್ರತ್ಯಾಗಮ *ಏತತ್;
      }

      ಯದಿ (wshift > 0) {
	    ಪ್ರತಿ (unsigned idx = 0 ;  idx < ಪದಗಳು-wshift ;  idx += 1)
		  vec_[idx] = vec_[idx+wshift];

	    ಪ್ರತಿ (unsigned idx = ಪದಗಳು-wshift ;  idx < ಪದಗಳು ;  idx += 1)
		  vec_[idx] = 0;
      }

      ಯದಿ (oshift > 0) {
	    unsigned long pad = 0;
	    ಪ್ರತಿ (unsigned idx = ಪದಗಳು ;  idx > 0 ;  idx -= 1) {
		  unsigned long new_pad = vec_[idx-1] <<(BITS_PER_WORD-oshift);
		  vec_[idx-1] = pad | (vec_[idx-1] >> oshift);
		  pad = new_pad;
	    }

	      // Cleanup the tail bits.

	    unsigned use_ಪದಗಳು = ಪದಗಳು;
	      // Mask_shift is the number of high bits of the top word
	      // that are to be masked off. We start with the number
	      // of bits that are not included even in the original,
	      // then we include the bits of the shift, that are to be
	      // masked to zero.
	    unsigned long mask_shift = BITS_PER_WORD - wid_%BITS_PER_WORD;
	    mask_shift %= BITS_PER_WORD;
	    mask_shift += oshift;
	    ಯಾವತ್ (mask_shift >= BITS_PER_WORD) {
		  vec_[use_ಪದಗಳು-1] = 0;
		  use_ಪದಗಳು -= 1;
		  mask_shift -= BITS_PER_WORD;
	    }
	    ಯದಿ (mask_shift > 0) {
		  ಪ್ರತಿಜ್ಞಾ(use_ಪದಗಳು > 0);
		  unsigned long mask = -1UL >> mask_shift;
		  vec_[use_ಪದಗಳು-1] &= mask;
	    }
      }

      ಪ್ರತ್ಯಾಗಮ *ಏತತ್;
}

ಸ್ಥಿರ unsigned long add_carry(unsigned long a, unsigned long b,
			       unsigned long&carry)
{
      unsigned long out = carry;
      carry = 0;

      ಯದಿ ((ULONG_MAX - out) < a)
	    carry += 1;
      out += a;

      ಯದಿ ((ULONG_MAX - out) < b)
	    carry += 1;
      out += b;

      ಪ್ರತ್ಯಾಗಮ out;
}

vvp_vector2_t& vvp_vector2_t::operator += (ಧ್ರುವ vvp_vector2_t&that)
{
      ಪ್ರತಿಜ್ಞಾ(wid_ == that.wid_);
      ಯದಿ (wid_ == 0)
	    ಪ್ರತ್ಯಾಗಮ *ಏತತ್;

      ಧ್ರುವ unsigned ಪದಗಳು = (wid_ + BITS_PER_WORD-1) / BITS_PER_WORD;

      unsigned long carry = 0;
      ಪ್ರತಿ (unsigned idx = 0 ;  idx < ಪದಗಳು ;  idx += 1) {
	    vec_[idx] = add_carry(vec_[idx], that.vec_[idx], carry);
      }


	// Cleanup the tail bits.
      unsigned long mask = -1UL >> (BITS_PER_WORD - wid_%BITS_PER_WORD);
      vec_[ಪದಗಳು-1] &= mask;

      ಪ್ರತ್ಯಾಗಮ *ಏತತ್;
}

vvp_vector2_t& vvp_vector2_t::operator -= (ಧ್ರುವ vvp_vector2_t&that)
{
      ಪ್ರತಿಜ್ಞಾ(wid_ == that.wid_);
      ಯದಿ (wid_ == 0)
	    ಪ್ರತ್ಯಾಗಮ *ಏತತ್;

      ಧ್ರುವ unsigned ಪದಗಳು = (wid_ + BITS_PER_WORD-1) / BITS_PER_WORD;

      unsigned long carry = 1;
      ಪ್ರತಿ (unsigned idx = 0 ;  idx < ಪದಗಳು ;  idx += 1) {
	    vec_[idx] = add_carry(vec_[idx], ~that.vec_[idx], carry);
      }

      ಪ್ರತ್ಯಾಗಮ *ಏತತ್;
}

ಶೂನ್ಯ vvp_vector2_t::trim()
{
      ಯಾವತ್ (value(wid_-1) == 0 && wid_ > 1) wid_ -= 1;
}

/* This is a special trim that is used on numbers we know represent a
 * negative signed value (they came from a negative real value). */
ಶೂನ್ಯ vvp_vector2_t::trim_neg()
{
      ಯದಿ (value(wid_-1) == 1 && wid_ > 32) {
	    ಯಾವತ್ (value(wid_-2) == 1 && wid_ > 32) wid_ -= 1;
      }
}

ಪೂರ್ಣಾಙ್ಕ vvp_vector2_t::value(unsigned idx) ಧ್ರುವ
{
      ಯದಿ (idx >= wid_)
	    ಪ್ರತ್ಯಾಗಮ 0;

      ಧ್ರುವ unsigned bits_per_word = 8 * ಪರಿಮಾಣ(vec_[0]);
      unsigned addr = idx/bits_per_word;
      unsigned mask = idx%bits_per_word;

      ಯದಿ (vec_[addr] & (1UL<<mask))
	    ಪ್ರತ್ಯಾಗಮ 1;
      ಅನ್ಯಥಾ
	    ಪ್ರತ್ಯಾಗಮ 0;
}

ಶೂನ್ಯ vvp_vector2_t::set_bit(unsigned idx, ಪೂರ್ಣಾಙ್ಕ bit)
{
      ಪ್ರತಿಜ್ಞಾ(idx < wid_);

      ಧ್ರುವ unsigned bits_per_word = 8 * ಪರಿಮಾಣ(vec_[0]);
      unsigned addr = idx/bits_per_word;
      unsigned long mask = idx%bits_per_word;

      ಯದಿ (bit)
	    vec_[addr] |= 1UL << mask;
      ಅನ್ಯಥಾ
	    vec_[addr] &= ~(1UL << mask);
}

ಶೂನ್ಯ vvp_vector2_t::set_vec(unsigned adr, ಧ್ರುವ vvp_vector2_t&that)
{
      ಪ್ರತಿಜ್ಞಾ((adr + that.wid_) <= wid_);

      ಪ್ರತಿ (unsigned idx = 0 ; idx < that.wid_ ; idx += 1)
	    set_bit(adr+idx, that.value(idx));
}

ತಾರ್ಕಿಕ vvp_vector2_t::is_NaN() ಧ್ರುವ
{
      ಪ್ರತ್ಯಾಗಮ wid_ == 0;
}

ತಾರ್ಕಿಕ vvp_vector2_t::is_zero() ಧ್ರುವ
{
      ಧ್ರುವ unsigned ಪದಗಳು = (wid_ + BITS_PER_WORD-1) / BITS_PER_WORD;

      ಪ್ರತಿ (unsigned idx = 0; idx < ಪದಗಳು; idx += 1) {
	    ಯದಿ (vec_[idx] == 0) ಅನುವರ್ತ;
	    ಪ್ರತ್ಯಾಗಮ ಅಸತ್ಯ;
      }

      ಪ್ರತ್ಯಾಗಮ ಸತ್ಯ;
}

/*
 * Basic idea from "Introduction to Programming using SML" by
 * Michael R. Hansen and Hans Rischel page 261 and "Seminumerical
 * Algorithms, Third Edition" by Donald E. Knuth section 4.6.3.
 */
vvp_vector2_t pow(ಧ್ರುವ vvp_vector2_t&x, vvp_vector2_t&y)
{
        /* If we have a zero exponent just return 1. */
      ಯದಿ (y == vvp_vector2_t(0L, 1)) {
	    ಪ್ರತ್ಯಾಗಮ vvp_vector2_t(1L, x.size());
      }

        /* Is the value odd? */
      ಯದಿ (y.value(0) == 1) {
	    y.set_bit(0, 0);  // A quick subtract by 1.
	    vvp_vector2_t res = x * pow(x, y);
	    ಪ್ರತ್ಯಾಗಮ res;
      }

      y >>= 1;  // A fast divide by two. We know the LSB is zero.
      vvp_vector2_t z = pow(x, y);
      vvp_vector2_t res = z * z;
      ಪ್ರತ್ಯಾಗಮ res;
}

ಸ್ಥಿರ ಶೂನ್ಯ multiply_long(unsigned long a, unsigned long b,
			  unsigned long&ಕೆಳಗಿನ, unsigned long&ಮೇಲಿನ)
{
      ಪ್ರತಿಜ್ಞಾ(ಪರಿಮಾಣ(unsigned long) %2 == 0);

      ಧ್ರುವ unsigned long word_mask = (1UL << 4UL*ಪರಿಮಾಣ(a)) - 1UL;
      unsigned long tmpa;
      unsigned long tmpb;
      unsigned long res[4];

      tmpa = a & word_mask;
      tmpb = b & word_mask;
      res[0] = tmpa * tmpb;
      res[1] = res[0] >> 4UL*ಪರಿಮಾಣ(unsigned long);
      res[0] &= word_mask;

      tmpa = (a >> 4UL*ಪರಿಮಾಣ(unsigned long)) & word_mask;
      res[1] += tmpa * tmpb;
      res[2] = res[1] >> 4UL*ಪರಿಮಾಣ(unsigned long);
      res[1] &= word_mask;

      tmpa = a & word_mask;
      tmpb = (b >> 4UL*ಪರಿಮಾಣ(unsigned long)) & word_mask;
      res[1] += tmpa * tmpb;
      res[2] += res[1] >> 4UL*ಪರಿಮಾಣ(unsigned long);
      res[3]  = res[2] >> 4UL*ಪರಿಮಾಣ(unsigned long);
      res[1] &= word_mask;
      res[2] &= word_mask;

      tmpa = (a >> 4UL*ಪರಿಮಾಣ(unsigned long)) & word_mask;
      res[2] += tmpa * tmpb;
      res[3] += res[2] >> 4UL*ಪರಿಮಾಣ(unsigned long);
      res[2] &= word_mask;

      ಮೇಲಿನ = (res[3] << 4UL*ಪರಿಮಾಣ(unsigned long)) | res[2];
      ಕೆಳಗಿನ  = (res[1] << 4UL*ಪರಿಮಾಣ(unsigned long)) | res[0];
}

vvp_vector2_t operator * (ಧ್ರುವ vvp_vector2_t&a, ಧ್ರುವ vvp_vector2_t&b)
{
      ಧ್ರುವ unsigned bits_per_word = 8 * ಪರಿಮಾಣ(a.vec_[0]);

	// The compiler ensures that the two operands are of equal size.
      ಪ್ರತಿಜ್ಞಾ(a.size() == b.size());
      vvp_vector2_t r (0, a.size());

      unsigned ಪದಗಳು = (r.wid_ + bits_per_word - 1) / bits_per_word;

      ಪ್ರತಿ (unsigned bdx = 0 ;  bdx < ಪದಗಳು ;  bdx += 1) {
	    unsigned long tmpb = b.vec_[bdx];
	    ಯದಿ (tmpb == 0)
		  ಅನುವರ್ತ;

	    ಪ್ರತಿ (unsigned adx = 0 ;  adx < ಪದಗಳು ;  adx += 1) {
		  unsigned long tmpa = a.vec_[adx];
		  ಯದಿ (tmpa == 0)
			ಅನುವರ್ತ;

		  unsigned long ಕೆಳಗಿನ, hig;
		  multiply_long(tmpa, tmpb, ಕೆಳಗಿನ, hig);

		  unsigned long carry = 0;
		  ಪ್ರತಿ (unsigned sdx = 0
			     ; (adx+bdx+sdx) < ಪದಗಳು
			     ;  sdx += 1) {

			r.vec_[adx+bdx+sdx] = add_carry(r.vec_[adx+bdx+sdx],
							ಕೆಳಗಿನ, carry);
			ಕೆಳಗಿನ = hig;
			hig = 0;
		  }
	    }
      }

      ಪ್ರತ್ಯಾಗಮ r;
}

ಸ್ಥಿರ ಶೂನ್ಯ div_mod (vvp_vector2_t dividend, vvp_vector2_t ಭಾಜಕ,
		     vvp_vector2_t&quotient, vvp_vector2_t&remainder)
{

      quotient = vvp_vector2_t(0, dividend.size());

      ಯದಿ (ಭಾಜಕ == quotient) {
	    cerr << "ERROR: division by zero, exiting." << endl;
	    exit(255);
      }

      ಯದಿ (dividend < ಭಾಜಕ) {
	    remainder = dividend;
	    ಪ್ರತ್ಯಾಗಮ;
      }

      vvp_vector2_t mask (1, dividend.size());

	// Make the dividend 1 bit larger to prevent overflow of
	// divtmp in startup.
      dividend = vvp_vector2_t(dividend, dividend.size()+1);
      vvp_vector2_t divtmp (ಭಾಜಕ, dividend.size());

      ಯಾವತ್ (divtmp < dividend) {
	    divtmp <<= 1;
	    mask <<= 1;
      }

      ಯಾವತ್ (dividend >= ಭಾಜಕ) {
	    ಯದಿ (divtmp <= dividend) {
		  dividend -= divtmp;
		  quotient += mask;
	    }

	    divtmp >>= 1;
	    mask >>= 1;
      }

      remainder = vvp_vector2_t(dividend, mask.size());
}

vvp_vector2_t operator - (ಧ್ರುವ vvp_vector2_t&that)
{
      vvp_vector2_t neg(that);
      ಯದಿ (neg.wid_ == 0) ಪ್ರತ್ಯಾಗಮ neg;

      ಧ್ರುವ unsigned ಪದಗಳು = (neg.wid_ + neg.BITS_PER_WORD-1) /
                                         neg.BITS_PER_WORD;
      ಪ್ರತಿ (unsigned idx = 0 ;  idx < ಪದಗಳು ;  idx += 1) {
	    neg.vec_[idx] = ~neg.vec_[idx];
      }
      neg += vvp_vector2_t(1, neg.wid_);

      ಪ್ರತ್ಯಾಗಮ neg;
}

vvp_vector2_t operator / (ಧ್ರುವ vvp_vector2_t&dividend,
			  ಧ್ರುವ vvp_vector2_t&ಭಾಜಕ)
{
      vvp_vector2_t quot, rem;
      div_mod(dividend, ಭಾಜಕ, quot, rem);
      ಪ್ರತ್ಯಾಗಮ quot;
}

vvp_vector2_t operator % (ಧ್ರುವ vvp_vector2_t&dividend,
			  ಧ್ರುವ vvp_vector2_t&ಭಾಜಕ)
{
      vvp_vector2_t quot, rem;
      div_mod(dividend, ಭಾಜಕ, quot, rem);
      ಪ್ರತ್ಯಾಗಮ rem;
}

ತಾರ್ಕಿಕ operator > (ಧ್ರುವ vvp_vector2_t&a, ಧ್ರುವ vvp_vector2_t&b)
{
      ಧ್ರುವ unsigned awords = (a.wid_ + vvp_vector2_t::BITS_PER_WORD-1) / vvp_vector2_t::BITS_PER_WORD;
      ಧ್ರುವ unsigned bwords = (b.wid_ + vvp_vector2_t::BITS_PER_WORD-1) / vvp_vector2_t::BITS_PER_WORD;

      ಧ್ರುವ unsigned ಪದಗಳು = awords > bwords? awords : bwords;

      ಪ್ರತಿ (unsigned idx = ಪದಗಳು ;  idx > 0 ;  idx -= 1) {
	    unsigned long aw = (idx <= awords)? a.vec_[idx-1] : 0;
	    unsigned long bw = (idx <= bwords)? b.vec_[idx-1] : 0;

	    ಯದಿ (aw > bw)
		  ಪ್ರತ್ಯಾಗಮ ಸತ್ಯ;
	    ಯದಿ (aw < bw)
		  ಪ್ರತ್ಯಾಗಮ ಅಸತ್ಯ;
      }

	// If the above loop finishes, then the vectors are equal.
      ಪ್ರತ್ಯಾಗಮ ಅಸತ್ಯ;
}

ತಾರ್ಕಿಕ operator >= (ಧ್ರುವ vvp_vector2_t&a, ಧ್ರುವ vvp_vector2_t&b)
{
      ಧ್ರುವ unsigned awords = (a.wid_ + vvp_vector2_t::BITS_PER_WORD-1) / vvp_vector2_t::BITS_PER_WORD;
      ಧ್ರುವ unsigned bwords = (b.wid_ + vvp_vector2_t::BITS_PER_WORD-1) / vvp_vector2_t::BITS_PER_WORD;

      ಧ್ರುವ unsigned ಪದಗಳು = awords > bwords? awords : bwords;

      ಪ್ರತಿ (unsigned idx = ಪದಗಳು ;  idx > 0 ;  idx -= 1) {
	    unsigned long aw = (idx <= awords)? a.vec_[idx-1] : 0;
	    unsigned long bw = (idx <= bwords)? b.vec_[idx-1] : 0;

	    ಯದಿ (aw > bw)
		  ಪ್ರತ್ಯಾಗಮ ಸತ್ಯ;
	    ಯದಿ (aw < bw)
		  ಪ್ರತ್ಯಾಗಮ ಅಸತ್ಯ;
      }

	// If the above loop finishes, then the vectors are equal.
      ಪ್ರತ್ಯಾಗಮ ಸತ್ಯ;
}

ತಾರ್ಕಿಕ operator < (ಧ್ರುವ vvp_vector2_t&a, ಧ್ರುವ vvp_vector2_t&b)
{
      ಧ್ರುವ unsigned awords = (a.wid_ + vvp_vector2_t::BITS_PER_WORD-1) / vvp_vector2_t::BITS_PER_WORD;
      ಧ್ರುವ unsigned bwords = (b.wid_ + vvp_vector2_t::BITS_PER_WORD-1) / vvp_vector2_t::BITS_PER_WORD;

      unsigned ಪದಗಳು = awords;
      ಯದಿ (bwords > ಪದಗಳು)
	    ಪದಗಳು = bwords;

      ಪ್ರತಿ (unsigned idx = ಪದಗಳು ;  idx > 0 ;  idx -= 1) {
	    unsigned long aw = (idx <= awords)? a.vec_[idx-1] : 0;
	    unsigned long bw = (idx <= bwords)? b.vec_[idx-1] : 0;

	    ಯದಿ (aw < bw)
		  ಪ್ರತ್ಯಾಗಮ ಸತ್ಯ;
	    ಯದಿ (aw > bw)
		  ಪ್ರತ್ಯಾಗಮ ಅಸತ್ಯ;
      }

	// If the above loop finishes, then the vectors are equal.
      ಪ್ರತ್ಯಾಗಮ ಅಸತ್ಯ;
}

ತಾರ್ಕಿಕ operator <= (ಧ್ರುವ vvp_vector2_t&a, ಧ್ರುವ vvp_vector2_t&b)
{
	// XXXX For now, only support equal width vectors.
      ಪ್ರತಿಜ್ಞಾ(a.wid_ == b.wid_);

      ಧ್ರುವ unsigned awords = (a.wid_ + vvp_vector2_t::BITS_PER_WORD-1) / vvp_vector2_t::BITS_PER_WORD;

      ಪ್ರತಿ (unsigned idx = awords ;  idx > 0 ;  idx -= 1) {
	    ಯದಿ (a.vec_[idx-1] < b.vec_[idx-1])
		  ಪ್ರತ್ಯಾಗಮ ಸತ್ಯ;
	    ಯದಿ (a.vec_[idx-1] > b.vec_[idx-1])
		  ಪ್ರತ್ಯಾಗಮ ಅಸತ್ಯ;
      }

	// If the above loop finishes, then the vectors are equal.
      ಪ್ರತ್ಯಾಗಮ ಸತ್ಯ;
}

ತಾರ್ಕಿಕ operator == (ಧ್ರುವ vvp_vector2_t&a, ಧ್ರುವ vvp_vector2_t&b)
{
      ಧ್ರುವ unsigned awords = (a.wid_ + vvp_vector2_t::BITS_PER_WORD-1) / vvp_vector2_t::BITS_PER_WORD;
      ಧ್ರುವ unsigned bwords = (b.wid_ + vvp_vector2_t::BITS_PER_WORD-1) / vvp_vector2_t::BITS_PER_WORD;

      ಧ್ರುವ unsigned ಪದಗಳು = awords > bwords? awords : bwords;

      ಪ್ರತಿ (unsigned idx = ಪದಗಳು ;  idx > 0 ;  idx -= 1) {
	    unsigned long aw = (idx <= awords)? a.vec_[idx-1] : 0;
	    unsigned long bw = (idx <= bwords)? b.vec_[idx-1] : 0;

	    ಯದಿ (aw > bw)
		  ಪ್ರತ್ಯಾಗಮ ಅಸತ್ಯ;
	    ಯದಿ (aw < bw)
		  ಪ್ರತ್ಯಾಗಮ ಅಸತ್ಯ;
      }

	// If the above loop finishes, then the vectors are equal.
      ಪ್ರತ್ಯಾಗಮ ಸತ್ಯ;
}


vvp_vector4_t vector2_to_vector4(ಧ್ರುವ vvp_vector2_t&that, unsigned wid)
{
      vvp_vector4_t res (wid);

      ಪ್ರತಿ (unsigned idx = 0 ;  idx < res.size() ;  idx += 1) {
	    vvp_bit4_t bit = BIT4_0;

	    ಯದಿ (that.value(idx))
		  bit = BIT4_1;

	    res.set_bit(idx, bit);
      }

      ಪ್ರತ್ಯಾಗಮ res;
}

ಸ್ಥಿರ ತಾರ್ಕಿಕ c4string_header_test(ಧ್ರುವ ವರ್ಣ*str)
{
      ಯದಿ ((str[0] != 'C') && (str[0] != 'c'))
	    ಪ್ರತ್ಯಾಗಮ ಅಸತ್ಯ;
      ಯದಿ ((str[1] != '4') || (str[2] != '<'))
	    ಪ್ರತ್ಯಾಗಮ ಅಸತ್ಯ;
      ಪ್ರತ್ಯಾಗಮ ಸತ್ಯ;
}

ತಾರ್ಕಿಕ c4string_test(ಧ್ರುವ ವರ್ಣ*str)
{
      ಯದಿ (!c4string_header_test(str))
	    ಪ್ರತ್ಯಾಗಮ ಅಸತ್ಯ;
      size_t ಮೌಲ್ಯ_size = strspn(str+3, "01xz");
      ಯದಿ (str[3+ಮೌಲ್ಯ_size] != '>')
	    ಪ್ರತ್ಯಾಗಮ ಅಸತ್ಯ;
      ಯದಿ (str[3+ಮೌಲ್ಯ_size+1] != 0)
	    ಪ್ರತ್ಯಾಗಮ ಅಸತ್ಯ;

      ಪ್ರತ್ಯಾಗಮ ಸತ್ಯ;
}

vvp_vector4_t c4string_to_vector4(ಧ್ರುವ ವರ್ಣ*str)
{
      ಪ್ರತಿಜ್ಞಾ(c4string_header_test(str));

      str += 3;
      ಧ್ರುವ ವರ್ಣ*tp = str + strspn(str,"01xz");
      ಪ್ರತಿಜ್ಞಾ(tp[0] == '>');

      vvp_vector4_t tmp (tp-str);

      ಪ್ರತಿ (unsigned idx = 0 ;  idx < tmp.size() ;  idx += 1) {
	    vvp_bit4_t bit;
	    ವಿಕಲ್ಪ (str[idx]) {
		ಪಕ್ಷ '0':
		  bit = BIT4_0;
		  ಭಙ್ಗ;
		ಪಕ್ಷ '1':
		  bit = BIT4_1;
		  ಭಙ್ಗ;
		ಪಕ್ಷ 'x':
		  bit = BIT4_X;
		  ಭಙ್ಗ;
		ಪಕ್ಷ 'z':
		  bit = BIT4_Z;
		  ಭಙ್ಗ;
		ಸ್ವಭಾವ:
		  fprintf(stderr, "Unsupported bit value %c(%d).\n", str[idx],
		          str[idx]);
		  ಪ್ರತಿಜ್ಞಾ(0);
		  bit = BIT4_0;
		  ಭಙ್ಗ;
	    }
	    tmp.set_bit(tmp.size()-idx-1, bit);
      }

      ಪ್ರತ್ಯಾಗಮ tmp;
}

ostream& operator<< (ostream&out, ಧ್ರುವ vvp_vector2_t&that)
{
      ಯದಿ (that.is_NaN()) {
	    out << "NaN";

      } ಅನ್ಯಥಾ {
	    out << vector2_to_vector4(that, that.size());
      }
      ಪ್ರತ್ಯಾಗಮ out;
}

vvp_vector8_t::vvp_vector8_t(ಧ್ರುವ vvp_vector8_t&that)
{
      size_ = that.size_;
      ಯದಿ (size_ <= ಪರಿಮಾಣ(val_)) {
	    memcpy(val_, that.val_, ಪರಿಮಾಣ val_);
      } ಅನ್ಯಥಾ {
	    ptr_ = ನೂತನ unsigned ವರ್ಣ[size_];
	    memcpy(ptr_, that.ptr_, size_);
      }
}

vvp_vector8_t::vvp_vector8_t(ಧ್ರುವ vvp_vector4_t&that,
			     unsigned str0, unsigned str1)
: size_(that.size())
{
      ಯದಿ (size_ == 0)
	    ಪ್ರತ್ಯಾಗಮ;

      ಯದಿ (size_ <= ಪರಿಮಾಣ(val_)) {
	    ptr_ = 0; // Prefill all val_ bytes
	    ಪ್ರತಿ (unsigned idx = 0 ; idx < size_ ; idx += 1)
		  val_[idx] = vvp_scalar_t(that.value(idx),str0, str1).raw();
      } ಅನ್ಯಥಾ {
	    ptr_ = ನೂತನ unsigned ವರ್ಣ[size_];
	    ಪ್ರತಿ (unsigned idx = 0 ;  idx < size_ ;  idx += 1)
		  ptr_[idx] = vvp_scalar_t(that.value(idx), str0, str1).raw();
      }
}

vvp_vector8_t::vvp_vector8_t(ಧ್ರುವ vvp_vector2_t&that,
			     unsigned str0, unsigned str1)
: size_(that.size())
{
      ಯದಿ (size_ == 0)
	    ಪ್ರತ್ಯಾಗಮ;

      ಯದಿ (size_ <= ಪರಿಮಾಣ(val_)) {
	    ptr_ = 0;
	    ಪ್ರತಿ (unsigned idx = 0 ; idx < size_ ; idx += 1)
		  val_[idx] = vvp_scalar_t(that.value(idx)? BIT4_1:BIT4_0, str0, str1).raw();
      } ಅನ್ಯಥಾ {
	    ptr_ = ನೂತನ unsigned ವರ್ಣ[size_];
	    ಪ್ರತಿ (unsigned idx = 0 ; idx < size_ ; idx += 1)
		  ptr_[idx] = vvp_scalar_t(that.value(idx)? BIT4_1:BIT4_0, str0, str1).raw();
      }
}

ಧ್ರುವ vvp_vector8_t vvp_vector8_t::nil;

vvp_vector8_t& vvp_vector8_t::operator= (ಧ್ರುವ vvp_vector8_t&that)
{
	// Assign to self.
      ಯದಿ (ಏತತ್ == &that)
	    ಪ್ರತ್ಯಾಗಮ *ಏತತ್;

      ಯದಿ (size_ != that.size_) {
	    ಯದಿ (size_ > ಪರಿಮಾಣ(val_))
		  delete[]ptr_;
	    size_ = 0;
      }

      ಯದಿ (that.size_ == 0) {
	    ಪ್ರತಿಜ್ಞಾ(size_ == 0);
	    ಪ್ರತ್ಯಾಗಮ *ಏತತ್;
      }

      ಯದಿ (that.size_ <= ಪರಿಮಾಣ(val_)) {
	    size_ = that.size_;
	    memcpy(val_, that.val_, ಪರಿಮಾಣ(val_));
	    ಪ್ರತ್ಯಾಗಮ *ಏತತ್;
      }

      ಯದಿ (size_ == 0) {
	    size_ = that.size_;
	    ptr_ = ನೂತನ unsigned ವರ್ಣ[size_];
      }

      memcpy(ptr_, that.ptr_, size_);

      ಪ್ರತ್ಯಾಗಮ *ಏತತ್;
}

vvp_vector8_t vvp_vector8_t::subvalue(unsigned base, unsigned wid) ಧ್ರುವ
{
      vvp_vector8_t tmp (wid);

      unsigned ವರ್ಣ*tmp_ptr = tmp.size_ <= ಪರಿಮಾಣ(val_) ? tmp.val_ : tmp.ptr_;
      ಧ್ರುವ unsigned ವರ್ಣ*use_ptr = size_ <= ಪರಿಮಾಣ(val_) ? val_ : ptr_;

      unsigned idx = 0;
      ಯಾವತ್ ((idx < wid) && (base+idx < size_)) {
	    tmp_ptr[idx] = use_ptr[base+idx];
	    idx += 1;
      }

      ಪ್ರತ್ಯಾಗಮ tmp;
}

ಶೂನ್ಯ vvp_vector8_t::set_vec(unsigned base, ಧ್ರುವ vvp_vector8_t&that)
{
      ಪ್ರತಿಜ್ಞಾ((base+that.size()) <= size());
      ಪ್ರತಿ (unsigned idx = 0 ; idx < that.size() ; idx += 1)
	    set_bit(base+idx, that.value(idx));
}

vvp_vector8_t part_expand(ಧ್ರುವ vvp_vector8_t&that, unsigned wid, unsigned off)
{
      ಪ್ರತಿಜ್ಞಾ(off < wid);
      vvp_vector8_t tmp (wid);

      unsigned ವರ್ಣ* tmp_ptr = tmp.size_<= ಪರಿಮಾಣ(tmp.val_) ?
                               tmp.val_ : tmp.ptr_;
      ಧ್ರುವ unsigned ವರ್ಣ* that_ptr = that.size_<= ಪರಿಮಾಣ(that.val_) ?
                                      that.val_ : that.ptr_;

      unsigned idx = off;

      ಯಾವತ್ (idx < wid && that.size_ > (idx-off)) {
	    tmp_ptr[idx] = that_ptr[idx-off];
	    idx += 1;
      }

      ಪ್ರತ್ಯಾಗಮ tmp;
}

ಸ್ಥಿರ ತಾರ್ಕಿಕ c8string_header_test(ಧ್ರುವ ವರ್ಣ*str)
{
      ಯದಿ ((str[0] != 'C') && (str[0] != 'c'))
	    ಪ್ರತ್ಯಾಗಮ ಅಸತ್ಯ;
      ಯದಿ ((str[1] != '8') || (str[2] != '<'))
	    ಪ್ರತ್ಯಾಗಮ ಅಸತ್ಯ;
      ಪ್ರತ್ಯಾಗಮ ಸತ್ಯ;
}

ತಾರ್ಕಿಕ c8string_test(ಧ್ರುವ ವರ್ಣ*str)
{
      ಯದಿ (!c8string_header_test(str))
	    ಪ್ರತ್ಯಾಗಮ ಅಸತ್ಯ;

      ಧ್ರುವ ವರ್ಣ*cp = str+3;
      ಪ್ರತಿ (;; cp += 1) {
	    ಯದಿ (cp[0] == '>' && cp[1] == 0) ಪ್ರತ್ಯಾಗಮ ಸತ್ಯ;
	    ಯದಿ (cp[0] >= '0' && cp[0] <= '9') ಅನುವರ್ತ;
	    ಯದಿ (cp[0] == 'x') ಅನುವರ್ತ;
	    ಯದಿ (cp[0] == 'z') ಅನುವರ್ತ;
	    ಪ್ರತ್ಯಾಗಮ ಅಸತ್ಯ;
      }
      ಪ್ರತ್ಯಾಗಮ ಅಸತ್ಯ;
}
/*
 * The format of a C8<> string is:
 *   C8<aaabbbccc...>
 * where aaa... is a 3 character bit descriptor.
 */
vvp_vector8_t c8string_to_vector8(ಧ್ರುವ ವರ್ಣ*str)
{
      ಪ್ರತಿಜ್ಞಾ(c8string_header_test(str));

      size_t vsize = strlen(str)-4;
      ಪ್ರತಿಜ್ಞಾ(vsize%3 == 0);
      vsize /= 3;
      vvp_vector8_t tmp (vsize);

      ಪ್ರತಿ (size_t idx = 0 ; idx < vsize ; idx += 1) {
	    ಧ್ರುವ ವರ್ಣ*cp = str+3+3*idx;
	    vvp_bit4_t bit = BIT4_X;
	    unsigned dr0 = cp[0]-'0';
	    unsigned dr1 = cp[1]-'0';
	    ವಿಕಲ್ಪ (cp[2]) {
		ಪಕ್ಷ '0':
		  bit = BIT4_0;
		  ಭಙ್ಗ;
		ಪಕ್ಷ '1':
		  bit = BIT4_1;
		  ಭಙ್ಗ;
		ಪಕ್ಷ 'x':
		  bit = BIT4_X;
		  ಭಙ್ಗ;
		ಪಕ್ಷ 'z':
		  bit = BIT4_Z;
		  ಭಙ್ಗ;
	    }
	    tmp.set_bit(vsize-idx-1, vvp_scalar_t(bit, dr0, dr1));
      }

      ಪ್ರತ್ಯಾಗಮ tmp;
}

ostream& operator<<(ostream&out, ಧ್ರುವ vvp_vector8_t&that)
{
      out << "C8<";
      ಪ್ರತಿ (unsigned idx = 0 ;  idx < that.size() ; idx += 1)
	    out << that.value(that.size()-idx-1);

      out << ">";
      ಪ್ರತ್ಯಾಗಮ out;
}

vvp_net_fun_t::vvp_net_fun_t()
{
      ಎಣಿಕೆ_functors += 1;
}

vvp_net_fun_t::~vvp_net_fun_t()
{
}

ಶೂನ್ಯ vvp_net_fun_t::recv_vec4(vvp_net_ptr_t, ಧ್ರುವ vvp_vector4_t&,
                              vvp_context_t)
{
      fprintf(stderr, "internal error: %s: recv_vec4 not implemented\n",
	      typeid(*ಏತತ್).name());
      ಪ್ರತಿಜ್ಞಾ(0);
}

ಶೂನ್ಯ vvp_net_fun_t::recv_vec4_pv_(vvp_net_ptr_t p, ಧ್ರುವ vvp_vector4_t&bit,
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

      ಪ್ರತಿಜ್ಞಾ(base + bit.size() <= vwid);

      vvp_vector4_t tmp(vwid, BIT4_Z);
      tmp.set_vec(base, bit);
      recv_vec4(p, tmp, 0);
}

ಶೂನ್ಯ vvp_net_fun_t::recv_vec4_pv(vvp_net_ptr_t, ಧ್ರುವ vvp_vector4_t&bit,
                                 unsigned base, unsigned vwid, vvp_context_t)
{
      cerr << "internal error: " << typeid(*ಏತತ್).name() << ": "
	   << "recv_vec4_pv(" << bit << ", " << base
	   << ", " << vwid << ") not implemented" << endl;
      ಪ್ರತಿಜ್ಞಾ(0);
}

ಶೂನ್ಯ vvp_net_fun_t::recv_vec8(vvp_net_ptr_t port, ಧ್ರುವ vvp_vector8_t&bit)
{
      recv_vec4(port, reduce4(bit), 0);
}

ಶೂನ್ಯ vvp_net_fun_t::recv_vec8_pv_(vvp_net_ptr_t p, ಧ್ರುವ vvp_vector8_t&bit,
				  unsigned base, unsigned vwid)
{
	// This is the strength-aware version of recv_vec4_pv_.

      ಪ್ರತಿಜ್ಞಾ(base + bit.size() <= vwid);

      vvp_vector8_t tmp(vwid);
      tmp.set_vec(base, bit);
      recv_vec8(p, tmp);
}

ಶೂನ್ಯ vvp_net_fun_t::recv_vec8_pv(vvp_net_ptr_t port, ಧ್ರುವ vvp_vector8_t&bit,
				 unsigned base, unsigned vwid)
{
      recv_vec4_pv(port, reduce4(bit), base, vwid, 0);
}

ಶೂನ್ಯ vvp_net_fun_t::recv_real(vvp_net_ptr_t, ದ್ವಿಗುಣ bit, vvp_context_t)
{
      fprintf(stderr, "internal error: %s: recv_real(%f) not implemented\n",
	      typeid(*ಏತತ್).name(), bit);
      ಪ್ರತಿಜ್ಞಾ(0);
}

ಶೂನ್ಯ vvp_net_fun_t::recv_string(vvp_net_ptr_t, ಧ್ರುವ std::string&bit, vvp_context_t)
{
      fprintf(stderr, "internal error: %s: recv_string(%s) not implemented\n",
	      typeid(*ಏತತ್).name(), bit.c_str());
      ಪ್ರತಿಜ್ಞಾ(0);
}

ಶೂನ್ಯ vvp_net_fun_t::recv_object(vvp_net_ptr_t, vvp_object_t, vvp_context_t)
{
      fprintf(stderr, "internal error: %s: recv_object(...) not implemented\n",
	      typeid(*ಏತತ್).name());
      ಪ್ರತಿಜ್ಞಾ(0);
}

ಶೂನ್ಯ vvp_net_fun_t::force_flag(ತಾರ್ಕಿಕ)
{
}

/* **** vvp_fun_drive methods **** */

vvp_fun_drive::vvp_fun_drive(unsigned str0, unsigned str1)
{
      ಪ್ರತಿಜ್ಞಾ(str0 < 8);
      ಪ್ರತಿಜ್ಞಾ(str1 < 8);

      drive0_ = str0;
      drive1_ = str1;
}

vvp_fun_drive::~vvp_fun_drive()
{
}

ಶೂನ್ಯ vvp_fun_drive::recv_vec4(vvp_net_ptr_t port, ಧ್ರುವ vvp_vector4_t&bit,
                              vvp_context_t)
{
      ಪ್ರತಿಜ್ಞಾ(port.port() == 0);
      port.ptr()->send_vec8(vvp_vector8_t(bit, drive0_, drive1_));
}

ಶೂನ್ಯ vvp_fun_drive::recv_vec4_pv(vvp_net_ptr_t ptr, ಧ್ರುವ vvp_vector4_t&bit,
				 unsigned base, unsigned vwid, vvp_context_t ctx)
{
      recv_vec4_pv_(ptr, bit, base, vwid, ctx);
}

/* **** vvp_wide_fun_* methods **** */

vvp_wide_fun_core::vvp_wide_fun_core(vvp_net_t*net, unsigned nports)
{
      ptr_ = net;
      nports_ = nports;
      port_ಮೌಲ್ಯಗಳು = 0;
      port_rvalues_ = 0;
}

vvp_wide_fun_core::~vvp_wide_fun_core()
{
      delete[] port_ಮೌಲ್ಯಗಳು;
      delete[] port_rvalues_;
}

ಶೂನ್ಯ vvp_wide_fun_core::propagate_vec4(ಧ್ರುವ vvp_vector4_t&bit,
				       vvp_time64_t delay)
{
      ಯದಿ (delay)
	    schedule_propagate_vector(ptr_, delay, bit);
      ಅನ್ಯಥಾ
	    ptr_->send_vec4(bit, 0);
}

ಶೂನ್ಯ vvp_wide_fun_core::propagate_real(ದ್ವಿಗುಣ bit,
				       vvp_time64_t delay)
{
      ಯದಿ (delay) {
	      // schedule_assign_vector(ptr_->out, bit, delay);
	    ಪ್ರತಿಜ್ಞಾ(0); // Need a real-value version of assign_vector.
      } ಅನ್ಯಥಾ {
	    ptr_->send_real(bit, 0);
      }
}


unsigned vvp_wide_fun_core::port_ಎಣಿಕೆ() ಧ್ರುವ
{
      ಪ್ರತ್ಯಾಗಮ nports_;
}

vvp_vector4_t& vvp_wide_fun_core::value(unsigned idx)
{
      ಪ್ರತಿಜ್ಞಾ(idx < nports_);
      ಯದಿ (port_ಮೌಲ್ಯಗಳು == 0)
	    port_ಮೌಲ್ಯಗಳು = ನೂತನ vvp_vector4_t [nports_];
      ಪ್ರತ್ಯಾಗಮ port_ಮೌಲ್ಯಗಳು[idx];
}

ದ್ವಿಗುಣ vvp_wide_fun_core::ಮೌಲ್ಯ_r(unsigned idx)
{
      ಪ್ರತಿಜ್ಞಾ(idx < nports_);
      ಪ್ರತ್ಯಾಗಮ port_rvalues_? port_rvalues_[idx] : 0.0;
}

ಶೂನ್ಯ vvp_wide_fun_core::recv_real_from_inputs(unsigned)
{
      ಪ್ರತಿಜ್ಞಾ(0);
}

ಶೂನ್ಯ vvp_wide_fun_core::dispatch_vec4_from_ಒಳಹರಿವು(unsigned port,
						  ಧ್ರುವ vvp_vector4_t&bit)
{
      ಪ್ರತಿಜ್ಞಾ(port < nports_);
      ಯದಿ (port_ಮೌಲ್ಯಗಳು == 0) port_ಮೌಲ್ಯಗಳು = ನೂತನ vvp_vector4_t [nports_];
      port_ಮೌಲ್ಯಗಳು[port] = bit;
      recv_vec4_from_inputs(port);
}

ಶೂನ್ಯ vvp_wide_fun_core::dispatch_real_from_ಒಳಹರಿವು(unsigned port,
						  ದ್ವಿಗುಣ bit)
{
      ಪ್ರತಿಜ್ಞಾ(port < nports_);
      ಯದಿ (port_rvalues_ == 0) port_rvalues_ = ನೂತನ ದ್ವಿಗುಣ[nports_];
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

ಶೂನ್ಯ vvp_wide_fun_t::recv_vec4(vvp_net_ptr_t port, ಧ್ರುವ vvp_vector4_t&bit,
                               vvp_context_t)
{
      unsigned pidx = port_base_ + port.port();
      core_->dispatch_vec4_from_ಒಳಹರಿವು(pidx, bit);
}

ಶೂನ್ಯ vvp_wide_fun_t::recv_vec4_pv(vvp_net_ptr_t ptr, ಧ್ರುವ vvp_vector4_t&bit,
				  unsigned base, unsigned vwid, vvp_context_t ctx)
{
      recv_vec4_pv_(ptr, bit, base, vwid, ctx);
}

ಶೂನ್ಯ vvp_wide_fun_t::recv_real(vvp_net_ptr_t port, ದ್ವಿಗುಣ bit,
                               vvp_context_t)
{
      unsigned pidx = port_base_ + port.port();
      core_->dispatch_real_from_ಒಳಹರಿವು(pidx, bit);
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

unsigned vvp_scalar_t::strength0() ಧ್ರುವ
{
      ಪ್ರತ್ಯಾಗಮ STREN0(ಮೌಲ್ಯ_2);
}

unsigned vvp_scalar_t::strength1() ಧ್ರುವ
{
      ಪ್ರತ್ಯಾಗಮ STREN1(ಮೌಲ್ಯ_2);
}

ostream& operator <<(ostream&out, vvp_scalar_t a)
{
      out << a.strength0() << a.strength1();
      ವಿಕಲ್ಪ (a.value()) {
	  ಪಕ್ಷ BIT4_0:
	    out << "0";
	    ಭಙ್ಗ;
	  ಪಕ್ಷ BIT4_1:
	    out << "1";
	    ಭಙ್ಗ;
	  ಪಕ್ಷ BIT4_X:
	    out << "X";
	    ಭಙ್ಗ;
	  ಪಕ್ಷ BIT4_Z:
	    out << "Z";
	    ಭಙ್ಗ;
      }
      ಪ್ರತ್ಯಾಗಮ out;
}

/*
 * This function is only called if the actual interface function rules
 * out some of the easy cases. If we get here, we can assume that
 * neither of the values is HiZ, and the values are not exactly equal.
 */
vvp_scalar_t fully_featured_resolv_(vvp_scalar_t a, vvp_scalar_t b)
{

      ಯದಿ (UNAMBIG(a.ಮೌಲ್ಯ_2) && UNAMBIG(b.ಮೌಲ್ಯ_2)) {

	      /* If both signals are unambiguous, simply choose
		 the stronger. If they have the same strength
		 but different values, then this becomes
		 ambiguous. */

	    ಯದಿ ((b.ಮೌಲ್ಯ_2&0x07) > (a.ಮೌಲ್ಯ_2&0x07)) {

		    /* b value is stronger. Take it. */
		  ಪ್ರತ್ಯಾಗಮ b;

	    } ಅನ್ಯಥಾ ಯದಿ ((b.ಮೌಲ್ಯ_2&0x77) == (a.ಮೌಲ್ಯ_2&0x77)) {

		    // Strengths are the same. Since we know already
		    // that the values are not the same, Make value
		    // into "x".
		  vvp_scalar_t tmp (a);
		  tmp.ಮೌಲ್ಯ_2 = (tmp.ಮೌಲ್ಯ_2&0x77) | 0x80;
		  ಪ್ರತ್ಯಾಗಮ tmp;

	    } ಅನ್ಯಥಾ {

		    /* Must be "a" is the stronger one. */
		  ಪ್ರತ್ಯಾಗಮ a;
	    }

      }

	/* If one of the signals is unambiguous, then it
	   will sweep up the weaker parts of the ambiguous
	   signal. The result may be ambiguous, or maybe not. */

      ಯದಿ (UNAMBIG(a.ಮೌಲ್ಯ_2)) {
	    vvp_scalar_t res;

	    ಯದಿ ((a.ಮೌಲ್ಯ_2&0x70) > (b.ಮೌಲ್ಯ_2&0x70))
		  res.ಮೌಲ್ಯ_2 |= a.ಮೌಲ್ಯ_2&0xf0;
	    ಅನ್ಯಥಾ
		  res.ಮೌಲ್ಯ_2 |= b.ಮೌಲ್ಯ_2&0xf0;

	    ಯದಿ ((a.ಮೌಲ್ಯ_2&0x07) > (b.ಮೌಲ್ಯ_2&0x07))
		  res.ಮೌಲ್ಯ_2 |= a.ಮೌಲ್ಯ_2&0x0f;
	    ಅನ್ಯಥಾ
		  res.ಮೌಲ್ಯ_2 |= b.ಮೌಲ್ಯ_2&0x0f;

	    ಪ್ರತ್ಯಾಗಮ res;

      } ಅನ್ಯಥಾ ಯದಿ (UNAMBIG(b.ಮೌಲ್ಯ_2)) {

	    vvp_scalar_t res;

	    ಯದಿ ((b.ಮೌಲ್ಯ_2&0x70) > (a.ಮೌಲ್ಯ_2&0x70))
		  res.ಮೌಲ್ಯ_2 |= b.ಮೌಲ್ಯ_2&0xf0;
	    ಅನ್ಯಥಾ
		  res.ಮೌಲ್ಯ_2 |= a.ಮೌಲ್ಯ_2&0xf0;

	    ಯದಿ ((b.ಮೌಲ್ಯ_2&0x07) > (a.ಮೌಲ್ಯ_2&0x07))
		  res.ಮೌಲ್ಯ_2 |= b.ಮೌಲ್ಯ_2&0x0f;
	    ಅನ್ಯಥಾ
		  res.ಮೌಲ್ಯ_2 |= a.ಮೌಲ್ಯ_2&0x0f;

	    ಪ್ರತ್ಯಾಗಮ res;

      }


	/* If both signals are ambiguous, then the result
	   has an even wider ambiguity. */

      unsigned tmp = 0;
      ಪೂರ್ಣಾಙ್ಕ sv1a = (a.ಮೌಲ್ಯ_2 & 0x80) ? STREN1(a.ಮೌಲ್ಯ_2) : - STREN1(a.ಮೌಲ್ಯ_2);
      ಪೂರ್ಣಾಙ್ಕ sv0a = (a.ಮೌಲ್ಯ_2 & 0x08) ? STREN0(a.ಮೌಲ್ಯ_2) : - STREN0(a.ಮೌಲ್ಯ_2);
      ಪೂರ್ಣಾಙ್ಕ sv1b = (b.ಮೌಲ್ಯ_2 & 0x80) ? STREN1(b.ಮೌಲ್ಯ_2) : - STREN1(b.ಮೌಲ್ಯ_2);
      ಪೂರ್ಣಾಙ್ಕ sv0b = (b.ಮೌಲ್ಯ_2 & 0x08) ? STREN0(b.ಮೌಲ್ಯ_2) : - STREN0(b.ಮೌಲ್ಯ_2);

      ಪೂರ್ಣಾಙ್ಕ sv1 = sv1a;
      ಪೂರ್ಣಾಙ್ಕ sv0 = sv0a;

      ಯದಿ (sv0a > sv1)
	    sv1 = sv0a;
      ಯದಿ (sv1b > sv1)
	    sv1 = sv1b;
      ಯದಿ (sv0b > sv1)
	    sv1 = sv0b;

      ಯದಿ (sv1a < sv0)
	    sv0 = sv1a;
      ಯದಿ (sv1b < sv0)
	    sv0 = sv1b;
      ಯದಿ (sv0b < sv0)
	    sv0 = sv0b;

      ಯದಿ (sv1 > 0) {
	    tmp |= 0x80;
	    tmp |= sv1 << 4;
      } ಅನ್ಯಥಾ {
	      /* Set the MSB when both arguments MSBs are set. This
		 can only happen if both one strengths are zero. */
	    tmp |= (a.ಮೌಲ್ಯ_2&b.ಮೌಲ್ಯ_2)&0x80;
	    tmp |= (-sv1) << 4;
      }

      ಯದಿ (sv0 > 0) {
	    tmp |= 0x08;
	    tmp |= sv0;
      } ಅನ್ಯಥಾ {
	    tmp |= (-sv0);
      }

      vvp_scalar_t res;
      res.ಮೌಲ್ಯ_2 = tmp;

	/* Canonicalize the HiZ value. */
      ಯದಿ ((res.ಮೌಲ್ಯ_2&0x77) == 0)
	    res.ಮೌಲ್ಯ_2 = 0;

      ಪ್ರತ್ಯಾಗಮ res;
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

vvp_vector4_t reduce4(ಧ್ರುವ vvp_vector8_t&that)
{
      vvp_vector4_t out (that.size());
      ಪ್ರತಿ (unsigned idx = 0 ;  idx < out.size() ;  idx += 1)
	    out.set_bit(idx, that.value(idx).value());

      ಪ್ರತ್ಯಾಗಮ out;
}

vvp_bit4_t compare_gtge(ಧ್ರುವ vvp_vector4_t&lef, ಧ್ರುವ vvp_vector4_t&rig,
			vvp_bit4_t out_if_equal)
{
      unsigned min_size = lef.size();
      ಯದಿ (rig.size() < min_size)
	    min_size = rig.size();

	// If one of the inputs is nil, treat is as all X values, and
	// that makes the result BIT4_X.
      ಯದಿ (min_size == 0)
	    ಪ್ರತ್ಯಾಗಮ BIT4_X;

	// As per the IEEE1364 definition of >, >=, < and <=, if there
	// are any X or Z values in either of the operand vectors,
	// then the result of the compare is BIT4_X.

	// Check for X/Z in the left operand
      ಯದಿ (lef.has_xz())
	    ಪ್ರತ್ಯಾಗಮ BIT4_X;

	// Check for X/Z in the right operand
      ಯದಿ (rig.has_xz())
	    ಪ್ರತ್ಯಾಗಮ BIT4_X;

      ಪ್ರತಿ (unsigned idx = lef.size() ; idx > rig.size() ;  idx -= 1) {
	    ಯದಿ (lef.value(idx-1) == BIT4_1)
		  ಪ್ರತ್ಯಾಗಮ BIT4_1;
      }

      ಪ್ರತಿ (unsigned idx = rig.size() ; idx > lef.size() ;  idx -= 1) {
	    ಯದಿ (rig.value(idx-1) == BIT4_1)
		  ಪ್ರತ್ಯಾಗಮ BIT4_0;
      }

      ಪ್ರತಿ (unsigned idx = min_size ; idx > 0 ;  idx -= 1) {
	    vvp_bit4_t lv = lef.value(idx-1);
	    vvp_bit4_t rv = rig.value(idx-1);

	    ಯದಿ (lv == rv)
		  ಅನುವರ್ತ;

	    ಯದಿ (lv == BIT4_1)
		  ಪ್ರತ್ಯಾಗಮ BIT4_1;
	    ಅನ್ಯಥಾ
		  ಪ್ರತ್ಯಾಗಮ BIT4_0;
      }

      ಪ್ರತ್ಯಾಗಮ out_if_equal;
}

vvp_bit4_t compare_gtge_signed(ಧ್ರುವ vvp_vector4_t&a,
			       ಧ್ರುವ vvp_vector4_t&b,
			       vvp_bit4_t out_if_equal)
{
      ಪ್ರತಿಜ್ಞಾ(a.size() == b.size());

      unsigned sign_idx = a.size()-1;
      vvp_bit4_t a_sign = a.value(sign_idx);
      vvp_bit4_t b_sign = b.value(sign_idx);

      ಯದಿ (bit4_is_xz(a_sign))
	    ಪ್ರತ್ಯಾಗಮ BIT4_X;
      ಯದಿ (bit4_is_xz(b_sign))
	    ಪ್ರತ್ಯಾಗಮ BIT4_X;

      ಯದಿ (a_sign == b_sign)
	    ಪ್ರತ್ಯಾಗಮ compare_gtge(a, b, out_if_equal);

      ಯದಿ (a.has_xz())
	    ಪ್ರತ್ಯಾಗಮ BIT4_X;

      ಯದಿ (b.has_xz())
	    ಪ್ರತ್ಯಾಗಮ BIT4_X;

      ಯದಿ(a_sign == BIT4_0)
	    ಪ್ರತ್ಯಾಗಮ BIT4_1;
      ಅನ್ಯಥಾ
	    ಪ್ರತ್ಯಾಗಮ BIT4_0;
}
