#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ IVL_compile_H
#ವ್ಯಾಖ್ಯಾನಿಸು IVL_compile_H
/*
 * Copyright (c) 2001-2026 Stephen Williams (steve@icarus.com)
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

# include  <cstdio>
# include  <fstream>
# include  <list>
# include  <vector>
# include  "parse_misc.h"
# include  "sv_vpi_user.h"
# include  "vvp_net.h"

/*
 * The file names are kept in this vector. Entry 0 is "N/A" and 1 is
 * for interactive commands.
 */
ಬಾಹ್ಯ std::ಸದಿಶ<ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ*> file_ಹೆಸರುಗಳು;

/*
 * The functions described here are the compile time support
 * functions. Various bits of the compile process are taken care of
 * here. What is called when is mostly controlled by the parser.
 *
 * Before compilation takes place, the compile_init function must be
 * called once to set stuff up.
 */

ಬಾಹ್ಯ ಶೂನ್ಯ compile_init(ಶೂನ್ಯ);

ಬಾಹ್ಯ ಶೂನ್ಯ compile_cleanup(ಶೂನ್ಯ);

ಬಾಹ್ಯ bool verbose_flag;

/*
 * If this file opened, then write debug information to this
 * file. This is used for debugging the VVP runtime itself.
 */
ಬಾಹ್ಯ std::ofstream debug_file;

/*
 *  Connect a list of symbols to a contiguous set of ipoints.
 *  Constants C<?> are handled by setting the ival of the ipoint.
 */
ಬಾಹ್ಯ ಶೂನ್ಯ inputs_connect(vvp_net_t*fdx, ಚಿಹ್ನೆರಹಿತ argc, ರಚನೆ symb_s*argv);
ಬಾಹ್ಯ ಶೂನ್ಯ ಒಳಹರಿವು_connect(vvp_net_t*fdx, ಚಿಹ್ನೆರಹಿತ ಪೋರ್ಟ್, ಅಕ್ಷರ*label);

/*
 * This function is an expansion of the inputs_connect function. It
 * uses the inputs_connect function, but it creates vvp_wide_fun_t
 * nodes to handle arbitrary width nodes, and connects those nodes to
 * the vvp_wide_fun_core object passed in.
 */
ಬಾಹ್ಯ ಶೂನ್ಯ wide_inputs_connect(vvp_wide_fun_core*core,
				ಚಿಹ್ನೆರಹಿತ argc, ರಚನೆ symb_s*argv);

ಬಾಹ್ಯ vvp_net_t* vvp_net_lookup(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ*label);
ಬಾಹ್ಯ vpiHandle vvp_lookup_handle(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ*label);

/*
 *  Add a functor to the symbol table
 */
ಬಾಹ್ಯ ಶೂನ್ಯ define_functor_ಚಿಹ್ನೆ(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ*label, vvp_net_t*ipt);


/*
 * This is a count of errors encountered during compilation. If this
 * is non-zero, then simulation is not recommended.
 */
ಬಾಹ್ಯ ಚಿಹ್ನೆರಹಿತ compile_errors;

ಬಾಹ್ಯ ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ* module_ಮಾರ್ಗ;
ಬಾಹ್ಯ ಶೂನ್ಯ compile_ತುಂಬು_vpi_module(ಅಕ್ಷರ*name);

ಬಾಹ್ಯ ಶೂನ್ಯ compile_vpi_ಸಮಯ_precision(ದೀರ್ಘ pre);

/*
 * This function is called by the parser to compile a functor
 * statement. The strings passed in are allocated by the lexor, but
 * this function will free them. (Or save them permanently.) This
 * includes the argv array and the strings it references.
 *
 * The argc and argv are a list of char* that are the port parameters
 * of the functor. The compile should match those port parameters up
 * to existing functors to manage the linking.
 */
ಬಾಹ್ಯ ಶೂನ್ಯ compile_functor(ಅಕ್ಷರ*label, ಅಕ್ಷರ*type, ಚಿಹ್ನೆರಹಿತ ಅಗಲ,
			    ಚಿಹ್ನೆರಹಿತ ostr0, ಚಿಹ್ನೆರಹಿತ ostr1,
			    ಚಿಹ್ನೆರಹಿತ argc, ರಚನೆ symb_s*argv);


/*
 * This is called by the parser to make a resolver. This is a special
 * kind of functor; a strength aware functor. It has up to 4 inputs
 * that are blended to make a resolved output. The type string selects
 * a resolution algorithm.
 */
ಬಾಹ್ಯ ಶೂನ್ಯ compile_resolver(ಅಕ್ಷರ*label, ಅಕ್ಷರ*type,
			     ಚಿಹ್ನೆರಹಿತ argc, ರಚನೆ symb_s*argv);

ಬಾಹ್ಯ ಶೂನ್ಯ compile_concat(ಅಕ್ಷರ*label, ಚಿಹ್ನೆರಹಿತ w0, ಚಿಹ್ನೆರಹಿತ w1,
			   ಚಿಹ್ನೆರಹಿತ w2, ಚಿಹ್ನೆರಹಿತ w3,
			   ಚಿಹ್ನೆರಹಿತ argc, ರಚನೆ symb_s*argv);

ಬಾಹ್ಯ ಶೂನ್ಯ compile_concat8(ಅಕ್ಷರ*label, ಚಿಹ್ನೆರಹಿತ w0, ಚಿಹ್ನೆರಹಿತ w1,
			    ಚಿಹ್ನೆರಹಿತ w2, ಚಿಹ್ನೆರಹಿತ w3,
			    ಚಿಹ್ನೆರಹಿತ argc, ರಚನೆ symb_s*argv);

