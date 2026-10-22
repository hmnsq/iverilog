#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ IVL_statistics_H
#ವ್ಯಾಖ್ಯಾನಿಸು IVL_statistics_H
/*
 * Copyright (c) 2002-2014 Stephen Williams (steve@icarus.com)
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

// The SunPro C++ compiler is broken and does not define size_t in cstddef.
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ __SUNPRO_CC
# include  <stddef.h>
#ಇಲ್ಲದಿದ್ದರೆ
# include  <cstddef>
#ಆದರೆ_ಅಂತ್ಯ

ಬಾಹ್ಯ ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ ಎಣಿಕೆ_opcodes;
ಬಾಹ್ಯ ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ ಎಣಿಕೆ_functors;
ಬಾಹ್ಯ ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ ಎಣಿಕೆ_functors_logic;
ಬಾಹ್ಯ ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ ಎಣಿಕೆ_functors_bufif;
ಬಾಹ್ಯ ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ ಎಣಿಕೆ_functors_resolv;
ಬಾಹ್ಯ ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ ಎಣಿಕೆ_functors_sig;
ಬಾಹ್ಯ ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ ಎಣಿಕೆ_filters;
ಬಾಹ್ಯ ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ ಎಣಿಕೆ_vvp_nets;
ಬಾಹ್ಯ ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ ಎಣಿಕೆ_vpi_nets;
ಬಾಹ್ಯ ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ ಎಣಿಕೆ_vpi_scopes;

ಬಾಹ್ಯ ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ ಎಣಿಕೆ_net_arrays;
ಬಾಹ್ಯ ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ ಎಣಿಕೆ_net_array_ಪದಗಳು;
ಬಾಹ್ಯ ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ ಎಣಿಕೆ_var_arrays;
ಬಾಹ್ಯ ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ ಎಣಿಕೆ_var_array_ಪದಗಳು;
ಬಾಹ್ಯ ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ ಎಣಿಕೆ_real_arrays;
ಬಾಹ್ಯ ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ ಎಣಿಕೆ_real_array_ಪದಗಳು;


ಬಾಹ್ಯ ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ ಎಣಿಕೆ_ಸಮಯ_events;
ಬಾಹ್ಯ ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ ಎಣಿಕೆ_ಸಮಯ_pool(ಶೂನ್ಯ);

ಬಾಹ್ಯ ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ ಎಣಿಕೆ_ನಿಯೋಜಿಸು_events;
ಬಾಹ್ಯ ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ ಎಣಿಕೆ_assign4_pool(ಶೂನ್ಯ);
ಬಾಹ್ಯ ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ ಎಣಿಕೆ_assign8_pool(ಶೂನ್ಯ);
ಬಾಹ್ಯ ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ ಎಣಿಕೆ_ನಿಯೋಜಿಸು_real_pool(ಶೂನ್ಯ);
ಬಾಹ್ಯ ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ ಎಣಿಕೆ_ನಿಯೋಜಿಸು_aword_pool(ಶೂನ್ಯ);
ಬಾಹ್ಯ ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ ಎಣಿಕೆ_ನಿಯೋಜಿಸು_arword_pool(ಶೂನ್ಯ);

ಬಾಹ್ಯ ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ ಎಣಿಕೆ_gen_events;
ಬಾಹ್ಯ ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ ಎಣಿಕೆ_gen_pool(ಶೂನ್ಯ);

ಬಾಹ್ಯ size_t ಗಾತ್ರ_opcodes;
ಬಾಹ್ಯ size_t ಗಾತ್ರ_vvp_nets;
ಬಾಹ್ಯ size_t ಗಾತ್ರ_vvp_net_funs;

#ಆದರೆ_ಅಂತ್ಯ /* IVL_statistics_H */
