#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ IVL_compile_H
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ IVL_compile_H
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
ਬਾਹਰੀ std::ਸਦਿਸ਼<ਅਚਲ ਅੱਖਰ*> file_ਨਾਂ;

/*
 * The functions described here are the compile time support
 * functions. Various bits of the compile process are taken care of
 * here. What is called when is mostly controlled by the parser.
 *
 * Before compilation takes place, the compile_init function must be
 * called once to set stuff up.
 */

ਬਾਹਰੀ ਖਾਲੀ compile_init(ਖਾਲੀ);

ਬਾਹਰੀ ਖਾਲੀ compile_cleanup(ਖਾਲੀ);

ਬਾਹਰੀ bool verbose_flag;

/*
 * If this file opened, then write debug information to this
 * file. This is used for debugging the VVP runtime itself.
 */
ਬਾਹਰੀ std::ofstream debug_file;

/*
 *  Connect a list of symbols to a contiguous set of ipoints.
 *  Constants C<?> are handled by setting the ival of the ipoint.
 */
ਬਾਹਰੀ ਖਾਲੀ inputs_connect(vvp_net_t*fdx, ਬਿਨਾਨਿਸ਼ਾਨੀ argc, ਬਣਤਰ symb_s*argv);
ਬਾਹਰੀ ਖਾਲੀ ਨਿਵੇਸ਼_connect(vvp_net_t*fdx, ਬਿਨਾਨਿਸ਼ਾਨੀ ਪੋਰਟ, ਅੱਖਰ*label);

/*
 * This function is an expansion of the inputs_connect function. It
 * uses the inputs_connect function, but it creates vvp_wide_fun_t
 * nodes to handle arbitrary width nodes, and connects those nodes to
 * the vvp_wide_fun_core object passed in.
 */
ਬਾਹਰੀ ਖਾਲੀ wide_inputs_connect(vvp_wide_fun_core*core,
				ਬਿਨਾਨਿਸ਼ਾਨੀ argc, ਬਣਤਰ symb_s*argv);

ਬਾਹਰੀ vvp_net_t* vvp_net_lookup(ਅਚਲ ਅੱਖਰ*label);
ਬਾਹਰੀ vpiHandle vvp_lookup_handle(ਅਚਲ ਅੱਖਰ*label);

/*
 *  Add a functor to the symbol table
 */
ਬਾਹਰੀ ਖਾਲੀ define_functor_ਚਿੰਨ੍ਹ(ਅਚਲ ਅੱਖਰ*label, vvp_net_t*ipt);


/*
 * This is a count of errors encountered during compilation. If this
 * is non-zero, then simulation is not recommended.
 */
ਬਾਹਰੀ ਬਿਨਾਨਿਸ਼ਾਨੀ compile_errors;

ਬਾਹਰੀ ਅਚਲ ਅੱਖਰ* module_ਰਸਤਾ;
ਬਾਹਰੀ ਖਾਲੀ compile_ਲੱਦੋ_vpi_module(ਅੱਖਰ*name);

ਬਾਹਰੀ ਖਾਲੀ compile_vpi_ਸਮਾਂ_precision(ਲੰਮਾ pre);

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
ਬਾਹਰੀ ਖਾਲੀ compile_functor(ਅੱਖਰ*label, ਅੱਖਰ*type, ਬਿਨਾਨਿਸ਼ਾਨੀ ਚੌੜਾਈ,
			    ਬਿਨਾਨਿਸ਼ਾਨੀ ostr0, ਬਿਨਾਨਿਸ਼ਾਨੀ ostr1,
			    ਬਿਨਾਨਿਸ਼ਾਨੀ argc, ਬਣਤਰ symb_s*argv);


/*
 * This is called by the parser to make a resolver. This is a special
 * kind of functor; a strength aware functor. It has up to 4 inputs
 * that are blended to make a resolved output. The type string selects
 * a resolution algorithm.
 */
ਬਾਹਰੀ ਖਾਲੀ compile_resolver(ਅੱਖਰ*label, ਅੱਖਰ*type,
			     ਬਿਨਾਨਿਸ਼ਾਨੀ argc, ਬਣਤਰ symb_s*argv);

ਬਾਹਰੀ ਖਾਲੀ compile_concat(ਅੱਖਰ*label, ਬਿਨਾਨਿਸ਼ਾਨੀ w0, ਬਿਨਾਨਿਸ਼ਾਨੀ w1,
			   ਬਿਨਾਨਿਸ਼ਾਨੀ w2, ਬਿਨਾਨਿਸ਼ਾਨੀ w3,
			   ਬਿਨਾਨਿਸ਼ਾਨੀ argc, ਬਣਤਰ symb_s*argv);

ਬਾਹਰੀ ਖਾਲੀ compile_concat8(ਅੱਖਰ*label, ਬਿਨਾਨਿਸ਼ਾਨੀ w0, ਬਿਨਾਨਿਸ਼ਾਨੀ w1,
			    ਬਿਨਾਨਿਸ਼ਾਨੀ w2, ਬਿਨਾਨਿਸ਼ਾਨੀ w3,
			    ਬਿਨਾਨਿਸ਼ਾਨੀ argc, ਬਣਤਰ symb_s*argv);

