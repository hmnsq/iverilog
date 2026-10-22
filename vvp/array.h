#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ IVL_array_H
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ IVL_array_H
/*
 * Copyright (c) 2007-2014 Stephen Williams (steve@icarus.com)
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

#ਅਨ੍ਤਰ੍ਭਾਵ "vvp_net.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "vpi_user.h"

ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਬਣਤਰ __vpiArray* vvp_array_t;
class ਮੁੱਲ_callback;

/*
 * This function tries to find the array (by label) in the global
 * table of all the arrays in the design.
 */
ਬਾਹਰੀ vvp_array_t array_ਲੱਭੋ(ਅਚਲ ਅੱਖਰ*label);

/* VPI hooks */
ਬਾਹਰੀ ਮੁੱਲ_callback* vpip_array_ਸ਼ਬਦ_change(p_cb_ਅੰਕੜੇ ਅੰਕੜੇ);
ਬਾਹਰੀ ਮੁੱਲ_callback* vpip_array_change(p_cb_ਅੰਕੜੇ ਅੰਕੜੇ);

#ਜੇ_ਅੰਤ /* IVL_array_H */
