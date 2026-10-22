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
#جے_تعریف_شدہ CHECK_WITH_VALGRIND
# include  <valgrind/memcheck.h>
# include  <map>
# include  "sfunc.h"
# include  "udp.h"
# include  "ivl_alloc.h"
#جے_انت

ورتوں نطاق std;

/* This is the size of an unsigned long in bits. This is just a
   convenience macro. */
# define CPU_لفظ_بٹ (8*حجم(بنانشانی لما))
# define چوٹی_ثنائی_ہندسہ (1UL << (CPU_لفظ_بٹ-1))

permaheap vvp_net_fun_t::ڈھیر;
permaheap vvp_net_fil_t::ڈھیر;

// Allocate around 1Megabyte/chunk.
ساكن ثابت size_t VVP_NET_CHUNK = 1024*1024/حجم(vvp_net_t);
ساكن vvp_net_t*vvp_net_alloc_جدول = عدم;
#جے_تعریف_شدہ CHECK_WITH_VALGRIND
ساكن vvp_net_t **vvp_net_pool = عدم;
ساكن بنانشانی vvp_net_pool_گنتی = 0;
#جے_انت
ساكن size_t vvp_net_alloc_بچیا = 0;
// For statistics, count the vvp_nets allocated and the bytes of alloc
// chunks allocated.
بنانشانی لما گنتی_vvp_nets = 0;
size_t ناپ_vvp_nets = 0;

فراغ* vvp_net_t::عامل جديد (size_t size)
{
      أكد(size == حجم(vvp_net_t));
      إذا (vvp_net_alloc_بچیا == 0) {
	    vvp_net_alloc_جدول = ::جديد vvp_net_t[VVP_NET_CHUNK];
	    vvp_net_alloc_بچیا = VVP_NET_CHUNK;
	    ناپ_vvp_nets += size*VVP_NET_CHUNK;
#جے_تعریف_شدہ CHECK_WITH_VALGRIND
	    VALGRIND_MAKE_MEM_NOACCESS(vvp_net_alloc_جدول, size*VVP_NET_CHUNK);
	    VALGRIND_CREATE_MEMPOOL(vvp_net_alloc_جدول, 0, 0);
	    vvp_net_pool_گنتی += 1;
	    vvp_net_pool = static_cast<vvp_net_t **>(realloc(vvp_net_pool,
	                   vvp_net_pool_گنتی*حجم(vvp_net_t **)));
	    vvp_net_pool[vvp_net_pool_گنتی-1] = vvp_net_alloc_جدول;
#جے_انت
      }

      vvp_net_t*return_this = vvp_net_alloc_جدول;
#جے_تعریف_شدہ CHECK_WITH_VALGRIND
      VALGRIND_MEMPOOL_ALLOC(vvp_net_pool[vvp_net_pool_گنتی-1],
                             return_this, size);
      return_this->pool = vvp_net_pool[vvp_net_pool_گنتی-1];
#جے_انت
      vvp_net_alloc_جدول += 1;
      vvp_net_alloc_بچیا -= 1;
      گنتی_vvp_nets += 1;
      أرجع return_this;
}

#جے_تعریف_شدہ CHECK_WITH_VALGRIND
ساكن map<vvp_net_t*, منطقي> vvp_net_map;
ساكن map<sfunc_core*, منطقي> sfunc_map;
ساكن map<vvp_udp_fun_core*, منطقي> udp_map;
ساكن map<resolv_core*, منطقي> resolv_map;
ساكن vvp_net_t **local_net_pool = 0;
ساكن بنانشانی local_net_pool_گنتی = 0;

فراغ pool_local_net(vvp_net_t*net)
{
      local_net_pool_گنتی += 1;
      local_net_pool = static_cast<vvp_net_t **>(realloc(local_net_pool,
                       local_net_pool_گنتی*حجم(vvp_net_t **)));
      local_net_pool[local_net_pool_گنتی-1] = net;
}

فراغ vvp_net_delete(vvp_net_t *شے)
{
      vvp_net_map[شے] = سچ;
      إذا (sfunc_core*tmp = dynamic_cast<sfunc_core*> (شے->fun)) {
	    sfunc_map[tmp] = سچ;
      }
      إذا (vvp_udp_fun_core*tmp = dynamic_cast<vvp_udp_fun_core*> (شے->fun)) {
	    udp_map[tmp] = سچ;
      }
      إذا (resolv_core*tmp = dynamic_cast<resolv_core*> (شے->fun)) {
	    resolv_map[tmp] = سچ;
      }
	/* Only delete static object variables. */
      إذا (vvp_fun_signal_object_sa*tmp = dynamic_cast<vvp_fun_signal_object_sa*> (شے->fun)) {
	    مٹاؤ tmp;
      }
	/* Only delete static string variables. */
      إذا (vvp_fun_signal_string_sa*tmp = dynamic_cast<vvp_fun_signal_string_sa*> (شے->fun)) {
	    مٹاؤ tmp;
      }
}

فراغ vvp_net_pool_delete()
{
      بنانشانی لما vvp_nets_del = 0;

      لكل (بنانشانی idx = 0; idx < local_net_pool_گنتی; idx += 1) {
	    vvp_net_delete(local_net_pool[idx]);
      }
      حرر(local_net_pool);
      local_net_pool = 0;
      local_net_pool_گنتی = 0;

      map<vvp_net_t*, منطقي>::iterator iter;
      لكل (iter = vvp_net_map.begin(); iter != vvp_net_map.سرا(); ++ iter ) {
	    vvp_nets_del += 1;
	    VALGRIND_MEMPOOL_FREE(iter->پہلا->pool, iter->پہلا);
      }
      vvp_net_map.clear();

      map<sfunc_core*, منطقي>::iterator siter;
      لكل (siter = sfunc_map.begin(); siter != sfunc_map.سرا(); ++ siter ) {
	    مٹاؤ siter->پہلا;
      }
      sfunc_map.clear();

      map<vvp_udp_fun_core*, منطقي>::iterator uiter;
      لكل (uiter = udp_map.begin(); uiter != udp_map.سرا(); ++ uiter ) {
	    مٹاؤ uiter->پہلا;
      }
      udp_map.clear();

      map<resolv_core*, منطقي>::iterator riter;
      لكل (riter = resolv_map.begin(); riter != resolv_map.سرا(); ++ riter ) {
	    مٹاؤ riter->پہلا;
      }
      resolv_map.clear();

      إذا (RUNNING_ON_VALGRIND && (vvp_nets_del != گنتی_vvp_nets)) {
	    fflush(عدم);
	    VALGRIND_PRINTF("Error: vvp missed deleting %ld of %lu net(s).",
	                    (لما) گنتی_vvp_nets - vvp_nets_del,
	                    گنتی_vvp_nets);
      }

      لكل (بنانشانی idx = 0; idx < vvp_net_pool_گنتی; idx += 1) {
	    VALGRIND_DESTROY_MEMPOOL(vvp_net_pool[idx]);
	    ::مٹاؤ [] vvp_net_pool[idx];
      }
      حرر(vvp_net_pool);
      vvp_net_pool = عدم;
      vvp_net_pool_گنتی = 0;
}
#جے_انت

فراغ vvp_net_t::عامل مٹاؤ(فراغ*)
{
      أكد(0);
}

vvp_net_t::vvp_net_t()
: باہرلا_2(vvp_net_ptr_t(0,0))
{
      fun = 0;
      fil = 0;
}

فراغ vvp_net_t::link(vvp_net_ptr_t پورٹ_منزل_link)
{
      vvp_net_t*net = پورٹ_منزل_link.ptr();

	// Connect nodes with vvp_fun_modpath_src to the head of the
	// linked list, so that vvp_fun_modpath_src are always evaluated
	// before their respective vvp_fun_modpath
      إذا (dynamic_cast<vvp_fun_modpath_src*>(net->fun)) {
	    net->پورٹ[پورٹ_منزل_link.پورٹ()] = باہرلا_2;
	    باہرلا_2 = پورٹ_منزل_link;
	// All other nodes connect after the last vvp_fun_modpath_src
      } وإلا {
	      // List is empty or first node has no vvp_fun_modpath_src
	      // then just insert the node into the head of linked list
	    إذا (باہرلا_2.ptr() == خالی_سوچک || ! dynamic_cast<vvp_fun_modpath_src*>(باہرلا_2.ptr()->fun)) {
		  net->پورٹ[پورٹ_منزل_link.پورٹ()] = باہرلا_2;
		  باہرلا_2 = پورٹ_منزل_link;
		  أرجع;
	    }

	    vvp_net_ptr_t cur = باہرلا_2;
	    vvp_net_ptr_t prev = vvp_net_ptr_t(خالی_سوچک,0);

	      // Traverse through linked list
	    طالما (dynamic_cast<vvp_fun_modpath_src*>(cur.ptr()->fun)) {
		  prev = cur;
		  cur = cur.ptr()->پورٹ[cur.پورٹ()];
		  إذا (! cur.ptr()) اكسر; // End of list
	    }

	    أكد(prev.ptr());

	      // Insert after last vvp_fun_modpath_src (or end of list)
	    net->پورٹ[پورٹ_منزل_link.پورٹ()] = cur;
	    prev.ptr()->پورٹ[prev.پورٹ()] = پورٹ_منزل_link;
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
      بنانشانی net_پورٹ = dst_ptr.پورٹ();

      إذا (باہرلا_2 == dst_ptr) {
	      /* If the drive fan-out list starts with this pointer,
		 then the unlink is easy. Pull the list forward. */
	    باہرلا_2 = net->پورٹ[net_پورٹ];
      } وإلا إذا (! باہرلا_2.nil()) {
	      /* Scan the linked list, looking for the net_ptr_t
		 pointer *before* the one we wish to remove. */
	    vvp_net_ptr_t cur = باہرلا_2;
	    أكد(!cur.nil());
	    vvp_net_t*cur_net = cur.ptr();
	    بنانشانی cur_پورٹ = cur.پورٹ();
	    طالما (cur_net && cur_net->پورٹ[cur_پورٹ] != dst_ptr) {
		  cur = cur_net->پورٹ[cur_پورٹ];
		  cur_net = cur.ptr();
		  cur_پورٹ = cur.پورٹ();
	    }
	      /* Unlink. */
	    إذا (cur_net) cur_net->پورٹ[cur_پورٹ] = net->پورٹ[net_پورٹ];
      }

      net->پورٹ[net_پورٹ] = vvp_net_ptr_t(0,0);
}

فراغ vvp_net_t::گنتی_drivers(بنانشانی idx, بنانشانی counts[4])
{
      counts[0] = 0;
      counts[1] = 0;
      counts[2] = 0;
      counts[3] = 0;

        /* $countdrivers can only be used on wires. */
      vvp_wire_ادھار*wire=dynamic_cast<vvp_wire_ادھار*>(fil);
      أكد(wire);

      إذا (wire->is_forced(idx))
            counts[3] = 1;

        /* If the net has multiple drivers, we need to interrogate the
           resolver network to get the driven values. */
      إذا (resolv_core*resolver = dynamic_cast<resolv_core*>(fun)) {
            resolver->گنتی_drivers(idx, counts);
            أرجع;
      }
      إذا (vvp_island_پورٹ*resolver = dynamic_cast<vvp_island_پورٹ*>(fun)) {
            resolver->گنتی_drivers(idx, counts);
            أرجع;
      }

        /* If the functor is not a resolver, there is only one driver, so
           we can just interrogate the filter to get the driven value. */
      update_driver_counts(wire->driven_قدر(idx), counts);
}

فراغ vvp_net_fun_t::عامل مٹاؤ(فراغ*)
{
#جے_بنا_تعریف CHECK_WITH_VALGRIND
      أكد(0);
#جے_انت
}


vvp_net_fil_t::vvp_net_fil_t()
{
      force_link_ = 0;
      force_propagate_ = خطأ;
      گنتی_filters += 1;
}

vvp_net_fil_t::~vvp_net_fil_t()
{
      أكد(force_link_ == 0);
}

vvp_net_fil_t::prop_t vvp_net_fil_t::filter_vec4(ثابت vvp_vector4_t&,
                                                 vvp_vector4_t&,
                                                 بنانشانی, بنانشانی)
{
      أرجع PROP;
}

vvp_net_fil_t::prop_t vvp_net_fil_t::filter_vec8(ثابت vvp_vector8_t&,
                                                 vvp_vector8_t&,
                                                 بنانشانی, بنانشانی)
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
      لكل (بنانشانی idx = 0 ; idx < نقاب.size() ; idx += 1) {
	    إذا (نقاب.value(idx) == 0)
		  استمر;

	    force_نقاب_2.سیٹ_ثنائی_ہندسہ(idx, 1);
	    force_propagate_ = سچ;
      }
}

فراغ vvp_net_fil_t::release_نقاب(ثابت vvp_vector2_t&نقاب)
{
      إذا (force_نقاب_2.size() == 0)
	    أرجع;

      أكد(force_نقاب_2.size() == نقاب.size());
      لكل (بنانشانی idx = 0 ; idx < نقاب.size() ; idx += 1) {
	    إذا (نقاب.value(idx))
		  force_نقاب_2.سیٹ_ثنائی_ہندسہ(idx, 0);
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
	    force_link_->پورٹ[3] = vvp_net_ptr_t(dst, 0);
	    force_link_->پورٹ[2] = vvp_net_ptr_t(0,0);
	    force_link_->fun = جديد vvp_fun_force;
      }

      force_unlink();
      أكد(force_link_->پورٹ[2] == vvp_net_ptr_t(0,0));

	// Use port[2] to hold the force source.
      force_link_->پورٹ[2] = vvp_net_ptr_t(src,0);

      vvp_net_ptr_t dst_ptr(force_link_, 0);
      src->link(dst_ptr);
}

فراغ vvp_net_fil_t::عامل مٹاؤ(فراغ*)
{
      أكد(0);
}

فراغ vvp_net_fil_t::force_unlink(فراغ)
{
      إذا (force_link_ == 0) أرجع;
      vvp_net_t*src = force_link_->پورٹ[2].ptr();
      إذا (src == 0) أرجع;

      src->unlink(vvp_net_ptr_t(force_link_,0));
      force_link_->پورٹ[2] = vvp_net_ptr_t(0,0);
}

