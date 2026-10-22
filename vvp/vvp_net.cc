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

using نطاق std;

/* This is the size of an unsigned long in bits. This is just a
   convenience macro. */
# define CPU_WORD_BITS (8*حجم(unsigned long))
# define قمة_BIT (1UL << (CPU_WORD_BITS-1))

permaheap vvp_net_fun_t::heap_;
permaheap vvp_net_fil_t::heap_;

// Allocate around 1Megabyte/chunk.
ساكن ثابت size_t VVP_NET_CHUNK = 1024*1024/حجم(vvp_net_t);
ساكن vvp_net_t*vvp_net_alloc_table = عدم;
#ifdef CHECK_WITH_VALGRIND
ساكن vvp_net_t **vvp_net_pool = عدم;
ساكن unsigned vvp_net_pool_تعداد = 0;
#endif
ساكن size_t vvp_net_alloc_remaining = 0;
// For statistics, count the vvp_nets allocated and the bytes of alloc
// chunks allocated.
unsigned long تعداد_vvp_nets = 0;
size_t size_vvp_nets = 0;

فراغ* vvp_net_t::operator جديد (size_t size)
{
      أكد(size == حجم(vvp_net_t));
      إذا (vvp_net_alloc_remaining == 0) {
	    vvp_net_alloc_table = ::جديد vvp_net_t[VVP_NET_CHUNK];
	    vvp_net_alloc_remaining = VVP_NET_CHUNK;
	    size_vvp_nets += size*VVP_NET_CHUNK;
#ifdef CHECK_WITH_VALGRIND
	    VALGRIND_MAKE_MEM_NOACCESS(vvp_net_alloc_table, size*VVP_NET_CHUNK);
	    VALGRIND_CREATE_MEMPOOL(vvp_net_alloc_table, 0, 0);
	    vvp_net_pool_تعداد += 1;
	    vvp_net_pool = static_cast<vvp_net_t **>(realloc(vvp_net_pool,
	                   vvp_net_pool_تعداد*حجم(vvp_net_t **)));
	    vvp_net_pool[vvp_net_pool_تعداد-1] = vvp_net_alloc_table;
#endif
      }

      vvp_net_t*return_this = vvp_net_alloc_table;
#ifdef CHECK_WITH_VALGRIND
      VALGRIND_MEMPOOL_ALLOC(vvp_net_pool[vvp_net_pool_تعداد-1],
                             return_this, size);
      return_this->pool = vvp_net_pool[vvp_net_pool_تعداد-1];
#endif
      vvp_net_alloc_table += 1;
      vvp_net_alloc_remaining -= 1;
      تعداد_vvp_nets += 1;
      إرجاع return_this;
}

#ifdef CHECK_WITH_VALGRIND
ساكن map<vvp_net_t*, منطقي> vvp_net_map;
ساكن map<sfunc_core*, منطقي> sfunc_map;
ساكن map<vvp_udp_fun_core*, منطقي> udp_map;
ساكن map<resolv_core*, منطقي> resolv_map;
ساكن vvp_net_t **local_net_pool = 0;
ساكن unsigned local_net_pool_تعداد = 0;

فراغ pool_local_net(vvp_net_t*net)
{
      local_net_pool_تعداد += 1;
      local_net_pool = static_cast<vvp_net_t **>(realloc(local_net_pool,
                       local_net_pool_تعداد*حجم(vvp_net_t **)));
      local_net_pool[local_net_pool_تعداد-1] = net;
}

فراغ vvp_net_delete(vvp_net_t *item)
{
      vvp_net_map[item] = true;
      إذا (sfunc_core*tmp = dynamic_cast<sfunc_core*> (item->fun)) {
	    sfunc_map[tmp] = true;
      }
      إذا (vvp_udp_fun_core*tmp = dynamic_cast<vvp_udp_fun_core*> (item->fun)) {
	    udp_map[tmp] = true;
      }
      إذا (resolv_core*tmp = dynamic_cast<resolv_core*> (item->fun)) {
	    resolv_map[tmp] = true;
      }
	/* Only delete static object variables. */
      إذا (vvp_fun_signal_object_sa*tmp = dynamic_cast<vvp_fun_signal_object_sa*> (item->fun)) {
	    delete tmp;
      }
	/* Only delete static string variables. */
      إذا (vvp_fun_signal_string_sa*tmp = dynamic_cast<vvp_fun_signal_string_sa*> (item->fun)) {
	    delete tmp;
      }
}

فراغ vvp_net_pool_delete()
{
      unsigned long vvp_nets_del = 0;

      لكل (unsigned idx = 0; idx < local_net_pool_تعداد; idx += 1) {
	    vvp_net_delete(local_net_pool[idx]);
      }
      حرر(local_net_pool);
      local_net_pool = 0;
      local_net_pool_تعداد = 0;

      map<vvp_net_t*, منطقي>::iterator iter;
      لكل (iter = vvp_net_map.begin(); iter != vvp_net_map.end(); ++ iter ) {
	    vvp_nets_del += 1;
	    VALGRIND_MEMPOOL_FREE(iter->first->pool, iter->first);
      }
      vvp_net_map.clear();

      map<sfunc_core*, منطقي>::iterator siter;
      لكل (siter = sfunc_map.begin(); siter != sfunc_map.end(); ++ siter ) {
	    delete siter->first;
      }
      sfunc_map.clear();

      map<vvp_udp_fun_core*, منطقي>::iterator uiter;
      لكل (uiter = udp_map.begin(); uiter != udp_map.end(); ++ uiter ) {
	    delete uiter->first;
      }
      udp_map.clear();

      map<resolv_core*, منطقي>::iterator riter;
      لكل (riter = resolv_map.begin(); riter != resolv_map.end(); ++ riter ) {
	    delete riter->first;
      }
      resolv_map.clear();

      إذا (RUNNING_ON_VALGRIND && (vvp_nets_del != تعداد_vvp_nets)) {
	    fflush(عدم);
	    VALGRIND_PRINTF("Error: vvp missed deleting %ld of %lu net(s).",
	                    (long) تعداد_vvp_nets - vvp_nets_del,
	                    تعداد_vvp_nets);
      }

      لكل (unsigned idx = 0; idx < vvp_net_pool_تعداد; idx += 1) {
	    VALGRIND_DESTROY_MEMPOOL(vvp_net_pool[idx]);
	    ::delete [] vvp_net_pool[idx];
      }
      حرر(vvp_net_pool);
      vvp_net_pool = عدم;
      vvp_net_pool_تعداد = 0;
}
#endif

فراغ vvp_net_t::operator delete(فراغ*)
{
      أكد(0);
}

vvp_net_t::vvp_net_t()
: out_(vvp_net_ptr_t(0,0))
{
      fun = 0;
      fil = 0;
}