ਬਾਹਰੀ ਖਾਲੀ compile_substitute(ਅੱਖਰ*label, ਬਿਨਾਨਿਸ਼ਾਨੀ ਚੌੜਾਈ,
			       ਬਿਨਾਨਿਸ਼ਾਨੀ soff, ਬਿਨਾਨਿਸ਼ਾਨੀ swidth,
			       ਬਿਨਾਨਿਸ਼ਾਨੀ argc, ਬਣਤਰ symb_s*argv);

/*
 * Arrange for the system task/function call to have its compiletf
 * function called.
 */
ਬਾਹਰੀ ਖਾਲੀ compile_compiletf(ਬਣਤਰ __vpiSysTaskCall*);

/*
 * Compile delay nodes of various form.
 */
ਬਾਹਰੀ ਖਾਲੀ compile_delay(ਅੱਖਰ*label, ਬਿਨਾਨਿਸ਼ਾਨੀ ਚੌੜਾਈ,
                          vvp_delay_t*del, ਬਣਤਰ symb_s ਨਿਵੇਸ਼);
ਬਾਹਰੀ ਖਾਲੀ compile_delay(ਅੱਖਰ*label, ਬਿਨਾਨਿਸ਼ਾਨੀ ਚੌੜਾਈ,
                          ਬਿਨਾਨਿਸ਼ਾਨੀ argc, ਬਣਤਰ symb_s*argv,
                          bool ignore_decay);

/*
 * This is called by the parser to create a part select node.
 * See the PART SELECT STATEMENT section in the README.txt
 */
ਬਾਹਰੀ ਖਾਲੀ compile_part_select(ਅੱਖਰ*label, ਅੱਖਰ*src,
				ਬਿਨਾਨਿਸ਼ਾਨੀ ਅਧਾਰ, ਬਿਨਾਨਿਸ਼ਾਨੀ wid);
ਬਾਹਰੀ ਖਾਲੀ compile_part_select_pv(ਅੱਖਰ*label, ਅੱਖਰ*src,
				   ਬਿਨਾਨਿਸ਼ਾਨੀ ਅਧਾਰ, ਬਿਨਾਨਿਸ਼ਾਨੀ wid,
				   ਬਿਨਾਨਿਸ਼ਾਨੀ vec_wid);
ਬਾਹਰੀ ਖਾਲੀ compile_part_select_var(ਅੱਖਰ*label, ਅੱਖਰ*src,
                                    ਅੱਖਰ*var, ਬਿਨਾਨਿਸ਼ਾਨੀ wid, bool is_signed);

/*
 * This is called by the parser to make the various arithmetic and
 * comparison functors.
 */
ਬਾਹਰੀ ਖਾਲੀ compile_arith_pow(ਅੱਖਰ*label, ਲੰਮਾ ਚੌੜਾਈ, bool signed_flag,
			      ਬਿਨਾਨਿਸ਼ਾਨੀ argc, ਬਣਤਰ symb_s*argv);
ਬਾਹਰੀ ਖਾਲੀ compile_arith_abs(ਅੱਖਰ*label,
			      ਬਿਨਾਨਿਸ਼ਾਨੀ argc, ਬਣਤਰ symb_s*argv);
ਬਾਹਰੀ ਖਾਲੀ compile_arith_cast_int(ਅੱਖਰ*label, ਲੰਮਾ ਚੌੜਾਈ,
			           ਬਿਨਾਨਿਸ਼ਾਨੀ argc, ਬਣਤਰ symb_s*argv);
ਬਾਹਰੀ ਖਾਲੀ compile_arith_cast_real(ਅੱਖਰ*label, bool signed_flag,
			            ਬਿਨਾਨਿਸ਼ਾਨੀ argc, ਬਣਤਰ symb_s*argv);
ਬਾਹਰੀ ਖਾਲੀ compile_arith_cast_vec2(ਅੱਖਰ*label, ਲੰਮਾ ਚੌੜਾਈ,
				    ਬਿਨਾਨਿਸ਼ਾਨੀ argc, ਬਣਤਰ symb_s*argv);
ਬਾਹਰੀ ਖਾਲੀ compile_arith_div(ਅੱਖਰ*label, ਲੰਮਾ ਚੌੜਾਈ, bool signed_flag,
			      ਬਿਨਾਨਿਸ਼ਾਨੀ argc, ਬਣਤਰ symb_s*argv);
ਬਾਹਰੀ ਖਾਲੀ compile_arith_mod(ਅੱਖਰ*label, ਲੰਮਾ ਚੌੜਾਈ, bool signed_flag,
			      ਬਿਨਾਨਿਸ਼ਾਨੀ argc, ਬਣਤਰ symb_s*argv);
ਬਾਹਰੀ ਖਾਲੀ compile_arith_mult(ਅੱਖਰ*label, ਲੰਮਾ ਚੌੜਾਈ,
			       ਬਿਨਾਨਿਸ਼ਾਨੀ argc, ਬਣਤਰ symb_s*argv);
ਬਾਹਰੀ ਖਾਲੀ compile_arith_ਜੋੜ(ਅੱਖਰ*label, ਲੰਮਾ ਚੌੜਾਈ,
			      ਬਿਨਾਨਿਸ਼ਾਨੀ argc, ਬਣਤਰ symb_s*argv);
