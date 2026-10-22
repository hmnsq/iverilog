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

using ନାମାକାଶ std;

/* This is the size of an unsigned long in bits. This is just a
   convenience macro. */
# define CPU_WORD_BITS (8*ଆକାର(unsigned long))
# define ଶୀର୍ଷ_BIT (1UL << (CPU_WORD_BITS-1))

permaheap vvp_net_fun_t::heap_;
permaheap vvp_net_fil_t::heap_;

// Allocate around 1Megabyte/chunk.
ସ୍ଥିର ଅଚଳ size_t VVP_NET_CHUNK = 1024*1024/ଆକାର(vvp_net_t);
ସ୍ଥିର vvp_net_t*vvp_net_alloc_table = ଖାଲି;
#ifdef CHECK_WITH_VALGRIND
ସ୍ଥିର vvp_net_t **vvp_net_pool = ଖାଲି;
ସ୍ଥିର unsigned vvp_net_pool_ଗଣନା = 0;
#endif
ସ୍ଥିର size_t vvp_net_alloc_remaining = 0;
// For statistics, count the vvp_nets allocated and the bytes of alloc
// chunks allocated.
unsigned long ଗଣନା_vvp_nets = 0;
size_t size_vvp_nets = 0;

ଶୂନ୍ୟ* vvp_net_t::operator ନୂତନ (size_t size)
{
      ପ୍ରତିଜ୍ଞା(size == ଆକାର(vvp_net_t));
      ଯଦି (vvp_net_alloc_remaining == 0) {
	    vvp_net_alloc_table = ::ନୂତନ vvp_net_t[VVP_NET_CHUNK];
	    vvp_net_alloc_remaining = VVP_NET_CHUNK;
	    size_vvp_nets += size*VVP_NET_CHUNK;
#ifdef CHECK_WITH_VALGRIND
	    VALGRIND_MAKE_MEM_NOACCESS(vvp_net_alloc_table, size*VVP_NET_CHUNK);
	    VALGRIND_CREATE_MEMPOOL(vvp_net_alloc_table, 0, 0);
	    vvp_net_pool_ଗଣନା += 1;
	    vvp_net_pool = static_cast<vvp_net_t **>(realloc(vvp_net_pool,
	                   vvp_net_pool_ଗଣନା*ଆକାର(vvp_net_t **)));
	    vvp_net_pool[vvp_net_pool_ଗଣନା-1] = vvp_net_alloc_table;
#endif
      }

      vvp_net_t*return_this = vvp_net_alloc_table;
#ifdef CHECK_WITH_VALGRIND
      VALGRIND_MEMPOOL_ALLOC(vvp_net_pool[vvp_net_pool_ଗଣନା-1],
                             return_this, size);
      return_this->pool = vvp_net_pool[vvp_net_pool_ଗଣନା-1];
#endif
      vvp_net_alloc_table += 1;
      vvp_net_alloc_remaining -= 1;
      ଗଣନା_vvp_nets += 1;
      ଫେରନ୍ତୁ return_this;
}

#ifdef CHECK_WITH_VALGRIND
ସ୍ଥିର map<vvp_net_t*, ତାର୍କିକ> vvp_net_map;
ସ୍ଥିର map<sfunc_core*, ତାର୍କିକ> sfunc_map;
ସ୍ଥିର map<vvp_udp_fun_core*, ତାର୍କିକ> udp_map;
ସ୍ଥିର map<resolv_core*, ତାର୍କିକ> resolv_map;
ସ୍ଥିର vvp_net_t **local_net_pool = 0;
ସ୍ଥିର unsigned local_net_pool_ଗଣନା = 0;

ଶୂନ୍ୟ pool_local_net(vvp_net_t*net)
{
      local_net_pool_ଗଣନା += 1;
      local_net_pool = static_cast<vvp_net_t **>(realloc(local_net_pool,
                       local_net_pool_ଗଣନା*ଆକାର(vvp_net_t **)));
      local_net_pool[local_net_pool_ଗଣନା-1] = net;
}

ଶୂନ୍ୟ vvp_net_delete(vvp_net_t *item)
{
      vvp_net_map[item] = ସତ୍ୟ;
      ଯଦି (sfunc_core*tmp = dynamic_cast<sfunc_core*> (item->fun)) {
	    sfunc_map[tmp] = ସତ୍ୟ;
      }
      ଯଦି (vvp_udp_fun_core*tmp = dynamic_cast<vvp_udp_fun_core*> (item->fun)) {
	    udp_map[tmp] = ସତ୍ୟ;
      }
      ଯଦି (resolv_core*tmp = dynamic_cast<resolv_core*> (item->fun)) {
	    resolv_map[tmp] = ସତ୍ୟ;
      }
	/* Only delete static object variables. */
      ଯଦି (vvp_fun_signal_object_sa*tmp = dynamic_cast<vvp_fun_signal_object_sa*> (item->fun)) {
	    delete tmp;
      }
	/* Only delete static string variables. */
      ଯଦି (vvp_fun_signal_string_sa*tmp = dynamic_cast<vvp_fun_signal_string_sa*> (item->fun)) {
	    delete tmp;
      }
}

ଶୂନ୍ୟ vvp_net_pool_delete()
{
      unsigned long vvp_nets_del = 0;

      ପାଇଁ (unsigned idx = 0; idx < local_net_pool_ଗଣନା; idx += 1) {
	    vvp_net_delete(local_net_pool[idx]);
      }
      ମୁକ୍ତ(local_net_pool);
      local_net_pool = 0;
      local_net_pool_ଗଣନା = 0;

      map<vvp_net_t*, ତାର୍କିକ>::iterator iter;
      ପାଇଁ (iter = vvp_net_map.begin(); iter != vvp_net_map.end(); ++ iter ) {
	    vvp_nets_del += 1;
	    VALGRIND_MEMPOOL_FREE(iter->first->pool, iter->first);
      }
      vvp_net_map.clear();

      map<sfunc_core*, ତାର୍କିକ>::iterator siter;
      ପାଇଁ (siter = sfunc_map.begin(); siter != sfunc_map.end(); ++ siter ) {
	    delete siter->first;
      }
      sfunc_map.clear();

      map<vvp_udp_fun_core*, ତାର୍କିକ>::iterator uiter;
      ପାଇଁ (uiter = udp_map.begin(); uiter != udp_map.end(); ++ uiter ) {
	    delete uiter->first;
      }
      udp_map.clear();

      map<resolv_core*, ତାର୍କିକ>::iterator riter;
      ପାଇଁ (riter = resolv_map.begin(); riter != resolv_map.end(); ++ riter ) {
	    delete riter->first;
      }
      resolv_map.clear();

      ଯଦି (RUNNING_ON_VALGRIND && (vvp_nets_del != ଗଣନା_vvp_nets)) {
	    fflush(ଖାଲି);
	    VALGRIND_PRINTF("Error: vvp missed deleting %ld of %lu net(s).",
	                    (long) ଗଣନା_vvp_nets - vvp_nets_del,
	                    ଗଣନା_vvp_nets);
      }

      ପାଇଁ (unsigned idx = 0; idx < vvp_net_pool_ଗଣନା; idx += 1) {
	    VALGRIND_DESTROY_MEMPOOL(vvp_net_pool[idx]);
	    ::delete [] vvp_net_pool[idx];
      }
      ମୁକ୍ତ(vvp_net_pool);
      vvp_net_pool = ଖାଲି;
      vvp_net_pool_ଗଣନା = 0;
}
#endif

ଶୂନ୍ୟ vvp_net_t::operator delete(ଶୂନ୍ୟ*)
{
      ପ୍ରତିଜ୍ଞା(0);
}

vvp_net_t::vvp_net_t()
: out_(vvp_net_ptr_t(0,0))
{
      fun = 0;
      fil = 0;
}