فراغ vvp_net_t::link(vvp_net_ptr_t port_to_link)
{
      vvp_net_t*net = port_to_link.ptr();

	// Connect nodes with vvp_fun_modpath_src to the head of the
	// linked list, so that vvp_fun_modpath_src are always evaluated
	// before their respective vvp_fun_modpath
      إذا (dynamic_cast<vvp_fun_modpath_src*>(net->fun)) {
	    net->port[port_to_link.port()] = out_;
	    out_ = port_to_link;
	// All other nodes connect after the last vvp_fun_modpath_src
      } وإلا {
	      // List is empty or first node has no vvp_fun_modpath_src
	      // then just insert the node into the head of linked list
	    إذا (out_.ptr() == nullptr || ! dynamic_cast<vvp_fun_modpath_src*>(out_.ptr()->fun)) {
		  net->port[port_to_link.port()] = out_;
		  out_ = port_to_link;
		  إرجاع;
	    }

	    vvp_net_ptr_t cur = out_;
	    vvp_net_ptr_t prev = vvp_net_ptr_t(nullptr,0);

	      // Traverse through linked list
	    بينما (dynamic_cast<vvp_fun_modpath_src*>(cur.ptr()->fun)) {
		  prev = cur;
		  cur = cur.ptr()->port[cur.port()];
		  إذا (! cur.ptr()) اكسر; // End of list
	    }

	    أكد(prev.ptr());

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
فراغ vvp_net_t::unlink(vvp_net_ptr_t dst_ptr)
{
      vvp_net_t*net = dst_ptr.ptr();
      unsigned net_port = dst_ptr.port();

      إذا (out_ == dst_ptr) {
	      /* If the drive fan-out list starts with this pointer,
		 then the unlink is easy. Pull the list forward. */
	    out_ = net->port[net_port];
      } وإلا إذا (! out_.nil()) {
	      /* Scan the linked list, looking for the net_ptr_t
		 pointer *before* the one we wish to remove. */
	    vvp_net_ptr_t cur = out_;
	    أكد(!cur.nil());
	    vvp_net_t*cur_net = cur.ptr();
	    unsigned cur_port = cur.port();
	    بينما (cur_net && cur_net->port[cur_port] != dst_ptr) {
		  cur = cur_net->port[cur_port];
		  cur_net = cur.ptr();
		  cur_port = cur.port();
	    }
	      /* Unlink. */
	    إذا (cur_net) cur_net->port[cur_port] = net->port[net_port];
      }

      net->port[net_port] = vvp_net_ptr_t(0,0);
}

فراغ vvp_net_t::تعداد_drivers(unsigned idx, unsigned counts[4])
{
      counts[0] = 0;
      counts[1] = 0;
      counts[2] = 0;
      counts[3] = 0;

        /* $countdrivers can only be used on wires. */
      vvp_wire_base*wire=dynamic_cast<vvp_wire_base*>(fil);
      أكد(wire);

      إذا (wire->is_forced(idx))
            counts[3] = 1;

        /* If the net has multiple drivers, we need to interrogate the
           resolver network to get the driven values. */
      إذا (resolv_core*resolver = dynamic_cast<resolv_core*>(fun)) {
            resolver->تعداد_drivers(idx, counts);
            إرجاع;
      }
      إذا (vvp_island_port*resolver = dynamic_cast<vvp_island_port*>(fun)) {
            resolver->تعداد_drivers(idx, counts);
            إرجاع;
      }

        /* If the functor is not a resolver, there is only one driver, so
           we can just interrogate the filter to get the driven value. */
      update_driver_counts(wire->driven_قيمة(idx), counts);
}

فراغ vvp_net_fun_t::operator delete(فراغ*)
{
#ifndef CHECK_WITH_VALGRIND
      أكد(0);
#endif
}


vvp_net_fil_t::vvp_net_fil_t()
{
      force_link_ = 0;
      force_propagate_ = خطأ;
      تعداد_filters += 1;
}

vvp_net_fil_t::~vvp_net_fil_t()
{
      أكد(force_link_ == 0);
}

vvp_net_fil_t::prop_t vvp_net_fil_t::filter_vec4(ثابت vvp_vector4_t&,
                                                 vvp_vector4_t&,
                                                 unsigned, unsigned)
{
      إرجاع PROP;
}

vvp_net_fil_t::prop_t vvp_net_fil_t::filter_vec8(ثابت vvp_vector8_t&,
                                                 vvp_vector8_t&,
                                                 unsigned, unsigned)
{
      إرجاع PROP;
}

vvp_net_fil_t::prop_t vvp_net_fil_t::filter_real(مزدوج&)
{
      إرجاع PROP;
}

vvp_net_fil_t::prop_t vvp_net_fil_t::filter_string(ثابت string&)
{
      إرجاع PROP;
}

vvp_net_fil_t::prop_t vvp_net_fil_t::filter_object(vvp_object_t&)
{
      إرجاع PROP;
}

فراغ vvp_net_fil_t::force_mask(ثابت vvp_vector2_t&mask)
{
      إذا (force_mask_.size() == 0)
	    force_mask_ = vvp_vector2_t(vvp_vector2_t::FILL0, mask.size());

      أكد(force_mask_.size() == mask.size());
      لكل (unsigned idx = 0 ; idx < mask.size() ; idx += 1) {
	    إذا (mask.value(idx) == 0)
		  استمر;

	    force_mask_.set_bit(idx, 1);
	    force_propagate_ = true;
      }
}

فراغ vvp_net_fil_t::release_mask(ثابت vvp_vector2_t&mask)
{
      إذا (force_mask_.size() == 0)
	    إرجاع;

      أكد(force_mask_.size() == mask.size());
      لكل (unsigned idx = 0 ; idx < mask.size() ; idx += 1) {
	    إذا (mask.value(idx))
		  force_mask_.set_bit(idx, 0);
      }

      إذا (force_mask_.is_zero())
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
فراغ vvp_net_fil_t::force_link(vvp_net_t*dst, vvp_net_t*src)
{
      أكد(dst->fil == هذا);

      إذا (force_link_ == 0) {
	    force_link_ = جديد vvp_net_t;
	      // Use port[3] to hold the force destination.
	    force_link_->port[3] = vvp_net_ptr_t(dst, 0);
	    force_link_->port[2] = vvp_net_ptr_t(0,0);
	    force_link_->fun = جديد vvp_fun_force;
      }

      force_unlink();
      أكد(force_link_->port[2] == vvp_net_ptr_t(0,0));

	// Use port[2] to hold the force source.
      force_link_->port[2] = vvp_net_ptr_t(src,0);

      vvp_net_ptr_t dst_ptr(force_link_, 0);
      src->link(dst_ptr);
}

فراغ vvp_net_fil_t::operator delete(فراغ*)
{
      أكد(0);
}

فراغ vvp_net_fil_t::force_unlink(فراغ)
{
      إذا (force_link_ == 0) إرجاع;
      vvp_net_t*src = force_link_->port[2].ptr();
      إذا (src == 0) إرجاع;

      src->unlink(vvp_net_ptr_t(force_link_,0));
      force_link_->port[2] = vvp_net_ptr_t(0,0);
}

/* *** BIT operations *** */
vvp_bit4_t add_with_carry(vvp_bit4_t a, vvp_bit4_t b, vvp_bit4_t&c)
{
      إذا (bit4_is_xz(a) || bit4_is_xz(b) || bit4_is_xz(c)) {
	    c = BIT4_X;
	    إرجاع BIT4_X;
      }

	// NOTE: This relies on the facts that XZ values have been
	// weeded out, and that BIT4_1 is 1 and BIT4_0 is 0.
      صحيح sum = (صحيح)a + (صحيح)b + (صحيح)c;

      بدل (sum) {
	  حالة 0:
	      // c must already be 0.
	    إرجاع BIT4_0;
	  حالة 1:
	    c = BIT4_0;
	    إرجاع BIT4_1;
	  حالة 2:
	    c = BIT4_1;
	    إرجاع BIT4_0;
	  حالة 3:
	    c = BIT4_1;
	    إرجاع BIT4_1;
      }
      fprintf(stderr, "Incorrect result %d.\n", sum);
      أكد(0);
      إرجاع BIT4_X;
}

vvp_bit4_t scalar_to_bit4(PLI_INT32 scalar)
{
      بدل(scalar) {
	  حالة vpi0:
	    إرجاع BIT4_0;
	  حالة vpi1:
	    إرجاع BIT4_1;
	  حالة vpiX:
	    إرجاع BIT4_X;
	  حالة vpiZ:
	    إرجاع BIT4_Z;
      }
      fprintf(stderr, "Unsupported scalar value %d.\n", (صحيح)scalar);
      أكد(0);
      إرجاع BIT4_X;
}

vvp_bit4_t operator ^ (vvp_bit4_t a, vvp_bit4_t b)
{
      إذا (bit4_is_xz(a))
	    إرجاع BIT4_X;
      إذا (bit4_is_xz(b))
	    إرجاع BIT4_X;
      إذا (a == BIT4_0)
	    إرجاع b;
      إذا (b == BIT4_0)
	    إرجاع a;
      إرجاع BIT4_0;
}

ostream& operator<<(ostream&out, vvp_bit4_t bit)
{
      بدل (bit) {
	  حالة BIT4_0:
	    out << "0";
	    اكسر;
	  حالة BIT4_1:
	    out << "1";
	    اكسر;
	  حالة BIT4_X:
	    out << "X";
	    اكسر;
	  حالة BIT4_Z:
	    out << "Z";
	    اكسر;
	  افتراضي:
	    out << "?";
	    اكسر;
      }
      إرجاع out;
}

تعريفنوع unsigned short edge_t;

مضمن edge_t VVP_EDGE(vvp_bit4_t from, vvp_bit4_t to)
{
      إرجاع 1 << ((from << 2) | to);
}

ثابت edge_t vvp_edge_posedge
      = VVP_EDGE(BIT4_0,BIT4_1)
      | VVP_EDGE(BIT4_0,BIT4_X)
      | VVP_EDGE(BIT4_0,BIT4_Z)
      | VVP_EDGE(BIT4_X,BIT4_1)
      | VVP_EDGE(BIT4_Z,BIT4_1)
      ;

ثابت edge_t vvp_edge_negedge
      = VVP_EDGE(BIT4_1,BIT4_0)
      | VVP_EDGE(BIT4_1,BIT4_X)
      | VVP_EDGE(BIT4_1,BIT4_Z)
      | VVP_EDGE(BIT4_X,BIT4_0)
      | VVP_EDGE(BIT4_Z,BIT4_0)
      ;

صحيح edge(vvp_bit4_t from, vvp_bit4_t to)
{
      edge_t mask = VVP_EDGE(from, to);
      إذا (mask & vvp_edge_posedge)
	    إرجاع 1;
      إذا (mask & vvp_edge_negedge)
	    إرجاع -1;
      إرجاع 0;
}

unsigned long multiply_with_carry(unsigned long a, unsigned long b,
				  unsigned long&carry)
{
      ثابت unsigned long mask = (1UL << (CPU_WORD_BITS/2)) - 1;
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
      إرجاع (r1 << (CPU_WORD_BITS/2)) + r00;
}


فراغ vvp_send_vec8(vvp_net_ptr_t ptr, ثابت vvp_vector8_t&val)
{
      بينما (vvp_net_t*cur = ptr.ptr()) {
	    vvp_net_ptr_t next_val = cur->port[ptr.port()];

	    إذا (cur->fun)
		  cur->fun->recv_vec8(ptr, val);

	    ptr = next_val;
      }
}

فراغ vvp_send_real(vvp_net_ptr_t ptr, مزدوج val, vvp_context_t context)
{
      بينما (vvp_net_t*cur = ptr.ptr()) {
	    vvp_net_ptr_t next_val = cur->port[ptr.port()];

	    إذا (cur->fun)
		  cur->fun->recv_real(ptr, val, context);

	    ptr = next_val;
      }
}

ثابت vvp_vector4_t vvp_vector4_t::nil;

فراغ vvp_vector4_t::copy_bits(ثابت vvp_vector4_t&that)
{

      إذا (size_ == that.size_) {
	    إذا (size_ > BITS_PER_WORD) {
		  unsigned كلمات = (size_+BITS_PER_WORD-1) / BITS_PER_WORD;
		  لكل (unsigned idx = 0 ;  idx < كلمات ;  idx += 1)
			abits_ptr_[idx] = that.abits_ptr_[idx];
		  لكل (unsigned idx = 0 ;  idx < كلمات ;  idx += 1)
			bbits_ptr_[idx] = that.bbits_ptr_[idx];
	    } وإلا {
		  abits_val_ = that.abits_val_;
		  bbits_val_ = that.bbits_val_;
	    }
	    إرجاع;
      }

	/* Now we know that the sizes of this and that are definitely
	   different. We can use that in code below. In any case, we
	   need to copy only the smaller of the sizes. */

	/* If source and destination are both short, then mask/copy
	   the bit values. */
      إذا (size_ <= BITS_PER_WORD && that.size_ <= BITS_PER_WORD) {
	    unsigned bits_to_copy = (that.size_ < size_) ? that.size_ : size_;
	    unsigned long mask = ((bits_to_copy < BITS_PER_WORD) ? (1UL << bits_to_copy) :
	                                                           (0UL)) - 1UL;
	    abits_val_ &= ~mask;
	    bbits_val_ &= ~mask;
	    abits_val_ |= that.abits_val_&mask;
	    bbits_val_ |= that.bbits_val_&mask;
	    إرجاع;
      }

	/* Now we know that either source or destination are long. If
	   the destination is short, then mask/copy from the low word
	   of the long source. */
      إذا (size_ <= BITS_PER_WORD) {
	    abits_val_ = that.abits_ptr_[0];
	    bbits_val_ = that.bbits_ptr_[0];
	    إذا (size_ < BITS_PER_WORD) {
		  unsigned long mask = (1UL << size_) - 1UL;
		  abits_val_ &= mask;
		  bbits_val_ &= mask;
	    }
	    إرجاع;
      }

	/* Now we know that the destination must be long. If the
	   source is short, then mask/copy from its value. */
      إذا (that.size_ <= BITS_PER_WORD) {
	    unsigned long mask;
	    إذا (that.size_ < BITS_PER_WORD) {
		  mask = (1UL << that.size_) - 1UL;
		  abits_ptr_[0] &= ~mask;
		  bbits_ptr_[0] &= ~mask;
	    } وإلا {
		  mask = -1UL;
	    }
	    abits_ptr_[0] |= that.abits_val_&mask;
	    bbits_ptr_[0] |= that.bbits_val_&mask;
	    إرجاع;
      }

	/* Finally, we know that source and destination are long. copy
	   words until we get to the last. */
      unsigned bits_to_copy = (that.size_ < size_) ? that.size_ : size_;
      unsigned word = 0;
      بينما (bits_to_copy >= BITS_PER_WORD) {
	    abits_ptr_[word] = that.abits_ptr_[word];
	    bbits_ptr_[word] = that.bbits_ptr_[word];
	    bits_to_copy -= BITS_PER_WORD;
	    word += 1;
      }
      إذا (bits_to_copy > 0) {
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
فراغ vvp_vector4_t::copy_from_big_(ثابت vvp_vector4_t&that)
{
      unsigned كلمات = (size_+BITS_PER_WORD-1) / BITS_PER_WORD;
      abits_ptr_ = جديد unsigned long[2*كلمات];
      bbits_ptr_ = abits_ptr_ + كلمات;

      لكل (unsigned idx = 0 ;  idx < كلمات ;  idx += 1)
	    abits_ptr_[idx] = that.abits_ptr_[idx];
      لكل (unsigned idx = 0 ;  idx < كلمات ;  idx += 1)
	    bbits_ptr_[idx] = that.bbits_ptr_[idx];
}

/*
 * The copy_inverted_from_ method is just like the copy_from_ method,
 * except that we combine that with an invert. This allows the ~ and
 * the assignment to be blended in many common cases.
 */
فراغ vvp_vector4_t::copy_inverted_from_(ثابت vvp_vector4_t&that)
{
      size_ = that.size_;
      إذا (size_ > BITS_PER_WORD) {
	    unsigned كلمات = (size_+BITS_PER_WORD-1) / BITS_PER_WORD;
	    abits_ptr_ = جديد unsigned long[2*كلمات];
	    bbits_ptr_ = abits_ptr_ + كلمات;

	    unsigned remaining = size_;
	    unsigned idx = 0;
	    بينما (remaining >= BITS_PER_WORD) {
		  abits_ptr_[idx] = that.bbits_ptr_[idx] | ~that.abits_ptr_[idx];
		  idx += 1;
		  remaining -= BITS_PER_WORD;
	    }
	    إذا (remaining > 0) {
		  unsigned long mask = (1UL<<remaining) - 1UL;
		  abits_ptr_[idx] = mask & (that.bbits_ptr_[idx] | ~that.abits_ptr_[idx]);
	    }

	    لكل (idx = 0 ;  idx < كلمات ;  idx += 1)
		  bbits_ptr_[idx] = that.bbits_ptr_[idx];

      } وإلا {
	    unsigned long mask = (size_<BITS_PER_WORD)? (1UL<<size_)-1UL : -1UL;
	    abits_val_ = mask & (that.bbits_val_ | ~that.abits_val_);
	    bbits_val_ = that.bbits_val_;
      }
}

/* Make sure to set size_ before calling this routine. */
فراغ vvp_vector4_t::allocate_كلمات(unsigned long inita, unsigned long initb)
{
      إذا (size_ > BITS_PER_WORD) {
	    unsigned cnt = (size_ + BITS_PER_WORD - 1) / BITS_PER_WORD;
	    abits_ptr_ = جديد unsigned long[2*cnt];
	    bbits_ptr_ = abits_ptr_ + cnt;
	    لكل (unsigned idx = 0 ;  idx < cnt ;  idx += 1)
		  abits_ptr_[idx] = inita;
	    لكل (unsigned idx = 0 ;  idx < cnt ;  idx += 1)
		  bbits_ptr_[idx] = initb;

      } وإلا {
	    abits_val_ = inita;
	    bbits_val_ = initb;
      }
}

vvp_vector4_t::vvp_vector4_t(unsigned size__, مزدوج val)
: size_(size__)
{
      منطقي is_neg = خطأ;
      مزدوج fraction;
      صحيح exponent;

	/* We return 'bx for a NaN or +/- infinity. */
      إذا (val != val || (val && (val == 0.5*val)))  {
	    allocate_كلمات(WORD_X_ABITS, WORD_X_BBITS);
	    إرجاع;
      }

	/* Convert to a positive result. */
      إذا (val < 0.0) {
	    is_neg = true;
	    val = -val;
      }
      allocate_كلمات(WORD_0_ABITS, WORD_0_BBITS);

	/* Get the exponent and fractional part of the number. */
      fraction = frexp(val, &exponent);

	/* If the value is small enough just use lround(). */
      إذا (exponent < BITS_PER_WORD-2) {
	    إذا (is_neg) هذا->invert();  // Invert the bits if negative.
	    long sval = lround(val);
	    إذا (is_neg) sval = -sval;
	      /* This requires that 0 and 1 have the same bbit value. */
	    إذا (size_ > BITS_PER_WORD) {
		  abits_ptr_[0] = sval;
	    } وإلا {
		  abits_val_ = sval;
	    }
	    إرجاع;
      }

      unsigned nwords = (exponent-1) / BITS_PER_WORD;
      unsigned my_كلمات = (size_ + BITS_PER_WORD - 1) / BITS_PER_WORD - 1;

      fraction = ldexp(fraction, (exponent-1) % BITS_PER_WORD + 1);

	/* Skip any leading bits. */
      لكل (صحيح idx = (signed) nwords; idx > (signed) my_كلمات; idx -=1) {
	    unsigned long bits = (unsigned long) fraction;
	    fraction = fraction - (مزدوج) bits;
	    fraction = ldexp(fraction, BITS_PER_WORD);
      }

	/* Convert the remaining bits as appropriate. */
      إذا (my_كلمات == 0) {
		  unsigned long bits = (unsigned long) fraction;
		  abits_val_ = bits;
		  fraction = fraction - (مزدوج) bits;
		    /* Round any fractional part up. */
		  إذا (fraction >= 0.5) *هذا += (int64_t) 1;
      } وإلا {
	    إذا (nwords < my_كلمات) my_كلمات = nwords;
	    لكل (صحيح idx = (signed)my_كلمات; idx >= 0; idx -= 1) {
		  unsigned long bits = (unsigned long) fraction;
		  abits_ptr_[idx] = bits;
		  fraction = fraction - (مزدوج) bits;
		  fraction = ldexp(fraction, BITS_PER_WORD);
	    }
	      /* Round any fractional part up. */
	    إذا (fraction >= ldexp(0.5, BITS_PER_WORD)) *هذا += (int64_t) 1;
       }

	/* Convert to a negative number if needed. */
      إذا (is_neg) {
	    هذا->invert();
	    *هذا += (int64_t) 1;
      }
}

vvp_vector4_t::vvp_vector4_t(ثابت vvp_vector4_t&that,
			    unsigned adr, unsigned wid)
{
	// Set up and initialize the destination.
      size_ = wid;
      allocate_كلمات(WORD_X_ABITS, WORD_X_BBITS);

	// Special case: selecting from far beyond the source vector,
	// to the result is all X bits. We're done.
      إذا (adr >= that.size_)
	    إرجاع;

	// Special case: The source is not quite big enough to supply
	// all bits, so get the bits that we can. The remainder will
	// be left at BIT4_X.
      إذا ((adr + wid) > that.size_) {
	    unsigned use_wid = that.size_ - adr;
	    لكل (unsigned idx = 0 ; idx < use_wid ; idx += 1)
		  set_bit(idx, that.value(adr+idx));

	    إرجاع;
      }

	// At the point, we know that the source part is entirely
	// contained in the source vector.
	// assert((adr + wid) <= that.size_);

      إذا (wid > BITS_PER_WORD) {
	      /* In this case, the subvector and the source vector are
		 long. Do the transfer reasonably efficiently. */
	    unsigned ptr = adr / BITS_PER_WORD;
	    unsigned long off = adr % BITS_PER_WORD;
	    unsigned long noff = BITS_PER_WORD - off;
	    unsigned long lmask = (1UL << off) - 1UL;
	    unsigned trans = 0;
	    unsigned dst = 0;
	    بينما (trans < wid) {
		    // The low bits of the result.
		  abits_ptr_[dst] = (that.abits_ptr_[ptr] & ~lmask) >> off;
		  bbits_ptr_[dst] = (that.bbits_ptr_[ptr] & ~lmask) >> off;
		  trans += noff;

		  إذا (trans >= wid)
			اكسر;

		  ptr += 1;

		    // The high bits of the result. Skip this if the
		    // source and destination are perfectly aligned.
		  إذا (noff != BITS_PER_WORD) {
			abits_ptr_[dst] |= (that.abits_ptr_[ptr]&lmask) << noff;
			bbits_ptr_[dst] |= (that.bbits_ptr_[ptr]&lmask) << noff;
			trans += off;
		  }

		  dst += 1;
	    }

      } وإلا إذا (that.size_ > BITS_PER_WORD) {
	      /* In this case, the subvector fits in a single word,
		 but the source is large. */
	    unsigned ptr = adr / BITS_PER_WORD;
	    unsigned long off = adr % BITS_PER_WORD;
	    unsigned trans = BITS_PER_WORD - off;
	    إذا (trans > wid)
		  trans = wid;

	    إذا (trans == BITS_PER_WORD) {
		    // Very special case: Copy exactly 1 perfectly
		    // aligned word.
		  abits_val_ = that.abits_ptr_[ptr];
		  bbits_val_ = that.bbits_ptr_[ptr];

	    } وإلا {
		    // lmask is the low bits of the destination,
		    // masked into the source.
		  unsigned long lmask = (1UL<<trans) - 1UL;
		  lmask <<= off;

		    // The low bits of the result.
		  abits_val_ = (that.abits_ptr_[ptr] & lmask) >> off;
		  bbits_val_ = (that.bbits_ptr_[ptr] & lmask) >> off;

		  إذا (trans < wid) {
			  // If there are more bits, then get them
			  // from the bottom of the next word of the
			  // source.
			unsigned long hmask = (1UL << (wid-trans)) - 1UL;

			  // The high bits of the result.
			abits_val_ |= (that.abits_ptr_[ptr+1]&hmask) << trans;
			bbits_val_ |= (that.bbits_ptr_[ptr+1]&hmask) << trans;
		  }
	    }

      } وإلا إذا (size_ == BITS_PER_WORD) {
	      /* We know that source and destination are short. If the
		 destination is a full word, then we know the copy is
		 aligned and complete. */
	    abits_val_ = that.abits_val_;
	    bbits_val_ = that.bbits_val_;

      } وإلا {
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
فراغ vvp_vector4_t::resize(unsigned newsize, vvp_bit4_t pad_bit)
{
      إذا (size_ == newsize)
	    إرجاع;

      unsigned cnt = (size_ + BITS_PER_WORD - 1) / BITS_PER_WORD;
      unsigned long word_pad_abits = 0;
      unsigned long word_pad_bbits = 0;
      بدل (pad_bit) {
	  حالة BIT4_0:
	    word_pad_abits = WORD_0_ABITS;
	    word_pad_bbits = WORD_0_BBITS;
	    اكسر;
	  حالة BIT4_1:
	    word_pad_abits = WORD_1_ABITS;
	    word_pad_bbits = WORD_1_BBITS;
	    اكسر;
	  حالة BIT4_X:
	    word_pad_abits = WORD_X_ABITS;
	    word_pad_bbits = WORD_X_BBITS;
	    اكسر;
	  حالة BIT4_Z:
	    word_pad_abits = WORD_Z_ABITS;
	    word_pad_bbits = WORD_Z_BBITS;
	    اكسر;
      }

      إذا (newsize > BITS_PER_WORD) {
	    unsigned newcnt = (newsize + BITS_PER_WORD - 1) / BITS_PER_WORD;
	    إذا (newcnt == cnt) {
		    // If the word count doesn't change, then there is
		    // no need for re-allocation so we are done now.
		  إذا (newsize > size_) {
			إذا (unsigned fill = size_ % BITS_PER_WORD) {
			      abits_ptr_[cnt-1] &= ~((-1UL) << fill);
			      bbits_ptr_[cnt-1] &= ~((-1UL) << fill);
			      abits_ptr_[cnt-1] |= word_pad_abits << fill;
			      bbits_ptr_[cnt-1] |= word_pad_bbits << fill;
			}
		  }
		  size_ = newsize;
		  إرجاع;
	    }

	    unsigned long*newbits = جديد unsigned long[2*newcnt];

	    إذا (cnt > 1) {
		  unsigned trans = cnt;
		  إذا (trans > newcnt)
			trans = newcnt;

		  لكل (unsigned idx = 0 ;  idx < trans ;  idx += 1)
			newbits[idx] = abits_ptr_[idx];
		  لكل (unsigned idx = 0 ;  idx < trans ;  idx += 1)
			newbits[newcnt+idx] = bbits_ptr_[idx];

		  delete[]abits_ptr_;

	    } وإلا {
		  newbits[0] = abits_val_;
		  newbits[newcnt] = bbits_val_;
	    }

	    إذا (newsize > size_) {
		  إذا (unsigned fill = size_ % BITS_PER_WORD) {
			newbits[cnt-1] &= ~((-1UL) << fill);
			newbits[cnt-1] |= word_pad_abits << fill;
			newbits[newcnt+cnt-1] &= ~((-1UL) << fill);
			newbits[newcnt+cnt-1] |= word_pad_bbits << fill;
		  }
		  لكل (unsigned idx = cnt ;  idx < newcnt ;  idx += 1)
			newbits[idx] = word_pad_abits;
		  لكل (unsigned idx = cnt ;  idx < newcnt ;  idx += 1)
			newbits[newcnt+idx] = word_pad_bbits;
	    }

	    size_ = newsize;
	    abits_ptr_ = newbits;
	    bbits_ptr_ = newbits + newcnt;

      } وإلا {
	    إذا (cnt > 1) {
		  unsigned long newvala = abits_ptr_[0];
		  unsigned long newvalb = bbits_ptr_[0];
		  delete[]abits_ptr_;
		  abits_val_ = newvala;
		  bbits_val_ = newvalb;
	    }

	    إذا (newsize > size_) {
		  abits_val_ &= ~((-1UL) << size_);
		  bbits_val_ &= ~((-1UL) << size_);
		  abits_val_ |= word_pad_abits << size_;
		  bbits_val_ |= word_pad_bbits << size_;
	    }

	    size_ = newsize;
      }
}


unsigned long* vvp_vector4_t::subarray(unsigned adr, unsigned wid, منطقي xz_to_0) ثابت
{
      ثابت unsigned BIT2_PER_WORD = 8*حجم(unsigned long);
      unsigned awid = (wid + BIT2_PER_WORD - 1) / (BIT2_PER_WORD);
      unsigned long*val = جديد unsigned long[awid];

      لكل (unsigned idx = 0 ;  idx < awid ;  idx += 1)
	    val[idx] = 0;

      إذا (size_ <= BITS_PER_WORD) {
	      /* Handle the special case that the array is small. The
		 entire value of the vector4 is within the xbits_val_
		 so we know that the result is a single word, the
		 source is a single word, and we just have to loop
		 through that word. */
	    unsigned long atmp = abits_val_ >> adr;
	    unsigned long btmp = bbits_val_ >> adr;
	    إذا (wid < BIT2_PER_WORD) {
		  atmp &= (1UL << wid) - 1;
		  btmp &= (1UL << wid) - 1;
	    }
	    إذا (btmp) {
		  إذا (xz_to_0)
			atmp &= ~btmp;
		  وإلا
			اذهب x_out;
	    }

	    val[0] = atmp;

      } وإلا {

	    unsigned val_ptr = 0;
	    unsigned val_off = 0;

	      /* Get the first word we are scanning. We may in fact be
		 somewhere in the middle of that word. */
	    بينما (wid > 0) {
		  unsigned long atmp = abits_ptr_[adr/BITS_PER_WORD];
		  unsigned long btmp = bbits_ptr_[adr/BITS_PER_WORD];
		  unsigned long off = adr%BITS_PER_WORD;
		  atmp >>= off;
		  btmp >>= off;

		  unsigned long trans = BITS_PER_WORD - off;
		  إذا (trans > (BIT2_PER_WORD - val_off))
			trans = BIT2_PER_WORD - val_off;
		  إذا (wid < trans)
			trans = wid;
		  إذا (trans < BIT2_PER_WORD) {
			atmp &= (1UL << trans) - 1;
			btmp &= (1UL << trans) - 1;
		  }
		  إذا (btmp) {
			إذا (xz_to_0)
			      atmp &= ~btmp;
			وإلا
			      اذهب x_out;
		  }

		  val[val_ptr] |= atmp << val_off;
		  adr += trans;
		  wid -= trans;
		  val_off += trans;
		  إذا (val_off == BIT2_PER_WORD) {
			val_ptr += 1;
			val_off = 0;
		  }
	    }
      }

      إرجاع val;

 x_out:
      delete[]val;
      إرجاع 0;
}

فراغ vvp_vector4_t::setarray(unsigned adr, unsigned wid, ثابت unsigned long*val)
{
      أكد(adr+wid <= size_);

      ثابت unsigned BIT2_PER_WORD = 8*حجم(unsigned long);

      إذا (size_ <= BITS_PER_WORD) {
	      // We know here that both the source and the target are
	      // within a single word. Write the bits into the
	      // abits_val_ directly.

	    أكد(BIT2_PER_WORD <= BITS_PER_WORD);
	    unsigned long lmask = (1UL << adr) - 1UL;
	    unsigned long hmask = ((adr+wid) < BITS_PER_WORD)
		  ? -1UL << (adr+wid)
		  : 0;
	    unsigned long mask = ~(hmask | lmask);

	    abits_val_ &= ~mask;
	    bbits_val_ &= ~mask;

	    abits_val_ |= mask & (val[0] << adr);

      } وإلا {
	      // The general case, there are multiple words of
	      // destination, and possibly multiple words of source
	      // data. Shift and mask as we go.
	    unsigned off = adr % BITS_PER_WORD;
	    unsigned ptr = adr / BITS_PER_WORD;
	    unsigned val_off = 0;
	    unsigned val_ptr = 0;
	    بينما (wid > 0) {
		  unsigned trans = wid;
		  إذا (trans > (BIT2_PER_WORD-val_off))
			trans = BIT2_PER_WORD-val_off;
		  إذا (trans > (BITS_PER_WORD-off))
			trans = BITS_PER_WORD-off;

		  unsigned long lmask = (1UL << off) - 1UL;
		  unsigned long hmask = ((off+trans) < BITS_PER_WORD)
			? -1UL << (off+trans)
			: 0;
		  unsigned long mask = ~(hmask | lmask);

		  abits_ptr_[ptr] &= ~mask;
		  bbits_ptr_[ptr] &= ~mask;
		  إذا (val_off >= off)
			abits_ptr_[ptr] |= mask & (val[val_ptr] >> (val_off-off));
		  وإلا
			abits_ptr_[ptr] |= mask & (val[val_ptr] << (off-val_off));

		  wid -= trans;
		  val_off += trans;
		  إذا (val_off == BIT2_PER_WORD) {
			val_off = 0;
			val_ptr += 1;
		  }
		  off += trans;
		  إذا (off == BITS_PER_WORD) {
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
منطقي vvp_vector4_t::set_vec(unsigned adr, ثابت vvp_vector4_t&that)
{
      أكد(adr+that.size_  <= size_);
      منطقي diff_flag = خطأ;

      إذا (size_ <= BITS_PER_WORD) {

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
	    إذا (hshift >= BITS_PER_WORD)
		  hmask = -1UL;
	    وإلا
		  hmask = (1UL << (adr+that.size_)) - 1;
	    unsigned long mask = hmask & ~lmask;

	    unsigned long tmp = (that.abits_val_<<adr)&mask;
	    إذا ((abits_val_&mask) != tmp) {
		  diff_flag = true;
		  abits_val_ = (abits_val_ & ~mask) | tmp;
	    }
	    tmp = (that.bbits_val_<<adr) & mask;
	    إذا ((bbits_val_&mask) != tmp) {
		  diff_flag = true;
		  bbits_val_ = (bbits_val_ & ~mask) | tmp;
	    }

      } وإلا إذا (that.size_ <= BITS_PER_WORD) {

	      /* This vector is more than a word, but that vector is
		 still small. Write into the destination, possibly
		 spanning two destination words, depending on whether
		 the source vector spans a word transition. */
	    unsigned long dptr = adr / BITS_PER_WORD;
	    unsigned long doff = adr % BITS_PER_WORD;

	    unsigned long lmask = (1UL << doff) - 1;
	    unsigned long hshift = doff+that.size_;
	    unsigned long hmask;
	    إذا (hshift >= BITS_PER_WORD)
		  hmask = -1UL;
	    وإلا
		  hmask = (1UL << hshift) - 1UL;

	    unsigned long mask = hmask & ~lmask;
	    unsigned long tmp;

	    tmp = (that.abits_val_ << doff) & mask;
	    إذا ((abits_ptr_[dptr] & mask) != tmp) {
		  diff_flag = true;
		  abits_ptr_[dptr] = (abits_ptr_[dptr] & ~mask) | tmp;
	    }
	    tmp = (that.bbits_val_ << doff) & mask;
	    إذا ((bbits_ptr_[dptr] & mask) != tmp) {
		  diff_flag = true;
		  bbits_ptr_[dptr] = (bbits_ptr_[dptr] & ~mask) | tmp;
	    }

	    إذا ((doff + that.size_) > BITS_PER_WORD) {
		  unsigned tail = doff + that.size_ - BITS_PER_WORD;
		  mask = (1UL << tail) - 1;

		  dptr += 1;
		  tmp = (that.abits_val_ >> (that.size_-tail)) & mask;
		  إذا ((abits_ptr_[dptr] & mask) != tmp) {
			diff_flag = true;
			abits_ptr_[dptr] = (abits_ptr_[dptr] & ~mask) | tmp;
		  }
		  tmp = (that.bbits_val_ >> (that.size_-tail)) & mask;
		  إذا ((bbits_ptr_[dptr] & mask) != tmp) {
			diff_flag = true;
			bbits_ptr_[dptr] = (bbits_ptr_[dptr] & ~mask) | tmp;
		  }
	    }

      } وإلا إذا (adr%BITS_PER_WORD == 0) {

	      /* In this case, both vectors are long, but the
		 destination is neatly aligned. That means all but the
		 last word can be simply copied with no masking. */

	    unsigned remain = that.size_;
	    unsigned sptr = 0;
	    unsigned dptr = adr / BITS_PER_WORD;
	    بينما (remain >= BITS_PER_WORD) {
		  إذا (abits_ptr_[dptr] != that.abits_ptr_[sptr]) {
			diff_flag = true;
			abits_ptr_[dptr] = that.abits_ptr_[sptr];
		  }
		  إذا (bbits_ptr_[dptr] != that.bbits_ptr_[sptr]) {
			diff_flag = true;
			bbits_ptr_[dptr] = that.bbits_ptr_[sptr];
		  }
		  dptr += 1;
		  sptr += 1;
		  remain -= BITS_PER_WORD;
	    }

	    إذا (remain > 0) {
		  unsigned long mask = (1UL << remain) - 1;
		  unsigned long tmp;

		  tmp = that.abits_ptr_[sptr] & mask;
		  إذا ((abits_ptr_[dptr] & mask) != tmp) {
			diff_flag = true;
			abits_ptr_[dptr] = (abits_ptr_[dptr] & ~mask) | tmp;
		  }
		  tmp = that.bbits_ptr_[sptr] & mask;
		  إذا ((bbits_ptr_[dptr] & mask) != tmp) {
			diff_flag = true;
			bbits_ptr_[dptr] = (bbits_ptr_[dptr] & ~mask) | tmp;
		  }
	    }

      } وإلا {

	      /* We know that there are two long vectors, and we know
		 that the destination is definitely NOT aligned. */

	    unsigned remain = that.size_;
	    unsigned sptr = 0;
	    unsigned dptr = adr / BITS_PER_WORD;
	    unsigned doff = adr % BITS_PER_WORD;
	    unsigned long lmask = (1UL << doff) - 1;
	    unsigned ndoff = BITS_PER_WORD - doff;
	    بينما (remain >= BITS_PER_WORD) {
		  unsigned long tmp;

		  tmp = (that.abits_ptr_[sptr] << doff) & ~lmask;
		  إذا ((abits_ptr_[dptr] & ~lmask) != tmp) {
			diff_flag = true;
			abits_ptr_[dptr] = (abits_ptr_[dptr] & lmask) | tmp;
		  }
		  tmp = (that.bbits_ptr_[sptr] << doff) & ~lmask;
		  إذا ((bbits_ptr_[dptr] & ~lmask) != tmp) {
			diff_flag = true;
			bbits_ptr_[dptr] = (bbits_ptr_[dptr] & lmask) | tmp;
		  }
		  dptr += 1;

		  tmp = (that.abits_ptr_[sptr] >> ndoff) & lmask;
		  إذا ((abits_ptr_[dptr] & lmask) != tmp) {
			diff_flag = true;
			abits_ptr_[dptr] = (abits_ptr_[dptr] & ~lmask) | tmp;
		  }
		  tmp = (that.bbits_ptr_[sptr] >> ndoff) & lmask;
		  إذا ((bbits_ptr_[dptr] & lmask) != tmp) {
			diff_flag = true;
			bbits_ptr_[dptr] = (bbits_ptr_[dptr] & ~lmask) | tmp;
		  }

		  remain -= BITS_PER_WORD;
		  sptr += 1;
	    }

	    إذا (remain > 0) {
		  unsigned long hshift = doff+remain;
		  unsigned long hmask;
		  إذا (hshift >= BITS_PER_WORD)
			hmask = -1UL;
		  وإلا
			hmask = (1UL << (doff+remain)) - 1;

		  unsigned long mask = hmask & ~lmask;
		  unsigned long tmp;

		  tmp = (that.abits_ptr_[sptr] << doff) & mask;
		  إذا ((abits_ptr_[dptr] & mask) != tmp) {
			diff_flag = true;
			abits_ptr_[dptr] = (abits_ptr_[dptr] & ~mask) | tmp;
		  }
		  tmp = (that.bbits_ptr_[sptr] << doff) & mask;
		  إذا ((bbits_ptr_[dptr] & mask) != tmp) {
			diff_flag = true;
			bbits_ptr_[dptr] = (bbits_ptr_[dptr] & ~mask) | tmp;
		  }

		  إذا ((doff + remain) > BITS_PER_WORD) {
			unsigned tail = doff + remain - BITS_PER_WORD;
			إذا (tail >= BITS_PER_WORD)
			      mask = -1UL;
			وإلا
			      mask = (1UL << tail) - 1;

			dptr += 1;

			tmp = (that.abits_ptr_[sptr] >> (remain-tail))&mask;
			إذا ((abits_ptr_[dptr] & mask) != tmp) {
			      diff_flag = true;
			      abits_ptr_[dptr] = (abits_ptr_[dptr] & ~mask) | tmp;
			}
			tmp = (that.bbits_ptr_[sptr] >> (remain-tail))&mask;
			إذا ((bbits_ptr_[dptr] & mask) != tmp) {
			      diff_flag = true;
			      bbits_ptr_[dptr] = (bbits_ptr_[dptr] & ~mask) | tmp;
			}
		  }
	    }
      }

      إرجاع diff_flag;
}

/*
 * Add that vector to this vector. Do it in the Verilog way, which
 * means if we detect any X or Z bits, change the entire results to
 * all X.
 *
 * Assume both vectors are the same size.
 */
فراغ vvp_vector4_t::add(ثابت vvp_vector4_t&that)
{
      أكد(size_ == that.size_);

      إذا (size_ < BITS_PER_WORD) {
	    unsigned long mask = ~(-1UL << size_);
	    إذا ((bbits_val_|that.bbits_val_) & mask) {
		  abits_val_ |= mask;
		  bbits_val_ |= mask;
		  إرجاع;
	    }

	    abits_val_ += that.abits_val_;
	    abits_val_ &= mask;
	    إرجاع;
      }

      إذا (size_ == BITS_PER_WORD) {
	    إذا (bbits_val_ | that.bbits_val_) {
		  abits_val_ = WORD_X_ABITS;
		  bbits_val_ = WORD_X_BBITS;
	    } وإلا {
		  abits_val_ += that.abits_val_;
	    }
	    إرجاع;
      }

      صحيح cnt = size_ / BITS_PER_WORD;
      unsigned long carry = 0;
      لكل (صحيح idx = 0 ; idx < cnt ; idx += 1) {
	    إذا (bbits_ptr_[idx] | that.bbits_ptr_[idx])
		  اذهب x_out;

	    abits_ptr_[idx] = add_with_carry(abits_ptr_[idx], that.abits_ptr_[idx], carry);
      }

      إذا (unsigned tail = size_ % BITS_PER_WORD) {
	    unsigned long mask = ~( -1UL << tail );
	    إذا ((bbits_ptr_[cnt] | that.bbits_ptr_[cnt])&mask)
		  اذهب x_out;

	    abits_ptr_[cnt] = add_with_carry(abits_ptr_[cnt], that.abits_ptr_[cnt], carry);
	    abits_ptr_[cnt] &= mask;
      }

      إرجاع;

 x_out:
      لكل (صحيح idx = 0 ; idx < cnt ; idx += 1) {
	    abits_ptr_[idx] = WORD_X_ABITS;
	    bbits_ptr_[idx] = WORD_X_BBITS;
      }
      إذا (unsigned tail = size_%BITS_PER_WORD) {
	    unsigned long mask = ~( -1UL << tail );
	    abits_ptr_[cnt] = WORD_X_ABITS&mask;
	    bbits_ptr_[cnt] = WORD_X_BBITS&mask;
      }
}

فراغ vvp_vector4_t::sub(ثابت vvp_vector4_t&that)
{
      أكد(size_ == that.size_);

      إذا (size_ < BITS_PER_WORD) {
	    unsigned long mask = ~(-1UL << size_);
	    إذا ((bbits_val_|that.bbits_val_) & mask) {
		  abits_val_ |= mask;
		  bbits_val_ |= mask;
		  إرجاع;
	    }

	    abits_val_ -= that.abits_val_;
	    abits_val_ &= mask;
	    إرجاع;
      }

      إذا (size_ == BITS_PER_WORD) {
	    إذا (bbits_val_ | that.bbits_val_) {
		  abits_val_ = WORD_X_ABITS;
		  bbits_val_ = WORD_X_BBITS;
	    } وإلا {
		  abits_val_ -= that.abits_val_;
	    }
	    إرجاع;
      }

      صحيح cnt = size_ / BITS_PER_WORD;
      unsigned long carry = 1;
      لكل (صحيح idx = 0 ; idx < cnt ; idx += 1) {
	    إذا (bbits_ptr_[idx] | that.bbits_ptr_[idx])
		  اذهب x_out;

	    abits_ptr_[idx] = add_with_carry(abits_ptr_[idx], ~that.abits_ptr_[idx], carry);
      }

      إذا (unsigned tail = size_ % BITS_PER_WORD) {
	    unsigned long mask = ~( -1UL << tail );
	    إذا ((bbits_ptr_[cnt] | that.bbits_ptr_[cnt])&mask)
		  اذهب x_out;

	    abits_ptr_[cnt] = add_with_carry(abits_ptr_[cnt], ~that.abits_ptr_[cnt], carry);
	    abits_ptr_[cnt] &= mask;
      }

      إرجاع;

 x_out:
      لكل (صحيح idx = 0 ; idx < cnt ; idx += 1) {
	    abits_ptr_[idx] = WORD_X_ABITS;
	    bbits_ptr_[idx] = WORD_X_BBITS;
      }
      إذا (unsigned tail = size_%BITS_PER_WORD) {
	    unsigned long mask = ~( -1UL << tail );
	    abits_ptr_[cnt] = WORD_X_ABITS&mask;
	    bbits_ptr_[cnt] = WORD_X_BBITS&mask;
      }

}

فراغ vvp_vector4_t::mov(unsigned dst, unsigned src, unsigned cnt)
{
      أكد(dst+cnt <= size_);
      أكد(src+cnt <= size_);

      إذا (size_ <= BITS_PER_WORD) {
	    unsigned long vmask = (1UL << cnt) - 1;
	    unsigned long tmp;

	    tmp = (abits_val_ >> src) & vmask;
	    abits_val_ &= ~ (vmask << dst);
	    abits_val_ |= tmp << dst;

	    tmp = (bbits_val_ >> src) & vmask;
	    bbits_val_ &= ~ (vmask << dst);
	    bbits_val_ |= tmp << dst;

      } وإلا {
	    unsigned sptr = src / BITS_PER_WORD;
	    unsigned dptr = dst / BITS_PER_WORD;
	    unsigned soff = src % BITS_PER_WORD;
	    unsigned doff = dst % BITS_PER_WORD;

	    بينما (cnt > 0) {
		  unsigned trans = cnt;
		  إذا ((soff+trans) > BITS_PER_WORD)
			trans = BITS_PER_WORD - soff;

		  إذا ((doff+trans) > BITS_PER_WORD)
			trans = BITS_PER_WORD - doff;

		  إذا (trans == BITS_PER_WORD) {
			  // Special case: the transfer count is
			  // exactly an entire word. For this to be
			  // true, it must also be true that the
			  // pointers are aligned. The work is easy,
			abits_ptr_[dptr] = abits_ptr_[sptr];
			bbits_ptr_[dptr] = bbits_ptr_[sptr];
			dptr += 1;
			sptr += 1;
			cnt -= BITS_PER_WORD;
			استمر;
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
		  إذا (soff >= BITS_PER_WORD) {
			soff = 0;
			sptr += 1;
		  }
		  doff += trans;
		  إذا (doff >= BITS_PER_WORD) {
			doff = 0;
			dptr += 1;
		  }
	    }
      }
}

فراغ vvp_vector4_t::mul(ثابت vvp_vector4_t&that)
{
      أكد(size_ == that.size_);

      إذا (size_ < BITS_PER_WORD) {
	    unsigned long mask = ~(-1UL << size_);
	    إذا ((bbits_val_|that.bbits_val_) & mask) {
		  abits_val_ |= mask;
		  bbits_val_ |= mask;
		  إرجاع;
	    }

	    abits_val_ *= that.abits_val_;
	    abits_val_ &= mask;
	    إرجاع;
      }

      إذا (size_ == BITS_PER_WORD) {
	    إذا (bbits_val_ || that.bbits_val_) {
		  abits_val_ = WORD_X_ABITS;
		  bbits_val_ = WORD_X_BBITS;
	    } وإلا {
		  abits_val_ *= that.abits_val_;
	    }
	    إرجاع;
      }

      ثابت صحيح cnt = (size_+BITS_PER_WORD-1) / BITS_PER_WORD;

      unsigned long mask;
      إذا (unsigned tail = size_%BITS_PER_WORD) {
	    mask = ~( -1UL << tail );
      } وإلا {
	    mask = ~0UL;
      }

	// Check for any XZ values ahead of time in a first pass. If
	// we find any, then force the entire result to be X and be
	// done.
      لكل (صحيح idx = 0 ; idx < cnt ; idx += 1) {
	    unsigned long lval = bbits_ptr_[idx];
	    unsigned long rval = that.bbits_ptr_[idx];
	    إذا (idx == (cnt-1)) {
		  lval &= mask;
		  rval &= mask;
	    }
	    إذا (lval || rval) {
		  لكل (صحيح xdx = 0 ; xdx < cnt-1 ; xdx += 1) {
			abits_ptr_[xdx] = WORD_X_ABITS;
			bbits_ptr_[xdx] = WORD_X_BBITS;
		  }
		  abits_ptr_[cnt-1] = WORD_X_ABITS & mask;
		  bbits_ptr_[cnt-1] = WORD_X_BBITS & mask;
		  إرجاع;
	    }
      }

	// Calculate the result into a res array. We need to keep is
	// separate from the "this" array because we are making
	// multiple passes.
      unsigned long*res = جديد unsigned long[cnt];
      لكل (صحيح idx = 0 ; idx < cnt ; idx += 1)
	    res[idx] = 0;

      لكل (صحيح mul_a = 0 ; mul_a < cnt ; mul_a += 1) {
	    unsigned long lval = abits_ptr_[mul_a];
	    إذا (mul_a == (cnt-1))
		  lval &= mask;

	    لكل (صحيح mul_b = 0 ; mul_b < (cnt-mul_a) ; mul_b += 1) {
		  unsigned long rval = that.abits_ptr_[mul_b];
		  إذا (mul_b == (cnt-1))
			rval &= mask;

		  unsigned long sum;
		  unsigned long tmp = multiply_with_carry(lval, rval, sum);
		  صحيح base = mul_a + mul_b;
		  unsigned long carry = 0;
		  res[base] = add_with_carry(res[base], tmp, carry);
		  لكل (صحيح add_idx = base+1 ; add_idx < cnt ; add_idx += 1) {
			res[add_idx] = add_with_carry(res[add_idx], sum, carry);
			sum = 0;
		  }
	    }
      }

	// Replace the "this" value with the calculated result. We
	// know a-priori that the bbits are zero and unchanged.
      res[cnt-1] &= mask;
      لكل (صحيح idx = 0 ; idx < cnt ; idx += 1)
	    abits_ptr_[idx] = res[idx];

      delete[]res;
      إرجاع;


}

منطقي vvp_vector4_t::eeq(ثابت vvp_vector4_t&that) ثابت
{
      إذا (size_ != that.size_)
	    إرجاع خطأ;

      إذا (size_ < BITS_PER_WORD) {
	    unsigned long mask = (1UL << size_) - 1;
	    إرجاع (abits_val_&mask) == (that.abits_val_&mask)
		  && (bbits_val_&mask) == (that.bbits_val_&mask);
      }

      إذا (size_ == BITS_PER_WORD) {
	    إرجاع (abits_val_ == that.abits_val_)
		  && (bbits_val_ == that.bbits_val_);
      }

      unsigned كلمات = size_ / BITS_PER_WORD;
      لكل (unsigned idx = 0 ;  idx < كلمات ;  idx += 1) {
	    إذا (abits_ptr_[idx] != that.abits_ptr_[idx])
		  إرجاع خطأ;
	    إذا (bbits_ptr_[idx] != that.bbits_ptr_[idx])
		  إرجاع خطأ;
      }

      unsigned long mask = size_%BITS_PER_WORD;
      إذا (mask > 0) {
	    mask = (1UL << mask) - 1;
	    إرجاع (abits_ptr_[كلمات]&mask) == (that.abits_ptr_[كلمات]&mask)
		  && (bbits_ptr_[كلمات]&mask) == (that.bbits_ptr_[كلمات]&mask);
      }

      إرجاع true;
}

منطقي vvp_vector4_t::eq_xz(ثابت vvp_vector4_t&that) ثابت
{
      إذا (size_ != that.size_)
	    إرجاع خطأ;

      إذا (size_ < BITS_PER_WORD) {
	    unsigned long mask = (1UL << size_) - 1;
	    إرجاع ((abits_val_|bbits_val_)&mask) == ((that.abits_val_|that.bbits_val_)&mask)
		  && (bbits_val_&mask) == (that.bbits_val_&mask);
      }

      إذا (size_ == BITS_PER_WORD) {
	    إرجاع ((abits_val_|bbits_val_) == (that.abits_val_|that.bbits_val_))
		  && (bbits_val_ == that.bbits_val_);
      }

      unsigned كلمات = size_ / BITS_PER_WORD;
      لكل (unsigned idx = 0 ;  idx < كلمات ;  idx += 1) {
	    إذا ((abits_ptr_[idx]|bbits_ptr_[idx]) != (that.abits_ptr_[idx]|that.bbits_ptr_[idx]))
		  إرجاع خطأ;
	    إذا (bbits_ptr_[idx] != that.bbits_ptr_[idx])
		  إرجاع خطأ;
      }

      unsigned long mask = size_%BITS_PER_WORD;
      إذا (mask > 0) {
	    mask = (1UL << mask) - 1;
	    إرجاع ((abits_ptr_[كلمات]|bbits_ptr_[كلمات])&mask) == ((that.abits_ptr_[كلمات]|that.bbits_ptr_[كلمات])&mask)
		  && (bbits_ptr_[كلمات]&mask) == (that.bbits_ptr_[كلمات]&mask);
      }

      إرجاع true;
}

منطقي vvp_vector4_t::has_xz() ثابت
{
      إذا (size_ < BITS_PER_WORD) {
	    unsigned long mask = -1UL >> (BITS_PER_WORD - size_);
	    إرجاع bbits_val_&mask;
      }

      إذا (size_ == BITS_PER_WORD) {
	    إرجاع bbits_val_;
      }

      unsigned كلمات = size_ / BITS_PER_WORD;
      لكل (unsigned idx = 0 ; idx < كلمات ; idx += 1) {
	    إذا (bbits_ptr_[idx])
		  إرجاع true;
      }

      unsigned long mask = size_%BITS_PER_WORD;
      إذا (mask > 0) {
	    mask = -1UL >> (BITS_PER_WORD - mask);
	    إرجاع bbits_ptr_[كلمات]&mask;
      }

      إرجاع خطأ;
}

فراغ vvp_vector4_t::change_z2x()
{
	// This method relies on the fact that both BIT4_X and BIT4_Z
	// have the bbit set in the vector4 encoding, and also that
	// the BIT4_X has abit set in the vector4 encoding. By simply
	// or-ing the bbit into the abit, BIT4_X and BIT4_Z both
	// become BIT4_X.

      إذا (size_ <= BITS_PER_WORD) {
	    abits_val_ |= bbits_val_;
      } وإلا {
	    unsigned كلمات = (size_+BITS_PER_WORD-1) / BITS_PER_WORD;
	    لكل (unsigned idx = 0 ;  idx < كلمات ;  idx += 1)
		  abits_ptr_[idx] |= bbits_ptr_[idx];
      }
}

فراغ vvp_vector4_t::fill_bits(vvp_bit4_t bit)
{
	/* note: this relies on the bit encoding for the vvp_bit4_t. */
      ساكن ثابت unsigned long init_atable[4] = {
	    WORD_0_ABITS,
	    WORD_1_ABITS,
	    WORD_Z_ABITS,
	    WORD_X_ABITS };
      ساكن ثابت unsigned long init_btable[4] = {
	    WORD_0_BBITS,
	    WORD_1_BBITS,
	    WORD_Z_BBITS,
	    WORD_X_BBITS };

      إذا (size_ <= BITS_PER_WORD) {
	    abits_val_ = init_atable[bit];
	    bbits_val_ = init_btable[bit];
      } وإلا {
	    unsigned كلمات = (size_ + BITS_PER_WORD - 1) / BITS_PER_WORD;
	    لكل (unsigned idx = 0; idx < كلمات; idx += 1) {
		  abits_ptr_[idx] = init_atable[bit];
		  bbits_ptr_[idx] = init_btable[bit];
	    }
      }
}

حرف* vvp_vector4_t::as_string(حرف*buf, size_t buf_len) ثابت
{
      حرف*res = buf;
      *buf++ = 'C';
      *buf++ = '4';
      *buf++ = '<';
      buf_len -= 3;

      لكل (unsigned idx = 0 ;  idx < size() && buf_len >= 2 ;  idx += 1) {
	    بدل (value(size()-idx-1)) {
		حالة BIT4_0:
		  *buf++ = '0';
		  اكسر;
		حالة BIT4_1:
		  *buf++ = '1';
		  اكسر;
		حالة BIT4_X:
		  *buf++ = 'x';
		  اكسر;
		حالة BIT4_Z:
		  *buf++ = 'z';
	    }
	    buf_len -= 1;
      }

      *buf++ = '>';
      *buf++ = 0;
      إرجاع res;
}

فراغ vvp_vector4_t::invert()
{
      إذا (size_ <= BITS_PER_WORD) {
	    unsigned long mask = (size_<BITS_PER_WORD)? (1UL<<size_)-1UL : -1UL;
	    abits_val_ = mask & ~abits_val_;
	    abits_val_ |= bbits_val_;
      } وإلا {
	    unsigned remaining = size_;
	    unsigned idx = 0;
	    بينما (remaining >= BITS_PER_WORD) {
		  abits_ptr_[idx] = ~abits_ptr_[idx];
		  abits_ptr_[idx] |= bbits_ptr_[idx];
		  idx += 1;
		  remaining -= BITS_PER_WORD;
	    }
	    إذا (remaining > 0) {
		  unsigned long mask = (1UL<<remaining) - 1UL;
		  abits_ptr_[idx] = mask & ~abits_ptr_[idx];
		  abits_ptr_[idx] |= bbits_ptr_[idx];
	    }
      }
}

#define BIT_MASK(n) ((n) ? ((~0UL) >> (BITS_PER_WORD - (n))) : ~0UL)

vvp_bit4_t vvp_vector4_t::reduce_or() ثابت
{
      unsigned long mask = BIT_MASK(size_ % BITS_PER_WORD);
      vvp_bit4_t res = BIT4_0;

      إذا (size_ <= BITS_PER_WORD) {
	    إذا ((abits_val_ & ~bbits_val_ & mask) != 0UL)
		  إرجاع BIT4_1;
	    إذا ((bbits_val_ & mask) != 0UL)
		  إرجاع BIT4_X;
      } وإلا {
	    unsigned كلمات = (size_ + BITS_PER_WORD - 1) / BITS_PER_WORD;
	    unsigned idx;
	    لكل (idx = 0; idx < كلمات - 1; idx += 1) {
		  إذا ((abits_ptr_[idx] & ~bbits_ptr_[idx]) != 0UL)
			إرجاع BIT4_1;
		  إذا (bbits_ptr_[idx] != 0UL)
			res = BIT4_X;
	    }
	    إذا ((abits_ptr_[idx] & ~bbits_ptr_[idx] & mask) != 0UL)
		  إرجاع BIT4_1;
	    إذا ((bbits_ptr_[idx] & mask) != 0UL)
		  res = BIT4_X;
      }

      إرجاع res;
}

vvp_bit4_t vvp_vector4_t::reduce_and() ثابت
{
      unsigned long mask = BIT_MASK(size_ % BITS_PER_WORD);
      vvp_bit4_t res = BIT4_1;

      إذا (size_ <= BITS_PER_WORD) {
	    إذا ((abits_val_ | bbits_val_ | ~mask) != ~0UL)
		  إرجاع BIT4_0;
	    إذا ((bbits_val_ & mask) != 0UL)
		  إرجاع BIT4_X;
	} وإلا {
	    unsigned كلمات = (size_ + BITS_PER_WORD - 1) / BITS_PER_WORD;
	    unsigned idx;
	    لكل (idx = 0; idx < كلمات - 1; idx += 1) {
		  إذا ((abits_ptr_[idx] | bbits_ptr_[idx]) != ~0UL)
			إرجاع BIT4_0;
		  إذا (bbits_ptr_[idx] != 0UL)
			res = BIT4_X;
	    }
	    إذا ((abits_ptr_[idx] | bbits_ptr_[idx] | ~mask) != ~0UL)
		  إرجاع BIT4_0;
	    إذا ((bbits_ptr_[idx] & mask) != 0UL)
		  res = BIT4_X;
      }

      إرجاع res;
}

ساكن unsigned long parity(unsigned long val)
{
#إذا defined(__GNUC__)
      // The compiler builtin can use target-specific CPU instructions.
      إرجاع __builtin_parityl(val);
#وإلا
#إذا ULONG_MAX > 0xffffffffUL
	val ^= val >> 32;
#endif
	val ^= val >> 16;
	val ^= val >> 8;
	val ^= val >> 4;

      إرجاع (0x6996 >> (val & 0xf)) & 1;
#endif
}

vvp_bit4_t vvp_vector4_t::reduce_xor() ثابت
{
      unsigned long mask = BIT_MASK(size_ % BITS_PER_WORD);

      إذا (size_ <= BITS_PER_WORD) {
	    إذا ((bbits_val_ & mask) != 0UL)
		  إرجاع BIT4_X;
	    إرجاع parity(abits_val_ & mask) ? BIT4_1 : BIT4_0;
      } وإلا {
	    unsigned كلمات = (size_ + BITS_PER_WORD - 1) / BITS_PER_WORD;
	    unsigned long val_a = 0UL;
	    unsigned idx;
	    لكل (idx = 0; idx < كلمات - 1; idx += 1) {
		  إذا (bbits_ptr_[idx] != 0UL)
			إرجاع BIT4_X;
		  val_a ^= abits_ptr_[idx];
	    }
	    إذا ((bbits_ptr_[idx] & mask) != 0UL)
		  إرجاع BIT4_X;
	    val_a ^= abits_ptr_[idx] & mask;
	    إرجاع parity(val_a) ? BIT4_1 : BIT4_0;
      }
}

vvp_vector4_t& vvp_vector4_t::operator &= (ثابت vvp_vector4_t&that)
{
	// The truth table is:
	//     00 01 11 10
	//  00 00 00 00 00
	//  01 00 01 11 11
	//  11 00 11 11 11
	//  10 00 11 11 11
      إذا (size_ <= BITS_PER_WORD) {
	    unsigned long tmp1 = abits_val_ | bbits_val_;
	    unsigned long tmp2 = that.abits_val_ | that.bbits_val_;
	    abits_val_ = tmp1 & tmp2;
	    bbits_val_ = (tmp1 & that.bbits_val_) | (tmp2 & bbits_val_);
      } وإلا {
	    unsigned كلمات = (size_ + BITS_PER_WORD - 1) / BITS_PER_WORD;
	    لكل (unsigned idx = 0; idx < كلمات ; idx += 1) {
		  unsigned long tmp1 = abits_ptr_[idx] | bbits_ptr_[idx];
		  unsigned long tmp2 = that.abits_ptr_[idx] |
		                       that.bbits_ptr_[idx];
		  abits_ptr_[idx] = tmp1 & tmp2;
		  bbits_ptr_[idx] = (tmp1 & that.bbits_ptr_[idx]) |
		                    (tmp2 & bbits_ptr_[idx]);
	    }
      }

      إرجاع *هذا;
}

vvp_vector4_t& vvp_vector4_t::operator |= (ثابت vvp_vector4_t&that)
{
	// The truth table is:
	//     00 01 11 10
	//  00 00 01 11 11
	//  01 01 01 01 01
	//  11 11 01 11 11
	//  10 11 01 11 11
      إذا (size_ <= BITS_PER_WORD) {
	    unsigned long tmp = abits_val_ | bbits_val_ |
	                        that.abits_val_ | that.bbits_val_;
	    bbits_val_ = ((~abits_val_ | bbits_val_) & that.bbits_val_) |
	                 ((~that.abits_val_ | that.bbits_val_) & bbits_val_);
	    abits_val_ = tmp;

      } وإلا {
	    unsigned كلمات = (size_ + BITS_PER_WORD - 1) / BITS_PER_WORD;
	    لكل (unsigned idx = 0; idx < كلمات ; idx += 1) {
		  unsigned long tmp = abits_ptr_[idx] | bbits_ptr_[idx] |
	                        that.abits_ptr_[idx] | that.bbits_ptr_[idx];
		  bbits_ptr_[idx] = ((~abits_ptr_[idx] | bbits_ptr_[idx]) &
		                     that.bbits_ptr_[idx]) |
		                    ((~that.abits_ptr_[idx] |
		                      that.bbits_ptr_[idx]) & bbits_ptr_[idx]);
		  abits_ptr_[idx] = tmp;
	    }
      }

      إرجاع *هذا;
}

vvp_vector4_t& vvp_vector4_t::operator ^= (ثابت vvp_vector4_t&that)
{
	// The truth table is:
	//     00 01 11 10
	//  00 00 01 11 11
	//  01 01 00 11 11
	//  11 11 11 11 11
	//  10 11 11 11 11
      إذا (size_ <= BITS_PER_WORD) {
	    unsigned long bval = bbits_val_ | that.bbits_val_;
	    bbits_val_ = bval;
	    abits_val_ = (abits_val_ ^ that.abits_val_) | bval;
      } وإلا {
	    unsigned كلمات = (size_ + BITS_PER_WORD - 1) / BITS_PER_WORD;
	    لكل (unsigned idx = 0; idx < كلمات ; idx += 1) {
		  unsigned long bval = bbits_ptr_[idx] | that.bbits_ptr_[idx];
		  bbits_ptr_[idx] = bval;
		  abits_ptr_[idx] = (abits_ptr_[idx] ^ that.abits_ptr_[idx]) | bval;
	    }
      }

      إرجاع *هذا;
}

/*
* Add an integer to the vvp_vector4_t in place, bit by bit so that
* there is no size limitations.
*/
vvp_vector4_t& vvp_vector4_t::operator += (int64_t that)
{
      vvp_bit4_t carry = BIT4_0;
      unsigned idx;

      إذا (has_xz()) {
	    vvp_vector4_t xxx (size(), BIT4_X);
	    *هذا = xxx;
	    إرجاع *هذا;
      }

      لكل (idx = 0 ; idx < size() ; idx += 1) {
	    إذا (that == 0 && carry==BIT4_0)
		  اكسر;

	    vvp_bit4_t that_bit = (that&1)? BIT4_1 : BIT4_0;
	    that >>= 1;

	    إذا (that_bit==BIT4_0 && carry==BIT4_0)
		  استمر;

	    vvp_bit4_t bit = value(idx);
	    bit = add_with_carry(bit, that_bit, carry);

	    set_bit(idx, bit);
      }

      إرجاع *هذا;
}

ostream& operator<< (ostream&out, ثابت vvp_vector4_t&that)
{
      out << that.size() << "'b";
      لكل (unsigned idx = 0 ;  idx < that.size() ;  idx += 1)
	    out << that.value(that.size()-idx-1);
      إرجاع out;
}

template <صنف INT>منطقي vector4_to_قيمة(ثابت vvp_vector4_t&vec, INT&val,
					  منطقي is_signed, منطقي is_arithmetic)
{
      INT res = 0;
      INT msk = 1;
      منطقي rc_flag = true;

      unsigned size = vec.size();
      إذا (size > 8*حجم(val)) size = 8*حجم(val);
      لكل (unsigned idx = 0 ;  idx < size ;  idx += 1) {
	    بدل (vec.value(idx)) {
		حالة BIT4_0:
		  اكسر;
		حالة BIT4_1:
		  res |= msk;
		  اكسر;
		افتراضي:
		  إذا (is_arithmetic)
			إرجاع خطأ;
		  وإلا
			rc_flag = خطأ;
	    }

	    msk <<= 1;
      }

      إذا (is_signed && vec.value(vec.size()-1) == BIT4_1) {
	    إذا (vec.size() < 8*حجم(val))
		  res |= static_cast<INT>(-1ULL << vec.size());
      }

      val = res;
      إرجاع rc_flag;
}

template منطقي vector4_to_قيمة(ثابت vvp_vector4_t&vec, int8_t&val,
			       منطقي is_signed, منطقي is_arithmetic);
template منطقي vector4_to_قيمة(ثابت vvp_vector4_t&vec, int16_t&val,
			       منطقي is_signed, منطقي is_arithmetic);
template منطقي vector4_to_قيمة(ثابت vvp_vector4_t&vec, int32_t&val,
			       منطقي is_signed, منطقي is_arithmetic);
template منطقي vector4_to_قيمة(ثابت vvp_vector4_t&vec, int64_t&val,
			       منطقي is_signed, منطقي is_arithmetic);
template منطقي vector4_to_قيمة(ثابت vvp_vector4_t&vec, uint8_t&val,
			       منطقي is_signed, منطقي is_arithmetic);
template منطقي vector4_to_قيمة(ثابت vvp_vector4_t&vec, uint16_t&val,
			       منطقي is_signed, منطقي is_arithmetic);
template منطقي vector4_to_قيمة(ثابت vvp_vector4_t&vec, uint32_t&val,
			       منطقي is_signed, منطقي is_arithmetic);
template منطقي vector4_to_قيمة(ثابت vvp_vector4_t&vec, uint64_t&val,
			       منطقي is_signed, منطقي is_arithmetic);

template <صنف T> منطقي vector4_to_قيمة(ثابت vvp_vector4_t&vec,
                                         منطقي&overflow_flag, T&val)
{
      T res = 0;
      T msk = 1;

      overflow_flag = خطأ;
      unsigned size = vec.size();
      لكل (unsigned idx = 0 ;  idx < size ;  idx += 1) {
	    بدل (vec.value(idx)) {
		حالة BIT4_0:
		  اكسر;
		حالة BIT4_1:
		  إذا (msk == 0)
			overflow_flag = true;
		  وإلا
			res |= msk;
		  اكسر;
		افتراضي:
		  إرجاع خطأ;
	    }

	    msk <<= static_cast<T>(1);
      }

      val = res;
      إرجاع true;
}

template منطقي vector4_to_قيمة(ثابت vvp_vector4_t&vec, منطقي&overflow_flag,
                               unsigned long&val);
#ifndef UL_AND_TIME64_SAME
template منطقي vector4_to_قيمة(ثابت vvp_vector4_t&vec, منطقي&overflow_flag,
                               vvp_time64_t&val);
#endif

منطقي vector4_to_قيمة(ثابت vvp_vector4_t&vec, مزدوج&val, منطقي signed_flag)
{

      إذا (vec.size() == 0) {
	    val = 0.0;
	    إرجاع true;
      }

      منطقي flag = true;

      إذا (vec.value(vec.size()-1) != BIT4_1) {
	    signed_flag = خطأ;
      }

      مزدوج res = 0.0;
      إذا (signed_flag) {
	    vvp_bit4_t carry = BIT4_1;
	    لكل (unsigned idx = 0 ;  idx < vec.size() ;  idx += 1) {
		  vvp_bit4_t a = ~vec.value(idx);
		  vvp_bit4_t x = add_with_carry(a, BIT4_0, carry);
		  بدل (x) {
		      حالة BIT4_0:
			اكسر;
		      حالة BIT4_1:
			res += pow(2.0, (صحيح)idx);
			اكسر;
		      افتراضي:
			flag = خطأ;
		  }
	    }
	    res *= -1.0;
      } وإلا {
	    لكل (unsigned idx = 0 ;  idx < vec.size() ;  idx += 1) {
		  بدل (vec.value(idx)) {
		      حالة BIT4_0:
			اكسر;
		      حالة BIT4_1:
			res += pow(2.0, (صحيح)idx);
			اكسر;
		      افتراضي:
			flag = خطأ;
		  }
	    }
      }
      val = res;
      إرجاع flag;
}

منطقي vector2_to_قيمة(ثابت vvp_vector2_t&a, int32_t&val, منطقي is_signed)
{
      val = 0;
      unsigned idx;
      uint32_t mask;
      لكل (idx = 0, mask = 1 ; idx < a.size() && idx < 32 ; idx += 1, mask <<= 1) {
	    إذا (a.value(idx)) val |= mask;
      }

      إذا (is_signed && a.size() < 32 && a.value(a.size()-1)) {
	    mask = -1;
	    mask <<= a.size();
	    val |= mask;
      }

      إرجاع a.size() <= 32;
}

vvp_vector4array_t::vvp_vector4array_t(unsigned width__, unsigned كلمات_2)
: width_(width__), كلمات_3(كلمات_2)
{
}

vvp_vector4array_t::~vvp_vector4array_t()
{
}

فراغ vvp_vector4array_t::set_word_(v4cell*cell, ثابت vvp_vector4_t&that)
{
      أكد(that.size_ == width_);

      إذا (width_ <= vvp_vector4_t::BITS_PER_WORD) {
	    cell->abits_val_ = that.abits_val_;
	    cell->bbits_val_ = that.bbits_val_;
	    إرجاع;
      }

      unsigned cnt = (width_ + vvp_vector4_t::BITS_PER_WORD-1)/vvp_vector4_t::BITS_PER_WORD;

      إذا (cell->abits_ptr_ == 0) {
	    cell->abits_ptr_ = جديد unsigned long[2*cnt];
	    cell->bbits_ptr_ = cell->abits_ptr_ + cnt;
      }

      لكل (unsigned idx = 0 ; idx < cnt ; idx += 1)
	    cell->abits_ptr_[idx] = that.abits_ptr_[idx];
      لكل (unsigned idx = 0 ; idx < cnt ; idx += 1)
	    cell->bbits_ptr_[idx] = that.bbits_ptr_[idx];
}

vvp_vector4_t vvp_vector4array_t::get_word_(v4cell*cell) ثابت
{
      إذا (width_ <= vvp_vector4_t::BITS_PER_WORD) {
	    vvp_vector4_t res;
	    res.size_ = width_;
	    res.abits_val_ = cell->abits_val_;
	    res.bbits_val_ = cell->bbits_val_;
	    إرجاع res;
      }

      vvp_vector4_t res (width_, BIT4_X);
      إذا (cell->abits_ptr_ == 0)
	    إرجاع res;

      unsigned cnt = (width_ + vvp_vector4_t::BITS_PER_WORD-1)/vvp_vector4_t::BITS_PER_WORD;

      لكل (unsigned idx = 0 ; idx < cnt ; idx += 1)
	    res.abits_ptr_[idx] = cell->abits_ptr_[idx];
      لكل (unsigned idx = 0 ; idx < cnt ; idx += 1)
	    res.bbits_ptr_[idx] = cell->bbits_ptr_[idx];

      إرجاع res;
}

vvp_vector4array_sa::vvp_vector4array_sa(unsigned width__, unsigned كلمات_2)
: vvp_vector4array_t(width__, كلمات_2)
{
      array_ = جديد v4cell[كلمات_3];

      إذا (width_ <= vvp_vector4_t::BITS_PER_WORD) {
	    لكل (unsigned idx = 0 ; idx < كلمات_3 ; idx += 1) {
		  array_[idx].abits_val_ = vvp_vector4_t::WORD_X_ABITS;
		  array_[idx].bbits_val_ = vvp_vector4_t::WORD_X_BBITS;
	    }
      } وإلا {
	    لكل (unsigned idx = 0 ; idx < كلمات_3 ; idx += 1) {
		  array_[idx].abits_ptr_ = 0;
		  array_[idx].bbits_ptr_ = 0;
	    }
      }
}

vvp_vector4array_sa::~vvp_vector4array_sa()
{
      إذا (array_) {
	    إذا (width_ > vvp_vector4_t::BITS_PER_WORD) {
		  لكل (unsigned idx = 0 ; idx < كلمات_3 ; idx += 1)
			إذا (array_[idx].abits_ptr_)
			      delete[]array_[idx].abits_ptr_;
	    }
	    delete[]array_;
      }
}

فراغ vvp_vector4array_sa::set_word(unsigned index, ثابت vvp_vector4_t&that)
{
      أكد(index < كلمات_3);

      v4cell*cell = &array_[index];

      set_word_(cell, that);
}

vvp_vector4_t vvp_vector4array_sa::get_word(unsigned index) ثابت
{
      إذا (index >= كلمات_3)
	    إرجاع vvp_vector4_t(width_, BIT4_X);

      أكد(index < كلمات_3);

      v4cell*cell = &array_[index];

      إرجاع get_word_(cell);
}

vvp_vector4array_aa::vvp_vector4array_aa(unsigned width__, unsigned كلمات_2)
: vvp_vector4array_t(width__, كلمات_2)
{
      context_idx_ = vpip_add_item_to_context(هذا, vpip_peek_context_scope());
}

vvp_vector4array_aa::~vvp_vector4array_aa()
{
}

فراغ vvp_vector4array_aa::alloc_instance(vvp_context_t context)
{
      v4cell*array = جديد v4cell[كلمات_3];

      إذا (width_ <= vvp_vector4_t::BITS_PER_WORD) {
	    لكل (unsigned idx = 0 ; idx < كلمات_3 ; idx += 1) {
		  array[idx].abits_val_ = vvp_vector4_t::WORD_X_ABITS;
		  array[idx].bbits_val_ = vvp_vector4_t::WORD_X_BBITS;
	    }
      } وإلا {
	    لكل (unsigned idx = 0 ; idx < كلمات_3 ; idx += 1) {
		  array[idx].abits_ptr_ = 0;
		  array[idx].bbits_ptr_ = 0;
	    }
      }

      vvp_set_context_item(context, context_idx_, array);
}

فراغ vvp_vector4array_aa::reset_instance(vvp_context_t context)
{
      v4cell*cell = static_cast<v4cell*>
            (vvp_get_context_item(context, context_idx_));

      إذا (width_ <= vvp_vector4_t::BITS_PER_WORD) {
	    لكل (unsigned idx = 0 ; idx < كلمات_3 ; idx += 1) {
		  cell->abits_val_ = vvp_vector4_t::WORD_X_ABITS;
		  cell->bbits_val_ = vvp_vector4_t::WORD_X_BBITS;
                  cell++;
	    }
      } وإلا {
            unsigned cnt = (width_ + vvp_vector4_t::BITS_PER_WORD-1)/vvp_vector4_t::BITS_PER_WORD;
	    لكل (unsigned idx = 0 ; idx < كلمات_3 ; idx += 1) {
		  إذا (cell->abits_ptr_) {
                        لكل (unsigned n = 0 ; n < cnt ; n += 1) {
			      cell->abits_ptr_[n] = vvp_vector4_t::WORD_X_ABITS;
			      cell->bbits_ptr_[n] = vvp_vector4_t::WORD_X_BBITS;
                        }
                  }
                  cell++;
	    }
      }
}

#ifdef CHECK_WITH_VALGRIND
فراغ vvp_vector4array_aa::free_instance(vvp_context_t context)
{
      v4cell*cell = static_cast<v4cell*>
            (vvp_get_context_item(context, context_idx_));
      delete [] cell;
}
#endif

فراغ vvp_vector4array_aa::set_word(unsigned index, ثابت vvp_vector4_t&that)
{
      أكد(index < كلمات_3);

      v4cell*cell = static_cast<v4cell*>
            (vthread_get_wt_context_item(context_idx_)) + index;

      set_word_(cell, that);
}

vvp_vector4_t vvp_vector4array_aa::get_word(unsigned index) ثابت
{
      إذا (index >= كلمات_3)
	    إرجاع vvp_vector4_t(width_, BIT4_X);

      أكد(index < كلمات_3);

      v4cell*cell = static_cast<v4cell*>
            (vthread_get_rd_context_item(context_idx_)) + index;

      إرجاع get_word_(cell);
}

vvp_vector2_t::vvp_vector2_t()
{
      vec_ = 0;
      wid_ = 0;
}

vvp_vector2_t::vvp_vector2_t(uint64_t v, unsigned wid)
{
      wid_ = wid;
      ثابت unsigned bits_per_word = 8 * حجم(vec_[0]);
      ثابت unsigned كلمات = (wid_ + bits_per_word-1) / bits_per_word;

      vec_ = جديد unsigned long[كلمات];
      لكل (unsigned idx = 0 ; idx < كلمات ; idx += 1) {
	    ثابت unsigned shift = idx * bits_per_word;
	    vec_[idx] = shift < 64 ? v >> shift : 0;
      }
}

vvp_vector2_t::vvp_vector2_t(vvp_vector2_t::fill_t fill, unsigned wid)
{
      wid_ = wid;
      ثابت unsigned bits_per_word = 8 * حجم(vec_[0]);
      ثابت unsigned كلمات = (wid_ + bits_per_word-1) / bits_per_word;

      vec_ = جديد unsigned long[كلمات];
      لكل (unsigned idx = 0 ;  idx < كلمات ;  idx += 1)
	    vec_[idx] = fill? -1 : 0;
}

vvp_vector2_t::vvp_vector2_t(ثابت vvp_vector2_t&that, unsigned base, unsigned wid)
{
      wid_ = wid;
      ثابت unsigned كلمات = (wid_ + BITS_PER_WORD-1) / BITS_PER_WORD;

      vec_ = جديد unsigned long[كلمات];

      لكل (unsigned idx = 0 ; idx < wid ; idx += 1) {
	    صحيح bit = that.value(base+idx);
	    إذا (bit == 0)
		  استمر;
	    unsigned word = idx / BITS_PER_WORD;
	    unsigned long mask = 1UL << (idx % BITS_PER_WORD);
	    vec_[word] |= mask;
      }
}

فراغ vvp_vector2_t::copy_from_that_(ثابت vvp_vector4_t&that)
{
      wid_ = that.size();
      ثابت unsigned كلمات = (that.size() + BITS_PER_WORD-1) / BITS_PER_WORD;

      إذا (كلمات == 0) {
	    vec_ = 0;
	    wid_ = 0;
	    إرجاع;
      }

	// Use the subarray method with the xz_to_0 flag set so that
	// we get values even when there are xz bits.
      vec_ = that.subarray(0, wid_, true);
}

فراغ vvp_vector2_t::copy_from_that_(ثابت vvp_vector2_t&that)
{
      wid_ = that.wid_;
      ثابت unsigned كلمات = (wid_ + BITS_PER_WORD-1) / BITS_PER_WORD;

      إذا (كلمات == 0) {
	    vec_ = 0;
	    wid_ = 0;
	    إرجاع;
      }

      vec_ = جديد unsigned long[كلمات];
      لكل (unsigned idx = 0 ;  idx < كلمات ;  idx += 1)
	    vec_[idx] = that.vec_[idx];
}

vvp_vector2_t::vvp_vector2_t(ثابت vvp_vector2_t&that, unsigned newsize)
{
      wid_ = newsize;
      إذا (newsize == 0) {
	    vec_ = 0;
	    إرجاع;
      }

      ثابت unsigned كلمات = (wid_ + BITS_PER_WORD-1) / BITS_PER_WORD;
      ثابت unsigned twords = (that.wid_ + BITS_PER_WORD-1) / BITS_PER_WORD;

      vec_ = جديد unsigned long[كلمات];
      لكل (unsigned idx = 0 ;  idx < كلمات ;  idx += 1) {
	    إذا (idx < twords)
		  vec_[idx] = that.vec_[idx];
	    وإلا
		  vec_[idx] = 0;
      }
}

vvp_vector2_t& vvp_vector2_t::operator= (ثابت vvp_vector2_t&that)
{
      إذا (هذا == &that)
	    إرجاع *هذا;

      delete[] vec_;
      vec_ = 0;

      copy_from_that_(that);
      إرجاع *هذا;
}

vvp_vector2_t& vvp_vector2_t::operator= (ثابت vvp_vector4_t&that)
{
      delete[]vec_;
      vec_ = 0;
      copy_from_that_(that);
      إرجاع *هذا;
}

vvp_vector2_t& vvp_vector2_t::operator <<= (unsigned صحيح shift)
{
      إذا (wid_ == 0)
	    إرجاع *هذا;

      ثابت unsigned كلمات = (wid_ + BITS_PER_WORD-1) / BITS_PER_WORD;

	// Number of words to shift
      ثابت unsigned wshift = shift / BITS_PER_WORD;
	// bits to shift within each word.
      ثابت unsigned long oshift = shift % BITS_PER_WORD;

	// If shifting the entire value away, then return zeros.
      إذا (wshift >= كلمات) {
	    لكل (unsigned idx = 0 ;  idx < كلمات ;  idx += 1)
		  vec_[idx] = 0;

	    إرجاع *هذا;
      }

	// Do the word shift first.
      إذا (wshift > 0) {
	    لكل (unsigned idx = 0 ;  idx < كلمات-wshift ;  idx += 1) {
		  unsigned sel = كلمات - idx - 1;
		  vec_[sel] = vec_[sel-wshift];
	    }

	    لكل (unsigned idx = 0 ;  idx < wshift ;  idx += 1)
		  vec_[idx] = 0;
      }

	// Do the fine shift.
      إذا (oshift != 0) {
	    unsigned long pad = 0;
	    لكل (unsigned idx = 0 ;  idx < كلمات ;  idx += 1) {
		  unsigned long next_pad = vec_[idx] >> (BITS_PER_WORD-oshift);
		  vec_[idx] = (vec_[idx] << oshift) | pad;
		  pad = next_pad;
	    }

	      // Cleanup the tail bits.
	    unsigned long mask = -1UL >> (BITS_PER_WORD - wid_%BITS_PER_WORD);
	    vec_[كلمات-1] &= mask;
      }

      إرجاع *هذا;
}

vvp_vector2_t& vvp_vector2_t::operator >>= (unsigned shift)
{
      إذا (wid_ == 0)
	    إرجاع *هذا;

      ثابت unsigned كلمات = (wid_ + BITS_PER_WORD-1) / BITS_PER_WORD;

	// Number of words to shift
      ثابت unsigned wshift = shift / BITS_PER_WORD;
	// bits to shift within each word.
      ثابت unsigned long oshift = shift % BITS_PER_WORD;

	// If shifting the entire value away, then return zeros.
      إذا (wshift >= كلمات) {
	    لكل (unsigned idx = 0 ;  idx < كلمات ;  idx += 1)
		  vec_[idx] = 0;

	    إرجاع *هذا;
      }

      إذا (wshift > 0) {
	    لكل (unsigned idx = 0 ;  idx < كلمات-wshift ;  idx += 1)
		  vec_[idx] = vec_[idx+wshift];

	    لكل (unsigned idx = كلمات-wshift ;  idx < كلمات ;  idx += 1)
		  vec_[idx] = 0;
      }

      إذا (oshift > 0) {
	    unsigned long pad = 0;
	    لكل (unsigned idx = كلمات ;  idx > 0 ;  idx -= 1) {
		  unsigned long new_pad = vec_[idx-1] <<(BITS_PER_WORD-oshift);
		  vec_[idx-1] = pad | (vec_[idx-1] >> oshift);
		  pad = new_pad;
	    }

	      // Cleanup the tail bits.

	    unsigned use_كلمات = كلمات;
	      // Mask_shift is the number of high bits of the top word
	      // that are to be masked off. We start with the number
	      // of bits that are not included even in the original,
	      // then we include the bits of the shift, that are to be
	      // masked to zero.
	    unsigned long mask_shift = BITS_PER_WORD - wid_%BITS_PER_WORD;
	    mask_shift %= BITS_PER_WORD;
	    mask_shift += oshift;
	    بينما (mask_shift >= BITS_PER_WORD) {
		  vec_[use_كلمات-1] = 0;
		  use_كلمات -= 1;
		  mask_shift -= BITS_PER_WORD;
	    }
	    إذا (mask_shift > 0) {
		  أكد(use_كلمات > 0);
		  unsigned long mask = -1UL >> mask_shift;
		  vec_[use_كلمات-1] &= mask;
	    }
      }

      إرجاع *هذا;
}

ساكن unsigned long add_carry(unsigned long a, unsigned long b,
			       unsigned long&carry)
{
      unsigned long out = carry;
      carry = 0;

      إذا ((ULONG_MAX - out) < a)
	    carry += 1;
      out += a;

      إذا ((ULONG_MAX - out) < b)
	    carry += 1;
      out += b;

      إرجاع out;
}

vvp_vector2_t& vvp_vector2_t::operator += (ثابت vvp_vector2_t&that)
{
      أكد(wid_ == that.wid_);
      إذا (wid_ == 0)
	    إرجاع *هذا;

      ثابت unsigned كلمات = (wid_ + BITS_PER_WORD-1) / BITS_PER_WORD;

      unsigned long carry = 0;
      لكل (unsigned idx = 0 ;  idx < كلمات ;  idx += 1) {
	    vec_[idx] = add_carry(vec_[idx], that.vec_[idx], carry);
      }


	// Cleanup the tail bits.
      unsigned long mask = -1UL >> (BITS_PER_WORD - wid_%BITS_PER_WORD);
      vec_[كلمات-1] &= mask;

      إرجاع *هذا;
}

vvp_vector2_t& vvp_vector2_t::operator -= (ثابت vvp_vector2_t&that)
{
      أكد(wid_ == that.wid_);
      إذا (wid_ == 0)
	    إرجاع *هذا;

      ثابت unsigned كلمات = (wid_ + BITS_PER_WORD-1) / BITS_PER_WORD;

      unsigned long carry = 1;
      لكل (unsigned idx = 0 ;  idx < كلمات ;  idx += 1) {
	    vec_[idx] = add_carry(vec_[idx], ~that.vec_[idx], carry);
      }

      إرجاع *هذا;
}

فراغ vvp_vector2_t::trim()
{
      بينما (value(wid_-1) == 0 && wid_ > 1) wid_ -= 1;
}

/* This is a special trim that is used on numbers we know represent a
 * negative signed value (they came from a negative real value). */
فراغ vvp_vector2_t::trim_neg()
{
      إذا (value(wid_-1) == 1 && wid_ > 32) {
	    بينما (value(wid_-2) == 1 && wid_ > 32) wid_ -= 1;
      }
}

صحيح vvp_vector2_t::value(unsigned idx) ثابت
{
      إذا (idx >= wid_)
	    إرجاع 0;

      ثابت unsigned bits_per_word = 8 * حجم(vec_[0]);
      unsigned addr = idx/bits_per_word;
      unsigned mask = idx%bits_per_word;

      إذا (vec_[addr] & (1UL<<mask))
	    إرجاع 1;
      وإلا
	    إرجاع 0;
}

فراغ vvp_vector2_t::set_bit(unsigned idx, صحيح bit)
{
      أكد(idx < wid_);

      ثابت unsigned bits_per_word = 8 * حجم(vec_[0]);
      unsigned addr = idx/bits_per_word;
      unsigned long mask = idx%bits_per_word;

      إذا (bit)
	    vec_[addr] |= 1UL << mask;
      وإلا
	    vec_[addr] &= ~(1UL << mask);
}

فراغ vvp_vector2_t::set_vec(unsigned adr, ثابت vvp_vector2_t&that)
{
      أكد((adr + that.wid_) <= wid_);

      لكل (unsigned idx = 0 ; idx < that.wid_ ; idx += 1)
	    set_bit(adr+idx, that.value(idx));
}

منطقي vvp_vector2_t::is_NaN() ثابت
{
      إرجاع wid_ == 0;
}

منطقي vvp_vector2_t::is_zero() ثابت
{
      ثابت unsigned كلمات = (wid_ + BITS_PER_WORD-1) / BITS_PER_WORD;

      لكل (unsigned idx = 0; idx < كلمات; idx += 1) {
	    إذا (vec_[idx] == 0) استمر;
	    إرجاع خطأ;
      }

      إرجاع true;
}

/*
 * Basic idea from "Introduction to Programming using SML" by
 * Michael R. Hansen and Hans Rischel page 261 and "Seminumerical
 * Algorithms, Third Edition" by Donald E. Knuth section 4.6.3.
 */
vvp_vector2_t pow(ثابت vvp_vector2_t&x, vvp_vector2_t&y)
{
        /* If we have a zero exponent just return 1. */
      إذا (y == vvp_vector2_t(0L, 1)) {
	    إرجاع vvp_vector2_t(1L, x.size());
      }

        /* Is the value odd? */
      إذا (y.value(0) == 1) {
	    y.set_bit(0, 0);  // A quick subtract by 1.
	    vvp_vector2_t res = x * pow(x, y);
	    إرجاع res;
      }

      y >>= 1;  // A fast divide by two. We know the LSB is zero.
      vvp_vector2_t z = pow(x, y);
      vvp_vector2_t res = z * z;
      إرجاع res;
}

ساكن فراغ multiply_long(unsigned long a, unsigned long b,
			  unsigned long&أدنى, unsigned long&أعلى)
{
      أكد(حجم(unsigned long) %2 == 0);

      ثابت unsigned long word_mask = (1UL << 4UL*حجم(a)) - 1UL;
      unsigned long tmpa;
      unsigned long tmpb;
      unsigned long res[4];

      tmpa = a & word_mask;
      tmpb = b & word_mask;
      res[0] = tmpa * tmpb;
      res[1] = res[0] >> 4UL*حجم(unsigned long);
      res[0] &= word_mask;

      tmpa = (a >> 4UL*حجم(unsigned long)) & word_mask;
      res[1] += tmpa * tmpb;
      res[2] = res[1] >> 4UL*حجم(unsigned long);
      res[1] &= word_mask;

      tmpa = a & word_mask;
      tmpb = (b >> 4UL*حجم(unsigned long)) & word_mask;
      res[1] += tmpa * tmpb;
      res[2] += res[1] >> 4UL*حجم(unsigned long);
      res[3]  = res[2] >> 4UL*حجم(unsigned long);
      res[1] &= word_mask;
      res[2] &= word_mask;

      tmpa = (a >> 4UL*حجم(unsigned long)) & word_mask;
      res[2] += tmpa * tmpb;
      res[3] += res[2] >> 4UL*حجم(unsigned long);
      res[2] &= word_mask;

      أعلى = (res[3] << 4UL*حجم(unsigned long)) | res[2];
      أدنى  = (res[1] << 4UL*حجم(unsigned long)) | res[0];
}

vvp_vector2_t operator * (ثابت vvp_vector2_t&a, ثابت vvp_vector2_t&b)
{
      ثابت unsigned bits_per_word = 8 * حجم(a.vec_[0]);

	// The compiler ensures that the two operands are of equal size.
      أكد(a.size() == b.size());
      vvp_vector2_t r (0, a.size());

      unsigned كلمات = (r.wid_ + bits_per_word - 1) / bits_per_word;

      لكل (unsigned bdx = 0 ;  bdx < كلمات ;  bdx += 1) {
	    unsigned long tmpb = b.vec_[bdx];
	    إذا (tmpb == 0)
		  استمر;

	    لكل (unsigned adx = 0 ;  adx < كلمات ;  adx += 1) {
		  unsigned long tmpa = a.vec_[adx];
		  إذا (tmpa == 0)
			استمر;

		  unsigned long أدنى, hig;
		  multiply_long(tmpa, tmpb, أدنى, hig);

		  unsigned long carry = 0;
		  لكل (unsigned sdx = 0
			     ; (adx+bdx+sdx) < كلمات
			     ;  sdx += 1) {

			r.vec_[adx+bdx+sdx] = add_carry(r.vec_[adx+bdx+sdx],
							أدنى, carry);
			أدنى = hig;
			hig = 0;
		  }
	    }
      }

      إرجاع r;
}

ساكن فراغ div_mod (vvp_vector2_t dividend, vvp_vector2_t قاسم,
		     vvp_vector2_t&quotient, vvp_vector2_t&remainder)
{

      quotient = vvp_vector2_t(0, dividend.size());

      إذا (قاسم == quotient) {
	    cerr << "ERROR: division by zero, exiting." << endl;
	    exit(255);
      }

      إذا (dividend < قاسم) {
	    remainder = dividend;
	    إرجاع;
      }

      vvp_vector2_t mask (1, dividend.size());

	// Make the dividend 1 bit larger to prevent overflow of
	// divtmp in startup.
      dividend = vvp_vector2_t(dividend, dividend.size()+1);
      vvp_vector2_t divtmp (قاسم, dividend.size());

      بينما (divtmp < dividend) {
	    divtmp <<= 1;
	    mask <<= 1;
      }

      بينما (dividend >= قاسم) {
	    إذا (divtmp <= dividend) {
		  dividend -= divtmp;
		  quotient += mask;
	    }

	    divtmp >>= 1;
	    mask >>= 1;
      }

      remainder = vvp_vector2_t(dividend, mask.size());
}

vvp_vector2_t operator - (ثابت vvp_vector2_t&that)
{
      vvp_vector2_t neg(that);
      إذا (neg.wid_ == 0) إرجاع neg;

      ثابت unsigned كلمات = (neg.wid_ + neg.BITS_PER_WORD-1) /
                                         neg.BITS_PER_WORD;
      لكل (unsigned idx = 0 ;  idx < كلمات ;  idx += 1) {
	    neg.vec_[idx] = ~neg.vec_[idx];
      }
      neg += vvp_vector2_t(1, neg.wid_);

      إرجاع neg;
}

vvp_vector2_t operator / (ثابت vvp_vector2_t&dividend,
			  ثابت vvp_vector2_t&قاسم)
{
      vvp_vector2_t quot, rem;
      div_mod(dividend, قاسم, quot, rem);
      إرجاع quot;
}

vvp_vector2_t operator % (ثابت vvp_vector2_t&dividend,
			  ثابت vvp_vector2_t&قاسم)
{
      vvp_vector2_t quot, rem;
      div_mod(dividend, قاسم, quot, rem);
      إرجاع rem;
}

منطقي operator > (ثابت vvp_vector2_t&a, ثابت vvp_vector2_t&b)
{
      ثابت unsigned awords = (a.wid_ + vvp_vector2_t::BITS_PER_WORD-1) / vvp_vector2_t::BITS_PER_WORD;
      ثابت unsigned bwords = (b.wid_ + vvp_vector2_t::BITS_PER_WORD-1) / vvp_vector2_t::BITS_PER_WORD;

      ثابت unsigned كلمات = awords > bwords? awords : bwords;

      لكل (unsigned idx = كلمات ;  idx > 0 ;  idx -= 1) {
	    unsigned long aw = (idx <= awords)? a.vec_[idx-1] : 0;
	    unsigned long bw = (idx <= bwords)? b.vec_[idx-1] : 0;

	    إذا (aw > bw)
		  إرجاع true;
	    إذا (aw < bw)
		  إرجاع خطأ;
      }

	// If the above loop finishes, then the vectors are equal.
      إرجاع خطأ;
}

منطقي operator >= (ثابت vvp_vector2_t&a, ثابت vvp_vector2_t&b)
{
      ثابت unsigned awords = (a.wid_ + vvp_vector2_t::BITS_PER_WORD-1) / vvp_vector2_t::BITS_PER_WORD;
      ثابت unsigned bwords = (b.wid_ + vvp_vector2_t::BITS_PER_WORD-1) / vvp_vector2_t::BITS_PER_WORD;

      ثابت unsigned كلمات = awords > bwords? awords : bwords;

      لكل (unsigned idx = كلمات ;  idx > 0 ;  idx -= 1) {
	    unsigned long aw = (idx <= awords)? a.vec_[idx-1] : 0;
	    unsigned long bw = (idx <= bwords)? b.vec_[idx-1] : 0;

	    إذا (aw > bw)
		  إرجاع true;
	    إذا (aw < bw)
		  إرجاع خطأ;
      }

	// If the above loop finishes, then the vectors are equal.
      إرجاع true;
}

منطقي operator < (ثابت vvp_vector2_t&a, ثابت vvp_vector2_t&b)
{
      ثابت unsigned awords = (a.wid_ + vvp_vector2_t::BITS_PER_WORD-1) / vvp_vector2_t::BITS_PER_WORD;
      ثابت unsigned bwords = (b.wid_ + vvp_vector2_t::BITS_PER_WORD-1) / vvp_vector2_t::BITS_PER_WORD;

      unsigned كلمات = awords;
      إذا (bwords > كلمات)
	    كلمات = bwords;

      لكل (unsigned idx = كلمات ;  idx > 0 ;  idx -= 1) {
	    unsigned long aw = (idx <= awords)? a.vec_[idx-1] : 0;
	    unsigned long bw = (idx <= bwords)? b.vec_[idx-1] : 0;

	    إذا (aw < bw)
		  إرجاع true;
	    إذا (aw > bw)
		  إرجاع خطأ;
      }

	// If the above loop finishes, then the vectors are equal.
      إرجاع خطأ;
}

منطقي operator <= (ثابت vvp_vector2_t&a, ثابت vvp_vector2_t&b)
{
	// XXXX For now, only support equal width vectors.
      أكد(a.wid_ == b.wid_);

      ثابت unsigned awords = (a.wid_ + vvp_vector2_t::BITS_PER_WORD-1) / vvp_vector2_t::BITS_PER_WORD;

      لكل (unsigned idx = awords ;  idx > 0 ;  idx -= 1) {
	    إذا (a.vec_[idx-1] < b.vec_[idx-1])
		  إرجاع true;
	    إذا (a.vec_[idx-1] > b.vec_[idx-1])
		  إرجاع خطأ;
      }

	// If the above loop finishes, then the vectors are equal.
      إرجاع true;
}

منطقي operator == (ثابت vvp_vector2_t&a, ثابت vvp_vector2_t&b)
{
      ثابت unsigned awords = (a.wid_ + vvp_vector2_t::BITS_PER_WORD-1) / vvp_vector2_t::BITS_PER_WORD;
      ثابت unsigned bwords = (b.wid_ + vvp_vector2_t::BITS_PER_WORD-1) / vvp_vector2_t::BITS_PER_WORD;

      ثابت unsigned كلمات = awords > bwords? awords : bwords;

      لكل (unsigned idx = كلمات ;  idx > 0 ;  idx -= 1) {
	    unsigned long aw = (idx <= awords)? a.vec_[idx-1] : 0;
	    unsigned long bw = (idx <= bwords)? b.vec_[idx-1] : 0;

	    إذا (aw > bw)
		  إرجاع خطأ;
	    إذا (aw < bw)
		  إرجاع خطأ;
      }

	// If the above loop finishes, then the vectors are equal.
      إرجاع true;
}


vvp_vector4_t vector2_to_vector4(ثابت vvp_vector2_t&that, unsigned wid)
{
      vvp_vector4_t res (wid);

      لكل (unsigned idx = 0 ;  idx < res.size() ;  idx += 1) {
	    vvp_bit4_t bit = BIT4_0;

	    إذا (that.value(idx))
		  bit = BIT4_1;

	    res.set_bit(idx, bit);
      }

      إرجاع res;
}

ساكن منطقي c4string_header_test(ثابت حرف*str)
{
      إذا ((str[0] != 'C') && (str[0] != 'c'))
	    إرجاع خطأ;
      إذا ((str[1] != '4') || (str[2] != '<'))
	    إرجاع خطأ;
      إرجاع true;
}

منطقي c4string_test(ثابت حرف*str)
{
      إذا (!c4string_header_test(str))
	    إرجاع خطأ;
      size_t قيمة_size = strspn(str+3, "01xz");
      إذا (str[3+قيمة_size] != '>')
	    إرجاع خطأ;
      إذا (str[3+قيمة_size+1] != 0)
	    إرجاع خطأ;

      إرجاع true;
}

vvp_vector4_t c4string_to_vector4(ثابت حرف*str)
{
      أكد(c4string_header_test(str));

      str += 3;
      ثابت حرف*tp = str + strspn(str,"01xz");
      أكد(tp[0] == '>');

      vvp_vector4_t tmp (tp-str);

      لكل (unsigned idx = 0 ;  idx < tmp.size() ;  idx += 1) {
	    vvp_bit4_t bit;
	    بدل (str[idx]) {
		حالة '0':
		  bit = BIT4_0;
		  اكسر;
		حالة '1':
		  bit = BIT4_1;
		  اكسر;
		حالة 'x':
		  bit = BIT4_X;
		  اكسر;
		حالة 'z':
		  bit = BIT4_Z;
		  اكسر;
		افتراضي:
		  fprintf(stderr, "Unsupported bit value %c(%d).\n", str[idx],
		          str[idx]);
		  أكد(0);
		  bit = BIT4_0;
		  اكسر;
	    }
	    tmp.set_bit(tmp.size()-idx-1, bit);
      }

      إرجاع tmp;
}

ostream& operator<< (ostream&out, ثابت vvp_vector2_t&that)
{
      إذا (that.is_NaN()) {
	    out << "NaN";

      } وإلا {
	    out << vector2_to_vector4(that, that.size());
      }
      إرجاع out;
}

vvp_vector8_t::vvp_vector8_t(ثابت vvp_vector8_t&that)
{
      size_ = that.size_;
      إذا (size_ <= حجم(val_)) {
	    memcpy(val_, that.val_, حجم val_);
      } وإلا {
	    ptr_ = جديد unsigned حرف[size_];
	    memcpy(ptr_, that.ptr_, size_);
      }
}

vvp_vector8_t::vvp_vector8_t(ثابت vvp_vector4_t&that,
			     unsigned str0, unsigned str1)
: size_(that.size())
{
      إذا (size_ == 0)
	    إرجاع;

      إذا (size_ <= حجم(val_)) {
	    ptr_ = 0; // Prefill all val_ bytes
	    لكل (unsigned idx = 0 ; idx < size_ ; idx += 1)
		  val_[idx] = vvp_scalar_t(that.value(idx),str0, str1).raw();
      } وإلا {
	    ptr_ = جديد unsigned حرف[size_];
	    لكل (unsigned idx = 0 ;  idx < size_ ;  idx += 1)
		  ptr_[idx] = vvp_scalar_t(that.value(idx), str0, str1).raw();
      }
}

vvp_vector8_t::vvp_vector8_t(ثابت vvp_vector2_t&that,
			     unsigned str0, unsigned str1)
: size_(that.size())
{
      إذا (size_ == 0)
	    إرجاع;

      إذا (size_ <= حجم(val_)) {
	    ptr_ = 0;
	    لكل (unsigned idx = 0 ; idx < size_ ; idx += 1)
		  val_[idx] = vvp_scalar_t(that.value(idx)? BIT4_1:BIT4_0, str0, str1).raw();
      } وإلا {
	    ptr_ = جديد unsigned حرف[size_];
	    لكل (unsigned idx = 0 ; idx < size_ ; idx += 1)
		  ptr_[idx] = vvp_scalar_t(that.value(idx)? BIT4_1:BIT4_0, str0, str1).raw();
      }
}

ثابت vvp_vector8_t vvp_vector8_t::nil;

vvp_vector8_t& vvp_vector8_t::operator= (ثابت vvp_vector8_t&that)
{
	// Assign to self.
      إذا (هذا == &that)
	    إرجاع *هذا;

      إذا (size_ != that.size_) {
	    إذا (size_ > حجم(val_))
		  delete[]ptr_;
	    size_ = 0;
      }

      إذا (that.size_ == 0) {
	    أكد(size_ == 0);
	    إرجاع *هذا;
      }

      إذا (that.size_ <= حجم(val_)) {
	    size_ = that.size_;
	    memcpy(val_, that.val_, حجم(val_));
	    إرجاع *هذا;
      }

      إذا (size_ == 0) {
	    size_ = that.size_;
	    ptr_ = جديد unsigned حرف[size_];
      }

      memcpy(ptr_, that.ptr_, size_);

      إرجاع *هذا;
}

vvp_vector8_t vvp_vector8_t::subvalue(unsigned base, unsigned wid) ثابت
{
      vvp_vector8_t tmp (wid);

      unsigned حرف*tmp_ptr = tmp.size_ <= حجم(val_) ? tmp.val_ : tmp.ptr_;
      ثابت unsigned حرف*use_ptr = size_ <= حجم(val_) ? val_ : ptr_;

      unsigned idx = 0;
      بينما ((idx < wid) && (base+idx < size_)) {
	    tmp_ptr[idx] = use_ptr[base+idx];
	    idx += 1;
      }

      إرجاع tmp;
}

فراغ vvp_vector8_t::set_vec(unsigned base, ثابت vvp_vector8_t&that)
{
      أكد((base+that.size()) <= size());
      لكل (unsigned idx = 0 ; idx < that.size() ; idx += 1)
	    set_bit(base+idx, that.value(idx));
}

vvp_vector8_t part_expand(ثابت vvp_vector8_t&that, unsigned wid, unsigned off)
{
      أكد(off < wid);
      vvp_vector8_t tmp (wid);

      unsigned حرف* tmp_ptr = tmp.size_<= حجم(tmp.val_) ?
                               tmp.val_ : tmp.ptr_;
      ثابت unsigned حرف* that_ptr = that.size_<= حجم(that.val_) ?
                                      that.val_ : that.ptr_;

      unsigned idx = off;

      بينما (idx < wid && that.size_ > (idx-off)) {
	    tmp_ptr[idx] = that_ptr[idx-off];
	    idx += 1;
      }

      إرجاع tmp;
}

ساكن منطقي c8string_header_test(ثابت حرف*str)
{
      إذا ((str[0] != 'C') && (str[0] != 'c'))
	    إرجاع خطأ;
      إذا ((str[1] != '8') || (str[2] != '<'))
	    إرجاع خطأ;
      إرجاع true;
}

منطقي c8string_test(ثابت حرف*str)
{
      إذا (!c8string_header_test(str))
	    إرجاع خطأ;

      ثابت حرف*cp = str+3;
      لكل (;; cp += 1) {
	    إذا (cp[0] == '>' && cp[1] == 0) إرجاع true;
	    إذا (cp[0] >= '0' && cp[0] <= '9') استمر;
	    إذا (cp[0] == 'x') استمر;
	    إذا (cp[0] == 'z') استمر;
	    إرجاع خطأ;
      }
      إرجاع خطأ;
}
/*
 * The format of a C8<> string is:
 *   C8<aaabbbccc...>
 * where aaa... is a 3 character bit descriptor.
 */
vvp_vector8_t c8string_to_vector8(ثابت حرف*str)
{
      أكد(c8string_header_test(str));

      size_t vsize = strlen(str)-4;
      أكد(vsize%3 == 0);
      vsize /= 3;
      vvp_vector8_t tmp (vsize);

      لكل (size_t idx = 0 ; idx < vsize ; idx += 1) {
	    ثابت حرف*cp = str+3+3*idx;
	    vvp_bit4_t bit = BIT4_X;
	    unsigned dr0 = cp[0]-'0';
	    unsigned dr1 = cp[1]-'0';
	    بدل (cp[2]) {
		حالة '0':
		  bit = BIT4_0;
		  اكسر;
		حالة '1':
		  bit = BIT4_1;
		  اكسر;
		حالة 'x':
		  bit = BIT4_X;
		  اكسر;
		حالة 'z':
		  bit = BIT4_Z;
		  اكسر;
	    }
	    tmp.set_bit(vsize-idx-1, vvp_scalar_t(bit, dr0, dr1));
      }

      إرجاع tmp;
}

ostream& operator<<(ostream&out, ثابت vvp_vector8_t&that)
{
      out << "C8<";
      لكل (unsigned idx = 0 ;  idx < that.size() ; idx += 1)
	    out << that.value(that.size()-idx-1);

      out << ">";
      إرجاع out;
}

vvp_net_fun_t::vvp_net_fun_t()
{
      تعداد_functors += 1;
}

vvp_net_fun_t::~vvp_net_fun_t()
{
}

فراغ vvp_net_fun_t::recv_vec4(vvp_net_ptr_t, ثابت vvp_vector4_t&,
                              vvp_context_t)
{
      fprintf(stderr, "internal error: %s: recv_vec4 not implemented\n",
	      typeid(*هذا).name());
      أكد(0);
}

فراغ vvp_net_fun_t::recv_vec4_pv_(vvp_net_ptr_t p, ثابت vvp_vector4_t&bit,
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

      أكد(base + bit.size() <= vwid);

      vvp_vector4_t tmp(vwid, BIT4_Z);
      tmp.set_vec(base, bit);
      recv_vec4(p, tmp, 0);
}

فراغ vvp_net_fun_t::recv_vec4_pv(vvp_net_ptr_t, ثابت vvp_vector4_t&bit,
                                 unsigned base, unsigned vwid, vvp_context_t)
{
      cerr << "internal error: " << typeid(*هذا).name() << ": "
	   << "recv_vec4_pv(" << bit << ", " << base
	   << ", " << vwid << ") not implemented" << endl;
      أكد(0);
}

فراغ vvp_net_fun_t::recv_vec8(vvp_net_ptr_t port, ثابت vvp_vector8_t&bit)
{
      recv_vec4(port, reduce4(bit), 0);
}

فراغ vvp_net_fun_t::recv_vec8_pv_(vvp_net_ptr_t p, ثابت vvp_vector8_t&bit,
				  unsigned base, unsigned vwid)
{
	// This is the strength-aware version of recv_vec4_pv_.

      أكد(base + bit.size() <= vwid);

      vvp_vector8_t tmp(vwid);
      tmp.set_vec(base, bit);
      recv_vec8(p, tmp);
}

فراغ vvp_net_fun_t::recv_vec8_pv(vvp_net_ptr_t port, ثابت vvp_vector8_t&bit,
				 unsigned base, unsigned vwid)
{
      recv_vec4_pv(port, reduce4(bit), base, vwid, 0);
}

فراغ vvp_net_fun_t::recv_real(vvp_net_ptr_t, مزدوج bit, vvp_context_t)
{
      fprintf(stderr, "internal error: %s: recv_real(%f) not implemented\n",
	      typeid(*هذا).name(), bit);
      أكد(0);
}

فراغ vvp_net_fun_t::recv_string(vvp_net_ptr_t, ثابت std::string&bit, vvp_context_t)
{
      fprintf(stderr, "internal error: %s: recv_string(%s) not implemented\n",
	      typeid(*هذا).name(), bit.c_str());
      أكد(0);
}

فراغ vvp_net_fun_t::recv_object(vvp_net_ptr_t, vvp_object_t, vvp_context_t)
{
      fprintf(stderr, "internal error: %s: recv_object(...) not implemented\n",
	      typeid(*هذا).name());
      أكد(0);
}

فراغ vvp_net_fun_t::force_flag(منطقي)
{
}

/* **** vvp_fun_drive methods **** */

vvp_fun_drive::vvp_fun_drive(unsigned str0, unsigned str1)
{
      أكد(str0 < 8);
      أكد(str1 < 8);

      drive0_ = str0;
      drive1_ = str1;
}

vvp_fun_drive::~vvp_fun_drive()
{
}

فراغ vvp_fun_drive::recv_vec4(vvp_net_ptr_t port, ثابت vvp_vector4_t&bit,
                              vvp_context_t)
{
      أكد(port.port() == 0);
      port.ptr()->send_vec8(vvp_vector8_t(bit, drive0_, drive1_));
}

فراغ vvp_fun_drive::recv_vec4_pv(vvp_net_ptr_t ptr, ثابت vvp_vector4_t&bit,
				 unsigned base, unsigned vwid, vvp_context_t ctx)
{
      recv_vec4_pv_(ptr, bit, base, vwid, ctx);
}

/* **** vvp_wide_fun_* methods **** */

vvp_wide_fun_core::vvp_wide_fun_core(vvp_net_t*net, unsigned nports)
{
      ptr_ = net;
      nports_ = nports;
      port_قيم = 0;
      port_rvalues_ = 0;
}

vvp_wide_fun_core::~vvp_wide_fun_core()
{
      delete[] port_قيم;
      delete[] port_rvalues_;
}

فراغ vvp_wide_fun_core::propagate_vec4(ثابت vvp_vector4_t&bit,
				       vvp_time64_t delay)
{
      إذا (delay)
	    schedule_propagate_vector(ptr_, delay, bit);
      وإلا
	    ptr_->send_vec4(bit, 0);
}

فراغ vvp_wide_fun_core::propagate_real(مزدوج bit,
				       vvp_time64_t delay)
{
      إذا (delay) {
	      // schedule_assign_vector(ptr_->out, bit, delay);
	    أكد(0); // Need a real-value version of assign_vector.
      } وإلا {
	    ptr_->send_real(bit, 0);
      }
}


unsigned vvp_wide_fun_core::port_تعداد() ثابت
{
      إرجاع nports_;
}

vvp_vector4_t& vvp_wide_fun_core::value(unsigned idx)
{
      أكد(idx < nports_);
      إذا (port_قيم == 0)
	    port_قيم = جديد vvp_vector4_t [nports_];
      إرجاع port_قيم[idx];
}

مزدوج vvp_wide_fun_core::قيمة_r(unsigned idx)
{
      أكد(idx < nports_);
      إرجاع port_rvalues_? port_rvalues_[idx] : 0.0;
}

فراغ vvp_wide_fun_core::recv_real_from_inputs(unsigned)
{
      أكد(0);
}

فراغ vvp_wide_fun_core::dispatch_vec4_from_إدخال(unsigned port,
						  ثابت vvp_vector4_t&bit)
{
      أكد(port < nports_);
      إذا (port_قيم == 0) port_قيم = جديد vvp_vector4_t [nports_];
      port_قيم[port] = bit;
      recv_vec4_from_inputs(port);
}

فراغ vvp_wide_fun_core::dispatch_real_from_إدخال(unsigned port,
						  مزدوج bit)
{
      أكد(port < nports_);
      إذا (port_rvalues_ == 0) port_rvalues_ = جديد مزدوج[nports_];
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

فراغ vvp_wide_fun_t::recv_vec4(vvp_net_ptr_t port, ثابت vvp_vector4_t&bit,
                               vvp_context_t)
{
      unsigned pidx = port_base_ + port.port();
      core_->dispatch_vec4_from_إدخال(pidx, bit);
}

فراغ vvp_wide_fun_t::recv_vec4_pv(vvp_net_ptr_t ptr, ثابت vvp_vector4_t&bit,
				  unsigned base, unsigned vwid, vvp_context_t ctx)
{
      recv_vec4_pv_(ptr, bit, base, vwid, ctx);
}

فراغ vvp_wide_fun_t::recv_real(vvp_net_ptr_t port, مزدوج bit,
                               vvp_context_t)
{
      unsigned pidx = port_base_ + port.port();
      core_->dispatch_real_from_إدخال(pidx, bit);
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

unsigned vvp_scalar_t::strength0() ثابت
{
      إرجاع STREN0(قيمة_2);
}

unsigned vvp_scalar_t::strength1() ثابت
{
      إرجاع STREN1(قيمة_2);
}

ostream& operator <<(ostream&out, vvp_scalar_t a)
{
      out << a.strength0() << a.strength1();
      بدل (a.value()) {
	  حالة BIT4_0:
	    out << "0";
	    اكسر;
	  حالة BIT4_1:
	    out << "1";
	    اكسر;
	  حالة BIT4_X:
	    out << "X";
	    اكسر;
	  حالة BIT4_Z:
	    out << "Z";
	    اكسر;
      }
      إرجاع out;
}

/*
 * This function is only called if the actual interface function rules
 * out some of the easy cases. If we get here, we can assume that
 * neither of the values is HiZ, and the values are not exactly equal.
 */
vvp_scalar_t fully_featured_resolv_(vvp_scalar_t a, vvp_scalar_t b)
{

      إذا (UNAMBIG(a.قيمة_2) && UNAMBIG(b.قيمة_2)) {

	      /* If both signals are unambiguous, simply choose
		 the stronger. If they have the same strength
		 but different values, then this becomes
		 ambiguous. */

	    إذا ((b.قيمة_2&0x07) > (a.قيمة_2&0x07)) {

		    /* b value is stronger. Take it. */
		  إرجاع b;

	    } وإلا إذا ((b.قيمة_2&0x77) == (a.قيمة_2&0x77)) {

		    // Strengths are the same. Since we know already
		    // that the values are not the same, Make value
		    // into "x".
		  vvp_scalar_t tmp (a);
		  tmp.قيمة_2 = (tmp.قيمة_2&0x77) | 0x80;
		  إرجاع tmp;

	    } وإلا {

		    /* Must be "a" is the stronger one. */
		  إرجاع a;
	    }

      }

	/* If one of the signals is unambiguous, then it
	   will sweep up the weaker parts of the ambiguous
	   signal. The result may be ambiguous, or maybe not. */

      إذا (UNAMBIG(a.قيمة_2)) {
	    vvp_scalar_t res;

	    إذا ((a.قيمة_2&0x70) > (b.قيمة_2&0x70))
		  res.قيمة_2 |= a.قيمة_2&0xf0;
	    وإلا
		  res.قيمة_2 |= b.قيمة_2&0xf0;

	    إذا ((a.قيمة_2&0x07) > (b.قيمة_2&0x07))
		  res.قيمة_2 |= a.قيمة_2&0x0f;
	    وإلا
		  res.قيمة_2 |= b.قيمة_2&0x0f;

	    إرجاع res;

      } وإلا إذا (UNAMBIG(b.قيمة_2)) {

	    vvp_scalar_t res;

	    إذا ((b.قيمة_2&0x70) > (a.قيمة_2&0x70))
		  res.قيمة_2 |= b.قيمة_2&0xf0;
	    وإلا
		  res.قيمة_2 |= a.قيمة_2&0xf0;

	    إذا ((b.قيمة_2&0x07) > (a.قيمة_2&0x07))
		  res.قيمة_2 |= b.قيمة_2&0x0f;
	    وإلا
		  res.قيمة_2 |= a.قيمة_2&0x0f;

	    إرجاع res;

      }


	/* If both signals are ambiguous, then the result
	   has an even wider ambiguity. */

      unsigned tmp = 0;
      صحيح sv1a = (a.قيمة_2 & 0x80) ? STREN1(a.قيمة_2) : - STREN1(a.قيمة_2);
      صحيح sv0a = (a.قيمة_2 & 0x08) ? STREN0(a.قيمة_2) : - STREN0(a.قيمة_2);
      صحيح sv1b = (b.قيمة_2 & 0x80) ? STREN1(b.قيمة_2) : - STREN1(b.قيمة_2);
      صحيح sv0b = (b.قيمة_2 & 0x08) ? STREN0(b.قيمة_2) : - STREN0(b.قيمة_2);

      صحيح sv1 = sv1a;
      صحيح sv0 = sv0a;

      إذا (sv0a > sv1)
	    sv1 = sv0a;
      إذا (sv1b > sv1)
	    sv1 = sv1b;
      إذا (sv0b > sv1)
	    sv1 = sv0b;

      إذا (sv1a < sv0)
	    sv0 = sv1a;
      إذا (sv1b < sv0)
	    sv0 = sv1b;
      إذا (sv0b < sv0)
	    sv0 = sv0b;

      إذا (sv1 > 0) {
	    tmp |= 0x80;
	    tmp |= sv1 << 4;
      } وإلا {
	      /* Set the MSB when both arguments MSBs are set. This
		 can only happen if both one strengths are zero. */
	    tmp |= (a.قيمة_2&b.قيمة_2)&0x80;
	    tmp |= (-sv1) << 4;
      }

      إذا (sv0 > 0) {
	    tmp |= 0x08;
	    tmp |= sv0;
      } وإلا {
	    tmp |= (-sv0);
      }

      vvp_scalar_t res;
      res.قيمة_2 = tmp;

	/* Canonicalize the HiZ value. */
      إذا ((res.قيمة_2&0x77) == 0)
	    res.قيمة_2 = 0;

      إرجاع res;
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

vvp_vector4_t reduce4(ثابت vvp_vector8_t&that)
{
      vvp_vector4_t out (that.size());
      لكل (unsigned idx = 0 ;  idx < out.size() ;  idx += 1)
	    out.set_bit(idx, that.value(idx).value());

      إرجاع out;
}

vvp_bit4_t compare_gtge(ثابت vvp_vector4_t&lef, ثابت vvp_vector4_t&rig,
			vvp_bit4_t out_if_equal)
{
      unsigned min_size = lef.size();
      إذا (rig.size() < min_size)
	    min_size = rig.size();

	// If one of the inputs is nil, treat is as all X values, and
	// that makes the result BIT4_X.
      إذا (min_size == 0)
	    إرجاع BIT4_X;

	// As per the IEEE1364 definition of >, >=, < and <=, if there
	// are any X or Z values in either of the operand vectors,
	// then the result of the compare is BIT4_X.

	// Check for X/Z in the left operand
      إذا (lef.has_xz())
	    إرجاع BIT4_X;

	// Check for X/Z in the right operand
      إذا (rig.has_xz())
	    إرجاع BIT4_X;

      لكل (unsigned idx = lef.size() ; idx > rig.size() ;  idx -= 1) {
	    إذا (lef.value(idx-1) == BIT4_1)
		  إرجاع BIT4_1;
      }

      لكل (unsigned idx = rig.size() ; idx > lef.size() ;  idx -= 1) {
	    إذا (rig.value(idx-1) == BIT4_1)
		  إرجاع BIT4_0;
      }

      لكل (unsigned idx = min_size ; idx > 0 ;  idx -= 1) {
	    vvp_bit4_t lv = lef.value(idx-1);
	    vvp_bit4_t rv = rig.value(idx-1);

	    إذا (lv == rv)
		  استمر;

	    إذا (lv == BIT4_1)
		  إرجاع BIT4_1;
	    وإلا
		  إرجاع BIT4_0;
      }

      إرجاع out_if_equal;
}

vvp_bit4_t compare_gtge_signed(ثابت vvp_vector4_t&a,
			       ثابت vvp_vector4_t&b,
			       vvp_bit4_t out_if_equal)
{
      أكد(a.size() == b.size());

      unsigned sign_idx = a.size()-1;
      vvp_bit4_t a_sign = a.value(sign_idx);
      vvp_bit4_t b_sign = b.value(sign_idx);

      إذا (bit4_is_xz(a_sign))
	    إرجاع BIT4_X;
      إذا (bit4_is_xz(b_sign))
	    إرجاع BIT4_X;

      إذا (a_sign == b_sign)
	    إرجاع compare_gtge(a, b, out_if_equal);

      إذا (a.has_xz())
	    إرجاع BIT4_X;

      إذا (b.has_xz())
	    إرجاع BIT4_X;

      إذا(a_sign == BIT4_0)
	    إرجاع BIT4_1;
      وإلا
	    إرجاع BIT4_0;
}