ਬਾਹਰੀ ਖਾਲੀ compile_arith_sub(ਅੱਖਰ*label, ਲੰਮਾ ਚੌੜਾਈ,
			      ਬਿਨਾਨਿਸ਼ਾਨੀ argc, ਬਣਤਰ symb_s*argv);
ਬਾਹਰੀ ਖਾਲੀ compile_cmp_eeq(ਅੱਖਰ*label, ਲੰਮਾ ਚੌੜਾਈ,
			   ਬਿਨਾਨਿਸ਼ਾਨੀ argc, ਬਣਤਰ symb_s*argv);
ਬਾਹਰੀ ਖਾਲੀ compile_cmp_nee(ਅੱਖਰ*label, ਲੰਮਾ ਚੌੜਾਈ,
			   ਬਿਨਾਨਿਸ਼ਾਨੀ argc, ਬਣਤਰ symb_s*argv);
ਬਾਹਰੀ ਖਾਲੀ compile_cmp_eq(ਅੱਖਰ*label, ਲੰਮਾ ਚੌੜਾਈ,
			   ਬਿਨਾਨਿਸ਼ਾਨੀ argc, ਬਣਤਰ symb_s*argv);
ਬਾਹਰੀ ਖਾਲੀ compile_cmp_eqx(ਅੱਖਰ*label, ਲੰਮਾ ਚੌੜਾਈ,
			    ਬਿਨਾਨਿਸ਼ਾਨੀ argc, ਬਣਤਰ symb_s*argv);
ਬਾਹਰੀ ਖਾਲੀ compile_cmp_eqz(ਅੱਖਰ*label, ਲੰਮਾ ਚੌੜਾਈ,
			    ਬਿਨਾਨਿਸ਼ਾਨੀ argc, ਬਣਤਰ symb_s*argv);
ਬਾਹਰੀ ਖਾਲੀ compile_cmp_ne(ਅੱਖਰ*label, ਲੰਮਾ ਚੌੜਾਈ,
			   ਬਿਨਾਨਿਸ਼ਾਨੀ argc, ਬਣਤਰ symb_s*argv);
ਬਾਹਰੀ ਖਾਲੀ compile_cmp_ge(ਅੱਖਰ*label, ਲੰਮਾ ਚੌੜਾਈ, bool signed_flag,
			   ਬਿਨਾਨਿਸ਼ਾਨੀ argc, ਬਣਤਰ symb_s*argv);
ਬਾਹਰੀ ਖਾਲੀ compile_cmp_gt(ਅੱਖਰ*label, ਲੰਮਾ ਚੌੜਾਈ, bool signed_flag,
			   ਬਿਨਾਨਿਸ਼ਾਨੀ argc, ਬਣਤਰ symb_s*argv);
ਬਾਹਰੀ ਖਾਲੀ compile_cmp_weq(ਅੱਖਰ*label, ਲੰਮਾ ਚੌੜਾਈ,
			   ਬਿਨਾਨਿਸ਼ਾਨੀ argc, ਬਣਤਰ symb_s*argv);
ਬਾਹਰੀ ਖਾਲੀ compile_cmp_wne(ਅੱਖਰ*label, ਲੰਮਾ ਚੌੜਾਈ,
			   ਬਿਨਾਨਿਸ਼ਾਨੀ argc, ਬਣਤਰ symb_s*argv);

ਬਾਹਰੀ ਖਾਲੀ compile_arith_mult_r(ਅੱਖਰ*label, ਬਿਨਾਨਿਸ਼ਾਨੀ argc,
                                 ਬਣਤਰ symb_s*argv);
ਬਾਹਰੀ ਖਾਲੀ compile_arith_pow_r(ਅੱਖਰ*label, ਬਿਨਾਨਿਸ਼ਾਨੀ argc, ਬਣਤਰ symb_s*argv);
ਬਾਹਰੀ ਖਾਲੀ compile_arith_div_r(ਅੱਖਰ*label, ਬਿਨਾਨਿਸ਼ਾਨੀ argc, ਬਣਤਰ symb_s*argv);
ਬਾਹਰੀ ਖਾਲੀ compile_arith_mod_r(ਅੱਖਰ*label, ਬਿਨਾਨਿਸ਼ਾਨੀ argc, ਬਣਤਰ symb_s*argv);
ਬਾਹਰੀ ਖਾਲੀ compile_arith_ਜੋੜ_r(ਅੱਖਰ*label, ਬਿਨਾਨਿਸ਼ਾਨੀ argc, ਬਣਤਰ symb_s*argv);
ਬਾਹਰੀ ਖਾਲੀ compile_arith_sub_r(ਅੱਖਰ*label, ਬਿਨਾਨਿਸ਼ਾਨੀ argc, ਬਣਤਰ symb_s*argv);
ਬਾਹਰੀ ਖਾਲੀ compile_cmp_eq_r(ਅੱਖਰ*label, ਬਿਨਾਨਿਸ਼ਾਨੀ argc, ਬਣਤਰ symb_s*argv);
ਬਾਹਰੀ ਖਾਲੀ compile_cmp_ne_r(ਅੱਖਰ*label, ਬਿਨਾਨਿਸ਼ਾਨੀ argc, ਬਣਤਰ symb_s*argv);
ਬਾਹਰੀ ਖਾਲੀ compile_cmp_ge_r(ਅੱਖਰ*label, ਬਿਨਾਨਿਸ਼ਾਨੀ argc, ਬਣਤਰ symb_s*argv);
ਬਾਹਰੀ ਖਾਲੀ compile_cmp_gt_r(ਅੱਖਰ*label, ਬਿਨਾਨਿਸ਼ਾਨੀ argc, ਬਣਤਰ symb_s*argv);

