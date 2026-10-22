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
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ CHECK_WITH_VALGRIND
# include  <valgrind/memcheck.h>
# include  <map>
# include  "sfunc.h"
# include  "udp.h"
# include  "ivl_alloc.h"
#ਜੇ_ਅੰਤ

ਵਰਤੋਂ ਨਾਮਾਕਾਸ਼ std;

/* This is the size of an unsigned long in bits. This is just a
   convenience macro. */
# define CPU_ਸ਼ਬਦ_ਬਿੱਟ (8*ਆਕਾਰ(ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ))
# define ਸਿਖਰ_ਦੋਆਧਾਰੀ_ਅੰਕ (1UL << (CPU_ਸ਼ਬਦ_ਬਿੱਟ-1))

permaheap vvp_net_fun_t::ਢੇਰ;
permaheap vvp_net_fil_t::ਢੇਰ;

// Allocate around 1Megabyte/chunk.
ਸਥਿਰ ਅਚਲ size_t VVP_NET_CHUNK = 1024*1024/ਆਕਾਰ(vvp_net_t);
ਸਥਿਰ vvp_net_t*vvp_net_alloc_ਸਾਰਣੀ = ਨਿਰਾ;
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ CHECK_WITH_VALGRIND
ਸਥਿਰ vvp_net_t **vvp_net_pool = ਨਿਰਾ;
ਸਥਿਰ ਬਿਨਾਨਿਸ਼ਾਨੀ vvp_net_pool_ਗਿਣਤੀ = 0;
#ਜੇ_ਅੰਤ
ਸਥਿਰ size_t vvp_net_alloc_ਬਚਿਆ = 0;
// For statistics, count the vvp_nets allocated and the bytes of alloc
// chunks allocated.
ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ ਗਿਣਤੀ_vvp_nets = 0;
size_t ਮਾਪ_vvp_nets = 0;

ਖਾਲੀ* vvp_net_t::ਸੰਚਾਲਕ ਨਵਾਂ (size_t size)
{
      ਪ੍ਰਤਿਜ੍ਞਾ(size == ਆਕਾਰ(vvp_net_t));
      ਜੇ (vvp_net_alloc_ਬਚਿਆ == 0) {
	    vvp_net_alloc_ਸਾਰਣੀ = ::ਨਵਾਂ vvp_net_t[VVP_NET_CHUNK];
	    vvp_net_alloc_ਬਚਿਆ = VVP_NET_CHUNK;
	    ਮਾਪ_vvp_nets += size*VVP_NET_CHUNK;
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ CHECK_WITH_VALGRIND
	    VALGRIND_MAKE_MEM_NOACCESS(vvp_net_alloc_ਸਾਰਣੀ, size*VVP_NET_CHUNK);
	    VALGRIND_CREATE_MEMPOOL(vvp_net_alloc_ਸਾਰਣੀ, 0, 0);
	    vvp_net_pool_ਗਿਣਤੀ += 1;
	    vvp_net_pool = static_cast<vvp_net_t **>(realloc(vvp_net_pool,
	                   vvp_net_pool_ਗਿਣਤੀ*ਆਕਾਰ(vvp_net_t **)));
	    vvp_net_pool[vvp_net_pool_ਗਿਣਤੀ-1] = vvp_net_alloc_ਸਾਰਣੀ;
#ਜੇ_ਅੰਤ
      }

      vvp_net_t*return_this = vvp_net_alloc_ਸਾਰਣੀ;
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ CHECK_WITH_VALGRIND
      VALGRIND_MEMPOOL_ALLOC(vvp_net_pool[vvp_net_pool_ਗਿਣਤੀ-1],
                             return_this, size);
      return_this->pool = vvp_net_pool[vvp_net_pool_ਗਿਣਤੀ-1];
#ਜੇ_ਅੰਤ
      vvp_net_alloc_ਸਾਰਣੀ += 1;
      vvp_net_alloc_ਬਚਿਆ -= 1;
      ਗਿਣਤੀ_vvp_nets += 1;
      ਵਾਪਸ return_this;
}

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ CHECK_WITH_VALGRIND
ਸਥਿਰ map<vvp_net_t*, ਤਾਰਕਿਕ> vvp_net_map;
ਸਥਿਰ map<sfunc_core*, ਤਾਰਕਿਕ> sfunc_map;
ਸਥਿਰ map<vvp_udp_fun_core*, ਤਾਰਕਿਕ> udp_map;
ਸਥਿਰ map<resolv_core*, ਤਾਰਕਿਕ> resolv_map;
ਸਥਿਰ vvp_net_t **local_net_pool = 0;
ਸਥਿਰ ਬਿਨਾਨਿਸ਼ਾਨੀ local_net_pool_ਗਿਣਤੀ = 0;

ਖਾਲੀ pool_local_net(vvp_net_t*net)
{
      local_net_pool_ਗਿਣਤੀ += 1;
      local_net_pool = static_cast<vvp_net_t **>(realloc(local_net_pool,
                       local_net_pool_ਗਿਣਤੀ*ਆਕਾਰ(vvp_net_t **)));
      local_net_pool[local_net_pool_ਗਿਣਤੀ-1] = net;
}

ਖਾਲੀ vvp_net_delete(vvp_net_t *ਵਸਤੂ)
{
      vvp_net_map[ਵਸਤੂ] = ਸੱਚ;
      ਜੇ (sfunc_core*tmp = dynamic_cast<sfunc_core*> (ਵਸਤੂ->fun)) {
	    sfunc_map[tmp] = ਸੱਚ;
      }
      ਜੇ (vvp_udp_fun_core*tmp = dynamic_cast<vvp_udp_fun_core*> (ਵਸਤੂ->fun)) {
	    udp_map[tmp] = ਸੱਚ;
      }
      ਜੇ (resolv_core*tmp = dynamic_cast<resolv_core*> (ਵਸਤੂ->fun)) {
	    resolv_map[tmp] = ਸੱਚ;
      }
	/* Only delete static object variables. */
      ਜੇ (vvp_fun_signal_object_sa*tmp = dynamic_cast<vvp_fun_signal_object_sa*> (ਵਸਤੂ->fun)) {
	    ਮਿਟਾਓ tmp;
      }
	/* Only delete static string variables. */
      ਜੇ (vvp_fun_signal_string_sa*tmp = dynamic_cast<vvp_fun_signal_string_sa*> (ਵਸਤੂ->fun)) {
	    ਮਿਟਾਓ tmp;
      }
}

ਖਾਲੀ vvp_net_pool_delete()
{
      ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ vvp_nets_del = 0;

      ਲਈ (ਬਿਨਾਨਿਸ਼ਾਨੀ idx = 0; idx < local_net_pool_ਗਿਣਤੀ; idx += 1) {
	    vvp_net_delete(local_net_pool[idx]);
      }
      ਮੁਕਤ(local_net_pool);
      local_net_pool = 0;
      local_net_pool_ਗਿਣਤੀ = 0;

      map<vvp_net_t*, ਤਾਰਕਿਕ>::iterator iter;
      ਲਈ (iter = vvp_net_map.begin(); iter != vvp_net_map.ਸਿਰਾ(); ++ iter ) {
	    vvp_nets_del += 1;
	    VALGRIND_MEMPOOL_FREE(iter->ਪਹਿਲਾ->pool, iter->ਪਹਿਲਾ);
      }
      vvp_net_map.clear();

      map<sfunc_core*, ਤਾਰਕਿਕ>::iterator siter;
      ਲਈ (siter = sfunc_map.begin(); siter != sfunc_map.ਸਿਰਾ(); ++ siter ) {
	    ਮਿਟਾਓ siter->ਪਹਿਲਾ;
      }
      sfunc_map.clear();

      map<vvp_udp_fun_core*, ਤਾਰਕਿਕ>::iterator uiter;
      ਲਈ (uiter = udp_map.begin(); uiter != udp_map.ਸਿਰਾ(); ++ uiter ) {
	    ਮਿਟਾਓ uiter->ਪਹਿਲਾ;
      }
      udp_map.clear();

      map<resolv_core*, ਤਾਰਕਿਕ>::iterator riter;
      ਲਈ (riter = resolv_map.begin(); riter != resolv_map.ਸਿਰਾ(); ++ riter ) {
	    ਮਿਟਾਓ riter->ਪਹਿਲਾ;
      }
      resolv_map.clear();

      ਜੇ (RUNNING_ON_VALGRIND && (vvp_nets_del != ਗਿਣਤੀ_vvp_nets)) {
	    fflush(ਨਿਰਾ);
	    VALGRIND_PRINTF("Error: vvp missed deleting %ld of %lu net(s).",
	                    (ਲੰਮਾ) ਗਿਣਤੀ_vvp_nets - vvp_nets_del,
	                    ਗਿਣਤੀ_vvp_nets);
      }

      ਲਈ (ਬਿਨਾਨਿਸ਼ਾਨੀ idx = 0; idx < vvp_net_pool_ਗਿਣਤੀ; idx += 1) {
	    VALGRIND_DESTROY_MEMPOOL(vvp_net_pool[idx]);
	    ::ਮਿਟਾਓ [] vvp_net_pool[idx];
      }
      ਮੁਕਤ(vvp_net_pool);
      vvp_net_pool = ਨਿਰਾ;
      vvp_net_pool_ਗਿਣਤੀ = 0;
}
#ਜੇ_ਅੰਤ

ਖਾਲੀ vvp_net_t::ਸੰਚਾਲਕ ਮਿਟਾਓ(ਖਾਲੀ*)
{
      ਪ੍ਰਤਿਜ੍ਞਾ(0);
}

vvp_net_t::vvp_net_t()
: ਬਾਹਰਲਾ_2(vvp_net_ptr_t(0,0))
{
      fun = 0;
      fil = 0;
}

ਖਾਲੀ vvp_net_t::link(vvp_net_ptr_t ਪੋਰਟ_ਮੰਜ਼ਿਲ_link)
{
      vvp_net_t*net = ਪੋਰਟ_ਮੰਜ਼ਿਲ_link.ptr();

	// Connect nodes with vvp_fun_modpath_src to the head of the
	// linked list, so that vvp_fun_modpath_src are always evaluated
	// before their respective vvp_fun_modpath
      ਜੇ (dynamic_cast<vvp_fun_modpath_src*>(net->fun)) {
	    net->ਪੋਰਟ[ਪੋਰਟ_ਮੰਜ਼ਿਲ_link.ਪੋਰਟ()] = ਬਾਹਰਲਾ_2;
	    ਬਾਹਰਲਾ_2 = ਪੋਰਟ_ਮੰਜ਼ਿਲ_link;
	// All other nodes connect after the last vvp_fun_modpath_src
      } ਨਹੀਂਤਾਂ {
	      // List is empty or first node has no vvp_fun_modpath_src
	      // then just insert the node into the head of linked list
	    ਜੇ (ਬਾਹਰਲਾ_2.ptr() == ਖਾਲੀ_ਸੂਚਕ || ! dynamic_cast<vvp_fun_modpath_src*>(ਬਾਹਰਲਾ_2.ptr()->fun)) {
		  net->ਪੋਰਟ[ਪੋਰਟ_ਮੰਜ਼ਿਲ_link.ਪੋਰਟ()] = ਬਾਹਰਲਾ_2;
		  ਬਾਹਰਲਾ_2 = ਪੋਰਟ_ਮੰਜ਼ਿਲ_link;
		  ਵਾਪਸ;
	    }

	    vvp_net_ptr_t cur = ਬਾਹਰਲਾ_2;
	    vvp_net_ptr_t prev = vvp_net_ptr_t(ਖਾਲੀ_ਸੂਚਕ,0);

	      // Traverse through linked list
	    ਜਦੋਂ (dynamic_cast<vvp_fun_modpath_src*>(cur.ptr()->fun)) {
		  prev = cur;
		  cur = cur.ptr()->ਪੋਰਟ[cur.ਪੋਰਟ()];
		  ਜੇ (! cur.ptr()) ਰੋਕੋ; // End of list
	    }

	    ਪ੍ਰਤਿਜ੍ਞਾ(prev.ptr());

	      // Insert after last vvp_fun_modpath_src (or end of list)
	    net->ਪੋਰਟ[ਪੋਰਟ_ਮੰਜ਼ਿਲ_link.ਪੋਰਟ()] = cur;
	    prev.ptr()->ਪੋਰਟ[prev.ਪੋਰਟ()] = ਪੋਰਟ_ਮੰਜ਼ਿਲ_link;
      }
}

/*
 * Unlink a ptr object from the driver. The input is the driver in the
 * form of a vvp_net_t pointer. The .out member of that object is the
 * driver. The dst_ptr argument is the receiver pin to be located and
 * removed from the fan-out list.
 */
ਖਾਲੀ vvp_net_t::unlink(vvp_net_ptr_t dst_ptr)
{
      vvp_net_t*net = dst_ptr.ptr();
      ਬਿਨਾਨਿਸ਼ਾਨੀ net_ਪੋਰਟ = dst_ptr.ਪੋਰਟ();

      ਜੇ (ਬਾਹਰਲਾ_2 == dst_ptr) {
	      /* If the drive fan-out list starts with this pointer,
		 then the unlink is easy. Pull the list forward. */
	    ਬਾਹਰਲਾ_2 = net->ਪੋਰਟ[net_ਪੋਰਟ];
      } ਨਹੀਂਤਾਂ ਜੇ (! ਬਾਹਰਲਾ_2.nil()) {
	      /* Scan the linked list, looking for the net_ptr_t
		 pointer *before* the one we wish to remove. */
	    vvp_net_ptr_t cur = ਬਾਹਰਲਾ_2;
	    ਪ੍ਰਤਿਜ੍ਞਾ(!cur.nil());
	    vvp_net_t*cur_net = cur.ptr();
	    ਬਿਨਾਨਿਸ਼ਾਨੀ cur_ਪੋਰਟ = cur.ਪੋਰਟ();
	    ਜਦੋਂ (cur_net && cur_net->ਪੋਰਟ[cur_ਪੋਰਟ] != dst_ptr) {
		  cur = cur_net->ਪੋਰਟ[cur_ਪੋਰਟ];
		  cur_net = cur.ptr();
		  cur_ਪੋਰਟ = cur.ਪੋਰਟ();
	    }
	      /* Unlink. */
	    ਜੇ (cur_net) cur_net->ਪੋਰਟ[cur_ਪੋਰਟ] = net->ਪੋਰਟ[net_ਪੋਰਟ];
      }

      net->ਪੋਰਟ[net_ਪੋਰਟ] = vvp_net_ptr_t(0,0);
}

ਖਾਲੀ vvp_net_t::ਗਿਣਤੀ_drivers(ਬਿਨਾਨਿਸ਼ਾਨੀ idx, ਬਿਨਾਨਿਸ਼ਾਨੀ counts[4])
{
      counts[0] = 0;
      counts[1] = 0;
      counts[2] = 0;
      counts[3] = 0;

        /* $countdrivers can only be used on wires. */
      vvp_wire_ਅਧਾਰ*wire=dynamic_cast<vvp_wire_ਅਧਾਰ*>(fil);
      ਪ੍ਰਤਿਜ੍ਞਾ(wire);

      ਜੇ (wire->is_forced(idx))
            counts[3] = 1;

        /* If the net has multiple drivers, we need to interrogate the
           resolver network to get the driven values. */
      ਜੇ (resolv_core*resolver = dynamic_cast<resolv_core*>(fun)) {
            resolver->ਗਿਣਤੀ_drivers(idx, counts);
            ਵਾਪਸ;
      }
      ਜੇ (vvp_island_ਪੋਰਟ*resolver = dynamic_cast<vvp_island_ਪੋਰਟ*>(fun)) {
            resolver->ਗਿਣਤੀ_drivers(idx, counts);
            ਵਾਪਸ;
      }

        /* If the functor is not a resolver, there is only one driver, so
           we can just interrogate the filter to get the driven value. */
      update_driver_counts(wire->driven_ਮੁੱਲ(idx), counts);
}

ਖਾਲੀ vvp_net_fun_t::ਸੰਚਾਲਕ ਮਿਟਾਓ(ਖਾਲੀ*)
{
#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ CHECK_WITH_VALGRIND
      ਪ੍ਰਤਿਜ੍ਞਾ(0);
#ਜੇ_ਅੰਤ
}


vvp_net_fil_t::vvp_net_fil_t()
{
      force_link_ = 0;
      force_propagate_ = ਝੂਠ;
      ਗਿਣਤੀ_filters += 1;
}

vvp_net_fil_t::~vvp_net_fil_t()
{
      ਪ੍ਰਤਿਜ੍ਞਾ(force_link_ == 0);
}

vvp_net_fil_t::prop_t vvp_net_fil_t::filter_vec4(ਅਚਲ vvp_vector4_t&,
                                                 vvp_vector4_t&,
                                                 ਬਿਨਾਨਿਸ਼ਾਨੀ, ਬਿਨਾਨਿਸ਼ਾਨੀ)
{
      ਵਾਪਸ PROP;
}

vvp_net_fil_t::prop_t vvp_net_fil_t::filter_vec8(ਅਚਲ vvp_vector8_t&,
                                                 vvp_vector8_t&,
                                                 ਬਿਨਾਨਿਸ਼ਾਨੀ, ਬਿਨਾਨਿਸ਼ਾਨੀ)
{
      ਵਾਪਸ PROP;
}

vvp_net_fil_t::prop_t vvp_net_fil_t::filter_real(ਦੁੱਗਣਾ&)
{
      ਵਾਪਸ PROP;
}

vvp_net_fil_t::prop_t vvp_net_fil_t::filter_string(ਅਚਲ string&)
{
      ਵਾਪਸ PROP;
}

vvp_net_fil_t::prop_t vvp_net_fil_t::filter_object(vvp_object_t&)
{
      ਵਾਪਸ PROP;
}

ਖਾਲੀ vvp_net_fil_t::force_ਨਕਾਬ(ਅਚਲ vvp_vector2_t&ਨਕਾਬ)
{
      ਜੇ (force_ਨਕਾਬ_2.size() == 0)
	    force_ਨਕਾਬ_2 = vvp_vector2_t(vvp_vector2_t::FILL0, ਨਕਾਬ.size());

      ਪ੍ਰਤਿਜ੍ਞਾ(force_ਨਕਾਬ_2.size() == ਨਕਾਬ.size());
      ਲਈ (ਬਿਨਾਨਿਸ਼ਾਨੀ idx = 0 ; idx < ਨਕਾਬ.size() ; idx += 1) {
	    ਜੇ (ਨਕਾਬ.value(idx) == 0)
		  ਜਾਰੀ;

	    force_ਨਕਾਬ_2.ਸੈੱਟ_ਦੋਆਧਾਰੀ_ਅੰਕ(idx, 1);
	    force_propagate_ = ਸੱਚ;
      }
}

