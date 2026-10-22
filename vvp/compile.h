#ifndef IVL_compile_H
#define IVL_compile_H
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
ಬಾಹ್ಯ std::vector<ಧ್ರುವ ವರ್ಣ*> file_names;

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
ಬಾಹ್ಯ ಶೂನ್ಯ inputs_connect(vvp_net_t*fdx, unsigned argc, ಸಂರಚನಾ symb_s*argv);
ಬಾಹ್ಯ ಶೂನ್ಯ ಒಳಹರಿವು_connect(vvp_net_t*fdx, unsigned port, ವರ್ಣ*label);

/*
 * This function is an expansion of the inputs_connect function. It
 * uses the inputs_connect function, but it creates vvp_wide_fun_t
 * nodes to handle arbitrary width nodes, and connects those nodes to
 * the vvp_wide_fun_core object passed in.
 */
ಬಾಹ್ಯ ಶೂನ್ಯ wide_inputs_connect(vvp_wide_fun_core*core,
				unsigned argc, ಸಂರಚನಾ symb_s*argv);

ಬಾಹ್ಯ vvp_net_t* vvp_net_lookup(ಧ್ರುವ ವರ್ಣ*label);
ಬಾಹ್ಯ vpiHandle vvp_lookup_handle(ಧ್ರುವ ವರ್ಣ*label);

/*
 *  Add a functor to the symbol table
 */
ಬಾಹ್ಯ ಶೂನ್ಯ define_functor_ಚಿಹ್ನೆ(ಧ್ರುವ ವರ್ಣ*label, vvp_net_t*ipt);


/*
 * This is a count of errors encountered during compilation. If this
 * is non-zero, then simulation is not recommended.
 */
ಬಾಹ್ಯ unsigned compile_errors;

ಬಾಹ್ಯ ಧ್ರುವ ವರ್ಣ* module_path;
ಬಾಹ್ಯ ಶೂನ್ಯ compile_load_vpi_module(ವರ್ಣ*name);

ಬಾಹ್ಯ ಶೂನ್ಯ compile_vpi_time_precision(long pre);

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
ಬಾಹ್ಯ ಶೂನ್ಯ compile_functor(ವರ್ಣ*label, ವರ್ಣ*type, unsigned width,
			    unsigned ostr0, unsigned ostr1,
			    unsigned argc, ಸಂರಚನಾ symb_s*argv);


/*
 * This is called by the parser to make a resolver. This is a special
 * kind of functor; a strength aware functor. It has up to 4 inputs
 * that are blended to make a resolved output. The type string selects
 * a resolution algorithm.
 */
ಬಾಹ್ಯ ಶೂನ್ಯ compile_resolver(ವರ್ಣ*label, ವರ್ಣ*type,
			     unsigned argc, ಸಂರಚನಾ symb_s*argv);

ಬಾಹ್ಯ ಶೂನ್ಯ compile_concat(ವರ್ಣ*label, unsigned w0, unsigned w1,
			   unsigned w2, unsigned w3,
			   unsigned argc, ಸಂರಚನಾ symb_s*argv);

ಬಾಹ್ಯ ಶೂನ್ಯ compile_concat8(ವರ್ಣ*label, unsigned w0, unsigned w1,
			    unsigned w2, unsigned w3,
			    unsigned argc, ಸಂರಚನಾ symb_s*argv);

ಬಾಹ್ಯ ಶೂನ್ಯ compile_substitute(ವರ್ಣ*label, unsigned width,
			       unsigned soff, unsigned swidth,
			       unsigned argc, ಸಂರಚನಾ symb_s*argv);

/*
 * Arrange for the system task/function call to have its compiletf
 * function called.
 */
ಬಾಹ್ಯ ಶೂನ್ಯ compile_compiletf(ಸಂರಚನಾ __vpiSysTaskCall*);

/*
 * Compile delay nodes of various form.
 */
ಬಾಹ್ಯ ಶೂನ್ಯ compile_delay(ವರ್ಣ*label, unsigned width,
                          vvp_delay_t*del, ಸಂರಚನಾ symb_s ಒಳಹರಿವು);
ಬಾಹ್ಯ ಶೂನ್ಯ compile_delay(ವರ್ಣ*label, unsigned width,
                          unsigned argc, ಸಂರಚನಾ symb_s*argv,
                          bool ignore_decay);

