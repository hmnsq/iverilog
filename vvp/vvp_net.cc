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
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ CHECK_WITH_VALGRIND
# include  <valgrind/memcheck.h>
# include  <map>
# include  "sfunc.h"
# include  "udp.h"
# include  "ivl_alloc.h"
#ಆದರೆ_ಅಂತ್ಯ

ಬಳಸಿ ನಾಮಾಕಾಶ std;

/* This is the size of an unsigned long in bits. This is just a
   convenience macro. */
# define CPU_ಶಬ್ದ_ಬಿಟ್ಟುಗಳು (8*ಗಾತ್ರ(ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ))
# define ತುದಿ_ಬಿಟ್ (1UL << (CPU_ಶಬ್ದ_ಬಿಟ್ಟುಗಳು-1))

permaheap vvp_net_fun_t::ರಾಶಿ;
permaheap vvp_net_fil_t::ರಾಶಿ;

// Allocate around 1Megabyte/chunk.
ಸ್ಥಿರ ಸ್ಥಿರಾಂಕ size_t VVP_NET_CHUNK = 1024*1024/ಗಾತ್ರ(vvp_net_t);
ಸ್ಥಿರ vvp_net_t*vvp_net_alloc_ಕೋಷ್ಟಕ = ಖಾಲಿ;
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ CHECK_WITH_VALGRIND
ಸ್ಥಿರ vvp_net_t **vvp_net_pool = ಖಾಲಿ;
ಸ್ಥಿರ ಚಿಹ್ನೆರಹಿತ vvp_net_pool_ಎಣಿಕೆ = 0;
#ಆದರೆ_ಅಂತ್ಯ
ಸ್ಥಿರ size_t vvp_net_alloc_ಉಳಿದ = 0;
// For statistics, count the vvp_nets allocated and the bytes of alloc
// chunks allocated.
ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ ಎಣಿಕೆ_vvp_nets = 0;
size_t ಗಾತ್ರ_vvp_nets = 0;

ಶೂನ್ಯ* vvp_net_t::ಕ್ರಿಯಾಕಾರಕ ಹೊಸ (size_t size)
{
      ಪ್ರತಿಜ್ಞಾ(size == ಗಾತ್ರ(vvp_net_t));
      ಆದರೆ (vvp_net_alloc_ಉಳಿದ == 0) {
	    vvp_net_alloc_ಕೋಷ್ಟಕ = ::ಹೊಸ vvp_net_t[VVP_NET_CHUNK];
	    vvp_net_alloc_ಉಳಿದ = VVP_NET_CHUNK;
	    ಗಾತ್ರ_vvp_nets += size*VVP_NET_CHUNK;
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ CHECK_WITH_VALGRIND
	    VALGRIND_MAKE_MEM_NOACCESS(vvp_net_alloc_ಕೋಷ್ಟಕ, size*VVP_NET_CHUNK);
	    VALGRIND_CREATE_MEMPOOL(vvp_net_alloc_ಕೋಷ್ಟಕ, 0, 0);
	    vvp_net_pool_ಎಣಿಕೆ += 1;
	    vvp_net_pool = static_cast<vvp_net_t **>(realloc(vvp_net_pool,
	                   vvp_net_pool_ಎಣಿಕೆ*ಗಾತ್ರ(vvp_net_t **)));
	    vvp_net_pool[vvp_net_pool_ಎಣಿಕೆ-1] = vvp_net_alloc_ಕೋಷ್ಟಕ;
#ಆದರೆ_ಅಂತ್ಯ
      }

      vvp_net_t*return_this = vvp_net_alloc_ಕೋಷ್ಟಕ;
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ CHECK_WITH_VALGRIND
      VALGRIND_MEMPOOL_ALLOC(vvp_net_pool[vvp_net_pool_ಎಣಿಕೆ-1],
                             return_this, size);
      return_this->pool = vvp_net_pool[vvp_net_pool_ಎಣಿಕೆ-1];
#ಆದರೆ_ಅಂತ್ಯ
      vvp_net_alloc_ಕೋಷ್ಟಕ += 1;
      vvp_net_alloc_ಉಳಿದ -= 1;
      ಎಣಿಕೆ_vvp_nets += 1;
      ಹಿಂತಿರುಗಿ return_this;
}

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ CHECK_WITH_VALGRIND
ಸ್ಥಿರ map<vvp_net_t*, ತಾರ್ಕಿಕ> vvp_net_map;
ಸ್ಥಿರ map<sfunc_core*, ತಾರ್ಕಿಕ> sfunc_map;
ಸ್ಥಿರ map<vvp_udp_fun_core*, ತಾರ್ಕಿಕ> udp_map;
ಸ್ಥಿರ map<resolv_core*, ತಾರ್ಕಿಕ> resolv_map;
ಸ್ಥಿರ vvp_net_t **local_net_pool = 0;
ಸ್ಥಿರ ಚಿಹ್ನೆರಹಿತ local_net_pool_ಎಣಿಕೆ = 0;

ಶೂನ್ಯ pool_local_net(vvp_net_t*net)
{
      local_net_pool_ಎಣಿಕೆ += 1;
      local_net_pool = static_cast<vvp_net_t **>(realloc(local_net_pool,
                       local_net_pool_ಎಣಿಕೆ*ಗಾತ್ರ(vvp_net_t **)));
      local_net_pool[local_net_pool_ಎಣಿಕೆ-1] = net;
}

ಶೂನ್ಯ vvp_net_delete(vvp_net_t *ಅಂಶ)
{
      vvp_net_map[ಅಂಶ] = ನಿಜ;
      ಆದರೆ (sfunc_core*tmp = dynamic_cast<sfunc_core*> (ಅಂಶ->fun)) {
	    sfunc_map[tmp] = ನಿಜ;
      }
      ಆದರೆ (vvp_udp_fun_core*tmp = dynamic_cast<vvp_udp_fun_core*> (ಅಂಶ->fun)) {
	    udp_map[tmp] = ನಿಜ;
      }
      ಆದರೆ (resolv_core*tmp = dynamic_cast<resolv_core*> (ಅಂಶ->fun)) {
	    resolv_map[tmp] = ನಿಜ;
      }
	/* Only delete static object variables. */
      ಆದರೆ (vvp_fun_signal_object_sa*tmp = dynamic_cast<vvp_fun_signal_object_sa*> (ಅಂಶ->fun)) {
	    ಅಳಿಸು tmp;
      }
	/* Only delete static string variables. */
      ಆದರೆ (vvp_fun_signal_string_sa*tmp = dynamic_cast<vvp_fun_signal_string_sa*> (ಅಂಶ->fun)) {
	    ಅಳಿಸು tmp;
      }
}

ಶೂನ್ಯ vvp_net_pool_delete()
{
      ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ vvp_nets_del = 0;

      ಗಾಗಿ (ಚಿಹ್ನೆರಹಿತ idx = 0; idx < local_net_pool_ಎಣಿಕೆ; idx += 1) {
	    vvp_net_delete(local_net_pool[idx]);
      }
      ಬಿಡುಗಡೆ(local_net_pool);
      local_net_pool = 0;
      local_net_pool_ಎಣಿಕೆ = 0;

      map<vvp_net_t*, ತಾರ್ಕಿಕ>::iterator iter;
      ಗಾಗಿ (iter = vvp_net_map.begin(); iter != vvp_net_map.ಕೊನೆ(); ++ iter ) {
	    vvp_nets_del += 1;
	    VALGRIND_MEMPOOL_FREE(iter->ಮೊದಲ->pool, iter->ಮೊದಲ);
      }
      vvp_net_map.clear();

      map<sfunc_core*, ತಾರ್ಕಿಕ>::iterator siter;
      ಗಾಗಿ (siter = sfunc_map.begin(); siter != sfunc_map.ಕೊನೆ(); ++ siter ) {
	    ಅಳಿಸು siter->ಮೊದಲ;
      }
      sfunc_map.clear();

      map<vvp_udp_fun_core*, ತಾರ್ಕಿಕ>::iterator uiter;
      ಗಾಗಿ (uiter = udp_map.begin(); uiter != udp_map.ಕೊನೆ(); ++ uiter ) {
	    ಅಳಿಸು uiter->ಮೊದಲ;
      }
      udp_map.clear();

      map<resolv_core*, ತಾರ್ಕಿಕ>::iterator riter;
      ಗಾಗಿ (riter = resolv_map.begin(); riter != resolv_map.ಕೊನೆ(); ++ riter ) {
	    ಅಳಿಸು riter->ಮೊದಲ;
      }
      resolv_map.clear();

      ಆದರೆ (RUNNING_ON_VALGRIND && (vvp_nets_del != ಎಣಿಕೆ_vvp_nets)) {
	    fflush(ಖಾಲಿ);
	    VALGRIND_PRINTF("Error: vvp missed deleting %ld of %lu net(s).",
	                    (ದೀರ್ಘ) ಎಣಿಕೆ_vvp_nets - vvp_nets_del,
	                    ಎಣಿಕೆ_vvp_nets);
      }

      ಗಾಗಿ (ಚಿಹ್ನೆರಹಿತ idx = 0; idx < vvp_net_pool_ಎಣಿಕೆ; idx += 1) {
	    VALGRIND_DESTROY_MEMPOOL(vvp_net_pool[idx]);
	    ::ಅಳಿಸು [] vvp_net_pool[idx];
      }
      ಬಿಡುಗಡೆ(vvp_net_pool);
      vvp_net_pool = ಖಾಲಿ;
      vvp_net_pool_ಎಣಿಕೆ = 0;
}
#ಆದರೆ_ಅಂತ್ಯ

ಶೂನ್ಯ vvp_net_t::ಕ್ರಿಯಾಕಾರಕ ಅಳಿಸು(ಶೂನ್ಯ*)
{
      ಪ್ರತಿಜ್ಞಾ(0);
}

vvp_net_t::vvp_net_t()
: ಹೊರಹೋಗು_2(vvp_net_ptr_t(0,0))
{
      fun = 0;
      fil = 0;
}

