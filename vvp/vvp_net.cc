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
#جيڪڏهن_وصف_ٿيل CHECK_WITH_VALGRIND
# include  <valgrind/memcheck.h>
# include  <map>
# include  "sfunc.h"
# include  "udp.h"
# include  "ivl_alloc.h"
#جيڪڏهن_پورو

استعمال نطاق std;

/* This is the size of an unsigned long in bits. This is just a
   convenience macro. */
# define CPU_لفظ_بٽ (8*حجم(بنانشان ڊگهو))
# define چوٽي_ٻٽو_انگ (1UL << (CPU_لفظ_بٽ-1))

permaheap vvp_net_fun_t::ڍير;
permaheap vvp_net_fil_t::ڍير;

// Allocate around 1Megabyte/chunk.
ساكن ثابت size_t VVP_NET_CHUNK = 1024*1024/حجم(vvp_net_t);
ساكن vvp_net_t*vvp_net_alloc_جدول = عدم;
#جيڪڏهن_وصف_ٿيل CHECK_WITH_VALGRIND
ساكن vvp_net_t **vvp_net_pool = عدم;
ساكن بنانشان vvp_net_pool_ڳڻپ = 0;
#جيڪڏهن_پورو
ساكن size_t vvp_net_alloc_بچيل = 0;
// For statistics, count the vvp_nets allocated and the bytes of alloc
// chunks allocated.
بنانشان ڊگهو ڳڻپ_vvp_nets = 0;
size_t سائيز_vvp_nets = 0;

فراغ* vvp_net_t::عامل جديد (size_t size)
{
      أكد(size == حجم(vvp_net_t));
      إذا (vvp_net_alloc_بچيل == 0) {
	    vvp_net_alloc_جدول = ::جديد vvp_net_t[VVP_NET_CHUNK];
	    vvp_net_alloc_بچيل = VVP_NET_CHUNK;
	    سائيز_vvp_nets += size*VVP_NET_CHUNK;
#جيڪڏهن_وصف_ٿيل CHECK_WITH_VALGRIND
	    VALGRIND_MAKE_MEM_NOACCESS(vvp_net_alloc_جدول, size*VVP_NET_CHUNK);
	    VALGRIND_CREATE_MEMPOOL(vvp_net_alloc_جدول, 0, 0);
	    vvp_net_pool_ڳڻپ += 1;
	    vvp_net_pool = static_cast<vvp_net_t **>(realloc(vvp_net_pool,
	                   vvp_net_pool_ڳڻپ*حجم(vvp_net_t **)));
	    vvp_net_pool[vvp_net_pool_ڳڻپ-1] = vvp_net_alloc_جدول;
#جيڪڏهن_پورو
      }

      vvp_net_t*return_this = vvp_net_alloc_جدول;
#جيڪڏهن_وصف_ٿيل CHECK_WITH_VALGRIND
      VALGRIND_MEMPOOL_ALLOC(vvp_net_pool[vvp_net_pool_ڳڻپ-1],
                             return_this, size);
      return_this->pool = vvp_net_pool[vvp_net_pool_ڳڻپ-1];
#جيڪڏهن_پورو
      vvp_net_alloc_جدول += 1;
      vvp_net_alloc_بچيل -= 1;
      ڳڻپ_vvp_nets += 1;
      أرجع return_this;
}

#جيڪڏهن_وصف_ٿيل CHECK_WITH_VALGRIND
ساكن map<vvp_net_t*, منطقي> vvp_net_map;
ساكن map<sfunc_core*, منطقي> sfunc_map;
ساكن map<vvp_udp_fun_core*, منطقي> udp_map;
ساكن map<resolv_core*, منطقي> resolv_map;
ساكن vvp_net_t **local_net_pool = 0;
ساكن بنانشان local_net_pool_ڳڻپ = 0;

فراغ pool_local_net(vvp_net_t*net)
{
      local_net_pool_ڳڻپ += 1;
      local_net_pool = static_cast<vvp_net_t **>(realloc(local_net_pool,
                       local_net_pool_ڳڻپ*حجم(vvp_net_t **)));
      local_net_pool[local_net_pool_ڳڻپ-1] = net;
}

فراغ vvp_net_delete(vvp_net_t *شي)
{
      vvp_net_map[شي] = سچ;
      إذا (sfunc_core*tmp = dynamic_cast<sfunc_core*> (شي->fun)) {
	    sfunc_map[tmp] = سچ;
      }
      إذا (vvp_udp_fun_core*tmp = dynamic_cast<vvp_udp_fun_core*> (شي->fun)) {
	    udp_map[tmp] = سچ;
      }
      إذا (resolv_core*tmp = dynamic_cast<resolv_core*> (شي->fun)) {
	    resolv_map[tmp] = سچ;
      }
	/* Only delete static object variables. */
      إذا (vvp_fun_signal_object_sa*tmp = dynamic_cast<vvp_fun_signal_object_sa*> (شي->fun)) {
	    ميٽيو tmp;
      }
	/* Only delete static string variables. */
      إذا (vvp_fun_signal_string_sa*tmp = dynamic_cast<vvp_fun_signal_string_sa*> (شي->fun)) {
	    ميٽيو tmp;
      }
}

فراغ vvp_net_pool_delete()
{
      بنانشان ڊگهو vvp_nets_del = 0;

      لكل (بنانشان idx = 0; idx < local_net_pool_ڳڻپ; idx += 1) {
	    vvp_net_delete(local_net_pool[idx]);
      }
      حرر(local_net_pool);
      local_net_pool = 0;
      local_net_pool_ڳڻپ = 0;

      map<vvp_net_t*, منطقي>::iterator iter;
      لكل (iter = vvp_net_map.begin(); iter != vvp_net_map.پڇاڙي(); ++ iter ) {
	    vvp_nets_del += 1;
	    VALGRIND_MEMPOOL_FREE(iter->پهريون->pool, iter->پهريون);
      }
      vvp_net_map.clear();

      map<sfunc_core*, منطقي>::iterator siter;
      لكل (siter = sfunc_map.begin(); siter != sfunc_map.پڇاڙي(); ++ siter ) {
	    ميٽيو siter->پهريون;
      }
      sfunc_map.clear();

      map<vvp_udp_fun_core*, منطقي>::iterator uiter;
      لكل (uiter = udp_map.begin(); uiter != udp_map.پڇاڙي(); ++ uiter ) {
	    ميٽيو uiter->پهريون;
      }
      udp_map.clear();

      map<resolv_core*, منطقي>::iterator riter;
      لكل (riter = resolv_map.begin(); riter != resolv_map.پڇاڙي(); ++ riter ) {
	    ميٽيو riter->پهريون;
      }
      resolv_map.clear();

      إذا (RUNNING_ON_VALGRIND && (vvp_nets_del != ڳڻپ_vvp_nets)) {
	    fflush(عدم);
	    VALGRIND_PRINTF("Error: vvp missed deleting %ld of %lu net(s).",
	                    (ڊگهو) ڳڻپ_vvp_nets - vvp_nets_del,
	                    ڳڻپ_vvp_nets);
      }

      لكل (بنانشان idx = 0; idx < vvp_net_pool_ڳڻپ; idx += 1) {
	    VALGRIND_DESTROY_MEMPOOL(vvp_net_pool[idx]);
	    ::ميٽيو [] vvp_net_pool[idx];
      }
      حرر(vvp_net_pool);
      vvp_net_pool = عدم;
      vvp_net_pool_ڳڻپ = 0;
}
#جيڪڏهن_پورو

فراغ vvp_net_t::عامل ميٽيو(فراغ*)
{
      أكد(0);
}

vvp_net_t::vvp_net_t()
: ٻاهريون_2(vvp_net_ptr_t(0,0))
{
      fun = 0;
      fil = 0;
}

فراغ vvp_net_t::link(vvp_net_ptr_t پورٽ_منزل_link)
{
      vvp_net_t*net = پورٽ_منزل_link.ptr();

	// Connect nodes with vvp_fun_modpath_src to the head of the
	// linked list, so that vvp_fun_modpath_src are always evaluated
	// before their respective vvp_fun_modpath
      إذا (dynamic_cast<vvp_fun_modpath_src*>(net->fun)) {
	    net->پورٽ[پورٽ_منزل_link.پورٽ()] = ٻاهريون_2;
	    ٻاهريون_2 = پورٽ_منزل_link;
	// All other nodes connect after the last vvp_fun_modpath_src
      } وإلا {
	      // List is empty or first node has no vvp_fun_modpath_src
	      // then just insert the node into the head of linked list
	    إذا (ٻاهريون_2.ptr() == خالي_اشارو || ! dynamic_cast<vvp_fun_modpath_src*>(ٻاهريون_2.ptr()->fun)) {
		  net->پورٽ[پورٽ_منزل_link.پورٽ()] = ٻاهريون_2;
		  ٻاهريون_2 = پورٽ_منزل_link;
		  أرجع;
	    }

	    vvp_net_ptr_t cur = ٻاهريون_2;
	    vvp_net_ptr_t prev = vvp_net_ptr_t(خالي_اشارو,0);

	      // Traverse through linked list
	    طالما (dynamic_cast<vvp_fun_modpath_src*>(cur.ptr()->fun)) {
		  prev = cur;
		  cur = cur.ptr()->پورٽ[cur.پورٽ()];
		  إذا (! cur.ptr()) اكسر; // End of list
	    }

	    أكد(prev.ptr());

	      // Insert after last vvp_fun_modpath_src (or end of list)
	    net->پورٽ[پورٽ_منزل_link.پورٽ()] = cur;
	    prev.ptr()->پورٽ[prev.پورٽ()] = پورٽ_منزل_link;
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
      بنانشان net_پورٽ = dst_ptr.پورٽ();

      إذا (ٻاهريون_2 == dst_ptr) {
	      /* If the drive fan-out list starts with this pointer,
		 then the unlink is easy. Pull the list forward. */
	    ٻاهريون_2 = net->پورٽ[net_پورٽ];
      } وإلا إذا (! ٻاهريون_2.nil()) {
	      /* Scan the linked list, looking for the net_ptr_t
		 pointer *before* the one we wish to remove. */
	    vvp_net_ptr_t cur = ٻاهريون_2;
	    أكد(!cur.nil());
	    vvp_net_t*cur_net = cur.ptr();
	    بنانشان cur_پورٽ = cur.پورٽ();
	    طالما (cur_net && cur_net->پورٽ[cur_پورٽ] != dst_ptr) {
		  cur = cur_net->پورٽ[cur_پورٽ];
		  cur_net = cur.ptr();
		  cur_پورٽ = cur.پورٽ();
	    }
	      /* Unlink. */
	    إذا (cur_net) cur_net->پورٽ[cur_پورٽ] = net->پورٽ[net_پورٽ];
      }

      net->پورٽ[net_پورٽ] = vvp_net_ptr_t(0,0);
}

فراغ vvp_net_t::ڳڻپ_drivers(بنانشان idx, بنانشان counts[4])
{
      counts[0] = 0;
      counts[1] = 0;
      counts[2] = 0;
      counts[3] = 0;

        /* $countdrivers can only be used on wires. */
      vvp_wire_بنياد*wire=dynamic_cast<vvp_wire_بنياد*>(fil);
      أكد(wire);

      إذا (wire->is_forced(idx))
            counts[3] = 1;

        /* If the net has multiple drivers, we need to interrogate the
           resolver network to get the driven values. */
      إذا (resolv_core*resolver = dynamic_cast<resolv_core*>(fun)) {
            resolver->ڳڻپ_drivers(idx, counts);
            أرجع;
      }
      إذا (vvp_island_پورٽ*resolver = dynamic_cast<vvp_island_پورٽ*>(fun)) {
            resolver->ڳڻپ_drivers(idx, counts);
            أرجع;
      }

        /* If the functor is not a resolver, there is only one driver, so
           we can just interrogate the filter to get the driven value. */
      update_driver_counts(wire->driven_قدر(idx), counts);
}

فراغ vvp_net_fun_t::عامل ميٽيو(فراغ*)
{
#جيڪڏهن_وصف_نه_ٿيل CHECK_WITH_VALGRIND
      أكد(0);
#جيڪڏهن_پورو
}


vvp_net_fil_t::vvp_net_fil_t()
{
      force_link_ = 0;
      force_propagate_ = خطأ;
      ڳڻپ_filters += 1;
}

vvp_net_fil_t::~vvp_net_fil_t()
{
      أكد(force_link_ == 0);
}

vvp_net_fil_t::prop_t vvp_net_fil_t::filter_vec4(ثابت vvp_vector4_t&,
                                                 vvp_vector4_t&,
                                                 بنانشان, بنانشان)
{
      أرجع PROP;
}

vvp_net_fil_t::prop_t vvp_net_fil_t::filter_vec8(ثابت vvp_vector8_t&,
                                                 vvp_vector8_t&,
                                                 بنانشان, بنانشان)
{
      أرجع PROP;
}

vvp_net_fil_t::prop_t vvp_net_fil_t::filter_real(مزدوج&)
{
      أرجع PROP;
}

vvp_net_fil_t::prop_t vvp_net_fil_t::filter_string(ثابت string&)
{
      أرجع PROP;
}

vvp_net_fil_t::prop_t vvp_net_fil_t::filter_object(vvp_object_t&)
{
      أرجع PROP;
}

فراغ vvp_net_fil_t::force_نقاب(ثابت vvp_vector2_t&نقاب)
{
      إذا (force_نقاب_2.size() == 0)
	    force_نقاب_2 = vvp_vector2_t(vvp_vector2_t::FILL0, نقاب.size());

      أكد(force_نقاب_2.size() == نقاب.size());
      لكل (بنانشان idx = 0 ; idx < نقاب.size() ; idx += 1) {
	    إذا (نقاب.value(idx) == 0)
		  استمر;

	    force_نقاب_2.سيٽ_ٻٽو_انگ(idx, 1);
	    force_propagate_ = سچ;
      }
}

فراغ vvp_net_fil_t::release_نقاب(ثابت vvp_vector2_t&نقاب)
{
      إذا (force_نقاب_2.size() == 0)
	    أرجع;

      أكد(force_نقاب_2.size() == نقاب.size());
      لكل (بنانشان idx = 0 ; idx < نقاب.size() ; idx += 1) {
	    إذا (نقاب.value(idx))
		  force_نقاب_2.سيٽ_ٻٽو_انگ(idx, 0);
      }

      إذا (force_نقاب_2.is_zero())
	    force_نقاب_2 = vvp_vector2_t();
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
	    force_link_->پورٽ[3] = vvp_net_ptr_t(dst, 0);
	    force_link_->پورٽ[2] = vvp_net_ptr_t(0,0);
	    force_link_->fun = جديد vvp_fun_force;
      }

      force_unlink();
      أكد(force_link_->پورٽ[2] == vvp_net_ptr_t(0,0));

	// Use port[2] to hold the force source.
      force_link_->پورٽ[2] = vvp_net_ptr_t(src,0);

      vvp_net_ptr_t dst_ptr(force_link_, 0);
      src->link(dst_ptr);
}

فراغ vvp_net_fil_t::عامل ميٽيو(فراغ*)
{
      أكد(0);
}

فراغ vvp_net_fil_t::force_unlink(فراغ)
{
      إذا (force_link_ == 0) أرجع;
      vvp_net_t*src = force_link_->پورٽ[2].ptr();
      إذا (src == 0) أرجع;

      src->unlink(vvp_net_ptr_t(force_link_,0));
      force_link_->پورٽ[2] = vvp_net_ptr_t(0,0);
}

/* *** BIT operations *** */
vvp_bit4_t جوڙيو_with_حاصل(vvp_bit4_t a, vvp_bit4_t b, vvp_bit4_t&c)
{
      إذا (bit4_is_xz(a) || bit4_is_xz(b) || bit4_is_xz(c)) {
	    c = BIT4_X;
	    أرجع BIT4_X;
      }

	// NOTE: This relies on the facts that XZ values have been
	// weeded out, and that BIT4_1 is 1 and BIT4_0 is 0.
      صحيح جوڙ = (صحيح)a + (صحيح)b + (صحيح)c;

      بدل (جوڙ) {
	  حالة 0:
	      // c must already be 0.
	    أرجع BIT4_0;
	  حالة 1:
	    c = BIT4_0;
	    أرجع BIT4_1;
	  حالة 2:
	    c = BIT4_1;
	    أرجع BIT4_0;
	  حالة 3:
	    c = BIT4_1;
	    أرجع BIT4_1;
      }
      fprintf(stderr, "Incorrect result %d.\n", جوڙ);
      أكد(0);
      أرجع BIT4_X;
}

vvp_bit4_t scalar_منزل_bit4(PLI_INT32 scalar)
{
      بدل(scalar) {
	  حالة vpi0:
	    أرجع BIT4_0;
	  حالة vpi1:
	    أرجع BIT4_1;
	  حالة vpiX:
	    أرجع BIT4_X;
	  حالة vpiZ:
	    أرجع BIT4_Z;
      }
      fprintf(stderr, "Unsupported scalar value %d.\n", (صحيح)scalar);
      أكد(0);
      أرجع BIT4_X;
}

vvp_bit4_t عامل ^ (vvp_bit4_t a, vvp_bit4_t b)
{
      إذا (bit4_is_xz(a))
	    أرجع BIT4_X;
      إذا (bit4_is_xz(b))
	    أرجع BIT4_X;
      إذا (a == BIT4_0)
	    أرجع b;
      إذا (b == BIT4_0)
	    أرجع a;
      أرجع BIT4_0;
}

