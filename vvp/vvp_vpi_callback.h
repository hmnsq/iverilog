#ifndef IVL_vvp_vpi_callback_H
#define IVL_vvp_vpi_callback_H
/*
 * Copyright (c) 2009-2014 Stephen Williams (steve@icarus.com)
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
# include  "vpi_user.h"

class مقدار_callback;

/*
 * Things derived from vvp_vpi_callback may have callbacks
 * attached. This is how vpi callbacks are attached to the vvp
 * structure.
 *
 * Things derived from vvp_vpi_callback may also be array'ed, so it
 * includes some members that arrays use.
 */
class vvp_vpi_callback {

    عمومی:
      vvp_vpi_callback();
      مجازی ~vvp_vpi_callback();

      تهی attach_as_word(ساختار __vpiArray* arr, unsigned long addr);

      تهی add_vpi_callback(مقدار_callback*);
#ifdef CHECK_WITH_VALGRIND
	/* This has only been tested at EOS. */
      تهی clear_all_callbacks(تهی);
#endif

	// Derived classes implement this method to provide a way for
	// vpi to get at the vvp value of the object.
      مجازی تهی get_مقدار(ساختار t_vpi_مقدار*مقدار) =0;

    محافظتشده:
	// Derived classes call this method to indicate that it is
	// time to call the callback.
      تهی run_vpi_callbacks();

    خصوصی:
      مقدار_callback*vpi_callbacks_;
      ساختار __vpi_array_word*array_واژگان;
};


#endif /* IVL_vvp_vpi_callback_H */
