#నిర్వచితం_కాకపోతే IVL_vvp_vpi_callback_H
#నిర్వచించు IVL_vvp_vpi_callback_H
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

class విలువ_callback;

/*
 * Things derived from vvp_vpi_callback may have callbacks
 * attached. This is how vpi callbacks are attached to the vvp
 * structure.
 *
 * Things derived from vvp_vpi_callback may also be array'ed, so it
 * includes some members that arrays use.
 */
class vvp_vpi_callback {

    బహిరంగ:
      vvp_vpi_callback();
      మాయా ~vvp_vpi_callback();

      శూన్య attach_as_మాట(నిర్మాణం __vpiArray* arr, గుర్తులేని దీర్ఘ addr);

      శూన్య కూడు_vpi_callback(విలువ_callback*);
#నిర్వచితమైతే CHECK_WITH_VALGRIND
	/* This has only been tested at EOS. */
      శూన్య clear_all_callbacks(శూన్య);
#అయితే_ముగింపు

	// Derived classes implement this method to provide a way for
	// vpi to get at the vvp value of the object.
      మాయా శూన్య get_విలువ(నిర్మాణం t_vpi_విలువ*విలువ) =0;

    రక్షిత:
	// Derived classes call this method to indicate that it is
	// time to call the callback.
      శూన్య నిరంతరం_vpi_callbacks();

    అంతరంగ:
      విలువ_callback*vpi_callbacks_;
      నిర్మాణం __vpi_array_word*array_పదాలు;
};


#అయితే_ముగింపు /* IVL_vvp_vpi_callback_H */