ಬಾಹ್ಯ ಶೂನ್ಯ compile_substitute(ಅಕ್ಷರ*label, ಚಿಹ್ನೆರಹಿತ ಅಗಲ,
			       ಚಿಹ್ನೆರಹಿತ soff, ಚಿಹ್ನೆರಹಿತ swidth,
			       ಚಿಹ್ನೆರಹಿತ argc, ರಚನೆ symb_s*argv);

/*
 * Arrange for the system task/function call to have its compiletf
 * function called.
 */
ಬಾಹ್ಯ ಶೂನ್ಯ compile_compiletf(ರಚನೆ __vpiSysTaskCall*);

/*
 * Compile delay nodes of various form.
 */
ಬಾಹ್ಯ ಶೂನ್ಯ compile_delay(ಅಕ್ಷರ*label, ಚಿಹ್ನೆರಹಿತ ಅಗಲ,
                          vvp_delay_t*del, ರಚನೆ symb_s ಒಳಹರಿವು);
ಬಾಹ್ಯ ಶೂನ್ಯ compile_delay(ಅಕ್ಷರ*label, ಚಿಹ್ನೆರಹಿತ ಅಗಲ,
                          ಚಿಹ್ನೆರಹಿತ argc, ರಚನೆ symb_s*argv,
                          bool ignore_decay);

/*
 * This is called by the parser to create a part select node.
 * See the PART SELECT STATEMENT section in the README.txt
 */
ಬಾಹ್ಯ ಶೂನ್ಯ compile_part_select(ಅಕ್ಷರ*label, ಅಕ್ಷರ*src,
				ಚಿಹ್ನೆರಹಿತ ತಳ, ಚಿಹ್ನೆರಹಿತ wid);
ಬಾಹ್ಯ ಶೂನ್ಯ compile_part_select_pv(ಅಕ್ಷರ*label, ಅಕ್ಷರ*src,
				   ಚಿಹ್ನೆರಹಿತ ತಳ, ಚಿಹ್ನೆರಹಿತ wid,
				   ಚಿಹ್ನೆರಹಿತ vec_wid);
ಬಾಹ್ಯ ಶೂನ್ಯ compile_part_select_var(ಅಕ್ಷರ*label, ಅಕ್ಷರ*src,
                                    ಅಕ್ಷರ*var, ಚಿಹ್ನೆರಹಿತ wid, bool is_signed);

/*
 * This is called by the parser to make the various arithmetic and
 * comparison functors.
 */
ಬಾಹ್ಯ ಶೂನ್ಯ compile_arith_pow(ಅಕ್ಷರ*label, ದೀರ್ಘ ಅಗಲ, bool signed_flag,
			      ಚಿಹ್ನೆರಹಿತ argc, ರಚನೆ symb_s*argv);
ಬಾಹ್ಯ ಶೂನ್ಯ compile_arith_abs(ಅಕ್ಷರ*label,
			      ಚಿಹ್ನೆರಹಿತ argc, ರಚನೆ symb_s*argv);
ಬಾಹ್ಯ ಶೂನ್ಯ compile_arith_cast_int(ಅಕ್ಷರ*label, ದೀರ್ಘ ಅಗಲ,
			           ಚಿಹ್ನೆರಹಿತ argc, ರಚನೆ symb_s*argv);
ಬಾಹ್ಯ ಶೂನ್ಯ compile_arith_cast_real(ಅಕ್ಷರ*label, bool signed_flag,
			            ಚಿಹ್ನೆರಹಿತ argc, ರಚನೆ symb_s*argv);
ಬಾಹ್ಯ ಶೂನ್ಯ compile_arith_cast_vec2(ಅಕ್ಷರ*label, ದೀರ್ಘ ಅಗಲ,
				    ಚಿಹ್ನೆರಹಿತ argc, ರಚನೆ symb_s*argv);
ಬಾಹ್ಯ ಶೂನ್ಯ compile_arith_div(ಅಕ್ಷರ*label, ದೀರ್ಘ ಅಗಲ, bool signed_flag,
			      ಚಿಹ್ನೆರಹಿತ argc, ರಚನೆ symb_s*argv);
ಬಾಹ್ಯ ಶೂನ್ಯ compile_arith_mod(ಅಕ್ಷರ*label, ದೀರ್ಘ ಅಗಲ, bool signed_flag,
			      ಚಿಹ್ನೆರಹಿತ argc, ರಚನೆ symb_s*argv);
ಬಾಹ್ಯ ಶೂನ್ಯ compile_arith_mult(ಅಕ್ಷರ*label, ದೀರ್ಘ ಅಗಲ,
			       ಚಿಹ್ನೆರಹಿತ argc, ರಚನೆ symb_s*argv);
ಬಾಹ್ಯ ಶೂನ್ಯ compile_arith_ಮೊತ್ತ(ಅಕ್ಷರ*label, ದೀರ್ಘ ಅಗಲ,
			      ಚಿಹ್ನೆರಹಿತ argc, ರಚನೆ symb_s*argv);
ಬಾಹ್ಯ ಶೂನ್ಯ compile_arith_sub(ಅಕ್ಷರ*label, ದೀರ್ಘ ಅಗಲ,
			      ಚಿಹ್ನೆರಹಿತ argc, ರಚನೆ symb_s*argv);
ಬಾಹ್ಯ ಶೂನ್ಯ compile_cmp_eeq(ಅಕ್ಷರ*label, ದೀರ್ಘ ಅಗಲ,
			   ಚಿಹ್ನೆರಹಿತ argc, ರಚನೆ symb_s*argv);
