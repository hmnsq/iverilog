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
#यदि_परिभाषितम् CHECK_WITH_VALGRIND
# include  <valgrind/memcheck.h>
# include  <map>
# include  "sfunc.h"
# include  "udp.h"
# include  "ivl_alloc.h"
#यदि_अन्तः

उपयुज्य नामाकाश std;

/* This is the size of an unsigned long in bits. This is just a
   convenience macro. */
# define CPU_WORD_द्वयङ्काः (8*परिमाण(अचिह्नित दीर्घ))
# define शिखरम्_BIT (1UL << (CPU_WORD_द्वयङ्काः-1))

permaheap vvp_net_fun_t::heap_;
permaheap vvp_net_fil_t::heap_;

// Allocate around 1Megabyte/chunk.
स्थिर ध्रुव size_t VVP_NET_CHUNK = 1024*1024/परिमाण(vvp_net_t);
स्थिर vvp_net_t*vvp_net_alloc_सारणी = रिक्त;
#यदि_परिभाषितम् CHECK_WITH_VALGRIND
स्थिर vvp_net_t **vvp_net_pool = रिक्त;
स्थिर अचिह्नित vvp_net_pool_गणनम् = 0;
#यदि_अन्तः
स्थिर size_t vvp_net_alloc_remaining = 0;
// For statistics, count the vvp_nets allocated and the bytes of alloc
// chunks allocated.
अचिह्नित दीर्घ गणनम्_vvp_nets = 0;
size_t आकार_vvp_nets = 0;

शून्य* vvp_net_t::संक्रियकः नूतन (size_t size)
{
      प्रतिज्ञा(size == परिमाण(vvp_net_t));
      यदि (vvp_net_alloc_remaining == 0) {
	    vvp_net_alloc_सारणी = ::नूतन vvp_net_t[VVP_NET_CHUNK];
	    vvp_net_alloc_remaining = VVP_NET_CHUNK;
	    आकार_vvp_nets += size*VVP_NET_CHUNK;
#यदि_परिभाषितम् CHECK_WITH_VALGRIND
	    VALGRIND_MAKE_MEM_NOACCESS(vvp_net_alloc_सारणी, size*VVP_NET_CHUNK);
	    VALGRIND_CREATE_MEMPOOL(vvp_net_alloc_सारणी, 0, 0);
	    vvp_net_pool_गणनम् += 1;
	    vvp_net_pool = static_cast<vvp_net_t **>(realloc(vvp_net_pool,
	                   vvp_net_pool_गणनम्*परिमाण(vvp_net_t **)));
	    vvp_net_pool[vvp_net_pool_गणनम्-1] = vvp_net_alloc_सारणी;
#यदि_अन्तः
      }

      vvp_net_t*return_this = vvp_net_alloc_सारणी;
#यदि_परिभाषितम् CHECK_WITH_VALGRIND
      VALGRIND_MEMPOOL_ALLOC(vvp_net_pool[vvp_net_pool_गणनम्-1],
                             return_this, size);
      return_this->pool = vvp_net_pool[vvp_net_pool_गणनम्-1];
#यदि_अन्तः
      vvp_net_alloc_सारणी += 1;
      vvp_net_alloc_remaining -= 1;
      गणनम्_vvp_nets += 1;
      प्रत्यागम return_this;
}

#यदि_परिभाषितम् CHECK_WITH_VALGRIND
स्थिर map<vvp_net_t*, तार्किक> vvp_net_map;
स्थिर map<sfunc_core*, तार्किक> sfunc_map;
स्थिर map<vvp_udp_fun_core*, तार्किक> udp_map;
स्थिर map<resolv_core*, तार्किक> resolv_map;
स्थिर vvp_net_t **local_net_pool = 0;
स्थिर अचिह्नित local_net_pool_गणनम् = 0;

शून्य pool_local_net(vvp_net_t*net)
{
      local_net_pool_गणनम् += 1;
      local_net_pool = static_cast<vvp_net_t **>(realloc(local_net_pool,
                       local_net_pool_गणनम्*परिमाण(vvp_net_t **)));
      local_net_pool[local_net_pool_गणनम्-1] = net;
}

शून्य vvp_net_delete(vvp_net_t *item)
{
      vvp_net_map[item] = सत्य;
      यदि (sfunc_core*tmp = dynamic_cast<sfunc_core*> (item->fun)) {
	    sfunc_map[tmp] = सत्य;
      }
      यदि (vvp_udp_fun_core*tmp = dynamic_cast<vvp_udp_fun_core*> (item->fun)) {
	    udp_map[tmp] = सत्य;
      }
      यदि (resolv_core*tmp = dynamic_cast<resolv_core*> (item->fun)) {
	    resolv_map[tmp] = सत्य;
      }
	/* Only delete static object variables. */
      यदि (vvp_fun_signal_object_sa*tmp = dynamic_cast<vvp_fun_signal_object_sa*> (item->fun)) {
	    विलोपय tmp;
      }
	/* Only delete static string variables. */
      यदि (vvp_fun_signal_string_sa*tmp = dynamic_cast<vvp_fun_signal_string_sa*> (item->fun)) {
	    विलोपय tmp;
      }
}

शून्य vvp_net_pool_delete()
{
      अचिह्नित दीर्घ vvp_nets_del = 0;

      प्रति (अचिह्नित idx = 0; idx < local_net_pool_गणनम्; idx += 1) {
	    vvp_net_delete(local_net_pool[idx]);
      }
      मुक्त(local_net_pool);
      local_net_pool = 0;
      local_net_pool_गणनम् = 0;

      map<vvp_net_t*, तार्किक>::iterator iter;
      प्रति (iter = vvp_net_map.begin(); iter != vvp_net_map.अन्तः(); ++ iter ) {
	    vvp_nets_del += 1;
	    VALGRIND_MEMPOOL_FREE(iter->first->pool, iter->first);
      }
      vvp_net_map.clear();

      map<sfunc_core*, तार्किक>::iterator siter;
      प्रति (siter = sfunc_map.begin(); siter != sfunc_map.अन्तः(); ++ siter ) {
	    विलोपय siter->first;
      }
      sfunc_map.clear();

      map<vvp_udp_fun_core*, तार्किक>::iterator uiter;
      प्रति (uiter = udp_map.begin(); uiter != udp_map.अन्तः(); ++ uiter ) {
	    विलोपय uiter->first;
      }
      udp_map.clear();

      map<resolv_core*, तार्किक>::iterator riter;
      प्रति (riter = resolv_map.begin(); riter != resolv_map.अन्तः(); ++ riter ) {
	    विलोपय riter->first;
      }
      resolv_map.clear();

      यदि (RUNNING_ON_VALGRIND && (vvp_nets_del != गणनम्_vvp_nets)) {
	    fflush(रिक्त);
	    VALGRIND_PRINTF("Error: vvp missed deleting %ld of %lu net(s).",
	                    (दीर्घ) गणनम्_vvp_nets - vvp_nets_del,
	                    गणनम्_vvp_nets);
      }

      प्रति (अचिह्नित idx = 0; idx < vvp_net_pool_गणनम्; idx += 1) {
	    VALGRIND_DESTROY_MEMPOOL(vvp_net_pool[idx]);
	    ::विलोपय [] vvp_net_pool[idx];
      }
      मुक्त(vvp_net_pool);
      vvp_net_pool = रिक्त;
      vvp_net_pool_गणनम् = 0;
}
#यदि_अन्तः

शून्य vvp_net_t::संक्रियकः विलोपय(शून्य*)
{
      प्रतिज्ञा(0);
}

vvp_net_t::vvp_net_t()
: बहिर्गमनम्_2(vvp_net_ptr_t(0,0))
{
      fun = 0;
      fil = 0;
}

शून्य vvp_net_t::link(vvp_net_ptr_t पत्तनम्_गन्तव्यम्_link)
{
      vvp_net_t*net = पत्तनम्_गन्तव्यम्_link.ptr();

	// Connect nodes with vvp_fun_modpath_src to the head of the
	// linked list, so that vvp_fun_modpath_src are always evaluated
	// before their respective vvp_fun_modpath
      यदि (dynamic_cast<vvp_fun_modpath_src*>(net->fun)) {
	    net->पत्तनम्[पत्तनम्_गन्तव्यम्_link.पत्तनम्()] = बहिर्गमनम्_2;
	    बहिर्गमनम्_2 = पत्तनम्_गन्तव्यम्_link;
	// All other nodes connect after the last vvp_fun_modpath_src
      } अन्यथा {
	      // List is empty or first node has no vvp_fun_modpath_src
	      // then just insert the node into the head of linked list
	    यदि (बहिर्गमनम्_2.ptr() == शून्यसूचकः || ! dynamic_cast<vvp_fun_modpath_src*>(बहिर्गमनम्_2.ptr()->fun)) {
		  net->पत्तनम्[पत्तनम्_गन्तव्यम्_link.पत्तनम्()] = बहिर्गमनम्_2;
		  बहिर्गमनम्_2 = पत्तनम्_गन्तव्यम्_link;
		  प्रत्यागम;
	    }

	    vvp_net_ptr_t cur = बहिर्गमनम्_2;
	    vvp_net_ptr_t prev = vvp_net_ptr_t(शून्यसूचकः,0);

	      // Traverse through linked list
	    यावत् (dynamic_cast<vvp_fun_modpath_src*>(cur.ptr()->fun)) {
		  prev = cur;
		  cur = cur.ptr()->पत्तनम्[cur.पत्तनम्()];
		  यदि (! cur.ptr()) भङ्ग; // End of list
	    }

	    प्रतिज्ञा(prev.ptr());

	      // Insert after last vvp_fun_modpath_src (or end of list)
	    net->पत्तनम्[पत्तनम्_गन्तव्यम्_link.पत्तनम्()] = cur;
	    prev.ptr()->पत्तनम्[prev.पत्तनम्()] = पत्तनम्_गन्तव्यम्_link;
      }
}

/*
 * Unlink a ptr object from the driver. The input is the driver in the
 * form of a vvp_net_t pointer. The .out member of that object is the
 * driver. The dst_ptr argument is the receiver pin to be located and
 * removed from the fan-out list.
 */
शून्य vvp_net_t::unlink(vvp_net_ptr_t dst_ptr)
{
      vvp_net_t*net = dst_ptr.ptr();
      अचिह्नित net_पत्तनम् = dst_ptr.पत्तनम्();

      यदि (बहिर्गमनम्_2 == dst_ptr) {
	      /* If the drive fan-out list starts with this pointer,
		 then the unlink is easy. Pull the list forward. */
	    बहिर्गमनम्_2 = net->पत्तनम्[net_पत्तनम्];
      } अन्यथा यदि (! बहिर्गमनम्_2.nil()) {
	      /* Scan the linked list, looking for the net_ptr_t
		 pointer *before* the one we wish to remove. */
	    vvp_net_ptr_t cur = बहिर्गमनम्_2;
	    प्रतिज्ञा(!cur.nil());
	    vvp_net_t*cur_net = cur.ptr();
	    अचिह्नित cur_पत्तनम् = cur.पत्तनम्();
	    यावत् (cur_net && cur_net->पत्तनम्[cur_पत्तनम्] != dst_ptr) {
		  cur = cur_net->पत्तनम्[cur_पत्तनम्];
		  cur_net = cur.ptr();
		  cur_पत्तनम् = cur.पत्तनम्();
	    }
	      /* Unlink. */
	    यदि (cur_net) cur_net->पत्तनम्[cur_पत्तनम्] = net->पत्तनम्[net_पत्तनम्];
      }

      net->पत्तनम्[net_पत्तनम्] = vvp_net_ptr_t(0,0);
}

शून्य vvp_net_t::गणनम्_drivers(अचिह्नित idx, अचिह्नित counts[4])
{
      counts[0] = 0;
      counts[1] = 0;
      counts[2] = 0;
      counts[3] = 0;

        /* $countdrivers can only be used on wires. */
      vvp_wire_आधारः*wire=dynamic_cast<vvp_wire_आधारः*>(fil);
      प्रतिज्ञा(wire);

      यदि (wire->is_forced(idx))
            counts[3] = 1;

        /* If the net has multiple drivers, we need to interrogate the
           resolver network to get the driven values. */
      यदि (resolv_core*resolver = dynamic_cast<resolv_core*>(fun)) {
            resolver->गणनम्_drivers(idx, counts);
            प्रत्यागम;
      }
      यदि (vvp_island_पत्तनम्*resolver = dynamic_cast<vvp_island_पत्तनम्*>(fun)) {
            resolver->गणनम्_drivers(idx, counts);
            प्रत्यागम;
      }

        /* If the functor is not a resolver, there is only one driver, so
           we can just interrogate the filter to get the driven value. */
      update_driver_counts(wire->driven_मानम्(idx), counts);
}

शून्य vvp_net_fun_t::संक्रियकः विलोपय(शून्य*)
{
#यदि_अपरिभाषितम् CHECK_WITH_VALGRIND
      प्रतिज्ञा(0);
#यदि_अन्तः
}


vvp_net_fil_t::vvp_net_fil_t()
{
      force_link_ = 0;
      force_propagate_ = असत्य;
      गणनम्_filters += 1;
}

vvp_net_fil_t::~vvp_net_fil_t()
{
      प्रतिज्ञा(force_link_ == 0);
}

vvp_net_fil_t::prop_t vvp_net_fil_t::filter_vec4(ध्रुव vvp_vector4_t&,
                                                 vvp_vector4_t&,
                                                 अचिह्नित, अचिह्नित)
{
      प्रत्यागम PROP;
}

vvp_net_fil_t::prop_t vvp_net_fil_t::filter_vec8(ध्रुव vvp_vector8_t&,
                                                 vvp_vector8_t&,
                                                 अचिह्नित, अचिह्नित)
{
      प्रत्यागम PROP;
}

vvp_net_fil_t::prop_t vvp_net_fil_t::filter_real(द्विगुण&)
{
      प्रत्यागम PROP;
}

vvp_net_fil_t::prop_t vvp_net_fil_t::filter_string(ध्रुव string&)
{
      प्रत्यागम PROP;
}

vvp_net_fil_t::prop_t vvp_net_fil_t::filter_object(vvp_object_t&)
{
      प्रत्यागम PROP;
}

शून्य vvp_net_fil_t::force_आवरणम्(ध्रुव vvp_vector2_t&आवरणम्)
{
      यदि (force_आवरणम्_2.size() == 0)
	    force_आवरणम्_2 = vvp_vector2_t(vvp_vector2_t::FILL0, आवरणम्.size());

      प्रतिज्ञा(force_आवरणम्_2.size() == आवरणम्.size());
      प्रति (अचिह्नित idx = 0 ; idx < आवरणम्.size() ; idx += 1) {
	    यदि (आवरणम्.value(idx) == 0)
		  अनुवर्त;

	    force_आवरणम्_2.समुच्चयः_bit(idx, 1);
	    force_propagate_ = सत्य;
      }
}

