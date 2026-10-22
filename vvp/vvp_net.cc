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
#अगर_घोषित CHECK_WITH_VALGRIND
# include  <valgrind/memcheck.h>
# include  <map>
# include  "sfunc.h"
# include  "udp.h"
# include  "ivl_alloc.h"
#पूर्ण_अगर

using namespace std;

/* This is the size of an unsigned long in bits. This is just a
   convenience macro. */
# define CPU_WORD_BITS (8*माप(अचिन्हित दीर्घ))
# define शीर्ष_BIT (1UL << (CPU_WORD_BITS-1))

permaheap vvp_net_fun_t::heap_;
permaheap vvp_net_fil_t::heap_;

// Allocate around 1Megabyte/chunk.
अटल स्थिर माप_प्रकार VVP_NET_CHUNK = 1024*1024/माप(vvp_net_t);
अटल vvp_net_t*vvp_net_alloc_table = शून्य;
#अगर_घोषित CHECK_WITH_VALGRIND
अटल vvp_net_t **vvp_net_pool = शून्य;
अटल अचिन्हित vvp_net_pool_गिनती = 0;
#पूर्ण_अगर
अटल माप_प्रकार vvp_net_alloc_remaining = 0;
// For statistics, count the vvp_nets allocated and the bytes of alloc
// chunks allocated.
अचिन्हित दीर्घ गिनती_vvp_nets = 0;
माप_प्रकार आकार_vvp_nets = 0;

व्योम* vvp_net_t::चालक नया (माप_प्रकार size)
{
      निश्चित(size == माप(vvp_net_t));
      अगर (vvp_net_alloc_remaining == 0) {
	    vvp_net_alloc_table = ::नया vvp_net_t[VVP_NET_CHUNK];
	    vvp_net_alloc_remaining = VVP_NET_CHUNK;
	    आकार_vvp_nets += size*VVP_NET_CHUNK;
#अगर_घोषित CHECK_WITH_VALGRIND
	    VALGRIND_MAKE_MEM_NOACCESS(vvp_net_alloc_table, size*VVP_NET_CHUNK);
	    VALGRIND_CREATE_MEMPOOL(vvp_net_alloc_table, 0, 0);
	    vvp_net_pool_गिनती += 1;
	    vvp_net_pool = static_cast<vvp_net_t **>(पुनः_स्मृति(vvp_net_pool,
	                   vvp_net_pool_गिनती*माप(vvp_net_t **)));
	    vvp_net_pool[vvp_net_pool_गिनती-1] = vvp_net_alloc_table;
#पूर्ण_अगर
      }

      vvp_net_t*return_this = vvp_net_alloc_table;
#अगर_घोषित CHECK_WITH_VALGRIND
      VALGRIND_MEMPOOL_ALLOC(vvp_net_pool[vvp_net_pool_गिनती-1],
                             return_this, size);
      return_this->pool = vvp_net_pool[vvp_net_pool_गिनती-1];
#पूर्ण_अगर
      vvp_net_alloc_table += 1;
      vvp_net_alloc_remaining -= 1;
      गिनती_vvp_nets += 1;
      वापस return_this;
}

#अगर_घोषित CHECK_WITH_VALGRIND
अटल map<vvp_net_t*, bool> vvp_net_map;
अटल map<sfunc_core*, bool> sfunc_map;
अटल map<vvp_udp_fun_core*, bool> udp_map;
अटल map<resolv_core*, bool> resolv_map;
अटल vvp_net_t **local_net_pool = 0;
अटल अचिन्हित local_net_pool_गिनती = 0;

व्योम pool_local_net(vvp_net_t*net)
{
      local_net_pool_गिनती += 1;
      local_net_pool = static_cast<vvp_net_t **>(पुनः_स्मृति(local_net_pool,
                       local_net_pool_गिनती*माप(vvp_net_t **)));
      local_net_pool[local_net_pool_गिनती-1] = net;
}

व्योम vvp_net_delete(vvp_net_t *item)
{
      vvp_net_map[item] = true;
      अगर (sfunc_core*tmp = dynamic_cast<sfunc_core*> (item->fun)) {
	    sfunc_map[tmp] = true;
      }
      अगर (vvp_udp_fun_core*tmp = dynamic_cast<vvp_udp_fun_core*> (item->fun)) {
	    udp_map[tmp] = true;
      }
      अगर (resolv_core*tmp = dynamic_cast<resolv_core*> (item->fun)) {
	    resolv_map[tmp] = true;
      }
	/* Only delete static object variables. */
      अगर (vvp_fun_signal_object_sa*tmp = dynamic_cast<vvp_fun_signal_object_sa*> (item->fun)) {
	    मिटाओ tmp;
      }
	/* Only delete static string variables. */
      अगर (vvp_fun_signal_string_sa*tmp = dynamic_cast<vvp_fun_signal_string_sa*> (item->fun)) {
	    मिटाओ tmp;
      }
}

व्योम vvp_net_pool_delete()
{
      अचिन्हित दीर्घ vvp_nets_del = 0;

      क्रम (अचिन्हित idx = 0; idx < local_net_pool_गिनती; idx += 1) {
	    vvp_net_delete(local_net_pool[idx]);
      }
      मुक्त(local_net_pool);
      local_net_pool = 0;
      local_net_pool_गिनती = 0;

      map<vvp_net_t*, bool>::iterator iter;
      क्रम (iter = vvp_net_map.begin(); iter != vvp_net_map.end(); ++ iter ) {
	    vvp_nets_del += 1;
	    VALGRIND_MEMPOOL_FREE(iter->first->pool, iter->first);
      }
      vvp_net_map.सफा();

      map<sfunc_core*, bool>::iterator siter;
      क्रम (siter = sfunc_map.begin(); siter != sfunc_map.end(); ++ siter ) {
	    मिटाओ siter->first;
      }
      sfunc_map.सफा();

      map<vvp_udp_fun_core*, bool>::iterator uiter;
      क्रम (uiter = udp_map.begin(); uiter != udp_map.end(); ++ uiter ) {
	    मिटाओ uiter->first;
      }
      udp_map.सफा();

      map<resolv_core*, bool>::iterator riter;
      क्रम (riter = resolv_map.begin(); riter != resolv_map.end(); ++ riter ) {
	    मिटाओ riter->first;
      }
      resolv_map.सफा();

      अगर (RUNNING_ON_VALGRIND && (vvp_nets_del != गिनती_vvp_nets)) {
	    ख_साफ(शून्य);
	    VALGRIND_PRINTF("Error: vvp missed deleting %ld of %lu net(s).",
	                    (दीर्घ) गिनती_vvp_nets - vvp_nets_del,
	                    गिनती_vvp_nets);
      }

      क्रम (अचिन्हित idx = 0; idx < vvp_net_pool_गिनती; idx += 1) {
	    VALGRIND_DESTROY_MEMPOOL(vvp_net_pool[idx]);
	    ::मिटाओ [] vvp_net_pool[idx];
      }
      मुक्त(vvp_net_pool);
      vvp_net_pool = शून्य;
      vvp_net_pool_गिनती = 0;
}
#पूर्ण_अगर

व्योम vvp_net_t::चालक मिटाओ(व्योम*)
{
      निश्चित(0);
}

vvp_net_t::vvp_net_t()
: out_(vvp_net_ptr_t(0,0))
{
      fun = 0;
      fil = 0;
}