ಬಾಹ್ಯ ಶೂನ್ಯ compile_cmp_nee(ಅಕ್ಷರ*label, ದೀರ್ಘ ಅಗಲ,
			   ಚಿಹ್ನೆರಹಿತ argc, ರಚನೆ symb_s*argv);
ಬಾಹ್ಯ ಶೂನ್ಯ compile_cmp_eq(ಅಕ್ಷರ*label, ದೀರ್ಘ ಅಗಲ,
			   ಚಿಹ್ನೆರಹಿತ argc, ರಚನೆ symb_s*argv);
ಬಾಹ್ಯ ಶೂನ್ಯ compile_cmp_eqx(ಅಕ್ಷರ*label, ದೀರ್ಘ ಅಗಲ,
			    ಚಿಹ್ನೆರಹಿತ argc, ರಚನೆ symb_s*argv);
ಬಾಹ್ಯ ಶೂನ್ಯ compile_cmp_eqz(ಅಕ್ಷರ*label, ದೀರ್ಘ ಅಗಲ,
			    ಚಿಹ್ನೆರಹಿತ argc, ರಚನೆ symb_s*argv);
ಬಾಹ್ಯ ಶೂನ್ಯ compile_cmp_ne(ಅಕ್ಷರ*label, ದೀರ್ಘ ಅಗಲ,
			   ಚಿಹ್ನೆರಹಿತ argc, ರಚನೆ symb_s*argv);
ಬಾಹ್ಯ ಶೂನ್ಯ compile_cmp_ge(ಅಕ್ಷರ*label, ದೀರ್ಘ ಅಗಲ, bool signed_flag,
			   ಚಿಹ್ನೆರಹಿತ argc, ರಚನೆ symb_s*argv);
ಬಾಹ್ಯ ಶೂನ್ಯ compile_cmp_gt(ಅಕ್ಷರ*label, ದೀರ್ಘ ಅಗಲ, bool signed_flag,
			   ಚಿಹ್ನೆರಹಿತ argc, ರಚನೆ symb_s*argv);
ಬಾಹ್ಯ ಶೂನ್ಯ compile_cmp_weq(ಅಕ್ಷರ*label, ದೀರ್ಘ ಅಗಲ,
			   ಚಿಹ್ನೆರಹಿತ argc, ರಚನೆ symb_s*argv);
ಬಾಹ್ಯ ಶೂನ್ಯ compile_cmp_wne(ಅಕ್ಷರ*label, ದೀರ್ಘ ಅಗಲ,
			   ಚಿಹ್ನೆರಹಿತ argc, ರಚನೆ symb_s*argv);

ಬಾಹ್ಯ ಶೂನ್ಯ compile_arith_mult_r(ಅಕ್ಷರ*label, ಚಿಹ್ನೆರಹಿತ argc,
                                 ರಚನೆ symb_s*argv);
ಬಾಹ್ಯ ಶೂನ್ಯ compile_arith_pow_r(ಅಕ್ಷರ*label, ಚಿಹ್ನೆರಹಿತ argc, ರಚನೆ symb_s*argv);
ಬಾಹ್ಯ ಶೂನ್ಯ compile_arith_div_r(ಅಕ್ಷರ*label, ಚಿಹ್ನೆರಹಿತ argc, ರಚನೆ symb_s*argv);
ಬಾಹ್ಯ ಶೂನ್ಯ compile_arith_mod_r(ಅಕ್ಷರ*label, ಚಿಹ್ನೆರಹಿತ argc, ರಚನೆ symb_s*argv);
ಬಾಹ್ಯ ಶೂನ್ಯ compile_arith_ಮೊತ್ತ_r(ಅಕ್ಷರ*label, ಚಿಹ್ನೆರಹಿತ argc, ರಚನೆ symb_s*argv);
ಬಾಹ್ಯ ಶೂನ್ಯ compile_arith_sub_r(ಅಕ್ಷರ*label, ಚಿಹ್ನೆರಹಿತ argc, ರಚನೆ symb_s*argv);
ಬಾಹ್ಯ ಶೂನ್ಯ compile_cmp_eq_r(ಅಕ್ಷರ*label, ಚಿಹ್ನೆರಹಿತ argc, ರಚನೆ symb_s*argv);
ಬಾಹ್ಯ ಶೂನ್ಯ compile_cmp_ne_r(ಅಕ್ಷರ*label, ಚಿಹ್ನೆರಹಿತ argc, ರಚನೆ symb_s*argv);
ಬಾಹ್ಯ ಶೂನ್ಯ compile_cmp_ge_r(ಅಕ್ಷರ*label, ಚಿಹ್ನೆರಹಿತ argc, ರಚನೆ symb_s*argv);
ಬಾಹ್ಯ ಶೂನ್ಯ compile_cmp_gt_r(ಅಕ್ಷರ*label, ಚಿಹ್ನೆರಹಿತ argc, ರಚನೆ symb_s*argv);

ಬಾಹ್ಯ ಶೂನ್ಯ compile_dff(ಅಕ್ಷರ*label, ಚಿಹ್ನೆರಹಿತ ಅಗಲ, bool negedge,
			ರಚನೆ symb_s arg_d,
			ರಚನೆ symb_s arg_c,
			ರಚನೆ symb_s arg_e);

ಬಾಹ್ಯ ಶೂನ್ಯ compile_dff_aclr(ಅಕ್ಷರ*label, ಚಿಹ್ನೆರಹಿತ ಅಗಲ, bool negedge,
			     ರಚನೆ symb_s arg_d,
			     ರಚನೆ symb_s arg_c,
			     ರಚನೆ symb_s arg_e,
			     ರಚನೆ symb_s arg_a);