ਬਾਹਰੀ ਖਾਲੀ compile_dff(ਅੱਖਰ*label, ਬਿਨਾਨਿਸ਼ਾਨੀ ਚੌੜਾਈ, bool negedge,
			ਬਣਤਰ symb_s arg_d,
			ਬਣਤਰ symb_s arg_c,
			ਬਣਤਰ symb_s arg_e);

ਬਾਹਰੀ ਖਾਲੀ compile_dff_aclr(ਅੱਖਰ*label, ਬਿਨਾਨਿਸ਼ਾਨੀ ਚੌੜਾਈ, bool negedge,
			     ਬਣਤਰ symb_s arg_d,
			     ਬਣਤਰ symb_s arg_c,
			     ਬਣਤਰ symb_s arg_e,
			     ਬਣਤਰ symb_s arg_a);
ਬਾਹਰੀ ਖਾਲੀ compile_dff_aset(ਅੱਖਰ*label, ਬਿਨਾਨਿਸ਼ਾਨੀ ਚੌੜਾਈ, bool negedge,
			     ਬਣਤਰ symb_s arg_d,
			     ਬਣਤਰ symb_s arg_c,
			     ਬਣਤਰ symb_s arg_e,
			     ਬਣਤਰ symb_s arg_a,
			     ਅੱਖਰ*asc_ਮੁੱਲ);

ਬਾਹਰੀ ਖਾਲੀ compile_latch(ਅੱਖਰ*label, ਬਿਨਾਨਿਸ਼ਾਨੀ ਚੌੜਾਈ,
			  ਬਣਤਰ symb_s arg_d,
			  ਬਣਤਰ symb_s arg_e);

ਬਾਹਰੀ ਖਾਲੀ compile_enum2_type(ਅੱਖਰ*label, ਲੰਮਾ ਚੌੜਾਈ, bool signed_flag,
			      std::list<ਬਣਤਰ enum_name_s>*ਨਾਂ);
ਬਾਹਰੀ ਖਾਲੀ compile_enum4_type(ਅੱਖਰ*label, ਲੰਮਾ ਚੌੜਾਈ, bool signed_flag,
			      std::list<ਬਣਤਰ enum_name_s>*ਨਾਂ);

ਬਣਤਰ __vpiModPath;
ਬਾਹਰੀ __vpiModPath* compile_modpath(ਅੱਖਰ*label,
                                     ਬਿਨਾਨਿਸ਼ਾਨੀ ਚੌੜਾਈ,
				     ਬਣਤਰ symb_s drv,
				     ਬਣਤਰ symb_s dest);
ਬਾਹਰੀ ਖਾਲੀ compile_modpath_src(__vpiModPath*dst,
				ਅੱਖਰ ਕਿਨਾਰਾ_c,
				ਅਚਲ ਬਣਤਰ symb_s&src,
				ਬਣਤਰ numbv_s&vals,
				ਅਚਲ ਬਣਤਰ symb_s&condit_src,
				ਅਚਲ ਬਣਤਰ symb_s&ਰਸਤਾ_ਪਦ_ਵਿੱਚ);
ਬਾਹਰੀ ਖਾਲੀ compile_modpath_src(__vpiModPath*dst,
				ਅੱਖਰ ਕਿਨਾਰਾ_c,
				ਅਚਲ ਬਣਤਰ symb_s&src,
				ਬਣਤਰ numbv_s&vals,
				ਪੂਰਨ condit_src, /* match with '0' */
				ਅਚਲ ਬਣਤਰ symb_s&ਰਸਤਾ_ਪਦ_ਵਿੱਚ,
				bool ifnone);

ਬਾਹਰੀ ਖਾਲੀ compile_reduce_and(ਅੱਖਰ*label, ਅਚਲ ਬਣਤਰ symb_s&arg);
ਬਾਹਰੀ ਖਾਲੀ compile_reduce_or(ਅੱਖਰ*label, ਅਚਲ ਬਣਤਰ symb_s&arg);
ਬਾਹਰੀ ਖਾਲੀ compile_reduce_xor(ਅੱਖਰ*label, ਅਚਲ ਬਣਤਰ symb_s&arg);
ਬਾਹਰੀ ਖਾਲੀ compile_reduce_nand(ਅੱਖਰ*label, ਅਚਲ ਬਣਤਰ symb_s&arg);
ਬਾਹਰੀ ਖਾਲੀ compile_reduce_nor(ਅੱਖਰ*label, ਅਚਲ ਬਣਤਰ symb_s&arg);
ਬਾਹਰੀ ਖਾਲੀ compile_reduce_xnor(ਅੱਖਰ*label, ਅਚਲ ਬਣਤਰ symb_s&arg);

ਬਾਹਰੀ ਖਾਲੀ compile_extend_signed(ਅੱਖਰ*label, ਲੰਮਾ ਚੌੜਾਈ, ਬਣਤਰ symb_s arg);