/*
 * This is called by the parser to create a part select node.
 * See the PART SELECT STATEMENT section in the README.txt
 */
ಬಾಹ್ಯ ಶೂನ್ಯ compile_part_select(ವರ್ಣ*label, ವರ್ಣ*src,
				unsigned base, unsigned wid);
ಬಾಹ್ಯ ಶೂನ್ಯ compile_part_select_pv(ವರ್ಣ*label, ವರ್ಣ*src,
				   unsigned base, unsigned wid,
				   unsigned vec_wid);
ಬಾಹ್ಯ ಶೂನ್ಯ compile_part_select_var(ವರ್ಣ*label, ವರ್ಣ*src,
                                    ವರ್ಣ*var, unsigned wid, bool is_signed);

/*
 * This is called by the parser to make the various arithmetic and
 * comparison functors.
 */
ಬಾಹ್ಯ ಶೂನ್ಯ compile_arith_pow(ವರ್ಣ*label, long width, bool signed_flag,
			      unsigned argc, ಸಂರಚನಾ symb_s*argv);
ಬಾಹ್ಯ ಶೂನ್ಯ compile_arith_abs(ವರ್ಣ*label,
			      unsigned argc, ಸಂರಚನಾ symb_s*argv);
ಬಾಹ್ಯ ಶೂನ್ಯ compile_arith_cast_int(ವರ್ಣ*label, long width,
			           unsigned argc, ಸಂರಚನಾ symb_s*argv);
ಬಾಹ್ಯ ಶೂನ್ಯ compile_arith_cast_real(ವರ್ಣ*label, bool signed_flag,
			            unsigned argc, ಸಂರಚನಾ symb_s*argv);
ಬಾಹ್ಯ ಶೂನ್ಯ compile_arith_cast_vec2(ವರ್ಣ*label, long width,
				    unsigned argc, ಸಂರಚನಾ symb_s*argv);
ಬಾಹ್ಯ ಶೂನ್ಯ compile_arith_div(ವರ್ಣ*label, long width, bool signed_flag,
			      unsigned argc, ಸಂರಚನಾ symb_s*argv);
ಬಾಹ್ಯ ಶೂನ್ಯ compile_arith_mod(ವರ್ಣ*label, long width, bool signed_flag,
			      unsigned argc, ಸಂರಚನಾ symb_s*argv);
ಬಾಹ್ಯ ಶೂನ್ಯ compile_arith_mult(ವರ್ಣ*label, long width,
			       unsigned argc, ಸಂರಚನಾ symb_s*argv);
ಬಾಹ್ಯ ಶೂನ್ಯ compile_arith_sum(ವರ್ಣ*label, long width,
			      unsigned argc, ಸಂರಚನಾ symb_s*argv);
ಬಾಹ್ಯ ಶೂನ್ಯ compile_arith_sub(ವರ್ಣ*label, long width,
			      unsigned argc, ಸಂರಚನಾ symb_s*argv);
ಬಾಹ್ಯ ಶೂನ್ಯ compile_cmp_eeq(ವರ್ಣ*label, long width,
			   unsigned argc, ಸಂರಚನಾ symb_s*argv);
ಬಾಹ್ಯ ಶೂನ್ಯ compile_cmp_nee(ವರ್ಣ*label, long width,
			   unsigned argc, ಸಂರಚನಾ symb_s*argv);
ಬಾಹ್ಯ ಶೂನ್ಯ compile_cmp_eq(ವರ್ಣ*label, long width,
			   unsigned argc, ಸಂರಚನಾ symb_s*argv);
ಬಾಹ್ಯ ಶೂನ್ಯ compile_cmp_eqx(ವರ್ಣ*label, long width,
			    unsigned argc, ಸಂರಚನಾ symb_s*argv);
ಬಾಹ್ಯ ಶೂನ್ಯ compile_cmp_eqz(ವರ್ಣ*label, long width,
			    unsigned argc, ಸಂರಚನಾ symb_s*argv);
ಬಾಹ್ಯ ಶೂನ್ಯ compile_cmp_ne(ವರ್ಣ*label, long width,
			   unsigned argc, ಸಂರಚನಾ symb_s*argv);