ಬಾಹ್ಯ ಶೂನ್ಯ compile_dff_aset(ಅಕ್ಷರ*label, ಚಿಹ್ನೆರಹಿತ ಅಗಲ, bool negedge,
			     ರಚನೆ symb_s arg_d,
			     ರಚನೆ symb_s arg_c,
			     ರಚನೆ symb_s arg_e,
			     ರಚನೆ symb_s arg_a,
			     ಅಕ್ಷರ*asc_ಮೌಲ್ಯ);

ಬಾಹ್ಯ ಶೂನ್ಯ compile_latch(ಅಕ್ಷರ*label, ಚಿಹ್ನೆರಹಿತ ಅಗಲ,
			  ರಚನೆ symb_s arg_d,
			  ರಚನೆ symb_s arg_e);

ಬಾಹ್ಯ ಶೂನ್ಯ compile_enum2_type(ಅಕ್ಷರ*label, ದೀರ್ಘ ಅಗಲ, bool signed_flag,
			      std::list<ರಚನೆ enum_name_s>*ಹೆಸರುಗಳು);
ಬಾಹ್ಯ ಶೂನ್ಯ compile_enum4_type(ಅಕ್ಷರ*label, ದೀರ್ಘ ಅಗಲ, bool signed_flag,
			      std::list<ರಚನೆ enum_name_s>*ಹೆಸರುಗಳು);

ರಚನೆ __vpiModPath;
ಬಾಹ್ಯ __vpiModPath* compile_modpath(ಅಕ್ಷರ*label,
                                     ಚಿಹ್ನೆರಹಿತ ಅಗಲ,
				     ರಚನೆ symb_s drv,
				     ರಚನೆ symb_s dest);
ಬಾಹ್ಯ ಶೂನ್ಯ compile_modpath_src(__vpiModPath*dst,
				ಅಕ್ಷರ ಅಂಚು_c,
				ಸ್ಥಿರಾಂಕ ರಚನೆ symb_s&src,
				ರಚನೆ numbv_s&vals,
				ಸ್ಥಿರಾಂಕ ರಚನೆ symb_s&condit_src,
				ಸ್ಥಿರಾಂಕ ರಚನೆ symb_s&ಮಾರ್ಗ_ಪದ_ಒಳ);
ಬಾಹ್ಯ ಶೂನ್ಯ compile_modpath_src(__vpiModPath*dst,
				ಅಕ್ಷರ ಅಂಚು_c,
				ಸ್ಥಿರಾಂಕ ರಚನೆ symb_s&src,
				ರಚನೆ numbv_s&vals,
				ಪೂರ್ಣಾಂಕ condit_src, /* match with '0' */
				ಸ್ಥಿರಾಂಕ ರಚನೆ symb_s&ಮಾರ್ಗ_ಪದ_ಒಳ,
				bool ifnone);

ಬಾಹ್ಯ ಶೂನ್ಯ compile_reduce_and(ಅಕ್ಷರ*label, ಸ್ಥಿರಾಂಕ ರಚನೆ symb_s&arg);
ಬಾಹ್ಯ ಶೂನ್ಯ compile_reduce_or(ಅಕ್ಷರ*label, ಸ್ಥಿರಾಂಕ ರಚನೆ symb_s&arg);
ಬಾಹ್ಯ ಶೂನ್ಯ compile_reduce_xor(ಅಕ್ಷರ*label, ಸ್ಥಿರಾಂಕ ರಚನೆ symb_s&arg);
ಬಾಹ್ಯ ಶೂನ್ಯ compile_reduce_nand(ಅಕ್ಷರ*label, ಸ್ಥಿರಾಂಕ ರಚನೆ symb_s&arg);
ಬಾಹ್ಯ ಶೂನ್ಯ compile_reduce_nor(ಅಕ್ಷರ*label, ಸ್ಥಿರಾಂಕ ರಚನೆ symb_s&arg);
ಬಾಹ್ಯ ಶೂನ್ಯ compile_reduce_xnor(ಅಕ್ಷರ*label, ಸ್ಥಿರಾಂಕ ರಚನೆ symb_s&arg);

ಬಾಹ್ಯ ಶೂನ್ಯ compile_extend_signed(ಅಕ್ಷರ*label, ದೀರ್ಘ ಅಗಲ, ರಚನೆ symb_s arg);

ಬಾಹ್ಯ ಶೂನ್ಯ compile_sfunc(ಅಕ್ಷರ*label, ಅಕ್ಷರ*name, ಅಕ್ಷರ*format_string,
			  ದೀರ್ಘ file_idx, ದೀರ್ಘ lineno,
			  ಚಿಹ್ನೆರಹಿತ argc, ರಚನೆ symb_s*argv,
                          ಅಕ್ಷರ*trigger_label);

ಬಾಹ್ಯ ಶೂನ್ಯ compile_repeat(ಅಕ್ಷರ*label, ದೀರ್ಘ ಅಗಲ, ದೀರ್ಘ repeat,
			   ರಚನೆ symb_s arg);

ಬಾಹ್ಯ ಶೂನ್ಯ compile_shiftl(ಅಕ್ಷರ*label, ದೀರ್ಘ ಅಗಲ,
			   ಚಿಹ್ನೆರಹಿತ argc, ರಚನೆ symb_s*argv);
ಬಾಹ್ಯ ಶೂನ್ಯ compile_shiftr(ಅಕ್ಷರ*label, ದೀರ್ಘ ಅಗಲ, bool signed_flag,
			   ಚಿಹ್ನೆರಹಿತ argc, ರಚನೆ symb_s*argv);