ਖਾਲੀ vvp_net_fil_t::release_ਨਕਾਬ(ਅਚਲ vvp_vector2_t&ਨਕਾਬ)
{
      ਜੇ (force_ਨਕਾਬ_2.size() == 0)
	    ਵਾਪਸ;

      ਪ੍ਰਤਿਜ੍ਞਾ(force_ਨਕਾਬ_2.size() == ਨਕਾਬ.size());
      ਲਈ (ਬਿਨਾਨਿਸ਼ਾਨੀ idx = 0 ; idx < ਨਕਾਬ.size() ; idx += 1) {
	    ਜੇ (ਨਕਾਬ.value(idx))
		  force_ਨਕਾਬ_2.ਸੈੱਟ_ਦੋਆਧਾਰੀ_ਅੰਕ(idx, 0);
      }

      ਜੇ (force_ਨਕਾਬ_2.is_zero())
	    force_ਨਕਾਬ_2 = vvp_vector2_t();
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
ਖਾਲੀ vvp_net_fil_t::force_link(vvp_net_t*dst, vvp_net_t*src)
{
      ਪ੍ਰਤਿਜ੍ਞਾ(dst->fil == ਇਹ);

      ਜੇ (force_link_ == 0) {
	    force_link_ = ਨਵਾਂ vvp_net_t;
	      // Use port[3] to hold the force destination.
	    force_link_->ਪੋਰਟ[3] = vvp_net_ptr_t(dst, 0);
	    force_link_->ਪੋਰਟ[2] = vvp_net_ptr_t(0,0);
	    force_link_->fun = ਨਵਾਂ vvp_fun_force;
      }

      force_unlink();
      ਪ੍ਰਤਿਜ੍ਞਾ(force_link_->ਪੋਰਟ[2] == vvp_net_ptr_t(0,0));

	// Use port[2] to hold the force source.
      force_link_->ਪੋਰਟ[2] = vvp_net_ptr_t(src,0);

      vvp_net_ptr_t dst_ptr(force_link_, 0);
      src->link(dst_ptr);
}

ਖਾਲੀ vvp_net_fil_t::ਸੰਚਾਲਕ ਮਿਟਾਓ(ਖਾਲੀ*)
{
      ਪ੍ਰਤਿਜ੍ਞਾ(0);
}

ਖਾਲੀ vvp_net_fil_t::force_unlink(ਖਾਲੀ)
{
      ਜੇ (force_link_ == 0) ਵਾਪਸ;
      vvp_net_t*src = force_link_->ਪੋਰਟ[2].ptr();
      ਜੇ (src == 0) ਵਾਪਸ;

      src->unlink(vvp_net_ptr_t(force_link_,0));
      force_link_->ਪੋਰਟ[2] = vvp_net_ptr_t(0,0);
}

/* *** BIT operations *** */
vvp_bit4_t ਜੋੜੋ_with_ਹਾਸਲ(vvp_bit4_t a, vvp_bit4_t b, vvp_bit4_t&c)
{
      ਜੇ (bit4_is_xz(a) || bit4_is_xz(b) || bit4_is_xz(c)) {
	    c = BIT4_X;
	    ਵਾਪਸ BIT4_X;
      }

	// NOTE: This relies on the facts that XZ values have been
	// weeded out, and that BIT4_1 is 1 and BIT4_0 is 0.
      ਪੂਰਨ ਜੋੜ = (ਪੂਰਨ)a + (ਪੂਰਨ)b + (ਪੂਰਨ)c;

      ਚੋਣ (ਜੋੜ) {
	  ਹਾਲਤ 0:
	      // c must already be 0.
	    ਵਾਪਸ BIT4_0;
	  ਹਾਲਤ 1:
	    c = BIT4_0;
	    ਵਾਪਸ BIT4_1;
	  ਹਾਲਤ 2:
	    c = BIT4_1;
	    ਵਾਪਸ BIT4_0;
	  ਹਾਲਤ 3:
	    c = BIT4_1;
	    ਵਾਪਸ BIT4_1;
      }
      fprintf(stderr, "Incorrect result %d.\n", ਜੋੜ);
      ਪ੍ਰਤਿਜ੍ਞਾ(0);
      ਵਾਪਸ BIT4_X;
}

vvp_bit4_t scalar_ਮੰਜ਼ਿਲ_bit4(PLI_INT32 scalar)
{
      ਚੋਣ(scalar) {
	  ਹਾਲਤ vpi0:
	    ਵਾਪਸ BIT4_0;
	  ਹਾਲਤ vpi1:
	    ਵਾਪਸ BIT4_1;
	  ਹਾਲਤ vpiX:
	    ਵਾਪਸ BIT4_X;
	  ਹਾਲਤ vpiZ:
	    ਵਾਪਸ BIT4_Z;
      }
      fprintf(stderr, "Unsupported scalar value %d.\n", (ਪੂਰਨ)scalar);
      ਪ੍ਰਤਿਜ੍ਞਾ(0);
      ਵਾਪਸ BIT4_X;
}

vvp_bit4_t ਸੰਚਾਲਕ ^ (vvp_bit4_t a, vvp_bit4_t b)
{
      ਜੇ (bit4_is_xz(a))
	    ਵਾਪਸ BIT4_X;
      ਜੇ (bit4_is_xz(b))
	    ਵਾਪਸ BIT4_X;
      ਜੇ (a == BIT4_0)
	    ਵਾਪਸ b;
      ਜੇ (b == BIT4_0)
	    ਵਾਪਸ a;
      ਵਾਪਸ BIT4_0;
}

ostream& ਸੰਚਾਲਕ<<(ostream&out, vvp_bit4_t ਦੋਆਧਾਰੀ_ਅੰਕ)
{
      ਚੋਣ (ਦੋਆਧਾਰੀ_ਅੰਕ) {
	  ਹਾਲਤ BIT4_0:
	    out << "0";
	    ਰੋਕੋ;
	  ਹਾਲਤ BIT4_1:
	    out << "1";
	    ਰੋਕੋ;
	  ਹਾਲਤ BIT4_X:
	    out << "X";
	    ਰੋਕੋ;
	  ਹਾਲਤ BIT4_Z:
	    out << "Z";
	    ਰੋਕੋ;
	  ਮੂਲ:
	    out << "?";
	    ਰੋਕੋ;
      }
      ਵਾਪਸ out;
}

ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਬਿਨਾਨਿਸ਼ਾਨੀ ਛੋਟਾ ਕਿਨਾਰਾ_t;

ਸਤਰਅੰਦਰ ਕਿਨਾਰਾ_t VVP_ਕਿਨਾਰਾ(vvp_bit4_t from, vvp_bit4_t to)
{
      ਵਾਪਸ 1 << ((from << 2) | to);
}

ਅਚਲ ਕਿਨਾਰਾ_t vvp_ਕਿਨਾਰਾ_posedge
      = VVP_ਕਿਨਾਰਾ(BIT4_0,BIT4_1)
      | VVP_ਕਿਨਾਰਾ(BIT4_0,BIT4_X)
      | VVP_ਕਿਨਾਰਾ(BIT4_0,BIT4_Z)
      | VVP_ਕਿਨਾਰਾ(BIT4_X,BIT4_1)
      | VVP_ਕਿਨਾਰਾ(BIT4_Z,BIT4_1)
      ;

ਅਚਲ ਕਿਨਾਰਾ_t vvp_ਕਿਨਾਰਾ_negedge
      = VVP_ਕਿਨਾਰਾ(BIT4_1,BIT4_0)
      | VVP_ਕਿਨਾਰਾ(BIT4_1,BIT4_X)
      | VVP_ਕਿਨਾਰਾ(BIT4_1,BIT4_Z)
      | VVP_ਕਿਨਾਰਾ(BIT4_X,BIT4_0)
      | VVP_ਕਿਨਾਰਾ(BIT4_Z,BIT4_0)
      ;

ਪੂਰਨ ਕਿਨਾਰਾ(vvp_bit4_t from, vvp_bit4_t to)
{
      ਕਿਨਾਰਾ_t ਨਕਾਬ = VVP_ਕਿਨਾਰਾ(from, to);
      ਜੇ (ਨਕਾਬ & vvp_ਕਿਨਾਰਾ_posedge)
	    ਵਾਪਸ 1;
      ਜੇ (ਨਕਾਬ & vvp_ਕਿਨਾਰਾ_negedge)
	    ਵਾਪਸ -1;
      ਵਾਪਸ 0;
}

ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ multiply_with_ਹਾਸਲ(ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ a, ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ b,
				  ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ&ਹਾਸਲ)
{
      ਅਚਲ ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ ਨਕਾਬ = (1UL << (CPU_ਸ਼ਬਦ_ਬਿੱਟ/2)) - 1;
      ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ a0 = a & ਨਕਾਬ;
      ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ a1 = (a >> (CPU_ਸ਼ਬਦ_ਬਿੱਟ/2)) & ਨਕਾਬ;
      ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ b0 = b & ਨਕਾਬ;
      ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ b1 = (b >> (CPU_ਸ਼ਬਦ_ਬਿੱਟ/2)) & ਨਕਾਬ;

      ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ tmp = a0 * b0;

      ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ r00 = tmp & ਨਕਾਬ;
      ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ c00 = (tmp >> (CPU_ਸ਼ਬਦ_ਬਿੱਟ/2)) & ਨਕਾਬ;

      tmp = a0 * b1;

      ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ r01 = tmp & ਨਕਾਬ;
      ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ c01 = (tmp >> (CPU_ਸ਼ਬਦ_ਬਿੱਟ/2)) & ਨਕਾਬ;

      tmp = a1 * b0;

      ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ r10 = tmp & ਨਕਾਬ;
      ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ c10 = (tmp >> (CPU_ਸ਼ਬਦ_ਬਿੱਟ/2)) & ਨਕਾਬ;

      tmp = a1 * b1;

      ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ r11 = tmp & ਨਕਾਬ;
      ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ c11 = (tmp >> (CPU_ਸ਼ਬਦ_ਬਿੱਟ/2)) & ਨਕਾਬ;

      ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ r1 = c00 + r01 + r10;
      ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ r2 = (r1 >> (CPU_ਸ਼ਬਦ_ਬਿੱਟ/2)) & ਨਕਾਬ;
      r1 &= ਨਕਾਬ;
      r2 += c01 + c10 + r11;
      ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ r3 = (r2 >> (CPU_ਸ਼ਬਦ_ਬਿੱਟ/2)) & ਨਕਾਬ;
      r2 &= ਨਕਾਬ;
      r3 += c11;
      r3 &= ਨਕਾਬ;

      ਹਾਸਲ = (r3 << (CPU_ਸ਼ਬਦ_ਬਿੱਟ/2)) + r2;
      ਵਾਪਸ (r1 << (CPU_ਸ਼ਬਦ_ਬਿੱਟ/2)) + r00;
}


ਖਾਲੀ vvp_send_vec8(vvp_net_ptr_t ptr, ਅਚਲ vvp_vector8_t&val)
{
      ਜਦੋਂ (vvp_net_t*cur = ptr.ptr()) {
	    vvp_net_ptr_t ਅਗਲਾ_val = cur->ਪੋਰਟ[ptr.ਪੋਰਟ()];

	    ਜੇ (cur->fun)
		  cur->fun->recv_vec8(ptr, val);

	    ptr = ਅਗਲਾ_val;
      }
}

ਖਾਲੀ vvp_send_real(vvp_net_ptr_t ptr, ਦੁੱਗਣਾ val, vvp_context_t context)
{
      ਜਦੋਂ (vvp_net_t*cur = ptr.ptr()) {
	    vvp_net_ptr_t ਅਗਲਾ_val = cur->ਪੋਰਟ[ptr.ਪੋਰਟ()];

	    ਜੇ (cur->fun)
		  cur->fun->recv_real(ptr, val, context);

	    ptr = ਅਗਲਾ_val;
      }
}

ਅਚਲ vvp_vector4_t vvp_vector4_t::nil;

ਖਾਲੀ vvp_vector4_t::copy_ਬਿੱਟ(ਅਚਲ vvp_vector4_t&that)
{

      ਜੇ (ਮਾਪ_2 == that.ਮਾਪ_2) {
	    ਜੇ (ਮਾਪ_2 > ਬਿੱਟ_ਪ੍ਰਤੀ_ਸ਼ਬਦ) {
		  ਬਿਨਾਨਿਸ਼ਾਨੀ ਸ਼ਬਦ = (ਮਾਪ_2+ਬਿੱਟ_ਪ੍ਰਤੀ_ਸ਼ਬਦ-1) / ਬਿੱਟ_ਪ੍ਰਤੀ_ਸ਼ਬਦ;
		  ਲਈ (ਬਿਨਾਨਿਸ਼ਾਨੀ idx = 0 ;  idx < ਸ਼ਬਦ ;  idx += 1)
			abits_ptr_[idx] = that.abits_ptr_[idx];
		  ਲਈ (ਬਿਨਾਨਿਸ਼ਾਨੀ idx = 0 ;  idx < ਸ਼ਬਦ ;  idx += 1)
			bbits_ptr_[idx] = that.bbits_ptr_[idx];
	    } ਨਹੀਂਤਾਂ {
		  abits_val_ = that.abits_val_;
		  bbits_val_ = that.bbits_val_;
	    }
	    ਵਾਪਸ;
      }

	/* Now we know that the sizes of this and that are definitely
	   different. We can use that in code below. In any case, we
	   need to copy only the smaller of the sizes. */

	/* If source and destination are both short, then mask/copy
	   the bit values. */
      ਜੇ (ਮਾਪ_2 <= ਬਿੱਟ_ਪ੍ਰਤੀ_ਸ਼ਬਦ && that.ਮਾਪ_2 <= ਬਿੱਟ_ਪ੍ਰਤੀ_ਸ਼ਬਦ) {
	    ਬਿਨਾਨਿਸ਼ਾਨੀ ਬਿੱਟ_ਮੰਜ਼ਿਲ_copy = (that.ਮਾਪ_2 < ਮਾਪ_2) ? that.ਮਾਪ_2 : ਮਾਪ_2;
	    ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ ਨਕਾਬ = ((ਬਿੱਟ_ਮੰਜ਼ਿਲ_copy < ਬਿੱਟ_ਪ੍ਰਤੀ_ਸ਼ਬਦ) ? (1UL << ਬਿੱਟ_ਮੰਜ਼ਿਲ_copy) :
	                                                           (0UL)) - 1UL;
	    abits_val_ &= ~ਨਕਾਬ;
	    bbits_val_ &= ~ਨਕਾਬ;
	    abits_val_ |= that.abits_val_&ਨਕਾਬ;
	    bbits_val_ |= that.bbits_val_&ਨਕਾਬ;
	    ਵਾਪਸ;
      }

	/* Now we know that either source or destination are long. If
	   the destination is short, then mask/copy from the low word
	   of the long source. */
      ਜੇ (ਮਾਪ_2 <= ਬਿੱਟ_ਪ੍ਰਤੀ_ਸ਼ਬਦ) {
	    abits_val_ = that.abits_ptr_[0];
	    bbits_val_ = that.bbits_ptr_[0];
	    ਜੇ (ਮਾਪ_2 < ਬਿੱਟ_ਪ੍ਰਤੀ_ਸ਼ਬਦ) {
		  ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ ਨਕਾਬ = (1UL << ਮਾਪ_2) - 1UL;
		  abits_val_ &= ਨਕਾਬ;
		  bbits_val_ &= ਨਕਾਬ;
	    }
	    ਵਾਪਸ;
      }

	/* Now we know that the destination must be long. If the
	   source is short, then mask/copy from its value. */
      ਜੇ (that.ਮਾਪ_2 <= ਬਿੱਟ_ਪ੍ਰਤੀ_ਸ਼ਬਦ) {
	    ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ ਨਕਾਬ;
	    ਜੇ (that.ਮਾਪ_2 < ਬਿੱਟ_ਪ੍ਰਤੀ_ਸ਼ਬਦ) {
		  ਨਕਾਬ = (1UL << that.ਮਾਪ_2) - 1UL;
		  abits_ptr_[0] &= ~ਨਕਾਬ;
		  bbits_ptr_[0] &= ~ਨਕਾਬ;
	    } ਨਹੀਂਤਾਂ {
		  ਨਕਾਬ = -1UL;
	    }
	    abits_ptr_[0] |= that.abits_val_&ਨਕਾਬ;
	    bbits_ptr_[0] |= that.bbits_val_&ਨਕਾਬ;
	    ਵਾਪਸ;
      }

	/* Finally, we know that source and destination are long. copy
	   words until we get to the last. */
      ਬਿਨਾਨਿਸ਼ਾਨੀ ਬਿੱਟ_ਮੰਜ਼ਿਲ_copy = (that.ਮਾਪ_2 < ਮਾਪ_2) ? that.ਮਾਪ_2 : ਮਾਪ_2;
      ਬਿਨਾਨਿਸ਼ਾਨੀ ਸ਼ਬਦ_2 = 0;
      ਜਦੋਂ (ਬਿੱਟ_ਮੰਜ਼ਿਲ_copy >= ਬਿੱਟ_ਪ੍ਰਤੀ_ਸ਼ਬਦ) {
	    abits_ptr_[ਸ਼ਬਦ_2] = that.abits_ptr_[ਸ਼ਬਦ_2];
	    bbits_ptr_[ਸ਼ਬਦ_2] = that.bbits_ptr_[ਸ਼ਬਦ_2];
	    ਬਿੱਟ_ਮੰਜ਼ਿਲ_copy -= ਬਿੱਟ_ਪ੍ਰਤੀ_ਸ਼ਬਦ;
	    ਸ਼ਬਦ_2 += 1;
      }
      ਜੇ (ਬਿੱਟ_ਮੰਜ਼ਿਲ_copy > 0) {
	    ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ ਨਕਾਬ = (1UL << ਬਿੱਟ_ਮੰਜ਼ਿਲ_copy) - 1UL;
	    abits_ptr_[ਸ਼ਬਦ_2] &= ~ਨਕਾਬ;
	    bbits_ptr_[ਸ਼ਬਦ_2] &= ~ਨਕਾਬ;
	    abits_ptr_[ਸ਼ਬਦ_2] |= that.abits_ptr_[ਸ਼ਬਦ_2] & ਨਕਾਬ;
	    bbits_ptr_[ਸ਼ਬਦ_2] |= that.bbits_ptr_[ਸ਼ਬਦ_2] & ਨਕਾਬ;
      }
}

/*
 * This function should ONLY BE CALLED FROM vvp_vector4_t::copy_from_,
 * as it performs part of that functions tasks.
 */
ਖਾਲੀ vvp_vector4_t::copy_from_big_(ਅਚਲ vvp_vector4_t&that)
{
      ਬਿਨਾਨਿਸ਼ਾਨੀ ਸ਼ਬਦ = (ਮਾਪ_2+ਬਿੱਟ_ਪ੍ਰਤੀ_ਸ਼ਬਦ-1) / ਬਿੱਟ_ਪ੍ਰਤੀ_ਸ਼ਬਦ;
      abits_ptr_ = ਨਵਾਂ ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ[2*ਸ਼ਬਦ];
      bbits_ptr_ = abits_ptr_ + ਸ਼ਬਦ;

      ਲਈ (ਬਿਨਾਨਿਸ਼ਾਨੀ idx = 0 ;  idx < ਸ਼ਬਦ ;  idx += 1)
	    abits_ptr_[idx] = that.abits_ptr_[idx];
      ਲਈ (ਬਿਨਾਨਿਸ਼ਾਨੀ idx = 0 ;  idx < ਸ਼ਬਦ ;  idx += 1)
	    bbits_ptr_[idx] = that.bbits_ptr_[idx];
}

/*
 * The copy_inverted_from_ method is just like the copy_from_ method,
 * except that we combine that with an invert. This allows the ~ and
 * the assignment to be blended in many common cases.
 */
ਖਾਲੀ vvp_vector4_t::copy_inverted_from_(ਅਚਲ vvp_vector4_t&that)
{
      ਮਾਪ_2 = that.ਮਾਪ_2;
      ਜੇ (ਮਾਪ_2 > ਬਿੱਟ_ਪ੍ਰਤੀ_ਸ਼ਬਦ) {
	    ਬਿਨਾਨਿਸ਼ਾਨੀ ਸ਼ਬਦ = (ਮਾਪ_2+ਬਿੱਟ_ਪ੍ਰਤੀ_ਸ਼ਬਦ-1) / ਬਿੱਟ_ਪ੍ਰਤੀ_ਸ਼ਬਦ;
	    abits_ptr_ = ਨਵਾਂ ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ[2*ਸ਼ਬਦ];
	    bbits_ptr_ = abits_ptr_ + ਸ਼ਬਦ;

	    ਬਿਨਾਨਿਸ਼ਾਨੀ ਬਚਿਆ = ਮਾਪ_2;
	    ਬਿਨਾਨਿਸ਼ਾਨੀ idx = 0;
	    ਜਦੋਂ (ਬਚਿਆ >= ਬਿੱਟ_ਪ੍ਰਤੀ_ਸ਼ਬਦ) {
		  abits_ptr_[idx] = that.bbits_ptr_[idx] | ~that.abits_ptr_[idx];
		  idx += 1;
		  ਬਚਿਆ -= ਬਿੱਟ_ਪ੍ਰਤੀ_ਸ਼ਬਦ;
	    }
	    ਜੇ (ਬਚਿਆ > 0) {
		  ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ ਨਕਾਬ = (1UL<<ਬਚਿਆ) - 1UL;
		  abits_ptr_[idx] = ਨਕਾਬ & (that.bbits_ptr_[idx] | ~that.abits_ptr_[idx]);
	    }

	    ਲਈ (idx = 0 ;  idx < ਸ਼ਬਦ ;  idx += 1)
		  bbits_ptr_[idx] = that.bbits_ptr_[idx];

      } ਨਹੀਂਤਾਂ {
	    ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ ਨਕਾਬ = (ਮਾਪ_2<ਬਿੱਟ_ਪ੍ਰਤੀ_ਸ਼ਬਦ)? (1UL<<ਮਾਪ_2)-1UL : -1UL;
	    abits_val_ = ਨਕਾਬ & (that.bbits_val_ | ~that.abits_val_);
	    bbits_val_ = that.bbits_val_;
      }
}

/* Make sure to set size_ before calling this routine. */
ਖਾਲੀ vvp_vector4_t::allocate_ਸ਼ਬਦ(ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ inita, ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ initb)
{
      ਜੇ (ਮਾਪ_2 > ਬਿੱਟ_ਪ੍ਰਤੀ_ਸ਼ਬਦ) {
	    ਬਿਨਾਨਿਸ਼ਾਨੀ cnt = (ਮਾਪ_2 + ਬਿੱਟ_ਪ੍ਰਤੀ_ਸ਼ਬਦ - 1) / ਬਿੱਟ_ਪ੍ਰਤੀ_ਸ਼ਬਦ;
	    abits_ptr_ = ਨਵਾਂ ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ[2*cnt];
	    bbits_ptr_ = abits_ptr_ + cnt;
	    ਲਈ (ਬਿਨਾਨਿਸ਼ਾਨੀ idx = 0 ;  idx < cnt ;  idx += 1)
		  abits_ptr_[idx] = inita;
	    ਲਈ (ਬਿਨਾਨਿਸ਼ਾਨੀ idx = 0 ;  idx < cnt ;  idx += 1)
		  bbits_ptr_[idx] = initb;

      } ਨਹੀਂਤਾਂ {
	    abits_val_ = inita;
	    bbits_val_ = initb;
      }
}

vvp_vector4_t::vvp_vector4_t(ਬਿਨਾਨਿਸ਼ਾਨੀ ਮਾਪ_3, ਦੁੱਗਣਾ val)
: ਮਾਪ_2(ਮਾਪ_3)
{
      ਤਾਰਕਿਕ is_neg = ਝੂਠ;
      ਦੁੱਗਣਾ fraction;
      ਪੂਰਨ exponent;

	/* We return 'bx for a NaN or +/- infinity. */
      ਜੇ (val != val || (val && (val == 0.5*val)))  {
	    allocate_ਸ਼ਬਦ(ਸ਼ਬਦ_X_ABITS, ਸ਼ਬਦ_X_BBITS);
	    ਵਾਪਸ;
      }

	/* Convert to a positive result. */
      ਜੇ (val < 0.0) {
	    is_neg = ਸੱਚ;
	    val = -val;
      }
      allocate_ਸ਼ਬਦ(ਸ਼ਬਦ_0_ABITS, ਸ਼ਬਦ_0_BBITS);

	/* Get the exponent and fractional part of the number. */
      fraction = frexp(val, &exponent);

	/* If the value is small enough just use lround(). */
      ਜੇ (exponent < ਬਿੱਟ_ਪ੍ਰਤੀ_ਸ਼ਬਦ-2) {
	    ਜੇ (is_neg) ਇਹ->invert();  // Invert the bits if negative.
	    ਲੰਮਾ sval = lround(val);
	    ਜੇ (is_neg) sval = -sval;
	      /* This requires that 0 and 1 have the same bbit value. */
	    ਜੇ (ਮਾਪ_2 > ਬਿੱਟ_ਪ੍ਰਤੀ_ਸ਼ਬਦ) {
		  abits_ptr_[0] = sval;
	    } ਨਹੀਂਤਾਂ {
		  abits_val_ = sval;
	    }
	    ਵਾਪਸ;
      }

      ਬਿਨਾਨਿਸ਼ਾਨੀ nwords = (exponent-1) / ਬਿੱਟ_ਪ੍ਰਤੀ_ਸ਼ਬਦ;
      ਬਿਨਾਨਿਸ਼ਾਨੀ my_ਸ਼ਬਦ = (ਮਾਪ_2 + ਬਿੱਟ_ਪ੍ਰਤੀ_ਸ਼ਬਦ - 1) / ਬਿੱਟ_ਪ੍ਰਤੀ_ਸ਼ਬਦ - 1;

      fraction = ldexp(fraction, (exponent-1) % ਬਿੱਟ_ਪ੍ਰਤੀ_ਸ਼ਬਦ + 1);

	/* Skip any leading bits. */
      ਲਈ (ਪੂਰਨ idx = (ਨਿਸ਼ਾਨੀਵਾਲਾ) nwords; idx > (ਨਿਸ਼ਾਨੀਵਾਲਾ) my_ਸ਼ਬਦ; idx -=1) {
	    ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ ਬਿੱਟ = (ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ) fraction;
	    fraction = fraction - (ਦੁੱਗਣਾ) ਬਿੱਟ;
	    fraction = ldexp(fraction, ਬਿੱਟ_ਪ੍ਰਤੀ_ਸ਼ਬਦ);
      }

	/* Convert the remaining bits as appropriate. */
      ਜੇ (my_ਸ਼ਬਦ == 0) {
		  ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ ਬਿੱਟ = (ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ) fraction;
		  abits_val_ = ਬਿੱਟ;
		  fraction = fraction - (ਦੁੱਗਣਾ) ਬਿੱਟ;
		    /* Round any fractional part up. */
		  ਜੇ (fraction >= 0.5) *ਇਹ += (int64_t) 1;
      } ਨਹੀਂਤਾਂ {
	    ਜੇ (nwords < my_ਸ਼ਬਦ) my_ਸ਼ਬਦ = nwords;
	    ਲਈ (ਪੂਰਨ idx = (ਨਿਸ਼ਾਨੀਵਾਲਾ)my_ਸ਼ਬਦ; idx >= 0; idx -= 1) {
		  ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ ਬਿੱਟ = (ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ) fraction;
		  abits_ptr_[idx] = ਬਿੱਟ;
		  fraction = fraction - (ਦੁੱਗਣਾ) ਬਿੱਟ;
		  fraction = ldexp(fraction, ਬਿੱਟ_ਪ੍ਰਤੀ_ਸ਼ਬਦ);
	    }
	      /* Round any fractional part up. */
	    ਜੇ (fraction >= ldexp(0.5, ਬਿੱਟ_ਪ੍ਰਤੀ_ਸ਼ਬਦ)) *ਇਹ += (int64_t) 1;
       }

	/* Convert to a negative number if needed. */
      ਜੇ (is_neg) {
	    ਇਹ->invert();
	    *ਇਹ += (int64_t) 1;
      }
}

vvp_vector4_t::vvp_vector4_t(ਅਚਲ vvp_vector4_t&that,
			    ਬਿਨਾਨਿਸ਼ਾਨੀ adr, ਬਿਨਾਨਿਸ਼ਾਨੀ wid)
{
	// Set up and initialize the destination.
      ਮਾਪ_2 = wid;
      allocate_ਸ਼ਬਦ(ਸ਼ਬਦ_X_ABITS, ਸ਼ਬਦ_X_BBITS);

	// Special case: selecting from far beyond the source vector,
	// to the result is all X bits. We're done.
      ਜੇ (adr >= that.ਮਾਪ_2)
	    ਵਾਪਸ;

	// Special case: The source is not quite big enough to supply
	// all bits, so get the bits that we can. The remainder will
	// be left at BIT4_X.
      ਜੇ ((adr + wid) > that.ਮਾਪ_2) {
	    ਬਿਨਾਨਿਸ਼ਾਨੀ use_wid = that.ਮਾਪ_2 - adr;
	    ਲਈ (ਬਿਨਾਨਿਸ਼ਾਨੀ idx = 0 ; idx < use_wid ; idx += 1)
		  ਸੈੱਟ_ਦੋਆਧਾਰੀ_ਅੰਕ(idx, that.value(adr+idx));

	    ਵਾਪਸ;
      }

	// At the point, we know that the source part is entirely
	// contained in the source vector.
	// assert((adr + wid) <= that.size_);

      ਜੇ (wid > ਬਿੱਟ_ਪ੍ਰਤੀ_ਸ਼ਬਦ) {
	      /* In this case, the subvector and the source vector are
		 long. Do the transfer reasonably efficiently. */
	    ਬਿਨਾਨਿਸ਼ਾਨੀ ptr = adr / ਬਿੱਟ_ਪ੍ਰਤੀ_ਸ਼ਬਦ;
	    ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ off = adr % ਬਿੱਟ_ਪ੍ਰਤੀ_ਸ਼ਬਦ;
	    ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ noff = ਬਿੱਟ_ਪ੍ਰਤੀ_ਸ਼ਬਦ - off;
	    ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ lmask = (1UL << off) - 1UL;
	    ਬਿਨਾਨਿਸ਼ਾਨੀ trans = 0;
	    ਬਿਨਾਨਿਸ਼ਾਨੀ dst = 0;
	    ਜਦੋਂ (trans < wid) {
		    // The low bits of the result.
		  abits_ptr_[dst] = (that.abits_ptr_[ptr] & ~lmask) >> off;
		  bbits_ptr_[dst] = (that.bbits_ptr_[ptr] & ~lmask) >> off;
		  trans += noff;

		  ਜੇ (trans >= wid)
			ਰੋਕੋ;

		  ptr += 1;

		    // The high bits of the result. Skip this if the
		    // source and destination are perfectly aligned.
		  ਜੇ (noff != ਬਿੱਟ_ਪ੍ਰਤੀ_ਸ਼ਬਦ) {
			abits_ptr_[dst] |= (that.abits_ptr_[ptr]&lmask) << noff;
			bbits_ptr_[dst] |= (that.bbits_ptr_[ptr]&lmask) << noff;
			trans += off;
		  }

		  dst += 1;
	    }

      } ਨਹੀਂਤਾਂ ਜੇ (that.ਮਾਪ_2 > ਬਿੱਟ_ਪ੍ਰਤੀ_ਸ਼ਬਦ) {
	      /* In this case, the subvector fits in a single word,
		 but the source is large. */
	    ਬਿਨਾਨਿਸ਼ਾਨੀ ptr = adr / ਬਿੱਟ_ਪ੍ਰਤੀ_ਸ਼ਬਦ;
	    ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ off = adr % ਬਿੱਟ_ਪ੍ਰਤੀ_ਸ਼ਬਦ;
	    ਬਿਨਾਨਿਸ਼ਾਨੀ trans = ਬਿੱਟ_ਪ੍ਰਤੀ_ਸ਼ਬਦ - off;
	    ਜੇ (trans > wid)
		  trans = wid;

	    ਜੇ (trans == ਬਿੱਟ_ਪ੍ਰਤੀ_ਸ਼ਬਦ) {
		    // Very special case: Copy exactly 1 perfectly
		    // aligned word.
		  abits_val_ = that.abits_ptr_[ptr];
		  bbits_val_ = that.bbits_ptr_[ptr];

	    } ਨਹੀਂਤਾਂ {
		    // lmask is the low bits of the destination,
		    // masked into the source.
		  ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ lmask = (1UL<<trans) - 1UL;
		  lmask <<= off;

		    // The low bits of the result.
		  abits_val_ = (that.abits_ptr_[ptr] & lmask) >> off;
		  bbits_val_ = (that.bbits_ptr_[ptr] & lmask) >> off;

		  ਜੇ (trans < wid) {
			  // If there are more bits, then get them
			  // from the bottom of the next word of the
			  // source.
			ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ hmask = (1UL << (wid-trans)) - 1UL;

			  // The high bits of the result.
			abits_val_ |= (that.abits_ptr_[ptr+1]&hmask) << trans;
			bbits_val_ |= (that.bbits_ptr_[ptr+1]&hmask) << trans;
		  }
	    }

      } ਨਹੀਂਤਾਂ ਜੇ (ਮਾਪ_2 == ਬਿੱਟ_ਪ੍ਰਤੀ_ਸ਼ਬਦ) {
	      /* We know that source and destination are short. If the
		 destination is a full word, then we know the copy is
		 aligned and complete. */
	    abits_val_ = that.abits_val_;
	    bbits_val_ = that.bbits_val_;

      } ਨਹੀਂਤਾਂ {
	      /* Finally, the source and destination vectors are both
		 short, so there is a single mask/shift/copy. */
	    ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ ਨਕਾਬ = (1UL << ਮਾਪ_2) - 1UL;
	    ਨਕਾਬ <<= adr;

	    abits_val_ = (that.abits_val_ & ਨਕਾਬ) >> adr;
	    bbits_val_ = (that.bbits_val_ & ਨਕਾਬ) >> adr;
      }

}

/*
 * Change the size of the vvp_vector4_t vector to the new size. Copy
 * the old values, as many as well fit, into the new vector.
 */
ਖਾਲੀ vvp_vector4_t::resize(ਬਿਨਾਨਿਸ਼ਾਨੀ newsize, vvp_bit4_t pad_ਦੋਆਧਾਰੀ_ਅੰਕ)
{
      ਜੇ (ਮਾਪ_2 == newsize)
	    ਵਾਪਸ;

      ਬਿਨਾਨਿਸ਼ਾਨੀ cnt = (ਮਾਪ_2 + ਬਿੱਟ_ਪ੍ਰਤੀ_ਸ਼ਬਦ - 1) / ਬਿੱਟ_ਪ੍ਰਤੀ_ਸ਼ਬਦ;
      ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ ਸ਼ਬਦ_pad_abits = 0;
      ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ ਸ਼ਬਦ_pad_bbits = 0;
      ਚੋਣ (pad_ਦੋਆਧਾਰੀ_ਅੰਕ) {
	  ਹਾਲਤ BIT4_0:
	    ਸ਼ਬਦ_pad_abits = ਸ਼ਬਦ_0_ABITS;
	    ਸ਼ਬਦ_pad_bbits = ਸ਼ਬਦ_0_BBITS;
	    ਰੋਕੋ;
	  ਹਾਲਤ BIT4_1:
	    ਸ਼ਬਦ_pad_abits = ਸ਼ਬਦ_1_ABITS;
	    ਸ਼ਬਦ_pad_bbits = ਸ਼ਬਦ_1_BBITS;
	    ਰੋਕੋ;
	  ਹਾਲਤ BIT4_X:
	    ਸ਼ਬਦ_pad_abits = ਸ਼ਬਦ_X_ABITS;
	    ਸ਼ਬਦ_pad_bbits = ਸ਼ਬਦ_X_BBITS;
	    ਰੋਕੋ;
	  ਹਾਲਤ BIT4_Z:
	    ਸ਼ਬਦ_pad_abits = ਸ਼ਬਦ_Z_ABITS;
	    ਸ਼ਬਦ_pad_bbits = ਸ਼ਬਦ_Z_BBITS;
	    ਰੋਕੋ;
      }

      ਜੇ (newsize > ਬਿੱਟ_ਪ੍ਰਤੀ_ਸ਼ਬਦ) {
	    ਬਿਨਾਨਿਸ਼ਾਨੀ newcnt = (newsize + ਬਿੱਟ_ਪ੍ਰਤੀ_ਸ਼ਬਦ - 1) / ਬਿੱਟ_ਪ੍ਰਤੀ_ਸ਼ਬਦ;
	    ਜੇ (newcnt == cnt) {
		    // If the word count doesn't change, then there is
		    // no need for re-allocation so we are done now.
		  ਜੇ (newsize > ਮਾਪ_2) {
			ਜੇ (ਬਿਨਾਨਿਸ਼ਾਨੀ fill = ਮਾਪ_2 % ਬਿੱਟ_ਪ੍ਰਤੀ_ਸ਼ਬਦ) {
			      abits_ptr_[cnt-1] &= ~((-1UL) << fill);
			      bbits_ptr_[cnt-1] &= ~((-1UL) << fill);
			      abits_ptr_[cnt-1] |= ਸ਼ਬਦ_pad_abits << fill;
			      bbits_ptr_[cnt-1] |= ਸ਼ਬਦ_pad_bbits << fill;
			}
		  }
		  ਮਾਪ_2 = newsize;
		  ਵਾਪਸ;
	    }

	    ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ*newbits = ਨਵਾਂ ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ[2*newcnt];

	    ਜੇ (cnt > 1) {
		  ਬਿਨਾਨਿਸ਼ਾਨੀ trans = cnt;
		  ਜੇ (trans > newcnt)
			trans = newcnt;

		  ਲਈ (ਬਿਨਾਨਿਸ਼ਾਨੀ idx = 0 ;  idx < trans ;  idx += 1)
			newbits[idx] = abits_ptr_[idx];
		  ਲਈ (ਬਿਨਾਨਿਸ਼ਾਨੀ idx = 0 ;  idx < trans ;  idx += 1)
			newbits[newcnt+idx] = bbits_ptr_[idx];

		  ਮਿਟਾਓ[]abits_ptr_;

	    } ਨਹੀਂਤਾਂ {
		  newbits[0] = abits_val_;
		  newbits[newcnt] = bbits_val_;
	    }

	    ਜੇ (newsize > ਮਾਪ_2) {
		  ਜੇ (ਬਿਨਾਨਿਸ਼ਾਨੀ fill = ਮਾਪ_2 % ਬਿੱਟ_ਪ੍ਰਤੀ_ਸ਼ਬਦ) {
			newbits[cnt-1] &= ~((-1UL) << fill);
			newbits[cnt-1] |= ਸ਼ਬਦ_pad_abits << fill;
			newbits[newcnt+cnt-1] &= ~((-1UL) << fill);
			newbits[newcnt+cnt-1] |= ਸ਼ਬਦ_pad_bbits << fill;
		  }
		  ਲਈ (ਬਿਨਾਨਿਸ਼ਾਨੀ idx = cnt ;  idx < newcnt ;  idx += 1)
			newbits[idx] = ਸ਼ਬਦ_pad_abits;
		  ਲਈ (ਬਿਨਾਨਿਸ਼ਾਨੀ idx = cnt ;  idx < newcnt ;  idx += 1)
			newbits[newcnt+idx] = ਸ਼ਬਦ_pad_bbits;
	    }

	    ਮਾਪ_2 = newsize;
	    abits_ptr_ = newbits;
	    bbits_ptr_ = newbits + newcnt;

      } ਨਹੀਂਤਾਂ {
	    ਜੇ (cnt > 1) {
		  ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ newvala = abits_ptr_[0];
		  ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ newvalb = bbits_ptr_[0];
		  ਮਿਟਾਓ[]abits_ptr_;
		  abits_val_ = newvala;
		  bbits_val_ = newvalb;
	    }

	    ਜੇ (newsize > ਮਾਪ_2) {
		  abits_val_ &= ~((-1UL) << ਮਾਪ_2);
		  bbits_val_ &= ~((-1UL) << ਮਾਪ_2);
		  abits_val_ |= ਸ਼ਬਦ_pad_abits << ਮਾਪ_2;
		  bbits_val_ |= ਸ਼ਬਦ_pad_bbits << ਮਾਪ_2;
	    }

	    ਮਾਪ_2 = newsize;
      }
}


ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ* vvp_vector4_t::subarray(ਬਿਨਾਨਿਸ਼ਾਨੀ adr, ਬਿਨਾਨਿਸ਼ਾਨੀ wid, ਤਾਰਕਿਕ xz_ਮੰਜ਼ਿਲ_0) ਅਚਲ
{
      ਅਚਲ ਬਿਨਾਨਿਸ਼ਾਨੀ BIT2_ਪ੍ਰਤੀ_ਸ਼ਬਦ = 8*ਆਕਾਰ(ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ);
      ਬਿਨਾਨਿਸ਼ਾਨੀ awid = (wid + BIT2_ਪ੍ਰਤੀ_ਸ਼ਬਦ - 1) / (BIT2_ਪ੍ਰਤੀ_ਸ਼ਬਦ);
      ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ*val = ਨਵਾਂ ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ[awid];

      ਲਈ (ਬਿਨਾਨਿਸ਼ਾਨੀ idx = 0 ;  idx < awid ;  idx += 1)
	    val[idx] = 0;

      ਜੇ (ਮਾਪ_2 <= ਬਿੱਟ_ਪ੍ਰਤੀ_ਸ਼ਬਦ) {
	      /* Handle the special case that the array is small. The
		 entire value of the vector4 is within the xbits_val_
		 so we know that the result is a single word, the
		 source is a single word, and we just have to loop
		 through that word. */
	    ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ atmp = abits_val_ >> adr;
	    ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ btmp = bbits_val_ >> adr;
	    ਜੇ (wid < BIT2_ਪ੍ਰਤੀ_ਸ਼ਬਦ) {
		  atmp &= (1UL << wid) - 1;
		  btmp &= (1UL << wid) - 1;
	    }
	    ਜੇ (btmp) {
		  ਜੇ (xz_ਮੰਜ਼ਿਲ_0)
			atmp &= ~btmp;
		  ਨਹੀਂਤਾਂ
			ਜਾਓ x_ਬਾਹਰਲਾ;
	    }

	    val[0] = atmp;

      } ਨਹੀਂਤਾਂ {

	    ਬਿਨਾਨਿਸ਼ਾਨੀ val_ptr = 0;
	    ਬਿਨਾਨਿਸ਼ਾਨੀ val_off = 0;

	      /* Get the first word we are scanning. We may in fact be
		 somewhere in the middle of that word. */
	    ਜਦੋਂ (wid > 0) {
		  ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ atmp = abits_ptr_[adr/ਬਿੱਟ_ਪ੍ਰਤੀ_ਸ਼ਬਦ];
		  ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ btmp = bbits_ptr_[adr/ਬਿੱਟ_ਪ੍ਰਤੀ_ਸ਼ਬਦ];
		  ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ off = adr%ਬਿੱਟ_ਪ੍ਰਤੀ_ਸ਼ਬਦ;
		  atmp >>= off;
		  btmp >>= off;

		  ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ trans = ਬਿੱਟ_ਪ੍ਰਤੀ_ਸ਼ਬਦ - off;
		  ਜੇ (trans > (BIT2_ਪ੍ਰਤੀ_ਸ਼ਬਦ - val_off))
			trans = BIT2_ਪ੍ਰਤੀ_ਸ਼ਬਦ - val_off;
		  ਜੇ (wid < trans)
			trans = wid;
		  ਜੇ (trans < BIT2_ਪ੍ਰਤੀ_ਸ਼ਬਦ) {
			atmp &= (1UL << trans) - 1;
			btmp &= (1UL << trans) - 1;
		  }
		  ਜੇ (btmp) {
			ਜੇ (xz_ਮੰਜ਼ਿਲ_0)
			      atmp &= ~btmp;
			ਨਹੀਂਤਾਂ
			      ਜਾਓ x_ਬਾਹਰਲਾ;
		  }

		  val[val_ptr] |= atmp << val_off;
		  adr += trans;
		  wid -= trans;
		  val_off += trans;
		  ਜੇ (val_off == BIT2_ਪ੍ਰਤੀ_ਸ਼ਬਦ) {
			val_ptr += 1;
			val_off = 0;
		  }
	    }
      }

      ਵਾਪਸ val;

 x_ਬਾਹਰਲਾ:
      ਮਿਟਾਓ[]val;
      ਵਾਪਸ 0;
}

ਖਾਲੀ vvp_vector4_t::setarray(ਬਿਨਾਨਿਸ਼ਾਨੀ adr, ਬਿਨਾਨਿਸ਼ਾਨੀ wid, ਅਚਲ ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ*val)
{
      ਪ੍ਰਤਿਜ੍ਞਾ(adr+wid <= ਮਾਪ_2);

      ਅਚਲ ਬਿਨਾਨਿਸ਼ਾਨੀ BIT2_ਪ੍ਰਤੀ_ਸ਼ਬਦ = 8*ਆਕਾਰ(ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ);

      ਜੇ (ਮਾਪ_2 <= ਬਿੱਟ_ਪ੍ਰਤੀ_ਸ਼ਬਦ) {
	      // We know here that both the source and the target are
	      // within a single word. Write the bits into the
	      // abits_val_ directly.

	    ਪ੍ਰਤਿਜ੍ਞਾ(BIT2_ਪ੍ਰਤੀ_ਸ਼ਬਦ <= ਬਿੱਟ_ਪ੍ਰਤੀ_ਸ਼ਬਦ);
	    ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ lmask = (1UL << adr) - 1UL;
	    ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ hmask = ((adr+wid) < ਬਿੱਟ_ਪ੍ਰਤੀ_ਸ਼ਬਦ)
		  ? -1UL << (adr+wid)
		  : 0;
	    ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ ਨਕਾਬ = ~(hmask | lmask);

	    abits_val_ &= ~ਨਕਾਬ;
	    bbits_val_ &= ~ਨਕਾਬ;

	    abits_val_ |= ਨਕਾਬ & (val[0] << adr);

      } ਨਹੀਂਤਾਂ {
	      // The general case, there are multiple words of
	      // destination, and possibly multiple words of source
	      // data. Shift and mask as we go.
	    ਬਿਨਾਨਿਸ਼ਾਨੀ off = adr % ਬਿੱਟ_ਪ੍ਰਤੀ_ਸ਼ਬਦ;
	    ਬਿਨਾਨਿਸ਼ਾਨੀ ptr = adr / ਬਿੱਟ_ਪ੍ਰਤੀ_ਸ਼ਬਦ;
	    ਬਿਨਾਨਿਸ਼ਾਨੀ val_off = 0;
	    ਬਿਨਾਨਿਸ਼ਾਨੀ val_ptr = 0;
	    ਜਦੋਂ (wid > 0) {
		  ਬਿਨਾਨਿਸ਼ਾਨੀ trans = wid;
		  ਜੇ (trans > (BIT2_ਪ੍ਰਤੀ_ਸ਼ਬਦ-val_off))
			trans = BIT2_ਪ੍ਰਤੀ_ਸ਼ਬਦ-val_off;
		  ਜੇ (trans > (ਬਿੱਟ_ਪ੍ਰਤੀ_ਸ਼ਬਦ-off))
			trans = ਬਿੱਟ_ਪ੍ਰਤੀ_ਸ਼ਬਦ-off;

		  ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ lmask = (1UL << off) - 1UL;
		  ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ hmask = ((off+trans) < ਬਿੱਟ_ਪ੍ਰਤੀ_ਸ਼ਬਦ)
			? -1UL << (off+trans)
			: 0;
		  ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ ਨਕਾਬ = ~(hmask | lmask);

		  abits_ptr_[ptr] &= ~ਨਕਾਬ;
		  bbits_ptr_[ptr] &= ~ਨਕਾਬ;
		  ਜੇ (val_off >= off)
			abits_ptr_[ptr] |= ਨਕਾਬ & (val[val_ptr] >> (val_off-off));
		  ਨਹੀਂਤਾਂ
			abits_ptr_[ptr] |= ਨਕਾਬ & (val[val_ptr] << (off-val_off));

		  wid -= trans;
		  val_off += trans;
		  ਜੇ (val_off == BIT2_ਪ੍ਰਤੀ_ਸ਼ਬਦ) {
			val_off = 0;
			val_ptr += 1;
		  }
		  off += trans;
		  ਜੇ (off == ਬਿੱਟ_ਪ੍ਰਤੀ_ਸ਼ਬਦ) {
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
ਤਾਰਕਿਕ vvp_vector4_t::ਸੈੱਟ_vec(ਬਿਨਾਨਿਸ਼ਾਨੀ adr, ਅਚਲ vvp_vector4_t&that)
{
      ਪ੍ਰਤਿਜ੍ਞਾ(adr+that.ਮਾਪ_2  <= ਮਾਪ_2);
      ਤਾਰਕਿਕ diff_flag = ਝੂਠ;

      ਜੇ (ਮਾਪ_2 <= ਬਿੱਟ_ਪ੍ਰਤੀ_ਸ਼ਬਦ) {

	      /* The destination vector (me!) is within a bits_val_
		 word, so the subvector is certainly within a
		 bits_val_ word. Therefore, the entire operation is a
		 matter of writing the bits of that into the addressed
		 bits of this. The mask below is calculated to be 1
		 for all the bits that are to come from that. Do the
		 job by some shifting, masking and OR. */

	    ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ lmask = (1UL << adr) - 1;
	    ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ hmask;
	    ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ hshift = adr+that.ਮਾਪ_2;
	    ਜੇ (hshift >= ਬਿੱਟ_ਪ੍ਰਤੀ_ਸ਼ਬਦ)
		  hmask = -1UL;
	    ਨਹੀਂਤਾਂ
		  hmask = (1UL << (adr+that.ਮਾਪ_2)) - 1;
	    ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ ਨਕਾਬ = hmask & ~lmask;

	    ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ tmp = (that.abits_val_<<adr)&ਨਕਾਬ;
	    ਜੇ ((abits_val_&ਨਕਾਬ) != tmp) {
		  diff_flag = ਸੱਚ;
		  abits_val_ = (abits_val_ & ~ਨਕਾਬ) | tmp;
	    }
	    tmp = (that.bbits_val_<<adr) & ਨਕਾਬ;
	    ਜੇ ((bbits_val_&ਨਕਾਬ) != tmp) {
		  diff_flag = ਸੱਚ;
		  bbits_val_ = (bbits_val_ & ~ਨਕਾਬ) | tmp;
	    }

      } ਨਹੀਂਤਾਂ ਜੇ (that.ਮਾਪ_2 <= ਬਿੱਟ_ਪ੍ਰਤੀ_ਸ਼ਬਦ) {

	      /* This vector is more than a word, but that vector is
		 still small. Write into the destination, possibly
		 spanning two destination words, depending on whether
		 the source vector spans a word transition. */
	    ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ dptr = adr / ਬਿੱਟ_ਪ੍ਰਤੀ_ਸ਼ਬਦ;
	    ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ doff = adr % ਬਿੱਟ_ਪ੍ਰਤੀ_ਸ਼ਬਦ;

	    ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ lmask = (1UL << doff) - 1;
	    ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ hshift = doff+that.ਮਾਪ_2;
	    ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ hmask;
	    ਜੇ (hshift >= ਬਿੱਟ_ਪ੍ਰਤੀ_ਸ਼ਬਦ)
		  hmask = -1UL;
	    ਨਹੀਂਤਾਂ
		  hmask = (1UL << hshift) - 1UL;

	    ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ ਨਕਾਬ = hmask & ~lmask;
	    ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ tmp;

	    tmp = (that.abits_val_ << doff) & ਨਕਾਬ;
	    ਜੇ ((abits_ptr_[dptr] & ਨਕਾਬ) != tmp) {
		  diff_flag = ਸੱਚ;
		  abits_ptr_[dptr] = (abits_ptr_[dptr] & ~ਨਕਾਬ) | tmp;
	    }
	    tmp = (that.bbits_val_ << doff) & ਨਕਾਬ;
	    ਜੇ ((bbits_ptr_[dptr] & ਨਕਾਬ) != tmp) {
		  diff_flag = ਸੱਚ;
		  bbits_ptr_[dptr] = (bbits_ptr_[dptr] & ~ਨਕਾਬ) | tmp;
	    }

	    ਜੇ ((doff + that.ਮਾਪ_2) > ਬਿੱਟ_ਪ੍ਰਤੀ_ਸ਼ਬਦ) {
		  ਬਿਨਾਨਿਸ਼ਾਨੀ ਪੂਛ = doff + that.ਮਾਪ_2 - ਬਿੱਟ_ਪ੍ਰਤੀ_ਸ਼ਬਦ;
		  ਨਕਾਬ = (1UL << ਪੂਛ) - 1;

		  dptr += 1;
		  tmp = (that.abits_val_ >> (that.ਮਾਪ_2-ਪੂਛ)) & ਨਕਾਬ;
		  ਜੇ ((abits_ptr_[dptr] & ਨਕਾਬ) != tmp) {
			diff_flag = ਸੱਚ;
			abits_ptr_[dptr] = (abits_ptr_[dptr] & ~ਨਕਾਬ) | tmp;
		  }
		  tmp = (that.bbits_val_ >> (that.ਮਾਪ_2-ਪੂਛ)) & ਨਕਾਬ;
		  ਜੇ ((bbits_ptr_[dptr] & ਨਕਾਬ) != tmp) {
			diff_flag = ਸੱਚ;
			bbits_ptr_[dptr] = (bbits_ptr_[dptr] & ~ਨਕਾਬ) | tmp;
		  }
	    }

      } ਨਹੀਂਤਾਂ ਜੇ (adr%ਬਿੱਟ_ਪ੍ਰਤੀ_ਸ਼ਬਦ == 0) {

	      /* In this case, both vectors are long, but the
		 destination is neatly aligned. That means all but the
		 last word can be simply copied with no masking. */

	    ਬਿਨਾਨਿਸ਼ਾਨੀ remain = that.ਮਾਪ_2;
	    ਬਿਨਾਨਿਸ਼ਾਨੀ sptr = 0;
	    ਬਿਨਾਨਿਸ਼ਾਨੀ dptr = adr / ਬਿੱਟ_ਪ੍ਰਤੀ_ਸ਼ਬਦ;
	    ਜਦੋਂ (remain >= ਬਿੱਟ_ਪ੍ਰਤੀ_ਸ਼ਬਦ) {
		  ਜੇ (abits_ptr_[dptr] != that.abits_ptr_[sptr]) {
			diff_flag = ਸੱਚ;
			abits_ptr_[dptr] = that.abits_ptr_[sptr];
		  }
		  ਜੇ (bbits_ptr_[dptr] != that.bbits_ptr_[sptr]) {
			diff_flag = ਸੱਚ;
			bbits_ptr_[dptr] = that.bbits_ptr_[sptr];
		  }
		  dptr += 1;
		  sptr += 1;
		  remain -= ਬਿੱਟ_ਪ੍ਰਤੀ_ਸ਼ਬਦ;
	    }

	    ਜੇ (remain > 0) {
		  ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ ਨਕਾਬ = (1UL << remain) - 1;
		  ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ tmp;

		  tmp = that.abits_ptr_[sptr] & ਨਕਾਬ;
		  ਜੇ ((abits_ptr_[dptr] & ਨਕਾਬ) != tmp) {
			diff_flag = ਸੱਚ;
			abits_ptr_[dptr] = (abits_ptr_[dptr] & ~ਨਕਾਬ) | tmp;
		  }
		  tmp = that.bbits_ptr_[sptr] & ਨਕਾਬ;
		  ਜੇ ((bbits_ptr_[dptr] & ਨਕਾਬ) != tmp) {
			diff_flag = ਸੱਚ;
			bbits_ptr_[dptr] = (bbits_ptr_[dptr] & ~ਨਕਾਬ) | tmp;
		  }
	    }

      } ਨਹੀਂਤਾਂ {

	      /* We know that there are two long vectors, and we know
		 that the destination is definitely NOT aligned. */

	    ਬਿਨਾਨਿਸ਼ਾਨੀ remain = that.ਮਾਪ_2;
	    ਬਿਨਾਨਿਸ਼ਾਨੀ sptr = 0;
	    ਬਿਨਾਨਿਸ਼ਾਨੀ dptr = adr / ਬਿੱਟ_ਪ੍ਰਤੀ_ਸ਼ਬਦ;
	    ਬਿਨਾਨਿਸ਼ਾਨੀ doff = adr % ਬਿੱਟ_ਪ੍ਰਤੀ_ਸ਼ਬਦ;
	    ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ lmask = (1UL << doff) - 1;
	    ਬਿਨਾਨਿਸ਼ਾਨੀ ndoff = ਬਿੱਟ_ਪ੍ਰਤੀ_ਸ਼ਬਦ - doff;
	    ਜਦੋਂ (remain >= ਬਿੱਟ_ਪ੍ਰਤੀ_ਸ਼ਬਦ) {
		  ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ tmp;

		  tmp = (that.abits_ptr_[sptr] << doff) & ~lmask;
		  ਜੇ ((abits_ptr_[dptr] & ~lmask) != tmp) {
			diff_flag = ਸੱਚ;
			abits_ptr_[dptr] = (abits_ptr_[dptr] & lmask) | tmp;
		  }
		  tmp = (that.bbits_ptr_[sptr] << doff) & ~lmask;
		  ਜੇ ((bbits_ptr_[dptr] & ~lmask) != tmp) {
			diff_flag = ਸੱਚ;
			bbits_ptr_[dptr] = (bbits_ptr_[dptr] & lmask) | tmp;
		  }
		  dptr += 1;

		  tmp = (that.abits_ptr_[sptr] >> ndoff) & lmask;
		  ਜੇ ((abits_ptr_[dptr] & lmask) != tmp) {
			diff_flag = ਸੱਚ;
			abits_ptr_[dptr] = (abits_ptr_[dptr] & ~lmask) | tmp;
		  }
		  tmp = (that.bbits_ptr_[sptr] >> ndoff) & lmask;
		  ਜੇ ((bbits_ptr_[dptr] & lmask) != tmp) {
			diff_flag = ਸੱਚ;
			bbits_ptr_[dptr] = (bbits_ptr_[dptr] & ~lmask) | tmp;
		  }

		  remain -= ਬਿੱਟ_ਪ੍ਰਤੀ_ਸ਼ਬਦ;
		  sptr += 1;
	    }

	    ਜੇ (remain > 0) {
		  ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ hshift = doff+remain;
		  ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ hmask;
		  ਜੇ (hshift >= ਬਿੱਟ_ਪ੍ਰਤੀ_ਸ਼ਬਦ)
			hmask = -1UL;
		  ਨਹੀਂਤਾਂ
			hmask = (1UL << (doff+remain)) - 1;

		  ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ ਨਕਾਬ = hmask & ~lmask;
		  ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ tmp;

		  tmp = (that.abits_ptr_[sptr] << doff) & ਨਕਾਬ;
		  ਜੇ ((abits_ptr_[dptr] & ਨਕਾਬ) != tmp) {
			diff_flag = ਸੱਚ;
			abits_ptr_[dptr] = (abits_ptr_[dptr] & ~ਨਕਾਬ) | tmp;
		  }
		  tmp = (that.bbits_ptr_[sptr] << doff) & ਨਕਾਬ;
		  ਜੇ ((bbits_ptr_[dptr] & ਨਕਾਬ) != tmp) {
			diff_flag = ਸੱਚ;
			bbits_ptr_[dptr] = (bbits_ptr_[dptr] & ~ਨਕਾਬ) | tmp;
		  }

		  ਜੇ ((doff + remain) > ਬਿੱਟ_ਪ੍ਰਤੀ_ਸ਼ਬਦ) {
			ਬਿਨਾਨਿਸ਼ਾਨੀ ਪੂਛ = doff + remain - ਬਿੱਟ_ਪ੍ਰਤੀ_ਸ਼ਬਦ;
			ਜੇ (ਪੂਛ >= ਬਿੱਟ_ਪ੍ਰਤੀ_ਸ਼ਬਦ)
			      ਨਕਾਬ = -1UL;
			ਨਹੀਂਤਾਂ
			      ਨਕਾਬ = (1UL << ਪੂਛ) - 1;

			dptr += 1;

			tmp = (that.abits_ptr_[sptr] >> (remain-ਪੂਛ))&ਨਕਾਬ;
			ਜੇ ((abits_ptr_[dptr] & ਨਕਾਬ) != tmp) {
			      diff_flag = ਸੱਚ;
			      abits_ptr_[dptr] = (abits_ptr_[dptr] & ~ਨਕਾਬ) | tmp;
			}
			tmp = (that.bbits_ptr_[sptr] >> (remain-ਪੂਛ))&ਨਕਾਬ;
			ਜੇ ((bbits_ptr_[dptr] & ਨਕਾਬ) != tmp) {
			      diff_flag = ਸੱਚ;
			      bbits_ptr_[dptr] = (bbits_ptr_[dptr] & ~ਨਕਾਬ) | tmp;
			}
		  }
	    }
      }

      ਵਾਪਸ diff_flag;
}

/*
 * Add that vector to this vector. Do it in the Verilog way, which
 * means if we detect any X or Z bits, change the entire results to
 * all X.
 *
 * Assume both vectors are the same size.
 */
ਖਾਲੀ vvp_vector4_t::ਜੋੜੋ(ਅਚਲ vvp_vector4_t&that)
{
      ਪ੍ਰਤਿਜ੍ਞਾ(ਮਾਪ_2 == that.ਮਾਪ_2);

      ਜੇ (ਮਾਪ_2 < ਬਿੱਟ_ਪ੍ਰਤੀ_ਸ਼ਬਦ) {
	    ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ ਨਕਾਬ = ~(-1UL << ਮਾਪ_2);
	    ਜੇ ((bbits_val_|that.bbits_val_) & ਨਕਾਬ) {
		  abits_val_ |= ਨਕਾਬ;
		  bbits_val_ |= ਨਕਾਬ;
		  ਵਾਪਸ;
	    }

	    abits_val_ += that.abits_val_;
	    abits_val_ &= ਨਕਾਬ;
	    ਵਾਪਸ;
      }

      ਜੇ (ਮਾਪ_2 == ਬਿੱਟ_ਪ੍ਰਤੀ_ਸ਼ਬਦ) {
	    ਜੇ (bbits_val_ | that.bbits_val_) {
		  abits_val_ = ਸ਼ਬਦ_X_ABITS;
		  bbits_val_ = ਸ਼ਬਦ_X_BBITS;
	    } ਨਹੀਂਤਾਂ {
		  abits_val_ += that.abits_val_;
	    }
	    ਵਾਪਸ;
      }

      ਪੂਰਨ cnt = ਮਾਪ_2 / ਬਿੱਟ_ਪ੍ਰਤੀ_ਸ਼ਬਦ;
      ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ ਹਾਸਲ = 0;
      ਲਈ (ਪੂਰਨ idx = 0 ; idx < cnt ; idx += 1) {
	    ਜੇ (bbits_ptr_[idx] | that.bbits_ptr_[idx])
		  ਜਾਓ x_ਬਾਹਰਲਾ;

	    abits_ptr_[idx] = ਜੋੜੋ_with_ਹਾਸਲ(abits_ptr_[idx], that.abits_ptr_[idx], ਹਾਸਲ);
      }

      ਜੇ (ਬਿਨਾਨਿਸ਼ਾਨੀ ਪੂਛ = ਮਾਪ_2 % ਬਿੱਟ_ਪ੍ਰਤੀ_ਸ਼ਬਦ) {
	    ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ ਨਕਾਬ = ~( -1UL << ਪੂਛ );
	    ਜੇ ((bbits_ptr_[cnt] | that.bbits_ptr_[cnt])&ਨਕਾਬ)
		  ਜਾਓ x_ਬਾਹਰਲਾ;

	    abits_ptr_[cnt] = ਜੋੜੋ_with_ਹਾਸਲ(abits_ptr_[cnt], that.abits_ptr_[cnt], ਹਾਸਲ);
	    abits_ptr_[cnt] &= ਨਕਾਬ;
      }

      ਵਾਪਸ;

 x_ਬਾਹਰਲਾ:
      ਲਈ (ਪੂਰਨ idx = 0 ; idx < cnt ; idx += 1) {
	    abits_ptr_[idx] = ਸ਼ਬਦ_X_ABITS;
	    bbits_ptr_[idx] = ਸ਼ਬਦ_X_BBITS;
      }
      ਜੇ (ਬਿਨਾਨਿਸ਼ਾਨੀ ਪੂਛ = ਮਾਪ_2%ਬਿੱਟ_ਪ੍ਰਤੀ_ਸ਼ਬਦ) {
	    ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ ਨਕਾਬ = ~( -1UL << ਪੂਛ );
	    abits_ptr_[cnt] = ਸ਼ਬਦ_X_ABITS&ਨਕਾਬ;
	    bbits_ptr_[cnt] = ਸ਼ਬਦ_X_BBITS&ਨਕਾਬ;
      }
}

ਖਾਲੀ vvp_vector4_t::sub(ਅਚਲ vvp_vector4_t&that)
{
      ਪ੍ਰਤਿਜ੍ਞਾ(ਮਾਪ_2 == that.ਮਾਪ_2);

      ਜੇ (ਮਾਪ_2 < ਬਿੱਟ_ਪ੍ਰਤੀ_ਸ਼ਬਦ) {
	    ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ ਨਕਾਬ = ~(-1UL << ਮਾਪ_2);
	    ਜੇ ((bbits_val_|that.bbits_val_) & ਨਕਾਬ) {
		  abits_val_ |= ਨਕਾਬ;
		  bbits_val_ |= ਨਕਾਬ;
		  ਵਾਪਸ;
	    }

	    abits_val_ -= that.abits_val_;
	    abits_val_ &= ਨਕਾਬ;
	    ਵਾਪਸ;
      }

      ਜੇ (ਮਾਪ_2 == ਬਿੱਟ_ਪ੍ਰਤੀ_ਸ਼ਬਦ) {
	    ਜੇ (bbits_val_ | that.bbits_val_) {
		  abits_val_ = ਸ਼ਬਦ_X_ABITS;
		  bbits_val_ = ਸ਼ਬਦ_X_BBITS;
	    } ਨਹੀਂਤਾਂ {
		  abits_val_ -= that.abits_val_;
	    }
	    ਵਾਪਸ;
      }

      ਪੂਰਨ cnt = ਮਾਪ_2 / ਬਿੱਟ_ਪ੍ਰਤੀ_ਸ਼ਬਦ;
      ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ ਹਾਸਲ = 1;
      ਲਈ (ਪੂਰਨ idx = 0 ; idx < cnt ; idx += 1) {
	    ਜੇ (bbits_ptr_[idx] | that.bbits_ptr_[idx])
		  ਜਾਓ x_ਬਾਹਰਲਾ;

	    abits_ptr_[idx] = ਜੋੜੋ_with_ਹਾਸਲ(abits_ptr_[idx], ~that.abits_ptr_[idx], ਹਾਸਲ);
      }

      ਜੇ (ਬਿਨਾਨਿਸ਼ਾਨੀ ਪੂਛ = ਮਾਪ_2 % ਬਿੱਟ_ਪ੍ਰਤੀ_ਸ਼ਬਦ) {
	    ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ ਨਕਾਬ = ~( -1UL << ਪੂਛ );
	    ਜੇ ((bbits_ptr_[cnt] | that.bbits_ptr_[cnt])&ਨਕਾਬ)
		  ਜਾਓ x_ਬਾਹਰਲਾ;

	    abits_ptr_[cnt] = ਜੋੜੋ_with_ਹਾਸਲ(abits_ptr_[cnt], ~that.abits_ptr_[cnt], ਹਾਸਲ);
	    abits_ptr_[cnt] &= ਨਕਾਬ;
      }

      ਵਾਪਸ;

 x_ਬਾਹਰਲਾ:
      ਲਈ (ਪੂਰਨ idx = 0 ; idx < cnt ; idx += 1) {
	    abits_ptr_[idx] = ਸ਼ਬਦ_X_ABITS;
	    bbits_ptr_[idx] = ਸ਼ਬਦ_X_BBITS;
      }
      ਜੇ (ਬਿਨਾਨਿਸ਼ਾਨੀ ਪੂਛ = ਮਾਪ_2%ਬਿੱਟ_ਪ੍ਰਤੀ_ਸ਼ਬਦ) {
	    ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ ਨਕਾਬ = ~( -1UL << ਪੂਛ );
	    abits_ptr_[cnt] = ਸ਼ਬਦ_X_ABITS&ਨਕਾਬ;
	    bbits_ptr_[cnt] = ਸ਼ਬਦ_X_BBITS&ਨਕਾਬ;
      }

}

ਖਾਲੀ vvp_vector4_t::mov(ਬਿਨਾਨਿਸ਼ਾਨੀ dst, ਬਿਨਾਨਿਸ਼ਾਨੀ src, ਬਿਨਾਨਿਸ਼ਾਨੀ cnt)
{
      ਪ੍ਰਤਿਜ੍ਞਾ(dst+cnt <= ਮਾਪ_2);
      ਪ੍ਰਤਿਜ੍ਞਾ(src+cnt <= ਮਾਪ_2);

      ਜੇ (ਮਾਪ_2 <= ਬਿੱਟ_ਪ੍ਰਤੀ_ਸ਼ਬਦ) {
	    ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ vmask = (1UL << cnt) - 1;
	    ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ tmp;

	    tmp = (abits_val_ >> src) & vmask;
	    abits_val_ &= ~ (vmask << dst);
	    abits_val_ |= tmp << dst;

	    tmp = (bbits_val_ >> src) & vmask;
	    bbits_val_ &= ~ (vmask << dst);
	    bbits_val_ |= tmp << dst;

      } ਨਹੀਂਤਾਂ {
	    ਬਿਨਾਨਿਸ਼ਾਨੀ sptr = src / ਬਿੱਟ_ਪ੍ਰਤੀ_ਸ਼ਬਦ;
	    ਬਿਨਾਨਿਸ਼ਾਨੀ dptr = dst / ਬਿੱਟ_ਪ੍ਰਤੀ_ਸ਼ਬਦ;
	    ਬਿਨਾਨਿਸ਼ਾਨੀ soff = src % ਬਿੱਟ_ਪ੍ਰਤੀ_ਸ਼ਬਦ;
	    ਬਿਨਾਨਿਸ਼ਾਨੀ doff = dst % ਬਿੱਟ_ਪ੍ਰਤੀ_ਸ਼ਬਦ;

	    ਜਦੋਂ (cnt > 0) {
		  ਬਿਨਾਨਿਸ਼ਾਨੀ trans = cnt;
		  ਜੇ ((soff+trans) > ਬਿੱਟ_ਪ੍ਰਤੀ_ਸ਼ਬਦ)
			trans = ਬਿੱਟ_ਪ੍ਰਤੀ_ਸ਼ਬਦ - soff;

		  ਜੇ ((doff+trans) > ਬਿੱਟ_ਪ੍ਰਤੀ_ਸ਼ਬਦ)
			trans = ਬਿੱਟ_ਪ੍ਰਤੀ_ਸ਼ਬਦ - doff;

		  ਜੇ (trans == ਬਿੱਟ_ਪ੍ਰਤੀ_ਸ਼ਬਦ) {
			  // Special case: the transfer count is
			  // exactly an entire word. For this to be
			  // true, it must also be true that the
			  // pointers are aligned. The work is easy,
			abits_ptr_[dptr] = abits_ptr_[sptr];
			bbits_ptr_[dptr] = bbits_ptr_[sptr];
			dptr += 1;
			sptr += 1;
			cnt -= ਬਿੱਟ_ਪ੍ਰਤੀ_ਸ਼ਬਦ;
			ਜਾਰੀ;
		  }

		    // Here we know that either the source or
		    // destination is unaligned, and also we know that
		    // the count is less than a full word.
		  ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ vmask = (1UL << trans) - 1;
		  ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ tmp;

		  tmp = (abits_ptr_[sptr] >> soff) & vmask;
		  abits_ptr_[dptr] &= ~ (vmask << doff);
		  abits_ptr_[dptr] |= tmp << doff;

		  tmp = (bbits_ptr_[sptr] >> soff) & vmask;
		  bbits_ptr_[dptr] &= ~ (vmask << doff);
		  bbits_ptr_[dptr] |= tmp << doff;

		  cnt -= trans;
		  soff += trans;
		  ਜੇ (soff >= ਬਿੱਟ_ਪ੍ਰਤੀ_ਸ਼ਬਦ) {
			soff = 0;
			sptr += 1;
		  }
		  doff += trans;
		  ਜੇ (doff >= ਬਿੱਟ_ਪ੍ਰਤੀ_ਸ਼ਬਦ) {
			doff = 0;
			dptr += 1;
		  }
	    }
      }
}

ਖਾਲੀ vvp_vector4_t::mul(ਅਚਲ vvp_vector4_t&that)
{
      ਪ੍ਰਤਿਜ੍ਞਾ(ਮਾਪ_2 == that.ਮਾਪ_2);

      ਜੇ (ਮਾਪ_2 < ਬਿੱਟ_ਪ੍ਰਤੀ_ਸ਼ਬਦ) {
	    ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ ਨਕਾਬ = ~(-1UL << ਮਾਪ_2);
	    ਜੇ ((bbits_val_|that.bbits_val_) & ਨਕਾਬ) {
		  abits_val_ |= ਨਕਾਬ;
		  bbits_val_ |= ਨਕਾਬ;
		  ਵਾਪਸ;
	    }

	    abits_val_ *= that.abits_val_;
	    abits_val_ &= ਨਕਾਬ;
	    ਵਾਪਸ;
      }

      ਜੇ (ਮਾਪ_2 == ਬਿੱਟ_ਪ੍ਰਤੀ_ਸ਼ਬਦ) {
	    ਜੇ (bbits_val_ || that.bbits_val_) {
		  abits_val_ = ਸ਼ਬਦ_X_ABITS;
		  bbits_val_ = ਸ਼ਬਦ_X_BBITS;
	    } ਨਹੀਂਤਾਂ {
		  abits_val_ *= that.abits_val_;
	    }
	    ਵਾਪਸ;
      }

      ਅਚਲ ਪੂਰਨ cnt = (ਮਾਪ_2+ਬਿੱਟ_ਪ੍ਰਤੀ_ਸ਼ਬਦ-1) / ਬਿੱਟ_ਪ੍ਰਤੀ_ਸ਼ਬਦ;

      ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ ਨਕਾਬ;
      ਜੇ (ਬਿਨਾਨਿਸ਼ਾਨੀ ਪੂਛ = ਮਾਪ_2%ਬਿੱਟ_ਪ੍ਰਤੀ_ਸ਼ਬਦ) {
	    ਨਕਾਬ = ~( -1UL << ਪੂਛ );
      } ਨਹੀਂਤਾਂ {
	    ਨਕਾਬ = ~0UL;
      }

	// Check for any XZ values ahead of time in a first pass. If
	// we find any, then force the entire result to be X and be
	// done.
      ਲਈ (ਪੂਰਨ idx = 0 ; idx < cnt ; idx += 1) {
	    ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ lval = bbits_ptr_[idx];
	    ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ rval = that.bbits_ptr_[idx];
	    ਜੇ (idx == (cnt-1)) {
		  lval &= ਨਕਾਬ;
		  rval &= ਨਕਾਬ;
	    }
	    ਜੇ (lval || rval) {
		  ਲਈ (ਪੂਰਨ xdx = 0 ; xdx < cnt-1 ; xdx += 1) {
			abits_ptr_[xdx] = ਸ਼ਬਦ_X_ABITS;
			bbits_ptr_[xdx] = ਸ਼ਬਦ_X_BBITS;
		  }
		  abits_ptr_[cnt-1] = ਸ਼ਬਦ_X_ABITS & ਨਕਾਬ;
		  bbits_ptr_[cnt-1] = ਸ਼ਬਦ_X_BBITS & ਨਕਾਬ;
		  ਵਾਪਸ;
	    }
      }

	// Calculate the result into a res array. We need to keep is
	// separate from the "this" array because we are making
	// multiple passes.
      ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ*res = ਨਵਾਂ ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ[cnt];
      ਲਈ (ਪੂਰਨ idx = 0 ; idx < cnt ; idx += 1)
	    res[idx] = 0;

      ਲਈ (ਪੂਰਨ mul_a = 0 ; mul_a < cnt ; mul_a += 1) {
	    ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ lval = abits_ptr_[mul_a];
	    ਜੇ (mul_a == (cnt-1))
		  lval &= ਨਕਾਬ;

	    ਲਈ (ਪੂਰਨ mul_b = 0 ; mul_b < (cnt-mul_a) ; mul_b += 1) {
		  ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ rval = that.abits_ptr_[mul_b];
		  ਜੇ (mul_b == (cnt-1))
			rval &= ਨਕਾਬ;

		  ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ ਜੋੜ;
		  ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ tmp = multiply_with_ਹਾਸਲ(lval, rval, ਜੋੜ);
		  ਪੂਰਨ ਅਧਾਰ = mul_a + mul_b;
		  ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ ਹਾਸਲ = 0;
		  res[ਅਧਾਰ] = ਜੋੜੋ_with_ਹਾਸਲ(res[ਅਧਾਰ], tmp, ਹਾਸਲ);
		  ਲਈ (ਪੂਰਨ ਜੋੜੋ_idx = ਅਧਾਰ+1 ; ਜੋੜੋ_idx < cnt ; ਜੋੜੋ_idx += 1) {
			res[ਜੋੜੋ_idx] = ਜੋੜੋ_with_ਹਾਸਲ(res[ਜੋੜੋ_idx], ਜੋੜ, ਹਾਸਲ);
			ਜੋੜ = 0;
		  }
	    }
      }

	// Replace the "this" value with the calculated result. We
	// know a-priori that the bbits are zero and unchanged.
      res[cnt-1] &= ਨਕਾਬ;
      ਲਈ (ਪੂਰਨ idx = 0 ; idx < cnt ; idx += 1)
	    abits_ptr_[idx] = res[idx];

      ਮਿਟਾਓ[]res;
      ਵਾਪਸ;


}

ਤਾਰਕਿਕ vvp_vector4_t::eeq(ਅਚਲ vvp_vector4_t&that) ਅਚਲ
{
      ਜੇ (ਮਾਪ_2 != that.ਮਾਪ_2)
	    ਵਾਪਸ ਝੂਠ;

      ਜੇ (ਮਾਪ_2 < ਬਿੱਟ_ਪ੍ਰਤੀ_ਸ਼ਬਦ) {
	    ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ ਨਕਾਬ = (1UL << ਮਾਪ_2) - 1;
	    ਵਾਪਸ (abits_val_&ਨਕਾਬ) == (that.abits_val_&ਨਕਾਬ)
		  && (bbits_val_&ਨਕਾਬ) == (that.bbits_val_&ਨਕਾਬ);
      }

      ਜੇ (ਮਾਪ_2 == ਬਿੱਟ_ਪ੍ਰਤੀ_ਸ਼ਬਦ) {
	    ਵਾਪਸ (abits_val_ == that.abits_val_)
		  && (bbits_val_ == that.bbits_val_);
      }

      ਬਿਨਾਨਿਸ਼ਾਨੀ ਸ਼ਬਦ = ਮਾਪ_2 / ਬਿੱਟ_ਪ੍ਰਤੀ_ਸ਼ਬਦ;
      ਲਈ (ਬਿਨਾਨਿਸ਼ਾਨੀ idx = 0 ;  idx < ਸ਼ਬਦ ;  idx += 1) {
	    ਜੇ (abits_ptr_[idx] != that.abits_ptr_[idx])
		  ਵਾਪਸ ਝੂਠ;
	    ਜੇ (bbits_ptr_[idx] != that.bbits_ptr_[idx])
		  ਵਾਪਸ ਝੂਠ;
      }

      ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ ਨਕਾਬ = ਮਾਪ_2%ਬਿੱਟ_ਪ੍ਰਤੀ_ਸ਼ਬਦ;
      ਜੇ (ਨਕਾਬ > 0) {
	    ਨਕਾਬ = (1UL << ਨਕਾਬ) - 1;
	    ਵਾਪਸ (abits_ptr_[ਸ਼ਬਦ]&ਨਕਾਬ) == (that.abits_ptr_[ਸ਼ਬਦ]&ਨਕਾਬ)
		  && (bbits_ptr_[ਸ਼ਬਦ]&ਨਕਾਬ) == (that.bbits_ptr_[ਸ਼ਬਦ]&ਨਕਾਬ);
      }

      ਵਾਪਸ ਸੱਚ;
}

ਤਾਰਕਿਕ vvp_vector4_t::eq_xz(ਅਚਲ vvp_vector4_t&that) ਅਚਲ
{
      ਜੇ (ਮਾਪ_2 != that.ਮਾਪ_2)
	    ਵਾਪਸ ਝੂਠ;

      ਜੇ (ਮਾਪ_2 < ਬਿੱਟ_ਪ੍ਰਤੀ_ਸ਼ਬਦ) {
	    ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ ਨਕਾਬ = (1UL << ਮਾਪ_2) - 1;
	    ਵਾਪਸ ((abits_val_|bbits_val_)&ਨਕਾਬ) == ((that.abits_val_|that.bbits_val_)&ਨਕਾਬ)
		  && (bbits_val_&ਨਕਾਬ) == (that.bbits_val_&ਨਕਾਬ);
      }

      ਜੇ (ਮਾਪ_2 == ਬਿੱਟ_ਪ੍ਰਤੀ_ਸ਼ਬਦ) {
	    ਵਾਪਸ ((abits_val_|bbits_val_) == (that.abits_val_|that.bbits_val_))
		  && (bbits_val_ == that.bbits_val_);
      }

      ਬਿਨਾਨਿਸ਼ਾਨੀ ਸ਼ਬਦ = ਮਾਪ_2 / ਬਿੱਟ_ਪ੍ਰਤੀ_ਸ਼ਬਦ;
      ਲਈ (ਬਿਨਾਨਿਸ਼ਾਨੀ idx = 0 ;  idx < ਸ਼ਬਦ ;  idx += 1) {
	    ਜੇ ((abits_ptr_[idx]|bbits_ptr_[idx]) != (that.abits_ptr_[idx]|that.bbits_ptr_[idx]))
		  ਵਾਪਸ ਝੂਠ;
	    ਜੇ (bbits_ptr_[idx] != that.bbits_ptr_[idx])
		  ਵਾਪਸ ਝੂਠ;
      }

      ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ ਨਕਾਬ = ਮਾਪ_2%ਬਿੱਟ_ਪ੍ਰਤੀ_ਸ਼ਬਦ;
      ਜੇ (ਨਕਾਬ > 0) {
	    ਨਕਾਬ = (1UL << ਨਕਾਬ) - 1;
	    ਵਾਪਸ ((abits_ptr_[ਸ਼ਬਦ]|bbits_ptr_[ਸ਼ਬਦ])&ਨਕਾਬ) == ((that.abits_ptr_[ਸ਼ਬਦ]|that.bbits_ptr_[ਸ਼ਬਦ])&ਨਕਾਬ)
		  && (bbits_ptr_[ਸ਼ਬਦ]&ਨਕਾਬ) == (that.bbits_ptr_[ਸ਼ਬਦ]&ਨਕਾਬ);
      }

      ਵਾਪਸ ਸੱਚ;
}

ਤਾਰਕਿਕ vvp_vector4_t::has_xz() ਅਚਲ
{
      ਜੇ (ਮਾਪ_2 < ਬਿੱਟ_ਪ੍ਰਤੀ_ਸ਼ਬਦ) {
	    ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ ਨਕਾਬ = -1UL >> (ਬਿੱਟ_ਪ੍ਰਤੀ_ਸ਼ਬਦ - ਮਾਪ_2);
	    ਵਾਪਸ bbits_val_&ਨਕਾਬ;
      }

      ਜੇ (ਮਾਪ_2 == ਬਿੱਟ_ਪ੍ਰਤੀ_ਸ਼ਬਦ) {
	    ਵਾਪਸ bbits_val_;
      }

      ਬਿਨਾਨਿਸ਼ਾਨੀ ਸ਼ਬਦ = ਮਾਪ_2 / ਬਿੱਟ_ਪ੍ਰਤੀ_ਸ਼ਬਦ;
      ਲਈ (ਬਿਨਾਨਿਸ਼ਾਨੀ idx = 0 ; idx < ਸ਼ਬਦ ; idx += 1) {
	    ਜੇ (bbits_ptr_[idx])
		  ਵਾਪਸ ਸੱਚ;
      }

      ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ ਨਕਾਬ = ਮਾਪ_2%ਬਿੱਟ_ਪ੍ਰਤੀ_ਸ਼ਬਦ;
      ਜੇ (ਨਕਾਬ > 0) {
	    ਨਕਾਬ = -1UL >> (ਬਿੱਟ_ਪ੍ਰਤੀ_ਸ਼ਬਦ - ਨਕਾਬ);
	    ਵਾਪਸ bbits_ptr_[ਸ਼ਬਦ]&ਨਕਾਬ;
      }

      ਵਾਪਸ ਝੂਠ;
}

ਖਾਲੀ vvp_vector4_t::change_z2x()
{
	// This method relies on the fact that both BIT4_X and BIT4_Z
	// have the bbit set in the vector4 encoding, and also that
	// the BIT4_X has abit set in the vector4 encoding. By simply
	// or-ing the bbit into the abit, BIT4_X and BIT4_Z both
	// become BIT4_X.

      ਜੇ (ਮਾਪ_2 <= ਬਿੱਟ_ਪ੍ਰਤੀ_ਸ਼ਬਦ) {
	    abits_val_ |= bbits_val_;
      } ਨਹੀਂਤਾਂ {
	    ਬਿਨਾਨਿਸ਼ਾਨੀ ਸ਼ਬਦ = (ਮਾਪ_2+ਬਿੱਟ_ਪ੍ਰਤੀ_ਸ਼ਬਦ-1) / ਬਿੱਟ_ਪ੍ਰਤੀ_ਸ਼ਬਦ;
	    ਲਈ (ਬਿਨਾਨਿਸ਼ਾਨੀ idx = 0 ;  idx < ਸ਼ਬਦ ;  idx += 1)
		  abits_ptr_[idx] |= bbits_ptr_[idx];
      }
}

ਖਾਲੀ vvp_vector4_t::fill_ਬਿੱਟ(vvp_bit4_t ਦੋਆਧਾਰੀ_ਅੰਕ)
{
	/* note: this relies on the bit encoding for the vvp_bit4_t. */
      ਸਥਿਰ ਅਚਲ ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ init_atable[4] = {
	    ਸ਼ਬਦ_0_ABITS,
	    ਸ਼ਬਦ_1_ABITS,
	    ਸ਼ਬਦ_Z_ABITS,
	    ਸ਼ਬਦ_X_ABITS };
      ਸਥਿਰ ਅਚਲ ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ init_btable[4] = {
	    ਸ਼ਬਦ_0_BBITS,
	    ਸ਼ਬਦ_1_BBITS,
	    ਸ਼ਬਦ_Z_BBITS,
	    ਸ਼ਬਦ_X_BBITS };

      ਜੇ (ਮਾਪ_2 <= ਬਿੱਟ_ਪ੍ਰਤੀ_ਸ਼ਬਦ) {
	    abits_val_ = init_atable[ਦੋਆਧਾਰੀ_ਅੰਕ];
	    bbits_val_ = init_btable[ਦੋਆਧਾਰੀ_ਅੰਕ];
      } ਨਹੀਂਤਾਂ {
	    ਬਿਨਾਨਿਸ਼ਾਨੀ ਸ਼ਬਦ = (ਮਾਪ_2 + ਬਿੱਟ_ਪ੍ਰਤੀ_ਸ਼ਬਦ - 1) / ਬਿੱਟ_ਪ੍ਰਤੀ_ਸ਼ਬਦ;
	    ਲਈ (ਬਿਨਾਨਿਸ਼ਾਨੀ idx = 0; idx < ਸ਼ਬਦ; idx += 1) {
		  abits_ptr_[idx] = init_atable[ਦੋਆਧਾਰੀ_ਅੰਕ];
		  bbits_ptr_[idx] = init_btable[ਦੋਆਧਾਰੀ_ਅੰਕ];
	    }
      }
}

ਅੱਖਰ* vvp_vector4_t::as_string(ਅੱਖਰ*buf, size_t buf_len) ਅਚਲ
{
      ਅੱਖਰ*res = buf;
      *buf++ = 'C';
      *buf++ = '4';
      *buf++ = '<';
      buf_len -= 3;

      ਲਈ (ਬਿਨਾਨਿਸ਼ਾਨੀ idx = 0 ;  idx < size() && buf_len >= 2 ;  idx += 1) {
	    ਚੋਣ (value(size()-idx-1)) {
		ਹਾਲਤ BIT4_0:
		  *buf++ = '0';
		  ਰੋਕੋ;
		ਹਾਲਤ BIT4_1:
		  *buf++ = '1';
		  ਰੋਕੋ;
		ਹਾਲਤ BIT4_X:
		  *buf++ = 'x';
		  ਰੋਕੋ;
		ਹਾਲਤ BIT4_Z:
		  *buf++ = 'z';
	    }
	    buf_len -= 1;
      }

      *buf++ = '>';
      *buf++ = 0;
      ਵਾਪਸ res;
}

ਖਾਲੀ vvp_vector4_t::invert()
{
      ਜੇ (ਮਾਪ_2 <= ਬਿੱਟ_ਪ੍ਰਤੀ_ਸ਼ਬਦ) {
	    ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ ਨਕਾਬ = (ਮਾਪ_2<ਬਿੱਟ_ਪ੍ਰਤੀ_ਸ਼ਬਦ)? (1UL<<ਮਾਪ_2)-1UL : -1UL;
	    abits_val_ = ਨਕਾਬ & ~abits_val_;
	    abits_val_ |= bbits_val_;
      } ਨਹੀਂਤਾਂ {
	    ਬਿਨਾਨਿਸ਼ਾਨੀ ਬਚਿਆ = ਮਾਪ_2;
	    ਬਿਨਾਨਿਸ਼ਾਨੀ idx = 0;
	    ਜਦੋਂ (ਬਚਿਆ >= ਬਿੱਟ_ਪ੍ਰਤੀ_ਸ਼ਬਦ) {
		  abits_ptr_[idx] = ~abits_ptr_[idx];
		  abits_ptr_[idx] |= bbits_ptr_[idx];
		  idx += 1;
		  ਬਚਿਆ -= ਬਿੱਟ_ਪ੍ਰਤੀ_ਸ਼ਬਦ;
	    }
	    ਜੇ (ਬਚਿਆ > 0) {
		  ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ ਨਕਾਬ = (1UL<<ਬਚਿਆ) - 1UL;
		  abits_ptr_[idx] = ਨਕਾਬ & ~abits_ptr_[idx];
		  abits_ptr_[idx] |= bbits_ptr_[idx];
	    }
      }
}

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ ਦੋਆਧਾਰੀ_ਅੰਕ_ਨਕਾਬ(n) ((n) ? ((~0UL) >> (ਬਿੱਟ_ਪ੍ਰਤੀ_ਸ਼ਬਦ - (n))) : ~0UL)

vvp_bit4_t vvp_vector4_t::reduce_or() ਅਚਲ
{
      ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ ਨਕਾਬ = ਦੋਆਧਾਰੀ_ਅੰਕ_ਨਕਾਬ(ਮਾਪ_2 % ਬਿੱਟ_ਪ੍ਰਤੀ_ਸ਼ਬਦ);
      vvp_bit4_t res = BIT4_0;

      ਜੇ (ਮਾਪ_2 <= ਬਿੱਟ_ਪ੍ਰਤੀ_ਸ਼ਬਦ) {
	    ਜੇ ((abits_val_ & ~bbits_val_ & ਨਕਾਬ) != 0UL)
		  ਵਾਪਸ BIT4_1;
	    ਜੇ ((bbits_val_ & ਨਕਾਬ) != 0UL)
		  ਵਾਪਸ BIT4_X;
      } ਨਹੀਂਤਾਂ {
	    ਬਿਨਾਨਿਸ਼ਾਨੀ ਸ਼ਬਦ = (ਮਾਪ_2 + ਬਿੱਟ_ਪ੍ਰਤੀ_ਸ਼ਬਦ - 1) / ਬਿੱਟ_ਪ੍ਰਤੀ_ਸ਼ਬਦ;
	    ਬਿਨਾਨਿਸ਼ਾਨੀ idx;
	    ਲਈ (idx = 0; idx < ਸ਼ਬਦ - 1; idx += 1) {
		  ਜੇ ((abits_ptr_[idx] & ~bbits_ptr_[idx]) != 0UL)
			ਵਾਪਸ BIT4_1;
		  ਜੇ (bbits_ptr_[idx] != 0UL)
			res = BIT4_X;
	    }
	    ਜੇ ((abits_ptr_[idx] & ~bbits_ptr_[idx] & ਨਕਾਬ) != 0UL)
		  ਵਾਪਸ BIT4_1;
	    ਜੇ ((bbits_ptr_[idx] & ਨਕਾਬ) != 0UL)
		  res = BIT4_X;
      }

      ਵਾਪਸ res;
}

vvp_bit4_t vvp_vector4_t::reduce_and() ਅਚਲ
{
      ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ ਨਕਾਬ = ਦੋਆਧਾਰੀ_ਅੰਕ_ਨਕਾਬ(ਮਾਪ_2 % ਬਿੱਟ_ਪ੍ਰਤੀ_ਸ਼ਬਦ);
      vvp_bit4_t res = BIT4_1;

      ਜੇ (ਮਾਪ_2 <= ਬਿੱਟ_ਪ੍ਰਤੀ_ਸ਼ਬਦ) {
	    ਜੇ ((abits_val_ | bbits_val_ | ~ਨਕਾਬ) != ~0UL)
		  ਵਾਪਸ BIT4_0;
	    ਜੇ ((bbits_val_ & ਨਕਾਬ) != 0UL)
		  ਵਾਪਸ BIT4_X;
	} ਨਹੀਂਤਾਂ {
	    ਬਿਨਾਨਿਸ਼ਾਨੀ ਸ਼ਬਦ = (ਮਾਪ_2 + ਬਿੱਟ_ਪ੍ਰਤੀ_ਸ਼ਬਦ - 1) / ਬਿੱਟ_ਪ੍ਰਤੀ_ਸ਼ਬਦ;
	    ਬਿਨਾਨਿਸ਼ਾਨੀ idx;
	    ਲਈ (idx = 0; idx < ਸ਼ਬਦ - 1; idx += 1) {
		  ਜੇ ((abits_ptr_[idx] | bbits_ptr_[idx]) != ~0UL)
			ਵਾਪਸ BIT4_0;
		  ਜੇ (bbits_ptr_[idx] != 0UL)
			res = BIT4_X;
	    }
	    ਜੇ ((abits_ptr_[idx] | bbits_ptr_[idx] | ~ਨਕਾਬ) != ~0UL)
		  ਵਾਪਸ BIT4_0;
	    ਜੇ ((bbits_ptr_[idx] & ਨਕਾਬ) != 0UL)
		  res = BIT4_X;
      }

      ਵਾਪਸ res;
}

ਸਥਿਰ ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ parity(ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ val)
{
#ਜੇ defined(__GNUC__)
      // The compiler builtin can use target-specific CPU instructions.
      ਵਾਪਸ __builtin_parityl(val);
#ਨਹੀਂਤਾਂ
#ਜੇ ULONG_MAX > 0xffffffffUL
	val ^= val >> 32;
#ਜੇ_ਅੰਤ
	val ^= val >> 16;
	val ^= val >> 8;
	val ^= val >> 4;

      ਵਾਪਸ (0x6996 >> (val & 0xf)) & 1;
#ਜੇ_ਅੰਤ
}

vvp_bit4_t vvp_vector4_t::reduce_xor() ਅਚਲ
{
      ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ ਨਕਾਬ = ਦੋਆਧਾਰੀ_ਅੰਕ_ਨਕਾਬ(ਮਾਪ_2 % ਬਿੱਟ_ਪ੍ਰਤੀ_ਸ਼ਬਦ);

      ਜੇ (ਮਾਪ_2 <= ਬਿੱਟ_ਪ੍ਰਤੀ_ਸ਼ਬਦ) {
	    ਜੇ ((bbits_val_ & ਨਕਾਬ) != 0UL)
		  ਵਾਪਸ BIT4_X;
	    ਵਾਪਸ parity(abits_val_ & ਨਕਾਬ) ? BIT4_1 : BIT4_0;
      } ਨਹੀਂਤਾਂ {
	    ਬਿਨਾਨਿਸ਼ਾਨੀ ਸ਼ਬਦ = (ਮਾਪ_2 + ਬਿੱਟ_ਪ੍ਰਤੀ_ਸ਼ਬਦ - 1) / ਬਿੱਟ_ਪ੍ਰਤੀ_ਸ਼ਬਦ;
	    ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ val_a = 0UL;
	    ਬਿਨਾਨਿਸ਼ਾਨੀ idx;
	    ਲਈ (idx = 0; idx < ਸ਼ਬਦ - 1; idx += 1) {
		  ਜੇ (bbits_ptr_[idx] != 0UL)
			ਵਾਪਸ BIT4_X;
		  val_a ^= abits_ptr_[idx];
	    }
	    ਜੇ ((bbits_ptr_[idx] & ਨਕਾਬ) != 0UL)
		  ਵਾਪਸ BIT4_X;
	    val_a ^= abits_ptr_[idx] & ਨਕਾਬ;
	    ਵਾਪਸ parity(val_a) ? BIT4_1 : BIT4_0;
      }
}

vvp_vector4_t& vvp_vector4_t::ਸੰਚਾਲਕ &= (ਅਚਲ vvp_vector4_t&that)
{
	// The truth table is:
	//     00 01 11 10
	//  00 00 00 00 00
	//  01 00 01 11 11
	//  11 00 11 11 11
	//  10 00 11 11 11
      ਜੇ (ਮਾਪ_2 <= ਬਿੱਟ_ਪ੍ਰਤੀ_ਸ਼ਬਦ) {
	    ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ tmp1 = abits_val_ | bbits_val_;
	    ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ tmp2 = that.abits_val_ | that.bbits_val_;
	    abits_val_ = tmp1 & tmp2;
	    bbits_val_ = (tmp1 & that.bbits_val_) | (tmp2 & bbits_val_);
      } ਨਹੀਂਤਾਂ {
	    ਬਿਨਾਨਿਸ਼ਾਨੀ ਸ਼ਬਦ = (ਮਾਪ_2 + ਬਿੱਟ_ਪ੍ਰਤੀ_ਸ਼ਬਦ - 1) / ਬਿੱਟ_ਪ੍ਰਤੀ_ਸ਼ਬਦ;
	    ਲਈ (ਬਿਨਾਨਿਸ਼ਾਨੀ idx = 0; idx < ਸ਼ਬਦ ; idx += 1) {
		  ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ tmp1 = abits_ptr_[idx] | bbits_ptr_[idx];
		  ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ tmp2 = that.abits_ptr_[idx] |
		                       that.bbits_ptr_[idx];
		  abits_ptr_[idx] = tmp1 & tmp2;
		  bbits_ptr_[idx] = (tmp1 & that.bbits_ptr_[idx]) |
		                    (tmp2 & bbits_ptr_[idx]);
	    }
      }

      ਵਾਪਸ *ਇਹ;
}

vvp_vector4_t& vvp_vector4_t::ਸੰਚਾਲਕ |= (ਅਚਲ vvp_vector4_t&that)
{
	// The truth table is:
	//     00 01 11 10
	//  00 00 01 11 11
	//  01 01 01 01 01
	//  11 11 01 11 11
	//  10 11 01 11 11
      ਜੇ (ਮਾਪ_2 <= ਬਿੱਟ_ਪ੍ਰਤੀ_ਸ਼ਬਦ) {
	    ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ tmp = abits_val_ | bbits_val_ |
	                        that.abits_val_ | that.bbits_val_;
	    bbits_val_ = ((~abits_val_ | bbits_val_) & that.bbits_val_) |
	                 ((~that.abits_val_ | that.bbits_val_) & bbits_val_);
	    abits_val_ = tmp;

      } ਨਹੀਂਤਾਂ {
	    ਬਿਨਾਨਿਸ਼ਾਨੀ ਸ਼ਬਦ = (ਮਾਪ_2 + ਬਿੱਟ_ਪ੍ਰਤੀ_ਸ਼ਬਦ - 1) / ਬਿੱਟ_ਪ੍ਰਤੀ_ਸ਼ਬਦ;
	    ਲਈ (ਬਿਨਾਨਿਸ਼ਾਨੀ idx = 0; idx < ਸ਼ਬਦ ; idx += 1) {
		  ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ tmp = abits_ptr_[idx] | bbits_ptr_[idx] |
	                        that.abits_ptr_[idx] | that.bbits_ptr_[idx];
		  bbits_ptr_[idx] = ((~abits_ptr_[idx] | bbits_ptr_[idx]) &
		                     that.bbits_ptr_[idx]) |
		                    ((~that.abits_ptr_[idx] |
		                      that.bbits_ptr_[idx]) & bbits_ptr_[idx]);
		  abits_ptr_[idx] = tmp;
	    }
      }

      ਵਾਪਸ *ਇਹ;
}

vvp_vector4_t& vvp_vector4_t::ਸੰਚਾਲਕ ^= (ਅਚਲ vvp_vector4_t&that)
{
	// The truth table is:
	//     00 01 11 10
	//  00 00 01 11 11
	//  01 01 00 11 11
	//  11 11 11 11 11
	//  10 11 11 11 11
      ਜੇ (ਮਾਪ_2 <= ਬਿੱਟ_ਪ੍ਰਤੀ_ਸ਼ਬਦ) {
	    ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ bval = bbits_val_ | that.bbits_val_;
	    bbits_val_ = bval;
	    abits_val_ = (abits_val_ ^ that.abits_val_) | bval;
      } ਨਹੀਂਤਾਂ {
	    ਬਿਨਾਨਿਸ਼ਾਨੀ ਸ਼ਬਦ = (ਮਾਪ_2 + ਬਿੱਟ_ਪ੍ਰਤੀ_ਸ਼ਬਦ - 1) / ਬਿੱਟ_ਪ੍ਰਤੀ_ਸ਼ਬਦ;
	    ਲਈ (ਬਿਨਾਨਿਸ਼ਾਨੀ idx = 0; idx < ਸ਼ਬਦ ; idx += 1) {
		  ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ bval = bbits_ptr_[idx] | that.bbits_ptr_[idx];
		  bbits_ptr_[idx] = bval;
		  abits_ptr_[idx] = (abits_ptr_[idx] ^ that.abits_ptr_[idx]) | bval;
	    }
      }

      ਵਾਪਸ *ਇਹ;
}

/*
* Add an integer to the vvp_vector4_t in place, bit by bit so that
* there is no size limitations.
*/
vvp_vector4_t& vvp_vector4_t::ਸੰਚਾਲਕ += (int64_t that)
{
      vvp_bit4_t ਹਾਸਲ = BIT4_0;
      ਬਿਨਾਨਿਸ਼ਾਨੀ idx;

      ਜੇ (has_xz()) {
	    vvp_vector4_t xxx (size(), BIT4_X);
	    *ਇਹ = xxx;
	    ਵਾਪਸ *ਇਹ;
      }

      ਲਈ (idx = 0 ; idx < size() ; idx += 1) {
	    ਜੇ (that == 0 && ਹਾਸਲ==BIT4_0)
		  ਰੋਕੋ;

	    vvp_bit4_t that_ਦੋਆਧਾਰੀ_ਅੰਕ = (that&1)? BIT4_1 : BIT4_0;
	    that >>= 1;

	    ਜੇ (that_ਦੋਆਧਾਰੀ_ਅੰਕ==BIT4_0 && ਹਾਸਲ==BIT4_0)
		  ਜਾਰੀ;

	    vvp_bit4_t ਦੋਆਧਾਰੀ_ਅੰਕ = value(idx);
	    ਦੋਆਧਾਰੀ_ਅੰਕ = ਜੋੜੋ_with_ਹਾਸਲ(ਦੋਆਧਾਰੀ_ਅੰਕ, that_ਦੋਆਧਾਰੀ_ਅੰਕ, ਹਾਸਲ);

	    ਸੈੱਟ_ਦੋਆਧਾਰੀ_ਅੰਕ(idx, ਦੋਆਧਾਰੀ_ਅੰਕ);
      }

      ਵਾਪਸ *ਇਹ;
}

ostream& ਸੰਚਾਲਕ<< (ostream&out, ਅਚਲ vvp_vector4_t&that)
{
      out << that.size() << "'b";
      ਲਈ (ਬਿਨਾਨਿਸ਼ਾਨੀ idx = 0 ;  idx < that.size() ;  idx += 1)
	    out << that.value(that.size()-idx-1);
      ਵਾਪਸ out;
}

ਸਾਂਚਾ <ਵਰਗ INT>ਤਾਰਕਿਕ vector4_ਮੰਜ਼ਿਲ_ਮੁੱਲ(ਅਚਲ vvp_vector4_t&vec, INT&val,
					  ਤਾਰਕਿਕ is_signed, ਤਾਰਕਿਕ is_arithmetic)
{
      INT res = 0;
      INT msk = 1;
      ਤਾਰਕਿਕ rc_flag = ਸੱਚ;

      ਬਿਨਾਨਿਸ਼ਾਨੀ size = vec.size();
      ਜੇ (size > 8*ਆਕਾਰ(val)) size = 8*ਆਕਾਰ(val);
      ਲਈ (ਬਿਨਾਨਿਸ਼ਾਨੀ idx = 0 ;  idx < size ;  idx += 1) {
	    ਚੋਣ (vec.value(idx)) {
		ਹਾਲਤ BIT4_0:
		  ਰੋਕੋ;
		ਹਾਲਤ BIT4_1:
		  res |= msk;
		  ਰੋਕੋ;
		ਮੂਲ:
		  ਜੇ (is_arithmetic)
			ਵਾਪਸ ਝੂਠ;
		  ਨਹੀਂਤਾਂ
			rc_flag = ਝੂਠ;
	    }

	    msk <<= 1;
      }

      ਜੇ (is_signed && vec.value(vec.size()-1) == BIT4_1) {
	    ਜੇ (vec.size() < 8*ਆਕਾਰ(val))
		  res |= static_cast<INT>(-1ULL << vec.size());
      }

      val = res;
      ਵਾਪਸ rc_flag;
}

ਸਾਂਚਾ ਤਾਰਕਿਕ vector4_ਮੰਜ਼ਿਲ_ਮੁੱਲ(ਅਚਲ vvp_vector4_t&vec, int8_t&val,
			       ਤਾਰਕਿਕ is_signed, ਤਾਰਕਿਕ is_arithmetic);
ਸਾਂਚਾ ਤਾਰਕਿਕ vector4_ਮੰਜ਼ਿਲ_ਮੁੱਲ(ਅਚਲ vvp_vector4_t&vec, int16_t&val,
			       ਤਾਰਕਿਕ is_signed, ਤਾਰਕਿਕ is_arithmetic);
ਸਾਂਚਾ ਤਾਰਕਿਕ vector4_ਮੰਜ਼ਿਲ_ਮੁੱਲ(ਅਚਲ vvp_vector4_t&vec, int32_t&val,
			       ਤਾਰਕਿਕ is_signed, ਤਾਰਕਿਕ is_arithmetic);
ਸਾਂਚਾ ਤਾਰਕਿਕ vector4_ਮੰਜ਼ਿਲ_ਮੁੱਲ(ਅਚਲ vvp_vector4_t&vec, int64_t&val,
			       ਤਾਰਕਿਕ is_signed, ਤਾਰਕਿਕ is_arithmetic);
ਸਾਂਚਾ ਤਾਰਕਿਕ vector4_ਮੰਜ਼ਿਲ_ਮੁੱਲ(ਅਚਲ vvp_vector4_t&vec, uint8_t&val,
			       ਤਾਰਕਿਕ is_signed, ਤਾਰਕਿਕ is_arithmetic);
ਸਾਂਚਾ ਤਾਰਕਿਕ vector4_ਮੰਜ਼ਿਲ_ਮੁੱਲ(ਅਚਲ vvp_vector4_t&vec, uint16_t&val,
			       ਤਾਰਕਿਕ is_signed, ਤਾਰਕਿਕ is_arithmetic);
ਸਾਂਚਾ ਤਾਰਕਿਕ vector4_ਮੰਜ਼ਿਲ_ਮੁੱਲ(ਅਚਲ vvp_vector4_t&vec, uint32_t&val,
			       ਤਾਰਕਿਕ is_signed, ਤਾਰਕਿਕ is_arithmetic);
ਸਾਂਚਾ ਤਾਰਕਿਕ vector4_ਮੰਜ਼ਿਲ_ਮੁੱਲ(ਅਚਲ vvp_vector4_t&vec, uint64_t&val,
			       ਤਾਰਕਿਕ is_signed, ਤਾਰਕਿਕ is_arithmetic);

ਸਾਂਚਾ <ਵਰਗ T> ਤਾਰਕਿਕ vector4_ਮੰਜ਼ਿਲ_ਮੁੱਲ(ਅਚਲ vvp_vector4_t&vec,
                                         ਤਾਰਕਿਕ&ਉਛਾਲ_flag, T&val)
{
      T res = 0;
      T msk = 1;

      ਉਛਾਲ_flag = ਝੂਠ;
      ਬਿਨਾਨਿਸ਼ਾਨੀ size = vec.size();
      ਲਈ (ਬਿਨਾਨਿਸ਼ਾਨੀ idx = 0 ;  idx < size ;  idx += 1) {
	    ਚੋਣ (vec.value(idx)) {
		ਹਾਲਤ BIT4_0:
		  ਰੋਕੋ;
		ਹਾਲਤ BIT4_1:
		  ਜੇ (msk == 0)
			ਉਛਾਲ_flag = ਸੱਚ;
		  ਨਹੀਂਤਾਂ
			res |= msk;
		  ਰੋਕੋ;
		ਮੂਲ:
		  ਵਾਪਸ ਝੂਠ;
	    }

	    msk <<= static_cast<T>(1);
      }

      val = res;
      ਵਾਪਸ ਸੱਚ;
}

ਸਾਂਚਾ ਤਾਰਕਿਕ vector4_ਮੰਜ਼ਿਲ_ਮੁੱਲ(ਅਚਲ vvp_vector4_t&vec, ਤਾਰਕਿਕ&ਉਛਾਲ_flag,
                               ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ&val);
#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ UL_AND_TIME64_SAME
ਸਾਂਚਾ ਤਾਰਕਿਕ vector4_ਮੰਜ਼ਿਲ_ਮੁੱਲ(ਅਚਲ vvp_vector4_t&vec, ਤਾਰਕਿਕ&ਉਛਾਲ_flag,
                               vvp_time64_t&val);
#ਜੇ_ਅੰਤ

ਤਾਰਕਿਕ vector4_ਮੰਜ਼ਿਲ_ਮੁੱਲ(ਅਚਲ vvp_vector4_t&vec, ਦੁੱਗਣਾ&val, ਤਾਰਕਿਕ signed_flag)
{

      ਜੇ (vec.size() == 0) {
	    val = 0.0;
	    ਵਾਪਸ ਸੱਚ;
      }

      ਤਾਰਕਿਕ flag = ਸੱਚ;

      ਜੇ (vec.value(vec.size()-1) != BIT4_1) {
	    signed_flag = ਝੂਠ;
      }

      ਦੁੱਗਣਾ res = 0.0;
      ਜੇ (signed_flag) {
	    vvp_bit4_t ਹਾਸਲ = BIT4_1;
	    ਲਈ (ਬਿਨਾਨਿਸ਼ਾਨੀ idx = 0 ;  idx < vec.size() ;  idx += 1) {
		  vvp_bit4_t a = ~vec.value(idx);
		  vvp_bit4_t x = ਜੋੜੋ_with_ਹਾਸਲ(a, BIT4_0, ਹਾਸਲ);
		  ਚੋਣ (x) {
		      ਹਾਲਤ BIT4_0:
			ਰੋਕੋ;
		      ਹਾਲਤ BIT4_1:
			res += ਘਾਤ(2.0, (ਪੂਰਨ)idx);
			ਰੋਕੋ;
		      ਮੂਲ:
			flag = ਝੂਠ;
		  }
	    }
	    res *= -1.0;
      } ਨਹੀਂਤਾਂ {
	    ਲਈ (ਬਿਨਾਨਿਸ਼ਾਨੀ idx = 0 ;  idx < vec.size() ;  idx += 1) {
		  ਚੋਣ (vec.value(idx)) {
		      ਹਾਲਤ BIT4_0:
			ਰੋਕੋ;
		      ਹਾਲਤ BIT4_1:
			res += ਘਾਤ(2.0, (ਪੂਰਨ)idx);
			ਰੋਕੋ;
		      ਮੂਲ:
			flag = ਝੂਠ;
		  }
	    }
      }
      val = res;
      ਵਾਪਸ flag;
}

ਤਾਰਕਿਕ vector2_ਮੰਜ਼ਿਲ_ਮੁੱਲ(ਅਚਲ vvp_vector2_t&a, int32_t&val, ਤਾਰਕਿਕ is_signed)
{
      val = 0;
      ਬਿਨਾਨਿਸ਼ਾਨੀ idx;
      uint32_t ਨਕਾਬ;
      ਲਈ (idx = 0, ਨਕਾਬ = 1 ; idx < a.size() && idx < 32 ; idx += 1, ਨਕਾਬ <<= 1) {
	    ਜੇ (a.value(idx)) val |= ਨਕਾਬ;
      }

      ਜੇ (is_signed && a.size() < 32 && a.value(a.size()-1)) {
	    ਨਕਾਬ = -1;
	    ਨਕਾਬ <<= a.size();
	    val |= ਨਕਾਬ;
      }

      ਵਾਪਸ a.size() <= 32;
}

vvp_vector4array_t::vvp_vector4array_t(ਬਿਨਾਨਿਸ਼ਾਨੀ ਚੌੜਾਈ_3, ਬਿਨਾਨਿਸ਼ਾਨੀ ਸ਼ਬਦ_3)
: ਚੌੜਾਈ_2(ਚੌੜਾਈ_3), ਸ਼ਬਦ_4(ਸ਼ਬਦ_3)
{
}

vvp_vector4array_t::~vvp_vector4array_t()
{
}

ਖਾਲੀ vvp_vector4array_t::ਸੈੱਟ_ਸ਼ਬਦ_2(v4cell*cell, ਅਚਲ vvp_vector4_t&that)
{
      ਪ੍ਰਤਿਜ੍ਞਾ(that.ਮਾਪ_2 == ਚੌੜਾਈ_2);

      ਜੇ (ਚੌੜਾਈ_2 <= vvp_vector4_t::ਬਿੱਟ_ਪ੍ਰਤੀ_ਸ਼ਬਦ) {
	    cell->abits_val_ = that.abits_val_;
	    cell->bbits_val_ = that.bbits_val_;
	    ਵਾਪਸ;
      }

      ਬਿਨਾਨਿਸ਼ਾਨੀ cnt = (ਚੌੜਾਈ_2 + vvp_vector4_t::ਬਿੱਟ_ਪ੍ਰਤੀ_ਸ਼ਬਦ-1)/vvp_vector4_t::ਬਿੱਟ_ਪ੍ਰਤੀ_ਸ਼ਬਦ;

      ਜੇ (cell->abits_ptr_ == 0) {
	    cell->abits_ptr_ = ਨਵਾਂ ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ[2*cnt];
	    cell->bbits_ptr_ = cell->abits_ptr_ + cnt;
      }

      ਲਈ (ਬਿਨਾਨਿਸ਼ਾਨੀ idx = 0 ; idx < cnt ; idx += 1)
	    cell->abits_ptr_[idx] = that.abits_ptr_[idx];
      ਲਈ (ਬਿਨਾਨਿਸ਼ਾਨੀ idx = 0 ; idx < cnt ; idx += 1)
	    cell->bbits_ptr_[idx] = that.bbits_ptr_[idx];
}

vvp_vector4_t vvp_vector4array_t::get_ਸ਼ਬਦ_2(v4cell*cell) ਅਚਲ
{
      ਜੇ (ਚੌੜਾਈ_2 <= vvp_vector4_t::ਬਿੱਟ_ਪ੍ਰਤੀ_ਸ਼ਬਦ) {
	    vvp_vector4_t res;
	    res.ਮਾਪ_2 = ਚੌੜਾਈ_2;
	    res.abits_val_ = cell->abits_val_;
	    res.bbits_val_ = cell->bbits_val_;
	    ਵਾਪਸ res;
      }

      vvp_vector4_t res (ਚੌੜਾਈ_2, BIT4_X);
      ਜੇ (cell->abits_ptr_ == 0)
	    ਵਾਪਸ res;

      ਬਿਨਾਨਿਸ਼ਾਨੀ cnt = (ਚੌੜਾਈ_2 + vvp_vector4_t::ਬਿੱਟ_ਪ੍ਰਤੀ_ਸ਼ਬਦ-1)/vvp_vector4_t::ਬਿੱਟ_ਪ੍ਰਤੀ_ਸ਼ਬਦ;

      ਲਈ (ਬਿਨਾਨਿਸ਼ਾਨੀ idx = 0 ; idx < cnt ; idx += 1)
	    res.abits_ptr_[idx] = cell->abits_ptr_[idx];
      ਲਈ (ਬਿਨਾਨਿਸ਼ਾਨੀ idx = 0 ; idx < cnt ; idx += 1)
	    res.bbits_ptr_[idx] = cell->bbits_ptr_[idx];

      ਵਾਪਸ res;
}

vvp_vector4array_sa::vvp_vector4array_sa(ਬਿਨਾਨਿਸ਼ਾਨੀ ਚੌੜਾਈ_3, ਬਿਨਾਨਿਸ਼ਾਨੀ ਸ਼ਬਦ_3)
: vvp_vector4array_t(ਚੌੜਾਈ_3, ਸ਼ਬਦ_3)
{
      array_ = ਨਵਾਂ v4cell[ਸ਼ਬਦ_4];

      ਜੇ (ਚੌੜਾਈ_2 <= vvp_vector4_t::ਬਿੱਟ_ਪ੍ਰਤੀ_ਸ਼ਬਦ) {
	    ਲਈ (ਬਿਨਾਨਿਸ਼ਾਨੀ idx = 0 ; idx < ਸ਼ਬਦ_4 ; idx += 1) {
		  array_[idx].abits_val_ = vvp_vector4_t::ਸ਼ਬਦ_X_ABITS;
		  array_[idx].bbits_val_ = vvp_vector4_t::ਸ਼ਬਦ_X_BBITS;
	    }
      } ਨਹੀਂਤਾਂ {
	    ਲਈ (ਬਿਨਾਨਿਸ਼ਾਨੀ idx = 0 ; idx < ਸ਼ਬਦ_4 ; idx += 1) {
		  array_[idx].abits_ptr_ = 0;
		  array_[idx].bbits_ptr_ = 0;
	    }
      }
}

vvp_vector4array_sa::~vvp_vector4array_sa()
{
      ਜੇ (array_) {
	    ਜੇ (ਚੌੜਾਈ_2 > vvp_vector4_t::ਬਿੱਟ_ਪ੍ਰਤੀ_ਸ਼ਬਦ) {
		  ਲਈ (ਬਿਨਾਨਿਸ਼ਾਨੀ idx = 0 ; idx < ਸ਼ਬਦ_4 ; idx += 1)
			ਜੇ (array_[idx].abits_ptr_)
			      ਮਿਟਾਓ[]array_[idx].abits_ptr_;
	    }
	    ਮਿਟਾਓ[]array_;
      }
}

ਖਾਲੀ vvp_vector4array_sa::ਸੈੱਟ_ਸ਼ਬਦ(ਬਿਨਾਨਿਸ਼ਾਨੀ ਸੂਚਕਾਂਕ, ਅਚਲ vvp_vector4_t&that)
{
      ਪ੍ਰਤਿਜ੍ਞਾ(ਸੂਚਕਾਂਕ < ਸ਼ਬਦ_4);

      v4cell*cell = &array_[ਸੂਚਕਾਂਕ];

      ਸੈੱਟ_ਸ਼ਬਦ_2(cell, that);
}

vvp_vector4_t vvp_vector4array_sa::get_ਸ਼ਬਦ(ਬਿਨਾਨਿਸ਼ਾਨੀ ਸੂਚਕਾਂਕ) ਅਚਲ
{
      ਜੇ (ਸੂਚਕਾਂਕ >= ਸ਼ਬਦ_4)
	    ਵਾਪਸ vvp_vector4_t(ਚੌੜਾਈ_2, BIT4_X);

      ਪ੍ਰਤਿਜ੍ਞਾ(ਸੂਚਕਾਂਕ < ਸ਼ਬਦ_4);

      v4cell*cell = &array_[ਸੂਚਕਾਂਕ];

      ਵਾਪਸ get_ਸ਼ਬਦ_2(cell);
}

vvp_vector4array_aa::vvp_vector4array_aa(ਬਿਨਾਨਿਸ਼ਾਨੀ ਚੌੜਾਈ_3, ਬਿਨਾਨਿਸ਼ਾਨੀ ਸ਼ਬਦ_3)
: vvp_vector4array_t(ਚੌੜਾਈ_3, ਸ਼ਬਦ_3)
{
      context_idx_ = vpip_ਜੋੜੋ_ਵਸਤੂ_ਮੰਜ਼ਿਲ_context(ਇਹ, vpip_peek_context_scope());
}

vvp_vector4array_aa::~vvp_vector4array_aa()
{
}

ਖਾਲੀ vvp_vector4array_aa::alloc_instance(vvp_context_t context)
{
      v4cell*array = ਨਵਾਂ v4cell[ਸ਼ਬਦ_4];

      ਜੇ (ਚੌੜਾਈ_2 <= vvp_vector4_t::ਬਿੱਟ_ਪ੍ਰਤੀ_ਸ਼ਬਦ) {
	    ਲਈ (ਬਿਨਾਨਿਸ਼ਾਨੀ idx = 0 ; idx < ਸ਼ਬਦ_4 ; idx += 1) {
		  array[idx].abits_val_ = vvp_vector4_t::ਸ਼ਬਦ_X_ABITS;
		  array[idx].bbits_val_ = vvp_vector4_t::ਸ਼ਬਦ_X_BBITS;
	    }
      } ਨਹੀਂਤਾਂ {
	    ਲਈ (ਬਿਨਾਨਿਸ਼ਾਨੀ idx = 0 ; idx < ਸ਼ਬਦ_4 ; idx += 1) {
		  array[idx].abits_ptr_ = 0;
		  array[idx].bbits_ptr_ = 0;
	    }
      }

      vvp_ਸੈੱਟ_context_ਵਸਤੂ(context, context_idx_, array);
}

ਖਾਲੀ vvp_vector4array_aa::reset_instance(vvp_context_t context)
{
      v4cell*cell = static_cast<v4cell*>
            (vvp_get_context_ਵਸਤੂ(context, context_idx_));

      ਜੇ (ਚੌੜਾਈ_2 <= vvp_vector4_t::ਬਿੱਟ_ਪ੍ਰਤੀ_ਸ਼ਬਦ) {
	    ਲਈ (ਬਿਨਾਨਿਸ਼ਾਨੀ idx = 0 ; idx < ਸ਼ਬਦ_4 ; idx += 1) {
		  cell->abits_val_ = vvp_vector4_t::ਸ਼ਬਦ_X_ABITS;
		  cell->bbits_val_ = vvp_vector4_t::ਸ਼ਬਦ_X_BBITS;
                  cell++;
	    }
      } ਨਹੀਂਤਾਂ {
            ਬਿਨਾਨਿਸ਼ਾਨੀ cnt = (ਚੌੜਾਈ_2 + vvp_vector4_t::ਬਿੱਟ_ਪ੍ਰਤੀ_ਸ਼ਬਦ-1)/vvp_vector4_t::ਬਿੱਟ_ਪ੍ਰਤੀ_ਸ਼ਬਦ;
	    ਲਈ (ਬਿਨਾਨਿਸ਼ਾਨੀ idx = 0 ; idx < ਸ਼ਬਦ_4 ; idx += 1) {
		  ਜੇ (cell->abits_ptr_) {
                        ਲਈ (ਬਿਨਾਨਿਸ਼ਾਨੀ n = 0 ; n < cnt ; n += 1) {
			      cell->abits_ptr_[n] = vvp_vector4_t::ਸ਼ਬਦ_X_ABITS;
			      cell->bbits_ptr_[n] = vvp_vector4_t::ਸ਼ਬਦ_X_BBITS;
                        }
                  }
                  cell++;
	    }
      }
}

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ CHECK_WITH_VALGRIND
ਖਾਲੀ vvp_vector4array_aa::free_instance(vvp_context_t context)
{
      v4cell*cell = static_cast<v4cell*>
            (vvp_get_context_ਵਸਤੂ(context, context_idx_));
      ਮਿਟਾਓ [] cell;
}
#ਜੇ_ਅੰਤ

ਖਾਲੀ vvp_vector4array_aa::ਸੈੱਟ_ਸ਼ਬਦ(ਬਿਨਾਨਿਸ਼ਾਨੀ ਸੂਚਕਾਂਕ, ਅਚਲ vvp_vector4_t&that)
{
      ਪ੍ਰਤਿਜ੍ਞਾ(ਸੂਚਕਾਂਕ < ਸ਼ਬਦ_4);

      v4cell*cell = static_cast<v4cell*>
            (vthread_get_wt_context_ਵਸਤੂ(context_idx_)) + ਸੂਚਕਾਂਕ;

      ਸੈੱਟ_ਸ਼ਬਦ_2(cell, that);
}

vvp_vector4_t vvp_vector4array_aa::get_ਸ਼ਬਦ(ਬਿਨਾਨਿਸ਼ਾਨੀ ਸੂਚਕਾਂਕ) ਅਚਲ
{
      ਜੇ (ਸੂਚਕਾਂਕ >= ਸ਼ਬਦ_4)
	    ਵਾਪਸ vvp_vector4_t(ਚੌੜਾਈ_2, BIT4_X);

      ਪ੍ਰਤਿਜ੍ਞਾ(ਸੂਚਕਾਂਕ < ਸ਼ਬਦ_4);

      v4cell*cell = static_cast<v4cell*>
            (vthread_get_rd_context_ਵਸਤੂ(context_idx_)) + ਸੂਚਕਾਂਕ;

      ਵਾਪਸ get_ਸ਼ਬਦ_2(cell);
}

vvp_vector2_t::vvp_vector2_t()
{
      vec_ = 0;
      wid_ = 0;
}

vvp_vector2_t::vvp_vector2_t(uint64_t v, ਬਿਨਾਨਿਸ਼ਾਨੀ wid)
{
      wid_ = wid;
      ਅਚਲ ਬਿਨਾਨਿਸ਼ਾਨੀ ਬਿੱਟ_ਪ੍ਰਤੀ_ਸ਼ਬਦ_2 = 8 * ਆਕਾਰ(vec_[0]);
      ਅਚਲ ਬਿਨਾਨਿਸ਼ਾਨੀ ਸ਼ਬਦ = (wid_ + ਬਿੱਟ_ਪ੍ਰਤੀ_ਸ਼ਬਦ_2-1) / ਬਿੱਟ_ਪ੍ਰਤੀ_ਸ਼ਬਦ_2;

      vec_ = ਨਵਾਂ ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ[ਸ਼ਬਦ];
      ਲਈ (ਬਿਨਾਨਿਸ਼ਾਨੀ idx = 0 ; idx < ਸ਼ਬਦ ; idx += 1) {
	    ਅਚਲ ਬਿਨਾਨਿਸ਼ਾਨੀ ਖਿਸਕਾਓ = idx * ਬਿੱਟ_ਪ੍ਰਤੀ_ਸ਼ਬਦ_2;
	    vec_[idx] = ਖਿਸਕਾਓ < 64 ? v >> ਖਿਸਕਾਓ : 0;
      }
}

vvp_vector2_t::vvp_vector2_t(vvp_vector2_t::fill_t fill, ਬਿਨਾਨਿਸ਼ਾਨੀ wid)
{
      wid_ = wid;
      ਅਚਲ ਬਿਨਾਨਿਸ਼ਾਨੀ ਬਿੱਟ_ਪ੍ਰਤੀ_ਸ਼ਬਦ_2 = 8 * ਆਕਾਰ(vec_[0]);
      ਅਚਲ ਬਿਨਾਨਿਸ਼ਾਨੀ ਸ਼ਬਦ = (wid_ + ਬਿੱਟ_ਪ੍ਰਤੀ_ਸ਼ਬਦ_2-1) / ਬਿੱਟ_ਪ੍ਰਤੀ_ਸ਼ਬਦ_2;

      vec_ = ਨਵਾਂ ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ[ਸ਼ਬਦ];
      ਲਈ (ਬਿਨਾਨਿਸ਼ਾਨੀ idx = 0 ;  idx < ਸ਼ਬਦ ;  idx += 1)
	    vec_[idx] = fill? -1 : 0;
}

vvp_vector2_t::vvp_vector2_t(ਅਚਲ vvp_vector2_t&that, ਬਿਨਾਨਿਸ਼ਾਨੀ ਅਧਾਰ, ਬਿਨਾਨਿਸ਼ਾਨੀ wid)
{
      wid_ = wid;
      ਅਚਲ ਬਿਨਾਨਿਸ਼ਾਨੀ ਸ਼ਬਦ = (wid_ + ਬਿੱਟ_ਪ੍ਰਤੀ_ਸ਼ਬਦ-1) / ਬਿੱਟ_ਪ੍ਰਤੀ_ਸ਼ਬਦ;

      vec_ = ਨਵਾਂ ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ[ਸ਼ਬਦ];

      ਲਈ (ਬਿਨਾਨਿਸ਼ਾਨੀ idx = 0 ; idx < wid ; idx += 1) {
	    ਪੂਰਨ ਦੋਆਧਾਰੀ_ਅੰਕ = that.value(ਅਧਾਰ+idx);
	    ਜੇ (ਦੋਆਧਾਰੀ_ਅੰਕ == 0)
		  ਜਾਰੀ;
	    ਬਿਨਾਨਿਸ਼ਾਨੀ ਸ਼ਬਦ_2 = idx / ਬਿੱਟ_ਪ੍ਰਤੀ_ਸ਼ਬਦ;
	    ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ ਨਕਾਬ = 1UL << (idx % ਬਿੱਟ_ਪ੍ਰਤੀ_ਸ਼ਬਦ);
	    vec_[ਸ਼ਬਦ_2] |= ਨਕਾਬ;
      }
}

ਖਾਲੀ vvp_vector2_t::copy_from_that_(ਅਚਲ vvp_vector4_t&that)
{
      wid_ = that.size();
      ਅਚਲ ਬਿਨਾਨਿਸ਼ਾਨੀ ਸ਼ਬਦ = (that.size() + ਬਿੱਟ_ਪ੍ਰਤੀ_ਸ਼ਬਦ-1) / ਬਿੱਟ_ਪ੍ਰਤੀ_ਸ਼ਬਦ;

      ਜੇ (ਸ਼ਬਦ == 0) {
	    vec_ = 0;
	    wid_ = 0;
	    ਵਾਪਸ;
      }

	// Use the subarray method with the xz_to_0 flag set so that
	// we get values even when there are xz bits.
      vec_ = that.subarray(0, wid_, ਸੱਚ);
}

ਖਾਲੀ vvp_vector2_t::copy_from_that_(ਅਚਲ vvp_vector2_t&that)
{
      wid_ = that.wid_;
      ਅਚਲ ਬਿਨਾਨਿਸ਼ਾਨੀ ਸ਼ਬਦ = (wid_ + ਬਿੱਟ_ਪ੍ਰਤੀ_ਸ਼ਬਦ-1) / ਬਿੱਟ_ਪ੍ਰਤੀ_ਸ਼ਬਦ;

      ਜੇ (ਸ਼ਬਦ == 0) {
	    vec_ = 0;
	    wid_ = 0;
	    ਵਾਪਸ;
      }

      vec_ = ਨਵਾਂ ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ[ਸ਼ਬਦ];
      ਲਈ (ਬਿਨਾਨਿਸ਼ਾਨੀ idx = 0 ;  idx < ਸ਼ਬਦ ;  idx += 1)
	    vec_[idx] = that.vec_[idx];
}

vvp_vector2_t::vvp_vector2_t(ਅਚਲ vvp_vector2_t&that, ਬਿਨਾਨਿਸ਼ਾਨੀ newsize)
{
      wid_ = newsize;
      ਜੇ (newsize == 0) {
	    vec_ = 0;
	    ਵਾਪਸ;
      }

      ਅਚਲ ਬਿਨਾਨਿਸ਼ਾਨੀ ਸ਼ਬਦ = (wid_ + ਬਿੱਟ_ਪ੍ਰਤੀ_ਸ਼ਬਦ-1) / ਬਿੱਟ_ਪ੍ਰਤੀ_ਸ਼ਬਦ;
      ਅਚਲ ਬਿਨਾਨਿਸ਼ਾਨੀ twords = (that.wid_ + ਬਿੱਟ_ਪ੍ਰਤੀ_ਸ਼ਬਦ-1) / ਬਿੱਟ_ਪ੍ਰਤੀ_ਸ਼ਬਦ;

      vec_ = ਨਵਾਂ ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ[ਸ਼ਬਦ];
      ਲਈ (ਬਿਨਾਨਿਸ਼ਾਨੀ idx = 0 ;  idx < ਸ਼ਬਦ ;  idx += 1) {
	    ਜੇ (idx < twords)
		  vec_[idx] = that.vec_[idx];
	    ਨਹੀਂਤਾਂ
		  vec_[idx] = 0;
      }
}

vvp_vector2_t& vvp_vector2_t::ਸੰਚਾਲਕ= (ਅਚਲ vvp_vector2_t&that)
{
      ਜੇ (ਇਹ == &that)
	    ਵਾਪਸ *ਇਹ;

      ਮਿਟਾਓ[] vec_;
      vec_ = 0;

      copy_from_that_(that);
      ਵਾਪਸ *ਇਹ;
}

vvp_vector2_t& vvp_vector2_t::ਸੰਚਾਲਕ= (ਅਚਲ vvp_vector4_t&that)
{
      ਮਿਟਾਓ[]vec_;
      vec_ = 0;
      copy_from_that_(that);
      ਵਾਪਸ *ਇਹ;
}

vvp_vector2_t& vvp_vector2_t::ਸੰਚਾਲਕ <<= (ਬਿਨਾਨਿਸ਼ਾਨੀ ਪੂਰਨ ਖਿਸਕਾਓ)
{
      ਜੇ (wid_ == 0)
	    ਵਾਪਸ *ਇਹ;

      ਅਚਲ ਬਿਨਾਨਿਸ਼ਾਨੀ ਸ਼ਬਦ = (wid_ + ਬਿੱਟ_ਪ੍ਰਤੀ_ਸ਼ਬਦ-1) / ਬਿੱਟ_ਪ੍ਰਤੀ_ਸ਼ਬਦ;

	// Number of words to shift
      ਅਚਲ ਬਿਨਾਨਿਸ਼ਾਨੀ wshift = ਖਿਸਕਾਓ / ਬਿੱਟ_ਪ੍ਰਤੀ_ਸ਼ਬਦ;
	// bits to shift within each word.
      ਅਚਲ ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ oshift = ਖਿਸਕਾਓ % ਬਿੱਟ_ਪ੍ਰਤੀ_ਸ਼ਬਦ;

	// If shifting the entire value away, then return zeros.
      ਜੇ (wshift >= ਸ਼ਬਦ) {
	    ਲਈ (ਬਿਨਾਨਿਸ਼ਾਨੀ idx = 0 ;  idx < ਸ਼ਬਦ ;  idx += 1)
		  vec_[idx] = 0;

	    ਵਾਪਸ *ਇਹ;
      }

	// Do the word shift first.
      ਜੇ (wshift > 0) {
	    ਲਈ (ਬਿਨਾਨਿਸ਼ਾਨੀ idx = 0 ;  idx < ਸ਼ਬਦ-wshift ;  idx += 1) {
		  ਬਿਨਾਨਿਸ਼ਾਨੀ sel = ਸ਼ਬਦ - idx - 1;
		  vec_[sel] = vec_[sel-wshift];
	    }

	    ਲਈ (ਬਿਨਾਨਿਸ਼ਾਨੀ idx = 0 ;  idx < wshift ;  idx += 1)
		  vec_[idx] = 0;
      }

	// Do the fine shift.
      ਜੇ (oshift != 0) {
	    ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ pad = 0;
	    ਲਈ (ਬਿਨਾਨਿਸ਼ਾਨੀ idx = 0 ;  idx < ਸ਼ਬਦ ;  idx += 1) {
		  ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ ਅਗਲਾ_pad = vec_[idx] >> (ਬਿੱਟ_ਪ੍ਰਤੀ_ਸ਼ਬਦ-oshift);
		  vec_[idx] = (vec_[idx] << oshift) | pad;
		  pad = ਅਗਲਾ_pad;
	    }

	      // Cleanup the tail bits.
	    ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ ਨਕਾਬ = -1UL >> (ਬਿੱਟ_ਪ੍ਰਤੀ_ਸ਼ਬਦ - wid_%ਬਿੱਟ_ਪ੍ਰਤੀ_ਸ਼ਬਦ);
	    vec_[ਸ਼ਬਦ-1] &= ਨਕਾਬ;
      }

      ਵਾਪਸ *ਇਹ;
}

vvp_vector2_t& vvp_vector2_t::ਸੰਚਾਲਕ >>= (ਬਿਨਾਨਿਸ਼ਾਨੀ ਖਿਸਕਾਓ)
{
      ਜੇ (wid_ == 0)
	    ਵਾਪਸ *ਇਹ;

      ਅਚਲ ਬਿਨਾਨਿਸ਼ਾਨੀ ਸ਼ਬਦ = (wid_ + ਬਿੱਟ_ਪ੍ਰਤੀ_ਸ਼ਬਦ-1) / ਬਿੱਟ_ਪ੍ਰਤੀ_ਸ਼ਬਦ;

	// Number of words to shift
      ਅਚਲ ਬਿਨਾਨਿਸ਼ਾਨੀ wshift = ਖਿਸਕਾਓ / ਬਿੱਟ_ਪ੍ਰਤੀ_ਸ਼ਬਦ;
	// bits to shift within each word.
      ਅਚਲ ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ oshift = ਖਿਸਕਾਓ % ਬਿੱਟ_ਪ੍ਰਤੀ_ਸ਼ਬਦ;

	// If shifting the entire value away, then return zeros.
      ਜੇ (wshift >= ਸ਼ਬਦ) {
	    ਲਈ (ਬਿਨਾਨਿਸ਼ਾਨੀ idx = 0 ;  idx < ਸ਼ਬਦ ;  idx += 1)
		  vec_[idx] = 0;

	    ਵਾਪਸ *ਇਹ;
      }

      ਜੇ (wshift > 0) {
	    ਲਈ (ਬਿਨਾਨਿਸ਼ਾਨੀ idx = 0 ;  idx < ਸ਼ਬਦ-wshift ;  idx += 1)
		  vec_[idx] = vec_[idx+wshift];

	    ਲਈ (ਬਿਨਾਨਿਸ਼ਾਨੀ idx = ਸ਼ਬਦ-wshift ;  idx < ਸ਼ਬਦ ;  idx += 1)
		  vec_[idx] = 0;
      }

      ਜੇ (oshift > 0) {
	    ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ pad = 0;
	    ਲਈ (ਬਿਨਾਨਿਸ਼ਾਨੀ idx = ਸ਼ਬਦ ;  idx > 0 ;  idx -= 1) {
		  ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ new_pad = vec_[idx-1] <<(ਬਿੱਟ_ਪ੍ਰਤੀ_ਸ਼ਬਦ-oshift);
		  vec_[idx-1] = pad | (vec_[idx-1] >> oshift);
		  pad = new_pad;
	    }

	      // Cleanup the tail bits.

	    ਬਿਨਾਨਿਸ਼ਾਨੀ use_ਸ਼ਬਦ = ਸ਼ਬਦ;
	      // Mask_shift is the number of high bits of the top word
	      // that are to be masked off. We start with the number
	      // of bits that are not included even in the original,
	      // then we include the bits of the shift, that are to be
	      // masked to zero.
	    ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ ਨਕਾਬ_ਖਿਸਕਾਓ = ਬਿੱਟ_ਪ੍ਰਤੀ_ਸ਼ਬਦ - wid_%ਬਿੱਟ_ਪ੍ਰਤੀ_ਸ਼ਬਦ;
	    ਨਕਾਬ_ਖਿਸਕਾਓ %= ਬਿੱਟ_ਪ੍ਰਤੀ_ਸ਼ਬਦ;
	    ਨਕਾਬ_ਖਿਸਕਾਓ += oshift;
	    ਜਦੋਂ (ਨਕਾਬ_ਖਿਸਕਾਓ >= ਬਿੱਟ_ਪ੍ਰਤੀ_ਸ਼ਬਦ) {
		  vec_[use_ਸ਼ਬਦ-1] = 0;
		  use_ਸ਼ਬਦ -= 1;
		  ਨਕਾਬ_ਖਿਸਕਾਓ -= ਬਿੱਟ_ਪ੍ਰਤੀ_ਸ਼ਬਦ;
	    }
	    ਜੇ (ਨਕਾਬ_ਖਿਸਕਾਓ > 0) {
		  ਪ੍ਰਤਿਜ੍ਞਾ(use_ਸ਼ਬਦ > 0);
		  ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ ਨਕਾਬ = -1UL >> ਨਕਾਬ_ਖਿਸਕਾਓ;
		  vec_[use_ਸ਼ਬਦ-1] &= ਨਕਾਬ;
	    }
      }

      ਵਾਪਸ *ਇਹ;
}

ਸਥਿਰ ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ ਜੋੜੋ_ਹਾਸਲ(ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ a, ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ b,
			       ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ&ਹਾਸਲ)
{
      ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ out = ਹਾਸਲ;
      ਹਾਸਲ = 0;

      ਜੇ ((ULONG_MAX - out) < a)
	    ਹਾਸਲ += 1;
      out += a;

      ਜੇ ((ULONG_MAX - out) < b)
	    ਹਾਸਲ += 1;
      out += b;

      ਵਾਪਸ out;
}

vvp_vector2_t& vvp_vector2_t::ਸੰਚਾਲਕ += (ਅਚਲ vvp_vector2_t&that)
{
      ਪ੍ਰਤਿਜ੍ਞਾ(wid_ == that.wid_);
      ਜੇ (wid_ == 0)
	    ਵਾਪਸ *ਇਹ;

      ਅਚਲ ਬਿਨਾਨਿਸ਼ਾਨੀ ਸ਼ਬਦ = (wid_ + ਬਿੱਟ_ਪ੍ਰਤੀ_ਸ਼ਬਦ-1) / ਬਿੱਟ_ਪ੍ਰਤੀ_ਸ਼ਬਦ;

      ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ ਹਾਸਲ = 0;
      ਲਈ (ਬਿਨਾਨਿਸ਼ਾਨੀ idx = 0 ;  idx < ਸ਼ਬਦ ;  idx += 1) {
	    vec_[idx] = ਜੋੜੋ_ਹਾਸਲ(vec_[idx], that.vec_[idx], ਹਾਸਲ);
      }


	// Cleanup the tail bits.
      ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ ਨਕਾਬ = -1UL >> (ਬਿੱਟ_ਪ੍ਰਤੀ_ਸ਼ਬਦ - wid_%ਬਿੱਟ_ਪ੍ਰਤੀ_ਸ਼ਬਦ);
      vec_[ਸ਼ਬਦ-1] &= ਨਕਾਬ;

      ਵਾਪਸ *ਇਹ;
}

vvp_vector2_t& vvp_vector2_t::ਸੰਚਾਲਕ -= (ਅਚਲ vvp_vector2_t&that)
{
      ਪ੍ਰਤਿਜ੍ਞਾ(wid_ == that.wid_);
      ਜੇ (wid_ == 0)
	    ਵਾਪਸ *ਇਹ;

      ਅਚਲ ਬਿਨਾਨਿਸ਼ਾਨੀ ਸ਼ਬਦ = (wid_ + ਬਿੱਟ_ਪ੍ਰਤੀ_ਸ਼ਬਦ-1) / ਬਿੱਟ_ਪ੍ਰਤੀ_ਸ਼ਬਦ;

      ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ ਹਾਸਲ = 1;
      ਲਈ (ਬਿਨਾਨਿਸ਼ਾਨੀ idx = 0 ;  idx < ਸ਼ਬਦ ;  idx += 1) {
	    vec_[idx] = ਜੋੜੋ_ਹਾਸਲ(vec_[idx], ~that.vec_[idx], ਹਾਸਲ);
      }

      ਵਾਪਸ *ਇਹ;
}

ਖਾਲੀ vvp_vector2_t::trim()
{
      ਜਦੋਂ (value(wid_-1) == 0 && wid_ > 1) wid_ -= 1;
}

/* This is a special trim that is used on numbers we know represent a
 * negative signed value (they came from a negative real value). */
ਖਾਲੀ vvp_vector2_t::trim_neg()
{
      ਜੇ (value(wid_-1) == 1 && wid_ > 32) {
	    ਜਦੋਂ (value(wid_-2) == 1 && wid_ > 32) wid_ -= 1;
      }
}

ਪੂਰਨ vvp_vector2_t::value(ਬਿਨਾਨਿਸ਼ਾਨੀ idx) ਅਚਲ
{
      ਜੇ (idx >= wid_)
	    ਵਾਪਸ 0;

      ਅਚਲ ਬਿਨਾਨਿਸ਼ਾਨੀ ਬਿੱਟ_ਪ੍ਰਤੀ_ਸ਼ਬਦ_2 = 8 * ਆਕਾਰ(vec_[0]);
      ਬਿਨਾਨਿਸ਼ਾਨੀ addr = idx/ਬਿੱਟ_ਪ੍ਰਤੀ_ਸ਼ਬਦ_2;
      ਬਿਨਾਨਿਸ਼ਾਨੀ ਨਕਾਬ = idx%ਬਿੱਟ_ਪ੍ਰਤੀ_ਸ਼ਬਦ_2;

      ਜੇ (vec_[addr] & (1UL<<ਨਕਾਬ))
	    ਵਾਪਸ 1;
      ਨਹੀਂਤਾਂ
	    ਵਾਪਸ 0;
}

ਖਾਲੀ vvp_vector2_t::ਸੈੱਟ_ਦੋਆਧਾਰੀ_ਅੰਕ(ਬਿਨਾਨਿਸ਼ਾਨੀ idx, ਪੂਰਨ ਦੋਆਧਾਰੀ_ਅੰਕ)
{
      ਪ੍ਰਤਿਜ੍ਞਾ(idx < wid_);

      ਅਚਲ ਬਿਨਾਨਿਸ਼ਾਨੀ ਬਿੱਟ_ਪ੍ਰਤੀ_ਸ਼ਬਦ_2 = 8 * ਆਕਾਰ(vec_[0]);
      ਬਿਨਾਨਿਸ਼ਾਨੀ addr = idx/ਬਿੱਟ_ਪ੍ਰਤੀ_ਸ਼ਬਦ_2;
      ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ ਨਕਾਬ = idx%ਬਿੱਟ_ਪ੍ਰਤੀ_ਸ਼ਬਦ_2;

      ਜੇ (ਦੋਆਧਾਰੀ_ਅੰਕ)
	    vec_[addr] |= 1UL << ਨਕਾਬ;
      ਨਹੀਂਤਾਂ
	    vec_[addr] &= ~(1UL << ਨਕਾਬ);
}

ਖਾਲੀ vvp_vector2_t::ਸੈੱਟ_vec(ਬਿਨਾਨਿਸ਼ਾਨੀ adr, ਅਚਲ vvp_vector2_t&that)
{
      ਪ੍ਰਤਿਜ੍ਞਾ((adr + that.wid_) <= wid_);

      ਲਈ (ਬਿਨਾਨਿਸ਼ਾਨੀ idx = 0 ; idx < that.wid_ ; idx += 1)
	    ਸੈੱਟ_ਦੋਆਧਾਰੀ_ਅੰਕ(adr+idx, that.value(idx));
}

ਤਾਰਕਿਕ vvp_vector2_t::is_NaN() ਅਚਲ
{
      ਵਾਪਸ wid_ == 0;
}

ਤਾਰਕਿਕ vvp_vector2_t::is_zero() ਅਚਲ
{
      ਅਚਲ ਬਿਨਾਨਿਸ਼ਾਨੀ ਸ਼ਬਦ = (wid_ + ਬਿੱਟ_ਪ੍ਰਤੀ_ਸ਼ਬਦ-1) / ਬਿੱਟ_ਪ੍ਰਤੀ_ਸ਼ਬਦ;

      ਲਈ (ਬਿਨਾਨਿਸ਼ਾਨੀ idx = 0; idx < ਸ਼ਬਦ; idx += 1) {
	    ਜੇ (vec_[idx] == 0) ਜਾਰੀ;
	    ਵਾਪਸ ਝੂਠ;
      }

      ਵਾਪਸ ਸੱਚ;
}

/*
 * Basic idea from "Introduction to Programming using SML" by
 * Michael R. Hansen and Hans Rischel page 261 and "Seminumerical
 * Algorithms, Third Edition" by Donald E. Knuth section 4.6.3.
 */
vvp_vector2_t ਘਾਤ(ਅਚਲ vvp_vector2_t&x, vvp_vector2_t&y)
{
        /* If we have a zero exponent just return 1. */
      ਜੇ (y == vvp_vector2_t(0L, 1)) {
	    ਵਾਪਸ vvp_vector2_t(1L, x.size());
      }

        /* Is the value odd? */
      ਜੇ (y.value(0) == 1) {
	    y.ਸੈੱਟ_ਦੋਆਧਾਰੀ_ਅੰਕ(0, 0);  // A quick subtract by 1.
	    vvp_vector2_t res = x * ਘਾਤ(x, y);
	    ਵਾਪਸ res;
      }

      y >>= 1;  // A fast divide by two. We know the LSB is zero.
      vvp_vector2_t z = ਘਾਤ(x, y);
      vvp_vector2_t res = z * z;
      ਵਾਪਸ res;
}

ਸਥਿਰ ਖਾਲੀ multiply_long(ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ a, ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ b,
			  ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ&ਨੀਵਾਂ, ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ&ਉੱਚਾ)
{
      ਪ੍ਰਤਿਜ੍ਞਾ(ਆਕਾਰ(ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ) %2 == 0);

      ਅਚਲ ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ ਸ਼ਬਦ_ਨਕਾਬ = (1UL << 4UL*ਆਕਾਰ(a)) - 1UL;
      ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ tmpa;
      ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ tmpb;
      ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ res[4];

      tmpa = a & ਸ਼ਬਦ_ਨਕਾਬ;
      tmpb = b & ਸ਼ਬਦ_ਨਕਾਬ;
      res[0] = tmpa * tmpb;
      res[1] = res[0] >> 4UL*ਆਕਾਰ(ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ);
      res[0] &= ਸ਼ਬਦ_ਨਕਾਬ;

      tmpa = (a >> 4UL*ਆਕਾਰ(ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ)) & ਸ਼ਬਦ_ਨਕਾਬ;
      res[1] += tmpa * tmpb;
      res[2] = res[1] >> 4UL*ਆਕਾਰ(ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ);
      res[1] &= ਸ਼ਬਦ_ਨਕਾਬ;

      tmpa = a & ਸ਼ਬਦ_ਨਕਾਬ;
      tmpb = (b >> 4UL*ਆਕਾਰ(ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ)) & ਸ਼ਬਦ_ਨਕਾਬ;
      res[1] += tmpa * tmpb;
      res[2] += res[1] >> 4UL*ਆਕਾਰ(ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ);
      res[3]  = res[2] >> 4UL*ਆਕਾਰ(ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ);
      res[1] &= ਸ਼ਬਦ_ਨਕਾਬ;
      res[2] &= ਸ਼ਬਦ_ਨਕਾਬ;

      tmpa = (a >> 4UL*ਆਕਾਰ(ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ)) & ਸ਼ਬਦ_ਨਕਾਬ;
      res[2] += tmpa * tmpb;
      res[3] += res[2] >> 4UL*ਆਕਾਰ(ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ);
      res[2] &= ਸ਼ਬਦ_ਨਕਾਬ;

      ਉੱਚਾ = (res[3] << 4UL*ਆਕਾਰ(ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ)) | res[2];
      ਨੀਵਾਂ  = (res[1] << 4UL*ਆਕਾਰ(ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ)) | res[0];
}

vvp_vector2_t ਸੰਚਾਲਕ * (ਅਚਲ vvp_vector2_t&a, ਅਚਲ vvp_vector2_t&b)
{
      ਅਚਲ ਬਿਨਾਨਿਸ਼ਾਨੀ ਬਿੱਟ_ਪ੍ਰਤੀ_ਸ਼ਬਦ_2 = 8 * ਆਕਾਰ(a.vec_[0]);

	// The compiler ensures that the two operands are of equal size.
      ਪ੍ਰਤਿਜ੍ਞਾ(a.size() == b.size());
      vvp_vector2_t r (0, a.size());

      ਬਿਨਾਨਿਸ਼ਾਨੀ ਸ਼ਬਦ = (r.wid_ + ਬਿੱਟ_ਪ੍ਰਤੀ_ਸ਼ਬਦ_2 - 1) / ਬਿੱਟ_ਪ੍ਰਤੀ_ਸ਼ਬਦ_2;

      ਲਈ (ਬਿਨਾਨਿਸ਼ਾਨੀ bdx = 0 ;  bdx < ਸ਼ਬਦ ;  bdx += 1) {
	    ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ tmpb = b.vec_[bdx];
	    ਜੇ (tmpb == 0)
		  ਜਾਰੀ;

	    ਲਈ (ਬਿਨਾਨਿਸ਼ਾਨੀ adx = 0 ;  adx < ਸ਼ਬਦ ;  adx += 1) {
		  ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ tmpa = a.vec_[adx];
		  ਜੇ (tmpa == 0)
			ਜਾਰੀ;

		  ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ ਨੀਵਾਂ, hig;
		  multiply_long(tmpa, tmpb, ਨੀਵਾਂ, hig);

		  ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ ਹਾਸਲ = 0;
		  ਲਈ (ਬਿਨਾਨਿਸ਼ਾਨੀ sdx = 0
			     ; (adx+bdx+sdx) < ਸ਼ਬਦ
			     ;  sdx += 1) {

			r.vec_[adx+bdx+sdx] = ਜੋੜੋ_ਹਾਸਲ(r.vec_[adx+bdx+sdx],
							ਨੀਵਾਂ, ਹਾਸਲ);
			ਨੀਵਾਂ = hig;
			hig = 0;
		  }
	    }
      }

      ਵਾਪਸ r;
}

ਸਥਿਰ ਖਾਲੀ div_mod (vvp_vector2_t dividend, vvp_vector2_t ਭਾਜਕ,
		     vvp_vector2_t&quotient, vvp_vector2_t&remainder)
{

      quotient = vvp_vector2_t(0, dividend.size());

      ਜੇ (ਭਾਜਕ == quotient) {
	    cerr << "ERROR: division by zero, exiting." << ਸਤਰ_ਅੰਤ;
	    ਬਾਹਰ_ਨਿਕਲੋ(255);
      }

      ਜੇ (dividend < ਭਾਜਕ) {
	    remainder = dividend;
	    ਵਾਪਸ;
      }

      vvp_vector2_t ਨਕਾਬ (1, dividend.size());

	// Make the dividend 1 bit larger to prevent overflow of
	// divtmp in startup.
      dividend = vvp_vector2_t(dividend, dividend.size()+1);
      vvp_vector2_t divtmp (ਭਾਜਕ, dividend.size());

      ਜਦੋਂ (divtmp < dividend) {
	    divtmp <<= 1;
	    ਨਕਾਬ <<= 1;
      }

      ਜਦੋਂ (dividend >= ਭਾਜਕ) {
	    ਜੇ (divtmp <= dividend) {
		  dividend -= divtmp;
		  quotient += ਨਕਾਬ;
	    }

	    divtmp >>= 1;
	    ਨਕਾਬ >>= 1;
      }

      remainder = vvp_vector2_t(dividend, ਨਕਾਬ.size());
}

vvp_vector2_t ਸੰਚਾਲਕ - (ਅਚਲ vvp_vector2_t&that)
{
      vvp_vector2_t neg(that);
      ਜੇ (neg.wid_ == 0) ਵਾਪਸ neg;

      ਅਚਲ ਬਿਨਾਨਿਸ਼ਾਨੀ ਸ਼ਬਦ = (neg.wid_ + neg.ਬਿੱਟ_ਪ੍ਰਤੀ_ਸ਼ਬਦ-1) /
                                         neg.ਬਿੱਟ_ਪ੍ਰਤੀ_ਸ਼ਬਦ;
      ਲਈ (ਬਿਨਾਨਿਸ਼ਾਨੀ idx = 0 ;  idx < ਸ਼ਬਦ ;  idx += 1) {
	    neg.vec_[idx] = ~neg.vec_[idx];
      }
      neg += vvp_vector2_t(1, neg.wid_);

      ਵਾਪਸ neg;
}

vvp_vector2_t ਸੰਚਾਲਕ / (ਅਚਲ vvp_vector2_t&dividend,
			  ਅਚਲ vvp_vector2_t&ਭਾਜਕ)
{
      vvp_vector2_t quot, rem;
      div_mod(dividend, ਭਾਜਕ, quot, rem);
      ਵਾਪਸ quot;
}

vvp_vector2_t ਸੰਚਾਲਕ % (ਅਚਲ vvp_vector2_t&dividend,
			  ਅਚਲ vvp_vector2_t&ਭਾਜਕ)
{
      vvp_vector2_t quot, rem;
      div_mod(dividend, ਭਾਜਕ, quot, rem);
      ਵਾਪਸ rem;
}

ਤਾਰਕਿਕ ਸੰਚਾਲਕ > (ਅਚਲ vvp_vector2_t&a, ਅਚਲ vvp_vector2_t&b)
{
      ਅਚਲ ਬਿਨਾਨਿਸ਼ਾਨੀ awords = (a.wid_ + vvp_vector2_t::ਬਿੱਟ_ਪ੍ਰਤੀ_ਸ਼ਬਦ-1) / vvp_vector2_t::ਬਿੱਟ_ਪ੍ਰਤੀ_ਸ਼ਬਦ;
      ਅਚਲ ਬਿਨਾਨਿਸ਼ਾਨੀ bwords = (b.wid_ + vvp_vector2_t::ਬਿੱਟ_ਪ੍ਰਤੀ_ਸ਼ਬਦ-1) / vvp_vector2_t::ਬਿੱਟ_ਪ੍ਰਤੀ_ਸ਼ਬਦ;

      ਅਚਲ ਬਿਨਾਨਿਸ਼ਾਨੀ ਸ਼ਬਦ = awords > bwords? awords : bwords;

      ਲਈ (ਬਿਨਾਨਿਸ਼ਾਨੀ idx = ਸ਼ਬਦ ;  idx > 0 ;  idx -= 1) {
	    ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ aw = (idx <= awords)? a.vec_[idx-1] : 0;
	    ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ bw = (idx <= bwords)? b.vec_[idx-1] : 0;

	    ਜੇ (aw > bw)
		  ਵਾਪਸ ਸੱਚ;
	    ਜੇ (aw < bw)
		  ਵਾਪਸ ਝੂਠ;
      }

	// If the above loop finishes, then the vectors are equal.
      ਵਾਪਸ ਝੂਠ;
}

ਤਾਰਕਿਕ ਸੰਚਾਲਕ >= (ਅਚਲ vvp_vector2_t&a, ਅਚਲ vvp_vector2_t&b)
{
      ਅਚਲ ਬਿਨਾਨਿਸ਼ਾਨੀ awords = (a.wid_ + vvp_vector2_t::ਬਿੱਟ_ਪ੍ਰਤੀ_ਸ਼ਬਦ-1) / vvp_vector2_t::ਬਿੱਟ_ਪ੍ਰਤੀ_ਸ਼ਬਦ;
      ਅਚਲ ਬਿਨਾਨਿਸ਼ਾਨੀ bwords = (b.wid_ + vvp_vector2_t::ਬਿੱਟ_ਪ੍ਰਤੀ_ਸ਼ਬਦ-1) / vvp_vector2_t::ਬਿੱਟ_ਪ੍ਰਤੀ_ਸ਼ਬਦ;

      ਅਚਲ ਬਿਨਾਨਿਸ਼ਾਨੀ ਸ਼ਬਦ = awords > bwords? awords : bwords;

      ਲਈ (ਬਿਨਾਨਿਸ਼ਾਨੀ idx = ਸ਼ਬਦ ;  idx > 0 ;  idx -= 1) {
	    ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ aw = (idx <= awords)? a.vec_[idx-1] : 0;
	    ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ bw = (idx <= bwords)? b.vec_[idx-1] : 0;

	    ਜੇ (aw > bw)
		  ਵਾਪਸ ਸੱਚ;
	    ਜੇ (aw < bw)
		  ਵਾਪਸ ਝੂਠ;
      }

	// If the above loop finishes, then the vectors are equal.
      ਵਾਪਸ ਸੱਚ;
}

ਤਾਰਕਿਕ ਸੰਚਾਲਕ < (ਅਚਲ vvp_vector2_t&a, ਅਚਲ vvp_vector2_t&b)
{
      ਅਚਲ ਬਿਨਾਨਿਸ਼ਾਨੀ awords = (a.wid_ + vvp_vector2_t::ਬਿੱਟ_ਪ੍ਰਤੀ_ਸ਼ਬਦ-1) / vvp_vector2_t::ਬਿੱਟ_ਪ੍ਰਤੀ_ਸ਼ਬਦ;
      ਅਚਲ ਬਿਨਾਨਿਸ਼ਾਨੀ bwords = (b.wid_ + vvp_vector2_t::ਬਿੱਟ_ਪ੍ਰਤੀ_ਸ਼ਬਦ-1) / vvp_vector2_t::ਬਿੱਟ_ਪ੍ਰਤੀ_ਸ਼ਬਦ;

      ਬਿਨਾਨਿਸ਼ਾਨੀ ਸ਼ਬਦ = awords;
      ਜੇ (bwords > ਸ਼ਬਦ)
	    ਸ਼ਬਦ = bwords;

      ਲਈ (ਬਿਨਾਨਿਸ਼ਾਨੀ idx = ਸ਼ਬਦ ;  idx > 0 ;  idx -= 1) {
	    ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ aw = (idx <= awords)? a.vec_[idx-1] : 0;
	    ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ bw = (idx <= bwords)? b.vec_[idx-1] : 0;

	    ਜੇ (aw < bw)
		  ਵਾਪਸ ਸੱਚ;
	    ਜੇ (aw > bw)
		  ਵਾਪਸ ਝੂਠ;
      }

	// If the above loop finishes, then the vectors are equal.
      ਵਾਪਸ ਝੂਠ;
}

ਤਾਰਕਿਕ ਸੰਚਾਲਕ <= (ਅਚਲ vvp_vector2_t&a, ਅਚਲ vvp_vector2_t&b)
{
	// XXXX For now, only support equal width vectors.
      ਪ੍ਰਤਿਜ੍ਞਾ(a.wid_ == b.wid_);

      ਅਚਲ ਬਿਨਾਨਿਸ਼ਾਨੀ awords = (a.wid_ + vvp_vector2_t::ਬਿੱਟ_ਪ੍ਰਤੀ_ਸ਼ਬਦ-1) / vvp_vector2_t::ਬਿੱਟ_ਪ੍ਰਤੀ_ਸ਼ਬਦ;

      ਲਈ (ਬਿਨਾਨਿਸ਼ਾਨੀ idx = awords ;  idx > 0 ;  idx -= 1) {
	    ਜੇ (a.vec_[idx-1] < b.vec_[idx-1])
		  ਵਾਪਸ ਸੱਚ;
	    ਜੇ (a.vec_[idx-1] > b.vec_[idx-1])
		  ਵਾਪਸ ਝੂਠ;
      }

	// If the above loop finishes, then the vectors are equal.
      ਵਾਪਸ ਸੱਚ;
}

ਤਾਰਕਿਕ ਸੰਚਾਲਕ == (ਅਚਲ vvp_vector2_t&a, ਅਚਲ vvp_vector2_t&b)
{
      ਅਚਲ ਬਿਨਾਨਿਸ਼ਾਨੀ awords = (a.wid_ + vvp_vector2_t::ਬਿੱਟ_ਪ੍ਰਤੀ_ਸ਼ਬਦ-1) / vvp_vector2_t::ਬਿੱਟ_ਪ੍ਰਤੀ_ਸ਼ਬਦ;
      ਅਚਲ ਬਿਨਾਨਿਸ਼ਾਨੀ bwords = (b.wid_ + vvp_vector2_t::ਬਿੱਟ_ਪ੍ਰਤੀ_ਸ਼ਬਦ-1) / vvp_vector2_t::ਬਿੱਟ_ਪ੍ਰਤੀ_ਸ਼ਬਦ;

      ਅਚਲ ਬਿਨਾਨਿਸ਼ਾਨੀ ਸ਼ਬਦ = awords > bwords? awords : bwords;

      ਲਈ (ਬਿਨਾਨਿਸ਼ਾਨੀ idx = ਸ਼ਬਦ ;  idx > 0 ;  idx -= 1) {
	    ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ aw = (idx <= awords)? a.vec_[idx-1] : 0;
	    ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ bw = (idx <= bwords)? b.vec_[idx-1] : 0;

	    ਜੇ (aw > bw)
		  ਵਾਪਸ ਝੂਠ;
	    ਜੇ (aw < bw)
		  ਵਾਪਸ ਝੂਠ;
      }

	// If the above loop finishes, then the vectors are equal.
      ਵਾਪਸ ਸੱਚ;
}


vvp_vector4_t vector2_ਮੰਜ਼ਿਲ_vector4(ਅਚਲ vvp_vector2_t&that, ਬਿਨਾਨਿਸ਼ਾਨੀ wid)
{
      vvp_vector4_t res (wid);

      ਲਈ (ਬਿਨਾਨਿਸ਼ਾਨੀ idx = 0 ;  idx < res.size() ;  idx += 1) {
	    vvp_bit4_t ਦੋਆਧਾਰੀ_ਅੰਕ = BIT4_0;

	    ਜੇ (that.value(idx))
		  ਦੋਆਧਾਰੀ_ਅੰਕ = BIT4_1;

	    res.ਸੈੱਟ_ਦੋਆਧਾਰੀ_ਅੰਕ(idx, ਦੋਆਧਾਰੀ_ਅੰਕ);
      }

      ਵਾਪਸ res;
}

ਸਥਿਰ ਤਾਰਕਿਕ c4string_ਸਿਰਲੇਖ_test(ਅਚਲ ਅੱਖਰ*str)
{
      ਜੇ ((str[0] != 'C') && (str[0] != 'c'))
	    ਵਾਪਸ ਝੂਠ;
      ਜੇ ((str[1] != '4') || (str[2] != '<'))
	    ਵਾਪਸ ਝੂਠ;
      ਵਾਪਸ ਸੱਚ;
}

ਤਾਰਕਿਕ c4string_test(ਅਚਲ ਅੱਖਰ*str)
{
      ਜੇ (!c4string_ਸਿਰਲੇਖ_test(str))
	    ਵਾਪਸ ਝੂਠ;
      size_t ਮੁੱਲ_ਮਾਪ = strspn(str+3, "01xz");
      ਜੇ (str[3+ਮੁੱਲ_ਮਾਪ] != '>')
	    ਵਾਪਸ ਝੂਠ;
      ਜੇ (str[3+ਮੁੱਲ_ਮਾਪ+1] != 0)
	    ਵਾਪਸ ਝੂਠ;

      ਵਾਪਸ ਸੱਚ;
}

vvp_vector4_t c4string_ਮੰਜ਼ਿਲ_vector4(ਅਚਲ ਅੱਖਰ*str)
{
      ਪ੍ਰਤਿਜ੍ਞਾ(c4string_ਸਿਰਲੇਖ_test(str));

      str += 3;
      ਅਚਲ ਅੱਖਰ*tp = str + strspn(str,"01xz");
      ਪ੍ਰਤਿਜ੍ਞਾ(tp[0] == '>');

      vvp_vector4_t tmp (tp-str);

      ਲਈ (ਬਿਨਾਨਿਸ਼ਾਨੀ idx = 0 ;  idx < tmp.size() ;  idx += 1) {
	    vvp_bit4_t ਦੋਆਧਾਰੀ_ਅੰਕ;
	    ਚੋਣ (str[idx]) {
		ਹਾਲਤ '0':
		  ਦੋਆਧਾਰੀ_ਅੰਕ = BIT4_0;
		  ਰੋਕੋ;
		ਹਾਲਤ '1':
		  ਦੋਆਧਾਰੀ_ਅੰਕ = BIT4_1;
		  ਰੋਕੋ;
		ਹਾਲਤ 'x':
		  ਦੋਆਧਾਰੀ_ਅੰਕ = BIT4_X;
		  ਰੋਕੋ;
		ਹਾਲਤ 'z':
		  ਦੋਆਧਾਰੀ_ਅੰਕ = BIT4_Z;
		  ਰੋਕੋ;
		ਮੂਲ:
		  fprintf(stderr, "Unsupported bit value %c(%d).\n", str[idx],
		          str[idx]);
		  ਪ੍ਰਤਿਜ੍ਞਾ(0);
		  ਦੋਆਧਾਰੀ_ਅੰਕ = BIT4_0;
		  ਰੋਕੋ;
	    }
	    tmp.ਸੈੱਟ_ਦੋਆਧਾਰੀ_ਅੰਕ(tmp.size()-idx-1, ਦੋਆਧਾਰੀ_ਅੰਕ);
      }

      ਵਾਪਸ tmp;
}

ostream& ਸੰਚਾਲਕ<< (ostream&out, ਅਚਲ vvp_vector2_t&that)
{
      ਜੇ (that.is_NaN()) {
	    out << "NaN";

      } ਨਹੀਂਤਾਂ {
	    out << vector2_ਮੰਜ਼ਿਲ_vector4(that, that.size());
      }
      ਵਾਪਸ out;
}

vvp_vector8_t::vvp_vector8_t(ਅਚਲ vvp_vector8_t&that)
{
      ਮਾਪ_2 = that.ਮਾਪ_2;
      ਜੇ (ਮਾਪ_2 <= ਆਕਾਰ(val_)) {
	    ਸਮ੍ਰਿਤੀ_ਨਕਲ(val_, that.val_, ਆਕਾਰ val_);
      } ਨਹੀਂਤਾਂ {
	    ptr_ = ਨਵਾਂ ਬਿਨਾਨਿਸ਼ਾਨੀ ਅੱਖਰ[ਮਾਪ_2];
	    ਸਮ੍ਰਿਤੀ_ਨਕਲ(ptr_, that.ptr_, ਮਾਪ_2);
      }
}

vvp_vector8_t::vvp_vector8_t(ਅਚਲ vvp_vector4_t&that,
			     ਬਿਨਾਨਿਸ਼ਾਨੀ str0, ਬਿਨਾਨਿਸ਼ਾਨੀ str1)
: ਮਾਪ_2(that.size())
{
      ਜੇ (ਮਾਪ_2 == 0)
	    ਵਾਪਸ;

      ਜੇ (ਮਾਪ_2 <= ਆਕਾਰ(val_)) {
	    ptr_ = 0; // Prefill all val_ bytes
	    ਲਈ (ਬਿਨਾਨਿਸ਼ਾਨੀ idx = 0 ; idx < ਮਾਪ_2 ; idx += 1)
		  val_[idx] = vvp_scalar_t(that.value(idx),str0, str1).raw();
      } ਨਹੀਂਤਾਂ {
	    ptr_ = ਨਵਾਂ ਬਿਨਾਨਿਸ਼ਾਨੀ ਅੱਖਰ[ਮਾਪ_2];
	    ਲਈ (ਬਿਨਾਨਿਸ਼ਾਨੀ idx = 0 ;  idx < ਮਾਪ_2 ;  idx += 1)
		  ptr_[idx] = vvp_scalar_t(that.value(idx), str0, str1).raw();
      }
}

vvp_vector8_t::vvp_vector8_t(ਅਚਲ vvp_vector2_t&that,
			     ਬਿਨਾਨਿਸ਼ਾਨੀ str0, ਬਿਨਾਨਿਸ਼ਾਨੀ str1)
: ਮਾਪ_2(that.size())
{
      ਜੇ (ਮਾਪ_2 == 0)
	    ਵਾਪਸ;

      ਜੇ (ਮਾਪ_2 <= ਆਕਾਰ(val_)) {
	    ptr_ = 0;
	    ਲਈ (ਬਿਨਾਨਿਸ਼ਾਨੀ idx = 0 ; idx < ਮਾਪ_2 ; idx += 1)
		  val_[idx] = vvp_scalar_t(that.value(idx)? BIT4_1:BIT4_0, str0, str1).raw();
      } ਨਹੀਂਤਾਂ {
	    ptr_ = ਨਵਾਂ ਬਿਨਾਨਿਸ਼ਾਨੀ ਅੱਖਰ[ਮਾਪ_2];
	    ਲਈ (ਬਿਨਾਨਿਸ਼ਾਨੀ idx = 0 ; idx < ਮਾਪ_2 ; idx += 1)
		  ptr_[idx] = vvp_scalar_t(that.value(idx)? BIT4_1:BIT4_0, str0, str1).raw();
      }
}

ਅਚਲ vvp_vector8_t vvp_vector8_t::nil;

vvp_vector8_t& vvp_vector8_t::ਸੰਚਾਲਕ= (ਅਚਲ vvp_vector8_t&that)
{
	// Assign to self.
      ਜੇ (ਇਹ == &that)
	    ਵਾਪਸ *ਇਹ;

      ਜੇ (ਮਾਪ_2 != that.ਮਾਪ_2) {
	    ਜੇ (ਮਾਪ_2 > ਆਕਾਰ(val_))
		  ਮਿਟਾਓ[]ptr_;
	    ਮਾਪ_2 = 0;
      }

      ਜੇ (that.ਮਾਪ_2 == 0) {
	    ਪ੍ਰਤਿਜ੍ਞਾ(ਮਾਪ_2 == 0);
	    ਵਾਪਸ *ਇਹ;
      }

      ਜੇ (that.ਮਾਪ_2 <= ਆਕਾਰ(val_)) {
	    ਮਾਪ_2 = that.ਮਾਪ_2;
	    ਸਮ੍ਰਿਤੀ_ਨਕਲ(val_, that.val_, ਆਕਾਰ(val_));
	    ਵਾਪਸ *ਇਹ;
      }

      ਜੇ (ਮਾਪ_2 == 0) {
	    ਮਾਪ_2 = that.ਮਾਪ_2;
	    ptr_ = ਨਵਾਂ ਬਿਨਾਨਿਸ਼ਾਨੀ ਅੱਖਰ[ਮਾਪ_2];
      }

      ਸਮ੍ਰਿਤੀ_ਨਕਲ(ptr_, that.ptr_, ਮਾਪ_2);

      ਵਾਪਸ *ਇਹ;
}

vvp_vector8_t vvp_vector8_t::subvalue(ਬਿਨਾਨਿਸ਼ਾਨੀ ਅਧਾਰ, ਬਿਨਾਨਿਸ਼ਾਨੀ wid) ਅਚਲ
{
      vvp_vector8_t tmp (wid);

      ਬਿਨਾਨਿਸ਼ਾਨੀ ਅੱਖਰ*tmp_ptr = tmp.ਮਾਪ_2 <= ਆਕਾਰ(val_) ? tmp.val_ : tmp.ptr_;
      ਅਚਲ ਬਿਨਾਨਿਸ਼ਾਨੀ ਅੱਖਰ*use_ptr = ਮਾਪ_2 <= ਆਕਾਰ(val_) ? val_ : ptr_;

      ਬਿਨਾਨਿਸ਼ਾਨੀ idx = 0;
      ਜਦੋਂ ((idx < wid) && (ਅਧਾਰ+idx < ਮਾਪ_2)) {
	    tmp_ptr[idx] = use_ptr[ਅਧਾਰ+idx];
	    idx += 1;
      }

      ਵਾਪਸ tmp;
}

ਖਾਲੀ vvp_vector8_t::ਸੈੱਟ_vec(ਬਿਨਾਨਿਸ਼ਾਨੀ ਅਧਾਰ, ਅਚਲ vvp_vector8_t&that)
{
      ਪ੍ਰਤਿਜ੍ਞਾ((ਅਧਾਰ+that.size()) <= size());
      ਲਈ (ਬਿਨਾਨਿਸ਼ਾਨੀ idx = 0 ; idx < that.size() ; idx += 1)
	    ਸੈੱਟ_ਦੋਆਧਾਰੀ_ਅੰਕ(ਅਧਾਰ+idx, that.value(idx));
}

vvp_vector8_t part_expand(ਅਚਲ vvp_vector8_t&that, ਬਿਨਾਨਿਸ਼ਾਨੀ wid, ਬਿਨਾਨਿਸ਼ਾਨੀ off)
{
      ਪ੍ਰਤਿਜ੍ਞਾ(off < wid);
      vvp_vector8_t tmp (wid);

      ਬਿਨਾਨਿਸ਼ਾਨੀ ਅੱਖਰ* tmp_ptr = tmp.ਮਾਪ_2<= ਆਕਾਰ(tmp.val_) ?
                               tmp.val_ : tmp.ptr_;
      ਅਚਲ ਬਿਨਾਨਿਸ਼ਾਨੀ ਅੱਖਰ* that_ptr = that.ਮਾਪ_2<= ਆਕਾਰ(that.val_) ?
                                      that.val_ : that.ptr_;

      ਬਿਨਾਨਿਸ਼ਾਨੀ idx = off;

      ਜਦੋਂ (idx < wid && that.ਮਾਪ_2 > (idx-off)) {
	    tmp_ptr[idx] = that_ptr[idx-off];
	    idx += 1;
      }

      ਵਾਪਸ tmp;
}

ਸਥਿਰ ਤਾਰਕਿਕ c8string_ਸਿਰਲੇਖ_test(ਅਚਲ ਅੱਖਰ*str)
{
      ਜੇ ((str[0] != 'C') && (str[0] != 'c'))
	    ਵਾਪਸ ਝੂਠ;
      ਜੇ ((str[1] != '8') || (str[2] != '<'))
	    ਵਾਪਸ ਝੂਠ;
      ਵਾਪਸ ਸੱਚ;
}

ਤਾਰਕਿਕ c8string_test(ਅਚਲ ਅੱਖਰ*str)
{
      ਜੇ (!c8string_ਸਿਰਲੇਖ_test(str))
	    ਵਾਪਸ ਝੂਠ;

      ਅਚਲ ਅੱਖਰ*cp = str+3;
      ਲਈ (;; cp += 1) {
	    ਜੇ (cp[0] == '>' && cp[1] == 0) ਵਾਪਸ ਸੱਚ;
	    ਜੇ (cp[0] >= '0' && cp[0] <= '9') ਜਾਰੀ;
	    ਜੇ (cp[0] == 'x') ਜਾਰੀ;
	    ਜੇ (cp[0] == 'z') ਜਾਰੀ;
	    ਵਾਪਸ ਝੂਠ;
      }
      ਵਾਪਸ ਝੂਠ;
}
/*
 * The format of a C8<> string is:
 *   C8<aaabbbccc...>
 * where aaa... is a 3 character bit descriptor.
 */
vvp_vector8_t c8string_ਮੰਜ਼ਿਲ_vector8(ਅਚਲ ਅੱਖਰ*str)
{
      ਪ੍ਰਤਿਜ੍ਞਾ(c8string_ਸਿਰਲੇਖ_test(str));

      size_t vsize = ਲੜੀ_ਲੰਬਾਈ(str)-4;
      ਪ੍ਰਤਿਜ੍ਞਾ(vsize%3 == 0);
      vsize /= 3;
      vvp_vector8_t tmp (vsize);

      ਲਈ (size_t idx = 0 ; idx < vsize ; idx += 1) {
	    ਅਚਲ ਅੱਖਰ*cp = str+3+3*idx;
	    vvp_bit4_t ਦੋਆਧਾਰੀ_ਅੰਕ = BIT4_X;
	    ਬਿਨਾਨਿਸ਼ਾਨੀ dr0 = cp[0]-'0';
	    ਬਿਨਾਨਿਸ਼ਾਨੀ dr1 = cp[1]-'0';
	    ਚੋਣ (cp[2]) {
		ਹਾਲਤ '0':
		  ਦੋਆਧਾਰੀ_ਅੰਕ = BIT4_0;
		  ਰੋਕੋ;
		ਹਾਲਤ '1':
		  ਦੋਆਧਾਰੀ_ਅੰਕ = BIT4_1;
		  ਰੋਕੋ;
		ਹਾਲਤ 'x':
		  ਦੋਆਧਾਰੀ_ਅੰਕ = BIT4_X;
		  ਰੋਕੋ;
		ਹਾਲਤ 'z':
		  ਦੋਆਧਾਰੀ_ਅੰਕ = BIT4_Z;
		  ਰੋਕੋ;
	    }
	    tmp.ਸੈੱਟ_ਦੋਆਧਾਰੀ_ਅੰਕ(vsize-idx-1, vvp_scalar_t(ਦੋਆਧਾਰੀ_ਅੰਕ, dr0, dr1));
      }

      ਵਾਪਸ tmp;
}

ostream& ਸੰਚਾਲਕ<<(ostream&out, ਅਚਲ vvp_vector8_t&that)
{
      out << "C8<";
      ਲਈ (ਬਿਨਾਨਿਸ਼ਾਨੀ idx = 0 ;  idx < that.size() ; idx += 1)
	    out << that.value(that.size()-idx-1);

      out << ">";
      ਵਾਪਸ out;
}

vvp_net_fun_t::vvp_net_fun_t()
{
      ਗਿਣਤੀ_functors += 1;
}

vvp_net_fun_t::~vvp_net_fun_t()
{
}

ਖਾਲੀ vvp_net_fun_t::recv_vec4(vvp_net_ptr_t, ਅਚਲ vvp_vector4_t&,
                              vvp_context_t)
{
      fprintf(stderr, "internal error: %s: recv_vec4 not implemented\n",
	      typeid(*ਇਹ).name());
      ਪ੍ਰਤਿਜ੍ਞਾ(0);
}

ਖਾਲੀ vvp_net_fun_t::recv_vec4_pv_(vvp_net_ptr_t p, ਅਚਲ vvp_vector4_t&ਦੋਆਧਾਰੀ_ਅੰਕ,
                                  ਬਿਨਾਨਿਸ਼ਾਨੀ ਅਧਾਰ, ਬਿਨਾਨਿਸ਼ਾਨੀ vwid, vvp_context_t)
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

      ਪ੍ਰਤਿਜ੍ਞਾ(ਅਧਾਰ + ਦੋਆਧਾਰੀ_ਅੰਕ.size() <= vwid);

      vvp_vector4_t tmp(vwid, BIT4_Z);
      tmp.ਸੈੱਟ_vec(ਅਧਾਰ, ਦੋਆਧਾਰੀ_ਅੰਕ);
      recv_vec4(p, tmp, 0);
}

ਖਾਲੀ vvp_net_fun_t::recv_vec4_pv(vvp_net_ptr_t, ਅਚਲ vvp_vector4_t&ਦੋਆਧਾਰੀ_ਅੰਕ,
                                 ਬਿਨਾਨਿਸ਼ਾਨੀ ਅਧਾਰ, ਬਿਨਾਨਿਸ਼ਾਨੀ vwid, vvp_context_t)
{
      cerr << "internal error: " << typeid(*ਇਹ).name() << ": "
	   << "recv_vec4_pv(" << ਦੋਆਧਾਰੀ_ਅੰਕ << ", " << ਅਧਾਰ
	   << ", " << vwid << ") not implemented" << ਸਤਰ_ਅੰਤ;
      ਪ੍ਰਤਿਜ੍ਞਾ(0);
}

ਖਾਲੀ vvp_net_fun_t::recv_vec8(vvp_net_ptr_t ਪੋਰਟ, ਅਚਲ vvp_vector8_t&ਦੋਆਧਾਰੀ_ਅੰਕ)
{
      recv_vec4(ਪੋਰਟ, reduce4(ਦੋਆਧਾਰੀ_ਅੰਕ), 0);
}

ਖਾਲੀ vvp_net_fun_t::recv_vec8_pv_(vvp_net_ptr_t p, ਅਚਲ vvp_vector8_t&ਦੋਆਧਾਰੀ_ਅੰਕ,
				  ਬਿਨਾਨਿਸ਼ਾਨੀ ਅਧਾਰ, ਬਿਨਾਨਿਸ਼ਾਨੀ vwid)
{
	// This is the strength-aware version of recv_vec4_pv_.

      ਪ੍ਰਤਿਜ੍ਞਾ(ਅਧਾਰ + ਦੋਆਧਾਰੀ_ਅੰਕ.size() <= vwid);

      vvp_vector8_t tmp(vwid);
      tmp.ਸੈੱਟ_vec(ਅਧਾਰ, ਦੋਆਧਾਰੀ_ਅੰਕ);
      recv_vec8(p, tmp);
}

ਖਾਲੀ vvp_net_fun_t::recv_vec8_pv(vvp_net_ptr_t ਪੋਰਟ, ਅਚਲ vvp_vector8_t&ਦੋਆਧਾਰੀ_ਅੰਕ,
				 ਬਿਨਾਨਿਸ਼ਾਨੀ ਅਧਾਰ, ਬਿਨਾਨਿਸ਼ਾਨੀ vwid)
{
      recv_vec4_pv(ਪੋਰਟ, reduce4(ਦੋਆਧਾਰੀ_ਅੰਕ), ਅਧਾਰ, vwid, 0);
}

ਖਾਲੀ vvp_net_fun_t::recv_real(vvp_net_ptr_t, ਦੁੱਗਣਾ ਦੋਆਧਾਰੀ_ਅੰਕ, vvp_context_t)
{
      fprintf(stderr, "internal error: %s: recv_real(%f) not implemented\n",
	      typeid(*ਇਹ).name(), ਦੋਆਧਾਰੀ_ਅੰਕ);
      ਪ੍ਰਤਿਜ੍ਞਾ(0);
}

ਖਾਲੀ vvp_net_fun_t::recv_string(vvp_net_ptr_t, ਅਚਲ std::string&ਦੋਆਧਾਰੀ_ਅੰਕ, vvp_context_t)
{
      fprintf(stderr, "internal error: %s: recv_string(%s) not implemented\n",
	      typeid(*ਇਹ).name(), ਦੋਆਧਾਰੀ_ਅੰਕ.c_str());
      ਪ੍ਰਤਿਜ੍ਞਾ(0);
}

ਖਾਲੀ vvp_net_fun_t::recv_object(vvp_net_ptr_t, vvp_object_t, vvp_context_t)
{
      fprintf(stderr, "internal error: %s: recv_object(...) not implemented\n",
	      typeid(*ਇਹ).name());
      ਪ੍ਰਤਿਜ੍ਞਾ(0);
}

ਖਾਲੀ vvp_net_fun_t::force_flag(ਤਾਰਕਿਕ)
{
}

/* **** vvp_fun_drive methods **** */

vvp_fun_drive::vvp_fun_drive(ਬਿਨਾਨਿਸ਼ਾਨੀ str0, ਬਿਨਾਨਿਸ਼ਾਨੀ str1)
{
      ਪ੍ਰਤਿਜ੍ਞਾ(str0 < 8);
      ਪ੍ਰਤਿਜ੍ਞਾ(str1 < 8);

      drive0_ = str0;
      drive1_ = str1;
}

vvp_fun_drive::~vvp_fun_drive()
{
}

ਖਾਲੀ vvp_fun_drive::recv_vec4(vvp_net_ptr_t ਪੋਰਟ, ਅਚਲ vvp_vector4_t&ਦੋਆਧਾਰੀ_ਅੰਕ,
                              vvp_context_t)
{
      ਪ੍ਰਤਿਜ੍ਞਾ(ਪੋਰਟ.ਪੋਰਟ() == 0);
      ਪੋਰਟ.ptr()->send_vec8(vvp_vector8_t(ਦੋਆਧਾਰੀ_ਅੰਕ, drive0_, drive1_));
}

ਖਾਲੀ vvp_fun_drive::recv_vec4_pv(vvp_net_ptr_t ptr, ਅਚਲ vvp_vector4_t&ਦੋਆਧਾਰੀ_ਅੰਕ,
				 ਬਿਨਾਨਿਸ਼ਾਨੀ ਅਧਾਰ, ਬਿਨਾਨਿਸ਼ਾਨੀ vwid, vvp_context_t ctx)
{
      recv_vec4_pv_(ptr, ਦੋਆਧਾਰੀ_ਅੰਕ, ਅਧਾਰ, vwid, ctx);
}

/* **** vvp_wide_fun_* methods **** */

vvp_wide_fun_core::vvp_wide_fun_core(vvp_net_t*net, ਬਿਨਾਨਿਸ਼ਾਨੀ nports)
{
      ptr_ = net;
      nports_ = nports;
      ਪੋਰਟ_ਮੁੱਲਾਂ = 0;
      ਪੋਰਟ_rvalues = 0;
}

vvp_wide_fun_core::~vvp_wide_fun_core()
{
      ਮਿਟਾਓ[] ਪੋਰਟ_ਮੁੱਲਾਂ;
      ਮਿਟਾਓ[] ਪੋਰਟ_rvalues;
}

ਖਾਲੀ vvp_wide_fun_core::propagate_vec4(ਅਚਲ vvp_vector4_t&ਦੋਆਧਾਰੀ_ਅੰਕ,
				       vvp_time64_t delay)
{
      ਜੇ (delay)
	    ਸਮਾਂ_ਸਾਰਣੀ_propagate_ਸਦਿਸ਼(ptr_, delay, ਦੋਆਧਾਰੀ_ਅੰਕ);
      ਨਹੀਂਤਾਂ
	    ptr_->send_vec4(ਦੋਆਧਾਰੀ_ਅੰਕ, 0);
}

ਖਾਲੀ vvp_wide_fun_core::propagate_real(ਦੁੱਗਣਾ ਦੋਆਧਾਰੀ_ਅੰਕ,
				       vvp_time64_t delay)
{
      ਜੇ (delay) {
	      // schedule_assign_vector(ptr_->out, bit, delay);
	    ਪ੍ਰਤਿਜ੍ਞਾ(0); // Need a real-value version of assign_vector.
      } ਨਹੀਂਤਾਂ {
	    ptr_->send_real(ਦੋਆਧਾਰੀ_ਅੰਕ, 0);
      }
}


ਬਿਨਾਨਿਸ਼ਾਨੀ vvp_wide_fun_core::ਪੋਰਟ_ਗਿਣਤੀ() ਅਚਲ
{
      ਵਾਪਸ nports_;
}

vvp_vector4_t& vvp_wide_fun_core::value(ਬਿਨਾਨਿਸ਼ਾਨੀ idx)
{
      ਪ੍ਰਤਿਜ੍ਞਾ(idx < nports_);
      ਜੇ (ਪੋਰਟ_ਮੁੱਲਾਂ == 0)
	    ਪੋਰਟ_ਮੁੱਲਾਂ = ਨਵਾਂ vvp_vector4_t [nports_];
      ਵਾਪਸ ਪੋਰਟ_ਮੁੱਲਾਂ[idx];
}

ਦੁੱਗਣਾ vvp_wide_fun_core::ਮੁੱਲ_r(ਬਿਨਾਨਿਸ਼ਾਨੀ idx)
{
      ਪ੍ਰਤਿਜ੍ਞਾ(idx < nports_);
      ਵਾਪਸ ਪੋਰਟ_rvalues? ਪੋਰਟ_rvalues[idx] : 0.0;
}

ਖਾਲੀ vvp_wide_fun_core::recv_real_from_inputs(ਬਿਨਾਨਿਸ਼ਾਨੀ)
{
      ਪ੍ਰਤਿਜ੍ਞਾ(0);
}

ਖਾਲੀ vvp_wide_fun_core::dispatch_vec4_from_ਨਿਵੇਸ਼(ਬਿਨਾਨਿਸ਼ਾਨੀ ਪੋਰਟ,
						  ਅਚਲ vvp_vector4_t&ਦੋਆਧਾਰੀ_ਅੰਕ)
{
      ਪ੍ਰਤਿਜ੍ਞਾ(ਪੋਰਟ < nports_);
      ਜੇ (ਪੋਰਟ_ਮੁੱਲਾਂ == 0) ਪੋਰਟ_ਮੁੱਲਾਂ = ਨਵਾਂ vvp_vector4_t [nports_];
      ਪੋਰਟ_ਮੁੱਲਾਂ[ਪੋਰਟ] = ਦੋਆਧਾਰੀ_ਅੰਕ;
      recv_vec4_from_inputs(ਪੋਰਟ);
}

ਖਾਲੀ vvp_wide_fun_core::dispatch_real_from_ਨਿਵੇਸ਼(ਬਿਨਾਨਿਸ਼ਾਨੀ ਪੋਰਟ,
						  ਦੁੱਗਣਾ ਦੋਆਧਾਰੀ_ਅੰਕ)
{
      ਪ੍ਰਤਿਜ੍ਞਾ(ਪੋਰਟ < nports_);
      ਜੇ (ਪੋਰਟ_rvalues == 0) ਪੋਰਟ_rvalues = ਨਵਾਂ ਦੁੱਗਣਾ[nports_];
      ਪੋਰਟ_rvalues[ਪੋਰਟ] = ਦੋਆਧਾਰੀ_ਅੰਕ;
      recv_real_from_inputs(ਪੋਰਟ);
}

vvp_wide_fun_t::vvp_wide_fun_t(vvp_wide_fun_core*c, ਬਿਨਾਨਿਸ਼ਾਨੀ ਅਧਾਰ)
: core_(c), ਪੋਰਟ_ਅਧਾਰ_2(ਅਧਾਰ)
{
}

vvp_wide_fun_t::~vvp_wide_fun_t()
{
}

ਖਾਲੀ vvp_wide_fun_t::recv_vec4(vvp_net_ptr_t ਪੋਰਟ, ਅਚਲ vvp_vector4_t&ਦੋਆਧਾਰੀ_ਅੰਕ,
                               vvp_context_t)
{
      ਬਿਨਾਨਿਸ਼ਾਨੀ pidx = ਪੋਰਟ_ਅਧਾਰ_2 + ਪੋਰਟ.ਪੋਰਟ();
      core_->dispatch_vec4_from_ਨਿਵੇਸ਼(pidx, ਦੋਆਧਾਰੀ_ਅੰਕ);
}

ਖਾਲੀ vvp_wide_fun_t::recv_vec4_pv(vvp_net_ptr_t ptr, ਅਚਲ vvp_vector4_t&ਦੋਆਧਾਰੀ_ਅੰਕ,
				  ਬਿਨਾਨਿਸ਼ਾਨੀ ਅਧਾਰ, ਬਿਨਾਨਿਸ਼ਾਨੀ vwid, vvp_context_t ctx)
{
      recv_vec4_pv_(ptr, ਦੋਆਧਾਰੀ_ਅੰਕ, ਅਧਾਰ, vwid, ctx);
}

ਖਾਲੀ vvp_wide_fun_t::recv_real(vvp_net_ptr_t ਪੋਰਟ, ਦੁੱਗਣਾ ਦੋਆਧਾਰੀ_ਅੰਕ,
                               vvp_context_t)
{
      ਬਿਨਾਨਿਸ਼ਾਨੀ pidx = ਪੋਰਟ_ਅਧਾਰ_2 + ਪੋਰਟ.ਪੋਰਟ();
      core_->dispatch_real_from_ਨਿਵੇਸ਼(pidx, ਦੋਆਧਾਰੀ_ਅੰਕ);
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

ਬਿਨਾਨਿਸ਼ਾਨੀ vvp_scalar_t::strength0() ਅਚਲ
{
      ਵਾਪਸ STREN0(ਮੁੱਲ_2);
}

ਬਿਨਾਨਿਸ਼ਾਨੀ vvp_scalar_t::strength1() ਅਚਲ
{
      ਵਾਪਸ STREN1(ਮੁੱਲ_2);
}

ostream& ਸੰਚਾਲਕ <<(ostream&out, vvp_scalar_t a)
{
      out << a.strength0() << a.strength1();
      ਚੋਣ (a.value()) {
	  ਹਾਲਤ BIT4_0:
	    out << "0";
	    ਰੋਕੋ;
	  ਹਾਲਤ BIT4_1:
	    out << "1";
	    ਰੋਕੋ;
	  ਹਾਲਤ BIT4_X:
	    out << "X";
	    ਰੋਕੋ;
	  ਹਾਲਤ BIT4_Z:
	    out << "Z";
	    ਰੋਕੋ;
      }
      ਵਾਪਸ out;
}

/*
 * This function is only called if the actual interface function rules
 * out some of the easy cases. If we get here, we can assume that
 * neither of the values is HiZ, and the values are not exactly equal.
 */
vvp_scalar_t fully_featured_resolv_(vvp_scalar_t a, vvp_scalar_t b)
{

      ਜੇ (UNAMBIG(a.ਮੁੱਲ_2) && UNAMBIG(b.ਮੁੱਲ_2)) {

	      /* If both signals are unambiguous, simply choose
		 the stronger. If they have the same strength
		 but different values, then this becomes
		 ambiguous. */

	    ਜੇ ((b.ਮੁੱਲ_2&0x07) > (a.ਮੁੱਲ_2&0x07)) {

		    /* b value is stronger. Take it. */
		  ਵਾਪਸ b;

	    } ਨਹੀਂਤਾਂ ਜੇ ((b.ਮੁੱਲ_2&0x77) == (a.ਮੁੱਲ_2&0x77)) {

		    // Strengths are the same. Since we know already
		    // that the values are not the same, Make value
		    // into "x".
		  vvp_scalar_t tmp (a);
		  tmp.ਮੁੱਲ_2 = (tmp.ਮੁੱਲ_2&0x77) | 0x80;
		  ਵਾਪਸ tmp;

	    } ਨਹੀਂਤਾਂ {

		    /* Must be "a" is the stronger one. */
		  ਵਾਪਸ a;
	    }

      }

	/* If one of the signals is unambiguous, then it
	   will sweep up the weaker parts of the ambiguous
	   signal. The result may be ambiguous, or maybe not. */

      ਜੇ (UNAMBIG(a.ਮੁੱਲ_2)) {
	    vvp_scalar_t res;

	    ਜੇ ((a.ਮੁੱਲ_2&0x70) > (b.ਮੁੱਲ_2&0x70))
		  res.ਮੁੱਲ_2 |= a.ਮੁੱਲ_2&0xf0;
	    ਨਹੀਂਤਾਂ
		  res.ਮੁੱਲ_2 |= b.ਮੁੱਲ_2&0xf0;

	    ਜੇ ((a.ਮੁੱਲ_2&0x07) > (b.ਮੁੱਲ_2&0x07))
		  res.ਮੁੱਲ_2 |= a.ਮੁੱਲ_2&0x0f;
	    ਨਹੀਂਤਾਂ
		  res.ਮੁੱਲ_2 |= b.ਮੁੱਲ_2&0x0f;

	    ਵਾਪਸ res;

      } ਨਹੀਂਤਾਂ ਜੇ (UNAMBIG(b.ਮੁੱਲ_2)) {

	    vvp_scalar_t res;

	    ਜੇ ((b.ਮੁੱਲ_2&0x70) > (a.ਮੁੱਲ_2&0x70))
		  res.ਮੁੱਲ_2 |= b.ਮੁੱਲ_2&0xf0;
	    ਨਹੀਂਤਾਂ
		  res.ਮੁੱਲ_2 |= a.ਮੁੱਲ_2&0xf0;

	    ਜੇ ((b.ਮੁੱਲ_2&0x07) > (a.ਮੁੱਲ_2&0x07))
		  res.ਮੁੱਲ_2 |= b.ਮੁੱਲ_2&0x0f;
	    ਨਹੀਂਤਾਂ
		  res.ਮੁੱਲ_2 |= a.ਮੁੱਲ_2&0x0f;

	    ਵਾਪਸ res;

      }


	/* If both signals are ambiguous, then the result
	   has an even wider ambiguity. */

      ਬਿਨਾਨਿਸ਼ਾਨੀ tmp = 0;
      ਪੂਰਨ sv1a = (a.ਮੁੱਲ_2 & 0x80) ? STREN1(a.ਮੁੱਲ_2) : - STREN1(a.ਮੁੱਲ_2);
      ਪੂਰਨ sv0a = (a.ਮੁੱਲ_2 & 0x08) ? STREN0(a.ਮੁੱਲ_2) : - STREN0(a.ਮੁੱਲ_2);
      ਪੂਰਨ sv1b = (b.ਮੁੱਲ_2 & 0x80) ? STREN1(b.ਮੁੱਲ_2) : - STREN1(b.ਮੁੱਲ_2);
      ਪੂਰਨ sv0b = (b.ਮੁੱਲ_2 & 0x08) ? STREN0(b.ਮੁੱਲ_2) : - STREN0(b.ਮੁੱਲ_2);

      ਪੂਰਨ sv1 = sv1a;
      ਪੂਰਨ sv0 = sv0a;

      ਜੇ (sv0a > sv1)
	    sv1 = sv0a;
      ਜੇ (sv1b > sv1)
	    sv1 = sv1b;
      ਜੇ (sv0b > sv1)
	    sv1 = sv0b;

      ਜੇ (sv1a < sv0)
	    sv0 = sv1a;
      ਜੇ (sv1b < sv0)
	    sv0 = sv1b;
      ਜੇ (sv0b < sv0)
	    sv0 = sv0b;

      ਜੇ (sv1 > 0) {
	    tmp |= 0x80;
	    tmp |= sv1 << 4;
      } ਨਹੀਂਤਾਂ {
	      /* Set the MSB when both arguments MSBs are set. This
		 can only happen if both one strengths are zero. */
	    tmp |= (a.ਮੁੱਲ_2&b.ਮੁੱਲ_2)&0x80;
	    tmp |= (-sv1) << 4;
      }

      ਜੇ (sv0 > 0) {
	    tmp |= 0x08;
	    tmp |= sv0;
      } ਨਹੀਂਤਾਂ {
	    tmp |= (-sv0);
      }

      vvp_scalar_t res;
      res.ਮੁੱਲ_2 = tmp;

	/* Canonicalize the HiZ value. */
      ਜੇ ((res.ਮੁੱਲ_2&0x77) == 0)
	    res.ਮੁੱਲ_2 = 0;

      ਵਾਪਸ res;
}

ਬਿਨਾਨਿਸ਼ਾਨੀ vvp_switch_strength_map[2][8] = {
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

vvp_vector4_t reduce4(ਅਚਲ vvp_vector8_t&that)
{
      vvp_vector4_t out (that.size());
      ਲਈ (ਬਿਨਾਨਿਸ਼ਾਨੀ idx = 0 ;  idx < out.size() ;  idx += 1)
	    out.ਸੈੱਟ_ਦੋਆਧਾਰੀ_ਅੰਕ(idx, that.value(idx).value());

      ਵਾਪਸ out;
}

vvp_bit4_t compare_gtge(ਅਚਲ vvp_vector4_t&lef, ਅਚਲ vvp_vector4_t&rig,
			vvp_bit4_t ਬਾਹਰਲਾ_if_equal)
{
      ਬਿਨਾਨਿਸ਼ਾਨੀ min_ਮਾਪ = lef.size();
      ਜੇ (rig.size() < min_ਮਾਪ)
	    min_ਮਾਪ = rig.size();

	// If one of the inputs is nil, treat is as all X values, and
	// that makes the result BIT4_X.
      ਜੇ (min_ਮਾਪ == 0)
	    ਵਾਪਸ BIT4_X;

	// As per the IEEE1364 definition of >, >=, < and <=, if there
	// are any X or Z values in either of the operand vectors,
	// then the result of the compare is BIT4_X.

	// Check for X/Z in the left operand
      ਜੇ (lef.has_xz())
	    ਵਾਪਸ BIT4_X;

	// Check for X/Z in the right operand
      ਜੇ (rig.has_xz())
	    ਵਾਪਸ BIT4_X;

      ਲਈ (ਬਿਨਾਨਿਸ਼ਾਨੀ idx = lef.size() ; idx > rig.size() ;  idx -= 1) {
	    ਜੇ (lef.value(idx-1) == BIT4_1)
		  ਵਾਪਸ BIT4_1;
      }

      ਲਈ (ਬਿਨਾਨਿਸ਼ਾਨੀ idx = rig.size() ; idx > lef.size() ;  idx -= 1) {
	    ਜੇ (rig.value(idx-1) == BIT4_1)
		  ਵਾਪਸ BIT4_0;
      }

      ਲਈ (ਬਿਨਾਨਿਸ਼ਾਨੀ idx = min_ਮਾਪ ; idx > 0 ;  idx -= 1) {
	    vvp_bit4_t lv = lef.value(idx-1);
	    vvp_bit4_t rv = rig.value(idx-1);

	    ਜੇ (lv == rv)
		  ਜਾਰੀ;

	    ਜੇ (lv == BIT4_1)
		  ਵਾਪਸ BIT4_1;
	    ਨਹੀਂਤਾਂ
		  ਵਾਪਸ BIT4_0;
      }

      ਵਾਪਸ ਬਾਹਰਲਾ_if_equal;
}

vvp_bit4_t compare_gtge_signed(ਅਚਲ vvp_vector4_t&a,
			       ਅਚਲ vvp_vector4_t&b,
			       vvp_bit4_t ਬਾਹਰਲਾ_if_equal)
{
      ਪ੍ਰਤਿਜ੍ਞਾ(a.size() == b.size());

      ਬਿਨਾਨਿਸ਼ਾਨੀ sign_idx = a.size()-1;
      vvp_bit4_t a_sign = a.value(sign_idx);
      vvp_bit4_t b_sign = b.value(sign_idx);

      ਜੇ (bit4_is_xz(a_sign))
	    ਵਾਪਸ BIT4_X;
      ਜੇ (bit4_is_xz(b_sign))
	    ਵਾਪਸ BIT4_X;

      ਜੇ (a_sign == b_sign)
	    ਵਾਪਸ compare_gtge(a, b, ਬਾਹਰਲਾ_if_equal);

      ਜੇ (a.has_xz())
	    ਵਾਪਸ BIT4_X;

      ਜੇ (b.has_xz())
	    ਵਾਪਸ BIT4_X;

      ਜੇ(a_sign == BIT4_0)
	    ਵਾਪਸ BIT4_1;
      ਨਹੀਂਤਾਂ
	    ਵਾਪਸ BIT4_0;
}