ಬಾಹ್ಯ ಶೂನ್ಯ compile_cmp_ge(ವರ್ಣ*label, long width, bool signed_flag,
			   unsigned argc, ಸಂರಚನಾ symb_s*argv);
ಬಾಹ್ಯ ಶೂನ್ಯ compile_cmp_gt(ವರ್ಣ*label, long width, bool signed_flag,
			   unsigned argc, ಸಂರಚನಾ symb_s*argv);
ಬಾಹ್ಯ ಶೂನ್ಯ compile_cmp_weq(ವರ್ಣ*label, long width,
			   unsigned argc, ಸಂರಚನಾ symb_s*argv);
ಬಾಹ್ಯ ಶೂನ್ಯ compile_cmp_wne(ವರ್ಣ*label, long width,
			   unsigned argc, ಸಂರಚನಾ symb_s*argv);

ಬಾಹ್ಯ ಶೂನ್ಯ compile_arith_mult_r(ವರ್ಣ*label, unsigned argc,
                                 ಸಂರಚನಾ symb_s*argv);
ಬಾಹ್ಯ ಶೂನ್ಯ compile_arith_pow_r(ವರ್ಣ*label, unsigned argc, ಸಂರಚನಾ symb_s*argv);
ಬಾಹ್ಯ ಶೂನ್ಯ compile_arith_div_r(ವರ್ಣ*label, unsigned argc, ಸಂರಚನಾ symb_s*argv);
ಬಾಹ್ಯ ಶೂನ್ಯ compile_arith_mod_r(ವರ್ಣ*label, unsigned argc, ಸಂರಚನಾ symb_s*argv);
ಬಾಹ್ಯ ಶೂನ್ಯ compile_arith_sum_r(ವರ್ಣ*label, unsigned argc, ಸಂರಚನಾ symb_s*argv);
ಬಾಹ್ಯ ಶೂನ್ಯ compile_arith_sub_r(ವರ್ಣ*label, unsigned argc, ಸಂರಚನಾ symb_s*argv);
ಬಾಹ್ಯ ಶೂನ್ಯ compile_cmp_eq_r(ವರ್ಣ*label, unsigned argc, ಸಂರಚನಾ symb_s*argv);
ಬಾಹ್ಯ ಶೂನ್ಯ compile_cmp_ne_r(ವರ್ಣ*label, unsigned argc, ಸಂರಚನಾ symb_s*argv);
ಬಾಹ್ಯ ಶೂನ್ಯ compile_cmp_ge_r(ವರ್ಣ*label, unsigned argc, ಸಂರಚನಾ symb_s*argv);
ಬಾಹ್ಯ ಶೂನ್ಯ compile_cmp_gt_r(ವರ್ಣ*label, unsigned argc, ಸಂರಚನಾ symb_s*argv);

ಬಾಹ್ಯ ಶೂನ್ಯ compile_dff(ವರ್ಣ*label, unsigned width, bool negedge,
			ಸಂರಚನಾ symb_s arg_d,
			ಸಂರಚನಾ symb_s arg_c,
			ಸಂರಚನಾ symb_s arg_e);

ಬಾಹ್ಯ ಶೂನ್ಯ compile_dff_aclr(ವರ್ಣ*label, unsigned width, bool negedge,
			     ಸಂರಚನಾ symb_s arg_d,
			     ಸಂರಚನಾ symb_s arg_c,
			     ಸಂರಚನಾ symb_s arg_e,
			     ಸಂರಚನಾ symb_s arg_a);
ಬಾಹ್ಯ ಶೂನ್ಯ compile_dff_aset(ವರ್ಣ*label, unsigned width, bool negedge,
			     ಸಂರಚನಾ symb_s arg_d,
			     ಸಂರಚನಾ symb_s arg_c,
			     ಸಂರಚನಾ symb_s arg_e,
			     ಸಂರಚನಾ symb_s arg_a,
			     ವರ್ಣ*asc_ಮೌಲ್ಯ);

ಬಾಹ್ಯ ಶೂನ್ಯ compile_latch(ವರ್ಣ*label, unsigned width,
			  ಸಂರಚನಾ symb_s arg_d,
			  ಸಂರಚನಾ symb_s arg_e);

ಬಾಹ್ಯ ಶೂನ್ಯ compile_enum2_type(ವರ್ಣ*label, long width, bool signed_flag,
			      std::list<ಸಂರಚನಾ enum_name_s>*names);
