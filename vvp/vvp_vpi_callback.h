#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ IVL_vvp_vpi_callback_H
#ವ್ಯಾಖ್ಯಾನಿಸು IVL_vvp_vpi_callback_H
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

class ಮೌಲ್ಯ_callback;

/*
 * Things derived from vvp_vpi_callback may have callbacks
 * attached. This is how vpi callbacks are attached to the vvp
 * structure.
 *
 * Things derived from vvp_vpi_callback may also be array'ed, so it
 * includes some members that arrays use.
 */
class vvp_vpi_callback {

    ಸಾರ್ವಜನಿಕ:
      vvp_vpi_callback();
      ವಾಸ್ತವಿಕ ~vvp_vpi_callback();

      ಶೂನ್ಯ attach_as_ಶಬ್ದ(ರಚನೆ __vpiArray* arr, ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ addr);

      ಶೂನ್ಯ ಕೂಡಿಸು_vpi_callback(ಮೌಲ್ಯ_callback*);
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ CHECK_WITH_VALGRIND
	/* This has only been tested at EOS. */
      ಶೂನ್ಯ clear_all_callbacks(ಶೂನ್ಯ);
#ಆದರೆ_ಅಂತ್ಯ

	// Derived classes implement this method to provide a way for
	// vpi to get at the vvp value of the object.
      ವಾಸ್ತವಿಕ ಶೂನ್ಯ get_ಮೌಲ್ಯ(ರಚನೆ t_vpi_ಮೌಲ್ಯ*ಮೌಲ್ಯ) =0;

    ಸಂರಕ್ಷಿತ:
	// Derived classes call this method to indicate that it is
	// time to call the callback.
      ಶೂನ್ಯ ಸತತ_vpi_callbacks();

    ಖಾಸಗಿ:
      ಮೌಲ್ಯ_callback*vpi_callbacks_;
      ರಚನೆ __vpi_array_word*array_ಪದಗಳು;
};


#ಆದರೆ_ಅಂತ್ಯ /* IVL_vvp_vpi_callback_H */
