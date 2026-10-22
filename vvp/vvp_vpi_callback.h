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

class કિંમત_callback;

/*
 * Things derived from vvp_vpi_callback may have callbacks
 * attached. This is how vpi callbacks are attached to the vvp
 * structure.
 *
 * Things derived from vvp_vpi_callback may also be array'ed, so it
 * includes some members that arrays use.
 */
class vvp_vpi_callback {

    જાહેર:
      vvp_vpi_callback();
      આભાસી ~vvp_vpi_callback();

      શૂન્ય attach_as_word(સંરચના __vpiArray* arr, unsigned long addr);

      શૂન્ય add_vpi_callback(કિંમત_callback*);
#ifdef CHECK_WITH_VALGRIND
	/* This has only been tested at EOS. */
      શૂન્ય clear_all_callbacks(શૂન્ય);
#endif

	// Derived classes implement this method to provide a way for
	// vpi to get at the vvp value of the object.
      આભાસી શૂન્ય get_કિંમત(સંરચના t_vpi_કિંમત*કિંમત) =0;

    સુરક્ષિત:
	// Derived classes call this method to indicate that it is
	// time to call the callback.
      શૂન્ય run_vpi_callbacks();

    ખાનગી:
      કિંમત_callback*vpi_callbacks_;
      સંરચના __vpi_array_word*array_શબ્દો;
};


#endif /* IVL_vvp_vpi_callback_H */
