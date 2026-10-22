#अगर_अघोषित IVL_vvp_vpi_callback_H
#घोषणा IVL_vvp_vpi_callback_H
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

class मूल्य_callback;

/*
 * Things derived from vvp_vpi_callback may have callbacks
 * attached. This is how vpi callbacks are attached to the vvp
 * structure.
 *
 * Things derived from vvp_vpi_callback may also be array'ed, so it
 * includes some members that arrays use.
 */
class vvp_vpi_callback {

    खुला:
      vvp_vpi_callback();
      भव ~vvp_vpi_callback();

      व्योम attach_as_word(काष्ठा __vpiArray* arr, अचिन्हित दीर्घ addr);

      व्योम add_vpi_callback(मूल्य_callback*);
#अगर_घोषित CHECK_WITH_VALGRIND
	/* This has only been tested at EOS. */
      व्योम clear_all_callbacks(व्योम);
#पूर्ण_अगर

	// Derived classes implement this method to provide a way for
	// vpi to get at the vvp value of the object.
      भव व्योम get_मूल्य(काष्ठा t_vpi_मूल्य*मूल्य) =0;

    रक्षित:
	// Derived classes call this method to indicate that it is
	// time to call the callback.
      व्योम run_vpi_callbacks();

    निजी:
      मूल्य_callback*vpi_callbacks_;
      काष्ठा __vpi_array_word*array_शब्दगण;
};


#पूर्ण_अगर /* IVL_vvp_vpi_callback_H */