/* *** BIT operations *** */
vvp_bit4_t جوڑو_with_حاصل(vvp_bit4_t a, vvp_bit4_t b, vvp_bit4_t&c)
{
      إذا (bit4_is_xz(a) || bit4_is_xz(b) || bit4_is_xz(c)) {
	    c = BIT4_X;
	    أرجع BIT4_X;
      }

	// NOTE: This relies on the facts that XZ values have been
	// weeded out, and that BIT4_1 is 1 and BIT4_0 is 0.
      صحيح جوڑ = (صحيح)a + (صحيح)b + (صحيح)c;

      بدل (جوڑ) {
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
      fprintf(stderr, "Incorrect result %d.\n", جوڑ);
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

ostream& عامل<<(ostream&out, vvp_bit4_t ثنائی_ہندسہ)
{
      بدل (ثنائی_ہندسہ) {
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

تعريفنوع بنانشانی چھوٹا کنارا_t;

مضمن کنارا_t VVP_کنارا(vvp_bit4_t from, vvp_bit4_t to)
{
      أرجع 1 << ((from << 2) | to);
}

ثابت کنارا_t vvp_کنارا_posedge
      = VVP_کنارا(BIT4_0,BIT4_1)
      | VVP_کنارا(BIT4_0,BIT4_X)
      | VVP_کنارا(BIT4_0,BIT4_Z)
      | VVP_کنارا(BIT4_X,BIT4_1)
      | VVP_کنارا(BIT4_Z,BIT4_1)
      ;

ثابت کنارا_t vvp_کنارا_negedge
      = VVP_کنارا(BIT4_1,BIT4_0)
      | VVP_کنارا(BIT4_1,BIT4_X)
      | VVP_کنارا(BIT4_1,BIT4_Z)
      | VVP_کنارا(BIT4_X,BIT4_0)
      | VVP_کنارا(BIT4_Z,BIT4_0)
      ;

صحيح کنارا(vvp_bit4_t from, vvp_bit4_t to)
{
      کنارا_t نقاب = VVP_کنارا(from, to);
      إذا (نقاب & vvp_کنارا_posedge)
	    أرجع 1;
      إذا (نقاب & vvp_کنارا_negedge)
	    أرجع -1;
      أرجع 0;
}

بنانشانی لما multiply_with_حاصل(بنانشانی لما a, بنانشانی لما b,
				  بنانشانی لما&حاصل)
{
      ثابت بنانشانی لما نقاب = (1UL << (CPU_لفظ_بٹ/2)) - 1;
      بنانشانی لما a0 = a & نقاب;
      بنانشانی لما a1 = (a >> (CPU_لفظ_بٹ/2)) & نقاب;
      بنانشانی لما b0 = b & نقاب;
      بنانشانی لما b1 = (b >> (CPU_لفظ_بٹ/2)) & نقاب;

      بنانشانی لما tmp = a0 * b0;

      بنانشانی لما r00 = tmp & نقاب;
      بنانشانی لما c00 = (tmp >> (CPU_لفظ_بٹ/2)) & نقاب;

      tmp = a0 * b1;

      بنانشانی لما r01 = tmp & نقاب;
      بنانشانی لما c01 = (tmp >> (CPU_لفظ_بٹ/2)) & نقاب;

      tmp = a1 * b0;

      بنانشانی لما r10 = tmp & نقاب;
      بنانشانی لما c10 = (tmp >> (CPU_لفظ_بٹ/2)) & نقاب;

      tmp = a1 * b1;

      بنانشانی لما r11 = tmp & نقاب;
      بنانشانی لما c11 = (tmp >> (CPU_لفظ_بٹ/2)) & نقاب;

      بنانشانی لما r1 = c00 + r01 + r10;
      بنانشانی لما r2 = (r1 >> (CPU_لفظ_بٹ/2)) & نقاب;
      r1 &= نقاب;
      r2 += c01 + c10 + r11;
      بنانشانی لما r3 = (r2 >> (CPU_لفظ_بٹ/2)) & نقاب;
      r2 &= نقاب;
      r3 += c11;
      r3 &= نقاب;

      حاصل = (r3 << (CPU_لفظ_بٹ/2)) + r2;
      أرجع (r1 << (CPU_لفظ_بٹ/2)) + r00;
}


فراغ vvp_send_vec8(vvp_net_ptr_t ptr, ثابت vvp_vector8_t&val)
{
      طالما (vvp_net_t*cur = ptr.ptr()) {
	    vvp_net_ptr_t اگلا_val = cur->پورٹ[ptr.پورٹ()];

	    إذا (cur->fun)
		  cur->fun->recv_vec8(ptr, val);

	    ptr = اگلا_val;
      }
}

فراغ vvp_send_real(vvp_net_ptr_t ptr, مزدوج val, vvp_context_t context)
{
      طالما (vvp_net_t*cur = ptr.ptr()) {
	    vvp_net_ptr_t اگلا_val = cur->پورٹ[ptr.پورٹ()];

	    إذا (cur->fun)
		  cur->fun->recv_real(ptr, val, context);

	    ptr = اگلا_val;
      }
}

ثابت vvp_vector4_t vvp_vector4_t::nil;

فراغ vvp_vector4_t::copy_بٹ(ثابت vvp_vector4_t&that)
{

      إذا (ناپ_2 == that.ناپ_2) {
	    إذا (ناپ_2 > بٹ_فی_لفظ) {
		  بنانشانی الفاظ = (ناپ_2+بٹ_فی_لفظ-1) / بٹ_فی_لفظ;
		  لكل (بنانشانی idx = 0 ;  idx < الفاظ ;  idx += 1)
			abits_ptr_[idx] = that.abits_ptr_[idx];
		  لكل (بنانشانی idx = 0 ;  idx < الفاظ ;  idx += 1)
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
      إذا (ناپ_2 <= بٹ_فی_لفظ && that.ناپ_2 <= بٹ_فی_لفظ) {
	    بنانشانی بٹ_منزل_copy = (that.ناپ_2 < ناپ_2) ? that.ناپ_2 : ناپ_2;
	    بنانشانی لما نقاب = ((بٹ_منزل_copy < بٹ_فی_لفظ) ? (1UL << بٹ_منزل_copy) :
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
      إذا (ناپ_2 <= بٹ_فی_لفظ) {
	    abits_val_ = that.abits_ptr_[0];
	    bbits_val_ = that.bbits_ptr_[0];
	    إذا (ناپ_2 < بٹ_فی_لفظ) {
		  بنانشانی لما نقاب = (1UL << ناپ_2) - 1UL;
		  abits_val_ &= نقاب;
		  bbits_val_ &= نقاب;
	    }
	    أرجع;
      }

	/* Now we know that the destination must be long. If the
	   source is short, then mask/copy from its value. */
      إذا (that.ناپ_2 <= بٹ_فی_لفظ) {
	    بنانشانی لما نقاب;
	    إذا (that.ناپ_2 < بٹ_فی_لفظ) {
		  نقاب = (1UL << that.ناپ_2) - 1UL;
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
      بنانشانی بٹ_منزل_copy = (that.ناپ_2 < ناپ_2) ? that.ناپ_2 : ناپ_2;
      بنانشانی لفظ = 0;
      طالما (بٹ_منزل_copy >= بٹ_فی_لفظ) {
	    abits_ptr_[لفظ] = that.abits_ptr_[لفظ];
	    bbits_ptr_[لفظ] = that.bbits_ptr_[لفظ];
	    بٹ_منزل_copy -= بٹ_فی_لفظ;
	    لفظ += 1;
      }
      إذا (بٹ_منزل_copy > 0) {
	    بنانشانی لما نقاب = (1UL << بٹ_منزل_copy) - 1UL;
	    abits_ptr_[لفظ] &= ~نقاب;
	    bbits_ptr_[لفظ] &= ~نقاب;
	    abits_ptr_[لفظ] |= that.abits_ptr_[لفظ] & نقاب;
	    bbits_ptr_[لفظ] |= that.bbits_ptr_[لفظ] & نقاب;
      }
}

/*
 * This function should ONLY BE CALLED FROM vvp_vector4_t::copy_from_,
 * as it performs part of that functions tasks.
 */
فراغ vvp_vector4_t::copy_from_big_(ثابت vvp_vector4_t&that)
{
      بنانشانی الفاظ = (ناپ_2+بٹ_فی_لفظ-1) / بٹ_فی_لفظ;
      abits_ptr_ = جديد بنانشانی لما[2*الفاظ];
      bbits_ptr_ = abits_ptr_ + الفاظ;

      لكل (بنانشانی idx = 0 ;  idx < الفاظ ;  idx += 1)
	    abits_ptr_[idx] = that.abits_ptr_[idx];
      لكل (بنانشانی idx = 0 ;  idx < الفاظ ;  idx += 1)
	    bbits_ptr_[idx] = that.bbits_ptr_[idx];
}

/*
 * The copy_inverted_from_ method is just like the copy_from_ method,
 * except that we combine that with an invert. This allows the ~ and
 * the assignment to be blended in many common cases.
 */
فراغ vvp_vector4_t::copy_inverted_from_(ثابت vvp_vector4_t&that)
{
      ناپ_2 = that.ناپ_2;
      إذا (ناپ_2 > بٹ_فی_لفظ) {
	    بنانشانی الفاظ = (ناپ_2+بٹ_فی_لفظ-1) / بٹ_فی_لفظ;
	    abits_ptr_ = جديد بنانشانی لما[2*الفاظ];
	    bbits_ptr_ = abits_ptr_ + الفاظ;

	    بنانشانی بچیا = ناپ_2;
	    بنانشانی idx = 0;
	    طالما (بچیا >= بٹ_فی_لفظ) {
		  abits_ptr_[idx] = that.bbits_ptr_[idx] | ~that.abits_ptr_[idx];
		  idx += 1;
		  بچیا -= بٹ_فی_لفظ;
	    }
	    إذا (بچیا > 0) {
		  بنانشانی لما نقاب = (1UL<<بچیا) - 1UL;
		  abits_ptr_[idx] = نقاب & (that.bbits_ptr_[idx] | ~that.abits_ptr_[idx]);
	    }

	    لكل (idx = 0 ;  idx < الفاظ ;  idx += 1)
		  bbits_ptr_[idx] = that.bbits_ptr_[idx];

      } وإلا {
	    بنانشانی لما نقاب = (ناپ_2<بٹ_فی_لفظ)? (1UL<<ناپ_2)-1UL : -1UL;
	    abits_val_ = نقاب & (that.bbits_val_ | ~that.abits_val_);
	    bbits_val_ = that.bbits_val_;
      }
}

/* Make sure to set size_ before calling this routine. */
فراغ vvp_vector4_t::allocate_الفاظ(بنانشانی لما inita, بنانشانی لما initb)
{
      إذا (ناپ_2 > بٹ_فی_لفظ) {
	    بنانشانی cnt = (ناپ_2 + بٹ_فی_لفظ - 1) / بٹ_فی_لفظ;
	    abits_ptr_ = جديد بنانشانی لما[2*cnt];
	    bbits_ptr_ = abits_ptr_ + cnt;
	    لكل (بنانشانی idx = 0 ;  idx < cnt ;  idx += 1)
		  abits_ptr_[idx] = inita;
	    لكل (بنانشانی idx = 0 ;  idx < cnt ;  idx += 1)
		  bbits_ptr_[idx] = initb;

      } وإلا {
	    abits_val_ = inita;
	    bbits_val_ = initb;
      }
}

vvp_vector4_t::vvp_vector4_t(بنانشانی ناپ_3, مزدوج val)
: ناپ_2(ناپ_3)
{
      منطقي is_neg = خطأ;
      مزدوج fraction;
      صحيح exponent;

	/* We return 'bx for a NaN or +/- infinity. */
      إذا (val != val || (val && (val == 0.5*val)))  {
	    allocate_الفاظ(لفظ_X_ABITS, لفظ_X_BBITS);
	    أرجع;
      }

	/* Convert to a positive result. */
      إذا (val < 0.0) {
	    is_neg = سچ;
	    val = -val;
      }
      allocate_الفاظ(لفظ_0_ABITS, لفظ_0_BBITS);

	/* Get the exponent and fractional part of the number. */
      fraction = frexp(val, &exponent);

	/* If the value is small enough just use lround(). */
      إذا (exponent < بٹ_فی_لفظ-2) {
	    إذا (is_neg) هذا->invert();  // Invert the bits if negative.
	    لما sval = lround(val);
	    إذا (is_neg) sval = -sval;
	      /* This requires that 0 and 1 have the same bbit value. */
	    إذا (ناپ_2 > بٹ_فی_لفظ) {
		  abits_ptr_[0] = sval;
	    } وإلا {
		  abits_val_ = sval;
	    }
	    أرجع;
      }

      بنانشانی nwords = (exponent-1) / بٹ_فی_لفظ;
      بنانشانی my_الفاظ = (ناپ_2 + بٹ_فی_لفظ - 1) / بٹ_فی_لفظ - 1;

      fraction = ldexp(fraction, (exponent-1) % بٹ_فی_لفظ + 1);

	/* Skip any leading bits. */
      لكل (صحيح idx = (نشانیوالا) nwords; idx > (نشانیوالا) my_الفاظ; idx -=1) {
	    بنانشانی لما بٹ = (بنانشانی لما) fraction;
	    fraction = fraction - (مزدوج) بٹ;
	    fraction = ldexp(fraction, بٹ_فی_لفظ);
      }

	/* Convert the remaining bits as appropriate. */
      إذا (my_الفاظ == 0) {
		  بنانشانی لما بٹ = (بنانشانی لما) fraction;
		  abits_val_ = بٹ;
		  fraction = fraction - (مزدوج) بٹ;
		    /* Round any fractional part up. */
		  إذا (fraction >= 0.5) *هذا += (int64_t) 1;
      } وإلا {
	    إذا (nwords < my_الفاظ) my_الفاظ = nwords;
	    لكل (صحيح idx = (نشانیوالا)my_الفاظ; idx >= 0; idx -= 1) {
		  بنانشانی لما بٹ = (بنانشانی لما) fraction;
		  abits_ptr_[idx] = بٹ;
		  fraction = fraction - (مزدوج) بٹ;
		  fraction = ldexp(fraction, بٹ_فی_لفظ);
	    }
	      /* Round any fractional part up. */
	    إذا (fraction >= ldexp(0.5, بٹ_فی_لفظ)) *هذا += (int64_t) 1;
       }

	/* Convert to a negative number if needed. */
      إذا (is_neg) {
	    هذا->invert();
	    *هذا += (int64_t) 1;
      }
}

vvp_vector4_t::vvp_vector4_t(ثابت vvp_vector4_t&that,
			    بنانشانی adr, بنانشانی wid)
{
	// Set up and initialize the destination.
      ناپ_2 = wid;
      allocate_الفاظ(لفظ_X_ABITS, لفظ_X_BBITS);

	// Special case: selecting from far beyond the source vector,
	// to the result is all X bits. We're done.
      إذا (adr >= that.ناپ_2)
	    أرجع;

	// Special case: The source is not quite big enough to supply
	// all bits, so get the bits that we can. The remainder will
	// be left at BIT4_X.
      إذا ((adr + wid) > that.ناپ_2) {
	    بنانشانی use_wid = that.ناپ_2 - adr;
	    لكل (بنانشانی idx = 0 ; idx < use_wid ; idx += 1)
		  سیٹ_ثنائی_ہندسہ(idx, that.value(adr+idx));

	    أرجع;
      }

	// At the point, we know that the source part is entirely
	// contained in the source vector.
	// assert((adr + wid) <= that.size_);

      إذا (wid > بٹ_فی_لفظ) {
	      /* In this case, the subvector and the source vector are
		 long. Do the transfer reasonably efficiently. */
	    بنانشانی ptr = adr / بٹ_فی_لفظ;
	    بنانشانی لما off = adr % بٹ_فی_لفظ;
	    بنانشانی لما noff = بٹ_فی_لفظ - off;
	    بنانشانی لما lmask = (1UL << off) - 1UL;
	    بنانشانی trans = 0;
	    بنانشانی dst = 0;
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
		  إذا (noff != بٹ_فی_لفظ) {
			abits_ptr_[dst] |= (that.abits_ptr_[ptr]&lmask) << noff;
			bbits_ptr_[dst] |= (that.bbits_ptr_[ptr]&lmask) << noff;
			trans += off;
		  }

		  dst += 1;
	    }

      } وإلا إذا (that.ناپ_2 > بٹ_فی_لفظ) {
	      /* In this case, the subvector fits in a single word,
		 but the source is large. */
	    بنانشانی ptr = adr / بٹ_فی_لفظ;
	    بنانشانی لما off = adr % بٹ_فی_لفظ;
	    بنانشانی trans = بٹ_فی_لفظ - off;
	    إذا (trans > wid)
		  trans = wid;

	    إذا (trans == بٹ_فی_لفظ) {
		    // Very special case: Copy exactly 1 perfectly
		    // aligned word.
		  abits_val_ = that.abits_ptr_[ptr];
		  bbits_val_ = that.bbits_ptr_[ptr];

	    } وإلا {
		    // lmask is the low bits of the destination,
		    // masked into the source.
		  بنانشانی لما lmask = (1UL<<trans) - 1UL;
		  lmask <<= off;

		    // The low bits of the result.
		  abits_val_ = (that.abits_ptr_[ptr] & lmask) >> off;
		  bbits_val_ = (that.bbits_ptr_[ptr] & lmask) >> off;

		  إذا (trans < wid) {
			  // If there are more bits, then get them
			  // from the bottom of the next word of the
			  // source.
			بنانشانی لما hmask = (1UL << (wid-trans)) - 1UL;

			  // The high bits of the result.
			abits_val_ |= (that.abits_ptr_[ptr+1]&hmask) << trans;
			bbits_val_ |= (that.bbits_ptr_[ptr+1]&hmask) << trans;
		  }
	    }

      } وإلا إذا (ناپ_2 == بٹ_فی_لفظ) {
	      /* We know that source and destination are short. If the
		 destination is a full word, then we know the copy is
		 aligned and complete. */
	    abits_val_ = that.abits_val_;
	    bbits_val_ = that.bbits_val_;

      } وإلا {
	      /* Finally, the source and destination vectors are both
		 short, so there is a single mask/shift/copy. */
	    بنانشانی لما نقاب = (1UL << ناپ_2) - 1UL;
	    نقاب <<= adr;

	    abits_val_ = (that.abits_val_ & نقاب) >> adr;
	    bbits_val_ = (that.bbits_val_ & نقاب) >> adr;
      }

}

/*
 * Change the size of the vvp_vector4_t vector to the new size. Copy
 * the old values, as many as well fit, into the new vector.
 */
فراغ vvp_vector4_t::resize(بنانشانی newsize, vvp_bit4_t pad_ثنائی_ہندسہ)
{
      إذا (ناپ_2 == newsize)
	    أرجع;

      بنانشانی cnt = (ناپ_2 + بٹ_فی_لفظ - 1) / بٹ_فی_لفظ;
      بنانشانی لما لفظ_pad_abits = 0;
      بنانشانی لما لفظ_pad_bbits = 0;
      بدل (pad_ثنائی_ہندسہ) {
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

      إذا (newsize > بٹ_فی_لفظ) {
	    بنانشانی newcnt = (newsize + بٹ_فی_لفظ - 1) / بٹ_فی_لفظ;
	    إذا (newcnt == cnt) {
		    // If the word count doesn't change, then there is
		    // no need for re-allocation so we are done now.
		  إذا (newsize > ناپ_2) {
			إذا (بنانشانی fill = ناپ_2 % بٹ_فی_لفظ) {
			      abits_ptr_[cnt-1] &= ~((-1UL) << fill);
			      bbits_ptr_[cnt-1] &= ~((-1UL) << fill);
			      abits_ptr_[cnt-1] |= لفظ_pad_abits << fill;
			      bbits_ptr_[cnt-1] |= لفظ_pad_bbits << fill;
			}
		  }
		  ناپ_2 = newsize;
		  أرجع;
	    }

	    بنانشانی لما*newbits = جديد بنانشانی لما[2*newcnt];

	    إذا (cnt > 1) {
		  بنانشانی trans = cnt;
		  إذا (trans > newcnt)
			trans = newcnt;

		  لكل (بنانشانی idx = 0 ;  idx < trans ;  idx += 1)
			newbits[idx] = abits_ptr_[idx];
		  لكل (بنانشانی idx = 0 ;  idx < trans ;  idx += 1)
			newbits[newcnt+idx] = bbits_ptr_[idx];

		  مٹاؤ[]abits_ptr_;

	    } وإلا {
		  newbits[0] = abits_val_;
		  newbits[newcnt] = bbits_val_;
	    }

	    إذا (newsize > ناپ_2) {
		  إذا (بنانشانی fill = ناپ_2 % بٹ_فی_لفظ) {
			newbits[cnt-1] &= ~((-1UL) << fill);
			newbits[cnt-1] |= لفظ_pad_abits << fill;
			newbits[newcnt+cnt-1] &= ~((-1UL) << fill);
			newbits[newcnt+cnt-1] |= لفظ_pad_bbits << fill;
		  }
		  لكل (بنانشانی idx = cnt ;  idx < newcnt ;  idx += 1)
			newbits[idx] = لفظ_pad_abits;
		  لكل (بنانشانی idx = cnt ;  idx < newcnt ;  idx += 1)
			newbits[newcnt+idx] = لفظ_pad_bbits;
	    }

	    ناپ_2 = newsize;
	    abits_ptr_ = newbits;
	    bbits_ptr_ = newbits + newcnt;

      } وإلا {
	    إذا (cnt > 1) {
		  بنانشانی لما newvala = abits_ptr_[0];
		  بنانشانی لما newvalb = bbits_ptr_[0];
		  مٹاؤ[]abits_ptr_;
		  abits_val_ = newvala;
		  bbits_val_ = newvalb;
	    }

	    إذا (newsize > ناپ_2) {
		  abits_val_ &= ~((-1UL) << ناپ_2);
		  bbits_val_ &= ~((-1UL) << ناپ_2);
		  abits_val_ |= لفظ_pad_abits << ناپ_2;
		  bbits_val_ |= لفظ_pad_bbits << ناپ_2;
	    }

	    ناپ_2 = newsize;
      }
}


بنانشانی لما* vvp_vector4_t::subarray(بنانشانی adr, بنانشانی wid, منطقي xz_منزل_0) ثابت
{
      ثابت بنانشانی BIT2_فی_لفظ = 8*حجم(بنانشانی لما);
      بنانشانی awid = (wid + BIT2_فی_لفظ - 1) / (BIT2_فی_لفظ);
      بنانشانی لما*val = جديد بنانشانی لما[awid];

      لكل (بنانشانی idx = 0 ;  idx < awid ;  idx += 1)
	    val[idx] = 0;

      إذا (ناپ_2 <= بٹ_فی_لفظ) {
	      /* Handle the special case that the array is small. The
		 entire value of the vector4 is within the xbits_val_
		 so we know that the result is a single word, the
		 source is a single word, and we just have to loop
		 through that word. */
	    بنانشانی لما atmp = abits_val_ >> adr;
	    بنانشانی لما btmp = bbits_val_ >> adr;
	    إذا (wid < BIT2_فی_لفظ) {
		  atmp &= (1UL << wid) - 1;
		  btmp &= (1UL << wid) - 1;
	    }
	    إذا (btmp) {
		  إذا (xz_منزل_0)
			atmp &= ~btmp;
		  وإلا
			اذهب x_باہرلا;
	    }

	    val[0] = atmp;

      } وإلا {

	    بنانشانی val_ptr = 0;
	    بنانشانی val_off = 0;

	      /* Get the first word we are scanning. We may in fact be
		 somewhere in the middle of that word. */
	    طالما (wid > 0) {
		  بنانشانی لما atmp = abits_ptr_[adr/بٹ_فی_لفظ];
		  بنانشانی لما btmp = bbits_ptr_[adr/بٹ_فی_لفظ];
		  بنانشانی لما off = adr%بٹ_فی_لفظ;
		  atmp >>= off;
		  btmp >>= off;

		  بنانشانی لما trans = بٹ_فی_لفظ - off;
		  إذا (trans > (BIT2_فی_لفظ - val_off))
			trans = BIT2_فی_لفظ - val_off;
		  إذا (wid < trans)
			trans = wid;
		  إذا (trans < BIT2_فی_لفظ) {
			atmp &= (1UL << trans) - 1;
			btmp &= (1UL << trans) - 1;
		  }
		  إذا (btmp) {
			إذا (xz_منزل_0)
			      atmp &= ~btmp;
			وإلا
			      اذهب x_باہرلا;
		  }

		  val[val_ptr] |= atmp << val_off;
		  adr += trans;
		  wid -= trans;
		  val_off += trans;
		  إذا (val_off == BIT2_فی_لفظ) {
			val_ptr += 1;
			val_off = 0;
		  }
	    }
      }

      أرجع val;

 x_باہرلا:
      مٹاؤ[]val;
      أرجع 0;
}

فراغ vvp_vector4_t::setarray(بنانشانی adr, بنانشانی wid, ثابت بنانشانی لما*val)
{
      أكد(adr+wid <= ناپ_2);

      ثابت بنانشانی BIT2_فی_لفظ = 8*حجم(بنانشانی لما);

      إذا (ناپ_2 <= بٹ_فی_لفظ) {
	      // We know here that both the source and the target are
	      // within a single word. Write the bits into the
	      // abits_val_ directly.

	    أكد(BIT2_فی_لفظ <= بٹ_فی_لفظ);
	    بنانشانی لما lmask = (1UL << adr) - 1UL;
	    بنانشانی لما hmask = ((adr+wid) < بٹ_فی_لفظ)
		  ? -1UL << (adr+wid)
		  : 0;
	    بنانشانی لما نقاب = ~(hmask | lmask);

	    abits_val_ &= ~نقاب;
	    bbits_val_ &= ~نقاب;

	    abits_val_ |= نقاب & (val[0] << adr);

      } وإلا {
	      // The general case, there are multiple words of
	      // destination, and possibly multiple words of source
	      // data. Shift and mask as we go.
	    بنانشانی off = adr % بٹ_فی_لفظ;
	    بنانشانی ptr = adr / بٹ_فی_لفظ;
	    بنانشانی val_off = 0;
	    بنانشانی val_ptr = 0;
	    طالما (wid > 0) {
		  بنانشانی trans = wid;
		  إذا (trans > (BIT2_فی_لفظ-val_off))
			trans = BIT2_فی_لفظ-val_off;
		  إذا (trans > (بٹ_فی_لفظ-off))
			trans = بٹ_فی_لفظ-off;

		  بنانشانی لما lmask = (1UL << off) - 1UL;
		  بنانشانی لما hmask = ((off+trans) < بٹ_فی_لفظ)
			? -1UL << (off+trans)
			: 0;
		  بنانشانی لما نقاب = ~(hmask | lmask);

		  abits_ptr_[ptr] &= ~نقاب;
		  bbits_ptr_[ptr] &= ~نقاب;
		  إذا (val_off >= off)
			abits_ptr_[ptr] |= نقاب & (val[val_ptr] >> (val_off-off));
		  وإلا
			abits_ptr_[ptr] |= نقاب & (val[val_ptr] << (off-val_off));

		  wid -= trans;
		  val_off += trans;
		  إذا (val_off == BIT2_فی_لفظ) {
			val_off = 0;
			val_ptr += 1;
		  }
		  off += trans;
		  إذا (off == بٹ_فی_لفظ) {
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
منطقي vvp_vector4_t::سیٹ_vec(بنانشانی adr, ثابت vvp_vector4_t&that)
{
      أكد(adr+that.ناپ_2  <= ناپ_2);
      منطقي diff_flag = خطأ;

      إذا (ناپ_2 <= بٹ_فی_لفظ) {

	      /* The destination vector (me!) is within a bits_val_
		 word, so the subvector is certainly within a
		 bits_val_ word. Therefore, the entire operation is a
		 matter of writing the bits of that into the addressed
		 bits of this. The mask below is calculated to be 1
		 for all the bits that are to come from that. Do the
		 job by some shifting, masking and OR. */

	    بنانشانی لما lmask = (1UL << adr) - 1;
	    بنانشانی لما hmask;
	    بنانشانی لما hshift = adr+that.ناپ_2;
	    إذا (hshift >= بٹ_فی_لفظ)
		  hmask = -1UL;
	    وإلا
		  hmask = (1UL << (adr+that.ناپ_2)) - 1;
	    بنانشانی لما نقاب = hmask & ~lmask;

	    بنانشانی لما tmp = (that.abits_val_<<adr)&نقاب;
	    إذا ((abits_val_&نقاب) != tmp) {
		  diff_flag = سچ;
		  abits_val_ = (abits_val_ & ~نقاب) | tmp;
	    }
	    tmp = (that.bbits_val_<<adr) & نقاب;
	    إذا ((bbits_val_&نقاب) != tmp) {
		  diff_flag = سچ;
		  bbits_val_ = (bbits_val_ & ~نقاب) | tmp;
	    }

      } وإلا إذا (that.ناپ_2 <= بٹ_فی_لفظ) {

	      /* This vector is more than a word, but that vector is
		 still small. Write into the destination, possibly
		 spanning two destination words, depending on whether
		 the source vector spans a word transition. */
	    بنانشانی لما dptr = adr / بٹ_فی_لفظ;
	    بنانشانی لما doff = adr % بٹ_فی_لفظ;

	    بنانشانی لما lmask = (1UL << doff) - 1;
	    بنانشانی لما hshift = doff+that.ناپ_2;
	    بنانشانی لما hmask;
	    إذا (hshift >= بٹ_فی_لفظ)
		  hmask = -1UL;
	    وإلا
		  hmask = (1UL << hshift) - 1UL;

	    بنانشانی لما نقاب = hmask & ~lmask;
	    بنانشانی لما tmp;

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

	    إذا ((doff + that.ناپ_2) > بٹ_فی_لفظ) {
		  بنانشانی پوچھ = doff + that.ناپ_2 - بٹ_فی_لفظ;
		  نقاب = (1UL << پوچھ) - 1;

		  dptr += 1;
		  tmp = (that.abits_val_ >> (that.ناپ_2-پوچھ)) & نقاب;
		  إذا ((abits_ptr_[dptr] & نقاب) != tmp) {
			diff_flag = سچ;
			abits_ptr_[dptr] = (abits_ptr_[dptr] & ~نقاب) | tmp;
		  }
		  tmp = (that.bbits_val_ >> (that.ناپ_2-پوچھ)) & نقاب;
		  إذا ((bbits_ptr_[dptr] & نقاب) != tmp) {
			diff_flag = سچ;
			bbits_ptr_[dptr] = (bbits_ptr_[dptr] & ~نقاب) | tmp;
		  }
	    }

      } وإلا إذا (adr%بٹ_فی_لفظ == 0) {

	      /* In this case, both vectors are long, but the
		 destination is neatly aligned. That means all but the
		 last word can be simply copied with no masking. */

	    بنانشانی remain = that.ناپ_2;
	    بنانشانی sptr = 0;
	    بنانشانی dptr = adr / بٹ_فی_لفظ;
	    طالما (remain >= بٹ_فی_لفظ) {
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
		  remain -= بٹ_فی_لفظ;
	    }

	    إذا (remain > 0) {
		  بنانشانی لما نقاب = (1UL << remain) - 1;
		  بنانشانی لما tmp;

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

	    بنانشانی remain = that.ناپ_2;
	    بنانشانی sptr = 0;
	    بنانشانی dptr = adr / بٹ_فی_لفظ;
	    بنانشانی doff = adr % بٹ_فی_لفظ;
	    بنانشانی لما lmask = (1UL << doff) - 1;
	    بنانشانی ndoff = بٹ_فی_لفظ - doff;
	    طالما (remain >= بٹ_فی_لفظ) {
		  بنانشانی لما tmp;

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

		  remain -= بٹ_فی_لفظ;
		  sptr += 1;
	    }

	    إذا (remain > 0) {
		  بنانشانی لما hshift = doff+remain;
		  بنانشانی لما hmask;
		  إذا (hshift >= بٹ_فی_لفظ)
			hmask = -1UL;
		  وإلا
			hmask = (1UL << (doff+remain)) - 1;

		  بنانشانی لما نقاب = hmask & ~lmask;
		  بنانشانی لما tmp;

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

		  إذا ((doff + remain) > بٹ_فی_لفظ) {
			بنانشانی پوچھ = doff + remain - بٹ_فی_لفظ;
			إذا (پوچھ >= بٹ_فی_لفظ)
			      نقاب = -1UL;
			وإلا
			      نقاب = (1UL << پوچھ) - 1;

			dptr += 1;

			tmp = (that.abits_ptr_[sptr] >> (remain-پوچھ))&نقاب;
			إذا ((abits_ptr_[dptr] & نقاب) != tmp) {
			      diff_flag = سچ;
			      abits_ptr_[dptr] = (abits_ptr_[dptr] & ~نقاب) | tmp;
			}
			tmp = (that.bbits_ptr_[sptr] >> (remain-پوچھ))&نقاب;
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
فراغ vvp_vector4_t::جوڑو(ثابت vvp_vector4_t&that)
{
      أكد(ناپ_2 == that.ناپ_2);

      إذا (ناپ_2 < بٹ_فی_لفظ) {
	    بنانشانی لما نقاب = ~(-1UL << ناپ_2);
	    إذا ((bbits_val_|that.bbits_val_) & نقاب) {
		  abits_val_ |= نقاب;
		  bbits_val_ |= نقاب;
		  أرجع;
	    }

	    abits_val_ += that.abits_val_;
	    abits_val_ &= نقاب;
	    أرجع;
      }

      إذا (ناپ_2 == بٹ_فی_لفظ) {
	    إذا (bbits_val_ | that.bbits_val_) {
		  abits_val_ = لفظ_X_ABITS;
		  bbits_val_ = لفظ_X_BBITS;
	    } وإلا {
		  abits_val_ += that.abits_val_;
	    }
	    أرجع;
      }

      صحيح cnt = ناپ_2 / بٹ_فی_لفظ;
      بنانشانی لما حاصل = 0;
      لكل (صحيح idx = 0 ; idx < cnt ; idx += 1) {
	    إذا (bbits_ptr_[idx] | that.bbits_ptr_[idx])
		  اذهب x_باہرلا;

	    abits_ptr_[idx] = جوڑو_with_حاصل(abits_ptr_[idx], that.abits_ptr_[idx], حاصل);
      }

      إذا (بنانشانی پوچھ = ناپ_2 % بٹ_فی_لفظ) {
	    بنانشانی لما نقاب = ~( -1UL << پوچھ );
	    إذا ((bbits_ptr_[cnt] | that.bbits_ptr_[cnt])&نقاب)
		  اذهب x_باہرلا;

	    abits_ptr_[cnt] = جوڑو_with_حاصل(abits_ptr_[cnt], that.abits_ptr_[cnt], حاصل);
	    abits_ptr_[cnt] &= نقاب;
      }

      أرجع;

 x_باہرلا:
      لكل (صحيح idx = 0 ; idx < cnt ; idx += 1) {
	    abits_ptr_[idx] = لفظ_X_ABITS;
	    bbits_ptr_[idx] = لفظ_X_BBITS;
      }
      إذا (بنانشانی پوچھ = ناپ_2%بٹ_فی_لفظ) {
	    بنانشانی لما نقاب = ~( -1UL << پوچھ );
	    abits_ptr_[cnt] = لفظ_X_ABITS&نقاب;
	    bbits_ptr_[cnt] = لفظ_X_BBITS&نقاب;
      }
}

فراغ vvp_vector4_t::sub(ثابت vvp_vector4_t&that)
{
      أكد(ناپ_2 == that.ناپ_2);

      إذا (ناپ_2 < بٹ_فی_لفظ) {
	    بنانشانی لما نقاب = ~(-1UL << ناپ_2);
	    إذا ((bbits_val_|that.bbits_val_) & نقاب) {
		  abits_val_ |= نقاب;
		  bbits_val_ |= نقاب;
		  أرجع;
	    }

	    abits_val_ -= that.abits_val_;
	    abits_val_ &= نقاب;
	    أرجع;
      }

      إذا (ناپ_2 == بٹ_فی_لفظ) {
	    إذا (bbits_val_ | that.bbits_val_) {
		  abits_val_ = لفظ_X_ABITS;
		  bbits_val_ = لفظ_X_BBITS;
	    } وإلا {
		  abits_val_ -= that.abits_val_;
	    }
	    أرجع;
      }

      صحيح cnt = ناپ_2 / بٹ_فی_لفظ;
      بنانشانی لما حاصل = 1;
      لكل (صحيح idx = 0 ; idx < cnt ; idx += 1) {
	    إذا (bbits_ptr_[idx] | that.bbits_ptr_[idx])
		  اذهب x_باہرلا;

	    abits_ptr_[idx] = جوڑو_with_حاصل(abits_ptr_[idx], ~that.abits_ptr_[idx], حاصل);
      }

      إذا (بنانشانی پوچھ = ناپ_2 % بٹ_فی_لفظ) {
	    بنانشانی لما نقاب = ~( -1UL << پوچھ );
	    إذا ((bbits_ptr_[cnt] | that.bbits_ptr_[cnt])&نقاب)
		  اذهب x_باہرلا;

	    abits_ptr_[cnt] = جوڑو_with_حاصل(abits_ptr_[cnt], ~that.abits_ptr_[cnt], حاصل);
	    abits_ptr_[cnt] &= نقاب;
      }

      أرجع;

 x_باہرلا:
      لكل (صحيح idx = 0 ; idx < cnt ; idx += 1) {
	    abits_ptr_[idx] = لفظ_X_ABITS;
	    bbits_ptr_[idx] = لفظ_X_BBITS;
      }
      إذا (بنانشانی پوچھ = ناپ_2%بٹ_فی_لفظ) {
	    بنانشانی لما نقاب = ~( -1UL << پوچھ );
	    abits_ptr_[cnt] = لفظ_X_ABITS&نقاب;
	    bbits_ptr_[cnt] = لفظ_X_BBITS&نقاب;
      }

}

فراغ vvp_vector4_t::mov(بنانشانی dst, بنانشانی src, بنانشانی cnt)
{
      أكد(dst+cnt <= ناپ_2);
      أكد(src+cnt <= ناپ_2);

      إذا (ناپ_2 <= بٹ_فی_لفظ) {
	    بنانشانی لما vmask = (1UL << cnt) - 1;
	    بنانشانی لما tmp;

	    tmp = (abits_val_ >> src) & vmask;
	    abits_val_ &= ~ (vmask << dst);
	    abits_val_ |= tmp << dst;

	    tmp = (bbits_val_ >> src) & vmask;
	    bbits_val_ &= ~ (vmask << dst);
	    bbits_val_ |= tmp << dst;

      } وإلا {
	    بنانشانی sptr = src / بٹ_فی_لفظ;
	    بنانشانی dptr = dst / بٹ_فی_لفظ;
	    بنانشانی soff = src % بٹ_فی_لفظ;
	    بنانشانی doff = dst % بٹ_فی_لفظ;

	    طالما (cnt > 0) {
		  بنانشانی trans = cnt;
		  إذا ((soff+trans) > بٹ_فی_لفظ)
			trans = بٹ_فی_لفظ - soff;

		  إذا ((doff+trans) > بٹ_فی_لفظ)
			trans = بٹ_فی_لفظ - doff;

		  إذا (trans == بٹ_فی_لفظ) {
			  // Special case: the transfer count is
			  // exactly an entire word. For this to be
			  // true, it must also be true that the
			  // pointers are aligned. The work is easy,
			abits_ptr_[dptr] = abits_ptr_[sptr];
			bbits_ptr_[dptr] = bbits_ptr_[sptr];
			dptr += 1;
			sptr += 1;
			cnt -= بٹ_فی_لفظ;
			استمر;
		  }

		    // Here we know that either the source or
		    // destination is unaligned, and also we know that
		    // the count is less than a full word.
		  بنانشانی لما vmask = (1UL << trans) - 1;
		  بنانشانی لما tmp;

		  tmp = (abits_ptr_[sptr] >> soff) & vmask;
		  abits_ptr_[dptr] &= ~ (vmask << doff);
		  abits_ptr_[dptr] |= tmp << doff;

		  tmp = (bbits_ptr_[sptr] >> soff) & vmask;
		  bbits_ptr_[dptr] &= ~ (vmask << doff);
		  bbits_ptr_[dptr] |= tmp << doff;

		  cnt -= trans;
		  soff += trans;
		  إذا (soff >= بٹ_فی_لفظ) {
			soff = 0;
			sptr += 1;
		  }
		  doff += trans;
		  إذا (doff >= بٹ_فی_لفظ) {
			doff = 0;
			dptr += 1;
		  }
	    }
      }
}

فراغ vvp_vector4_t::mul(ثابت vvp_vector4_t&that)
{
      أكد(ناپ_2 == that.ناپ_2);

      إذا (ناپ_2 < بٹ_فی_لفظ) {
	    بنانشانی لما نقاب = ~(-1UL << ناپ_2);
	    إذا ((bbits_val_|that.bbits_val_) & نقاب) {
		  abits_val_ |= نقاب;
		  bbits_val_ |= نقاب;
		  أرجع;
	    }

	    abits_val_ *= that.abits_val_;
	    abits_val_ &= نقاب;
	    أرجع;
      }

      إذا (ناپ_2 == بٹ_فی_لفظ) {
	    إذا (bbits_val_ || that.bbits_val_) {
		  abits_val_ = لفظ_X_ABITS;
		  bbits_val_ = لفظ_X_BBITS;
	    } وإلا {
		  abits_val_ *= that.abits_val_;
	    }
	    أرجع;
      }

      ثابت صحيح cnt = (ناپ_2+بٹ_فی_لفظ-1) / بٹ_فی_لفظ;

      بنانشانی لما نقاب;
      إذا (بنانشانی پوچھ = ناپ_2%بٹ_فی_لفظ) {
	    نقاب = ~( -1UL << پوچھ );
      } وإلا {
	    نقاب = ~0UL;
      }

	// Check for any XZ values ahead of time in a first pass. If
	// we find any, then force the entire result to be X and be
	// done.
      لكل (صحيح idx = 0 ; idx < cnt ; idx += 1) {
	    بنانشانی لما lval = bbits_ptr_[idx];
	    بنانشانی لما rval = that.bbits_ptr_[idx];
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
      بنانشانی لما*res = جديد بنانشانی لما[cnt];
      لكل (صحيح idx = 0 ; idx < cnt ; idx += 1)
	    res[idx] = 0;

      لكل (صحيح mul_a = 0 ; mul_a < cnt ; mul_a += 1) {
	    بنانشانی لما lval = abits_ptr_[mul_a];
	    إذا (mul_a == (cnt-1))
		  lval &= نقاب;

	    لكل (صحيح mul_b = 0 ; mul_b < (cnt-mul_a) ; mul_b += 1) {
		  بنانشانی لما rval = that.abits_ptr_[mul_b];
		  إذا (mul_b == (cnt-1))
			rval &= نقاب;

		  بنانشانی لما جوڑ;
		  بنانشانی لما tmp = multiply_with_حاصل(lval, rval, جوڑ);
		  صحيح ادھار = mul_a + mul_b;
		  بنانشانی لما حاصل = 0;
		  res[ادھار] = جوڑو_with_حاصل(res[ادھار], tmp, حاصل);
		  لكل (صحيح جوڑو_idx = ادھار+1 ; جوڑو_idx < cnt ; جوڑو_idx += 1) {
			res[جوڑو_idx] = جوڑو_with_حاصل(res[جوڑو_idx], جوڑ, حاصل);
			جوڑ = 0;
		  }
	    }
      }

	// Replace the "this" value with the calculated result. We
	// know a-priori that the bbits are zero and unchanged.
      res[cnt-1] &= نقاب;
      لكل (صحيح idx = 0 ; idx < cnt ; idx += 1)
	    abits_ptr_[idx] = res[idx];

      مٹاؤ[]res;
      أرجع;


}

منطقي vvp_vector4_t::eeq(ثابت vvp_vector4_t&that) ثابت
{
      إذا (ناپ_2 != that.ناپ_2)
	    أرجع خطأ;

      إذا (ناپ_2 < بٹ_فی_لفظ) {
	    بنانشانی لما نقاب = (1UL << ناپ_2) - 1;
	    أرجع (abits_val_&نقاب) == (that.abits_val_&نقاب)
		  && (bbits_val_&نقاب) == (that.bbits_val_&نقاب);
      }

      إذا (ناپ_2 == بٹ_فی_لفظ) {
	    أرجع (abits_val_ == that.abits_val_)
		  && (bbits_val_ == that.bbits_val_);
      }

      بنانشانی الفاظ = ناپ_2 / بٹ_فی_لفظ;
      لكل (بنانشانی idx = 0 ;  idx < الفاظ ;  idx += 1) {
	    إذا (abits_ptr_[idx] != that.abits_ptr_[idx])
		  أرجع خطأ;
	    إذا (bbits_ptr_[idx] != that.bbits_ptr_[idx])
		  أرجع خطأ;
      }

      بنانشانی لما نقاب = ناپ_2%بٹ_فی_لفظ;
      إذا (نقاب > 0) {
	    نقاب = (1UL << نقاب) - 1;
	    أرجع (abits_ptr_[الفاظ]&نقاب) == (that.abits_ptr_[الفاظ]&نقاب)
		  && (bbits_ptr_[الفاظ]&نقاب) == (that.bbits_ptr_[الفاظ]&نقاب);
      }

      أرجع سچ;
}

منطقي vvp_vector4_t::eq_xz(ثابت vvp_vector4_t&that) ثابت
{
      إذا (ناپ_2 != that.ناپ_2)
	    أرجع خطأ;

      إذا (ناپ_2 < بٹ_فی_لفظ) {
	    بنانشانی لما نقاب = (1UL << ناپ_2) - 1;
	    أرجع ((abits_val_|bbits_val_)&نقاب) == ((that.abits_val_|that.bbits_val_)&نقاب)
		  && (bbits_val_&نقاب) == (that.bbits_val_&نقاب);
      }

      إذا (ناپ_2 == بٹ_فی_لفظ) {
	    أرجع ((abits_val_|bbits_val_) == (that.abits_val_|that.bbits_val_))
		  && (bbits_val_ == that.bbits_val_);
      }

      بنانشانی الفاظ = ناپ_2 / بٹ_فی_لفظ;
      لكل (بنانشانی idx = 0 ;  idx < الفاظ ;  idx += 1) {
	    إذا ((abits_ptr_[idx]|bbits_ptr_[idx]) != (that.abits_ptr_[idx]|that.bbits_ptr_[idx]))
		  أرجع خطأ;
	    إذا (bbits_ptr_[idx] != that.bbits_ptr_[idx])
		  أرجع خطأ;
      }

      بنانشانی لما نقاب = ناپ_2%بٹ_فی_لفظ;
      إذا (نقاب > 0) {
	    نقاب = (1UL << نقاب) - 1;
	    أرجع ((abits_ptr_[الفاظ]|bbits_ptr_[الفاظ])&نقاب) == ((that.abits_ptr_[الفاظ]|that.bbits_ptr_[الفاظ])&نقاب)
		  && (bbits_ptr_[الفاظ]&نقاب) == (that.bbits_ptr_[الفاظ]&نقاب);
      }

      أرجع سچ;
}

منطقي vvp_vector4_t::has_xz() ثابت
{
      إذا (ناپ_2 < بٹ_فی_لفظ) {
	    بنانشانی لما نقاب = -1UL >> (بٹ_فی_لفظ - ناپ_2);
	    أرجع bbits_val_&نقاب;
      }

      إذا (ناپ_2 == بٹ_فی_لفظ) {
	    أرجع bbits_val_;
      }

      بنانشانی الفاظ = ناپ_2 / بٹ_فی_لفظ;
      لكل (بنانشانی idx = 0 ; idx < الفاظ ; idx += 1) {
	    إذا (bbits_ptr_[idx])
		  أرجع سچ;
      }

      بنانشانی لما نقاب = ناپ_2%بٹ_فی_لفظ;
      إذا (نقاب > 0) {
	    نقاب = -1UL >> (بٹ_فی_لفظ - نقاب);
	    أرجع bbits_ptr_[الفاظ]&نقاب;
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

      إذا (ناپ_2 <= بٹ_فی_لفظ) {
	    abits_val_ |= bbits_val_;
      } وإلا {
	    بنانشانی الفاظ = (ناپ_2+بٹ_فی_لفظ-1) / بٹ_فی_لفظ;
	    لكل (بنانشانی idx = 0 ;  idx < الفاظ ;  idx += 1)
		  abits_ptr_[idx] |= bbits_ptr_[idx];
      }
}

فراغ vvp_vector4_t::fill_بٹ(vvp_bit4_t ثنائی_ہندسہ)
{
	/* note: this relies on the bit encoding for the vvp_bit4_t. */
      ساكن ثابت بنانشانی لما init_atable[4] = {
	    لفظ_0_ABITS,
	    لفظ_1_ABITS,
	    لفظ_Z_ABITS,
	    لفظ_X_ABITS };
      ساكن ثابت بنانشانی لما init_btable[4] = {
	    لفظ_0_BBITS,
	    لفظ_1_BBITS,
	    لفظ_Z_BBITS,
	    لفظ_X_BBITS };

      إذا (ناپ_2 <= بٹ_فی_لفظ) {
	    abits_val_ = init_atable[ثنائی_ہندسہ];
	    bbits_val_ = init_btable[ثنائی_ہندسہ];
      } وإلا {
	    بنانشانی الفاظ = (ناپ_2 + بٹ_فی_لفظ - 1) / بٹ_فی_لفظ;
	    لكل (بنانشانی idx = 0; idx < الفاظ; idx += 1) {
		  abits_ptr_[idx] = init_atable[ثنائی_ہندسہ];
		  bbits_ptr_[idx] = init_btable[ثنائی_ہندسہ];
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

      لكل (بنانشانی idx = 0 ;  idx < size() && buf_len >= 2 ;  idx += 1) {
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
      إذا (ناپ_2 <= بٹ_فی_لفظ) {
	    بنانشانی لما نقاب = (ناپ_2<بٹ_فی_لفظ)? (1UL<<ناپ_2)-1UL : -1UL;
	    abits_val_ = نقاب & ~abits_val_;
	    abits_val_ |= bbits_val_;
      } وإلا {
	    بنانشانی بچیا = ناپ_2;
	    بنانشانی idx = 0;
	    طالما (بچیا >= بٹ_فی_لفظ) {
		  abits_ptr_[idx] = ~abits_ptr_[idx];
		  abits_ptr_[idx] |= bbits_ptr_[idx];
		  idx += 1;
		  بچیا -= بٹ_فی_لفظ;
	    }
	    إذا (بچیا > 0) {
		  بنانشانی لما نقاب = (1UL<<بچیا) - 1UL;
		  abits_ptr_[idx] = نقاب & ~abits_ptr_[idx];
		  abits_ptr_[idx] |= bbits_ptr_[idx];
	    }
      }
}

#تعریف_کرو ثنائی_ہندسہ_نقاب(n) ((n) ? ((~0UL) >> (بٹ_فی_لفظ - (n))) : ~0UL)

vvp_bit4_t vvp_vector4_t::reduce_or() ثابت
{
      بنانشانی لما نقاب = ثنائی_ہندسہ_نقاب(ناپ_2 % بٹ_فی_لفظ);
      vvp_bit4_t res = BIT4_0;

      إذا (ناپ_2 <= بٹ_فی_لفظ) {
	    إذا ((abits_val_ & ~bbits_val_ & نقاب) != 0UL)
		  أرجع BIT4_1;
	    إذا ((bbits_val_ & نقاب) != 0UL)
		  أرجع BIT4_X;
      } وإلا {
	    بنانشانی الفاظ = (ناپ_2 + بٹ_فی_لفظ - 1) / بٹ_فی_لفظ;
	    بنانشانی idx;
	    لكل (idx = 0; idx < الفاظ - 1; idx += 1) {
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
      بنانشانی لما نقاب = ثنائی_ہندسہ_نقاب(ناپ_2 % بٹ_فی_لفظ);
      vvp_bit4_t res = BIT4_1;

      إذا (ناپ_2 <= بٹ_فی_لفظ) {
	    إذا ((abits_val_ | bbits_val_ | ~نقاب) != ~0UL)
		  أرجع BIT4_0;
	    إذا ((bbits_val_ & نقاب) != 0UL)
		  أرجع BIT4_X;
	} وإلا {
	    بنانشانی الفاظ = (ناپ_2 + بٹ_فی_لفظ - 1) / بٹ_فی_لفظ;
	    بنانشانی idx;
	    لكل (idx = 0; idx < الفاظ - 1; idx += 1) {
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

ساكن بنانشانی لما parity(بنانشانی لما val)
{
#إذا defined(__GNUC__)
      // The compiler builtin can use target-specific CPU instructions.
      أرجع __builtin_parityl(val);
#وإلا
#إذا ULONG_MAX > 0xffffffffUL
	val ^= val >> 32;
#جے_انت
	val ^= val >> 16;
	val ^= val >> 8;
	val ^= val >> 4;

      أرجع (0x6996 >> (val & 0xf)) & 1;
#جے_انت
}

vvp_bit4_t vvp_vector4_t::reduce_xor() ثابت
{
      بنانشانی لما نقاب = ثنائی_ہندسہ_نقاب(ناپ_2 % بٹ_فی_لفظ);

      إذا (ناپ_2 <= بٹ_فی_لفظ) {
	    إذا ((bbits_val_ & نقاب) != 0UL)
		  أرجع BIT4_X;
	    أرجع parity(abits_val_ & نقاب) ? BIT4_1 : BIT4_0;
      } وإلا {
	    بنانشانی الفاظ = (ناپ_2 + بٹ_فی_لفظ - 1) / بٹ_فی_لفظ;
	    بنانشانی لما val_a = 0UL;
	    بنانشانی idx;
	    لكل (idx = 0; idx < الفاظ - 1; idx += 1) {
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
      إذا (ناپ_2 <= بٹ_فی_لفظ) {
	    بنانشانی لما tmp1 = abits_val_ | bbits_val_;
	    بنانشانی لما tmp2 = that.abits_val_ | that.bbits_val_;
	    abits_val_ = tmp1 & tmp2;
	    bbits_val_ = (tmp1 & that.bbits_val_) | (tmp2 & bbits_val_);
      } وإلا {
	    بنانشانی الفاظ = (ناپ_2 + بٹ_فی_لفظ - 1) / بٹ_فی_لفظ;
	    لكل (بنانشانی idx = 0; idx < الفاظ ; idx += 1) {
		  بنانشانی لما tmp1 = abits_ptr_[idx] | bbits_ptr_[idx];
		  بنانشانی لما tmp2 = that.abits_ptr_[idx] |
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
      إذا (ناپ_2 <= بٹ_فی_لفظ) {
	    بنانشانی لما tmp = abits_val_ | bbits_val_ |
	                        that.abits_val_ | that.bbits_val_;
	    bbits_val_ = ((~abits_val_ | bbits_val_) & that.bbits_val_) |
	                 ((~that.abits_val_ | that.bbits_val_) & bbits_val_);
	    abits_val_ = tmp;

      } وإلا {
	    بنانشانی الفاظ = (ناپ_2 + بٹ_فی_لفظ - 1) / بٹ_فی_لفظ;
	    لكل (بنانشانی idx = 0; idx < الفاظ ; idx += 1) {
		  بنانشانی لما tmp = abits_ptr_[idx] | bbits_ptr_[idx] |
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
      إذا (ناپ_2 <= بٹ_فی_لفظ) {
	    بنانشانی لما bval = bbits_val_ | that.bbits_val_;
	    bbits_val_ = bval;
	    abits_val_ = (abits_val_ ^ that.abits_val_) | bval;
      } وإلا {
	    بنانشانی الفاظ = (ناپ_2 + بٹ_فی_لفظ - 1) / بٹ_فی_لفظ;
	    لكل (بنانشانی idx = 0; idx < الفاظ ; idx += 1) {
		  بنانشانی لما bval = bbits_ptr_[idx] | that.bbits_ptr_[idx];
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
      بنانشانی idx;

      إذا (has_xz()) {
	    vvp_vector4_t xxx (size(), BIT4_X);
	    *هذا = xxx;
	    أرجع *هذا;
      }

      لكل (idx = 0 ; idx < size() ; idx += 1) {
	    إذا (that == 0 && حاصل==BIT4_0)
		  اكسر;

	    vvp_bit4_t that_ثنائی_ہندسہ = (that&1)? BIT4_1 : BIT4_0;
	    that >>= 1;

	    إذا (that_ثنائی_ہندسہ==BIT4_0 && حاصل==BIT4_0)
		  استمر;

	    vvp_bit4_t ثنائی_ہندسہ = value(idx);
	    ثنائی_ہندسہ = جوڑو_with_حاصل(ثنائی_ہندسہ, that_ثنائی_ہندسہ, حاصل);

	    سیٹ_ثنائی_ہندسہ(idx, ثنائی_ہندسہ);
      }

      أرجع *هذا;
}

ostream& عامل<< (ostream&out, ثابت vvp_vector4_t&that)
{
      out << that.size() << "'b";
      لكل (بنانشانی idx = 0 ;  idx < that.size() ;  idx += 1)
	    out << that.value(that.size()-idx-1);
      أرجع out;
}

سانچا <صنف INT>منطقي vector4_منزل_قدر(ثابت vvp_vector4_t&vec, INT&val,
					  منطقي is_signed, منطقي is_arithmetic)
{
      INT res = 0;
      INT msk = 1;
      منطقي rc_flag = سچ;

      بنانشانی size = vec.size();
      إذا (size > 8*حجم(val)) size = 8*حجم(val);
      لكل (بنانشانی idx = 0 ;  idx < size ;  idx += 1) {
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

سانچا منطقي vector4_منزل_قدر(ثابت vvp_vector4_t&vec, int8_t&val,
			       منطقي is_signed, منطقي is_arithmetic);
سانچا منطقي vector4_منزل_قدر(ثابت vvp_vector4_t&vec, int16_t&val,
			       منطقي is_signed, منطقي is_arithmetic);
سانچا منطقي vector4_منزل_قدر(ثابت vvp_vector4_t&vec, int32_t&val,
			       منطقي is_signed, منطقي is_arithmetic);
سانچا منطقي vector4_منزل_قدر(ثابت vvp_vector4_t&vec, int64_t&val,
			       منطقي is_signed, منطقي is_arithmetic);
سانچا منطقي vector4_منزل_قدر(ثابت vvp_vector4_t&vec, uint8_t&val,
			       منطقي is_signed, منطقي is_arithmetic);
سانچا منطقي vector4_منزل_قدر(ثابت vvp_vector4_t&vec, uint16_t&val,
			       منطقي is_signed, منطقي is_arithmetic);
سانچا منطقي vector4_منزل_قدر(ثابت vvp_vector4_t&vec, uint32_t&val,
			       منطقي is_signed, منطقي is_arithmetic);
سانچا منطقي vector4_منزل_قدر(ثابت vvp_vector4_t&vec, uint64_t&val,
			       منطقي is_signed, منطقي is_arithmetic);

سانچا <صنف T> منطقي vector4_منزل_قدر(ثابت vvp_vector4_t&vec,
                                         منطقي&حد_توں_ودھ_flag, T&val)
{
      T res = 0;
      T msk = 1;

      حد_توں_ودھ_flag = خطأ;
      بنانشانی size = vec.size();
      لكل (بنانشانی idx = 0 ;  idx < size ;  idx += 1) {
	    بدل (vec.value(idx)) {
		حالة BIT4_0:
		  اكسر;
		حالة BIT4_1:
		  إذا (msk == 0)
			حد_توں_ودھ_flag = سچ;
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

سانچا منطقي vector4_منزل_قدر(ثابت vvp_vector4_t&vec, منطقي&حد_توں_ودھ_flag,
                               بنانشانی لما&val);
#جے_بنا_تعریف UL_AND_TIME64_SAME
سانچا منطقي vector4_منزل_قدر(ثابت vvp_vector4_t&vec, منطقي&حد_توں_ودھ_flag,
                               vvp_time64_t&val);
#جے_انت

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
	    لكل (بنانشانی idx = 0 ;  idx < vec.size() ;  idx += 1) {
		  vvp_bit4_t a = ~vec.value(idx);
		  vvp_bit4_t x = جوڑو_with_حاصل(a, BIT4_0, حاصل);
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
	    لكل (بنانشانی idx = 0 ;  idx < vec.size() ;  idx += 1) {
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
      بنانشانی idx;
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

vvp_vector4array_t::vvp_vector4array_t(بنانشانی چوڑائی_3, بنانشانی الفاظ_2)
: چوڑائی_2(چوڑائی_3), الفاظ_3(الفاظ_2)
{
}

vvp_vector4array_t::~vvp_vector4array_t()
{
}

فراغ vvp_vector4array_t::سیٹ_لفظ_2(v4cell*cell, ثابت vvp_vector4_t&that)
{
      أكد(that.ناپ_2 == چوڑائی_2);

      إذا (چوڑائی_2 <= vvp_vector4_t::بٹ_فی_لفظ) {
	    cell->abits_val_ = that.abits_val_;
	    cell->bbits_val_ = that.bbits_val_;
	    أرجع;
      }

      بنانشانی cnt = (چوڑائی_2 + vvp_vector4_t::بٹ_فی_لفظ-1)/vvp_vector4_t::بٹ_فی_لفظ;

      إذا (cell->abits_ptr_ == 0) {
	    cell->abits_ptr_ = جديد بنانشانی لما[2*cnt];
	    cell->bbits_ptr_ = cell->abits_ptr_ + cnt;
      }

      لكل (بنانشانی idx = 0 ; idx < cnt ; idx += 1)
	    cell->abits_ptr_[idx] = that.abits_ptr_[idx];
      لكل (بنانشانی idx = 0 ; idx < cnt ; idx += 1)
	    cell->bbits_ptr_[idx] = that.bbits_ptr_[idx];
}

vvp_vector4_t vvp_vector4array_t::get_لفظ_2(v4cell*cell) ثابت
{
      إذا (چوڑائی_2 <= vvp_vector4_t::بٹ_فی_لفظ) {
	    vvp_vector4_t res;
	    res.ناپ_2 = چوڑائی_2;
	    res.abits_val_ = cell->abits_val_;
	    res.bbits_val_ = cell->bbits_val_;
	    أرجع res;
      }

      vvp_vector4_t res (چوڑائی_2, BIT4_X);
      إذا (cell->abits_ptr_ == 0)
	    أرجع res;

      بنانشانی cnt = (چوڑائی_2 + vvp_vector4_t::بٹ_فی_لفظ-1)/vvp_vector4_t::بٹ_فی_لفظ;

      لكل (بنانشانی idx = 0 ; idx < cnt ; idx += 1)
	    res.abits_ptr_[idx] = cell->abits_ptr_[idx];
      لكل (بنانشانی idx = 0 ; idx < cnt ; idx += 1)
	    res.bbits_ptr_[idx] = cell->bbits_ptr_[idx];

      أرجع res;
}

vvp_vector4array_sa::vvp_vector4array_sa(بنانشانی چوڑائی_3, بنانشانی الفاظ_2)
: vvp_vector4array_t(چوڑائی_3, الفاظ_2)
{
      array_ = جديد v4cell[الفاظ_3];

      إذا (چوڑائی_2 <= vvp_vector4_t::بٹ_فی_لفظ) {
	    لكل (بنانشانی idx = 0 ; idx < الفاظ_3 ; idx += 1) {
		  array_[idx].abits_val_ = vvp_vector4_t::لفظ_X_ABITS;
		  array_[idx].bbits_val_ = vvp_vector4_t::لفظ_X_BBITS;
	    }
      } وإلا {
	    لكل (بنانشانی idx = 0 ; idx < الفاظ_3 ; idx += 1) {
		  array_[idx].abits_ptr_ = 0;
		  array_[idx].bbits_ptr_ = 0;
	    }
      }
}

vvp_vector4array_sa::~vvp_vector4array_sa()
{
      إذا (array_) {
	    إذا (چوڑائی_2 > vvp_vector4_t::بٹ_فی_لفظ) {
		  لكل (بنانشانی idx = 0 ; idx < الفاظ_3 ; idx += 1)
			إذا (array_[idx].abits_ptr_)
			      مٹاؤ[]array_[idx].abits_ptr_;
	    }
	    مٹاؤ[]array_;
      }
}

فراغ vvp_vector4array_sa::سیٹ_لفظ(بنانشانی اشاریہ, ثابت vvp_vector4_t&that)
{
      أكد(اشاریہ < الفاظ_3);

      v4cell*cell = &array_[اشاریہ];

      سیٹ_لفظ_2(cell, that);
}

vvp_vector4_t vvp_vector4array_sa::get_لفظ(بنانشانی اشاریہ) ثابت
{
      إذا (اشاریہ >= الفاظ_3)
	    أرجع vvp_vector4_t(چوڑائی_2, BIT4_X);

      أكد(اشاریہ < الفاظ_3);

      v4cell*cell = &array_[اشاریہ];

      أرجع get_لفظ_2(cell);
}

vvp_vector4array_aa::vvp_vector4array_aa(بنانشانی چوڑائی_3, بنانشانی الفاظ_2)
: vvp_vector4array_t(چوڑائی_3, الفاظ_2)
{
      context_idx_ = vpip_جوڑو_شے_منزل_context(هذا, vpip_peek_context_scope());
}

vvp_vector4array_aa::~vvp_vector4array_aa()
{
}

فراغ vvp_vector4array_aa::alloc_instance(vvp_context_t context)
{
      v4cell*array = جديد v4cell[الفاظ_3];

      إذا (چوڑائی_2 <= vvp_vector4_t::بٹ_فی_لفظ) {
	    لكل (بنانشانی idx = 0 ; idx < الفاظ_3 ; idx += 1) {
		  array[idx].abits_val_ = vvp_vector4_t::لفظ_X_ABITS;
		  array[idx].bbits_val_ = vvp_vector4_t::لفظ_X_BBITS;
	    }
      } وإلا {
	    لكل (بنانشانی idx = 0 ; idx < الفاظ_3 ; idx += 1) {
		  array[idx].abits_ptr_ = 0;
		  array[idx].bbits_ptr_ = 0;
	    }
      }

      vvp_سیٹ_context_شے(context, context_idx_, array);
}

فراغ vvp_vector4array_aa::reset_instance(vvp_context_t context)
{
      v4cell*cell = static_cast<v4cell*>
            (vvp_get_context_شے(context, context_idx_));

      إذا (چوڑائی_2 <= vvp_vector4_t::بٹ_فی_لفظ) {
	    لكل (بنانشانی idx = 0 ; idx < الفاظ_3 ; idx += 1) {
		  cell->abits_val_ = vvp_vector4_t::لفظ_X_ABITS;
		  cell->bbits_val_ = vvp_vector4_t::لفظ_X_BBITS;
                  cell++;
	    }
      } وإلا {
            بنانشانی cnt = (چوڑائی_2 + vvp_vector4_t::بٹ_فی_لفظ-1)/vvp_vector4_t::بٹ_فی_لفظ;
	    لكل (بنانشانی idx = 0 ; idx < الفاظ_3 ; idx += 1) {
		  إذا (cell->abits_ptr_) {
                        لكل (بنانشانی n = 0 ; n < cnt ; n += 1) {
			      cell->abits_ptr_[n] = vvp_vector4_t::لفظ_X_ABITS;
			      cell->bbits_ptr_[n] = vvp_vector4_t::لفظ_X_BBITS;
                        }
                  }
                  cell++;
	    }
      }
}

#جے_تعریف_شدہ CHECK_WITH_VALGRIND
فراغ vvp_vector4array_aa::free_instance(vvp_context_t context)
{
      v4cell*cell = static_cast<v4cell*>
            (vvp_get_context_شے(context, context_idx_));
      مٹاؤ [] cell;
}
#جے_انت

فراغ vvp_vector4array_aa::سیٹ_لفظ(بنانشانی اشاریہ, ثابت vvp_vector4_t&that)
{
      أكد(اشاریہ < الفاظ_3);

      v4cell*cell = static_cast<v4cell*>
            (vthread_get_wt_context_شے(context_idx_)) + اشاریہ;

      سیٹ_لفظ_2(cell, that);
}

vvp_vector4_t vvp_vector4array_aa::get_لفظ(بنانشانی اشاریہ) ثابت
{
      إذا (اشاریہ >= الفاظ_3)
	    أرجع vvp_vector4_t(چوڑائی_2, BIT4_X);

      أكد(اشاریہ < الفاظ_3);

      v4cell*cell = static_cast<v4cell*>
            (vthread_get_rd_context_شے(context_idx_)) + اشاریہ;

      أرجع get_لفظ_2(cell);
}

vvp_vector2_t::vvp_vector2_t()
{
      vec_ = 0;
      wid_ = 0;
}

vvp_vector2_t::vvp_vector2_t(uint64_t v, بنانشانی wid)
{
      wid_ = wid;
      ثابت بنانشانی بٹ_فی_لفظ_2 = 8 * حجم(vec_[0]);
      ثابت بنانشانی الفاظ = (wid_ + بٹ_فی_لفظ_2-1) / بٹ_فی_لفظ_2;

      vec_ = جديد بنانشانی لما[الفاظ];
      لكل (بنانشانی idx = 0 ; idx < الفاظ ; idx += 1) {
	    ثابت بنانشانی کھسکاؤ = idx * بٹ_فی_لفظ_2;
	    vec_[idx] = کھسکاؤ < 64 ? v >> کھسکاؤ : 0;
      }
}

vvp_vector2_t::vvp_vector2_t(vvp_vector2_t::fill_t fill, بنانشانی wid)
{
      wid_ = wid;
      ثابت بنانشانی بٹ_فی_لفظ_2 = 8 * حجم(vec_[0]);
      ثابت بنانشانی الفاظ = (wid_ + بٹ_فی_لفظ_2-1) / بٹ_فی_لفظ_2;

      vec_ = جديد بنانشانی لما[الفاظ];
      لكل (بنانشانی idx = 0 ;  idx < الفاظ ;  idx += 1)
	    vec_[idx] = fill? -1 : 0;
}

vvp_vector2_t::vvp_vector2_t(ثابت vvp_vector2_t&that, بنانشانی ادھار, بنانشانی wid)
{
      wid_ = wid;
      ثابت بنانشانی الفاظ = (wid_ + بٹ_فی_لفظ-1) / بٹ_فی_لفظ;

      vec_ = جديد بنانشانی لما[الفاظ];

      لكل (بنانشانی idx = 0 ; idx < wid ; idx += 1) {
	    صحيح ثنائی_ہندسہ = that.value(ادھار+idx);
	    إذا (ثنائی_ہندسہ == 0)
		  استمر;
	    بنانشانی لفظ = idx / بٹ_فی_لفظ;
	    بنانشانی لما نقاب = 1UL << (idx % بٹ_فی_لفظ);
	    vec_[لفظ] |= نقاب;
      }
}

فراغ vvp_vector2_t::copy_from_that_(ثابت vvp_vector4_t&that)
{
      wid_ = that.size();
      ثابت بنانشانی الفاظ = (that.size() + بٹ_فی_لفظ-1) / بٹ_فی_لفظ;

      إذا (الفاظ == 0) {
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
      ثابت بنانشانی الفاظ = (wid_ + بٹ_فی_لفظ-1) / بٹ_فی_لفظ;

      إذا (الفاظ == 0) {
	    vec_ = 0;
	    wid_ = 0;
	    أرجع;
      }

      vec_ = جديد بنانشانی لما[الفاظ];
      لكل (بنانشانی idx = 0 ;  idx < الفاظ ;  idx += 1)
	    vec_[idx] = that.vec_[idx];
}

vvp_vector2_t::vvp_vector2_t(ثابت vvp_vector2_t&that, بنانشانی newsize)
{
      wid_ = newsize;
      إذا (newsize == 0) {
	    vec_ = 0;
	    أرجع;
      }

      ثابت بنانشانی الفاظ = (wid_ + بٹ_فی_لفظ-1) / بٹ_فی_لفظ;
      ثابت بنانشانی twords = (that.wid_ + بٹ_فی_لفظ-1) / بٹ_فی_لفظ;

      vec_ = جديد بنانشانی لما[الفاظ];
      لكل (بنانشانی idx = 0 ;  idx < الفاظ ;  idx += 1) {
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

      مٹاؤ[] vec_;
      vec_ = 0;

      copy_from_that_(that);
      أرجع *هذا;
}

vvp_vector2_t& vvp_vector2_t::عامل= (ثابت vvp_vector4_t&that)
{
      مٹاؤ[]vec_;
      vec_ = 0;
      copy_from_that_(that);
      أرجع *هذا;
}

vvp_vector2_t& vvp_vector2_t::عامل <<= (بنانشانی صحيح کھسکاؤ)
{
      إذا (wid_ == 0)
	    أرجع *هذا;

      ثابت بنانشانی الفاظ = (wid_ + بٹ_فی_لفظ-1) / بٹ_فی_لفظ;

	// Number of words to shift
      ثابت بنانشانی wshift = کھسکاؤ / بٹ_فی_لفظ;
	// bits to shift within each word.
      ثابت بنانشانی لما oshift = کھسکاؤ % بٹ_فی_لفظ;

	// If shifting the entire value away, then return zeros.
      إذا (wshift >= الفاظ) {
	    لكل (بنانشانی idx = 0 ;  idx < الفاظ ;  idx += 1)
		  vec_[idx] = 0;

	    أرجع *هذا;
      }

	// Do the word shift first.
      إذا (wshift > 0) {
	    لكل (بنانشانی idx = 0 ;  idx < الفاظ-wshift ;  idx += 1) {
		  بنانشانی sel = الفاظ - idx - 1;
		  vec_[sel] = vec_[sel-wshift];
	    }

	    لكل (بنانشانی idx = 0 ;  idx < wshift ;  idx += 1)
		  vec_[idx] = 0;
      }

	// Do the fine shift.
      إذا (oshift != 0) {
	    بنانشانی لما pad = 0;
	    لكل (بنانشانی idx = 0 ;  idx < الفاظ ;  idx += 1) {
		  بنانشانی لما اگلا_pad = vec_[idx] >> (بٹ_فی_لفظ-oshift);
		  vec_[idx] = (vec_[idx] << oshift) | pad;
		  pad = اگلا_pad;
	    }

	      // Cleanup the tail bits.
	    بنانشانی لما نقاب = -1UL >> (بٹ_فی_لفظ - wid_%بٹ_فی_لفظ);
	    vec_[الفاظ-1] &= نقاب;
      }

      أرجع *هذا;
}

vvp_vector2_t& vvp_vector2_t::عامل >>= (بنانشانی کھسکاؤ)
{
      إذا (wid_ == 0)
	    أرجع *هذا;

      ثابت بنانشانی الفاظ = (wid_ + بٹ_فی_لفظ-1) / بٹ_فی_لفظ;

	// Number of words to shift
      ثابت بنانشانی wshift = کھسکاؤ / بٹ_فی_لفظ;
	// bits to shift within each word.
      ثابت بنانشانی لما oshift = کھسکاؤ % بٹ_فی_لفظ;

	// If shifting the entire value away, then return zeros.
      إذا (wshift >= الفاظ) {
	    لكل (بنانشانی idx = 0 ;  idx < الفاظ ;  idx += 1)
		  vec_[idx] = 0;

	    أرجع *هذا;
      }

      إذا (wshift > 0) {
	    لكل (بنانشانی idx = 0 ;  idx < الفاظ-wshift ;  idx += 1)
		  vec_[idx] = vec_[idx+wshift];

	    لكل (بنانشانی idx = الفاظ-wshift ;  idx < الفاظ ;  idx += 1)
		  vec_[idx] = 0;
      }

      إذا (oshift > 0) {
	    بنانشانی لما pad = 0;
	    لكل (بنانشانی idx = الفاظ ;  idx > 0 ;  idx -= 1) {
		  بنانشانی لما new_pad = vec_[idx-1] <<(بٹ_فی_لفظ-oshift);
		  vec_[idx-1] = pad | (vec_[idx-1] >> oshift);
		  pad = new_pad;
	    }

	      // Cleanup the tail bits.

	    بنانشانی use_الفاظ = الفاظ;
	      // Mask_shift is the number of high bits of the top word
	      // that are to be masked off. We start with the number
	      // of bits that are not included even in the original,
	      // then we include the bits of the shift, that are to be
	      // masked to zero.
	    بنانشانی لما نقاب_کھسکاؤ = بٹ_فی_لفظ - wid_%بٹ_فی_لفظ;
	    نقاب_کھسکاؤ %= بٹ_فی_لفظ;
	    نقاب_کھسکاؤ += oshift;
	    طالما (نقاب_کھسکاؤ >= بٹ_فی_لفظ) {
		  vec_[use_الفاظ-1] = 0;
		  use_الفاظ -= 1;
		  نقاب_کھسکاؤ -= بٹ_فی_لفظ;
	    }
	    إذا (نقاب_کھسکاؤ > 0) {
		  أكد(use_الفاظ > 0);
		  بنانشانی لما نقاب = -1UL >> نقاب_کھسکاؤ;
		  vec_[use_الفاظ-1] &= نقاب;
	    }
      }

      أرجع *هذا;
}

ساكن بنانشانی لما جوڑو_حاصل(بنانشانی لما a, بنانشانی لما b,
			       بنانشانی لما&حاصل)
{
      بنانشانی لما out = حاصل;
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

      ثابت بنانشانی الفاظ = (wid_ + بٹ_فی_لفظ-1) / بٹ_فی_لفظ;

      بنانشانی لما حاصل = 0;
      لكل (بنانشانی idx = 0 ;  idx < الفاظ ;  idx += 1) {
	    vec_[idx] = جوڑو_حاصل(vec_[idx], that.vec_[idx], حاصل);
      }


	// Cleanup the tail bits.
      بنانشانی لما نقاب = -1UL >> (بٹ_فی_لفظ - wid_%بٹ_فی_لفظ);
      vec_[الفاظ-1] &= نقاب;

      أرجع *هذا;
}

vvp_vector2_t& vvp_vector2_t::عامل -= (ثابت vvp_vector2_t&that)
{
      أكد(wid_ == that.wid_);
      إذا (wid_ == 0)
	    أرجع *هذا;

      ثابت بنانشانی الفاظ = (wid_ + بٹ_فی_لفظ-1) / بٹ_فی_لفظ;

      بنانشانی لما حاصل = 1;
      لكل (بنانشانی idx = 0 ;  idx < الفاظ ;  idx += 1) {
	    vec_[idx] = جوڑو_حاصل(vec_[idx], ~that.vec_[idx], حاصل);
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

صحيح vvp_vector2_t::value(بنانشانی idx) ثابت
{
      إذا (idx >= wid_)
	    أرجع 0;

      ثابت بنانشانی بٹ_فی_لفظ_2 = 8 * حجم(vec_[0]);
      بنانشانی addr = idx/بٹ_فی_لفظ_2;
      بنانشانی نقاب = idx%بٹ_فی_لفظ_2;

      إذا (vec_[addr] & (1UL<<نقاب))
	    أرجع 1;
      وإلا
	    أرجع 0;
}

فراغ vvp_vector2_t::سیٹ_ثنائی_ہندسہ(بنانشانی idx, صحيح ثنائی_ہندسہ)
{
      أكد(idx < wid_);

      ثابت بنانشانی بٹ_فی_لفظ_2 = 8 * حجم(vec_[0]);
      بنانشانی addr = idx/بٹ_فی_لفظ_2;
      بنانشانی لما نقاب = idx%بٹ_فی_لفظ_2;

      إذا (ثنائی_ہندسہ)
	    vec_[addr] |= 1UL << نقاب;
      وإلا
	    vec_[addr] &= ~(1UL << نقاب);
}

فراغ vvp_vector2_t::سیٹ_vec(بنانشانی adr, ثابت vvp_vector2_t&that)
{
      أكد((adr + that.wid_) <= wid_);

      لكل (بنانشانی idx = 0 ; idx < that.wid_ ; idx += 1)
	    سیٹ_ثنائی_ہندسہ(adr+idx, that.value(idx));
}

منطقي vvp_vector2_t::is_NaN() ثابت
{
      أرجع wid_ == 0;
}

منطقي vvp_vector2_t::is_zero() ثابت
{
      ثابت بنانشانی الفاظ = (wid_ + بٹ_فی_لفظ-1) / بٹ_فی_لفظ;

      لكل (بنانشانی idx = 0; idx < الفاظ; idx += 1) {
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
	    y.سیٹ_ثنائی_ہندسہ(0, 0);  // A quick subtract by 1.
	    vvp_vector2_t res = x * قوت(x, y);
	    أرجع res;
      }

      y >>= 1;  // A fast divide by two. We know the LSB is zero.
      vvp_vector2_t z = قوت(x, y);
      vvp_vector2_t res = z * z;
      أرجع res;
}

ساكن فراغ multiply_long(بنانشانی لما a, بنانشانی لما b,
			  بنانشانی لما&کم, بنانشانی لما&بلند)
{
      أكد(حجم(بنانشانی لما) %2 == 0);

      ثابت بنانشانی لما لفظ_نقاب = (1UL << 4UL*حجم(a)) - 1UL;
      بنانشانی لما tmpa;
      بنانشانی لما tmpb;
      بنانشانی لما res[4];

      tmpa = a & لفظ_نقاب;
      tmpb = b & لفظ_نقاب;
      res[0] = tmpa * tmpb;
      res[1] = res[0] >> 4UL*حجم(بنانشانی لما);
      res[0] &= لفظ_نقاب;

      tmpa = (a >> 4UL*حجم(بنانشانی لما)) & لفظ_نقاب;
      res[1] += tmpa * tmpb;
      res[2] = res[1] >> 4UL*حجم(بنانشانی لما);
      res[1] &= لفظ_نقاب;

      tmpa = a & لفظ_نقاب;
      tmpb = (b >> 4UL*حجم(بنانشانی لما)) & لفظ_نقاب;
      res[1] += tmpa * tmpb;
      res[2] += res[1] >> 4UL*حجم(بنانشانی لما);
      res[3]  = res[2] >> 4UL*حجم(بنانشانی لما);
      res[1] &= لفظ_نقاب;
      res[2] &= لفظ_نقاب;

      tmpa = (a >> 4UL*حجم(بنانشانی لما)) & لفظ_نقاب;
      res[2] += tmpa * tmpb;
      res[3] += res[2] >> 4UL*حجم(بنانشانی لما);
      res[2] &= لفظ_نقاب;

      بلند = (res[3] << 4UL*حجم(بنانشانی لما)) | res[2];
      کم  = (res[1] << 4UL*حجم(بنانشانی لما)) | res[0];
}

vvp_vector2_t عامل * (ثابت vvp_vector2_t&a, ثابت vvp_vector2_t&b)
{
      ثابت بنانشانی بٹ_فی_لفظ_2 = 8 * حجم(a.vec_[0]);

	// The compiler ensures that the two operands are of equal size.
      أكد(a.size() == b.size());
      vvp_vector2_t r (0, a.size());

      بنانشانی الفاظ = (r.wid_ + بٹ_فی_لفظ_2 - 1) / بٹ_فی_لفظ_2;

      لكل (بنانشانی bdx = 0 ;  bdx < الفاظ ;  bdx += 1) {
	    بنانشانی لما tmpb = b.vec_[bdx];
	    إذا (tmpb == 0)
		  استمر;

	    لكل (بنانشانی adx = 0 ;  adx < الفاظ ;  adx += 1) {
		  بنانشانی لما tmpa = a.vec_[adx];
		  إذا (tmpa == 0)
			استمر;

		  بنانشانی لما کم, hig;
		  multiply_long(tmpa, tmpb, کم, hig);

		  بنانشانی لما حاصل = 0;
		  لكل (بنانشانی sdx = 0
			     ; (adx+bdx+sdx) < الفاظ
			     ;  sdx += 1) {

			r.vec_[adx+bdx+sdx] = جوڑو_حاصل(r.vec_[adx+bdx+sdx],
							کم, حاصل);
			کم = hig;
			hig = 0;
		  }
	    }
      }

      أرجع r;
}

ساكن فراغ div_mod (vvp_vector2_t dividend, vvp_vector2_t مقسوم_علیہ,
		     vvp_vector2_t&quotient, vvp_vector2_t&remainder)
{

      quotient = vvp_vector2_t(0, dividend.size());

      إذا (مقسوم_علیہ == quotient) {
	    cerr << "ERROR: division by zero, exiting." << سطر_انت;
	    باہر_نکلو(255);
      }

      إذا (dividend < مقسوم_علیہ) {
	    remainder = dividend;
	    أرجع;
      }

      vvp_vector2_t نقاب (1, dividend.size());

	// Make the dividend 1 bit larger to prevent overflow of
	// divtmp in startup.
      dividend = vvp_vector2_t(dividend, dividend.size()+1);
      vvp_vector2_t divtmp (مقسوم_علیہ, dividend.size());

      طالما (divtmp < dividend) {
	    divtmp <<= 1;
	    نقاب <<= 1;
      }

      طالما (dividend >= مقسوم_علیہ) {
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

      ثابت بنانشانی الفاظ = (neg.wid_ + neg.بٹ_فی_لفظ-1) /
                                         neg.بٹ_فی_لفظ;
      لكل (بنانشانی idx = 0 ;  idx < الفاظ ;  idx += 1) {
	    neg.vec_[idx] = ~neg.vec_[idx];
      }
      neg += vvp_vector2_t(1, neg.wid_);

      أرجع neg;
}

vvp_vector2_t عامل / (ثابت vvp_vector2_t&dividend,
			  ثابت vvp_vector2_t&مقسوم_علیہ)
{
      vvp_vector2_t quot, rem;
      div_mod(dividend, مقسوم_علیہ, quot, rem);
      أرجع quot;
}

vvp_vector2_t عامل % (ثابت vvp_vector2_t&dividend,
			  ثابت vvp_vector2_t&مقسوم_علیہ)
{
      vvp_vector2_t quot, rem;
      div_mod(dividend, مقسوم_علیہ, quot, rem);
      أرجع rem;
}

منطقي عامل > (ثابت vvp_vector2_t&a, ثابت vvp_vector2_t&b)
{
      ثابت بنانشانی awords = (a.wid_ + vvp_vector2_t::بٹ_فی_لفظ-1) / vvp_vector2_t::بٹ_فی_لفظ;
      ثابت بنانشانی bwords = (b.wid_ + vvp_vector2_t::بٹ_فی_لفظ-1) / vvp_vector2_t::بٹ_فی_لفظ;

      ثابت بنانشانی الفاظ = awords > bwords? awords : bwords;

      لكل (بنانشانی idx = الفاظ ;  idx > 0 ;  idx -= 1) {
	    بنانشانی لما aw = (idx <= awords)? a.vec_[idx-1] : 0;
	    بنانشانی لما bw = (idx <= bwords)? b.vec_[idx-1] : 0;

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
      ثابت بنانشانی awords = (a.wid_ + vvp_vector2_t::بٹ_فی_لفظ-1) / vvp_vector2_t::بٹ_فی_لفظ;
      ثابت بنانشانی bwords = (b.wid_ + vvp_vector2_t::بٹ_فی_لفظ-1) / vvp_vector2_t::بٹ_فی_لفظ;

      ثابت بنانشانی الفاظ = awords > bwords? awords : bwords;

      لكل (بنانشانی idx = الفاظ ;  idx > 0 ;  idx -= 1) {
	    بنانشانی لما aw = (idx <= awords)? a.vec_[idx-1] : 0;
	    بنانشانی لما bw = (idx <= bwords)? b.vec_[idx-1] : 0;

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
      ثابت بنانشانی awords = (a.wid_ + vvp_vector2_t::بٹ_فی_لفظ-1) / vvp_vector2_t::بٹ_فی_لفظ;
      ثابت بنانشانی bwords = (b.wid_ + vvp_vector2_t::بٹ_فی_لفظ-1) / vvp_vector2_t::بٹ_فی_لفظ;

      بنانشانی الفاظ = awords;
      إذا (bwords > الفاظ)
	    الفاظ = bwords;

      لكل (بنانشانی idx = الفاظ ;  idx > 0 ;  idx -= 1) {
	    بنانشانی لما aw = (idx <= awords)? a.vec_[idx-1] : 0;
	    بنانشانی لما bw = (idx <= bwords)? b.vec_[idx-1] : 0;

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

      ثابت بنانشانی awords = (a.wid_ + vvp_vector2_t::بٹ_فی_لفظ-1) / vvp_vector2_t::بٹ_فی_لفظ;

      لكل (بنانشانی idx = awords ;  idx > 0 ;  idx -= 1) {
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
      ثابت بنانشانی awords = (a.wid_ + vvp_vector2_t::بٹ_فی_لفظ-1) / vvp_vector2_t::بٹ_فی_لفظ;
      ثابت بنانشانی bwords = (b.wid_ + vvp_vector2_t::بٹ_فی_لفظ-1) / vvp_vector2_t::بٹ_فی_لفظ;

      ثابت بنانشانی الفاظ = awords > bwords? awords : bwords;

      لكل (بنانشانی idx = الفاظ ;  idx > 0 ;  idx -= 1) {
	    بنانشانی لما aw = (idx <= awords)? a.vec_[idx-1] : 0;
	    بنانشانی لما bw = (idx <= bwords)? b.vec_[idx-1] : 0;

	    إذا (aw > bw)
		  أرجع خطأ;
	    إذا (aw < bw)
		  أرجع خطأ;
      }

	// If the above loop finishes, then the vectors are equal.
      أرجع سچ;
}


vvp_vector4_t vector2_منزل_vector4(ثابت vvp_vector2_t&that, بنانشانی wid)
{
      vvp_vector4_t res (wid);

      لكل (بنانشانی idx = 0 ;  idx < res.size() ;  idx += 1) {
	    vvp_bit4_t ثنائی_ہندسہ = BIT4_0;

	    إذا (that.value(idx))
		  ثنائی_ہندسہ = BIT4_1;

	    res.سیٹ_ثنائی_ہندسہ(idx, ثنائی_ہندسہ);
      }

      أرجع res;
}

ساكن منطقي c4string_سرخی_test(ثابت حرف*str)
{
      إذا ((str[0] != 'C') && (str[0] != 'c'))
	    أرجع خطأ;
      إذا ((str[1] != '4') || (str[2] != '<'))
	    أرجع خطأ;
      أرجع سچ;
}

منطقي c4string_test(ثابت حرف*str)
{
      إذا (!c4string_سرخی_test(str))
	    أرجع خطأ;
      size_t قدر_ناپ = strspn(str+3, "01xz");
      إذا (str[3+قدر_ناپ] != '>')
	    أرجع خطأ;
      إذا (str[3+قدر_ناپ+1] != 0)
	    أرجع خطأ;

      أرجع سچ;
}

vvp_vector4_t c4string_منزل_vector4(ثابت حرف*str)
{
      أكد(c4string_سرخی_test(str));

      str += 3;
      ثابت حرف*tp = str + strspn(str,"01xz");
      أكد(tp[0] == '>');

      vvp_vector4_t tmp (tp-str);

      لكل (بنانشانی idx = 0 ;  idx < tmp.size() ;  idx += 1) {
	    vvp_bit4_t ثنائی_ہندسہ;
	    بدل (str[idx]) {
		حالة '0':
		  ثنائی_ہندسہ = BIT4_0;
		  اكسر;
		حالة '1':
		  ثنائی_ہندسہ = BIT4_1;
		  اكسر;
		حالة 'x':
		  ثنائی_ہندسہ = BIT4_X;
		  اكسر;
		حالة 'z':
		  ثنائی_ہندسہ = BIT4_Z;
		  اكسر;
		افتراضي:
		  fprintf(stderr, "Unsupported bit value %c(%d).\n", str[idx],
		          str[idx]);
		  أكد(0);
		  ثنائی_ہندسہ = BIT4_0;
		  اكسر;
	    }
	    tmp.سیٹ_ثنائی_ہندسہ(tmp.size()-idx-1, ثنائی_ہندسہ);
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
      ناپ_2 = that.ناپ_2;
      إذا (ناپ_2 <= حجم(val_)) {
	    حافظہ_نقل(val_, that.val_, حجم val_);
      } وإلا {
	    ptr_ = جديد بنانشانی حرف[ناپ_2];
	    حافظہ_نقل(ptr_, that.ptr_, ناپ_2);
      }
}

vvp_vector8_t::vvp_vector8_t(ثابت vvp_vector4_t&that,
			     بنانشانی str0, بنانشانی str1)
: ناپ_2(that.size())
{
      إذا (ناپ_2 == 0)
	    أرجع;

      إذا (ناپ_2 <= حجم(val_)) {
	    ptr_ = 0; // Prefill all val_ bytes
	    لكل (بنانشانی idx = 0 ; idx < ناپ_2 ; idx += 1)
		  val_[idx] = vvp_scalar_t(that.value(idx),str0, str1).raw();
      } وإلا {
	    ptr_ = جديد بنانشانی حرف[ناپ_2];
	    لكل (بنانشانی idx = 0 ;  idx < ناپ_2 ;  idx += 1)
		  ptr_[idx] = vvp_scalar_t(that.value(idx), str0, str1).raw();
      }
}

vvp_vector8_t::vvp_vector8_t(ثابت vvp_vector2_t&that,
			     بنانشانی str0, بنانشانی str1)
: ناپ_2(that.size())
{
      إذا (ناپ_2 == 0)
	    أرجع;

      إذا (ناپ_2 <= حجم(val_)) {
	    ptr_ = 0;
	    لكل (بنانشانی idx = 0 ; idx < ناپ_2 ; idx += 1)
		  val_[idx] = vvp_scalar_t(that.value(idx)? BIT4_1:BIT4_0, str0, str1).raw();
      } وإلا {
	    ptr_ = جديد بنانشانی حرف[ناپ_2];
	    لكل (بنانشانی idx = 0 ; idx < ناپ_2 ; idx += 1)
		  ptr_[idx] = vvp_scalar_t(that.value(idx)? BIT4_1:BIT4_0, str0, str1).raw();
      }
}

ثابت vvp_vector8_t vvp_vector8_t::nil;

vvp_vector8_t& vvp_vector8_t::عامل= (ثابت vvp_vector8_t&that)
{
	// Assign to self.
      إذا (هذا == &that)
	    أرجع *هذا;

      إذا (ناپ_2 != that.ناپ_2) {
	    إذا (ناپ_2 > حجم(val_))
		  مٹاؤ[]ptr_;
	    ناپ_2 = 0;
      }

      إذا (that.ناپ_2 == 0) {
	    أكد(ناپ_2 == 0);
	    أرجع *هذا;
      }

      إذا (that.ناپ_2 <= حجم(val_)) {
	    ناپ_2 = that.ناپ_2;
	    حافظہ_نقل(val_, that.val_, حجم(val_));
	    أرجع *هذا;
      }

      إذا (ناپ_2 == 0) {
	    ناپ_2 = that.ناپ_2;
	    ptr_ = جديد بنانشانی حرف[ناپ_2];
      }

      حافظہ_نقل(ptr_, that.ptr_, ناپ_2);

      أرجع *هذا;
}

vvp_vector8_t vvp_vector8_t::subvalue(بنانشانی ادھار, بنانشانی wid) ثابت
{
      vvp_vector8_t tmp (wid);

      بنانشانی حرف*tmp_ptr = tmp.ناپ_2 <= حجم(val_) ? tmp.val_ : tmp.ptr_;
      ثابت بنانشانی حرف*use_ptr = ناپ_2 <= حجم(val_) ? val_ : ptr_;

      بنانشانی idx = 0;
      طالما ((idx < wid) && (ادھار+idx < ناپ_2)) {
	    tmp_ptr[idx] = use_ptr[ادھار+idx];
	    idx += 1;
      }

      أرجع tmp;
}

فراغ vvp_vector8_t::سیٹ_vec(بنانشانی ادھار, ثابت vvp_vector8_t&that)
{
      أكد((ادھار+that.size()) <= size());
      لكل (بنانشانی idx = 0 ; idx < that.size() ; idx += 1)
	    سیٹ_ثنائی_ہندسہ(ادھار+idx, that.value(idx));
}

vvp_vector8_t part_expand(ثابت vvp_vector8_t&that, بنانشانی wid, بنانشانی off)
{
      أكد(off < wid);
      vvp_vector8_t tmp (wid);

      بنانشانی حرف* tmp_ptr = tmp.ناپ_2<= حجم(tmp.val_) ?
                               tmp.val_ : tmp.ptr_;
      ثابت بنانشانی حرف* that_ptr = that.ناپ_2<= حجم(that.val_) ?
                                      that.val_ : that.ptr_;

      بنانشانی idx = off;

      طالما (idx < wid && that.ناپ_2 > (idx-off)) {
	    tmp_ptr[idx] = that_ptr[idx-off];
	    idx += 1;
      }

      أرجع tmp;
}

ساكن منطقي c8string_سرخی_test(ثابت حرف*str)
{
      إذا ((str[0] != 'C') && (str[0] != 'c'))
	    أرجع خطأ;
      إذا ((str[1] != '8') || (str[2] != '<'))
	    أرجع خطأ;
      أرجع سچ;
}

منطقي c8string_test(ثابت حرف*str)
{
      إذا (!c8string_سرخی_test(str))
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
      أكد(c8string_سرخی_test(str));

      size_t vsize = لڑی_لمبائی(str)-4;
      أكد(vsize%3 == 0);
      vsize /= 3;
      vvp_vector8_t tmp (vsize);

      لكل (size_t idx = 0 ; idx < vsize ; idx += 1) {
	    ثابت حرف*cp = str+3+3*idx;
	    vvp_bit4_t ثنائی_ہندسہ = BIT4_X;
	    بنانشانی dr0 = cp[0]-'0';
	    بنانشانی dr1 = cp[1]-'0';
	    بدل (cp[2]) {
		حالة '0':
		  ثنائی_ہندسہ = BIT4_0;
		  اكسر;
		حالة '1':
		  ثنائی_ہندسہ = BIT4_1;
		  اكسر;
		حالة 'x':
		  ثنائی_ہندسہ = BIT4_X;
		  اكسر;
		حالة 'z':
		  ثنائی_ہندسہ = BIT4_Z;
		  اكسر;
	    }
	    tmp.سیٹ_ثنائی_ہندسہ(vsize-idx-1, vvp_scalar_t(ثنائی_ہندسہ, dr0, dr1));
      }

      أرجع tmp;
}

ostream& عامل<<(ostream&out, ثابت vvp_vector8_t&that)
{
      out << "C8<";
      لكل (بنانشانی idx = 0 ;  idx < that.size() ; idx += 1)
	    out << that.value(that.size()-idx-1);

      out << ">";
      أرجع out;
}

vvp_net_fun_t::vvp_net_fun_t()
{
      گنتی_functors += 1;
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

فراغ vvp_net_fun_t::recv_vec4_pv_(vvp_net_ptr_t p, ثابت vvp_vector4_t&ثنائی_ہندسہ,
                                  بنانشانی ادھار, بنانشانی vwid, vvp_context_t)
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

      أكد(ادھار + ثنائی_ہندسہ.size() <= vwid);

      vvp_vector4_t tmp(vwid, BIT4_Z);
      tmp.سیٹ_vec(ادھار, ثنائی_ہندسہ);
      recv_vec4(p, tmp, 0);
}

فراغ vvp_net_fun_t::recv_vec4_pv(vvp_net_ptr_t, ثابت vvp_vector4_t&ثنائی_ہندسہ,
                                 بنانشانی ادھار, بنانشانی vwid, vvp_context_t)
{
      cerr << "internal error: " << typeid(*هذا).name() << ": "
	   << "recv_vec4_pv(" << ثنائی_ہندسہ << ", " << ادھار
	   << ", " << vwid << ") not implemented" << سطر_انت;
      أكد(0);
}

فراغ vvp_net_fun_t::recv_vec8(vvp_net_ptr_t پورٹ, ثابت vvp_vector8_t&ثنائی_ہندسہ)
{
      recv_vec4(پورٹ, reduce4(ثنائی_ہندسہ), 0);
}

فراغ vvp_net_fun_t::recv_vec8_pv_(vvp_net_ptr_t p, ثابت vvp_vector8_t&ثنائی_ہندسہ,
				  بنانشانی ادھار, بنانشانی vwid)
{
	// This is the strength-aware version of recv_vec4_pv_.

      أكد(ادھار + ثنائی_ہندسہ.size() <= vwid);

      vvp_vector8_t tmp(vwid);
      tmp.سیٹ_vec(ادھار, ثنائی_ہندسہ);
      recv_vec8(p, tmp);
}

فراغ vvp_net_fun_t::recv_vec8_pv(vvp_net_ptr_t پورٹ, ثابت vvp_vector8_t&ثنائی_ہندسہ,
				 بنانشانی ادھار, بنانشانی vwid)
{
      recv_vec4_pv(پورٹ, reduce4(ثنائی_ہندسہ), ادھار, vwid, 0);
}

فراغ vvp_net_fun_t::recv_real(vvp_net_ptr_t, مزدوج ثنائی_ہندسہ, vvp_context_t)
{
      fprintf(stderr, "internal error: %s: recv_real(%f) not implemented\n",
	      typeid(*هذا).name(), ثنائی_ہندسہ);
      أكد(0);
}

فراغ vvp_net_fun_t::recv_string(vvp_net_ptr_t, ثابت std::string&ثنائی_ہندسہ, vvp_context_t)
{
      fprintf(stderr, "internal error: %s: recv_string(%s) not implemented\n",
	      typeid(*هذا).name(), ثنائی_ہندسہ.c_str());
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

vvp_fun_drive::vvp_fun_drive(بنانشانی str0, بنانشانی str1)
{
      أكد(str0 < 8);
      أكد(str1 < 8);

      drive0_ = str0;
      drive1_ = str1;
}

vvp_fun_drive::~vvp_fun_drive()
{
}

فراغ vvp_fun_drive::recv_vec4(vvp_net_ptr_t پورٹ, ثابت vvp_vector4_t&ثنائی_ہندسہ,
                              vvp_context_t)
{
      أكد(پورٹ.پورٹ() == 0);
      پورٹ.ptr()->send_vec8(vvp_vector8_t(ثنائی_ہندسہ, drive0_, drive1_));
}

فراغ vvp_fun_drive::recv_vec4_pv(vvp_net_ptr_t ptr, ثابت vvp_vector4_t&ثنائی_ہندسہ,
				 بنانشانی ادھار, بنانشانی vwid, vvp_context_t ctx)
{
      recv_vec4_pv_(ptr, ثنائی_ہندسہ, ادھار, vwid, ctx);
}

/* **** vvp_wide_fun_* methods **** */

vvp_wide_fun_core::vvp_wide_fun_core(vvp_net_t*net, بنانشانی nports)
{
      ptr_ = net;
      nports_ = nports;
      پورٹ_اقدار = 0;
      پورٹ_rvalues = 0;
}

vvp_wide_fun_core::~vvp_wide_fun_core()
{
      مٹاؤ[] پورٹ_اقدار;
      مٹاؤ[] پورٹ_rvalues;
}

فراغ vvp_wide_fun_core::propagate_vec4(ثابت vvp_vector4_t&ثنائی_ہندسہ,
				       vvp_time64_t delay)
{
      إذا (delay)
	    ویلا_سارنی_propagate_سمتیہ(ptr_, delay, ثنائی_ہندسہ);
      وإلا
	    ptr_->send_vec4(ثنائی_ہندسہ, 0);
}

فراغ vvp_wide_fun_core::propagate_real(مزدوج ثنائی_ہندسہ,
				       vvp_time64_t delay)
{
      إذا (delay) {
	      // schedule_assign_vector(ptr_->out, bit, delay);
	    أكد(0); // Need a real-value version of assign_vector.
      } وإلا {
	    ptr_->send_real(ثنائی_ہندسہ, 0);
      }
}


بنانشانی vvp_wide_fun_core::پورٹ_گنتی() ثابت
{
      أرجع nports_;
}

vvp_vector4_t& vvp_wide_fun_core::value(بنانشانی idx)
{
      أكد(idx < nports_);
      إذا (پورٹ_اقدار == 0)
	    پورٹ_اقدار = جديد vvp_vector4_t [nports_];
      أرجع پورٹ_اقدار[idx];
}

مزدوج vvp_wide_fun_core::قدر_r(بنانشانی idx)
{
      أكد(idx < nports_);
      أرجع پورٹ_rvalues? پورٹ_rvalues[idx] : 0.0;
}

فراغ vvp_wide_fun_core::recv_real_from_inputs(بنانشانی)
{
      أكد(0);
}

فراغ vvp_wide_fun_core::dispatch_vec4_from_داخلہ(بنانشانی پورٹ,
						  ثابت vvp_vector4_t&ثنائی_ہندسہ)
{
      أكد(پورٹ < nports_);
      إذا (پورٹ_اقدار == 0) پورٹ_اقدار = جديد vvp_vector4_t [nports_];
      پورٹ_اقدار[پورٹ] = ثنائی_ہندسہ;
      recv_vec4_from_inputs(پورٹ);
}

فراغ vvp_wide_fun_core::dispatch_real_from_داخلہ(بنانشانی پورٹ,
						  مزدوج ثنائی_ہندسہ)
{
      أكد(پورٹ < nports_);
      إذا (پورٹ_rvalues == 0) پورٹ_rvalues = جديد مزدوج[nports_];
      پورٹ_rvalues[پورٹ] = ثنائی_ہندسہ;
      recv_real_from_inputs(پورٹ);
}

vvp_wide_fun_t::vvp_wide_fun_t(vvp_wide_fun_core*c, بنانشانی ادھار)
: core_(c), پورٹ_ادھار_2(ادھار)
{
}

vvp_wide_fun_t::~vvp_wide_fun_t()
{
}

فراغ vvp_wide_fun_t::recv_vec4(vvp_net_ptr_t پورٹ, ثابت vvp_vector4_t&ثنائی_ہندسہ,
                               vvp_context_t)
{
      بنانشانی pidx = پورٹ_ادھار_2 + پورٹ.پورٹ();
      core_->dispatch_vec4_from_داخلہ(pidx, ثنائی_ہندسہ);
}

فراغ vvp_wide_fun_t::recv_vec4_pv(vvp_net_ptr_t ptr, ثابت vvp_vector4_t&ثنائی_ہندسہ,
				  بنانشانی ادھار, بنانشانی vwid, vvp_context_t ctx)
{
      recv_vec4_pv_(ptr, ثنائی_ہندسہ, ادھار, vwid, ctx);
}

فراغ vvp_wide_fun_t::recv_real(vvp_net_ptr_t پورٹ, مزدوج ثنائی_ہندسہ,
                               vvp_context_t)
{
      بنانشانی pidx = پورٹ_ادھار_2 + پورٹ.پورٹ();
      core_->dispatch_real_from_داخلہ(pidx, ثنائی_ہندسہ);
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

بنانشانی vvp_scalar_t::strength0() ثابت
{
      أرجع STREN0(قدر_2);
}

بنانشانی vvp_scalar_t::strength1() ثابت
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

      بنانشانی tmp = 0;
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

بنانشانی vvp_switch_strength_map[2][8] = {
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
      لكل (بنانشانی idx = 0 ;  idx < out.size() ;  idx += 1)
	    out.سیٹ_ثنائی_ہندسہ(idx, that.value(idx).value());

      أرجع out;
}

vvp_bit4_t compare_gtge(ثابت vvp_vector4_t&lef, ثابت vvp_vector4_t&rig,
			vvp_bit4_t باہرلا_if_equal)
{
      بنانشانی min_ناپ = lef.size();
      إذا (rig.size() < min_ناپ)
	    min_ناپ = rig.size();

	// If one of the inputs is nil, treat is as all X values, and
	// that makes the result BIT4_X.
      إذا (min_ناپ == 0)
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

      لكل (بنانشانی idx = lef.size() ; idx > rig.size() ;  idx -= 1) {
	    إذا (lef.value(idx-1) == BIT4_1)
		  أرجع BIT4_1;
      }

      لكل (بنانشانی idx = rig.size() ; idx > lef.size() ;  idx -= 1) {
	    إذا (rig.value(idx-1) == BIT4_1)
		  أرجع BIT4_0;
      }

      لكل (بنانشانی idx = min_ناپ ; idx > 0 ;  idx -= 1) {
	    vvp_bit4_t lv = lef.value(idx-1);
	    vvp_bit4_t rv = rig.value(idx-1);

	    إذا (lv == rv)
		  استمر;

	    إذا (lv == BIT4_1)
		  أرجع BIT4_1;
	    وإلا
		  أرجع BIT4_0;
      }

      أرجع باہرلا_if_equal;
}

vvp_bit4_t compare_gtge_signed(ثابت vvp_vector4_t&a,
			       ثابت vvp_vector4_t&b,
			       vvp_bit4_t باہرلا_if_equal)
{
      أكد(a.size() == b.size());

      بنانشانی sign_idx = a.size()-1;
      vvp_bit4_t a_sign = a.value(sign_idx);
      vvp_bit4_t b_sign = b.value(sign_idx);

      إذا (bit4_is_xz(a_sign))
	    أرجع BIT4_X;
      إذا (bit4_is_xz(b_sign))
	    أرجع BIT4_X;

      إذا (a_sign == b_sign)
	    أرجع compare_gtge(a, b, باہرلا_if_equal);

      إذا (a.has_xz())
	    أرجع BIT4_X;

      إذا (b.has_xz())
	    أرجع BIT4_X;

      إذا(a_sign == BIT4_0)
	    أرجع BIT4_1;
      وإلا
	    أرجع BIT4_0;
}