ਬਾਹਰੀ ਖਾਲੀ compile_sfunc(ਅੱਖਰ*label, ਅੱਖਰ*name, ਅੱਖਰ*format_string,
			  ਲੰਮਾ file_idx, ਲੰਮਾ lineno,
			  ਬਿਨਾਨਿਸ਼ਾਨੀ argc, ਬਣਤਰ symb_s*argv,
                          ਅੱਖਰ*trigger_label);

ਬਾਹਰੀ ਖਾਲੀ compile_repeat(ਅੱਖਰ*label, ਲੰਮਾ ਚੌੜਾਈ, ਲੰਮਾ repeat,
			   ਬਣਤਰ symb_s arg);

ਬਾਹਰੀ ਖਾਲੀ compile_shiftl(ਅੱਖਰ*label, ਲੰਮਾ ਚੌੜਾਈ,
			   ਬਿਨਾਨਿਸ਼ਾਨੀ argc, ਬਣਤਰ symb_s*argv);
ਬਾਹਰੀ ਖਾਲੀ compile_shiftr(ਅੱਖਰ*label, ਲੰਮਾ ਚੌੜਾਈ, bool signed_flag,
			   ਬਿਨਾਨਿਸ਼ਾਨੀ argc, ਬਣਤਰ symb_s*argv);

ਬਾਹਰੀ ਖਾਲੀ compile_timescale(ਲੰਮਾ units, ਲੰਮਾ precision);

ਬਾਹਰੀ ਖਾਲੀ compile_vpi_ਚਿੰਨ੍ਹ(ਅਚਲ ਅੱਖਰ*label, vpiHandle obj);
ਬਾਹਰੀ ਖਾਲੀ compile_vpi_lookup(vpiHandle *objref, ਅੱਖਰ*label);

ਬਾਹਰੀ ਖਾਲੀ compile_param_string(ਅੱਖਰ*label, ਅਚਲ ਅੱਖਰ*name, ਅੱਖਰ*ਮੁੱਲ,
                                 bool local_flag,
                                 ਲੰਮਾ file_idx, ਲੰਮਾ lineno);
ਬਾਹਰੀ ਖਾਲੀ compile_param_logic(ਅੱਖਰ*label, ਅਚਲ ਅੱਖਰ*name, ਅੱਖਰ*ਮੁੱਲ,
				bool signed_flag, bool local_flag,
                                ਲੰਮਾ file_idx, ਲੰਮਾ lineno);
ਬਾਹਰੀ ਖਾਲੀ compile_param_real(ਅੱਖਰ*label, ਅਚਲ ਅੱਖਰ*name, ਅੱਖਰ*ਮੁੱਲ,
                               bool local_flag,
                               ਲੰਮਾ file_idx, ਲੰਮਾ lineno);

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

    ਜਨਤਕ:
      explicit resolv_list_s(ਅੱਖਰ*lab) : label_(lab) {
	    ਅਗਲਾ_2 = ਨਿਰਾ;
      }
      ਆਭਾਸੀ ~resolv_list_s();

      resolv_list_s(ਅਚਲ resolv_list_s&) = delete;
      resolv_list_s& ਸੰਚਾਲਕ=(ਅਚਲ resolv_list_s&) = delete;

      ਆਭਾਸੀ bool resolve(bool mes = false) = 0;

    ਸੁਰੱਖਿਅਤ:
      ਅਚਲ ਅੱਖਰ*label() ਅਚਲ { ਵਾਪਸ label_; }

    ਨਿਜੀ:
      ਮਿੱਤਰ ਖਾਲੀ resolv_submit(class resolv_list_s*cur);
      ਮਿੱਤਰ ਖਾਲੀ compile_cleanup(ਖਾਲੀ);

      ਅੱਖਰ*label_;
      class resolv_list_s*ਅਗਲਾ_2;
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
ਬਾਹਰੀ ਖਾਲੀ functor_ref_lookup(vvp_net_t**ref, ਅੱਖਰ*lab);

ਬਾਹਰੀ vpiHandle vpip_make_prop_ਕਤਾਰ_ref(ਅੱਖਰ* class_label,
                                          ਬਿਨਾਨਿਸ਼ਾਨੀ prop_idx,
                                          ਬਿਨਾਨਿਸ਼ਾਨੀ is_ਕਤਾਰ_flag);

/*
 * This function schedules a lookup of the labeled instruction. The
 * code points to a code structure that points to the instruction
 * field that receives the result, and the label is the name to
 * lookup. The lookup will free the label text when it is done.
 *
 * The cptr2 flag tells the lookup to write the code pointer into the
 * cptr2 member of the code, instead of the cptr member.
 */
ਬਾਹਰੀ ਖਾਲੀ ਕੋਡ_label_lookup(ਬਣਤਰ vvp_ਕੋਡ_s *ਕੋਡ, ਅੱਖਰ *label, bool cptr2);

/*
 * The `compile_udp_def' function creates a UDP.  The `table' is a
 * NULL terminated array of char*, as assembled by `compile_udp_table'.
 * `compile_udp_table' is called with `table'==NULL to create a new
 * table, or with an existing table to append to.
 *
 * `compile_udp_functor' creates a mode-3 functor referring to the
 * labeled UDP.
 */