ಬಾಹ್ಯ ಶೂನ್ಯ compile_enum4_type(ವರ್ಣ*label, long width, bool signed_flag,
			      std::list<ಸಂರಚನಾ enum_name_s>*names);

ಸಂರಚನಾ __vpiModPath;
ಬಾಹ್ಯ __vpiModPath* compile_modpath(ವರ್ಣ*label,
                                     unsigned width,
				     ಸಂರಚನಾ symb_s drv,
				     ಸಂರಚನಾ symb_s dest);
ಬಾಹ್ಯ ಶೂನ್ಯ compile_modpath_src(__vpiModPath*dst,
				ವರ್ಣ edge_c,
				ಧ್ರುವ ಸಂರಚನಾ symb_s&src,
				ಸಂರಚನಾ numbv_s&vals,
				ಧ್ರುವ ಸಂರಚನಾ symb_s&condit_src,
				ಧ್ರುವ ಸಂರಚನಾ symb_s&path_term_in);
ಬಾಹ್ಯ ಶೂನ್ಯ compile_modpath_src(__vpiModPath*dst,
				ವರ್ಣ edge_c,
				ಧ್ರುವ ಸಂರಚನಾ symb_s&src,
				ಸಂರಚನಾ numbv_s&vals,
				ಪೂರ್ಣಾಙ್ಕ condit_src, /* match with '0' */
				ಧ್ರುವ ಸಂರಚನಾ symb_s&path_term_in,
				bool ifnone);

ಬಾಹ್ಯ ಶೂನ್ಯ compile_reduce_and(ವರ್ಣ*label, ಧ್ರುವ ಸಂರಚನಾ symb_s&arg);
ಬಾಹ್ಯ ಶೂನ್ಯ compile_reduce_or(ವರ್ಣ*label, ಧ್ರುವ ಸಂರಚನಾ symb_s&arg);
ಬಾಹ್ಯ ಶೂನ್ಯ compile_reduce_xor(ವರ್ಣ*label, ಧ್ರುವ ಸಂರಚನಾ symb_s&arg);
ಬಾಹ್ಯ ಶೂನ್ಯ compile_reduce_nand(ವರ್ಣ*label, ಧ್ರುವ ಸಂರಚನಾ symb_s&arg);
ಬಾಹ್ಯ ಶೂನ್ಯ compile_reduce_nor(ವರ್ಣ*label, ಧ್ರುವ ಸಂರಚನಾ symb_s&arg);
ಬಾಹ್ಯ ಶೂನ್ಯ compile_reduce_xnor(ವರ್ಣ*label, ಧ್ರುವ ಸಂರಚನಾ symb_s&arg);

ಬಾಹ್ಯ ಶೂನ್ಯ compile_extend_signed(ವರ್ಣ*label, long width, ಸಂರಚನಾ symb_s arg);

ಬಾಹ್ಯ ಶೂನ್ಯ compile_sfunc(ವರ್ಣ*label, ವರ್ಣ*name, ವರ್ಣ*format_string,
			  long file_idx, long lineno,
			  unsigned argc, ಸಂರಚನಾ symb_s*argv,
                          ವರ್ಣ*trigger_label);

ಬಾಹ್ಯ ಶೂನ್ಯ compile_repeat(ವರ್ಣ*label, long width, long repeat,
			   ಸಂರಚನಾ symb_s arg);

ಬಾಹ್ಯ ಶೂನ್ಯ compile_shiftl(ವರ್ಣ*label, long width,
			   unsigned argc, ಸಂರಚನಾ symb_s*argv);
ಬಾಹ್ಯ ಶೂನ್ಯ compile_shiftr(ವರ್ಣ*label, long width, bool signed_flag,
			   unsigned argc, ಸಂರಚನಾ symb_s*argv);

ಬಾಹ್ಯ ಶೂನ್ಯ compile_timescale(long units, long precision);

ಬಾಹ್ಯ ಶೂನ್ಯ compile_vpi_ಚಿಹ್ನೆ(ಧ್ರುವ ವರ್ಣ*label, vpiHandle obj);
ಬಾಹ್ಯ ಶೂನ್ಯ compile_vpi_lookup(vpiHandle *objref, ವರ್ಣ*label);