शून्य vvp_net_fil_t::release_आवरणम्(ध्रुव vvp_vector2_t&आवरणम्)
{
      यदि (force_आवरणम्_2.size() == 0)
	    प्रत्यागम;

      प्रतिज्ञा(force_आवरणम्_2.size() == आवरणम्.size());
      प्रति (अचिह्नित idx = 0 ; idx < आवरणम्.size() ; idx += 1) {
	    यदि (आवरणम्.value(idx))
		  force_आवरणम्_2.समुच्चयः_bit(idx, 0);
      }

      यदि (force_आवरणम्_2.is_zero())
	    force_आवरणम्_2 = vvp_vector2_t();
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
शून्य vvp_net_fil_t::force_link(vvp_net_t*dst, vvp_net_t*src)
{
      प्रतिज्ञा(dst->fil == एतत्);

      यदि (force_link_ == 0) {
	    force_link_ = नूतन vvp_net_t;
	      // Use port[3] to hold the force destination.
	    force_link_->पत्तनम्[3] = vvp_net_ptr_t(dst, 0);
	    force_link_->पत्तनम्[2] = vvp_net_ptr_t(0,0);
	    force_link_->fun = नूतन vvp_fun_force;
      }

      force_unlink();
      प्रतिज्ञा(force_link_->पत्तनम्[2] == vvp_net_ptr_t(0,0));

	// Use port[2] to hold the force source.
      force_link_->पत्तनम्[2] = vvp_net_ptr_t(src,0);

      vvp_net_ptr_t dst_ptr(force_link_, 0);
      src->link(dst_ptr);
}

शून्य vvp_net_fil_t::संक्रियकः विलोपय(शून्य*)
{
      प्रतिज्ञा(0);
}

शून्य vvp_net_fil_t::force_unlink(शून्य)
{
      यदि (force_link_ == 0) प्रत्यागम;
      vvp_net_t*src = force_link_->पत्तनम्[2].ptr();
      यदि (src == 0) प्रत्यागम;

      src->unlink(vvp_net_ptr_t(force_link_,0));
      force_link_->पत्तनम्[2] = vvp_net_ptr_t(0,0);
}

/* *** BIT operations *** */
vvp_bit4_t सङ्कलय_with_carry(vvp_bit4_t a, vvp_bit4_t b, vvp_bit4_t&c)
{
      यदि (bit4_is_xz(a) || bit4_is_xz(b) || bit4_is_xz(c)) {
	    c = BIT4_X;
	    प्रत्यागम BIT4_X;
      }

	// NOTE: This relies on the facts that XZ values have been
	// weeded out, and that BIT4_1 is 1 and BIT4_0 is 0.
      पूर्णाङ्क योग = (पूर्णाङ्क)a + (पूर्णाङ्क)b + (पूर्णाङ्क)c;

      विकल्प (योग) {
	  पक्ष 0:
	      // c must already be 0.
	    प्रत्यागम BIT4_0;
	  पक्ष 1:
	    c = BIT4_0;
	    प्रत्यागम BIT4_1;
	  पक्ष 2:
	    c = BIT4_1;
	    प्रत्यागम BIT4_0;
	  पक्ष 3:
	    c = BIT4_1;
	    प्रत्यागम BIT4_1;
      }
      fprintf(stderr, "Incorrect result %d.\n", योग);
      प्रतिज्ञा(0);
      प्रत्यागम BIT4_X;
}

vvp_bit4_t scalar_गन्तव्यम्_bit4(PLI_INT32 scalar)
{
      विकल्प(scalar) {
	  पक्ष vpi0:
	    प्रत्यागम BIT4_0;
	  पक्ष vpi1:
	    प्रत्यागम BIT4_1;
	  पक्ष vpiX:
	    प्रत्यागम BIT4_X;
	  पक्ष vpiZ:
	    प्रत्यागम BIT4_Z;
      }
      fprintf(stderr, "Unsupported scalar value %d.\n", (पूर्णाङ्क)scalar);
      प्रतिज्ञा(0);
      प्रत्यागम BIT4_X;
}

vvp_bit4_t संक्रियकः ^ (vvp_bit4_t a, vvp_bit4_t b)
{
      यदि (bit4_is_xz(a))
	    प्रत्यागम BIT4_X;
      यदि (bit4_is_xz(b))
	    प्रत्यागम BIT4_X;
      यदि (a == BIT4_0)
	    प्रत्यागम b;
      यदि (b == BIT4_0)
	    प्रत्यागम a;
      प्रत्यागम BIT4_0;
}

ostream& संक्रियकः<<(ostream&out, vvp_bit4_t bit)
{
      विकल्प (bit) {
	  पक्ष BIT4_0:
	    out << "0";
	    भङ्ग;
	  पक्ष BIT4_1:
	    out << "1";
	    भङ्ग;
	  पक्ष BIT4_X:
	    out << "X";
	    भङ्ग;
	  पक्ष BIT4_Z:
	    out << "Z";
	    भङ्ग;
	  स्वभाव:
	    out << "?";
	    भङ्ग;
      }
      प्रत्यागम out;
}

प्रकारनिर्देश अचिह्नित ह्रस्व edge_t;

अन्तःस्थ edge_t VVP_EDGE(vvp_bit4_t from, vvp_bit4_t to)
{
      प्रत्यागम 1 << ((from << 2) | to);
}

ध्रुव edge_t vvp_edge_posedge
      = VVP_EDGE(BIT4_0,BIT4_1)
      | VVP_EDGE(BIT4_0,BIT4_X)
      | VVP_EDGE(BIT4_0,BIT4_Z)
      | VVP_EDGE(BIT4_X,BIT4_1)
      | VVP_EDGE(BIT4_Z,BIT4_1)
      ;

ध्रुव edge_t vvp_edge_negedge
      = VVP_EDGE(BIT4_1,BIT4_0)
      | VVP_EDGE(BIT4_1,BIT4_X)
      | VVP_EDGE(BIT4_1,BIT4_Z)
      | VVP_EDGE(BIT4_X,BIT4_0)
      | VVP_EDGE(BIT4_Z,BIT4_0)
      ;

पूर्णाङ्क edge(vvp_bit4_t from, vvp_bit4_t to)
{
      edge_t आवरणम् = VVP_EDGE(from, to);
      यदि (आवरणम् & vvp_edge_posedge)
	    प्रत्यागम 1;
      यदि (आवरणम् & vvp_edge_negedge)
	    प्रत्यागम -1;
      प्रत्यागम 0;
}

अचिह्नित दीर्घ multiply_with_carry(अचिह्नित दीर्घ a, अचिह्नित दीर्घ b,
				  अचिह्नित दीर्घ&carry)
{
      ध्रुव अचिह्नित दीर्घ आवरणम् = (1UL << (CPU_WORD_द्वयङ्काः/2)) - 1;
      अचिह्नित दीर्घ a0 = a & आवरणम्;
      अचिह्नित दीर्घ a1 = (a >> (CPU_WORD_द्वयङ्काः/2)) & आवरणम्;
      अचिह्नित दीर्घ b0 = b & आवरणम्;
      अचिह्नित दीर्घ b1 = (b >> (CPU_WORD_द्वयङ्काः/2)) & आवरणम्;

      अचिह्नित दीर्घ tmp = a0 * b0;

      अचिह्नित दीर्घ r00 = tmp & आवरणम्;
      अचिह्नित दीर्घ c00 = (tmp >> (CPU_WORD_द्वयङ्काः/2)) & आवरणम्;

      tmp = a0 * b1;

      अचिह्नित दीर्घ r01 = tmp & आवरणम्;
      अचिह्नित दीर्घ c01 = (tmp >> (CPU_WORD_द्वयङ्काः/2)) & आवरणम्;

      tmp = a1 * b0;

      अचिह्नित दीर्घ r10 = tmp & आवरणम्;
      अचिह्नित दीर्घ c10 = (tmp >> (CPU_WORD_द्वयङ्काः/2)) & आवरणम्;

      tmp = a1 * b1;

      अचिह्नित दीर्घ r11 = tmp & आवरणम्;
      अचिह्नित दीर्घ c11 = (tmp >> (CPU_WORD_द्वयङ्काः/2)) & आवरणम्;

      अचिह्नित दीर्घ r1 = c00 + r01 + r10;
      अचिह्नित दीर्घ r2 = (r1 >> (CPU_WORD_द्वयङ्काः/2)) & आवरणम्;
      r1 &= आवरणम्;
      r2 += c01 + c10 + r11;
      अचिह्नित दीर्घ r3 = (r2 >> (CPU_WORD_द्वयङ्काः/2)) & आवरणम्;
      r2 &= आवरणम्;
      r3 += c11;
      r3 &= आवरणम्;

      carry = (r3 << (CPU_WORD_द्वयङ्काः/2)) + r2;
      प्रत्यागम (r1 << (CPU_WORD_द्वयङ्काः/2)) + r00;
}


शून्य vvp_send_vec8(vvp_net_ptr_t ptr, ध्रुव vvp_vector8_t&val)
{
      यावत् (vvp_net_t*cur = ptr.ptr()) {
	    vvp_net_ptr_t अग्र_val = cur->पत्तनम्[ptr.पत्तनम्()];

	    यदि (cur->fun)
		  cur->fun->recv_vec8(ptr, val);

	    ptr = अग्र_val;
      }
}

शून्य vvp_send_real(vvp_net_ptr_t ptr, द्विगुण val, vvp_context_t context)
{
      यावत् (vvp_net_t*cur = ptr.ptr()) {
	    vvp_net_ptr_t अग्र_val = cur->पत्तनम्[ptr.पत्तनम्()];

	    यदि (cur->fun)
		  cur->fun->recv_real(ptr, val, context);

	    ptr = अग्र_val;
      }
}

ध्रुव vvp_vector4_t vvp_vector4_t::nil;

शून्य vvp_vector4_t::copy_द्वयङ्काः(ध्रुव vvp_vector4_t&that)
{

      यदि (आकार_2 == that.आकार_2) {
	    यदि (आकार_2 > द्वयङ्काः_प्रत्येकम्_WORD) {
		  अचिह्नित शब्दाः = (आकार_2+द्वयङ्काः_प्रत्येकम्_WORD-1) / द्वयङ्काः_प्रत्येकम्_WORD;
		  प्रति (अचिह्नित idx = 0 ;  idx < शब्दाः ;  idx += 1)
			abits_ptr_[idx] = that.abits_ptr_[idx];
		  प्रति (अचिह्नित idx = 0 ;  idx < शब्दाः ;  idx += 1)
			bbits_ptr_[idx] = that.bbits_ptr_[idx];
	    } अन्यथा {
		  abits_val_ = that.abits_val_;
		  bbits_val_ = that.bbits_val_;
	    }
	    प्रत्यागम;
      }

	/* Now we know that the sizes of this and that are definitely
	   different. We can use that in code below. In any case, we
	   need to copy only the smaller of the sizes. */

	/* If source and destination are both short, then mask/copy
	   the bit values. */
      यदि (आकार_2 <= द्वयङ्काः_प्रत्येकम्_WORD && that.आकार_2 <= द्वयङ्काः_प्रत्येकम्_WORD) {
	    अचिह्नित द्वयङ्काः_गन्तव्यम्_copy = (that.आकार_2 < आकार_2) ? that.आकार_2 : आकार_2;
	    अचिह्नित दीर्घ आवरणम् = ((द्वयङ्काः_गन्तव्यम्_copy < द्वयङ्काः_प्रत्येकम्_WORD) ? (1UL << द्वयङ्काः_गन्तव्यम्_copy) :
	                                                           (0UL)) - 1UL;
	    abits_val_ &= ~आवरणम्;
	    bbits_val_ &= ~आवरणम्;
	    abits_val_ |= that.abits_val_&आवरणम्;
	    bbits_val_ |= that.bbits_val_&आवरणम्;
	    प्रत्यागम;
      }

	/* Now we know that either source or destination are long. If
	   the destination is short, then mask/copy from the low word
	   of the long source. */
      यदि (आकार_2 <= द्वयङ्काः_प्रत्येकम्_WORD) {
	    abits_val_ = that.abits_ptr_[0];
	    bbits_val_ = that.bbits_ptr_[0];
	    यदि (आकार_2 < द्वयङ्काः_प्रत्येकम्_WORD) {
		  अचिह्नित दीर्घ आवरणम् = (1UL << आकार_2) - 1UL;
		  abits_val_ &= आवरणम्;
		  bbits_val_ &= आवरणम्;
	    }
	    प्रत्यागम;
      }

	/* Now we know that the destination must be long. If the
	   source is short, then mask/copy from its value. */
      यदि (that.आकार_2 <= द्वयङ्काः_प्रत्येकम्_WORD) {
	    अचिह्नित दीर्घ आवरणम्;
	    यदि (that.आकार_2 < द्वयङ्काः_प्रत्येकम्_WORD) {
		  आवरणम् = (1UL << that.आकार_2) - 1UL;
		  abits_ptr_[0] &= ~आवरणम्;
		  bbits_ptr_[0] &= ~आवरणम्;
	    } अन्यथा {
		  आवरणम् = -1UL;
	    }
	    abits_ptr_[0] |= that.abits_val_&आवरणम्;
	    bbits_ptr_[0] |= that.bbits_val_&आवरणम्;
	    प्रत्यागम;
      }

	/* Finally, we know that source and destination are long. copy
	   words until we get to the last. */
      अचिह्नित द्वयङ्काः_गन्तव्यम्_copy = (that.आकार_2 < आकार_2) ? that.आकार_2 : आकार_2;
      अचिह्नित word = 0;
      यावत् (द्वयङ्काः_गन्तव्यम्_copy >= द्वयङ्काः_प्रत्येकम्_WORD) {
	    abits_ptr_[word] = that.abits_ptr_[word];
	    bbits_ptr_[word] = that.bbits_ptr_[word];
	    द्वयङ्काः_गन्तव्यम्_copy -= द्वयङ्काः_प्रत्येकम्_WORD;
	    word += 1;
      }
      यदि (द्वयङ्काः_गन्तव्यम्_copy > 0) {
	    अचिह्नित दीर्घ आवरणम् = (1UL << द्वयङ्काः_गन्तव्यम्_copy) - 1UL;
	    abits_ptr_[word] &= ~आवरणम्;
	    bbits_ptr_[word] &= ~आवरणम्;
	    abits_ptr_[word] |= that.abits_ptr_[word] & आवरणम्;
	    bbits_ptr_[word] |= that.bbits_ptr_[word] & आवरणम्;
      }
}

/*
 * This function should ONLY BE CALLED FROM vvp_vector4_t::copy_from_,
 * as it performs part of that functions tasks.
 */
शून्य vvp_vector4_t::copy_from_big_(ध्रुव vvp_vector4_t&that)
{
      अचिह्नित शब्दाः = (आकार_2+द्वयङ्काः_प्रत्येकम्_WORD-1) / द्वयङ्काः_प्रत्येकम्_WORD;
      abits_ptr_ = नूतन अचिह्नित दीर्घ[2*शब्दाः];
      bbits_ptr_ = abits_ptr_ + शब्दाः;

      प्रति (अचिह्नित idx = 0 ;  idx < शब्दाः ;  idx += 1)
	    abits_ptr_[idx] = that.abits_ptr_[idx];
      प्रति (अचिह्नित idx = 0 ;  idx < शब्दाः ;  idx += 1)
	    bbits_ptr_[idx] = that.bbits_ptr_[idx];
}

/*
 * The copy_inverted_from_ method is just like the copy_from_ method,
 * except that we combine that with an invert. This allows the ~ and
 * the assignment to be blended in many common cases.
 */
शून्य vvp_vector4_t::copy_inverted_from_(ध्रुव vvp_vector4_t&that)
{
      आकार_2 = that.आकार_2;
      यदि (आकार_2 > द्वयङ्काः_प्रत्येकम्_WORD) {
	    अचिह्नित शब्दाः = (आकार_2+द्वयङ्काः_प्रत्येकम्_WORD-1) / द्वयङ्काः_प्रत्येकम्_WORD;
	    abits_ptr_ = नूतन अचिह्नित दीर्घ[2*शब्दाः];
	    bbits_ptr_ = abits_ptr_ + शब्दाः;

	    अचिह्नित remaining = आकार_2;
	    अचिह्नित idx = 0;
	    यावत् (remaining >= द्वयङ्काः_प्रत्येकम्_WORD) {
		  abits_ptr_[idx] = that.bbits_ptr_[idx] | ~that.abits_ptr_[idx];
		  idx += 1;
		  remaining -= द्वयङ्काः_प्रत्येकम्_WORD;
	    }
	    यदि (remaining > 0) {
		  अचिह्नित दीर्घ आवरणम् = (1UL<<remaining) - 1UL;
		  abits_ptr_[idx] = आवरणम् & (that.bbits_ptr_[idx] | ~that.abits_ptr_[idx]);
	    }

	    प्रति (idx = 0 ;  idx < शब्दाः ;  idx += 1)
		  bbits_ptr_[idx] = that.bbits_ptr_[idx];

      } अन्यथा {
	    अचिह्नित दीर्घ आवरणम् = (आकार_2<द्वयङ्काः_प्रत्येकम्_WORD)? (1UL<<आकार_2)-1UL : -1UL;
	    abits_val_ = आवरणम् & (that.bbits_val_ | ~that.abits_val_);
	    bbits_val_ = that.bbits_val_;
      }
}

/* Make sure to set size_ before calling this routine. */
शून्य vvp_vector4_t::allocate_शब्दाः(अचिह्नित दीर्घ inita, अचिह्नित दीर्घ initb)
{
      यदि (आकार_2 > द्वयङ्काः_प्रत्येकम्_WORD) {
	    अचिह्नित cnt = (आकार_2 + द्वयङ्काः_प्रत्येकम्_WORD - 1) / द्वयङ्काः_प्रत्येकम्_WORD;
	    abits_ptr_ = नूतन अचिह्नित दीर्घ[2*cnt];
	    bbits_ptr_ = abits_ptr_ + cnt;
	    प्रति (अचिह्नित idx = 0 ;  idx < cnt ;  idx += 1)
		  abits_ptr_[idx] = inita;
	    प्रति (अचिह्नित idx = 0 ;  idx < cnt ;  idx += 1)
		  bbits_ptr_[idx] = initb;

      } अन्यथा {
	    abits_val_ = inita;
	    bbits_val_ = initb;
      }
}

vvp_vector4_t::vvp_vector4_t(अचिह्नित आकार_3, द्विगुण val)
: आकार_2(आकार_3)
{
      तार्किक is_neg = असत्य;
      द्विगुण fraction;
      पूर्णाङ्क exponent;

	/* We return 'bx for a NaN or +/- infinity. */
      यदि (val != val || (val && (val == 0.5*val)))  {
	    allocate_शब्दाः(WORD_X_ABITS, WORD_X_BBITS);
	    प्रत्यागम;
      }

	/* Convert to a positive result. */
      यदि (val < 0.0) {
	    is_neg = सत्य;
	    val = -val;
      }
      allocate_शब्दाः(WORD_0_ABITS, WORD_0_BBITS);

	/* Get the exponent and fractional part of the number. */
      fraction = frexp(val, &exponent);

	/* If the value is small enough just use lround(). */
      यदि (exponent < द्वयङ्काः_प्रत्येकम्_WORD-2) {
	    यदि (is_neg) एतत्->invert();  // Invert the bits if negative.
	    दीर्घ sval = lround(val);
	    यदि (is_neg) sval = -sval;
	      /* This requires that 0 and 1 have the same bbit value. */
	    यदि (आकार_2 > द्वयङ्काः_प्रत्येकम्_WORD) {
		  abits_ptr_[0] = sval;
	    } अन्यथा {
		  abits_val_ = sval;
	    }
	    प्रत्यागम;
      }

      अचिह्नित nwords = (exponent-1) / द्वयङ्काः_प्रत्येकम्_WORD;
      अचिह्नित my_शब्दाः = (आकार_2 + द्वयङ्काः_प्रत्येकम्_WORD - 1) / द्वयङ्काः_प्रत्येकम्_WORD - 1;

      fraction = ldexp(fraction, (exponent-1) % द्वयङ्काः_प्रत्येकम्_WORD + 1);

	/* Skip any leading bits. */
      प्रति (पूर्णाङ्क idx = (चिह्नित) nwords; idx > (चिह्नित) my_शब्दाः; idx -=1) {
	    अचिह्नित दीर्घ द्वयङ्काः = (अचिह्नित दीर्घ) fraction;
	    fraction = fraction - (द्विगुण) द्वयङ्काः;
	    fraction = ldexp(fraction, द्वयङ्काः_प्रत्येकम्_WORD);
      }

	/* Convert the remaining bits as appropriate. */
      यदि (my_शब्दाः == 0) {
		  अचिह्नित दीर्घ द्वयङ्काः = (अचिह्नित दीर्घ) fraction;
		  abits_val_ = द्वयङ्काः;
		  fraction = fraction - (द्विगुण) द्वयङ्काः;
		    /* Round any fractional part up. */
		  यदि (fraction >= 0.5) *एतत् += (int64_t) 1;
      } अन्यथा {
	    यदि (nwords < my_शब्दाः) my_शब्दाः = nwords;
	    प्रति (पूर्णाङ्क idx = (चिह्नित)my_शब्दाः; idx >= 0; idx -= 1) {
		  अचिह्नित दीर्घ द्वयङ्काः = (अचिह्नित दीर्घ) fraction;
		  abits_ptr_[idx] = द्वयङ्काः;
		  fraction = fraction - (द्विगुण) द्वयङ्काः;
		  fraction = ldexp(fraction, द्वयङ्काः_प्रत्येकम्_WORD);
	    }
	      /* Round any fractional part up. */
	    यदि (fraction >= ldexp(0.5, द्वयङ्काः_प्रत्येकम्_WORD)) *एतत् += (int64_t) 1;
       }

	/* Convert to a negative number if needed. */
      यदि (is_neg) {
	    एतत्->invert();
	    *एतत् += (int64_t) 1;
      }
}

vvp_vector4_t::vvp_vector4_t(ध्रुव vvp_vector4_t&that,
			    अचिह्नित adr, अचिह्नित wid)
{
	// Set up and initialize the destination.
      आकार_2 = wid;
      allocate_शब्दाः(WORD_X_ABITS, WORD_X_BBITS);

	// Special case: selecting from far beyond the source vector,
	// to the result is all X bits. We're done.
      यदि (adr >= that.आकार_2)
	    प्रत्यागम;

	// Special case: The source is not quite big enough to supply
	// all bits, so get the bits that we can. The remainder will
	// be left at BIT4_X.
      यदि ((adr + wid) > that.आकार_2) {
	    अचिह्नित use_wid = that.आकार_2 - adr;
	    प्रति (अचिह्नित idx = 0 ; idx < use_wid ; idx += 1)
		  समुच्चयः_bit(idx, that.value(adr+idx));

	    प्रत्यागम;
      }

	// At the point, we know that the source part is entirely
	// contained in the source vector.
	// assert((adr + wid) <= that.size_);

      यदि (wid > द्वयङ्काः_प्रत्येकम्_WORD) {
	      /* In this case, the subvector and the source vector are
		 long. Do the transfer reasonably efficiently. */
	    अचिह्नित ptr = adr / द्वयङ्काः_प्रत्येकम्_WORD;
	    अचिह्नित दीर्घ off = adr % द्वयङ्काः_प्रत्येकम्_WORD;
	    अचिह्नित दीर्घ noff = द्वयङ्काः_प्रत्येकम्_WORD - off;
	    अचिह्नित दीर्घ lmask = (1UL << off) - 1UL;
	    अचिह्नित trans = 0;
	    अचिह्नित dst = 0;
	    यावत् (trans < wid) {
		    // The low bits of the result.
		  abits_ptr_[dst] = (that.abits_ptr_[ptr] & ~lmask) >> off;
		  bbits_ptr_[dst] = (that.bbits_ptr_[ptr] & ~lmask) >> off;
		  trans += noff;

		  यदि (trans >= wid)
			भङ्ग;

		  ptr += 1;

		    // The high bits of the result. Skip this if the
		    // source and destination are perfectly aligned.
		  यदि (noff != द्वयङ्काः_प्रत्येकम्_WORD) {
			abits_ptr_[dst] |= (that.abits_ptr_[ptr]&lmask) << noff;
			bbits_ptr_[dst] |= (that.bbits_ptr_[ptr]&lmask) << noff;
			trans += off;
		  }

		  dst += 1;
	    }

      } अन्यथा यदि (that.आकार_2 > द्वयङ्काः_प्रत्येकम्_WORD) {
	      /* In this case, the subvector fits in a single word,
		 but the source is large. */
	    अचिह्नित ptr = adr / द्वयङ्काः_प्रत्येकम्_WORD;
	    अचिह्नित दीर्घ off = adr % द्वयङ्काः_प्रत्येकम्_WORD;
	    अचिह्नित trans = द्वयङ्काः_प्रत्येकम्_WORD - off;
	    यदि (trans > wid)
		  trans = wid;

	    यदि (trans == द्वयङ्काः_प्रत्येकम्_WORD) {
		    // Very special case: Copy exactly 1 perfectly
		    // aligned word.
		  abits_val_ = that.abits_ptr_[ptr];
		  bbits_val_ = that.bbits_ptr_[ptr];

	    } अन्यथा {
		    // lmask is the low bits of the destination,
		    // masked into the source.
		  अचिह्नित दीर्घ lmask = (1UL<<trans) - 1UL;
		  lmask <<= off;

		    // The low bits of the result.
		  abits_val_ = (that.abits_ptr_[ptr] & lmask) >> off;
		  bbits_val_ = (that.bbits_ptr_[ptr] & lmask) >> off;

		  यदि (trans < wid) {
			  // If there are more bits, then get them
			  // from the bottom of the next word of the
			  // source.
			अचिह्नित दीर्घ hmask = (1UL << (wid-trans)) - 1UL;

			  // The high bits of the result.
			abits_val_ |= (that.abits_ptr_[ptr+1]&hmask) << trans;
			bbits_val_ |= (that.bbits_ptr_[ptr+1]&hmask) << trans;
		  }
	    }

      } अन्यथा यदि (आकार_2 == द्वयङ्काः_प्रत्येकम्_WORD) {
	      /* We know that source and destination are short. If the
		 destination is a full word, then we know the copy is
		 aligned and complete. */
	    abits_val_ = that.abits_val_;
	    bbits_val_ = that.bbits_val_;

      } अन्यथा {
	      /* Finally, the source and destination vectors are both
		 short, so there is a single mask/shift/copy. */
	    अचिह्नित दीर्घ आवरणम् = (1UL << आकार_2) - 1UL;
	    आवरणम् <<= adr;

	    abits_val_ = (that.abits_val_ & आवरणम्) >> adr;
	    bbits_val_ = (that.bbits_val_ & आवरणम्) >> adr;
      }

}

/*
 * Change the size of the vvp_vector4_t vector to the new size. Copy
 * the old values, as many as well fit, into the new vector.
 */
शून्य vvp_vector4_t::resize(अचिह्नित newsize, vvp_bit4_t pad_bit)
{
      यदि (आकार_2 == newsize)
	    प्रत्यागम;

      अचिह्नित cnt = (आकार_2 + द्वयङ्काः_प्रत्येकम्_WORD - 1) / द्वयङ्काः_प्रत्येकम्_WORD;
      अचिह्नित दीर्घ word_pad_abits = 0;
      अचिह्नित दीर्घ word_pad_bbits = 0;
      विकल्प (pad_bit) {
	  पक्ष BIT4_0:
	    word_pad_abits = WORD_0_ABITS;
	    word_pad_bbits = WORD_0_BBITS;
	    भङ्ग;
	  पक्ष BIT4_1:
	    word_pad_abits = WORD_1_ABITS;
	    word_pad_bbits = WORD_1_BBITS;
	    भङ्ग;
	  पक्ष BIT4_X:
	    word_pad_abits = WORD_X_ABITS;
	    word_pad_bbits = WORD_X_BBITS;
	    भङ्ग;
	  पक्ष BIT4_Z:
	    word_pad_abits = WORD_Z_ABITS;
	    word_pad_bbits = WORD_Z_BBITS;
	    भङ्ग;
      }

      यदि (newsize > द्वयङ्काः_प्रत्येकम्_WORD) {
	    अचिह्नित newcnt = (newsize + द्वयङ्काः_प्रत्येकम्_WORD - 1) / द्वयङ्काः_प्रत्येकम्_WORD;
	    यदि (newcnt == cnt) {
		    // If the word count doesn't change, then there is
		    // no need for re-allocation so we are done now.
		  यदि (newsize > आकार_2) {
			यदि (अचिह्नित fill = आकार_2 % द्वयङ्काः_प्रत्येकम्_WORD) {
			      abits_ptr_[cnt-1] &= ~((-1UL) << fill);
			      bbits_ptr_[cnt-1] &= ~((-1UL) << fill);
			      abits_ptr_[cnt-1] |= word_pad_abits << fill;
			      bbits_ptr_[cnt-1] |= word_pad_bbits << fill;
			}
		  }
		  आकार_2 = newsize;
		  प्रत्यागम;
	    }

	    अचिह्नित दीर्घ*newbits = नूतन अचिह्नित दीर्घ[2*newcnt];

	    यदि (cnt > 1) {
		  अचिह्नित trans = cnt;
		  यदि (trans > newcnt)
			trans = newcnt;

		  प्रति (अचिह्नित idx = 0 ;  idx < trans ;  idx += 1)
			newbits[idx] = abits_ptr_[idx];
		  प्रति (अचिह्नित idx = 0 ;  idx < trans ;  idx += 1)
			newbits[newcnt+idx] = bbits_ptr_[idx];

		  विलोपय[]abits_ptr_;

	    } अन्यथा {
		  newbits[0] = abits_val_;
		  newbits[newcnt] = bbits_val_;
	    }

	    यदि (newsize > आकार_2) {
		  यदि (अचिह्नित fill = आकार_2 % द्वयङ्काः_प्रत्येकम्_WORD) {
			newbits[cnt-1] &= ~((-1UL) << fill);
			newbits[cnt-1] |= word_pad_abits << fill;
			newbits[newcnt+cnt-1] &= ~((-1UL) << fill);
			newbits[newcnt+cnt-1] |= word_pad_bbits << fill;
		  }
		  प्रति (अचिह्नित idx = cnt ;  idx < newcnt ;  idx += 1)
			newbits[idx] = word_pad_abits;
		  प्रति (अचिह्नित idx = cnt ;  idx < newcnt ;  idx += 1)
			newbits[newcnt+idx] = word_pad_bbits;
	    }

	    आकार_2 = newsize;
	    abits_ptr_ = newbits;
	    bbits_ptr_ = newbits + newcnt;

      } अन्यथा {
	    यदि (cnt > 1) {
		  अचिह्नित दीर्घ newvala = abits_ptr_[0];
		  अचिह्नित दीर्घ newvalb = bbits_ptr_[0];
		  विलोपय[]abits_ptr_;
		  abits_val_ = newvala;
		  bbits_val_ = newvalb;
	    }

	    यदि (newsize > आकार_2) {
		  abits_val_ &= ~((-1UL) << आकार_2);
		  bbits_val_ &= ~((-1UL) << आकार_2);
		  abits_val_ |= word_pad_abits << आकार_2;
		  bbits_val_ |= word_pad_bbits << आकार_2;
	    }

	    आकार_2 = newsize;
      }
}


अचिह्नित दीर्घ* vvp_vector4_t::subarray(अचिह्नित adr, अचिह्नित wid, तार्किक xz_गन्तव्यम्_0) ध्रुव
{
      ध्रुव अचिह्नित BIT2_प्रत्येकम्_WORD = 8*परिमाण(अचिह्नित दीर्घ);
      अचिह्नित awid = (wid + BIT2_प्रत्येकम्_WORD - 1) / (BIT2_प्रत्येकम्_WORD);
      अचिह्नित दीर्घ*val = नूतन अचिह्नित दीर्घ[awid];

      प्रति (अचिह्नित idx = 0 ;  idx < awid ;  idx += 1)
	    val[idx] = 0;

      यदि (आकार_2 <= द्वयङ्काः_प्रत्येकम्_WORD) {
	      /* Handle the special case that the array is small. The
		 entire value of the vector4 is within the xbits_val_
		 so we know that the result is a single word, the
		 source is a single word, and we just have to loop
		 through that word. */
	    अचिह्नित दीर्घ atmp = abits_val_ >> adr;
	    अचिह्नित दीर्घ btmp = bbits_val_ >> adr;
	    यदि (wid < BIT2_प्रत्येकम्_WORD) {
		  atmp &= (1UL << wid) - 1;
		  btmp &= (1UL << wid) - 1;
	    }
	    यदि (btmp) {
		  यदि (xz_गन्तव्यम्_0)
			atmp &= ~btmp;
		  अन्यथा
			गच्छ x_बहिर्गमनम्;
	    }

	    val[0] = atmp;

      } अन्यथा {

	    अचिह्नित val_ptr = 0;
	    अचिह्नित val_off = 0;

	      /* Get the first word we are scanning. We may in fact be
		 somewhere in the middle of that word. */
	    यावत् (wid > 0) {
		  अचिह्नित दीर्घ atmp = abits_ptr_[adr/द्वयङ्काः_प्रत्येकम्_WORD];
		  अचिह्नित दीर्घ btmp = bbits_ptr_[adr/द्वयङ्काः_प्रत्येकम्_WORD];
		  अचिह्नित दीर्घ off = adr%द्वयङ्काः_प्रत्येकम्_WORD;
		  atmp >>= off;
		  btmp >>= off;

		  अचिह्नित दीर्घ trans = द्वयङ्काः_प्रत्येकम्_WORD - off;
		  यदि (trans > (BIT2_प्रत्येकम्_WORD - val_off))
			trans = BIT2_प्रत्येकम्_WORD - val_off;
		  यदि (wid < trans)
			trans = wid;
		  यदि (trans < BIT2_प्रत्येकम्_WORD) {
			atmp &= (1UL << trans) - 1;
			btmp &= (1UL << trans) - 1;
		  }
		  यदि (btmp) {
			यदि (xz_गन्तव्यम्_0)
			      atmp &= ~btmp;
			अन्यथा
			      गच्छ x_बहिर्गमनम्;
		  }

		  val[val_ptr] |= atmp << val_off;
		  adr += trans;
		  wid -= trans;
		  val_off += trans;
		  यदि (val_off == BIT2_प्रत्येकम्_WORD) {
			val_ptr += 1;
			val_off = 0;
		  }
	    }
      }

      प्रत्यागम val;

 x_बहिर्गमनम्:
      विलोपय[]val;
      प्रत्यागम 0;
}

शून्य vvp_vector4_t::setarray(अचिह्नित adr, अचिह्नित wid, ध्रुव अचिह्नित दीर्घ*val)
{
      प्रतिज्ञा(adr+wid <= आकार_2);

      ध्रुव अचिह्नित BIT2_प्रत्येकम्_WORD = 8*परिमाण(अचिह्नित दीर्घ);

      यदि (आकार_2 <= द्वयङ्काः_प्रत्येकम्_WORD) {
	      // We know here that both the source and the target are
	      // within a single word. Write the bits into the
	      // abits_val_ directly.

	    प्रतिज्ञा(BIT2_प्रत्येकम्_WORD <= द्वयङ्काः_प्रत्येकम्_WORD);
	    अचिह्नित दीर्घ lmask = (1UL << adr) - 1UL;
	    अचिह्नित दीर्घ hmask = ((adr+wid) < द्वयङ्काः_प्रत्येकम्_WORD)
		  ? -1UL << (adr+wid)
		  : 0;
	    अचिह्नित दीर्घ आवरणम् = ~(hmask | lmask);

	    abits_val_ &= ~आवरणम्;
	    bbits_val_ &= ~आवरणम्;

	    abits_val_ |= आवरणम् & (val[0] << adr);

      } अन्यथा {
	      // The general case, there are multiple words of
	      // destination, and possibly multiple words of source
	      // data. Shift and mask as we go.
	    अचिह्नित off = adr % द्वयङ्काः_प्रत्येकम्_WORD;
	    अचिह्नित ptr = adr / द्वयङ्काः_प्रत्येकम्_WORD;
	    अचिह्नित val_off = 0;
	    अचिह्नित val_ptr = 0;
	    यावत् (wid > 0) {
		  अचिह्नित trans = wid;
		  यदि (trans > (BIT2_प्रत्येकम्_WORD-val_off))
			trans = BIT2_प्रत्येकम्_WORD-val_off;
		  यदि (trans > (द्वयङ्काः_प्रत्येकम्_WORD-off))
			trans = द्वयङ्काः_प्रत्येकम्_WORD-off;

		  अचिह्नित दीर्घ lmask = (1UL << off) - 1UL;
		  अचिह्नित दीर्घ hmask = ((off+trans) < द्वयङ्काः_प्रत्येकम्_WORD)
			? -1UL << (off+trans)
			: 0;
		  अचिह्नित दीर्घ आवरणम् = ~(hmask | lmask);

		  abits_ptr_[ptr] &= ~आवरणम्;
		  bbits_ptr_[ptr] &= ~आवरणम्;
		  यदि (val_off >= off)
			abits_ptr_[ptr] |= आवरणम् & (val[val_ptr] >> (val_off-off));
		  अन्यथा
			abits_ptr_[ptr] |= आवरणम् & (val[val_ptr] << (off-val_off));

		  wid -= trans;
		  val_off += trans;
		  यदि (val_off == BIT2_प्रत्येकम्_WORD) {
			val_off = 0;
			val_ptr += 1;
		  }
		  off += trans;
		  यदि (off == द्वयङ्काः_प्रत्येकम्_WORD) {
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
तार्किक vvp_vector4_t::समुच्चयः_vec(अचिह्नित adr, ध्रुव vvp_vector4_t&that)
{
      प्रतिज्ञा(adr+that.आकार_2  <= आकार_2);
      तार्किक diff_flag = असत्य;

      यदि (आकार_2 <= द्वयङ्काः_प्रत्येकम्_WORD) {

	      /* The destination vector (me!) is within a bits_val_
		 word, so the subvector is certainly within a
		 bits_val_ word. Therefore, the entire operation is a
		 matter of writing the bits of that into the addressed
		 bits of this. The mask below is calculated to be 1
		 for all the bits that are to come from that. Do the
		 job by some shifting, masking and OR. */

	    अचिह्नित दीर्घ lmask = (1UL << adr) - 1;
	    अचिह्नित दीर्घ hmask;
	    अचिह्नित दीर्घ hshift = adr+that.आकार_2;
	    यदि (hshift >= द्वयङ्काः_प्रत्येकम्_WORD)
		  hmask = -1UL;
	    अन्यथा
		  hmask = (1UL << (adr+that.आकार_2)) - 1;
	    अचिह्नित दीर्घ आवरणम् = hmask & ~lmask;

	    अचिह्नित दीर्घ tmp = (that.abits_val_<<adr)&आवरणम्;
	    यदि ((abits_val_&आवरणम्) != tmp) {
		  diff_flag = सत्य;
		  abits_val_ = (abits_val_ & ~आवरणम्) | tmp;
	    }
	    tmp = (that.bbits_val_<<adr) & आवरणम्;
	    यदि ((bbits_val_&आवरणम्) != tmp) {
		  diff_flag = सत्य;
		  bbits_val_ = (bbits_val_ & ~आवरणम्) | tmp;
	    }

      } अन्यथा यदि (that.आकार_2 <= द्वयङ्काः_प्रत्येकम्_WORD) {

	      /* This vector is more than a word, but that vector is
		 still small. Write into the destination, possibly
		 spanning two destination words, depending on whether
		 the source vector spans a word transition. */
	    अचिह्नित दीर्घ dptr = adr / द्वयङ्काः_प्रत्येकम्_WORD;
	    अचिह्नित दीर्घ doff = adr % द्वयङ्काः_प्रत्येकम्_WORD;

	    अचिह्नित दीर्घ lmask = (1UL << doff) - 1;
	    अचिह्नित दीर्घ hshift = doff+that.आकार_2;
	    अचिह्नित दीर्घ hmask;
	    यदि (hshift >= द्वयङ्काः_प्रत्येकम्_WORD)
		  hmask = -1UL;
	    अन्यथा
		  hmask = (1UL << hshift) - 1UL;

	    अचिह्नित दीर्घ आवरणम् = hmask & ~lmask;
	    अचिह्नित दीर्घ tmp;

	    tmp = (that.abits_val_ << doff) & आवरणम्;
	    यदि ((abits_ptr_[dptr] & आवरणम्) != tmp) {
		  diff_flag = सत्य;
		  abits_ptr_[dptr] = (abits_ptr_[dptr] & ~आवरणम्) | tmp;
	    }
	    tmp = (that.bbits_val_ << doff) & आवरणम्;
	    यदि ((bbits_ptr_[dptr] & आवरणम्) != tmp) {
		  diff_flag = सत्य;
		  bbits_ptr_[dptr] = (bbits_ptr_[dptr] & ~आवरणम्) | tmp;
	    }

	    यदि ((doff + that.आकार_2) > द्वयङ्काः_प्रत्येकम्_WORD) {
		  अचिह्नित पुच्छ = doff + that.आकार_2 - द्वयङ्काः_प्रत्येकम्_WORD;
		  आवरणम् = (1UL << पुच्छ) - 1;

		  dptr += 1;
		  tmp = (that.abits_val_ >> (that.आकार_2-पुच्छ)) & आवरणम्;
		  यदि ((abits_ptr_[dptr] & आवरणम्) != tmp) {
			diff_flag = सत्य;
			abits_ptr_[dptr] = (abits_ptr_[dptr] & ~आवरणम्) | tmp;
		  }
		  tmp = (that.bbits_val_ >> (that.आकार_2-पुच्छ)) & आवरणम्;
		  यदि ((bbits_ptr_[dptr] & आवरणम्) != tmp) {
			diff_flag = सत्य;
			bbits_ptr_[dptr] = (bbits_ptr_[dptr] & ~आवरणम्) | tmp;
		  }
	    }

      } अन्यथा यदि (adr%द्वयङ्काः_प्रत्येकम्_WORD == 0) {

	      /* In this case, both vectors are long, but the
		 destination is neatly aligned. That means all but the
		 last word can be simply copied with no masking. */

	    अचिह्नित remain = that.आकार_2;
	    अचिह्नित sptr = 0;
	    अचिह्नित dptr = adr / द्वयङ्काः_प्रत्येकम्_WORD;
	    यावत् (remain >= द्वयङ्काः_प्रत्येकम्_WORD) {
		  यदि (abits_ptr_[dptr] != that.abits_ptr_[sptr]) {
			diff_flag = सत्य;
			abits_ptr_[dptr] = that.abits_ptr_[sptr];
		  }
		  यदि (bbits_ptr_[dptr] != that.bbits_ptr_[sptr]) {
			diff_flag = सत्य;
			bbits_ptr_[dptr] = that.bbits_ptr_[sptr];
		  }
		  dptr += 1;
		  sptr += 1;
		  remain -= द्वयङ्काः_प्रत्येकम्_WORD;
	    }

	    यदि (remain > 0) {
		  अचिह्नित दीर्घ आवरणम् = (1UL << remain) - 1;
		  अचिह्नित दीर्घ tmp;

		  tmp = that.abits_ptr_[sptr] & आवरणम्;
		  यदि ((abits_ptr_[dptr] & आवरणम्) != tmp) {
			diff_flag = सत्य;
			abits_ptr_[dptr] = (abits_ptr_[dptr] & ~आवरणम्) | tmp;
		  }
		  tmp = that.bbits_ptr_[sptr] & आवरणम्;
		  यदि ((bbits_ptr_[dptr] & आवरणम्) != tmp) {
			diff_flag = सत्य;
			bbits_ptr_[dptr] = (bbits_ptr_[dptr] & ~आवरणम्) | tmp;
		  }
	    }

      } अन्यथा {

	      /* We know that there are two long vectors, and we know
		 that the destination is definitely NOT aligned. */

	    अचिह्नित remain = that.आकार_2;
	    अचिह्नित sptr = 0;
	    अचिह्नित dptr = adr / द्वयङ्काः_प्रत्येकम्_WORD;
	    अचिह्नित doff = adr % द्वयङ्काः_प्रत्येकम्_WORD;
	    अचिह्नित दीर्घ lmask = (1UL << doff) - 1;
	    अचिह्नित ndoff = द्वयङ्काः_प्रत्येकम्_WORD - doff;
	    यावत् (remain >= द्वयङ्काः_प्रत्येकम्_WORD) {
		  अचिह्नित दीर्घ tmp;

		  tmp = (that.abits_ptr_[sptr] << doff) & ~lmask;
		  यदि ((abits_ptr_[dptr] & ~lmask) != tmp) {
			diff_flag = सत्य;
			abits_ptr_[dptr] = (abits_ptr_[dptr] & lmask) | tmp;
		  }
		  tmp = (that.bbits_ptr_[sptr] << doff) & ~lmask;
		  यदि ((bbits_ptr_[dptr] & ~lmask) != tmp) {
			diff_flag = सत्य;
			bbits_ptr_[dptr] = (bbits_ptr_[dptr] & lmask) | tmp;
		  }
		  dptr += 1;

		  tmp = (that.abits_ptr_[sptr] >> ndoff) & lmask;
		  यदि ((abits_ptr_[dptr] & lmask) != tmp) {
			diff_flag = सत्य;
			abits_ptr_[dptr] = (abits_ptr_[dptr] & ~lmask) | tmp;
		  }
		  tmp = (that.bbits_ptr_[sptr] >> ndoff) & lmask;
		  यदि ((bbits_ptr_[dptr] & lmask) != tmp) {
			diff_flag = सत्य;
			bbits_ptr_[dptr] = (bbits_ptr_[dptr] & ~lmask) | tmp;
		  }

		  remain -= द्वयङ्काः_प्रत्येकम्_WORD;
		  sptr += 1;
	    }

	    यदि (remain > 0) {
		  अचिह्नित दीर्घ hshift = doff+remain;
		  अचिह्नित दीर्घ hmask;
		  यदि (hshift >= द्वयङ्काः_प्रत्येकम्_WORD)
			hmask = -1UL;
		  अन्यथा
			hmask = (1UL << (doff+remain)) - 1;

		  अचिह्नित दीर्घ आवरणम् = hmask & ~lmask;
		  अचिह्नित दीर्घ tmp;

		  tmp = (that.abits_ptr_[sptr] << doff) & आवरणम्;
		  यदि ((abits_ptr_[dptr] & आवरणम्) != tmp) {
			diff_flag = सत्य;
			abits_ptr_[dptr] = (abits_ptr_[dptr] & ~आवरणम्) | tmp;
		  }
		  tmp = (that.bbits_ptr_[sptr] << doff) & आवरणम्;
		  यदि ((bbits_ptr_[dptr] & आवरणम्) != tmp) {
			diff_flag = सत्य;
			bbits_ptr_[dptr] = (bbits_ptr_[dptr] & ~आवरणम्) | tmp;
		  }

		  यदि ((doff + remain) > द्वयङ्काः_प्रत्येकम्_WORD) {
			अचिह्नित पुच्छ = doff + remain - द्वयङ्काः_प्रत्येकम्_WORD;
			यदि (पुच्छ >= द्वयङ्काः_प्रत्येकम्_WORD)
			      आवरणम् = -1UL;
			अन्यथा
			      आवरणम् = (1UL << पुच्छ) - 1;

			dptr += 1;

			tmp = (that.abits_ptr_[sptr] >> (remain-पुच्छ))&आवरणम्;
			यदि ((abits_ptr_[dptr] & आवरणम्) != tmp) {
			      diff_flag = सत्य;
			      abits_ptr_[dptr] = (abits_ptr_[dptr] & ~आवरणम्) | tmp;
			}
			tmp = (that.bbits_ptr_[sptr] >> (remain-पुच्छ))&आवरणम्;
			यदि ((bbits_ptr_[dptr] & आवरणम्) != tmp) {
			      diff_flag = सत्य;
			      bbits_ptr_[dptr] = (bbits_ptr_[dptr] & ~आवरणम्) | tmp;
			}
		  }
	    }
      }

      प्रत्यागम diff_flag;
}

/*
 * Add that vector to this vector. Do it in the Verilog way, which
 * means if we detect any X or Z bits, change the entire results to
 * all X.
 *
 * Assume both vectors are the same size.
 */
शून्य vvp_vector4_t::सङ्कलय(ध्रुव vvp_vector4_t&that)
{
      प्रतिज्ञा(आकार_2 == that.आकार_2);

      यदि (आकार_2 < द्वयङ्काः_प्रत्येकम्_WORD) {
	    अचिह्नित दीर्घ आवरणम् = ~(-1UL << आकार_2);
	    यदि ((bbits_val_|that.bbits_val_) & आवरणम्) {
		  abits_val_ |= आवरणम्;
		  bbits_val_ |= आवरणम्;
		  प्रत्यागम;
	    }

	    abits_val_ += that.abits_val_;
	    abits_val_ &= आवरणम्;
	    प्रत्यागम;
      }

      यदि (आकार_2 == द्वयङ्काः_प्रत्येकम्_WORD) {
	    यदि (bbits_val_ | that.bbits_val_) {
		  abits_val_ = WORD_X_ABITS;
		  bbits_val_ = WORD_X_BBITS;
	    } अन्यथा {
		  abits_val_ += that.abits_val_;
	    }
	    प्रत्यागम;
      }

      पूर्णाङ्क cnt = आकार_2 / द्वयङ्काः_प्रत्येकम्_WORD;
      अचिह्नित दीर्घ carry = 0;
      प्रति (पूर्णाङ्क idx = 0 ; idx < cnt ; idx += 1) {
	    यदि (bbits_ptr_[idx] | that.bbits_ptr_[idx])
		  गच्छ x_बहिर्गमनम्;

	    abits_ptr_[idx] = सङ्कलय_with_carry(abits_ptr_[idx], that.abits_ptr_[idx], carry);
      }

      यदि (अचिह्नित पुच्छ = आकार_2 % द्वयङ्काः_प्रत्येकम्_WORD) {
	    अचिह्नित दीर्घ आवरणम् = ~( -1UL << पुच्छ );
	    यदि ((bbits_ptr_[cnt] | that.bbits_ptr_[cnt])&आवरणम्)
		  गच्छ x_बहिर्गमनम्;

	    abits_ptr_[cnt] = सङ्कलय_with_carry(abits_ptr_[cnt], that.abits_ptr_[cnt], carry);
	    abits_ptr_[cnt] &= आवरणम्;
      }

      प्रत्यागम;

 x_बहिर्गमनम्:
      प्रति (पूर्णाङ्क idx = 0 ; idx < cnt ; idx += 1) {
	    abits_ptr_[idx] = WORD_X_ABITS;
	    bbits_ptr_[idx] = WORD_X_BBITS;
      }
      यदि (अचिह्नित पुच्छ = आकार_2%द्वयङ्काः_प्रत्येकम्_WORD) {
	    अचिह्नित दीर्घ आवरणम् = ~( -1UL << पुच्छ );
	    abits_ptr_[cnt] = WORD_X_ABITS&आवरणम्;
	    bbits_ptr_[cnt] = WORD_X_BBITS&आवरणम्;
      }
}

शून्य vvp_vector4_t::sub(ध्रुव vvp_vector4_t&that)
{
      प्रतिज्ञा(आकार_2 == that.आकार_2);

      यदि (आकार_2 < द्वयङ्काः_प्रत्येकम्_WORD) {
	    अचिह्नित दीर्घ आवरणम् = ~(-1UL << आकार_2);
	    यदि ((bbits_val_|that.bbits_val_) & आवरणम्) {
		  abits_val_ |= आवरणम्;
		  bbits_val_ |= आवरणम्;
		  प्रत्यागम;
	    }

	    abits_val_ -= that.abits_val_;
	    abits_val_ &= आवरणम्;
	    प्रत्यागम;
      }

      यदि (आकार_2 == द्वयङ्काः_प्रत्येकम्_WORD) {
	    यदि (bbits_val_ | that.bbits_val_) {
		  abits_val_ = WORD_X_ABITS;
		  bbits_val_ = WORD_X_BBITS;
	    } अन्यथा {
		  abits_val_ -= that.abits_val_;
	    }
	    प्रत्यागम;
      }

      पूर्णाङ्क cnt = आकार_2 / द्वयङ्काः_प्रत्येकम्_WORD;
      अचिह्नित दीर्घ carry = 1;
      प्रति (पूर्णाङ्क idx = 0 ; idx < cnt ; idx += 1) {
	    यदि (bbits_ptr_[idx] | that.bbits_ptr_[idx])
		  गच्छ x_बहिर्गमनम्;

	    abits_ptr_[idx] = सङ्कलय_with_carry(abits_ptr_[idx], ~that.abits_ptr_[idx], carry);
      }

      यदि (अचिह्नित पुच्छ = आकार_2 % द्वयङ्काः_प्रत्येकम्_WORD) {
	    अचिह्नित दीर्घ आवरणम् = ~( -1UL << पुच्छ );
	    यदि ((bbits_ptr_[cnt] | that.bbits_ptr_[cnt])&आवरणम्)
		  गच्छ x_बहिर्गमनम्;

	    abits_ptr_[cnt] = सङ्कलय_with_carry(abits_ptr_[cnt], ~that.abits_ptr_[cnt], carry);
	    abits_ptr_[cnt] &= आवरणम्;
      }

      प्रत्यागम;

 x_बहिर्गमनम्:
      प्रति (पूर्णाङ्क idx = 0 ; idx < cnt ; idx += 1) {
	    abits_ptr_[idx] = WORD_X_ABITS;
	    bbits_ptr_[idx] = WORD_X_BBITS;
      }
      यदि (अचिह्नित पुच्छ = आकार_2%द्वयङ्काः_प्रत्येकम्_WORD) {
	    अचिह्नित दीर्घ आवरणम् = ~( -1UL << पुच्छ );
	    abits_ptr_[cnt] = WORD_X_ABITS&आवरणम्;
	    bbits_ptr_[cnt] = WORD_X_BBITS&आवरणम्;
      }

}

शून्य vvp_vector4_t::mov(अचिह्नित dst, अचिह्नित src, अचिह्नित cnt)
{
      प्रतिज्ञा(dst+cnt <= आकार_2);
      प्रतिज्ञा(src+cnt <= आकार_2);

      यदि (आकार_2 <= द्वयङ्काः_प्रत्येकम्_WORD) {
	    अचिह्नित दीर्घ vmask = (1UL << cnt) - 1;
	    अचिह्नित दीर्घ tmp;

	    tmp = (abits_val_ >> src) & vmask;
	    abits_val_ &= ~ (vmask << dst);
	    abits_val_ |= tmp << dst;

	    tmp = (bbits_val_ >> src) & vmask;
	    bbits_val_ &= ~ (vmask << dst);
	    bbits_val_ |= tmp << dst;

      } अन्यथा {
	    अचिह्नित sptr = src / द्वयङ्काः_प्रत्येकम्_WORD;
	    अचिह्नित dptr = dst / द्वयङ्काः_प्रत्येकम्_WORD;
	    अचिह्नित soff = src % द्वयङ्काः_प्रत्येकम्_WORD;
	    अचिह्नित doff = dst % द्वयङ्काः_प्रत्येकम्_WORD;

	    यावत् (cnt > 0) {
		  अचिह्नित trans = cnt;
		  यदि ((soff+trans) > द्वयङ्काः_प्रत्येकम्_WORD)
			trans = द्वयङ्काः_प्रत्येकम्_WORD - soff;

		  यदि ((doff+trans) > द्वयङ्काः_प्रत्येकम्_WORD)
			trans = द्वयङ्काः_प्रत्येकम्_WORD - doff;

		  यदि (trans == द्वयङ्काः_प्रत्येकम्_WORD) {
			  // Special case: the transfer count is
			  // exactly an entire word. For this to be
			  // true, it must also be true that the
			  // pointers are aligned. The work is easy,
			abits_ptr_[dptr] = abits_ptr_[sptr];
			bbits_ptr_[dptr] = bbits_ptr_[sptr];
			dptr += 1;
			sptr += 1;
			cnt -= द्वयङ्काः_प्रत्येकम्_WORD;
			अनुवर्त;
		  }

		    // Here we know that either the source or
		    // destination is unaligned, and also we know that
		    // the count is less than a full word.
		  अचिह्नित दीर्घ vmask = (1UL << trans) - 1;
		  अचिह्नित दीर्घ tmp;

		  tmp = (abits_ptr_[sptr] >> soff) & vmask;
		  abits_ptr_[dptr] &= ~ (vmask << doff);
		  abits_ptr_[dptr] |= tmp << doff;

		  tmp = (bbits_ptr_[sptr] >> soff) & vmask;
		  bbits_ptr_[dptr] &= ~ (vmask << doff);
		  bbits_ptr_[dptr] |= tmp << doff;

		  cnt -= trans;
		  soff += trans;
		  यदि (soff >= द्वयङ्काः_प्रत्येकम्_WORD) {
			soff = 0;
			sptr += 1;
		  }
		  doff += trans;
		  यदि (doff >= द्वयङ्काः_प्रत्येकम्_WORD) {
			doff = 0;
			dptr += 1;
		  }
	    }
      }
}

शून्य vvp_vector4_t::mul(ध्रुव vvp_vector4_t&that)
{
      प्रतिज्ञा(आकार_2 == that.आकार_2);

      यदि (आकार_2 < द्वयङ्काः_प्रत्येकम्_WORD) {
	    अचिह्नित दीर्घ आवरणम् = ~(-1UL << आकार_2);
	    यदि ((bbits_val_|that.bbits_val_) & आवरणम्) {
		  abits_val_ |= आवरणम्;
		  bbits_val_ |= आवरणम्;
		  प्रत्यागम;
	    }

	    abits_val_ *= that.abits_val_;
	    abits_val_ &= आवरणम्;
	    प्रत्यागम;
      }

      यदि (आकार_2 == द्वयङ्काः_प्रत्येकम्_WORD) {
	    यदि (bbits_val_ || that.bbits_val_) {
		  abits_val_ = WORD_X_ABITS;
		  bbits_val_ = WORD_X_BBITS;
	    } अन्यथा {
		  abits_val_ *= that.abits_val_;
	    }
	    प्रत्यागम;
      }

      ध्रुव पूर्णाङ्क cnt = (आकार_2+द्वयङ्काः_प्रत्येकम्_WORD-1) / द्वयङ्काः_प्रत्येकम्_WORD;

      अचिह्नित दीर्घ आवरणम्;
      यदि (अचिह्नित पुच्छ = आकार_2%द्वयङ्काः_प्रत्येकम्_WORD) {
	    आवरणम् = ~( -1UL << पुच्छ );
      } अन्यथा {
	    आवरणम् = ~0UL;
      }

	// Check for any XZ values ahead of time in a first pass. If
	// we find any, then force the entire result to be X and be
	// done.
      प्रति (पूर्णाङ्क idx = 0 ; idx < cnt ; idx += 1) {
	    अचिह्नित दीर्घ lval = bbits_ptr_[idx];
	    अचिह्नित दीर्घ rval = that.bbits_ptr_[idx];
	    यदि (idx == (cnt-1)) {
		  lval &= आवरणम्;
		  rval &= आवरणम्;
	    }
	    यदि (lval || rval) {
		  प्रति (पूर्णाङ्क xdx = 0 ; xdx < cnt-1 ; xdx += 1) {
			abits_ptr_[xdx] = WORD_X_ABITS;
			bbits_ptr_[xdx] = WORD_X_BBITS;
		  }
		  abits_ptr_[cnt-1] = WORD_X_ABITS & आवरणम्;
		  bbits_ptr_[cnt-1] = WORD_X_BBITS & आवरणम्;
		  प्रत्यागम;
	    }
      }

	// Calculate the result into a res array. We need to keep is
	// separate from the "this" array because we are making
	// multiple passes.
      अचिह्नित दीर्घ*res = नूतन अचिह्नित दीर्घ[cnt];
      प्रति (पूर्णाङ्क idx = 0 ; idx < cnt ; idx += 1)
	    res[idx] = 0;

      प्रति (पूर्णाङ्क mul_a = 0 ; mul_a < cnt ; mul_a += 1) {
	    अचिह्नित दीर्घ lval = abits_ptr_[mul_a];
	    यदि (mul_a == (cnt-1))
		  lval &= आवरणम्;

	    प्रति (पूर्णाङ्क mul_b = 0 ; mul_b < (cnt-mul_a) ; mul_b += 1) {
		  अचिह्नित दीर्घ rval = that.abits_ptr_[mul_b];
		  यदि (mul_b == (cnt-1))
			rval &= आवरणम्;

		  अचिह्नित दीर्घ योग;
		  अचिह्नित दीर्घ tmp = multiply_with_carry(lval, rval, योग);
		  पूर्णाङ्क आधारः = mul_a + mul_b;
		  अचिह्नित दीर्घ carry = 0;
		  res[आधारः] = सङ्कलय_with_carry(res[आधारः], tmp, carry);
		  प्रति (पूर्णाङ्क सङ्कलय_idx = आधारः+1 ; सङ्कलय_idx < cnt ; सङ्कलय_idx += 1) {
			res[सङ्कलय_idx] = सङ्कलय_with_carry(res[सङ्कलय_idx], योग, carry);
			योग = 0;
		  }
	    }
      }

	// Replace the "this" value with the calculated result. We
	// know a-priori that the bbits are zero and unchanged.
      res[cnt-1] &= आवरणम्;
      प्रति (पूर्णाङ्क idx = 0 ; idx < cnt ; idx += 1)
	    abits_ptr_[idx] = res[idx];

      विलोपय[]res;
      प्रत्यागम;


}

तार्किक vvp_vector4_t::eeq(ध्रुव vvp_vector4_t&that) ध्रुव
{
      यदि (आकार_2 != that.आकार_2)
	    प्रत्यागम असत्य;

      यदि (आकार_2 < द्वयङ्काः_प्रत्येकम्_WORD) {
	    अचिह्नित दीर्घ आवरणम् = (1UL << आकार_2) - 1;
	    प्रत्यागम (abits_val_&आवरणम्) == (that.abits_val_&आवरणम्)
		  && (bbits_val_&आवरणम्) == (that.bbits_val_&आवरणम्);
      }

      यदि (आकार_2 == द्वयङ्काः_प्रत्येकम्_WORD) {
	    प्रत्यागम (abits_val_ == that.abits_val_)
		  && (bbits_val_ == that.bbits_val_);
      }

      अचिह्नित शब्दाः = आकार_2 / द्वयङ्काः_प्रत्येकम्_WORD;
      प्रति (अचिह्नित idx = 0 ;  idx < शब्दाः ;  idx += 1) {
	    यदि (abits_ptr_[idx] != that.abits_ptr_[idx])
		  प्रत्यागम असत्य;
	    यदि (bbits_ptr_[idx] != that.bbits_ptr_[idx])
		  प्रत्यागम असत्य;
      }

      अचिह्नित दीर्घ आवरणम् = आकार_2%द्वयङ्काः_प्रत्येकम्_WORD;
      यदि (आवरणम् > 0) {
	    आवरणम् = (1UL << आवरणम्) - 1;
	    प्रत्यागम (abits_ptr_[शब्दाः]&आवरणम्) == (that.abits_ptr_[शब्दाः]&आवरणम्)
		  && (bbits_ptr_[शब्दाः]&आवरणम्) == (that.bbits_ptr_[शब्दाः]&आवरणम्);
      }

      प्रत्यागम सत्य;
}

तार्किक vvp_vector4_t::eq_xz(ध्रुव vvp_vector4_t&that) ध्रुव
{
      यदि (आकार_2 != that.आकार_2)
	    प्रत्यागम असत्य;

      यदि (आकार_2 < द्वयङ्काः_प्रत्येकम्_WORD) {
	    अचिह्नित दीर्घ आवरणम् = (1UL << आकार_2) - 1;
	    प्रत्यागम ((abits_val_|bbits_val_)&आवरणम्) == ((that.abits_val_|that.bbits_val_)&आवरणम्)
		  && (bbits_val_&आवरणम्) == (that.bbits_val_&आवरणम्);
      }

      यदि (आकार_2 == द्वयङ्काः_प्रत्येकम्_WORD) {
	    प्रत्यागम ((abits_val_|bbits_val_) == (that.abits_val_|that.bbits_val_))
		  && (bbits_val_ == that.bbits_val_);
      }

      अचिह्नित शब्दाः = आकार_2 / द्वयङ्काः_प्रत्येकम्_WORD;
      प्रति (अचिह्नित idx = 0 ;  idx < शब्दाः ;  idx += 1) {
	    यदि ((abits_ptr_[idx]|bbits_ptr_[idx]) != (that.abits_ptr_[idx]|that.bbits_ptr_[idx]))
		  प्रत्यागम असत्य;
	    यदि (bbits_ptr_[idx] != that.bbits_ptr_[idx])
		  प्रत्यागम असत्य;
      }

      अचिह्नित दीर्घ आवरणम् = आकार_2%द्वयङ्काः_प्रत्येकम्_WORD;
      यदि (आवरणम् > 0) {
	    आवरणम् = (1UL << आवरणम्) - 1;
	    प्रत्यागम ((abits_ptr_[शब्दाः]|bbits_ptr_[शब्दाः])&आवरणम्) == ((that.abits_ptr_[शब्दाः]|that.bbits_ptr_[शब्दाः])&आवरणम्)
		  && (bbits_ptr_[शब्दाः]&आवरणम्) == (that.bbits_ptr_[शब्दाः]&आवरणम्);
      }

      प्रत्यागम सत्य;
}

तार्किक vvp_vector4_t::has_xz() ध्रुव
{
      यदि (आकार_2 < द्वयङ्काः_प्रत्येकम्_WORD) {
	    अचिह्नित दीर्घ आवरणम् = -1UL >> (द्वयङ्काः_प्रत्येकम्_WORD - आकार_2);
	    प्रत्यागम bbits_val_&आवरणम्;
      }

      यदि (आकार_2 == द्वयङ्काः_प्रत्येकम्_WORD) {
	    प्रत्यागम bbits_val_;
      }

      अचिह्नित शब्दाः = आकार_2 / द्वयङ्काः_प्रत्येकम्_WORD;
      प्रति (अचिह्नित idx = 0 ; idx < शब्दाः ; idx += 1) {
	    यदि (bbits_ptr_[idx])
		  प्रत्यागम सत्य;
      }

      अचिह्नित दीर्घ आवरणम् = आकार_2%द्वयङ्काः_प्रत्येकम्_WORD;
      यदि (आवरणम् > 0) {
	    आवरणम् = -1UL >> (द्वयङ्काः_प्रत्येकम्_WORD - आवरणम्);
	    प्रत्यागम bbits_ptr_[शब्दाः]&आवरणम्;
      }

      प्रत्यागम असत्य;
}

शून्य vvp_vector4_t::change_z2x()
{
	// This method relies on the fact that both BIT4_X and BIT4_Z
	// have the bbit set in the vector4 encoding, and also that
	// the BIT4_X has abit set in the vector4 encoding. By simply
	// or-ing the bbit into the abit, BIT4_X and BIT4_Z both
	// become BIT4_X.

      यदि (आकार_2 <= द्वयङ्काः_प्रत्येकम्_WORD) {
	    abits_val_ |= bbits_val_;
      } अन्यथा {
	    अचिह्नित शब्दाः = (आकार_2+द्वयङ्काः_प्रत्येकम्_WORD-1) / द्वयङ्काः_प्रत्येकम्_WORD;
	    प्रति (अचिह्नित idx = 0 ;  idx < शब्दाः ;  idx += 1)
		  abits_ptr_[idx] |= bbits_ptr_[idx];
      }
}

शून्य vvp_vector4_t::fill_द्वयङ्काः(vvp_bit4_t bit)
{
	/* note: this relies on the bit encoding for the vvp_bit4_t. */
      स्थिर ध्रुव अचिह्नित दीर्घ init_atable[4] = {
	    WORD_0_ABITS,
	    WORD_1_ABITS,
	    WORD_Z_ABITS,
	    WORD_X_ABITS };
      स्थिर ध्रुव अचिह्नित दीर्घ init_btable[4] = {
	    WORD_0_BBITS,
	    WORD_1_BBITS,
	    WORD_Z_BBITS,
	    WORD_X_BBITS };

      यदि (आकार_2 <= द्वयङ्काः_प्रत्येकम्_WORD) {
	    abits_val_ = init_atable[bit];
	    bbits_val_ = init_btable[bit];
      } अन्यथा {
	    अचिह्नित शब्दाः = (आकार_2 + द्वयङ्काः_प्रत्येकम्_WORD - 1) / द्वयङ्काः_प्रत्येकम्_WORD;
	    प्रति (अचिह्नित idx = 0; idx < शब्दाः; idx += 1) {
		  abits_ptr_[idx] = init_atable[bit];
		  bbits_ptr_[idx] = init_btable[bit];
	    }
      }
}

वर्ण* vvp_vector4_t::as_string(वर्ण*buf, size_t buf_len) ध्रुव
{
      वर्ण*res = buf;
      *buf++ = 'C';
      *buf++ = '4';
      *buf++ = '<';
      buf_len -= 3;

      प्रति (अचिह्नित idx = 0 ;  idx < size() && buf_len >= 2 ;  idx += 1) {
	    विकल्प (value(size()-idx-1)) {
		पक्ष BIT4_0:
		  *buf++ = '0';
		  भङ्ग;
		पक्ष BIT4_1:
		  *buf++ = '1';
		  भङ्ग;
		पक्ष BIT4_X:
		  *buf++ = 'x';
		  भङ्ग;
		पक्ष BIT4_Z:
		  *buf++ = 'z';
	    }
	    buf_len -= 1;
      }

      *buf++ = '>';
      *buf++ = 0;
      प्रत्यागम res;
}

शून्य vvp_vector4_t::invert()
{
      यदि (आकार_2 <= द्वयङ्काः_प्रत्येकम्_WORD) {
	    अचिह्नित दीर्घ आवरणम् = (आकार_2<द्वयङ्काः_प्रत्येकम्_WORD)? (1UL<<आकार_2)-1UL : -1UL;
	    abits_val_ = आवरणम् & ~abits_val_;
	    abits_val_ |= bbits_val_;
      } अन्यथा {
	    अचिह्नित remaining = आकार_2;
	    अचिह्नित idx = 0;
	    यावत् (remaining >= द्वयङ्काः_प्रत्येकम्_WORD) {
		  abits_ptr_[idx] = ~abits_ptr_[idx];
		  abits_ptr_[idx] |= bbits_ptr_[idx];
		  idx += 1;
		  remaining -= द्वयङ्काः_प्रत्येकम्_WORD;
	    }
	    यदि (remaining > 0) {
		  अचिह्नित दीर्घ आवरणम् = (1UL<<remaining) - 1UL;
		  abits_ptr_[idx] = आवरणम् & ~abits_ptr_[idx];
		  abits_ptr_[idx] |= bbits_ptr_[idx];
	    }
      }
}

#परिभाषय BIT_आवरणम्(n) ((n) ? ((~0UL) >> (द्वयङ्काः_प्रत्येकम्_WORD - (n))) : ~0UL)

vvp_bit4_t vvp_vector4_t::reduce_or() ध्रुव
{
      अचिह्नित दीर्घ आवरणम् = BIT_आवरणम्(आकार_2 % द्वयङ्काः_प्रत्येकम्_WORD);
      vvp_bit4_t res = BIT4_0;

      यदि (आकार_2 <= द्वयङ्काः_प्रत्येकम्_WORD) {
	    यदि ((abits_val_ & ~bbits_val_ & आवरणम्) != 0UL)
		  प्रत्यागम BIT4_1;
	    यदि ((bbits_val_ & आवरणम्) != 0UL)
		  प्रत्यागम BIT4_X;
      } अन्यथा {
	    अचिह्नित शब्दाः = (आकार_2 + द्वयङ्काः_प्रत्येकम्_WORD - 1) / द्वयङ्काः_प्रत्येकम्_WORD;
	    अचिह्नित idx;
	    प्रति (idx = 0; idx < शब्दाः - 1; idx += 1) {
		  यदि ((abits_ptr_[idx] & ~bbits_ptr_[idx]) != 0UL)
			प्रत्यागम BIT4_1;
		  यदि (bbits_ptr_[idx] != 0UL)
			res = BIT4_X;
	    }
	    यदि ((abits_ptr_[idx] & ~bbits_ptr_[idx] & आवरणम्) != 0UL)
		  प्रत्यागम BIT4_1;
	    यदि ((bbits_ptr_[idx] & आवरणम्) != 0UL)
		  res = BIT4_X;
      }

      प्रत्यागम res;
}

vvp_bit4_t vvp_vector4_t::reduce_and() ध्रुव
{
      अचिह्नित दीर्घ आवरणम् = BIT_आवरणम्(आकार_2 % द्वयङ्काः_प्रत्येकम्_WORD);
      vvp_bit4_t res = BIT4_1;

      यदि (आकार_2 <= द्वयङ्काः_प्रत्येकम्_WORD) {
	    यदि ((abits_val_ | bbits_val_ | ~आवरणम्) != ~0UL)
		  प्रत्यागम BIT4_0;
	    यदि ((bbits_val_ & आवरणम्) != 0UL)
		  प्रत्यागम BIT4_X;
	} अन्यथा {
	    अचिह्नित शब्दाः = (आकार_2 + द्वयङ्काः_प्रत्येकम्_WORD - 1) / द्वयङ्काः_प्रत्येकम्_WORD;
	    अचिह्नित idx;
	    प्रति (idx = 0; idx < शब्दाः - 1; idx += 1) {
		  यदि ((abits_ptr_[idx] | bbits_ptr_[idx]) != ~0UL)
			प्रत्यागम BIT4_0;
		  यदि (bbits_ptr_[idx] != 0UL)
			res = BIT4_X;
	    }
	    यदि ((abits_ptr_[idx] | bbits_ptr_[idx] | ~आवरणम्) != ~0UL)
		  प्रत्यागम BIT4_0;
	    यदि ((bbits_ptr_[idx] & आवरणम्) != 0UL)
		  res = BIT4_X;
      }

      प्रत्यागम res;
}

स्थिर अचिह्नित दीर्घ parity(अचिह्नित दीर्घ val)
{
#यदि defined(__GNUC__)
      // The compiler builtin can use target-specific CPU instructions.
      प्रत्यागम __builtin_parityl(val);
#अन्यथा
#यदि ULONG_MAX > 0xffffffffUL
	val ^= val >> 32;
#यदि_अन्तः
	val ^= val >> 16;
	val ^= val >> 8;
	val ^= val >> 4;

      प्रत्यागम (0x6996 >> (val & 0xf)) & 1;
#यदि_अन्तः
}

vvp_bit4_t vvp_vector4_t::reduce_xor() ध्रुव
{
      अचिह्नित दीर्घ आवरणम् = BIT_आवरणम्(आकार_2 % द्वयङ्काः_प्रत्येकम्_WORD);

      यदि (आकार_2 <= द्वयङ्काः_प्रत्येकम्_WORD) {
	    यदि ((bbits_val_ & आवरणम्) != 0UL)
		  प्रत्यागम BIT4_X;
	    प्रत्यागम parity(abits_val_ & आवरणम्) ? BIT4_1 : BIT4_0;
      } अन्यथा {
	    अचिह्नित शब्दाः = (आकार_2 + द्वयङ्काः_प्रत्येकम्_WORD - 1) / द्वयङ्काः_प्रत्येकम्_WORD;
	    अचिह्नित दीर्घ val_a = 0UL;
	    अचिह्नित idx;
	    प्रति (idx = 0; idx < शब्दाः - 1; idx += 1) {
		  यदि (bbits_ptr_[idx] != 0UL)
			प्रत्यागम BIT4_X;
		  val_a ^= abits_ptr_[idx];
	    }
	    यदि ((bbits_ptr_[idx] & आवरणम्) != 0UL)
		  प्रत्यागम BIT4_X;
	    val_a ^= abits_ptr_[idx] & आवरणम्;
	    प्रत्यागम parity(val_a) ? BIT4_1 : BIT4_0;
      }
}

vvp_vector4_t& vvp_vector4_t::संक्रियकः &= (ध्रुव vvp_vector4_t&that)
{
	// The truth table is:
	//     00 01 11 10
	//  00 00 00 00 00
	//  01 00 01 11 11
	//  11 00 11 11 11
	//  10 00 11 11 11
      यदि (आकार_2 <= द्वयङ्काः_प्रत्येकम्_WORD) {
	    अचिह्नित दीर्घ tmp1 = abits_val_ | bbits_val_;
	    अचिह्नित दीर्घ tmp2 = that.abits_val_ | that.bbits_val_;
	    abits_val_ = tmp1 & tmp2;
	    bbits_val_ = (tmp1 & that.bbits_val_) | (tmp2 & bbits_val_);
      } अन्यथा {
	    अचिह्नित शब्दाः = (आकार_2 + द्वयङ्काः_प्रत्येकम्_WORD - 1) / द्वयङ्काः_प्रत्येकम्_WORD;
	    प्रति (अचिह्नित idx = 0; idx < शब्दाः ; idx += 1) {
		  अचिह्नित दीर्घ tmp1 = abits_ptr_[idx] | bbits_ptr_[idx];
		  अचिह्नित दीर्घ tmp2 = that.abits_ptr_[idx] |
		                       that.bbits_ptr_[idx];
		  abits_ptr_[idx] = tmp1 & tmp2;
		  bbits_ptr_[idx] = (tmp1 & that.bbits_ptr_[idx]) |
		                    (tmp2 & bbits_ptr_[idx]);
	    }
      }

      प्रत्यागम *एतत्;
}

vvp_vector4_t& vvp_vector4_t::संक्रियकः |= (ध्रुव vvp_vector4_t&that)
{
	// The truth table is:
	//     00 01 11 10
	//  00 00 01 11 11
	//  01 01 01 01 01
	//  11 11 01 11 11
	//  10 11 01 11 11
      यदि (आकार_2 <= द्वयङ्काः_प्रत्येकम्_WORD) {
	    अचिह्नित दीर्घ tmp = abits_val_ | bbits_val_ |
	                        that.abits_val_ | that.bbits_val_;
	    bbits_val_ = ((~abits_val_ | bbits_val_) & that.bbits_val_) |
	                 ((~that.abits_val_ | that.bbits_val_) & bbits_val_);
	    abits_val_ = tmp;

      } अन्यथा {
	    अचिह्नित शब्दाः = (आकार_2 + द्वयङ्काः_प्रत्येकम्_WORD - 1) / द्वयङ्काः_प्रत्येकम्_WORD;
	    प्रति (अचिह्नित idx = 0; idx < शब्दाः ; idx += 1) {
		  अचिह्नित दीर्घ tmp = abits_ptr_[idx] | bbits_ptr_[idx] |
	                        that.abits_ptr_[idx] | that.bbits_ptr_[idx];
		  bbits_ptr_[idx] = ((~abits_ptr_[idx] | bbits_ptr_[idx]) &
		                     that.bbits_ptr_[idx]) |
		                    ((~that.abits_ptr_[idx] |
		                      that.bbits_ptr_[idx]) & bbits_ptr_[idx]);
		  abits_ptr_[idx] = tmp;
	    }
      }

      प्रत्यागम *एतत्;
}

vvp_vector4_t& vvp_vector4_t::संक्रियकः ^= (ध्रुव vvp_vector4_t&that)
{
	// The truth table is:
	//     00 01 11 10
	//  00 00 01 11 11
	//  01 01 00 11 11
	//  11 11 11 11 11
	//  10 11 11 11 11
      यदि (आकार_2 <= द्वयङ्काः_प्रत्येकम्_WORD) {
	    अचिह्नित दीर्घ bval = bbits_val_ | that.bbits_val_;
	    bbits_val_ = bval;
	    abits_val_ = (abits_val_ ^ that.abits_val_) | bval;
      } अन्यथा {
	    अचिह्नित शब्दाः = (आकार_2 + द्वयङ्काः_प्रत्येकम्_WORD - 1) / द्वयङ्काः_प्रत्येकम्_WORD;
	    प्रति (अचिह्नित idx = 0; idx < शब्दाः ; idx += 1) {
		  अचिह्नित दीर्घ bval = bbits_ptr_[idx] | that.bbits_ptr_[idx];
		  bbits_ptr_[idx] = bval;
		  abits_ptr_[idx] = (abits_ptr_[idx] ^ that.abits_ptr_[idx]) | bval;
	    }
      }

      प्रत्यागम *एतत्;
}

/*
* Add an integer to the vvp_vector4_t in place, bit by bit so that
* there is no size limitations.
*/
vvp_vector4_t& vvp_vector4_t::संक्रियकः += (int64_t that)
{
      vvp_bit4_t carry = BIT4_0;
      अचिह्नित idx;

      यदि (has_xz()) {
	    vvp_vector4_t xxx (size(), BIT4_X);
	    *एतत् = xxx;
	    प्रत्यागम *एतत्;
      }

      प्रति (idx = 0 ; idx < size() ; idx += 1) {
	    यदि (that == 0 && carry==BIT4_0)
		  भङ्ग;

	    vvp_bit4_t that_bit = (that&1)? BIT4_1 : BIT4_0;
	    that >>= 1;

	    यदि (that_bit==BIT4_0 && carry==BIT4_0)
		  अनुवर्त;

	    vvp_bit4_t bit = value(idx);
	    bit = सङ्कलय_with_carry(bit, that_bit, carry);

	    समुच्चयः_bit(idx, bit);
      }

      प्रत्यागम *एतत्;
}

ostream& संक्रियकः<< (ostream&out, ध्रुव vvp_vector4_t&that)
{
      out << that.size() << "'b";
      प्रति (अचिह्नित idx = 0 ;  idx < that.size() ;  idx += 1)
	    out << that.value(that.size()-idx-1);
      प्रत्यागम out;
}

प्रतिरूपम् <वर्ग INT>तार्किक vector4_गन्तव्यम्_मानम्(ध्रुव vvp_vector4_t&vec, INT&val,
					  तार्किक is_signed, तार्किक is_arithmetic)
{
      INT res = 0;
      INT msk = 1;
      तार्किक rc_flag = सत्य;

      अचिह्नित size = vec.size();
      यदि (size > 8*परिमाण(val)) size = 8*परिमाण(val);
      प्रति (अचिह्नित idx = 0 ;  idx < size ;  idx += 1) {
	    विकल्प (vec.value(idx)) {
		पक्ष BIT4_0:
		  भङ्ग;
		पक्ष BIT4_1:
		  res |= msk;
		  भङ्ग;
		स्वभाव:
		  यदि (is_arithmetic)
			प्रत्यागम असत्य;
		  अन्यथा
			rc_flag = असत्य;
	    }

	    msk <<= 1;
      }

      यदि (is_signed && vec.value(vec.size()-1) == BIT4_1) {
	    यदि (vec.size() < 8*परिमाण(val))
		  res |= static_cast<INT>(-1ULL << vec.size());
      }

      val = res;
      प्रत्यागम rc_flag;
}

प्रतिरूपम् तार्किक vector4_गन्तव्यम्_मानम्(ध्रुव vvp_vector4_t&vec, int8_t&val,
			       तार्किक is_signed, तार्किक is_arithmetic);
प्रतिरूपम् तार्किक vector4_गन्तव्यम्_मानम्(ध्रुव vvp_vector4_t&vec, int16_t&val,
			       तार्किक is_signed, तार्किक is_arithmetic);
प्रतिरूपम् तार्किक vector4_गन्तव्यम्_मानम्(ध्रुव vvp_vector4_t&vec, int32_t&val,
			       तार्किक is_signed, तार्किक is_arithmetic);
प्रतिरूपम् तार्किक vector4_गन्तव्यम्_मानम्(ध्रुव vvp_vector4_t&vec, int64_t&val,
			       तार्किक is_signed, तार्किक is_arithmetic);
प्रतिरूपम् तार्किक vector4_गन्तव्यम्_मानम्(ध्रुव vvp_vector4_t&vec, uint8_t&val,
			       तार्किक is_signed, तार्किक is_arithmetic);
प्रतिरूपम् तार्किक vector4_गन्तव्यम्_मानम्(ध्रुव vvp_vector4_t&vec, uint16_t&val,
			       तार्किक is_signed, तार्किक is_arithmetic);
प्रतिरूपम् तार्किक vector4_गन्तव्यम्_मानम्(ध्रुव vvp_vector4_t&vec, uint32_t&val,
			       तार्किक is_signed, तार्किक is_arithmetic);
प्रतिरूपम् तार्किक vector4_गन्तव्यम्_मानम्(ध्रुव vvp_vector4_t&vec, uint64_t&val,
			       तार्किक is_signed, तार्किक is_arithmetic);

प्रतिरूपम् <वर्ग T> तार्किक vector4_गन्तव्यम्_मानम्(ध्रुव vvp_vector4_t&vec,
                                         तार्किक&अतिप्रवाहः_flag, T&val)
{
      T res = 0;
      T msk = 1;

      अतिप्रवाहः_flag = असत्य;
      अचिह्नित size = vec.size();
      प्रति (अचिह्नित idx = 0 ;  idx < size ;  idx += 1) {
	    विकल्प (vec.value(idx)) {
		पक्ष BIT4_0:
		  भङ्ग;
		पक्ष BIT4_1:
		  यदि (msk == 0)
			अतिप्रवाहः_flag = सत्य;
		  अन्यथा
			res |= msk;
		  भङ्ग;
		स्वभाव:
		  प्रत्यागम असत्य;
	    }

	    msk <<= static_cast<T>(1);
      }

      val = res;
      प्रत्यागम सत्य;
}

प्रतिरूपम् तार्किक vector4_गन्तव्यम्_मानम्(ध्रुव vvp_vector4_t&vec, तार्किक&अतिप्रवाहः_flag,
                               अचिह्नित दीर्घ&val);
#यदि_अपरिभाषितम् UL_AND_TIME64_SAME
प्रतिरूपम् तार्किक vector4_गन्तव्यम्_मानम्(ध्रुव vvp_vector4_t&vec, तार्किक&अतिप्रवाहः_flag,
                               vvp_time64_t&val);
#यदि_अन्तः

तार्किक vector4_गन्तव्यम्_मानम्(ध्रुव vvp_vector4_t&vec, द्विगुण&val, तार्किक signed_flag)
{

      यदि (vec.size() == 0) {
	    val = 0.0;
	    प्रत्यागम सत्य;
      }

      तार्किक flag = सत्य;

      यदि (vec.value(vec.size()-1) != BIT4_1) {
	    signed_flag = असत्य;
      }

      द्विगुण res = 0.0;
      यदि (signed_flag) {
	    vvp_bit4_t carry = BIT4_1;
	    प्रति (अचिह्नित idx = 0 ;  idx < vec.size() ;  idx += 1) {
		  vvp_bit4_t a = ~vec.value(idx);
		  vvp_bit4_t x = सङ्कलय_with_carry(a, BIT4_0, carry);
		  विकल्प (x) {
		      पक्ष BIT4_0:
			भङ्ग;
		      पक्ष BIT4_1:
			res += घातः(2.0, (पूर्णाङ्क)idx);
			भङ्ग;
		      स्वभाव:
			flag = असत्य;
		  }
	    }
	    res *= -1.0;
      } अन्यथा {
	    प्रति (अचिह्नित idx = 0 ;  idx < vec.size() ;  idx += 1) {
		  विकल्प (vec.value(idx)) {
		      पक्ष BIT4_0:
			भङ्ग;
		      पक्ष BIT4_1:
			res += घातः(2.0, (पूर्णाङ्क)idx);
			भङ्ग;
		      स्वभाव:
			flag = असत्य;
		  }
	    }
      }
      val = res;
      प्रत्यागम flag;
}

तार्किक vector2_गन्तव्यम्_मानम्(ध्रुव vvp_vector2_t&a, int32_t&val, तार्किक is_signed)
{
      val = 0;
      अचिह्नित idx;
      uint32_t आवरणम्;
      प्रति (idx = 0, आवरणम् = 1 ; idx < a.size() && idx < 32 ; idx += 1, आवरणम् <<= 1) {
	    यदि (a.value(idx)) val |= आवरणम्;
      }

      यदि (is_signed && a.size() < 32 && a.value(a.size()-1)) {
	    आवरणम् = -1;
	    आवरणम् <<= a.size();
	    val |= आवरणम्;
      }

      प्रत्यागम a.size() <= 32;
}

vvp_vector4array_t::vvp_vector4array_t(अचिह्नित विस्तारः_3, अचिह्नित शब्दाः_2)
: विस्तारः_2(विस्तारः_3), शब्दाः_3(शब्दाः_2)
{
}

vvp_vector4array_t::~vvp_vector4array_t()
{
}

शून्य vvp_vector4array_t::समुच्चयः_word_2(v4cell*cell, ध्रुव vvp_vector4_t&that)
{
      प्रतिज्ञा(that.आकार_2 == विस्तारः_2);

      यदि (विस्तारः_2 <= vvp_vector4_t::द्वयङ्काः_प्रत्येकम्_WORD) {
	    cell->abits_val_ = that.abits_val_;
	    cell->bbits_val_ = that.bbits_val_;
	    प्रत्यागम;
      }

      अचिह्नित cnt = (विस्तारः_2 + vvp_vector4_t::द्वयङ्काः_प्रत्येकम्_WORD-1)/vvp_vector4_t::द्वयङ्काः_प्रत्येकम्_WORD;

      यदि (cell->abits_ptr_ == 0) {
	    cell->abits_ptr_ = नूतन अचिह्नित दीर्घ[2*cnt];
	    cell->bbits_ptr_ = cell->abits_ptr_ + cnt;
      }

      प्रति (अचिह्नित idx = 0 ; idx < cnt ; idx += 1)
	    cell->abits_ptr_[idx] = that.abits_ptr_[idx];
      प्रति (अचिह्नित idx = 0 ; idx < cnt ; idx += 1)
	    cell->bbits_ptr_[idx] = that.bbits_ptr_[idx];
}

vvp_vector4_t vvp_vector4array_t::get_word_(v4cell*cell) ध्रुव
{
      यदि (विस्तारः_2 <= vvp_vector4_t::द्वयङ्काः_प्रत्येकम्_WORD) {
	    vvp_vector4_t res;
	    res.आकार_2 = विस्तारः_2;
	    res.abits_val_ = cell->abits_val_;
	    res.bbits_val_ = cell->bbits_val_;
	    प्रत्यागम res;
      }

      vvp_vector4_t res (विस्तारः_2, BIT4_X);
      यदि (cell->abits_ptr_ == 0)
	    प्रत्यागम res;

      अचिह्नित cnt = (विस्तारः_2 + vvp_vector4_t::द्वयङ्काः_प्रत्येकम्_WORD-1)/vvp_vector4_t::द्वयङ्काः_प्रत्येकम्_WORD;

      प्रति (अचिह्नित idx = 0 ; idx < cnt ; idx += 1)
	    res.abits_ptr_[idx] = cell->abits_ptr_[idx];
      प्रति (अचिह्नित idx = 0 ; idx < cnt ; idx += 1)
	    res.bbits_ptr_[idx] = cell->bbits_ptr_[idx];

      प्रत्यागम res;
}

vvp_vector4array_sa::vvp_vector4array_sa(अचिह्नित विस्तारः_3, अचिह्नित शब्दाः_2)
: vvp_vector4array_t(विस्तारः_3, शब्दाः_2)
{
      array_ = नूतन v4cell[शब्दाः_3];

      यदि (विस्तारः_2 <= vvp_vector4_t::द्वयङ्काः_प्रत्येकम्_WORD) {
	    प्रति (अचिह्नित idx = 0 ; idx < शब्दाः_3 ; idx += 1) {
		  array_[idx].abits_val_ = vvp_vector4_t::WORD_X_ABITS;
		  array_[idx].bbits_val_ = vvp_vector4_t::WORD_X_BBITS;
	    }
      } अन्यथा {
	    प्रति (अचिह्नित idx = 0 ; idx < शब्दाः_3 ; idx += 1) {
		  array_[idx].abits_ptr_ = 0;
		  array_[idx].bbits_ptr_ = 0;
	    }
      }
}

vvp_vector4array_sa::~vvp_vector4array_sa()
{
      यदि (array_) {
	    यदि (विस्तारः_2 > vvp_vector4_t::द्वयङ्काः_प्रत्येकम्_WORD) {
		  प्रति (अचिह्नित idx = 0 ; idx < शब्दाः_3 ; idx += 1)
			यदि (array_[idx].abits_ptr_)
			      विलोपय[]array_[idx].abits_ptr_;
	    }
	    विलोपय[]array_;
      }
}

शून्य vvp_vector4array_sa::समुच्चयः_word(अचिह्नित सूचकांक, ध्रुव vvp_vector4_t&that)
{
      प्रतिज्ञा(सूचकांक < शब्दाः_3);

      v4cell*cell = &array_[सूचकांक];

      समुच्चयः_word_2(cell, that);
}

vvp_vector4_t vvp_vector4array_sa::get_word(अचिह्नित सूचकांक) ध्रुव
{
      यदि (सूचकांक >= शब्दाः_3)
	    प्रत्यागम vvp_vector4_t(विस्तारः_2, BIT4_X);

      प्रतिज्ञा(सूचकांक < शब्दाः_3);

      v4cell*cell = &array_[सूचकांक];

      प्रत्यागम get_word_(cell);
}

vvp_vector4array_aa::vvp_vector4array_aa(अचिह्नित विस्तारः_3, अचिह्नित शब्दाः_2)
: vvp_vector4array_t(विस्तारः_3, शब्दाः_2)
{
      context_idx_ = vpip_सङ्कलय_item_गन्तव्यम्_context(एतत्, vpip_peek_context_scope());
}

vvp_vector4array_aa::~vvp_vector4array_aa()
{
}

शून्य vvp_vector4array_aa::alloc_instance(vvp_context_t context)
{
      v4cell*array = नूतन v4cell[शब्दाः_3];

      यदि (विस्तारः_2 <= vvp_vector4_t::द्वयङ्काः_प्रत्येकम्_WORD) {
	    प्रति (अचिह्नित idx = 0 ; idx < शब्दाः_3 ; idx += 1) {
		  array[idx].abits_val_ = vvp_vector4_t::WORD_X_ABITS;
		  array[idx].bbits_val_ = vvp_vector4_t::WORD_X_BBITS;
	    }
      } अन्यथा {
	    प्रति (अचिह्नित idx = 0 ; idx < शब्दाः_3 ; idx += 1) {
		  array[idx].abits_ptr_ = 0;
		  array[idx].bbits_ptr_ = 0;
	    }
      }

      vvp_समुच्चयः_context_item(context, context_idx_, array);
}

शून्य vvp_vector4array_aa::reset_instance(vvp_context_t context)
{
      v4cell*cell = static_cast<v4cell*>
            (vvp_get_context_item(context, context_idx_));

      यदि (विस्तारः_2 <= vvp_vector4_t::द्वयङ्काः_प्रत्येकम्_WORD) {
	    प्रति (अचिह्नित idx = 0 ; idx < शब्दाः_3 ; idx += 1) {
		  cell->abits_val_ = vvp_vector4_t::WORD_X_ABITS;
		  cell->bbits_val_ = vvp_vector4_t::WORD_X_BBITS;
                  cell++;
	    }
      } अन्यथा {
            अचिह्नित cnt = (विस्तारः_2 + vvp_vector4_t::द्वयङ्काः_प्रत्येकम्_WORD-1)/vvp_vector4_t::द्वयङ्काः_प्रत्येकम्_WORD;
	    प्रति (अचिह्नित idx = 0 ; idx < शब्दाः_3 ; idx += 1) {
		  यदि (cell->abits_ptr_) {
                        प्रति (अचिह्नित n = 0 ; n < cnt ; n += 1) {
			      cell->abits_ptr_[n] = vvp_vector4_t::WORD_X_ABITS;
			      cell->bbits_ptr_[n] = vvp_vector4_t::WORD_X_BBITS;
                        }
                  }
                  cell++;
	    }
      }
}

#यदि_परिभाषितम् CHECK_WITH_VALGRIND
शून्य vvp_vector4array_aa::free_instance(vvp_context_t context)
{
      v4cell*cell = static_cast<v4cell*>
            (vvp_get_context_item(context, context_idx_));
      विलोपय [] cell;
}
#यदि_अन्तः

शून्य vvp_vector4array_aa::समुच्चयः_word(अचिह्नित सूचकांक, ध्रुव vvp_vector4_t&that)
{
      प्रतिज्ञा(सूचकांक < शब्दाः_3);

      v4cell*cell = static_cast<v4cell*>
            (vthread_get_wt_context_item(context_idx_)) + सूचकांक;

      समुच्चयः_word_2(cell, that);
}

vvp_vector4_t vvp_vector4array_aa::get_word(अचिह्नित सूचकांक) ध्रुव
{
      यदि (सूचकांक >= शब्दाः_3)
	    प्रत्यागम vvp_vector4_t(विस्तारः_2, BIT4_X);

      प्रतिज्ञा(सूचकांक < शब्दाः_3);

      v4cell*cell = static_cast<v4cell*>
            (vthread_get_rd_context_item(context_idx_)) + सूचकांक;

      प्रत्यागम get_word_(cell);
}

vvp_vector2_t::vvp_vector2_t()
{
      vec_ = 0;
      wid_ = 0;
}

vvp_vector2_t::vvp_vector2_t(uint64_t v, अचिह्नित wid)
{
      wid_ = wid;
      ध्रुव अचिह्नित द्वयङ्काः_प्रत्येकम्_word = 8 * परिमाण(vec_[0]);
      ध्रुव अचिह्नित शब्दाः = (wid_ + द्वयङ्काः_प्रत्येकम्_word-1) / द्वयङ्काः_प्रत्येकम्_word;

      vec_ = नूतन अचिह्नित दीर्घ[शब्दाः];
      प्रति (अचिह्नित idx = 0 ; idx < शब्दाः ; idx += 1) {
	    ध्रुव अचिह्नित सरणम् = idx * द्वयङ्काः_प्रत्येकम्_word;
	    vec_[idx] = सरणम् < 64 ? v >> सरणम् : 0;
      }
}

vvp_vector2_t::vvp_vector2_t(vvp_vector2_t::fill_t fill, अचिह्नित wid)
{
      wid_ = wid;
      ध्रुव अचिह्नित द्वयङ्काः_प्रत्येकम्_word = 8 * परिमाण(vec_[0]);
      ध्रुव अचिह्नित शब्दाः = (wid_ + द्वयङ्काः_प्रत्येकम्_word-1) / द्वयङ्काः_प्रत्येकम्_word;

      vec_ = नूतन अचिह्नित दीर्घ[शब्दाः];
      प्रति (अचिह्नित idx = 0 ;  idx < शब्दाः ;  idx += 1)
	    vec_[idx] = fill? -1 : 0;
}

vvp_vector2_t::vvp_vector2_t(ध्रुव vvp_vector2_t&that, अचिह्नित आधारः, अचिह्नित wid)
{
      wid_ = wid;
      ध्रुव अचिह्नित शब्दाः = (wid_ + द्वयङ्काः_प्रत्येकम्_WORD-1) / द्वयङ्काः_प्रत्येकम्_WORD;

      vec_ = नूतन अचिह्नित दीर्घ[शब्दाः];

      प्रति (अचिह्नित idx = 0 ; idx < wid ; idx += 1) {
	    पूर्णाङ्क bit = that.value(आधारः+idx);
	    यदि (bit == 0)
		  अनुवर्त;
	    अचिह्नित word = idx / द्वयङ्काः_प्रत्येकम्_WORD;
	    अचिह्नित दीर्घ आवरणम् = 1UL << (idx % द्वयङ्काः_प्रत्येकम्_WORD);
	    vec_[word] |= आवरणम्;
      }
}

शून्य vvp_vector2_t::copy_from_that_(ध्रुव vvp_vector4_t&that)
{
      wid_ = that.size();
      ध्रुव अचिह्नित शब्दाः = (that.size() + द्वयङ्काः_प्रत्येकम्_WORD-1) / द्वयङ्काः_प्रत्येकम्_WORD;

      यदि (शब्दाः == 0) {
	    vec_ = 0;
	    wid_ = 0;
	    प्रत्यागम;
      }

	// Use the subarray method with the xz_to_0 flag set so that
	// we get values even when there are xz bits.
      vec_ = that.subarray(0, wid_, सत्य);
}

शून्य vvp_vector2_t::copy_from_that_(ध्रुव vvp_vector2_t&that)
{
      wid_ = that.wid_;
      ध्रुव अचिह्नित शब्दाः = (wid_ + द्वयङ्काः_प्रत्येकम्_WORD-1) / द्वयङ्काः_प्रत्येकम्_WORD;

      यदि (शब्दाः == 0) {
	    vec_ = 0;
	    wid_ = 0;
	    प्रत्यागम;
      }

      vec_ = नूतन अचिह्नित दीर्घ[शब्दाः];
      प्रति (अचिह्नित idx = 0 ;  idx < शब्दाः ;  idx += 1)
	    vec_[idx] = that.vec_[idx];
}

vvp_vector2_t::vvp_vector2_t(ध्रुव vvp_vector2_t&that, अचिह्नित newsize)
{
      wid_ = newsize;
      यदि (newsize == 0) {
	    vec_ = 0;
	    प्रत्यागम;
      }

      ध्रुव अचिह्नित शब्दाः = (wid_ + द्वयङ्काः_प्रत्येकम्_WORD-1) / द्वयङ्काः_प्रत्येकम्_WORD;
      ध्रुव अचिह्नित twords = (that.wid_ + द्वयङ्काः_प्रत्येकम्_WORD-1) / द्वयङ्काः_प्रत्येकम्_WORD;

      vec_ = नूतन अचिह्नित दीर्घ[शब्दाः];
      प्रति (अचिह्नित idx = 0 ;  idx < शब्दाः ;  idx += 1) {
	    यदि (idx < twords)
		  vec_[idx] = that.vec_[idx];
	    अन्यथा
		  vec_[idx] = 0;
      }
}

vvp_vector2_t& vvp_vector2_t::संक्रियकः= (ध्रुव vvp_vector2_t&that)
{
      यदि (एतत् == &that)
	    प्रत्यागम *एतत्;

      विलोपय[] vec_;
      vec_ = 0;

      copy_from_that_(that);
      प्रत्यागम *एतत्;
}

vvp_vector2_t& vvp_vector2_t::संक्रियकः= (ध्रुव vvp_vector4_t&that)
{
      विलोपय[]vec_;
      vec_ = 0;
      copy_from_that_(that);
      प्रत्यागम *एतत्;
}

vvp_vector2_t& vvp_vector2_t::संक्रियकः <<= (अचिह्नित पूर्णाङ्क सरणम्)
{
      यदि (wid_ == 0)
	    प्रत्यागम *एतत्;

      ध्रुव अचिह्नित शब्दाः = (wid_ + द्वयङ्काः_प्रत्येकम्_WORD-1) / द्वयङ्काः_प्रत्येकम्_WORD;

	// Number of words to shift
      ध्रुव अचिह्नित wshift = सरणम् / द्वयङ्काः_प्रत्येकम्_WORD;
	// bits to shift within each word.
      ध्रुव अचिह्नित दीर्घ oshift = सरणम् % द्वयङ्काः_प्रत्येकम्_WORD;

	// If shifting the entire value away, then return zeros.
      यदि (wshift >= शब्दाः) {
	    प्रति (अचिह्नित idx = 0 ;  idx < शब्दाः ;  idx += 1)
		  vec_[idx] = 0;

	    प्रत्यागम *एतत्;
      }

	// Do the word shift first.
      यदि (wshift > 0) {
	    प्रति (अचिह्नित idx = 0 ;  idx < शब्दाः-wshift ;  idx += 1) {
		  अचिह्नित sel = शब्दाः - idx - 1;
		  vec_[sel] = vec_[sel-wshift];
	    }

	    प्रति (अचिह्नित idx = 0 ;  idx < wshift ;  idx += 1)
		  vec_[idx] = 0;
      }

	// Do the fine shift.
      यदि (oshift != 0) {
	    अचिह्नित दीर्घ pad = 0;
	    प्रति (अचिह्नित idx = 0 ;  idx < शब्दाः ;  idx += 1) {
		  अचिह्नित दीर्घ अग्र_pad = vec_[idx] >> (द्वयङ्काः_प्रत्येकम्_WORD-oshift);
		  vec_[idx] = (vec_[idx] << oshift) | pad;
		  pad = अग्र_pad;
	    }

	      // Cleanup the tail bits.
	    अचिह्नित दीर्घ आवरणम् = -1UL >> (द्वयङ्काः_प्रत्येकम्_WORD - wid_%द्वयङ्काः_प्रत्येकम्_WORD);
	    vec_[शब्दाः-1] &= आवरणम्;
      }

      प्रत्यागम *एतत्;
}

vvp_vector2_t& vvp_vector2_t::संक्रियकः >>= (अचिह्नित सरणम्)
{
      यदि (wid_ == 0)
	    प्रत्यागम *एतत्;

      ध्रुव अचिह्नित शब्दाः = (wid_ + द्वयङ्काः_प्रत्येकम्_WORD-1) / द्वयङ्काः_प्रत्येकम्_WORD;

	// Number of words to shift
      ध्रुव अचिह्नित wshift = सरणम् / द्वयङ्काः_प्रत्येकम्_WORD;
	// bits to shift within each word.
      ध्रुव अचिह्नित दीर्घ oshift = सरणम् % द्वयङ्काः_प्रत्येकम्_WORD;

	// If shifting the entire value away, then return zeros.
      यदि (wshift >= शब्दाः) {
	    प्रति (अचिह्नित idx = 0 ;  idx < शब्दाः ;  idx += 1)
		  vec_[idx] = 0;

	    प्रत्यागम *एतत्;
      }

      यदि (wshift > 0) {
	    प्रति (अचिह्नित idx = 0 ;  idx < शब्दाः-wshift ;  idx += 1)
		  vec_[idx] = vec_[idx+wshift];

	    प्रति (अचिह्नित idx = शब्दाः-wshift ;  idx < शब्दाः ;  idx += 1)
		  vec_[idx] = 0;
      }

      यदि (oshift > 0) {
	    अचिह्नित दीर्घ pad = 0;
	    प्रति (अचिह्नित idx = शब्दाः ;  idx > 0 ;  idx -= 1) {
		  अचिह्नित दीर्घ new_pad = vec_[idx-1] <<(द्वयङ्काः_प्रत्येकम्_WORD-oshift);
		  vec_[idx-1] = pad | (vec_[idx-1] >> oshift);
		  pad = new_pad;
	    }

	      // Cleanup the tail bits.

	    अचिह्नित use_शब्दाः = शब्दाः;
	      // Mask_shift is the number of high bits of the top word
	      // that are to be masked off. We start with the number
	      // of bits that are not included even in the original,
	      // then we include the bits of the shift, that are to be
	      // masked to zero.
	    अचिह्नित दीर्घ आवरणम्_सरणम् = द्वयङ्काः_प्रत्येकम्_WORD - wid_%द्वयङ्काः_प्रत्येकम्_WORD;
	    आवरणम्_सरणम् %= द्वयङ्काः_प्रत्येकम्_WORD;
	    आवरणम्_सरणम् += oshift;
	    यावत् (आवरणम्_सरणम् >= द्वयङ्काः_प्रत्येकम्_WORD) {
		  vec_[use_शब्दाः-1] = 0;
		  use_शब्दाः -= 1;
		  आवरणम्_सरणम् -= द्वयङ्काः_प्रत्येकम्_WORD;
	    }
	    यदि (आवरणम्_सरणम् > 0) {
		  प्रतिज्ञा(use_शब्दाः > 0);
		  अचिह्नित दीर्घ आवरणम् = -1UL >> आवरणम्_सरणम्;
		  vec_[use_शब्दाः-1] &= आवरणम्;
	    }
      }

      प्रत्यागम *एतत्;
}

स्थिर अचिह्नित दीर्घ सङ्कलय_carry(अचिह्नित दीर्घ a, अचिह्नित दीर्घ b,
			       अचिह्नित दीर्घ&carry)
{
      अचिह्नित दीर्घ out = carry;
      carry = 0;

      यदि ((ULONG_MAX - out) < a)
	    carry += 1;
      out += a;

      यदि ((ULONG_MAX - out) < b)
	    carry += 1;
      out += b;

      प्रत्यागम out;
}

vvp_vector2_t& vvp_vector2_t::संक्रियकः += (ध्रुव vvp_vector2_t&that)
{
      प्रतिज्ञा(wid_ == that.wid_);
      यदि (wid_ == 0)
	    प्रत्यागम *एतत्;

      ध्रुव अचिह्नित शब्दाः = (wid_ + द्वयङ्काः_प्रत्येकम्_WORD-1) / द्वयङ्काः_प्रत्येकम्_WORD;

      अचिह्नित दीर्घ carry = 0;
      प्रति (अचिह्नित idx = 0 ;  idx < शब्दाः ;  idx += 1) {
	    vec_[idx] = सङ्कलय_carry(vec_[idx], that.vec_[idx], carry);
      }


	// Cleanup the tail bits.
      अचिह्नित दीर्घ आवरणम् = -1UL >> (द्वयङ्काः_प्रत्येकम्_WORD - wid_%द्वयङ्काः_प्रत्येकम्_WORD);
      vec_[शब्दाः-1] &= आवरणम्;

      प्रत्यागम *एतत्;
}

vvp_vector2_t& vvp_vector2_t::संक्रियकः -= (ध्रुव vvp_vector2_t&that)
{
      प्रतिज्ञा(wid_ == that.wid_);
      यदि (wid_ == 0)
	    प्रत्यागम *एतत्;

      ध्रुव अचिह्नित शब्दाः = (wid_ + द्वयङ्काः_प्रत्येकम्_WORD-1) / द्वयङ्काः_प्रत्येकम्_WORD;

      अचिह्नित दीर्घ carry = 1;
      प्रति (अचिह्नित idx = 0 ;  idx < शब्दाः ;  idx += 1) {
	    vec_[idx] = सङ्कलय_carry(vec_[idx], ~that.vec_[idx], carry);
      }

      प्रत्यागम *एतत्;
}

शून्य vvp_vector2_t::trim()
{
      यावत् (value(wid_-1) == 0 && wid_ > 1) wid_ -= 1;
}

/* This is a special trim that is used on numbers we know represent a
 * negative signed value (they came from a negative real value). */
शून्य vvp_vector2_t::trim_neg()
{
      यदि (value(wid_-1) == 1 && wid_ > 32) {
	    यावत् (value(wid_-2) == 1 && wid_ > 32) wid_ -= 1;
      }
}

पूर्णाङ्क vvp_vector2_t::value(अचिह्नित idx) ध्रुव
{
      यदि (idx >= wid_)
	    प्रत्यागम 0;

      ध्रुव अचिह्नित द्वयङ्काः_प्रत्येकम्_word = 8 * परिमाण(vec_[0]);
      अचिह्नित addr = idx/द्वयङ्काः_प्रत्येकम्_word;
      अचिह्नित आवरणम् = idx%द्वयङ्काः_प्रत्येकम्_word;

      यदि (vec_[addr] & (1UL<<आवरणम्))
	    प्रत्यागम 1;
      अन्यथा
	    प्रत्यागम 0;
}

शून्य vvp_vector2_t::समुच्चयः_bit(अचिह्नित idx, पूर्णाङ्क bit)
{
      प्रतिज्ञा(idx < wid_);

      ध्रुव अचिह्नित द्वयङ्काः_प्रत्येकम्_word = 8 * परिमाण(vec_[0]);
      अचिह्नित addr = idx/द्वयङ्काः_प्रत्येकम्_word;
      अचिह्नित दीर्घ आवरणम् = idx%द्वयङ्काः_प्रत्येकम्_word;

      यदि (bit)
	    vec_[addr] |= 1UL << आवरणम्;
      अन्यथा
	    vec_[addr] &= ~(1UL << आवरणम्);
}

शून्य vvp_vector2_t::समुच्चयः_vec(अचिह्नित adr, ध्रुव vvp_vector2_t&that)
{
      प्रतिज्ञा((adr + that.wid_) <= wid_);

      प्रति (अचिह्नित idx = 0 ; idx < that.wid_ ; idx += 1)
	    समुच्चयः_bit(adr+idx, that.value(idx));
}

तार्किक vvp_vector2_t::is_NaN() ध्रुव
{
      प्रत्यागम wid_ == 0;
}

तार्किक vvp_vector2_t::is_zero() ध्रुव
{
      ध्रुव अचिह्नित शब्दाः = (wid_ + द्वयङ्काः_प्रत्येकम्_WORD-1) / द्वयङ्काः_प्रत्येकम्_WORD;

      प्रति (अचिह्नित idx = 0; idx < शब्दाः; idx += 1) {
	    यदि (vec_[idx] == 0) अनुवर्त;
	    प्रत्यागम असत्य;
      }

      प्रत्यागम सत्य;
}

/*
 * Basic idea from "Introduction to Programming using SML" by
 * Michael R. Hansen and Hans Rischel page 261 and "Seminumerical
 * Algorithms, Third Edition" by Donald E. Knuth section 4.6.3.
 */
vvp_vector2_t घातः(ध्रुव vvp_vector2_t&x, vvp_vector2_t&y)
{
        /* If we have a zero exponent just return 1. */
      यदि (y == vvp_vector2_t(0L, 1)) {
	    प्रत्यागम vvp_vector2_t(1L, x.size());
      }

        /* Is the value odd? */
      यदि (y.value(0) == 1) {
	    y.समुच्चयः_bit(0, 0);  // A quick subtract by 1.
	    vvp_vector2_t res = x * घातः(x, y);
	    प्रत्यागम res;
      }

      y >>= 1;  // A fast divide by two. We know the LSB is zero.
      vvp_vector2_t z = घातः(x, y);
      vvp_vector2_t res = z * z;
      प्रत्यागम res;
}

स्थिर शून्य multiply_long(अचिह्नित दीर्घ a, अचिह्नित दीर्घ b,
			  अचिह्नित दीर्घ&नीचः, अचिह्नित दीर्घ&उच्चः)
{
      प्रतिज्ञा(परिमाण(अचिह्नित दीर्घ) %2 == 0);

      ध्रुव अचिह्नित दीर्घ word_आवरणम् = (1UL << 4UL*परिमाण(a)) - 1UL;
      अचिह्नित दीर्घ tmpa;
      अचिह्नित दीर्घ tmpb;
      अचिह्नित दीर्घ res[4];

      tmpa = a & word_आवरणम्;
      tmpb = b & word_आवरणम्;
      res[0] = tmpa * tmpb;
      res[1] = res[0] >> 4UL*परिमाण(अचिह्नित दीर्घ);
      res[0] &= word_आवरणम्;

      tmpa = (a >> 4UL*परिमाण(अचिह्नित दीर्घ)) & word_आवरणम्;
      res[1] += tmpa * tmpb;
      res[2] = res[1] >> 4UL*परिमाण(अचिह्नित दीर्घ);
      res[1] &= word_आवरणम्;

      tmpa = a & word_आवरणम्;
      tmpb = (b >> 4UL*परिमाण(अचिह्नित दीर्घ)) & word_आवरणम्;
      res[1] += tmpa * tmpb;
      res[2] += res[1] >> 4UL*परिमाण(अचिह्नित दीर्घ);
      res[3]  = res[2] >> 4UL*परिमाण(अचिह्नित दीर्घ);
      res[1] &= word_आवरणम्;
      res[2] &= word_आवरणम्;

      tmpa = (a >> 4UL*परिमाण(अचिह्नित दीर्घ)) & word_आवरणम्;
      res[2] += tmpa * tmpb;
      res[3] += res[2] >> 4UL*परिमाण(अचिह्नित दीर्घ);
      res[2] &= word_आवरणम्;

      उच्चः = (res[3] << 4UL*परिमाण(अचिह्नित दीर्घ)) | res[2];
      नीचः  = (res[1] << 4UL*परिमाण(अचिह्नित दीर्घ)) | res[0];
}

vvp_vector2_t संक्रियकः * (ध्रुव vvp_vector2_t&a, ध्रुव vvp_vector2_t&b)
{
      ध्रुव अचिह्नित द्वयङ्काः_प्रत्येकम्_word = 8 * परिमाण(a.vec_[0]);

	// The compiler ensures that the two operands are of equal size.
      प्रतिज्ञा(a.size() == b.size());
      vvp_vector2_t r (0, a.size());

      अचिह्नित शब्दाः = (r.wid_ + द्वयङ्काः_प्रत्येकम्_word - 1) / द्वयङ्काः_प्रत्येकम्_word;

      प्रति (अचिह्नित bdx = 0 ;  bdx < शब्दाः ;  bdx += 1) {
	    अचिह्नित दीर्घ tmpb = b.vec_[bdx];
	    यदि (tmpb == 0)
		  अनुवर्त;

	    प्रति (अचिह्नित adx = 0 ;  adx < शब्दाः ;  adx += 1) {
		  अचिह्नित दीर्घ tmpa = a.vec_[adx];
		  यदि (tmpa == 0)
			अनुवर्त;

		  अचिह्नित दीर्घ नीचः, hig;
		  multiply_long(tmpa, tmpb, नीचः, hig);

		  अचिह्नित दीर्घ carry = 0;
		  प्रति (अचिह्नित sdx = 0
			     ; (adx+bdx+sdx) < शब्दाः
			     ;  sdx += 1) {

			r.vec_[adx+bdx+sdx] = सङ्कलय_carry(r.vec_[adx+bdx+sdx],
							नीचः, carry);
			नीचः = hig;
			hig = 0;
		  }
	    }
      }

      प्रत्यागम r;
}

स्थिर शून्य div_mod (vvp_vector2_t dividend, vvp_vector2_t भाजकः,
		     vvp_vector2_t&quotient, vvp_vector2_t&remainder)
{

      quotient = vvp_vector2_t(0, dividend.size());

      यदि (भाजकः == quotient) {
	    cerr << "ERROR: division by zero, exiting." << पङ्क्त्यन्तः;
	    निर्गच्छ(255);
      }

      यदि (dividend < भाजकः) {
	    remainder = dividend;
	    प्रत्यागम;
      }

      vvp_vector2_t आवरणम् (1, dividend.size());

	// Make the dividend 1 bit larger to prevent overflow of
	// divtmp in startup.
      dividend = vvp_vector2_t(dividend, dividend.size()+1);
      vvp_vector2_t divtmp (भाजकः, dividend.size());

      यावत् (divtmp < dividend) {
	    divtmp <<= 1;
	    आवरणम् <<= 1;
      }

      यावत् (dividend >= भाजकः) {
	    यदि (divtmp <= dividend) {
		  dividend -= divtmp;
		  quotient += आवरणम्;
	    }

	    divtmp >>= 1;
	    आवरणम् >>= 1;
      }

      remainder = vvp_vector2_t(dividend, आवरणम्.size());
}

vvp_vector2_t संक्रियकः - (ध्रुव vvp_vector2_t&that)
{
      vvp_vector2_t neg(that);
      यदि (neg.wid_ == 0) प्रत्यागम neg;

      ध्रुव अचिह्नित शब्दाः = (neg.wid_ + neg.द्वयङ्काः_प्रत्येकम्_WORD-1) /
                                         neg.द्वयङ्काः_प्रत्येकम्_WORD;
      प्रति (अचिह्नित idx = 0 ;  idx < शब्दाः ;  idx += 1) {
	    neg.vec_[idx] = ~neg.vec_[idx];
      }
      neg += vvp_vector2_t(1, neg.wid_);

      प्रत्यागम neg;
}

vvp_vector2_t संक्रियकः / (ध्रुव vvp_vector2_t&dividend,
			  ध्रुव vvp_vector2_t&भाजकः)
{
      vvp_vector2_t quot, rem;
      div_mod(dividend, भाजकः, quot, rem);
      प्रत्यागम quot;
}

vvp_vector2_t संक्रियकः % (ध्रुव vvp_vector2_t&dividend,
			  ध्रुव vvp_vector2_t&भाजकः)
{
      vvp_vector2_t quot, rem;
      div_mod(dividend, भाजकः, quot, rem);
      प्रत्यागम rem;
}

तार्किक संक्रियकः > (ध्रुव vvp_vector2_t&a, ध्रुव vvp_vector2_t&b)
{
      ध्रुव अचिह्नित awords = (a.wid_ + vvp_vector2_t::द्वयङ्काः_प्रत्येकम्_WORD-1) / vvp_vector2_t::द्वयङ्काः_प्रत्येकम्_WORD;
      ध्रुव अचिह्नित bwords = (b.wid_ + vvp_vector2_t::द्वयङ्काः_प्रत्येकम्_WORD-1) / vvp_vector2_t::द्वयङ्काः_प्रत्येकम्_WORD;

      ध्रुव अचिह्नित शब्दाः = awords > bwords? awords : bwords;

      प्रति (अचिह्नित idx = शब्दाः ;  idx > 0 ;  idx -= 1) {
	    अचिह्नित दीर्घ aw = (idx <= awords)? a.vec_[idx-1] : 0;
	    अचिह्नित दीर्घ bw = (idx <= bwords)? b.vec_[idx-1] : 0;

	    यदि (aw > bw)
		  प्रत्यागम सत्य;
	    यदि (aw < bw)
		  प्रत्यागम असत्य;
      }

	// If the above loop finishes, then the vectors are equal.
      प्रत्यागम असत्य;
}

तार्किक संक्रियकः >= (ध्रुव vvp_vector2_t&a, ध्रुव vvp_vector2_t&b)
{
      ध्रुव अचिह्नित awords = (a.wid_ + vvp_vector2_t::द्वयङ्काः_प्रत्येकम्_WORD-1) / vvp_vector2_t::द्वयङ्काः_प्रत्येकम्_WORD;
      ध्रुव अचिह्नित bwords = (b.wid_ + vvp_vector2_t::द्वयङ्काः_प्रत्येकम्_WORD-1) / vvp_vector2_t::द्वयङ्काः_प्रत्येकम्_WORD;

      ध्रुव अचिह्नित शब्दाः = awords > bwords? awords : bwords;

      प्रति (अचिह्नित idx = शब्दाः ;  idx > 0 ;  idx -= 1) {
	    अचिह्नित दीर्घ aw = (idx <= awords)? a.vec_[idx-1] : 0;
	    अचिह्नित दीर्घ bw = (idx <= bwords)? b.vec_[idx-1] : 0;

	    यदि (aw > bw)
		  प्रत्यागम सत्य;
	    यदि (aw < bw)
		  प्रत्यागम असत्य;
      }

	// If the above loop finishes, then the vectors are equal.
      प्रत्यागम सत्य;
}

तार्किक संक्रियकः < (ध्रुव vvp_vector2_t&a, ध्रुव vvp_vector2_t&b)
{
      ध्रुव अचिह्नित awords = (a.wid_ + vvp_vector2_t::द्वयङ्काः_प्रत्येकम्_WORD-1) / vvp_vector2_t::द्वयङ्काः_प्रत्येकम्_WORD;
      ध्रुव अचिह्नित bwords = (b.wid_ + vvp_vector2_t::द्वयङ्काः_प्रत्येकम्_WORD-1) / vvp_vector2_t::द्वयङ्काः_प्रत्येकम्_WORD;

      अचिह्नित शब्दाः = awords;
      यदि (bwords > शब्दाः)
	    शब्दाः = bwords;

      प्रति (अचिह्नित idx = शब्दाः ;  idx > 0 ;  idx -= 1) {
	    अचिह्नित दीर्घ aw = (idx <= awords)? a.vec_[idx-1] : 0;
	    अचिह्नित दीर्घ bw = (idx <= bwords)? b.vec_[idx-1] : 0;

	    यदि (aw < bw)
		  प्रत्यागम सत्य;
	    यदि (aw > bw)
		  प्रत्यागम असत्य;
      }

	// If the above loop finishes, then the vectors are equal.
      प्रत्यागम असत्य;
}

तार्किक संक्रियकः <= (ध्रुव vvp_vector2_t&a, ध्रुव vvp_vector2_t&b)
{
	// XXXX For now, only support equal width vectors.
      प्रतिज्ञा(a.wid_ == b.wid_);

      ध्रुव अचिह्नित awords = (a.wid_ + vvp_vector2_t::द्वयङ्काः_प्रत्येकम्_WORD-1) / vvp_vector2_t::द्वयङ्काः_प्रत्येकम्_WORD;

      प्रति (अचिह्नित idx = awords ;  idx > 0 ;  idx -= 1) {
	    यदि (a.vec_[idx-1] < b.vec_[idx-1])
		  प्रत्यागम सत्य;
	    यदि (a.vec_[idx-1] > b.vec_[idx-1])
		  प्रत्यागम असत्य;
      }

	// If the above loop finishes, then the vectors are equal.
      प्रत्यागम सत्य;
}

तार्किक संक्रियकः == (ध्रुव vvp_vector2_t&a, ध्रुव vvp_vector2_t&b)
{
      ध्रुव अचिह्नित awords = (a.wid_ + vvp_vector2_t::द्वयङ्काः_प्रत्येकम्_WORD-1) / vvp_vector2_t::द्वयङ्काः_प्रत्येकम्_WORD;
      ध्रुव अचिह्नित bwords = (b.wid_ + vvp_vector2_t::द्वयङ्काः_प्रत्येकम्_WORD-1) / vvp_vector2_t::द्वयङ्काः_प्रत्येकम्_WORD;

      ध्रुव अचिह्नित शब्दाः = awords > bwords? awords : bwords;

      प्रति (अचिह्नित idx = शब्दाः ;  idx > 0 ;  idx -= 1) {
	    अचिह्नित दीर्घ aw = (idx <= awords)? a.vec_[idx-1] : 0;
	    अचिह्नित दीर्घ bw = (idx <= bwords)? b.vec_[idx-1] : 0;

	    यदि (aw > bw)
		  प्रत्यागम असत्य;
	    यदि (aw < bw)
		  प्रत्यागम असत्य;
      }

	// If the above loop finishes, then the vectors are equal.
      प्रत्यागम सत्य;
}


vvp_vector4_t vector2_गन्तव्यम्_vector4(ध्रुव vvp_vector2_t&that, अचिह्नित wid)
{
      vvp_vector4_t res (wid);

      प्रति (अचिह्नित idx = 0 ;  idx < res.size() ;  idx += 1) {
	    vvp_bit4_t bit = BIT4_0;

	    यदि (that.value(idx))
		  bit = BIT4_1;

	    res.समुच्चयः_bit(idx, bit);
      }

      प्रत्यागम res;
}

स्थिर तार्किक c4string_शीर्षकम्_test(ध्रुव वर्ण*str)
{
      यदि ((str[0] != 'C') && (str[0] != 'c'))
	    प्रत्यागम असत्य;
      यदि ((str[1] != '4') || (str[2] != '<'))
	    प्रत्यागम असत्य;
      प्रत्यागम सत्य;
}

तार्किक c4string_test(ध्रुव वर्ण*str)
{
      यदि (!c4string_शीर्षकम्_test(str))
	    प्रत्यागम असत्य;
      size_t मानम्_आकार = strspn(str+3, "01xz");
      यदि (str[3+मानम्_आकार] != '>')
	    प्रत्यागम असत्य;
      यदि (str[3+मानम्_आकार+1] != 0)
	    प्रत्यागम असत्य;

      प्रत्यागम सत्य;
}

vvp_vector4_t c4string_गन्तव्यम्_vector4(ध्रुव वर्ण*str)
{
      प्रतिज्ञा(c4string_शीर्षकम्_test(str));

      str += 3;
      ध्रुव वर्ण*tp = str + strspn(str,"01xz");
      प्रतिज्ञा(tp[0] == '>');

      vvp_vector4_t tmp (tp-str);

      प्रति (अचिह्नित idx = 0 ;  idx < tmp.size() ;  idx += 1) {
	    vvp_bit4_t bit;
	    विकल्प (str[idx]) {
		पक्ष '0':
		  bit = BIT4_0;
		  भङ्ग;
		पक्ष '1':
		  bit = BIT4_1;
		  भङ्ग;
		पक्ष 'x':
		  bit = BIT4_X;
		  भङ्ग;
		पक्ष 'z':
		  bit = BIT4_Z;
		  भङ्ग;
		स्वभाव:
		  fprintf(stderr, "Unsupported bit value %c(%d).\n", str[idx],
		          str[idx]);
		  प्रतिज्ञा(0);
		  bit = BIT4_0;
		  भङ्ग;
	    }
	    tmp.समुच्चयः_bit(tmp.size()-idx-1, bit);
      }

      प्रत्यागम tmp;
}

ostream& संक्रियकः<< (ostream&out, ध्रुव vvp_vector2_t&that)
{
      यदि (that.is_NaN()) {
	    out << "NaN";

      } अन्यथा {
	    out << vector2_गन्तव्यम्_vector4(that, that.size());
      }
      प्रत्यागम out;
}

vvp_vector8_t::vvp_vector8_t(ध्रुव vvp_vector8_t&that)
{
      आकार_2 = that.आकार_2;
      यदि (आकार_2 <= परिमाण(val_)) {
	    स्मृतिं_प्रतिलिख(val_, that.val_, परिमाण val_);
      } अन्यथा {
	    ptr_ = नूतन अचिह्नित वर्ण[आकार_2];
	    स्मृतिं_प्रतिलिख(ptr_, that.ptr_, आकार_2);
      }
}

vvp_vector8_t::vvp_vector8_t(ध्रुव vvp_vector4_t&that,
			     अचिह्नित str0, अचिह्नित str1)
: आकार_2(that.size())
{
      यदि (आकार_2 == 0)
	    प्रत्यागम;

      यदि (आकार_2 <= परिमाण(val_)) {
	    ptr_ = 0; // Prefill all val_ bytes
	    प्रति (अचिह्नित idx = 0 ; idx < आकार_2 ; idx += 1)
		  val_[idx] = vvp_scalar_t(that.value(idx),str0, str1).raw();
      } अन्यथा {
	    ptr_ = नूतन अचिह्नित वर्ण[आकार_2];
	    प्रति (अचिह्नित idx = 0 ;  idx < आकार_2 ;  idx += 1)
		  ptr_[idx] = vvp_scalar_t(that.value(idx), str0, str1).raw();
      }
}

vvp_vector8_t::vvp_vector8_t(ध्रुव vvp_vector2_t&that,
			     अचिह्नित str0, अचिह्नित str1)
: आकार_2(that.size())
{
      यदि (आकार_2 == 0)
	    प्रत्यागम;

      यदि (आकार_2 <= परिमाण(val_)) {
	    ptr_ = 0;
	    प्रति (अचिह्नित idx = 0 ; idx < आकार_2 ; idx += 1)
		  val_[idx] = vvp_scalar_t(that.value(idx)? BIT4_1:BIT4_0, str0, str1).raw();
      } अन्यथा {
	    ptr_ = नूतन अचिह्नित वर्ण[आकार_2];
	    प्रति (अचिह्नित idx = 0 ; idx < आकार_2 ; idx += 1)
		  ptr_[idx] = vvp_scalar_t(that.value(idx)? BIT4_1:BIT4_0, str0, str1).raw();
      }
}

ध्रुव vvp_vector8_t vvp_vector8_t::nil;

vvp_vector8_t& vvp_vector8_t::संक्रियकः= (ध्रुव vvp_vector8_t&that)
{
	// Assign to self.
      यदि (एतत् == &that)
	    प्रत्यागम *एतत्;

      यदि (आकार_2 != that.आकार_2) {
	    यदि (आकार_2 > परिमाण(val_))
		  विलोपय[]ptr_;
	    आकार_2 = 0;
      }

      यदि (that.आकार_2 == 0) {
	    प्रतिज्ञा(आकार_2 == 0);
	    प्रत्यागम *एतत्;
      }

      यदि (that.आकार_2 <= परिमाण(val_)) {
	    आकार_2 = that.आकार_2;
	    स्मृतिं_प्रतिलिख(val_, that.val_, परिमाण(val_));
	    प्रत्यागम *एतत्;
      }

      यदि (आकार_2 == 0) {
	    आकार_2 = that.आकार_2;
	    ptr_ = नूतन अचिह्नित वर्ण[आकार_2];
      }

      स्मृतिं_प्रतिलिख(ptr_, that.ptr_, आकार_2);

      प्रत्यागम *एतत्;
}

vvp_vector8_t vvp_vector8_t::subvalue(अचिह्नित आधारः, अचिह्नित wid) ध्रुव
{
      vvp_vector8_t tmp (wid);

      अचिह्नित वर्ण*tmp_ptr = tmp.आकार_2 <= परिमाण(val_) ? tmp.val_ : tmp.ptr_;
      ध्रुव अचिह्नित वर्ण*use_ptr = आकार_2 <= परिमाण(val_) ? val_ : ptr_;

      अचिह्नित idx = 0;
      यावत् ((idx < wid) && (आधारः+idx < आकार_2)) {
	    tmp_ptr[idx] = use_ptr[आधारः+idx];
	    idx += 1;
      }

      प्रत्यागम tmp;
}

शून्य vvp_vector8_t::समुच्चयः_vec(अचिह्नित आधारः, ध्रुव vvp_vector8_t&that)
{
      प्रतिज्ञा((आधारः+that.size()) <= size());
      प्रति (अचिह्नित idx = 0 ; idx < that.size() ; idx += 1)
	    समुच्चयः_bit(आधारः+idx, that.value(idx));
}

vvp_vector8_t part_expand(ध्रुव vvp_vector8_t&that, अचिह्नित wid, अचिह्नित off)
{
      प्रतिज्ञा(off < wid);
      vvp_vector8_t tmp (wid);

      अचिह्नित वर्ण* tmp_ptr = tmp.आकार_2<= परिमाण(tmp.val_) ?
                               tmp.val_ : tmp.ptr_;
      ध्रुव अचिह्नित वर्ण* that_ptr = that.आकार_2<= परिमाण(that.val_) ?
                                      that.val_ : that.ptr_;

      अचिह्नित idx = off;

      यावत् (idx < wid && that.आकार_2 > (idx-off)) {
	    tmp_ptr[idx] = that_ptr[idx-off];
	    idx += 1;
      }

      प्रत्यागम tmp;
}

स्थिर तार्किक c8string_शीर्षकम्_test(ध्रुव वर्ण*str)
{
      यदि ((str[0] != 'C') && (str[0] != 'c'))
	    प्रत्यागम असत्य;
      यदि ((str[1] != '8') || (str[2] != '<'))
	    प्रत्यागम असत्य;
      प्रत्यागम सत्य;
}

तार्किक c8string_test(ध्रुव वर्ण*str)
{
      यदि (!c8string_शीर्षकम्_test(str))
	    प्रत्यागम असत्य;

      ध्रुव वर्ण*cp = str+3;
      प्रति (;; cp += 1) {
	    यदि (cp[0] == '>' && cp[1] == 0) प्रत्यागम सत्य;
	    यदि (cp[0] >= '0' && cp[0] <= '9') अनुवर्त;
	    यदि (cp[0] == 'x') अनुवर्त;
	    यदि (cp[0] == 'z') अनुवर्त;
	    प्रत्यागम असत्य;
      }
      प्रत्यागम असत्य;
}
/*
 * The format of a C8<> string is:
 *   C8<aaabbbccc...>
 * where aaa... is a 3 character bit descriptor.
 */
vvp_vector8_t c8string_गन्तव्यम्_vector8(ध्रुव वर्ण*str)
{
      प्रतिज्ञा(c8string_शीर्षकम्_test(str));

      size_t vsize = वाक्यदैर्घ्यम्(str)-4;
      प्रतिज्ञा(vsize%3 == 0);
      vsize /= 3;
      vvp_vector8_t tmp (vsize);

      प्रति (size_t idx = 0 ; idx < vsize ; idx += 1) {
	    ध्रुव वर्ण*cp = str+3+3*idx;
	    vvp_bit4_t bit = BIT4_X;
	    अचिह्नित dr0 = cp[0]-'0';
	    अचिह्नित dr1 = cp[1]-'0';
	    विकल्प (cp[2]) {
		पक्ष '0':
		  bit = BIT4_0;
		  भङ्ग;
		पक्ष '1':
		  bit = BIT4_1;
		  भङ्ग;
		पक्ष 'x':
		  bit = BIT4_X;
		  भङ्ग;
		पक्ष 'z':
		  bit = BIT4_Z;
		  भङ्ग;
	    }
	    tmp.समुच्चयः_bit(vsize-idx-1, vvp_scalar_t(bit, dr0, dr1));
      }

      प्रत्यागम tmp;
}

ostream& संक्रियकः<<(ostream&out, ध्रुव vvp_vector8_t&that)
{
      out << "C8<";
      प्रति (अचिह्नित idx = 0 ;  idx < that.size() ; idx += 1)
	    out << that.value(that.size()-idx-1);

      out << ">";
      प्रत्यागम out;
}

vvp_net_fun_t::vvp_net_fun_t()
{
      गणनम्_functors += 1;
}

vvp_net_fun_t::~vvp_net_fun_t()
{
}

शून्य vvp_net_fun_t::recv_vec4(vvp_net_ptr_t, ध्रुव vvp_vector4_t&,
                              vvp_context_t)
{
      fprintf(stderr, "internal error: %s: recv_vec4 not implemented\n",
	      typeid(*एतत्).name());
      प्रतिज्ञा(0);
}

शून्य vvp_net_fun_t::recv_vec4_pv_(vvp_net_ptr_t p, ध्रुव vvp_vector4_t&bit,
                                  अचिह्नित आधारः, अचिह्नित vwid, vvp_context_t)
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

      प्रतिज्ञा(आधारः + bit.size() <= vwid);

      vvp_vector4_t tmp(vwid, BIT4_Z);
      tmp.समुच्चयः_vec(आधारः, bit);
      recv_vec4(p, tmp, 0);
}

शून्य vvp_net_fun_t::recv_vec4_pv(vvp_net_ptr_t, ध्रुव vvp_vector4_t&bit,
                                 अचिह्नित आधारः, अचिह्नित vwid, vvp_context_t)
{
      cerr << "internal error: " << typeid(*एतत्).name() << ": "
	   << "recv_vec4_pv(" << bit << ", " << आधारः
	   << ", " << vwid << ") not implemented" << पङ्क्त्यन्तः;
      प्रतिज्ञा(0);
}

शून्य vvp_net_fun_t::recv_vec8(vvp_net_ptr_t पत्तनम्, ध्रुव vvp_vector8_t&bit)
{
      recv_vec4(पत्तनम्, reduce4(bit), 0);
}

शून्य vvp_net_fun_t::recv_vec8_pv_(vvp_net_ptr_t p, ध्रुव vvp_vector8_t&bit,
				  अचिह्नित आधारः, अचिह्नित vwid)
{
	// This is the strength-aware version of recv_vec4_pv_.

      प्रतिज्ञा(आधारः + bit.size() <= vwid);

      vvp_vector8_t tmp(vwid);
      tmp.समुच्चयः_vec(आधारः, bit);
      recv_vec8(p, tmp);
}

शून्य vvp_net_fun_t::recv_vec8_pv(vvp_net_ptr_t पत्तनम्, ध्रुव vvp_vector8_t&bit,
				 अचिह्नित आधारः, अचिह्नित vwid)
{
      recv_vec4_pv(पत्तनम्, reduce4(bit), आधारः, vwid, 0);
}

शून्य vvp_net_fun_t::recv_real(vvp_net_ptr_t, द्विगुण bit, vvp_context_t)
{
      fprintf(stderr, "internal error: %s: recv_real(%f) not implemented\n",
	      typeid(*एतत्).name(), bit);
      प्रतिज्ञा(0);
}

शून्य vvp_net_fun_t::recv_string(vvp_net_ptr_t, ध्रुव std::string&bit, vvp_context_t)
{
      fprintf(stderr, "internal error: %s: recv_string(%s) not implemented\n",
	      typeid(*एतत्).name(), bit.c_str());
      प्रतिज्ञा(0);
}

शून्य vvp_net_fun_t::recv_object(vvp_net_ptr_t, vvp_object_t, vvp_context_t)
{
      fprintf(stderr, "internal error: %s: recv_object(...) not implemented\n",
	      typeid(*एतत्).name());
      प्रतिज्ञा(0);
}

शून्य vvp_net_fun_t::force_flag(तार्किक)
{
}

/* **** vvp_fun_drive methods **** */

vvp_fun_drive::vvp_fun_drive(अचिह्नित str0, अचिह्नित str1)
{
      प्रतिज्ञा(str0 < 8);
      प्रतिज्ञा(str1 < 8);

      drive0_ = str0;
      drive1_ = str1;
}

vvp_fun_drive::~vvp_fun_drive()
{
}

शून्य vvp_fun_drive::recv_vec4(vvp_net_ptr_t पत्तनम्, ध्रुव vvp_vector4_t&bit,
                              vvp_context_t)
{
      प्रतिज्ञा(पत्तनम्.पत्तनम्() == 0);
      पत्तनम्.ptr()->send_vec8(vvp_vector8_t(bit, drive0_, drive1_));
}

शून्य vvp_fun_drive::recv_vec4_pv(vvp_net_ptr_t ptr, ध्रुव vvp_vector4_t&bit,
				 अचिह्नित आधारः, अचिह्नित vwid, vvp_context_t ctx)
{
      recv_vec4_pv_(ptr, bit, आधारः, vwid, ctx);
}

/* **** vvp_wide_fun_* methods **** */

vvp_wide_fun_core::vvp_wide_fun_core(vvp_net_t*net, अचिह्नित nports)
{
      ptr_ = net;
      nports_ = nports;
      पत्तनम्_मानानि = 0;
      पत्तनम्_rvalues = 0;
}

vvp_wide_fun_core::~vvp_wide_fun_core()
{
      विलोपय[] पत्तनम्_मानानि;
      विलोपय[] पत्तनम्_rvalues;
}

शून्य vvp_wide_fun_core::propagate_vec4(ध्रुव vvp_vector4_t&bit,
				       vvp_time64_t delay)
{
      यदि (delay)
	    कालसारणी_propagate_सदिशम्(ptr_, delay, bit);
      अन्यथा
	    ptr_->send_vec4(bit, 0);
}

शून्य vvp_wide_fun_core::propagate_real(द्विगुण bit,
				       vvp_time64_t delay)
{
      यदि (delay) {
	      // schedule_assign_vector(ptr_->out, bit, delay);
	    प्रतिज्ञा(0); // Need a real-value version of assign_vector.
      } अन्यथा {
	    ptr_->send_real(bit, 0);
      }
}


अचिह्नित vvp_wide_fun_core::पत्तनम्_गणनम्() ध्रुव
{
      प्रत्यागम nports_;
}

vvp_vector4_t& vvp_wide_fun_core::value(अचिह्नित idx)
{
      प्रतिज्ञा(idx < nports_);
      यदि (पत्तनम्_मानानि == 0)
	    पत्तनम्_मानानि = नूतन vvp_vector4_t [nports_];
      प्रत्यागम पत्तनम्_मानानि[idx];
}

द्विगुण vvp_wide_fun_core::मानम्_r(अचिह्नित idx)
{
      प्रतिज्ञा(idx < nports_);
      प्रत्यागम पत्तनम्_rvalues? पत्तनम्_rvalues[idx] : 0.0;
}

शून्य vvp_wide_fun_core::recv_real_from_inputs(अचिह्नित)
{
      प्रतिज्ञा(0);
}

शून्य vvp_wide_fun_core::dispatch_vec4_from_निवेशः(अचिह्नित पत्तनम्,
						  ध्रुव vvp_vector4_t&bit)
{
      प्रतिज्ञा(पत्तनम् < nports_);
      यदि (पत्तनम्_मानानि == 0) पत्तनम्_मानानि = नूतन vvp_vector4_t [nports_];
      पत्तनम्_मानानि[पत्तनम्] = bit;
      recv_vec4_from_inputs(पत्तनम्);
}

शून्य vvp_wide_fun_core::dispatch_real_from_निवेशः(अचिह्नित पत्तनम्,
						  द्विगुण bit)
{
      प्रतिज्ञा(पत्तनम् < nports_);
      यदि (पत्तनम्_rvalues == 0) पत्तनम्_rvalues = नूतन द्विगुण[nports_];
      पत्तनम्_rvalues[पत्तनम्] = bit;
      recv_real_from_inputs(पत्तनम्);
}

vvp_wide_fun_t::vvp_wide_fun_t(vvp_wide_fun_core*c, अचिह्नित आधारः)
: core_(c), पत्तनम्_आधारः_2(आधारः)
{
}

vvp_wide_fun_t::~vvp_wide_fun_t()
{
}

शून्य vvp_wide_fun_t::recv_vec4(vvp_net_ptr_t पत्तनम्, ध्रुव vvp_vector4_t&bit,
                               vvp_context_t)
{
      अचिह्नित pidx = पत्तनम्_आधारः_2 + पत्तनम्.पत्तनम्();
      core_->dispatch_vec4_from_निवेशः(pidx, bit);
}

शून्य vvp_wide_fun_t::recv_vec4_pv(vvp_net_ptr_t ptr, ध्रुव vvp_vector4_t&bit,
				  अचिह्नित आधारः, अचिह्नित vwid, vvp_context_t ctx)
{
      recv_vec4_pv_(ptr, bit, आधारः, vwid, ctx);
}

शून्य vvp_wide_fun_t::recv_real(vvp_net_ptr_t पत्तनम्, द्विगुण bit,
                               vvp_context_t)
{
      अचिह्नित pidx = पत्तनम्_आधारः_2 + पत्तनम्.पत्तनम्();
      core_->dispatch_real_from_निवेशः(pidx, bit);
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

अचिह्नित vvp_scalar_t::strength0() ध्रुव
{
      प्रत्यागम STREN0(मानम्_2);
}

अचिह्नित vvp_scalar_t::strength1() ध्रुव
{
      प्रत्यागम STREN1(मानम्_2);
}

ostream& संक्रियकः <<(ostream&out, vvp_scalar_t a)
{
      out << a.strength0() << a.strength1();
      विकल्प (a.value()) {
	  पक्ष BIT4_0:
	    out << "0";
	    भङ्ग;
	  पक्ष BIT4_1:
	    out << "1";
	    भङ्ग;
	  पक्ष BIT4_X:
	    out << "X";
	    भङ्ग;
	  पक्ष BIT4_Z:
	    out << "Z";
	    भङ्ग;
      }
      प्रत्यागम out;
}

/*
 * This function is only called if the actual interface function rules
 * out some of the easy cases. If we get here, we can assume that
 * neither of the values is HiZ, and the values are not exactly equal.
 */
vvp_scalar_t fully_featured_resolv_(vvp_scalar_t a, vvp_scalar_t b)
{

      यदि (UNAMBIG(a.मानम्_2) && UNAMBIG(b.मानम्_2)) {

	      /* If both signals are unambiguous, simply choose
		 the stronger. If they have the same strength
		 but different values, then this becomes
		 ambiguous. */

	    यदि ((b.मानम्_2&0x07) > (a.मानम्_2&0x07)) {

		    /* b value is stronger. Take it. */
		  प्रत्यागम b;

	    } अन्यथा यदि ((b.मानम्_2&0x77) == (a.मानम्_2&0x77)) {

		    // Strengths are the same. Since we know already
		    // that the values are not the same, Make value
		    // into "x".
		  vvp_scalar_t tmp (a);
		  tmp.मानम्_2 = (tmp.मानम्_2&0x77) | 0x80;
		  प्रत्यागम tmp;

	    } अन्यथा {

		    /* Must be "a" is the stronger one. */
		  प्रत्यागम a;
	    }

      }

	/* If one of the signals is unambiguous, then it
	   will sweep up the weaker parts of the ambiguous
	   signal. The result may be ambiguous, or maybe not. */

      यदि (UNAMBIG(a.मानम्_2)) {
	    vvp_scalar_t res;

	    यदि ((a.मानम्_2&0x70) > (b.मानम्_2&0x70))
		  res.मानम्_2 |= a.मानम्_2&0xf0;
	    अन्यथा
		  res.मानम्_2 |= b.मानम्_2&0xf0;

	    यदि ((a.मानम्_2&0x07) > (b.मानम्_2&0x07))
		  res.मानम्_2 |= a.मानम्_2&0x0f;
	    अन्यथा
		  res.मानम्_2 |= b.मानम्_2&0x0f;

	    प्रत्यागम res;

      } अन्यथा यदि (UNAMBIG(b.मानम्_2)) {

	    vvp_scalar_t res;

	    यदि ((b.मानम्_2&0x70) > (a.मानम्_2&0x70))
		  res.मानम्_2 |= b.मानम्_2&0xf0;
	    अन्यथा
		  res.मानम्_2 |= a.मानम्_2&0xf0;

	    यदि ((b.मानम्_2&0x07) > (a.मानम्_2&0x07))
		  res.मानम्_2 |= b.मानम्_2&0x0f;
	    अन्यथा
		  res.मानम्_2 |= a.मानम्_2&0x0f;

	    प्रत्यागम res;

      }


	/* If both signals are ambiguous, then the result
	   has an even wider ambiguity. */

      अचिह्नित tmp = 0;
      पूर्णाङ्क sv1a = (a.मानम्_2 & 0x80) ? STREN1(a.मानम्_2) : - STREN1(a.मानम्_2);
      पूर्णाङ्क sv0a = (a.मानम्_2 & 0x08) ? STREN0(a.मानम्_2) : - STREN0(a.मानम्_2);
      पूर्णाङ्क sv1b = (b.मानम्_2 & 0x80) ? STREN1(b.मानम्_2) : - STREN1(b.मानम्_2);
      पूर्णाङ्क sv0b = (b.मानम्_2 & 0x08) ? STREN0(b.मानम्_2) : - STREN0(b.मानम्_2);

      पूर्णाङ्क sv1 = sv1a;
      पूर्णाङ्क sv0 = sv0a;

      यदि (sv0a > sv1)
	    sv1 = sv0a;
      यदि (sv1b > sv1)
	    sv1 = sv1b;
      यदि (sv0b > sv1)
	    sv1 = sv0b;

      यदि (sv1a < sv0)
	    sv0 = sv1a;
      यदि (sv1b < sv0)
	    sv0 = sv1b;
      यदि (sv0b < sv0)
	    sv0 = sv0b;

      यदि (sv1 > 0) {
	    tmp |= 0x80;
	    tmp |= sv1 << 4;
      } अन्यथा {
	      /* Set the MSB when both arguments MSBs are set. This
		 can only happen if both one strengths are zero. */
	    tmp |= (a.मानम्_2&b.मानम्_2)&0x80;
	    tmp |= (-sv1) << 4;
      }

      यदि (sv0 > 0) {
	    tmp |= 0x08;
	    tmp |= sv0;
      } अन्यथा {
	    tmp |= (-sv0);
      }

      vvp_scalar_t res;
      res.मानम्_2 = tmp;

	/* Canonicalize the HiZ value. */
      यदि ((res.मानम्_2&0x77) == 0)
	    res.मानम्_2 = 0;

      प्रत्यागम res;
}

अचिह्नित vvp_switch_strength_map[2][8] = {
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

vvp_vector4_t reduce4(ध्रुव vvp_vector8_t&that)
{
      vvp_vector4_t out (that.size());
      प्रति (अचिह्नित idx = 0 ;  idx < out.size() ;  idx += 1)
	    out.समुच्चयः_bit(idx, that.value(idx).value());

      प्रत्यागम out;
}

vvp_bit4_t compare_gtge(ध्रुव vvp_vector4_t&lef, ध्रुव vvp_vector4_t&rig,
			vvp_bit4_t बहिर्गमनम्_if_equal)
{
      अचिह्नित min_आकार = lef.size();
      यदि (rig.size() < min_आकार)
	    min_आकार = rig.size();

	// If one of the inputs is nil, treat is as all X values, and
	// that makes the result BIT4_X.
      यदि (min_आकार == 0)
	    प्रत्यागम BIT4_X;

	// As per the IEEE1364 definition of >, >=, < and <=, if there
	// are any X or Z values in either of the operand vectors,
	// then the result of the compare is BIT4_X.

	// Check for X/Z in the left operand
      यदि (lef.has_xz())
	    प्रत्यागम BIT4_X;

	// Check for X/Z in the right operand
      यदि (rig.has_xz())
	    प्रत्यागम BIT4_X;

      प्रति (अचिह्नित idx = lef.size() ; idx > rig.size() ;  idx -= 1) {
	    यदि (lef.value(idx-1) == BIT4_1)
		  प्रत्यागम BIT4_1;
      }

      प्रति (अचिह्नित idx = rig.size() ; idx > lef.size() ;  idx -= 1) {
	    यदि (rig.value(idx-1) == BIT4_1)
		  प्रत्यागम BIT4_0;
      }

      प्रति (अचिह्नित idx = min_आकार ; idx > 0 ;  idx -= 1) {
	    vvp_bit4_t lv = lef.value(idx-1);
	    vvp_bit4_t rv = rig.value(idx-1);

	    यदि (lv == rv)
		  अनुवर्त;

	    यदि (lv == BIT4_1)
		  प्रत्यागम BIT4_1;
	    अन्यथा
		  प्रत्यागम BIT4_0;
      }

      प्रत्यागम बहिर्गमनम्_if_equal;
}

vvp_bit4_t compare_gtge_signed(ध्रुव vvp_vector4_t&a,
			       ध्रुव vvp_vector4_t&b,
			       vvp_bit4_t बहिर्गमनम्_if_equal)
{
      प्रतिज्ञा(a.size() == b.size());

      अचिह्नित sign_idx = a.size()-1;
      vvp_bit4_t a_sign = a.value(sign_idx);
      vvp_bit4_t b_sign = b.value(sign_idx);

      यदि (bit4_is_xz(a_sign))
	    प्रत्यागम BIT4_X;
      यदि (bit4_is_xz(b_sign))
	    प्रत्यागम BIT4_X;

      यदि (a_sign == b_sign)
	    प्रत्यागम compare_gtge(a, b, बहिर्गमनम्_if_equal);

      यदि (a.has_xz())
	    प्रत्यागम BIT4_X;

      यदि (b.has_xz())
	    प्रत्यागम BIT4_X;

      यदि(a_sign == BIT4_0)
	    प्रत्यागम BIT4_1;
      अन्यथा
	    प्रत्यागम BIT4_0;
}