ଶୂନ୍ୟ vvp_net_t::link(vvp_net_ptr_t port_to_link)
{
      vvp_net_t*net = port_to_link.ptr();

	// Connect nodes with vvp_fun_modpath_src to the head of the
	// linked list, so that vvp_fun_modpath_src are always evaluated
	// before their respective vvp_fun_modpath
      ଯଦି (dynamic_cast<vvp_fun_modpath_src*>(net->fun)) {
	    net->port[port_to_link.port()] = out_;
	    out_ = port_to_link;
	// All other nodes connect after the last vvp_fun_modpath_src
      } ଅନ୍ୟଥା {
	      // List is empty or first node has no vvp_fun_modpath_src
	      // then just insert the node into the head of linked list
	    ଯଦି (out_.ptr() == nullptr || ! dynamic_cast<vvp_fun_modpath_src*>(out_.ptr()->fun)) {
		  net->port[port_to_link.port()] = out_;
		  out_ = port_to_link;
		  ଫେରନ୍ତୁ;
	    }

	    vvp_net_ptr_t cur = out_;
	    vvp_net_ptr_t prev = vvp_net_ptr_t(nullptr,0);

	      // Traverse through linked list
	    ଯେତେବେଳେ (dynamic_cast<vvp_fun_modpath_src*>(cur.ptr()->fun)) {
		  prev = cur;
		  cur = cur.ptr()->port[cur.port()];
		  ଯଦି (! cur.ptr()) ଭାଙ୍ଗ; // End of list
	    }

	    ପ୍ରତିଜ୍ଞା(prev.ptr());

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
ଶୂନ୍ୟ vvp_net_t::unlink(vvp_net_ptr_t dst_ptr)
{
      vvp_net_t*net = dst_ptr.ptr();
      unsigned net_port = dst_ptr.port();

      ଯଦି (out_ == dst_ptr) {
	      /* If the drive fan-out list starts with this pointer,
		 then the unlink is easy. Pull the list forward. */
	    out_ = net->port[net_port];
      } ଅନ୍ୟଥା ଯଦି (! out_.nil()) {
	      /* Scan the linked list, looking for the net_ptr_t
		 pointer *before* the one we wish to remove. */
	    vvp_net_ptr_t cur = out_;
	    ପ୍ରତିଜ୍ଞା(!cur.nil());
	    vvp_net_t*cur_net = cur.ptr();
	    unsigned cur_port = cur.port();
	    ଯେତେବେଳେ (cur_net && cur_net->port[cur_port] != dst_ptr) {
		  cur = cur_net->port[cur_port];
		  cur_net = cur.ptr();
		  cur_port = cur.port();
	    }
	      /* Unlink. */
	    ଯଦି (cur_net) cur_net->port[cur_port] = net->port[net_port];
      }

      net->port[net_port] = vvp_net_ptr_t(0,0);
}

ଶୂନ୍ୟ vvp_net_t::ଗଣନା_drivers(unsigned idx, unsigned counts[4])
{
      counts[0] = 0;
      counts[1] = 0;
      counts[2] = 0;
      counts[3] = 0;

        /* $countdrivers can only be used on wires. */
      vvp_wire_base*wire=dynamic_cast<vvp_wire_base*>(fil);
      ପ୍ରତିଜ୍ଞା(wire);

      ଯଦି (wire->is_forced(idx))
            counts[3] = 1;

        /* If the net has multiple drivers, we need to interrogate the
           resolver network to get the driven values. */
      ଯଦି (resolv_core*resolver = dynamic_cast<resolv_core*>(fun)) {
            resolver->ଗଣନା_drivers(idx, counts);
            ଫେରନ୍ତୁ;
      }
      ଯଦି (vvp_island_port*resolver = dynamic_cast<vvp_island_port*>(fun)) {
            resolver->ଗଣନା_drivers(idx, counts);
            ଫେରନ୍ତୁ;
      }

        /* If the functor is not a resolver, there is only one driver, so
           we can just interrogate the filter to get the driven value. */
      update_driver_counts(wire->driven_ମୂଲ୍ୟ(idx), counts);
}

ଶୂନ୍ୟ vvp_net_fun_t::operator delete(ଶୂନ୍ୟ*)
{
#ifndef CHECK_WITH_VALGRIND
      ପ୍ରତିଜ୍ଞା(0);
#endif
}


vvp_net_fil_t::vvp_net_fil_t()
{
      force_link_ = 0;
      force_propagate_ = ମିଥ୍ୟା;
      ଗଣନା_filters += 1;
}

vvp_net_fil_t::~vvp_net_fil_t()
{
      ପ୍ରତିଜ୍ଞା(force_link_ == 0);
}

vvp_net_fil_t::prop_t vvp_net_fil_t::filter_vec4(ଅଚଳ vvp_vector4_t&,
                                                 vvp_vector4_t&,
                                                 unsigned, unsigned)
{
      ଫେରନ୍ତୁ PROP;
}

vvp_net_fil_t::prop_t vvp_net_fil_t::filter_vec8(ଅଚଳ vvp_vector8_t&,
                                                 vvp_vector8_t&,
                                                 unsigned, unsigned)
{
      ଫେରନ୍ତୁ PROP;
}

vvp_net_fil_t::prop_t vvp_net_fil_t::filter_real(ଦ୍ୱିଗୁଣ&)
{
      ଫେରନ୍ତୁ PROP;
}

vvp_net_fil_t::prop_t vvp_net_fil_t::filter_string(ଅଚଳ string&)
{
      ଫେରନ୍ତୁ PROP;
}

vvp_net_fil_t::prop_t vvp_net_fil_t::filter_object(vvp_object_t&)
{
      ଫେରନ୍ତୁ PROP;
}

ଶୂନ୍ୟ vvp_net_fil_t::force_mask(ଅଚଳ vvp_vector2_t&mask)
{
      ଯଦି (force_mask_.size() == 0)
	    force_mask_ = vvp_vector2_t(vvp_vector2_t::FILL0, mask.size());

      ପ୍ରତିଜ୍ଞା(force_mask_.size() == mask.size());
      ପାଇଁ (unsigned idx = 0 ; idx < mask.size() ; idx += 1) {
	    ଯଦି (mask.value(idx) == 0)
		  ଜାରି;

	    force_mask_.set_bit(idx, 1);
	    force_propagate_ = ସତ୍ୟ;
      }
}

ଶୂନ୍ୟ vvp_net_fil_t::release_mask(ଅଚଳ vvp_vector2_t&mask)
{
      ଯଦି (force_mask_.size() == 0)
	    ଫେରନ୍ତୁ;

      ପ୍ରତିଜ୍ଞା(force_mask_.size() == mask.size());
      ପାଇଁ (unsigned idx = 0 ; idx < mask.size() ; idx += 1) {
	    ଯଦି (mask.value(idx))
		  force_mask_.set_bit(idx, 0);
      }

      ଯଦି (force_mask_.is_zero())
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
ଶୂନ୍ୟ vvp_net_fil_t::force_link(vvp_net_t*dst, vvp_net_t*src)
{
      ପ୍ରତିଜ୍ଞା(dst->fil == ଏହି);

      ଯଦି (force_link_ == 0) {
	    force_link_ = ନୂତନ vvp_net_t;
	      // Use port[3] to hold the force destination.
	    force_link_->port[3] = vvp_net_ptr_t(dst, 0);
	    force_link_->port[2] = vvp_net_ptr_t(0,0);
	    force_link_->fun = ନୂତନ vvp_fun_force;
      }

      force_unlink();
      ପ୍ରତିଜ୍ଞା(force_link_->port[2] == vvp_net_ptr_t(0,0));

	// Use port[2] to hold the force source.
      force_link_->port[2] = vvp_net_ptr_t(src,0);

      vvp_net_ptr_t dst_ptr(force_link_, 0);
      src->link(dst_ptr);
}

ଶୂନ୍ୟ vvp_net_fil_t::operator delete(ଶୂନ୍ୟ*)
{
      ପ୍ରତିଜ୍ଞା(0);
}

ଶୂନ୍ୟ vvp_net_fil_t::force_unlink(ଶୂନ୍ୟ)
{
      ଯଦି (force_link_ == 0) ଫେରନ୍ତୁ;
      vvp_net_t*src = force_link_->port[2].ptr();
      ଯଦି (src == 0) ଫେରନ୍ତୁ;

      src->unlink(vvp_net_ptr_t(force_link_,0));
      force_link_->port[2] = vvp_net_ptr_t(0,0);
}

/* *** BIT operations *** */
vvp_bit4_t add_with_carry(vvp_bit4_t a, vvp_bit4_t b, vvp_bit4_t&c)
{
      ଯଦି (bit4_is_xz(a) || bit4_is_xz(b) || bit4_is_xz(c)) {
	    c = BIT4_X;
	    ଫେରନ୍ତୁ BIT4_X;
      }

	// NOTE: This relies on the facts that XZ values have been
	// weeded out, and that BIT4_1 is 1 and BIT4_0 is 0.
      ପୂର୍ଣ୍ଣସଂଖ୍ୟା sum = (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)a + (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)b + (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)c;

      ବାଛ (sum) {
	  ସ୍ଥିତି 0:
	      // c must already be 0.
	    ଫେରନ୍ତୁ BIT4_0;
	  ସ୍ଥିତି 1:
	    c = BIT4_0;
	    ଫେରନ୍ତୁ BIT4_1;
	  ସ୍ଥିତି 2:
	    c = BIT4_1;
	    ଫେରନ୍ତୁ BIT4_0;
	  ସ୍ଥିତି 3:
	    c = BIT4_1;
	    ଫେରନ୍ତୁ BIT4_1;
      }
      fprintf(stderr, "Incorrect result %d.\n", sum);
      ପ୍ରତିଜ୍ଞା(0);
      ଫେରନ୍ତୁ BIT4_X;
}

vvp_bit4_t scalar_to_bit4(PLI_INT32 scalar)
{
      ବାଛ(scalar) {
	  ସ୍ଥିତି vpi0:
	    ଫେରନ୍ତୁ BIT4_0;
	  ସ୍ଥିତି vpi1:
	    ଫେରନ୍ତୁ BIT4_1;
	  ସ୍ଥିତି vpiX:
	    ଫେରନ୍ତୁ BIT4_X;
	  ସ୍ଥିତି vpiZ:
	    ଫେରନ୍ତୁ BIT4_Z;
      }
      fprintf(stderr, "Unsupported scalar value %d.\n", (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)scalar);
      ପ୍ରତିଜ୍ଞା(0);
      ଫେରନ୍ତୁ BIT4_X;
}

vvp_bit4_t operator ^ (vvp_bit4_t a, vvp_bit4_t b)
{
      ଯଦି (bit4_is_xz(a))
	    ଫେରନ୍ତୁ BIT4_X;
      ଯଦି (bit4_is_xz(b))
	    ଫେରନ୍ତୁ BIT4_X;
      ଯଦି (a == BIT4_0)
	    ଫେରନ୍ତୁ b;
      ଯଦି (b == BIT4_0)
	    ଫେରନ୍ତୁ a;
      ଫେରନ୍ତୁ BIT4_0;
}

ostream& operator<<(ostream&out, vvp_bit4_t bit)
{
      ବାଛ (bit) {
	  ସ୍ଥିତି BIT4_0:
	    out << "0";
	    ଭାଙ୍ଗ;
	  ସ୍ଥିତି BIT4_1:
	    out << "1";
	    ଭାଙ୍ଗ;
	  ସ୍ଥିତି BIT4_X:
	    out << "X";
	    ଭାଙ୍ଗ;
	  ସ୍ଥିତି BIT4_Z:
	    out << "Z";
	    ଭାଙ୍ଗ;
	  ପୂର୍ବନିର୍ଦ୍ଧାରିତ:
	    out << "?";
	    ଭାଙ୍ଗ;
      }
      ଫେରନ୍ତୁ out;
}

ପ୍ରକାରବ୍ୟାଖ୍ୟା unsigned short edge_t;

ଧାଡ଼ିଭିତରେ edge_t VVP_EDGE(vvp_bit4_t from, vvp_bit4_t to)
{
      ଫେରନ୍ତୁ 1 << ((from << 2) | to);
}

ଅଚଳ edge_t vvp_edge_posedge
      = VVP_EDGE(BIT4_0,BIT4_1)
      | VVP_EDGE(BIT4_0,BIT4_X)
      | VVP_EDGE(BIT4_0,BIT4_Z)
      | VVP_EDGE(BIT4_X,BIT4_1)
      | VVP_EDGE(BIT4_Z,BIT4_1)
      ;

ଅଚଳ edge_t vvp_edge_negedge
      = VVP_EDGE(BIT4_1,BIT4_0)
      | VVP_EDGE(BIT4_1,BIT4_X)
      | VVP_EDGE(BIT4_1,BIT4_Z)
      | VVP_EDGE(BIT4_X,BIT4_0)
      | VVP_EDGE(BIT4_Z,BIT4_0)
      ;

ପୂର୍ଣ୍ଣସଂଖ୍ୟା edge(vvp_bit4_t from, vvp_bit4_t to)
{
      edge_t mask = VVP_EDGE(from, to);
      ଯଦି (mask & vvp_edge_posedge)
	    ଫେରନ୍ତୁ 1;
      ଯଦି (mask & vvp_edge_negedge)
	    ଫେରନ୍ତୁ -1;
      ଫେରନ୍ତୁ 0;
}

unsigned long multiply_with_carry(unsigned long a, unsigned long b,
				  unsigned long&carry)
{
      ଅଚଳ unsigned long mask = (1UL << (CPU_WORD_BITS/2)) - 1;
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
      ଫେରନ୍ତୁ (r1 << (CPU_WORD_BITS/2)) + r00;
}


ଶୂନ୍ୟ vvp_send_vec8(vvp_net_ptr_t ptr, ଅଚଳ vvp_vector8_t&val)
{
      ଯେତେବେଳେ (vvp_net_t*cur = ptr.ptr()) {
	    vvp_net_ptr_t next_val = cur->port[ptr.port()];

	    ଯଦି (cur->fun)
		  cur->fun->recv_vec8(ptr, val);

	    ptr = next_val;
      }
}

ଶୂନ୍ୟ vvp_send_real(vvp_net_ptr_t ptr, ଦ୍ୱିଗୁଣ val, vvp_context_t context)
{
      ଯେତେବେଳେ (vvp_net_t*cur = ptr.ptr()) {
	    vvp_net_ptr_t next_val = cur->port[ptr.port()];

	    ଯଦି (cur->fun)
		  cur->fun->recv_real(ptr, val, context);

	    ptr = next_val;
      }
}

ଅଚଳ vvp_vector4_t vvp_vector4_t::nil;

ଶୂନ୍ୟ vvp_vector4_t::copy_bits(ଅଚଳ vvp_vector4_t&that)
{

      ଯଦି (size_ == that.size_) {
	    ଯଦି (size_ > BITS_PER_WORD) {
		  unsigned ଶବ୍ଦଗୁଡ଼ିକ = (size_+BITS_PER_WORD-1) / BITS_PER_WORD;
		  ପାଇଁ (unsigned idx = 0 ;  idx < ଶବ୍ଦଗୁଡ଼ିକ ;  idx += 1)
			abits_ptr_[idx] = that.abits_ptr_[idx];
		  ପାଇଁ (unsigned idx = 0 ;  idx < ଶବ୍ଦଗୁଡ଼ିକ ;  idx += 1)
			bbits_ptr_[idx] = that.bbits_ptr_[idx];
	    } ଅନ୍ୟଥା {
		  abits_val_ = that.abits_val_;
		  bbits_val_ = that.bbits_val_;
	    }
	    ଫେରନ୍ତୁ;
      }

	/* Now we know that the sizes of this and that are definitely
	   different. We can use that in code below. In any case, we
	   need to copy only the smaller of the sizes. */

	/* If source and destination are both short, then mask/copy
	   the bit values. */
      ଯଦି (size_ <= BITS_PER_WORD && that.size_ <= BITS_PER_WORD) {
	    unsigned bits_to_copy = (that.size_ < size_) ? that.size_ : size_;
	    unsigned long mask = ((bits_to_copy < BITS_PER_WORD) ? (1UL << bits_to_copy) :
	                                                           (0UL)) - 1UL;
	    abits_val_ &= ~mask;
	    bbits_val_ &= ~mask;
	    abits_val_ |= that.abits_val_&mask;
	    bbits_val_ |= that.bbits_val_&mask;
	    ଫେରନ୍ତୁ;
      }

	/* Now we know that either source or destination are long. If
	   the destination is short, then mask/copy from the low word
	   of the long source. */
      ଯଦି (size_ <= BITS_PER_WORD) {
	    abits_val_ = that.abits_ptr_[0];
	    bbits_val_ = that.bbits_ptr_[0];
	    ଯଦି (size_ < BITS_PER_WORD) {
		  unsigned long mask = (1UL << size_) - 1UL;
		  abits_val_ &= mask;
		  bbits_val_ &= mask;
	    }
	    ଫେରନ୍ତୁ;
      }

	/* Now we know that the destination must be long. If the
	   source is short, then mask/copy from its value. */
      ଯଦି (that.size_ <= BITS_PER_WORD) {
	    unsigned long mask;
	    ଯଦି (that.size_ < BITS_PER_WORD) {
		  mask = (1UL << that.size_) - 1UL;
		  abits_ptr_[0] &= ~mask;
		  bbits_ptr_[0] &= ~mask;
	    } ଅନ୍ୟଥା {
		  mask = -1UL;
	    }
	    abits_ptr_[0] |= that.abits_val_&mask;
	    bbits_ptr_[0] |= that.bbits_val_&mask;
	    ଫେରନ୍ତୁ;
      }

	/* Finally, we know that source and destination are long. copy
	   words until we get to the last. */
      unsigned bits_to_copy = (that.size_ < size_) ? that.size_ : size_;
      unsigned word = 0;
      ଯେତେବେଳେ (bits_to_copy >= BITS_PER_WORD) {
	    abits_ptr_[word] = that.abits_ptr_[word];
	    bbits_ptr_[word] = that.bbits_ptr_[word];
	    bits_to_copy -= BITS_PER_WORD;
	    word += 1;
      }
      ଯଦି (bits_to_copy > 0) {
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
ଶୂନ୍ୟ vvp_vector4_t::copy_from_big_(ଅଚଳ vvp_vector4_t&that)
{
      unsigned ଶବ୍ଦଗୁଡ଼ିକ = (size_+BITS_PER_WORD-1) / BITS_PER_WORD;
      abits_ptr_ = ନୂତନ unsigned long[2*ଶବ୍ଦଗୁଡ଼ିକ];
      bbits_ptr_ = abits_ptr_ + ଶବ୍ଦଗୁଡ଼ିକ;

      ପାଇଁ (unsigned idx = 0 ;  idx < ଶବ୍ଦଗୁଡ଼ିକ ;  idx += 1)
	    abits_ptr_[idx] = that.abits_ptr_[idx];
      ପାଇଁ (unsigned idx = 0 ;  idx < ଶବ୍ଦଗୁଡ଼ିକ ;  idx += 1)
	    bbits_ptr_[idx] = that.bbits_ptr_[idx];
}

/*
 * The copy_inverted_from_ method is just like the copy_from_ method,
 * except that we combine that with an invert. This allows the ~ and
 * the assignment to be blended in many common cases.
 */
ଶୂନ୍ୟ vvp_vector4_t::copy_inverted_from_(ଅଚଳ vvp_vector4_t&that)
{
      size_ = that.size_;
      ଯଦି (size_ > BITS_PER_WORD) {
	    unsigned ଶବ୍ଦଗୁଡ଼ିକ = (size_+BITS_PER_WORD-1) / BITS_PER_WORD;
	    abits_ptr_ = ନୂତନ unsigned long[2*ଶବ୍ଦଗୁଡ଼ିକ];
	    bbits_ptr_ = abits_ptr_ + ଶବ୍ଦଗୁଡ଼ିକ;

	    unsigned remaining = size_;
	    unsigned idx = 0;
	    ଯେତେବେଳେ (remaining >= BITS_PER_WORD) {
		  abits_ptr_[idx] = that.bbits_ptr_[idx] | ~that.abits_ptr_[idx];
		  idx += 1;
		  remaining -= BITS_PER_WORD;
	    }
	    ଯଦି (remaining > 0) {
		  unsigned long mask = (1UL<<remaining) - 1UL;
		  abits_ptr_[idx] = mask & (that.bbits_ptr_[idx] | ~that.abits_ptr_[idx]);
	    }

	    ପାଇଁ (idx = 0 ;  idx < ଶବ୍ଦଗୁଡ଼ିକ ;  idx += 1)
		  bbits_ptr_[idx] = that.bbits_ptr_[idx];

      } ଅନ୍ୟଥା {
	    unsigned long mask = (size_<BITS_PER_WORD)? (1UL<<size_)-1UL : -1UL;
	    abits_val_ = mask & (that.bbits_val_ | ~that.abits_val_);
	    bbits_val_ = that.bbits_val_;
      }
}

/* Make sure to set size_ before calling this routine. */
ଶୂନ୍ୟ vvp_vector4_t::allocate_ଶବ୍ଦଗୁଡ଼ିକ(unsigned long inita, unsigned long initb)
{
      ଯଦି (size_ > BITS_PER_WORD) {
	    unsigned cnt = (size_ + BITS_PER_WORD - 1) / BITS_PER_WORD;
	    abits_ptr_ = ନୂତନ unsigned long[2*cnt];
	    bbits_ptr_ = abits_ptr_ + cnt;
	    ପାଇଁ (unsigned idx = 0 ;  idx < cnt ;  idx += 1)
		  abits_ptr_[idx] = inita;
	    ପାଇଁ (unsigned idx = 0 ;  idx < cnt ;  idx += 1)
		  bbits_ptr_[idx] = initb;

      } ଅନ୍ୟଥା {
	    abits_val_ = inita;
	    bbits_val_ = initb;
      }
}

vvp_vector4_t::vvp_vector4_t(unsigned size__, ଦ୍ୱିଗୁଣ val)
: size_(size__)
{
      ତାର୍କିକ is_neg = ମିଥ୍ୟା;
      ଦ୍ୱିଗୁଣ fraction;
      ପୂର୍ଣ୍ଣସଂଖ୍ୟା exponent;

	/* We return 'bx for a NaN or +/- infinity. */
      ଯଦି (val != val || (val && (val == 0.5*val)))  {
	    allocate_ଶବ୍ଦଗୁଡ଼ିକ(WORD_X_ABITS, WORD_X_BBITS);
	    ଫେରନ୍ତୁ;
      }

	/* Convert to a positive result. */
      ଯଦି (val < 0.0) {
	    is_neg = ସତ୍ୟ;
	    val = -val;
      }
      allocate_ଶବ୍ଦଗୁଡ଼ିକ(WORD_0_ABITS, WORD_0_BBITS);

	/* Get the exponent and fractional part of the number. */
      fraction = frexp(val, &exponent);

	/* If the value is small enough just use lround(). */
      ଯଦି (exponent < BITS_PER_WORD-2) {
	    ଯଦି (is_neg) ଏହି->invert();  // Invert the bits if negative.
	    long sval = lround(val);
	    ଯଦି (is_neg) sval = -sval;
	      /* This requires that 0 and 1 have the same bbit value. */
	    ଯଦି (size_ > BITS_PER_WORD) {
		  abits_ptr_[0] = sval;
	    } ଅନ୍ୟଥା {
		  abits_val_ = sval;
	    }
	    ଫେରନ୍ତୁ;
      }

      unsigned nwords = (exponent-1) / BITS_PER_WORD;
      unsigned my_ଶବ୍ଦଗୁଡ଼ିକ = (size_ + BITS_PER_WORD - 1) / BITS_PER_WORD - 1;

      fraction = ldexp(fraction, (exponent-1) % BITS_PER_WORD + 1);

	/* Skip any leading bits. */
      ପାଇଁ (ପୂର୍ଣ୍ଣସଂଖ୍ୟା idx = (signed) nwords; idx > (signed) my_ଶବ୍ଦଗୁଡ଼ିକ; idx -=1) {
	    unsigned long bits = (unsigned long) fraction;
	    fraction = fraction - (ଦ୍ୱିଗୁଣ) bits;
	    fraction = ldexp(fraction, BITS_PER_WORD);
      }

	/* Convert the remaining bits as appropriate. */
      ଯଦି (my_ଶବ୍ଦଗୁଡ଼ିକ == 0) {
		  unsigned long bits = (unsigned long) fraction;
		  abits_val_ = bits;
		  fraction = fraction - (ଦ୍ୱିଗୁଣ) bits;
		    /* Round any fractional part up. */
		  ଯଦି (fraction >= 0.5) *ଏହି += (int64_t) 1;
      } ଅନ୍ୟଥା {
	    ଯଦି (nwords < my_ଶବ୍ଦଗୁଡ଼ିକ) my_ଶବ୍ଦଗୁଡ଼ିକ = nwords;
	    ପାଇଁ (ପୂର୍ଣ୍ଣସଂଖ୍ୟା idx = (signed)my_ଶବ୍ଦଗୁଡ଼ିକ; idx >= 0; idx -= 1) {
		  unsigned long bits = (unsigned long) fraction;
		  abits_ptr_[idx] = bits;
		  fraction = fraction - (ଦ୍ୱିଗୁଣ) bits;
		  fraction = ldexp(fraction, BITS_PER_WORD);
	    }
	      /* Round any fractional part up. */
	    ଯଦି (fraction >= ldexp(0.5, BITS_PER_WORD)) *ଏହି += (int64_t) 1;
       }

	/* Convert to a negative number if needed. */
      ଯଦି (is_neg) {
	    ଏହି->invert();
	    *ଏହି += (int64_t) 1;
      }
}

vvp_vector4_t::vvp_vector4_t(ଅଚଳ vvp_vector4_t&that,
			    unsigned adr, unsigned wid)
{
	// Set up and initialize the destination.
      size_ = wid;
      allocate_ଶବ୍ଦଗୁଡ଼ିକ(WORD_X_ABITS, WORD_X_BBITS);

	// Special case: selecting from far beyond the source vector,
	// to the result is all X bits. We're done.
      ଯଦି (adr >= that.size_)
	    ଫେରନ୍ତୁ;

	// Special case: The source is not quite big enough to supply
	// all bits, so get the bits that we can. The remainder will
	// be left at BIT4_X.
      ଯଦି ((adr + wid) > that.size_) {
	    unsigned use_wid = that.size_ - adr;
	    ପାଇଁ (unsigned idx = 0 ; idx < use_wid ; idx += 1)
		  set_bit(idx, that.value(adr+idx));

	    ଫେରନ୍ତୁ;
      }

	// At the point, we know that the source part is entirely
	// contained in the source vector.
	// assert((adr + wid) <= that.size_);

      ଯଦି (wid > BITS_PER_WORD) {
	      /* In this case, the subvector and the source vector are
		 long. Do the transfer reasonably efficiently. */
	    unsigned ptr = adr / BITS_PER_WORD;
	    unsigned long off = adr % BITS_PER_WORD;
	    unsigned long noff = BITS_PER_WORD - off;
	    unsigned long lmask = (1UL << off) - 1UL;
	    unsigned trans = 0;
	    unsigned dst = 0;
	    ଯେତେବେଳେ (trans < wid) {
		    // The low bits of the result.
		  abits_ptr_[dst] = (that.abits_ptr_[ptr] & ~lmask) >> off;
		  bbits_ptr_[dst] = (that.bbits_ptr_[ptr] & ~lmask) >> off;
		  trans += noff;

		  ଯଦି (trans >= wid)
			ଭାଙ୍ଗ;

		  ptr += 1;

		    // The high bits of the result. Skip this if the
		    // source and destination are perfectly aligned.
		  ଯଦି (noff != BITS_PER_WORD) {
			abits_ptr_[dst] |= (that.abits_ptr_[ptr]&lmask) << noff;
			bbits_ptr_[dst] |= (that.bbits_ptr_[ptr]&lmask) << noff;
			trans += off;
		  }

		  dst += 1;
	    }

      } ଅନ୍ୟଥା ଯଦି (that.size_ > BITS_PER_WORD) {
	      /* In this case, the subvector fits in a single word,
		 but the source is large. */
	    unsigned ptr = adr / BITS_PER_WORD;
	    unsigned long off = adr % BITS_PER_WORD;
	    unsigned trans = BITS_PER_WORD - off;
	    ଯଦି (trans > wid)
		  trans = wid;

	    ଯଦି (trans == BITS_PER_WORD) {
		    // Very special case: Copy exactly 1 perfectly
		    // aligned word.
		  abits_val_ = that.abits_ptr_[ptr];
		  bbits_val_ = that.bbits_ptr_[ptr];

	    } ଅନ୍ୟଥା {
		    // lmask is the low bits of the destination,
		    // masked into the source.
		  unsigned long lmask = (1UL<<trans) - 1UL;
		  lmask <<= off;

		    // The low bits of the result.
		  abits_val_ = (that.abits_ptr_[ptr] & lmask) >> off;
		  bbits_val_ = (that.bbits_ptr_[ptr] & lmask) >> off;

		  ଯଦି (trans < wid) {
			  // If there are more bits, then get them
			  // from the bottom of the next word of the
			  // source.
			unsigned long hmask = (1UL << (wid-trans)) - 1UL;

			  // The high bits of the result.
			abits_val_ |= (that.abits_ptr_[ptr+1]&hmask) << trans;
			bbits_val_ |= (that.bbits_ptr_[ptr+1]&hmask) << trans;
		  }
	    }

      } ଅନ୍ୟଥା ଯଦି (size_ == BITS_PER_WORD) {
	      /* We know that source and destination are short. If the
		 destination is a full word, then we know the copy is
		 aligned and complete. */
	    abits_val_ = that.abits_val_;
	    bbits_val_ = that.bbits_val_;

      } ଅନ୍ୟଥା {
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
ଶୂନ୍ୟ vvp_vector4_t::resize(unsigned newsize, vvp_bit4_t pad_bit)
{
      ଯଦି (size_ == newsize)
	    ଫେରନ୍ତୁ;

      unsigned cnt = (size_ + BITS_PER_WORD - 1) / BITS_PER_WORD;
      unsigned long word_pad_abits = 0;
      unsigned long word_pad_bbits = 0;
      ବାଛ (pad_bit) {
	  ସ୍ଥିତି BIT4_0:
	    word_pad_abits = WORD_0_ABITS;
	    word_pad_bbits = WORD_0_BBITS;
	    ଭାଙ୍ଗ;
	  ସ୍ଥିତି BIT4_1:
	    word_pad_abits = WORD_1_ABITS;
	    word_pad_bbits = WORD_1_BBITS;
	    ଭାଙ୍ଗ;
	  ସ୍ଥିତି BIT4_X:
	    word_pad_abits = WORD_X_ABITS;
	    word_pad_bbits = WORD_X_BBITS;
	    ଭାଙ୍ଗ;
	  ସ୍ଥିତି BIT4_Z:
	    word_pad_abits = WORD_Z_ABITS;
	    word_pad_bbits = WORD_Z_BBITS;
	    ଭାଙ୍ଗ;
      }

      ଯଦି (newsize > BITS_PER_WORD) {
	    unsigned newcnt = (newsize + BITS_PER_WORD - 1) / BITS_PER_WORD;
	    ଯଦି (newcnt == cnt) {
		    // If the word count doesn't change, then there is
		    // no need for re-allocation so we are done now.
		  ଯଦି (newsize > size_) {
			ଯଦି (unsigned fill = size_ % BITS_PER_WORD) {
			      abits_ptr_[cnt-1] &= ~((-1UL) << fill);
			      bbits_ptr_[cnt-1] &= ~((-1UL) << fill);
			      abits_ptr_[cnt-1] |= word_pad_abits << fill;
			      bbits_ptr_[cnt-1] |= word_pad_bbits << fill;
			}
		  }
		  size_ = newsize;
		  ଫେରନ୍ତୁ;
	    }

	    unsigned long*newbits = ନୂତନ unsigned long[2*newcnt];

	    ଯଦି (cnt > 1) {
		  unsigned trans = cnt;
		  ଯଦି (trans > newcnt)
			trans = newcnt;

		  ପାଇଁ (unsigned idx = 0 ;  idx < trans ;  idx += 1)
			newbits[idx] = abits_ptr_[idx];
		  ପାଇଁ (unsigned idx = 0 ;  idx < trans ;  idx += 1)
			newbits[newcnt+idx] = bbits_ptr_[idx];

		  delete[]abits_ptr_;

	    } ଅନ୍ୟଥା {
		  newbits[0] = abits_val_;
		  newbits[newcnt] = bbits_val_;
	    }

	    ଯଦି (newsize > size_) {
		  ଯଦି (unsigned fill = size_ % BITS_PER_WORD) {
			newbits[cnt-1] &= ~((-1UL) << fill);
			newbits[cnt-1] |= word_pad_abits << fill;
			newbits[newcnt+cnt-1] &= ~((-1UL) << fill);
			newbits[newcnt+cnt-1] |= word_pad_bbits << fill;
		  }
		  ପାଇଁ (unsigned idx = cnt ;  idx < newcnt ;  idx += 1)
			newbits[idx] = word_pad_abits;
		  ପାଇଁ (unsigned idx = cnt ;  idx < newcnt ;  idx += 1)
			newbits[newcnt+idx] = word_pad_bbits;
	    }

	    size_ = newsize;
	    abits_ptr_ = newbits;
	    bbits_ptr_ = newbits + newcnt;

      } ଅନ୍ୟଥା {
	    ଯଦି (cnt > 1) {
		  unsigned long newvala = abits_ptr_[0];
		  unsigned long newvalb = bbits_ptr_[0];
		  delete[]abits_ptr_;
		  abits_val_ = newvala;
		  bbits_val_ = newvalb;
	    }

	    ଯଦି (newsize > size_) {
		  abits_val_ &= ~((-1UL) << size_);
		  bbits_val_ &= ~((-1UL) << size_);
		  abits_val_ |= word_pad_abits << size_;
		  bbits_val_ |= word_pad_bbits << size_;
	    }

	    size_ = newsize;
      }
}


unsigned long* vvp_vector4_t::subarray(unsigned adr, unsigned wid, ତାର୍କିକ xz_to_0) ଅଚଳ
{
      ଅଚଳ unsigned BIT2_PER_WORD = 8*ଆକାର(unsigned long);
      unsigned awid = (wid + BIT2_PER_WORD - 1) / (BIT2_PER_WORD);
      unsigned long*val = ନୂତନ unsigned long[awid];

      ପାଇଁ (unsigned idx = 0 ;  idx < awid ;  idx += 1)
	    val[idx] = 0;

      ଯଦି (size_ <= BITS_PER_WORD) {
	      /* Handle the special case that the array is small. The
		 entire value of the vector4 is within the xbits_val_
		 so we know that the result is a single word, the
		 source is a single word, and we just have to loop
		 through that word. */
	    unsigned long atmp = abits_val_ >> adr;
	    unsigned long btmp = bbits_val_ >> adr;
	    ଯଦି (wid < BIT2_PER_WORD) {
		  atmp &= (1UL << wid) - 1;
		  btmp &= (1UL << wid) - 1;
	    }
	    ଯଦି (btmp) {
		  ଯଦି (xz_to_0)
			atmp &= ~btmp;
		  ଅନ୍ୟଥା
			ଯାଅ x_out;
	    }

	    val[0] = atmp;

      } ଅନ୍ୟଥା {

	    unsigned val_ptr = 0;
	    unsigned val_off = 0;

	      /* Get the first word we are scanning. We may in fact be
		 somewhere in the middle of that word. */
	    ଯେତେବେଳେ (wid > 0) {
		  unsigned long atmp = abits_ptr_[adr/BITS_PER_WORD];
		  unsigned long btmp = bbits_ptr_[adr/BITS_PER_WORD];
		  unsigned long off = adr%BITS_PER_WORD;
		  atmp >>= off;
		  btmp >>= off;

		  unsigned long trans = BITS_PER_WORD - off;
		  ଯଦି (trans > (BIT2_PER_WORD - val_off))
			trans = BIT2_PER_WORD - val_off;
		  ଯଦି (wid < trans)
			trans = wid;
		  ଯଦି (trans < BIT2_PER_WORD) {
			atmp &= (1UL << trans) - 1;
			btmp &= (1UL << trans) - 1;
		  }
		  ଯଦି (btmp) {
			ଯଦି (xz_to_0)
			      atmp &= ~btmp;
			ଅନ୍ୟଥା
			      ଯାଅ x_out;
		  }

		  val[val_ptr] |= atmp << val_off;
		  adr += trans;
		  wid -= trans;
		  val_off += trans;
		  ଯଦି (val_off == BIT2_PER_WORD) {
			val_ptr += 1;
			val_off = 0;
		  }
	    }
      }

      ଫେରନ୍ତୁ val;

 x_out:
      delete[]val;
      ଫେରନ୍ତୁ 0;
}

ଶୂନ୍ୟ vvp_vector4_t::setarray(unsigned adr, unsigned wid, ଅଚଳ unsigned long*val)
{
      ପ୍ରତିଜ୍ଞା(adr+wid <= size_);

      ଅଚଳ unsigned BIT2_PER_WORD = 8*ଆକାର(unsigned long);

      ଯଦି (size_ <= BITS_PER_WORD) {
	      // We know here that both the source and the target are
	      // within a single word. Write the bits into the
	      // abits_val_ directly.

	    ପ୍ରତିଜ୍ଞା(BIT2_PER_WORD <= BITS_PER_WORD);
	    unsigned long lmask = (1UL << adr) - 1UL;
	    unsigned long hmask = ((adr+wid) < BITS_PER_WORD)
		  ? -1UL << (adr+wid)
		  : 0;
	    unsigned long mask = ~(hmask | lmask);

	    abits_val_ &= ~mask;
	    bbits_val_ &= ~mask;

	    abits_val_ |= mask & (val[0] << adr);

      } ଅନ୍ୟଥା {
	      // The general case, there are multiple words of
	      // destination, and possibly multiple words of source
	      // data. Shift and mask as we go.
	    unsigned off = adr % BITS_PER_WORD;
	    unsigned ptr = adr / BITS_PER_WORD;
	    unsigned val_off = 0;
	    unsigned val_ptr = 0;
	    ଯେତେବେଳେ (wid > 0) {
		  unsigned trans = wid;
		  ଯଦି (trans > (BIT2_PER_WORD-val_off))
			trans = BIT2_PER_WORD-val_off;
		  ଯଦି (trans > (BITS_PER_WORD-off))
			trans = BITS_PER_WORD-off;

		  unsigned long lmask = (1UL << off) - 1UL;
		  unsigned long hmask = ((off+trans) < BITS_PER_WORD)
			? -1UL << (off+trans)
			: 0;
		  unsigned long mask = ~(hmask | lmask);

		  abits_ptr_[ptr] &= ~mask;
		  bbits_ptr_[ptr] &= ~mask;
		  ଯଦି (val_off >= off)
			abits_ptr_[ptr] |= mask & (val[val_ptr] >> (val_off-off));
		  ଅନ୍ୟଥା
			abits_ptr_[ptr] |= mask & (val[val_ptr] << (off-val_off));

		  wid -= trans;
		  val_off += trans;
		  ଯଦି (val_off == BIT2_PER_WORD) {
			val_off = 0;
			val_ptr += 1;
		  }
		  off += trans;
		  ଯଦି (off == BITS_PER_WORD) {
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
ତାର୍କିକ vvp_vector4_t::set_vec(unsigned adr, ଅଚଳ vvp_vector4_t&that)
{
      ପ୍ରତିଜ୍ଞା(adr+that.size_  <= size_);
      ତାର୍କିକ diff_flag = ମିଥ୍ୟା;

      ଯଦି (size_ <= BITS_PER_WORD) {

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
	    ଯଦି (hshift >= BITS_PER_WORD)
		  hmask = -1UL;
	    ଅନ୍ୟଥା
		  hmask = (1UL << (adr+that.size_)) - 1;
	    unsigned long mask = hmask & ~lmask;

	    unsigned long tmp = (that.abits_val_<<adr)&mask;
	    ଯଦି ((abits_val_&mask) != tmp) {
		  diff_flag = ସତ୍ୟ;
		  abits_val_ = (abits_val_ & ~mask) | tmp;
	    }
	    tmp = (that.bbits_val_<<adr) & mask;
	    ଯଦି ((bbits_val_&mask) != tmp) {
		  diff_flag = ସତ୍ୟ;
		  bbits_val_ = (bbits_val_ & ~mask) | tmp;
	    }

      } ଅନ୍ୟଥା ଯଦି (that.size_ <= BITS_PER_WORD) {

	      /* This vector is more than a word, but that vector is
		 still small. Write into the destination, possibly
		 spanning two destination words, depending on whether
		 the source vector spans a word transition. */
	    unsigned long dptr = adr / BITS_PER_WORD;
	    unsigned long doff = adr % BITS_PER_WORD;

	    unsigned long lmask = (1UL << doff) - 1;
	    unsigned long hshift = doff+that.size_;
	    unsigned long hmask;
	    ଯଦି (hshift >= BITS_PER_WORD)
		  hmask = -1UL;
	    ଅନ୍ୟଥା
		  hmask = (1UL << hshift) - 1UL;

	    unsigned long mask = hmask & ~lmask;
	    unsigned long tmp;

	    tmp = (that.abits_val_ << doff) & mask;
	    ଯଦି ((abits_ptr_[dptr] & mask) != tmp) {
		  diff_flag = ସତ୍ୟ;
		  abits_ptr_[dptr] = (abits_ptr_[dptr] & ~mask) | tmp;
	    }
	    tmp = (that.bbits_val_ << doff) & mask;
	    ଯଦି ((bbits_ptr_[dptr] & mask) != tmp) {
		  diff_flag = ସତ୍ୟ;
		  bbits_ptr_[dptr] = (bbits_ptr_[dptr] & ~mask) | tmp;
	    }

	    ଯଦି ((doff + that.size_) > BITS_PER_WORD) {
		  unsigned tail = doff + that.size_ - BITS_PER_WORD;
		  mask = (1UL << tail) - 1;

		  dptr += 1;
		  tmp = (that.abits_val_ >> (that.size_-tail)) & mask;
		  ଯଦି ((abits_ptr_[dptr] & mask) != tmp) {
			diff_flag = ସତ୍ୟ;
			abits_ptr_[dptr] = (abits_ptr_[dptr] & ~mask) | tmp;
		  }
		  tmp = (that.bbits_val_ >> (that.size_-tail)) & mask;
		  ଯଦି ((bbits_ptr_[dptr] & mask) != tmp) {
			diff_flag = ସତ୍ୟ;
			bbits_ptr_[dptr] = (bbits_ptr_[dptr] & ~mask) | tmp;
		  }
	    }

      } ଅନ୍ୟଥା ଯଦି (adr%BITS_PER_WORD == 0) {

	      /* In this case, both vectors are long, but the
		 destination is neatly aligned. That means all but the
		 last word can be simply copied with no masking. */

	    unsigned remain = that.size_;
	    unsigned sptr = 0;
	    unsigned dptr = adr / BITS_PER_WORD;
	    ଯେତେବେଳେ (remain >= BITS_PER_WORD) {
		  ଯଦି (abits_ptr_[dptr] != that.abits_ptr_[sptr]) {
			diff_flag = ସତ୍ୟ;
			abits_ptr_[dptr] = that.abits_ptr_[sptr];
		  }
		  ଯଦି (bbits_ptr_[dptr] != that.bbits_ptr_[sptr]) {
			diff_flag = ସତ୍ୟ;
			bbits_ptr_[dptr] = that.bbits_ptr_[sptr];
		  }
		  dptr += 1;
		  sptr += 1;
		  remain -= BITS_PER_WORD;
	    }

	    ଯଦି (remain > 0) {
		  unsigned long mask = (1UL << remain) - 1;
		  unsigned long tmp;

		  tmp = that.abits_ptr_[sptr] & mask;
		  ଯଦି ((abits_ptr_[dptr] & mask) != tmp) {
			diff_flag = ସତ୍ୟ;
			abits_ptr_[dptr] = (abits_ptr_[dptr] & ~mask) | tmp;
		  }
		  tmp = that.bbits_ptr_[sptr] & mask;
		  ଯଦି ((bbits_ptr_[dptr] & mask) != tmp) {
			diff_flag = ସତ୍ୟ;
			bbits_ptr_[dptr] = (bbits_ptr_[dptr] & ~mask) | tmp;
		  }
	    }

      } ଅନ୍ୟଥା {

	      /* We know that there are two long vectors, and we know
		 that the destination is definitely NOT aligned. */

	    unsigned remain = that.size_;
	    unsigned sptr = 0;
	    unsigned dptr = adr / BITS_PER_WORD;
	    unsigned doff = adr % BITS_PER_WORD;
	    unsigned long lmask = (1UL << doff) - 1;
	    unsigned ndoff = BITS_PER_WORD - doff;
	    ଯେତେବେଳେ (remain >= BITS_PER_WORD) {
		  unsigned long tmp;

		  tmp = (that.abits_ptr_[sptr] << doff) & ~lmask;
		  ଯଦି ((abits_ptr_[dptr] & ~lmask) != tmp) {
			diff_flag = ସତ୍ୟ;
			abits_ptr_[dptr] = (abits_ptr_[dptr] & lmask) | tmp;
		  }
		  tmp = (that.bbits_ptr_[sptr] << doff) & ~lmask;
		  ଯଦି ((bbits_ptr_[dptr] & ~lmask) != tmp) {
			diff_flag = ସତ୍ୟ;
			bbits_ptr_[dptr] = (bbits_ptr_[dptr] & lmask) | tmp;
		  }
		  dptr += 1;

		  tmp = (that.abits_ptr_[sptr] >> ndoff) & lmask;
		  ଯଦି ((abits_ptr_[dptr] & lmask) != tmp) {
			diff_flag = ସତ୍ୟ;
			abits_ptr_[dptr] = (abits_ptr_[dptr] & ~lmask) | tmp;
		  }
		  tmp = (that.bbits_ptr_[sptr] >> ndoff) & lmask;
		  ଯଦି ((bbits_ptr_[dptr] & lmask) != tmp) {
			diff_flag = ସତ୍ୟ;
			bbits_ptr_[dptr] = (bbits_ptr_[dptr] & ~lmask) | tmp;
		  }

		  remain -= BITS_PER_WORD;
		  sptr += 1;
	    }

	    ଯଦି (remain > 0) {
		  unsigned long hshift = doff+remain;
		  unsigned long hmask;
		  ଯଦି (hshift >= BITS_PER_WORD)
			hmask = -1UL;
		  ଅନ୍ୟଥା
			hmask = (1UL << (doff+remain)) - 1;

		  unsigned long mask = hmask & ~lmask;
		  unsigned long tmp;

		  tmp = (that.abits_ptr_[sptr] << doff) & mask;
		  ଯଦି ((abits_ptr_[dptr] & mask) != tmp) {
			diff_flag = ସତ୍ୟ;
			abits_ptr_[dptr] = (abits_ptr_[dptr] & ~mask) | tmp;
		  }
		  tmp = (that.bbits_ptr_[sptr] << doff) & mask;
		  ଯଦି ((bbits_ptr_[dptr] & mask) != tmp) {
			diff_flag = ସତ୍ୟ;
			bbits_ptr_[dptr] = (bbits_ptr_[dptr] & ~mask) | tmp;
		  }

		  ଯଦି ((doff + remain) > BITS_PER_WORD) {
			unsigned tail = doff + remain - BITS_PER_WORD;
			ଯଦି (tail >= BITS_PER_WORD)
			      mask = -1UL;
			ଅନ୍ୟଥା
			      mask = (1UL << tail) - 1;

			dptr += 1;

			tmp = (that.abits_ptr_[sptr] >> (remain-tail))&mask;
			ଯଦି ((abits_ptr_[dptr] & mask) != tmp) {
			      diff_flag = ସତ୍ୟ;
			      abits_ptr_[dptr] = (abits_ptr_[dptr] & ~mask) | tmp;
			}
			tmp = (that.bbits_ptr_[sptr] >> (remain-tail))&mask;
			ଯଦି ((bbits_ptr_[dptr] & mask) != tmp) {
			      diff_flag = ସତ୍ୟ;
			      bbits_ptr_[dptr] = (bbits_ptr_[dptr] & ~mask) | tmp;
			}
		  }
	    }
      }

      ଫେରନ୍ତୁ diff_flag;
}

/*
 * Add that vector to this vector. Do it in the Verilog way, which
 * means if we detect any X or Z bits, change the entire results to
 * all X.
 *
 * Assume both vectors are the same size.
 */
ଶୂନ୍ୟ vvp_vector4_t::add(ଅଚଳ vvp_vector4_t&that)
{
      ପ୍ରତିଜ୍ଞା(size_ == that.size_);

      ଯଦି (size_ < BITS_PER_WORD) {
	    unsigned long mask = ~(-1UL << size_);
	    ଯଦି ((bbits_val_|that.bbits_val_) & mask) {
		  abits_val_ |= mask;
		  bbits_val_ |= mask;
		  ଫେରନ୍ତୁ;
	    }

	    abits_val_ += that.abits_val_;
	    abits_val_ &= mask;
	    ଫେରନ୍ତୁ;
      }

      ଯଦି (size_ == BITS_PER_WORD) {
	    ଯଦି (bbits_val_ | that.bbits_val_) {
		  abits_val_ = WORD_X_ABITS;
		  bbits_val_ = WORD_X_BBITS;
	    } ଅନ୍ୟଥା {
		  abits_val_ += that.abits_val_;
	    }
	    ଫେରନ୍ତୁ;
      }

      ପୂର୍ଣ୍ଣସଂଖ୍ୟା cnt = size_ / BITS_PER_WORD;
      unsigned long carry = 0;
      ପାଇଁ (ପୂର୍ଣ୍ଣସଂଖ୍ୟା idx = 0 ; idx < cnt ; idx += 1) {
	    ଯଦି (bbits_ptr_[idx] | that.bbits_ptr_[idx])
		  ଯାଅ x_out;

	    abits_ptr_[idx] = add_with_carry(abits_ptr_[idx], that.abits_ptr_[idx], carry);
      }

      ଯଦି (unsigned tail = size_ % BITS_PER_WORD) {
	    unsigned long mask = ~( -1UL << tail );
	    ଯଦି ((bbits_ptr_[cnt] | that.bbits_ptr_[cnt])&mask)
		  ଯାଅ x_out;

	    abits_ptr_[cnt] = add_with_carry(abits_ptr_[cnt], that.abits_ptr_[cnt], carry);
	    abits_ptr_[cnt] &= mask;
      }

      ଫେରନ୍ତୁ;

 x_out:
      ପାଇଁ (ପୂର୍ଣ୍ଣସଂଖ୍ୟା idx = 0 ; idx < cnt ; idx += 1) {
	    abits_ptr_[idx] = WORD_X_ABITS;
	    bbits_ptr_[idx] = WORD_X_BBITS;
      }
      ଯଦି (unsigned tail = size_%BITS_PER_WORD) {
	    unsigned long mask = ~( -1UL << tail );
	    abits_ptr_[cnt] = WORD_X_ABITS&mask;
	    bbits_ptr_[cnt] = WORD_X_BBITS&mask;
      }
}

ଶୂନ୍ୟ vvp_vector4_t::sub(ଅଚଳ vvp_vector4_t&that)
{
      ପ୍ରତିଜ୍ଞା(size_ == that.size_);

      ଯଦି (size_ < BITS_PER_WORD) {
	    unsigned long mask = ~(-1UL << size_);
	    ଯଦି ((bbits_val_|that.bbits_val_) & mask) {
		  abits_val_ |= mask;
		  bbits_val_ |= mask;
		  ଫେରନ୍ତୁ;
	    }

	    abits_val_ -= that.abits_val_;
	    abits_val_ &= mask;
	    ଫେରନ୍ତୁ;
      }

      ଯଦି (size_ == BITS_PER_WORD) {
	    ଯଦି (bbits_val_ | that.bbits_val_) {
		  abits_val_ = WORD_X_ABITS;
		  bbits_val_ = WORD_X_BBITS;
	    } ଅନ୍ୟଥା {
		  abits_val_ -= that.abits_val_;
	    }
	    ଫେରନ୍ତୁ;
      }

      ପୂର୍ଣ୍ଣସଂଖ୍ୟା cnt = size_ / BITS_PER_WORD;
      unsigned long carry = 1;
      ପାଇଁ (ପୂର୍ଣ୍ଣସଂଖ୍ୟା idx = 0 ; idx < cnt ; idx += 1) {
	    ଯଦି (bbits_ptr_[idx] | that.bbits_ptr_[idx])
		  ଯାଅ x_out;

	    abits_ptr_[idx] = add_with_carry(abits_ptr_[idx], ~that.abits_ptr_[idx], carry);
      }

      ଯଦି (unsigned tail = size_ % BITS_PER_WORD) {
	    unsigned long mask = ~( -1UL << tail );
	    ଯଦି ((bbits_ptr_[cnt] | that.bbits_ptr_[cnt])&mask)
		  ଯାଅ x_out;

	    abits_ptr_[cnt] = add_with_carry(abits_ptr_[cnt], ~that.abits_ptr_[cnt], carry);
	    abits_ptr_[cnt] &= mask;
      }

      ଫେରନ୍ତୁ;

 x_out:
      ପାଇଁ (ପୂର୍ଣ୍ଣସଂଖ୍ୟା idx = 0 ; idx < cnt ; idx += 1) {
	    abits_ptr_[idx] = WORD_X_ABITS;
	    bbits_ptr_[idx] = WORD_X_BBITS;
      }
      ଯଦି (unsigned tail = size_%BITS_PER_WORD) {
	    unsigned long mask = ~( -1UL << tail );
	    abits_ptr_[cnt] = WORD_X_ABITS&mask;
	    bbits_ptr_[cnt] = WORD_X_BBITS&mask;
      }

}

ଶୂନ୍ୟ vvp_vector4_t::mov(unsigned dst, unsigned src, unsigned cnt)
{
      ପ୍ରତିଜ୍ଞା(dst+cnt <= size_);
      ପ୍ରତିଜ୍ଞା(src+cnt <= size_);

      ଯଦି (size_ <= BITS_PER_WORD) {
	    unsigned long vmask = (1UL << cnt) - 1;
	    unsigned long tmp;

	    tmp = (abits_val_ >> src) & vmask;
	    abits_val_ &= ~ (vmask << dst);
	    abits_val_ |= tmp << dst;

	    tmp = (bbits_val_ >> src) & vmask;
	    bbits_val_ &= ~ (vmask << dst);
	    bbits_val_ |= tmp << dst;

      } ଅନ୍ୟଥା {
	    unsigned sptr = src / BITS_PER_WORD;
	    unsigned dptr = dst / BITS_PER_WORD;
	    unsigned soff = src % BITS_PER_WORD;
	    unsigned doff = dst % BITS_PER_WORD;

	    ଯେତେବେଳେ (cnt > 0) {
		  unsigned trans = cnt;
		  ଯଦି ((soff+trans) > BITS_PER_WORD)
			trans = BITS_PER_WORD - soff;

		  ଯଦି ((doff+trans) > BITS_PER_WORD)
			trans = BITS_PER_WORD - doff;

		  ଯଦି (trans == BITS_PER_WORD) {
			  // Special case: the transfer count is
			  // exactly an entire word. For this to be
			  // true, it must also be true that the
			  // pointers are aligned. The work is easy,
			abits_ptr_[dptr] = abits_ptr_[sptr];
			bbits_ptr_[dptr] = bbits_ptr_[sptr];
			dptr += 1;
			sptr += 1;
			cnt -= BITS_PER_WORD;
			ଜାରି;
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
		  ଯଦି (soff >= BITS_PER_WORD) {
			soff = 0;
			sptr += 1;
		  }
		  doff += trans;
		  ଯଦି (doff >= BITS_PER_WORD) {
			doff = 0;
			dptr += 1;
		  }
	    }
      }
}

ଶୂନ୍ୟ vvp_vector4_t::mul(ଅଚଳ vvp_vector4_t&that)
{
      ପ୍ରତିଜ୍ଞା(size_ == that.size_);

      ଯଦି (size_ < BITS_PER_WORD) {
	    unsigned long mask = ~(-1UL << size_);
	    ଯଦି ((bbits_val_|that.bbits_val_) & mask) {
		  abits_val_ |= mask;
		  bbits_val_ |= mask;
		  ଫେରନ୍ତୁ;
	    }

	    abits_val_ *= that.abits_val_;
	    abits_val_ &= mask;
	    ଫେରନ୍ତୁ;
      }

      ଯଦି (size_ == BITS_PER_WORD) {
	    ଯଦି (bbits_val_ || that.bbits_val_) {
		  abits_val_ = WORD_X_ABITS;
		  bbits_val_ = WORD_X_BBITS;
	    } ଅନ୍ୟଥା {
		  abits_val_ *= that.abits_val_;
	    }
	    ଫେରନ୍ତୁ;
      }

      ଅଚଳ ପୂର୍ଣ୍ଣସଂଖ୍ୟା cnt = (size_+BITS_PER_WORD-1) / BITS_PER_WORD;

      unsigned long mask;
      ଯଦି (unsigned tail = size_%BITS_PER_WORD) {
	    mask = ~( -1UL << tail );
      } ଅନ୍ୟଥା {
	    mask = ~0UL;
      }

	// Check for any XZ values ahead of time in a first pass. If
	// we find any, then force the entire result to be X and be
	// done.
      ପାଇଁ (ପୂର୍ଣ୍ଣସଂଖ୍ୟା idx = 0 ; idx < cnt ; idx += 1) {
	    unsigned long lval = bbits_ptr_[idx];
	    unsigned long rval = that.bbits_ptr_[idx];
	    ଯଦି (idx == (cnt-1)) {
		  lval &= mask;
		  rval &= mask;
	    }
	    ଯଦି (lval || rval) {
		  ପାଇଁ (ପୂର୍ଣ୍ଣସଂଖ୍ୟା xdx = 0 ; xdx < cnt-1 ; xdx += 1) {
			abits_ptr_[xdx] = WORD_X_ABITS;
			bbits_ptr_[xdx] = WORD_X_BBITS;
		  }
		  abits_ptr_[cnt-1] = WORD_X_ABITS & mask;
		  bbits_ptr_[cnt-1] = WORD_X_BBITS & mask;
		  ଫେରନ୍ତୁ;
	    }
      }

	// Calculate the result into a res array. We need to keep is
	// separate from the "this" array because we are making
	// multiple passes.
      unsigned long*res = ନୂତନ unsigned long[cnt];
      ପାଇଁ (ପୂର୍ଣ୍ଣସଂଖ୍ୟା idx = 0 ; idx < cnt ; idx += 1)
	    res[idx] = 0;

      ପାଇଁ (ପୂର୍ଣ୍ଣସଂଖ୍ୟା mul_a = 0 ; mul_a < cnt ; mul_a += 1) {
	    unsigned long lval = abits_ptr_[mul_a];
	    ଯଦି (mul_a == (cnt-1))
		  lval &= mask;

	    ପାଇଁ (ପୂର୍ଣ୍ଣସଂଖ୍ୟା mul_b = 0 ; mul_b < (cnt-mul_a) ; mul_b += 1) {
		  unsigned long rval = that.abits_ptr_[mul_b];
		  ଯଦି (mul_b == (cnt-1))
			rval &= mask;

		  unsigned long sum;
		  unsigned long tmp = multiply_with_carry(lval, rval, sum);
		  ପୂର୍ଣ୍ଣସଂଖ୍ୟା base = mul_a + mul_b;
		  unsigned long carry = 0;
		  res[base] = add_with_carry(res[base], tmp, carry);
		  ପାଇଁ (ପୂର୍ଣ୍ଣସଂଖ୍ୟା add_idx = base+1 ; add_idx < cnt ; add_idx += 1) {
			res[add_idx] = add_with_carry(res[add_idx], sum, carry);
			sum = 0;
		  }
	    }
      }

	// Replace the "this" value with the calculated result. We
	// know a-priori that the bbits are zero and unchanged.
      res[cnt-1] &= mask;
      ପାଇଁ (ପୂର୍ଣ୍ଣସଂଖ୍ୟା idx = 0 ; idx < cnt ; idx += 1)
	    abits_ptr_[idx] = res[idx];

      delete[]res;
      ଫେରନ୍ତୁ;


}

ତାର୍କିକ vvp_vector4_t::eeq(ଅଚଳ vvp_vector4_t&that) ଅଚଳ
{
      ଯଦି (size_ != that.size_)
	    ଫେରନ୍ତୁ ମିଥ୍ୟା;

      ଯଦି (size_ < BITS_PER_WORD) {
	    unsigned long mask = (1UL << size_) - 1;
	    ଫେରନ୍ତୁ (abits_val_&mask) == (that.abits_val_&mask)
		  && (bbits_val_&mask) == (that.bbits_val_&mask);
      }

      ଯଦି (size_ == BITS_PER_WORD) {
	    ଫେରନ୍ତୁ (abits_val_ == that.abits_val_)
		  && (bbits_val_ == that.bbits_val_);
      }

      unsigned ଶବ୍ଦଗୁଡ଼ିକ = size_ / BITS_PER_WORD;
      ପାଇଁ (unsigned idx = 0 ;  idx < ଶବ୍ଦଗୁଡ଼ିକ ;  idx += 1) {
	    ଯଦି (abits_ptr_[idx] != that.abits_ptr_[idx])
		  ଫେରନ୍ତୁ ମିଥ୍ୟା;
	    ଯଦି (bbits_ptr_[idx] != that.bbits_ptr_[idx])
		  ଫେରନ୍ତୁ ମିଥ୍ୟା;
      }

      unsigned long mask = size_%BITS_PER_WORD;
      ଯଦି (mask > 0) {
	    mask = (1UL << mask) - 1;
	    ଫେରନ୍ତୁ (abits_ptr_[ଶବ୍ଦଗୁଡ଼ିକ]&mask) == (that.abits_ptr_[ଶବ୍ଦଗୁଡ଼ିକ]&mask)
		  && (bbits_ptr_[ଶବ୍ଦଗୁଡ଼ିକ]&mask) == (that.bbits_ptr_[ଶବ୍ଦଗୁଡ଼ିକ]&mask);
      }

      ଫେରନ୍ତୁ ସତ୍ୟ;
}

ତାର୍କିକ vvp_vector4_t::eq_xz(ଅଚଳ vvp_vector4_t&that) ଅଚଳ
{
      ଯଦି (size_ != that.size_)
	    ଫେରନ୍ତୁ ମିଥ୍ୟା;

      ଯଦି (size_ < BITS_PER_WORD) {
	    unsigned long mask = (1UL << size_) - 1;
	    ଫେରନ୍ତୁ ((abits_val_|bbits_val_)&mask) == ((that.abits_val_|that.bbits_val_)&mask)
		  && (bbits_val_&mask) == (that.bbits_val_&mask);
      }

      ଯଦି (size_ == BITS_PER_WORD) {
	    ଫେରନ୍ତୁ ((abits_val_|bbits_val_) == (that.abits_val_|that.bbits_val_))
		  && (bbits_val_ == that.bbits_val_);
      }

      unsigned ଶବ୍ଦଗୁଡ଼ିକ = size_ / BITS_PER_WORD;
      ପାଇଁ (unsigned idx = 0 ;  idx < ଶବ୍ଦଗୁଡ଼ିକ ;  idx += 1) {
	    ଯଦି ((abits_ptr_[idx]|bbits_ptr_[idx]) != (that.abits_ptr_[idx]|that.bbits_ptr_[idx]))
		  ଫେରନ୍ତୁ ମିଥ୍ୟା;
	    ଯଦି (bbits_ptr_[idx] != that.bbits_ptr_[idx])
		  ଫେରନ୍ତୁ ମିଥ୍ୟା;
      }

      unsigned long mask = size_%BITS_PER_WORD;
      ଯଦି (mask > 0) {
	    mask = (1UL << mask) - 1;
	    ଫେରନ୍ତୁ ((abits_ptr_[ଶବ୍ଦଗୁଡ଼ିକ]|bbits_ptr_[ଶବ୍ଦଗୁଡ଼ିକ])&mask) == ((that.abits_ptr_[ଶବ୍ଦଗୁଡ଼ିକ]|that.bbits_ptr_[ଶବ୍ଦଗୁଡ଼ିକ])&mask)
		  && (bbits_ptr_[ଶବ୍ଦଗୁଡ଼ିକ]&mask) == (that.bbits_ptr_[ଶବ୍ଦଗୁଡ଼ିକ]&mask);
      }

      ଫେରନ୍ତୁ ସତ୍ୟ;
}

ତାର୍କିକ vvp_vector4_t::has_xz() ଅଚଳ
{
      ଯଦି (size_ < BITS_PER_WORD) {
	    unsigned long mask = -1UL >> (BITS_PER_WORD - size_);
	    ଫେରନ୍ତୁ bbits_val_&mask;
      }

      ଯଦି (size_ == BITS_PER_WORD) {
	    ଫେରନ୍ତୁ bbits_val_;
      }

      unsigned ଶବ୍ଦଗୁଡ଼ିକ = size_ / BITS_PER_WORD;
      ପାଇଁ (unsigned idx = 0 ; idx < ଶବ୍ଦଗୁଡ଼ିକ ; idx += 1) {
	    ଯଦି (bbits_ptr_[idx])
		  ଫେରନ୍ତୁ ସତ୍ୟ;
      }

      unsigned long mask = size_%BITS_PER_WORD;
      ଯଦି (mask > 0) {
	    mask = -1UL >> (BITS_PER_WORD - mask);
	    ଫେରନ୍ତୁ bbits_ptr_[ଶବ୍ଦଗୁଡ଼ିକ]&mask;
      }

      ଫେରନ୍ତୁ ମିଥ୍ୟା;
}

ଶୂନ୍ୟ vvp_vector4_t::change_z2x()
{
	// This method relies on the fact that both BIT4_X and BIT4_Z
	// have the bbit set in the vector4 encoding, and also that
	// the BIT4_X has abit set in the vector4 encoding. By simply
	// or-ing the bbit into the abit, BIT4_X and BIT4_Z both
	// become BIT4_X.

      ଯଦି (size_ <= BITS_PER_WORD) {
	    abits_val_ |= bbits_val_;
      } ଅନ୍ୟଥା {
	    unsigned ଶବ୍ଦଗୁଡ଼ିକ = (size_+BITS_PER_WORD-1) / BITS_PER_WORD;
	    ପାଇଁ (unsigned idx = 0 ;  idx < ଶବ୍ଦଗୁଡ଼ିକ ;  idx += 1)
		  abits_ptr_[idx] |= bbits_ptr_[idx];
      }
}

ଶୂନ୍ୟ vvp_vector4_t::fill_bits(vvp_bit4_t bit)
{
	/* note: this relies on the bit encoding for the vvp_bit4_t. */
      ସ୍ଥିର ଅଚଳ unsigned long init_atable[4] = {
	    WORD_0_ABITS,
	    WORD_1_ABITS,
	    WORD_Z_ABITS,
	    WORD_X_ABITS };
      ସ୍ଥିର ଅଚଳ unsigned long init_btable[4] = {
	    WORD_0_BBITS,
	    WORD_1_BBITS,
	    WORD_Z_BBITS,
	    WORD_X_BBITS };

      ଯଦି (size_ <= BITS_PER_WORD) {
	    abits_val_ = init_atable[bit];
	    bbits_val_ = init_btable[bit];
      } ଅନ୍ୟଥା {
	    unsigned ଶବ୍ଦଗୁଡ଼ିକ = (size_ + BITS_PER_WORD - 1) / BITS_PER_WORD;
	    ପାଇଁ (unsigned idx = 0; idx < ଶବ୍ଦଗୁଡ଼ିକ; idx += 1) {
		  abits_ptr_[idx] = init_atable[bit];
		  bbits_ptr_[idx] = init_btable[bit];
	    }
      }
}

ଅକ୍ଷର* vvp_vector4_t::as_string(ଅକ୍ଷର*buf, size_t buf_len) ଅଚଳ
{
      ଅକ୍ଷର*res = buf;
      *buf++ = 'C';
      *buf++ = '4';
      *buf++ = '<';
      buf_len -= 3;

      ପାଇଁ (unsigned idx = 0 ;  idx < size() && buf_len >= 2 ;  idx += 1) {
	    ବାଛ (value(size()-idx-1)) {
		ସ୍ଥିତି BIT4_0:
		  *buf++ = '0';
		  ଭାଙ୍ଗ;
		ସ୍ଥିତି BIT4_1:
		  *buf++ = '1';
		  ଭାଙ୍ଗ;
		ସ୍ଥିତି BIT4_X:
		  *buf++ = 'x';
		  ଭାଙ୍ଗ;
		ସ୍ଥିତି BIT4_Z:
		  *buf++ = 'z';
	    }
	    buf_len -= 1;
      }

      *buf++ = '>';
      *buf++ = 0;
      ଫେରନ୍ତୁ res;
}

ଶୂନ୍ୟ vvp_vector4_t::invert()
{
      ଯଦି (size_ <= BITS_PER_WORD) {
	    unsigned long mask = (size_<BITS_PER_WORD)? (1UL<<size_)-1UL : -1UL;
	    abits_val_ = mask & ~abits_val_;
	    abits_val_ |= bbits_val_;
      } ଅନ୍ୟଥା {
	    unsigned remaining = size_;
	    unsigned idx = 0;
	    ଯେତେବେଳେ (remaining >= BITS_PER_WORD) {
		  abits_ptr_[idx] = ~abits_ptr_[idx];
		  abits_ptr_[idx] |= bbits_ptr_[idx];
		  idx += 1;
		  remaining -= BITS_PER_WORD;
	    }
	    ଯଦି (remaining > 0) {
		  unsigned long mask = (1UL<<remaining) - 1UL;
		  abits_ptr_[idx] = mask & ~abits_ptr_[idx];
		  abits_ptr_[idx] |= bbits_ptr_[idx];
	    }
      }
}

#define BIT_MASK(n) ((n) ? ((~0UL) >> (BITS_PER_WORD - (n))) : ~0UL)

vvp_bit4_t vvp_vector4_t::reduce_or() ଅଚଳ
{
      unsigned long mask = BIT_MASK(size_ % BITS_PER_WORD);
      vvp_bit4_t res = BIT4_0;

      ଯଦି (size_ <= BITS_PER_WORD) {
	    ଯଦି ((abits_val_ & ~bbits_val_ & mask) != 0UL)
		  ଫେରନ୍ତୁ BIT4_1;
	    ଯଦି ((bbits_val_ & mask) != 0UL)
		  ଫେରନ୍ତୁ BIT4_X;
      } ଅନ୍ୟଥା {
	    unsigned ଶବ୍ଦଗୁଡ଼ିକ = (size_ + BITS_PER_WORD - 1) / BITS_PER_WORD;
	    unsigned idx;
	    ପାଇଁ (idx = 0; idx < ଶବ୍ଦଗୁଡ଼ିକ - 1; idx += 1) {
		  ଯଦି ((abits_ptr_[idx] & ~bbits_ptr_[idx]) != 0UL)
			ଫେରନ୍ତୁ BIT4_1;
		  ଯଦି (bbits_ptr_[idx] != 0UL)
			res = BIT4_X;
	    }
	    ଯଦି ((abits_ptr_[idx] & ~bbits_ptr_[idx] & mask) != 0UL)
		  ଫେରନ୍ତୁ BIT4_1;
	    ଯଦି ((bbits_ptr_[idx] & mask) != 0UL)
		  res = BIT4_X;
      }

      ଫେରନ୍ତୁ res;
}

vvp_bit4_t vvp_vector4_t::reduce_and() ଅଚଳ
{
      unsigned long mask = BIT_MASK(size_ % BITS_PER_WORD);
      vvp_bit4_t res = BIT4_1;

      ଯଦି (size_ <= BITS_PER_WORD) {
	    ଯଦି ((abits_val_ | bbits_val_ | ~mask) != ~0UL)
		  ଫେରନ୍ତୁ BIT4_0;
	    ଯଦି ((bbits_val_ & mask) != 0UL)
		  ଫେରନ୍ତୁ BIT4_X;
	} ଅନ୍ୟଥା {
	    unsigned ଶବ୍ଦଗୁଡ଼ିକ = (size_ + BITS_PER_WORD - 1) / BITS_PER_WORD;
	    unsigned idx;
	    ପାଇଁ (idx = 0; idx < ଶବ୍ଦଗୁଡ଼ିକ - 1; idx += 1) {
		  ଯଦି ((abits_ptr_[idx] | bbits_ptr_[idx]) != ~0UL)
			ଫେରନ୍ତୁ BIT4_0;
		  ଯଦି (bbits_ptr_[idx] != 0UL)
			res = BIT4_X;
	    }
	    ଯଦି ((abits_ptr_[idx] | bbits_ptr_[idx] | ~mask) != ~0UL)
		  ଫେରନ୍ତୁ BIT4_0;
	    ଯଦି ((bbits_ptr_[idx] & mask) != 0UL)
		  res = BIT4_X;
      }

      ଫେରନ୍ତୁ res;
}

ସ୍ଥିର unsigned long parity(unsigned long val)
{
#ଯଦି defined(__GNUC__)
      // The compiler builtin can use target-specific CPU instructions.
      ଫେରନ୍ତୁ __builtin_parityl(val);
#ଅନ୍ୟଥା
#ଯଦି ULONG_MAX > 0xffffffffUL
	val ^= val >> 32;
#endif
	val ^= val >> 16;
	val ^= val >> 8;
	val ^= val >> 4;

      ଫେରନ୍ତୁ (0x6996 >> (val & 0xf)) & 1;
#endif
}

vvp_bit4_t vvp_vector4_t::reduce_xor() ଅଚଳ
{
      unsigned long mask = BIT_MASK(size_ % BITS_PER_WORD);

      ଯଦି (size_ <= BITS_PER_WORD) {
	    ଯଦି ((bbits_val_ & mask) != 0UL)
		  ଫେରନ୍ତୁ BIT4_X;
	    ଫେରନ୍ତୁ parity(abits_val_ & mask) ? BIT4_1 : BIT4_0;
      } ଅନ୍ୟଥା {
	    unsigned ଶବ୍ଦଗୁଡ଼ିକ = (size_ + BITS_PER_WORD - 1) / BITS_PER_WORD;
	    unsigned long val_a = 0UL;
	    unsigned idx;
	    ପାଇଁ (idx = 0; idx < ଶବ୍ଦଗୁଡ଼ିକ - 1; idx += 1) {
		  ଯଦି (bbits_ptr_[idx] != 0UL)
			ଫେରନ୍ତୁ BIT4_X;
		  val_a ^= abits_ptr_[idx];
	    }
	    ଯଦି ((bbits_ptr_[idx] & mask) != 0UL)
		  ଫେରନ୍ତୁ BIT4_X;
	    val_a ^= abits_ptr_[idx] & mask;
	    ଫେରନ୍ତୁ parity(val_a) ? BIT4_1 : BIT4_0;
      }
}

vvp_vector4_t& vvp_vector4_t::operator &= (ଅଚଳ vvp_vector4_t&that)
{
	// The truth table is:
	//     00 01 11 10
	//  00 00 00 00 00
	//  01 00 01 11 11
	//  11 00 11 11 11
	//  10 00 11 11 11
      ଯଦି (size_ <= BITS_PER_WORD) {
	    unsigned long tmp1 = abits_val_ | bbits_val_;
	    unsigned long tmp2 = that.abits_val_ | that.bbits_val_;
	    abits_val_ = tmp1 & tmp2;
	    bbits_val_ = (tmp1 & that.bbits_val_) | (tmp2 & bbits_val_);
      } ଅନ୍ୟଥା {
	    unsigned ଶବ୍ଦଗୁଡ଼ିକ = (size_ + BITS_PER_WORD - 1) / BITS_PER_WORD;
	    ପାଇଁ (unsigned idx = 0; idx < ଶବ୍ଦଗୁଡ଼ିକ ; idx += 1) {
		  unsigned long tmp1 = abits_ptr_[idx] | bbits_ptr_[idx];
		  unsigned long tmp2 = that.abits_ptr_[idx] |
		                       that.bbits_ptr_[idx];
		  abits_ptr_[idx] = tmp1 & tmp2;
		  bbits_ptr_[idx] = (tmp1 & that.bbits_ptr_[idx]) |
		                    (tmp2 & bbits_ptr_[idx]);
	    }
      }

      ଫେରନ୍ତୁ *ଏହି;
}

vvp_vector4_t& vvp_vector4_t::operator |= (ଅଚଳ vvp_vector4_t&that)
{
	// The truth table is:
	//     00 01 11 10
	//  00 00 01 11 11
	//  01 01 01 01 01
	//  11 11 01 11 11
	//  10 11 01 11 11
      ଯଦି (size_ <= BITS_PER_WORD) {
	    unsigned long tmp = abits_val_ | bbits_val_ |
	                        that.abits_val_ | that.bbits_val_;
	    bbits_val_ = ((~abits_val_ | bbits_val_) & that.bbits_val_) |
	                 ((~that.abits_val_ | that.bbits_val_) & bbits_val_);
	    abits_val_ = tmp;

      } ଅନ୍ୟଥା {
	    unsigned ଶବ୍ଦଗୁଡ଼ିକ = (size_ + BITS_PER_WORD - 1) / BITS_PER_WORD;
	    ପାଇଁ (unsigned idx = 0; idx < ଶବ୍ଦଗୁଡ଼ିକ ; idx += 1) {
		  unsigned long tmp = abits_ptr_[idx] | bbits_ptr_[idx] |
	                        that.abits_ptr_[idx] | that.bbits_ptr_[idx];
		  bbits_ptr_[idx] = ((~abits_ptr_[idx] | bbits_ptr_[idx]) &
		                     that.bbits_ptr_[idx]) |
		                    ((~that.abits_ptr_[idx] |
		                      that.bbits_ptr_[idx]) & bbits_ptr_[idx]);
		  abits_ptr_[idx] = tmp;
	    }
      }

      ଫେରନ୍ତୁ *ଏହି;
}

vvp_vector4_t& vvp_vector4_t::operator ^= (ଅଚଳ vvp_vector4_t&that)
{
	// The truth table is:
	//     00 01 11 10
	//  00 00 01 11 11
	//  01 01 00 11 11
	//  11 11 11 11 11
	//  10 11 11 11 11
      ଯଦି (size_ <= BITS_PER_WORD) {
	    unsigned long bval = bbits_val_ | that.bbits_val_;
	    bbits_val_ = bval;
	    abits_val_ = (abits_val_ ^ that.abits_val_) | bval;
      } ଅନ୍ୟଥା {
	    unsigned ଶବ୍ଦଗୁଡ଼ିକ = (size_ + BITS_PER_WORD - 1) / BITS_PER_WORD;
	    ପାଇଁ (unsigned idx = 0; idx < ଶବ୍ଦଗୁଡ଼ିକ ; idx += 1) {
		  unsigned long bval = bbits_ptr_[idx] | that.bbits_ptr_[idx];
		  bbits_ptr_[idx] = bval;
		  abits_ptr_[idx] = (abits_ptr_[idx] ^ that.abits_ptr_[idx]) | bval;
	    }
      }

      ଫେରନ୍ତୁ *ଏହି;
}

/*
* Add an integer to the vvp_vector4_t in place, bit by bit so that
* there is no size limitations.
*/
vvp_vector4_t& vvp_vector4_t::operator += (int64_t that)
{
      vvp_bit4_t carry = BIT4_0;
      unsigned idx;

      ଯଦି (has_xz()) {
	    vvp_vector4_t xxx (size(), BIT4_X);
	    *ଏହି = xxx;
	    ଫେରନ୍ତୁ *ଏହି;
      }

      ପାଇଁ (idx = 0 ; idx < size() ; idx += 1) {
	    ଯଦି (that == 0 && carry==BIT4_0)
		  ଭାଙ୍ଗ;

	    vvp_bit4_t that_bit = (that&1)? BIT4_1 : BIT4_0;
	    that >>= 1;

	    ଯଦି (that_bit==BIT4_0 && carry==BIT4_0)
		  ଜାରି;

	    vvp_bit4_t bit = value(idx);
	    bit = add_with_carry(bit, that_bit, carry);

	    set_bit(idx, bit);
      }

      ଫେରନ୍ତୁ *ଏହି;
}

ostream& operator<< (ostream&out, ଅଚଳ vvp_vector4_t&that)
{
      out << that.size() << "'b";
      ପାଇଁ (unsigned idx = 0 ;  idx < that.size() ;  idx += 1)
	    out << that.value(that.size()-idx-1);
      ଫେରନ୍ତୁ out;
}

template <ଶ୍ରେଣୀ INT>ତାର୍କିକ vector4_to_ମୂଲ୍ୟ(ଅଚଳ vvp_vector4_t&vec, INT&val,
					  ତାର୍କିକ is_signed, ତାର୍କିକ is_arithmetic)
{
      INT res = 0;
      INT msk = 1;
      ତାର୍କିକ rc_flag = ସତ୍ୟ;

      unsigned size = vec.size();
      ଯଦି (size > 8*ଆକାର(val)) size = 8*ଆକାର(val);
      ପାଇଁ (unsigned idx = 0 ;  idx < size ;  idx += 1) {
	    ବାଛ (vec.value(idx)) {
		ସ୍ଥିତି BIT4_0:
		  ଭାଙ୍ଗ;
		ସ୍ଥିତି BIT4_1:
		  res |= msk;
		  ଭାଙ୍ଗ;
		ପୂର୍ବନିର୍ଦ୍ଧାରିତ:
		  ଯଦି (is_arithmetic)
			ଫେରନ୍ତୁ ମିଥ୍ୟା;
		  ଅନ୍ୟଥା
			rc_flag = ମିଥ୍ୟା;
	    }

	    msk <<= 1;
      }

      ଯଦି (is_signed && vec.value(vec.size()-1) == BIT4_1) {
	    ଯଦି (vec.size() < 8*ଆକାର(val))
		  res |= static_cast<INT>(-1ULL << vec.size());
      }

      val = res;
      ଫେରନ୍ତୁ rc_flag;
}

template ତାର୍କିକ vector4_to_ମୂଲ୍ୟ(ଅଚଳ vvp_vector4_t&vec, int8_t&val,
			       ତାର୍କିକ is_signed, ତାର୍କିକ is_arithmetic);
template ତାର୍କିକ vector4_to_ମୂଲ୍ୟ(ଅଚଳ vvp_vector4_t&vec, int16_t&val,
			       ତାର୍କିକ is_signed, ତାର୍କିକ is_arithmetic);
template ତାର୍କିକ vector4_to_ମୂଲ୍ୟ(ଅଚଳ vvp_vector4_t&vec, int32_t&val,
			       ତାର୍କିକ is_signed, ତାର୍କିକ is_arithmetic);
template ତାର୍କିକ vector4_to_ମୂଲ୍ୟ(ଅଚଳ vvp_vector4_t&vec, int64_t&val,
			       ତାର୍କିକ is_signed, ତାର୍କିକ is_arithmetic);
template ତାର୍କିକ vector4_to_ମୂଲ୍ୟ(ଅଚଳ vvp_vector4_t&vec, uint8_t&val,
			       ତାର୍କିକ is_signed, ତାର୍କିକ is_arithmetic);
template ତାର୍କିକ vector4_to_ମୂଲ୍ୟ(ଅଚଳ vvp_vector4_t&vec, uint16_t&val,
			       ତାର୍କିକ is_signed, ତାର୍କିକ is_arithmetic);
template ତାର୍କିକ vector4_to_ମୂଲ୍ୟ(ଅଚଳ vvp_vector4_t&vec, uint32_t&val,
			       ତାର୍କିକ is_signed, ତାର୍କିକ is_arithmetic);
template ତାର୍କିକ vector4_to_ମୂଲ୍ୟ(ଅଚଳ vvp_vector4_t&vec, uint64_t&val,
			       ତାର୍କିକ is_signed, ତାର୍କିକ is_arithmetic);

template <ଶ୍ରେଣୀ T> ତାର୍କିକ vector4_to_ମୂଲ୍ୟ(ଅଚଳ vvp_vector4_t&vec,
                                         ତାର୍କିକ&overflow_flag, T&val)
{
      T res = 0;
      T msk = 1;

      overflow_flag = ମିଥ୍ୟା;
      unsigned size = vec.size();
      ପାଇଁ (unsigned idx = 0 ;  idx < size ;  idx += 1) {
	    ବାଛ (vec.value(idx)) {
		ସ୍ଥିତି BIT4_0:
		  ଭାଙ୍ଗ;
		ସ୍ଥିତି BIT4_1:
		  ଯଦି (msk == 0)
			overflow_flag = ସତ୍ୟ;
		  ଅନ୍ୟଥା
			res |= msk;
		  ଭାଙ୍ଗ;
		ପୂର୍ବନିର୍ଦ୍ଧାରିତ:
		  ଫେରନ୍ତୁ ମିଥ୍ୟା;
	    }

	    msk <<= static_cast<T>(1);
      }

      val = res;
      ଫେରନ୍ତୁ ସତ୍ୟ;
}

template ତାର୍କିକ vector4_to_ମୂଲ୍ୟ(ଅଚଳ vvp_vector4_t&vec, ତାର୍କିକ&overflow_flag,
                               unsigned long&val);
#ifndef UL_AND_TIME64_SAME
template ତାର୍କିକ vector4_to_ମୂଲ୍ୟ(ଅଚଳ vvp_vector4_t&vec, ତାର୍କିକ&overflow_flag,
                               vvp_time64_t&val);
#endif

ତାର୍କିକ vector4_to_ମୂଲ୍ୟ(ଅଚଳ vvp_vector4_t&vec, ଦ୍ୱିଗୁଣ&val, ତାର୍କିକ signed_flag)
{

      ଯଦି (vec.size() == 0) {
	    val = 0.0;
	    ଫେରନ୍ତୁ ସତ୍ୟ;
      }

      ତାର୍କିକ flag = ସତ୍ୟ;

      ଯଦି (vec.value(vec.size()-1) != BIT4_1) {
	    signed_flag = ମିଥ୍ୟା;
      }

      ଦ୍ୱିଗୁଣ res = 0.0;
      ଯଦି (signed_flag) {
	    vvp_bit4_t carry = BIT4_1;
	    ପାଇଁ (unsigned idx = 0 ;  idx < vec.size() ;  idx += 1) {
		  vvp_bit4_t a = ~vec.value(idx);
		  vvp_bit4_t x = add_with_carry(a, BIT4_0, carry);
		  ବାଛ (x) {
		      ସ୍ଥିତି BIT4_0:
			ଭାଙ୍ଗ;
		      ସ୍ଥିତି BIT4_1:
			res += pow(2.0, (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)idx);
			ଭାଙ୍ଗ;
		      ପୂର୍ବନିର୍ଦ୍ଧାରିତ:
			flag = ମିଥ୍ୟା;
		  }
	    }
	    res *= -1.0;
      } ଅନ୍ୟଥା {
	    ପାଇଁ (unsigned idx = 0 ;  idx < vec.size() ;  idx += 1) {
		  ବାଛ (vec.value(idx)) {
		      ସ୍ଥିତି BIT4_0:
			ଭାଙ୍ଗ;
		      ସ୍ଥିତି BIT4_1:
			res += pow(2.0, (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)idx);
			ଭାଙ୍ଗ;
		      ପୂର୍ବନିର୍ଦ୍ଧାରିତ:
			flag = ମିଥ୍ୟା;
		  }
	    }
      }
      val = res;
      ଫେରନ୍ତୁ flag;
}

ତାର୍କିକ vector2_to_ମୂଲ୍ୟ(ଅଚଳ vvp_vector2_t&a, int32_t&val, ତାର୍କିକ is_signed)
{
      val = 0;
      unsigned idx;
      uint32_t mask;
      ପାଇଁ (idx = 0, mask = 1 ; idx < a.size() && idx < 32 ; idx += 1, mask <<= 1) {
	    ଯଦି (a.value(idx)) val |= mask;
      }

      ଯଦି (is_signed && a.size() < 32 && a.value(a.size()-1)) {
	    mask = -1;
	    mask <<= a.size();
	    val |= mask;
      }

      ଫେରନ୍ତୁ a.size() <= 32;
}

vvp_vector4array_t::vvp_vector4array_t(unsigned width__, unsigned ଶବ୍ଦଗୁଡ଼ିକ_2)
: width_(width__), ଶବ୍ଦଗୁଡ଼ିକ_3(ଶବ୍ଦଗୁଡ଼ିକ_2)
{
}

vvp_vector4array_t::~vvp_vector4array_t()
{
}

ଶୂନ୍ୟ vvp_vector4array_t::set_word_(v4cell*cell, ଅଚଳ vvp_vector4_t&that)
{
      ପ୍ରତିଜ୍ଞା(that.size_ == width_);

      ଯଦି (width_ <= vvp_vector4_t::BITS_PER_WORD) {
	    cell->abits_val_ = that.abits_val_;
	    cell->bbits_val_ = that.bbits_val_;
	    ଫେରନ୍ତୁ;
      }

      unsigned cnt = (width_ + vvp_vector4_t::BITS_PER_WORD-1)/vvp_vector4_t::BITS_PER_WORD;

      ଯଦି (cell->abits_ptr_ == 0) {
	    cell->abits_ptr_ = ନୂତନ unsigned long[2*cnt];
	    cell->bbits_ptr_ = cell->abits_ptr_ + cnt;
      }

      ପାଇଁ (unsigned idx = 0 ; idx < cnt ; idx += 1)
	    cell->abits_ptr_[idx] = that.abits_ptr_[idx];
      ପାଇଁ (unsigned idx = 0 ; idx < cnt ; idx += 1)
	    cell->bbits_ptr_[idx] = that.bbits_ptr_[idx];
}

vvp_vector4_t vvp_vector4array_t::get_word_(v4cell*cell) ଅଚଳ
{
      ଯଦି (width_ <= vvp_vector4_t::BITS_PER_WORD) {
	    vvp_vector4_t res;
	    res.size_ = width_;
	    res.abits_val_ = cell->abits_val_;
	    res.bbits_val_ = cell->bbits_val_;
	    ଫେରନ୍ତୁ res;
      }

      vvp_vector4_t res (width_, BIT4_X);
      ଯଦି (cell->abits_ptr_ == 0)
	    ଫେରନ୍ତୁ res;

      unsigned cnt = (width_ + vvp_vector4_t::BITS_PER_WORD-1)/vvp_vector4_t::BITS_PER_WORD;

      ପାଇଁ (unsigned idx = 0 ; idx < cnt ; idx += 1)
	    res.abits_ptr_[idx] = cell->abits_ptr_[idx];
      ପାଇଁ (unsigned idx = 0 ; idx < cnt ; idx += 1)
	    res.bbits_ptr_[idx] = cell->bbits_ptr_[idx];

      ଫେରନ୍ତୁ res;
}

vvp_vector4array_sa::vvp_vector4array_sa(unsigned width__, unsigned ଶବ୍ଦଗୁଡ଼ିକ_2)
: vvp_vector4array_t(width__, ଶବ୍ଦଗୁଡ଼ିକ_2)
{
      array_ = ନୂତନ v4cell[ଶବ୍ଦଗୁଡ଼ିକ_3];

      ଯଦି (width_ <= vvp_vector4_t::BITS_PER_WORD) {
	    ପାଇଁ (unsigned idx = 0 ; idx < ଶବ୍ଦଗୁଡ଼ିକ_3 ; idx += 1) {
		  array_[idx].abits_val_ = vvp_vector4_t::WORD_X_ABITS;
		  array_[idx].bbits_val_ = vvp_vector4_t::WORD_X_BBITS;
	    }
      } ଅନ୍ୟଥା {
	    ପାଇଁ (unsigned idx = 0 ; idx < ଶବ୍ଦଗୁଡ଼ିକ_3 ; idx += 1) {
		  array_[idx].abits_ptr_ = 0;
		  array_[idx].bbits_ptr_ = 0;
	    }
      }
}

vvp_vector4array_sa::~vvp_vector4array_sa()
{
      ଯଦି (array_) {
	    ଯଦି (width_ > vvp_vector4_t::BITS_PER_WORD) {
		  ପାଇଁ (unsigned idx = 0 ; idx < ଶବ୍ଦଗୁଡ଼ିକ_3 ; idx += 1)
			ଯଦି (array_[idx].abits_ptr_)
			      delete[]array_[idx].abits_ptr_;
	    }
	    delete[]array_;
      }
}

ଶୂନ୍ୟ vvp_vector4array_sa::set_word(unsigned index, ଅଚଳ vvp_vector4_t&that)
{
      ପ୍ରତିଜ୍ଞା(index < ଶବ୍ଦଗୁଡ଼ିକ_3);

      v4cell*cell = &array_[index];

      set_word_(cell, that);
}

vvp_vector4_t vvp_vector4array_sa::get_word(unsigned index) ଅଚଳ
{
      ଯଦି (index >= ଶବ୍ଦଗୁଡ଼ିକ_3)
	    ଫେରନ୍ତୁ vvp_vector4_t(width_, BIT4_X);

      ପ୍ରତିଜ୍ଞା(index < ଶବ୍ଦଗୁଡ଼ିକ_3);

      v4cell*cell = &array_[index];

      ଫେରନ୍ତୁ get_word_(cell);
}

vvp_vector4array_aa::vvp_vector4array_aa(unsigned width__, unsigned ଶବ୍ଦଗୁଡ଼ିକ_2)
: vvp_vector4array_t(width__, ଶବ୍ଦଗୁଡ଼ିକ_2)
{
      context_idx_ = vpip_add_item_to_context(ଏହି, vpip_peek_context_scope());
}

vvp_vector4array_aa::~vvp_vector4array_aa()
{
}

ଶୂନ୍ୟ vvp_vector4array_aa::alloc_instance(vvp_context_t context)
{
      v4cell*array = ନୂତନ v4cell[ଶବ୍ଦଗୁଡ଼ିକ_3];

      ଯଦି (width_ <= vvp_vector4_t::BITS_PER_WORD) {
	    ପାଇଁ (unsigned idx = 0 ; idx < ଶବ୍ଦଗୁଡ଼ିକ_3 ; idx += 1) {
		  array[idx].abits_val_ = vvp_vector4_t::WORD_X_ABITS;
		  array[idx].bbits_val_ = vvp_vector4_t::WORD_X_BBITS;
	    }
      } ଅନ୍ୟଥା {
	    ପାଇଁ (unsigned idx = 0 ; idx < ଶବ୍ଦଗୁଡ଼ିକ_3 ; idx += 1) {
		  array[idx].abits_ptr_ = 0;
		  array[idx].bbits_ptr_ = 0;
	    }
      }

      vvp_set_context_item(context, context_idx_, array);
}

ଶୂନ୍ୟ vvp_vector4array_aa::reset_instance(vvp_context_t context)
{
      v4cell*cell = static_cast<v4cell*>
            (vvp_get_context_item(context, context_idx_));

      ଯଦି (width_ <= vvp_vector4_t::BITS_PER_WORD) {
	    ପାଇଁ (unsigned idx = 0 ; idx < ଶବ୍ଦଗୁଡ଼ିକ_3 ; idx += 1) {
		  cell->abits_val_ = vvp_vector4_t::WORD_X_ABITS;
		  cell->bbits_val_ = vvp_vector4_t::WORD_X_BBITS;
                  cell++;
	    }
      } ଅନ୍ୟଥା {
            unsigned cnt = (width_ + vvp_vector4_t::BITS_PER_WORD-1)/vvp_vector4_t::BITS_PER_WORD;
	    ପାଇଁ (unsigned idx = 0 ; idx < ଶବ୍ଦଗୁଡ଼ିକ_3 ; idx += 1) {
		  ଯଦି (cell->abits_ptr_) {
                        ପାଇଁ (unsigned n = 0 ; n < cnt ; n += 1) {
			      cell->abits_ptr_[n] = vvp_vector4_t::WORD_X_ABITS;
			      cell->bbits_ptr_[n] = vvp_vector4_t::WORD_X_BBITS;
                        }
                  }
                  cell++;
	    }
      }
}

#ifdef CHECK_WITH_VALGRIND
ଶୂନ୍ୟ vvp_vector4array_aa::free_instance(vvp_context_t context)
{
      v4cell*cell = static_cast<v4cell*>
            (vvp_get_context_item(context, context_idx_));
      delete [] cell;
}
#endif

ଶୂନ୍ୟ vvp_vector4array_aa::set_word(unsigned index, ଅଚଳ vvp_vector4_t&that)
{
      ପ୍ରତିଜ୍ଞା(index < ଶବ୍ଦଗୁଡ଼ିକ_3);

      v4cell*cell = static_cast<v4cell*>
            (vthread_get_wt_context_item(context_idx_)) + index;

      set_word_(cell, that);
}

vvp_vector4_t vvp_vector4array_aa::get_word(unsigned index) ଅଚଳ
{
      ଯଦି (index >= ଶବ୍ଦଗୁଡ଼ିକ_3)
	    ଫେରନ୍ତୁ vvp_vector4_t(width_, BIT4_X);

      ପ୍ରତିଜ୍ଞା(index < ଶବ୍ଦଗୁଡ଼ିକ_3);

      v4cell*cell = static_cast<v4cell*>
            (vthread_get_rd_context_item(context_idx_)) + index;

      ଫେରନ୍ତୁ get_word_(cell);
}

vvp_vector2_t::vvp_vector2_t()
{
      vec_ = 0;
      wid_ = 0;
}

vvp_vector2_t::vvp_vector2_t(uint64_t v, unsigned wid)
{
      wid_ = wid;
      ଅଚଳ unsigned bits_per_word = 8 * ଆକାର(vec_[0]);
      ଅଚଳ unsigned ଶବ୍ଦଗୁଡ଼ିକ = (wid_ + bits_per_word-1) / bits_per_word;

      vec_ = ନୂତନ unsigned long[ଶବ୍ଦଗୁଡ଼ିକ];
      ପାଇଁ (unsigned idx = 0 ; idx < ଶବ୍ଦଗୁଡ଼ିକ ; idx += 1) {
	    ଅଚଳ unsigned shift = idx * bits_per_word;
	    vec_[idx] = shift < 64 ? v >> shift : 0;
      }
}

vvp_vector2_t::vvp_vector2_t(vvp_vector2_t::fill_t fill, unsigned wid)
{
      wid_ = wid;
      ଅଚଳ unsigned bits_per_word = 8 * ଆକାର(vec_[0]);
      ଅଚଳ unsigned ଶବ୍ଦଗୁଡ଼ିକ = (wid_ + bits_per_word-1) / bits_per_word;

      vec_ = ନୂତନ unsigned long[ଶବ୍ଦଗୁଡ଼ିକ];
      ପାଇଁ (unsigned idx = 0 ;  idx < ଶବ୍ଦଗୁଡ଼ିକ ;  idx += 1)
	    vec_[idx] = fill? -1 : 0;
}

vvp_vector2_t::vvp_vector2_t(ଅଚଳ vvp_vector2_t&that, unsigned base, unsigned wid)
{
      wid_ = wid;
      ଅଚଳ unsigned ଶବ୍ଦଗୁଡ଼ିକ = (wid_ + BITS_PER_WORD-1) / BITS_PER_WORD;

      vec_ = ନୂତନ unsigned long[ଶବ୍ଦଗୁଡ଼ିକ];

      ପାଇଁ (unsigned idx = 0 ; idx < wid ; idx += 1) {
	    ପୂର୍ଣ୍ଣସଂଖ୍ୟା bit = that.value(base+idx);
	    ଯଦି (bit == 0)
		  ଜାରି;
	    unsigned word = idx / BITS_PER_WORD;
	    unsigned long mask = 1UL << (idx % BITS_PER_WORD);
	    vec_[word] |= mask;
      }
}

ଶୂନ୍ୟ vvp_vector2_t::copy_from_that_(ଅଚଳ vvp_vector4_t&that)
{
      wid_ = that.size();
      ଅଚଳ unsigned ଶବ୍ଦଗୁଡ଼ିକ = (that.size() + BITS_PER_WORD-1) / BITS_PER_WORD;

      ଯଦି (ଶବ୍ଦଗୁଡ଼ିକ == 0) {
	    vec_ = 0;
	    wid_ = 0;
	    ଫେରନ୍ତୁ;
      }

	// Use the subarray method with the xz_to_0 flag set so that
	// we get values even when there are xz bits.
      vec_ = that.subarray(0, wid_, ସତ୍ୟ);
}

ଶୂନ୍ୟ vvp_vector2_t::copy_from_that_(ଅଚଳ vvp_vector2_t&that)
{
      wid_ = that.wid_;
      ଅଚଳ unsigned ଶବ୍ଦଗୁଡ଼ିକ = (wid_ + BITS_PER_WORD-1) / BITS_PER_WORD;

      ଯଦି (ଶବ୍ଦଗୁଡ଼ିକ == 0) {
	    vec_ = 0;
	    wid_ = 0;
	    ଫେରନ୍ତୁ;
      }

      vec_ = ନୂତନ unsigned long[ଶବ୍ଦଗୁଡ଼ିକ];
      ପାଇଁ (unsigned idx = 0 ;  idx < ଶବ୍ଦଗୁଡ଼ିକ ;  idx += 1)
	    vec_[idx] = that.vec_[idx];
}

vvp_vector2_t::vvp_vector2_t(ଅଚଳ vvp_vector2_t&that, unsigned newsize)
{
      wid_ = newsize;
      ଯଦି (newsize == 0) {
	    vec_ = 0;
	    ଫେରନ୍ତୁ;
      }

      ଅଚଳ unsigned ଶବ୍ଦଗୁଡ଼ିକ = (wid_ + BITS_PER_WORD-1) / BITS_PER_WORD;
      ଅଚଳ unsigned twords = (that.wid_ + BITS_PER_WORD-1) / BITS_PER_WORD;

      vec_ = ନୂତନ unsigned long[ଶବ୍ଦଗୁଡ଼ିକ];
      ପାଇଁ (unsigned idx = 0 ;  idx < ଶବ୍ଦଗୁଡ଼ିକ ;  idx += 1) {
	    ଯଦି (idx < twords)
		  vec_[idx] = that.vec_[idx];
	    ଅନ୍ୟଥା
		  vec_[idx] = 0;
      }
}

vvp_vector2_t& vvp_vector2_t::operator= (ଅଚଳ vvp_vector2_t&that)
{
      ଯଦି (ଏହି == &that)
	    ଫେରନ୍ତୁ *ଏହି;

      delete[] vec_;
      vec_ = 0;

      copy_from_that_(that);
      ଫେରନ୍ତୁ *ଏହି;
}

vvp_vector2_t& vvp_vector2_t::operator= (ଅଚଳ vvp_vector4_t&that)
{
      delete[]vec_;
      vec_ = 0;
      copy_from_that_(that);
      ଫେରନ୍ତୁ *ଏହି;
}

vvp_vector2_t& vvp_vector2_t::operator <<= (unsigned ପୂର୍ଣ୍ଣସଂଖ୍ୟା shift)
{
      ଯଦି (wid_ == 0)
	    ଫେରନ୍ତୁ *ଏହି;

      ଅଚଳ unsigned ଶବ୍ଦଗୁଡ଼ିକ = (wid_ + BITS_PER_WORD-1) / BITS_PER_WORD;

	// Number of words to shift
      ଅଚଳ unsigned wshift = shift / BITS_PER_WORD;
	// bits to shift within each word.
      ଅଚଳ unsigned long oshift = shift % BITS_PER_WORD;

	// If shifting the entire value away, then return zeros.
      ଯଦି (wshift >= ଶବ୍ଦଗୁଡ଼ିକ) {
	    ପାଇଁ (unsigned idx = 0 ;  idx < ଶବ୍ଦଗୁଡ଼ିକ ;  idx += 1)
		  vec_[idx] = 0;

	    ଫେରନ୍ତୁ *ଏହି;
      }

	// Do the word shift first.
      ଯଦି (wshift > 0) {
	    ପାଇଁ (unsigned idx = 0 ;  idx < ଶବ୍ଦଗୁଡ଼ିକ-wshift ;  idx += 1) {
		  unsigned sel = ଶବ୍ଦଗୁଡ଼ିକ - idx - 1;
		  vec_[sel] = vec_[sel-wshift];
	    }

	    ପାଇଁ (unsigned idx = 0 ;  idx < wshift ;  idx += 1)
		  vec_[idx] = 0;
      }

	// Do the fine shift.
      ଯଦି (oshift != 0) {
	    unsigned long pad = 0;
	    ପାଇଁ (unsigned idx = 0 ;  idx < ଶବ୍ଦଗୁଡ଼ିକ ;  idx += 1) {
		  unsigned long next_pad = vec_[idx] >> (BITS_PER_WORD-oshift);
		  vec_[idx] = (vec_[idx] << oshift) | pad;
		  pad = next_pad;
	    }

	      // Cleanup the tail bits.
	    unsigned long mask = -1UL >> (BITS_PER_WORD - wid_%BITS_PER_WORD);
	    vec_[ଶବ୍ଦଗୁଡ଼ିକ-1] &= mask;
      }

      ଫେରନ୍ତୁ *ଏହି;
}

vvp_vector2_t& vvp_vector2_t::operator >>= (unsigned shift)
{
      ଯଦି (wid_ == 0)
	    ଫେରନ୍ତୁ *ଏହି;

      ଅଚଳ unsigned ଶବ୍ଦଗୁଡ଼ିକ = (wid_ + BITS_PER_WORD-1) / BITS_PER_WORD;

	// Number of words to shift
      ଅଚଳ unsigned wshift = shift / BITS_PER_WORD;
	// bits to shift within each word.
      ଅଚଳ unsigned long oshift = shift % BITS_PER_WORD;

	// If shifting the entire value away, then return zeros.
      ଯଦି (wshift >= ଶବ୍ଦଗୁଡ଼ିକ) {
	    ପାଇଁ (unsigned idx = 0 ;  idx < ଶବ୍ଦଗୁଡ଼ିକ ;  idx += 1)
		  vec_[idx] = 0;

	    ଫେରନ୍ତୁ *ଏହି;
      }

      ଯଦି (wshift > 0) {
	    ପାଇଁ (unsigned idx = 0 ;  idx < ଶବ୍ଦଗୁଡ଼ିକ-wshift ;  idx += 1)
		  vec_[idx] = vec_[idx+wshift];

	    ପାଇଁ (unsigned idx = ଶବ୍ଦଗୁଡ଼ିକ-wshift ;  idx < ଶବ୍ଦଗୁଡ଼ିକ ;  idx += 1)
		  vec_[idx] = 0;
      }

      ଯଦି (oshift > 0) {
	    unsigned long pad = 0;
	    ପାଇଁ (unsigned idx = ଶବ୍ଦଗୁଡ଼ିକ ;  idx > 0 ;  idx -= 1) {
		  unsigned long new_pad = vec_[idx-1] <<(BITS_PER_WORD-oshift);
		  vec_[idx-1] = pad | (vec_[idx-1] >> oshift);
		  pad = new_pad;
	    }

	      // Cleanup the tail bits.

	    unsigned use_ଶବ୍ଦଗୁଡ଼ିକ = ଶବ୍ଦଗୁଡ଼ିକ;
	      // Mask_shift is the number of high bits of the top word
	      // that are to be masked off. We start with the number
	      // of bits that are not included even in the original,
	      // then we include the bits of the shift, that are to be
	      // masked to zero.
	    unsigned long mask_shift = BITS_PER_WORD - wid_%BITS_PER_WORD;
	    mask_shift %= BITS_PER_WORD;
	    mask_shift += oshift;
	    ଯେତେବେଳେ (mask_shift >= BITS_PER_WORD) {
		  vec_[use_ଶବ୍ଦଗୁଡ଼ିକ-1] = 0;
		  use_ଶବ୍ଦଗୁଡ଼ିକ -= 1;
		  mask_shift -= BITS_PER_WORD;
	    }
	    ଯଦି (mask_shift > 0) {
		  ପ୍ରତିଜ୍ଞା(use_ଶବ୍ଦଗୁଡ଼ିକ > 0);
		  unsigned long mask = -1UL >> mask_shift;
		  vec_[use_ଶବ୍ଦଗୁଡ଼ିକ-1] &= mask;
	    }
      }

      ଫେରନ୍ତୁ *ଏହି;
}

ସ୍ଥିର unsigned long add_carry(unsigned long a, unsigned long b,
			       unsigned long&carry)
{
      unsigned long out = carry;
      carry = 0;

      ଯଦି ((ULONG_MAX - out) < a)
	    carry += 1;
      out += a;

      ଯଦି ((ULONG_MAX - out) < b)
	    carry += 1;
      out += b;

      ଫେରନ୍ତୁ out;
}

vvp_vector2_t& vvp_vector2_t::operator += (ଅଚଳ vvp_vector2_t&that)
{
      ପ୍ରତିଜ୍ଞା(wid_ == that.wid_);
      ଯଦି (wid_ == 0)
	    ଫେରନ୍ତୁ *ଏହି;

      ଅଚଳ unsigned ଶବ୍ଦଗୁଡ଼ିକ = (wid_ + BITS_PER_WORD-1) / BITS_PER_WORD;

      unsigned long carry = 0;
      ପାଇଁ (unsigned idx = 0 ;  idx < ଶବ୍ଦଗୁଡ଼ିକ ;  idx += 1) {
	    vec_[idx] = add_carry(vec_[idx], that.vec_[idx], carry);
      }


	// Cleanup the tail bits.
      unsigned long mask = -1UL >> (BITS_PER_WORD - wid_%BITS_PER_WORD);
      vec_[ଶବ୍ଦଗୁଡ଼ିକ-1] &= mask;

      ଫେରନ୍ତୁ *ଏହି;
}

vvp_vector2_t& vvp_vector2_t::operator -= (ଅଚଳ vvp_vector2_t&that)
{
      ପ୍ରତିଜ୍ଞା(wid_ == that.wid_);
      ଯଦି (wid_ == 0)
	    ଫେରନ୍ତୁ *ଏହି;

      ଅଚଳ unsigned ଶବ୍ଦଗୁଡ଼ିକ = (wid_ + BITS_PER_WORD-1) / BITS_PER_WORD;

      unsigned long carry = 1;
      ପାଇଁ (unsigned idx = 0 ;  idx < ଶବ୍ଦଗୁଡ଼ିକ ;  idx += 1) {
	    vec_[idx] = add_carry(vec_[idx], ~that.vec_[idx], carry);
      }

      ଫେରନ୍ତୁ *ଏହି;
}

ଶୂନ୍ୟ vvp_vector2_t::trim()
{
      ଯେତେବେଳେ (value(wid_-1) == 0 && wid_ > 1) wid_ -= 1;
}

/* This is a special trim that is used on numbers we know represent a
 * negative signed value (they came from a negative real value). */
ଶୂନ୍ୟ vvp_vector2_t::trim_neg()
{
      ଯଦି (value(wid_-1) == 1 && wid_ > 32) {
	    ଯେତେବେଳେ (value(wid_-2) == 1 && wid_ > 32) wid_ -= 1;
      }
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା vvp_vector2_t::value(unsigned idx) ଅଚଳ
{
      ଯଦି (idx >= wid_)
	    ଫେରନ୍ତୁ 0;

      ଅଚଳ unsigned bits_per_word = 8 * ଆକାର(vec_[0]);
      unsigned addr = idx/bits_per_word;
      unsigned mask = idx%bits_per_word;

      ଯଦି (vec_[addr] & (1UL<<mask))
	    ଫେରନ୍ତୁ 1;
      ଅନ୍ୟଥା
	    ଫେରନ୍ତୁ 0;
}

ଶୂନ୍ୟ vvp_vector2_t::set_bit(unsigned idx, ପୂର୍ଣ୍ଣସଂଖ୍ୟା bit)
{
      ପ୍ରତିଜ୍ଞା(idx < wid_);

      ଅଚଳ unsigned bits_per_word = 8 * ଆକାର(vec_[0]);
      unsigned addr = idx/bits_per_word;
      unsigned long mask = idx%bits_per_word;

      ଯଦି (bit)
	    vec_[addr] |= 1UL << mask;
      ଅନ୍ୟଥା
	    vec_[addr] &= ~(1UL << mask);
}

ଶୂନ୍ୟ vvp_vector2_t::set_vec(unsigned adr, ଅଚଳ vvp_vector2_t&that)
{
      ପ୍ରତିଜ୍ଞା((adr + that.wid_) <= wid_);

      ପାଇଁ (unsigned idx = 0 ; idx < that.wid_ ; idx += 1)
	    set_bit(adr+idx, that.value(idx));
}

ତାର୍କିକ vvp_vector2_t::is_NaN() ଅଚଳ
{
      ଫେରନ୍ତୁ wid_ == 0;
}

ତାର୍କିକ vvp_vector2_t::is_zero() ଅଚଳ
{
      ଅଚଳ unsigned ଶବ୍ଦଗୁଡ଼ିକ = (wid_ + BITS_PER_WORD-1) / BITS_PER_WORD;

      ପାଇଁ (unsigned idx = 0; idx < ଶବ୍ଦଗୁଡ଼ିକ; idx += 1) {
	    ଯଦି (vec_[idx] == 0) ଜାରି;
	    ଫେରନ୍ତୁ ମିଥ୍ୟା;
      }

      ଫେରନ୍ତୁ ସତ୍ୟ;
}

/*
 * Basic idea from "Introduction to Programming using SML" by
 * Michael R. Hansen and Hans Rischel page 261 and "Seminumerical
 * Algorithms, Third Edition" by Donald E. Knuth section 4.6.3.
 */
vvp_vector2_t pow(ଅଚଳ vvp_vector2_t&x, vvp_vector2_t&y)
{
        /* If we have a zero exponent just return 1. */
      ଯଦି (y == vvp_vector2_t(0L, 1)) {
	    ଫେରନ୍ତୁ vvp_vector2_t(1L, x.size());
      }

        /* Is the value odd? */
      ଯଦି (y.value(0) == 1) {
	    y.set_bit(0, 0);  // A quick subtract by 1.
	    vvp_vector2_t res = x * pow(x, y);
	    ଫେରନ୍ତୁ res;
      }

      y >>= 1;  // A fast divide by two. We know the LSB is zero.
      vvp_vector2_t z = pow(x, y);
      vvp_vector2_t res = z * z;
      ଫେରନ୍ତୁ res;
}

ସ୍ଥିର ଶୂନ୍ୟ multiply_long(unsigned long a, unsigned long b,
			  unsigned long&ତଳ, unsigned long&ଉପର)
{
      ପ୍ରତିଜ୍ଞା(ଆକାର(unsigned long) %2 == 0);

      ଅଚଳ unsigned long word_mask = (1UL << 4UL*ଆକାର(a)) - 1UL;
      unsigned long tmpa;
      unsigned long tmpb;
      unsigned long res[4];

      tmpa = a & word_mask;
      tmpb = b & word_mask;
      res[0] = tmpa * tmpb;
      res[1] = res[0] >> 4UL*ଆକାର(unsigned long);
      res[0] &= word_mask;

      tmpa = (a >> 4UL*ଆକାର(unsigned long)) & word_mask;
      res[1] += tmpa * tmpb;
      res[2] = res[1] >> 4UL*ଆକାର(unsigned long);
      res[1] &= word_mask;

      tmpa = a & word_mask;
      tmpb = (b >> 4UL*ଆକାର(unsigned long)) & word_mask;
      res[1] += tmpa * tmpb;
      res[2] += res[1] >> 4UL*ଆକାର(unsigned long);
      res[3]  = res[2] >> 4UL*ଆକାର(unsigned long);
      res[1] &= word_mask;
      res[2] &= word_mask;

      tmpa = (a >> 4UL*ଆକାର(unsigned long)) & word_mask;
      res[2] += tmpa * tmpb;
      res[3] += res[2] >> 4UL*ଆକାର(unsigned long);
      res[2] &= word_mask;

      ଉପର = (res[3] << 4UL*ଆକାର(unsigned long)) | res[2];
      ତଳ  = (res[1] << 4UL*ଆକାର(unsigned long)) | res[0];
}

vvp_vector2_t operator * (ଅଚଳ vvp_vector2_t&a, ଅଚଳ vvp_vector2_t&b)
{
      ଅଚଳ unsigned bits_per_word = 8 * ଆକାର(a.vec_[0]);

	// The compiler ensures that the two operands are of equal size.
      ପ୍ରତିଜ୍ଞା(a.size() == b.size());
      vvp_vector2_t r (0, a.size());

      unsigned ଶବ୍ଦଗୁଡ଼ିକ = (r.wid_ + bits_per_word - 1) / bits_per_word;

      ପାଇଁ (unsigned bdx = 0 ;  bdx < ଶବ୍ଦଗୁଡ଼ିକ ;  bdx += 1) {
	    unsigned long tmpb = b.vec_[bdx];
	    ଯଦି (tmpb == 0)
		  ଜାରି;

	    ପାଇଁ (unsigned adx = 0 ;  adx < ଶବ୍ଦଗୁଡ଼ିକ ;  adx += 1) {
		  unsigned long tmpa = a.vec_[adx];
		  ଯଦି (tmpa == 0)
			ଜାରି;

		  unsigned long ତଳ, hig;
		  multiply_long(tmpa, tmpb, ତଳ, hig);

		  unsigned long carry = 0;
		  ପାଇଁ (unsigned sdx = 0
			     ; (adx+bdx+sdx) < ଶବ୍ଦଗୁଡ଼ିକ
			     ;  sdx += 1) {

			r.vec_[adx+bdx+sdx] = add_carry(r.vec_[adx+bdx+sdx],
							ତଳ, carry);
			ତଳ = hig;
			hig = 0;
		  }
	    }
      }

      ଫେରନ୍ତୁ r;
}

ସ୍ଥିର ଶୂନ୍ୟ div_mod (vvp_vector2_t dividend, vvp_vector2_t ଭାଜକ,
		     vvp_vector2_t&quotient, vvp_vector2_t&remainder)
{

      quotient = vvp_vector2_t(0, dividend.size());

      ଯଦି (ଭାଜକ == quotient) {
	    cerr << "ERROR: division by zero, exiting." << endl;
	    exit(255);
      }

      ଯଦି (dividend < ଭାଜକ) {
	    remainder = dividend;
	    ଫେରନ୍ତୁ;
      }

      vvp_vector2_t mask (1, dividend.size());

	// Make the dividend 1 bit larger to prevent overflow of
	// divtmp in startup.
      dividend = vvp_vector2_t(dividend, dividend.size()+1);
      vvp_vector2_t divtmp (ଭାଜକ, dividend.size());

      ଯେତେବେଳେ (divtmp < dividend) {
	    divtmp <<= 1;
	    mask <<= 1;
      }

      ଯେତେବେଳେ (dividend >= ଭାଜକ) {
	    ଯଦି (divtmp <= dividend) {
		  dividend -= divtmp;
		  quotient += mask;
	    }

	    divtmp >>= 1;
	    mask >>= 1;
      }

      remainder = vvp_vector2_t(dividend, mask.size());
}

vvp_vector2_t operator - (ଅଚଳ vvp_vector2_t&that)
{
      vvp_vector2_t neg(that);
      ଯଦି (neg.wid_ == 0) ଫେରନ୍ତୁ neg;

      ଅଚଳ unsigned ଶବ୍ଦଗୁଡ଼ିକ = (neg.wid_ + neg.BITS_PER_WORD-1) /
                                         neg.BITS_PER_WORD;
      ପାଇଁ (unsigned idx = 0 ;  idx < ଶବ୍ଦଗୁଡ଼ିକ ;  idx += 1) {
	    neg.vec_[idx] = ~neg.vec_[idx];
      }
      neg += vvp_vector2_t(1, neg.wid_);

      ଫେରନ୍ତୁ neg;
}

vvp_vector2_t operator / (ଅଚଳ vvp_vector2_t&dividend,
			  ଅଚଳ vvp_vector2_t&ଭାଜକ)
{
      vvp_vector2_t quot, rem;
      div_mod(dividend, ଭାଜକ, quot, rem);
      ଫେରନ୍ତୁ quot;
}

vvp_vector2_t operator % (ଅଚଳ vvp_vector2_t&dividend,
			  ଅଚଳ vvp_vector2_t&ଭାଜକ)
{
      vvp_vector2_t quot, rem;
      div_mod(dividend, ଭାଜକ, quot, rem);
      ଫେରନ୍ତୁ rem;
}

ତାର୍କିକ operator > (ଅଚଳ vvp_vector2_t&a, ଅଚଳ vvp_vector2_t&b)
{
      ଅଚଳ unsigned awords = (a.wid_ + vvp_vector2_t::BITS_PER_WORD-1) / vvp_vector2_t::BITS_PER_WORD;
      ଅଚଳ unsigned bwords = (b.wid_ + vvp_vector2_t::BITS_PER_WORD-1) / vvp_vector2_t::BITS_PER_WORD;

      ଅଚଳ unsigned ଶବ୍ଦଗୁଡ଼ିକ = awords > bwords? awords : bwords;

      ପାଇଁ (unsigned idx = ଶବ୍ଦଗୁଡ଼ିକ ;  idx > 0 ;  idx -= 1) {
	    unsigned long aw = (idx <= awords)? a.vec_[idx-1] : 0;
	    unsigned long bw = (idx <= bwords)? b.vec_[idx-1] : 0;

	    ଯଦି (aw > bw)
		  ଫେରନ୍ତୁ ସତ୍ୟ;
	    ଯଦି (aw < bw)
		  ଫେରନ୍ତୁ ମିଥ୍ୟା;
      }

	// If the above loop finishes, then the vectors are equal.
      ଫେରନ୍ତୁ ମିଥ୍ୟା;
}

ତାର୍କିକ operator >= (ଅଚଳ vvp_vector2_t&a, ଅଚଳ vvp_vector2_t&b)
{
      ଅଚଳ unsigned awords = (a.wid_ + vvp_vector2_t::BITS_PER_WORD-1) / vvp_vector2_t::BITS_PER_WORD;
      ଅଚଳ unsigned bwords = (b.wid_ + vvp_vector2_t::BITS_PER_WORD-1) / vvp_vector2_t::BITS_PER_WORD;

      ଅଚଳ unsigned ଶବ୍ଦଗୁଡ଼ିକ = awords > bwords? awords : bwords;

      ପାଇଁ (unsigned idx = ଶବ୍ଦଗୁଡ଼ିକ ;  idx > 0 ;  idx -= 1) {
	    unsigned long aw = (idx <= awords)? a.vec_[idx-1] : 0;
	    unsigned long bw = (idx <= bwords)? b.vec_[idx-1] : 0;

	    ଯଦି (aw > bw)
		  ଫେରନ୍ତୁ ସତ୍ୟ;
	    ଯଦି (aw < bw)
		  ଫେରନ୍ତୁ ମିଥ୍ୟା;
      }

	// If the above loop finishes, then the vectors are equal.
      ଫେରନ୍ତୁ ସତ୍ୟ;
}

ତାର୍କିକ operator < (ଅଚଳ vvp_vector2_t&a, ଅଚଳ vvp_vector2_t&b)
{
      ଅଚଳ unsigned awords = (a.wid_ + vvp_vector2_t::BITS_PER_WORD-1) / vvp_vector2_t::BITS_PER_WORD;
      ଅଚଳ unsigned bwords = (b.wid_ + vvp_vector2_t::BITS_PER_WORD-1) / vvp_vector2_t::BITS_PER_WORD;

      unsigned ଶବ୍ଦଗୁଡ଼ିକ = awords;
      ଯଦି (bwords > ଶବ୍ଦଗୁଡ଼ିକ)
	    ଶବ୍ଦଗୁଡ଼ିକ = bwords;

      ପାଇଁ (unsigned idx = ଶବ୍ଦଗୁଡ଼ିକ ;  idx > 0 ;  idx -= 1) {
	    unsigned long aw = (idx <= awords)? a.vec_[idx-1] : 0;
	    unsigned long bw = (idx <= bwords)? b.vec_[idx-1] : 0;

	    ଯଦି (aw < bw)
		  ଫେରନ୍ତୁ ସତ୍ୟ;
	    ଯଦି (aw > bw)
		  ଫେରନ୍ତୁ ମିଥ୍ୟା;
      }

	// If the above loop finishes, then the vectors are equal.
      ଫେରନ୍ତୁ ମିଥ୍ୟା;
}

ତାର୍କିକ operator <= (ଅଚଳ vvp_vector2_t&a, ଅଚଳ vvp_vector2_t&b)
{
	// XXXX For now, only support equal width vectors.
      ପ୍ରତିଜ୍ଞା(a.wid_ == b.wid_);

      ଅଚଳ unsigned awords = (a.wid_ + vvp_vector2_t::BITS_PER_WORD-1) / vvp_vector2_t::BITS_PER_WORD;

      ପାଇଁ (unsigned idx = awords ;  idx > 0 ;  idx -= 1) {
	    ଯଦି (a.vec_[idx-1] < b.vec_[idx-1])
		  ଫେରନ୍ତୁ ସତ୍ୟ;
	    ଯଦି (a.vec_[idx-1] > b.vec_[idx-1])
		  ଫେରନ୍ତୁ ମିଥ୍ୟା;
      }

	// If the above loop finishes, then the vectors are equal.
      ଫେରନ୍ତୁ ସତ୍ୟ;
}

ତାର୍କିକ operator == (ଅଚଳ vvp_vector2_t&a, ଅଚଳ vvp_vector2_t&b)
{
      ଅଚଳ unsigned awords = (a.wid_ + vvp_vector2_t::BITS_PER_WORD-1) / vvp_vector2_t::BITS_PER_WORD;
      ଅଚଳ unsigned bwords = (b.wid_ + vvp_vector2_t::BITS_PER_WORD-1) / vvp_vector2_t::BITS_PER_WORD;

      ଅଚଳ unsigned ଶବ୍ଦଗୁଡ଼ିକ = awords > bwords? awords : bwords;

      ପାଇଁ (unsigned idx = ଶବ୍ଦଗୁଡ଼ିକ ;  idx > 0 ;  idx -= 1) {
	    unsigned long aw = (idx <= awords)? a.vec_[idx-1] : 0;
	    unsigned long bw = (idx <= bwords)? b.vec_[idx-1] : 0;

	    ଯଦି (aw > bw)
		  ଫେରନ୍ତୁ ମିଥ୍ୟା;
	    ଯଦି (aw < bw)
		  ଫେରନ୍ତୁ ମିଥ୍ୟା;
      }

	// If the above loop finishes, then the vectors are equal.
      ଫେରନ୍ତୁ ସତ୍ୟ;
}


vvp_vector4_t vector2_to_vector4(ଅଚଳ vvp_vector2_t&that, unsigned wid)
{
      vvp_vector4_t res (wid);

      ପାଇଁ (unsigned idx = 0 ;  idx < res.size() ;  idx += 1) {
	    vvp_bit4_t bit = BIT4_0;

	    ଯଦି (that.value(idx))
		  bit = BIT4_1;

	    res.set_bit(idx, bit);
      }

      ଫେରନ୍ତୁ res;
}

ସ୍ଥିର ତାର୍କିକ c4string_header_test(ଅଚଳ ଅକ୍ଷର*str)
{
      ଯଦି ((str[0] != 'C') && (str[0] != 'c'))
	    ଫେରନ୍ତୁ ମିଥ୍ୟା;
      ଯଦି ((str[1] != '4') || (str[2] != '<'))
	    ଫେରନ୍ତୁ ମିଥ୍ୟା;
      ଫେରନ୍ତୁ ସତ୍ୟ;
}

ତାର୍କିକ c4string_test(ଅଚଳ ଅକ୍ଷର*str)
{
      ଯଦି (!c4string_header_test(str))
	    ଫେରନ୍ତୁ ମିଥ୍ୟା;
      size_t ମୂଲ୍ୟ_size = strspn(str+3, "01xz");
      ଯଦି (str[3+ମୂଲ୍ୟ_size] != '>')
	    ଫେରନ୍ତୁ ମିଥ୍ୟା;
      ଯଦି (str[3+ମୂଲ୍ୟ_size+1] != 0)
	    ଫେରନ୍ତୁ ମିଥ୍ୟା;

      ଫେରନ୍ତୁ ସତ୍ୟ;
}

vvp_vector4_t c4string_to_vector4(ଅଚଳ ଅକ୍ଷର*str)
{
      ପ୍ରତିଜ୍ଞା(c4string_header_test(str));

      str += 3;
      ଅଚଳ ଅକ୍ଷର*tp = str + strspn(str,"01xz");
      ପ୍ରତିଜ୍ଞା(tp[0] == '>');

      vvp_vector4_t tmp (tp-str);

      ପାଇଁ (unsigned idx = 0 ;  idx < tmp.size() ;  idx += 1) {
	    vvp_bit4_t bit;
	    ବାଛ (str[idx]) {
		ସ୍ଥିତି '0':
		  bit = BIT4_0;
		  ଭାଙ୍ଗ;
		ସ୍ଥିତି '1':
		  bit = BIT4_1;
		  ଭାଙ୍ଗ;
		ସ୍ଥିତି 'x':
		  bit = BIT4_X;
		  ଭାଙ୍ଗ;
		ସ୍ଥିତି 'z':
		  bit = BIT4_Z;
		  ଭାଙ୍ଗ;
		ପୂର୍ବନିର୍ଦ୍ଧାରିତ:
		  fprintf(stderr, "Unsupported bit value %c(%d).\n", str[idx],
		          str[idx]);
		  ପ୍ରତିଜ୍ଞା(0);
		  bit = BIT4_0;
		  ଭାଙ୍ଗ;
	    }
	    tmp.set_bit(tmp.size()-idx-1, bit);
      }

      ଫେରନ୍ତୁ tmp;
}

ostream& operator<< (ostream&out, ଅଚଳ vvp_vector2_t&that)
{
      ଯଦି (that.is_NaN()) {
	    out << "NaN";

      } ଅନ୍ୟଥା {
	    out << vector2_to_vector4(that, that.size());
      }
      ଫେରନ୍ତୁ out;
}

vvp_vector8_t::vvp_vector8_t(ଅଚଳ vvp_vector8_t&that)
{
      size_ = that.size_;
      ଯଦି (size_ <= ଆକାର(val_)) {
	    memcpy(val_, that.val_, ଆକାର val_);
      } ଅନ୍ୟଥା {
	    ptr_ = ନୂତନ unsigned ଅକ୍ଷର[size_];
	    memcpy(ptr_, that.ptr_, size_);
      }
}

vvp_vector8_t::vvp_vector8_t(ଅଚଳ vvp_vector4_t&that,
			     unsigned str0, unsigned str1)
: size_(that.size())
{
      ଯଦି (size_ == 0)
	    ଫେରନ୍ତୁ;

      ଯଦି (size_ <= ଆକାର(val_)) {
	    ptr_ = 0; // Prefill all val_ bytes
	    ପାଇଁ (unsigned idx = 0 ; idx < size_ ; idx += 1)
		  val_[idx] = vvp_scalar_t(that.value(idx),str0, str1).raw();
      } ଅନ୍ୟଥା {
	    ptr_ = ନୂତନ unsigned ଅକ୍ଷର[size_];
	    ପାଇଁ (unsigned idx = 0 ;  idx < size_ ;  idx += 1)
		  ptr_[idx] = vvp_scalar_t(that.value(idx), str0, str1).raw();
      }
}

vvp_vector8_t::vvp_vector8_t(ଅଚଳ vvp_vector2_t&that,
			     unsigned str0, unsigned str1)
: size_(that.size())
{
      ଯଦି (size_ == 0)
	    ଫେରନ୍ତୁ;

      ଯଦି (size_ <= ଆକାର(val_)) {
	    ptr_ = 0;
	    ପାଇଁ (unsigned idx = 0 ; idx < size_ ; idx += 1)
		  val_[idx] = vvp_scalar_t(that.value(idx)? BIT4_1:BIT4_0, str0, str1).raw();
      } ଅନ୍ୟଥା {
	    ptr_ = ନୂତନ unsigned ଅକ୍ଷର[size_];
	    ପାଇଁ (unsigned idx = 0 ; idx < size_ ; idx += 1)
		  ptr_[idx] = vvp_scalar_t(that.value(idx)? BIT4_1:BIT4_0, str0, str1).raw();
      }
}

ଅଚଳ vvp_vector8_t vvp_vector8_t::nil;

vvp_vector8_t& vvp_vector8_t::operator= (ଅଚଳ vvp_vector8_t&that)
{
	// Assign to self.
      ଯଦି (ଏହି == &that)
	    ଫେରନ୍ତୁ *ଏହି;

      ଯଦି (size_ != that.size_) {
	    ଯଦି (size_ > ଆକାର(val_))
		  delete[]ptr_;
	    size_ = 0;
      }

      ଯଦି (that.size_ == 0) {
	    ପ୍ରତିଜ୍ଞା(size_ == 0);
	    ଫେରନ୍ତୁ *ଏହି;
      }

      ଯଦି (that.size_ <= ଆକାର(val_)) {
	    size_ = that.size_;
	    memcpy(val_, that.val_, ଆକାର(val_));
	    ଫେରନ୍ତୁ *ଏହି;
      }

      ଯଦି (size_ == 0) {
	    size_ = that.size_;
	    ptr_ = ନୂତନ unsigned ଅକ୍ଷର[size_];
      }

      memcpy(ptr_, that.ptr_, size_);

      ଫେରନ୍ତୁ *ଏହି;
}

vvp_vector8_t vvp_vector8_t::subvalue(unsigned base, unsigned wid) ଅଚଳ
{
      vvp_vector8_t tmp (wid);

      unsigned ଅକ୍ଷର*tmp_ptr = tmp.size_ <= ଆକାର(val_) ? tmp.val_ : tmp.ptr_;
      ଅଚଳ unsigned ଅକ୍ଷର*use_ptr = size_ <= ଆକାର(val_) ? val_ : ptr_;

      unsigned idx = 0;
      ଯେତେବେଳେ ((idx < wid) && (base+idx < size_)) {
	    tmp_ptr[idx] = use_ptr[base+idx];
	    idx += 1;
      }

      ଫେରନ୍ତୁ tmp;
}

ଶୂନ୍ୟ vvp_vector8_t::set_vec(unsigned base, ଅଚଳ vvp_vector8_t&that)
{
      ପ୍ରତିଜ୍ଞା((base+that.size()) <= size());
      ପାଇଁ (unsigned idx = 0 ; idx < that.size() ; idx += 1)
	    set_bit(base+idx, that.value(idx));
}

vvp_vector8_t part_expand(ଅଚଳ vvp_vector8_t&that, unsigned wid, unsigned off)
{
      ପ୍ରତିଜ୍ଞା(off < wid);
      vvp_vector8_t tmp (wid);

      unsigned ଅକ୍ଷର* tmp_ptr = tmp.size_<= ଆକାର(tmp.val_) ?
                               tmp.val_ : tmp.ptr_;
      ଅଚଳ unsigned ଅକ୍ଷର* that_ptr = that.size_<= ଆକାର(that.val_) ?
                                      that.val_ : that.ptr_;

      unsigned idx = off;

      ଯେତେବେଳେ (idx < wid && that.size_ > (idx-off)) {
	    tmp_ptr[idx] = that_ptr[idx-off];
	    idx += 1;
      }

      ଫେରନ୍ତୁ tmp;
}

ସ୍ଥିର ତାର୍କିକ c8string_header_test(ଅଚଳ ଅକ୍ଷର*str)
{
      ଯଦି ((str[0] != 'C') && (str[0] != 'c'))
	    ଫେରନ୍ତୁ ମିଥ୍ୟା;
      ଯଦି ((str[1] != '8') || (str[2] != '<'))
	    ଫେରନ୍ତୁ ମିଥ୍ୟା;
      ଫେରନ୍ତୁ ସତ୍ୟ;
}

ତାର୍କିକ c8string_test(ଅଚଳ ଅକ୍ଷର*str)
{
      ଯଦି (!c8string_header_test(str))
	    ଫେରନ୍ତୁ ମିଥ୍ୟା;

      ଅଚଳ ଅକ୍ଷର*cp = str+3;
      ପାଇଁ (;; cp += 1) {
	    ଯଦି (cp[0] == '>' && cp[1] == 0) ଫେରନ୍ତୁ ସତ୍ୟ;
	    ଯଦି (cp[0] >= '0' && cp[0] <= '9') ଜାରି;
	    ଯଦି (cp[0] == 'x') ଜାରି;
	    ଯଦି (cp[0] == 'z') ଜାରି;
	    ଫେରନ୍ତୁ ମିଥ୍ୟା;
      }
      ଫେରନ୍ତୁ ମିଥ୍ୟା;
}
/*
 * The format of a C8<> string is:
 *   C8<aaabbbccc...>
 * where aaa... is a 3 character bit descriptor.
 */
vvp_vector8_t c8string_to_vector8(ଅଚଳ ଅକ୍ଷର*str)
{
      ପ୍ରତିଜ୍ଞା(c8string_header_test(str));

      size_t vsize = strlen(str)-4;
      ପ୍ରତିଜ୍ଞା(vsize%3 == 0);
      vsize /= 3;
      vvp_vector8_t tmp (vsize);

      ପାଇଁ (size_t idx = 0 ; idx < vsize ; idx += 1) {
	    ଅଚଳ ଅକ୍ଷର*cp = str+3+3*idx;
	    vvp_bit4_t bit = BIT4_X;
	    unsigned dr0 = cp[0]-'0';
	    unsigned dr1 = cp[1]-'0';
	    ବାଛ (cp[2]) {
		ସ୍ଥିତି '0':
		  bit = BIT4_0;
		  ଭାଙ୍ଗ;
		ସ୍ଥିତି '1':
		  bit = BIT4_1;
		  ଭାଙ୍ଗ;
		ସ୍ଥିତି 'x':
		  bit = BIT4_X;
		  ଭାଙ୍ଗ;
		ସ୍ଥିତି 'z':
		  bit = BIT4_Z;
		  ଭାଙ୍ଗ;
	    }
	    tmp.set_bit(vsize-idx-1, vvp_scalar_t(bit, dr0, dr1));
      }

      ଫେରନ୍ତୁ tmp;
}

ostream& operator<<(ostream&out, ଅଚଳ vvp_vector8_t&that)
{
      out << "C8<";
      ପାଇଁ (unsigned idx = 0 ;  idx < that.size() ; idx += 1)
	    out << that.value(that.size()-idx-1);

      out << ">";
      ଫେରନ୍ତୁ out;
}

vvp_net_fun_t::vvp_net_fun_t()
{
      ଗଣନା_functors += 1;
}

vvp_net_fun_t::~vvp_net_fun_t()
{
}

ଶୂନ୍ୟ vvp_net_fun_t::recv_vec4(vvp_net_ptr_t, ଅଚଳ vvp_vector4_t&,
                              vvp_context_t)
{
      fprintf(stderr, "internal error: %s: recv_vec4 not implemented\n",
	      typeid(*ଏହି).name());
      ପ୍ରତିଜ୍ଞା(0);
}

ଶୂନ୍ୟ vvp_net_fun_t::recv_vec4_pv_(vvp_net_ptr_t p, ଅଚଳ vvp_vector4_t&bit,
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

      ପ୍ରତିଜ୍ଞା(base + bit.size() <= vwid);

      vvp_vector4_t tmp(vwid, BIT4_Z);
      tmp.set_vec(base, bit);
      recv_vec4(p, tmp, 0);
}

ଶୂନ୍ୟ vvp_net_fun_t::recv_vec4_pv(vvp_net_ptr_t, ଅଚଳ vvp_vector4_t&bit,
                                 unsigned base, unsigned vwid, vvp_context_t)
{
      cerr << "internal error: " << typeid(*ଏହି).name() << ": "
	   << "recv_vec4_pv(" << bit << ", " << base
	   << ", " << vwid << ") not implemented" << endl;
      ପ୍ରତିଜ୍ଞା(0);
}

ଶୂନ୍ୟ vvp_net_fun_t::recv_vec8(vvp_net_ptr_t port, ଅଚଳ vvp_vector8_t&bit)
{
      recv_vec4(port, reduce4(bit), 0);
}

ଶୂନ୍ୟ vvp_net_fun_t::recv_vec8_pv_(vvp_net_ptr_t p, ଅଚଳ vvp_vector8_t&bit,
				  unsigned base, unsigned vwid)
{
	// This is the strength-aware version of recv_vec4_pv_.

      ପ୍ରତିଜ୍ଞା(base + bit.size() <= vwid);

      vvp_vector8_t tmp(vwid);
      tmp.set_vec(base, bit);
      recv_vec8(p, tmp);
}

ଶୂନ୍ୟ vvp_net_fun_t::recv_vec8_pv(vvp_net_ptr_t port, ଅଚଳ vvp_vector8_t&bit,
				 unsigned base, unsigned vwid)
{
      recv_vec4_pv(port, reduce4(bit), base, vwid, 0);
}

ଶୂନ୍ୟ vvp_net_fun_t::recv_real(vvp_net_ptr_t, ଦ୍ୱିଗୁଣ bit, vvp_context_t)
{
      fprintf(stderr, "internal error: %s: recv_real(%f) not implemented\n",
	      typeid(*ଏହି).name(), bit);
      ପ୍ରତିଜ୍ଞା(0);
}

ଶୂନ୍ୟ vvp_net_fun_t::recv_string(vvp_net_ptr_t, ଅଚଳ std::string&bit, vvp_context_t)
{
      fprintf(stderr, "internal error: %s: recv_string(%s) not implemented\n",
	      typeid(*ଏହି).name(), bit.c_str());
      ପ୍ରତିଜ୍ଞା(0);
}

ଶୂନ୍ୟ vvp_net_fun_t::recv_object(vvp_net_ptr_t, vvp_object_t, vvp_context_t)
{
      fprintf(stderr, "internal error: %s: recv_object(...) not implemented\n",
	      typeid(*ଏହି).name());
      ପ୍ରତିଜ୍ଞା(0);
}

ଶୂନ୍ୟ vvp_net_fun_t::force_flag(ତାର୍କିକ)
{
}

/* **** vvp_fun_drive methods **** */

vvp_fun_drive::vvp_fun_drive(unsigned str0, unsigned str1)
{
      ପ୍ରତିଜ୍ଞା(str0 < 8);
      ପ୍ରତିଜ୍ଞା(str1 < 8);

      drive0_ = str0;
      drive1_ = str1;
}

vvp_fun_drive::~vvp_fun_drive()
{
}

ଶୂନ୍ୟ vvp_fun_drive::recv_vec4(vvp_net_ptr_t port, ଅଚଳ vvp_vector4_t&bit,
                              vvp_context_t)
{
      ପ୍ରତିଜ୍ଞା(port.port() == 0);
      port.ptr()->send_vec8(vvp_vector8_t(bit, drive0_, drive1_));
}

ଶୂନ୍ୟ vvp_fun_drive::recv_vec4_pv(vvp_net_ptr_t ptr, ଅଚଳ vvp_vector4_t&bit,
				 unsigned base, unsigned vwid, vvp_context_t ctx)
{
      recv_vec4_pv_(ptr, bit, base, vwid, ctx);
}

/* **** vvp_wide_fun_* methods **** */

vvp_wide_fun_core::vvp_wide_fun_core(vvp_net_t*net, unsigned nports)
{
      ptr_ = net;
      nports_ = nports;
      port_ମୂଲ୍ୟଗୁଡ଼ିକ = 0;
      port_rvalues_ = 0;
}

vvp_wide_fun_core::~vvp_wide_fun_core()
{
      delete[] port_ମୂଲ୍ୟଗୁଡ଼ିକ;
      delete[] port_rvalues_;
}

ଶୂନ୍ୟ vvp_wide_fun_core::propagate_vec4(ଅଚଳ vvp_vector4_t&bit,
				       vvp_time64_t delay)
{
      ଯଦି (delay)
	    schedule_propagate_vector(ptr_, delay, bit);
      ଅନ୍ୟଥା
	    ptr_->send_vec4(bit, 0);
}

ଶୂନ୍ୟ vvp_wide_fun_core::propagate_real(ଦ୍ୱିଗୁଣ bit,
				       vvp_time64_t delay)
{
      ଯଦି (delay) {
	      // schedule_assign_vector(ptr_->out, bit, delay);
	    ପ୍ରତିଜ୍ଞା(0); // Need a real-value version of assign_vector.
      } ଅନ୍ୟଥା {
	    ptr_->send_real(bit, 0);
      }
}


unsigned vvp_wide_fun_core::port_ଗଣନା() ଅଚଳ
{
      ଫେରନ୍ତୁ nports_;
}

vvp_vector4_t& vvp_wide_fun_core::value(unsigned idx)
{
      ପ୍ରତିଜ୍ଞା(idx < nports_);
      ଯଦି (port_ମୂଲ୍ୟଗୁଡ଼ିକ == 0)
	    port_ମୂଲ୍ୟଗୁଡ଼ିକ = ନୂତନ vvp_vector4_t [nports_];
      ଫେରନ୍ତୁ port_ମୂଲ୍ୟଗୁଡ଼ିକ[idx];
}

ଦ୍ୱିଗୁଣ vvp_wide_fun_core::ମୂଲ୍ୟ_r(unsigned idx)
{
      ପ୍ରତିଜ୍ଞା(idx < nports_);
      ଫେରନ୍ତୁ port_rvalues_? port_rvalues_[idx] : 0.0;
}

ଶୂନ୍ୟ vvp_wide_fun_core::recv_real_from_inputs(unsigned)
{
      ପ୍ରତିଜ୍ଞା(0);
}

ଶୂନ୍ୟ vvp_wide_fun_core::dispatch_vec4_from_ନିବେଶ(unsigned port,
						  ଅଚଳ vvp_vector4_t&bit)
{
      ପ୍ରତିଜ୍ଞା(port < nports_);
      ଯଦି (port_ମୂଲ୍ୟଗୁଡ଼ିକ == 0) port_ମୂଲ୍ୟଗୁଡ଼ିକ = ନୂତନ vvp_vector4_t [nports_];
      port_ମୂଲ୍ୟଗୁଡ଼ିକ[port] = bit;
      recv_vec4_from_inputs(port);
}

ଶୂନ୍ୟ vvp_wide_fun_core::dispatch_real_from_ନିବେଶ(unsigned port,
						  ଦ୍ୱିଗୁଣ bit)
{
      ପ୍ରତିଜ୍ଞା(port < nports_);
      ଯଦି (port_rvalues_ == 0) port_rvalues_ = ନୂତନ ଦ୍ୱିଗୁଣ[nports_];
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

ଶୂନ୍ୟ vvp_wide_fun_t::recv_vec4(vvp_net_ptr_t port, ଅଚଳ vvp_vector4_t&bit,
                               vvp_context_t)
{
      unsigned pidx = port_base_ + port.port();
      core_->dispatch_vec4_from_ନିବେଶ(pidx, bit);
}

ଶୂନ୍ୟ vvp_wide_fun_t::recv_vec4_pv(vvp_net_ptr_t ptr, ଅଚଳ vvp_vector4_t&bit,
				  unsigned base, unsigned vwid, vvp_context_t ctx)
{
      recv_vec4_pv_(ptr, bit, base, vwid, ctx);
}

ଶୂନ୍ୟ vvp_wide_fun_t::recv_real(vvp_net_ptr_t port, ଦ୍ୱିଗୁଣ bit,
                               vvp_context_t)
{
      unsigned pidx = port_base_ + port.port();
      core_->dispatch_real_from_ନିବେଶ(pidx, bit);
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

unsigned vvp_scalar_t::strength0() ଅଚଳ
{
      ଫେରନ୍ତୁ STREN0(ମୂଲ୍ୟ_2);
}

unsigned vvp_scalar_t::strength1() ଅଚଳ
{
      ଫେରନ୍ତୁ STREN1(ମୂଲ୍ୟ_2);
}

ostream& operator <<(ostream&out, vvp_scalar_t a)
{
      out << a.strength0() << a.strength1();
      ବାଛ (a.value()) {
	  ସ୍ଥିତି BIT4_0:
	    out << "0";
	    ଭାଙ୍ଗ;
	  ସ୍ଥିତି BIT4_1:
	    out << "1";
	    ଭାଙ୍ଗ;
	  ସ୍ଥିତି BIT4_X:
	    out << "X";
	    ଭାଙ୍ଗ;
	  ସ୍ଥିତି BIT4_Z:
	    out << "Z";
	    ଭାଙ୍ଗ;
      }
      ଫେରନ୍ତୁ out;
}

/*
 * This function is only called if the actual interface function rules
 * out some of the easy cases. If we get here, we can assume that
 * neither of the values is HiZ, and the values are not exactly equal.
 */
vvp_scalar_t fully_featured_resolv_(vvp_scalar_t a, vvp_scalar_t b)
{

      ଯଦି (UNAMBIG(a.ମୂଲ୍ୟ_2) && UNAMBIG(b.ମୂଲ୍ୟ_2)) {

	      /* If both signals are unambiguous, simply choose
		 the stronger. If they have the same strength
		 but different values, then this becomes
		 ambiguous. */

	    ଯଦି ((b.ମୂଲ୍ୟ_2&0x07) > (a.ମୂଲ୍ୟ_2&0x07)) {

		    /* b value is stronger. Take it. */
		  ଫେରନ୍ତୁ b;

	    } ଅନ୍ୟଥା ଯଦି ((b.ମୂଲ୍ୟ_2&0x77) == (a.ମୂଲ୍ୟ_2&0x77)) {

		    // Strengths are the same. Since we know already
		    // that the values are not the same, Make value
		    // into "x".
		  vvp_scalar_t tmp (a);
		  tmp.ମୂଲ୍ୟ_2 = (tmp.ମୂଲ୍ୟ_2&0x77) | 0x80;
		  ଫେରନ୍ତୁ tmp;

	    } ଅନ୍ୟଥା {

		    /* Must be "a" is the stronger one. */
		  ଫେରନ୍ତୁ a;
	    }

      }

	/* If one of the signals is unambiguous, then it
	   will sweep up the weaker parts of the ambiguous
	   signal. The result may be ambiguous, or maybe not. */

      ଯଦି (UNAMBIG(a.ମୂଲ୍ୟ_2)) {
	    vvp_scalar_t res;

	    ଯଦି ((a.ମୂଲ୍ୟ_2&0x70) > (b.ମୂଲ୍ୟ_2&0x70))
		  res.ମୂଲ୍ୟ_2 |= a.ମୂଲ୍ୟ_2&0xf0;
	    ଅନ୍ୟଥା
		  res.ମୂଲ୍ୟ_2 |= b.ମୂଲ୍ୟ_2&0xf0;

	    ଯଦି ((a.ମୂଲ୍ୟ_2&0x07) > (b.ମୂଲ୍ୟ_2&0x07))
		  res.ମୂଲ୍ୟ_2 |= a.ମୂଲ୍ୟ_2&0x0f;
	    ଅନ୍ୟଥା
		  res.ମୂଲ୍ୟ_2 |= b.ମୂଲ୍ୟ_2&0x0f;

	    ଫେରନ୍ତୁ res;

      } ଅନ୍ୟଥା ଯଦି (UNAMBIG(b.ମୂଲ୍ୟ_2)) {

	    vvp_scalar_t res;

	    ଯଦି ((b.ମୂଲ୍ୟ_2&0x70) > (a.ମୂଲ୍ୟ_2&0x70))
		  res.ମୂଲ୍ୟ_2 |= b.ମୂଲ୍ୟ_2&0xf0;
	    ଅନ୍ୟଥା
		  res.ମୂଲ୍ୟ_2 |= a.ମୂଲ୍ୟ_2&0xf0;

	    ଯଦି ((b.ମୂଲ୍ୟ_2&0x07) > (a.ମୂଲ୍ୟ_2&0x07))
		  res.ମୂଲ୍ୟ_2 |= b.ମୂଲ୍ୟ_2&0x0f;
	    ଅନ୍ୟଥା
		  res.ମୂଲ୍ୟ_2 |= a.ମୂଲ୍ୟ_2&0x0f;

	    ଫେରନ୍ତୁ res;

      }


	/* If both signals are ambiguous, then the result
	   has an even wider ambiguity. */

      unsigned tmp = 0;
      ପୂର୍ଣ୍ଣସଂଖ୍ୟା sv1a = (a.ମୂଲ୍ୟ_2 & 0x80) ? STREN1(a.ମୂଲ୍ୟ_2) : - STREN1(a.ମୂଲ୍ୟ_2);
      ପୂର୍ଣ୍ଣସଂଖ୍ୟା sv0a = (a.ମୂଲ୍ୟ_2 & 0x08) ? STREN0(a.ମୂଲ୍ୟ_2) : - STREN0(a.ମୂଲ୍ୟ_2);
      ପୂର୍ଣ୍ଣସଂଖ୍ୟା sv1b = (b.ମୂଲ୍ୟ_2 & 0x80) ? STREN1(b.ମୂଲ୍ୟ_2) : - STREN1(b.ମୂଲ୍ୟ_2);
      ପୂର୍ଣ୍ଣସଂଖ୍ୟା sv0b = (b.ମୂଲ୍ୟ_2 & 0x08) ? STREN0(b.ମୂଲ୍ୟ_2) : - STREN0(b.ମୂଲ୍ୟ_2);

      ପୂର୍ଣ୍ଣସଂଖ୍ୟା sv1 = sv1a;
      ପୂର୍ଣ୍ଣସଂଖ୍ୟା sv0 = sv0a;

      ଯଦି (sv0a > sv1)
	    sv1 = sv0a;
      ଯଦି (sv1b > sv1)
	    sv1 = sv1b;
      ଯଦି (sv0b > sv1)
	    sv1 = sv0b;

      ଯଦି (sv1a < sv0)
	    sv0 = sv1a;
      ଯଦି (sv1b < sv0)
	    sv0 = sv1b;
      ଯଦି (sv0b < sv0)
	    sv0 = sv0b;

      ଯଦି (sv1 > 0) {
	    tmp |= 0x80;
	    tmp |= sv1 << 4;
      } ଅନ୍ୟଥା {
	      /* Set the MSB when both arguments MSBs are set. This
		 can only happen if both one strengths are zero. */
	    tmp |= (a.ମୂଲ୍ୟ_2&b.ମୂଲ୍ୟ_2)&0x80;
	    tmp |= (-sv1) << 4;
      }

      ଯଦି (sv0 > 0) {
	    tmp |= 0x08;
	    tmp |= sv0;
      } ଅନ୍ୟଥା {
	    tmp |= (-sv0);
      }

      vvp_scalar_t res;
      res.ମୂଲ୍ୟ_2 = tmp;

	/* Canonicalize the HiZ value. */
      ଯଦି ((res.ମୂଲ୍ୟ_2&0x77) == 0)
	    res.ମୂଲ୍ୟ_2 = 0;

      ଫେରନ୍ତୁ res;
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

vvp_vector4_t reduce4(ଅଚଳ vvp_vector8_t&that)
{
      vvp_vector4_t out (that.size());
      ପାଇଁ (unsigned idx = 0 ;  idx < out.size() ;  idx += 1)
	    out.set_bit(idx, that.value(idx).value());

      ଫେରନ୍ତୁ out;
}

vvp_bit4_t compare_gtge(ଅଚଳ vvp_vector4_t&lef, ଅଚଳ vvp_vector4_t&rig,
			vvp_bit4_t out_if_equal)
{
      unsigned min_size = lef.size();
      ଯଦି (rig.size() < min_size)
	    min_size = rig.size();

	// If one of the inputs is nil, treat is as all X values, and
	// that makes the result BIT4_X.
      ଯଦି (min_size == 0)
	    ଫେରନ୍ତୁ BIT4_X;

	// As per the IEEE1364 definition of >, >=, < and <=, if there
	// are any X or Z values in either of the operand vectors,
	// then the result of the compare is BIT4_X.

	// Check for X/Z in the left operand
      ଯଦି (lef.has_xz())
	    ଫେରନ୍ତୁ BIT4_X;

	// Check for X/Z in the right operand
      ଯଦି (rig.has_xz())
	    ଫେରନ୍ତୁ BIT4_X;

      ପାଇଁ (unsigned idx = lef.size() ; idx > rig.size() ;  idx -= 1) {
	    ଯଦି (lef.value(idx-1) == BIT4_1)
		  ଫେରନ୍ତୁ BIT4_1;
      }

      ପାଇଁ (unsigned idx = rig.size() ; idx > lef.size() ;  idx -= 1) {
	    ଯଦି (rig.value(idx-1) == BIT4_1)
		  ଫେରନ୍ତୁ BIT4_0;
      }

      ପାଇଁ (unsigned idx = min_size ; idx > 0 ;  idx -= 1) {
	    vvp_bit4_t lv = lef.value(idx-1);
	    vvp_bit4_t rv = rig.value(idx-1);

	    ଯଦି (lv == rv)
		  ଜାରି;

	    ଯଦି (lv == BIT4_1)
		  ଫେରନ୍ତୁ BIT4_1;
	    ଅନ୍ୟଥା
		  ଫେରନ୍ତୁ BIT4_0;
      }

      ଫେରନ୍ତୁ out_if_equal;
}

vvp_bit4_t compare_gtge_signed(ଅଚଳ vvp_vector4_t&a,
			       ଅଚଳ vvp_vector4_t&b,
			       vvp_bit4_t out_if_equal)
{
      ପ୍ରତିଜ୍ଞା(a.size() == b.size());

      unsigned sign_idx = a.size()-1;
      vvp_bit4_t a_sign = a.value(sign_idx);
      vvp_bit4_t b_sign = b.value(sign_idx);

      ଯଦି (bit4_is_xz(a_sign))
	    ଫେରନ୍ତୁ BIT4_X;
      ଯଦି (bit4_is_xz(b_sign))
	    ଫେରନ୍ତୁ BIT4_X;

      ଯଦି (a_sign == b_sign)
	    ଫେରନ୍ତୁ compare_gtge(a, b, out_if_equal);

      ଯଦି (a.has_xz())
	    ଫେରନ୍ତୁ BIT4_X;

      ଯଦି (b.has_xz())
	    ଫେରନ୍ତୁ BIT4_X;

      ଯଦି(a_sign == BIT4_0)
	    ଫେରନ୍ତୁ BIT4_1;
      ଅନ୍ୟଥା
	    ଫେରନ୍ତୁ BIT4_0;
}