ಬಾಹ್ಯ ಶೂನ್ಯ compile_param_string(ವರ್ಣ*label, ಧ್ರುವ ವರ್ಣ*name, ವರ್ಣ*ಮೌಲ್ಯ,
                                 bool local_flag,
                                 long file_idx, long lineno);
ಬಾಹ್ಯ ಶೂನ್ಯ compile_param_logic(ವರ್ಣ*label, ಧ್ರುವ ವರ್ಣ*name, ವರ್ಣ*ಮೌಲ್ಯ,
				bool signed_flag, bool local_flag,
                                long file_idx, long lineno);
ಬಾಹ್ಯ ಶೂನ್ಯ compile_param_real(ವರ್ಣ*label, ಧ್ರುವ ವರ್ಣ*name, ವರ್ಣ*ಮೌಲ್ಯ,
                               bool local_flag,
                               long file_idx, long lineno);

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
      explicit resolv_list_s(ವರ್ಣ*lab) : label_(lab) {
	    next = ರಿಕ್ತ;
      }
      ಆಭಾಸ ~resolv_list_s();

      resolv_list_s(ಧ್ರುವ resolv_list_s&) = delete;
      resolv_list_s& operator=(ಧ್ರುವ resolv_list_s&) = delete;

      ಆಭಾಸ bool resolve(bool mes = false) = 0;

    ರಕ್ಷಿತ:
      ಧ್ರುವ ವರ್ಣ*label() ಧ್ರುವ { ಪ್ರತ್ಯಾಗಮ label_; }

    ಗುಪ್ತ:
      friend ಶೂನ್ಯ resolv_submit(class resolv_list_s*cur);
      friend ಶೂನ್ಯ compile_cleanup(ಶೂನ್ಯ);

      ವರ್ಣ*label_;
      class resolv_list_s*next;
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
ಬಾಹ್ಯ ಶೂನ್ಯ functor_ref_lookup(vvp_net_t**ref, ವರ್ಣ*lab);

ಬಾಹ್ಯ vpiHandle vpip_make_prop_queue_ref(ವರ್ಣ* class_label,
                                          unsigned prop_idx,
                                          unsigned is_queue_flag);

/*
 * This function schedules a lookup of the labeled instruction. The
 * code points to a code structure that points to the instruction
 * field that receives the result, and the label is the name to
 * lookup. The lookup will free the label text when it is done.
 *
 * The cptr2 flag tells the lookup to write the code pointer into the
 * cptr2 member of the code, instead of the cptr member.
 */
ಬಾಹ್ಯ ಶೂನ್ಯ code_label_lookup(ಸಂರಚನಾ vvp_code_s *code, ವರ್ಣ *label, bool cptr2);

/*
 * The `compile_udp_def' function creates a UDP.  The `table' is a
 * NULL terminated array of char*, as assembled by `compile_udp_table'.
 * `compile_udp_table' is called with `table'==NULL to create a new
 * table, or with an existing table to append to.
 *
 * `compile_udp_functor' creates a mode-3 functor referring to the
 * labeled UDP.
 */

ಬಾಹ್ಯ ಶೂನ್ಯ compile_udp_def(ಪೂರ್ಣಾಙ್ಕ sequ, ವರ್ಣ*label, ವರ್ಣ *name,
			    unsigned nin, unsigned init, ವರ್ಣ **table);

ಬಾಹ್ಯ ಶೂನ್ಯ compile_udp_functor(ವರ್ಣ*label, ವರ್ಣ*type,
				unsigned argc, ಸಂರಚನಾ symb_s*argv);

ಬಾಹ್ಯ ವರ್ಣ **compile_udp_table(ವರ್ಣ **table, ವರ್ಣ *row);

/*
 * Memory Instances, Ports, and Initialization
 */

ಬಾಹ್ಯ ಶೂನ್ಯ compile_var_array(ವರ್ಣ*label, ವರ್ಣ*name,
			      ಪೂರ್ಣಾಙ್ಕ last, ಪೂರ್ಣಾಙ್ಕ first,
			      ಪೂರ್ಣಾಙ್ಕ msb, ಪೂರ್ಣಾಙ್ಕ lsb, ವರ್ಣ signed_flag);