ਬਾਹਰੀ ਖਾਲੀ compile_udp_def(ਪੂਰਨ sequ, ਅੱਖਰ*label, ਅੱਖਰ *name,
			    ਬਿਨਾਨਿਸ਼ਾਨੀ nin, ਬਿਨਾਨਿਸ਼ਾਨੀ init, ਅੱਖਰ **ਸਾਰਣੀ);

ਬਾਹਰੀ ਖਾਲੀ compile_udp_functor(ਅੱਖਰ*label, ਅੱਖਰ*type,
				ਬਿਨਾਨਿਸ਼ਾਨੀ argc, ਬਣਤਰ symb_s*argv);

ਬਾਹਰੀ ਅੱਖਰ **compile_udp_ਸਾਰਣੀ(ਅੱਖਰ **ਸਾਰਣੀ, ਅੱਖਰ *ਪੰਕਤੀ);

/*
 * Memory Instances, Ports, and Initialization
 */

ਬਾਹਰੀ ਖਾਲੀ compile_var_array(ਅੱਖਰ*label, ਅੱਖਰ*name,
			      ਪੂਰਨ ਆਖਰੀ, ਪੂਰਨ ਪਹਿਲਾ,
			      ਪੂਰਨ msb, ਪੂਰਨ lsb, ਅੱਖਰ signed_flag);
ਬਾਹਰੀ ਖਾਲੀ compile_var2_array(ਅੱਖਰ*label, ਅੱਖਰ*name,
			      ਪੂਰਨ ਆਖਰੀ, ਪੂਰਨ ਪਹਿਲਾ,
			      ਪੂਰਨ msb, ਪੂਰਨ lsb, bool signed_flag);
ਬਾਹਰੀ ਖਾਲੀ compile_real_array(ਅੱਖਰ*label, ਅੱਖਰ*name,
			       ਪੂਰਨ ਆਖਰੀ, ਪੂਰਨ ਪਹਿਲਾ);
ਬਾਹਰੀ ਖਾਲੀ compile_string_array(ਅੱਖਰ*label, ਅੱਖਰ*name,
				 ਪੂਰਨ ਆਖਰੀ, ਪੂਰਨ ਪਹਿਲਾ);
ਬਾਹਰੀ ਖਾਲੀ compile_object_array(ਅੱਖਰ*label, ਅੱਖਰ*name,
				 ਪੂਰਨ ਆਖਰੀ, ਪੂਰਨ ਪਹਿਲਾ);
ਬਾਹਰੀ ਖਾਲੀ compile_net_array(ਅੱਖਰ*label, ਅੱਖਰ*name,
			      ਪੂਰਨ ਆਖਰੀ, ਪੂਰਨ ਪਹਿਲਾ);
ਬਾਹਰੀ ਖਾਲੀ compile_array_alias(ਅੱਖਰ*label, ਅੱਖਰ*name, ਅੱਖਰ*src);

  /* Index is a net. */
ਬਾਹਰੀ ਖਾਲੀ compile_array_ਪੋਰਟ(ਅੱਖਰ*label, ਅੱਖਰ*name, ਅੱਖਰ*addr);
  /* Index is a constant address */
ਬਾਹਰੀ ਖਾਲੀ compile_array_ਪੋਰਟ(ਅੱਖਰ*label, ਅੱਖਰ*name, ਲੰਮਾ addr);

ਬਾਹਰੀ ਖਾਲੀ compile_array_cleanup(ਖਾਲੀ);

/*
 * Compile the .ufunc statement.
 */
ਬਾਹਰੀ ਖਾਲੀ compile_ufunc_real(ਅੱਖਰ*label, ਅੱਖਰ*ਕੋਡ, ਬਿਨਾਨਿਸ਼ਾਨੀ wid,
			  ਬਿਨਾਨਿਸ਼ਾਨੀ argc, ਬਣਤਰ symb_s*argv,
			  ਬਿਨਾਨਿਸ਼ਾਨੀ portc, ਬਣਤਰ symb_s*portv,
			  ਅੱਖਰ*scope_label, ਅੱਖਰ*trigger_label);
ਬਾਹਰੀ ਖਾਲੀ compile_ufunc_vec4(ਅੱਖਰ*label, ਅੱਖਰ*ਕੋਡ, ਬਿਨਾਨਿਸ਼ਾਨੀ wid,
			  ਬਿਨਾਨਿਸ਼ਾਨੀ argc, ਬਣਤਰ symb_s*argv,
			  ਬਿਨਾਨਿਸ਼ਾਨੀ portc, ਬਣਤਰ symb_s*portv,
			  ਅੱਖਰ*scope_label, ਅੱਖਰ*trigger_label);

/*
 * The compile_event function takes the parts of the event statement
 * and makes the various objects needed to simulate it. This includes
 * the functor that receives the signals and the event_t that holds
 * the threads.
 */
ਬਾਹਰੀ ਖਾਲੀ compile_event(ਅੱਖਰ*label, ਅੱਖਰ*type,
			  ਬਿਨਾਨਿਸ਼ਾਨੀ argc, ਬਣਤਰ symb_s*argv);
ਬਾਹਰੀ ਖਾਲੀ compile_named_event(ਅੱਖਰ*label, ਅੱਖਰ*type, bool local_flag=false);