ಬಾಹ್ಯ ಶೂನ್ಯ compile_timescale(ದೀರ್ಘ units, ದೀರ್ಘ precision);

ಬಾಹ್ಯ ಶೂನ್ಯ compile_vpi_ಚಿಹ್ನೆ(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ*label, vpiHandle obj);
ಬಾಹ್ಯ ಶೂನ್ಯ compile_vpi_lookup(vpiHandle *objref, ಅಕ್ಷರ*label);

ಬಾಹ್ಯ ಶೂನ್ಯ compile_param_string(ಅಕ್ಷರ*label, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ*name, ಅಕ್ಷರ*ಮೌಲ್ಯ,
                                 bool local_flag,
                                 ದೀರ್ಘ file_idx, ದೀರ್ಘ lineno);
ಬಾಹ್ಯ ಶೂನ್ಯ compile_param_logic(ಅಕ್ಷರ*label, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ*name, ಅಕ್ಷರ*ಮೌಲ್ಯ,
				bool signed_flag, bool local_flag,
                                ದೀರ್ಘ file_idx, ದೀರ್ಘ lineno);
ಬಾಹ್ಯ ಶೂನ್ಯ compile_param_real(ಅಕ್ಷರ*label, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ*name, ಅಕ್ಷರ*ಮೌಲ್ಯ,
                               bool local_flag,
                               ದೀರ್ಘ file_idx, ದೀರ್ಘ lineno);

/*
 * The resolv_list_s is the base class for a symbol resolve
 * action. Some function creates an instance of a resolv_list_s object
 * that contains the data pertinent to that resolution request, and
 * executes it with the resolv_submit function. If the operation can
 * complete, then the resolv_submit deletes the object. Otherwise, it
 * pushes it onto the resolv_list for later processing.
 *
 * Derived classes implement the resolve function to perform the
 * actual binding or resolution that the instance requires. If the
 * function succeeds, the resolve method returns true and the object
 * can be deleted any time.
 *
 * The mes parameter of the resolve method tells the resolver that
 * this call is its last chance. If it cannot complete the operation,
 * it must print an error message and return false.
 */
class resolv_list_s {

    ಸಾರ್ವಜನಿಕ:
      explicit resolv_list_s(ಅಕ್ಷರ*lab) : label_(lab) {
	    ಮುಂದಿನ_2 = ಖಾಲಿ;
      }
      ವಾಸ್ತವಿಕ ~resolv_list_s();

      resolv_list_s(ಸ್ಥಿರಾಂಕ resolv_list_s&) = delete;
      resolv_list_s& ಕ್ರಿಯಾಕಾರಕ=(ಸ್ಥಿರಾಂಕ resolv_list_s&) = delete;

      ವಾಸ್ತವಿಕ bool resolve(bool mes = false) = 0;

    ಸಂರಕ್ಷಿತ:
      ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ*label() ಸ್ಥಿರಾಂಕ { ಹಿಂತಿರುಗಿ label_; }

    ಖಾಸಗಿ:
      ಮಿತ್ರ ಶೂನ್ಯ resolv_submit(class resolv_list_s*cur);
      ಮಿತ್ರ ಶೂನ್ಯ compile_cleanup(ಶೂನ್ಯ);

      ಅಕ್ಷರ*label_;
      class resolv_list_s*ಮುಂದಿನ_2;
};

/*
 * This function schedules a lookup of an indexed label. The ref
 * points to the vvp_net_t that receives the result. The result may
 * be assigned later, if the symbol is defined later in the source
 * file, so the memory that ref points to must persist.
 *
 * The text for the label will be deleted by this function, or the
 * cleanup that completes the binding.
 */
ಬಾಹ್ಯ ಶೂನ್ಯ functor_ref_lookup(vvp_net_t**ref, ಅಕ್ಷರ*lab);

ಬಾಹ್ಯ vpiHandle vpip_make_prop_ಸರತಿ_ref(ಅಕ್ಷರ* class_label,
                                          ಚಿಹ್ನೆರಹಿತ prop_idx,
                                          ಚಿಹ್ನೆರಹಿತ is_ಸರತಿ_flag);

/*
 * This function schedules a lookup of the labeled instruction. The
 * code points to a code structure that points to the instruction
 * field that receives the result, and the label is the name to
 * lookup. The lookup will free the label text when it is done.
 *
 * The cptr2 flag tells the lookup to write the code pointer into the
 * cptr2 member of the code, instead of the cptr member.
 */
ಬಾಹ್ಯ ಶೂನ್ಯ ಸಂಕೇತ_label_lookup(ರಚನೆ vvp_ಸಂಕೇತ_s *ಸಂಕೇತ_2, ಅಕ್ಷರ *label, bool cptr2);

/*
 * The `compile_udp_def' function creates a UDP.  The `table' is a
 * NULL terminated array of char*, as assembled by `compile_udp_table'.
 * `compile_udp_table' is called with `table'==NULL to create a new
 * table, or with an existing table to append to.
 *
 * `compile_udp_functor' creates a mode-3 functor referring to the
 * labeled UDP.
 */

ಬಾಹ್ಯ ಶೂನ್ಯ compile_udp_def(ಪೂರ್ಣಾಂಕ sequ, ಅಕ್ಷರ*label, ಅಕ್ಷರ *name,
			    ಚಿಹ್ನೆರಹಿತ nin, ಚಿಹ್ನೆರಹಿತ init, ಅಕ್ಷರ **ಕೋಷ್ಟಕ);

ಬಾಹ್ಯ ಶೂನ್ಯ compile_udp_functor(ಅಕ್ಷರ*label, ಅಕ್ಷರ*type,
				ಚಿಹ್ನೆರಹಿತ argc, ರಚನೆ symb_s*argv);