ಬಾಹ್ಯ ಶೂನ್ಯ compile_var2_array(ವರ್ಣ*label, ವರ್ಣ*name,
			      ಪೂರ್ಣಾಙ್ಕ last, ಪೂರ್ಣಾಙ್ಕ first,
			      ಪೂರ್ಣಾಙ್ಕ msb, ಪೂರ್ಣಾಙ್ಕ lsb, bool signed_flag);
ಬಾಹ್ಯ ಶೂನ್ಯ compile_real_array(ವರ್ಣ*label, ವರ್ಣ*name,
			       ಪೂರ್ಣಾಙ್ಕ last, ಪೂರ್ಣಾಙ್ಕ first);
ಬಾಹ್ಯ ಶೂನ್ಯ compile_string_array(ವರ್ಣ*label, ವರ್ಣ*name,
				 ಪೂರ್ಣಾಙ್ಕ last, ಪೂರ್ಣಾಙ್ಕ first);
ಬಾಹ್ಯ ಶೂನ್ಯ compile_object_array(ವರ್ಣ*label, ವರ್ಣ*name,
				 ಪೂರ್ಣಾಙ್ಕ last, ಪೂರ್ಣಾಙ್ಕ first);
ಬಾಹ್ಯ ಶೂನ್ಯ compile_net_array(ವರ್ಣ*label, ವರ್ಣ*name,
			      ಪೂರ್ಣಾಙ್ಕ last, ಪೂರ್ಣಾಙ್ಕ first);
ಬಾಹ್ಯ ಶೂನ್ಯ compile_array_alias(ವರ್ಣ*label, ವರ್ಣ*name, ವರ್ಣ*src);

  /* Index is a net. */
ಬಾಹ್ಯ ಶೂನ್ಯ compile_array_port(ವರ್ಣ*label, ವರ್ಣ*name, ವರ್ಣ*addr);
  /* Index is a constant address */
ಬಾಹ್ಯ ಶೂನ್ಯ compile_array_port(ವರ್ಣ*label, ವರ್ಣ*name, long addr);

ಬಾಹ್ಯ ಶೂನ್ಯ compile_array_cleanup(ಶೂನ್ಯ);

/*
 * Compile the .ufunc statement.
 */
ಬಾಹ್ಯ ಶೂನ್ಯ compile_ufunc_real(ವರ್ಣ*label, ವರ್ಣ*code, unsigned wid,
			  unsigned argc, ಸಂರಚನಾ symb_s*argv,
			  unsigned portc, ಸಂರಚನಾ symb_s*portv,
			  ವರ್ಣ*scope_label, ವರ್ಣ*trigger_label);
ಬಾಹ್ಯ ಶೂನ್ಯ compile_ufunc_vec4(ವರ್ಣ*label, ವರ್ಣ*code, unsigned wid,
			  unsigned argc, ಸಂರಚನಾ symb_s*argv,
			  unsigned portc, ಸಂರಚನಾ symb_s*portv,
			  ವರ್ಣ*scope_label, ವರ್ಣ*trigger_label);

/*
 * The compile_event function takes the parts of the event statement
 * and makes the various objects needed to simulate it. This includes
 * the functor that receives the signals and the event_t that holds
 * the threads.
 */
ಬಾಹ್ಯ ಶೂನ್ಯ compile_event(ವರ್ಣ*label, ವರ್ಣ*type,
			  unsigned argc, ಸಂರಚನಾ symb_s*argv);
ಬಾಹ್ಯ ಶೂನ್ಯ compile_named_event(ವರ್ಣ*label, ವರ್ಣ*type, bool local_flag=false);


/*
 * A code statement is a label, an opcode and up to 3 operands. There
 * are a few lexical types that the parser recognizes of the operands,
 * given by the ltype_e enumeration. The compile_code function takes
 * the label, mnemonic and parsed operands and writes a properly
 * formed instruction into the code space. The label is set into the
 * symbol table with the address of the instruction.
 */

#define OPERAND_MAX 3
ಗಣನಾ ltype_e { L_NUMB, L_SYMB, L_STRING };

ಸಂರಚನಾ comp_operands_s {
      unsigned argc;
      ಸಂರಚನಾ {
	    ಗಣನಾ ltype_e ltype;
	    ಸಙ್ಘ {
		  unsigned long numb;
		  ಸಂರಚನಾ symb_s symb;
		  ಧ್ರುವ ವರ್ಣ   *text;
	    };
      } argv[OPERAND_MAX];
};