ಶೂನ್ಯ vvp_net_t::link(vvp_net_ptr_t ಪೋರ್ಟ್_ಗಮ್ಯ_link)
{
      vvp_net_t*net = ಪೋರ್ಟ್_ಗಮ್ಯ_link.ptr();

	// Connect nodes with vvp_fun_modpath_src to the head of the
	// linked list, so that vvp_fun_modpath_src are always evaluated
	// before their respective vvp_fun_modpath
      ಆದರೆ (dynamic_cast<vvp_fun_modpath_src*>(net->fun)) {
	    net->ಪೋರ್ಟ್[ಪೋರ್ಟ್_ಗಮ್ಯ_link.ಪೋರ್ಟ್()] = ಹೊರಹೋಗು_2;
	    ಹೊರಹೋಗು_2 = ಪೋರ್ಟ್_ಗಮ್ಯ_link;
	// All other nodes connect after the last vvp_fun_modpath_src
      } ಇಲ್ಲದಿದ್ದರೆ {
	      // List is empty or first node has no vvp_fun_modpath_src
	      // then just insert the node into the head of linked list
	    ಆದರೆ (ಹೊರಹೋಗು_2.ptr() == ಶೂನ್ಯಸೂಚಕ || ! dynamic_cast<vvp_fun_modpath_src*>(ಹೊರಹೋಗು_2.ptr()->fun)) {
		  net->ಪೋರ್ಟ್[ಪೋರ್ಟ್_ಗಮ್ಯ_link.ಪೋರ್ಟ್()] = ಹೊರಹೋಗು_2;
		  ಹೊರಹೋಗು_2 = ಪೋರ್ಟ್_ಗಮ್ಯ_link;
		  ಹಿಂತಿರುಗಿ;
	    }

	    vvp_net_ptr_t cur = ಹೊರಹೋಗು_2;
	    vvp_net_ptr_t prev = vvp_net_ptr_t(ಶೂನ್ಯಸೂಚಕ,0);

	      // Traverse through linked list
	    ಆಗ (dynamic_cast<vvp_fun_modpath_src*>(cur.ptr()->fun)) {
		  prev = cur;
		  cur = cur.ptr()->ಪೋರ್ಟ್[cur.ಪೋರ್ಟ್()];
		  ಆದರೆ (! cur.ptr()) ನಿಲ್ಲಿಸು; // End of list
	    }

	    ಪ್ರತಿಜ್ಞಾ(prev.ptr());

	      // Insert after last vvp_fun_modpath_src (or end of list)
	    net->ಪೋರ್ಟ್[ಪೋರ್ಟ್_ಗಮ್ಯ_link.ಪೋರ್ಟ್()] = cur;
	    prev.ptr()->ಪೋರ್ಟ್[prev.ಪೋರ್ಟ್()] = ಪೋರ್ಟ್_ಗಮ್ಯ_link;
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
      ಚಿಹ್ನೆರಹಿತ net_ಪೋರ್ಟ್ = dst_ptr.ಪೋರ್ಟ್();

      ಆದರೆ (ಹೊರಹೋಗು_2 == dst_ptr) {
	      /* If the drive fan-out list starts with this pointer,
		 then the unlink is easy. Pull the list forward. */
	    ಹೊರಹೋಗು_2 = net->ಪೋರ್ಟ್[net_ಪೋರ್ಟ್];
      } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (! ಹೊರಹೋಗು_2.nil()) {
	      /* Scan the linked list, looking for the net_ptr_t
		 pointer *before* the one we wish to remove. */
	    vvp_net_ptr_t cur = ಹೊರಹೋಗು_2;
	    ಪ್ರತಿಜ್ಞಾ(!cur.nil());
	    vvp_net_t*cur_net = cur.ptr();
	    ಚಿಹ್ನೆರಹಿತ cur_ಪೋರ್ಟ್ = cur.ಪೋರ್ಟ್();
	    ಆಗ (cur_net && cur_net->ಪೋರ್ಟ್[cur_ಪೋರ್ಟ್] != dst_ptr) {
		  cur = cur_net->ಪೋರ್ಟ್[cur_ಪೋರ್ಟ್];
		  cur_net = cur.ptr();
		  cur_ಪೋರ್ಟ್ = cur.ಪೋರ್ಟ್();
	    }
	      /* Unlink. */
	    ಆದರೆ (cur_net) cur_net->ಪೋರ್ಟ್[cur_ಪೋರ್ಟ್] = net->ಪೋರ್ಟ್[net_ಪೋರ್ಟ್];
      }

      net->ಪೋರ್ಟ್[net_ಪೋರ್ಟ್] = vvp_net_ptr_t(0,0);
}

ಶೂನ್ಯ vvp_net_t::ಎಣಿಕೆ_drivers(ಚಿಹ್ನೆರಹಿತ idx, ಚಿಹ್ನೆರಹಿತ counts[4])
{
      counts[0] = 0;
      counts[1] = 0;
      counts[2] = 0;
      counts[3] = 0;

        /* $countdrivers can only be used on wires. */
      vvp_wire_ತಳ*wire=dynamic_cast<vvp_wire_ತಳ*>(fil);
      ಪ್ರತಿಜ್ಞಾ(wire);

      ಆದರೆ (wire->is_forced(idx))
            counts[3] = 1;

        /* If the net has multiple drivers, we need to interrogate the
           resolver network to get the driven values. */
      ಆದರೆ (resolv_core*resolver = dynamic_cast<resolv_core*>(fun)) {
            resolver->ಎಣಿಕೆ_drivers(idx, counts);
            ಹಿಂತಿರುಗಿ;
      }
      ಆದರೆ (vvp_island_ಪೋರ್ಟ್*resolver = dynamic_cast<vvp_island_ಪೋರ್ಟ್*>(fun)) {
            resolver->ಎಣಿಕೆ_drivers(idx, counts);
            ಹಿಂತಿರುಗಿ;
      }

        /* If the functor is not a resolver, there is only one driver, so
           we can just interrogate the filter to get the driven value. */
      update_driver_counts(wire->driven_ಮೌಲ್ಯ(idx), counts);
}

ಶೂನ್ಯ vvp_net_fun_t::ಕ್ರಿಯಾಕಾರಕ ಅಳಿಸು(ಶೂನ್ಯ*)
{
#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ CHECK_WITH_VALGRIND
      ಪ್ರತಿಜ್ಞಾ(0);
#ಆದರೆ_ಅಂತ್ಯ
}


vvp_net_fil_t::vvp_net_fil_t()
{
      force_link_ = 0;
      force_propagate_ = ಸುಳ್ಳು;
      ಎಣಿಕೆ_filters += 1;
}

vvp_net_fil_t::~vvp_net_fil_t()
{
      ಪ್ರತಿಜ್ಞಾ(force_link_ == 0);
}

vvp_net_fil_t::prop_t vvp_net_fil_t::filter_vec4(ಸ್ಥಿರಾಂಕ vvp_vector4_t&,
                                                 vvp_vector4_t&,
                                                 ಚಿಹ್ನೆರಹಿತ, ಚಿಹ್ನೆರಹಿತ)
{
      ಹಿಂತಿರುಗಿ PROP;
}

vvp_net_fil_t::prop_t vvp_net_fil_t::filter_vec8(ಸ್ಥಿರಾಂಕ vvp_vector8_t&,
                                                 vvp_vector8_t&,
                                                 ಚಿಹ್ನೆರಹಿತ, ಚಿಹ್ನೆರಹಿತ)
{
      ಹಿಂತಿರುಗಿ PROP;
}

vvp_net_fil_t::prop_t vvp_net_fil_t::filter_real(ದ್ವಿಗುಣ&)
{
      ಹಿಂತಿರುಗಿ PROP;
}

vvp_net_fil_t::prop_t vvp_net_fil_t::filter_string(ಸ್ಥಿರಾಂಕ string&)
{
      ಹಿಂತಿರುಗಿ PROP;
}

vvp_net_fil_t::prop_t vvp_net_fil_t::filter_object(vvp_object_t&)
{
      ಹಿಂತಿರುಗಿ PROP;
}

ಶೂನ್ಯ vvp_net_fil_t::force_ಮುಖವಾಡ(ಸ್ಥಿರಾಂಕ vvp_vector2_t&ಮುಖವಾಡ)
{
      ಆದರೆ (force_ಮುಖವಾಡ_2.size() == 0)
	    force_ಮುಖವಾಡ_2 = vvp_vector2_t(vvp_vector2_t::FILL0, ಮುಖವಾಡ.size());

      ಪ್ರತಿಜ್ಞಾ(force_ಮುಖವಾಡ_2.size() == ಮುಖವಾಡ.size());
      ಗಾಗಿ (ಚಿಹ್ನೆರಹಿತ idx = 0 ; idx < ಮುಖವಾಡ.size() ; idx += 1) {
	    ಆದರೆ (ಮುಖವಾಡ.value(idx) == 0)
		  ಮುಂದುವರಿ;

	    force_ಮುಖವಾಡ_2.ಗಣ_ಬಿಟ್(idx, 1);
	    force_propagate_ = ನಿಜ;
      }
}

ಶೂನ್ಯ vvp_net_fil_t::release_ಮುಖವಾಡ(ಸ್ಥಿರಾಂಕ vvp_vector2_t&ಮುಖವಾಡ)
{
      ಆದರೆ (force_ಮುಖವಾಡ_2.size() == 0)
	    ಹಿಂತಿರುಗಿ;

      ಪ್ರತಿಜ್ಞಾ(force_ಮುಖವಾಡ_2.size() == ಮುಖವಾಡ.size());
      ಗಾಗಿ (ಚಿಹ್ನೆರಹಿತ idx = 0 ; idx < ಮುಖವಾಡ.size() ; idx += 1) {
	    ಆದರೆ (ಮುಖವಾಡ.value(idx))
		  force_ಮುಖವಾಡ_2.ಗಣ_ಬಿಟ್(idx, 0);
      }

      ಆದರೆ (force_ಮುಖವಾಡ_2.is_zero())
	    force_ಮುಖವಾಡ_2 = vvp_vector2_t();
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
      ಪ್ರತಿಜ್ಞಾ(dst->fil == ಇದು);

      ಆದರೆ (force_link_ == 0) {
	    force_link_ = ಹೊಸ vvp_net_t;
	      // Use port[3] to hold the force destination.
	    force_link_->ಪೋರ್ಟ್[3] = vvp_net_ptr_t(dst, 0);
	    force_link_->ಪೋರ್ಟ್[2] = vvp_net_ptr_t(0,0);
	    force_link_->fun = ಹೊಸ vvp_fun_force;
      }

      force_unlink();
      ಪ್ರತಿಜ್ಞಾ(force_link_->ಪೋರ್ಟ್[2] == vvp_net_ptr_t(0,0));

	// Use port[2] to hold the force source.
      force_link_->ಪೋರ್ಟ್[2] = vvp_net_ptr_t(src,0);

      vvp_net_ptr_t dst_ptr(force_link_, 0);
      src->link(dst_ptr);
}

ಶೂನ್ಯ vvp_net_fil_t::ಕ್ರಿಯಾಕಾರಕ ಅಳಿಸು(ಶೂನ್ಯ*)
{
      ಪ್ರತಿಜ್ಞಾ(0);
}

ಶೂನ್ಯ vvp_net_fil_t::force_unlink(ಶೂನ್ಯ)
{
      ಆದರೆ (force_link_ == 0) ಹಿಂತಿರುಗಿ;
      vvp_net_t*src = force_link_->ಪೋರ್ಟ್[2].ptr();
      ಆದರೆ (src == 0) ಹಿಂತಿರುಗಿ;

      src->unlink(vvp_net_ptr_t(force_link_,0));
      force_link_->ಪೋರ್ಟ್[2] = vvp_net_ptr_t(0,0);
}

/* *** BIT operations *** */
vvp_bit4_t ಕೂಡಿಸು_with_ವಾಹಕ(vvp_bit4_t a, vvp_bit4_t b, vvp_bit4_t&c)
{
      ಆದರೆ (bit4_is_xz(a) || bit4_is_xz(b) || bit4_is_xz(c)) {
	    c = BIT4_X;
	    ಹಿಂತಿರುಗಿ BIT4_X;
      }

	// NOTE: This relies on the facts that XZ values have been
	// weeded out, and that BIT4_1 is 1 and BIT4_0 is 0.
      ಪೂರ್ಣಾಂಕ ಮೊತ್ತ = (ಪೂರ್ಣಾಂಕ)a + (ಪೂರ್ಣಾಂಕ)b + (ಪೂರ್ಣಾಂಕ)c;

      ಆಯ್ಕೆ (ಮೊತ್ತ) {
	  ಸ್ಥಿತಿ 0:
	      // c must already be 0.
	    ಹಿಂತಿರುಗಿ BIT4_0;
	  ಸ್ಥಿತಿ 1:
	    c = BIT4_0;
	    ಹಿಂತಿರುಗಿ BIT4_1;
	  ಸ್ಥಿತಿ 2:
	    c = BIT4_1;
	    ಹಿಂತಿರುಗಿ BIT4_0;
	  ಸ್ಥಿತಿ 3:
	    c = BIT4_1;
	    ಹಿಂತಿರುಗಿ BIT4_1;
      }
      fprintf(stderr, "Incorrect result %d.\n", ಮೊತ್ತ);
      ಪ್ರತಿಜ್ಞಾ(0);
      ಹಿಂತಿರುಗಿ BIT4_X;
}

vvp_bit4_t scalar_ಗಮ್ಯ_bit4(PLI_INT32 scalar)
{
      ಆಯ್ಕೆ(scalar) {
	  ಸ್ಥಿತಿ vpi0:
	    ಹಿಂತಿರುಗಿ BIT4_0;
	  ಸ್ಥಿತಿ vpi1:
	    ಹಿಂತಿರುಗಿ BIT4_1;
	  ಸ್ಥಿತಿ vpiX:
	    ಹಿಂತಿರುಗಿ BIT4_X;
	  ಸ್ಥಿತಿ vpiZ:
	    ಹಿಂತಿರುಗಿ BIT4_Z;
      }
      fprintf(stderr, "Unsupported scalar value %d.\n", (ಪೂರ್ಣಾಂಕ)scalar);
      ಪ್ರತಿಜ್ಞಾ(0);
      ಹಿಂತಿರುಗಿ BIT4_X;
}

vvp_bit4_t ಕ್ರಿಯಾಕಾರಕ ^ (vvp_bit4_t a, vvp_bit4_t b)
{
      ಆದರೆ (bit4_is_xz(a))
	    ಹಿಂತಿರುಗಿ BIT4_X;
      ಆದರೆ (bit4_is_xz(b))
	    ಹಿಂತಿರುಗಿ BIT4_X;
      ಆದರೆ (a == BIT4_0)
	    ಹಿಂತಿರುಗಿ b;
      ಆದರೆ (b == BIT4_0)
	    ಹಿಂತಿರುಗಿ a;
      ಹಿಂತಿರುಗಿ BIT4_0;
}

ostream& ಕ್ರಿಯಾಕಾರಕ<<(ostream&out, vvp_bit4_t ಬಿಟ್)
{
      ಆಯ್ಕೆ (ಬಿಟ್) {
	  ಸ್ಥಿತಿ BIT4_0:
	    out << "0";
	    ನಿಲ್ಲಿಸು;
	  ಸ್ಥಿತಿ BIT4_1:
	    out << "1";
	    ನಿಲ್ಲಿಸು;
	  ಸ್ಥಿತಿ BIT4_X:
	    out << "X";
	    ನಿಲ್ಲಿಸು;
	  ಸ್ಥಿತಿ BIT4_Z:
	    out << "Z";
	    ನಿಲ್ಲಿಸು;
	  ಪೂರ್ವನಿಯೋಜಿತ:
	    out << "?";
	    ನಿಲ್ಲಿಸು;
      }
      ಹಿಂತಿರುಗಿ out;
}

ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ಚಿಹ್ನೆರಹಿತ ಗಿಡ್ಡ ಅಂಚು_t;

ಸಾಲೊಳಗೆ ಅಂಚು_t VVP_ಅಂಚು(vvp_bit4_t from, vvp_bit4_t to)
{
      ಹಿಂತಿರುಗಿ 1 << ((from << 2) | to);
}

ಸ್ಥಿರಾಂಕ ಅಂಚು_t vvp_ಅಂಚು_posedge
      = VVP_ಅಂಚು(BIT4_0,BIT4_1)
      | VVP_ಅಂಚು(BIT4_0,BIT4_X)
      | VVP_ಅಂಚು(BIT4_0,BIT4_Z)
      | VVP_ಅಂಚು(BIT4_X,BIT4_1)
      | VVP_ಅಂಚು(BIT4_Z,BIT4_1)
      ;

ಸ್ಥಿರಾಂಕ ಅಂಚು_t vvp_ಅಂಚು_negedge
      = VVP_ಅಂಚು(BIT4_1,BIT4_0)
      | VVP_ಅಂಚು(BIT4_1,BIT4_X)
      | VVP_ಅಂಚು(BIT4_1,BIT4_Z)
      | VVP_ಅಂಚು(BIT4_X,BIT4_0)
      | VVP_ಅಂಚು(BIT4_Z,BIT4_0)
      ;

ಪೂರ್ಣಾಂಕ ಅಂಚು(vvp_bit4_t from, vvp_bit4_t to)
{
      ಅಂಚು_t ಮುಖವಾಡ = VVP_ಅಂಚು(from, to);
      ಆದರೆ (ಮುಖವಾಡ & vvp_ಅಂಚು_posedge)
	    ಹಿಂತಿರುಗಿ 1;
      ಆದರೆ (ಮುಖವಾಡ & vvp_ಅಂಚು_negedge)
	    ಹಿಂತಿರುಗಿ -1;
      ಹಿಂತಿರುಗಿ 0;
}

ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ multiply_with_ವಾಹಕ(ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ a, ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ b,
				  ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ&ವಾಹಕ)
{
      ಸ್ಥಿರಾಂಕ ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ ಮುಖವಾಡ = (1UL << (CPU_ಶಬ್ದ_ಬಿಟ್ಟುಗಳು/2)) - 1;
      ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ a0 = a & ಮುಖವಾಡ;
      ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ a1 = (a >> (CPU_ಶಬ್ದ_ಬಿಟ್ಟುಗಳು/2)) & ಮುಖವಾಡ;
      ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ b0 = b & ಮುಖವಾಡ;
      ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ b1 = (b >> (CPU_ಶಬ್ದ_ಬಿಟ್ಟುಗಳು/2)) & ಮುಖವಾಡ;

      ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ tmp = a0 * b0;

      ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ r00 = tmp & ಮುಖವಾಡ;
      ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ c00 = (tmp >> (CPU_ಶಬ್ದ_ಬಿಟ್ಟುಗಳು/2)) & ಮುಖವಾಡ;

      tmp = a0 * b1;

      ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ r01 = tmp & ಮುಖವಾಡ;
      ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ c01 = (tmp >> (CPU_ಶಬ್ದ_ಬಿಟ್ಟುಗಳು/2)) & ಮುಖವಾಡ;

      tmp = a1 * b0;

      ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ r10 = tmp & ಮುಖವಾಡ;
      ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ c10 = (tmp >> (CPU_ಶಬ್ದ_ಬಿಟ್ಟುಗಳು/2)) & ಮುಖವಾಡ;

      tmp = a1 * b1;

      ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ r11 = tmp & ಮುಖವಾಡ;
      ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ c11 = (tmp >> (CPU_ಶಬ್ದ_ಬಿಟ್ಟುಗಳು/2)) & ಮುಖವಾಡ;

      ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ r1 = c00 + r01 + r10;
      ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ r2 = (r1 >> (CPU_ಶಬ್ದ_ಬಿಟ್ಟುಗಳು/2)) & ಮುಖವಾಡ;
      r1 &= ಮುಖವಾಡ;
      r2 += c01 + c10 + r11;
      ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ r3 = (r2 >> (CPU_ಶಬ್ದ_ಬಿಟ್ಟುಗಳು/2)) & ಮುಖವಾಡ;
      r2 &= ಮುಖವಾಡ;
      r3 += c11;
      r3 &= ಮುಖವಾಡ;

      ವಾಹಕ = (r3 << (CPU_ಶಬ್ದ_ಬಿಟ್ಟುಗಳು/2)) + r2;
      ಹಿಂತಿರುಗಿ (r1 << (CPU_ಶಬ್ದ_ಬಿಟ್ಟುಗಳು/2)) + r00;
}


ಶೂನ್ಯ vvp_send_vec8(vvp_net_ptr_t ptr, ಸ್ಥಿರಾಂಕ vvp_vector8_t&val)
{
      ಆಗ (vvp_net_t*cur = ptr.ptr()) {
	    vvp_net_ptr_t ಮುಂದಿನ_val = cur->ಪೋರ್ಟ್[ptr.ಪೋರ್ಟ್()];

	    ಆದರೆ (cur->fun)
		  cur->fun->recv_vec8(ptr, val);

	    ptr = ಮುಂದಿನ_val;
      }
}

ಶೂನ್ಯ vvp_send_real(vvp_net_ptr_t ptr, ದ್ವಿಗುಣ val, vvp_context_t context)
{
      ಆಗ (vvp_net_t*cur = ptr.ptr()) {
	    vvp_net_ptr_t ಮುಂದಿನ_val = cur->ಪೋರ್ಟ್[ptr.ಪೋರ್ಟ್()];

	    ಆದರೆ (cur->fun)
		  cur->fun->recv_real(ptr, val, context);

	    ptr = ಮುಂದಿನ_val;
      }
}

ಸ್ಥಿರಾಂಕ vvp_vector4_t vvp_vector4_t::nil;

ಶೂನ್ಯ vvp_vector4_t::copy_ಬಿಟ್ಟುಗಳು(ಸ್ಥಿರಾಂಕ vvp_vector4_t&that)
{

      ಆದರೆ (ಗಾತ್ರ_3 == that.ಗಾತ್ರ_3) {
	    ಆದರೆ (ಗಾತ್ರ_3 > ಬಿಟ್ಟುಗಳು_ಪ್ರತಿ_ಶಬ್ದ) {
		  ಚಿಹ್ನೆರಹಿತ ಪದಗಳು = (ಗಾತ್ರ_3+ಬಿಟ್ಟುಗಳು_ಪ್ರತಿ_ಶಬ್ದ-1) / ಬಿಟ್ಟುಗಳು_ಪ್ರತಿ_ಶಬ್ದ;
		  ಗಾಗಿ (ಚಿಹ್ನೆರಹಿತ idx = 0 ;  idx < ಪದಗಳು ;  idx += 1)
			abits_ptr_[idx] = that.abits_ptr_[idx];
		  ಗಾಗಿ (ಚಿಹ್ನೆರಹಿತ idx = 0 ;  idx < ಪದಗಳು ;  idx += 1)
			bbits_ptr_[idx] = that.bbits_ptr_[idx];
	    } ಇಲ್ಲದಿದ್ದರೆ {
		  abits_val_ = that.abits_val_;
		  bbits_val_ = that.bbits_val_;
	    }
	    ಹಿಂತಿರುಗಿ;
      }

	/* Now we know that the sizes of this and that are definitely
	   different. We can use that in code below. In any case, we
	   need to copy only the smaller of the sizes. */

	/* If source and destination are both short, then mask/copy
	   the bit values. */
      ಆದರೆ (ಗಾತ್ರ_3 <= ಬಿಟ್ಟುಗಳು_ಪ್ರತಿ_ಶಬ್ದ && that.ಗಾತ್ರ_3 <= ಬಿಟ್ಟುಗಳು_ಪ್ರತಿ_ಶಬ್ದ) {
	    ಚಿಹ್ನೆರಹಿತ ಬಿಟ್ಟುಗಳು_ಗಮ್ಯ_copy = (that.ಗಾತ್ರ_3 < ಗಾತ್ರ_3) ? that.ಗಾತ್ರ_3 : ಗಾತ್ರ_3;
	    ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ ಮುಖವಾಡ = ((ಬಿಟ್ಟುಗಳು_ಗಮ್ಯ_copy < ಬಿಟ್ಟುಗಳು_ಪ್ರತಿ_ಶಬ್ದ) ? (1UL << ಬಿಟ್ಟುಗಳು_ಗಮ್ಯ_copy) :
	                                                           (0UL)) - 1UL;
	    abits_val_ &= ~ಮುಖವಾಡ;
	    bbits_val_ &= ~ಮುಖವಾಡ;
	    abits_val_ |= that.abits_val_&ಮುಖವಾಡ;
	    bbits_val_ |= that.bbits_val_&ಮುಖವಾಡ;
	    ಹಿಂತಿರುಗಿ;
      }

	/* Now we know that either source or destination are long. If
	   the destination is short, then mask/copy from the low word
	   of the long source. */
      ಆದರೆ (ಗಾತ್ರ_3 <= ಬಿಟ್ಟುಗಳು_ಪ್ರತಿ_ಶಬ್ದ) {
	    abits_val_ = that.abits_ptr_[0];
	    bbits_val_ = that.bbits_ptr_[0];
	    ಆದರೆ (ಗಾತ್ರ_3 < ಬಿಟ್ಟುಗಳು_ಪ್ರತಿ_ಶಬ್ದ) {
		  ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ ಮುಖವಾಡ = (1UL << ಗಾತ್ರ_3) - 1UL;
		  abits_val_ &= ಮುಖವಾಡ;
		  bbits_val_ &= ಮುಖವಾಡ;
	    }
	    ಹಿಂತಿರುಗಿ;
      }

	/* Now we know that the destination must be long. If the
	   source is short, then mask/copy from its value. */
      ಆದರೆ (that.ಗಾತ್ರ_3 <= ಬಿಟ್ಟುಗಳು_ಪ್ರತಿ_ಶಬ್ದ) {
	    ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ ಮುಖವಾಡ;
	    ಆದರೆ (that.ಗಾತ್ರ_3 < ಬಿಟ್ಟುಗಳು_ಪ್ರತಿ_ಶಬ್ದ) {
		  ಮುಖವಾಡ = (1UL << that.ಗಾತ್ರ_3) - 1UL;
		  abits_ptr_[0] &= ~ಮುಖವಾಡ;
		  bbits_ptr_[0] &= ~ಮುಖವಾಡ;
	    } ಇಲ್ಲದಿದ್ದರೆ {
		  ಮುಖವಾಡ = -1UL;
	    }
	    abits_ptr_[0] |= that.abits_val_&ಮುಖವಾಡ;
	    bbits_ptr_[0] |= that.bbits_val_&ಮುಖವಾಡ;
	    ಹಿಂತಿರುಗಿ;
      }

	/* Finally, we know that source and destination are long. copy
	   words until we get to the last. */
      ಚಿಹ್ನೆರಹಿತ ಬಿಟ್ಟುಗಳು_ಗಮ್ಯ_copy = (that.ಗಾತ್ರ_3 < ಗಾತ್ರ_3) ? that.ಗಾತ್ರ_3 : ಗಾತ್ರ_3;
      ಚಿಹ್ನೆರಹಿತ ಶಬ್ದ = 0;
      ಆಗ (ಬಿಟ್ಟುಗಳು_ಗಮ್ಯ_copy >= ಬಿಟ್ಟುಗಳು_ಪ್ರತಿ_ಶಬ್ದ) {
	    abits_ptr_[ಶಬ್ದ] = that.abits_ptr_[ಶಬ್ದ];
	    bbits_ptr_[ಶಬ್ದ] = that.bbits_ptr_[ಶಬ್ದ];
	    ಬಿಟ್ಟುಗಳು_ಗಮ್ಯ_copy -= ಬಿಟ್ಟುಗಳು_ಪ್ರತಿ_ಶಬ್ದ;
	    ಶಬ್ದ += 1;
      }
      ಆದರೆ (ಬಿಟ್ಟುಗಳು_ಗಮ್ಯ_copy > 0) {
	    ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ ಮುಖವಾಡ = (1UL << ಬಿಟ್ಟುಗಳು_ಗಮ್ಯ_copy) - 1UL;
	    abits_ptr_[ಶಬ್ದ] &= ~ಮುಖವಾಡ;
	    bbits_ptr_[ಶಬ್ದ] &= ~ಮುಖವಾಡ;
	    abits_ptr_[ಶಬ್ದ] |= that.abits_ptr_[ಶಬ್ದ] & ಮುಖವಾಡ;
	    bbits_ptr_[ಶಬ್ದ] |= that.bbits_ptr_[ಶಬ್ದ] & ಮುಖವಾಡ;
      }
}

/*
 * This function should ONLY BE CALLED FROM vvp_vector4_t::copy_from_,
 * as it performs part of that functions tasks.
 */
ಶೂನ್ಯ vvp_vector4_t::copy_from_big_(ಸ್ಥಿರಾಂಕ vvp_vector4_t&that)
{
      ಚಿಹ್ನೆರಹಿತ ಪದಗಳು = (ಗಾತ್ರ_3+ಬಿಟ್ಟುಗಳು_ಪ್ರತಿ_ಶಬ್ದ-1) / ಬಿಟ್ಟುಗಳು_ಪ್ರತಿ_ಶಬ್ದ;
      abits_ptr_ = ಹೊಸ ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ[2*ಪದಗಳು];
      bbits_ptr_ = abits_ptr_ + ಪದಗಳು;

      ಗಾಗಿ (ಚಿಹ್ನೆರಹಿತ idx = 0 ;  idx < ಪದಗಳು ;  idx += 1)
	    abits_ptr_[idx] = that.abits_ptr_[idx];
      ಗಾಗಿ (ಚಿಹ್ನೆರಹಿತ idx = 0 ;  idx < ಪದಗಳು ;  idx += 1)
	    bbits_ptr_[idx] = that.bbits_ptr_[idx];
}

/*
 * The copy_inverted_from_ method is just like the copy_from_ method,
 * except that we combine that with an invert. This allows the ~ and
 * the assignment to be blended in many common cases.
 */
ಶೂನ್ಯ vvp_vector4_t::copy_inverted_from_(ಸ್ಥಿರಾಂಕ vvp_vector4_t&that)
{
      ಗಾತ್ರ_3 = that.ಗಾತ್ರ_3;
      ಆದರೆ (ಗಾತ್ರ_3 > ಬಿಟ್ಟುಗಳು_ಪ್ರತಿ_ಶಬ್ದ) {
	    ಚಿಹ್ನೆರಹಿತ ಪದಗಳು = (ಗಾತ್ರ_3+ಬಿಟ್ಟುಗಳು_ಪ್ರತಿ_ಶಬ್ದ-1) / ಬಿಟ್ಟುಗಳು_ಪ್ರತಿ_ಶಬ್ದ;
	    abits_ptr_ = ಹೊಸ ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ[2*ಪದಗಳು];
	    bbits_ptr_ = abits_ptr_ + ಪದಗಳು;

	    ಚಿಹ್ನೆರಹಿತ ಉಳಿದ = ಗಾತ್ರ_3;
	    ಚಿಹ್ನೆರಹಿತ idx = 0;
	    ಆಗ (ಉಳಿದ >= ಬಿಟ್ಟುಗಳು_ಪ್ರತಿ_ಶಬ್ದ) {
		  abits_ptr_[idx] = that.bbits_ptr_[idx] | ~that.abits_ptr_[idx];
		  idx += 1;
		  ಉಳಿದ -= ಬಿಟ್ಟುಗಳು_ಪ್ರತಿ_ಶಬ್ದ;
	    }
	    ಆದರೆ (ಉಳಿದ > 0) {
		  ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ ಮುಖವಾಡ = (1UL<<ಉಳಿದ) - 1UL;
		  abits_ptr_[idx] = ಮುಖವಾಡ & (that.bbits_ptr_[idx] | ~that.abits_ptr_[idx]);
	    }

	    ಗಾಗಿ (idx = 0 ;  idx < ಪದಗಳು ;  idx += 1)
		  bbits_ptr_[idx] = that.bbits_ptr_[idx];

      } ಇಲ್ಲದಿದ್ದರೆ {
	    ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ ಮುಖವಾಡ = (ಗಾತ್ರ_3<ಬಿಟ್ಟುಗಳು_ಪ್ರತಿ_ಶಬ್ದ)? (1UL<<ಗಾತ್ರ_3)-1UL : -1UL;
	    abits_val_ = ಮುಖವಾಡ & (that.bbits_val_ | ~that.abits_val_);
	    bbits_val_ = that.bbits_val_;
      }
}

/* Make sure to set size_ before calling this routine. */
ಶೂನ್ಯ vvp_vector4_t::allocate_ಪದಗಳು(ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ inita, ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ initb)
{
      ಆದರೆ (ಗಾತ್ರ_3 > ಬಿಟ್ಟುಗಳು_ಪ್ರತಿ_ಶಬ್ದ) {
	    ಚಿಹ್ನೆರಹಿತ cnt = (ಗಾತ್ರ_3 + ಬಿಟ್ಟುಗಳು_ಪ್ರತಿ_ಶಬ್ದ - 1) / ಬಿಟ್ಟುಗಳು_ಪ್ರತಿ_ಶಬ್ದ;
	    abits_ptr_ = ಹೊಸ ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ[2*cnt];
	    bbits_ptr_ = abits_ptr_ + cnt;
	    ಗಾಗಿ (ಚಿಹ್ನೆರಹಿತ idx = 0 ;  idx < cnt ;  idx += 1)
		  abits_ptr_[idx] = inita;
	    ಗಾಗಿ (ಚಿಹ್ನೆರಹಿತ idx = 0 ;  idx < cnt ;  idx += 1)
		  bbits_ptr_[idx] = initb;

      } ಇಲ್ಲದಿದ್ದರೆ {
	    abits_val_ = inita;
	    bbits_val_ = initb;
      }
}

vvp_vector4_t::vvp_vector4_t(ಚಿಹ್ನೆರಹಿತ ಗಾತ್ರ_4, ದ್ವಿಗುಣ val)
: ಗಾತ್ರ_3(ಗಾತ್ರ_4)
{
      ತಾರ್ಕಿಕ is_neg = ಸುಳ್ಳು;
      ದ್ವಿಗುಣ fraction;
      ಪೂರ್ಣಾಂಕ exponent;

	/* We return 'bx for a NaN or +/- infinity. */
      ಆದರೆ (val != val || (val && (val == 0.5*val)))  {
	    allocate_ಪದಗಳು(ಶಬ್ದ_X_ABITS, ಶಬ್ದ_X_BBITS);
	    ಹಿಂತಿರುಗಿ;
      }

	/* Convert to a positive result. */
      ಆದರೆ (val < 0.0) {
	    is_neg = ನಿಜ;
	    val = -val;
      }
      allocate_ಪದಗಳು(ಶಬ್ದ_0_ABITS, ಶಬ್ದ_0_BBITS);

	/* Get the exponent and fractional part of the number. */
      fraction = frexp(val, &exponent);

	/* If the value is small enough just use lround(). */
      ಆದರೆ (exponent < ಬಿಟ್ಟುಗಳು_ಪ್ರತಿ_ಶಬ್ದ-2) {
	    ಆದರೆ (is_neg) ಇದು->invert();  // Invert the bits if negative.
	    ದೀರ್ಘ sval = lround(val);
	    ಆದರೆ (is_neg) sval = -sval;
	      /* This requires that 0 and 1 have the same bbit value. */
	    ಆದರೆ (ಗಾತ್ರ_3 > ಬಿಟ್ಟುಗಳು_ಪ್ರತಿ_ಶಬ್ದ) {
		  abits_ptr_[0] = sval;
	    } ಇಲ್ಲದಿದ್ದರೆ {
		  abits_val_ = sval;
	    }
	    ಹಿಂತಿರುಗಿ;
      }

      ಚಿಹ್ನೆರಹಿತ nwords = (exponent-1) / ಬಿಟ್ಟುಗಳು_ಪ್ರತಿ_ಶಬ್ದ;
      ಚಿಹ್ನೆರಹಿತ my_ಪದಗಳು = (ಗಾತ್ರ_3 + ಬಿಟ್ಟುಗಳು_ಪ್ರತಿ_ಶಬ್ದ - 1) / ಬಿಟ್ಟುಗಳು_ಪ್ರತಿ_ಶಬ್ದ - 1;

      fraction = ldexp(fraction, (exponent-1) % ಬಿಟ್ಟುಗಳು_ಪ್ರತಿ_ಶಬ್ದ + 1);

	/* Skip any leading bits. */
      ಗಾಗಿ (ಪೂರ್ಣಾಂಕ idx = (ಚಿಹ್ನೆಯುಳ್ಳ) nwords; idx > (ಚಿಹ್ನೆಯುಳ್ಳ) my_ಪದಗಳು; idx -=1) {
	    ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ ಬಿಟ್ಟುಗಳು = (ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ) fraction;
	    fraction = fraction - (ದ್ವಿಗುಣ) ಬಿಟ್ಟುಗಳು;
	    fraction = ldexp(fraction, ಬಿಟ್ಟುಗಳು_ಪ್ರತಿ_ಶಬ್ದ);
      }

	/* Convert the remaining bits as appropriate. */
      ಆದರೆ (my_ಪದಗಳು == 0) {
		  ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ ಬಿಟ್ಟುಗಳು = (ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ) fraction;
		  abits_val_ = ಬಿಟ್ಟುಗಳು;
		  fraction = fraction - (ದ್ವಿಗುಣ) ಬಿಟ್ಟುಗಳು;
		    /* Round any fractional part up. */
		  ಆದರೆ (fraction >= 0.5) *ಇದು += (int64_t) 1;
      } ಇಲ್ಲದಿದ್ದರೆ {
	    ಆದರೆ (nwords < my_ಪದಗಳು) my_ಪದಗಳು = nwords;
	    ಗಾಗಿ (ಪೂರ್ಣಾಂಕ idx = (ಚಿಹ್ನೆಯುಳ್ಳ)my_ಪದಗಳು; idx >= 0; idx -= 1) {
		  ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ ಬಿಟ್ಟುಗಳು = (ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ) fraction;
		  abits_ptr_[idx] = ಬಿಟ್ಟುಗಳು;
		  fraction = fraction - (ದ್ವಿಗುಣ) ಬಿಟ್ಟುಗಳು;
		  fraction = ldexp(fraction, ಬಿಟ್ಟುಗಳು_ಪ್ರತಿ_ಶಬ್ದ);
	    }
	      /* Round any fractional part up. */
	    ಆದರೆ (fraction >= ldexp(0.5, ಬಿಟ್ಟುಗಳು_ಪ್ರತಿ_ಶಬ್ದ)) *ಇದು += (int64_t) 1;
       }

	/* Convert to a negative number if needed. */
      ಆದರೆ (is_neg) {
	    ಇದು->invert();
	    *ಇದು += (int64_t) 1;
      }
}

vvp_vector4_t::vvp_vector4_t(ಸ್ಥಿರಾಂಕ vvp_vector4_t&that,
			    ಚಿಹ್ನೆರಹಿತ adr, ಚಿಹ್ನೆರಹಿತ wid)
{
	// Set up and initialize the destination.
      ಗಾತ್ರ_3 = wid;
      allocate_ಪದಗಳು(ಶಬ್ದ_X_ABITS, ಶಬ್ದ_X_BBITS);

	// Special case: selecting from far beyond the source vector,
	// to the result is all X bits. We're done.
      ಆದರೆ (adr >= that.ಗಾತ್ರ_3)
	    ಹಿಂತಿರುಗಿ;

	// Special case: The source is not quite big enough to supply
	// all bits, so get the bits that we can. The remainder will
	// be left at BIT4_X.
      ಆದರೆ ((adr + wid) > that.ಗಾತ್ರ_3) {
	    ಚಿಹ್ನೆರಹಿತ use_wid = that.ಗಾತ್ರ_3 - adr;
	    ಗಾಗಿ (ಚಿಹ್ನೆರಹಿತ idx = 0 ; idx < use_wid ; idx += 1)
		  ಗಣ_ಬಿಟ್(idx, that.value(adr+idx));

	    ಹಿಂತಿರುಗಿ;
      }

	// At the point, we know that the source part is entirely
	// contained in the source vector.
	// assert((adr + wid) <= that.size_);

      ಆದರೆ (wid > ಬಿಟ್ಟುಗಳು_ಪ್ರತಿ_ಶಬ್ದ) {
	      /* In this case, the subvector and the source vector are
		 long. Do the transfer reasonably efficiently. */
	    ಚಿಹ್ನೆರಹಿತ ptr = adr / ಬಿಟ್ಟುಗಳು_ಪ್ರತಿ_ಶಬ್ದ;
	    ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ off = adr % ಬಿಟ್ಟುಗಳು_ಪ್ರತಿ_ಶಬ್ದ;
	    ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ noff = ಬಿಟ್ಟುಗಳು_ಪ್ರತಿ_ಶಬ್ದ - off;
	    ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ lmask = (1UL << off) - 1UL;
	    ಚಿಹ್ನೆರಹಿತ trans = 0;
	    ಚಿಹ್ನೆರಹಿತ dst = 0;
	    ಆಗ (trans < wid) {
		    // The low bits of the result.
		  abits_ptr_[dst] = (that.abits_ptr_[ptr] & ~lmask) >> off;
		  bbits_ptr_[dst] = (that.bbits_ptr_[ptr] & ~lmask) >> off;
		  trans += noff;

		  ಆದರೆ (trans >= wid)
			ನಿಲ್ಲಿಸು;

		  ptr += 1;

		    // The high bits of the result. Skip this if the
		    // source and destination are perfectly aligned.
		  ಆದರೆ (noff != ಬಿಟ್ಟುಗಳು_ಪ್ರತಿ_ಶಬ್ದ) {
			abits_ptr_[dst] |= (that.abits_ptr_[ptr]&lmask) << noff;
			bbits_ptr_[dst] |= (that.bbits_ptr_[ptr]&lmask) << noff;
			trans += off;
		  }

		  dst += 1;
	    }

      } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (that.ಗಾತ್ರ_3 > ಬಿಟ್ಟುಗಳು_ಪ್ರತಿ_ಶಬ್ದ) {
	      /* In this case, the subvector fits in a single word,
		 but the source is large. */
	    ಚಿಹ್ನೆರಹಿತ ptr = adr / ಬಿಟ್ಟುಗಳು_ಪ್ರತಿ_ಶಬ್ದ;
	    ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ off = adr % ಬಿಟ್ಟುಗಳು_ಪ್ರತಿ_ಶಬ್ದ;
	    ಚಿಹ್ನೆರಹಿತ trans = ಬಿಟ್ಟುಗಳು_ಪ್ರತಿ_ಶಬ್ದ - off;
	    ಆದರೆ (trans > wid)
		  trans = wid;

	    ಆದರೆ (trans == ಬಿಟ್ಟುಗಳು_ಪ್ರತಿ_ಶಬ್ದ) {
		    // Very special case: Copy exactly 1 perfectly
		    // aligned word.
		  abits_val_ = that.abits_ptr_[ptr];
		  bbits_val_ = that.bbits_ptr_[ptr];

	    } ಇಲ್ಲದಿದ್ದರೆ {
		    // lmask is the low bits of the destination,
		    // masked into the source.
		  ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ lmask = (1UL<<trans) - 1UL;
		  lmask <<= off;

		    // The low bits of the result.
		  abits_val_ = (that.abits_ptr_[ptr] & lmask) >> off;
		  bbits_val_ = (that.bbits_ptr_[ptr] & lmask) >> off;

		  ಆದರೆ (trans < wid) {
			  // If there are more bits, then get them
			  // from the bottom of the next word of the
			  // source.
			ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ hmask = (1UL << (wid-trans)) - 1UL;

			  // The high bits of the result.
			abits_val_ |= (that.abits_ptr_[ptr+1]&hmask) << trans;
			bbits_val_ |= (that.bbits_ptr_[ptr+1]&hmask) << trans;
		  }
	    }

      } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (ಗಾತ್ರ_3 == ಬಿಟ್ಟುಗಳು_ಪ್ರತಿ_ಶಬ್ದ) {
	      /* We know that source and destination are short. If the
		 destination is a full word, then we know the copy is
		 aligned and complete. */
	    abits_val_ = that.abits_val_;
	    bbits_val_ = that.bbits_val_;

      } ಇಲ್ಲದಿದ್ದರೆ {
	      /* Finally, the source and destination vectors are both
		 short, so there is a single mask/shift/copy. */
	    ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ ಮುಖವಾಡ = (1UL << ಗಾತ್ರ_3) - 1UL;
	    ಮುಖವಾಡ <<= adr;

	    abits_val_ = (that.abits_val_ & ಮುಖವಾಡ) >> adr;
	    bbits_val_ = (that.bbits_val_ & ಮುಖವಾಡ) >> adr;
      }

}

/*
 * Change the size of the vvp_vector4_t vector to the new size. Copy
 * the old values, as many as well fit, into the new vector.
 */
ಶೂನ್ಯ vvp_vector4_t::resize(ಚಿಹ್ನೆರಹಿತ newsize, vvp_bit4_t pad_ಬಿಟ್)
{
      ಆದರೆ (ಗಾತ್ರ_3 == newsize)
	    ಹಿಂತಿರುಗಿ;

      ಚಿಹ್ನೆರಹಿತ cnt = (ಗಾತ್ರ_3 + ಬಿಟ್ಟುಗಳು_ಪ್ರತಿ_ಶಬ್ದ - 1) / ಬಿಟ್ಟುಗಳು_ಪ್ರತಿ_ಶಬ್ದ;
      ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ ಶಬ್ದ_pad_abits = 0;
      ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ ಶಬ್ದ_pad_bbits = 0;
      ಆಯ್ಕೆ (pad_ಬಿಟ್) {
	  ಸ್ಥಿತಿ BIT4_0:
	    ಶಬ್ದ_pad_abits = ಶಬ್ದ_0_ABITS;
	    ಶಬ್ದ_pad_bbits = ಶಬ್ದ_0_BBITS;
	    ನಿಲ್ಲಿಸು;
	  ಸ್ಥಿತಿ BIT4_1:
	    ಶಬ್ದ_pad_abits = ಶಬ್ದ_1_ABITS;
	    ಶಬ್ದ_pad_bbits = ಶಬ್ದ_1_BBITS;
	    ನಿಲ್ಲಿಸು;
	  ಸ್ಥಿತಿ BIT4_X:
	    ಶಬ್ದ_pad_abits = ಶಬ್ದ_X_ABITS;
	    ಶಬ್ದ_pad_bbits = ಶಬ್ದ_X_BBITS;
	    ನಿಲ್ಲಿಸು;
	  ಸ್ಥಿತಿ BIT4_Z:
	    ಶಬ್ದ_pad_abits = ಶಬ್ದ_Z_ABITS;
	    ಶಬ್ದ_pad_bbits = ಶಬ್ದ_Z_BBITS;
	    ನಿಲ್ಲಿಸು;
      }

      ಆದರೆ (newsize > ಬಿಟ್ಟುಗಳು_ಪ್ರತಿ_ಶಬ್ದ) {
	    ಚಿಹ್ನೆರಹಿತ newcnt = (newsize + ಬಿಟ್ಟುಗಳು_ಪ್ರತಿ_ಶಬ್ದ - 1) / ಬಿಟ್ಟುಗಳು_ಪ್ರತಿ_ಶಬ್ದ;
	    ಆದರೆ (newcnt == cnt) {
		    // If the word count doesn't change, then there is
		    // no need for re-allocation so we are done now.
		  ಆದರೆ (newsize > ಗಾತ್ರ_3) {
			ಆದರೆ (ಚಿಹ್ನೆರಹಿತ fill = ಗಾತ್ರ_3 % ಬಿಟ್ಟುಗಳು_ಪ್ರತಿ_ಶಬ್ದ) {
			      abits_ptr_[cnt-1] &= ~((-1UL) << fill);
			      bbits_ptr_[cnt-1] &= ~((-1UL) << fill);
			      abits_ptr_[cnt-1] |= ಶಬ್ದ_pad_abits << fill;
			      bbits_ptr_[cnt-1] |= ಶಬ್ದ_pad_bbits << fill;
			}
		  }
		  ಗಾತ್ರ_3 = newsize;
		  ಹಿಂತಿರುಗಿ;
	    }

	    ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ*newbits = ಹೊಸ ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ[2*newcnt];

	    ಆದರೆ (cnt > 1) {
		  ಚಿಹ್ನೆರಹಿತ trans = cnt;
		  ಆದರೆ (trans > newcnt)
			trans = newcnt;

		  ಗಾಗಿ (ಚಿಹ್ನೆರಹಿತ idx = 0 ;  idx < trans ;  idx += 1)
			newbits[idx] = abits_ptr_[idx];
		  ಗಾಗಿ (ಚಿಹ್ನೆರಹಿತ idx = 0 ;  idx < trans ;  idx += 1)
			newbits[newcnt+idx] = bbits_ptr_[idx];

		  ಅಳಿಸು[]abits_ptr_;

	    } ಇಲ್ಲದಿದ್ದರೆ {
		  newbits[0] = abits_val_;
		  newbits[newcnt] = bbits_val_;
	    }

	    ಆದರೆ (newsize > ಗಾತ್ರ_3) {
		  ಆದರೆ (ಚಿಹ್ನೆರಹಿತ fill = ಗಾತ್ರ_3 % ಬಿಟ್ಟುಗಳು_ಪ್ರತಿ_ಶಬ್ದ) {
			newbits[cnt-1] &= ~((-1UL) << fill);
			newbits[cnt-1] |= ಶಬ್ದ_pad_abits << fill;
			newbits[newcnt+cnt-1] &= ~((-1UL) << fill);
			newbits[newcnt+cnt-1] |= ಶಬ್ದ_pad_bbits << fill;
		  }
		  ಗಾಗಿ (ಚಿಹ್ನೆರಹಿತ idx = cnt ;  idx < newcnt ;  idx += 1)
			newbits[idx] = ಶಬ್ದ_pad_abits;
		  ಗಾಗಿ (ಚಿಹ್ನೆರಹಿತ idx = cnt ;  idx < newcnt ;  idx += 1)
			newbits[newcnt+idx] = ಶಬ್ದ_pad_bbits;
	    }

	    ಗಾತ್ರ_3 = newsize;
	    abits_ptr_ = newbits;
	    bbits_ptr_ = newbits + newcnt;

      } ಇಲ್ಲದಿದ್ದರೆ {
	    ಆದರೆ (cnt > 1) {
		  ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ newvala = abits_ptr_[0];
		  ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ newvalb = bbits_ptr_[0];
		  ಅಳಿಸು[]abits_ptr_;
		  abits_val_ = newvala;
		  bbits_val_ = newvalb;
	    }

	    ಆದರೆ (newsize > ಗಾತ್ರ_3) {
		  abits_val_ &= ~((-1UL) << ಗಾತ್ರ_3);
		  bbits_val_ &= ~((-1UL) << ಗಾತ್ರ_3);
		  abits_val_ |= ಶಬ್ದ_pad_abits << ಗಾತ್ರ_3;
		  bbits_val_ |= ಶಬ್ದ_pad_bbits << ಗಾತ್ರ_3;
	    }

	    ಗಾತ್ರ_3 = newsize;
      }
}


ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ* vvp_vector4_t::subarray(ಚಿಹ್ನೆರಹಿತ adr, ಚಿಹ್ನೆರಹಿತ wid, ತಾರ್ಕಿಕ xz_ಗಮ್ಯ_0) ಸ್ಥಿರಾಂಕ
{
      ಸ್ಥಿರಾಂಕ ಚಿಹ್ನೆರಹಿತ BIT2_ಪ್ರತಿ_ಶಬ್ದ = 8*ಗಾತ್ರ(ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ);
      ಚಿಹ್ನೆರಹಿತ awid = (wid + BIT2_ಪ್ರತಿ_ಶಬ್ದ - 1) / (BIT2_ಪ್ರತಿ_ಶಬ್ದ);
      ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ*val = ಹೊಸ ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ[awid];

      ಗಾಗಿ (ಚಿಹ್ನೆರಹಿತ idx = 0 ;  idx < awid ;  idx += 1)
	    val[idx] = 0;

      ಆದರೆ (ಗಾತ್ರ_3 <= ಬಿಟ್ಟುಗಳು_ಪ್ರತಿ_ಶಬ್ದ) {
	      /* Handle the special case that the array is small. The
		 entire value of the vector4 is within the xbits_val_
		 so we know that the result is a single word, the
		 source is a single word, and we just have to loop
		 through that word. */
	    ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ atmp = abits_val_ >> adr;
	    ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ btmp = bbits_val_ >> adr;
	    ಆದರೆ (wid < BIT2_ಪ್ರತಿ_ಶಬ್ದ) {
		  atmp &= (1UL << wid) - 1;
		  btmp &= (1UL << wid) - 1;
	    }
	    ಆದರೆ (btmp) {
		  ಆದರೆ (xz_ಗಮ್ಯ_0)
			atmp &= ~btmp;
		  ಇಲ್ಲದಿದ್ದರೆ
			ಹೋಗು x_ಹೊರಹೋಗು;
	    }

	    val[0] = atmp;

      } ಇಲ್ಲದಿದ್ದರೆ {

	    ಚಿಹ್ನೆರಹಿತ val_ptr = 0;
	    ಚಿಹ್ನೆರಹಿತ val_off = 0;

	      /* Get the first word we are scanning. We may in fact be
		 somewhere in the middle of that word. */
	    ಆಗ (wid > 0) {
		  ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ atmp = abits_ptr_[adr/ಬಿಟ್ಟುಗಳು_ಪ್ರತಿ_ಶಬ್ದ];
		  ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ btmp = bbits_ptr_[adr/ಬಿಟ್ಟುಗಳು_ಪ್ರತಿ_ಶಬ್ದ];
		  ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ off = adr%ಬಿಟ್ಟುಗಳು_ಪ್ರತಿ_ಶಬ್ದ;
		  atmp >>= off;
		  btmp >>= off;

		  ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ trans = ಬಿಟ್ಟುಗಳು_ಪ್ರತಿ_ಶಬ್ದ - off;
		  ಆದರೆ (trans > (BIT2_ಪ್ರತಿ_ಶಬ್ದ - val_off))
			trans = BIT2_ಪ್ರತಿ_ಶಬ್ದ - val_off;
		  ಆದರೆ (wid < trans)
			trans = wid;
		  ಆದರೆ (trans < BIT2_ಪ್ರತಿ_ಶಬ್ದ) {
			atmp &= (1UL << trans) - 1;
			btmp &= (1UL << trans) - 1;
		  }
		  ಆದರೆ (btmp) {
			ಆದರೆ (xz_ಗಮ್ಯ_0)
			      atmp &= ~btmp;
			ಇಲ್ಲದಿದ್ದರೆ
			      ಹೋಗು x_ಹೊರಹೋಗು;
		  }

		  val[val_ptr] |= atmp << val_off;
		  adr += trans;
		  wid -= trans;
		  val_off += trans;
		  ಆದರೆ (val_off == BIT2_ಪ್ರತಿ_ಶಬ್ದ) {
			val_ptr += 1;
			val_off = 0;
		  }
	    }
      }

      ಹಿಂತಿರುಗಿ val;

 x_ಹೊರಹೋಗು:
      ಅಳಿಸು[]val;
      ಹಿಂತಿರುಗಿ 0;
}

ಶೂನ್ಯ vvp_vector4_t::setarray(ಚಿಹ್ನೆರಹಿತ adr, ಚಿಹ್ನೆರಹಿತ wid, ಸ್ಥಿರಾಂಕ ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ*val)
{
      ಪ್ರತಿಜ್ಞಾ(adr+wid <= ಗಾತ್ರ_3);

      ಸ್ಥಿರಾಂಕ ಚಿಹ್ನೆರಹಿತ BIT2_ಪ್ರತಿ_ಶಬ್ದ = 8*ಗಾತ್ರ(ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ);

      ಆದರೆ (ಗಾತ್ರ_3 <= ಬಿಟ್ಟುಗಳು_ಪ್ರತಿ_ಶಬ್ದ) {
	      // We know here that both the source and the target are
	      // within a single word. Write the bits into the
	      // abits_val_ directly.

	    ಪ್ರತಿಜ್ಞಾ(BIT2_ಪ್ರತಿ_ಶಬ್ದ <= ಬಿಟ್ಟುಗಳು_ಪ್ರತಿ_ಶಬ್ದ);
	    ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ lmask = (1UL << adr) - 1UL;
	    ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ hmask = ((adr+wid) < ಬಿಟ್ಟುಗಳು_ಪ್ರತಿ_ಶಬ್ದ)
		  ? -1UL << (adr+wid)
		  : 0;
	    ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ ಮುಖವಾಡ = ~(hmask | lmask);

	    abits_val_ &= ~ಮುಖವಾಡ;
	    bbits_val_ &= ~ಮುಖವಾಡ;

	    abits_val_ |= ಮುಖವಾಡ & (val[0] << adr);

      } ಇಲ್ಲದಿದ್ದರೆ {
	      // The general case, there are multiple words of
	      // destination, and possibly multiple words of source
	      // data. Shift and mask as we go.
	    ಚಿಹ್ನೆರಹಿತ off = adr % ಬಿಟ್ಟುಗಳು_ಪ್ರತಿ_ಶಬ್ದ;
	    ಚಿಹ್ನೆರಹಿತ ptr = adr / ಬಿಟ್ಟುಗಳು_ಪ್ರತಿ_ಶಬ್ದ;
	    ಚಿಹ್ನೆರಹಿತ val_off = 0;
	    ಚಿಹ್ನೆರಹಿತ val_ptr = 0;
	    ಆಗ (wid > 0) {
		  ಚಿಹ್ನೆರಹಿತ trans = wid;
		  ಆದರೆ (trans > (BIT2_ಪ್ರತಿ_ಶಬ್ದ-val_off))
			trans = BIT2_ಪ್ರತಿ_ಶಬ್ದ-val_off;
		  ಆದರೆ (trans > (ಬಿಟ್ಟುಗಳು_ಪ್ರತಿ_ಶಬ್ದ-off))
			trans = ಬಿಟ್ಟುಗಳು_ಪ್ರತಿ_ಶಬ್ದ-off;

		  ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ lmask = (1UL << off) - 1UL;
		  ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ hmask = ((off+trans) < ಬಿಟ್ಟುಗಳು_ಪ್ರತಿ_ಶಬ್ದ)
			? -1UL << (off+trans)
			: 0;
		  ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ ಮುಖವಾಡ = ~(hmask | lmask);

		  abits_ptr_[ptr] &= ~ಮುಖವಾಡ;
		  bbits_ptr_[ptr] &= ~ಮುಖವಾಡ;
		  ಆದರೆ (val_off >= off)
			abits_ptr_[ptr] |= ಮುಖವಾಡ & (val[val_ptr] >> (val_off-off));
		  ಇಲ್ಲದಿದ್ದರೆ
			abits_ptr_[ptr] |= ಮುಖವಾಡ & (val[val_ptr] << (off-val_off));

		  wid -= trans;
		  val_off += trans;
		  ಆದರೆ (val_off == BIT2_ಪ್ರತಿ_ಶಬ್ದ) {
			val_off = 0;
			val_ptr += 1;
		  }
		  off += trans;
		  ಆದರೆ (off == ಬಿಟ್ಟುಗಳು_ಪ್ರತಿ_ಶಬ್ದ) {
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
ತಾರ್ಕಿಕ vvp_vector4_t::ಗಣ_vec(ಚಿಹ್ನೆರಹಿತ adr, ಸ್ಥಿರಾಂಕ vvp_vector4_t&that)
{
      ಪ್ರತಿಜ್ಞಾ(adr+that.ಗಾತ್ರ_3  <= ಗಾತ್ರ_3);
      ತಾರ್ಕಿಕ diff_flag = ಸುಳ್ಳು;

      ಆದರೆ (ಗಾತ್ರ_3 <= ಬಿಟ್ಟುಗಳು_ಪ್ರತಿ_ಶಬ್ದ) {

	      /* The destination vector (me!) is within a bits_val_
		 word, so the subvector is certainly within a
		 bits_val_ word. Therefore, the entire operation is a
		 matter of writing the bits of that into the addressed
		 bits of this. The mask below is calculated to be 1
		 for all the bits that are to come from that. Do the
		 job by some shifting, masking and OR. */

	    ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ lmask = (1UL << adr) - 1;
	    ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ hmask;
	    ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ hshift = adr+that.ಗಾತ್ರ_3;
	    ಆದರೆ (hshift >= ಬಿಟ್ಟುಗಳು_ಪ್ರತಿ_ಶಬ್ದ)
		  hmask = -1UL;
	    ಇಲ್ಲದಿದ್ದರೆ
		  hmask = (1UL << (adr+that.ಗಾತ್ರ_3)) - 1;
	    ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ ಮುಖವಾಡ = hmask & ~lmask;

	    ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ tmp = (that.abits_val_<<adr)&ಮುಖವಾಡ;
	    ಆದರೆ ((abits_val_&ಮುಖವಾಡ) != tmp) {
		  diff_flag = ನಿಜ;
		  abits_val_ = (abits_val_ & ~ಮುಖವಾಡ) | tmp;
	    }
	    tmp = (that.bbits_val_<<adr) & ಮುಖವಾಡ;
	    ಆದರೆ ((bbits_val_&ಮುಖವಾಡ) != tmp) {
		  diff_flag = ನಿಜ;
		  bbits_val_ = (bbits_val_ & ~ಮುಖವಾಡ) | tmp;
	    }

      } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (that.ಗಾತ್ರ_3 <= ಬಿಟ್ಟುಗಳು_ಪ್ರತಿ_ಶಬ್ದ) {

	      /* This vector is more than a word, but that vector is
		 still small. Write into the destination, possibly
		 spanning two destination words, depending on whether
		 the source vector spans a word transition. */
	    ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ dptr = adr / ಬಿಟ್ಟುಗಳು_ಪ್ರತಿ_ಶಬ್ದ;
	    ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ doff = adr % ಬಿಟ್ಟುಗಳು_ಪ್ರತಿ_ಶಬ್ದ;

	    ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ lmask = (1UL << doff) - 1;
	    ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ hshift = doff+that.ಗಾತ್ರ_3;
	    ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ hmask;
	    ಆದರೆ (hshift >= ಬಿಟ್ಟುಗಳು_ಪ್ರತಿ_ಶಬ್ದ)
		  hmask = -1UL;
	    ಇಲ್ಲದಿದ್ದರೆ
		  hmask = (1UL << hshift) - 1UL;

	    ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ ಮುಖವಾಡ = hmask & ~lmask;
	    ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ tmp;

	    tmp = (that.abits_val_ << doff) & ಮುಖವಾಡ;
	    ಆದರೆ ((abits_ptr_[dptr] & ಮುಖವಾಡ) != tmp) {
		  diff_flag = ನಿಜ;
		  abits_ptr_[dptr] = (abits_ptr_[dptr] & ~ಮುಖವಾಡ) | tmp;
	    }
	    tmp = (that.bbits_val_ << doff) & ಮುಖವಾಡ;
	    ಆದರೆ ((bbits_ptr_[dptr] & ಮುಖವಾಡ) != tmp) {
		  diff_flag = ನಿಜ;
		  bbits_ptr_[dptr] = (bbits_ptr_[dptr] & ~ಮುಖವಾಡ) | tmp;
	    }

	    ಆದರೆ ((doff + that.ಗಾತ್ರ_3) > ಬಿಟ್ಟುಗಳು_ಪ್ರತಿ_ಶಬ್ದ) {
		  ಚಿಹ್ನೆರಹಿತ ಬಾಲ = doff + that.ಗಾತ್ರ_3 - ಬಿಟ್ಟುಗಳು_ಪ್ರತಿ_ಶಬ್ದ;
		  ಮುಖವಾಡ = (1UL << ಬಾಲ) - 1;

		  dptr += 1;
		  tmp = (that.abits_val_ >> (that.ಗಾತ್ರ_3-ಬಾಲ)) & ಮುಖವಾಡ;
		  ಆದರೆ ((abits_ptr_[dptr] & ಮುಖವಾಡ) != tmp) {
			diff_flag = ನಿಜ;
			abits_ptr_[dptr] = (abits_ptr_[dptr] & ~ಮುಖವಾಡ) | tmp;
		  }
		  tmp = (that.bbits_val_ >> (that.ಗಾತ್ರ_3-ಬಾಲ)) & ಮುಖವಾಡ;
		  ಆದರೆ ((bbits_ptr_[dptr] & ಮುಖವಾಡ) != tmp) {
			diff_flag = ನಿಜ;
			bbits_ptr_[dptr] = (bbits_ptr_[dptr] & ~ಮುಖವಾಡ) | tmp;
		  }
	    }

      } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (adr%ಬಿಟ್ಟುಗಳು_ಪ್ರತಿ_ಶಬ್ದ == 0) {

	      /* In this case, both vectors are long, but the
		 destination is neatly aligned. That means all but the
		 last word can be simply copied with no masking. */

	    ಚಿಹ್ನೆರಹಿತ remain = that.ಗಾತ್ರ_3;
	    ಚಿಹ್ನೆರಹಿತ sptr = 0;
	    ಚಿಹ್ನೆರಹಿತ dptr = adr / ಬಿಟ್ಟುಗಳು_ಪ್ರತಿ_ಶಬ್ದ;
	    ಆಗ (remain >= ಬಿಟ್ಟುಗಳು_ಪ್ರತಿ_ಶಬ್ದ) {
		  ಆದರೆ (abits_ptr_[dptr] != that.abits_ptr_[sptr]) {
			diff_flag = ನಿಜ;
			abits_ptr_[dptr] = that.abits_ptr_[sptr];
		  }
		  ಆದರೆ (bbits_ptr_[dptr] != that.bbits_ptr_[sptr]) {
			diff_flag = ನಿಜ;
			bbits_ptr_[dptr] = that.bbits_ptr_[sptr];
		  }
		  dptr += 1;
		  sptr += 1;
		  remain -= ಬಿಟ್ಟುಗಳು_ಪ್ರತಿ_ಶಬ್ದ;
	    }

	    ಆದರೆ (remain > 0) {
		  ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ ಮುಖವಾಡ = (1UL << remain) - 1;
		  ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ tmp;

		  tmp = that.abits_ptr_[sptr] & ಮುಖವಾಡ;
		  ಆದರೆ ((abits_ptr_[dptr] & ಮುಖವಾಡ) != tmp) {
			diff_flag = ನಿಜ;
			abits_ptr_[dptr] = (abits_ptr_[dptr] & ~ಮುಖವಾಡ) | tmp;
		  }
		  tmp = that.bbits_ptr_[sptr] & ಮುಖವಾಡ;
		  ಆದರೆ ((bbits_ptr_[dptr] & ಮುಖವಾಡ) != tmp) {
			diff_flag = ನಿಜ;
			bbits_ptr_[dptr] = (bbits_ptr_[dptr] & ~ಮುಖವಾಡ) | tmp;
		  }
	    }

      } ಇಲ್ಲದಿದ್ದರೆ {

	      /* We know that there are two long vectors, and we know
		 that the destination is definitely NOT aligned. */

	    ಚಿಹ್ನೆರಹಿತ remain = that.ಗಾತ್ರ_3;
	    ಚಿಹ್ನೆರಹಿತ sptr = 0;
	    ಚಿಹ್ನೆರಹಿತ dptr = adr / ಬಿಟ್ಟುಗಳು_ಪ್ರತಿ_ಶಬ್ದ;
	    ಚಿಹ್ನೆರಹಿತ doff = adr % ಬಿಟ್ಟುಗಳು_ಪ್ರತಿ_ಶಬ್ದ;
	    ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ lmask = (1UL << doff) - 1;
	    ಚಿಹ್ನೆರಹಿತ ndoff = ಬಿಟ್ಟುಗಳು_ಪ್ರತಿ_ಶಬ್ದ - doff;
	    ಆಗ (remain >= ಬಿಟ್ಟುಗಳು_ಪ್ರತಿ_ಶಬ್ದ) {
		  ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ tmp;

		  tmp = (that.abits_ptr_[sptr] << doff) & ~lmask;
		  ಆದರೆ ((abits_ptr_[dptr] & ~lmask) != tmp) {
			diff_flag = ನಿಜ;
			abits_ptr_[dptr] = (abits_ptr_[dptr] & lmask) | tmp;
		  }
		  tmp = (that.bbits_ptr_[sptr] << doff) & ~lmask;
		  ಆದರೆ ((bbits_ptr_[dptr] & ~lmask) != tmp) {
			diff_flag = ನಿಜ;
			bbits_ptr_[dptr] = (bbits_ptr_[dptr] & lmask) | tmp;
		  }
		  dptr += 1;

		  tmp = (that.abits_ptr_[sptr] >> ndoff) & lmask;
		  ಆದರೆ ((abits_ptr_[dptr] & lmask) != tmp) {
			diff_flag = ನಿಜ;
			abits_ptr_[dptr] = (abits_ptr_[dptr] & ~lmask) | tmp;
		  }
		  tmp = (that.bbits_ptr_[sptr] >> ndoff) & lmask;
		  ಆದರೆ ((bbits_ptr_[dptr] & lmask) != tmp) {
			diff_flag = ನಿಜ;
			bbits_ptr_[dptr] = (bbits_ptr_[dptr] & ~lmask) | tmp;
		  }

		  remain -= ಬಿಟ್ಟುಗಳು_ಪ್ರತಿ_ಶಬ್ದ;
		  sptr += 1;
	    }

	    ಆದರೆ (remain > 0) {
		  ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ hshift = doff+remain;
		  ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ hmask;
		  ಆದರೆ (hshift >= ಬಿಟ್ಟುಗಳು_ಪ್ರತಿ_ಶಬ್ದ)
			hmask = -1UL;
		  ಇಲ್ಲದಿದ್ದರೆ
			hmask = (1UL << (doff+remain)) - 1;

		  ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ ಮುಖವಾಡ = hmask & ~lmask;
		  ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ tmp;

		  tmp = (that.abits_ptr_[sptr] << doff) & ಮುಖವಾಡ;
		  ಆದರೆ ((abits_ptr_[dptr] & ಮುಖವಾಡ) != tmp) {
			diff_flag = ನಿಜ;
			abits_ptr_[dptr] = (abits_ptr_[dptr] & ~ಮುಖವಾಡ) | tmp;
		  }
		  tmp = (that.bbits_ptr_[sptr] << doff) & ಮುಖವಾಡ;
		  ಆದರೆ ((bbits_ptr_[dptr] & ಮುಖವಾಡ) != tmp) {
			diff_flag = ನಿಜ;
			bbits_ptr_[dptr] = (bbits_ptr_[dptr] & ~ಮುಖವಾಡ) | tmp;
		  }

		  ಆದರೆ ((doff + remain) > ಬಿಟ್ಟುಗಳು_ಪ್ರತಿ_ಶಬ್ದ) {
			ಚಿಹ್ನೆರಹಿತ ಬಾಲ = doff + remain - ಬಿಟ್ಟುಗಳು_ಪ್ರತಿ_ಶಬ್ದ;
			ಆದರೆ (ಬಾಲ >= ಬಿಟ್ಟುಗಳು_ಪ್ರತಿ_ಶಬ್ದ)
			      ಮುಖವಾಡ = -1UL;
			ಇಲ್ಲದಿದ್ದರೆ
			      ಮುಖವಾಡ = (1UL << ಬಾಲ) - 1;

			dptr += 1;

			tmp = (that.abits_ptr_[sptr] >> (remain-ಬಾಲ))&ಮುಖವಾಡ;
			ಆದರೆ ((abits_ptr_[dptr] & ಮುಖವಾಡ) != tmp) {
			      diff_flag = ನಿಜ;
			      abits_ptr_[dptr] = (abits_ptr_[dptr] & ~ಮುಖವಾಡ) | tmp;
			}
			tmp = (that.bbits_ptr_[sptr] >> (remain-ಬಾಲ))&ಮುಖವಾಡ;
			ಆದರೆ ((bbits_ptr_[dptr] & ಮುಖವಾಡ) != tmp) {
			      diff_flag = ನಿಜ;
			      bbits_ptr_[dptr] = (bbits_ptr_[dptr] & ~ಮುಖವಾಡ) | tmp;
			}
		  }
	    }
      }

      ಹಿಂತಿರುಗಿ diff_flag;
}

/*
 * Add that vector to this vector. Do it in the Verilog way, which
 * means if we detect any X or Z bits, change the entire results to
 * all X.
 *
 * Assume both vectors are the same size.
 */
ಶೂನ್ಯ vvp_vector4_t::ಕೂಡಿಸು(ಸ್ಥಿರಾಂಕ vvp_vector4_t&that)
{
      ಪ್ರತಿಜ್ಞಾ(ಗಾತ್ರ_3 == that.ಗಾತ್ರ_3);

      ಆದರೆ (ಗಾತ್ರ_3 < ಬಿಟ್ಟುಗಳು_ಪ್ರತಿ_ಶಬ್ದ) {
	    ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ ಮುಖವಾಡ = ~(-1UL << ಗಾತ್ರ_3);
	    ಆದರೆ ((bbits_val_|that.bbits_val_) & ಮುಖವಾಡ) {
		  abits_val_ |= ಮುಖವಾಡ;
		  bbits_val_ |= ಮುಖವಾಡ;
		  ಹಿಂತಿರುಗಿ;
	    }

	    abits_val_ += that.abits_val_;
	    abits_val_ &= ಮುಖವಾಡ;
	    ಹಿಂತಿರುಗಿ;
      }

      ಆದರೆ (ಗಾತ್ರ_3 == ಬಿಟ್ಟುಗಳು_ಪ್ರತಿ_ಶಬ್ದ) {
	    ಆದರೆ (bbits_val_ | that.bbits_val_) {
		  abits_val_ = ಶಬ್ದ_X_ABITS;
		  bbits_val_ = ಶಬ್ದ_X_BBITS;
	    } ಇಲ್ಲದಿದ್ದರೆ {
		  abits_val_ += that.abits_val_;
	    }
	    ಹಿಂತಿರುಗಿ;
      }

      ಪೂರ್ಣಾಂಕ cnt = ಗಾತ್ರ_3 / ಬಿಟ್ಟುಗಳು_ಪ್ರತಿ_ಶಬ್ದ;
      ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ ವಾಹಕ = 0;
      ಗಾಗಿ (ಪೂರ್ಣಾಂಕ idx = 0 ; idx < cnt ; idx += 1) {
	    ಆದರೆ (bbits_ptr_[idx] | that.bbits_ptr_[idx])
		  ಹೋಗು x_ಹೊರಹೋಗು;

	    abits_ptr_[idx] = ಕೂಡಿಸು_with_ವಾಹಕ(abits_ptr_[idx], that.abits_ptr_[idx], ವಾಹಕ);
      }

      ಆದರೆ (ಚಿಹ್ನೆರಹಿತ ಬಾಲ = ಗಾತ್ರ_3 % ಬಿಟ್ಟುಗಳು_ಪ್ರತಿ_ಶಬ್ದ) {
	    ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ ಮುಖವಾಡ = ~( -1UL << ಬಾಲ );
	    ಆದರೆ ((bbits_ptr_[cnt] | that.bbits_ptr_[cnt])&ಮುಖವಾಡ)
		  ಹೋಗು x_ಹೊರಹೋಗು;

	    abits_ptr_[cnt] = ಕೂಡಿಸು_with_ವಾಹಕ(abits_ptr_[cnt], that.abits_ptr_[cnt], ವಾಹಕ);
	    abits_ptr_[cnt] &= ಮುಖವಾಡ;
      }

      ಹಿಂತಿರುಗಿ;

 x_ಹೊರಹೋಗು:
      ಗಾಗಿ (ಪೂರ್ಣಾಂಕ idx = 0 ; idx < cnt ; idx += 1) {
	    abits_ptr_[idx] = ಶಬ್ದ_X_ABITS;
	    bbits_ptr_[idx] = ಶಬ್ದ_X_BBITS;
      }
      ಆದರೆ (ಚಿಹ್ನೆರಹಿತ ಬಾಲ = ಗಾತ್ರ_3%ಬಿಟ್ಟುಗಳು_ಪ್ರತಿ_ಶಬ್ದ) {
	    ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ ಮುಖವಾಡ = ~( -1UL << ಬಾಲ );
	    abits_ptr_[cnt] = ಶಬ್ದ_X_ABITS&ಮುಖವಾಡ;
	    bbits_ptr_[cnt] = ಶಬ್ದ_X_BBITS&ಮುಖವಾಡ;
      }
}

ಶೂನ್ಯ vvp_vector4_t::sub(ಸ್ಥಿರಾಂಕ vvp_vector4_t&that)
{
      ಪ್ರತಿಜ್ಞಾ(ಗಾತ್ರ_3 == that.ಗಾತ್ರ_3);

      ಆದರೆ (ಗಾತ್ರ_3 < ಬಿಟ್ಟುಗಳು_ಪ್ರತಿ_ಶಬ್ದ) {
	    ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ ಮುಖವಾಡ = ~(-1UL << ಗಾತ್ರ_3);
	    ಆದರೆ ((bbits_val_|that.bbits_val_) & ಮುಖವಾಡ) {
		  abits_val_ |= ಮುಖವಾಡ;
		  bbits_val_ |= ಮುಖವಾಡ;
		  ಹಿಂತಿರುಗಿ;
	    }

	    abits_val_ -= that.abits_val_;
	    abits_val_ &= ಮುಖವಾಡ;
	    ಹಿಂತಿರುಗಿ;
      }

      ಆದರೆ (ಗಾತ್ರ_3 == ಬಿಟ್ಟುಗಳು_ಪ್ರತಿ_ಶಬ್ದ) {
	    ಆದರೆ (bbits_val_ | that.bbits_val_) {
		  abits_val_ = ಶಬ್ದ_X_ABITS;
		  bbits_val_ = ಶಬ್ದ_X_BBITS;
	    } ಇಲ್ಲದಿದ್ದರೆ {
		  abits_val_ -= that.abits_val_;
	    }
	    ಹಿಂತಿರುಗಿ;
      }

      ಪೂರ್ಣಾಂಕ cnt = ಗಾತ್ರ_3 / ಬಿಟ್ಟುಗಳು_ಪ್ರತಿ_ಶಬ್ದ;
      ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ ವಾಹಕ = 1;
      ಗಾಗಿ (ಪೂರ್ಣಾಂಕ idx = 0 ; idx < cnt ; idx += 1) {
	    ಆದರೆ (bbits_ptr_[idx] | that.bbits_ptr_[idx])
		  ಹೋಗು x_ಹೊರಹೋಗು;

	    abits_ptr_[idx] = ಕೂಡಿಸು_with_ವಾಹಕ(abits_ptr_[idx], ~that.abits_ptr_[idx], ವಾಹಕ);
      }

      ಆದರೆ (ಚಿಹ್ನೆರಹಿತ ಬಾಲ = ಗಾತ್ರ_3 % ಬಿಟ್ಟುಗಳು_ಪ್ರತಿ_ಶಬ್ದ) {
	    ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ ಮುಖವಾಡ = ~( -1UL << ಬಾಲ );
	    ಆದರೆ ((bbits_ptr_[cnt] | that.bbits_ptr_[cnt])&ಮುಖವಾಡ)
		  ಹೋಗು x_ಹೊರಹೋಗು;

	    abits_ptr_[cnt] = ಕೂಡಿಸು_with_ವಾಹಕ(abits_ptr_[cnt], ~that.abits_ptr_[cnt], ವಾಹಕ);
	    abits_ptr_[cnt] &= ಮುಖವಾಡ;
      }

      ಹಿಂತಿರುಗಿ;

 x_ಹೊರಹೋಗು:
      ಗಾಗಿ (ಪೂರ್ಣಾಂಕ idx = 0 ; idx < cnt ; idx += 1) {
	    abits_ptr_[idx] = ಶಬ್ದ_X_ABITS;
	    bbits_ptr_[idx] = ಶಬ್ದ_X_BBITS;
      }
      ಆದರೆ (ಚಿಹ್ನೆರಹಿತ ಬಾಲ = ಗಾತ್ರ_3%ಬಿಟ್ಟುಗಳು_ಪ್ರತಿ_ಶಬ್ದ) {
	    ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ ಮುಖವಾಡ = ~( -1UL << ಬಾಲ );
	    abits_ptr_[cnt] = ಶಬ್ದ_X_ABITS&ಮುಖವಾಡ;
	    bbits_ptr_[cnt] = ಶಬ್ದ_X_BBITS&ಮುಖವಾಡ;
      }

}

ಶೂನ್ಯ vvp_vector4_t::mov(ಚಿಹ್ನೆರಹಿತ dst, ಚಿಹ್ನೆರಹಿತ src, ಚಿಹ್ನೆರಹಿತ cnt)
{
      ಪ್ರತಿಜ್ಞಾ(dst+cnt <= ಗಾತ್ರ_3);
      ಪ್ರತಿಜ್ಞಾ(src+cnt <= ಗಾತ್ರ_3);

      ಆದರೆ (ಗಾತ್ರ_3 <= ಬಿಟ್ಟುಗಳು_ಪ್ರತಿ_ಶಬ್ದ) {
	    ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ vmask = (1UL << cnt) - 1;
	    ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ tmp;

	    tmp = (abits_val_ >> src) & vmask;
	    abits_val_ &= ~ (vmask << dst);
	    abits_val_ |= tmp << dst;

	    tmp = (bbits_val_ >> src) & vmask;
	    bbits_val_ &= ~ (vmask << dst);
	    bbits_val_ |= tmp << dst;

      } ಇಲ್ಲದಿದ್ದರೆ {
	    ಚಿಹ್ನೆರಹಿತ sptr = src / ಬಿಟ್ಟುಗಳು_ಪ್ರತಿ_ಶಬ್ದ;
	    ಚಿಹ್ನೆರಹಿತ dptr = dst / ಬಿಟ್ಟುಗಳು_ಪ್ರತಿ_ಶಬ್ದ;
	    ಚಿಹ್ನೆರಹಿತ soff = src % ಬಿಟ್ಟುಗಳು_ಪ್ರತಿ_ಶಬ್ದ;
	    ಚಿಹ್ನೆರಹಿತ doff = dst % ಬಿಟ್ಟುಗಳು_ಪ್ರತಿ_ಶಬ್ದ;

	    ಆಗ (cnt > 0) {
		  ಚಿಹ್ನೆರಹಿತ trans = cnt;
		  ಆದರೆ ((soff+trans) > ಬಿಟ್ಟುಗಳು_ಪ್ರತಿ_ಶಬ್ದ)
			trans = ಬಿಟ್ಟುಗಳು_ಪ್ರತಿ_ಶಬ್ದ - soff;

		  ಆದರೆ ((doff+trans) > ಬಿಟ್ಟುಗಳು_ಪ್ರತಿ_ಶಬ್ದ)
			trans = ಬಿಟ್ಟುಗಳು_ಪ್ರತಿ_ಶಬ್ದ - doff;

		  ಆದರೆ (trans == ಬಿಟ್ಟುಗಳು_ಪ್ರತಿ_ಶಬ್ದ) {
			  // Special case: the transfer count is
			  // exactly an entire word. For this to be
			  // true, it must also be true that the
			  // pointers are aligned. The work is easy,
			abits_ptr_[dptr] = abits_ptr_[sptr];
			bbits_ptr_[dptr] = bbits_ptr_[sptr];
			dptr += 1;
			sptr += 1;
			cnt -= ಬಿಟ್ಟುಗಳು_ಪ್ರತಿ_ಶಬ್ದ;
			ಮುಂದುವರಿ;
		  }

		    // Here we know that either the source or
		    // destination is unaligned, and also we know that
		    // the count is less than a full word.
		  ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ vmask = (1UL << trans) - 1;
		  ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ tmp;

		  tmp = (abits_ptr_[sptr] >> soff) & vmask;
		  abits_ptr_[dptr] &= ~ (vmask << doff);
		  abits_ptr_[dptr] |= tmp << doff;

		  tmp = (bbits_ptr_[sptr] >> soff) & vmask;
		  bbits_ptr_[dptr] &= ~ (vmask << doff);
		  bbits_ptr_[dptr] |= tmp << doff;

		  cnt -= trans;
		  soff += trans;
		  ಆದರೆ (soff >= ಬಿಟ್ಟುಗಳು_ಪ್ರತಿ_ಶಬ್ದ) {
			soff = 0;
			sptr += 1;
		  }
		  doff += trans;
		  ಆದರೆ (doff >= ಬಿಟ್ಟುಗಳು_ಪ್ರತಿ_ಶಬ್ದ) {
			doff = 0;
			dptr += 1;
		  }
	    }
      }
}

ಶೂನ್ಯ vvp_vector4_t::mul(ಸ್ಥಿರಾಂಕ vvp_vector4_t&that)
{
      ಪ್ರತಿಜ್ಞಾ(ಗಾತ್ರ_3 == that.ಗಾತ್ರ_3);

      ಆದರೆ (ಗಾತ್ರ_3 < ಬಿಟ್ಟುಗಳು_ಪ್ರತಿ_ಶಬ್ದ) {
	    ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ ಮುಖವಾಡ = ~(-1UL << ಗಾತ್ರ_3);
	    ಆದರೆ ((bbits_val_|that.bbits_val_) & ಮುಖವಾಡ) {
		  abits_val_ |= ಮುಖವಾಡ;
		  bbits_val_ |= ಮುಖವಾಡ;
		  ಹಿಂತಿರುಗಿ;
	    }

	    abits_val_ *= that.abits_val_;
	    abits_val_ &= ಮುಖವಾಡ;
	    ಹಿಂತಿರುಗಿ;
      }

      ಆದರೆ (ಗಾತ್ರ_3 == ಬಿಟ್ಟುಗಳು_ಪ್ರತಿ_ಶಬ್ದ) {
	    ಆದರೆ (bbits_val_ || that.bbits_val_) {
		  abits_val_ = ಶಬ್ದ_X_ABITS;
		  bbits_val_ = ಶಬ್ದ_X_BBITS;
	    } ಇಲ್ಲದಿದ್ದರೆ {
		  abits_val_ *= that.abits_val_;
	    }
	    ಹಿಂತಿರುಗಿ;
      }

      ಸ್ಥಿರಾಂಕ ಪೂರ್ಣಾಂಕ cnt = (ಗಾತ್ರ_3+ಬಿಟ್ಟುಗಳು_ಪ್ರತಿ_ಶಬ್ದ-1) / ಬಿಟ್ಟುಗಳು_ಪ್ರತಿ_ಶಬ್ದ;

      ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ ಮುಖವಾಡ;
      ಆದರೆ (ಚಿಹ್ನೆರಹಿತ ಬಾಲ = ಗಾತ್ರ_3%ಬಿಟ್ಟುಗಳು_ಪ್ರತಿ_ಶಬ್ದ) {
	    ಮುಖವಾಡ = ~( -1UL << ಬಾಲ );
      } ಇಲ್ಲದಿದ್ದರೆ {
	    ಮುಖವಾಡ = ~0UL;
      }

	// Check for any XZ values ahead of time in a first pass. If
	// we find any, then force the entire result to be X and be
	// done.
      ಗಾಗಿ (ಪೂರ್ಣಾಂಕ idx = 0 ; idx < cnt ; idx += 1) {
	    ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ lval = bbits_ptr_[idx];
	    ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ rval = that.bbits_ptr_[idx];
	    ಆದರೆ (idx == (cnt-1)) {
		  lval &= ಮುಖವಾಡ;
		  rval &= ಮುಖವಾಡ;
	    }
	    ಆದರೆ (lval || rval) {
		  ಗಾಗಿ (ಪೂರ್ಣಾಂಕ xdx = 0 ; xdx < cnt-1 ; xdx += 1) {
			abits_ptr_[xdx] = ಶಬ್ದ_X_ABITS;
			bbits_ptr_[xdx] = ಶಬ್ದ_X_BBITS;
		  }
		  abits_ptr_[cnt-1] = ಶಬ್ದ_X_ABITS & ಮುಖವಾಡ;
		  bbits_ptr_[cnt-1] = ಶಬ್ದ_X_BBITS & ಮುಖವಾಡ;
		  ಹಿಂತಿರುಗಿ;
	    }
      }

	// Calculate the result into a res array. We need to keep is
	// separate from the "this" array because we are making
	// multiple passes.
      ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ*res = ಹೊಸ ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ[cnt];
      ಗಾಗಿ (ಪೂರ್ಣಾಂಕ idx = 0 ; idx < cnt ; idx += 1)
	    res[idx] = 0;

      ಗಾಗಿ (ಪೂರ್ಣಾಂಕ mul_a = 0 ; mul_a < cnt ; mul_a += 1) {
	    ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ lval = abits_ptr_[mul_a];
	    ಆದರೆ (mul_a == (cnt-1))
		  lval &= ಮುಖವಾಡ;

	    ಗಾಗಿ (ಪೂರ್ಣಾಂಕ mul_b = 0 ; mul_b < (cnt-mul_a) ; mul_b += 1) {
		  ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ rval = that.abits_ptr_[mul_b];
		  ಆದರೆ (mul_b == (cnt-1))
			rval &= ಮುಖವಾಡ;

		  ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ ಮೊತ್ತ;
		  ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ tmp = multiply_with_ವಾಹಕ(lval, rval, ಮೊತ್ತ);
		  ಪೂರ್ಣಾಂಕ ತಳ = mul_a + mul_b;
		  ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ ವಾಹಕ = 0;
		  res[ತಳ] = ಕೂಡಿಸು_with_ವಾಹಕ(res[ತಳ], tmp, ವಾಹಕ);
		  ಗಾಗಿ (ಪೂರ್ಣಾಂಕ ಕೂಡಿಸು_idx = ತಳ+1 ; ಕೂಡಿಸು_idx < cnt ; ಕೂಡಿಸು_idx += 1) {
			res[ಕೂಡಿಸು_idx] = ಕೂಡಿಸು_with_ವಾಹಕ(res[ಕೂಡಿಸು_idx], ಮೊತ್ತ, ವಾಹಕ);
			ಮೊತ್ತ = 0;
		  }
	    }
      }

	// Replace the "this" value with the calculated result. We
	// know a-priori that the bbits are zero and unchanged.
      res[cnt-1] &= ಮುಖವಾಡ;
      ಗಾಗಿ (ಪೂರ್ಣಾಂಕ idx = 0 ; idx < cnt ; idx += 1)
	    abits_ptr_[idx] = res[idx];

      ಅಳಿಸು[]res;
      ಹಿಂತಿರುಗಿ;


}

ತಾರ್ಕಿಕ vvp_vector4_t::eeq(ಸ್ಥಿರಾಂಕ vvp_vector4_t&that) ಸ್ಥಿರಾಂಕ
{
      ಆದರೆ (ಗಾತ್ರ_3 != that.ಗಾತ್ರ_3)
	    ಹಿಂತಿರುಗಿ ಸುಳ್ಳು;

      ಆದರೆ (ಗಾತ್ರ_3 < ಬಿಟ್ಟುಗಳು_ಪ್ರತಿ_ಶಬ್ದ) {
	    ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ ಮುಖವಾಡ = (1UL << ಗಾತ್ರ_3) - 1;
	    ಹಿಂತಿರುಗಿ (abits_val_&ಮುಖವಾಡ) == (that.abits_val_&ಮುಖವಾಡ)
		  && (bbits_val_&ಮುಖವಾಡ) == (that.bbits_val_&ಮುಖವಾಡ);
      }

      ಆದರೆ (ಗಾತ್ರ_3 == ಬಿಟ್ಟುಗಳು_ಪ್ರತಿ_ಶಬ್ದ) {
	    ಹಿಂತಿರುಗಿ (abits_val_ == that.abits_val_)
		  && (bbits_val_ == that.bbits_val_);
      }

      ಚಿಹ್ನೆರಹಿತ ಪದಗಳು = ಗಾತ್ರ_3 / ಬಿಟ್ಟುಗಳು_ಪ್ರತಿ_ಶಬ್ದ;
      ಗಾಗಿ (ಚಿಹ್ನೆರಹಿತ idx = 0 ;  idx < ಪದಗಳು ;  idx += 1) {
	    ಆದರೆ (abits_ptr_[idx] != that.abits_ptr_[idx])
		  ಹಿಂತಿರುಗಿ ಸುಳ್ಳು;
	    ಆದರೆ (bbits_ptr_[idx] != that.bbits_ptr_[idx])
		  ಹಿಂತಿರುಗಿ ಸುಳ್ಳು;
      }

      ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ ಮುಖವಾಡ = ಗಾತ್ರ_3%ಬಿಟ್ಟುಗಳು_ಪ್ರತಿ_ಶಬ್ದ;
      ಆದರೆ (ಮುಖವಾಡ > 0) {
	    ಮುಖವಾಡ = (1UL << ಮುಖವಾಡ) - 1;
	    ಹಿಂತಿರುಗಿ (abits_ptr_[ಪದಗಳು]&ಮುಖವಾಡ) == (that.abits_ptr_[ಪದಗಳು]&ಮುಖವಾಡ)
		  && (bbits_ptr_[ಪದಗಳು]&ಮುಖವಾಡ) == (that.bbits_ptr_[ಪದಗಳು]&ಮುಖವಾಡ);
      }

      ಹಿಂತಿರುಗಿ ನಿಜ;
}

ತಾರ್ಕಿಕ vvp_vector4_t::eq_xz(ಸ್ಥಿರಾಂಕ vvp_vector4_t&that) ಸ್ಥಿರಾಂಕ
{
      ಆದರೆ (ಗಾತ್ರ_3 != that.ಗಾತ್ರ_3)
	    ಹಿಂತಿರುಗಿ ಸುಳ್ಳು;

      ಆದರೆ (ಗಾತ್ರ_3 < ಬಿಟ್ಟುಗಳು_ಪ್ರತಿ_ಶಬ್ದ) {
	    ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ ಮುಖವಾಡ = (1UL << ಗಾತ್ರ_3) - 1;
	    ಹಿಂತಿರುಗಿ ((abits_val_|bbits_val_)&ಮುಖವಾಡ) == ((that.abits_val_|that.bbits_val_)&ಮುಖವಾಡ)
		  && (bbits_val_&ಮುಖವಾಡ) == (that.bbits_val_&ಮುಖವಾಡ);
      }

      ಆದರೆ (ಗಾತ್ರ_3 == ಬಿಟ್ಟುಗಳು_ಪ್ರತಿ_ಶಬ್ದ) {
	    ಹಿಂತಿರುಗಿ ((abits_val_|bbits_val_) == (that.abits_val_|that.bbits_val_))
		  && (bbits_val_ == that.bbits_val_);
      }

      ಚಿಹ್ನೆರಹಿತ ಪದಗಳು = ಗಾತ್ರ_3 / ಬಿಟ್ಟುಗಳು_ಪ್ರತಿ_ಶಬ್ದ;
      ಗಾಗಿ (ಚಿಹ್ನೆರಹಿತ idx = 0 ;  idx < ಪದಗಳು ;  idx += 1) {
	    ಆದರೆ ((abits_ptr_[idx]|bbits_ptr_[idx]) != (that.abits_ptr_[idx]|that.bbits_ptr_[idx]))
		  ಹಿಂತಿರುಗಿ ಸುಳ್ಳು;
	    ಆದರೆ (bbits_ptr_[idx] != that.bbits_ptr_[idx])
		  ಹಿಂತಿರುಗಿ ಸುಳ್ಳು;
      }

      ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ ಮುಖವಾಡ = ಗಾತ್ರ_3%ಬಿಟ್ಟುಗಳು_ಪ್ರತಿ_ಶಬ್ದ;
      ಆದರೆ (ಮುಖವಾಡ > 0) {
	    ಮುಖವಾಡ = (1UL << ಮುಖವಾಡ) - 1;
	    ಹಿಂತಿರುಗಿ ((abits_ptr_[ಪದಗಳು]|bbits_ptr_[ಪದಗಳು])&ಮುಖವಾಡ) == ((that.abits_ptr_[ಪದಗಳು]|that.bbits_ptr_[ಪದಗಳು])&ಮುಖವಾಡ)
		  && (bbits_ptr_[ಪದಗಳು]&ಮುಖವಾಡ) == (that.bbits_ptr_[ಪದಗಳು]&ಮುಖವಾಡ);
      }

      ಹಿಂತಿರುಗಿ ನಿಜ;
}

ತಾರ್ಕಿಕ vvp_vector4_t::has_xz() ಸ್ಥಿರಾಂಕ
{
      ಆದರೆ (ಗಾತ್ರ_3 < ಬಿಟ್ಟುಗಳು_ಪ್ರತಿ_ಶಬ್ದ) {
	    ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ ಮುಖವಾಡ = -1UL >> (ಬಿಟ್ಟುಗಳು_ಪ್ರತಿ_ಶಬ್ದ - ಗಾತ್ರ_3);
	    ಹಿಂತಿರುಗಿ bbits_val_&ಮುಖವಾಡ;
      }

      ಆದರೆ (ಗಾತ್ರ_3 == ಬಿಟ್ಟುಗಳು_ಪ್ರತಿ_ಶಬ್ದ) {
	    ಹಿಂತಿರುಗಿ bbits_val_;
      }

      ಚಿಹ್ನೆರಹಿತ ಪದಗಳು = ಗಾತ್ರ_3 / ಬಿಟ್ಟುಗಳು_ಪ್ರತಿ_ಶಬ್ದ;
      ಗಾಗಿ (ಚಿಹ್ನೆರಹಿತ idx = 0 ; idx < ಪದಗಳು ; idx += 1) {
	    ಆದರೆ (bbits_ptr_[idx])
		  ಹಿಂತಿರುಗಿ ನಿಜ;
      }

      ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ ಮುಖವಾಡ = ಗಾತ್ರ_3%ಬಿಟ್ಟುಗಳು_ಪ್ರತಿ_ಶಬ್ದ;
      ಆದರೆ (ಮುಖವಾಡ > 0) {
	    ಮುಖವಾಡ = -1UL >> (ಬಿಟ್ಟುಗಳು_ಪ್ರತಿ_ಶಬ್ದ - ಮುಖವಾಡ);
	    ಹಿಂತಿರುಗಿ bbits_ptr_[ಪದಗಳು]&ಮುಖವಾಡ;
      }

      ಹಿಂತಿರುಗಿ ಸುಳ್ಳು;
}

ಶೂನ್ಯ vvp_vector4_t::change_z2x()
{
	// This method relies on the fact that both BIT4_X and BIT4_Z
	// have the bbit set in the vector4 encoding, and also that
	// the BIT4_X has abit set in the vector4 encoding. By simply
	// or-ing the bbit into the abit, BIT4_X and BIT4_Z both
	// become BIT4_X.

      ಆದರೆ (ಗಾತ್ರ_3 <= ಬಿಟ್ಟುಗಳು_ಪ್ರತಿ_ಶಬ್ದ) {
	    abits_val_ |= bbits_val_;
      } ಇಲ್ಲದಿದ್ದರೆ {
	    ಚಿಹ್ನೆರಹಿತ ಪದಗಳು = (ಗಾತ್ರ_3+ಬಿಟ್ಟುಗಳು_ಪ್ರತಿ_ಶಬ್ದ-1) / ಬಿಟ್ಟುಗಳು_ಪ್ರತಿ_ಶಬ್ದ;
	    ಗಾಗಿ (ಚಿಹ್ನೆರಹಿತ idx = 0 ;  idx < ಪದಗಳು ;  idx += 1)
		  abits_ptr_[idx] |= bbits_ptr_[idx];
      }
}

ಶೂನ್ಯ vvp_vector4_t::fill_ಬಿಟ್ಟುಗಳು(vvp_bit4_t ಬಿಟ್)
{
	/* note: this relies on the bit encoding for the vvp_bit4_t. */
      ಸ್ಥಿರ ಸ್ಥಿರಾಂಕ ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ init_atable[4] = {
	    ಶಬ್ದ_0_ABITS,
	    ಶಬ್ದ_1_ABITS,
	    ಶಬ್ದ_Z_ABITS,
	    ಶಬ್ದ_X_ABITS };
      ಸ್ಥಿರ ಸ್ಥಿರಾಂಕ ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ init_btable[4] = {
	    ಶಬ್ದ_0_BBITS,
	    ಶಬ್ದ_1_BBITS,
	    ಶಬ್ದ_Z_BBITS,
	    ಶಬ್ದ_X_BBITS };

      ಆದರೆ (ಗಾತ್ರ_3 <= ಬಿಟ್ಟುಗಳು_ಪ್ರತಿ_ಶಬ್ದ) {
	    abits_val_ = init_atable[ಬಿಟ್];
	    bbits_val_ = init_btable[ಬಿಟ್];
      } ಇಲ್ಲದಿದ್ದರೆ {
	    ಚಿಹ್ನೆರಹಿತ ಪದಗಳು = (ಗಾತ್ರ_3 + ಬಿಟ್ಟುಗಳು_ಪ್ರತಿ_ಶಬ್ದ - 1) / ಬಿಟ್ಟುಗಳು_ಪ್ರತಿ_ಶಬ್ದ;
	    ಗಾಗಿ (ಚಿಹ್ನೆರಹಿತ idx = 0; idx < ಪದಗಳು; idx += 1) {
		  abits_ptr_[idx] = init_atable[ಬಿಟ್];
		  bbits_ptr_[idx] = init_btable[ಬಿಟ್];
	    }
      }
}

ಅಕ್ಷರ* vvp_vector4_t::as_string(ಅಕ್ಷರ*buf, size_t buf_len) ಸ್ಥಿರಾಂಕ
{
      ಅಕ್ಷರ*res = buf;
      *buf++ = 'C';
      *buf++ = '4';
      *buf++ = '<';
      buf_len -= 3;

      ಗಾಗಿ (ಚಿಹ್ನೆರಹಿತ idx = 0 ;  idx < size() && buf_len >= 2 ;  idx += 1) {
	    ಆಯ್ಕೆ (value(size()-idx-1)) {
		ಸ್ಥಿತಿ BIT4_0:
		  *buf++ = '0';
		  ನಿಲ್ಲಿಸು;
		ಸ್ಥಿತಿ BIT4_1:
		  *buf++ = '1';
		  ನಿಲ್ಲಿಸು;
		ಸ್ಥಿತಿ BIT4_X:
		  *buf++ = 'x';
		  ನಿಲ್ಲಿಸು;
		ಸ್ಥಿತಿ BIT4_Z:
		  *buf++ = 'z';
	    }
	    buf_len -= 1;
      }

      *buf++ = '>';
      *buf++ = 0;
      ಹಿಂತಿರುಗಿ res;
}

ಶೂನ್ಯ vvp_vector4_t::invert()
{
      ಆದರೆ (ಗಾತ್ರ_3 <= ಬಿಟ್ಟುಗಳು_ಪ್ರತಿ_ಶಬ್ದ) {
	    ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ ಮುಖವಾಡ = (ಗಾತ್ರ_3<ಬಿಟ್ಟುಗಳು_ಪ್ರತಿ_ಶಬ್ದ)? (1UL<<ಗಾತ್ರ_3)-1UL : -1UL;
	    abits_val_ = ಮುಖವಾಡ & ~abits_val_;
	    abits_val_ |= bbits_val_;
      } ಇಲ್ಲದಿದ್ದರೆ {
	    ಚಿಹ್ನೆರಹಿತ ಉಳಿದ = ಗಾತ್ರ_3;
	    ಚಿಹ್ನೆರಹಿತ idx = 0;
	    ಆಗ (ಉಳಿದ >= ಬಿಟ್ಟುಗಳು_ಪ್ರತಿ_ಶಬ್ದ) {
		  abits_ptr_[idx] = ~abits_ptr_[idx];
		  abits_ptr_[idx] |= bbits_ptr_[idx];
		  idx += 1;
		  ಉಳಿದ -= ಬಿಟ್ಟುಗಳು_ಪ್ರತಿ_ಶಬ್ದ;
	    }
	    ಆದರೆ (ಉಳಿದ > 0) {
		  ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ ಮುಖವಾಡ = (1UL<<ಉಳಿದ) - 1UL;
		  abits_ptr_[idx] = ಮುಖವಾಡ & ~abits_ptr_[idx];
		  abits_ptr_[idx] |= bbits_ptr_[idx];
	    }
      }
}

#ವ್ಯಾಖ್ಯಾನಿಸು ಬಿಟ್_ಮುಖವಾಡ(n) ((n) ? ((~0UL) >> (ಬಿಟ್ಟುಗಳು_ಪ್ರತಿ_ಶಬ್ದ - (n))) : ~0UL)

vvp_bit4_t vvp_vector4_t::reduce_or() ಸ್ಥಿರಾಂಕ
{
      ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ ಮುಖವಾಡ = ಬಿಟ್_ಮುಖವಾಡ(ಗಾತ್ರ_3 % ಬಿಟ್ಟುಗಳು_ಪ್ರತಿ_ಶಬ್ದ);
      vvp_bit4_t res = BIT4_0;

      ಆದರೆ (ಗಾತ್ರ_3 <= ಬಿಟ್ಟುಗಳು_ಪ್ರತಿ_ಶಬ್ದ) {
	    ಆದರೆ ((abits_val_ & ~bbits_val_ & ಮುಖವಾಡ) != 0UL)
		  ಹಿಂತಿರುಗಿ BIT4_1;
	    ಆದರೆ ((bbits_val_ & ಮುಖವಾಡ) != 0UL)
		  ಹಿಂತಿರುಗಿ BIT4_X;
      } ಇಲ್ಲದಿದ್ದರೆ {
	    ಚಿಹ್ನೆರಹಿತ ಪದಗಳು = (ಗಾತ್ರ_3 + ಬಿಟ್ಟುಗಳು_ಪ್ರತಿ_ಶಬ್ದ - 1) / ಬಿಟ್ಟುಗಳು_ಪ್ರತಿ_ಶಬ್ದ;
	    ಚಿಹ್ನೆರಹಿತ idx;
	    ಗಾಗಿ (idx = 0; idx < ಪದಗಳು - 1; idx += 1) {
		  ಆದರೆ ((abits_ptr_[idx] & ~bbits_ptr_[idx]) != 0UL)
			ಹಿಂತಿರುಗಿ BIT4_1;
		  ಆದರೆ (bbits_ptr_[idx] != 0UL)
			res = BIT4_X;
	    }
	    ಆದರೆ ((abits_ptr_[idx] & ~bbits_ptr_[idx] & ಮುಖವಾಡ) != 0UL)
		  ಹಿಂತಿರುಗಿ BIT4_1;
	    ಆದರೆ ((bbits_ptr_[idx] & ಮುಖವಾಡ) != 0UL)
		  res = BIT4_X;
      }

      ಹಿಂತಿರುಗಿ res;
}

vvp_bit4_t vvp_vector4_t::reduce_and() ಸ್ಥಿರಾಂಕ
{
      ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ ಮುಖವಾಡ = ಬಿಟ್_ಮುಖವಾಡ(ಗಾತ್ರ_3 % ಬಿಟ್ಟುಗಳು_ಪ್ರತಿ_ಶಬ್ದ);
      vvp_bit4_t res = BIT4_1;

      ಆದರೆ (ಗಾತ್ರ_3 <= ಬಿಟ್ಟುಗಳು_ಪ್ರತಿ_ಶಬ್ದ) {
	    ಆದರೆ ((abits_val_ | bbits_val_ | ~ಮುಖವಾಡ) != ~0UL)
		  ಹಿಂತಿರುಗಿ BIT4_0;
	    ಆದರೆ ((bbits_val_ & ಮುಖವಾಡ) != 0UL)
		  ಹಿಂತಿರುಗಿ BIT4_X;
	} ಇಲ್ಲದಿದ್ದರೆ {
	    ಚಿಹ್ನೆರಹಿತ ಪದಗಳು = (ಗಾತ್ರ_3 + ಬಿಟ್ಟುಗಳು_ಪ್ರತಿ_ಶಬ್ದ - 1) / ಬಿಟ್ಟುಗಳು_ಪ್ರತಿ_ಶಬ್ದ;
	    ಚಿಹ್ನೆರಹಿತ idx;
	    ಗಾಗಿ (idx = 0; idx < ಪದಗಳು - 1; idx += 1) {
		  ಆದರೆ ((abits_ptr_[idx] | bbits_ptr_[idx]) != ~0UL)
			ಹಿಂತಿರುಗಿ BIT4_0;
		  ಆದರೆ (bbits_ptr_[idx] != 0UL)
			res = BIT4_X;
	    }
	    ಆದರೆ ((abits_ptr_[idx] | bbits_ptr_[idx] | ~ಮುಖವಾಡ) != ~0UL)
		  ಹಿಂತಿರುಗಿ BIT4_0;
	    ಆದರೆ ((bbits_ptr_[idx] & ಮುಖವಾಡ) != 0UL)
		  res = BIT4_X;
      }

      ಹಿಂತಿರುಗಿ res;
}

ಸ್ಥಿರ ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ parity(ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ val)
{
#ಆದರೆ defined(__GNUC__)
      // The compiler builtin can use target-specific CPU instructions.
      ಹಿಂತಿರುಗಿ __builtin_parityl(val);
#ಇಲ್ಲದಿದ್ದರೆ
#ಆದರೆ ULONG_MAX > 0xffffffffUL
	val ^= val >> 32;
#ಆದರೆ_ಅಂತ್ಯ
	val ^= val >> 16;
	val ^= val >> 8;
	val ^= val >> 4;

      ಹಿಂತಿರುಗಿ (0x6996 >> (val & 0xf)) & 1;
#ಆದರೆ_ಅಂತ್ಯ
}

vvp_bit4_t vvp_vector4_t::reduce_xor() ಸ್ಥಿರಾಂಕ
{
      ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ ಮುಖವಾಡ = ಬಿಟ್_ಮುಖವಾಡ(ಗಾತ್ರ_3 % ಬಿಟ್ಟುಗಳು_ಪ್ರತಿ_ಶಬ್ದ);

      ಆದರೆ (ಗಾತ್ರ_3 <= ಬಿಟ್ಟುಗಳು_ಪ್ರತಿ_ಶಬ್ದ) {
	    ಆದರೆ ((bbits_val_ & ಮುಖವಾಡ) != 0UL)
		  ಹಿಂತಿರುಗಿ BIT4_X;
	    ಹಿಂತಿರುಗಿ parity(abits_val_ & ಮುಖವಾಡ) ? BIT4_1 : BIT4_0;
      } ಇಲ್ಲದಿದ್ದರೆ {
	    ಚಿಹ್ನೆರಹಿತ ಪದಗಳು = (ಗಾತ್ರ_3 + ಬಿಟ್ಟುಗಳು_ಪ್ರತಿ_ಶಬ್ದ - 1) / ಬಿಟ್ಟುಗಳು_ಪ್ರತಿ_ಶಬ್ದ;
	    ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ val_a = 0UL;
	    ಚಿಹ್ನೆರಹಿತ idx;
	    ಗಾಗಿ (idx = 0; idx < ಪದಗಳು - 1; idx += 1) {
		  ಆದರೆ (bbits_ptr_[idx] != 0UL)
			ಹಿಂತಿರುಗಿ BIT4_X;
		  val_a ^= abits_ptr_[idx];
	    }
	    ಆದರೆ ((bbits_ptr_[idx] & ಮುಖವಾಡ) != 0UL)
		  ಹಿಂತಿರುಗಿ BIT4_X;
	    val_a ^= abits_ptr_[idx] & ಮುಖವಾಡ;
	    ಹಿಂತಿರುಗಿ parity(val_a) ? BIT4_1 : BIT4_0;
      }
}

vvp_vector4_t& vvp_vector4_t::ಕ್ರಿಯಾಕಾರಕ &= (ಸ್ಥಿರಾಂಕ vvp_vector4_t&that)
{
	// The truth table is:
	//     00 01 11 10
	//  00 00 00 00 00
	//  01 00 01 11 11
	//  11 00 11 11 11
	//  10 00 11 11 11
      ಆದರೆ (ಗಾತ್ರ_3 <= ಬಿಟ್ಟುಗಳು_ಪ್ರತಿ_ಶಬ್ದ) {
	    ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ tmp1 = abits_val_ | bbits_val_;
	    ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ tmp2 = that.abits_val_ | that.bbits_val_;
	    abits_val_ = tmp1 & tmp2;
	    bbits_val_ = (tmp1 & that.bbits_val_) | (tmp2 & bbits_val_);
      } ಇಲ್ಲದಿದ್ದರೆ {
	    ಚಿಹ್ನೆರಹಿತ ಪದಗಳು = (ಗಾತ್ರ_3 + ಬಿಟ್ಟುಗಳು_ಪ್ರತಿ_ಶಬ್ದ - 1) / ಬಿಟ್ಟುಗಳು_ಪ್ರತಿ_ಶಬ್ದ;
	    ಗಾಗಿ (ಚಿಹ್ನೆರಹಿತ idx = 0; idx < ಪದಗಳು ; idx += 1) {
		  ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ tmp1 = abits_ptr_[idx] | bbits_ptr_[idx];
		  ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ tmp2 = that.abits_ptr_[idx] |
		                       that.bbits_ptr_[idx];
		  abits_ptr_[idx] = tmp1 & tmp2;
		  bbits_ptr_[idx] = (tmp1 & that.bbits_ptr_[idx]) |
		                    (tmp2 & bbits_ptr_[idx]);
	    }
      }

      ಹಿಂತಿರುಗಿ *ಇದು;
}

vvp_vector4_t& vvp_vector4_t::ಕ್ರಿಯಾಕಾರಕ |= (ಸ್ಥಿರಾಂಕ vvp_vector4_t&that)
{
	// The truth table is:
	//     00 01 11 10
	//  00 00 01 11 11
	//  01 01 01 01 01
	//  11 11 01 11 11
	//  10 11 01 11 11
      ಆದರೆ (ಗಾತ್ರ_3 <= ಬಿಟ್ಟುಗಳು_ಪ್ರತಿ_ಶಬ್ದ) {
	    ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ tmp = abits_val_ | bbits_val_ |
	                        that.abits_val_ | that.bbits_val_;
	    bbits_val_ = ((~abits_val_ | bbits_val_) & that.bbits_val_) |
	                 ((~that.abits_val_ | that.bbits_val_) & bbits_val_);
	    abits_val_ = tmp;

      } ಇಲ್ಲದಿದ್ದರೆ {
	    ಚಿಹ್ನೆರಹಿತ ಪದಗಳು = (ಗಾತ್ರ_3 + ಬಿಟ್ಟುಗಳು_ಪ್ರತಿ_ಶಬ್ದ - 1) / ಬಿಟ್ಟುಗಳು_ಪ್ರತಿ_ಶಬ್ದ;
	    ಗಾಗಿ (ಚಿಹ್ನೆರಹಿತ idx = 0; idx < ಪದಗಳು ; idx += 1) {
		  ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ tmp = abits_ptr_[idx] | bbits_ptr_[idx] |
	                        that.abits_ptr_[idx] | that.bbits_ptr_[idx];
		  bbits_ptr_[idx] = ((~abits_ptr_[idx] | bbits_ptr_[idx]) &
		                     that.bbits_ptr_[idx]) |
		                    ((~that.abits_ptr_[idx] |
		                      that.bbits_ptr_[idx]) & bbits_ptr_[idx]);
		  abits_ptr_[idx] = tmp;
	    }
      }

      ಹಿಂತಿರುಗಿ *ಇದು;
}

vvp_vector4_t& vvp_vector4_t::ಕ್ರಿಯಾಕಾರಕ ^= (ಸ್ಥಿರಾಂಕ vvp_vector4_t&that)
{
	// The truth table is:
	//     00 01 11 10
	//  00 00 01 11 11
	//  01 01 00 11 11
	//  11 11 11 11 11
	//  10 11 11 11 11
      ಆದರೆ (ಗಾತ್ರ_3 <= ಬಿಟ್ಟುಗಳು_ಪ್ರತಿ_ಶಬ್ದ) {
	    ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ bval = bbits_val_ | that.bbits_val_;
	    bbits_val_ = bval;
	    abits_val_ = (abits_val_ ^ that.abits_val_) | bval;
      } ಇಲ್ಲದಿದ್ದರೆ {
	    ಚಿಹ್ನೆರಹಿತ ಪದಗಳು = (ಗಾತ್ರ_3 + ಬಿಟ್ಟುಗಳು_ಪ್ರತಿ_ಶಬ್ದ - 1) / ಬಿಟ್ಟುಗಳು_ಪ್ರತಿ_ಶಬ್ದ;
	    ಗಾಗಿ (ಚಿಹ್ನೆರಹಿತ idx = 0; idx < ಪದಗಳು ; idx += 1) {
		  ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ bval = bbits_ptr_[idx] | that.bbits_ptr_[idx];
		  bbits_ptr_[idx] = bval;
		  abits_ptr_[idx] = (abits_ptr_[idx] ^ that.abits_ptr_[idx]) | bval;
	    }
      }

      ಹಿಂತಿರುಗಿ *ಇದು;
}

/*
* Add an integer to the vvp_vector4_t in place, bit by bit so that
* there is no size limitations.
*/
vvp_vector4_t& vvp_vector4_t::ಕ್ರಿಯಾಕಾರಕ += (int64_t that)
{
      vvp_bit4_t ವಾಹಕ = BIT4_0;
      ಚಿಹ್ನೆರಹಿತ idx;

      ಆದರೆ (has_xz()) {
	    vvp_vector4_t xxx (size(), BIT4_X);
	    *ಇದು = xxx;
	    ಹಿಂತಿರುಗಿ *ಇದು;
      }

      ಗಾಗಿ (idx = 0 ; idx < size() ; idx += 1) {
	    ಆದರೆ (that == 0 && ವಾಹಕ==BIT4_0)
		  ನಿಲ್ಲಿಸು;

	    vvp_bit4_t that_ಬಿಟ್ = (that&1)? BIT4_1 : BIT4_0;
	    that >>= 1;

	    ಆದರೆ (that_ಬಿಟ್==BIT4_0 && ವಾಹಕ==BIT4_0)
		  ಮುಂದುವರಿ;

	    vvp_bit4_t ಬಿಟ್ = value(idx);
	    ಬಿಟ್ = ಕೂಡಿಸು_with_ವಾಹಕ(ಬಿಟ್, that_ಬಿಟ್, ವಾಹಕ);

	    ಗಣ_ಬಿಟ್(idx, ಬಿಟ್);
      }

      ಹಿಂತಿರುಗಿ *ಇದು;
}

ostream& ಕ್ರಿಯಾಕಾರಕ<< (ostream&out, ಸ್ಥಿರಾಂಕ vvp_vector4_t&that)
{
      out << that.size() << "'b";
      ಗಾಗಿ (ಚಿಹ್ನೆರಹಿತ idx = 0 ;  idx < that.size() ;  idx += 1)
	    out << that.value(that.size()-idx-1);
      ಹಿಂತಿರುಗಿ out;
}

ಮಾದರಿ <ವರ್ಗ INT>ತಾರ್ಕಿಕ vector4_ಗಮ್ಯ_ಮೌಲ್ಯ(ಸ್ಥಿರಾಂಕ vvp_vector4_t&vec, INT&val,
					  ತಾರ್ಕಿಕ is_signed, ತಾರ್ಕಿಕ is_arithmetic)
{
      INT res = 0;
      INT msk = 1;
      ತಾರ್ಕಿಕ rc_flag = ನಿಜ;

      ಚಿಹ್ನೆರಹಿತ size = vec.size();
      ಆದರೆ (size > 8*ಗಾತ್ರ(val)) size = 8*ಗಾತ್ರ(val);
      ಗಾಗಿ (ಚಿಹ್ನೆರಹಿತ idx = 0 ;  idx < size ;  idx += 1) {
	    ಆಯ್ಕೆ (vec.value(idx)) {
		ಸ್ಥಿತಿ BIT4_0:
		  ನಿಲ್ಲಿಸು;
		ಸ್ಥಿತಿ BIT4_1:
		  res |= msk;
		  ನಿಲ್ಲಿಸು;
		ಪೂರ್ವನಿಯೋಜಿತ:
		  ಆದರೆ (is_arithmetic)
			ಹಿಂತಿರುಗಿ ಸುಳ್ಳು;
		  ಇಲ್ಲದಿದ್ದರೆ
			rc_flag = ಸುಳ್ಳು;
	    }

	    msk <<= 1;
      }

      ಆದರೆ (is_signed && vec.value(vec.size()-1) == BIT4_1) {
	    ಆದರೆ (vec.size() < 8*ಗಾತ್ರ(val))
		  res |= static_cast<INT>(-1ULL << vec.size());
      }

      val = res;
      ಹಿಂತಿರುಗಿ rc_flag;
}

ಮಾದರಿ ತಾರ್ಕಿಕ vector4_ಗಮ್ಯ_ಮೌಲ್ಯ(ಸ್ಥಿರಾಂಕ vvp_vector4_t&vec, int8_t&val,
			       ತಾರ್ಕಿಕ is_signed, ತಾರ್ಕಿಕ is_arithmetic);
ಮಾದರಿ ತಾರ್ಕಿಕ vector4_ಗಮ್ಯ_ಮೌಲ್ಯ(ಸ್ಥಿರಾಂಕ vvp_vector4_t&vec, int16_t&val,
			       ತಾರ್ಕಿಕ is_signed, ತಾರ್ಕಿಕ is_arithmetic);
ಮಾದರಿ ತಾರ್ಕಿಕ vector4_ಗಮ್ಯ_ಮೌಲ್ಯ(ಸ್ಥಿರಾಂಕ vvp_vector4_t&vec, int32_t&val,
			       ತಾರ್ಕಿಕ is_signed, ತಾರ್ಕಿಕ is_arithmetic);
ಮಾದರಿ ತಾರ್ಕಿಕ vector4_ಗಮ್ಯ_ಮೌಲ್ಯ(ಸ್ಥಿರಾಂಕ vvp_vector4_t&vec, int64_t&val,
			       ತಾರ್ಕಿಕ is_signed, ತಾರ್ಕಿಕ is_arithmetic);
ಮಾದರಿ ತಾರ್ಕಿಕ vector4_ಗಮ್ಯ_ಮೌಲ್ಯ(ಸ್ಥಿರಾಂಕ vvp_vector4_t&vec, uint8_t&val,
			       ತಾರ್ಕಿಕ is_signed, ತಾರ್ಕಿಕ is_arithmetic);
ಮಾದರಿ ತಾರ್ಕಿಕ vector4_ಗಮ್ಯ_ಮೌಲ್ಯ(ಸ್ಥಿರಾಂಕ vvp_vector4_t&vec, uint16_t&val,
			       ತಾರ್ಕಿಕ is_signed, ತಾರ್ಕಿಕ is_arithmetic);
ಮಾದರಿ ತಾರ್ಕಿಕ vector4_ಗಮ್ಯ_ಮೌಲ್ಯ(ಸ್ಥಿರಾಂಕ vvp_vector4_t&vec, uint32_t&val,
			       ತಾರ್ಕಿಕ is_signed, ತಾರ್ಕಿಕ is_arithmetic);
ಮಾದರಿ ತಾರ್ಕಿಕ vector4_ಗಮ್ಯ_ಮೌಲ್ಯ(ಸ್ಥಿರಾಂಕ vvp_vector4_t&vec, uint64_t&val,
			       ತಾರ್ಕಿಕ is_signed, ತಾರ್ಕಿಕ is_arithmetic);

ಮಾದರಿ <ವರ್ಗ T> ತಾರ್ಕಿಕ vector4_ಗಮ್ಯ_ಮೌಲ್ಯ(ಸ್ಥಿರಾಂಕ vvp_vector4_t&vec,
                                         ತಾರ್ಕಿಕ&ಅತಿಪ್ರವಾಹ_flag, T&val)
{
      T res = 0;
      T msk = 1;

      ಅತಿಪ್ರವಾಹ_flag = ಸುಳ್ಳು;
      ಚಿಹ್ನೆರಹಿತ size = vec.size();
      ಗಾಗಿ (ಚಿಹ್ನೆರಹಿತ idx = 0 ;  idx < size ;  idx += 1) {
	    ಆಯ್ಕೆ (vec.value(idx)) {
		ಸ್ಥಿತಿ BIT4_0:
		  ನಿಲ್ಲಿಸು;
		ಸ್ಥಿತಿ BIT4_1:
		  ಆದರೆ (msk == 0)
			ಅತಿಪ್ರವಾಹ_flag = ನಿಜ;
		  ಇಲ್ಲದಿದ್ದರೆ
			res |= msk;
		  ನಿಲ್ಲಿಸು;
		ಪೂರ್ವನಿಯೋಜಿತ:
		  ಹಿಂತಿರುಗಿ ಸುಳ್ಳು;
	    }

	    msk <<= static_cast<T>(1);
      }

      val = res;
      ಹಿಂತಿರುಗಿ ನಿಜ;
}

ಮಾದರಿ ತಾರ್ಕಿಕ vector4_ಗಮ್ಯ_ಮೌಲ್ಯ(ಸ್ಥಿರಾಂಕ vvp_vector4_t&vec, ತಾರ್ಕಿಕ&ಅತಿಪ್ರವಾಹ_flag,
                               ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ&val);
#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ UL_AND_TIME64_SAME
ಮಾದರಿ ತಾರ್ಕಿಕ vector4_ಗಮ್ಯ_ಮೌಲ್ಯ(ಸ್ಥಿರಾಂಕ vvp_vector4_t&vec, ತಾರ್ಕಿಕ&ಅತಿಪ್ರವಾಹ_flag,
                               vvp_time64_t&val);
#ಆದರೆ_ಅಂತ್ಯ

ತಾರ್ಕಿಕ vector4_ಗಮ್ಯ_ಮೌಲ್ಯ(ಸ್ಥಿರಾಂಕ vvp_vector4_t&vec, ದ್ವಿಗುಣ&val, ತಾರ್ಕಿಕ signed_flag)
{

      ಆದರೆ (vec.size() == 0) {
	    val = 0.0;
	    ಹಿಂತಿರುಗಿ ನಿಜ;
      }

      ತಾರ್ಕಿಕ flag = ನಿಜ;

      ಆದರೆ (vec.value(vec.size()-1) != BIT4_1) {
	    signed_flag = ಸುಳ್ಳು;
      }

      ದ್ವಿಗುಣ res = 0.0;
      ಆದರೆ (signed_flag) {
	    vvp_bit4_t ವಾಹಕ = BIT4_1;
	    ಗಾಗಿ (ಚಿಹ್ನೆರಹಿತ idx = 0 ;  idx < vec.size() ;  idx += 1) {
		  vvp_bit4_t a = ~vec.value(idx);
		  vvp_bit4_t x = ಕೂಡಿಸು_with_ವಾಹಕ(a, BIT4_0, ವಾಹಕ);
		  ಆಯ್ಕೆ (x) {
		      ಸ್ಥಿತಿ BIT4_0:
			ನಿಲ್ಲಿಸು;
		      ಸ್ಥಿತಿ BIT4_1:
			res += ಘಾತ(2.0, (ಪೂರ್ಣಾಂಕ)idx);
			ನಿಲ್ಲಿಸು;
		      ಪೂರ್ವನಿಯೋಜಿತ:
			flag = ಸುಳ್ಳು;
		  }
	    }
	    res *= -1.0;
      } ಇಲ್ಲದಿದ್ದರೆ {
	    ಗಾಗಿ (ಚಿಹ್ನೆರಹಿತ idx = 0 ;  idx < vec.size() ;  idx += 1) {
		  ಆಯ್ಕೆ (vec.value(idx)) {
		      ಸ್ಥಿತಿ BIT4_0:
			ನಿಲ್ಲಿಸು;
		      ಸ್ಥಿತಿ BIT4_1:
			res += ಘಾತ(2.0, (ಪೂರ್ಣಾಂಕ)idx);
			ನಿಲ್ಲಿಸು;
		      ಪೂರ್ವನಿಯೋಜಿತ:
			flag = ಸುಳ್ಳು;
		  }
	    }
      }
      val = res;
      ಹಿಂತಿರುಗಿ flag;
}

ತಾರ್ಕಿಕ vector2_ಗಮ್ಯ_ಮೌಲ್ಯ(ಸ್ಥಿರಾಂಕ vvp_vector2_t&a, int32_t&val, ತಾರ್ಕಿಕ is_signed)
{
      val = 0;
      ಚಿಹ್ನೆರಹಿತ idx;
      uint32_t ಮುಖವಾಡ;
      ಗಾಗಿ (idx = 0, ಮುಖವಾಡ = 1 ; idx < a.size() && idx < 32 ; idx += 1, ಮುಖವಾಡ <<= 1) {
	    ಆದರೆ (a.value(idx)) val |= ಮುಖವಾಡ;
      }

      ಆದರೆ (is_signed && a.size() < 32 && a.value(a.size()-1)) {
	    ಮುಖವಾಡ = -1;
	    ಮುಖವಾಡ <<= a.size();
	    val |= ಮುಖವಾಡ;
      }

      ಹಿಂತಿರುಗಿ a.size() <= 32;
}

vvp_vector4array_t::vvp_vector4array_t(ಚಿಹ್ನೆರಹಿತ ಅಗಲ_3, ಚಿಹ್ನೆರಹಿತ ಪದಗಳು_2)
: ಅಗಲ_2(ಅಗಲ_3), ಪದಗಳು_3(ಪದಗಳು_2)
{
}

vvp_vector4array_t::~vvp_vector4array_t()
{
}

ಶೂನ್ಯ vvp_vector4array_t::ಗಣ_ಶಬ್ದ_2(v4cell*cell, ಸ್ಥಿರಾಂಕ vvp_vector4_t&that)
{
      ಪ್ರತಿಜ್ಞಾ(that.ಗಾತ್ರ_3 == ಅಗಲ_2);

      ಆದರೆ (ಅಗಲ_2 <= vvp_vector4_t::ಬಿಟ್ಟುಗಳು_ಪ್ರತಿ_ಶಬ್ದ) {
	    cell->abits_val_ = that.abits_val_;
	    cell->bbits_val_ = that.bbits_val_;
	    ಹಿಂತಿರುಗಿ;
      }

      ಚಿಹ್ನೆರಹಿತ cnt = (ಅಗಲ_2 + vvp_vector4_t::ಬಿಟ್ಟುಗಳು_ಪ್ರತಿ_ಶಬ್ದ-1)/vvp_vector4_t::ಬಿಟ್ಟುಗಳು_ಪ್ರತಿ_ಶಬ್ದ;

      ಆದರೆ (cell->abits_ptr_ == 0) {
	    cell->abits_ptr_ = ಹೊಸ ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ[2*cnt];
	    cell->bbits_ptr_ = cell->abits_ptr_ + cnt;
      }

      ಗಾಗಿ (ಚಿಹ್ನೆರಹಿತ idx = 0 ; idx < cnt ; idx += 1)
	    cell->abits_ptr_[idx] = that.abits_ptr_[idx];
      ಗಾಗಿ (ಚಿಹ್ನೆರಹಿತ idx = 0 ; idx < cnt ; idx += 1)
	    cell->bbits_ptr_[idx] = that.bbits_ptr_[idx];
}

vvp_vector4_t vvp_vector4array_t::get_ಶಬ್ದ_2(v4cell*cell) ಸ್ಥಿರಾಂಕ
{
      ಆದರೆ (ಅಗಲ_2 <= vvp_vector4_t::ಬಿಟ್ಟುಗಳು_ಪ್ರತಿ_ಶಬ್ದ) {
	    vvp_vector4_t res;
	    res.ಗಾತ್ರ_3 = ಅಗಲ_2;
	    res.abits_val_ = cell->abits_val_;
	    res.bbits_val_ = cell->bbits_val_;
	    ಹಿಂತಿರುಗಿ res;
      }

      vvp_vector4_t res (ಅಗಲ_2, BIT4_X);
      ಆದರೆ (cell->abits_ptr_ == 0)
	    ಹಿಂತಿರುಗಿ res;

      ಚಿಹ್ನೆರಹಿತ cnt = (ಅಗಲ_2 + vvp_vector4_t::ಬಿಟ್ಟುಗಳು_ಪ್ರತಿ_ಶಬ್ದ-1)/vvp_vector4_t::ಬಿಟ್ಟುಗಳು_ಪ್ರತಿ_ಶಬ್ದ;

      ಗಾಗಿ (ಚಿಹ್ನೆರಹಿತ idx = 0 ; idx < cnt ; idx += 1)
	    res.abits_ptr_[idx] = cell->abits_ptr_[idx];
      ಗಾಗಿ (ಚಿಹ್ನೆರಹಿತ idx = 0 ; idx < cnt ; idx += 1)
	    res.bbits_ptr_[idx] = cell->bbits_ptr_[idx];

      ಹಿಂತಿರುಗಿ res;
}

vvp_vector4array_sa::vvp_vector4array_sa(ಚಿಹ್ನೆರಹಿತ ಅಗಲ_3, ಚಿಹ್ನೆರಹಿತ ಪದಗಳು_2)
: vvp_vector4array_t(ಅಗಲ_3, ಪದಗಳು_2)
{
      array_ = ಹೊಸ v4cell[ಪದಗಳು_3];

      ಆದರೆ (ಅಗಲ_2 <= vvp_vector4_t::ಬಿಟ್ಟುಗಳು_ಪ್ರತಿ_ಶಬ್ದ) {
	    ಗಾಗಿ (ಚಿಹ್ನೆರಹಿತ idx = 0 ; idx < ಪದಗಳು_3 ; idx += 1) {
		  array_[idx].abits_val_ = vvp_vector4_t::ಶಬ್ದ_X_ABITS;
		  array_[idx].bbits_val_ = vvp_vector4_t::ಶಬ್ದ_X_BBITS;
	    }
      } ಇಲ್ಲದಿದ್ದರೆ {
	    ಗಾಗಿ (ಚಿಹ್ನೆರಹಿತ idx = 0 ; idx < ಪದಗಳು_3 ; idx += 1) {
		  array_[idx].abits_ptr_ = 0;
		  array_[idx].bbits_ptr_ = 0;
	    }
      }
}

vvp_vector4array_sa::~vvp_vector4array_sa()
{
      ಆದರೆ (array_) {
	    ಆದರೆ (ಅಗಲ_2 > vvp_vector4_t::ಬಿಟ್ಟುಗಳು_ಪ್ರತಿ_ಶಬ್ದ) {
		  ಗಾಗಿ (ಚಿಹ್ನೆರಹಿತ idx = 0 ; idx < ಪದಗಳು_3 ; idx += 1)
			ಆದರೆ (array_[idx].abits_ptr_)
			      ಅಳಿಸು[]array_[idx].abits_ptr_;
	    }
	    ಅಳಿಸು[]array_;
      }
}

ಶೂನ್ಯ vvp_vector4array_sa::ಗಣ_ಶಬ್ದ(ಚಿಹ್ನೆರಹಿತ ಸೂಚ್ಯಂಕ, ಸ್ಥಿರಾಂಕ vvp_vector4_t&that)
{
      ಪ್ರತಿಜ್ಞಾ(ಸೂಚ್ಯಂಕ < ಪದಗಳು_3);

      v4cell*cell = &array_[ಸೂಚ್ಯಂಕ];

      ಗಣ_ಶಬ್ದ_2(cell, that);
}

vvp_vector4_t vvp_vector4array_sa::get_ಶಬ್ದ(ಚಿಹ್ನೆರಹಿತ ಸೂಚ್ಯಂಕ) ಸ್ಥಿರಾಂಕ
{
      ಆದರೆ (ಸೂಚ್ಯಂಕ >= ಪದಗಳು_3)
	    ಹಿಂತಿರುಗಿ vvp_vector4_t(ಅಗಲ_2, BIT4_X);

      ಪ್ರತಿಜ್ಞಾ(ಸೂಚ್ಯಂಕ < ಪದಗಳು_3);

      v4cell*cell = &array_[ಸೂಚ್ಯಂಕ];

      ಹಿಂತಿರುಗಿ get_ಶಬ್ದ_2(cell);
}

vvp_vector4array_aa::vvp_vector4array_aa(ಚಿಹ್ನೆರಹಿತ ಅಗಲ_3, ಚಿಹ್ನೆರಹಿತ ಪದಗಳು_2)
: vvp_vector4array_t(ಅಗಲ_3, ಪದಗಳು_2)
{
      context_idx_ = vpip_ಕೂಡಿಸು_ಅಂಶ_ಗಮ್ಯ_context(ಇದು, vpip_peek_context_scope());
}

vvp_vector4array_aa::~vvp_vector4array_aa()
{
}

ಶೂನ್ಯ vvp_vector4array_aa::alloc_instance(vvp_context_t context)
{
      v4cell*array = ಹೊಸ v4cell[ಪದಗಳು_3];

      ಆದರೆ (ಅಗಲ_2 <= vvp_vector4_t::ಬಿಟ್ಟುಗಳು_ಪ್ರತಿ_ಶಬ್ದ) {
	    ಗಾಗಿ (ಚಿಹ್ನೆರಹಿತ idx = 0 ; idx < ಪದಗಳು_3 ; idx += 1) {
		  array[idx].abits_val_ = vvp_vector4_t::ಶಬ್ದ_X_ABITS;
		  array[idx].bbits_val_ = vvp_vector4_t::ಶಬ್ದ_X_BBITS;
	    }
      } ಇಲ್ಲದಿದ್ದರೆ {
	    ಗಾಗಿ (ಚಿಹ್ನೆರಹಿತ idx = 0 ; idx < ಪದಗಳು_3 ; idx += 1) {
		  array[idx].abits_ptr_ = 0;
		  array[idx].bbits_ptr_ = 0;
	    }
      }

      vvp_ಗಣ_context_ಅಂಶ(context, context_idx_, array);
}

ಶೂನ್ಯ vvp_vector4array_aa::reset_instance(vvp_context_t context)
{
      v4cell*cell = static_cast<v4cell*>
            (vvp_get_context_ಅಂಶ(context, context_idx_));

      ಆದರೆ (ಅಗಲ_2 <= vvp_vector4_t::ಬಿಟ್ಟುಗಳು_ಪ್ರತಿ_ಶಬ್ದ) {
	    ಗಾಗಿ (ಚಿಹ್ನೆರಹಿತ idx = 0 ; idx < ಪದಗಳು_3 ; idx += 1) {
		  cell->abits_val_ = vvp_vector4_t::ಶಬ್ದ_X_ABITS;
		  cell->bbits_val_ = vvp_vector4_t::ಶಬ್ದ_X_BBITS;
                  cell++;
	    }
      } ಇಲ್ಲದಿದ್ದರೆ {
            ಚಿಹ್ನೆರಹಿತ cnt = (ಅಗಲ_2 + vvp_vector4_t::ಬಿಟ್ಟುಗಳು_ಪ್ರತಿ_ಶಬ್ದ-1)/vvp_vector4_t::ಬಿಟ್ಟುಗಳು_ಪ್ರತಿ_ಶಬ್ದ;
	    ಗಾಗಿ (ಚಿಹ್ನೆರಹಿತ idx = 0 ; idx < ಪದಗಳು_3 ; idx += 1) {
		  ಆದರೆ (cell->abits_ptr_) {
                        ಗಾಗಿ (ಚಿಹ್ನೆರಹಿತ n = 0 ; n < cnt ; n += 1) {
			      cell->abits_ptr_[n] = vvp_vector4_t::ಶಬ್ದ_X_ABITS;
			      cell->bbits_ptr_[n] = vvp_vector4_t::ಶಬ್ದ_X_BBITS;
                        }
                  }
                  cell++;
	    }
      }
}

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ CHECK_WITH_VALGRIND
ಶೂನ್ಯ vvp_vector4array_aa::free_instance(vvp_context_t context)
{
      v4cell*cell = static_cast<v4cell*>
            (vvp_get_context_ಅಂಶ(context, context_idx_));
      ಅಳಿಸು [] cell;
}
#ಆದರೆ_ಅಂತ್ಯ

ಶೂನ್ಯ vvp_vector4array_aa::ಗಣ_ಶಬ್ದ(ಚಿಹ್ನೆರಹಿತ ಸೂಚ್ಯಂಕ, ಸ್ಥಿರಾಂಕ vvp_vector4_t&that)
{
      ಪ್ರತಿಜ್ಞಾ(ಸೂಚ್ಯಂಕ < ಪದಗಳು_3);

      v4cell*cell = static_cast<v4cell*>
            (vthread_get_wt_context_ಅಂಶ(context_idx_)) + ಸೂಚ್ಯಂಕ;

      ಗಣ_ಶಬ್ದ_2(cell, that);
}

vvp_vector4_t vvp_vector4array_aa::get_ಶಬ್ದ(ಚಿಹ್ನೆರಹಿತ ಸೂಚ್ಯಂಕ) ಸ್ಥಿರಾಂಕ
{
      ಆದರೆ (ಸೂಚ್ಯಂಕ >= ಪದಗಳು_3)
	    ಹಿಂತಿರುಗಿ vvp_vector4_t(ಅಗಲ_2, BIT4_X);

      ಪ್ರತಿಜ್ಞಾ(ಸೂಚ್ಯಂಕ < ಪದಗಳು_3);

      v4cell*cell = static_cast<v4cell*>
            (vthread_get_rd_context_ಅಂಶ(context_idx_)) + ಸೂಚ್ಯಂಕ;

      ಹಿಂತಿರುಗಿ get_ಶಬ್ದ_2(cell);
}

vvp_vector2_t::vvp_vector2_t()
{
      vec_ = 0;
      wid_ = 0;
}

vvp_vector2_t::vvp_vector2_t(uint64_t v, ಚಿಹ್ನೆರಹಿತ wid)
{
      wid_ = wid;
      ಸ್ಥಿರಾಂಕ ಚಿಹ್ನೆರಹಿತ ಬಿಟ್ಟುಗಳು_ಪ್ರತಿ_ಶಬ್ದ_2 = 8 * ಗಾತ್ರ(vec_[0]);
      ಸ್ಥಿರಾಂಕ ಚಿಹ್ನೆರಹಿತ ಪದಗಳು = (wid_ + ಬಿಟ್ಟುಗಳು_ಪ್ರತಿ_ಶಬ್ದ_2-1) / ಬಿಟ್ಟುಗಳು_ಪ್ರತಿ_ಶಬ್ದ_2;

      vec_ = ಹೊಸ ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ[ಪದಗಳು];
      ಗಾಗಿ (ಚಿಹ್ನೆರಹಿತ idx = 0 ; idx < ಪದಗಳು ; idx += 1) {
	    ಸ್ಥಿರಾಂಕ ಚಿಹ್ನೆರಹಿತ ಸರಿಸು = idx * ಬಿಟ್ಟುಗಳು_ಪ್ರತಿ_ಶಬ್ದ_2;
	    vec_[idx] = ಸರಿಸು < 64 ? v >> ಸರಿಸು : 0;
      }
}

vvp_vector2_t::vvp_vector2_t(vvp_vector2_t::fill_t fill, ಚಿಹ್ನೆರಹಿತ wid)
{
      wid_ = wid;
      ಸ್ಥಿರಾಂಕ ಚಿಹ್ನೆರಹಿತ ಬಿಟ್ಟುಗಳು_ಪ್ರತಿ_ಶಬ್ದ_2 = 8 * ಗಾತ್ರ(vec_[0]);
      ಸ್ಥಿರಾಂಕ ಚಿಹ್ನೆರಹಿತ ಪದಗಳು = (wid_ + ಬಿಟ್ಟುಗಳು_ಪ್ರತಿ_ಶಬ್ದ_2-1) / ಬಿಟ್ಟುಗಳು_ಪ್ರತಿ_ಶಬ್ದ_2;

      vec_ = ಹೊಸ ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ[ಪದಗಳು];
      ಗಾಗಿ (ಚಿಹ್ನೆರಹಿತ idx = 0 ;  idx < ಪದಗಳು ;  idx += 1)
	    vec_[idx] = fill? -1 : 0;
}

vvp_vector2_t::vvp_vector2_t(ಸ್ಥಿರಾಂಕ vvp_vector2_t&that, ಚಿಹ್ನೆರಹಿತ ತಳ, ಚಿಹ್ನೆರಹಿತ wid)
{
      wid_ = wid;
      ಸ್ಥಿರಾಂಕ ಚಿಹ್ನೆರಹಿತ ಪದಗಳು = (wid_ + ಬಿಟ್ಟುಗಳು_ಪ್ರತಿ_ಶಬ್ದ-1) / ಬಿಟ್ಟುಗಳು_ಪ್ರತಿ_ಶಬ್ದ;

      vec_ = ಹೊಸ ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ[ಪದಗಳು];

      ಗಾಗಿ (ಚಿಹ್ನೆರಹಿತ idx = 0 ; idx < wid ; idx += 1) {
	    ಪೂರ್ಣಾಂಕ ಬಿಟ್ = that.value(ತಳ+idx);
	    ಆದರೆ (ಬಿಟ್ == 0)
		  ಮುಂದುವರಿ;
	    ಚಿಹ್ನೆರಹಿತ ಶಬ್ದ = idx / ಬಿಟ್ಟುಗಳು_ಪ್ರತಿ_ಶಬ್ದ;
	    ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ ಮುಖವಾಡ = 1UL << (idx % ಬಿಟ್ಟುಗಳು_ಪ್ರತಿ_ಶಬ್ದ);
	    vec_[ಶಬ್ದ] |= ಮುಖವಾಡ;
      }
}

ಶೂನ್ಯ vvp_vector2_t::copy_from_that_(ಸ್ಥಿರಾಂಕ vvp_vector4_t&that)
{
      wid_ = that.size();
      ಸ್ಥಿರಾಂಕ ಚಿಹ್ನೆರಹಿತ ಪದಗಳು = (that.size() + ಬಿಟ್ಟುಗಳು_ಪ್ರತಿ_ಶಬ್ದ-1) / ಬಿಟ್ಟುಗಳು_ಪ್ರತಿ_ಶಬ್ದ;

      ಆದರೆ (ಪದಗಳು == 0) {
	    vec_ = 0;
	    wid_ = 0;
	    ಹಿಂತಿರುಗಿ;
      }

	// Use the subarray method with the xz_to_0 flag set so that
	// we get values even when there are xz bits.
      vec_ = that.subarray(0, wid_, ನಿಜ);
}

ಶೂನ್ಯ vvp_vector2_t::copy_from_that_(ಸ್ಥಿರಾಂಕ vvp_vector2_t&that)
{
      wid_ = that.wid_;
      ಸ್ಥಿರಾಂಕ ಚಿಹ್ನೆರಹಿತ ಪದಗಳು = (wid_ + ಬಿಟ್ಟುಗಳು_ಪ್ರತಿ_ಶಬ್ದ-1) / ಬಿಟ್ಟುಗಳು_ಪ್ರತಿ_ಶಬ್ದ;

      ಆದರೆ (ಪದಗಳು == 0) {
	    vec_ = 0;
	    wid_ = 0;
	    ಹಿಂತಿರುಗಿ;
      }

      vec_ = ಹೊಸ ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ[ಪದಗಳು];
      ಗಾಗಿ (ಚಿಹ್ನೆರಹಿತ idx = 0 ;  idx < ಪದಗಳು ;  idx += 1)
	    vec_[idx] = that.vec_[idx];
}

vvp_vector2_t::vvp_vector2_t(ಸ್ಥಿರಾಂಕ vvp_vector2_t&that, ಚಿಹ್ನೆರಹಿತ newsize)
{
      wid_ = newsize;
      ಆದರೆ (newsize == 0) {
	    vec_ = 0;
	    ಹಿಂತಿರುಗಿ;
      }

      ಸ್ಥಿರಾಂಕ ಚಿಹ್ನೆರಹಿತ ಪದಗಳು = (wid_ + ಬಿಟ್ಟುಗಳು_ಪ್ರತಿ_ಶಬ್ದ-1) / ಬಿಟ್ಟುಗಳು_ಪ್ರತಿ_ಶಬ್ದ;
      ಸ್ಥಿರಾಂಕ ಚಿಹ್ನೆರಹಿತ twords = (that.wid_ + ಬಿಟ್ಟುಗಳು_ಪ್ರತಿ_ಶಬ್ದ-1) / ಬಿಟ್ಟುಗಳು_ಪ್ರತಿ_ಶಬ್ದ;

      vec_ = ಹೊಸ ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ[ಪದಗಳು];
      ಗಾಗಿ (ಚಿಹ್ನೆರಹಿತ idx = 0 ;  idx < ಪದಗಳು ;  idx += 1) {
	    ಆದರೆ (idx < twords)
		  vec_[idx] = that.vec_[idx];
	    ಇಲ್ಲದಿದ್ದರೆ
		  vec_[idx] = 0;
      }
}

vvp_vector2_t& vvp_vector2_t::ಕ್ರಿಯಾಕಾರಕ= (ಸ್ಥಿರಾಂಕ vvp_vector2_t&that)
{
      ಆದರೆ (ಇದು == &that)
	    ಹಿಂತಿರುಗಿ *ಇದು;

      ಅಳಿಸು[] vec_;
      vec_ = 0;

      copy_from_that_(that);
      ಹಿಂತಿರುಗಿ *ಇದು;
}

vvp_vector2_t& vvp_vector2_t::ಕ್ರಿಯಾಕಾರಕ= (ಸ್ಥಿರಾಂಕ vvp_vector4_t&that)
{
      ಅಳಿಸು[]vec_;
      vec_ = 0;
      copy_from_that_(that);
      ಹಿಂತಿರುಗಿ *ಇದು;
}

vvp_vector2_t& vvp_vector2_t::ಕ್ರಿಯಾಕಾರಕ <<= (ಚಿಹ್ನೆರಹಿತ ಪೂರ್ಣಾಂಕ ಸರಿಸು)
{
      ಆದರೆ (wid_ == 0)
	    ಹಿಂತಿರುಗಿ *ಇದು;

      ಸ್ಥಿರಾಂಕ ಚಿಹ್ನೆರಹಿತ ಪದಗಳು = (wid_ + ಬಿಟ್ಟುಗಳು_ಪ್ರತಿ_ಶಬ್ದ-1) / ಬಿಟ್ಟುಗಳು_ಪ್ರತಿ_ಶಬ್ದ;

	// Number of words to shift
      ಸ್ಥಿರಾಂಕ ಚಿಹ್ನೆರಹಿತ wshift = ಸರಿಸು / ಬಿಟ್ಟುಗಳು_ಪ್ರತಿ_ಶಬ್ದ;
	// bits to shift within each word.
      ಸ್ಥಿರಾಂಕ ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ oshift = ಸರಿಸು % ಬಿಟ್ಟುಗಳು_ಪ್ರತಿ_ಶಬ್ದ;

	// If shifting the entire value away, then return zeros.
      ಆದರೆ (wshift >= ಪದಗಳು) {
	    ಗಾಗಿ (ಚಿಹ್ನೆರಹಿತ idx = 0 ;  idx < ಪದಗಳು ;  idx += 1)
		  vec_[idx] = 0;

	    ಹಿಂತಿರುಗಿ *ಇದು;
      }

	// Do the word shift first.
      ಆದರೆ (wshift > 0) {
	    ಗಾಗಿ (ಚಿಹ್ನೆರಹಿತ idx = 0 ;  idx < ಪದಗಳು-wshift ;  idx += 1) {
		  ಚಿಹ್ನೆರಹಿತ sel = ಪದಗಳು - idx - 1;
		  vec_[sel] = vec_[sel-wshift];
	    }

	    ಗಾಗಿ (ಚಿಹ್ನೆರಹಿತ idx = 0 ;  idx < wshift ;  idx += 1)
		  vec_[idx] = 0;
      }

	// Do the fine shift.
      ಆದರೆ (oshift != 0) {
	    ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ pad = 0;
	    ಗಾಗಿ (ಚಿಹ್ನೆರಹಿತ idx = 0 ;  idx < ಪದಗಳು ;  idx += 1) {
		  ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ ಮುಂದಿನ_pad = vec_[idx] >> (ಬಿಟ್ಟುಗಳು_ಪ್ರತಿ_ಶಬ್ದ-oshift);
		  vec_[idx] = (vec_[idx] << oshift) | pad;
		  pad = ಮುಂದಿನ_pad;
	    }

	      // Cleanup the tail bits.
	    ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ ಮುಖವಾಡ = -1UL >> (ಬಿಟ್ಟುಗಳು_ಪ್ರತಿ_ಶಬ್ದ - wid_%ಬಿಟ್ಟುಗಳು_ಪ್ರತಿ_ಶಬ್ದ);
	    vec_[ಪದಗಳು-1] &= ಮುಖವಾಡ;
      }

      ಹಿಂತಿರುಗಿ *ಇದು;
}

vvp_vector2_t& vvp_vector2_t::ಕ್ರಿಯಾಕಾರಕ >>= (ಚಿಹ್ನೆರಹಿತ ಸರಿಸು)
{
      ಆದರೆ (wid_ == 0)
	    ಹಿಂತಿರುಗಿ *ಇದು;

      ಸ್ಥಿರಾಂಕ ಚಿಹ್ನೆರಹಿತ ಪದಗಳು = (wid_ + ಬಿಟ್ಟುಗಳು_ಪ್ರತಿ_ಶಬ್ದ-1) / ಬಿಟ್ಟುಗಳು_ಪ್ರತಿ_ಶಬ್ದ;

	// Number of words to shift
      ಸ್ಥಿರಾಂಕ ಚಿಹ್ನೆರಹಿತ wshift = ಸರಿಸು / ಬಿಟ್ಟುಗಳು_ಪ್ರತಿ_ಶಬ್ದ;
	// bits to shift within each word.
      ಸ್ಥಿರಾಂಕ ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ oshift = ಸರಿಸು % ಬಿಟ್ಟುಗಳು_ಪ್ರತಿ_ಶಬ್ದ;

	// If shifting the entire value away, then return zeros.
      ಆದರೆ (wshift >= ಪದಗಳು) {
	    ಗಾಗಿ (ಚಿಹ್ನೆರಹಿತ idx = 0 ;  idx < ಪದಗಳು ;  idx += 1)
		  vec_[idx] = 0;

	    ಹಿಂತಿರುಗಿ *ಇದು;
      }

      ಆದರೆ (wshift > 0) {
	    ಗಾಗಿ (ಚಿಹ್ನೆರಹಿತ idx = 0 ;  idx < ಪದಗಳು-wshift ;  idx += 1)
		  vec_[idx] = vec_[idx+wshift];

	    ಗಾಗಿ (ಚಿಹ್ನೆರಹಿತ idx = ಪದಗಳು-wshift ;  idx < ಪದಗಳು ;  idx += 1)
		  vec_[idx] = 0;
      }

      ಆದರೆ (oshift > 0) {
	    ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ pad = 0;
	    ಗಾಗಿ (ಚಿಹ್ನೆರಹಿತ idx = ಪದಗಳು ;  idx > 0 ;  idx -= 1) {
		  ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ new_pad = vec_[idx-1] <<(ಬಿಟ್ಟುಗಳು_ಪ್ರತಿ_ಶಬ್ದ-oshift);
		  vec_[idx-1] = pad | (vec_[idx-1] >> oshift);
		  pad = new_pad;
	    }

	      // Cleanup the tail bits.

	    ಚಿಹ್ನೆರಹಿತ use_ಪದಗಳು = ಪದಗಳು;
	      // Mask_shift is the number of high bits of the top word
	      // that are to be masked off. We start with the number
	      // of bits that are not included even in the original,
	      // then we include the bits of the shift, that are to be
	      // masked to zero.
	    ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ ಮುಖವಾಡ_ಸರಿಸು = ಬಿಟ್ಟುಗಳು_ಪ್ರತಿ_ಶಬ್ದ - wid_%ಬಿಟ್ಟುಗಳು_ಪ್ರತಿ_ಶಬ್ದ;
	    ಮುಖವಾಡ_ಸರಿಸು %= ಬಿಟ್ಟುಗಳು_ಪ್ರತಿ_ಶಬ್ದ;
	    ಮುಖವಾಡ_ಸರಿಸು += oshift;
	    ಆಗ (ಮುಖವಾಡ_ಸರಿಸು >= ಬಿಟ್ಟುಗಳು_ಪ್ರತಿ_ಶಬ್ದ) {
		  vec_[use_ಪದಗಳು-1] = 0;
		  use_ಪದಗಳು -= 1;
		  ಮುಖವಾಡ_ಸರಿಸು -= ಬಿಟ್ಟುಗಳು_ಪ್ರತಿ_ಶಬ್ದ;
	    }
	    ಆದರೆ (ಮುಖವಾಡ_ಸರಿಸು > 0) {
		  ಪ್ರತಿಜ್ಞಾ(use_ಪದಗಳು > 0);
		  ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ ಮುಖವಾಡ = -1UL >> ಮುಖವಾಡ_ಸರಿಸು;
		  vec_[use_ಪದಗಳು-1] &= ಮುಖವಾಡ;
	    }
      }

      ಹಿಂತಿರುಗಿ *ಇದು;
}

ಸ್ಥಿರ ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ ಕೂಡಿಸು_ವಾಹಕ(ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ a, ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ b,
			       ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ&ವಾಹಕ)
{
      ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ out = ವಾಹಕ;
      ವಾಹಕ = 0;

      ಆದರೆ ((ULONG_MAX - out) < a)
	    ವಾಹಕ += 1;
      out += a;

      ಆದರೆ ((ULONG_MAX - out) < b)
	    ವಾಹಕ += 1;
      out += b;

      ಹಿಂತಿರುಗಿ out;
}

vvp_vector2_t& vvp_vector2_t::ಕ್ರಿಯಾಕಾರಕ += (ಸ್ಥಿರಾಂಕ vvp_vector2_t&that)
{
      ಪ್ರತಿಜ್ಞಾ(wid_ == that.wid_);
      ಆದರೆ (wid_ == 0)
	    ಹಿಂತಿರುಗಿ *ಇದು;

      ಸ್ಥಿರಾಂಕ ಚಿಹ್ನೆರಹಿತ ಪದಗಳು = (wid_ + ಬಿಟ್ಟುಗಳು_ಪ್ರತಿ_ಶಬ್ದ-1) / ಬಿಟ್ಟುಗಳು_ಪ್ರತಿ_ಶಬ್ದ;

      ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ ವಾಹಕ = 0;
      ಗಾಗಿ (ಚಿಹ್ನೆರಹಿತ idx = 0 ;  idx < ಪದಗಳು ;  idx += 1) {
	    vec_[idx] = ಕೂಡಿಸು_ವಾಹಕ(vec_[idx], that.vec_[idx], ವಾಹಕ);
      }


	// Cleanup the tail bits.
      ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ ಮುಖವಾಡ = -1UL >> (ಬಿಟ್ಟುಗಳು_ಪ್ರತಿ_ಶಬ್ದ - wid_%ಬಿಟ್ಟುಗಳು_ಪ್ರತಿ_ಶಬ್ದ);
      vec_[ಪದಗಳು-1] &= ಮುಖವಾಡ;

      ಹಿಂತಿರುಗಿ *ಇದು;
}

vvp_vector2_t& vvp_vector2_t::ಕ್ರಿಯಾಕಾರಕ -= (ಸ್ಥಿರಾಂಕ vvp_vector2_t&that)
{
      ಪ್ರತಿಜ್ಞಾ(wid_ == that.wid_);
      ಆದರೆ (wid_ == 0)
	    ಹಿಂತಿರುಗಿ *ಇದು;

      ಸ್ಥಿರಾಂಕ ಚಿಹ್ನೆರಹಿತ ಪದಗಳು = (wid_ + ಬಿಟ್ಟುಗಳು_ಪ್ರತಿ_ಶಬ್ದ-1) / ಬಿಟ್ಟುಗಳು_ಪ್ರತಿ_ಶಬ್ದ;

      ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ ವಾಹಕ = 1;
      ಗಾಗಿ (ಚಿಹ್ನೆರಹಿತ idx = 0 ;  idx < ಪದಗಳು ;  idx += 1) {
	    vec_[idx] = ಕೂಡಿಸು_ವಾಹಕ(vec_[idx], ~that.vec_[idx], ವಾಹಕ);
      }

      ಹಿಂತಿರುಗಿ *ಇದು;
}

ಶೂನ್ಯ vvp_vector2_t::trim()
{
      ಆಗ (value(wid_-1) == 0 && wid_ > 1) wid_ -= 1;
}

/* This is a special trim that is used on numbers we know represent a
 * negative signed value (they came from a negative real value). */
ಶೂನ್ಯ vvp_vector2_t::trim_neg()
{
      ಆದರೆ (value(wid_-1) == 1 && wid_ > 32) {
	    ಆಗ (value(wid_-2) == 1 && wid_ > 32) wid_ -= 1;
      }
}

ಪೂರ್ಣಾಂಕ vvp_vector2_t::value(ಚಿಹ್ನೆರಹಿತ idx) ಸ್ಥಿರಾಂಕ
{
      ಆದರೆ (idx >= wid_)
	    ಹಿಂತಿರುಗಿ 0;

      ಸ್ಥಿರಾಂಕ ಚಿಹ್ನೆರಹಿತ ಬಿಟ್ಟುಗಳು_ಪ್ರತಿ_ಶಬ್ದ_2 = 8 * ಗಾತ್ರ(vec_[0]);
      ಚಿಹ್ನೆರಹಿತ addr = idx/ಬಿಟ್ಟುಗಳು_ಪ್ರತಿ_ಶಬ್ದ_2;
      ಚಿಹ್ನೆರಹಿತ ಮುಖವಾಡ = idx%ಬಿಟ್ಟುಗಳು_ಪ್ರತಿ_ಶಬ್ದ_2;

      ಆದರೆ (vec_[addr] & (1UL<<ಮುಖವಾಡ))
	    ಹಿಂತಿರುಗಿ 1;
      ಇಲ್ಲದಿದ್ದರೆ
	    ಹಿಂತಿರುಗಿ 0;
}

ಶೂನ್ಯ vvp_vector2_t::ಗಣ_ಬಿಟ್(ಚಿಹ್ನೆರಹಿತ idx, ಪೂರ್ಣಾಂಕ ಬಿಟ್)
{
      ಪ್ರತಿಜ್ಞಾ(idx < wid_);

      ಸ್ಥಿರಾಂಕ ಚಿಹ್ನೆರಹಿತ ಬಿಟ್ಟುಗಳು_ಪ್ರತಿ_ಶಬ್ದ_2 = 8 * ಗಾತ್ರ(vec_[0]);
      ಚಿಹ್ನೆರಹಿತ addr = idx/ಬಿಟ್ಟುಗಳು_ಪ್ರತಿ_ಶಬ್ದ_2;
      ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ ಮುಖವಾಡ = idx%ಬಿಟ್ಟುಗಳು_ಪ್ರತಿ_ಶಬ್ದ_2;

      ಆದರೆ (ಬಿಟ್)
	    vec_[addr] |= 1UL << ಮುಖವಾಡ;
      ಇಲ್ಲದಿದ್ದರೆ
	    vec_[addr] &= ~(1UL << ಮುಖವಾಡ);
}

ಶೂನ್ಯ vvp_vector2_t::ಗಣ_vec(ಚಿಹ್ನೆರಹಿತ adr, ಸ್ಥಿರಾಂಕ vvp_vector2_t&that)
{
      ಪ್ರತಿಜ್ಞಾ((adr + that.wid_) <= wid_);

      ಗಾಗಿ (ಚಿಹ್ನೆರಹಿತ idx = 0 ; idx < that.wid_ ; idx += 1)
	    ಗಣ_ಬಿಟ್(adr+idx, that.value(idx));
}

ತಾರ್ಕಿಕ vvp_vector2_t::is_NaN() ಸ್ಥಿರಾಂಕ
{
      ಹಿಂತಿರುಗಿ wid_ == 0;
}

ತಾರ್ಕಿಕ vvp_vector2_t::is_zero() ಸ್ಥಿರಾಂಕ
{
      ಸ್ಥಿರಾಂಕ ಚಿಹ್ನೆರಹಿತ ಪದಗಳು = (wid_ + ಬಿಟ್ಟುಗಳು_ಪ್ರತಿ_ಶಬ್ದ-1) / ಬಿಟ್ಟುಗಳು_ಪ್ರತಿ_ಶಬ್ದ;

      ಗಾಗಿ (ಚಿಹ್ನೆರಹಿತ idx = 0; idx < ಪದಗಳು; idx += 1) {
	    ಆದರೆ (vec_[idx] == 0) ಮುಂದುವರಿ;
	    ಹಿಂತಿರುಗಿ ಸುಳ್ಳು;
      }

      ಹಿಂತಿರುಗಿ ನಿಜ;
}

/*
 * Basic idea from "Introduction to Programming using SML" by
 * Michael R. Hansen and Hans Rischel page 261 and "Seminumerical
 * Algorithms, Third Edition" by Donald E. Knuth section 4.6.3.
 */
vvp_vector2_t ಘಾತ(ಸ್ಥಿರಾಂಕ vvp_vector2_t&x, vvp_vector2_t&y)
{
        /* If we have a zero exponent just return 1. */
      ಆದರೆ (y == vvp_vector2_t(0L, 1)) {
	    ಹಿಂತಿರುಗಿ vvp_vector2_t(1L, x.size());
      }

        /* Is the value odd? */
      ಆದರೆ (y.value(0) == 1) {
	    y.ಗಣ_ಬಿಟ್(0, 0);  // A quick subtract by 1.
	    vvp_vector2_t res = x * ಘಾತ(x, y);
	    ಹಿಂತಿರುಗಿ res;
      }

      y >>= 1;  // A fast divide by two. We know the LSB is zero.
      vvp_vector2_t z = ಘಾತ(x, y);
      vvp_vector2_t res = z * z;
      ಹಿಂತಿರುಗಿ res;
}

ಸ್ಥಿರ ಶೂನ್ಯ multiply_long(ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ a, ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ b,
			  ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ&ಕೆಳಗಿನ, ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ&ಮೇಲಿನ)
{
      ಪ್ರತಿಜ್ಞಾ(ಗಾತ್ರ(ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ) %2 == 0);

      ಸ್ಥಿರಾಂಕ ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ ಶಬ್ದ_ಮುಖವಾಡ = (1UL << 4UL*ಗಾತ್ರ(a)) - 1UL;
      ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ tmpa;
      ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ tmpb;
      ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ res[4];

      tmpa = a & ಶಬ್ದ_ಮುಖವಾಡ;
      tmpb = b & ಶಬ್ದ_ಮುಖವಾಡ;
      res[0] = tmpa * tmpb;
      res[1] = res[0] >> 4UL*ಗಾತ್ರ(ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ);
      res[0] &= ಶಬ್ದ_ಮುಖವಾಡ;

      tmpa = (a >> 4UL*ಗಾತ್ರ(ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ)) & ಶಬ್ದ_ಮುಖವಾಡ;
      res[1] += tmpa * tmpb;
      res[2] = res[1] >> 4UL*ಗಾತ್ರ(ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ);
      res[1] &= ಶಬ್ದ_ಮುಖವಾಡ;

      tmpa = a & ಶಬ್ದ_ಮುಖವಾಡ;
      tmpb = (b >> 4UL*ಗಾತ್ರ(ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ)) & ಶಬ್ದ_ಮುಖವಾಡ;
      res[1] += tmpa * tmpb;
      res[2] += res[1] >> 4UL*ಗಾತ್ರ(ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ);
      res[3]  = res[2] >> 4UL*ಗಾತ್ರ(ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ);
      res[1] &= ಶಬ್ದ_ಮುಖವಾಡ;
      res[2] &= ಶಬ್ದ_ಮುಖವಾಡ;

      tmpa = (a >> 4UL*ಗಾತ್ರ(ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ)) & ಶಬ್ದ_ಮುಖವಾಡ;
      res[2] += tmpa * tmpb;
      res[3] += res[2] >> 4UL*ಗಾತ್ರ(ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ);
      res[2] &= ಶಬ್ದ_ಮುಖವಾಡ;

      ಮೇಲಿನ = (res[3] << 4UL*ಗಾತ್ರ(ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ)) | res[2];
      ಕೆಳಗಿನ  = (res[1] << 4UL*ಗಾತ್ರ(ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ)) | res[0];
}

vvp_vector2_t ಕ್ರಿಯಾಕಾರಕ * (ಸ್ಥಿರಾಂಕ vvp_vector2_t&a, ಸ್ಥಿರಾಂಕ vvp_vector2_t&b)
{
      ಸ್ಥಿರಾಂಕ ಚಿಹ್ನೆರಹಿತ ಬಿಟ್ಟುಗಳು_ಪ್ರತಿ_ಶಬ್ದ_2 = 8 * ಗಾತ್ರ(a.vec_[0]);

	// The compiler ensures that the two operands are of equal size.
      ಪ್ರತಿಜ್ಞಾ(a.size() == b.size());
      vvp_vector2_t r (0, a.size());

      ಚಿಹ್ನೆರಹಿತ ಪದಗಳು = (r.wid_ + ಬಿಟ್ಟುಗಳು_ಪ್ರತಿ_ಶಬ್ದ_2 - 1) / ಬಿಟ್ಟುಗಳು_ಪ್ರತಿ_ಶಬ್ದ_2;

      ಗಾಗಿ (ಚಿಹ್ನೆರಹಿತ bdx = 0 ;  bdx < ಪದಗಳು ;  bdx += 1) {
	    ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ tmpb = b.vec_[bdx];
	    ಆದರೆ (tmpb == 0)
		  ಮುಂದುವರಿ;

	    ಗಾಗಿ (ಚಿಹ್ನೆರಹಿತ adx = 0 ;  adx < ಪದಗಳು ;  adx += 1) {
		  ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ tmpa = a.vec_[adx];
		  ಆದರೆ (tmpa == 0)
			ಮುಂದುವರಿ;

		  ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ ಕೆಳಗಿನ, hig;
		  multiply_long(tmpa, tmpb, ಕೆಳಗಿನ, hig);

		  ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ ವಾಹಕ = 0;
		  ಗಾಗಿ (ಚಿಹ್ನೆರಹಿತ sdx = 0
			     ; (adx+bdx+sdx) < ಪದಗಳು
			     ;  sdx += 1) {

			r.vec_[adx+bdx+sdx] = ಕೂಡಿಸು_ವಾಹಕ(r.vec_[adx+bdx+sdx],
							ಕೆಳಗಿನ, ವಾಹಕ);
			ಕೆಳಗಿನ = hig;
			hig = 0;
		  }
	    }
      }

      ಹಿಂತಿರುಗಿ r;
}

ಸ್ಥಿರ ಶೂನ್ಯ div_mod (vvp_vector2_t dividend, vvp_vector2_t ಭಾಜಕ,
		     vvp_vector2_t&quotient, vvp_vector2_t&remainder)
{

      quotient = vvp_vector2_t(0, dividend.size());

      ಆದರೆ (ಭಾಜಕ == quotient) {
	    cerr << "ERROR: division by zero, exiting." << ಸಾಲು_ಅಂತ್ಯ;
	    ನಿರ್ಗಮಿಸು(255);
      }

      ಆದರೆ (dividend < ಭಾಜಕ) {
	    remainder = dividend;
	    ಹಿಂತಿರುಗಿ;
      }

      vvp_vector2_t ಮುಖವಾಡ (1, dividend.size());

	// Make the dividend 1 bit larger to prevent overflow of
	// divtmp in startup.
      dividend = vvp_vector2_t(dividend, dividend.size()+1);
      vvp_vector2_t divtmp (ಭಾಜಕ, dividend.size());

      ಆಗ (divtmp < dividend) {
	    divtmp <<= 1;
	    ಮುಖವಾಡ <<= 1;
      }

      ಆಗ (dividend >= ಭಾಜಕ) {
	    ಆದರೆ (divtmp <= dividend) {
		  dividend -= divtmp;
		  quotient += ಮುಖವಾಡ;
	    }

	    divtmp >>= 1;
	    ಮುಖವಾಡ >>= 1;
      }

      remainder = vvp_vector2_t(dividend, ಮುಖವಾಡ.size());
}

vvp_vector2_t ಕ್ರಿಯಾಕಾರಕ - (ಸ್ಥಿರಾಂಕ vvp_vector2_t&that)
{
      vvp_vector2_t neg(that);
      ಆದರೆ (neg.wid_ == 0) ಹಿಂತಿರುಗಿ neg;

      ಸ್ಥಿರಾಂಕ ಚಿಹ್ನೆರಹಿತ ಪದಗಳು = (neg.wid_ + neg.ಬಿಟ್ಟುಗಳು_ಪ್ರತಿ_ಶಬ್ದ-1) /
                                         neg.ಬಿಟ್ಟುಗಳು_ಪ್ರತಿ_ಶಬ್ದ;
      ಗಾಗಿ (ಚಿಹ್ನೆರಹಿತ idx = 0 ;  idx < ಪದಗಳು ;  idx += 1) {
	    neg.vec_[idx] = ~neg.vec_[idx];
      }
      neg += vvp_vector2_t(1, neg.wid_);

      ಹಿಂತಿರುಗಿ neg;
}

vvp_vector2_t ಕ್ರಿಯಾಕಾರಕ / (ಸ್ಥಿರಾಂಕ vvp_vector2_t&dividend,
			  ಸ್ಥಿರಾಂಕ vvp_vector2_t&ಭಾಜಕ)
{
      vvp_vector2_t quot, rem;
      div_mod(dividend, ಭಾಜಕ, quot, rem);
      ಹಿಂತಿರುಗಿ quot;
}

vvp_vector2_t ಕ್ರಿಯಾಕಾರಕ % (ಸ್ಥಿರಾಂಕ vvp_vector2_t&dividend,
			  ಸ್ಥಿರಾಂಕ vvp_vector2_t&ಭಾಜಕ)
{
      vvp_vector2_t quot, rem;
      div_mod(dividend, ಭಾಜಕ, quot, rem);
      ಹಿಂತಿರುಗಿ rem;
}

ತಾರ್ಕಿಕ ಕ್ರಿಯಾಕಾರಕ > (ಸ್ಥಿರಾಂಕ vvp_vector2_t&a, ಸ್ಥಿರಾಂಕ vvp_vector2_t&b)
{
      ಸ್ಥಿರಾಂಕ ಚಿಹ್ನೆರಹಿತ awords = (a.wid_ + vvp_vector2_t::ಬಿಟ್ಟುಗಳು_ಪ್ರತಿ_ಶಬ್ದ-1) / vvp_vector2_t::ಬಿಟ್ಟುಗಳು_ಪ್ರತಿ_ಶಬ್ದ;
      ಸ್ಥಿರಾಂಕ ಚಿಹ್ನೆರಹಿತ bwords = (b.wid_ + vvp_vector2_t::ಬಿಟ್ಟುಗಳು_ಪ್ರತಿ_ಶಬ್ದ-1) / vvp_vector2_t::ಬಿಟ್ಟುಗಳು_ಪ್ರತಿ_ಶಬ್ದ;

      ಸ್ಥಿರಾಂಕ ಚಿಹ್ನೆರಹಿತ ಪದಗಳು = awords > bwords? awords : bwords;

      ಗಾಗಿ (ಚಿಹ್ನೆರಹಿತ idx = ಪದಗಳು ;  idx > 0 ;  idx -= 1) {
	    ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ aw = (idx <= awords)? a.vec_[idx-1] : 0;
	    ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ bw = (idx <= bwords)? b.vec_[idx-1] : 0;

	    ಆದರೆ (aw > bw)
		  ಹಿಂತಿರುಗಿ ನಿಜ;
	    ಆದರೆ (aw < bw)
		  ಹಿಂತಿರುಗಿ ಸುಳ್ಳು;
      }

	// If the above loop finishes, then the vectors are equal.
      ಹಿಂತಿರುಗಿ ಸುಳ್ಳು;
}

ತಾರ್ಕಿಕ ಕ್ರಿಯಾಕಾರಕ >= (ಸ್ಥಿರಾಂಕ vvp_vector2_t&a, ಸ್ಥಿರಾಂಕ vvp_vector2_t&b)
{
      ಸ್ಥಿರಾಂಕ ಚಿಹ್ನೆರಹಿತ awords = (a.wid_ + vvp_vector2_t::ಬಿಟ್ಟುಗಳು_ಪ್ರತಿ_ಶಬ್ದ-1) / vvp_vector2_t::ಬಿಟ್ಟುಗಳು_ಪ್ರತಿ_ಶಬ್ದ;
      ಸ್ಥಿರಾಂಕ ಚಿಹ್ನೆರಹಿತ bwords = (b.wid_ + vvp_vector2_t::ಬಿಟ್ಟುಗಳು_ಪ್ರತಿ_ಶಬ್ದ-1) / vvp_vector2_t::ಬಿಟ್ಟುಗಳು_ಪ್ರತಿ_ಶಬ್ದ;

      ಸ್ಥಿರಾಂಕ ಚಿಹ್ನೆರಹಿತ ಪದಗಳು = awords > bwords? awords : bwords;

      ಗಾಗಿ (ಚಿಹ್ನೆರಹಿತ idx = ಪದಗಳು ;  idx > 0 ;  idx -= 1) {
	    ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ aw = (idx <= awords)? a.vec_[idx-1] : 0;
	    ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ bw = (idx <= bwords)? b.vec_[idx-1] : 0;

	    ಆದರೆ (aw > bw)
		  ಹಿಂತಿರುಗಿ ನಿಜ;
	    ಆದರೆ (aw < bw)
		  ಹಿಂತಿರುಗಿ ಸುಳ್ಳು;
      }

	// If the above loop finishes, then the vectors are equal.
      ಹಿಂತಿರುಗಿ ನಿಜ;
}

ತಾರ್ಕಿಕ ಕ್ರಿಯಾಕಾರಕ < (ಸ್ಥಿರಾಂಕ vvp_vector2_t&a, ಸ್ಥಿರಾಂಕ vvp_vector2_t&b)
{
      ಸ್ಥಿರಾಂಕ ಚಿಹ್ನೆರಹಿತ awords = (a.wid_ + vvp_vector2_t::ಬಿಟ್ಟುಗಳು_ಪ್ರತಿ_ಶಬ್ದ-1) / vvp_vector2_t::ಬಿಟ್ಟುಗಳು_ಪ್ರತಿ_ಶಬ್ದ;
      ಸ್ಥಿರಾಂಕ ಚಿಹ್ನೆರಹಿತ bwords = (b.wid_ + vvp_vector2_t::ಬಿಟ್ಟುಗಳು_ಪ್ರತಿ_ಶಬ್ದ-1) / vvp_vector2_t::ಬಿಟ್ಟುಗಳು_ಪ್ರತಿ_ಶಬ್ದ;

      ಚಿಹ್ನೆರಹಿತ ಪದಗಳು = awords;
      ಆದರೆ (bwords > ಪದಗಳು)
	    ಪದಗಳು = bwords;

      ಗಾಗಿ (ಚಿಹ್ನೆರಹಿತ idx = ಪದಗಳು ;  idx > 0 ;  idx -= 1) {
	    ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ aw = (idx <= awords)? a.vec_[idx-1] : 0;
	    ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ bw = (idx <= bwords)? b.vec_[idx-1] : 0;

	    ಆದರೆ (aw < bw)
		  ಹಿಂತಿರುಗಿ ನಿಜ;
	    ಆದರೆ (aw > bw)
		  ಹಿಂತಿರುಗಿ ಸುಳ್ಳು;
      }

	// If the above loop finishes, then the vectors are equal.
      ಹಿಂತಿರುಗಿ ಸುಳ್ಳು;
}

ತಾರ್ಕಿಕ ಕ್ರಿಯಾಕಾರಕ <= (ಸ್ಥಿರಾಂಕ vvp_vector2_t&a, ಸ್ಥಿರಾಂಕ vvp_vector2_t&b)
{
	// XXXX For now, only support equal width vectors.
      ಪ್ರತಿಜ್ಞಾ(a.wid_ == b.wid_);

      ಸ್ಥಿರಾಂಕ ಚಿಹ್ನೆರಹಿತ awords = (a.wid_ + vvp_vector2_t::ಬಿಟ್ಟುಗಳು_ಪ್ರತಿ_ಶಬ್ದ-1) / vvp_vector2_t::ಬಿಟ್ಟುಗಳು_ಪ್ರತಿ_ಶಬ್ದ;

      ಗಾಗಿ (ಚಿಹ್ನೆರಹಿತ idx = awords ;  idx > 0 ;  idx -= 1) {
	    ಆದರೆ (a.vec_[idx-1] < b.vec_[idx-1])
		  ಹಿಂತಿರುಗಿ ನಿಜ;
	    ಆದರೆ (a.vec_[idx-1] > b.vec_[idx-1])
		  ಹಿಂತಿರುಗಿ ಸುಳ್ಳು;
      }

	// If the above loop finishes, then the vectors are equal.
      ಹಿಂತಿರುಗಿ ನಿಜ;
}

ತಾರ್ಕಿಕ ಕ್ರಿಯಾಕಾರಕ == (ಸ್ಥಿರಾಂಕ vvp_vector2_t&a, ಸ್ಥಿರಾಂಕ vvp_vector2_t&b)
{
      ಸ್ಥಿರಾಂಕ ಚಿಹ್ನೆರಹಿತ awords = (a.wid_ + vvp_vector2_t::ಬಿಟ್ಟುಗಳು_ಪ್ರತಿ_ಶಬ್ದ-1) / vvp_vector2_t::ಬಿಟ್ಟುಗಳು_ಪ್ರತಿ_ಶಬ್ದ;
      ಸ್ಥಿರಾಂಕ ಚಿಹ್ನೆರಹಿತ bwords = (b.wid_ + vvp_vector2_t::ಬಿಟ್ಟುಗಳು_ಪ್ರತಿ_ಶಬ್ದ-1) / vvp_vector2_t::ಬಿಟ್ಟುಗಳು_ಪ್ರತಿ_ಶಬ್ದ;

      ಸ್ಥಿರಾಂಕ ಚಿಹ್ನೆರಹಿತ ಪದಗಳು = awords > bwords? awords : bwords;

      ಗಾಗಿ (ಚಿಹ್ನೆರಹಿತ idx = ಪದಗಳು ;  idx > 0 ;  idx -= 1) {
	    ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ aw = (idx <= awords)? a.vec_[idx-1] : 0;
	    ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ bw = (idx <= bwords)? b.vec_[idx-1] : 0;

	    ಆದರೆ (aw > bw)
		  ಹಿಂತಿರುಗಿ ಸುಳ್ಳು;
	    ಆದರೆ (aw < bw)
		  ಹಿಂತಿರುಗಿ ಸುಳ್ಳು;
      }

	// If the above loop finishes, then the vectors are equal.
      ಹಿಂತಿರುಗಿ ನಿಜ;
}


vvp_vector4_t vector2_ಗಮ್ಯ_vector4(ಸ್ಥಿರಾಂಕ vvp_vector2_t&that, ಚಿಹ್ನೆರಹಿತ wid)
{
      vvp_vector4_t res (wid);

      ಗಾಗಿ (ಚಿಹ್ನೆರಹಿತ idx = 0 ;  idx < res.size() ;  idx += 1) {
	    vvp_bit4_t ಬಿಟ್ = BIT4_0;

	    ಆದರೆ (that.value(idx))
		  ಬಿಟ್ = BIT4_1;

	    res.ಗಣ_ಬಿಟ್(idx, ಬಿಟ್);
      }

      ಹಿಂತಿರುಗಿ res;
}

ಸ್ಥಿರ ತಾರ್ಕಿಕ c4string_ಶೀರ್ಷಿಕೆ_test(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ*str)
{
      ಆದರೆ ((str[0] != 'C') && (str[0] != 'c'))
	    ಹಿಂತಿರುಗಿ ಸುಳ್ಳು;
      ಆದರೆ ((str[1] != '4') || (str[2] != '<'))
	    ಹಿಂತಿರುಗಿ ಸುಳ್ಳು;
      ಹಿಂತಿರುಗಿ ನಿಜ;
}

ತಾರ್ಕಿಕ c4string_test(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ*str)
{
      ಆದರೆ (!c4string_ಶೀರ್ಷಿಕೆ_test(str))
	    ಹಿಂತಿರುಗಿ ಸುಳ್ಳು;
      size_t ಮೌಲ್ಯ_ಗಾತ್ರ = strspn(str+3, "01xz");
      ಆದರೆ (str[3+ಮೌಲ್ಯ_ಗಾತ್ರ] != '>')
	    ಹಿಂತಿರುಗಿ ಸುಳ್ಳು;
      ಆದರೆ (str[3+ಮೌಲ್ಯ_ಗಾತ್ರ+1] != 0)
	    ಹಿಂತಿರುಗಿ ಸುಳ್ಳು;

      ಹಿಂತಿರುಗಿ ನಿಜ;
}

vvp_vector4_t c4string_ಗಮ್ಯ_vector4(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ*str)
{
      ಪ್ರತಿಜ್ಞಾ(c4string_ಶೀರ್ಷಿಕೆ_test(str));

      str += 3;
      ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ*tp = str + strspn(str,"01xz");
      ಪ್ರತಿಜ್ಞಾ(tp[0] == '>');

      vvp_vector4_t tmp (tp-str);

      ಗಾಗಿ (ಚಿಹ್ನೆರಹಿತ idx = 0 ;  idx < tmp.size() ;  idx += 1) {
	    vvp_bit4_t ಬಿಟ್;
	    ಆಯ್ಕೆ (str[idx]) {
		ಸ್ಥಿತಿ '0':
		  ಬಿಟ್ = BIT4_0;
		  ನಿಲ್ಲಿಸು;
		ಸ್ಥಿತಿ '1':
		  ಬಿಟ್ = BIT4_1;
		  ನಿಲ್ಲಿಸು;
		ಸ್ಥಿತಿ 'x':
		  ಬಿಟ್ = BIT4_X;
		  ನಿಲ್ಲಿಸು;
		ಸ್ಥಿತಿ 'z':
		  ಬಿಟ್ = BIT4_Z;
		  ನಿಲ್ಲಿಸು;
		ಪೂರ್ವನಿಯೋಜಿತ:
		  fprintf(stderr, "Unsupported bit value %c(%d).\n", str[idx],
		          str[idx]);
		  ಪ್ರತಿಜ್ಞಾ(0);
		  ಬಿಟ್ = BIT4_0;
		  ನಿಲ್ಲಿಸು;
	    }
	    tmp.ಗಣ_ಬಿಟ್(tmp.size()-idx-1, ಬಿಟ್);
      }

      ಹಿಂತಿರುಗಿ tmp;
}

ostream& ಕ್ರಿಯಾಕಾರಕ<< (ostream&out, ಸ್ಥಿರಾಂಕ vvp_vector2_t&that)
{
      ಆದರೆ (that.is_NaN()) {
	    out << "NaN";

      } ಇಲ್ಲದಿದ್ದರೆ {
	    out << vector2_ಗಮ್ಯ_vector4(that, that.size());
      }
      ಹಿಂತಿರುಗಿ out;
}

vvp_vector8_t::vvp_vector8_t(ಸ್ಥಿರಾಂಕ vvp_vector8_t&that)
{
      ಗಾತ್ರ_3 = that.ಗಾತ್ರ_3;
      ಆದರೆ (ಗಾತ್ರ_3 <= ಗಾತ್ರ(val_)) {
	    ಸ್ಮರಣೆ_ನಕಲು(val_, that.val_, ಗಾತ್ರ val_);
      } ಇಲ್ಲದಿದ್ದರೆ {
	    ptr_ = ಹೊಸ ಚಿಹ್ನೆರಹಿತ ಅಕ್ಷರ[ಗಾತ್ರ_3];
	    ಸ್ಮರಣೆ_ನಕಲು(ptr_, that.ptr_, ಗಾತ್ರ_3);
      }
}

vvp_vector8_t::vvp_vector8_t(ಸ್ಥಿರಾಂಕ vvp_vector4_t&that,
			     ಚಿಹ್ನೆರಹಿತ str0, ಚಿಹ್ನೆರಹಿತ str1)
: ಗಾತ್ರ_3(that.size())
{
      ಆದರೆ (ಗಾತ್ರ_3 == 0)
	    ಹಿಂತಿರುಗಿ;

      ಆದರೆ (ಗಾತ್ರ_3 <= ಗಾತ್ರ(val_)) {
	    ptr_ = 0; // Prefill all val_ bytes
	    ಗಾಗಿ (ಚಿಹ್ನೆರಹಿತ idx = 0 ; idx < ಗಾತ್ರ_3 ; idx += 1)
		  val_[idx] = vvp_scalar_t(that.value(idx),str0, str1).raw();
      } ಇಲ್ಲದಿದ್ದರೆ {
	    ptr_ = ಹೊಸ ಚಿಹ್ನೆರಹಿತ ಅಕ್ಷರ[ಗಾತ್ರ_3];
	    ಗಾಗಿ (ಚಿಹ್ನೆರಹಿತ idx = 0 ;  idx < ಗಾತ್ರ_3 ;  idx += 1)
		  ptr_[idx] = vvp_scalar_t(that.value(idx), str0, str1).raw();
      }
}

vvp_vector8_t::vvp_vector8_t(ಸ್ಥಿರಾಂಕ vvp_vector2_t&that,
			     ಚಿಹ್ನೆರಹಿತ str0, ಚಿಹ್ನೆರಹಿತ str1)
: ಗಾತ್ರ_3(that.size())
{
      ಆದರೆ (ಗಾತ್ರ_3 == 0)
	    ಹಿಂತಿರುಗಿ;

      ಆದರೆ (ಗಾತ್ರ_3 <= ಗಾತ್ರ(val_)) {
	    ptr_ = 0;
	    ಗಾಗಿ (ಚಿಹ್ನೆರಹಿತ idx = 0 ; idx < ಗಾತ್ರ_3 ; idx += 1)
		  val_[idx] = vvp_scalar_t(that.value(idx)? BIT4_1:BIT4_0, str0, str1).raw();
      } ಇಲ್ಲದಿದ್ದರೆ {
	    ptr_ = ಹೊಸ ಚಿಹ್ನೆರಹಿತ ಅಕ್ಷರ[ಗಾತ್ರ_3];
	    ಗಾಗಿ (ಚಿಹ್ನೆರಹಿತ idx = 0 ; idx < ಗಾತ್ರ_3 ; idx += 1)
		  ptr_[idx] = vvp_scalar_t(that.value(idx)? BIT4_1:BIT4_0, str0, str1).raw();
      }
}

ಸ್ಥಿರಾಂಕ vvp_vector8_t vvp_vector8_t::nil;

vvp_vector8_t& vvp_vector8_t::ಕ್ರಿಯಾಕಾರಕ= (ಸ್ಥಿರಾಂಕ vvp_vector8_t&that)
{
	// Assign to self.
      ಆದರೆ (ಇದು == &that)
	    ಹಿಂತಿರುಗಿ *ಇದು;

      ಆದರೆ (ಗಾತ್ರ_3 != that.ಗಾತ್ರ_3) {
	    ಆದರೆ (ಗಾತ್ರ_3 > ಗಾತ್ರ(val_))
		  ಅಳಿಸು[]ptr_;
	    ಗಾತ್ರ_3 = 0;
      }

      ಆದರೆ (that.ಗಾತ್ರ_3 == 0) {
	    ಪ್ರತಿಜ್ಞಾ(ಗಾತ್ರ_3 == 0);
	    ಹಿಂತಿರುಗಿ *ಇದು;
      }

      ಆದರೆ (that.ಗಾತ್ರ_3 <= ಗಾತ್ರ(val_)) {
	    ಗಾತ್ರ_3 = that.ಗಾತ್ರ_3;
	    ಸ್ಮರಣೆ_ನಕಲು(val_, that.val_, ಗಾತ್ರ(val_));
	    ಹಿಂತಿರುಗಿ *ಇದು;
      }

      ಆದರೆ (ಗಾತ್ರ_3 == 0) {
	    ಗಾತ್ರ_3 = that.ಗಾತ್ರ_3;
	    ptr_ = ಹೊಸ ಚಿಹ್ನೆರಹಿತ ಅಕ್ಷರ[ಗಾತ್ರ_3];
      }

      ಸ್ಮರಣೆ_ನಕಲು(ptr_, that.ptr_, ಗಾತ್ರ_3);

      ಹಿಂತಿರುಗಿ *ಇದು;
}

vvp_vector8_t vvp_vector8_t::subvalue(ಚಿಹ್ನೆರಹಿತ ತಳ, ಚಿಹ್ನೆರಹಿತ wid) ಸ್ಥಿರಾಂಕ
{
      vvp_vector8_t tmp (wid);

      ಚಿಹ್ನೆರಹಿತ ಅಕ್ಷರ*tmp_ptr = tmp.ಗಾತ್ರ_3 <= ಗಾತ್ರ(val_) ? tmp.val_ : tmp.ptr_;
      ಸ್ಥಿರಾಂಕ ಚಿಹ್ನೆರಹಿತ ಅಕ್ಷರ*use_ptr = ಗಾತ್ರ_3 <= ಗಾತ್ರ(val_) ? val_ : ptr_;

      ಚಿಹ್ನೆರಹಿತ idx = 0;
      ಆಗ ((idx < wid) && (ತಳ+idx < ಗಾತ್ರ_3)) {
	    tmp_ptr[idx] = use_ptr[ತಳ+idx];
	    idx += 1;
      }

      ಹಿಂತಿರುಗಿ tmp;
}

ಶೂನ್ಯ vvp_vector8_t::ಗಣ_vec(ಚಿಹ್ನೆರಹಿತ ತಳ, ಸ್ಥಿರಾಂಕ vvp_vector8_t&that)
{
      ಪ್ರತಿಜ್ಞಾ((ತಳ+that.size()) <= size());
      ಗಾಗಿ (ಚಿಹ್ನೆರಹಿತ idx = 0 ; idx < that.size() ; idx += 1)
	    ಗಣ_ಬಿಟ್(ತಳ+idx, that.value(idx));
}

vvp_vector8_t part_expand(ಸ್ಥಿರಾಂಕ vvp_vector8_t&that, ಚಿಹ್ನೆರಹಿತ wid, ಚಿಹ್ನೆರಹಿತ off)
{
      ಪ್ರತಿಜ್ಞಾ(off < wid);
      vvp_vector8_t tmp (wid);

      ಚಿಹ್ನೆರಹಿತ ಅಕ್ಷರ* tmp_ptr = tmp.ಗಾತ್ರ_3<= ಗಾತ್ರ(tmp.val_) ?
                               tmp.val_ : tmp.ptr_;
      ಸ್ಥಿರಾಂಕ ಚಿಹ್ನೆರಹಿತ ಅಕ್ಷರ* that_ptr = that.ಗಾತ್ರ_3<= ಗಾತ್ರ(that.val_) ?
                                      that.val_ : that.ptr_;

      ಚಿಹ್ನೆರಹಿತ idx = off;

      ಆಗ (idx < wid && that.ಗಾತ್ರ_3 > (idx-off)) {
	    tmp_ptr[idx] = that_ptr[idx-off];
	    idx += 1;
      }

      ಹಿಂತಿರುಗಿ tmp;
}

ಸ್ಥಿರ ತಾರ್ಕಿಕ c8string_ಶೀರ್ಷಿಕೆ_test(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ*str)
{
      ಆದರೆ ((str[0] != 'C') && (str[0] != 'c'))
	    ಹಿಂತಿರುಗಿ ಸುಳ್ಳು;
      ಆದರೆ ((str[1] != '8') || (str[2] != '<'))
	    ಹಿಂತಿರುಗಿ ಸುಳ್ಳು;
      ಹಿಂತಿರುಗಿ ನಿಜ;
}

ತಾರ್ಕಿಕ c8string_test(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ*str)
{
      ಆದರೆ (!c8string_ಶೀರ್ಷಿಕೆ_test(str))
	    ಹಿಂತಿರುಗಿ ಸುಳ್ಳು;

      ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ*cp = str+3;
      ಗಾಗಿ (;; cp += 1) {
	    ಆದರೆ (cp[0] == '>' && cp[1] == 0) ಹಿಂತಿರುಗಿ ನಿಜ;
	    ಆದರೆ (cp[0] >= '0' && cp[0] <= '9') ಮುಂದುವರಿ;
	    ಆದರೆ (cp[0] == 'x') ಮುಂದುವರಿ;
	    ಆದರೆ (cp[0] == 'z') ಮುಂದುವರಿ;
	    ಹಿಂತಿರುಗಿ ಸುಳ್ಳು;
      }
      ಹಿಂತಿರುಗಿ ಸುಳ್ಳು;
}
/*
 * The format of a C8<> string is:
 *   C8<aaabbbccc...>
 * where aaa... is a 3 character bit descriptor.
 */
vvp_vector8_t c8string_ಗಮ್ಯ_vector8(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ*str)
{
      ಪ್ರತಿಜ್ಞಾ(c8string_ಶೀರ್ಷಿಕೆ_test(str));

      size_t vsize = ಅಕ್ಷರಸರಣಿ_ಉದ್ದ(str)-4;
      ಪ್ರತಿಜ್ಞಾ(vsize%3 == 0);
      vsize /= 3;
      vvp_vector8_t tmp (vsize);

      ಗಾಗಿ (size_t idx = 0 ; idx < vsize ; idx += 1) {
	    ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ*cp = str+3+3*idx;
	    vvp_bit4_t ಬಿಟ್ = BIT4_X;
	    ಚಿಹ್ನೆರಹಿತ dr0 = cp[0]-'0';
	    ಚಿಹ್ನೆರಹಿತ dr1 = cp[1]-'0';
	    ಆಯ್ಕೆ (cp[2]) {
		ಸ್ಥಿತಿ '0':
		  ಬಿಟ್ = BIT4_0;
		  ನಿಲ್ಲಿಸು;
		ಸ್ಥಿತಿ '1':
		  ಬಿಟ್ = BIT4_1;
		  ನಿಲ್ಲಿಸು;
		ಸ್ಥಿತಿ 'x':
		  ಬಿಟ್ = BIT4_X;
		  ನಿಲ್ಲಿಸು;
		ಸ್ಥಿತಿ 'z':
		  ಬಿಟ್ = BIT4_Z;
		  ನಿಲ್ಲಿಸು;
	    }
	    tmp.ಗಣ_ಬಿಟ್(vsize-idx-1, vvp_scalar_t(ಬಿಟ್, dr0, dr1));
      }

      ಹಿಂತಿರುಗಿ tmp;
}

ostream& ಕ್ರಿಯಾಕಾರಕ<<(ostream&out, ಸ್ಥಿರಾಂಕ vvp_vector8_t&that)
{
      out << "C8<";
      ಗಾಗಿ (ಚಿಹ್ನೆರಹಿತ idx = 0 ;  idx < that.size() ; idx += 1)
	    out << that.value(that.size()-idx-1);

      out << ">";
      ಹಿಂತಿರುಗಿ out;
}

vvp_net_fun_t::vvp_net_fun_t()
{
      ಎಣಿಕೆ_functors += 1;
}

vvp_net_fun_t::~vvp_net_fun_t()
{
}

ಶೂನ್ಯ vvp_net_fun_t::recv_vec4(vvp_net_ptr_t, ಸ್ಥಿರಾಂಕ vvp_vector4_t&,
                              vvp_context_t)
{
      fprintf(stderr, "internal error: %s: recv_vec4 not implemented\n",
	      typeid(*ಇದು).name());
      ಪ್ರತಿಜ್ಞಾ(0);
}

ಶೂನ್ಯ vvp_net_fun_t::recv_vec4_pv_(vvp_net_ptr_t p, ಸ್ಥಿರಾಂಕ vvp_vector4_t&ಬಿಟ್,
                                  ಚಿಹ್ನೆರಹಿತ ತಳ, ಚಿಹ್ನೆರಹಿತ vwid, vvp_context_t)
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

      ಪ್ರತಿಜ್ಞಾ(ತಳ + ಬಿಟ್.size() <= vwid);

      vvp_vector4_t tmp(vwid, BIT4_Z);
      tmp.ಗಣ_vec(ತಳ, ಬಿಟ್);
      recv_vec4(p, tmp, 0);
}

ಶೂನ್ಯ vvp_net_fun_t::recv_vec4_pv(vvp_net_ptr_t, ಸ್ಥಿರಾಂಕ vvp_vector4_t&ಬಿಟ್,
                                 ಚಿಹ್ನೆರಹಿತ ತಳ, ಚಿಹ್ನೆರಹಿತ vwid, vvp_context_t)
{
      cerr << "internal error: " << typeid(*ಇದು).name() << ": "
	   << "recv_vec4_pv(" << ಬಿಟ್ << ", " << ತಳ
	   << ", " << vwid << ") not implemented" << ಸಾಲು_ಅಂತ್ಯ;
      ಪ್ರತಿಜ್ಞಾ(0);
}

ಶೂನ್ಯ vvp_net_fun_t::recv_vec8(vvp_net_ptr_t ಪೋರ್ಟ್, ಸ್ಥಿರಾಂಕ vvp_vector8_t&ಬಿಟ್)
{
      recv_vec4(ಪೋರ್ಟ್, reduce4(ಬಿಟ್), 0);
}

ಶೂನ್ಯ vvp_net_fun_t::recv_vec8_pv_(vvp_net_ptr_t p, ಸ್ಥಿರಾಂಕ vvp_vector8_t&ಬಿಟ್,
				  ಚಿಹ್ನೆರಹಿತ ತಳ, ಚಿಹ್ನೆರಹಿತ vwid)
{
	// This is the strength-aware version of recv_vec4_pv_.

      ಪ್ರತಿಜ್ಞಾ(ತಳ + ಬಿಟ್.size() <= vwid);

      vvp_vector8_t tmp(vwid);
      tmp.ಗಣ_vec(ತಳ, ಬಿಟ್);
      recv_vec8(p, tmp);
}

ಶೂನ್ಯ vvp_net_fun_t::recv_vec8_pv(vvp_net_ptr_t ಪೋರ್ಟ್, ಸ್ಥಿರಾಂಕ vvp_vector8_t&ಬಿಟ್,
				 ಚಿಹ್ನೆರಹಿತ ತಳ, ಚಿಹ್ನೆರಹಿತ vwid)
{
      recv_vec4_pv(ಪೋರ್ಟ್, reduce4(ಬಿಟ್), ತಳ, vwid, 0);
}

ಶೂನ್ಯ vvp_net_fun_t::recv_real(vvp_net_ptr_t, ದ್ವಿಗುಣ ಬಿಟ್, vvp_context_t)
{
      fprintf(stderr, "internal error: %s: recv_real(%f) not implemented\n",
	      typeid(*ಇದು).name(), ಬಿಟ್);
      ಪ್ರತಿಜ್ಞಾ(0);
}

ಶೂನ್ಯ vvp_net_fun_t::recv_string(vvp_net_ptr_t, ಸ್ಥಿರಾಂಕ std::string&ಬಿಟ್, vvp_context_t)
{
      fprintf(stderr, "internal error: %s: recv_string(%s) not implemented\n",
	      typeid(*ಇದು).name(), ಬಿಟ್.c_str());
      ಪ್ರತಿಜ್ಞಾ(0);
}

ಶೂನ್ಯ vvp_net_fun_t::recv_object(vvp_net_ptr_t, vvp_object_t, vvp_context_t)
{
      fprintf(stderr, "internal error: %s: recv_object(...) not implemented\n",
	      typeid(*ಇದು).name());
      ಪ್ರತಿಜ್ಞಾ(0);
}

ಶೂನ್ಯ vvp_net_fun_t::force_flag(ತಾರ್ಕಿಕ)
{
}

/* **** vvp_fun_drive methods **** */

vvp_fun_drive::vvp_fun_drive(ಚಿಹ್ನೆರಹಿತ str0, ಚಿಹ್ನೆರಹಿತ str1)
{
      ಪ್ರತಿಜ್ಞಾ(str0 < 8);
      ಪ್ರತಿಜ್ಞಾ(str1 < 8);

      drive0_ = str0;
      drive1_ = str1;
}

vvp_fun_drive::~vvp_fun_drive()
{
}

ಶೂನ್ಯ vvp_fun_drive::recv_vec4(vvp_net_ptr_t ಪೋರ್ಟ್, ಸ್ಥಿರಾಂಕ vvp_vector4_t&ಬಿಟ್,
                              vvp_context_t)
{
      ಪ್ರತಿಜ್ಞಾ(ಪೋರ್ಟ್.ಪೋರ್ಟ್() == 0);
      ಪೋರ್ಟ್.ptr()->send_vec8(vvp_vector8_t(ಬಿಟ್, drive0_, drive1_));
}

ಶೂನ್ಯ vvp_fun_drive::recv_vec4_pv(vvp_net_ptr_t ptr, ಸ್ಥಿರಾಂಕ vvp_vector4_t&ಬಿಟ್,
				 ಚಿಹ್ನೆರಹಿತ ತಳ, ಚಿಹ್ನೆರಹಿತ vwid, vvp_context_t ctx)
{
      recv_vec4_pv_(ptr, ಬಿಟ್, ತಳ, vwid, ctx);
}

/* **** vvp_wide_fun_* methods **** */

vvp_wide_fun_core::vvp_wide_fun_core(vvp_net_t*net, ಚಿಹ್ನೆರಹಿತ nports)
{
      ptr_ = net;
      nports_ = nports;
      ಪೋರ್ಟ್_ಮೌಲ್ಯಗಳು = 0;
      ಪೋರ್ಟ್_rvalues = 0;
}

vvp_wide_fun_core::~vvp_wide_fun_core()
{
      ಅಳಿಸು[] ಪೋರ್ಟ್_ಮೌಲ್ಯಗಳು;
      ಅಳಿಸು[] ಪೋರ್ಟ್_rvalues;
}

ಶೂನ್ಯ vvp_wide_fun_core::propagate_vec4(ಸ್ಥಿರಾಂಕ vvp_vector4_t&ಬಿಟ್,
				       vvp_time64_t delay)
{
      ಆದರೆ (delay)
	    ವೇಳಾಪಟ್ಟಿ_propagate_ಸದಿಶ(ptr_, delay, ಬಿಟ್);
      ಇಲ್ಲದಿದ್ದರೆ
	    ptr_->send_vec4(ಬಿಟ್, 0);
}

ಶೂನ್ಯ vvp_wide_fun_core::propagate_real(ದ್ವಿಗುಣ ಬಿಟ್,
				       vvp_time64_t delay)
{
      ಆದರೆ (delay) {
	      // schedule_assign_vector(ptr_->out, bit, delay);
	    ಪ್ರತಿಜ್ಞಾ(0); // Need a real-value version of assign_vector.
      } ಇಲ್ಲದಿದ್ದರೆ {
	    ptr_->send_real(ಬಿಟ್, 0);
      }
}


ಚಿಹ್ನೆರಹಿತ vvp_wide_fun_core::ಪೋರ್ಟ್_ಎಣಿಕೆ() ಸ್ಥಿರಾಂಕ
{
      ಹಿಂತಿರುಗಿ nports_;
}

vvp_vector4_t& vvp_wide_fun_core::value(ಚಿಹ್ನೆರಹಿತ idx)
{
      ಪ್ರತಿಜ್ಞಾ(idx < nports_);
      ಆದರೆ (ಪೋರ್ಟ್_ಮೌಲ್ಯಗಳು == 0)
	    ಪೋರ್ಟ್_ಮೌಲ್ಯಗಳು = ಹೊಸ vvp_vector4_t [nports_];
      ಹಿಂತಿರುಗಿ ಪೋರ್ಟ್_ಮೌಲ್ಯಗಳು[idx];
}

ದ್ವಿಗುಣ vvp_wide_fun_core::ಮೌಲ್ಯ_r(ಚಿಹ್ನೆರಹಿತ idx)
{
      ಪ್ರತಿಜ್ಞಾ(idx < nports_);
      ಹಿಂತಿರುಗಿ ಪೋರ್ಟ್_rvalues? ಪೋರ್ಟ್_rvalues[idx] : 0.0;
}

ಶೂನ್ಯ vvp_wide_fun_core::recv_real_from_inputs(ಚಿಹ್ನೆರಹಿತ)
{
      ಪ್ರತಿಜ್ಞಾ(0);
}

ಶೂನ್ಯ vvp_wide_fun_core::dispatch_vec4_from_ಒಳಹರಿವು(ಚಿಹ್ನೆರಹಿತ ಪೋರ್ಟ್,
						  ಸ್ಥಿರಾಂಕ vvp_vector4_t&ಬಿಟ್)
{
      ಪ್ರತಿಜ್ಞಾ(ಪೋರ್ಟ್ < nports_);
      ಆದರೆ (ಪೋರ್ಟ್_ಮೌಲ್ಯಗಳು == 0) ಪೋರ್ಟ್_ಮೌಲ್ಯಗಳು = ಹೊಸ vvp_vector4_t [nports_];
      ಪೋರ್ಟ್_ಮೌಲ್ಯಗಳು[ಪೋರ್ಟ್] = ಬಿಟ್;
      recv_vec4_from_inputs(ಪೋರ್ಟ್);
}

ಶೂನ್ಯ vvp_wide_fun_core::dispatch_real_from_ಒಳಹರಿವು(ಚಿಹ್ನೆರಹಿತ ಪೋರ್ಟ್,
						  ದ್ವಿಗುಣ ಬಿಟ್)
{
      ಪ್ರತಿಜ್ಞಾ(ಪೋರ್ಟ್ < nports_);
      ಆದರೆ (ಪೋರ್ಟ್_rvalues == 0) ಪೋರ್ಟ್_rvalues = ಹೊಸ ದ್ವಿಗುಣ[nports_];
      ಪೋರ್ಟ್_rvalues[ಪೋರ್ಟ್] = ಬಿಟ್;
      recv_real_from_inputs(ಪೋರ್ಟ್);
}

vvp_wide_fun_t::vvp_wide_fun_t(vvp_wide_fun_core*c, ಚಿಹ್ನೆರಹಿತ ತಳ)
: core_(c), ಪೋರ್ಟ್_ತಳ_2(ತಳ)
{
}

vvp_wide_fun_t::~vvp_wide_fun_t()
{
}

ಶೂನ್ಯ vvp_wide_fun_t::recv_vec4(vvp_net_ptr_t ಪೋರ್ಟ್, ಸ್ಥಿರಾಂಕ vvp_vector4_t&ಬಿಟ್,
                               vvp_context_t)
{
      ಚಿಹ್ನೆರಹಿತ pidx = ಪೋರ್ಟ್_ತಳ_2 + ಪೋರ್ಟ್.ಪೋರ್ಟ್();
      core_->dispatch_vec4_from_ಒಳಹರಿವು(pidx, ಬಿಟ್);
}

ಶೂನ್ಯ vvp_wide_fun_t::recv_vec4_pv(vvp_net_ptr_t ptr, ಸ್ಥಿರಾಂಕ vvp_vector4_t&ಬಿಟ್,
				  ಚಿಹ್ನೆರಹಿತ ತಳ, ಚಿಹ್ನೆರಹಿತ vwid, vvp_context_t ctx)
{
      recv_vec4_pv_(ptr, ಬಿಟ್, ತಳ, vwid, ctx);
}

ಶೂನ್ಯ vvp_wide_fun_t::recv_real(vvp_net_ptr_t ಪೋರ್ಟ್, ದ್ವಿಗುಣ ಬಿಟ್,
                               vvp_context_t)
{
      ಚಿಹ್ನೆರಹಿತ pidx = ಪೋರ್ಟ್_ತಳ_2 + ಪೋರ್ಟ್.ಪೋರ್ಟ್();
      core_->dispatch_real_from_ಒಳಹರಿವು(pidx, ಬಿಟ್);
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

ಚಿಹ್ನೆರಹಿತ vvp_scalar_t::strength0() ಸ್ಥಿರಾಂಕ
{
      ಹಿಂತಿರುಗಿ STREN0(ಮೌಲ್ಯ_2);
}

ಚಿಹ್ನೆರಹಿತ vvp_scalar_t::strength1() ಸ್ಥಿರಾಂಕ
{
      ಹಿಂತಿರುಗಿ STREN1(ಮೌಲ್ಯ_2);
}

ostream& ಕ್ರಿಯಾಕಾರಕ <<(ostream&out, vvp_scalar_t a)
{
      out << a.strength0() << a.strength1();
      ಆಯ್ಕೆ (a.value()) {
	  ಸ್ಥಿತಿ BIT4_0:
	    out << "0";
	    ನಿಲ್ಲಿಸು;
	  ಸ್ಥಿತಿ BIT4_1:
	    out << "1";
	    ನಿಲ್ಲಿಸು;
	  ಸ್ಥಿತಿ BIT4_X:
	    out << "X";
	    ನಿಲ್ಲಿಸು;
	  ಸ್ಥಿತಿ BIT4_Z:
	    out << "Z";
	    ನಿಲ್ಲಿಸು;
      }
      ಹಿಂತಿರುಗಿ out;
}

/*
 * This function is only called if the actual interface function rules
 * out some of the easy cases. If we get here, we can assume that
 * neither of the values is HiZ, and the values are not exactly equal.
 */
vvp_scalar_t fully_featured_resolv_(vvp_scalar_t a, vvp_scalar_t b)
{

      ಆದರೆ (UNAMBIG(a.ಮೌಲ್ಯ_2) && UNAMBIG(b.ಮೌಲ್ಯ_2)) {

	      /* If both signals are unambiguous, simply choose
		 the stronger. If they have the same strength
		 but different values, then this becomes
		 ambiguous. */

	    ಆದರೆ ((b.ಮೌಲ್ಯ_2&0x07) > (a.ಮೌಲ್ಯ_2&0x07)) {

		    /* b value is stronger. Take it. */
		  ಹಿಂತಿರುಗಿ b;

	    } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ ((b.ಮೌಲ್ಯ_2&0x77) == (a.ಮೌಲ್ಯ_2&0x77)) {

		    // Strengths are the same. Since we know already
		    // that the values are not the same, Make value
		    // into "x".
		  vvp_scalar_t tmp (a);
		  tmp.ಮೌಲ್ಯ_2 = (tmp.ಮೌಲ್ಯ_2&0x77) | 0x80;
		  ಹಿಂತಿರುಗಿ tmp;

	    } ಇಲ್ಲದಿದ್ದರೆ {

		    /* Must be "a" is the stronger one. */
		  ಹಿಂತಿರುಗಿ a;
	    }

      }

	/* If one of the signals is unambiguous, then it
	   will sweep up the weaker parts of the ambiguous
	   signal. The result may be ambiguous, or maybe not. */

      ಆದರೆ (UNAMBIG(a.ಮೌಲ್ಯ_2)) {
	    vvp_scalar_t res;

	    ಆದರೆ ((a.ಮೌಲ್ಯ_2&0x70) > (b.ಮೌಲ್ಯ_2&0x70))
		  res.ಮೌಲ್ಯ_2 |= a.ಮೌಲ್ಯ_2&0xf0;
	    ಇಲ್ಲದಿದ್ದರೆ
		  res.ಮೌಲ್ಯ_2 |= b.ಮೌಲ್ಯ_2&0xf0;

	    ಆದರೆ ((a.ಮೌಲ್ಯ_2&0x07) > (b.ಮೌಲ್ಯ_2&0x07))
		  res.ಮೌಲ್ಯ_2 |= a.ಮೌಲ್ಯ_2&0x0f;
	    ಇಲ್ಲದಿದ್ದರೆ
		  res.ಮೌಲ್ಯ_2 |= b.ಮೌಲ್ಯ_2&0x0f;

	    ಹಿಂತಿರುಗಿ res;

      } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (UNAMBIG(b.ಮೌಲ್ಯ_2)) {

	    vvp_scalar_t res;

	    ಆದರೆ ((b.ಮೌಲ್ಯ_2&0x70) > (a.ಮೌಲ್ಯ_2&0x70))
		  res.ಮೌಲ್ಯ_2 |= b.ಮೌಲ್ಯ_2&0xf0;
	    ಇಲ್ಲದಿದ್ದರೆ
		  res.ಮೌಲ್ಯ_2 |= a.ಮೌಲ್ಯ_2&0xf0;

	    ಆದರೆ ((b.ಮೌಲ್ಯ_2&0x07) > (a.ಮೌಲ್ಯ_2&0x07))
		  res.ಮೌಲ್ಯ_2 |= b.ಮೌಲ್ಯ_2&0x0f;
	    ಇಲ್ಲದಿದ್ದರೆ
		  res.ಮೌಲ್ಯ_2 |= a.ಮೌಲ್ಯ_2&0x0f;

	    ಹಿಂತಿರುಗಿ res;

      }


	/* If both signals are ambiguous, then the result
	   has an even wider ambiguity. */

      ಚಿಹ್ನೆರಹಿತ tmp = 0;
      ಪೂರ್ಣಾಂಕ sv1a = (a.ಮೌಲ್ಯ_2 & 0x80) ? STREN1(a.ಮೌಲ್ಯ_2) : - STREN1(a.ಮೌಲ್ಯ_2);
      ಪೂರ್ಣಾಂಕ sv0a = (a.ಮೌಲ್ಯ_2 & 0x08) ? STREN0(a.ಮೌಲ್ಯ_2) : - STREN0(a.ಮೌಲ್ಯ_2);
      ಪೂರ್ಣಾಂಕ sv1b = (b.ಮೌಲ್ಯ_2 & 0x80) ? STREN1(b.ಮೌಲ್ಯ_2) : - STREN1(b.ಮೌಲ್ಯ_2);
      ಪೂರ್ಣಾಂಕ sv0b = (b.ಮೌಲ್ಯ_2 & 0x08) ? STREN0(b.ಮೌಲ್ಯ_2) : - STREN0(b.ಮೌಲ್ಯ_2);

      ಪೂರ್ಣಾಂಕ sv1 = sv1a;
      ಪೂರ್ಣಾಂಕ sv0 = sv0a;

      ಆದರೆ (sv0a > sv1)
	    sv1 = sv0a;
      ಆದರೆ (sv1b > sv1)
	    sv1 = sv1b;
      ಆದರೆ (sv0b > sv1)
	    sv1 = sv0b;

      ಆದರೆ (sv1a < sv0)
	    sv0 = sv1a;
      ಆದರೆ (sv1b < sv0)
	    sv0 = sv1b;
      ಆದರೆ (sv0b < sv0)
	    sv0 = sv0b;

      ಆದರೆ (sv1 > 0) {
	    tmp |= 0x80;
	    tmp |= sv1 << 4;
      } ಇಲ್ಲದಿದ್ದರೆ {
	      /* Set the MSB when both arguments MSBs are set. This
		 can only happen if both one strengths are zero. */
	    tmp |= (a.ಮೌಲ್ಯ_2&b.ಮೌಲ್ಯ_2)&0x80;
	    tmp |= (-sv1) << 4;
      }

      ಆದರೆ (sv0 > 0) {
	    tmp |= 0x08;
	    tmp |= sv0;
      } ಇಲ್ಲದಿದ್ದರೆ {
	    tmp |= (-sv0);
      }

      vvp_scalar_t res;
      res.ಮೌಲ್ಯ_2 = tmp;

	/* Canonicalize the HiZ value. */
      ಆದರೆ ((res.ಮೌಲ್ಯ_2&0x77) == 0)
	    res.ಮೌಲ್ಯ_2 = 0;

      ಹಿಂತಿರುಗಿ res;
}

ಚಿಹ್ನೆರಹಿತ vvp_switch_strength_map[2][8] = {
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

vvp_vector4_t reduce4(ಸ್ಥಿರಾಂಕ vvp_vector8_t&that)
{
      vvp_vector4_t out (that.size());
      ಗಾಗಿ (ಚಿಹ್ನೆರಹಿತ idx = 0 ;  idx < out.size() ;  idx += 1)
	    out.ಗಣ_ಬಿಟ್(idx, that.value(idx).value());

      ಹಿಂತಿರುಗಿ out;
}

vvp_bit4_t compare_gtge(ಸ್ಥಿರಾಂಕ vvp_vector4_t&lef, ಸ್ಥಿರಾಂಕ vvp_vector4_t&rig,
			vvp_bit4_t ಹೊರಹೋಗು_if_equal)
{
      ಚಿಹ್ನೆರಹಿತ min_ಗಾತ್ರ = lef.size();
      ಆದರೆ (rig.size() < min_ಗಾತ್ರ)
	    min_ಗಾತ್ರ = rig.size();

	// If one of the inputs is nil, treat is as all X values, and
	// that makes the result BIT4_X.
      ಆದರೆ (min_ಗಾತ್ರ == 0)
	    ಹಿಂತಿರುಗಿ BIT4_X;

	// As per the IEEE1364 definition of >, >=, < and <=, if there
	// are any X or Z values in either of the operand vectors,
	// then the result of the compare is BIT4_X.

	// Check for X/Z in the left operand
      ಆದರೆ (lef.has_xz())
	    ಹಿಂತಿರುಗಿ BIT4_X;

	// Check for X/Z in the right operand
      ಆದರೆ (rig.has_xz())
	    ಹಿಂತಿರುಗಿ BIT4_X;

      ಗಾಗಿ (ಚಿಹ್ನೆರಹಿತ idx = lef.size() ; idx > rig.size() ;  idx -= 1) {
	    ಆದರೆ (lef.value(idx-1) == BIT4_1)
		  ಹಿಂತಿರುಗಿ BIT4_1;
      }

      ಗಾಗಿ (ಚಿಹ್ನೆರಹಿತ idx = rig.size() ; idx > lef.size() ;  idx -= 1) {
	    ಆದರೆ (rig.value(idx-1) == BIT4_1)
		  ಹಿಂತಿರುಗಿ BIT4_0;
      }

      ಗಾಗಿ (ಚಿಹ್ನೆರಹಿತ idx = min_ಗಾತ್ರ ; idx > 0 ;  idx -= 1) {
	    vvp_bit4_t lv = lef.value(idx-1);
	    vvp_bit4_t rv = rig.value(idx-1);

	    ಆದರೆ (lv == rv)
		  ಮುಂದುವರಿ;

	    ಆದರೆ (lv == BIT4_1)
		  ಹಿಂತಿರುಗಿ BIT4_1;
	    ಇಲ್ಲದಿದ್ದರೆ
		  ಹಿಂತಿರುಗಿ BIT4_0;
      }

      ಹಿಂತಿರುಗಿ ಹೊರಹೋಗು_if_equal;
}

vvp_bit4_t compare_gtge_signed(ಸ್ಥಿರಾಂಕ vvp_vector4_t&a,
			       ಸ್ಥಿರಾಂಕ vvp_vector4_t&b,
			       vvp_bit4_t ಹೊರಹೋಗು_if_equal)
{
      ಪ್ರತಿಜ್ಞಾ(a.size() == b.size());

      ಚಿಹ್ನೆರಹಿತ sign_idx = a.size()-1;
      vvp_bit4_t a_sign = a.value(sign_idx);
      vvp_bit4_t b_sign = b.value(sign_idx);

      ಆದರೆ (bit4_is_xz(a_sign))
	    ಹಿಂತಿರುಗಿ BIT4_X;
      ಆದರೆ (bit4_is_xz(b_sign))
	    ಹಿಂತಿರುಗಿ BIT4_X;

      ಆದರೆ (a_sign == b_sign)
	    ಹಿಂತಿರುಗಿ compare_gtge(a, b, ಹೊರಹೋಗು_if_equal);

      ಆದರೆ (a.has_xz())
	    ಹಿಂತಿರುಗಿ BIT4_X;

      ಆದರೆ (b.has_xz())
	    ಹಿಂತಿರುಗಿ BIT4_X;

      ಆದರೆ(a_sign == BIT4_0)
	    ಹಿಂತಿರುಗಿ BIT4_1;
      ಇಲ್ಲದಿದ್ದರೆ
	    ಹಿಂತಿರುಗಿ BIT4_0;
}