ಬಾಹ್ಯ ಅಕ್ಷರ **compile_udp_ಕೋಷ್ಟಕ(ಅಕ್ಷರ **ಕೋಷ್ಟಕ, ಅಕ್ಷರ *ಅಡ್ಡಸಾಲು);

/*
 * Memory Instances, Ports, and Initialization
 */

ಬಾಹ್ಯ ಶೂನ್ಯ compile_var_array(ಅಕ್ಷರ*label, ಅಕ್ಷರ*name,
			      ಪೂರ್ಣಾಂಕ ಕೊನೆಯ, ಪೂರ್ಣಾಂಕ ಮೊದಲ,
			      ಪೂರ್ಣಾಂಕ msb, ಪೂರ್ಣಾಂಕ lsb, ಅಕ್ಷರ signed_flag);
ಬಾಹ್ಯ ಶೂನ್ಯ compile_var2_array(ಅಕ್ಷರ*label, ಅಕ್ಷರ*name,
			      ಪೂರ್ಣಾಂಕ ಕೊನೆಯ, ಪೂರ್ಣಾಂಕ ಮೊದಲ,
			      ಪೂರ್ಣಾಂಕ msb, ಪೂರ್ಣಾಂಕ lsb, bool signed_flag);
ಬಾಹ್ಯ ಶೂನ್ಯ compile_real_array(ಅಕ್ಷರ*label, ಅಕ್ಷರ*name,
			       ಪೂರ್ಣಾಂಕ ಕೊನೆಯ, ಪೂರ್ಣಾಂಕ ಮೊದಲ);
ಬಾಹ್ಯ ಶೂನ್ಯ compile_string_array(ಅಕ್ಷರ*label, ಅಕ್ಷರ*name,
				 ಪೂರ್ಣಾಂಕ ಕೊನೆಯ, ಪೂರ್ಣಾಂಕ ಮೊದಲ);
ಬಾಹ್ಯ ಶೂನ್ಯ compile_object_array(ಅಕ್ಷರ*label, ಅಕ್ಷರ*name,
				 ಪೂರ್ಣಾಂಕ ಕೊನೆಯ, ಪೂರ್ಣಾಂಕ ಮೊದಲ);
ಬಾಹ್ಯ ಶೂನ್ಯ compile_net_array(ಅಕ್ಷರ*label, ಅಕ್ಷರ*name,
			      ಪೂರ್ಣಾಂಕ ಕೊನೆಯ, ಪೂರ್ಣಾಂಕ ಮೊದಲ);
ಬಾಹ್ಯ ಶೂನ್ಯ compile_array_alias(ಅಕ್ಷರ*label, ಅಕ್ಷರ*name, ಅಕ್ಷರ*src);

  /* Index is a net. */
ಬಾಹ್ಯ ಶೂನ್ಯ compile_array_ಪೋರ್ಟ್(ಅಕ್ಷರ*label, ಅಕ್ಷರ*name, ಅಕ್ಷರ*addr);
  /* Index is a constant address */
ಬಾಹ್ಯ ಶೂನ್ಯ compile_array_ಪೋರ್ಟ್(ಅಕ್ಷರ*label, ಅಕ್ಷರ*name, ದೀರ್ಘ addr);

ಬಾಹ್ಯ ಶೂನ್ಯ compile_array_cleanup(ಶೂನ್ಯ);

/*
 * Compile the .ufunc statement.
 */
ಬಾಹ್ಯ ಶೂನ್ಯ compile_ufunc_real(ಅಕ್ಷರ*label, ಅಕ್ಷರ*ಸಂಕೇತ_2, ಚಿಹ್ನೆರಹಿತ wid,
			  ಚಿಹ್ನೆರಹಿತ argc, ರಚನೆ symb_s*argv,
			  ಚಿಹ್ನೆರಹಿತ portc, ರಚನೆ symb_s*portv,
			  ಅಕ್ಷರ*scope_label, ಅಕ್ಷರ*trigger_label);
ಬಾಹ್ಯ ಶೂನ್ಯ compile_ufunc_vec4(ಅಕ್ಷರ*label, ಅಕ್ಷರ*ಸಂಕೇತ_2, ಚಿಹ್ನೆರಹಿತ wid,
			  ಚಿಹ್ನೆರಹಿತ argc, ರಚನೆ symb_s*argv,
			  ಚಿಹ್ನೆರಹಿತ portc, ರಚನೆ symb_s*portv,
			  ಅಕ್ಷರ*scope_label, ಅಕ್ಷರ*trigger_label);

/*
 * The compile_event function takes the parts of the event statement
 * and makes the various objects needed to simulate it. This includes
 * the functor that receives the signals and the event_t that holds
 * the threads.
 */
ಬಾಹ್ಯ ಶೂನ್ಯ compile_event(ಅಕ್ಷರ*label, ಅಕ್ಷರ*type,
			  ಚಿಹ್ನೆರಹಿತ argc, ರಚನೆ symb_s*argv);
ಬಾಹ್ಯ ಶೂನ್ಯ compile_named_event(ಅಕ್ಷರ*label, ಅಕ್ಷರ*type, bool local_flag=false);


/*
 * A code statement is a label, an opcode and up to 3 operands. There
 * are a few lexical types that the parser recognizes of the operands,
 * given by the ltype_e enumeration. The compile_code function takes
 * the label, mnemonic and parsed operands and writes a properly
 * formed instruction into the code space. The label is set into the
 * symbol table with the address of the instruction.
 */