व्योम vvp_net_t::link(vvp_net_ptr_t port_to_link)
{
      vvp_net_t*net = port_to_link.ptr();

	// Connect nodes with vvp_fun_modpath_src to the head of the
	// linked list, so that vvp_fun_modpath_src are always evaluated
	// before their respective vvp_fun_modpath
      अगर (dynamic_cast<vvp_fun_modpath_src*>(net->fun)) {
	    net->port[port_to_link.port()] = out_;
	    out_ = port_to_link;
	// All other nodes connect after the last vvp_fun_modpath_src
      } अन्यथा {
	      // List is empty or first node has no vvp_fun_modpath_src
	      // then just insert the node into the head of linked list
	    अगर (out_.ptr() == nullptr || ! dynamic_cast<vvp_fun_modpath_src*>(out_.ptr()->fun)) {
		  net->port[port_to_link.port()] = out_;
		  out_ = port_to_link;
		  वापस;
	    }

	    vvp_net_ptr_t cur = out_;
	    vvp_net_ptr_t prev = vvp_net_ptr_t(nullptr,0);

	      // Traverse through linked list
	    जबतक (dynamic_cast<vvp_fun_modpath_src*>(cur.ptr()->fun)) {
		  prev = cur;
		  cur = cur.ptr()->port[cur.port()];
		  अगर (! cur.ptr()) अवरोध; // End of list
	    }

	    निश्चित(prev.ptr());

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
व्योम vvp_net_t::unlink(vvp_net_ptr_t dst_ptr)
{
      vvp_net_t*net = dst_ptr.ptr();
      अचिन्हित net_port = dst_ptr.port();

      अगर (out_ == dst_ptr) {
	      /* If the drive fan-out list starts with this pointer,
		 then the unlink is easy. Pull the list forward. */
	    out_ = net->port[net_port];
      } अन्यथा अगर (! out_.nil()) {
	      /* Scan the linked list, looking for the net_ptr_t
		 pointer *before* the one we wish to remove. */
	    vvp_net_ptr_t cur = out_;
	    निश्चित(!cur.nil());
	    vvp_net_t*cur_net = cur.ptr();
	    अचिन्हित cur_port = cur.port();
	    जबतक (cur_net && cur_net->port[cur_port] != dst_ptr) {
		  cur = cur_net->port[cur_port];
		  cur_net = cur.ptr();
		  cur_port = cur.port();
	    }
	      /* Unlink. */
	    अगर (cur_net) cur_net->port[cur_port] = net->port[net_port];
      }

      net->port[net_port] = vvp_net_ptr_t(0,0);
}

व्योम vvp_net_t::गिनती_drivers(अचिन्हित idx, अचिन्हित counts[4])
{
      counts[0] = 0;
      counts[1] = 0;
      counts[2] = 0;
      counts[3] = 0;

        /* $countdrivers can only be used on wires. */
      vvp_wire_base*wire=dynamic_cast<vvp_wire_base*>(fil);
      निश्चित(wire);

      अगर (wire->is_forced(idx))
            counts[3] = 1;

        /* If the net has multiple drivers, we need to interrogate the
           resolver network to get the driven values. */
      अगर (resolv_core*resolver = dynamic_cast<resolv_core*>(fun)) {
            resolver->गिनती_drivers(idx, counts);
            वापस;
      }
      अगर (vvp_island_port*resolver = dynamic_cast<vvp_island_port*>(fun)) {
            resolver->गिनती_drivers(idx, counts);
            वापस;
      }

        /* If the functor is not a resolver, there is only one driver, so
           we can just interrogate the filter to get the driven value. */
      update_driver_counts(wire->driven_मूल्य(idx), counts);
}

व्योम vvp_net_fun_t::चालक मिटाओ(व्योम*)
{
#अगर_अघोषित CHECK_WITH_VALGRIND
      निश्चित(0);
#पूर्ण_अगर
}


vvp_net_fil_t::vvp_net_fil_t()
{
      force_link_ = 0;
      force_propagate_ = false;
      गिनती_filters += 1;
}

vvp_net_fil_t::~vvp_net_fil_t()
{
      निश्चित(force_link_ == 0);
}

vvp_net_fil_t::prop_t vvp_net_fil_t::filter_vec4(स्थिर vvp_vector4_t&,
                                                 vvp_vector4_t&,
                                                 अचिन्हित, अचिन्हित)
{
      वापस PROP;
}

vvp_net_fil_t::prop_t vvp_net_fil_t::filter_vec8(स्थिर vvp_vector8_t&,
                                                 vvp_vector8_t&,
                                                 अचिन्हित, अचिन्हित)
{
      वापस PROP;
}

vvp_net_fil_t::prop_t vvp_net_fil_t::filter_real(द्विगुन&)
{
      वापस PROP;
}

vvp_net_fil_t::prop_t vvp_net_fil_t::filter_string(स्थिर string&)
{
      वापस PROP;
}

vvp_net_fil_t::prop_t vvp_net_fil_t::filter_object(vvp_object_t&)
{
      वापस PROP;
}

व्योम vvp_net_fil_t::force_mask(स्थिर vvp_vector2_t&mask)
{
      अगर (force_mask_.size() == 0)
	    force_mask_ = vvp_vector2_t(vvp_vector2_t::FILL0, mask.size());

      निश्चित(force_mask_.size() == mask.size());
      क्रम (अचिन्हित idx = 0 ; idx < mask.size() ; idx += 1) {
	    अगर (mask.value(idx) == 0)
		  जारी;

	    force_mask_.set_bit(idx, 1);
	    force_propagate_ = true;
      }
}

व्योम vvp_net_fil_t::release_mask(स्थिर vvp_vector2_t&mask)
{
      अगर (force_mask_.size() == 0)
	    वापस;

      निश्चित(force_mask_.size() == mask.size());
      क्रम (अचिन्हित idx = 0 ; idx < mask.size() ; idx += 1) {
	    अगर (mask.value(idx))
		  force_mask_.set_bit(idx, 0);
      }

      अगर (force_mask_.is_zero())
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
व्योम vvp_net_fil_t::force_link(vvp_net_t*dst, vvp_net_t*src)
{
      निश्चित(dst->fil == यह);

      अगर (force_link_ == 0) {
	    force_link_ = नया vvp_net_t;
	      // Use port[3] to hold the force destination.
	    force_link_->port[3] = vvp_net_ptr_t(dst, 0);
	    force_link_->port[2] = vvp_net_ptr_t(0,0);
	    force_link_->fun = नया vvp_fun_force;
      }

      force_unlink();
      निश्चित(force_link_->port[2] == vvp_net_ptr_t(0,0));

	// Use port[2] to hold the force source.
      force_link_->port[2] = vvp_net_ptr_t(src,0);

      vvp_net_ptr_t dst_ptr(force_link_, 0);
      src->link(dst_ptr);
}

व्योम vvp_net_fil_t::चालक मिटाओ(व्योम*)
{
      निश्चित(0);
}

व्योम vvp_net_fil_t::force_unlink(व्योम)
{
      अगर (force_link_ == 0) वापस;
      vvp_net_t*src = force_link_->port[2].ptr();
      अगर (src == 0) वापस;

      src->unlink(vvp_net_ptr_t(force_link_,0));
      force_link_->port[2] = vvp_net_ptr_t(0,0);
}

/* *** BIT operations *** */
vvp_bit4_t add_with_carry(vvp_bit4_t a, vvp_bit4_t b, vvp_bit4_t&c)
{
      अगर (bit4_is_xz(a) || bit4_is_xz(b) || bit4_is_xz(c)) {
	    c = BIT4_X;
	    वापस BIT4_X;
      }

	// NOTE: This relies on the facts that XZ values have been
	// weeded out, and that BIT4_1 is 1 and BIT4_0 is 0.
      पूर्णांक योग = (पूर्णांक)a + (पूर्णांक)b + (पूर्णांक)c;

      चयन (योग) {
	  हाल 0:
	      // c must already be 0.
	    वापस BIT4_0;
	  हाल 1:
	    c = BIT4_0;
	    वापस BIT4_1;
	  हाल 2:
	    c = BIT4_1;
	    वापस BIT4_0;
	  हाल 3:
	    c = BIT4_1;
	    वापस BIT4_1;
      }
      ख_लिखो(मानक_त्रुटि, "Incorrect result %d.\n", योग);
      निश्चित(0);
      वापस BIT4_X;
}

vvp_bit4_t scalar_to_bit4(PLI_INT32 scalar)
{
      चयन(scalar) {
	  हाल vpi0:
	    वापस BIT4_0;
	  हाल vpi1:
	    वापस BIT4_1;
	  हाल vpiX:
	    वापस BIT4_X;
	  हाल vpiZ:
	    वापस BIT4_Z;
      }
      ख_लिखो(मानक_त्रुटि, "Unsupported scalar value %d.\n", (पूर्णांक)scalar);
      निश्चित(0);
      वापस BIT4_X;
}

vvp_bit4_t चालक ^ (vvp_bit4_t a, vvp_bit4_t b)
{
      अगर (bit4_is_xz(a))
	    वापस BIT4_X;
      अगर (bit4_is_xz(b))
	    वापस BIT4_X;
      अगर (a == BIT4_0)
	    वापस b;
      अगर (b == BIT4_0)
	    वापस a;
      वापस BIT4_0;
}

ब_प्रवाह& चालक<<(ब_प्रवाह&out, vvp_bit4_t bit)
{
      चयन (bit) {
	  हाल BIT4_0:
	    out << "0";
	    अवरोध;
	  हाल BIT4_1:
	    out << "1";
	    अवरोध;
	  हाल BIT4_X:
	    out << "X";
	    अवरोध;
	  हाल BIT4_Z:
	    out << "Z";
	    अवरोध;
	  शेष:
	    out << "?";
	    अवरोध;
      }
      वापस out;
}

प्रकार अचिन्हित लघु edge_t;

अंतरभूत edge_t VVP_EDGE(vvp_bit4_t from, vvp_bit4_t to)
{
      वापस 1 << ((from << 2) | to);
}

स्थिर edge_t vvp_edge_posedge
      = VVP_EDGE(BIT4_0,BIT4_1)
      | VVP_EDGE(BIT4_0,BIT4_X)
      | VVP_EDGE(BIT4_0,BIT4_Z)
      | VVP_EDGE(BIT4_X,BIT4_1)
      | VVP_EDGE(BIT4_Z,BIT4_1)
      ;

स्थिर edge_t vvp_edge_negedge
      = VVP_EDGE(BIT4_1,BIT4_0)
      | VVP_EDGE(BIT4_1,BIT4_X)
      | VVP_EDGE(BIT4_1,BIT4_Z)
      | VVP_EDGE(BIT4_X,BIT4_0)
      | VVP_EDGE(BIT4_Z,BIT4_0)
      ;

पूर्णांक edge(vvp_bit4_t from, vvp_bit4_t to)
{
      edge_t mask = VVP_EDGE(from, to);
      अगर (mask & vvp_edge_posedge)
	    वापस 1;
      अगर (mask & vvp_edge_negedge)
	    वापस -1;
      वापस 0;
}

अचिन्हित दीर्घ multiply_with_carry(अचिन्हित दीर्घ a, अचिन्हित दीर्घ b,
				  अचिन्हित दीर्घ&carry)
{
      स्थिर अचिन्हित दीर्घ mask = (1UL << (CPU_WORD_BITS/2)) - 1;
      अचिन्हित दीर्घ a0 = a & mask;
      अचिन्हित दीर्घ a1 = (a >> (CPU_WORD_BITS/2)) & mask;
      अचिन्हित दीर्घ b0 = b & mask;
      अचिन्हित दीर्घ b1 = (b >> (CPU_WORD_BITS/2)) & mask;

      अचिन्हित दीर्घ tmp = a0 * b0;

      अचिन्हित दीर्घ r00 = tmp & mask;
      अचिन्हित दीर्घ c00 = (tmp >> (CPU_WORD_BITS/2)) & mask;

      tmp = a0 * b1;

      अचिन्हित दीर्घ r01 = tmp & mask;
      अचिन्हित दीर्घ c01 = (tmp >> (CPU_WORD_BITS/2)) & mask;

      tmp = a1 * b0;

      अचिन्हित दीर्घ r10 = tmp & mask;
      अचिन्हित दीर्घ c10 = (tmp >> (CPU_WORD_BITS/2)) & mask;

      tmp = a1 * b1;

      अचिन्हित दीर्घ r11 = tmp & mask;
      अचिन्हित दीर्घ c11 = (tmp >> (CPU_WORD_BITS/2)) & mask;

      अचिन्हित दीर्घ r1 = c00 + r01 + r10;
      अचिन्हित दीर्घ r2 = (r1 >> (CPU_WORD_BITS/2)) & mask;
      r1 &= mask;
      r2 += c01 + c10 + r11;
      अचिन्हित दीर्घ r3 = (r2 >> (CPU_WORD_BITS/2)) & mask;
      r2 &= mask;
      r3 += c11;
      r3 &= mask;

      carry = (r3 << (CPU_WORD_BITS/2)) + r2;
      वापस (r1 << (CPU_WORD_BITS/2)) + r00;
}


व्योम vvp_send_vec8(vvp_net_ptr_t ptr, स्थिर vvp_vector8_t&val)
{
      जबतक (vvp_net_t*cur = ptr.ptr()) {
	    vvp_net_ptr_t अग्र_val = cur->port[ptr.port()];

	    अगर (cur->fun)
		  cur->fun->recv_vec8(ptr, val);

	    ptr = अग्र_val;
      }
}

व्योम vvp_send_real(vvp_net_ptr_t ptr, द्विगुन val, vvp_context_t context)
{
      जबतक (vvp_net_t*cur = ptr.ptr()) {
	    vvp_net_ptr_t अग्र_val = cur->port[ptr.port()];

	    अगर (cur->fun)
		  cur->fun->recv_real(ptr, val, context);

	    ptr = अग्र_val;
      }
}

स्थिर vvp_vector4_t vvp_vector4_t::nil;

व्योम vvp_vector4_t::copy_bits(स्थिर vvp_vector4_t&that)
{

      अगर (आकार_2 == that.आकार_2) {
	    अगर (आकार_2 > BITS_PER_WORD) {
		  अचिन्हित शब्दगण = (आकार_2+BITS_PER_WORD-1) / BITS_PER_WORD;
		  क्रम (अचिन्हित idx = 0 ;  idx < शब्दगण ;  idx += 1)
			abits_ptr_[idx] = that.abits_ptr_[idx];
		  क्रम (अचिन्हित idx = 0 ;  idx < शब्दगण ;  idx += 1)
			bbits_ptr_[idx] = that.bbits_ptr_[idx];
	    } अन्यथा {
		  abits_val_ = that.abits_val_;
		  bbits_val_ = that.bbits_val_;
	    }
	    वापस;
      }

	/* Now we know that the sizes of this and that are definitely
	   different. We can use that in code below. In any case, we
	   need to copy only the smaller of the sizes. */

	/* If source and destination are both short, then mask/copy
	   the bit values. */
      अगर (आकार_2 <= BITS_PER_WORD && that.आकार_2 <= BITS_PER_WORD) {
	    अचिन्हित bits_to_copy = (that.आकार_2 < आकार_2) ? that.आकार_2 : आकार_2;
	    अचिन्हित दीर्घ mask = ((bits_to_copy < BITS_PER_WORD) ? (1UL << bits_to_copy) :
	                                                           (0UL)) - 1UL;
	    abits_val_ &= ~mask;
	    bbits_val_ &= ~mask;
	    abits_val_ |= that.abits_val_&mask;
	    bbits_val_ |= that.bbits_val_&mask;
	    वापस;
      }

	/* Now we know that either source or destination are long. If
	   the destination is short, then mask/copy from the low word
	   of the long source. */
      अगर (आकार_2 <= BITS_PER_WORD) {
	    abits_val_ = that.abits_ptr_[0];
	    bbits_val_ = that.bbits_ptr_[0];
	    अगर (आकार_2 < BITS_PER_WORD) {
		  अचिन्हित दीर्घ mask = (1UL << आकार_2) - 1UL;
		  abits_val_ &= mask;
		  bbits_val_ &= mask;
	    }
	    वापस;
      }

	/* Now we know that the destination must be long. If the
	   source is short, then mask/copy from its value. */
      अगर (that.आकार_2 <= BITS_PER_WORD) {
	    अचिन्हित दीर्घ mask;
	    अगर (that.आकार_2 < BITS_PER_WORD) {
		  mask = (1UL << that.आकार_2) - 1UL;
		  abits_ptr_[0] &= ~mask;
		  bbits_ptr_[0] &= ~mask;
	    } अन्यथा {
		  mask = -1UL;
	    }
	    abits_ptr_[0] |= that.abits_val_&mask;
	    bbits_ptr_[0] |= that.bbits_val_&mask;
	    वापस;
      }

	/* Finally, we know that source and destination are long. copy
	   words until we get to the last. */
      अचिन्हित bits_to_copy = (that.आकार_2 < आकार_2) ? that.आकार_2 : आकार_2;
      अचिन्हित word = 0;
      जबतक (bits_to_copy >= BITS_PER_WORD) {
	    abits_ptr_[word] = that.abits_ptr_[word];
	    bbits_ptr_[word] = that.bbits_ptr_[word];
	    bits_to_copy -= BITS_PER_WORD;
	    word += 1;
      }
      अगर (bits_to_copy > 0) {
	    अचिन्हित दीर्घ mask = (1UL << bits_to_copy) - 1UL;
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
व्योम vvp_vector4_t::copy_from_big_(स्थिर vvp_vector4_t&that)
{
      अचिन्हित शब्दगण = (आकार_2+BITS_PER_WORD-1) / BITS_PER_WORD;
      abits_ptr_ = नया अचिन्हित दीर्घ[2*शब्दगण];
      bbits_ptr_ = abits_ptr_ + शब्दगण;

      क्रम (अचिन्हित idx = 0 ;  idx < शब्दगण ;  idx += 1)
	    abits_ptr_[idx] = that.abits_ptr_[idx];
      क्रम (अचिन्हित idx = 0 ;  idx < शब्दगण ;  idx += 1)
	    bbits_ptr_[idx] = that.bbits_ptr_[idx];
}

/*
 * The copy_inverted_from_ method is just like the copy_from_ method,
 * except that we combine that with an invert. This allows the ~ and
 * the assignment to be blended in many common cases.
 */
व्योम vvp_vector4_t::copy_inverted_from_(स्थिर vvp_vector4_t&that)
{
      आकार_2 = that.आकार_2;
      अगर (आकार_2 > BITS_PER_WORD) {
	    अचिन्हित शब्दगण = (आकार_2+BITS_PER_WORD-1) / BITS_PER_WORD;
	    abits_ptr_ = नया अचिन्हित दीर्घ[2*शब्दगण];
	    bbits_ptr_ = abits_ptr_ + शब्दगण;

	    अचिन्हित remaining = आकार_2;
	    अचिन्हित idx = 0;
	    जबतक (remaining >= BITS_PER_WORD) {
		  abits_ptr_[idx] = that.bbits_ptr_[idx] | ~that.abits_ptr_[idx];
		  idx += 1;
		  remaining -= BITS_PER_WORD;
	    }
	    अगर (remaining > 0) {
		  अचिन्हित दीर्घ mask = (1UL<<remaining) - 1UL;
		  abits_ptr_[idx] = mask & (that.bbits_ptr_[idx] | ~that.abits_ptr_[idx]);
	    }

	    क्रम (idx = 0 ;  idx < शब्दगण ;  idx += 1)
		  bbits_ptr_[idx] = that.bbits_ptr_[idx];

      } अन्यथा {
	    अचिन्हित दीर्घ mask = (आकार_2<BITS_PER_WORD)? (1UL<<आकार_2)-1UL : -1UL;
	    abits_val_ = mask & (that.bbits_val_ | ~that.abits_val_);
	    bbits_val_ = that.bbits_val_;
      }
}

/* Make sure to set size_ before calling this routine. */
व्योम vvp_vector4_t::allocate_शब्दगण(अचिन्हित दीर्घ inita, अचिन्हित दीर्घ initb)
{
      अगर (आकार_2 > BITS_PER_WORD) {
	    अचिन्हित cnt = (आकार_2 + BITS_PER_WORD - 1) / BITS_PER_WORD;
	    abits_ptr_ = नया अचिन्हित दीर्घ[2*cnt];
	    bbits_ptr_ = abits_ptr_ + cnt;
	    क्रम (अचिन्हित idx = 0 ;  idx < cnt ;  idx += 1)
		  abits_ptr_[idx] = inita;
	    क्रम (अचिन्हित idx = 0 ;  idx < cnt ;  idx += 1)
		  bbits_ptr_[idx] = initb;

      } अन्यथा {
	    abits_val_ = inita;
	    bbits_val_ = initb;
      }
}

vvp_vector4_t::vvp_vector4_t(अचिन्हित आकार_3, द्विगुन val)
: आकार_2(आकार_3)
{
      bool is_neg = false;
      द्विगुन fraction;
      पूर्णांक exponent;

	/* We return 'bx for a NaN or +/- infinity. */
      अगर (val != val || (val && (val == 0.5*val)))  {
	    allocate_शब्दगण(WORD_X_ABITS, WORD_X_BBITS);
	    वापस;
      }

	/* Convert to a positive result. */
      अगर (val < 0.0) {
	    is_neg = true;
	    val = -val;
      }
      allocate_शब्दगण(WORD_0_ABITS, WORD_0_BBITS);

	/* Get the exponent and fractional part of the number. */
      fraction = frexp(val, &exponent);

	/* If the value is small enough just use lround(). */
      अगर (exponent < BITS_PER_WORD-2) {
	    अगर (is_neg) यह->invert();  // Invert the bits if negative.
	    दीर्घ sval = lround(val);
	    अगर (is_neg) sval = -sval;
	      /* This requires that 0 and 1 have the same bbit value. */
	    अगर (आकार_2 > BITS_PER_WORD) {
		  abits_ptr_[0] = sval;
	    } अन्यथा {
		  abits_val_ = sval;
	    }
	    वापस;
      }

      अचिन्हित nwords = (exponent-1) / BITS_PER_WORD;
      अचिन्हित my_शब्दगण = (आकार_2 + BITS_PER_WORD - 1) / BITS_PER_WORD - 1;

      fraction = ldexp(fraction, (exponent-1) % BITS_PER_WORD + 1);

	/* Skip any leading bits. */
      क्रम (पूर्णांक idx = (चिन्हित) nwords; idx > (चिन्हित) my_शब्दगण; idx -=1) {
	    अचिन्हित दीर्घ bits = (अचिन्हित दीर्घ) fraction;
	    fraction = fraction - (द्विगुन) bits;
	    fraction = ldexp(fraction, BITS_PER_WORD);
      }

	/* Convert the remaining bits as appropriate. */
      अगर (my_शब्दगण == 0) {
		  अचिन्हित दीर्घ bits = (अचिन्हित दीर्घ) fraction;
		  abits_val_ = bits;
		  fraction = fraction - (द्विगुन) bits;
		    /* Round any fractional part up. */
		  अगर (fraction >= 0.5) *यह += (int64_t) 1;
      } अन्यथा {
	    अगर (nwords < my_शब्दगण) my_शब्दगण = nwords;
	    क्रम (पूर्णांक idx = (चिन्हित)my_शब्दगण; idx >= 0; idx -= 1) {
		  अचिन्हित दीर्घ bits = (अचिन्हित दीर्घ) fraction;
		  abits_ptr_[idx] = bits;
		  fraction = fraction - (द्विगुन) bits;
		  fraction = ldexp(fraction, BITS_PER_WORD);
	    }
	      /* Round any fractional part up. */
	    अगर (fraction >= ldexp(0.5, BITS_PER_WORD)) *यह += (int64_t) 1;
       }

	/* Convert to a negative number if needed. */
      अगर (is_neg) {
	    यह->invert();
	    *यह += (int64_t) 1;
      }
}

vvp_vector4_t::vvp_vector4_t(स्थिर vvp_vector4_t&that,
			    अचिन्हित adr, अचिन्हित wid)
{
	// Set up and initialize the destination.
      आकार_2 = wid;
      allocate_शब्दगण(WORD_X_ABITS, WORD_X_BBITS);

	// Special case: selecting from far beyond the source vector,
	// to the result is all X bits. We're done.
      अगर (adr >= that.आकार_2)
	    वापस;

	// Special case: The source is not quite big enough to supply
	// all bits, so get the bits that we can. The remainder will
	// be left at BIT4_X.
      अगर ((adr + wid) > that.आकार_2) {
	    अचिन्हित use_wid = that.आकार_2 - adr;
	    क्रम (अचिन्हित idx = 0 ; idx < use_wid ; idx += 1)
		  set_bit(idx, that.value(adr+idx));

	    वापस;
      }

	// At the point, we know that the source part is entirely
	// contained in the source vector.
	// assert((adr + wid) <= that.size_);

      अगर (wid > BITS_PER_WORD) {
	      /* In this case, the subvector and the source vector are
		 long. Do the transfer reasonably efficiently. */
	    अचिन्हित ptr = adr / BITS_PER_WORD;
	    अचिन्हित दीर्घ off = adr % BITS_PER_WORD;
	    अचिन्हित दीर्घ noff = BITS_PER_WORD - off;
	    अचिन्हित दीर्घ lmask = (1UL << off) - 1UL;
	    अचिन्हित trans = 0;
	    अचिन्हित dst = 0;
	    जबतक (trans < wid) {
		    // The low bits of the result.
		  abits_ptr_[dst] = (that.abits_ptr_[ptr] & ~lmask) >> off;
		  bbits_ptr_[dst] = (that.bbits_ptr_[ptr] & ~lmask) >> off;
		  trans += noff;

		  अगर (trans >= wid)
			अवरोध;

		  ptr += 1;

		    // The high bits of the result. Skip this if the
		    // source and destination are perfectly aligned.
		  अगर (noff != BITS_PER_WORD) {
			abits_ptr_[dst] |= (that.abits_ptr_[ptr]&lmask) << noff;
			bbits_ptr_[dst] |= (that.bbits_ptr_[ptr]&lmask) << noff;
			trans += off;
		  }

		  dst += 1;
	    }

      } अन्यथा अगर (that.आकार_2 > BITS_PER_WORD) {
	      /* In this case, the subvector fits in a single word,
		 but the source is large. */
	    अचिन्हित ptr = adr / BITS_PER_WORD;
	    अचिन्हित दीर्घ off = adr % BITS_PER_WORD;
	    अचिन्हित trans = BITS_PER_WORD - off;
	    अगर (trans > wid)
		  trans = wid;

	    अगर (trans == BITS_PER_WORD) {
		    // Very special case: Copy exactly 1 perfectly
		    // aligned word.
		  abits_val_ = that.abits_ptr_[ptr];
		  bbits_val_ = that.bbits_ptr_[ptr];

	    } अन्यथा {
		    // lmask is the low bits of the destination,
		    // masked into the source.
		  अचिन्हित दीर्घ lmask = (1UL<<trans) - 1UL;
		  lmask <<= off;

		    // The low bits of the result.
		  abits_val_ = (that.abits_ptr_[ptr] & lmask) >> off;
		  bbits_val_ = (that.bbits_ptr_[ptr] & lmask) >> off;

		  अगर (trans < wid) {
			  // If there are more bits, then get them
			  // from the bottom of the next word of the
			  // source.
			अचिन्हित दीर्घ hmask = (1UL << (wid-trans)) - 1UL;

			  // The high bits of the result.
			abits_val_ |= (that.abits_ptr_[ptr+1]&hmask) << trans;
			bbits_val_ |= (that.bbits_ptr_[ptr+1]&hmask) << trans;
		  }
	    }

      } अन्यथा अगर (आकार_2 == BITS_PER_WORD) {
	      /* We know that source and destination are short. If the
		 destination is a full word, then we know the copy is
		 aligned and complete. */
	    abits_val_ = that.abits_val_;
	    bbits_val_ = that.bbits_val_;

      } अन्यथा {
	      /* Finally, the source and destination vectors are both
		 short, so there is a single mask/shift/copy. */
	    अचिन्हित दीर्घ mask = (1UL << आकार_2) - 1UL;
	    mask <<= adr;

	    abits_val_ = (that.abits_val_ & mask) >> adr;
	    bbits_val_ = (that.bbits_val_ & mask) >> adr;
      }

}

/*
 * Change the size of the vvp_vector4_t vector to the new size. Copy
 * the old values, as many as well fit, into the new vector.
 */
व्योम vvp_vector4_t::resize(अचिन्हित newsize, vvp_bit4_t pad_bit)
{
      अगर (आकार_2 == newsize)
	    वापस;

      अचिन्हित cnt = (आकार_2 + BITS_PER_WORD - 1) / BITS_PER_WORD;
      अचिन्हित दीर्घ word_pad_abits = 0;
      अचिन्हित दीर्घ word_pad_bbits = 0;
      चयन (pad_bit) {
	  हाल BIT4_0:
	    word_pad_abits = WORD_0_ABITS;
	    word_pad_bbits = WORD_0_BBITS;
	    अवरोध;
	  हाल BIT4_1:
	    word_pad_abits = WORD_1_ABITS;
	    word_pad_bbits = WORD_1_BBITS;
	    अवरोध;
	  हाल BIT4_X:
	    word_pad_abits = WORD_X_ABITS;
	    word_pad_bbits = WORD_X_BBITS;
	    अवरोध;
	  हाल BIT4_Z:
	    word_pad_abits = WORD_Z_ABITS;
	    word_pad_bbits = WORD_Z_BBITS;
	    अवरोध;
      }

      अगर (newsize > BITS_PER_WORD) {
	    अचिन्हित newcnt = (newsize + BITS_PER_WORD - 1) / BITS_PER_WORD;
	    अगर (newcnt == cnt) {
		    // If the word count doesn't change, then there is
		    // no need for re-allocation so we are done now.
		  अगर (newsize > आकार_2) {
			अगर (अचिन्हित fill = आकार_2 % BITS_PER_WORD) {
			      abits_ptr_[cnt-1] &= ~((-1UL) << fill);
			      bbits_ptr_[cnt-1] &= ~((-1UL) << fill);
			      abits_ptr_[cnt-1] |= word_pad_abits << fill;
			      bbits_ptr_[cnt-1] |= word_pad_bbits << fill;
			}
		  }
		  आकार_2 = newsize;
		  वापस;
	    }

	    अचिन्हित दीर्घ*newbits = नया अचिन्हित दीर्घ[2*newcnt];

	    अगर (cnt > 1) {
		  अचिन्हित trans = cnt;
		  अगर (trans > newcnt)
			trans = newcnt;

		  क्रम (अचिन्हित idx = 0 ;  idx < trans ;  idx += 1)
			newbits[idx] = abits_ptr_[idx];
		  क्रम (अचिन्हित idx = 0 ;  idx < trans ;  idx += 1)
			newbits[newcnt+idx] = bbits_ptr_[idx];

		  मिटाओ[]abits_ptr_;

	    } अन्यथा {
		  newbits[0] = abits_val_;
		  newbits[newcnt] = bbits_val_;
	    }

	    अगर (newsize > आकार_2) {
		  अगर (अचिन्हित fill = आकार_2 % BITS_PER_WORD) {
			newbits[cnt-1] &= ~((-1UL) << fill);
			newbits[cnt-1] |= word_pad_abits << fill;
			newbits[newcnt+cnt-1] &= ~((-1UL) << fill);
			newbits[newcnt+cnt-1] |= word_pad_bbits << fill;
		  }
		  क्रम (अचिन्हित idx = cnt ;  idx < newcnt ;  idx += 1)
			newbits[idx] = word_pad_abits;
		  क्रम (अचिन्हित idx = cnt ;  idx < newcnt ;  idx += 1)
			newbits[newcnt+idx] = word_pad_bbits;
	    }

	    आकार_2 = newsize;
	    abits_ptr_ = newbits;
	    bbits_ptr_ = newbits + newcnt;

      } अन्यथा {
	    अगर (cnt > 1) {
		  अचिन्हित दीर्घ newvala = abits_ptr_[0];
		  अचिन्हित दीर्घ newvalb = bbits_ptr_[0];
		  मिटाओ[]abits_ptr_;
		  abits_val_ = newvala;
		  bbits_val_ = newvalb;
	    }

	    अगर (newsize > आकार_2) {
		  abits_val_ &= ~((-1UL) << आकार_2);
		  bbits_val_ &= ~((-1UL) << आकार_2);
		  abits_val_ |= word_pad_abits << आकार_2;
		  bbits_val_ |= word_pad_bbits << आकार_2;
	    }

	    आकार_2 = newsize;
      }
}


अचिन्हित दीर्घ* vvp_vector4_t::subarray(अचिन्हित adr, अचिन्हित wid, bool xz_to_0) स्थिर
{
      स्थिर अचिन्हित BIT2_PER_WORD = 8*माप(अचिन्हित दीर्घ);
      अचिन्हित awid = (wid + BIT2_PER_WORD - 1) / (BIT2_PER_WORD);
      अचिन्हित दीर्घ*val = नया अचिन्हित दीर्घ[awid];

      क्रम (अचिन्हित idx = 0 ;  idx < awid ;  idx += 1)
	    val[idx] = 0;

      अगर (आकार_2 <= BITS_PER_WORD) {
	      /* Handle the special case that the array is small. The
		 entire value of the vector4 is within the xbits_val_
		 so we know that the result is a single word, the
		 source is a single word, and we just have to loop
		 through that word. */
	    अचिन्हित दीर्घ atmp = abits_val_ >> adr;
	    अचिन्हित दीर्घ btmp = bbits_val_ >> adr;
	    अगर (wid < BIT2_PER_WORD) {
		  atmp &= (1UL << wid) - 1;
		  btmp &= (1UL << wid) - 1;
	    }
	    अगर (btmp) {
		  अगर (xz_to_0)
			atmp &= ~btmp;
		  अन्यथा
			जाओ x_out;
	    }

	    val[0] = atmp;

      } अन्यथा {

	    अचिन्हित val_ptr = 0;
	    अचिन्हित val_off = 0;

	      /* Get the first word we are scanning. We may in fact be
		 somewhere in the middle of that word. */
	    जबतक (wid > 0) {
		  अचिन्हित दीर्घ atmp = abits_ptr_[adr/BITS_PER_WORD];
		  अचिन्हित दीर्घ btmp = bbits_ptr_[adr/BITS_PER_WORD];
		  अचिन्हित दीर्घ off = adr%BITS_PER_WORD;
		  atmp >>= off;
		  btmp >>= off;

		  अचिन्हित दीर्घ trans = BITS_PER_WORD - off;
		  अगर (trans > (BIT2_PER_WORD - val_off))
			trans = BIT2_PER_WORD - val_off;
		  अगर (wid < trans)
			trans = wid;
		  अगर (trans < BIT2_PER_WORD) {
			atmp &= (1UL << trans) - 1;
			btmp &= (1UL << trans) - 1;
		  }
		  अगर (btmp) {
			अगर (xz_to_0)
			      atmp &= ~btmp;
			अन्यथा
			      जाओ x_out;
		  }

		  val[val_ptr] |= atmp << val_off;
		  adr += trans;
		  wid -= trans;
		  val_off += trans;
		  अगर (val_off == BIT2_PER_WORD) {
			val_ptr += 1;
			val_off = 0;
		  }
	    }
      }

      वापस val;

 x_out:
      मिटाओ[]val;
      वापस 0;
}

व्योम vvp_vector4_t::setarray(अचिन्हित adr, अचिन्हित wid, स्थिर अचिन्हित दीर्घ*val)
{
      निश्चित(adr+wid <= आकार_2);

      स्थिर अचिन्हित BIT2_PER_WORD = 8*माप(अचिन्हित दीर्घ);

      अगर (आकार_2 <= BITS_PER_WORD) {
	      // We know here that both the source and the target are
	      // within a single word. Write the bits into the
	      // abits_val_ directly.

	    निश्चित(BIT2_PER_WORD <= BITS_PER_WORD);
	    अचिन्हित दीर्घ lmask = (1UL << adr) - 1UL;
	    अचिन्हित दीर्घ hmask = ((adr+wid) < BITS_PER_WORD)
		  ? -1UL << (adr+wid)
		  : 0;
	    अचिन्हित दीर्घ mask = ~(hmask | lmask);

	    abits_val_ &= ~mask;
	    bbits_val_ &= ~mask;

	    abits_val_ |= mask & (val[0] << adr);

      } अन्यथा {
	      // The general case, there are multiple words of
	      // destination, and possibly multiple words of source
	      // data. Shift and mask as we go.
	    अचिन्हित off = adr % BITS_PER_WORD;
	    अचिन्हित ptr = adr / BITS_PER_WORD;
	    अचिन्हित val_off = 0;
	    अचिन्हित val_ptr = 0;
	    जबतक (wid > 0) {
		  अचिन्हित trans = wid;
		  अगर (trans > (BIT2_PER_WORD-val_off))
			trans = BIT2_PER_WORD-val_off;
		  अगर (trans > (BITS_PER_WORD-off))
			trans = BITS_PER_WORD-off;

		  अचिन्हित दीर्घ lmask = (1UL << off) - 1UL;
		  अचिन्हित दीर्घ hmask = ((off+trans) < BITS_PER_WORD)
			? -1UL << (off+trans)
			: 0;
		  अचिन्हित दीर्घ mask = ~(hmask | lmask);

		  abits_ptr_[ptr] &= ~mask;
		  bbits_ptr_[ptr] &= ~mask;
		  अगर (val_off >= off)
			abits_ptr_[ptr] |= mask & (val[val_ptr] >> (val_off-off));
		  अन्यथा
			abits_ptr_[ptr] |= mask & (val[val_ptr] << (off-val_off));

		  wid -= trans;
		  val_off += trans;
		  अगर (val_off == BIT2_PER_WORD) {
			val_off = 0;
			val_ptr += 1;
		  }
		  off += trans;
		  अगर (off == BITS_PER_WORD) {
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
bool vvp_vector4_t::set_vec(अचिन्हित adr, स्थिर vvp_vector4_t&that)
{
      निश्चित(adr+that.आकार_2  <= आकार_2);
      bool diff_flag = false;

      अगर (आकार_2 <= BITS_PER_WORD) {

	      /* The destination vector (me!) is within a bits_val_
		 word, so the subvector is certainly within a
		 bits_val_ word. Therefore, the entire operation is a
		 matter of writing the bits of that into the addressed
		 bits of this. The mask below is calculated to be 1
		 for all the bits that are to come from that. Do the
		 job by some shifting, masking and OR. */

	    अचिन्हित दीर्घ lmask = (1UL << adr) - 1;
	    अचिन्हित दीर्घ hmask;
	    अचिन्हित दीर्घ hshift = adr+that.आकार_2;
	    अगर (hshift >= BITS_PER_WORD)
		  hmask = -1UL;
	    अन्यथा
		  hmask = (1UL << (adr+that.आकार_2)) - 1;
	    अचिन्हित दीर्घ mask = hmask & ~lmask;

	    अचिन्हित दीर्घ tmp = (that.abits_val_<<adr)&mask;
	    अगर ((abits_val_&mask) != tmp) {
		  diff_flag = true;
		  abits_val_ = (abits_val_ & ~mask) | tmp;
	    }
	    tmp = (that.bbits_val_<<adr) & mask;
	    अगर ((bbits_val_&mask) != tmp) {
		  diff_flag = true;
		  bbits_val_ = (bbits_val_ & ~mask) | tmp;
	    }

      } अन्यथा अगर (that.आकार_2 <= BITS_PER_WORD) {

	      /* This vector is more than a word, but that vector is
		 still small. Write into the destination, possibly
		 spanning two destination words, depending on whether
		 the source vector spans a word transition. */
	    अचिन्हित दीर्घ dptr = adr / BITS_PER_WORD;
	    अचिन्हित दीर्घ doff = adr % BITS_PER_WORD;

	    अचिन्हित दीर्घ lmask = (1UL << doff) - 1;
	    अचिन्हित दीर्घ hshift = doff+that.आकार_2;
	    अचिन्हित दीर्घ hmask;
	    अगर (hshift >= BITS_PER_WORD)
		  hmask = -1UL;
	    अन्यथा
		  hmask = (1UL << hshift) - 1UL;

	    अचिन्हित दीर्घ mask = hmask & ~lmask;
	    अचिन्हित दीर्घ tmp;

	    tmp = (that.abits_val_ << doff) & mask;
	    अगर ((abits_ptr_[dptr] & mask) != tmp) {
		  diff_flag = true;
		  abits_ptr_[dptr] = (abits_ptr_[dptr] & ~mask) | tmp;
	    }
	    tmp = (that.bbits_val_ << doff) & mask;
	    अगर ((bbits_ptr_[dptr] & mask) != tmp) {
		  diff_flag = true;
		  bbits_ptr_[dptr] = (bbits_ptr_[dptr] & ~mask) | tmp;
	    }

	    अगर ((doff + that.आकार_2) > BITS_PER_WORD) {
		  अचिन्हित पुच्छ = doff + that.आकार_2 - BITS_PER_WORD;
		  mask = (1UL << पुच्छ) - 1;

		  dptr += 1;
		  tmp = (that.abits_val_ >> (that.आकार_2-पुच्छ)) & mask;
		  अगर ((abits_ptr_[dptr] & mask) != tmp) {
			diff_flag = true;
			abits_ptr_[dptr] = (abits_ptr_[dptr] & ~mask) | tmp;
		  }
		  tmp = (that.bbits_val_ >> (that.आकार_2-पुच्छ)) & mask;
		  अगर ((bbits_ptr_[dptr] & mask) != tmp) {
			diff_flag = true;
			bbits_ptr_[dptr] = (bbits_ptr_[dptr] & ~mask) | tmp;
		  }
	    }

      } अन्यथा अगर (adr%BITS_PER_WORD == 0) {

	      /* In this case, both vectors are long, but the
		 destination is neatly aligned. That means all but the
		 last word can be simply copied with no masking. */

	    अचिन्हित remain = that.आकार_2;
	    अचिन्हित sptr = 0;
	    अचिन्हित dptr = adr / BITS_PER_WORD;
	    जबतक (remain >= BITS_PER_WORD) {
		  अगर (abits_ptr_[dptr] != that.abits_ptr_[sptr]) {
			diff_flag = true;
			abits_ptr_[dptr] = that.abits_ptr_[sptr];
		  }
		  अगर (bbits_ptr_[dptr] != that.bbits_ptr_[sptr]) {
			diff_flag = true;
			bbits_ptr_[dptr] = that.bbits_ptr_[sptr];
		  }
		  dptr += 1;
		  sptr += 1;
		  remain -= BITS_PER_WORD;
	    }

	    अगर (remain > 0) {
		  अचिन्हित दीर्घ mask = (1UL << remain) - 1;
		  अचिन्हित दीर्घ tmp;

		  tmp = that.abits_ptr_[sptr] & mask;
		  अगर ((abits_ptr_[dptr] & mask) != tmp) {
			diff_flag = true;
			abits_ptr_[dptr] = (abits_ptr_[dptr] & ~mask) | tmp;
		  }
		  tmp = that.bbits_ptr_[sptr] & mask;
		  अगर ((bbits_ptr_[dptr] & mask) != tmp) {
			diff_flag = true;
			bbits_ptr_[dptr] = (bbits_ptr_[dptr] & ~mask) | tmp;
		  }
	    }

      } अन्यथा {

	      /* We know that there are two long vectors, and we know
		 that the destination is definitely NOT aligned. */

	    अचिन्हित remain = that.आकार_2;
	    अचिन्हित sptr = 0;
	    अचिन्हित dptr = adr / BITS_PER_WORD;
	    अचिन्हित doff = adr % BITS_PER_WORD;
	    अचिन्हित दीर्घ lmask = (1UL << doff) - 1;
	    अचिन्हित ndoff = BITS_PER_WORD - doff;
	    जबतक (remain >= BITS_PER_WORD) {
		  अचिन्हित दीर्घ tmp;

		  tmp = (that.abits_ptr_[sptr] << doff) & ~lmask;
		  अगर ((abits_ptr_[dptr] & ~lmask) != tmp) {
			diff_flag = true;
			abits_ptr_[dptr] = (abits_ptr_[dptr] & lmask) | tmp;
		  }
		  tmp = (that.bbits_ptr_[sptr] << doff) & ~lmask;
		  अगर ((bbits_ptr_[dptr] & ~lmask) != tmp) {
			diff_flag = true;
			bbits_ptr_[dptr] = (bbits_ptr_[dptr] & lmask) | tmp;
		  }
		  dptr += 1;

		  tmp = (that.abits_ptr_[sptr] >> ndoff) & lmask;
		  अगर ((abits_ptr_[dptr] & lmask) != tmp) {
			diff_flag = true;
			abits_ptr_[dptr] = (abits_ptr_[dptr] & ~lmask) | tmp;
		  }
		  tmp = (that.bbits_ptr_[sptr] >> ndoff) & lmask;
		  अगर ((bbits_ptr_[dptr] & lmask) != tmp) {
			diff_flag = true;
			bbits_ptr_[dptr] = (bbits_ptr_[dptr] & ~lmask) | tmp;
		  }

		  remain -= BITS_PER_WORD;
		  sptr += 1;
	    }

	    अगर (remain > 0) {
		  अचिन्हित दीर्घ hshift = doff+remain;
		  अचिन्हित दीर्घ hmask;
		  अगर (hshift >= BITS_PER_WORD)
			hmask = -1UL;
		  अन्यथा
			hmask = (1UL << (doff+remain)) - 1;

		  अचिन्हित दीर्घ mask = hmask & ~lmask;
		  अचिन्हित दीर्घ tmp;

		  tmp = (that.abits_ptr_[sptr] << doff) & mask;
		  अगर ((abits_ptr_[dptr] & mask) != tmp) {
			diff_flag = true;
			abits_ptr_[dptr] = (abits_ptr_[dptr] & ~mask) | tmp;
		  }
		  tmp = (that.bbits_ptr_[sptr] << doff) & mask;
		  अगर ((bbits_ptr_[dptr] & mask) != tmp) {
			diff_flag = true;
			bbits_ptr_[dptr] = (bbits_ptr_[dptr] & ~mask) | tmp;
		  }

		  अगर ((doff + remain) > BITS_PER_WORD) {
			अचिन्हित पुच्छ = doff + remain - BITS_PER_WORD;
			अगर (पुच्छ >= BITS_PER_WORD)
			      mask = -1UL;
			अन्यथा
			      mask = (1UL << पुच्छ) - 1;

			dptr += 1;

			tmp = (that.abits_ptr_[sptr] >> (remain-पुच्छ))&mask;
			अगर ((abits_ptr_[dptr] & mask) != tmp) {
			      diff_flag = true;
			      abits_ptr_[dptr] = (abits_ptr_[dptr] & ~mask) | tmp;
			}
			tmp = (that.bbits_ptr_[sptr] >> (remain-पुच्छ))&mask;
			अगर ((bbits_ptr_[dptr] & mask) != tmp) {
			      diff_flag = true;
			      bbits_ptr_[dptr] = (bbits_ptr_[dptr] & ~mask) | tmp;
			}
		  }
	    }
      }

      वापस diff_flag;
}

/*
 * Add that vector to this vector. Do it in the Verilog way, which
 * means if we detect any X or Z bits, change the entire results to
 * all X.
 *
 * Assume both vectors are the same size.
 */
व्योम vvp_vector4_t::add(स्थिर vvp_vector4_t&that)
{
      निश्चित(आकार_2 == that.आकार_2);

      अगर (आकार_2 < BITS_PER_WORD) {
	    अचिन्हित दीर्घ mask = ~(-1UL << आकार_2);
	    अगर ((bbits_val_|that.bbits_val_) & mask) {
		  abits_val_ |= mask;
		  bbits_val_ |= mask;
		  वापस;
	    }

	    abits_val_ += that.abits_val_;
	    abits_val_ &= mask;
	    वापस;
      }

      अगर (आकार_2 == BITS_PER_WORD) {
	    अगर (bbits_val_ | that.bbits_val_) {
		  abits_val_ = WORD_X_ABITS;
		  bbits_val_ = WORD_X_BBITS;
	    } अन्यथा {
		  abits_val_ += that.abits_val_;
	    }
	    वापस;
      }

      पूर्णांक cnt = आकार_2 / BITS_PER_WORD;
      अचिन्हित दीर्घ carry = 0;
      क्रम (पूर्णांक idx = 0 ; idx < cnt ; idx += 1) {
	    अगर (bbits_ptr_[idx] | that.bbits_ptr_[idx])
		  जाओ x_out;

	    abits_ptr_[idx] = add_with_carry(abits_ptr_[idx], that.abits_ptr_[idx], carry);
      }

      अगर (अचिन्हित पुच्छ = आकार_2 % BITS_PER_WORD) {
	    अचिन्हित दीर्घ mask = ~( -1UL << पुच्छ );
	    अगर ((bbits_ptr_[cnt] | that.bbits_ptr_[cnt])&mask)
		  जाओ x_out;

	    abits_ptr_[cnt] = add_with_carry(abits_ptr_[cnt], that.abits_ptr_[cnt], carry);
	    abits_ptr_[cnt] &= mask;
      }

      वापस;

 x_out:
      क्रम (पूर्णांक idx = 0 ; idx < cnt ; idx += 1) {
	    abits_ptr_[idx] = WORD_X_ABITS;
	    bbits_ptr_[idx] = WORD_X_BBITS;
      }
      अगर (अचिन्हित पुच्छ = आकार_2%BITS_PER_WORD) {
	    अचिन्हित दीर्घ mask = ~( -1UL << पुच्छ );
	    abits_ptr_[cnt] = WORD_X_ABITS&mask;
	    bbits_ptr_[cnt] = WORD_X_BBITS&mask;
      }
}

व्योम vvp_vector4_t::sub(स्थिर vvp_vector4_t&that)
{
      निश्चित(आकार_2 == that.आकार_2);

      अगर (आकार_2 < BITS_PER_WORD) {
	    अचिन्हित दीर्घ mask = ~(-1UL << आकार_2);
	    अगर ((bbits_val_|that.bbits_val_) & mask) {
		  abits_val_ |= mask;
		  bbits_val_ |= mask;
		  वापस;
	    }

	    abits_val_ -= that.abits_val_;
	    abits_val_ &= mask;
	    वापस;
      }

      अगर (आकार_2 == BITS_PER_WORD) {
	    अगर (bbits_val_ | that.bbits_val_) {
		  abits_val_ = WORD_X_ABITS;
		  bbits_val_ = WORD_X_BBITS;
	    } अन्यथा {
		  abits_val_ -= that.abits_val_;
	    }
	    वापस;
      }

      पूर्णांक cnt = आकार_2 / BITS_PER_WORD;
      अचिन्हित दीर्घ carry = 1;
      क्रम (पूर्णांक idx = 0 ; idx < cnt ; idx += 1) {
	    अगर (bbits_ptr_[idx] | that.bbits_ptr_[idx])
		  जाओ x_out;

	    abits_ptr_[idx] = add_with_carry(abits_ptr_[idx], ~that.abits_ptr_[idx], carry);
      }

      अगर (अचिन्हित पुच्छ = आकार_2 % BITS_PER_WORD) {
	    अचिन्हित दीर्घ mask = ~( -1UL << पुच्छ );
	    अगर ((bbits_ptr_[cnt] | that.bbits_ptr_[cnt])&mask)
		  जाओ x_out;

	    abits_ptr_[cnt] = add_with_carry(abits_ptr_[cnt], ~that.abits_ptr_[cnt], carry);
	    abits_ptr_[cnt] &= mask;
      }

      वापस;

 x_out:
      क्रम (पूर्णांक idx = 0 ; idx < cnt ; idx += 1) {
	    abits_ptr_[idx] = WORD_X_ABITS;
	    bbits_ptr_[idx] = WORD_X_BBITS;
      }
      अगर (अचिन्हित पुच्छ = आकार_2%BITS_PER_WORD) {
	    अचिन्हित दीर्घ mask = ~( -1UL << पुच्छ );
	    abits_ptr_[cnt] = WORD_X_ABITS&mask;
	    bbits_ptr_[cnt] = WORD_X_BBITS&mask;
      }

}

व्योम vvp_vector4_t::mov(अचिन्हित dst, अचिन्हित src, अचिन्हित cnt)
{
      निश्चित(dst+cnt <= आकार_2);
      निश्चित(src+cnt <= आकार_2);

      अगर (आकार_2 <= BITS_PER_WORD) {
	    अचिन्हित दीर्घ vmask = (1UL << cnt) - 1;
	    अचिन्हित दीर्घ tmp;

	    tmp = (abits_val_ >> src) & vmask;
	    abits_val_ &= ~ (vmask << dst);
	    abits_val_ |= tmp << dst;

	    tmp = (bbits_val_ >> src) & vmask;
	    bbits_val_ &= ~ (vmask << dst);
	    bbits_val_ |= tmp << dst;

      } अन्यथा {
	    अचिन्हित sptr = src / BITS_PER_WORD;
	    अचिन्हित dptr = dst / BITS_PER_WORD;
	    अचिन्हित soff = src % BITS_PER_WORD;
	    अचिन्हित doff = dst % BITS_PER_WORD;

	    जबतक (cnt > 0) {
		  अचिन्हित trans = cnt;
		  अगर ((soff+trans) > BITS_PER_WORD)
			trans = BITS_PER_WORD - soff;

		  अगर ((doff+trans) > BITS_PER_WORD)
			trans = BITS_PER_WORD - doff;

		  अगर (trans == BITS_PER_WORD) {
			  // Special case: the transfer count is
			  // exactly an entire word. For this to be
			  // true, it must also be true that the
			  // pointers are aligned. The work is easy,
			abits_ptr_[dptr] = abits_ptr_[sptr];
			bbits_ptr_[dptr] = bbits_ptr_[sptr];
			dptr += 1;
			sptr += 1;
			cnt -= BITS_PER_WORD;
			जारी;
		  }

		    // Here we know that either the source or
		    // destination is unaligned, and also we know that
		    // the count is less than a full word.
		  अचिन्हित दीर्घ vmask = (1UL << trans) - 1;
		  अचिन्हित दीर्घ tmp;

		  tmp = (abits_ptr_[sptr] >> soff) & vmask;
		  abits_ptr_[dptr] &= ~ (vmask << doff);
		  abits_ptr_[dptr] |= tmp << doff;

		  tmp = (bbits_ptr_[sptr] >> soff) & vmask;
		  bbits_ptr_[dptr] &= ~ (vmask << doff);
		  bbits_ptr_[dptr] |= tmp << doff;

		  cnt -= trans;
		  soff += trans;
		  अगर (soff >= BITS_PER_WORD) {
			soff = 0;
			sptr += 1;
		  }
		  doff += trans;
		  अगर (doff >= BITS_PER_WORD) {
			doff = 0;
			dptr += 1;
		  }
	    }
      }
}

व्योम vvp_vector4_t::mul(स्थिर vvp_vector4_t&that)
{
      निश्चित(आकार_2 == that.आकार_2);

      अगर (आकार_2 < BITS_PER_WORD) {
	    अचिन्हित दीर्घ mask = ~(-1UL << आकार_2);
	    अगर ((bbits_val_|that.bbits_val_) & mask) {
		  abits_val_ |= mask;
		  bbits_val_ |= mask;
		  वापस;
	    }

	    abits_val_ *= that.abits_val_;
	    abits_val_ &= mask;
	    वापस;
      }

      अगर (आकार_2 == BITS_PER_WORD) {
	    अगर (bbits_val_ || that.bbits_val_) {
		  abits_val_ = WORD_X_ABITS;
		  bbits_val_ = WORD_X_BBITS;
	    } अन्यथा {
		  abits_val_ *= that.abits_val_;
	    }
	    वापस;
      }

      स्थिर पूर्णांक cnt = (आकार_2+BITS_PER_WORD-1) / BITS_PER_WORD;

      अचिन्हित दीर्घ mask;
      अगर (अचिन्हित पुच्छ = आकार_2%BITS_PER_WORD) {
	    mask = ~( -1UL << पुच्छ );
      } अन्यथा {
	    mask = ~0UL;
      }

	// Check for any XZ values ahead of time in a first pass. If
	// we find any, then force the entire result to be X and be
	// done.
      क्रम (पूर्णांक idx = 0 ; idx < cnt ; idx += 1) {
	    अचिन्हित दीर्घ lval = bbits_ptr_[idx];
	    अचिन्हित दीर्घ rval = that.bbits_ptr_[idx];
	    अगर (idx == (cnt-1)) {
		  lval &= mask;
		  rval &= mask;
	    }
	    अगर (lval || rval) {
		  क्रम (पूर्णांक xdx = 0 ; xdx < cnt-1 ; xdx += 1) {
			abits_ptr_[xdx] = WORD_X_ABITS;
			bbits_ptr_[xdx] = WORD_X_BBITS;
		  }
		  abits_ptr_[cnt-1] = WORD_X_ABITS & mask;
		  bbits_ptr_[cnt-1] = WORD_X_BBITS & mask;
		  वापस;
	    }
      }

	// Calculate the result into a res array. We need to keep is
	// separate from the "this" array because we are making
	// multiple passes.
      अचिन्हित दीर्घ*res = नया अचिन्हित दीर्घ[cnt];
      क्रम (पूर्णांक idx = 0 ; idx < cnt ; idx += 1)
	    res[idx] = 0;

      क्रम (पूर्णांक mul_a = 0 ; mul_a < cnt ; mul_a += 1) {
	    अचिन्हित दीर्घ lval = abits_ptr_[mul_a];
	    अगर (mul_a == (cnt-1))
		  lval &= mask;

	    क्रम (पूर्णांक mul_b = 0 ; mul_b < (cnt-mul_a) ; mul_b += 1) {
		  अचिन्हित दीर्घ rval = that.abits_ptr_[mul_b];
		  अगर (mul_b == (cnt-1))
			rval &= mask;

		  अचिन्हित दीर्घ योग;
		  अचिन्हित दीर्घ tmp = multiply_with_carry(lval, rval, योग);
		  पूर्णांक base = mul_a + mul_b;
		  अचिन्हित दीर्घ carry = 0;
		  res[base] = add_with_carry(res[base], tmp, carry);
		  क्रम (पूर्णांक add_idx = base+1 ; add_idx < cnt ; add_idx += 1) {
			res[add_idx] = add_with_carry(res[add_idx], योग, carry);
			योग = 0;
		  }
	    }
      }

	// Replace the "this" value with the calculated result. We
	// know a-priori that the bbits are zero and unchanged.
      res[cnt-1] &= mask;
      क्रम (पूर्णांक idx = 0 ; idx < cnt ; idx += 1)
	    abits_ptr_[idx] = res[idx];

      मिटाओ[]res;
      वापस;


}

bool vvp_vector4_t::eeq(स्थिर vvp_vector4_t&that) स्थिर
{
      अगर (आकार_2 != that.आकार_2)
	    वापस false;

      अगर (आकार_2 < BITS_PER_WORD) {
	    अचिन्हित दीर्घ mask = (1UL << आकार_2) - 1;
	    वापस (abits_val_&mask) == (that.abits_val_&mask)
		  && (bbits_val_&mask) == (that.bbits_val_&mask);
      }

      अगर (आकार_2 == BITS_PER_WORD) {
	    वापस (abits_val_ == that.abits_val_)
		  && (bbits_val_ == that.bbits_val_);
      }

      अचिन्हित शब्दगण = आकार_2 / BITS_PER_WORD;
      क्रम (अचिन्हित idx = 0 ;  idx < शब्दगण ;  idx += 1) {
	    अगर (abits_ptr_[idx] != that.abits_ptr_[idx])
		  वापस false;
	    अगर (bbits_ptr_[idx] != that.bbits_ptr_[idx])
		  वापस false;
      }

      अचिन्हित दीर्घ mask = आकार_2%BITS_PER_WORD;
      अगर (mask > 0) {
	    mask = (1UL << mask) - 1;
	    वापस (abits_ptr_[शब्दगण]&mask) == (that.abits_ptr_[शब्दगण]&mask)
		  && (bbits_ptr_[शब्दगण]&mask) == (that.bbits_ptr_[शब्दगण]&mask);
      }

      वापस true;
}

bool vvp_vector4_t::eq_xz(स्थिर vvp_vector4_t&that) स्थिर
{
      अगर (आकार_2 != that.आकार_2)
	    वापस false;

      अगर (आकार_2 < BITS_PER_WORD) {
	    अचिन्हित दीर्घ mask = (1UL << आकार_2) - 1;
	    वापस ((abits_val_|bbits_val_)&mask) == ((that.abits_val_|that.bbits_val_)&mask)
		  && (bbits_val_&mask) == (that.bbits_val_&mask);
      }

      अगर (आकार_2 == BITS_PER_WORD) {
	    वापस ((abits_val_|bbits_val_) == (that.abits_val_|that.bbits_val_))
		  && (bbits_val_ == that.bbits_val_);
      }

      अचिन्हित शब्दगण = आकार_2 / BITS_PER_WORD;
      क्रम (अचिन्हित idx = 0 ;  idx < शब्दगण ;  idx += 1) {
	    अगर ((abits_ptr_[idx]|bbits_ptr_[idx]) != (that.abits_ptr_[idx]|that.bbits_ptr_[idx]))
		  वापस false;
	    अगर (bbits_ptr_[idx] != that.bbits_ptr_[idx])
		  वापस false;
      }

      अचिन्हित दीर्घ mask = आकार_2%BITS_PER_WORD;
      अगर (mask > 0) {
	    mask = (1UL << mask) - 1;
	    वापस ((abits_ptr_[शब्दगण]|bbits_ptr_[शब्दगण])&mask) == ((that.abits_ptr_[शब्दगण]|that.bbits_ptr_[शब्दगण])&mask)
		  && (bbits_ptr_[शब्दगण]&mask) == (that.bbits_ptr_[शब्दगण]&mask);
      }

      वापस true;
}

bool vvp_vector4_t::has_xz() स्थिर
{
      अगर (आकार_2 < BITS_PER_WORD) {
	    अचिन्हित दीर्घ mask = -1UL >> (BITS_PER_WORD - आकार_2);
	    वापस bbits_val_&mask;
      }

      अगर (आकार_2 == BITS_PER_WORD) {
	    वापस bbits_val_;
      }

      अचिन्हित शब्दगण = आकार_2 / BITS_PER_WORD;
      क्रम (अचिन्हित idx = 0 ; idx < शब्दगण ; idx += 1) {
	    अगर (bbits_ptr_[idx])
		  वापस true;
      }

      अचिन्हित दीर्घ mask = आकार_2%BITS_PER_WORD;
      अगर (mask > 0) {
	    mask = -1UL >> (BITS_PER_WORD - mask);
	    वापस bbits_ptr_[शब्दगण]&mask;
      }

      वापस false;
}

व्योम vvp_vector4_t::change_z2x()
{
	// This method relies on the fact that both BIT4_X and BIT4_Z
	// have the bbit set in the vector4 encoding, and also that
	// the BIT4_X has abit set in the vector4 encoding. By simply
	// or-ing the bbit into the abit, BIT4_X and BIT4_Z both
	// become BIT4_X.

      अगर (आकार_2 <= BITS_PER_WORD) {
	    abits_val_ |= bbits_val_;
      } अन्यथा {
	    अचिन्हित शब्दगण = (आकार_2+BITS_PER_WORD-1) / BITS_PER_WORD;
	    क्रम (अचिन्हित idx = 0 ;  idx < शब्दगण ;  idx += 1)
		  abits_ptr_[idx] |= bbits_ptr_[idx];
      }
}

व्योम vvp_vector4_t::fill_bits(vvp_bit4_t bit)
{
	/* note: this relies on the bit encoding for the vvp_bit4_t. */
      अटल स्थिर अचिन्हित दीर्घ init_atable[4] = {
	    WORD_0_ABITS,
	    WORD_1_ABITS,
	    WORD_Z_ABITS,
	    WORD_X_ABITS };
      अटल स्थिर अचिन्हित दीर्घ init_btable[4] = {
	    WORD_0_BBITS,
	    WORD_1_BBITS,
	    WORD_Z_BBITS,
	    WORD_X_BBITS };

      अगर (आकार_2 <= BITS_PER_WORD) {
	    abits_val_ = init_atable[bit];
	    bbits_val_ = init_btable[bit];
      } अन्यथा {
	    अचिन्हित शब्दगण = (आकार_2 + BITS_PER_WORD - 1) / BITS_PER_WORD;
	    क्रम (अचिन्हित idx = 0; idx < शब्दगण; idx += 1) {
		  abits_ptr_[idx] = init_atable[bit];
		  bbits_ptr_[idx] = init_btable[bit];
	    }
      }
}

अक्षर* vvp_vector4_t::as_string(अक्षर*buf, माप_प्रकार buf_len) स्थिर
{
      अक्षर*res = buf;
      *buf++ = 'C';
      *buf++ = '4';
      *buf++ = '<';
      buf_len -= 3;

      क्रम (अचिन्हित idx = 0 ;  idx < size() && buf_len >= 2 ;  idx += 1) {
	    चयन (value(size()-idx-1)) {
		हाल BIT4_0:
		  *buf++ = '0';
		  अवरोध;
		हाल BIT4_1:
		  *buf++ = '1';
		  अवरोध;
		हाल BIT4_X:
		  *buf++ = 'x';
		  अवरोध;
		हाल BIT4_Z:
		  *buf++ = 'z';
	    }
	    buf_len -= 1;
      }

      *buf++ = '>';
      *buf++ = 0;
      वापस res;
}

व्योम vvp_vector4_t::invert()
{
      अगर (आकार_2 <= BITS_PER_WORD) {
	    अचिन्हित दीर्घ mask = (आकार_2<BITS_PER_WORD)? (1UL<<आकार_2)-1UL : -1UL;
	    abits_val_ = mask & ~abits_val_;
	    abits_val_ |= bbits_val_;
      } अन्यथा {
	    अचिन्हित remaining = आकार_2;
	    अचिन्हित idx = 0;
	    जबतक (remaining >= BITS_PER_WORD) {
		  abits_ptr_[idx] = ~abits_ptr_[idx];
		  abits_ptr_[idx] |= bbits_ptr_[idx];
		  idx += 1;
		  remaining -= BITS_PER_WORD;
	    }
	    अगर (remaining > 0) {
		  अचिन्हित दीर्घ mask = (1UL<<remaining) - 1UL;
		  abits_ptr_[idx] = mask & ~abits_ptr_[idx];
		  abits_ptr_[idx] |= bbits_ptr_[idx];
	    }
      }
}

#घोषणा BIT_MASK(n) ((n) ? ((~0UL) >> (BITS_PER_WORD - (n))) : ~0UL)

vvp_bit4_t vvp_vector4_t::reduce_or() स्थिर
{
      अचिन्हित दीर्घ mask = BIT_MASK(आकार_2 % BITS_PER_WORD);
      vvp_bit4_t res = BIT4_0;

      अगर (आकार_2 <= BITS_PER_WORD) {
	    अगर ((abits_val_ & ~bbits_val_ & mask) != 0UL)
		  वापस BIT4_1;
	    अगर ((bbits_val_ & mask) != 0UL)
		  वापस BIT4_X;
      } अन्यथा {
	    अचिन्हित शब्दगण = (आकार_2 + BITS_PER_WORD - 1) / BITS_PER_WORD;
	    अचिन्हित idx;
	    क्रम (idx = 0; idx < शब्दगण - 1; idx += 1) {
		  अगर ((abits_ptr_[idx] & ~bbits_ptr_[idx]) != 0UL)
			वापस BIT4_1;
		  अगर (bbits_ptr_[idx] != 0UL)
			res = BIT4_X;
	    }
	    अगर ((abits_ptr_[idx] & ~bbits_ptr_[idx] & mask) != 0UL)
		  वापस BIT4_1;
	    अगर ((bbits_ptr_[idx] & mask) != 0UL)
		  res = BIT4_X;
      }

      वापस res;
}

vvp_bit4_t vvp_vector4_t::reduce_and() स्थिर
{
      अचिन्हित दीर्घ mask = BIT_MASK(आकार_2 % BITS_PER_WORD);
      vvp_bit4_t res = BIT4_1;

      अगर (आकार_2 <= BITS_PER_WORD) {
	    अगर ((abits_val_ | bbits_val_ | ~mask) != ~0UL)
		  वापस BIT4_0;
	    अगर ((bbits_val_ & mask) != 0UL)
		  वापस BIT4_X;
	} अन्यथा {
	    अचिन्हित शब्दगण = (आकार_2 + BITS_PER_WORD - 1) / BITS_PER_WORD;
	    अचिन्हित idx;
	    क्रम (idx = 0; idx < शब्दगण - 1; idx += 1) {
		  अगर ((abits_ptr_[idx] | bbits_ptr_[idx]) != ~0UL)
			वापस BIT4_0;
		  अगर (bbits_ptr_[idx] != 0UL)
			res = BIT4_X;
	    }
	    अगर ((abits_ptr_[idx] | bbits_ptr_[idx] | ~mask) != ~0UL)
		  वापस BIT4_0;
	    अगर ((bbits_ptr_[idx] & mask) != 0UL)
		  res = BIT4_X;
      }

      वापस res;
}

अटल अचिन्हित दीर्घ parity(अचिन्हित दीर्घ val)
{
#अगर defined(__GNUC__)
      // The compiler builtin can use target-specific CPU instructions.
      वापस __builtin_parityl(val);
#अन्यथा
#अगर अच_दीर्घ_उच्च > 0xffffffffUL
	val ^= val >> 32;
#पूर्ण_अगर
	val ^= val >> 16;
	val ^= val >> 8;
	val ^= val >> 4;

      वापस (0x6996 >> (val & 0xf)) & 1;
#पूर्ण_अगर
}

vvp_bit4_t vvp_vector4_t::reduce_xor() स्थिर
{
      अचिन्हित दीर्घ mask = BIT_MASK(आकार_2 % BITS_PER_WORD);

      अगर (आकार_2 <= BITS_PER_WORD) {
	    अगर ((bbits_val_ & mask) != 0UL)
		  वापस BIT4_X;
	    वापस parity(abits_val_ & mask) ? BIT4_1 : BIT4_0;
      } अन्यथा {
	    अचिन्हित शब्दगण = (आकार_2 + BITS_PER_WORD - 1) / BITS_PER_WORD;
	    अचिन्हित दीर्घ val_a = 0UL;
	    अचिन्हित idx;
	    क्रम (idx = 0; idx < शब्दगण - 1; idx += 1) {
		  अगर (bbits_ptr_[idx] != 0UL)
			वापस BIT4_X;
		  val_a ^= abits_ptr_[idx];
	    }
	    अगर ((bbits_ptr_[idx] & mask) != 0UL)
		  वापस BIT4_X;
	    val_a ^= abits_ptr_[idx] & mask;
	    वापस parity(val_a) ? BIT4_1 : BIT4_0;
      }
}

vvp_vector4_t& vvp_vector4_t::चालक &= (स्थिर vvp_vector4_t&that)
{
	// The truth table is:
	//     00 01 11 10
	//  00 00 00 00 00
	//  01 00 01 11 11
	//  11 00 11 11 11
	//  10 00 11 11 11
      अगर (आकार_2 <= BITS_PER_WORD) {
	    अचिन्हित दीर्घ tmp1 = abits_val_ | bbits_val_;
	    अचिन्हित दीर्घ tmp2 = that.abits_val_ | that.bbits_val_;
	    abits_val_ = tmp1 & tmp2;
	    bbits_val_ = (tmp1 & that.bbits_val_) | (tmp2 & bbits_val_);
      } अन्यथा {
	    अचिन्हित शब्दगण = (आकार_2 + BITS_PER_WORD - 1) / BITS_PER_WORD;
	    क्रम (अचिन्हित idx = 0; idx < शब्दगण ; idx += 1) {
		  अचिन्हित दीर्घ tmp1 = abits_ptr_[idx] | bbits_ptr_[idx];
		  अचिन्हित दीर्घ tmp2 = that.abits_ptr_[idx] |
		                       that.bbits_ptr_[idx];
		  abits_ptr_[idx] = tmp1 & tmp2;
		  bbits_ptr_[idx] = (tmp1 & that.bbits_ptr_[idx]) |
		                    (tmp2 & bbits_ptr_[idx]);
	    }
      }

      वापस *यह;
}

vvp_vector4_t& vvp_vector4_t::चालक |= (स्थिर vvp_vector4_t&that)
{
	// The truth table is:
	//     00 01 11 10
	//  00 00 01 11 11
	//  01 01 01 01 01
	//  11 11 01 11 11
	//  10 11 01 11 11
      अगर (आकार_2 <= BITS_PER_WORD) {
	    अचिन्हित दीर्घ tmp = abits_val_ | bbits_val_ |
	                        that.abits_val_ | that.bbits_val_;
	    bbits_val_ = ((~abits_val_ | bbits_val_) & that.bbits_val_) |
	                 ((~that.abits_val_ | that.bbits_val_) & bbits_val_);
	    abits_val_ = tmp;

      } अन्यथा {
	    अचिन्हित शब्दगण = (आकार_2 + BITS_PER_WORD - 1) / BITS_PER_WORD;
	    क्रम (अचिन्हित idx = 0; idx < शब्दगण ; idx += 1) {
		  अचिन्हित दीर्घ tmp = abits_ptr_[idx] | bbits_ptr_[idx] |
	                        that.abits_ptr_[idx] | that.bbits_ptr_[idx];
		  bbits_ptr_[idx] = ((~abits_ptr_[idx] | bbits_ptr_[idx]) &
		                     that.bbits_ptr_[idx]) |
		                    ((~that.abits_ptr_[idx] |
		                      that.bbits_ptr_[idx]) & bbits_ptr_[idx]);
		  abits_ptr_[idx] = tmp;
	    }
      }

      वापस *यह;
}

vvp_vector4_t& vvp_vector4_t::चालक ^= (स्थिर vvp_vector4_t&that)
{
	// The truth table is:
	//     00 01 11 10
	//  00 00 01 11 11
	//  01 01 00 11 11
	//  11 11 11 11 11
	//  10 11 11 11 11
      अगर (आकार_2 <= BITS_PER_WORD) {
	    अचिन्हित दीर्घ bval = bbits_val_ | that.bbits_val_;
	    bbits_val_ = bval;
	    abits_val_ = (abits_val_ ^ that.abits_val_) | bval;
      } अन्यथा {
	    अचिन्हित शब्दगण = (आकार_2 + BITS_PER_WORD - 1) / BITS_PER_WORD;
	    क्रम (अचिन्हित idx = 0; idx < शब्दगण ; idx += 1) {
		  अचिन्हित दीर्घ bval = bbits_ptr_[idx] | that.bbits_ptr_[idx];
		  bbits_ptr_[idx] = bval;
		  abits_ptr_[idx] = (abits_ptr_[idx] ^ that.abits_ptr_[idx]) | bval;
	    }
      }

      वापस *यह;
}

/*
* Add an integer to the vvp_vector4_t in place, bit by bit so that
* there is no size limitations.
*/
vvp_vector4_t& vvp_vector4_t::चालक += (int64_t that)
{
      vvp_bit4_t carry = BIT4_0;
      अचिन्हित idx;

      अगर (has_xz()) {
	    vvp_vector4_t xxx (size(), BIT4_X);
	    *यह = xxx;
	    वापस *यह;
      }

      क्रम (idx = 0 ; idx < size() ; idx += 1) {
	    अगर (that == 0 && carry==BIT4_0)
		  अवरोध;

	    vvp_bit4_t that_bit = (that&1)? BIT4_1 : BIT4_0;
	    that >>= 1;

	    अगर (that_bit==BIT4_0 && carry==BIT4_0)
		  जारी;

	    vvp_bit4_t bit = value(idx);
	    bit = add_with_carry(bit, that_bit, carry);

	    set_bit(idx, bit);
      }

      वापस *यह;
}

ब_प्रवाह& चालक<< (ब_प्रवाह&out, स्थिर vvp_vector4_t&that)
{
      out << that.size() << "'b";
      क्रम (अचिन्हित idx = 0 ;  idx < that.size() ;  idx += 1)
	    out << that.value(that.size()-idx-1);
      वापस out;
}

ढाँचा <श्रेणी INT>bool vector4_to_मूल्य(स्थिर vvp_vector4_t&vec, INT&val,
					  bool is_signed, bool is_arithmetic)
{
      INT res = 0;
      INT msk = 1;
      bool rc_flag = true;

      अचिन्हित size = vec.size();
      अगर (size > 8*माप(val)) size = 8*माप(val);
      क्रम (अचिन्हित idx = 0 ;  idx < size ;  idx += 1) {
	    चयन (vec.value(idx)) {
		हाल BIT4_0:
		  अवरोध;
		हाल BIT4_1:
		  res |= msk;
		  अवरोध;
		शेष:
		  अगर (is_arithmetic)
			वापस false;
		  अन्यथा
			rc_flag = false;
	    }

	    msk <<= 1;
      }

      अगर (is_signed && vec.value(vec.size()-1) == BIT4_1) {
	    अगर (vec.size() < 8*माप(val))
		  res |= static_cast<INT>(-1ULL << vec.size());
      }

      val = res;
      वापस rc_flag;
}

ढाँचा bool vector4_to_मूल्य(स्थिर vvp_vector4_t&vec, int8_t&val,
			       bool is_signed, bool is_arithmetic);
ढाँचा bool vector4_to_मूल्य(स्थिर vvp_vector4_t&vec, int16_t&val,
			       bool is_signed, bool is_arithmetic);
ढाँचा bool vector4_to_मूल्य(स्थिर vvp_vector4_t&vec, int32_t&val,
			       bool is_signed, bool is_arithmetic);
ढाँचा bool vector4_to_मूल्य(स्थिर vvp_vector4_t&vec, int64_t&val,
			       bool is_signed, bool is_arithmetic);
ढाँचा bool vector4_to_मूल्य(स्थिर vvp_vector4_t&vec, uint8_t&val,
			       bool is_signed, bool is_arithmetic);
ढाँचा bool vector4_to_मूल्य(स्थिर vvp_vector4_t&vec, uint16_t&val,
			       bool is_signed, bool is_arithmetic);
ढाँचा bool vector4_to_मूल्य(स्थिर vvp_vector4_t&vec, uint32_t&val,
			       bool is_signed, bool is_arithmetic);
ढाँचा bool vector4_to_मूल्य(स्थिर vvp_vector4_t&vec, uint64_t&val,
			       bool is_signed, bool is_arithmetic);

ढाँचा <श्रेणी T> bool vector4_to_मूल्य(स्थिर vvp_vector4_t&vec,
                                         bool&overflow_flag, T&val)
{
      T res = 0;
      T msk = 1;

      overflow_flag = false;
      अचिन्हित size = vec.size();
      क्रम (अचिन्हित idx = 0 ;  idx < size ;  idx += 1) {
	    चयन (vec.value(idx)) {
		हाल BIT4_0:
		  अवरोध;
		हाल BIT4_1:
		  अगर (msk == 0)
			overflow_flag = true;
		  अन्यथा
			res |= msk;
		  अवरोध;
		शेष:
		  वापस false;
	    }

	    msk <<= static_cast<T>(1);
      }

      val = res;
      वापस true;
}

ढाँचा bool vector4_to_मूल्य(स्थिर vvp_vector4_t&vec, bool&overflow_flag,
                               अचिन्हित दीर्घ&val);
#अगर_अघोषित UL_AND_TIME64_SAME
ढाँचा bool vector4_to_मूल्य(स्थिर vvp_vector4_t&vec, bool&overflow_flag,
                               vvp_time64_t&val);
#पूर्ण_अगर

bool vector4_to_मूल्य(स्थिर vvp_vector4_t&vec, द्विगुन&val, bool signed_flag)
{

      अगर (vec.size() == 0) {
	    val = 0.0;
	    वापस true;
      }

      bool flag = true;

      अगर (vec.value(vec.size()-1) != BIT4_1) {
	    signed_flag = false;
      }

      द्विगुन res = 0.0;
      अगर (signed_flag) {
	    vvp_bit4_t carry = BIT4_1;
	    क्रम (अचिन्हित idx = 0 ;  idx < vec.size() ;  idx += 1) {
		  vvp_bit4_t a = ~vec.value(idx);
		  vvp_bit4_t x = add_with_carry(a, BIT4_0, carry);
		  चयन (x) {
		      हाल BIT4_0:
			अवरोध;
		      हाल BIT4_1:
			res += घात(2.0, (पूर्णांक)idx);
			अवरोध;
		      शेष:
			flag = false;
		  }
	    }
	    res *= -1.0;
      } अन्यथा {
	    क्रम (अचिन्हित idx = 0 ;  idx < vec.size() ;  idx += 1) {
		  चयन (vec.value(idx)) {
		      हाल BIT4_0:
			अवरोध;
		      हाल BIT4_1:
			res += घात(2.0, (पूर्णांक)idx);
			अवरोध;
		      शेष:
			flag = false;
		  }
	    }
      }
      val = res;
      वापस flag;
}

bool vector2_to_मूल्य(स्थिर vvp_vector2_t&a, int32_t&val, bool is_signed)
{
      val = 0;
      अचिन्हित idx;
      uint32_t mask;
      क्रम (idx = 0, mask = 1 ; idx < a.size() && idx < 32 ; idx += 1, mask <<= 1) {
	    अगर (a.value(idx)) val |= mask;
      }

      अगर (is_signed && a.size() < 32 && a.value(a.size()-1)) {
	    mask = -1;
	    mask <<= a.size();
	    val |= mask;
      }

      वापस a.size() <= 32;
}

vvp_vector4array_t::vvp_vector4array_t(अचिन्हित width__, अचिन्हित शब्दगण_2)
: width_(width__), शब्दगण_3(शब्दगण_2)
{
}

vvp_vector4array_t::~vvp_vector4array_t()
{
}

व्योम vvp_vector4array_t::set_word_(v4cell*cell, स्थिर vvp_vector4_t&that)
{
      निश्चित(that.आकार_2 == width_);

      अगर (width_ <= vvp_vector4_t::BITS_PER_WORD) {
	    cell->abits_val_ = that.abits_val_;
	    cell->bbits_val_ = that.bbits_val_;
	    वापस;
      }

      अचिन्हित cnt = (width_ + vvp_vector4_t::BITS_PER_WORD-1)/vvp_vector4_t::BITS_PER_WORD;

      अगर (cell->abits_ptr_ == 0) {
	    cell->abits_ptr_ = नया अचिन्हित दीर्घ[2*cnt];
	    cell->bbits_ptr_ = cell->abits_ptr_ + cnt;
      }

      क्रम (अचिन्हित idx = 0 ; idx < cnt ; idx += 1)
	    cell->abits_ptr_[idx] = that.abits_ptr_[idx];
      क्रम (अचिन्हित idx = 0 ; idx < cnt ; idx += 1)
	    cell->bbits_ptr_[idx] = that.bbits_ptr_[idx];
}

vvp_vector4_t vvp_vector4array_t::get_word_(v4cell*cell) स्थिर
{
      अगर (width_ <= vvp_vector4_t::BITS_PER_WORD) {
	    vvp_vector4_t res;
	    res.आकार_2 = width_;
	    res.abits_val_ = cell->abits_val_;
	    res.bbits_val_ = cell->bbits_val_;
	    वापस res;
      }

      vvp_vector4_t res (width_, BIT4_X);
      अगर (cell->abits_ptr_ == 0)
	    वापस res;

      अचिन्हित cnt = (width_ + vvp_vector4_t::BITS_PER_WORD-1)/vvp_vector4_t::BITS_PER_WORD;

      क्रम (अचिन्हित idx = 0 ; idx < cnt ; idx += 1)
	    res.abits_ptr_[idx] = cell->abits_ptr_[idx];
      क्रम (अचिन्हित idx = 0 ; idx < cnt ; idx += 1)
	    res.bbits_ptr_[idx] = cell->bbits_ptr_[idx];

      वापस res;
}

vvp_vector4array_sa::vvp_vector4array_sa(अचिन्हित width__, अचिन्हित शब्दगण_2)
: vvp_vector4array_t(width__, शब्दगण_2)
{
      array_ = नया v4cell[शब्दगण_3];

      अगर (width_ <= vvp_vector4_t::BITS_PER_WORD) {
	    क्रम (अचिन्हित idx = 0 ; idx < शब्दगण_3 ; idx += 1) {
		  array_[idx].abits_val_ = vvp_vector4_t::WORD_X_ABITS;
		  array_[idx].bbits_val_ = vvp_vector4_t::WORD_X_BBITS;
	    }
      } अन्यथा {
	    क्रम (अचिन्हित idx = 0 ; idx < शब्दगण_3 ; idx += 1) {
		  array_[idx].abits_ptr_ = 0;
		  array_[idx].bbits_ptr_ = 0;
	    }
      }
}

vvp_vector4array_sa::~vvp_vector4array_sa()
{
      अगर (array_) {
	    अगर (width_ > vvp_vector4_t::BITS_PER_WORD) {
		  क्रम (अचिन्हित idx = 0 ; idx < शब्दगण_3 ; idx += 1)
			अगर (array_[idx].abits_ptr_)
			      मिटाओ[]array_[idx].abits_ptr_;
	    }
	    मिटाओ[]array_;
      }
}

व्योम vvp_vector4array_sa::set_word(अचिन्हित सूचकांक, स्थिर vvp_vector4_t&that)
{
      निश्चित(सूचकांक < शब्दगण_3);

      v4cell*cell = &array_[सूचकांक];

      set_word_(cell, that);
}

vvp_vector4_t vvp_vector4array_sa::get_word(अचिन्हित सूचकांक) स्थिर
{
      अगर (सूचकांक >= शब्दगण_3)
	    वापस vvp_vector4_t(width_, BIT4_X);

      निश्चित(सूचकांक < शब्दगण_3);

      v4cell*cell = &array_[सूचकांक];

      वापस get_word_(cell);
}

vvp_vector4array_aa::vvp_vector4array_aa(अचिन्हित width__, अचिन्हित शब्दगण_2)
: vvp_vector4array_t(width__, शब्दगण_2)
{
      context_idx_ = vpip_add_item_to_context(यह, vpip_peek_context_scope());
}

vvp_vector4array_aa::~vvp_vector4array_aa()
{
}

व्योम vvp_vector4array_aa::alloc_instance(vvp_context_t context)
{
      v4cell*array = नया v4cell[शब्दगण_3];

      अगर (width_ <= vvp_vector4_t::BITS_PER_WORD) {
	    क्रम (अचिन्हित idx = 0 ; idx < शब्दगण_3 ; idx += 1) {
		  array[idx].abits_val_ = vvp_vector4_t::WORD_X_ABITS;
		  array[idx].bbits_val_ = vvp_vector4_t::WORD_X_BBITS;
	    }
      } अन्यथा {
	    क्रम (अचिन्हित idx = 0 ; idx < शब्दगण_3 ; idx += 1) {
		  array[idx].abits_ptr_ = 0;
		  array[idx].bbits_ptr_ = 0;
	    }
      }

      vvp_set_context_item(context, context_idx_, array);
}

व्योम vvp_vector4array_aa::reset_instance(vvp_context_t context)
{
      v4cell*cell = static_cast<v4cell*>
            (vvp_get_context_item(context, context_idx_));

      अगर (width_ <= vvp_vector4_t::BITS_PER_WORD) {
	    क्रम (अचिन्हित idx = 0 ; idx < शब्दगण_3 ; idx += 1) {
		  cell->abits_val_ = vvp_vector4_t::WORD_X_ABITS;
		  cell->bbits_val_ = vvp_vector4_t::WORD_X_BBITS;
                  cell++;
	    }
      } अन्यथा {
            अचिन्हित cnt = (width_ + vvp_vector4_t::BITS_PER_WORD-1)/vvp_vector4_t::BITS_PER_WORD;
	    क्रम (अचिन्हित idx = 0 ; idx < शब्दगण_3 ; idx += 1) {
		  अगर (cell->abits_ptr_) {
                        क्रम (अचिन्हित n = 0 ; n < cnt ; n += 1) {
			      cell->abits_ptr_[n] = vvp_vector4_t::WORD_X_ABITS;
			      cell->bbits_ptr_[n] = vvp_vector4_t::WORD_X_BBITS;
                        }
                  }
                  cell++;
	    }
      }
}

#अगर_घोषित CHECK_WITH_VALGRIND
व्योम vvp_vector4array_aa::free_instance(vvp_context_t context)
{
      v4cell*cell = static_cast<v4cell*>
            (vvp_get_context_item(context, context_idx_));
      मिटाओ [] cell;
}
#पूर्ण_अगर

व्योम vvp_vector4array_aa::set_word(अचिन्हित सूचकांक, स्थिर vvp_vector4_t&that)
{
      निश्चित(सूचकांक < शब्दगण_3);

      v4cell*cell = static_cast<v4cell*>
            (vthread_get_wt_context_item(context_idx_)) + सूचकांक;

      set_word_(cell, that);
}

vvp_vector4_t vvp_vector4array_aa::get_word(अचिन्हित सूचकांक) स्थिर
{
      अगर (सूचकांक >= शब्दगण_3)
	    वापस vvp_vector4_t(width_, BIT4_X);

      निश्चित(सूचकांक < शब्दगण_3);

      v4cell*cell = static_cast<v4cell*>
            (vthread_get_rd_context_item(context_idx_)) + सूचकांक;

      वापस get_word_(cell);
}

vvp_vector2_t::vvp_vector2_t()
{
      vec_ = 0;
      wid_ = 0;
}

vvp_vector2_t::vvp_vector2_t(uint64_t v, अचिन्हित wid)
{
      wid_ = wid;
      स्थिर अचिन्हित bits_per_word = 8 * माप(vec_[0]);
      स्थिर अचिन्हित शब्दगण = (wid_ + bits_per_word-1) / bits_per_word;

      vec_ = नया अचिन्हित दीर्घ[शब्दगण];
      क्रम (अचिन्हित idx = 0 ; idx < शब्दगण ; idx += 1) {
	    स्थिर अचिन्हित shift = idx * bits_per_word;
	    vec_[idx] = shift < 64 ? v >> shift : 0;
      }
}

vvp_vector2_t::vvp_vector2_t(vvp_vector2_t::fill_t fill, अचिन्हित wid)
{
      wid_ = wid;
      स्थिर अचिन्हित bits_per_word = 8 * माप(vec_[0]);
      स्थिर अचिन्हित शब्दगण = (wid_ + bits_per_word-1) / bits_per_word;

      vec_ = नया अचिन्हित दीर्घ[शब्दगण];
      क्रम (अचिन्हित idx = 0 ;  idx < शब्दगण ;  idx += 1)
	    vec_[idx] = fill? -1 : 0;
}

vvp_vector2_t::vvp_vector2_t(स्थिर vvp_vector2_t&that, अचिन्हित base, अचिन्हित wid)
{
      wid_ = wid;
      स्थिर अचिन्हित शब्दगण = (wid_ + BITS_PER_WORD-1) / BITS_PER_WORD;

      vec_ = नया अचिन्हित दीर्घ[शब्दगण];

      क्रम (अचिन्हित idx = 0 ; idx < wid ; idx += 1) {
	    पूर्णांक bit = that.value(base+idx);
	    अगर (bit == 0)
		  जारी;
	    अचिन्हित word = idx / BITS_PER_WORD;
	    अचिन्हित दीर्घ mask = 1UL << (idx % BITS_PER_WORD);
	    vec_[word] |= mask;
      }
}

व्योम vvp_vector2_t::copy_from_that_(स्थिर vvp_vector4_t&that)
{
      wid_ = that.size();
      स्थिर अचिन्हित शब्दगण = (that.size() + BITS_PER_WORD-1) / BITS_PER_WORD;

      अगर (शब्दगण == 0) {
	    vec_ = 0;
	    wid_ = 0;
	    वापस;
      }

	// Use the subarray method with the xz_to_0 flag set so that
	// we get values even when there are xz bits.
      vec_ = that.subarray(0, wid_, true);
}

व्योम vvp_vector2_t::copy_from_that_(स्थिर vvp_vector2_t&that)
{
      wid_ = that.wid_;
      स्थिर अचिन्हित शब्दगण = (wid_ + BITS_PER_WORD-1) / BITS_PER_WORD;

      अगर (शब्दगण == 0) {
	    vec_ = 0;
	    wid_ = 0;
	    वापस;
      }

      vec_ = नया अचिन्हित दीर्घ[शब्दगण];
      क्रम (अचिन्हित idx = 0 ;  idx < शब्दगण ;  idx += 1)
	    vec_[idx] = that.vec_[idx];
}

vvp_vector2_t::vvp_vector2_t(स्थिर vvp_vector2_t&that, अचिन्हित newsize)
{
      wid_ = newsize;
      अगर (newsize == 0) {
	    vec_ = 0;
	    वापस;
      }

      स्थिर अचिन्हित शब्दगण = (wid_ + BITS_PER_WORD-1) / BITS_PER_WORD;
      स्थिर अचिन्हित twords = (that.wid_ + BITS_PER_WORD-1) / BITS_PER_WORD;

      vec_ = नया अचिन्हित दीर्घ[शब्दगण];
      क्रम (अचिन्हित idx = 0 ;  idx < शब्दगण ;  idx += 1) {
	    अगर (idx < twords)
		  vec_[idx] = that.vec_[idx];
	    अन्यथा
		  vec_[idx] = 0;
      }
}

vvp_vector2_t& vvp_vector2_t::चालक= (स्थिर vvp_vector2_t&that)
{
      अगर (यह == &that)
	    वापस *यह;

      मिटाओ[] vec_;
      vec_ = 0;

      copy_from_that_(that);
      वापस *यह;
}

vvp_vector2_t& vvp_vector2_t::चालक= (स्थिर vvp_vector4_t&that)
{
      मिटाओ[]vec_;
      vec_ = 0;
      copy_from_that_(that);
      वापस *यह;
}

vvp_vector2_t& vvp_vector2_t::चालक <<= (अचिन्हित पूर्णांक shift)
{
      अगर (wid_ == 0)
	    वापस *यह;

      स्थिर अचिन्हित शब्दगण = (wid_ + BITS_PER_WORD-1) / BITS_PER_WORD;

	// Number of words to shift
      स्थिर अचिन्हित wshift = shift / BITS_PER_WORD;
	// bits to shift within each word.
      स्थिर अचिन्हित दीर्घ oshift = shift % BITS_PER_WORD;

	// If shifting the entire value away, then return zeros.
      अगर (wshift >= शब्दगण) {
	    क्रम (अचिन्हित idx = 0 ;  idx < शब्दगण ;  idx += 1)
		  vec_[idx] = 0;

	    वापस *यह;
      }

	// Do the word shift first.
      अगर (wshift > 0) {
	    क्रम (अचिन्हित idx = 0 ;  idx < शब्दगण-wshift ;  idx += 1) {
		  अचिन्हित sel = शब्दगण - idx - 1;
		  vec_[sel] = vec_[sel-wshift];
	    }

	    क्रम (अचिन्हित idx = 0 ;  idx < wshift ;  idx += 1)
		  vec_[idx] = 0;
      }

	// Do the fine shift.
      अगर (oshift != 0) {
	    अचिन्हित दीर्घ pad = 0;
	    क्रम (अचिन्हित idx = 0 ;  idx < शब्दगण ;  idx += 1) {
		  अचिन्हित दीर्घ अग्र_pad = vec_[idx] >> (BITS_PER_WORD-oshift);
		  vec_[idx] = (vec_[idx] << oshift) | pad;
		  pad = अग्र_pad;
	    }

	      // Cleanup the tail bits.
	    अचिन्हित दीर्घ mask = -1UL >> (BITS_PER_WORD - wid_%BITS_PER_WORD);
	    vec_[शब्दगण-1] &= mask;
      }

      वापस *यह;
}

vvp_vector2_t& vvp_vector2_t::चालक >>= (अचिन्हित shift)
{
      अगर (wid_ == 0)
	    वापस *यह;

      स्थिर अचिन्हित शब्दगण = (wid_ + BITS_PER_WORD-1) / BITS_PER_WORD;

	// Number of words to shift
      स्थिर अचिन्हित wshift = shift / BITS_PER_WORD;
	// bits to shift within each word.
      स्थिर अचिन्हित दीर्घ oshift = shift % BITS_PER_WORD;

	// If shifting the entire value away, then return zeros.
      अगर (wshift >= शब्दगण) {
	    क्रम (अचिन्हित idx = 0 ;  idx < शब्दगण ;  idx += 1)
		  vec_[idx] = 0;

	    वापस *यह;
      }

      अगर (wshift > 0) {
	    क्रम (अचिन्हित idx = 0 ;  idx < शब्दगण-wshift ;  idx += 1)
		  vec_[idx] = vec_[idx+wshift];

	    क्रम (अचिन्हित idx = शब्दगण-wshift ;  idx < शब्दगण ;  idx += 1)
		  vec_[idx] = 0;
      }

      अगर (oshift > 0) {
	    अचिन्हित दीर्घ pad = 0;
	    क्रम (अचिन्हित idx = शब्दगण ;  idx > 0 ;  idx -= 1) {
		  अचिन्हित दीर्घ new_pad = vec_[idx-1] <<(BITS_PER_WORD-oshift);
		  vec_[idx-1] = pad | (vec_[idx-1] >> oshift);
		  pad = new_pad;
	    }

	      // Cleanup the tail bits.

	    अचिन्हित use_शब्दगण = शब्दगण;
	      // Mask_shift is the number of high bits of the top word
	      // that are to be masked off. We start with the number
	      // of bits that are not included even in the original,
	      // then we include the bits of the shift, that are to be
	      // masked to zero.
	    अचिन्हित दीर्घ mask_shift = BITS_PER_WORD - wid_%BITS_PER_WORD;
	    mask_shift %= BITS_PER_WORD;
	    mask_shift += oshift;
	    जबतक (mask_shift >= BITS_PER_WORD) {
		  vec_[use_शब्दगण-1] = 0;
		  use_शब्दगण -= 1;
		  mask_shift -= BITS_PER_WORD;
	    }
	    अगर (mask_shift > 0) {
		  निश्चित(use_शब्दगण > 0);
		  अचिन्हित दीर्घ mask = -1UL >> mask_shift;
		  vec_[use_शब्दगण-1] &= mask;
	    }
      }

      वापस *यह;
}

अटल अचिन्हित दीर्घ add_carry(अचिन्हित दीर्घ a, अचिन्हित दीर्घ b,
			       अचिन्हित दीर्घ&carry)
{
      अचिन्हित दीर्घ out = carry;
      carry = 0;

      अगर ((अच_दीर्घ_उच्च - out) < a)
	    carry += 1;
      out += a;

      अगर ((अच_दीर्घ_उच्च - out) < b)
	    carry += 1;
      out += b;

      वापस out;
}

vvp_vector2_t& vvp_vector2_t::चालक += (स्थिर vvp_vector2_t&that)
{
      निश्चित(wid_ == that.wid_);
      अगर (wid_ == 0)
	    वापस *यह;

      स्थिर अचिन्हित शब्दगण = (wid_ + BITS_PER_WORD-1) / BITS_PER_WORD;

      अचिन्हित दीर्घ carry = 0;
      क्रम (अचिन्हित idx = 0 ;  idx < शब्दगण ;  idx += 1) {
	    vec_[idx] = add_carry(vec_[idx], that.vec_[idx], carry);
      }


	// Cleanup the tail bits.
      अचिन्हित दीर्घ mask = -1UL >> (BITS_PER_WORD - wid_%BITS_PER_WORD);
      vec_[शब्दगण-1] &= mask;

      वापस *यह;
}

vvp_vector2_t& vvp_vector2_t::चालक -= (स्थिर vvp_vector2_t&that)
{
      निश्चित(wid_ == that.wid_);
      अगर (wid_ == 0)
	    वापस *यह;

      स्थिर अचिन्हित शब्दगण = (wid_ + BITS_PER_WORD-1) / BITS_PER_WORD;

      अचिन्हित दीर्घ carry = 1;
      क्रम (अचिन्हित idx = 0 ;  idx < शब्दगण ;  idx += 1) {
	    vec_[idx] = add_carry(vec_[idx], ~that.vec_[idx], carry);
      }

      वापस *यह;
}

व्योम vvp_vector2_t::trim()
{
      जबतक (value(wid_-1) == 0 && wid_ > 1) wid_ -= 1;
}

/* This is a special trim that is used on numbers we know represent a
 * negative signed value (they came from a negative real value). */
व्योम vvp_vector2_t::trim_neg()
{
      अगर (value(wid_-1) == 1 && wid_ > 32) {
	    जबतक (value(wid_-2) == 1 && wid_ > 32) wid_ -= 1;
      }
}

पूर्णांक vvp_vector2_t::value(अचिन्हित idx) स्थिर
{
      अगर (idx >= wid_)
	    वापस 0;

      स्थिर अचिन्हित bits_per_word = 8 * माप(vec_[0]);
      अचिन्हित addr = idx/bits_per_word;
      अचिन्हित mask = idx%bits_per_word;

      अगर (vec_[addr] & (1UL<<mask))
	    वापस 1;
      अन्यथा
	    वापस 0;
}

व्योम vvp_vector2_t::set_bit(अचिन्हित idx, पूर्णांक bit)
{
      निश्चित(idx < wid_);

      स्थिर अचिन्हित bits_per_word = 8 * माप(vec_[0]);
      अचिन्हित addr = idx/bits_per_word;
      अचिन्हित दीर्घ mask = idx%bits_per_word;

      अगर (bit)
	    vec_[addr] |= 1UL << mask;
      अन्यथा
	    vec_[addr] &= ~(1UL << mask);
}

व्योम vvp_vector2_t::set_vec(अचिन्हित adr, स्थिर vvp_vector2_t&that)
{
      निश्चित((adr + that.wid_) <= wid_);

      क्रम (अचिन्हित idx = 0 ; idx < that.wid_ ; idx += 1)
	    set_bit(adr+idx, that.value(idx));
}

bool vvp_vector2_t::is_NaN() स्थिर
{
      वापस wid_ == 0;
}

bool vvp_vector2_t::is_zero() स्थिर
{
      स्थिर अचिन्हित शब्दगण = (wid_ + BITS_PER_WORD-1) / BITS_PER_WORD;

      क्रम (अचिन्हित idx = 0; idx < शब्दगण; idx += 1) {
	    अगर (vec_[idx] == 0) जारी;
	    वापस false;
      }

      वापस true;
}

/*
 * Basic idea from "Introduction to Programming using SML" by
 * Michael R. Hansen and Hans Rischel page 261 and "Seminumerical
 * Algorithms, Third Edition" by Donald E. Knuth section 4.6.3.
 */
vvp_vector2_t घात(स्थिर vvp_vector2_t&x, vvp_vector2_t&y)
{
        /* If we have a zero exponent just return 1. */
      अगर (y == vvp_vector2_t(0L, 1)) {
	    वापस vvp_vector2_t(1L, x.size());
      }

        /* Is the value odd? */
      अगर (y.value(0) == 1) {
	    y.set_bit(0, 0);  // A quick subtract by 1.
	    vvp_vector2_t res = x * घात(x, y);
	    वापस res;
      }

      y >>= 1;  // A fast divide by two. We know the LSB is zero.
      vvp_vector2_t z = घात(x, y);
      vvp_vector2_t res = z * z;
      वापस res;
}

अटल व्योम multiply_long(अचिन्हित दीर्घ a, अचिन्हित दीर्घ b,
			  अचिन्हित दीर्घ&निचला, अचिन्हित दीर्घ&ऊपरी)
{
      निश्चित(माप(अचिन्हित दीर्घ) %2 == 0);

      स्थिर अचिन्हित दीर्घ word_mask = (1UL << 4UL*माप(a)) - 1UL;
      अचिन्हित दीर्घ tmpa;
      अचिन्हित दीर्घ tmpb;
      अचिन्हित दीर्घ res[4];

      tmpa = a & word_mask;
      tmpb = b & word_mask;
      res[0] = tmpa * tmpb;
      res[1] = res[0] >> 4UL*माप(अचिन्हित दीर्घ);
      res[0] &= word_mask;

      tmpa = (a >> 4UL*माप(अचिन्हित दीर्घ)) & word_mask;
      res[1] += tmpa * tmpb;
      res[2] = res[1] >> 4UL*माप(अचिन्हित दीर्घ);
      res[1] &= word_mask;

      tmpa = a & word_mask;
      tmpb = (b >> 4UL*माप(अचिन्हित दीर्घ)) & word_mask;
      res[1] += tmpa * tmpb;
      res[2] += res[1] >> 4UL*माप(अचिन्हित दीर्घ);
      res[3]  = res[2] >> 4UL*माप(अचिन्हित दीर्घ);
      res[1] &= word_mask;
      res[2] &= word_mask;

      tmpa = (a >> 4UL*माप(अचिन्हित दीर्घ)) & word_mask;
      res[2] += tmpa * tmpb;
      res[3] += res[2] >> 4UL*माप(अचिन्हित दीर्घ);
      res[2] &= word_mask;

      ऊपरी = (res[3] << 4UL*माप(अचिन्हित दीर्घ)) | res[2];
      निचला  = (res[1] << 4UL*माप(अचिन्हित दीर्घ)) | res[0];
}

vvp_vector2_t चालक * (स्थिर vvp_vector2_t&a, स्थिर vvp_vector2_t&b)
{
      स्थिर अचिन्हित bits_per_word = 8 * माप(a.vec_[0]);

	// The compiler ensures that the two operands are of equal size.
      निश्चित(a.size() == b.size());
      vvp_vector2_t r (0, a.size());

      अचिन्हित शब्दगण = (r.wid_ + bits_per_word - 1) / bits_per_word;

      क्रम (अचिन्हित bdx = 0 ;  bdx < शब्दगण ;  bdx += 1) {
	    अचिन्हित दीर्घ tmpb = b.vec_[bdx];
	    अगर (tmpb == 0)
		  जारी;

	    क्रम (अचिन्हित adx = 0 ;  adx < शब्दगण ;  adx += 1) {
		  अचिन्हित दीर्घ tmpa = a.vec_[adx];
		  अगर (tmpa == 0)
			जारी;

		  अचिन्हित दीर्घ निचला, hig;
		  multiply_long(tmpa, tmpb, निचला, hig);

		  अचिन्हित दीर्घ carry = 0;
		  क्रम (अचिन्हित sdx = 0
			     ; (adx+bdx+sdx) < शब्दगण
			     ;  sdx += 1) {

			r.vec_[adx+bdx+sdx] = add_carry(r.vec_[adx+bdx+sdx],
							निचला, carry);
			निचला = hig;
			hig = 0;
		  }
	    }
      }

      वापस r;
}

अटल व्योम div_mod (vvp_vector2_t dividend, vvp_vector2_t भाजक,
		     vvp_vector2_t&quotient, vvp_vector2_t&remainder)
{

      quotient = vvp_vector2_t(0, dividend.size());

      अगर (भाजक == quotient) {
	    अ_त्रुटि << "ERROR: division by zero, exiting." << प_पूर्ण;
	    निकास(255);
      }

      अगर (dividend < भाजक) {
	    remainder = dividend;
	    वापस;
      }

      vvp_vector2_t mask (1, dividend.size());

	// Make the dividend 1 bit larger to prevent overflow of
	// divtmp in startup.
      dividend = vvp_vector2_t(dividend, dividend.size()+1);
      vvp_vector2_t divtmp (भाजक, dividend.size());

      जबतक (divtmp < dividend) {
	    divtmp <<= 1;
	    mask <<= 1;
      }

      जबतक (dividend >= भाजक) {
	    अगर (divtmp <= dividend) {
		  dividend -= divtmp;
		  quotient += mask;
	    }

	    divtmp >>= 1;
	    mask >>= 1;
      }

      remainder = vvp_vector2_t(dividend, mask.size());
}

vvp_vector2_t चालक - (स्थिर vvp_vector2_t&that)
{
      vvp_vector2_t neg(that);
      अगर (neg.wid_ == 0) वापस neg;

      स्थिर अचिन्हित शब्दगण = (neg.wid_ + neg.BITS_PER_WORD-1) /
                                         neg.BITS_PER_WORD;
      क्रम (अचिन्हित idx = 0 ;  idx < शब्दगण ;  idx += 1) {
	    neg.vec_[idx] = ~neg.vec_[idx];
      }
      neg += vvp_vector2_t(1, neg.wid_);

      वापस neg;
}

vvp_vector2_t चालक / (स्थिर vvp_vector2_t&dividend,
			  स्थिर vvp_vector2_t&भाजक)
{
      vvp_vector2_t quot, rem;
      div_mod(dividend, भाजक, quot, rem);
      वापस quot;
}

vvp_vector2_t चालक % (स्थिर vvp_vector2_t&dividend,
			  स्थिर vvp_vector2_t&भाजक)
{
      vvp_vector2_t quot, rem;
      div_mod(dividend, भाजक, quot, rem);
      वापस rem;
}

bool चालक > (स्थिर vvp_vector2_t&a, स्थिर vvp_vector2_t&b)
{
      स्थिर अचिन्हित awords = (a.wid_ + vvp_vector2_t::BITS_PER_WORD-1) / vvp_vector2_t::BITS_PER_WORD;
      स्थिर अचिन्हित bwords = (b.wid_ + vvp_vector2_t::BITS_PER_WORD-1) / vvp_vector2_t::BITS_PER_WORD;

      स्थिर अचिन्हित शब्दगण = awords > bwords? awords : bwords;

      क्रम (अचिन्हित idx = शब्दगण ;  idx > 0 ;  idx -= 1) {
	    अचिन्हित दीर्घ aw = (idx <= awords)? a.vec_[idx-1] : 0;
	    अचिन्हित दीर्घ bw = (idx <= bwords)? b.vec_[idx-1] : 0;

	    अगर (aw > bw)
		  वापस true;
	    अगर (aw < bw)
		  वापस false;
      }

	// If the above loop finishes, then the vectors are equal.
      वापस false;
}

bool चालक >= (स्थिर vvp_vector2_t&a, स्थिर vvp_vector2_t&b)
{
      स्थिर अचिन्हित awords = (a.wid_ + vvp_vector2_t::BITS_PER_WORD-1) / vvp_vector2_t::BITS_PER_WORD;
      स्थिर अचिन्हित bwords = (b.wid_ + vvp_vector2_t::BITS_PER_WORD-1) / vvp_vector2_t::BITS_PER_WORD;

      स्थिर अचिन्हित शब्दगण = awords > bwords? awords : bwords;

      क्रम (अचिन्हित idx = शब्दगण ;  idx > 0 ;  idx -= 1) {
	    अचिन्हित दीर्घ aw = (idx <= awords)? a.vec_[idx-1] : 0;
	    अचिन्हित दीर्घ bw = (idx <= bwords)? b.vec_[idx-1] : 0;

	    अगर (aw > bw)
		  वापस true;
	    अगर (aw < bw)
		  वापस false;
      }

	// If the above loop finishes, then the vectors are equal.
      वापस true;
}

bool चालक < (स्थिर vvp_vector2_t&a, स्थिर vvp_vector2_t&b)
{
      स्थिर अचिन्हित awords = (a.wid_ + vvp_vector2_t::BITS_PER_WORD-1) / vvp_vector2_t::BITS_PER_WORD;
      स्थिर अचिन्हित bwords = (b.wid_ + vvp_vector2_t::BITS_PER_WORD-1) / vvp_vector2_t::BITS_PER_WORD;

      अचिन्हित शब्दगण = awords;
      अगर (bwords > शब्दगण)
	    शब्दगण = bwords;

      क्रम (अचिन्हित idx = शब्दगण ;  idx > 0 ;  idx -= 1) {
	    अचिन्हित दीर्घ aw = (idx <= awords)? a.vec_[idx-1] : 0;
	    अचिन्हित दीर्घ bw = (idx <= bwords)? b.vec_[idx-1] : 0;

	    अगर (aw < bw)
		  वापस true;
	    अगर (aw > bw)
		  वापस false;
      }

	// If the above loop finishes, then the vectors are equal.
      वापस false;
}

bool चालक <= (स्थिर vvp_vector2_t&a, स्थिर vvp_vector2_t&b)
{
	// XXXX For now, only support equal width vectors.
      निश्चित(a.wid_ == b.wid_);

      स्थिर अचिन्हित awords = (a.wid_ + vvp_vector2_t::BITS_PER_WORD-1) / vvp_vector2_t::BITS_PER_WORD;

      क्रम (अचिन्हित idx = awords ;  idx > 0 ;  idx -= 1) {
	    अगर (a.vec_[idx-1] < b.vec_[idx-1])
		  वापस true;
	    अगर (a.vec_[idx-1] > b.vec_[idx-1])
		  वापस false;
      }

	// If the above loop finishes, then the vectors are equal.
      वापस true;
}

bool चालक == (स्थिर vvp_vector2_t&a, स्थिर vvp_vector2_t&b)
{
      स्थिर अचिन्हित awords = (a.wid_ + vvp_vector2_t::BITS_PER_WORD-1) / vvp_vector2_t::BITS_PER_WORD;
      स्थिर अचिन्हित bwords = (b.wid_ + vvp_vector2_t::BITS_PER_WORD-1) / vvp_vector2_t::BITS_PER_WORD;

      स्थिर अचिन्हित शब्दगण = awords > bwords? awords : bwords;

      क्रम (अचिन्हित idx = शब्दगण ;  idx > 0 ;  idx -= 1) {
	    अचिन्हित दीर्घ aw = (idx <= awords)? a.vec_[idx-1] : 0;
	    अचिन्हित दीर्घ bw = (idx <= bwords)? b.vec_[idx-1] : 0;

	    अगर (aw > bw)
		  वापस false;
	    अगर (aw < bw)
		  वापस false;
      }

	// If the above loop finishes, then the vectors are equal.
      वापस true;
}


vvp_vector4_t vector2_to_vector4(स्थिर vvp_vector2_t&that, अचिन्हित wid)
{
      vvp_vector4_t res (wid);

      क्रम (अचिन्हित idx = 0 ;  idx < res.size() ;  idx += 1) {
	    vvp_bit4_t bit = BIT4_0;

	    अगर (that.value(idx))
		  bit = BIT4_1;

	    res.set_bit(idx, bit);
      }

      वापस res;
}

अटल bool c4string_header_test(स्थिर अक्षर*str)
{
      अगर ((str[0] != 'C') && (str[0] != 'c'))
	    वापस false;
      अगर ((str[1] != '4') || (str[2] != '<'))
	    वापस false;
      वापस true;
}

bool c4string_test(स्थिर अक्षर*str)
{
      अगर (!c4string_header_test(str))
	    वापस false;
      माप_प्रकार मूल्य_आकार = म_अखोज(str+3, "01xz");
      अगर (str[3+मूल्य_आकार] != '>')
	    वापस false;
      अगर (str[3+मूल्य_आकार+1] != 0)
	    वापस false;

      वापस true;
}

vvp_vector4_t c4string_to_vector4(स्थिर अक्षर*str)
{
      निश्चित(c4string_header_test(str));

      str += 3;
      स्थिर अक्षर*tp = str + म_अखोज(str,"01xz");
      निश्चित(tp[0] == '>');

      vvp_vector4_t tmp (tp-str);

      क्रम (अचिन्हित idx = 0 ;  idx < tmp.size() ;  idx += 1) {
	    vvp_bit4_t bit;
	    चयन (str[idx]) {
		हाल '0':
		  bit = BIT4_0;
		  अवरोध;
		हाल '1':
		  bit = BIT4_1;
		  अवरोध;
		हाल 'x':
		  bit = BIT4_X;
		  अवरोध;
		हाल 'z':
		  bit = BIT4_Z;
		  अवरोध;
		शेष:
		  ख_लिखो(मानक_त्रुटि, "Unsupported bit value %c(%d).\n", str[idx],
		          str[idx]);
		  निश्चित(0);
		  bit = BIT4_0;
		  अवरोध;
	    }
	    tmp.set_bit(tmp.size()-idx-1, bit);
      }

      वापस tmp;
}

ब_प्रवाह& चालक<< (ब_प्रवाह&out, स्थिर vvp_vector2_t&that)
{
      अगर (that.is_NaN()) {
	    out << "NaN";

      } अन्यथा {
	    out << vector2_to_vector4(that, that.size());
      }
      वापस out;
}

vvp_vector8_t::vvp_vector8_t(स्थिर vvp_vector8_t&that)
{
      आकार_2 = that.आकार_2;
      अगर (आकार_2 <= माप(val_)) {
	    स_नकल(val_, that.val_, माप val_);
      } अन्यथा {
	    ptr_ = नया अचिन्हित अक्षर[आकार_2];
	    स_नकल(ptr_, that.ptr_, आकार_2);
      }
}

vvp_vector8_t::vvp_vector8_t(स्थिर vvp_vector4_t&that,
			     अचिन्हित str0, अचिन्हित str1)
: आकार_2(that.size())
{
      अगर (आकार_2 == 0)
	    वापस;

      अगर (आकार_2 <= माप(val_)) {
	    ptr_ = 0; // Prefill all val_ bytes
	    क्रम (अचिन्हित idx = 0 ; idx < आकार_2 ; idx += 1)
		  val_[idx] = vvp_scalar_t(that.value(idx),str0, str1).raw();
      } अन्यथा {
	    ptr_ = नया अचिन्हित अक्षर[आकार_2];
	    क्रम (अचिन्हित idx = 0 ;  idx < आकार_2 ;  idx += 1)
		  ptr_[idx] = vvp_scalar_t(that.value(idx), str0, str1).raw();
      }
}

vvp_vector8_t::vvp_vector8_t(स्थिर vvp_vector2_t&that,
			     अचिन्हित str0, अचिन्हित str1)
: आकार_2(that.size())
{
      अगर (आकार_2 == 0)
	    वापस;

      अगर (आकार_2 <= माप(val_)) {
	    ptr_ = 0;
	    क्रम (अचिन्हित idx = 0 ; idx < आकार_2 ; idx += 1)
		  val_[idx] = vvp_scalar_t(that.value(idx)? BIT4_1:BIT4_0, str0, str1).raw();
      } अन्यथा {
	    ptr_ = नया अचिन्हित अक्षर[आकार_2];
	    क्रम (अचिन्हित idx = 0 ; idx < आकार_2 ; idx += 1)
		  ptr_[idx] = vvp_scalar_t(that.value(idx)? BIT4_1:BIT4_0, str0, str1).raw();
      }
}

स्थिर vvp_vector8_t vvp_vector8_t::nil;

vvp_vector8_t& vvp_vector8_t::चालक= (स्थिर vvp_vector8_t&that)
{
	// Assign to self.
      अगर (यह == &that)
	    वापस *यह;

      अगर (आकार_2 != that.आकार_2) {
	    अगर (आकार_2 > माप(val_))
		  मिटाओ[]ptr_;
	    आकार_2 = 0;
      }

      अगर (that.आकार_2 == 0) {
	    निश्चित(आकार_2 == 0);
	    वापस *यह;
      }

      अगर (that.आकार_2 <= माप(val_)) {
	    आकार_2 = that.आकार_2;
	    स_नकल(val_, that.val_, माप(val_));
	    वापस *यह;
      }

      अगर (आकार_2 == 0) {
	    आकार_2 = that.आकार_2;
	    ptr_ = नया अचिन्हित अक्षर[आकार_2];
      }

      स_नकल(ptr_, that.ptr_, आकार_2);

      वापस *यह;
}

vvp_vector8_t vvp_vector8_t::subvalue(अचिन्हित base, अचिन्हित wid) स्थिर
{
      vvp_vector8_t tmp (wid);

      अचिन्हित अक्षर*tmp_ptr = tmp.आकार_2 <= माप(val_) ? tmp.val_ : tmp.ptr_;
      स्थिर अचिन्हित अक्षर*use_ptr = आकार_2 <= माप(val_) ? val_ : ptr_;

      अचिन्हित idx = 0;
      जबतक ((idx < wid) && (base+idx < आकार_2)) {
	    tmp_ptr[idx] = use_ptr[base+idx];
	    idx += 1;
      }

      वापस tmp;
}

व्योम vvp_vector8_t::set_vec(अचिन्हित base, स्थिर vvp_vector8_t&that)
{
      निश्चित((base+that.size()) <= size());
      क्रम (अचिन्हित idx = 0 ; idx < that.size() ; idx += 1)
	    set_bit(base+idx, that.value(idx));
}

vvp_vector8_t part_expand(स्थिर vvp_vector8_t&that, अचिन्हित wid, अचिन्हित off)
{
      निश्चित(off < wid);
      vvp_vector8_t tmp (wid);

      अचिन्हित अक्षर* tmp_ptr = tmp.आकार_2<= माप(tmp.val_) ?
                               tmp.val_ : tmp.ptr_;
      स्थिर अचिन्हित अक्षर* that_ptr = that.आकार_2<= माप(that.val_) ?
                                      that.val_ : that.ptr_;

      अचिन्हित idx = off;

      जबतक (idx < wid && that.आकार_2 > (idx-off)) {
	    tmp_ptr[idx] = that_ptr[idx-off];
	    idx += 1;
      }

      वापस tmp;
}

अटल bool c8string_header_test(स्थिर अक्षर*str)
{
      अगर ((str[0] != 'C') && (str[0] != 'c'))
	    वापस false;
      अगर ((str[1] != '8') || (str[2] != '<'))
	    वापस false;
      वापस true;
}

bool c8string_test(स्थिर अक्षर*str)
{
      अगर (!c8string_header_test(str))
	    वापस false;

      स्थिर अक्षर*cp = str+3;
      क्रम (;; cp += 1) {
	    अगर (cp[0] == '>' && cp[1] == 0) वापस true;
	    अगर (cp[0] >= '0' && cp[0] <= '9') जारी;
	    अगर (cp[0] == 'x') जारी;
	    अगर (cp[0] == 'z') जारी;
	    वापस false;
      }
      वापस false;
}
/*
 * The format of a C8<> string is:
 *   C8<aaabbbccc...>
 * where aaa... is a 3 character bit descriptor.
 */
vvp_vector8_t c8string_to_vector8(स्थिर अक्षर*str)
{
      निश्चित(c8string_header_test(str));

      माप_प्रकार vsize = म_माप(str)-4;
      निश्चित(vsize%3 == 0);
      vsize /= 3;
      vvp_vector8_t tmp (vsize);

      क्रम (माप_प्रकार idx = 0 ; idx < vsize ; idx += 1) {
	    स्थिर अक्षर*cp = str+3+3*idx;
	    vvp_bit4_t bit = BIT4_X;
	    अचिन्हित dr0 = cp[0]-'0';
	    अचिन्हित dr1 = cp[1]-'0';
	    चयन (cp[2]) {
		हाल '0':
		  bit = BIT4_0;
		  अवरोध;
		हाल '1':
		  bit = BIT4_1;
		  अवरोध;
		हाल 'x':
		  bit = BIT4_X;
		  अवरोध;
		हाल 'z':
		  bit = BIT4_Z;
		  अवरोध;
	    }
	    tmp.set_bit(vsize-idx-1, vvp_scalar_t(bit, dr0, dr1));
      }

      वापस tmp;
}

ब_प्रवाह& चालक<<(ब_प्रवाह&out, स्थिर vvp_vector8_t&that)
{
      out << "C8<";
      क्रम (अचिन्हित idx = 0 ;  idx < that.size() ; idx += 1)
	    out << that.value(that.size()-idx-1);

      out << ">";
      वापस out;
}

vvp_net_fun_t::vvp_net_fun_t()
{
      गिनती_functors += 1;
}

vvp_net_fun_t::~vvp_net_fun_t()
{
}

व्योम vvp_net_fun_t::recv_vec4(vvp_net_ptr_t, स्थिर vvp_vector4_t&,
                              vvp_context_t)
{
      ख_लिखो(मानक_त्रुटि, "internal error: %s: recv_vec4 not implemented\n",
	      typeid(*यह).name());
      निश्चित(0);
}

व्योम vvp_net_fun_t::recv_vec4_pv_(vvp_net_ptr_t p, स्थिर vvp_vector4_t&bit,
                                  अचिन्हित base, अचिन्हित vwid, vvp_context_t)
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

      निश्चित(base + bit.size() <= vwid);

      vvp_vector4_t tmp(vwid, BIT4_Z);
      tmp.set_vec(base, bit);
      recv_vec4(p, tmp, 0);
}

व्योम vvp_net_fun_t::recv_vec4_pv(vvp_net_ptr_t, स्थिर vvp_vector4_t&bit,
                                 अचिन्हित base, अचिन्हित vwid, vvp_context_t)
{
      अ_त्रुटि << "internal error: " << typeid(*यह).name() << ": "
	   << "recv_vec4_pv(" << bit << ", " << base
	   << ", " << vwid << ") not implemented" << प_पूर्ण;
      निश्चित(0);
}

व्योम vvp_net_fun_t::recv_vec8(vvp_net_ptr_t port, स्थिर vvp_vector8_t&bit)
{
      recv_vec4(port, reduce4(bit), 0);
}

व्योम vvp_net_fun_t::recv_vec8_pv_(vvp_net_ptr_t p, स्थिर vvp_vector8_t&bit,
				  अचिन्हित base, अचिन्हित vwid)
{
	// This is the strength-aware version of recv_vec4_pv_.

      निश्चित(base + bit.size() <= vwid);

      vvp_vector8_t tmp(vwid);
      tmp.set_vec(base, bit);
      recv_vec8(p, tmp);
}

व्योम vvp_net_fun_t::recv_vec8_pv(vvp_net_ptr_t port, स्थिर vvp_vector8_t&bit,
				 अचिन्हित base, अचिन्हित vwid)
{
      recv_vec4_pv(port, reduce4(bit), base, vwid, 0);
}

व्योम vvp_net_fun_t::recv_real(vvp_net_ptr_t, द्विगुन bit, vvp_context_t)
{
      ख_लिखो(मानक_त्रुटि, "internal error: %s: recv_real(%f) not implemented\n",
	      typeid(*यह).name(), bit);
      निश्चित(0);
}

व्योम vvp_net_fun_t::recv_string(vvp_net_ptr_t, स्थिर std::string&bit, vvp_context_t)
{
      ख_लिखो(मानक_त्रुटि, "internal error: %s: recv_string(%s) not implemented\n",
	      typeid(*यह).name(), bit.c_str());
      निश्चित(0);
}

व्योम vvp_net_fun_t::recv_object(vvp_net_ptr_t, vvp_object_t, vvp_context_t)
{
      ख_लिखो(मानक_त्रुटि, "internal error: %s: recv_object(...) not implemented\n",
	      typeid(*यह).name());
      निश्चित(0);
}

व्योम vvp_net_fun_t::force_flag(bool)
{
}

/* **** vvp_fun_drive methods **** */

vvp_fun_drive::vvp_fun_drive(अचिन्हित str0, अचिन्हित str1)
{
      निश्चित(str0 < 8);
      निश्चित(str1 < 8);

      drive0_ = str0;
      drive1_ = str1;
}

vvp_fun_drive::~vvp_fun_drive()
{
}

व्योम vvp_fun_drive::recv_vec4(vvp_net_ptr_t port, स्थिर vvp_vector4_t&bit,
                              vvp_context_t)
{
      निश्चित(port.port() == 0);
      port.ptr()->send_vec8(vvp_vector8_t(bit, drive0_, drive1_));
}

व्योम vvp_fun_drive::recv_vec4_pv(vvp_net_ptr_t ptr, स्थिर vvp_vector4_t&bit,
				 अचिन्हित base, अचिन्हित vwid, vvp_context_t ctx)
{
      recv_vec4_pv_(ptr, bit, base, vwid, ctx);
}

/* **** vvp_wide_fun_* methods **** */

vvp_wide_fun_core::vvp_wide_fun_core(vvp_net_t*net, अचिन्हित nports)
{
      ptr_ = net;
      nports_ = nports;
      port_मूल्यावली = 0;
      port_rvalues_ = 0;
}

vvp_wide_fun_core::~vvp_wide_fun_core()
{
      मिटाओ[] port_मूल्यावली;
      मिटाओ[] port_rvalues_;
}

व्योम vvp_wide_fun_core::propagate_vec4(स्थिर vvp_vector4_t&bit,
				       vvp_time64_t delay)
{
      अगर (delay)
	    schedule_propagate_vector(ptr_, delay, bit);
      अन्यथा
	    ptr_->send_vec4(bit, 0);
}

व्योम vvp_wide_fun_core::propagate_real(द्विगुन bit,
				       vvp_time64_t delay)
{
      अगर (delay) {
	      // schedule_assign_vector(ptr_->out, bit, delay);
	    निश्चित(0); // Need a real-value version of assign_vector.
      } अन्यथा {
	    ptr_->send_real(bit, 0);
      }
}


अचिन्हित vvp_wide_fun_core::port_गिनती() स्थिर
{
      वापस nports_;
}

vvp_vector4_t& vvp_wide_fun_core::value(अचिन्हित idx)
{
      निश्चित(idx < nports_);
      अगर (port_मूल्यावली == 0)
	    port_मूल्यावली = नया vvp_vector4_t [nports_];
      वापस port_मूल्यावली[idx];
}

द्विगुन vvp_wide_fun_core::मूल्य_r(अचिन्हित idx)
{
      निश्चित(idx < nports_);
      वापस port_rvalues_? port_rvalues_[idx] : 0.0;
}

व्योम vvp_wide_fun_core::recv_real_from_inputs(अचिन्हित)
{
      निश्चित(0);
}

व्योम vvp_wide_fun_core::dispatch_vec4_from_निवेश(अचिन्हित port,
						  स्थिर vvp_vector4_t&bit)
{
      निश्चित(port < nports_);
      अगर (port_मूल्यावली == 0) port_मूल्यावली = नया vvp_vector4_t [nports_];
      port_मूल्यावली[port] = bit;
      recv_vec4_from_inputs(port);
}

व्योम vvp_wide_fun_core::dispatch_real_from_निवेश(अचिन्हित port,
						  द्विगुन bit)
{
      निश्चित(port < nports_);
      अगर (port_rvalues_ == 0) port_rvalues_ = नया द्विगुन[nports_];
      port_rvalues_[port] = bit;
      recv_real_from_inputs(port);
}

vvp_wide_fun_t::vvp_wide_fun_t(vvp_wide_fun_core*c, अचिन्हित base)
: core_(c), port_base_(base)
{
}

vvp_wide_fun_t::~vvp_wide_fun_t()
{
}

व्योम vvp_wide_fun_t::recv_vec4(vvp_net_ptr_t port, स्थिर vvp_vector4_t&bit,
                               vvp_context_t)
{
      अचिन्हित pidx = port_base_ + port.port();
      core_->dispatch_vec4_from_निवेश(pidx, bit);
}

व्योम vvp_wide_fun_t::recv_vec4_pv(vvp_net_ptr_t ptr, स्थिर vvp_vector4_t&bit,
				  अचिन्हित base, अचिन्हित vwid, vvp_context_t ctx)
{
      recv_vec4_pv_(ptr, bit, base, vwid, ctx);
}

व्योम vvp_wide_fun_t::recv_real(vvp_net_ptr_t port, द्विगुन bit,
                               vvp_context_t)
{
      अचिन्हित pidx = port_base_ + port.port();
      core_->dispatch_real_from_निवेश(pidx, bit);
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

अचिन्हित vvp_scalar_t::strength0() स्थिर
{
      वापस STREN0(मूल्य_2);
}

अचिन्हित vvp_scalar_t::strength1() स्थिर
{
      वापस STREN1(मूल्य_2);
}

ब_प्रवाह& चालक <<(ब_प्रवाह&out, vvp_scalar_t a)
{
      out << a.strength0() << a.strength1();
      चयन (a.value()) {
	  हाल BIT4_0:
	    out << "0";
	    अवरोध;
	  हाल BIT4_1:
	    out << "1";
	    अवरोध;
	  हाल BIT4_X:
	    out << "X";
	    अवरोध;
	  हाल BIT4_Z:
	    out << "Z";
	    अवरोध;
      }
      वापस out;
}

/*
 * This function is only called if the actual interface function rules
 * out some of the easy cases. If we get here, we can assume that
 * neither of the values is HiZ, and the values are not exactly equal.
 */
vvp_scalar_t fully_featured_resolv_(vvp_scalar_t a, vvp_scalar_t b)
{

      अगर (UNAMBIG(a.मूल्य_2) && UNAMBIG(b.मूल्य_2)) {

	      /* If both signals are unambiguous, simply choose
		 the stronger. If they have the same strength
		 but different values, then this becomes
		 ambiguous. */

	    अगर ((b.मूल्य_2&0x07) > (a.मूल्य_2&0x07)) {

		    /* b value is stronger. Take it. */
		  वापस b;

	    } अन्यथा अगर ((b.मूल्य_2&0x77) == (a.मूल्य_2&0x77)) {

		    // Strengths are the same. Since we know already
		    // that the values are not the same, Make value
		    // into "x".
		  vvp_scalar_t tmp (a);
		  tmp.मूल्य_2 = (tmp.मूल्य_2&0x77) | 0x80;
		  वापस tmp;

	    } अन्यथा {

		    /* Must be "a" is the stronger one. */
		  वापस a;
	    }

      }

	/* If one of the signals is unambiguous, then it
	   will sweep up the weaker parts of the ambiguous
	   signal. The result may be ambiguous, or maybe not. */

      अगर (UNAMBIG(a.मूल्य_2)) {
	    vvp_scalar_t res;

	    अगर ((a.मूल्य_2&0x70) > (b.मूल्य_2&0x70))
		  res.मूल्य_2 |= a.मूल्य_2&0xf0;
	    अन्यथा
		  res.मूल्य_2 |= b.मूल्य_2&0xf0;

	    अगर ((a.मूल्य_2&0x07) > (b.मूल्य_2&0x07))
		  res.मूल्य_2 |= a.मूल्य_2&0x0f;
	    अन्यथा
		  res.मूल्य_2 |= b.मूल्य_2&0x0f;

	    वापस res;

      } अन्यथा अगर (UNAMBIG(b.मूल्य_2)) {

	    vvp_scalar_t res;

	    अगर ((b.मूल्य_2&0x70) > (a.मूल्य_2&0x70))
		  res.मूल्य_2 |= b.मूल्य_2&0xf0;
	    अन्यथा
		  res.मूल्य_2 |= a.मूल्य_2&0xf0;

	    अगर ((b.मूल्य_2&0x07) > (a.मूल्य_2&0x07))
		  res.मूल्य_2 |= b.मूल्य_2&0x0f;
	    अन्यथा
		  res.मूल्य_2 |= a.मूल्य_2&0x0f;

	    वापस res;

      }


	/* If both signals are ambiguous, then the result
	   has an even wider ambiguity. */

      अचिन्हित tmp = 0;
      पूर्णांक sv1a = (a.मूल्य_2 & 0x80) ? STREN1(a.मूल्य_2) : - STREN1(a.मूल्य_2);
      पूर्णांक sv0a = (a.मूल्य_2 & 0x08) ? STREN0(a.मूल्य_2) : - STREN0(a.मूल्य_2);
      पूर्णांक sv1b = (b.मूल्य_2 & 0x80) ? STREN1(b.मूल्य_2) : - STREN1(b.मूल्य_2);
      पूर्णांक sv0b = (b.मूल्य_2 & 0x08) ? STREN0(b.मूल्य_2) : - STREN0(b.मूल्य_2);

      पूर्णांक sv1 = sv1a;
      पूर्णांक sv0 = sv0a;

      अगर (sv0a > sv1)
	    sv1 = sv0a;
      अगर (sv1b > sv1)
	    sv1 = sv1b;
      अगर (sv0b > sv1)
	    sv1 = sv0b;

      अगर (sv1a < sv0)
	    sv0 = sv1a;
      अगर (sv1b < sv0)
	    sv0 = sv1b;
      अगर (sv0b < sv0)
	    sv0 = sv0b;

      अगर (sv1 > 0) {
	    tmp |= 0x80;
	    tmp |= sv1 << 4;
      } अन्यथा {
	      /* Set the MSB when both arguments MSBs are set. This
		 can only happen if both one strengths are zero. */
	    tmp |= (a.मूल्य_2&b.मूल्य_2)&0x80;
	    tmp |= (-sv1) << 4;
      }

      अगर (sv0 > 0) {
	    tmp |= 0x08;
	    tmp |= sv0;
      } अन्यथा {
	    tmp |= (-sv0);
      }

      vvp_scalar_t res;
      res.मूल्य_2 = tmp;

	/* Canonicalize the HiZ value. */
      अगर ((res.मूल्य_2&0x77) == 0)
	    res.मूल्य_2 = 0;

      वापस res;
}

अचिन्हित vvp_switch_strength_map[2][8] = {
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

vvp_vector4_t reduce4(स्थिर vvp_vector8_t&that)
{
      vvp_vector4_t out (that.size());
      क्रम (अचिन्हित idx = 0 ;  idx < out.size() ;  idx += 1)
	    out.set_bit(idx, that.value(idx).value());

      वापस out;
}

vvp_bit4_t compare_gtge(स्थिर vvp_vector4_t&lef, स्थिर vvp_vector4_t&rig,
			vvp_bit4_t out_if_equal)
{
      अचिन्हित min_आकार = lef.size();
      अगर (rig.size() < min_आकार)
	    min_आकार = rig.size();

	// If one of the inputs is nil, treat is as all X values, and
	// that makes the result BIT4_X.
      अगर (min_आकार == 0)
	    वापस BIT4_X;

	// As per the IEEE1364 definition of >, >=, < and <=, if there
	// are any X or Z values in either of the operand vectors,
	// then the result of the compare is BIT4_X.

	// Check for X/Z in the left operand
      अगर (lef.has_xz())
	    वापस BIT4_X;

	// Check for X/Z in the right operand
      अगर (rig.has_xz())
	    वापस BIT4_X;

      क्रम (अचिन्हित idx = lef.size() ; idx > rig.size() ;  idx -= 1) {
	    अगर (lef.value(idx-1) == BIT4_1)
		  वापस BIT4_1;
      }

      क्रम (अचिन्हित idx = rig.size() ; idx > lef.size() ;  idx -= 1) {
	    अगर (rig.value(idx-1) == BIT4_1)
		  वापस BIT4_0;
      }

      क्रम (अचिन्हित idx = min_आकार ; idx > 0 ;  idx -= 1) {
	    vvp_bit4_t lv = lef.value(idx-1);
	    vvp_bit4_t rv = rig.value(idx-1);

	    अगर (lv == rv)
		  जारी;

	    अगर (lv == BIT4_1)
		  वापस BIT4_1;
	    अन्यथा
		  वापस BIT4_0;
      }

      वापस out_if_equal;
}

vvp_bit4_t compare_gtge_signed(स्थिर vvp_vector4_t&a,
			       स्थिर vvp_vector4_t&b,
			       vvp_bit4_t out_if_equal)
{
      निश्चित(a.size() == b.size());

      अचिन्हित sign_idx = a.size()-1;
      vvp_bit4_t a_sign = a.value(sign_idx);
      vvp_bit4_t b_sign = b.value(sign_idx);

      अगर (bit4_is_xz(a_sign))
	    वापस BIT4_X;
      अगर (bit4_is_xz(b_sign))
	    वापस BIT4_X;

      अगर (a_sign == b_sign)
	    वापस compare_gtge(a, b, out_if_equal);

      अगर (a.has_xz())
	    वापस BIT4_X;

      अगर (b.has_xz())
	    वापस BIT4_X;

      अगर(a_sign == BIT4_0)
	    वापस BIT4_1;
      अन्यथा
	    वापस BIT4_0;
}