/*
 * A code statement is a label, an opcode and up to 3 operands. There
 * are a few lexical types that the parser recognizes of the operands,
 * given by the ltype_e enumeration. The compile_code function takes
 * the label, mnemonic and parsed operands and writes a properly
 * formed instruction into the code space. The label is set into the
 * symbol table with the address of the instruction.
 */

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ ਸੰਕਾਰਜ_MAX 3
ਗਿਣਤੀ ltype_e { L_NUMB, L_SYMB, L_STRING };

ਬਣਤਰ comp_operands_s {
      ਬਿਨਾਨਿਸ਼ਾਨੀ argc;
      ਬਣਤਰ {
	    ਗਿਣਤੀ ltype_e ltype;
	    ਸੰਘ {
		  ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ numb;
		  ਬਣਤਰ symb_s symb;
		  ਅਚਲ ਅੱਖਰ   *ਪਾਠ;
	    };
      } argv[ਸੰਕਾਰਜ_MAX];
};

ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਬਣਤਰ comp_operands_s*comp_operands_t;

ਬਾਹਰੀ ਖਾਲੀ compile_ਕੋਡ(ਅੱਖਰ*label, ਅੱਖਰ*mnem, comp_operands_t opa);

ਬਾਹਰੀ ਖਾਲੀ compile_file_ਸਤਰ(ਅੱਖਰ*label, ਲੰਮਾ file_idx, ਲੰਮਾ lineno,
                              ਅੱਖਰ*description);

ਬਾਹਰੀ ਖਾਲੀ compile_vpi_call(ਅੱਖਰ*label, ਅੱਖਰ*name,
			     bool func_as_task_err, bool func_as_task_warn,
			     ਲੰਮਾ file_idx, ਲੰਮਾ lineno,
			     ਬਿਨਾਨਿਸ਼ਾਨੀ argc, vpiHandle*argv,
			     ਬਿਨਾਨਿਸ਼ਾਨੀ vec4_ਢੇਰ,
			     ਬਿਨਾਨਿਸ਼ਾਨੀ real_ਢੇਰ,
			     ਬਿਨਾਨਿਸ਼ਾਨੀ string_ਢੇਰ);

/* Compile a function call. The vbit and vwid encode the return
   type. If the vwid >0, the return type is a vector. If the vwid is
   <0, the return type is -vpiRealConst or some other constant subtype
   code that represents the function type. */
ਬਾਹਰੀ ਖਾਲੀ compile_vpi_func_call(ਅੱਖਰ*label, ਅੱਖਰ*name,
				  ਪੂਰਨ val_type, ਬਿਨਾਨਿਸ਼ਾਨੀ val_wid,
				  ਲੰਮਾ file_idx, ਲੰਮਾ lineno,
				  ਬਿਨਾਨਿਸ਼ਾਨੀ argc, vpiHandle*argv,
				  ਬਿਨਾਨਿਸ਼ਾਨੀ vec4_ਢੇਰ,
				  ਬਿਨਾਨਿਸ਼ਾਨੀ real_ਢੇਰ,
				  ਬਿਨਾਨਿਸ਼ਾਨੀ string_ਢੇਰ);
ਬਾਹਰੀ ਖਾਲੀ print_vpi_call_errors();

ਬਾਹਰੀ ਖਾਲੀ compile_codelabel(ਅੱਖਰ*label);

/*
 * The parser uses these functions to compile .scope statements.
 * The implementations of these live in the vpi_scope.cc file.
 */
ਬਾਹਰੀ ਖਾਲੀ compile_scope_decl(ਅੱਖਰ*typ, ਅੱਖਰ*lab, ਅੱਖਰ*nam, ਅੱਖਰ*tnam,
                               ਅੱਖਰ*par, ਲੰਮਾ file_idx, ਲੰਮਾ lineno,
                               ਲੰਮਾ def_file_idx, ਲੰਮਾ def_lineno,
                               ਲੰਮਾ is_cell);
ਬਾਹਰੀ ਖਾਲੀ compile_scope_recall(ਅੱਖਰ*sym);

/*
 * The parser uses this function to declare a thread. The start_sym is
 * the start instruction, and must already be defined.
 */
ਬਾਹਰੀ ਖਾਲੀ compile_thread(ਅੱਖਰ*ਸ਼ੁਰੂ_sym, ਅੱਖਰ*flag);

/*
 * This function is called to create a var vector with the given name.
 *
 * The vpi_type_code argument of compile_variable() is one of the vpi
 * object codes that identify the type: vpiReg, vpiIntegerVar,
 * vpiIntVar, etc.
 */
ਬਾਹਰੀ ਖਾਲੀ compile_variable(ਅੱਖਰ*label, ਅੱਖਰ*name,
			     ਪੂਰਨ msb, ਪੂਰਨ lsb, ਪੂਰਨ vpi_type_ਕੋਡ,
			     bool signed_flag, bool local_flag);