ಪ್ರಕಾರನಿರ್ದೇಶ ಸಂರಚನಾ comp_operands_s*comp_operands_t;

ಬಾಹ್ಯ ಶೂನ್ಯ compile_code(ವರ್ಣ*label, ವರ್ಣ*mnem, comp_operands_t opa);

ಬಾಹ್ಯ ಶೂನ್ಯ compile_file_ಸಾಲು(ವರ್ಣ*label, long file_idx, long lineno,
                              ವರ್ಣ*description);

ಬಾಹ್ಯ ಶೂನ್ಯ compile_vpi_call(ವರ್ಣ*label, ವರ್ಣ*name,
			     bool func_as_task_err, bool func_as_task_warn,
			     long file_idx, long lineno,
			     unsigned argc, vpiHandle*argv,
			     unsigned vec4_ರಾಶಿ,
			     unsigned real_ರಾಶಿ,
			     unsigned string_ರಾಶಿ);

/* Compile a function call. The vbit and vwid encode the return
   type. If the vwid >0, the return type is a vector. If the vwid is
   <0, the return type is -vpiRealConst or some other constant subtype
   code that represents the function type. */
ಬಾಹ್ಯ ಶೂನ್ಯ compile_vpi_func_call(ವರ್ಣ*label, ವರ್ಣ*name,
				  ಪೂರ್ಣಾಙ್ಕ val_type, unsigned val_wid,
				  long file_idx, long lineno,
				  unsigned argc, vpiHandle*argv,
				  unsigned vec4_ರಾಶಿ,
				  unsigned real_ರಾಶಿ,
				  unsigned string_ರಾಶಿ);
ಬಾಹ್ಯ ಶೂನ್ಯ print_vpi_call_errors();

ಬಾಹ್ಯ ಶೂನ್ಯ compile_codelabel(ವರ್ಣ*label);

/*
 * The parser uses these functions to compile .scope statements.
 * The implementations of these live in the vpi_scope.cc file.
 */
ಬಾಹ್ಯ ಶೂನ್ಯ compile_scope_decl(ವರ್ಣ*typ, ವರ್ಣ*lab, ವರ್ಣ*nam, ವರ್ಣ*tnam,
                               ವರ್ಣ*par, long file_idx, long lineno,
                               long def_file_idx, long def_lineno,
                               long is_cell);
ಬಾಹ್ಯ ಶೂನ್ಯ compile_scope_recall(ವರ್ಣ*sym);

/*
 * The parser uses this function to declare a thread. The start_sym is
 * the start instruction, and must already be defined.
 */
ಬಾಹ್ಯ ಶೂನ್ಯ compile_thread(ವರ್ಣ*start_sym, ವರ್ಣ*flag);

/*
 * This function is called to create a var vector with the given name.
 *
 * The vpi_type_code argument of compile_variable() is one of the vpi
 * object codes that identify the type: vpiReg, vpiIntegerVar,
 * vpiIntVar, etc.
 */
ಬಾಹ್ಯ ಶೂನ್ಯ compile_variable(ವರ್ಣ*label, ವರ್ಣ*name,
			     ಪೂರ್ಣಾಙ್ಕ msb, ಪೂರ್ಣಾಙ್ಕ lsb, ಪೂರ್ಣಾಙ್ಕ vpi_type_code,
			     bool signed_flag, bool local_flag);

ಬಾಹ್ಯ ಶೂನ್ಯ compile_var_real(ವರ್ಣ*label, ವರ್ಣ*name, bool local_flag);
ಬಾಹ್ಯ ಶೂನ್ಯ compile_var_string(ವರ್ಣ*label, ವರ್ಣ*name);
ಬಾಹ್ಯ ಶೂನ್ಯ compile_var_darray(ವರ್ಣ*label, ವರ್ಣ*name, unsigned size);
ಬಾಹ್ಯ ಶೂನ್ಯ compile_var_cobject(ವರ್ಣ*label, ವರ್ಣ*name);
ಬಾಹ್ಯ ಶೂನ್ಯ compile_var_queue(ವರ್ಣ*label, ವರ್ಣ*name, unsigned size);