#ವ್ಯಾಖ್ಯಾನಿಸು ಕಾರ್ಯಾಂಶ_MAX 3
ಎಣಿಕೆ ltype_e { L_NUMB, L_SYMB, L_STRING };

ರಚನೆ comp_operands_s {
      ಚಿಹ್ನೆರಹಿತ argc;
      ರಚನೆ {
	    ಎಣಿಕೆ ltype_e ltype;
	    ಒಕ್ಕೂಟ {
		  ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ numb;
		  ರಚನೆ symb_s symb;
		  ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ   *ಪಠ್ಯ;
	    };
      } argv[ಕಾರ್ಯಾಂಶ_MAX];
};

ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ರಚನೆ comp_operands_s*comp_operands_t;

ಬಾಹ್ಯ ಶೂನ್ಯ compile_ಸಂಕೇತ(ಅಕ್ಷರ*label, ಅಕ್ಷರ*mnem, comp_operands_t opa);

ಬಾಹ್ಯ ಶೂನ್ಯ compile_file_ಸಾಲು(ಅಕ್ಷರ*label, ದೀರ್ಘ file_idx, ದೀರ್ಘ lineno,
                              ಅಕ್ಷರ*description);

ಬಾಹ್ಯ ಶೂನ್ಯ compile_vpi_call(ಅಕ್ಷರ*label, ಅಕ್ಷರ*name,
			     bool func_as_task_err, bool func_as_task_warn,
			     ದೀರ್ಘ file_idx, ದೀರ್ಘ lineno,
			     ಚಿಹ್ನೆರಹಿತ argc, vpiHandle*argv,
			     ಚಿಹ್ನೆರಹಿತ vec4_ರಾಶಿ,
			     ಚಿಹ್ನೆರಹಿತ real_ರಾಶಿ,
			     ಚಿಹ್ನೆರಹಿತ string_ರಾಶಿ);

/* Compile a function call. The vbit and vwid encode the return
   type. If the vwid >0, the return type is a vector. If the vwid is
   <0, the return type is -vpiRealConst or some other constant subtype
   code that represents the function type. */
ಬಾಹ್ಯ ಶೂನ್ಯ compile_vpi_func_call(ಅಕ್ಷರ*label, ಅಕ್ಷರ*name,
				  ಪೂರ್ಣಾಂಕ val_type, ಚಿಹ್ನೆರಹಿತ val_wid,
				  ದೀರ್ಘ file_idx, ದೀರ್ಘ lineno,
				  ಚಿಹ್ನೆರಹಿತ argc, vpiHandle*argv,
				  ಚಿಹ್ನೆರಹಿತ vec4_ರಾಶಿ,
				  ಚಿಹ್ನೆರಹಿತ real_ರಾಶಿ,
				  ಚಿಹ್ನೆರಹಿತ string_ರಾಶಿ);
ಬಾಹ್ಯ ಶೂನ್ಯ print_vpi_call_errors();

ಬಾಹ್ಯ ಶೂನ್ಯ compile_codelabel(ಅಕ್ಷರ*label);

/*
 * The parser uses these functions to compile .scope statements.
 * The implementations of these live in the vpi_scope.cc file.
 */
ಬಾಹ್ಯ ಶೂನ್ಯ compile_scope_decl(ಅಕ್ಷರ*typ, ಅಕ್ಷರ*lab, ಅಕ್ಷರ*nam, ಅಕ್ಷರ*tnam,
                               ಅಕ್ಷರ*par, ದೀರ್ಘ file_idx, ದೀರ್ಘ lineno,
                               ದೀರ್ಘ def_file_idx, ದೀರ್ಘ def_lineno,
                               ದೀರ್ಘ is_cell);
ಬಾಹ್ಯ ಶೂನ್ಯ compile_scope_recall(ಅಕ್ಷರ*sym);

/*
 * The parser uses this function to declare a thread. The start_sym is
 * the start instruction, and must already be defined.
 */
ಬಾಹ್ಯ ಶೂನ್ಯ compile_thread(ಅಕ್ಷರ*ಆರಂಭ_sym, ಅಕ್ಷರ*flag);

/*
 * This function is called to create a var vector with the given name.
 *
 * The vpi_type_code argument of compile_variable() is one of the vpi
 * object codes that identify the type: vpiReg, vpiIntegerVar,
 * vpiIntVar, etc.
 */
ಬಾಹ್ಯ ಶೂನ್ಯ compile_variable(ಅಕ್ಷರ*label, ಅಕ್ಷರ*name,
			     ಪೂರ್ಣಾಂಕ msb, ಪೂರ್ಣಾಂಕ lsb, ಪೂರ್ಣಾಂಕ vpi_type_ಸಂಕೇತ,
			     bool signed_flag, bool local_flag);

ಬಾಹ್ಯ ಶೂನ್ಯ compile_var_real(ಅಕ್ಷರ*label, ಅಕ್ಷರ*name, bool local_flag);
ಬಾಹ್ಯ ಶೂನ್ಯ compile_var_string(ಅಕ್ಷರ*label, ಅಕ್ಷರ*name);
ಬಾಹ್ಯ ಶೂನ್ಯ compile_var_darray(ಅಕ್ಷರ*label, ಅಕ್ಷರ*name, ಚಿಹ್ನೆರಹಿತ ಗಾತ್ರ_2);
ಬಾಹ್ಯ ಶೂನ್ಯ compile_var_cobject(ಅಕ್ಷರ*label, ಅಕ್ಷರ*name);
ಬಾಹ್ಯ ಶೂನ್ಯ compile_var_ಸರತಿ(ಅಕ್ಷರ*label, ಅಕ್ಷರ*name, ಚಿಹ್ನೆರಹಿತ ಗಾತ್ರ_2);

