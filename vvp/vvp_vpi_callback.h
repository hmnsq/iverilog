#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ IVL_vvp_vpi_callback_H
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ IVL_vvp_vpi_callback_H
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

class ਮੁੱਲ_callback;

/*
 * Things derived from vvp_vpi_callback may have callbacks
 * attached. This is how vpi callbacks are attached to the vvp
 * structure.
 *
 * Things derived from vvp_vpi_callback may also be array'ed, so it
 * includes some members that arrays use.
 */
class vvp_vpi_callback {

    ਜਨਤਕ:
      vvp_vpi_callback();
      ਆਭਾਸੀ ~vvp_vpi_callback();

      ਖਾਲੀ attach_as_ਸ਼ਬਦ(ਬਣਤਰ __vpiArray* arr, ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ addr);

      ਖਾਲੀ ਜੋੜੋ_vpi_callback(ਮੁੱਲ_callback*);
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ CHECK_WITH_VALGRIND
	/* This has only been tested at EOS. */
      ਖਾਲੀ clear_all_callbacks(ਖਾਲੀ);
#ਜੇ_ਅੰਤ

	// Derived classes implement this method to provide a way for
	// vpi to get at the vvp value of the object.
      ਆਭਾਸੀ ਖਾਲੀ get_ਮੁੱਲ(ਬਣਤਰ t_vpi_ਮੁੱਲ*ਮੁੱਲ) =0;

    ਸੁਰੱਖਿਅਤ:
	// Derived classes call this method to indicate that it is
	// time to call the callback.
      ਖਾਲੀ ਲੜੀਵਾਰ_vpi_callbacks();

    ਨਿਜੀ:
      ਮੁੱਲ_callback*vpi_callbacks_;
      ਬਣਤਰ __vpi_array_word*array_ਸ਼ਬਦ;
};


#ਜੇ_ਅੰਤ /* IVL_vvp_vpi_callback_H */