ਬਾਹਰੀ ਖਾਲੀ compile_var_real(ਅੱਖਰ*label, ਅੱਖਰ*name, bool local_flag);
ਬਾਹਰੀ ਖਾਲੀ compile_var_string(ਅੱਖਰ*label, ਅੱਖਰ*name);
ਬਾਹਰੀ ਖਾਲੀ compile_var_darray(ਅੱਖਰ*label, ਅੱਖਰ*name, ਬਿਨਾਨਿਸ਼ਾਨੀ ਮਾਪ);
ਬਾਹਰੀ ਖਾਲੀ compile_var_cobject(ਅੱਖਰ*label, ਅੱਖਰ*name);
ਬਾਹਰੀ ਖਾਲੀ compile_var_ਕਤਾਰ(ਅੱਖਰ*label, ਅੱਖਰ*name, ਬਿਨਾਨਿਸ਼ਾਨੀ ਮਾਪ);

/*
 * This function is used to create a scope port
 * Current ONLY module ports are supported and ports exist purely
 * as meta-data for VPI queries (i.e. there is NO corresponding net etc)
 * as elaboration internally eliminates port-nets by directly connecting
 * nets connected through module ports.
 */

ਬਾਹਰੀ ਖਾਲੀ compile_ਪੋਰਟ_info( ਬਿਨਾਨਿਸ਼ਾਨੀ ਸੂਚਕਾਂਕ, ਪੂਰਨ vpi_ਪੋਰਟ_type, ਬਿਨਾਨਿਸ਼ਾਨੀ ਚੌੜਾਈ, ਅਚਲ ਅੱਖਰ *name, ਅੱਖਰ* ਬਫਰ );


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
ਬਾਹਰੀ ਖਾਲੀ compile_net(ਅੱਖਰ*label, ਅੱਖਰ*name, ਪੂਰਨ msb, ਪੂਰਨ lsb,
			ਪੂਰਨ vpi_type_ਕੋਡ, bool signed_flag, bool local_flag,
			ਬਿਨਾਨਿਸ਼ਾਨੀ argc, ਬਣਤਰ symb_s*argv);

ਬਾਹਰੀ ਖਾਲੀ compile_net_real(ਅੱਖਰ*label, ਅੱਖਰ*name,
			     ਪੂਰਨ msb, ਪੂਰਨ lsb, bool local_flag,
			     ਬਿਨਾਨਿਸ਼ਾਨੀ argc, ਬਣਤਰ symb_s*argv);

ਬਾਹਰੀ ਖਾਲੀ compile_netw(ਅੱਖਰ*label, ਅੱਖਰ*array_label, ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ array_addr,
			 ਪੂਰਨ msb, ਪੂਰਨ lsb, ਪੂਰਨ vpi_type_ਕੋਡ, bool signed_flag,
			 ਬਿਨਾਨਿਸ਼ਾਨੀ argc, ਬਣਤਰ symb_s*argv);

ਬਾਹਰੀ ਖਾਲੀ compile_netw_real(ਅੱਖਰ*label, ਅੱਖਰ*array_ਚਿੰਨ੍ਹ,
			      ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ array_addr,
			      ਪੂਰਨ msb, ਪੂਰਨ lsb,
			      ਬਿਨਾਨਿਸ਼ਾਨੀ argc, ਬਣਤਰ symb_s*argv);


ਬਾਹਰੀ ਖਾਲੀ compile_island(ਅੱਖਰ*label, ਅੱਖਰ*type);
ਬਾਹਰੀ ਖਾਲੀ compile_island_ਪੋਰਟ(ਅੱਖਰ*label, ਅੱਖਰ*island, ਅੱਖਰ*src);
ਬਾਹਰੀ ਖਾਲੀ compile_island_import(ਅੱਖਰ*label, ਅੱਖਰ*island, ਅੱਖਰ*src);
ਬਾਹਰੀ ਖਾਲੀ compile_island_export(ਅੱਖਰ*label, ਅੱਖਰ*island);
ਬਾਹਰੀ ਖਾਲੀ compile_island_cleanup(ਖਾਲੀ);

ਬਾਹਰੀ ਖਾਲੀ compile_island_tran(ਅੱਖਰ*label);
ਬਾਹਰੀ ਖਾਲੀ compile_island_tranif(ਪੂਰਨ sense, ਅੱਖਰ*island,
				  ਅੱਖਰ*ba, ਅੱਖਰ*bb, ਅੱਖਰ*src, bool resistive);
ਬਾਹਰੀ ਖਾਲੀ compile_island_tranvp(ਅੱਖਰ*island, ਅੱਖਰ*ba, ਅੱਖਰ*bb,
				  ਬਿਨਾਨਿਸ਼ਾਨੀ ਚੌੜਾਈ, ਬਿਨਾਨਿਸ਼ਾਨੀ part, ਬਿਨਾਨਿਸ਼ਾਨੀ off);

ਬਾਹਰੀ ਖਾਲੀ delete_udp_symbols(ਖਾਲੀ);

ਬਾਹਰੀ ਖਾਲੀ compile_class_ਸ਼ੁਰੂ(ਅੱਖਰ*lab, ਅੱਖਰ*nam, ਬਿਨਾਨਿਸ਼ਾਨੀ nprop);
ਬਾਹਰੀ ਖਾਲੀ compile_class_property(ਬਿਨਾਨਿਸ਼ਾਨੀ idx, ਅੱਖਰ*nam, ਅੱਖਰ*typ, uint64_t array_ਮਾਪ_2);
ਬਾਹਰੀ ਖਾਲੀ compile_class_ਪੂਰਾ(ਖਾਲੀ);

#ਜੇ_ਅੰਤ /* IVL_compile_H */