/*
 * This function is used to create a scope port
 * Current ONLY module ports are supported and ports exist purely
 * as meta-data for VPI queries (i.e. there is NO corresponding net etc)
 * as elaboration internally eliminates port-nets by directly connecting
 * nets connected through module ports.
 */

ಬಾಹ್ಯ ಶೂನ್ಯ compile_ಪೋರ್ಟ್_info( ಚಿಹ್ನೆರಹಿತ ಸೂಚ್ಯಂಕ, ಪೂರ್ಣಾಂಕ vpi_ಪೋರ್ಟ್_type, ಚಿಹ್ನೆರಹಿತ ಅಗಲ, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *name, ಅಕ್ಷರ* ಬಫರ್ );


/*
 * The compile_net function is called to create a .net vector with a
 * given name.
 *
 * The vpi_type_code argument of compile_net() is one of the vpi
 * object codes for the equivalent variable types. The supported codes
 * are:
 *   vpiLogic  -- 4-value logic
 *   vpiIntVar -- 2-value logic
 *  -vpiLogic  -- 8-value (i.e. strength aware) logic
 */
ಬಾಹ್ಯ ಶೂನ್ಯ compile_net(ಅಕ್ಷರ*label, ಅಕ್ಷರ*name, ಪೂರ್ಣಾಂಕ msb, ಪೂರ್ಣಾಂಕ lsb,
			ಪೂರ್ಣಾಂಕ vpi_type_ಸಂಕೇತ, bool signed_flag, bool local_flag,
			ಚಿಹ್ನೆರಹಿತ argc, ರಚನೆ symb_s*argv);

ಬಾಹ್ಯ ಶೂನ್ಯ compile_net_real(ಅಕ್ಷರ*label, ಅಕ್ಷರ*name,
			     ಪೂರ್ಣಾಂಕ msb, ಪೂರ್ಣಾಂಕ lsb, bool local_flag,
			     ಚಿಹ್ನೆರಹಿತ argc, ರಚನೆ symb_s*argv);

ಬಾಹ್ಯ ಶೂನ್ಯ compile_netw(ಅಕ್ಷರ*label, ಅಕ್ಷರ*array_label, ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ array_addr,
			 ಪೂರ್ಣಾಂಕ msb, ಪೂರ್ಣಾಂಕ lsb, ಪೂರ್ಣಾಂಕ vpi_type_ಸಂಕೇತ, bool signed_flag,
			 ಚಿಹ್ನೆರಹಿತ argc, ರಚನೆ symb_s*argv);

ಬಾಹ್ಯ ಶೂನ್ಯ compile_netw_real(ಅಕ್ಷರ*label, ಅಕ್ಷರ*array_ಚಿಹ್ನೆ,
			      ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ array_addr,
			      ಪೂರ್ಣಾಂಕ msb, ಪೂರ್ಣಾಂಕ lsb,
			      ಚಿಹ್ನೆರಹಿತ argc, ರಚನೆ symb_s*argv);


ಬಾಹ್ಯ ಶೂನ್ಯ compile_island(ಅಕ್ಷರ*label, ಅಕ್ಷರ*type);
ಬಾಹ್ಯ ಶೂನ್ಯ compile_island_ಪೋರ್ಟ್(ಅಕ್ಷರ*label, ಅಕ್ಷರ*island, ಅಕ್ಷರ*src);
ಬಾಹ್ಯ ಶೂನ್ಯ compile_island_import(ಅಕ್ಷರ*label, ಅಕ್ಷರ*island, ಅಕ್ಷರ*src);
ಬಾಹ್ಯ ಶೂನ್ಯ compile_island_export(ಅಕ್ಷರ*label, ಅಕ್ಷರ*island);
ಬಾಹ್ಯ ಶೂನ್ಯ compile_island_cleanup(ಶೂನ್ಯ);

ಬಾಹ್ಯ ಶೂನ್ಯ compile_island_tran(ಅಕ್ಷರ*label);
ಬಾಹ್ಯ ಶೂನ್ಯ compile_island_tranif(ಪೂರ್ಣಾಂಕ sense, ಅಕ್ಷರ*island,
				  ಅಕ್ಷರ*ba, ಅಕ್ಷರ*bb, ಅಕ್ಷರ*src, bool resistive);
ಬಾಹ್ಯ ಶೂನ್ಯ compile_island_tranvp(ಅಕ್ಷರ*island, ಅಕ್ಷರ*ba, ಅಕ್ಷರ*bb,
				  ಚಿಹ್ನೆರಹಿತ ಅಗಲ, ಚಿಹ್ನೆರಹಿತ part, ಚಿಹ್ನೆರಹಿತ off);

ಬಾಹ್ಯ ಶೂನ್ಯ delete_udp_symbols(ಶೂನ್ಯ);

ಬಾಹ್ಯ ಶೂನ್ಯ compile_class_ಆರಂಭ(ಅಕ್ಷರ*lab, ಅಕ್ಷರ*nam, ಚಿಹ್ನೆರಹಿತ nprop);
ಬಾಹ್ಯ ಶೂನ್ಯ compile_class_property(ಚಿಹ್ನೆರಹಿತ idx, ಅಕ್ಷರ*nam, ಅಕ್ಷರ*typ, uint64_t array_ಗಾತ್ರ_2);
ಬಾಹ್ಯ ಶೂನ್ಯ compile_class_ಮುಗಿದ(ಶೂನ್ಯ);

#ಆದರೆ_ಅಂತ್ಯ /* IVL_compile_H */