ostream& عامل<<(ostream&out, vvp_bit4_t ٻٽو_انگ)
{
      بدل (ٻٽو_انگ) {
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
      أرجع out;
}

تعريفنوع بنانشان ننڍو ڪنارو_t;

مضمن ڪنارو_t VVP_ڪنارو(vvp_bit4_t from, vvp_bit4_t to)
{
      أرجع 1 << ((from << 2) | to);
}

ثابت ڪنارو_t vvp_ڪنارو_posedge
      = VVP_ڪنارو(BIT4_0,BIT4_1)
      | VVP_ڪنارو(BIT4_0,BIT4_X)
      | VVP_ڪنارو(BIT4_0,BIT4_Z)
      | VVP_ڪنارو(BIT4_X,BIT4_1)
      | VVP_ڪنارو(BIT4_Z,BIT4_1)
      ;

ثابت ڪنارو_t vvp_ڪنارو_negedge
      = VVP_ڪنارو(BIT4_1,BIT4_0)
      | VVP_ڪنارو(BIT4_1,BIT4_X)
      | VVP_ڪنارو(BIT4_1,BIT4_Z)
      | VVP_ڪنارو(BIT4_X,BIT4_0)
      | VVP_ڪنارو(BIT4_Z,BIT4_0)
      ;

صحيح ڪنارو(vvp_bit4_t from, vvp_bit4_t to)
{
      ڪنارو_t نقاب = VVP_ڪنارو(from, to);
      إذا (نقاب & vvp_ڪنارو_posedge)
	    أرجع 1;
      إذا (نقاب & vvp_ڪنارو_negedge)
	    أرجع -1;
      أرجع 0;
}

بنانشان ڊگهو multiply_with_حاصل(بنانشان ڊگهو a, بنانشان ڊگهو b,
				  بنانشان ڊگهو&حاصل)
{
      ثابت بنانشان ڊگهو نقاب = (1UL << (CPU_لفظ_بٽ/2)) - 1;
      بنانشان ڊگهو a0 = a & نقاب;
      بنانشان ڊگهو a1 = (a >> (CPU_لفظ_بٽ/2)) & نقاب;
      بنانشان ڊگهو b0 = b & نقاب;
      بنانشان ڊگهو b1 = (b >> (CPU_لفظ_بٽ/2)) & نقاب;

      بنانشان ڊگهو tmp = a0 * b0;

      بنانشان ڊگهو r00 = tmp & نقاب;
      بنانشان ڊگهو c00 = (tmp >> (CPU_لفظ_بٽ/2)) & نقاب;

      tmp = a0 * b1;

      بنانشان ڊگهو r01 = tmp & نقاب;
      بنانشان ڊگهو c01 = (tmp >> (CPU_لفظ_بٽ/2)) & نقاب;

      tmp = a1 * b0;

      بنانشان ڊگهو r10 = tmp & نقاب;
      بنانشان ڊگهو c10 = (tmp >> (CPU_لفظ_بٽ/2)) & نقاب;

      tmp = a1 * b1;

      بنانشان ڊگهو r11 = tmp & نقاب;
      بنانشان ڊگهو c11 = (tmp >> (CPU_لفظ_بٽ/2)) & نقاب;

      بنانشان ڊگهو r1 = c00 + r01 + r10;
      بنانشان ڊگهو r2 = (r1 >> (CPU_لفظ_بٽ/2)) & نقاب;
      r1 &= نقاب;
      r2 += c01 + c10 + r11;
      بنانشان ڊگهو r3 = (r2 >> (CPU_لفظ_بٽ/2)) & نقاب;
      r2 &= نقاب;
      r3 += c11;
      r3 &= نقاب;

      حاصل = (r3 << (CPU_لفظ_بٽ/2)) + r2;
      أرجع (r1 << (CPU_لفظ_بٽ/2)) + r00;
}


فراغ vvp_send_vec8(vvp_net_ptr_t ptr, ثابت vvp_vector8_t&val)
{
      طالما (vvp_net_t*cur = ptr.ptr()) {
	    vvp_net_ptr_t ايندڙ_val = cur->پورٽ[ptr.پورٽ()];

	    إذا (cur->fun)
		  cur->fun->recv_vec8(ptr, val);

	    ptr = ايندڙ_val;
      }
}

فراغ vvp_send_real(vvp_net_ptr_t ptr, مزدوج val, vvp_context_t context)
{
      طالما (vvp_net_t*cur = ptr.ptr()) {
	    vvp_net_ptr_t ايندڙ_val = cur->پورٽ[ptr.پورٽ()];

	    إذا (cur->fun)
		  cur->fun->recv_real(ptr, val, context);

	    ptr = ايندڙ_val;
      }
}

ثابت vvp_vector4_t vvp_vector4_t::nil;

فراغ vvp_vector4_t::copy_بٽ(ثابت vvp_vector4_t&that)
{

      إذا (سائيز_2 == that.سائيز_2) {
	    إذا (سائيز_2 > بٽ_في_لفظ) {
		  بنانشان لفظ = (سائيز_2+بٽ_في_لفظ-1) / بٽ_في_لفظ;
		  لكل (بنانشان idx = 0 ;  idx < لفظ ;  idx += 1)
			abits_ptr_[idx] = that.abits_ptr_[idx];
		  لكل (بنانشان idx = 0 ;  idx < لفظ ;  idx += 1)
			bbits_ptr_[idx] = that.bbits_ptr_[idx];
	    } وإلا {
		  abits_val_ = that.abits_val_;
		  bbits_val_ = that.bbits_val_;
	    }
	    أرجع;
      }

	/* Now we know that the sizes of this and that are definitely
	   different. We can use that in code below. In any case, we
	   need to copy only the smaller of the sizes. */

	/* If source and destination are both short, then mask/copy
	   the bit values. */
      إذا (سائيز_2 <= بٽ_في_لفظ && that.سائيز_2 <= بٽ_في_لفظ) {
	    بنانشان بٽ_منزل_copy = (that.سائيز_2 < سائيز_2) ? that.سائيز_2 : سائيز_2;
	    بنانشان ڊگهو نقاب = ((بٽ_منزل_copy < بٽ_في_لفظ) ? (1UL << بٽ_منزل_copy) :
	                                                           (0UL)) - 1UL;
	    abits_val_ &= ~نقاب;
	    bbits_val_ &= ~نقاب;
	    abits_val_ |= that.abits_val_&نقاب;
	    bbits_val_ |= that.bbits_val_&نقاب;
	    أرجع;
      }

	/* Now we know that either source or destination are long. If
	   the destination is short, then mask/copy from the low word
	   of the long source. */
      إذا (سائيز_2 <= بٽ_في_لفظ) {
	    abits_val_ = that.abits_ptr_[0];
	    bbits_val_ = that.bbits_ptr_[0];
	    إذا (سائيز_2 < بٽ_في_لفظ) {
		  بنانشان ڊگهو نقاب = (1UL << سائيز_2) - 1UL;
		  abits_val_ &= نقاب;
		  bbits_val_ &= نقاب;
	    }
	    أرجع;
      }

	/* Now we know that the destination must be long. If the
	   source is short, then mask/copy from its value. */
      إذا (that.سائيز_2 <= بٽ_في_لفظ) {
	    بنانشان ڊگهو نقاب;
	    إذا (that.سائيز_2 < بٽ_في_لفظ) {
		  نقاب = (1UL << that.سائيز_2) - 1UL;
		  abits_ptr_[0] &= ~نقاب;
		  bbits_ptr_[0] &= ~نقاب;
	    } وإلا {
		  نقاب = -1UL;
	    }
	    abits_ptr_[0] |= that.abits_val_&نقاب;
	    bbits_ptr_[0] |= that.bbits_val_&نقاب;
	    أرجع;
      }

	/* Finally, we know that source and destination are long. copy
	   words until we get to the last. */
      بنانشان بٽ_منزل_copy = (that.سائيز_2 < سائيز_2) ? that.سائيز_2 : سائيز_2;
      بنانشان لفظ_2 = 0;
      طالما (بٽ_منزل_copy >= بٽ_في_لفظ) {
	    abits_ptr_[لفظ_2] = that.abits_ptr_[لفظ_2];
	    bbits_ptr_[لفظ_2] = that.bbits_ptr_[لفظ_2];
	    بٽ_منزل_copy -= بٽ_في_لفظ;
	    لفظ_2 += 1;
      }
      إذا (بٽ_منزل_copy > 0) {
	    بنانشان ڊگهو نقاب = (1UL << بٽ_منزل_copy) - 1UL;
	    abits_ptr_[لفظ_2] &= ~نقاب;
	    bbits_ptr_[لفظ_2] &= ~نقاب;
	    abits_ptr_[لفظ_2] |= that.abits_ptr_[لفظ_2] & نقاب;
	    bbits_ptr_[لفظ_2] |= that.bbits_ptr_[لفظ_2] & نقاب;
      }
}

/*
 * This function should ONLY BE CALLED FROM vvp_vector4_t::copy_from_,
 * as it performs part of that functions tasks.
 */
فراغ vvp_vector4_t::copy_from_big_(ثابت vvp_vector4_t&that)
{
      بنانشان لفظ = (سائيز_2+بٽ_في_لفظ-1) / بٽ_في_لفظ;
      abits_ptr_ = جديد بنانشان ڊگهو[2*لفظ];
      bbits_ptr_ = abits_ptr_ + لفظ;

      لكل (بنانشان idx = 0 ;  idx < لفظ ;  idx += 1)
	    abits_ptr_[idx] = that.abits_ptr_[idx];
      لكل (بنانشان idx = 0 ;  idx < لفظ ;  idx += 1)
	    bbits_ptr_[idx] = that.bbits_ptr_[idx];
}

/*
 * The copy_inverted_from_ method is just like the copy_from_ method,
 * except that we combine that with an invert. This allows the ~ and
 * the assignment to be blended in many common cases.
 */
فراغ vvp_vector4_t::copy_inverted_from_(ثابت vvp_vector4_t&that)
{
      سائيز_2 = that.سائيز_2;
      إذا (سائيز_2 > بٽ_في_لفظ) {
	    بنانشان لفظ = (سائيز_2+بٽ_في_لفظ-1) / بٽ_في_لفظ;
	    abits_ptr_ = جديد بنانشان ڊگهو[2*لفظ];
	    bbits_ptr_ = abits_ptr_ + لفظ;

	    بنانشان بچيل = سائيز_2;
	    بنانشان idx = 0;
	    طالما (بچيل >= بٽ_في_لفظ) {
		  abits_ptr_[idx] = that.bbits_ptr_[idx] | ~that.abits_ptr_[idx];
		  idx += 1;
		  بچيل -= بٽ_في_لفظ;
	    }
	    إذا (بچيل > 0) {
		  بنانشان ڊگهو نقاب = (1UL<<بچيل) - 1UL;
		  abits_ptr_[idx] = نقاب & (that.bbits_ptr_[idx] | ~that.abits_ptr_[idx]);
	    }

	    لكل (idx = 0 ;  idx < لفظ ;  idx += 1)
		  bbits_ptr_[idx] = that.bbits_ptr_[idx];

      } وإلا {
	    بنانشان ڊگهو نقاب = (سائيز_2<بٽ_في_لفظ)? (1UL<<سائيز_2)-1UL : -1UL;
	    abits_val_ = نقاب & (that.bbits_val_ | ~that.abits_val_);
	    bbits_val_ = that.bbits_val_;
      }
}

/* Make sure to set size_ before calling this routine. */
فراغ vvp_vector4_t::allocate_لفظ(بنانشان ڊگهو inita, بنانشان ڊگهو initb)
{
      إذا (سائيز_2 > بٽ_في_لفظ) {
	    بنانشان cnt = (سائيز_2 + بٽ_في_لفظ - 1) / بٽ_في_لفظ;
	    abits_ptr_ = جديد بنانشان ڊگهو[2*cnt];
	    bbits_ptr_ = abits_ptr_ + cnt;
	    لكل (بنانشان idx = 0 ;  idx < cnt ;  idx += 1)
		  abits_ptr_[idx] = inita;
	    لكل (بنانشان idx = 0 ;  idx < cnt ;  idx += 1)
		  bbits_ptr_[idx] = initb;

      } وإلا {
	    abits_val_ = inita;
	    bbits_val_ = initb;
      }
}

vvp_vector4_t::vvp_vector4_t(بنانشان سائيز_3, مزدوج val)
: سائيز_2(سائيز_3)
{
      منطقي is_neg = خطأ;
      مزدوج fraction;
      صحيح exponent;

	/* We return 'bx for a NaN or +/- infinity. */
      إذا (val != val || (val && (val == 0.5*val)))  {
	    allocate_لفظ(لفظ_X_ABITS, لفظ_X_BBITS);
	    أرجع;
      }

	/* Convert to a positive result. */
      إذا (val < 0.0) {
	    is_neg = سچ;
	    val = -val;
      }
      allocate_لفظ(لفظ_0_ABITS, لفظ_0_BBITS);

	/* Get the exponent and fractional part of the number. */
      fraction = frexp(val, &exponent);

	/* If the value is small enough just use lround(). */
      إذا (exponent < بٽ_في_لفظ-2) {
	    إذا (is_neg) هذا->invert();  // Invert the bits if negative.
	    ڊگهو sval = lround(val);
	    إذا (is_neg) sval = -sval;
	      /* This requires that 0 and 1 have the same bbit value. */
	    إذا (سائيز_2 > بٽ_في_لفظ) {
		  abits_ptr_[0] = sval;
	    } وإلا {
		  abits_val_ = sval;
	    }
	    أرجع;
      }

      بنانشان nwords = (exponent-1) / بٽ_في_لفظ;
      بنانشان my_لفظ = (سائيز_2 + بٽ_في_لفظ - 1) / بٽ_في_لفظ - 1;

      fraction = ldexp(fraction, (exponent-1) % بٽ_في_لفظ + 1);

	/* Skip any leading bits. */
      لكل (صحيح idx = (نشانوارو) nwords; idx > (نشانوارو) my_لفظ; idx -=1) {
	    بنانشان ڊگهو بٽ = (بنانشان ڊگهو) fraction;
	    fraction = fraction - (مزدوج) بٽ;
	    fraction = ldexp(fraction, بٽ_في_لفظ);
      }

	/* Convert the remaining bits as appropriate. */
      إذا (my_لفظ == 0) {
		  بنانشان ڊگهو بٽ = (بنانشان ڊگهو) fraction;
		  abits_val_ = بٽ;
		  fraction = fraction - (مزدوج) بٽ;
		    /* Round any fractional part up. */
		  إذا (fraction >= 0.5) *هذا += (int64_t) 1;
      } وإلا {
	    إذا (nwords < my_لفظ) my_لفظ = nwords;
	    لكل (صحيح idx = (نشانوارو)my_لفظ; idx >= 0; idx -= 1) {
		  بنانشان ڊگهو بٽ = (بنانشان ڊگهو) fraction;
		  abits_ptr_[idx] = بٽ;
		  fraction = fraction - (مزدوج) بٽ;
		  fraction = ldexp(fraction, بٽ_في_لفظ);
	    }
	      /* Round any fractional part up. */
	    إذا (fraction >= ldexp(0.5, بٽ_في_لفظ)) *هذا += (int64_t) 1;
       }

	/* Convert to a negative number if needed. */
      إذا (is_neg) {
	    هذا->invert();
	    *هذا += (int64_t) 1;
      }
}

vvp_vector4_t::vvp_vector4_t(ثابت vvp_vector4_t&that,
			    بنانشان adr, بنانشان wid)
{
	// Set up and initialize the destination.
      سائيز_2 = wid;
      allocate_لفظ(لفظ_X_ABITS, لفظ_X_BBITS);

	// Special case: selecting from far beyond the source vector,
	// to the result is all X bits. We're done.
      إذا (adr >= that.سائيز_2)
	    أرجع;

	// Special case: The source is not quite big enough to supply
	// all bits, so get the bits that we can. The remainder will
	// be left at BIT4_X.
      إذا ((adr + wid) > that.سائيز_2) {
	    بنانشان use_wid = that.سائيز_2 - adr;
	    لكل (بنانشان idx = 0 ; idx < use_wid ; idx += 1)
		  سيٽ_ٻٽو_انگ(idx, that.value(adr+idx));

	    أرجع;
      }

	// At the point, we know that the source part is entirely
	// contained in the source vector.
	// assert((adr + wid) <= that.size_);

      إذا (wid > بٽ_في_لفظ) {
	      /* In this case, the subvector and the source vector are
		 long. Do the transfer reasonably efficiently. */
	    بنانشان ptr = adr / بٽ_في_لفظ;
	    بنانشان ڊگهو off = adr % بٽ_في_لفظ;
	    بنانشان ڊگهو noff = بٽ_في_لفظ - off;
	    بنانشان ڊگهو lmask = (1UL << off) - 1UL;
	    بنانشان trans = 0;
	    بنانشان dst = 0;
	    طالما (trans < wid) {
		    // The low bits of the result.
		  abits_ptr_[dst] = (that.abits_ptr_[ptr] & ~lmask) >> off;
		  bbits_ptr_[dst] = (that.bbits_ptr_[ptr] & ~lmask) >> off;
		  trans += noff;

		  إذا (trans >= wid)
			اكسر;

		  ptr += 1;

		    // The high bits of the result. Skip this if the
		    // source and destination are perfectly aligned.
		  إذا (noff != بٽ_في_لفظ) {
			abits_ptr_[dst] |= (that.abits_ptr_[ptr]&lmask) << noff;
			bbits_ptr_[dst] |= (that.bbits_ptr_[ptr]&lmask) << noff;
			trans += off;
		  }

		  dst += 1;
	    }

      } وإلا إذا (that.سائيز_2 > بٽ_في_لفظ) {
	      /* In this case, the subvector fits in a single word,
		 but the source is large. */
	    بنانشان ptr = adr / بٽ_في_لفظ;
	    بنانشان ڊگهو off = adr % بٽ_في_لفظ;
	    بنانشان trans = بٽ_في_لفظ - off;
	    إذا (trans > wid)
		  trans = wid;

	    إذا (trans == بٽ_في_لفظ) {
		    // Very special case: Copy exactly 1 perfectly
		    // aligned word.
		  abits_val_ = that.abits_ptr_[ptr];
		  bbits_val_ = that.bbits_ptr_[ptr];

	    } وإلا {
		    // lmask is the low bits of the destination,
		    // masked into the source.
		  بنانشان ڊگهو lmask = (1UL<<trans) - 1UL;
		  lmask <<= off;

		    // The low bits of the result.
		  abits_val_ = (that.abits_ptr_[ptr] & lmask) >> off;
		  bbits_val_ = (that.bbits_ptr_[ptr] & lmask) >> off;

		  إذا (trans < wid) {
			  // If there are more bits, then get them
			  // from the bottom of the next word of the
			  // source.
			بنانشان ڊگهو hmask = (1UL << (wid-trans)) - 1UL;

			  // The high bits of the result.
			abits_val_ |= (that.abits_ptr_[ptr+1]&hmask) << trans;
			bbits_val_ |= (that.bbits_ptr_[ptr+1]&hmask) << trans;
		  }
	    }

      } وإلا إذا (سائيز_2 == بٽ_في_لفظ) {
	      /* We know that source and destination are short. If the
		 destination is a full word, then we know the copy is
		 aligned and complete. */
	    abits_val_ = that.abits_val_;
	    bbits_val_ = that.bbits_val_;

      } وإلا {
	      /* Finally, the source and destination vectors are both
		 short, so there is a single mask/shift/copy. */
	    بنانشان ڊگهو نقاب = (1UL << سائيز_2) - 1UL;
	    نقاب <<= adr;

	    abits_val_ = (that.abits_val_ & نقاب) >> adr;
	    bbits_val_ = (that.bbits_val_ & نقاب) >> adr;
      }

}

/*
 * Change the size of the vvp_vector4_t vector to the new size. Copy
 * the old values, as many as well fit, into the new vector.
 */
فراغ vvp_vector4_t::resize(بنانشان newsize, vvp_bit4_t pad_ٻٽو_انگ)
{
      إذا (سائيز_2 == newsize)
	    أرجع;

      بنانشان cnt = (سائيز_2 + بٽ_في_لفظ - 1) / بٽ_في_لفظ;
      بنانشان ڊگهو لفظ_pad_abits = 0;
      بنانشان ڊگهو لفظ_pad_bbits = 0;
      بدل (pad_ٻٽو_انگ) {
	  حالة BIT4_0:
	    لفظ_pad_abits = لفظ_0_ABITS;
	    لفظ_pad_bbits = لفظ_0_BBITS;
	    اكسر;
	  حالة BIT4_1:
	    لفظ_pad_abits = لفظ_1_ABITS;
	    لفظ_pad_bbits = لفظ_1_BBITS;
	    اكسر;
	  حالة BIT4_X:
	    لفظ_pad_abits = لفظ_X_ABITS;
	    لفظ_pad_bbits = لفظ_X_BBITS;
	    اكسر;
	  حالة BIT4_Z:
	    لفظ_pad_abits = لفظ_Z_ABITS;
	    لفظ_pad_bbits = لفظ_Z_BBITS;
	    اكسر;
      }

      إذا (newsize > بٽ_في_لفظ) {
	    بنانشان newcnt = (newsize + بٽ_في_لفظ - 1) / بٽ_في_لفظ;
	    إذا (newcnt == cnt) {
		    // If the word count doesn't change, then there is
		    // no need for re-allocation so we are done now.
		  إذا (newsize > سائيز_2) {
			إذا (بنانشان fill = سائيز_2 % بٽ_في_لفظ) {
			      abits_ptr_[cnt-1] &= ~((-1UL) << fill);
			      bbits_ptr_[cnt-1] &= ~((-1UL) << fill);
			      abits_ptr_[cnt-1] |= لفظ_pad_abits << fill;
			      bbits_ptr_[cnt-1] |= لفظ_pad_bbits << fill;
			}
		  }
		  سائيز_2 = newsize;
		  أرجع;
	    }

	    بنانشان ڊگهو*newbits = جديد بنانشان ڊگهو[2*newcnt];

	    إذا (cnt > 1) {
		  بنانشان trans = cnt;
		  إذا (trans > newcnt)
			trans = newcnt;

		  لكل (بنانشان idx = 0 ;  idx < trans ;  idx += 1)
			newbits[idx] = abits_ptr_[idx];
		  لكل (بنانشان idx = 0 ;  idx < trans ;  idx += 1)
			newbits[newcnt+idx] = bbits_ptr_[idx];

		  ميٽيو[]abits_ptr_;

	    } وإلا {
		  newbits[0] = abits_val_;
		  newbits[newcnt] = bbits_val_;
	    }

	    إذا (newsize > سائيز_2) {
		  إذا (بنانشان fill = سائيز_2 % بٽ_في_لفظ) {
			newbits[cnt-1] &= ~((-1UL) << fill);
			newbits[cnt-1] |= لفظ_pad_abits << fill;
			newbits[newcnt+cnt-1] &= ~((-1UL) << fill);
			newbits[newcnt+cnt-1] |= لفظ_pad_bbits << fill;
		  }
		  لكل (بنانشان idx = cnt ;  idx < newcnt ;  idx += 1)
			newbits[idx] = لفظ_pad_abits;
		  لكل (بنانشان idx = cnt ;  idx < newcnt ;  idx += 1)
			newbits[newcnt+idx] = لفظ_pad_bbits;
	    }

	    سائيز_2 = newsize;
	    abits_ptr_ = newbits;
	    bbits_ptr_ = newbits + newcnt;

      } وإلا {
	    إذا (cnt > 1) {
		  بنانشان ڊگهو newvala = abits_ptr_[0];
		  بنانشان ڊگهو newvalb = bbits_ptr_[0];
		  ميٽيو[]abits_ptr_;
		  abits_val_ = newvala;
		  bbits_val_ = newvalb;
	    }

	    إذا (newsize > سائيز_2) {
		  abits_val_ &= ~((-1UL) << سائيز_2);
		  bbits_val_ &= ~((-1UL) << سائيز_2);
		  abits_val_ |= لفظ_pad_abits << سائيز_2;
		  bbits_val_ |= لفظ_pad_bbits << سائيز_2;
	    }

	    سائيز_2 = newsize;
      }
}


بنانشان ڊگهو* vvp_vector4_t::subarray(بنانشان adr, بنانشان wid, منطقي xz_منزل_0) ثابت
{
      ثابت بنانشان BIT2_في_لفظ = 8*حجم(بنانشان ڊگهو);
      بنانشان awid = (wid + BIT2_في_لفظ - 1) / (BIT2_في_لفظ);
      بنانشان ڊگهو*val = جديد بنانشان ڊگهو[awid];

      لكل (بنانشان idx = 0 ;  idx < awid ;  idx += 1)
	    val[idx] = 0;

      إذا (سائيز_2 <= بٽ_في_لفظ) {
	      /* Handle the special case that the array is small. The
		 entire value of the vector4 is within the xbits_val_
		 so we know that the result is a single word, the
		 source is a single word, and we just have to loop
		 through that word. */
	    بنانشان ڊگهو atmp = abits_val_ >> adr;
	    بنانشان ڊگهو btmp = bbits_val_ >> adr;
	    إذا (wid < BIT2_في_لفظ) {
		  atmp &= (1UL << wid) - 1;
		  btmp &= (1UL << wid) - 1;
	    }
	    إذا (btmp) {
		  إذا (xz_منزل_0)
			atmp &= ~btmp;
		  وإلا
			اذهب x_ٻاهريون;
	    }

	    val[0] = atmp;

      } وإلا {

	    بنانشان val_ptr = 0;
	    بنانشان val_off = 0;

	      /* Get the first word we are scanning. We may in fact be
		 somewhere in the middle of that word. */
	    طالما (wid > 0) {
		  بنانشان ڊگهو atmp = abits_ptr_[adr/بٽ_في_لفظ];
		  بنانشان ڊگهو btmp = bbits_ptr_[adr/بٽ_في_لفظ];
		  بنانشان ڊگهو off = adr%بٽ_في_لفظ;
		  atmp >>= off;
		  btmp >>= off;

		  بنانشان ڊگهو trans = بٽ_في_لفظ - off;
		  إذا (trans > (BIT2_في_لفظ - val_off))
			trans = BIT2_في_لفظ - val_off;
		  إذا (wid < trans)
			trans = wid;
		  إذا (trans < BIT2_في_لفظ) {
			atmp &= (1UL << trans) - 1;
			btmp &= (1UL << trans) - 1;
		  }
		  إذا (btmp) {
			إذا (xz_منزل_0)
			      atmp &= ~btmp;
			وإلا
			      اذهب x_ٻاهريون;
		  }

		  val[val_ptr] |= atmp << val_off;
		  adr += trans;
		  wid -= trans;
		  val_off += trans;
		  إذا (val_off == BIT2_في_لفظ) {
			val_ptr += 1;
			val_off = 0;
		  }
	    }
      }

      أرجع val;

 x_ٻاهريون:
      ميٽيو[]val;
      أرجع 0;
}

فراغ vvp_vector4_t::setarray(بنانشان adr, بنانشان wid, ثابت بنانشان ڊگهو*val)
{
      أكد(adr+wid <= سائيز_2);

      ثابت بنانشان BIT2_في_لفظ = 8*حجم(بنانشان ڊگهو);

      إذا (سائيز_2 <= بٽ_في_لفظ) {
	      // We know here that both the source and the target are
	      // within a single word. Write the bits into the
	      // abits_val_ directly.

	    أكد(BIT2_في_لفظ <= بٽ_في_لفظ);
	    بنانشان ڊگهو lmask = (1UL << adr) - 1UL;
	    بنانشان ڊگهو hmask = ((adr+wid) < بٽ_في_لفظ)
		  ? -1UL << (adr+wid)
		  : 0;
	    بنانشان ڊگهو نقاب = ~(hmask | lmask);

	    abits_val_ &= ~نقاب;
	    bbits_val_ &= ~نقاب;

	    abits_val_ |= نقاب & (val[0] << adr);

      } وإلا {
	      // The general case, there are multiple words of
	      // destination, and possibly multiple words of source
	      // data. Shift and mask as we go.
	    بنانشان off = adr % بٽ_في_لفظ;
	    بنانشان ptr = adr / بٽ_في_لفظ;
	    بنانشان val_off = 0;
	    بنانشان val_ptr = 0;
	    طالما (wid > 0) {
		  بنانشان trans = wid;
		  إذا (trans > (BIT2_في_لفظ-val_off))
			trans = BIT2_في_لفظ-val_off;
		  إذا (trans > (بٽ_في_لفظ-off))
			trans = بٽ_في_لفظ-off;

		  بنانشان ڊگهو lmask = (1UL << off) - 1UL;
		  بنانشان ڊگهو hmask = ((off+trans) < بٽ_في_لفظ)
			? -1UL << (off+trans)
			: 0;
		  بنانشان ڊگهو نقاب = ~(hmask | lmask);

		  abits_ptr_[ptr] &= ~نقاب;
		  bbits_ptr_[ptr] &= ~نقاب;
		  إذا (val_off >= off)
			abits_ptr_[ptr] |= نقاب & (val[val_ptr] >> (val_off-off));
		  وإلا
			abits_ptr_[ptr] |= نقاب & (val[val_ptr] << (off-val_off));

		  wid -= trans;
		  val_off += trans;
		  إذا (val_off == BIT2_في_لفظ) {
			val_off = 0;
			val_ptr += 1;
		  }
		  off += trans;
		  إذا (off == بٽ_في_لفظ) {
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
منطقي vvp_vector4_t::سيٽ_vec(بنانشان adr, ثابت vvp_vector4_t&that)
{
      أكد(adr+that.سائيز_2  <= سائيز_2);
      منطقي diff_flag = خطأ;

      إذا (سائيز_2 <= بٽ_في_لفظ) {

	      /* The destination vector (me!) is within a bits_val_
		 word, so the subvector is certainly within a
		 bits_val_ word. Therefore, the entire operation is a
		 matter of writing the bits of that into the addressed
		 bits of this. The mask below is calculated to be 1
		 for all the bits that are to come from that. Do the
		 job by some shifting, masking and OR. */

	    بنانشان ڊگهو lmask = (1UL << adr) - 1;
	    بنانشان ڊگهو hmask;
	    بنانشان ڊگهو hshift = adr+that.سائيز_2;
	    إذا (hshift >= بٽ_في_لفظ)
		  hmask = -1UL;
	    وإلا
		  hmask = (1UL << (adr+that.سائيز_2)) - 1;
	    بنانشان ڊگهو نقاب = hmask & ~lmask;

	    بنانشان ڊگهو tmp = (that.abits_val_<<adr)&نقاب;
	    إذا ((abits_val_&نقاب) != tmp) {
		  diff_flag = سچ;
		  abits_val_ = (abits_val_ & ~نقاب) | tmp;
	    }
	    tmp = (that.bbits_val_<<adr) & نقاب;
	    إذا ((bbits_val_&نقاب) != tmp) {
		  diff_flag = سچ;
		  bbits_val_ = (bbits_val_ & ~نقاب) | tmp;
	    }

      } وإلا إذا (that.سائيز_2 <= بٽ_في_لفظ) {

	      /* This vector is more than a word, but that vector is
		 still small. Write into the destination, possibly
		 spanning two destination words, depending on whether
		 the source vector spans a word transition. */
	    بنانشان ڊگهو dptr = adr / بٽ_في_لفظ;
	    بنانشان ڊگهو doff = adr % بٽ_في_لفظ;

	    بنانشان ڊگهو lmask = (1UL << doff) - 1;
	    بنانشان ڊگهو hshift = doff+that.سائيز_2;
	    بنانشان ڊگهو hmask;
	    إذا (hshift >= بٽ_في_لفظ)
		  hmask = -1UL;
	    وإلا
		  hmask = (1UL << hshift) - 1UL;

	    بنانشان ڊگهو نقاب = hmask & ~lmask;
	    بنانشان ڊگهو tmp;

	    tmp = (that.abits_val_ << doff) & نقاب;
	    إذا ((abits_ptr_[dptr] & نقاب) != tmp) {
		  diff_flag = سچ;
		  abits_ptr_[dptr] = (abits_ptr_[dptr] & ~نقاب) | tmp;
	    }
	    tmp = (that.bbits_val_ << doff) & نقاب;
	    إذا ((bbits_ptr_[dptr] & نقاب) != tmp) {
		  diff_flag = سچ;
		  bbits_ptr_[dptr] = (bbits_ptr_[dptr] & ~نقاب) | tmp;
	    }

	    إذا ((doff + that.سائيز_2) > بٽ_في_لفظ) {
		  بنانشان پڇ = doff + that.سائيز_2 - بٽ_في_لفظ;
		  نقاب = (1UL << پڇ) - 1;

		  dptr += 1;
		  tmp = (that.abits_val_ >> (that.سائيز_2-پڇ)) & نقاب;
		  إذا ((abits_ptr_[dptr] & نقاب) != tmp) {
			diff_flag = سچ;
			abits_ptr_[dptr] = (abits_ptr_[dptr] & ~نقاب) | tmp;
		  }
		  tmp = (that.bbits_val_ >> (that.سائيز_2-پڇ)) & نقاب;
		  إذا ((bbits_ptr_[dptr] & نقاب) != tmp) {
			diff_flag = سچ;
			bbits_ptr_[dptr] = (bbits_ptr_[dptr] & ~نقاب) | tmp;
		  }
	    }

      } وإلا إذا (adr%بٽ_في_لفظ == 0) {

	      /* In this case, both vectors are long, but the
		 destination is neatly aligned. That means all but the
		 last word can be simply copied with no masking. */

	    بنانشان remain = that.سائيز_2;
	    بنانشان sptr = 0;
	    بنانشان dptr = adr / بٽ_في_لفظ;
	    طالما (remain >= بٽ_في_لفظ) {
		  إذا (abits_ptr_[dptr] != that.abits_ptr_[sptr]) {
			diff_flag = سچ;
			abits_ptr_[dptr] = that.abits_ptr_[sptr];
		  }
		  إذا (bbits_ptr_[dptr] != that.bbits_ptr_[sptr]) {
			diff_flag = سچ;
			bbits_ptr_[dptr] = that.bbits_ptr_[sptr];
		  }
		  dptr += 1;
		  sptr += 1;
		  remain -= بٽ_في_لفظ;
	    }

	    إذا (remain > 0) {
		  بنانشان ڊگهو نقاب = (1UL << remain) - 1;
		  بنانشان ڊگهو tmp;

		  tmp = that.abits_ptr_[sptr] & نقاب;
		  إذا ((abits_ptr_[dptr] & نقاب) != tmp) {
			diff_flag = سچ;
			abits_ptr_[dptr] = (abits_ptr_[dptr] & ~نقاب) | tmp;
		  }
		  tmp = that.bbits_ptr_[sptr] & نقاب;
		  إذا ((bbits_ptr_[dptr] & نقاب) != tmp) {
			diff_flag = سچ;
			bbits_ptr_[dptr] = (bbits_ptr_[dptr] & ~نقاب) | tmp;
		  }
	    }

      } وإلا {

	      /* We know that there are two long vectors, and we know
		 that the destination is definitely NOT aligned. */

	    بنانشان remain = that.سائيز_2;
	    بنانشان sptr = 0;
	    بنانشان dptr = adr / بٽ_في_لفظ;
	    بنانشان doff = adr % بٽ_في_لفظ;
	    بنانشان ڊگهو lmask = (1UL << doff) - 1;
	    بنانشان ndoff = بٽ_في_لفظ - doff;
	    طالما (remain >= بٽ_في_لفظ) {
		  بنانشان ڊگهو tmp;

		  tmp = (that.abits_ptr_[sptr] << doff) & ~lmask;
		  إذا ((abits_ptr_[dptr] & ~lmask) != tmp) {
			diff_flag = سچ;
			abits_ptr_[dptr] = (abits_ptr_[dptr] & lmask) | tmp;
		  }
		  tmp = (that.bbits_ptr_[sptr] << doff) & ~lmask;
		  إذا ((bbits_ptr_[dptr] & ~lmask) != tmp) {
			diff_flag = سچ;
			bbits_ptr_[dptr] = (bbits_ptr_[dptr] & lmask) | tmp;
		  }
		  dptr += 1;

		  tmp = (that.abits_ptr_[sptr] >> ndoff) & lmask;
		  إذا ((abits_ptr_[dptr] & lmask) != tmp) {
			diff_flag = سچ;
			abits_ptr_[dptr] = (abits_ptr_[dptr] & ~lmask) | tmp;
		  }
		  tmp = (that.bbits_ptr_[sptr] >> ndoff) & lmask;
		  إذا ((bbits_ptr_[dptr] & lmask) != tmp) {
			diff_flag = سچ;
			bbits_ptr_[dptr] = (bbits_ptr_[dptr] & ~lmask) | tmp;
		  }

		  remain -= بٽ_في_لفظ;
		  sptr += 1;
	    }

	    إذا (remain > 0) {
		  بنانشان ڊگهو hshift = doff+remain;
		  بنانشان ڊگهو hmask;
		  إذا (hshift >= بٽ_في_لفظ)
			hmask = -1UL;
		  وإلا
			hmask = (1UL << (doff+remain)) - 1;

		  بنانشان ڊگهو نقاب = hmask & ~lmask;
		  بنانشان ڊگهو tmp;

		  tmp = (that.abits_ptr_[sptr] << doff) & نقاب;
		  إذا ((abits_ptr_[dptr] & نقاب) != tmp) {
			diff_flag = سچ;
			abits_ptr_[dptr] = (abits_ptr_[dptr] & ~نقاب) | tmp;
		  }
		  tmp = (that.bbits_ptr_[sptr] << doff) & نقاب;
		  إذا ((bbits_ptr_[dptr] & نقاب) != tmp) {
			diff_flag = سچ;
			bbits_ptr_[dptr] = (bbits_ptr_[dptr] & ~نقاب) | tmp;
		  }

		  إذا ((doff + remain) > بٽ_في_لفظ) {
			بنانشان پڇ = doff + remain - بٽ_في_لفظ;
			إذا (پڇ >= بٽ_في_لفظ)
			      نقاب = -1UL;
			وإلا
			      نقاب = (1UL << پڇ) - 1;

			dptr += 1;

			tmp = (that.abits_ptr_[sptr] >> (remain-پڇ))&نقاب;
			إذا ((abits_ptr_[dptr] & نقاب) != tmp) {
			      diff_flag = سچ;
			      abits_ptr_[dptr] = (abits_ptr_[dptr] & ~نقاب) | tmp;
			}
			tmp = (that.bbits_ptr_[sptr] >> (remain-پڇ))&نقاب;
			إذا ((bbits_ptr_[dptr] & نقاب) != tmp) {
			      diff_flag = سچ;
			      bbits_ptr_[dptr] = (bbits_ptr_[dptr] & ~نقاب) | tmp;
			}
		  }
	    }
      }

      أرجع diff_flag;
}

/*
 * Add that vector to this vector. Do it in the Verilog way, which
 * means if we detect any X or Z bits, change the entire results to
 * all X.
 *
 * Assume both vectors are the same size.
 */
فراغ vvp_vector4_t::جوڙيو(ثابت vvp_vector4_t&that)
{
      أكد(سائيز_2 == that.سائيز_2);

      إذا (سائيز_2 < بٽ_في_لفظ) {
	    بنانشان ڊگهو نقاب = ~(-1UL << سائيز_2);
	    إذا ((bbits_val_|that.bbits_val_) & نقاب) {
		  abits_val_ |= نقاب;
		  bbits_val_ |= نقاب;
		  أرجع;
	    }

	    abits_val_ += that.abits_val_;
	    abits_val_ &= نقاب;
	    أرجع;
      }

      إذا (سائيز_2 == بٽ_في_لفظ) {
	    إذا (bbits_val_ | that.bbits_val_) {
		  abits_val_ = لفظ_X_ABITS;
		  bbits_val_ = لفظ_X_BBITS;
	    } وإلا {
		  abits_val_ += that.abits_val_;
	    }
	    أرجع;
      }

      صحيح cnt = سائيز_2 / بٽ_في_لفظ;
      بنانشان ڊگهو حاصل = 0;
      لكل (صحيح idx = 0 ; idx < cnt ; idx += 1) {
	    إذا (bbits_ptr_[idx] | that.bbits_ptr_[idx])
		  اذهب x_ٻاهريون;

	    abits_ptr_[idx] = جوڙيو_with_حاصل(abits_ptr_[idx], that.abits_ptr_[idx], حاصل);
      }

      إذا (بنانشان پڇ = سائيز_2 % بٽ_في_لفظ) {
	    بنانشان ڊگهو نقاب = ~( -1UL << پڇ );
	    إذا ((bbits_ptr_[cnt] | that.bbits_ptr_[cnt])&نقاب)
		  اذهب x_ٻاهريون;

	    abits_ptr_[cnt] = جوڙيو_with_حاصل(abits_ptr_[cnt], that.abits_ptr_[cnt], حاصل);
	    abits_ptr_[cnt] &= نقاب;
      }

      أرجع;

 x_ٻاهريون:
      لكل (صحيح idx = 0 ; idx < cnt ; idx += 1) {
	    abits_ptr_[idx] = لفظ_X_ABITS;
	    bbits_ptr_[idx] = لفظ_X_BBITS;
      }
      إذا (بنانشان پڇ = سائيز_2%بٽ_في_لفظ) {
	    بنانشان ڊگهو نقاب = ~( -1UL << پڇ );
	    abits_ptr_[cnt] = لفظ_X_ABITS&نقاب;
	    bbits_ptr_[cnt] = لفظ_X_BBITS&نقاب;
      }
}

فراغ vvp_vector4_t::sub(ثابت vvp_vector4_t&that)
{
      أكد(سائيز_2 == that.سائيز_2);

      إذا (سائيز_2 < بٽ_في_لفظ) {
	    بنانشان ڊگهو نقاب = ~(-1UL << سائيز_2);
	    إذا ((bbits_val_|that.bbits_val_) & نقاب) {
		  abits_val_ |= نقاب;
		  bbits_val_ |= نقاب;
		  أرجع;
	    }

	    abits_val_ -= that.abits_val_;
	    abits_val_ &= نقاب;
	    أرجع;
      }

      إذا (سائيز_2 == بٽ_في_لفظ) {
	    إذا (bbits_val_ | that.bbits_val_) {
		  abits_val_ = لفظ_X_ABITS;
		  bbits_val_ = لفظ_X_BBITS;
	    } وإلا {
		  abits_val_ -= that.abits_val_;
	    }
	    أرجع;
      }

      صحيح cnt = سائيز_2 / بٽ_في_لفظ;
      بنانشان ڊگهو حاصل = 1;
      لكل (صحيح idx = 0 ; idx < cnt ; idx += 1) {
	    إذا (bbits_ptr_[idx] | that.bbits_ptr_[idx])
		  اذهب x_ٻاهريون;

	    abits_ptr_[idx] = جوڙيو_with_حاصل(abits_ptr_[idx], ~that.abits_ptr_[idx], حاصل);
      }

      إذا (بنانشان پڇ = سائيز_2 % بٽ_في_لفظ) {
	    بنانشان ڊگهو نقاب = ~( -1UL << پڇ );
	    إذا ((bbits_ptr_[cnt] | that.bbits_ptr_[cnt])&نقاب)
		  اذهب x_ٻاهريون;

	    abits_ptr_[cnt] = جوڙيو_with_حاصل(abits_ptr_[cnt], ~that.abits_ptr_[cnt], حاصل);
	    abits_ptr_[cnt] &= نقاب;
      }

      أرجع;

 x_ٻاهريون:
      لكل (صحيح idx = 0 ; idx < cnt ; idx += 1) {
	    abits_ptr_[idx] = لفظ_X_ABITS;
	    bbits_ptr_[idx] = لفظ_X_BBITS;
      }
      إذا (بنانشان پڇ = سائيز_2%بٽ_في_لفظ) {
	    بنانشان ڊگهو نقاب = ~( -1UL << پڇ );
	    abits_ptr_[cnt] = لفظ_X_ABITS&نقاب;
	    bbits_ptr_[cnt] = لفظ_X_BBITS&نقاب;
      }

}

فراغ vvp_vector4_t::mov(بنانشان dst, بنانشان src, بنانشان cnt)
{
      أكد(dst+cnt <= سائيز_2);
      أكد(src+cnt <= سائيز_2);

      إذا (سائيز_2 <= بٽ_في_لفظ) {
	    بنانشان ڊگهو vmask = (1UL << cnt) - 1;
	    بنانشان ڊگهو tmp;

	    tmp = (abits_val_ >> src) & vmask;
	    abits_val_ &= ~ (vmask << dst);
	    abits_val_ |= tmp << dst;

	    tmp = (bbits_val_ >> src) & vmask;
	    bbits_val_ &= ~ (vmask << dst);
	    bbits_val_ |= tmp << dst;

      } وإلا {
	    بنانشان sptr = src / بٽ_في_لفظ;
	    بنانشان dptr = dst / بٽ_في_لفظ;
	    بنانشان soff = src % بٽ_في_لفظ;
	    بنانشان doff = dst % بٽ_في_لفظ;

	    طالما (cnt > 0) {
		  بنانشان trans = cnt;
		  إذا ((soff+trans) > بٽ_في_لفظ)
			trans = بٽ_في_لفظ - soff;

		  إذا ((doff+trans) > بٽ_في_لفظ)
			trans = بٽ_في_لفظ - doff;

		  إذا (trans == بٽ_في_لفظ) {
			  // Special case: the transfer count is
			  // exactly an entire word. For this to be
			  // true, it must also be true that the
			  // pointers are aligned. The work is easy,
			abits_ptr_[dptr] = abits_ptr_[sptr];
			bbits_ptr_[dptr] = bbits_ptr_[sptr];
			dptr += 1;
			sptr += 1;
			cnt -= بٽ_في_لفظ;
			استمر;
		  }

		    // Here we know that either the source or
		    // destination is unaligned, and also we know that
		    // the count is less than a full word.
		  بنانشان ڊگهو vmask = (1UL << trans) - 1;
		  بنانشان ڊگهو tmp;

		  tmp = (abits_ptr_[sptr] >> soff) & vmask;
		  abits_ptr_[dptr] &= ~ (vmask << doff);
		  abits_ptr_[dptr] |= tmp << doff;

		  tmp = (bbits_ptr_[sptr] >> soff) & vmask;
		  bbits_ptr_[dptr] &= ~ (vmask << doff);
		  bbits_ptr_[dptr] |= tmp << doff;

		  cnt -= trans;
		  soff += trans;
		  إذا (soff >= بٽ_في_لفظ) {
			soff = 0;
			sptr += 1;
		  }
		  doff += trans;
		  إذا (doff >= بٽ_في_لفظ) {
			doff = 0;
			dptr += 1;
		  }
	    }
      }
}

فراغ vvp_vector4_t::mul(ثابت vvp_vector4_t&that)
{
      أكد(سائيز_2 == that.سائيز_2);

      إذا (سائيز_2 < بٽ_في_لفظ) {
	    بنانشان ڊگهو نقاب = ~(-1UL << سائيز_2);
	    إذا ((bbits_val_|that.bbits_val_) & نقاب) {
		  abits_val_ |= نقاب;
		  bbits_val_ |= نقاب;
		  أرجع;
	    }

	    abits_val_ *= that.abits_val_;
	    abits_val_ &= نقاب;
	    أرجع;
      }

      إذا (سائيز_2 == بٽ_في_لفظ) {
	    إذا (bbits_val_ || that.bbits_val_) {
		  abits_val_ = لفظ_X_ABITS;
		  bbits_val_ = لفظ_X_BBITS;
	    } وإلا {
		  abits_val_ *= that.abits_val_;
	    }
	    أرجع;
      }

      ثابت صحيح cnt = (سائيز_2+بٽ_في_لفظ-1) / بٽ_في_لفظ;

      بنانشان ڊگهو نقاب;
      إذا (بنانشان پڇ = سائيز_2%بٽ_في_لفظ) {
	    نقاب = ~( -1UL << پڇ );
      } وإلا {
	    نقاب = ~0UL;
      }

	// Check for any XZ values ahead of time in a first pass. If
	// we find any, then force the entire result to be X and be
	// done.
      لكل (صحيح idx = 0 ; idx < cnt ; idx += 1) {
	    بنانشان ڊگهو lval = bbits_ptr_[idx];
	    بنانشان ڊگهو rval = that.bbits_ptr_[idx];
	    إذا (idx == (cnt-1)) {
		  lval &= نقاب;
		  rval &= نقاب;
	    }
	    إذا (lval || rval) {
		  لكل (صحيح xdx = 0 ; xdx < cnt-1 ; xdx += 1) {
			abits_ptr_[xdx] = لفظ_X_ABITS;
			bbits_ptr_[xdx] = لفظ_X_BBITS;
		  }
		  abits_ptr_[cnt-1] = لفظ_X_ABITS & نقاب;
		  bbits_ptr_[cnt-1] = لفظ_X_BBITS & نقاب;
		  أرجع;
	    }
      }

	// Calculate the result into a res array. We need to keep is
	// separate from the "this" array because we are making
	// multiple passes.
      بنانشان ڊگهو*res = جديد بنانشان ڊگهو[cnt];
      لكل (صحيح idx = 0 ; idx < cnt ; idx += 1)
	    res[idx] = 0;

      لكل (صحيح mul_a = 0 ; mul_a < cnt ; mul_a += 1) {
	    بنانشان ڊگهو lval = abits_ptr_[mul_a];
	    إذا (mul_a == (cnt-1))
		  lval &= نقاب;

	    لكل (صحيح mul_b = 0 ; mul_b < (cnt-mul_a) ; mul_b += 1) {
		  بنانشان ڊگهو rval = that.abits_ptr_[mul_b];
		  إذا (mul_b == (cnt-1))
			rval &= نقاب;

		  بنانشان ڊگهو جوڙ;
		  بنانشان ڊگهو tmp = multiply_with_حاصل(lval, rval, جوڙ);
		  صحيح بنياد = mul_a + mul_b;
		  بنانشان ڊگهو حاصل = 0;
		  res[بنياد] = جوڙيو_with_حاصل(res[بنياد], tmp, حاصل);
		  لكل (صحيح جوڙيو_idx = بنياد+1 ; جوڙيو_idx < cnt ; جوڙيو_idx += 1) {
			res[جوڙيو_idx] = جوڙيو_with_حاصل(res[جوڙيو_idx], جوڙ, حاصل);
			جوڙ = 0;
		  }
	    }
      }

	// Replace the "this" value with the calculated result. We
	// know a-priori that the bbits are zero and unchanged.
      res[cnt-1] &= نقاب;
      لكل (صحيح idx = 0 ; idx < cnt ; idx += 1)
	    abits_ptr_[idx] = res[idx];

      ميٽيو[]res;
      أرجع;


}

منطقي vvp_vector4_t::eeq(ثابت vvp_vector4_t&that) ثابت
{
      إذا (سائيز_2 != that.سائيز_2)
	    أرجع خطأ;

      إذا (سائيز_2 < بٽ_في_لفظ) {
	    بنانشان ڊگهو نقاب = (1UL << سائيز_2) - 1;
	    أرجع (abits_val_&نقاب) == (that.abits_val_&نقاب)
		  && (bbits_val_&نقاب) == (that.bbits_val_&نقاب);
      }

      إذا (سائيز_2 == بٽ_في_لفظ) {
	    أرجع (abits_val_ == that.abits_val_)
		  && (bbits_val_ == that.bbits_val_);
      }

      بنانشان لفظ = سائيز_2 / بٽ_في_لفظ;
      لكل (بنانشان idx = 0 ;  idx < لفظ ;  idx += 1) {
	    إذا (abits_ptr_[idx] != that.abits_ptr_[idx])
		  أرجع خطأ;
	    إذا (bbits_ptr_[idx] != that.bbits_ptr_[idx])
		  أرجع خطأ;
      }

      بنانشان ڊگهو نقاب = سائيز_2%بٽ_في_لفظ;
      إذا (نقاب > 0) {
	    نقاب = (1UL << نقاب) - 1;
	    أرجع (abits_ptr_[لفظ]&نقاب) == (that.abits_ptr_[لفظ]&نقاب)
		  && (bbits_ptr_[لفظ]&نقاب) == (that.bbits_ptr_[لفظ]&نقاب);
      }

      أرجع سچ;
}

منطقي vvp_vector4_t::eq_xz(ثابت vvp_vector4_t&that) ثابت
{
      إذا (سائيز_2 != that.سائيز_2)
	    أرجع خطأ;

      إذا (سائيز_2 < بٽ_في_لفظ) {
	    بنانشان ڊگهو نقاب = (1UL << سائيز_2) - 1;
	    أرجع ((abits_val_|bbits_val_)&نقاب) == ((that.abits_val_|that.bbits_val_)&نقاب)
		  && (bbits_val_&نقاب) == (that.bbits_val_&نقاب);
      }

      إذا (سائيز_2 == بٽ_في_لفظ) {
	    أرجع ((abits_val_|bbits_val_) == (that.abits_val_|that.bbits_val_))
		  && (bbits_val_ == that.bbits_val_);
      }

      بنانشان لفظ = سائيز_2 / بٽ_في_لفظ;
      لكل (بنانشان idx = 0 ;  idx < لفظ ;  idx += 1) {
	    إذا ((abits_ptr_[idx]|bbits_ptr_[idx]) != (that.abits_ptr_[idx]|that.bbits_ptr_[idx]))
		  أرجع خطأ;
	    إذا (bbits_ptr_[idx] != that.bbits_ptr_[idx])
		  أرجع خطأ;
      }

      بنانشان ڊگهو نقاب = سائيز_2%بٽ_في_لفظ;
      إذا (نقاب > 0) {
	    نقاب = (1UL << نقاب) - 1;
	    أرجع ((abits_ptr_[لفظ]|bbits_ptr_[لفظ])&نقاب) == ((that.abits_ptr_[لفظ]|that.bbits_ptr_[لفظ])&نقاب)
		  && (bbits_ptr_[لفظ]&نقاب) == (that.bbits_ptr_[لفظ]&نقاب);
      }

      أرجع سچ;
}

منطقي vvp_vector4_t::has_xz() ثابت
{
      إذا (سائيز_2 < بٽ_في_لفظ) {
	    بنانشان ڊگهو نقاب = -1UL >> (بٽ_في_لفظ - سائيز_2);
	    أرجع bbits_val_&نقاب;
      }

      إذا (سائيز_2 == بٽ_في_لفظ) {
	    أرجع bbits_val_;
      }

      بنانشان لفظ = سائيز_2 / بٽ_في_لفظ;
      لكل (بنانشان idx = 0 ; idx < لفظ ; idx += 1) {
	    إذا (bbits_ptr_[idx])
		  أرجع سچ;
      }

      بنانشان ڊگهو نقاب = سائيز_2%بٽ_في_لفظ;
      إذا (نقاب > 0) {
	    نقاب = -1UL >> (بٽ_في_لفظ - نقاب);
	    أرجع bbits_ptr_[لفظ]&نقاب;
      }

      أرجع خطأ;
}

فراغ vvp_vector4_t::change_z2x()
{
	// This method relies on the fact that both BIT4_X and BIT4_Z
	// have the bbit set in the vector4 encoding, and also that
	// the BIT4_X has abit set in the vector4 encoding. By simply
	// or-ing the bbit into the abit, BIT4_X and BIT4_Z both
	// become BIT4_X.

      إذا (سائيز_2 <= بٽ_في_لفظ) {
	    abits_val_ |= bbits_val_;
      } وإلا {
	    بنانشان لفظ = (سائيز_2+بٽ_في_لفظ-1) / بٽ_في_لفظ;
	    لكل (بنانشان idx = 0 ;  idx < لفظ ;  idx += 1)
		  abits_ptr_[idx] |= bbits_ptr_[idx];
      }
}

فراغ vvp_vector4_t::fill_بٽ(vvp_bit4_t ٻٽو_انگ)
{
	/* note: this relies on the bit encoding for the vvp_bit4_t. */
      ساكن ثابت بنانشان ڊگهو init_atable[4] = {
	    لفظ_0_ABITS,
	    لفظ_1_ABITS,
	    لفظ_Z_ABITS,
	    لفظ_X_ABITS };
      ساكن ثابت بنانشان ڊگهو init_btable[4] = {
	    لفظ_0_BBITS,
	    لفظ_1_BBITS,
	    لفظ_Z_BBITS,
	    لفظ_X_BBITS };

      إذا (سائيز_2 <= بٽ_في_لفظ) {
	    abits_val_ = init_atable[ٻٽو_انگ];
	    bbits_val_ = init_btable[ٻٽو_انگ];
      } وإلا {
	    بنانشان لفظ = (سائيز_2 + بٽ_في_لفظ - 1) / بٽ_في_لفظ;
	    لكل (بنانشان idx = 0; idx < لفظ; idx += 1) {
		  abits_ptr_[idx] = init_atable[ٻٽو_انگ];
		  bbits_ptr_[idx] = init_btable[ٻٽو_انگ];
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

      لكل (بنانشان idx = 0 ;  idx < size() && buf_len >= 2 ;  idx += 1) {
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
      أرجع res;
}

فراغ vvp_vector4_t::invert()
{
      إذا (سائيز_2 <= بٽ_في_لفظ) {
	    بنانشان ڊگهو نقاب = (سائيز_2<بٽ_في_لفظ)? (1UL<<سائيز_2)-1UL : -1UL;
	    abits_val_ = نقاب & ~abits_val_;
	    abits_val_ |= bbits_val_;
      } وإلا {
	    بنانشان بچيل = سائيز_2;
	    بنانشان idx = 0;
	    طالما (بچيل >= بٽ_في_لفظ) {
		  abits_ptr_[idx] = ~abits_ptr_[idx];
		  abits_ptr_[idx] |= bbits_ptr_[idx];
		  idx += 1;
		  بچيل -= بٽ_في_لفظ;
	    }
	    إذا (بچيل > 0) {
		  بنانشان ڊگهو نقاب = (1UL<<بچيل) - 1UL;
		  abits_ptr_[idx] = نقاب & ~abits_ptr_[idx];
		  abits_ptr_[idx] |= bbits_ptr_[idx];
	    }
      }
}

#وصف_ڪريو ٻٽو_انگ_نقاب(n) ((n) ? ((~0UL) >> (بٽ_في_لفظ - (n))) : ~0UL)

vvp_bit4_t vvp_vector4_t::reduce_or() ثابت
{
      بنانشان ڊگهو نقاب = ٻٽو_انگ_نقاب(سائيز_2 % بٽ_في_لفظ);
      vvp_bit4_t res = BIT4_0;

      إذا (سائيز_2 <= بٽ_في_لفظ) {
	    إذا ((abits_val_ & ~bbits_val_ & نقاب) != 0UL)
		  أرجع BIT4_1;
	    إذا ((bbits_val_ & نقاب) != 0UL)
		  أرجع BIT4_X;
      } وإلا {
	    بنانشان لفظ = (سائيز_2 + بٽ_في_لفظ - 1) / بٽ_في_لفظ;
	    بنانشان idx;
	    لكل (idx = 0; idx < لفظ - 1; idx += 1) {
		  إذا ((abits_ptr_[idx] & ~bbits_ptr_[idx]) != 0UL)
			أرجع BIT4_1;
		  إذا (bbits_ptr_[idx] != 0UL)
			res = BIT4_X;
	    }
	    إذا ((abits_ptr_[idx] & ~bbits_ptr_[idx] & نقاب) != 0UL)
		  أرجع BIT4_1;
	    إذا ((bbits_ptr_[idx] & نقاب) != 0UL)
		  res = BIT4_X;
      }

      أرجع res;
}

vvp_bit4_t vvp_vector4_t::reduce_and() ثابت
{
      بنانشان ڊگهو نقاب = ٻٽو_انگ_نقاب(سائيز_2 % بٽ_في_لفظ);
      vvp_bit4_t res = BIT4_1;

      إذا (سائيز_2 <= بٽ_في_لفظ) {
	    إذا ((abits_val_ | bbits_val_ | ~نقاب) != ~0UL)
		  أرجع BIT4_0;
	    إذا ((bbits_val_ & نقاب) != 0UL)
		  أرجع BIT4_X;
	} وإلا {
	    بنانشان لفظ = (سائيز_2 + بٽ_في_لفظ - 1) / بٽ_في_لفظ;
	    بنانشان idx;
	    لكل (idx = 0; idx < لفظ - 1; idx += 1) {
		  إذا ((abits_ptr_[idx] | bbits_ptr_[idx]) != ~0UL)
			أرجع BIT4_0;
		  إذا (bbits_ptr_[idx] != 0UL)
			res = BIT4_X;
	    }
	    إذا ((abits_ptr_[idx] | bbits_ptr_[idx] | ~نقاب) != ~0UL)
		  أرجع BIT4_0;
	    إذا ((bbits_ptr_[idx] & نقاب) != 0UL)
		  res = BIT4_X;
      }

      أرجع res;
}

ساكن بنانشان ڊگهو parity(بنانشان ڊگهو val)
{
#إذا defined(__GNUC__)
      // The compiler builtin can use target-specific CPU instructions.
      أرجع __builtin_parityl(val);
#وإلا
#إذا ULONG_MAX > 0xffffffffUL
	val ^= val >> 32;
#جيڪڏهن_پورو
	val ^= val >> 16;
	val ^= val >> 8;
	val ^= val >> 4;

      أرجع (0x6996 >> (val & 0xf)) & 1;
#جيڪڏهن_پورو
}

vvp_bit4_t vvp_vector4_t::reduce_xor() ثابت
{
      بنانشان ڊگهو نقاب = ٻٽو_انگ_نقاب(سائيز_2 % بٽ_في_لفظ);

      إذا (سائيز_2 <= بٽ_في_لفظ) {
	    إذا ((bbits_val_ & نقاب) != 0UL)
		  أرجع BIT4_X;
	    أرجع parity(abits_val_ & نقاب) ? BIT4_1 : BIT4_0;
      } وإلا {
	    بنانشان لفظ = (سائيز_2 + بٽ_في_لفظ - 1) / بٽ_في_لفظ;
	    بنانشان ڊگهو val_a = 0UL;
	    بنانشان idx;
	    لكل (idx = 0; idx < لفظ - 1; idx += 1) {
		  إذا (bbits_ptr_[idx] != 0UL)
			أرجع BIT4_X;
		  val_a ^= abits_ptr_[idx];
	    }
	    إذا ((bbits_ptr_[idx] & نقاب) != 0UL)
		  أرجع BIT4_X;
	    val_a ^= abits_ptr_[idx] & نقاب;
	    أرجع parity(val_a) ? BIT4_1 : BIT4_0;
      }
}

vvp_vector4_t& vvp_vector4_t::عامل &= (ثابت vvp_vector4_t&that)
{
	// The truth table is:
	//     00 01 11 10
	//  00 00 00 00 00
	//  01 00 01 11 11
	//  11 00 11 11 11
	//  10 00 11 11 11
      إذا (سائيز_2 <= بٽ_في_لفظ) {
	    بنانشان ڊگهو tmp1 = abits_val_ | bbits_val_;
	    بنانشان ڊگهو tmp2 = that.abits_val_ | that.bbits_val_;
	    abits_val_ = tmp1 & tmp2;
	    bbits_val_ = (tmp1 & that.bbits_val_) | (tmp2 & bbits_val_);
      } وإلا {
	    بنانشان لفظ = (سائيز_2 + بٽ_في_لفظ - 1) / بٽ_في_لفظ;
	    لكل (بنانشان idx = 0; idx < لفظ ; idx += 1) {
		  بنانشان ڊگهو tmp1 = abits_ptr_[idx] | bbits_ptr_[idx];
		  بنانشان ڊگهو tmp2 = that.abits_ptr_[idx] |
		                       that.bbits_ptr_[idx];
		  abits_ptr_[idx] = tmp1 & tmp2;
		  bbits_ptr_[idx] = (tmp1 & that.bbits_ptr_[idx]) |
		                    (tmp2 & bbits_ptr_[idx]);
	    }
      }

      أرجع *هذا;
}

vvp_vector4_t& vvp_vector4_t::عامل |= (ثابت vvp_vector4_t&that)
{
	// The truth table is:
	//     00 01 11 10
	//  00 00 01 11 11
	//  01 01 01 01 01
	//  11 11 01 11 11
	//  10 11 01 11 11
      إذا (سائيز_2 <= بٽ_في_لفظ) {
	    بنانشان ڊگهو tmp = abits_val_ | bbits_val_ |
	                        that.abits_val_ | that.bbits_val_;
	    bbits_val_ = ((~abits_val_ | bbits_val_) & that.bbits_val_) |
	                 ((~that.abits_val_ | that.bbits_val_) & bbits_val_);
	    abits_val_ = tmp;

      } وإلا {
	    بنانشان لفظ = (سائيز_2 + بٽ_في_لفظ - 1) / بٽ_في_لفظ;
	    لكل (بنانشان idx = 0; idx < لفظ ; idx += 1) {
		  بنانشان ڊگهو tmp = abits_ptr_[idx] | bbits_ptr_[idx] |
	                        that.abits_ptr_[idx] | that.bbits_ptr_[idx];
		  bbits_ptr_[idx] = ((~abits_ptr_[idx] | bbits_ptr_[idx]) &
		                     that.bbits_ptr_[idx]) |
		                    ((~that.abits_ptr_[idx] |
		                      that.bbits_ptr_[idx]) & bbits_ptr_[idx]);
		  abits_ptr_[idx] = tmp;
	    }
      }

      أرجع *هذا;
}

vvp_vector4_t& vvp_vector4_t::عامل ^= (ثابت vvp_vector4_t&that)
{
	// The truth table is:
	//     00 01 11 10
	//  00 00 01 11 11
	//  01 01 00 11 11
	//  11 11 11 11 11
	//  10 11 11 11 11
      إذا (سائيز_2 <= بٽ_في_لفظ) {
	    بنانشان ڊگهو bval = bbits_val_ | that.bbits_val_;
	    bbits_val_ = bval;
	    abits_val_ = (abits_val_ ^ that.abits_val_) | bval;
      } وإلا {
	    بنانشان لفظ = (سائيز_2 + بٽ_في_لفظ - 1) / بٽ_في_لفظ;
	    لكل (بنانشان idx = 0; idx < لفظ ; idx += 1) {
		  بنانشان ڊگهو bval = bbits_ptr_[idx] | that.bbits_ptr_[idx];
		  bbits_ptr_[idx] = bval;
		  abits_ptr_[idx] = (abits_ptr_[idx] ^ that.abits_ptr_[idx]) | bval;
	    }
      }

      أرجع *هذا;
}

/*
* Add an integer to the vvp_vector4_t in place, bit by bit so that
* there is no size limitations.
*/
vvp_vector4_t& vvp_vector4_t::عامل += (int64_t that)
{
      vvp_bit4_t حاصل = BIT4_0;
      بنانشان idx;

      إذا (has_xz()) {
	    vvp_vector4_t xxx (size(), BIT4_X);
	    *هذا = xxx;
	    أرجع *هذا;
      }

      لكل (idx = 0 ; idx < size() ; idx += 1) {
	    إذا (that == 0 && حاصل==BIT4_0)
		  اكسر;

	    vvp_bit4_t that_ٻٽو_انگ = (that&1)? BIT4_1 : BIT4_0;
	    that >>= 1;

	    إذا (that_ٻٽو_انگ==BIT4_0 && حاصل==BIT4_0)
		  استمر;

	    vvp_bit4_t ٻٽو_انگ = value(idx);
	    ٻٽو_انگ = جوڙيو_with_حاصل(ٻٽو_انگ, that_ٻٽو_انگ, حاصل);

	    سيٽ_ٻٽو_انگ(idx, ٻٽو_انگ);
      }

      أرجع *هذا;
}

ostream& عامل<< (ostream&out, ثابت vvp_vector4_t&that)
{
      out << that.size() << "'b";
      لكل (بنانشان idx = 0 ;  idx < that.size() ;  idx += 1)
	    out << that.value(that.size()-idx-1);
      أرجع out;
}

سانچو <صنف INT>منطقي vector4_منزل_قدر(ثابت vvp_vector4_t&vec, INT&val,
					  منطقي is_signed, منطقي is_arithmetic)
{
      INT res = 0;
      INT msk = 1;
      منطقي rc_flag = سچ;

      بنانشان size = vec.size();
      إذا (size > 8*حجم(val)) size = 8*حجم(val);
      لكل (بنانشان idx = 0 ;  idx < size ;  idx += 1) {
	    بدل (vec.value(idx)) {
		حالة BIT4_0:
		  اكسر;
		حالة BIT4_1:
		  res |= msk;
		  اكسر;
		افتراضي:
		  إذا (is_arithmetic)
			أرجع خطأ;
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
      أرجع rc_flag;
}

سانچو منطقي vector4_منزل_قدر(ثابت vvp_vector4_t&vec, int8_t&val,
			       منطقي is_signed, منطقي is_arithmetic);
سانچو منطقي vector4_منزل_قدر(ثابت vvp_vector4_t&vec, int16_t&val,
			       منطقي is_signed, منطقي is_arithmetic);
سانچو منطقي vector4_منزل_قدر(ثابت vvp_vector4_t&vec, int32_t&val,
			       منطقي is_signed, منطقي is_arithmetic);
سانچو منطقي vector4_منزل_قدر(ثابت vvp_vector4_t&vec, int64_t&val,
			       منطقي is_signed, منطقي is_arithmetic);
سانچو منطقي vector4_منزل_قدر(ثابت vvp_vector4_t&vec, uint8_t&val,
			       منطقي is_signed, منطقي is_arithmetic);
سانچو منطقي vector4_منزل_قدر(ثابت vvp_vector4_t&vec, uint16_t&val,
			       منطقي is_signed, منطقي is_arithmetic);
سانچو منطقي vector4_منزل_قدر(ثابت vvp_vector4_t&vec, uint32_t&val,
			       منطقي is_signed, منطقي is_arithmetic);
سانچو منطقي vector4_منزل_قدر(ثابت vvp_vector4_t&vec, uint64_t&val,
			       منطقي is_signed, منطقي is_arithmetic);

سانچو <صنف T> منطقي vector4_منزل_قدر(ثابت vvp_vector4_t&vec,
                                         منطقي&حد_کان_مٿي_flag, T&val)
{
      T res = 0;
      T msk = 1;

      حد_کان_مٿي_flag = خطأ;
      بنانشان size = vec.size();
      لكل (بنانشان idx = 0 ;  idx < size ;  idx += 1) {
	    بدل (vec.value(idx)) {
		حالة BIT4_0:
		  اكسر;
		حالة BIT4_1:
		  إذا (msk == 0)
			حد_کان_مٿي_flag = سچ;
		  وإلا
			res |= msk;
		  اكسر;
		افتراضي:
		  أرجع خطأ;
	    }

	    msk <<= static_cast<T>(1);
      }

      val = res;
      أرجع سچ;
}

سانچو منطقي vector4_منزل_قدر(ثابت vvp_vector4_t&vec, منطقي&حد_کان_مٿي_flag,
                               بنانشان ڊگهو&val);
#جيڪڏهن_وصف_نه_ٿيل UL_AND_TIME64_SAME
سانچو منطقي vector4_منزل_قدر(ثابت vvp_vector4_t&vec, منطقي&حد_کان_مٿي_flag,
                               vvp_time64_t&val);
#جيڪڏهن_پورو

منطقي vector4_منزل_قدر(ثابت vvp_vector4_t&vec, مزدوج&val, منطقي signed_flag)
{

      إذا (vec.size() == 0) {
	    val = 0.0;
	    أرجع سچ;
      }

      منطقي flag = سچ;

      إذا (vec.value(vec.size()-1) != BIT4_1) {
	    signed_flag = خطأ;
      }

      مزدوج res = 0.0;
      إذا (signed_flag) {
	    vvp_bit4_t حاصل = BIT4_1;
	    لكل (بنانشان idx = 0 ;  idx < vec.size() ;  idx += 1) {
		  vvp_bit4_t a = ~vec.value(idx);
		  vvp_bit4_t x = جوڙيو_with_حاصل(a, BIT4_0, حاصل);
		  بدل (x) {
		      حالة BIT4_0:
			اكسر;
		      حالة BIT4_1:
			res += قوت(2.0, (صحيح)idx);
			اكسر;
		      افتراضي:
			flag = خطأ;
		  }
	    }
	    res *= -1.0;
      } وإلا {
	    لكل (بنانشان idx = 0 ;  idx < vec.size() ;  idx += 1) {
		  بدل (vec.value(idx)) {
		      حالة BIT4_0:
			اكسر;
		      حالة BIT4_1:
			res += قوت(2.0, (صحيح)idx);
			اكسر;
		      افتراضي:
			flag = خطأ;
		  }
	    }
      }
      val = res;
      أرجع flag;
}

منطقي vector2_منزل_قدر(ثابت vvp_vector2_t&a, int32_t&val, منطقي is_signed)
{
      val = 0;
      بنانشان idx;
      uint32_t نقاب;
      لكل (idx = 0, نقاب = 1 ; idx < a.size() && idx < 32 ; idx += 1, نقاب <<= 1) {
	    إذا (a.value(idx)) val |= نقاب;
      }

      إذا (is_signed && a.size() < 32 && a.value(a.size()-1)) {
	    نقاب = -1;
	    نقاب <<= a.size();
	    val |= نقاب;
      }

      أرجع a.size() <= 32;
}

vvp_vector4array_t::vvp_vector4array_t(بنانشان ويڪر_3, بنانشان لفظ_3)
: ويڪر_2(ويڪر_3), لفظ_4(لفظ_3)
{
}

vvp_vector4array_t::~vvp_vector4array_t()
{
}

فراغ vvp_vector4array_t::سيٽ_لفظ_2(v4cell*cell, ثابت vvp_vector4_t&that)
{
      أكد(that.سائيز_2 == ويڪر_2);

      إذا (ويڪر_2 <= vvp_vector4_t::بٽ_في_لفظ) {
	    cell->abits_val_ = that.abits_val_;
	    cell->bbits_val_ = that.bbits_val_;
	    أرجع;
      }

      بنانشان cnt = (ويڪر_2 + vvp_vector4_t::بٽ_في_لفظ-1)/vvp_vector4_t::بٽ_في_لفظ;

      إذا (cell->abits_ptr_ == 0) {
	    cell->abits_ptr_ = جديد بنانشان ڊگهو[2*cnt];
	    cell->bbits_ptr_ = cell->abits_ptr_ + cnt;
      }

      لكل (بنانشان idx = 0 ; idx < cnt ; idx += 1)
	    cell->abits_ptr_[idx] = that.abits_ptr_[idx];
      لكل (بنانشان idx = 0 ; idx < cnt ; idx += 1)
	    cell->bbits_ptr_[idx] = that.bbits_ptr_[idx];
}

vvp_vector4_t vvp_vector4array_t::get_لفظ_2(v4cell*cell) ثابت
{
      إذا (ويڪر_2 <= vvp_vector4_t::بٽ_في_لفظ) {
	    vvp_vector4_t res;
	    res.سائيز_2 = ويڪر_2;
	    res.abits_val_ = cell->abits_val_;
	    res.bbits_val_ = cell->bbits_val_;
	    أرجع res;
      }

      vvp_vector4_t res (ويڪر_2, BIT4_X);
      إذا (cell->abits_ptr_ == 0)
	    أرجع res;

      بنانشان cnt = (ويڪر_2 + vvp_vector4_t::بٽ_في_لفظ-1)/vvp_vector4_t::بٽ_في_لفظ;

      لكل (بنانشان idx = 0 ; idx < cnt ; idx += 1)
	    res.abits_ptr_[idx] = cell->abits_ptr_[idx];
      لكل (بنانشان idx = 0 ; idx < cnt ; idx += 1)
	    res.bbits_ptr_[idx] = cell->bbits_ptr_[idx];

      أرجع res;
}

vvp_vector4array_sa::vvp_vector4array_sa(بنانشان ويڪر_3, بنانشان لفظ_3)
: vvp_vector4array_t(ويڪر_3, لفظ_3)
{
      array_ = جديد v4cell[لفظ_4];

      إذا (ويڪر_2 <= vvp_vector4_t::بٽ_في_لفظ) {
	    لكل (بنانشان idx = 0 ; idx < لفظ_4 ; idx += 1) {
		  array_[idx].abits_val_ = vvp_vector4_t::لفظ_X_ABITS;
		  array_[idx].bbits_val_ = vvp_vector4_t::لفظ_X_BBITS;
	    }
      } وإلا {
	    لكل (بنانشان idx = 0 ; idx < لفظ_4 ; idx += 1) {
		  array_[idx].abits_ptr_ = 0;
		  array_[idx].bbits_ptr_ = 0;
	    }
      }
}

vvp_vector4array_sa::~vvp_vector4array_sa()
{
      إذا (array_) {
	    إذا (ويڪر_2 > vvp_vector4_t::بٽ_في_لفظ) {
		  لكل (بنانشان idx = 0 ; idx < لفظ_4 ; idx += 1)
			إذا (array_[idx].abits_ptr_)
			      ميٽيو[]array_[idx].abits_ptr_;
	    }
	    ميٽيو[]array_;
      }
}

فراغ vvp_vector4array_sa::سيٽ_لفظ(بنانشان اشاريو, ثابت vvp_vector4_t&that)
{
      أكد(اشاريو < لفظ_4);

      v4cell*cell = &array_[اشاريو];

      سيٽ_لفظ_2(cell, that);
}

vvp_vector4_t vvp_vector4array_sa::get_لفظ(بنانشان اشاريو) ثابت
{
      إذا (اشاريو >= لفظ_4)
	    أرجع vvp_vector4_t(ويڪر_2, BIT4_X);

      أكد(اشاريو < لفظ_4);

      v4cell*cell = &array_[اشاريو];

      أرجع get_لفظ_2(cell);
}

vvp_vector4array_aa::vvp_vector4array_aa(بنانشان ويڪر_3, بنانشان لفظ_3)
: vvp_vector4array_t(ويڪر_3, لفظ_3)
{
      context_idx_ = vpip_جوڙيو_شي_منزل_context(هذا, vpip_peek_context_scope());
}

vvp_vector4array_aa::~vvp_vector4array_aa()
{
}

فراغ vvp_vector4array_aa::alloc_instance(vvp_context_t context)
{
      v4cell*array = جديد v4cell[لفظ_4];

      إذا (ويڪر_2 <= vvp_vector4_t::بٽ_في_لفظ) {
	    لكل (بنانشان idx = 0 ; idx < لفظ_4 ; idx += 1) {
		  array[idx].abits_val_ = vvp_vector4_t::لفظ_X_ABITS;
		  array[idx].bbits_val_ = vvp_vector4_t::لفظ_X_BBITS;
	    }
      } وإلا {
	    لكل (بنانشان idx = 0 ; idx < لفظ_4 ; idx += 1) {
		  array[idx].abits_ptr_ = 0;
		  array[idx].bbits_ptr_ = 0;
	    }
      }

      vvp_سيٽ_context_شي(context, context_idx_, array);
}

فراغ vvp_vector4array_aa::reset_instance(vvp_context_t context)
{
      v4cell*cell = static_cast<v4cell*>
            (vvp_get_context_شي(context, context_idx_));

      إذا (ويڪر_2 <= vvp_vector4_t::بٽ_في_لفظ) {
	    لكل (بنانشان idx = 0 ; idx < لفظ_4 ; idx += 1) {
		  cell->abits_val_ = vvp_vector4_t::لفظ_X_ABITS;
		  cell->bbits_val_ = vvp_vector4_t::لفظ_X_BBITS;
                  cell++;
	    }
      } وإلا {
            بنانشان cnt = (ويڪر_2 + vvp_vector4_t::بٽ_في_لفظ-1)/vvp_vector4_t::بٽ_في_لفظ;
	    لكل (بنانشان idx = 0 ; idx < لفظ_4 ; idx += 1) {
		  إذا (cell->abits_ptr_) {
                        لكل (بنانشان n = 0 ; n < cnt ; n += 1) {
			      cell->abits_ptr_[n] = vvp_vector4_t::لفظ_X_ABITS;
			      cell->bbits_ptr_[n] = vvp_vector4_t::لفظ_X_BBITS;
                        }
                  }
                  cell++;
	    }
      }
}

#جيڪڏهن_وصف_ٿيل CHECK_WITH_VALGRIND
فراغ vvp_vector4array_aa::free_instance(vvp_context_t context)
{
      v4cell*cell = static_cast<v4cell*>
            (vvp_get_context_شي(context, context_idx_));
      ميٽيو [] cell;
}
#جيڪڏهن_پورو

فراغ vvp_vector4array_aa::سيٽ_لفظ(بنانشان اشاريو, ثابت vvp_vector4_t&that)
{
      أكد(اشاريو < لفظ_4);

      v4cell*cell = static_cast<v4cell*>
            (vthread_get_wt_context_شي(context_idx_)) + اشاريو;

      سيٽ_لفظ_2(cell, that);
}

vvp_vector4_t vvp_vector4array_aa::get_لفظ(بنانشان اشاريو) ثابت
{
      إذا (اشاريو >= لفظ_4)
	    أرجع vvp_vector4_t(ويڪر_2, BIT4_X);

      أكد(اشاريو < لفظ_4);

      v4cell*cell = static_cast<v4cell*>
            (vthread_get_rd_context_شي(context_idx_)) + اشاريو;

      أرجع get_لفظ_2(cell);
}

vvp_vector2_t::vvp_vector2_t()
{
      vec_ = 0;
      wid_ = 0;
}

vvp_vector2_t::vvp_vector2_t(uint64_t v, بنانشان wid)
{
      wid_ = wid;
      ثابت بنانشان بٽ_في_لفظ_2 = 8 * حجم(vec_[0]);
      ثابت بنانشان لفظ = (wid_ + بٽ_في_لفظ_2-1) / بٽ_في_لفظ_2;

      vec_ = جديد بنانشان ڊگهو[لفظ];
      لكل (بنانشان idx = 0 ; idx < لفظ ; idx += 1) {
	    ثابت بنانشان سرڪايو = idx * بٽ_في_لفظ_2;
	    vec_[idx] = سرڪايو < 64 ? v >> سرڪايو : 0;
      }
}

vvp_vector2_t::vvp_vector2_t(vvp_vector2_t::fill_t fill, بنانشان wid)
{
      wid_ = wid;
      ثابت بنانشان بٽ_في_لفظ_2 = 8 * حجم(vec_[0]);
      ثابت بنانشان لفظ = (wid_ + بٽ_في_لفظ_2-1) / بٽ_في_لفظ_2;

      vec_ = جديد بنانشان ڊگهو[لفظ];
      لكل (بنانشان idx = 0 ;  idx < لفظ ;  idx += 1)
	    vec_[idx] = fill? -1 : 0;
}

vvp_vector2_t::vvp_vector2_t(ثابت vvp_vector2_t&that, بنانشان بنياد, بنانشان wid)
{
      wid_ = wid;
      ثابت بنانشان لفظ = (wid_ + بٽ_في_لفظ-1) / بٽ_في_لفظ;

      vec_ = جديد بنانشان ڊگهو[لفظ];

      لكل (بنانشان idx = 0 ; idx < wid ; idx += 1) {
	    صحيح ٻٽو_انگ = that.value(بنياد+idx);
	    إذا (ٻٽو_انگ == 0)
		  استمر;
	    بنانشان لفظ_2 = idx / بٽ_في_لفظ;
	    بنانشان ڊگهو نقاب = 1UL << (idx % بٽ_في_لفظ);
	    vec_[لفظ_2] |= نقاب;
      }
}

فراغ vvp_vector2_t::copy_from_that_(ثابت vvp_vector4_t&that)
{
      wid_ = that.size();
      ثابت بنانشان لفظ = (that.size() + بٽ_في_لفظ-1) / بٽ_في_لفظ;

      إذا (لفظ == 0) {
	    vec_ = 0;
	    wid_ = 0;
	    أرجع;
      }

	// Use the subarray method with the xz_to_0 flag set so that
	// we get values even when there are xz bits.
      vec_ = that.subarray(0, wid_, سچ);
}

فراغ vvp_vector2_t::copy_from_that_(ثابت vvp_vector2_t&that)
{
      wid_ = that.wid_;
      ثابت بنانشان لفظ = (wid_ + بٽ_في_لفظ-1) / بٽ_في_لفظ;

      إذا (لفظ == 0) {
	    vec_ = 0;
	    wid_ = 0;
	    أرجع;
      }

      vec_ = جديد بنانشان ڊگهو[لفظ];
      لكل (بنانشان idx = 0 ;  idx < لفظ ;  idx += 1)
	    vec_[idx] = that.vec_[idx];
}

vvp_vector2_t::vvp_vector2_t(ثابت vvp_vector2_t&that, بنانشان newsize)
{
      wid_ = newsize;
      إذا (newsize == 0) {
	    vec_ = 0;
	    أرجع;
      }

      ثابت بنانشان لفظ = (wid_ + بٽ_في_لفظ-1) / بٽ_في_لفظ;
      ثابت بنانشان twords = (that.wid_ + بٽ_في_لفظ-1) / بٽ_في_لفظ;

      vec_ = جديد بنانشان ڊگهو[لفظ];
      لكل (بنانشان idx = 0 ;  idx < لفظ ;  idx += 1) {
	    إذا (idx < twords)
		  vec_[idx] = that.vec_[idx];
	    وإلا
		  vec_[idx] = 0;
      }
}

vvp_vector2_t& vvp_vector2_t::عامل= (ثابت vvp_vector2_t&that)
{
      إذا (هذا == &that)
	    أرجع *هذا;

      ميٽيو[] vec_;
      vec_ = 0;

      copy_from_that_(that);
      أرجع *هذا;
}

vvp_vector2_t& vvp_vector2_t::عامل= (ثابت vvp_vector4_t&that)
{
      ميٽيو[]vec_;
      vec_ = 0;
      copy_from_that_(that);
      أرجع *هذا;
}

vvp_vector2_t& vvp_vector2_t::عامل <<= (بنانشان صحيح سرڪايو)
{
      إذا (wid_ == 0)
	    أرجع *هذا;

      ثابت بنانشان لفظ = (wid_ + بٽ_في_لفظ-1) / بٽ_في_لفظ;

	// Number of words to shift
      ثابت بنانشان wshift = سرڪايو / بٽ_في_لفظ;
	// bits to shift within each word.
      ثابت بنانشان ڊگهو oshift = سرڪايو % بٽ_في_لفظ;

	// If shifting the entire value away, then return zeros.
      إذا (wshift >= لفظ) {
	    لكل (بنانشان idx = 0 ;  idx < لفظ ;  idx += 1)
		  vec_[idx] = 0;

	    أرجع *هذا;
      }

	// Do the word shift first.
      إذا (wshift > 0) {
	    لكل (بنانشان idx = 0 ;  idx < لفظ-wshift ;  idx += 1) {
		  بنانشان sel = لفظ - idx - 1;
		  vec_[sel] = vec_[sel-wshift];
	    }

	    لكل (بنانشان idx = 0 ;  idx < wshift ;  idx += 1)
		  vec_[idx] = 0;
      }

	// Do the fine shift.
      إذا (oshift != 0) {
	    بنانشان ڊگهو pad = 0;
	    لكل (بنانشان idx = 0 ;  idx < لفظ ;  idx += 1) {
		  بنانشان ڊگهو ايندڙ_pad = vec_[idx] >> (بٽ_في_لفظ-oshift);
		  vec_[idx] = (vec_[idx] << oshift) | pad;
		  pad = ايندڙ_pad;
	    }

	      // Cleanup the tail bits.
	    بنانشان ڊگهو نقاب = -1UL >> (بٽ_في_لفظ - wid_%بٽ_في_لفظ);
	    vec_[لفظ-1] &= نقاب;
      }

      أرجع *هذا;
}

vvp_vector2_t& vvp_vector2_t::عامل >>= (بنانشان سرڪايو)
{
      إذا (wid_ == 0)
	    أرجع *هذا;

      ثابت بنانشان لفظ = (wid_ + بٽ_في_لفظ-1) / بٽ_في_لفظ;

	// Number of words to shift
      ثابت بنانشان wshift = سرڪايو / بٽ_في_لفظ;
	// bits to shift within each word.
      ثابت بنانشان ڊگهو oshift = سرڪايو % بٽ_في_لفظ;

	// If shifting the entire value away, then return zeros.
      إذا (wshift >= لفظ) {
	    لكل (بنانشان idx = 0 ;  idx < لفظ ;  idx += 1)
		  vec_[idx] = 0;

	    أرجع *هذا;
      }

      إذا (wshift > 0) {
	    لكل (بنانشان idx = 0 ;  idx < لفظ-wshift ;  idx += 1)
		  vec_[idx] = vec_[idx+wshift];

	    لكل (بنانشان idx = لفظ-wshift ;  idx < لفظ ;  idx += 1)
		  vec_[idx] = 0;
      }

      إذا (oshift > 0) {
	    بنانشان ڊگهو pad = 0;
	    لكل (بنانشان idx = لفظ ;  idx > 0 ;  idx -= 1) {
		  بنانشان ڊگهو new_pad = vec_[idx-1] <<(بٽ_في_لفظ-oshift);
		  vec_[idx-1] = pad | (vec_[idx-1] >> oshift);
		  pad = new_pad;
	    }

	      // Cleanup the tail bits.

	    بنانشان use_لفظ = لفظ;
	      // Mask_shift is the number of high bits of the top word
	      // that are to be masked off. We start with the number
	      // of bits that are not included even in the original,
	      // then we include the bits of the shift, that are to be
	      // masked to zero.
	    بنانشان ڊگهو نقاب_سرڪايو = بٽ_في_لفظ - wid_%بٽ_في_لفظ;
	    نقاب_سرڪايو %= بٽ_في_لفظ;
	    نقاب_سرڪايو += oshift;
	    طالما (نقاب_سرڪايو >= بٽ_في_لفظ) {
		  vec_[use_لفظ-1] = 0;
		  use_لفظ -= 1;
		  نقاب_سرڪايو -= بٽ_في_لفظ;
	    }
	    إذا (نقاب_سرڪايو > 0) {
		  أكد(use_لفظ > 0);
		  بنانشان ڊگهو نقاب = -1UL >> نقاب_سرڪايو;
		  vec_[use_لفظ-1] &= نقاب;
	    }
      }

      أرجع *هذا;
}

ساكن بنانشان ڊگهو جوڙيو_حاصل(بنانشان ڊگهو a, بنانشان ڊگهو b,
			       بنانشان ڊگهو&حاصل)
{
      بنانشان ڊگهو out = حاصل;
      حاصل = 0;

      إذا ((ULONG_MAX - out) < a)
	    حاصل += 1;
      out += a;

      إذا ((ULONG_MAX - out) < b)
	    حاصل += 1;
      out += b;

      أرجع out;
}

vvp_vector2_t& vvp_vector2_t::عامل += (ثابت vvp_vector2_t&that)
{
      أكد(wid_ == that.wid_);
      إذا (wid_ == 0)
	    أرجع *هذا;

      ثابت بنانشان لفظ = (wid_ + بٽ_في_لفظ-1) / بٽ_في_لفظ;

      بنانشان ڊگهو حاصل = 0;
      لكل (بنانشان idx = 0 ;  idx < لفظ ;  idx += 1) {
	    vec_[idx] = جوڙيو_حاصل(vec_[idx], that.vec_[idx], حاصل);
      }


	// Cleanup the tail bits.
      بنانشان ڊگهو نقاب = -1UL >> (بٽ_في_لفظ - wid_%بٽ_في_لفظ);
      vec_[لفظ-1] &= نقاب;

      أرجع *هذا;
}

vvp_vector2_t& vvp_vector2_t::عامل -= (ثابت vvp_vector2_t&that)
{
      أكد(wid_ == that.wid_);
      إذا (wid_ == 0)
	    أرجع *هذا;

      ثابت بنانشان لفظ = (wid_ + بٽ_في_لفظ-1) / بٽ_في_لفظ;

      بنانشان ڊگهو حاصل = 1;
      لكل (بنانشان idx = 0 ;  idx < لفظ ;  idx += 1) {
	    vec_[idx] = جوڙيو_حاصل(vec_[idx], ~that.vec_[idx], حاصل);
      }

      أرجع *هذا;
}

فراغ vvp_vector2_t::trim()
{
      طالما (value(wid_-1) == 0 && wid_ > 1) wid_ -= 1;
}

/* This is a special trim that is used on numbers we know represent a
 * negative signed value (they came from a negative real value). */
فراغ vvp_vector2_t::trim_neg()
{
      إذا (value(wid_-1) == 1 && wid_ > 32) {
	    طالما (value(wid_-2) == 1 && wid_ > 32) wid_ -= 1;
      }
}

صحيح vvp_vector2_t::value(بنانشان idx) ثابت
{
      إذا (idx >= wid_)
	    أرجع 0;

      ثابت بنانشان بٽ_في_لفظ_2 = 8 * حجم(vec_[0]);
      بنانشان addr = idx/بٽ_في_لفظ_2;
      بنانشان نقاب = idx%بٽ_في_لفظ_2;

      إذا (vec_[addr] & (1UL<<نقاب))
	    أرجع 1;
      وإلا
	    أرجع 0;
}

فراغ vvp_vector2_t::سيٽ_ٻٽو_انگ(بنانشان idx, صحيح ٻٽو_انگ)
{
      أكد(idx < wid_);

      ثابت بنانشان بٽ_في_لفظ_2 = 8 * حجم(vec_[0]);
      بنانشان addr = idx/بٽ_في_لفظ_2;
      بنانشان ڊگهو نقاب = idx%بٽ_في_لفظ_2;

      إذا (ٻٽو_انگ)
	    vec_[addr] |= 1UL << نقاب;
      وإلا
	    vec_[addr] &= ~(1UL << نقاب);
}

فراغ vvp_vector2_t::سيٽ_vec(بنانشان adr, ثابت vvp_vector2_t&that)
{
      أكد((adr + that.wid_) <= wid_);

      لكل (بنانشان idx = 0 ; idx < that.wid_ ; idx += 1)
	    سيٽ_ٻٽو_انگ(adr+idx, that.value(idx));
}

منطقي vvp_vector2_t::is_NaN() ثابت
{
      أرجع wid_ == 0;
}

منطقي vvp_vector2_t::is_zero() ثابت
{
      ثابت بنانشان لفظ = (wid_ + بٽ_في_لفظ-1) / بٽ_في_لفظ;

      لكل (بنانشان idx = 0; idx < لفظ; idx += 1) {
	    إذا (vec_[idx] == 0) استمر;
	    أرجع خطأ;
      }

      أرجع سچ;
}

/*
 * Basic idea from "Introduction to Programming using SML" by
 * Michael R. Hansen and Hans Rischel page 261 and "Seminumerical
 * Algorithms, Third Edition" by Donald E. Knuth section 4.6.3.
 */
vvp_vector2_t قوت(ثابت vvp_vector2_t&x, vvp_vector2_t&y)
{
        /* If we have a zero exponent just return 1. */
      إذا (y == vvp_vector2_t(0L, 1)) {
	    أرجع vvp_vector2_t(1L, x.size());
      }

        /* Is the value odd? */
      إذا (y.value(0) == 1) {
	    y.سيٽ_ٻٽو_انگ(0, 0);  // A quick subtract by 1.
	    vvp_vector2_t res = x * قوت(x, y);
	    أرجع res;
      }

      y >>= 1;  // A fast divide by two. We know the LSB is zero.
      vvp_vector2_t z = قوت(x, y);
      vvp_vector2_t res = z * z;
      أرجع res;
}

ساكن فراغ multiply_long(بنانشان ڊگهو a, بنانشان ڊگهو b,
			  بنانشان ڊگهو&هيٺيون, بنانشان ڊگهو&مٿاهون)
{
      أكد(حجم(بنانشان ڊگهو) %2 == 0);

      ثابت بنانشان ڊگهو لفظ_نقاب = (1UL << 4UL*حجم(a)) - 1UL;
      بنانشان ڊگهو tmpa;
      بنانشان ڊگهو tmpb;
      بنانشان ڊگهو res[4];

      tmpa = a & لفظ_نقاب;
      tmpb = b & لفظ_نقاب;
      res[0] = tmpa * tmpb;
      res[1] = res[0] >> 4UL*حجم(بنانشان ڊگهو);
      res[0] &= لفظ_نقاب;

      tmpa = (a >> 4UL*حجم(بنانشان ڊگهو)) & لفظ_نقاب;
      res[1] += tmpa * tmpb;
      res[2] = res[1] >> 4UL*حجم(بنانشان ڊگهو);
      res[1] &= لفظ_نقاب;

      tmpa = a & لفظ_نقاب;
      tmpb = (b >> 4UL*حجم(بنانشان ڊگهو)) & لفظ_نقاب;
      res[1] += tmpa * tmpb;
      res[2] += res[1] >> 4UL*حجم(بنانشان ڊگهو);
      res[3]  = res[2] >> 4UL*حجم(بنانشان ڊگهو);
      res[1] &= لفظ_نقاب;
      res[2] &= لفظ_نقاب;

      tmpa = (a >> 4UL*حجم(بنانشان ڊگهو)) & لفظ_نقاب;
      res[2] += tmpa * tmpb;
      res[3] += res[2] >> 4UL*حجم(بنانشان ڊگهو);
      res[2] &= لفظ_نقاب;

      مٿاهون = (res[3] << 4UL*حجم(بنانشان ڊگهو)) | res[2];
      هيٺيون  = (res[1] << 4UL*حجم(بنانشان ڊگهو)) | res[0];
}

vvp_vector2_t عامل * (ثابت vvp_vector2_t&a, ثابت vvp_vector2_t&b)
{
      ثابت بنانشان بٽ_في_لفظ_2 = 8 * حجم(a.vec_[0]);

	// The compiler ensures that the two operands are of equal size.
      أكد(a.size() == b.size());
      vvp_vector2_t r (0, a.size());

      بنانشان لفظ = (r.wid_ + بٽ_في_لفظ_2 - 1) / بٽ_في_لفظ_2;

      لكل (بنانشان bdx = 0 ;  bdx < لفظ ;  bdx += 1) {
	    بنانشان ڊگهو tmpb = b.vec_[bdx];
	    إذا (tmpb == 0)
		  استمر;

	    لكل (بنانشان adx = 0 ;  adx < لفظ ;  adx += 1) {
		  بنانشان ڊگهو tmpa = a.vec_[adx];
		  إذا (tmpa == 0)
			استمر;

		  بنانشان ڊگهو هيٺيون, hig;
		  multiply_long(tmpa, tmpb, هيٺيون, hig);

		  بنانشان ڊگهو حاصل = 0;
		  لكل (بنانشان sdx = 0
			     ; (adx+bdx+sdx) < لفظ
			     ;  sdx += 1) {

			r.vec_[adx+bdx+sdx] = جوڙيو_حاصل(r.vec_[adx+bdx+sdx],
							هيٺيون, حاصل);
			هيٺيون = hig;
			hig = 0;
		  }
	    }
      }

      أرجع r;
}

ساكن فراغ div_mod (vvp_vector2_t dividend, vvp_vector2_t ورهائيندڙ,
		     vvp_vector2_t&quotient, vvp_vector2_t&remainder)
{

      quotient = vvp_vector2_t(0, dividend.size());

      إذا (ورهائيندڙ == quotient) {
	    cerr << "ERROR: division by zero, exiting." << سٽ_پڄاڻي;
	    ٻاهر_نڪرو(255);
      }

      إذا (dividend < ورهائيندڙ) {
	    remainder = dividend;
	    أرجع;
      }

      vvp_vector2_t نقاب (1, dividend.size());

	// Make the dividend 1 bit larger to prevent overflow of
	// divtmp in startup.
      dividend = vvp_vector2_t(dividend, dividend.size()+1);
      vvp_vector2_t divtmp (ورهائيندڙ, dividend.size());

      طالما (divtmp < dividend) {
	    divtmp <<= 1;
	    نقاب <<= 1;
      }

      طالما (dividend >= ورهائيندڙ) {
	    إذا (divtmp <= dividend) {
		  dividend -= divtmp;
		  quotient += نقاب;
	    }

	    divtmp >>= 1;
	    نقاب >>= 1;
      }

      remainder = vvp_vector2_t(dividend, نقاب.size());
}

vvp_vector2_t عامل - (ثابت vvp_vector2_t&that)
{
      vvp_vector2_t neg(that);
      إذا (neg.wid_ == 0) أرجع neg;

      ثابت بنانشان لفظ = (neg.wid_ + neg.بٽ_في_لفظ-1) /
                                         neg.بٽ_في_لفظ;
      لكل (بنانشان idx = 0 ;  idx < لفظ ;  idx += 1) {
	    neg.vec_[idx] = ~neg.vec_[idx];
      }
      neg += vvp_vector2_t(1, neg.wid_);

      أرجع neg;
}

vvp_vector2_t عامل / (ثابت vvp_vector2_t&dividend,
			  ثابت vvp_vector2_t&ورهائيندڙ)
{
      vvp_vector2_t quot, rem;
      div_mod(dividend, ورهائيندڙ, quot, rem);
      أرجع quot;
}

vvp_vector2_t عامل % (ثابت vvp_vector2_t&dividend,
			  ثابت vvp_vector2_t&ورهائيندڙ)
{
      vvp_vector2_t quot, rem;
      div_mod(dividend, ورهائيندڙ, quot, rem);
      أرجع rem;
}

منطقي عامل > (ثابت vvp_vector2_t&a, ثابت vvp_vector2_t&b)
{
      ثابت بنانشان awords = (a.wid_ + vvp_vector2_t::بٽ_في_لفظ-1) / vvp_vector2_t::بٽ_في_لفظ;
      ثابت بنانشان bwords = (b.wid_ + vvp_vector2_t::بٽ_في_لفظ-1) / vvp_vector2_t::بٽ_في_لفظ;

      ثابت بنانشان لفظ = awords > bwords? awords : bwords;

      لكل (بنانشان idx = لفظ ;  idx > 0 ;  idx -= 1) {
	    بنانشان ڊگهو aw = (idx <= awords)? a.vec_[idx-1] : 0;
	    بنانشان ڊگهو bw = (idx <= bwords)? b.vec_[idx-1] : 0;

	    إذا (aw > bw)
		  أرجع سچ;
	    إذا (aw < bw)
		  أرجع خطأ;
      }

	// If the above loop finishes, then the vectors are equal.
      أرجع خطأ;
}

منطقي عامل >= (ثابت vvp_vector2_t&a, ثابت vvp_vector2_t&b)
{
      ثابت بنانشان awords = (a.wid_ + vvp_vector2_t::بٽ_في_لفظ-1) / vvp_vector2_t::بٽ_في_لفظ;
      ثابت بنانشان bwords = (b.wid_ + vvp_vector2_t::بٽ_في_لفظ-1) / vvp_vector2_t::بٽ_في_لفظ;

      ثابت بنانشان لفظ = awords > bwords? awords : bwords;

      لكل (بنانشان idx = لفظ ;  idx > 0 ;  idx -= 1) {
	    بنانشان ڊگهو aw = (idx <= awords)? a.vec_[idx-1] : 0;
	    بنانشان ڊگهو bw = (idx <= bwords)? b.vec_[idx-1] : 0;

	    إذا (aw > bw)
		  أرجع سچ;
	    إذا (aw < bw)
		  أرجع خطأ;
      }

	// If the above loop finishes, then the vectors are equal.
      أرجع سچ;
}

منطقي عامل < (ثابت vvp_vector2_t&a, ثابت vvp_vector2_t&b)
{
      ثابت بنانشان awords = (a.wid_ + vvp_vector2_t::بٽ_في_لفظ-1) / vvp_vector2_t::بٽ_في_لفظ;
      ثابت بنانشان bwords = (b.wid_ + vvp_vector2_t::بٽ_في_لفظ-1) / vvp_vector2_t::بٽ_في_لفظ;

      بنانشان لفظ = awords;
      إذا (bwords > لفظ)
	    لفظ = bwords;

      لكل (بنانشان idx = لفظ ;  idx > 0 ;  idx -= 1) {
	    بنانشان ڊگهو aw = (idx <= awords)? a.vec_[idx-1] : 0;
	    بنانشان ڊگهو bw = (idx <= bwords)? b.vec_[idx-1] : 0;

	    إذا (aw < bw)
		  أرجع سچ;
	    إذا (aw > bw)
		  أرجع خطأ;
      }

	// If the above loop finishes, then the vectors are equal.
      أرجع خطأ;
}

منطقي عامل <= (ثابت vvp_vector2_t&a, ثابت vvp_vector2_t&b)
{
	// XXXX For now, only support equal width vectors.
      أكد(a.wid_ == b.wid_);

      ثابت بنانشان awords = (a.wid_ + vvp_vector2_t::بٽ_في_لفظ-1) / vvp_vector2_t::بٽ_في_لفظ;

      لكل (بنانشان idx = awords ;  idx > 0 ;  idx -= 1) {
	    إذا (a.vec_[idx-1] < b.vec_[idx-1])
		  أرجع سچ;
	    إذا (a.vec_[idx-1] > b.vec_[idx-1])
		  أرجع خطأ;
      }

	// If the above loop finishes, then the vectors are equal.
      أرجع سچ;
}

منطقي عامل == (ثابت vvp_vector2_t&a, ثابت vvp_vector2_t&b)
{
      ثابت بنانشان awords = (a.wid_ + vvp_vector2_t::بٽ_في_لفظ-1) / vvp_vector2_t::بٽ_في_لفظ;
      ثابت بنانشان bwords = (b.wid_ + vvp_vector2_t::بٽ_في_لفظ-1) / vvp_vector2_t::بٽ_في_لفظ;

      ثابت بنانشان لفظ = awords > bwords? awords : bwords;

      لكل (بنانشان idx = لفظ ;  idx > 0 ;  idx -= 1) {
	    بنانشان ڊگهو aw = (idx <= awords)? a.vec_[idx-1] : 0;
	    بنانشان ڊگهو bw = (idx <= bwords)? b.vec_[idx-1] : 0;

	    إذا (aw > bw)
		  أرجع خطأ;
	    إذا (aw < bw)
		  أرجع خطأ;
      }

	// If the above loop finishes, then the vectors are equal.
      أرجع سچ;
}


vvp_vector4_t vector2_منزل_vector4(ثابت vvp_vector2_t&that, بنانشان wid)
{
      vvp_vector4_t res (wid);

      لكل (بنانشان idx = 0 ;  idx < res.size() ;  idx += 1) {
	    vvp_bit4_t ٻٽو_انگ = BIT4_0;

	    إذا (that.value(idx))
		  ٻٽو_انگ = BIT4_1;

	    res.سيٽ_ٻٽو_انگ(idx, ٻٽو_انگ);
      }

      أرجع res;
}

ساكن منطقي c4string_سرو_test(ثابت حرف*str)
{
      إذا ((str[0] != 'C') && (str[0] != 'c'))
	    أرجع خطأ;
      إذا ((str[1] != '4') || (str[2] != '<'))
	    أرجع خطأ;
      أرجع سچ;
}

منطقي c4string_test(ثابت حرف*str)
{
      إذا (!c4string_سرو_test(str))
	    أرجع خطأ;
      size_t قدر_سائيز = strspn(str+3, "01xz");
      إذا (str[3+قدر_سائيز] != '>')
	    أرجع خطأ;
      إذا (str[3+قدر_سائيز+1] != 0)
	    أرجع خطأ;

      أرجع سچ;
}

vvp_vector4_t c4string_منزل_vector4(ثابت حرف*str)
{
      أكد(c4string_سرو_test(str));

      str += 3;
      ثابت حرف*tp = str + strspn(str,"01xz");
      أكد(tp[0] == '>');

      vvp_vector4_t tmp (tp-str);

      لكل (بنانشان idx = 0 ;  idx < tmp.size() ;  idx += 1) {
	    vvp_bit4_t ٻٽو_انگ;
	    بدل (str[idx]) {
		حالة '0':
		  ٻٽو_انگ = BIT4_0;
		  اكسر;
		حالة '1':
		  ٻٽو_انگ = BIT4_1;
		  اكسر;
		حالة 'x':
		  ٻٽو_انگ = BIT4_X;
		  اكسر;
		حالة 'z':
		  ٻٽو_انگ = BIT4_Z;
		  اكسر;
		افتراضي:
		  fprintf(stderr, "Unsupported bit value %c(%d).\n", str[idx],
		          str[idx]);
		  أكد(0);
		  ٻٽو_انگ = BIT4_0;
		  اكسر;
	    }
	    tmp.سيٽ_ٻٽو_انگ(tmp.size()-idx-1, ٻٽو_انگ);
      }

      أرجع tmp;
}

ostream& عامل<< (ostream&out, ثابت vvp_vector2_t&that)
{
      إذا (that.is_NaN()) {
	    out << "NaN";

      } وإلا {
	    out << vector2_منزل_vector4(that, that.size());
      }
      أرجع out;
}

vvp_vector8_t::vvp_vector8_t(ثابت vvp_vector8_t&that)
{
      سائيز_2 = that.سائيز_2;
      إذا (سائيز_2 <= حجم(val_)) {
	    يادگيري_نقل(val_, that.val_, حجم val_);
      } وإلا {
	    ptr_ = جديد بنانشان حرف[سائيز_2];
	    يادگيري_نقل(ptr_, that.ptr_, سائيز_2);
      }
}

vvp_vector8_t::vvp_vector8_t(ثابت vvp_vector4_t&that,
			     بنانشان str0, بنانشان str1)
: سائيز_2(that.size())
{
      إذا (سائيز_2 == 0)
	    أرجع;

      إذا (سائيز_2 <= حجم(val_)) {
	    ptr_ = 0; // Prefill all val_ bytes
	    لكل (بنانشان idx = 0 ; idx < سائيز_2 ; idx += 1)
		  val_[idx] = vvp_scalar_t(that.value(idx),str0, str1).raw();
      } وإلا {
	    ptr_ = جديد بنانشان حرف[سائيز_2];
	    لكل (بنانشان idx = 0 ;  idx < سائيز_2 ;  idx += 1)
		  ptr_[idx] = vvp_scalar_t(that.value(idx), str0, str1).raw();
      }
}

vvp_vector8_t::vvp_vector8_t(ثابت vvp_vector2_t&that,
			     بنانشان str0, بنانشان str1)
: سائيز_2(that.size())
{
      إذا (سائيز_2 == 0)
	    أرجع;

      إذا (سائيز_2 <= حجم(val_)) {
	    ptr_ = 0;
	    لكل (بنانشان idx = 0 ; idx < سائيز_2 ; idx += 1)
		  val_[idx] = vvp_scalar_t(that.value(idx)? BIT4_1:BIT4_0, str0, str1).raw();
      } وإلا {
	    ptr_ = جديد بنانشان حرف[سائيز_2];
	    لكل (بنانشان idx = 0 ; idx < سائيز_2 ; idx += 1)
		  ptr_[idx] = vvp_scalar_t(that.value(idx)? BIT4_1:BIT4_0, str0, str1).raw();
      }
}

ثابت vvp_vector8_t vvp_vector8_t::nil;

vvp_vector8_t& vvp_vector8_t::عامل= (ثابت vvp_vector8_t&that)
{
	// Assign to self.
      إذا (هذا == &that)
	    أرجع *هذا;

      إذا (سائيز_2 != that.سائيز_2) {
	    إذا (سائيز_2 > حجم(val_))
		  ميٽيو[]ptr_;
	    سائيز_2 = 0;
      }

      إذا (that.سائيز_2 == 0) {
	    أكد(سائيز_2 == 0);
	    أرجع *هذا;
      }

      إذا (that.سائيز_2 <= حجم(val_)) {
	    سائيز_2 = that.سائيز_2;
	    يادگيري_نقل(val_, that.val_, حجم(val_));
	    أرجع *هذا;
      }

      إذا (سائيز_2 == 0) {
	    سائيز_2 = that.سائيز_2;
	    ptr_ = جديد بنانشان حرف[سائيز_2];
      }

      يادگيري_نقل(ptr_, that.ptr_, سائيز_2);

      أرجع *هذا;
}

vvp_vector8_t vvp_vector8_t::subvalue(بنانشان بنياد, بنانشان wid) ثابت
{
      vvp_vector8_t tmp (wid);

      بنانشان حرف*tmp_ptr = tmp.سائيز_2 <= حجم(val_) ? tmp.val_ : tmp.ptr_;
      ثابت بنانشان حرف*use_ptr = سائيز_2 <= حجم(val_) ? val_ : ptr_;

      بنانشان idx = 0;
      طالما ((idx < wid) && (بنياد+idx < سائيز_2)) {
	    tmp_ptr[idx] = use_ptr[بنياد+idx];
	    idx += 1;
      }

      أرجع tmp;
}

فراغ vvp_vector8_t::سيٽ_vec(بنانشان بنياد, ثابت vvp_vector8_t&that)
{
      أكد((بنياد+that.size()) <= size());
      لكل (بنانشان idx = 0 ; idx < that.size() ; idx += 1)
	    سيٽ_ٻٽو_انگ(بنياد+idx, that.value(idx));
}

vvp_vector8_t part_expand(ثابت vvp_vector8_t&that, بنانشان wid, بنانشان off)
{
      أكد(off < wid);
      vvp_vector8_t tmp (wid);

      بنانشان حرف* tmp_ptr = tmp.سائيز_2<= حجم(tmp.val_) ?
                               tmp.val_ : tmp.ptr_;
      ثابت بنانشان حرف* that_ptr = that.سائيز_2<= حجم(that.val_) ?
                                      that.val_ : that.ptr_;

      بنانشان idx = off;

      طالما (idx < wid && that.سائيز_2 > (idx-off)) {
	    tmp_ptr[idx] = that_ptr[idx-off];
	    idx += 1;
      }

      أرجع tmp;
}

ساكن منطقي c8string_سرو_test(ثابت حرف*str)
{
      إذا ((str[0] != 'C') && (str[0] != 'c'))
	    أرجع خطأ;
      إذا ((str[1] != '8') || (str[2] != '<'))
	    أرجع خطأ;
      أرجع سچ;
}

منطقي c8string_test(ثابت حرف*str)
{
      إذا (!c8string_سرو_test(str))
	    أرجع خطأ;

      ثابت حرف*cp = str+3;
      لكل (;; cp += 1) {
	    إذا (cp[0] == '>' && cp[1] == 0) أرجع سچ;
	    إذا (cp[0] >= '0' && cp[0] <= '9') استمر;
	    إذا (cp[0] == 'x') استمر;
	    إذا (cp[0] == 'z') استمر;
	    أرجع خطأ;
      }
      أرجع خطأ;
}
/*
 * The format of a C8<> string is:
 *   C8<aaabbbccc...>
 * where aaa... is a 3 character bit descriptor.
 */
vvp_vector8_t c8string_منزل_vector8(ثابت حرف*str)
{
      أكد(c8string_سرو_test(str));

      size_t vsize = لڙي_ڊيگهه(str)-4;
      أكد(vsize%3 == 0);
      vsize /= 3;
      vvp_vector8_t tmp (vsize);

      لكل (size_t idx = 0 ; idx < vsize ; idx += 1) {
	    ثابت حرف*cp = str+3+3*idx;
	    vvp_bit4_t ٻٽو_انگ = BIT4_X;
	    بنانشان dr0 = cp[0]-'0';
	    بنانشان dr1 = cp[1]-'0';
	    بدل (cp[2]) {
		حالة '0':
		  ٻٽو_انگ = BIT4_0;
		  اكسر;
		حالة '1':
		  ٻٽو_انگ = BIT4_1;
		  اكسر;
		حالة 'x':
		  ٻٽو_انگ = BIT4_X;
		  اكسر;
		حالة 'z':
		  ٻٽو_انگ = BIT4_Z;
		  اكسر;
	    }
	    tmp.سيٽ_ٻٽو_انگ(vsize-idx-1, vvp_scalar_t(ٻٽو_انگ, dr0, dr1));
      }

      أرجع tmp;
}

ostream& عامل<<(ostream&out, ثابت vvp_vector8_t&that)
{
      out << "C8<";
      لكل (بنانشان idx = 0 ;  idx < that.size() ; idx += 1)
	    out << that.value(that.size()-idx-1);

      out << ">";
      أرجع out;
}

vvp_net_fun_t::vvp_net_fun_t()
{
      ڳڻپ_functors += 1;
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

فراغ vvp_net_fun_t::recv_vec4_pv_(vvp_net_ptr_t p, ثابت vvp_vector4_t&ٻٽو_انگ,
                                  بنانشان بنياد, بنانشان vwid, vvp_context_t)
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

      أكد(بنياد + ٻٽو_انگ.size() <= vwid);

      vvp_vector4_t tmp(vwid, BIT4_Z);
      tmp.سيٽ_vec(بنياد, ٻٽو_انگ);
      recv_vec4(p, tmp, 0);
}

فراغ vvp_net_fun_t::recv_vec4_pv(vvp_net_ptr_t, ثابت vvp_vector4_t&ٻٽو_انگ,
                                 بنانشان بنياد, بنانشان vwid, vvp_context_t)
{
      cerr << "internal error: " << typeid(*هذا).name() << ": "
	   << "recv_vec4_pv(" << ٻٽو_انگ << ", " << بنياد
	   << ", " << vwid << ") not implemented" << سٽ_پڄاڻي;
      أكد(0);
}

فراغ vvp_net_fun_t::recv_vec8(vvp_net_ptr_t پورٽ, ثابت vvp_vector8_t&ٻٽو_انگ)
{
      recv_vec4(پورٽ, reduce4(ٻٽو_انگ), 0);
}

فراغ vvp_net_fun_t::recv_vec8_pv_(vvp_net_ptr_t p, ثابت vvp_vector8_t&ٻٽو_انگ,
				  بنانشان بنياد, بنانشان vwid)
{
	// This is the strength-aware version of recv_vec4_pv_.

      أكد(بنياد + ٻٽو_انگ.size() <= vwid);

      vvp_vector8_t tmp(vwid);
      tmp.سيٽ_vec(بنياد, ٻٽو_انگ);
      recv_vec8(p, tmp);
}

فراغ vvp_net_fun_t::recv_vec8_pv(vvp_net_ptr_t پورٽ, ثابت vvp_vector8_t&ٻٽو_انگ,
				 بنانشان بنياد, بنانشان vwid)
{
      recv_vec4_pv(پورٽ, reduce4(ٻٽو_انگ), بنياد, vwid, 0);
}

فراغ vvp_net_fun_t::recv_real(vvp_net_ptr_t, مزدوج ٻٽو_انگ, vvp_context_t)
{
      fprintf(stderr, "internal error: %s: recv_real(%f) not implemented\n",
	      typeid(*هذا).name(), ٻٽو_انگ);
      أكد(0);
}

فراغ vvp_net_fun_t::recv_string(vvp_net_ptr_t, ثابت std::string&ٻٽو_انگ, vvp_context_t)
{
      fprintf(stderr, "internal error: %s: recv_string(%s) not implemented\n",
	      typeid(*هذا).name(), ٻٽو_انگ.c_str());
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

vvp_fun_drive::vvp_fun_drive(بنانشان str0, بنانشان str1)
{
      أكد(str0 < 8);
      أكد(str1 < 8);

      drive0_ = str0;
      drive1_ = str1;
}

vvp_fun_drive::~vvp_fun_drive()
{
}

فراغ vvp_fun_drive::recv_vec4(vvp_net_ptr_t پورٽ, ثابت vvp_vector4_t&ٻٽو_انگ,
                              vvp_context_t)
{
      أكد(پورٽ.پورٽ() == 0);
      پورٽ.ptr()->send_vec8(vvp_vector8_t(ٻٽو_انگ, drive0_, drive1_));
}

فراغ vvp_fun_drive::recv_vec4_pv(vvp_net_ptr_t ptr, ثابت vvp_vector4_t&ٻٽو_انگ,
				 بنانشان بنياد, بنانشان vwid, vvp_context_t ctx)
{
      recv_vec4_pv_(ptr, ٻٽو_انگ, بنياد, vwid, ctx);
}

/* **** vvp_wide_fun_* methods **** */

vvp_wide_fun_core::vvp_wide_fun_core(vvp_net_t*net, بنانشان nports)
{
      ptr_ = net;
      nports_ = nports;
      پورٽ_قدرون = 0;
      پورٽ_rvalues = 0;
}

vvp_wide_fun_core::~vvp_wide_fun_core()
{
      ميٽيو[] پورٽ_قدرون;
      ميٽيو[] پورٽ_rvalues;
}

فراغ vvp_wide_fun_core::propagate_vec4(ثابت vvp_vector4_t&ٻٽو_انگ,
				       vvp_time64_t delay)
{
      إذا (delay)
	    وقت_جدول_propagate_ويڪٽر(ptr_, delay, ٻٽو_انگ);
      وإلا
	    ptr_->send_vec4(ٻٽو_انگ, 0);
}

فراغ vvp_wide_fun_core::propagate_real(مزدوج ٻٽو_انگ,
				       vvp_time64_t delay)
{
      إذا (delay) {
	      // schedule_assign_vector(ptr_->out, bit, delay);
	    أكد(0); // Need a real-value version of assign_vector.
      } وإلا {
	    ptr_->send_real(ٻٽو_انگ, 0);
      }
}


بنانشان vvp_wide_fun_core::پورٽ_ڳڻپ() ثابت
{
      أرجع nports_;
}

vvp_vector4_t& vvp_wide_fun_core::value(بنانشان idx)
{
      أكد(idx < nports_);
      إذا (پورٽ_قدرون == 0)
	    پورٽ_قدرون = جديد vvp_vector4_t [nports_];
      أرجع پورٽ_قدرون[idx];
}

مزدوج vvp_wide_fun_core::قدر_r(بنانشان idx)
{
      أكد(idx < nports_);
      أرجع پورٽ_rvalues? پورٽ_rvalues[idx] : 0.0;
}

فراغ vvp_wide_fun_core::recv_real_from_inputs(بنانشان)
{
      أكد(0);
}

فراغ vvp_wide_fun_core::dispatch_vec4_from_داخلا(بنانشان پورٽ,
						  ثابت vvp_vector4_t&ٻٽو_انگ)
{
      أكد(پورٽ < nports_);
      إذا (پورٽ_قدرون == 0) پورٽ_قدرون = جديد vvp_vector4_t [nports_];
      پورٽ_قدرون[پورٽ] = ٻٽو_انگ;
      recv_vec4_from_inputs(پورٽ);
}

فراغ vvp_wide_fun_core::dispatch_real_from_داخلا(بنانشان پورٽ,
						  مزدوج ٻٽو_انگ)
{
      أكد(پورٽ < nports_);
      إذا (پورٽ_rvalues == 0) پورٽ_rvalues = جديد مزدوج[nports_];
      پورٽ_rvalues[پورٽ] = ٻٽو_انگ;
      recv_real_from_inputs(پورٽ);
}

vvp_wide_fun_t::vvp_wide_fun_t(vvp_wide_fun_core*c, بنانشان بنياد)
: core_(c), پورٽ_بنياد_2(بنياد)
{
}

vvp_wide_fun_t::~vvp_wide_fun_t()
{
}

فراغ vvp_wide_fun_t::recv_vec4(vvp_net_ptr_t پورٽ, ثابت vvp_vector4_t&ٻٽو_انگ,
                               vvp_context_t)
{
      بنانشان pidx = پورٽ_بنياد_2 + پورٽ.پورٽ();
      core_->dispatch_vec4_from_داخلا(pidx, ٻٽو_انگ);
}

فراغ vvp_wide_fun_t::recv_vec4_pv(vvp_net_ptr_t ptr, ثابت vvp_vector4_t&ٻٽو_انگ,
				  بنانشان بنياد, بنانشان vwid, vvp_context_t ctx)
{
      recv_vec4_pv_(ptr, ٻٽو_انگ, بنياد, vwid, ctx);
}

فراغ vvp_wide_fun_t::recv_real(vvp_net_ptr_t پورٽ, مزدوج ٻٽو_انگ,
                               vvp_context_t)
{
      بنانشان pidx = پورٽ_بنياد_2 + پورٽ.پورٽ();
      core_->dispatch_real_from_داخلا(pidx, ٻٽو_انگ);
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

بنانشان vvp_scalar_t::strength0() ثابت
{
      أرجع STREN0(قدر_2);
}

بنانشان vvp_scalar_t::strength1() ثابت
{
      أرجع STREN1(قدر_2);
}

ostream& عامل <<(ostream&out, vvp_scalar_t a)
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
      أرجع out;
}

/*
 * This function is only called if the actual interface function rules
 * out some of the easy cases. If we get here, we can assume that
 * neither of the values is HiZ, and the values are not exactly equal.
 */
vvp_scalar_t fully_featured_resolv_(vvp_scalar_t a, vvp_scalar_t b)
{

      إذا (UNAMBIG(a.قدر_2) && UNAMBIG(b.قدر_2)) {

	      /* If both signals are unambiguous, simply choose
		 the stronger. If they have the same strength
		 but different values, then this becomes
		 ambiguous. */

	    إذا ((b.قدر_2&0x07) > (a.قدر_2&0x07)) {

		    /* b value is stronger. Take it. */
		  أرجع b;

	    } وإلا إذا ((b.قدر_2&0x77) == (a.قدر_2&0x77)) {

		    // Strengths are the same. Since we know already
		    // that the values are not the same, Make value
		    // into "x".
		  vvp_scalar_t tmp (a);
		  tmp.قدر_2 = (tmp.قدر_2&0x77) | 0x80;
		  أرجع tmp;

	    } وإلا {

		    /* Must be "a" is the stronger one. */
		  أرجع a;
	    }

      }

	/* If one of the signals is unambiguous, then it
	   will sweep up the weaker parts of the ambiguous
	   signal. The result may be ambiguous, or maybe not. */

      إذا (UNAMBIG(a.قدر_2)) {
	    vvp_scalar_t res;

	    إذا ((a.قدر_2&0x70) > (b.قدر_2&0x70))
		  res.قدر_2 |= a.قدر_2&0xf0;
	    وإلا
		  res.قدر_2 |= b.قدر_2&0xf0;

	    إذا ((a.قدر_2&0x07) > (b.قدر_2&0x07))
		  res.قدر_2 |= a.قدر_2&0x0f;
	    وإلا
		  res.قدر_2 |= b.قدر_2&0x0f;

	    أرجع res;

      } وإلا إذا (UNAMBIG(b.قدر_2)) {

	    vvp_scalar_t res;

	    إذا ((b.قدر_2&0x70) > (a.قدر_2&0x70))
		  res.قدر_2 |= b.قدر_2&0xf0;
	    وإلا
		  res.قدر_2 |= a.قدر_2&0xf0;

	    إذا ((b.قدر_2&0x07) > (a.قدر_2&0x07))
		  res.قدر_2 |= b.قدر_2&0x0f;
	    وإلا
		  res.قدر_2 |= a.قدر_2&0x0f;

	    أرجع res;

      }


	/* If both signals are ambiguous, then the result
	   has an even wider ambiguity. */

      بنانشان tmp = 0;
      صحيح sv1a = (a.قدر_2 & 0x80) ? STREN1(a.قدر_2) : - STREN1(a.قدر_2);
      صحيح sv0a = (a.قدر_2 & 0x08) ? STREN0(a.قدر_2) : - STREN0(a.قدر_2);
      صحيح sv1b = (b.قدر_2 & 0x80) ? STREN1(b.قدر_2) : - STREN1(b.قدر_2);
      صحيح sv0b = (b.قدر_2 & 0x08) ? STREN0(b.قدر_2) : - STREN0(b.قدر_2);

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
	    tmp |= (a.قدر_2&b.قدر_2)&0x80;
	    tmp |= (-sv1) << 4;
      }

      إذا (sv0 > 0) {
	    tmp |= 0x08;
	    tmp |= sv0;
      } وإلا {
	    tmp |= (-sv0);
      }

      vvp_scalar_t res;
      res.قدر_2 = tmp;

	/* Canonicalize the HiZ value. */
      إذا ((res.قدر_2&0x77) == 0)
	    res.قدر_2 = 0;

      أرجع res;
}

بنانشان vvp_switch_strength_map[2][8] = {
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
      لكل (بنانشان idx = 0 ;  idx < out.size() ;  idx += 1)
	    out.سيٽ_ٻٽو_انگ(idx, that.value(idx).value());

      أرجع out;
}

vvp_bit4_t compare_gtge(ثابت vvp_vector4_t&lef, ثابت vvp_vector4_t&rig,
			vvp_bit4_t ٻاهريون_if_equal)
{
      بنانشان min_سائيز = lef.size();
      إذا (rig.size() < min_سائيز)
	    min_سائيز = rig.size();

	// If one of the inputs is nil, treat is as all X values, and
	// that makes the result BIT4_X.
      إذا (min_سائيز == 0)
	    أرجع BIT4_X;

	// As per the IEEE1364 definition of >, >=, < and <=, if there
	// are any X or Z values in either of the operand vectors,
	// then the result of the compare is BIT4_X.

	// Check for X/Z in the left operand
      إذا (lef.has_xz())
	    أرجع BIT4_X;

	// Check for X/Z in the right operand
      إذا (rig.has_xz())
	    أرجع BIT4_X;

      لكل (بنانشان idx = lef.size() ; idx > rig.size() ;  idx -= 1) {
	    إذا (lef.value(idx-1) == BIT4_1)
		  أرجع BIT4_1;
      }

      لكل (بنانشان idx = rig.size() ; idx > lef.size() ;  idx -= 1) {
	    إذا (rig.value(idx-1) == BIT4_1)
		  أرجع BIT4_0;
      }

      لكل (بنانشان idx = min_سائيز ; idx > 0 ;  idx -= 1) {
	    vvp_bit4_t lv = lef.value(idx-1);
	    vvp_bit4_t rv = rig.value(idx-1);

	    إذا (lv == rv)
		  استمر;

	    إذا (lv == BIT4_1)
		  أرجع BIT4_1;
	    وإلا
		  أرجع BIT4_0;
      }

      أرجع ٻاهريون_if_equal;
}

vvp_bit4_t compare_gtge_signed(ثابت vvp_vector4_t&a,
			       ثابت vvp_vector4_t&b,
			       vvp_bit4_t ٻاهريون_if_equal)
{
      أكد(a.size() == b.size());

      بنانشان sign_idx = a.size()-1;
      vvp_bit4_t a_sign = a.value(sign_idx);
      vvp_bit4_t b_sign = b.value(sign_idx);

      إذا (bit4_is_xz(a_sign))
	    أرجع BIT4_X;
      إذا (bit4_is_xz(b_sign))
	    أرجع BIT4_X;

      إذا (a_sign == b_sign)
	    أرجع compare_gtge(a, b, ٻاهريون_if_equal);

      إذا (a.has_xz())
	    أرجع BIT4_X;

      إذا (b.has_xz())
	    أرجع BIT4_X;

      إذا(a_sign == BIT4_0)
	    أرجع BIT4_1;
      وإلا
	    أرجع BIT4_0;
}