/*
 * This function is used to create a scope port
 * Current ONLY module ports are supported and ports exist purely
 * as meta-data for VPI queries (i.e. there is NO corresponding net etc)
 * as elaboration internally eliminates port-nets by directly connecting
 * nets connected through module ports.
 */

ಬಾಹ್ಯ ಶೂನ್ಯ compile_port_info( unsigned index, ಪೂರ್ಣಾಙ್ಕ vpi_port_type, unsigned width, ಧ್ರುವ ವರ್ಣ *name, ವರ್ಣ* buffer );


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
ಬಾಹ್ಯ ಶೂನ್ಯ compile_net(ವರ್ಣ*label, ವರ್ಣ*name, ಪೂರ್ಣಾಙ್ಕ msb, ಪೂರ್ಣಾಙ್ಕ lsb,
			ಪೂರ್ಣಾಙ್ಕ vpi_type_code, bool signed_flag, bool local_flag,
			unsigned argc, ಸಂರಚನಾ symb_s*argv);

ಬಾಹ್ಯ ಶೂನ್ಯ compile_net_real(ವರ್ಣ*label, ವರ್ಣ*name,
			     ಪೂರ್ಣಾಙ್ಕ msb, ಪೂರ್ಣಾಙ್ಕ lsb, bool local_flag,
			     unsigned argc, ಸಂರಚನಾ symb_s*argv);

ಬಾಹ್ಯ ಶೂನ್ಯ compile_netw(ವರ್ಣ*label, ವರ್ಣ*array_label, unsigned long array_addr,
			 ಪೂರ್ಣಾಙ್ಕ msb, ಪೂರ್ಣಾಙ್ಕ lsb, ಪೂರ್ಣಾಙ್ಕ vpi_type_code, bool signed_flag,
			 unsigned argc, ಸಂರಚನಾ symb_s*argv);

ಬಾಹ್ಯ ಶೂನ್ಯ compile_netw_real(ವರ್ಣ*label, ವರ್ಣ*array_ಚಿಹ್ನೆ,
			      unsigned long array_addr,
			      ಪೂರ್ಣಾಙ್ಕ msb, ಪೂರ್ಣಾಙ್ಕ lsb,
			      unsigned argc, ಸಂರಚನಾ symb_s*argv);


ಬಾಹ್ಯ ಶೂನ್ಯ compile_island(ವರ್ಣ*label, ವರ್ಣ*type);
ಬಾಹ್ಯ ಶೂನ್ಯ compile_island_port(ವರ್ಣ*label, ವರ್ಣ*island, ವರ್ಣ*src);
ಬಾಹ್ಯ ಶೂನ್ಯ compile_island_import(ವರ್ಣ*label, ವರ್ಣ*island, ವರ್ಣ*src);
ಬಾಹ್ಯ ಶೂನ್ಯ compile_island_export(ವರ್ಣ*label, ವರ್ಣ*island);
ಬಾಹ್ಯ ಶೂನ್ಯ compile_island_cleanup(ಶೂನ್ಯ);

ಬಾಹ್ಯ ಶೂನ್ಯ compile_island_tran(ವರ್ಣ*label);
ಬಾಹ್ಯ ಶೂನ್ಯ compile_island_tranif(ಪೂರ್ಣಾಙ್ಕ sense, ವರ್ಣ*island,
				  ವರ್ಣ*ba, ವರ್ಣ*bb, ವರ್ಣ*src, bool resistive);
ಬಾಹ್ಯ ಶೂನ್ಯ compile_island_tranvp(ವರ್ಣ*island, ವರ್ಣ*ba, ವರ್ಣ*bb,
				  unsigned width, unsigned part, unsigned off);

ಬಾಹ್ಯ ಶೂನ್ಯ delete_udp_symbols(ಶೂನ್ಯ);

ಬಾಹ್ಯ ಶೂನ್ಯ compile_class_start(ವರ್ಣ*lab, ವರ್ಣ*nam, unsigned nprop);
ಬಾಹ್ಯ ಶೂನ್ಯ compile_class_property(unsigned idx, ವರ್ಣ*nam, ವರ್ಣ*typ, uint64_t array_size);
ಬಾಹ್ಯ ಶೂನ್ಯ compile_class_done(ಶೂನ್ಯ);

#endif /* IVL_compile_H */
