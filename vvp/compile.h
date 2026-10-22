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
ବାହ୍ୟ std::vector<ଅଚଳ ଅକ୍ଷର*> file_names;

/*
 * The functions described here are the compile time support
 * functions. Various bits of the compile process are taken care of
 * here. What is called when is mostly controlled by the parser.
 *
 * Before compilation takes place, the compile_init function must be
 * called once to set stuff up.
 */

ବାହ୍ୟ ଶୂନ୍ୟ compile_init(ଶୂନ୍ୟ);

ବାହ୍ୟ ଶୂନ୍ୟ compile_cleanup(ଶୂନ୍ୟ);

ବାହ୍ୟ bool verbose_flag;

/*
 * If this file opened, then write debug information to this
 * file. This is used for debugging the VVP runtime itself.
 */
ବାହ୍ୟ std::ofstream debug_file;

/*
 *  Connect a list of symbols to a contiguous set of ipoints.
 *  Constants C<?> are handled by setting the ival of the ipoint.
 */
ବାହ୍ୟ ଶୂନ୍ୟ inputs_connect(vvp_net_t*fdx, unsigned argc, ସଂରଚନା symb_s*argv);
ବାହ୍ୟ ଶୂନ୍ୟ ନିବେଶ_connect(vvp_net_t*fdx, unsigned port, ଅକ୍ଷର*label);

/*
 * This function is an expansion of the inputs_connect function. It
 * uses the inputs_connect function, but it creates vvp_wide_fun_t
 * nodes to handle arbitrary width nodes, and connects those nodes to
 * the vvp_wide_fun_core object passed in.
 */
ବାହ୍ୟ ଶୂନ୍ୟ wide_inputs_connect(vvp_wide_fun_core*core,
				unsigned argc, ସଂରଚନା symb_s*argv);

ବାହ୍ୟ vvp_net_t* vvp_net_lookup(ଅଚଳ ଅକ୍ଷର*label);
ବାହ୍ୟ vpiHandle vvp_lookup_handle(ଅଚଳ ଅକ୍ଷର*label);

/*
 *  Add a functor to the symbol table
 */
ବାହ୍ୟ ଶୂନ୍ୟ define_functor_ପ୍ରତୀକ(ଅଚଳ ଅକ୍ଷର*label, vvp_net_t*ipt);


/*
 * This is a count of errors encountered during compilation. If this
 * is non-zero, then simulation is not recommended.
 */
ବାହ୍ୟ unsigned compile_errors;

ବାହ୍ୟ ଅଚଳ ଅକ୍ଷର* module_path;
ବାହ୍ୟ ଶୂନ୍ୟ compile_load_vpi_module(ଅକ୍ଷର*name);

ବାହ୍ୟ ଶୂନ୍ୟ compile_vpi_time_precision(long pre);

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
ବାହ୍ୟ ଶୂନ୍ୟ compile_functor(ଅକ୍ଷର*label, ଅକ୍ଷର*type, unsigned width,
			    unsigned ostr0, unsigned ostr1,
			    unsigned argc, ସଂରଚନା symb_s*argv);


/*
 * This is called by the parser to make a resolver. This is a special
 * kind of functor; a strength aware functor. It has up to 4 inputs
 * that are blended to make a resolved output. The type string selects
 * a resolution algorithm.
 */
ବାହ୍ୟ ଶୂନ୍ୟ compile_resolver(ଅକ୍ଷର*label, ଅକ୍ଷର*type,
			     unsigned argc, ସଂରଚନା symb_s*argv);

ବାହ୍ୟ ଶୂନ୍ୟ compile_concat(ଅକ୍ଷର*label, unsigned w0, unsigned w1,
			   unsigned w2, unsigned w3,
			   unsigned argc, ସଂରଚନା symb_s*argv);

ବାହ୍ୟ ଶୂନ୍ୟ compile_concat8(ଅକ୍ଷର*label, unsigned w0, unsigned w1,
			    unsigned w2, unsigned w3,
			    unsigned argc, ସଂରଚନା symb_s*argv);

ବାହ୍ୟ ଶୂନ୍ୟ compile_substitute(ଅକ୍ଷର*label, unsigned width,
			       unsigned soff, unsigned swidth,
			       unsigned argc, ସଂରଚନା symb_s*argv);

/*
 * Arrange for the system task/function call to have its compiletf
 * function called.
 */
ବାହ୍ୟ ଶୂନ୍ୟ compile_compiletf(ସଂରଚନା __vpiSysTaskCall*);

/*
 * Compile delay nodes of various form.
 */
ବାହ୍ୟ ଶୂନ୍ୟ compile_delay(ଅକ୍ଷର*label, unsigned width,
                          vvp_delay_t*del, ସଂରଚନା symb_s ନିବେଶ);
ବାହ୍ୟ ଶୂନ୍ୟ compile_delay(ଅକ୍ଷର*label, unsigned width,
                          unsigned argc, ସଂରଚନା symb_s*argv,
                          bool ignore_decay);

/*
 * This is called by the parser to create a part select node.
 * See the PART SELECT STATEMENT section in the README.txt
 */
ବାହ୍ୟ ଶୂନ୍ୟ compile_part_select(ଅକ୍ଷର*label, ଅକ୍ଷର*src,
				unsigned base, unsigned wid);
ବାହ୍ୟ ଶୂନ୍ୟ compile_part_select_pv(ଅକ୍ଷର*label, ଅକ୍ଷର*src,
				   unsigned base, unsigned wid,
				   unsigned vec_wid);
ବାହ୍ୟ ଶୂନ୍ୟ compile_part_select_var(ଅକ୍ଷର*label, ଅକ୍ଷର*src,
                                    ଅକ୍ଷର*var, unsigned wid, bool is_signed);

/*
 * This is called by the parser to make the various arithmetic and
 * comparison functors.
 */
ବାହ୍ୟ ଶୂନ୍ୟ compile_arith_pow(ଅକ୍ଷର*label, long width, bool signed_flag,
			      unsigned argc, ସଂରଚନା symb_s*argv);
ବାହ୍ୟ ଶୂନ୍ୟ compile_arith_abs(ଅକ୍ଷର*label,
			      unsigned argc, ସଂରଚନା symb_s*argv);
ବାହ୍ୟ ଶୂନ୍ୟ compile_arith_cast_int(ଅକ୍ଷର*label, long width,
			           unsigned argc, ସଂରଚନା symb_s*argv);
ବାହ୍ୟ ଶୂନ୍ୟ compile_arith_cast_real(ଅକ୍ଷର*label, bool signed_flag,
			            unsigned argc, ସଂରଚନା symb_s*argv);
ବାହ୍ୟ ଶୂନ୍ୟ compile_arith_cast_vec2(ଅକ୍ଷର*label, long width,
				    unsigned argc, ସଂରଚନା symb_s*argv);
ବାହ୍ୟ ଶୂନ୍ୟ compile_arith_div(ଅକ୍ଷର*label, long width, bool signed_flag,
			      unsigned argc, ସଂରଚନା symb_s*argv);
ବାହ୍ୟ ଶୂନ୍ୟ compile_arith_mod(ଅକ୍ଷର*label, long width, bool signed_flag,
			      unsigned argc, ସଂରଚନା symb_s*argv);
ବାହ୍ୟ ଶୂନ୍ୟ compile_arith_mult(ଅକ୍ଷର*label, long width,
			       unsigned argc, ସଂରଚନା symb_s*argv);
ବାହ୍ୟ ଶୂନ୍ୟ compile_arith_sum(ଅକ୍ଷର*label, long width,
			      unsigned argc, ସଂରଚନା symb_s*argv);
ବାହ୍ୟ ଶୂନ୍ୟ compile_arith_sub(ଅକ୍ଷର*label, long width,
			      unsigned argc, ସଂରଚନା symb_s*argv);
ବାହ୍ୟ ଶୂନ୍ୟ compile_cmp_eeq(ଅକ୍ଷର*label, long width,
			   unsigned argc, ସଂରଚନା symb_s*argv);
ବାହ୍ୟ ଶୂନ୍ୟ compile_cmp_nee(ଅକ୍ଷର*label, long width,
			   unsigned argc, ସଂରଚନା symb_s*argv);
ବାହ୍ୟ ଶୂନ୍ୟ compile_cmp_eq(ଅକ୍ଷର*label, long width,
			   unsigned argc, ସଂରଚନା symb_s*argv);
ବାହ୍ୟ ଶୂନ୍ୟ compile_cmp_eqx(ଅକ୍ଷର*label, long width,
			    unsigned argc, ସଂରଚନା symb_s*argv);
ବାହ୍ୟ ଶୂନ୍ୟ compile_cmp_eqz(ଅକ୍ଷର*label, long width,
			    unsigned argc, ସଂରଚନା symb_s*argv);
ବାହ୍ୟ ଶୂନ୍ୟ compile_cmp_ne(ଅକ୍ଷର*label, long width,
			   unsigned argc, ସଂରଚନା symb_s*argv);
ବାହ୍ୟ ଶୂନ୍ୟ compile_cmp_ge(ଅକ୍ଷର*label, long width, bool signed_flag,
			   unsigned argc, ସଂରଚନା symb_s*argv);
ବାହ୍ୟ ଶୂନ୍ୟ compile_cmp_gt(ଅକ୍ଷର*label, long width, bool signed_flag,
			   unsigned argc, ସଂରଚନା symb_s*argv);
ବାହ୍ୟ ଶୂନ୍ୟ compile_cmp_weq(ଅକ୍ଷର*label, long width,
			   unsigned argc, ସଂରଚନା symb_s*argv);
ବାହ୍ୟ ଶୂନ୍ୟ compile_cmp_wne(ଅକ୍ଷର*label, long width,
			   unsigned argc, ସଂରଚନା symb_s*argv);

ବାହ୍ୟ ଶୂନ୍ୟ compile_arith_mult_r(ଅକ୍ଷର*label, unsigned argc,
                                 ସଂରଚନା symb_s*argv);
ବାହ୍ୟ ଶୂନ୍ୟ compile_arith_pow_r(ଅକ୍ଷର*label, unsigned argc, ସଂରଚନା symb_s*argv);
ବାହ୍ୟ ଶୂନ୍ୟ compile_arith_div_r(ଅକ୍ଷର*label, unsigned argc, ସଂରଚନା symb_s*argv);
ବାହ୍ୟ ଶୂନ୍ୟ compile_arith_mod_r(ଅକ୍ଷର*label, unsigned argc, ସଂରଚନା symb_s*argv);
ବାହ୍ୟ ଶୂନ୍ୟ compile_arith_sum_r(ଅକ୍ଷର*label, unsigned argc, ସଂରଚନା symb_s*argv);
ବାହ୍ୟ ଶୂନ୍ୟ compile_arith_sub_r(ଅକ୍ଷର*label, unsigned argc, ସଂରଚନା symb_s*argv);
ବାହ୍ୟ ଶୂନ୍ୟ compile_cmp_eq_r(ଅକ୍ଷର*label, unsigned argc, ସଂରଚନା symb_s*argv);
ବାହ୍ୟ ଶୂନ୍ୟ compile_cmp_ne_r(ଅକ୍ଷର*label, unsigned argc, ସଂରଚନା symb_s*argv);
ବାହ୍ୟ ଶୂନ୍ୟ compile_cmp_ge_r(ଅକ୍ଷର*label, unsigned argc, ସଂରଚନା symb_s*argv);
ବାହ୍ୟ ଶୂନ୍ୟ compile_cmp_gt_r(ଅକ୍ଷର*label, unsigned argc, ସଂରଚନା symb_s*argv);

ବାହ୍ୟ ଶୂନ୍ୟ compile_dff(ଅକ୍ଷର*label, unsigned width, bool negedge,
			ସଂରଚନା symb_s arg_d,
			ସଂରଚନା symb_s arg_c,
			ସଂରଚନା symb_s arg_e);

ବାହ୍ୟ ଶୂନ୍ୟ compile_dff_aclr(ଅକ୍ଷର*label, unsigned width, bool negedge,
			     ସଂରଚନା symb_s arg_d,
			     ସଂରଚନା symb_s arg_c,
			     ସଂରଚନା symb_s arg_e,
			     ସଂରଚନା symb_s arg_a);
ବାହ୍ୟ ଶୂନ୍ୟ compile_dff_aset(ଅକ୍ଷର*label, unsigned width, bool negedge,
			     ସଂରଚନା symb_s arg_d,
			     ସଂରଚନା symb_s arg_c,
			     ସଂରଚନା symb_s arg_e,
			     ସଂରଚନା symb_s arg_a,
			     ଅକ୍ଷର*asc_ମୂଲ୍ୟ);

ବାହ୍ୟ ଶୂନ୍ୟ compile_latch(ଅକ୍ଷର*label, unsigned width,
			  ସଂରଚନା symb_s arg_d,
			  ସଂରଚନା symb_s arg_e);

ବାହ୍ୟ ଶୂନ୍ୟ compile_enum2_type(ଅକ୍ଷର*label, long width, bool signed_flag,
			      std::list<ସଂରଚନା enum_name_s>*names);
ବାହ୍ୟ ଶୂନ୍ୟ compile_enum4_type(ଅକ୍ଷର*label, long width, bool signed_flag,
			      std::list<ସଂରଚନା enum_name_s>*names);

ସଂରଚନା __vpiModPath;
ବାହ୍ୟ __vpiModPath* compile_modpath(ଅକ୍ଷର*label,
                                     unsigned width,
				     ସଂରଚନା symb_s drv,
				     ସଂରଚନା symb_s dest);
ବାହ୍ୟ ଶୂନ୍ୟ compile_modpath_src(__vpiModPath*dst,
				ଅକ୍ଷର edge_c,
				ଅଚଳ ସଂରଚନା symb_s&src,
				ସଂରଚନା numbv_s&vals,
				ଅଚଳ ସଂରଚନା symb_s&condit_src,
				ଅଚଳ ସଂରଚନା symb_s&path_term_in);
ବାହ୍ୟ ଶୂନ୍ୟ compile_modpath_src(__vpiModPath*dst,
				ଅକ୍ଷର edge_c,
				ଅଚଳ ସଂରଚନା symb_s&src,
				ସଂରଚନା numbv_s&vals,
				ପୂର୍ଣ୍ଣସଂଖ୍ୟା condit_src, /* match with '0' */
				ଅଚଳ ସଂରଚନା symb_s&path_term_in,
				bool ifnone);

ବାହ୍ୟ ଶୂନ୍ୟ compile_reduce_and(ଅକ୍ଷର*label, ଅଚଳ ସଂରଚନା symb_s&arg);
ବାହ୍ୟ ଶୂନ୍ୟ compile_reduce_or(ଅକ୍ଷର*label, ଅଚଳ ସଂରଚନା symb_s&arg);
ବାହ୍ୟ ଶୂନ୍ୟ compile_reduce_xor(ଅକ୍ଷର*label, ଅଚଳ ସଂରଚନା symb_s&arg);
ବାହ୍ୟ ଶୂନ୍ୟ compile_reduce_nand(ଅକ୍ଷର*label, ଅଚଳ ସଂରଚନା symb_s&arg);
ବାହ୍ୟ ଶୂନ୍ୟ compile_reduce_nor(ଅକ୍ଷର*label, ଅଚଳ ସଂରଚନା symb_s&arg);
ବାହ୍ୟ ଶୂନ୍ୟ compile_reduce_xnor(ଅକ୍ଷର*label, ଅଚଳ ସଂରଚନା symb_s&arg);

ବାହ୍ୟ ଶୂନ୍ୟ compile_extend_signed(ଅକ୍ଷର*label, long width, ସଂରଚନା symb_s arg);

ବାହ୍ୟ ଶୂନ୍ୟ compile_sfunc(ଅକ୍ଷର*label, ଅକ୍ଷର*name, ଅକ୍ଷର*format_string,
			  long file_idx, long lineno,
			  unsigned argc, ସଂରଚନା symb_s*argv,
                          ଅକ୍ଷର*trigger_label);

ବାହ୍ୟ ଶୂନ୍ୟ compile_repeat(ଅକ୍ଷର*label, long width, long repeat,
			   ସଂରଚନା symb_s arg);

ବାହ୍ୟ ଶୂନ୍ୟ compile_shiftl(ଅକ୍ଷର*label, long width,
			   unsigned argc, ସଂରଚନା symb_s*argv);
ବାହ୍ୟ ଶୂନ୍ୟ compile_shiftr(ଅକ୍ଷର*label, long width, bool signed_flag,
			   unsigned argc, ସଂରଚନା symb_s*argv);

ବାହ୍ୟ ଶୂନ୍ୟ compile_timescale(long units, long precision);

ବାହ୍ୟ ଶୂନ୍ୟ compile_vpi_ପ୍ରତୀକ(ଅଚଳ ଅକ୍ଷର*label, vpiHandle obj);
ବାହ୍ୟ ଶୂନ୍ୟ compile_vpi_lookup(vpiHandle *objref, ଅକ୍ଷର*label);

ବାହ୍ୟ ଶୂନ୍ୟ compile_param_string(ଅକ୍ଷର*label, ଅଚଳ ଅକ୍ଷର*name, ଅକ୍ଷର*ମୂଲ୍ୟ,
                                 bool local_flag,
                                 long file_idx, long lineno);
ବାହ୍ୟ ଶୂନ୍ୟ compile_param_logic(ଅକ୍ଷର*label, ଅଚଳ ଅକ୍ଷର*name, ଅକ୍ଷର*ମୂଲ୍ୟ,
				bool signed_flag, bool local_flag,
                                long file_idx, long lineno);
ବାହ୍ୟ ଶୂନ୍ୟ compile_param_real(ଅକ୍ଷର*label, ଅଚଳ ଅକ୍ଷର*name, ଅକ୍ଷର*ମୂଲ୍ୟ,
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

    ସର୍ବସାଧାରଣ:
      explicit resolv_list_s(ଅକ୍ଷର*lab) : label_(lab) {
	    next = ଖାଲି;
      }
      ଆଭାସୀ ~resolv_list_s();

      resolv_list_s(ଅଚଳ resolv_list_s&) = delete;
      resolv_list_s& operator=(ଅଚଳ resolv_list_s&) = delete;

      ଆଭାସୀ bool resolve(bool mes = false) = 0;

    ସୁରକ୍ଷିତ:
      ଅଚଳ ଅକ୍ଷର*label() ଅଚଳ { ଫେରନ୍ତୁ label_; }

    ବ୍ୟକ୍ତିଗତ:
      friend ଶୂନ୍ୟ resolv_submit(class resolv_list_s*cur);
      friend ଶୂନ୍ୟ compile_cleanup(ଶୂନ୍ୟ);

      ଅକ୍ଷର*label_;
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
ବାହ୍ୟ ଶୂନ୍ୟ functor_ref_lookup(vvp_net_t**ref, ଅକ୍ଷର*lab);

ବାହ୍ୟ vpiHandle vpip_make_prop_queue_ref(ଅକ୍ଷର* class_label,
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
ବାହ୍ୟ ଶୂନ୍ୟ code_label_lookup(ସଂରଚନା vvp_code_s *code, ଅକ୍ଷର *label, bool cptr2);

/*
 * The `compile_udp_def' function creates a UDP.  The `table' is a
 * NULL terminated array of char*, as assembled by `compile_udp_table'.
 * `compile_udp_table' is called with `table'==NULL to create a new
 * table, or with an existing table to append to.
 *
 * `compile_udp_functor' creates a mode-3 functor referring to the
 * labeled UDP.
 */

ବାହ୍ୟ ଶୂନ୍ୟ compile_udp_def(ପୂର୍ଣ୍ଣସଂଖ୍ୟା sequ, ଅକ୍ଷର*label, ଅକ୍ଷର *name,
			    unsigned nin, unsigned init, ଅକ୍ଷର **table);

ବାହ୍ୟ ଶୂନ୍ୟ compile_udp_functor(ଅକ୍ଷର*label, ଅକ୍ଷର*type,
				unsigned argc, ସଂରଚନା symb_s*argv);

ବାହ୍ୟ ଅକ୍ଷର **compile_udp_table(ଅକ୍ଷର **table, ଅକ୍ଷର *row);

/*
 * Memory Instances, Ports, and Initialization
 */

ବାହ୍ୟ ଶୂନ୍ୟ compile_var_array(ଅକ୍ଷର*label, ଅକ୍ଷର*name,
			      ପୂର୍ଣ୍ଣସଂଖ୍ୟା last, ପୂର୍ଣ୍ଣସଂଖ୍ୟା first,
			      ପୂର୍ଣ୍ଣସଂଖ୍ୟା msb, ପୂର୍ଣ୍ଣସଂଖ୍ୟା lsb, ଅକ୍ଷର signed_flag);
ବାହ୍ୟ ଶୂନ୍ୟ compile_var2_array(ଅକ୍ଷର*label, ଅକ୍ଷର*name,
			      ପୂର୍ଣ୍ଣସଂଖ୍ୟା last, ପୂର୍ଣ୍ଣସଂଖ୍ୟା first,
			      ପୂର୍ଣ୍ଣସଂଖ୍ୟା msb, ପୂର୍ଣ୍ଣସଂଖ୍ୟା lsb, bool signed_flag);
ବାହ୍ୟ ଶୂନ୍ୟ compile_real_array(ଅକ୍ଷର*label, ଅକ୍ଷର*name,
			       ପୂର୍ଣ୍ଣସଂଖ୍ୟା last, ପୂର୍ଣ୍ଣସଂଖ୍ୟା first);
ବାହ୍ୟ ଶୂନ୍ୟ compile_string_array(ଅକ୍ଷର*label, ଅକ୍ଷର*name,
				 ପୂର୍ଣ୍ଣସଂଖ୍ୟା last, ପୂର୍ଣ୍ଣସଂଖ୍ୟା first);
ବାହ୍ୟ ଶୂନ୍ୟ compile_object_array(ଅକ୍ଷର*label, ଅକ୍ଷର*name,
				 ପୂର୍ଣ୍ଣସଂଖ୍ୟା last, ପୂର୍ଣ୍ଣସଂଖ୍ୟା first);
ବାହ୍ୟ ଶୂନ୍ୟ compile_net_array(ଅକ୍ଷର*label, ଅକ୍ଷର*name,
			      ପୂର୍ଣ୍ଣସଂଖ୍ୟା last, ପୂର୍ଣ୍ଣସଂଖ୍ୟା first);
ବାହ୍ୟ ଶୂନ୍ୟ compile_array_alias(ଅକ୍ଷର*label, ଅକ୍ଷର*name, ଅକ୍ଷର*src);

  /* Index is a net. */
ବାହ୍ୟ ଶୂନ୍ୟ compile_array_port(ଅକ୍ଷର*label, ଅକ୍ଷର*name, ଅକ୍ଷର*addr);
  /* Index is a constant address */
ବାହ୍ୟ ଶୂନ୍ୟ compile_array_port(ଅକ୍ଷର*label, ଅକ୍ଷର*name, long addr);

ବାହ୍ୟ ଶୂନ୍ୟ compile_array_cleanup(ଶୂନ୍ୟ);

/*
 * Compile the .ufunc statement.
 */
ବାହ୍ୟ ଶୂନ୍ୟ compile_ufunc_real(ଅକ୍ଷର*label, ଅକ୍ଷର*code, unsigned wid,
			  unsigned argc, ସଂରଚନା symb_s*argv,
			  unsigned portc, ସଂରଚନା symb_s*portv,
			  ଅକ୍ଷର*scope_label, ଅକ୍ଷର*trigger_label);
ବାହ୍ୟ ଶୂନ୍ୟ compile_ufunc_vec4(ଅକ୍ଷର*label, ଅକ୍ଷର*code, unsigned wid,
			  unsigned argc, ସଂରଚନା symb_s*argv,
			  unsigned portc, ସଂରଚନା symb_s*portv,
			  ଅକ୍ଷର*scope_label, ଅକ୍ଷର*trigger_label);

/*
 * The compile_event function takes the parts of the event statement
 * and makes the various objects needed to simulate it. This includes
 * the functor that receives the signals and the event_t that holds
 * the threads.
 */
ବାହ୍ୟ ଶୂନ୍ୟ compile_event(ଅକ୍ଷର*label, ଅକ୍ଷର*type,
			  unsigned argc, ସଂରଚନା symb_s*argv);
ବାହ୍ୟ ଶୂନ୍ୟ compile_named_event(ଅକ୍ଷର*label, ଅକ୍ଷର*type, bool local_flag=false);


/*
 * A code statement is a label, an opcode and up to 3 operands. There
 * are a few lexical types that the parser recognizes of the operands,
 * given by the ltype_e enumeration. The compile_code function takes
 * the label, mnemonic and parsed operands and writes a properly
 * formed instruction into the code space. The label is set into the
 * symbol table with the address of the instruction.
 */

#define OPERAND_MAX 3
ଗଣନା ltype_e { L_NUMB, L_SYMB, L_STRING };

ସଂରଚନା comp_operands_s {
      unsigned argc;
      ସଂରଚନା {
	    ଗଣନା ltype_e ltype;
	    ସଂଘ {
		  unsigned long numb;
		  ସଂରଚନା symb_s symb;
		  ଅଚଳ ଅକ୍ଷର   *text;
	    };
      } argv[OPERAND_MAX];
};

ପ୍ରକାରବ୍ୟାଖ୍ୟା ସଂରଚନା comp_operands_s*comp_operands_t;

ବାହ୍ୟ ଶୂନ୍ୟ compile_code(ଅକ୍ଷର*label, ଅକ୍ଷର*mnem, comp_operands_t opa);

ବାହ୍ୟ ଶୂନ୍ୟ compile_file_ଧାଡ଼ି(ଅକ୍ଷର*label, long file_idx, long lineno,
                              ଅକ୍ଷର*description);

ବାହ୍ୟ ଶୂନ୍ୟ compile_vpi_call(ଅକ୍ଷର*label, ଅକ୍ଷର*name,
			     bool func_as_task_err, bool func_as_task_warn,
			     long file_idx, long lineno,
			     unsigned argc, vpiHandle*argv,
			     unsigned vec4_ଗଦା,
			     unsigned real_ଗଦା,
			     unsigned string_ଗଦା);

/* Compile a function call. The vbit and vwid encode the return
   type. If the vwid >0, the return type is a vector. If the vwid is
   <0, the return type is -vpiRealConst or some other constant subtype
   code that represents the function type. */
ବାହ୍ୟ ଶୂନ୍ୟ compile_vpi_func_call(ଅକ୍ଷର*label, ଅକ୍ଷର*name,
				  ପୂର୍ଣ୍ଣସଂଖ୍ୟା val_type, unsigned val_wid,
				  long file_idx, long lineno,
				  unsigned argc, vpiHandle*argv,
				  unsigned vec4_ଗଦା,
				  unsigned real_ଗଦା,
				  unsigned string_ଗଦା);
ବାହ୍ୟ ଶୂନ୍ୟ print_vpi_call_errors();

ବାହ୍ୟ ଶୂନ୍ୟ compile_codelabel(ଅକ୍ଷର*label);

/*
 * The parser uses these functions to compile .scope statements.
 * The implementations of these live in the vpi_scope.cc file.
 */
ବାହ୍ୟ ଶୂନ୍ୟ compile_scope_decl(ଅକ୍ଷର*typ, ଅକ୍ଷର*lab, ଅକ୍ଷର*nam, ଅକ୍ଷର*tnam,
                               ଅକ୍ଷର*par, long file_idx, long lineno,
                               long def_file_idx, long def_lineno,
                               long is_cell);
ବାହ୍ୟ ଶୂନ୍ୟ compile_scope_recall(ଅକ୍ଷର*sym);

/*
 * The parser uses this function to declare a thread. The start_sym is
 * the start instruction, and must already be defined.
 */
ବାହ୍ୟ ଶୂନ୍ୟ compile_thread(ଅକ୍ଷର*start_sym, ଅକ୍ଷର*flag);

/*
 * This function is called to create a var vector with the given name.
 *
 * The vpi_type_code argument of compile_variable() is one of the vpi
 * object codes that identify the type: vpiReg, vpiIntegerVar,
 * vpiIntVar, etc.
 */
ବାହ୍ୟ ଶୂନ୍ୟ compile_variable(ଅକ୍ଷର*label, ଅକ୍ଷର*name,
			     ପୂର୍ଣ୍ଣସଂଖ୍ୟା msb, ପୂର୍ଣ୍ଣସଂଖ୍ୟା lsb, ପୂର୍ଣ୍ଣସଂଖ୍ୟା vpi_type_code,
			     bool signed_flag, bool local_flag);

ବାହ୍ୟ ଶୂନ୍ୟ compile_var_real(ଅକ୍ଷର*label, ଅକ୍ଷର*name, bool local_flag);
ବାହ୍ୟ ଶୂନ୍ୟ compile_var_string(ଅକ୍ଷର*label, ଅକ୍ଷର*name);
ବାହ୍ୟ ଶୂନ୍ୟ compile_var_darray(ଅକ୍ଷର*label, ଅକ୍ଷର*name, unsigned size);
ବାହ୍ୟ ଶୂନ୍ୟ compile_var_cobject(ଅକ୍ଷର*label, ଅକ୍ଷର*name);
ବାହ୍ୟ ଶୂନ୍ୟ compile_var_queue(ଅକ୍ଷର*label, ଅକ୍ଷର*name, unsigned size);

/*
 * This function is used to create a scope port
 * Current ONLY module ports are supported and ports exist purely
 * as meta-data for VPI queries (i.e. there is NO corresponding net etc)
 * as elaboration internally eliminates port-nets by directly connecting
 * nets connected through module ports.
 */

ବାହ୍ୟ ଶୂନ୍ୟ compile_port_info( unsigned index, ପୂର୍ଣ୍ଣସଂଖ୍ୟା vpi_port_type, unsigned width, ଅଚଳ ଅକ୍ଷର *name, ଅକ୍ଷର* buffer );


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
ବାହ୍ୟ ଶୂନ୍ୟ compile_net(ଅକ୍ଷର*label, ଅକ୍ଷର*name, ପୂର୍ଣ୍ଣସଂଖ୍ୟା msb, ପୂର୍ଣ୍ଣସଂଖ୍ୟା lsb,
			ପୂର୍ଣ୍ଣସଂଖ୍ୟା vpi_type_code, bool signed_flag, bool local_flag,
			unsigned argc, ସଂରଚନା symb_s*argv);

ବାହ୍ୟ ଶୂନ୍ୟ compile_net_real(ଅକ୍ଷର*label, ଅକ୍ଷର*name,
			     ପୂର୍ଣ୍ଣସଂଖ୍ୟା msb, ପୂର୍ଣ୍ଣସଂଖ୍ୟା lsb, bool local_flag,
			     unsigned argc, ସଂରଚନା symb_s*argv);

ବାହ୍ୟ ଶୂନ୍ୟ compile_netw(ଅକ୍ଷର*label, ଅକ୍ଷର*array_label, unsigned long array_addr,
			 ପୂର୍ଣ୍ଣସଂଖ୍ୟା msb, ପୂର୍ଣ୍ଣସଂଖ୍ୟା lsb, ପୂର୍ଣ୍ଣସଂଖ୍ୟା vpi_type_code, bool signed_flag,
			 unsigned argc, ସଂରଚନା symb_s*argv);

ବାହ୍ୟ ଶୂନ୍ୟ compile_netw_real(ଅକ୍ଷର*label, ଅକ୍ଷର*array_ପ୍ରତୀକ,
			      unsigned long array_addr,
			      ପୂର୍ଣ୍ଣସଂଖ୍ୟା msb, ପୂର୍ଣ୍ଣସଂଖ୍ୟା lsb,
			      unsigned argc, ସଂରଚନା symb_s*argv);


ବାହ୍ୟ ଶୂନ୍ୟ compile_island(ଅକ୍ଷର*label, ଅକ୍ଷର*type);
ବାହ୍ୟ ଶୂନ୍ୟ compile_island_port(ଅକ୍ଷର*label, ଅକ୍ଷର*island, ଅକ୍ଷର*src);
ବାହ୍ୟ ଶୂନ୍ୟ compile_island_import(ଅକ୍ଷର*label, ଅକ୍ଷର*island, ଅକ୍ଷର*src);
ବାହ୍ୟ ଶୂନ୍ୟ compile_island_export(ଅକ୍ଷର*label, ଅକ୍ଷର*island);
ବାହ୍ୟ ଶୂନ୍ୟ compile_island_cleanup(ଶୂନ୍ୟ);

ବାହ୍ୟ ଶୂନ୍ୟ compile_island_tran(ଅକ୍ଷର*label);
ବାହ୍ୟ ଶୂନ୍ୟ compile_island_tranif(ପୂର୍ଣ୍ଣସଂଖ୍ୟା sense, ଅକ୍ଷର*island,
				  ଅକ୍ଷର*ba, ଅକ୍ଷର*bb, ଅକ୍ଷର*src, bool resistive);
ବାହ୍ୟ ଶୂନ୍ୟ compile_island_tranvp(ଅକ୍ଷର*island, ଅକ୍ଷର*ba, ଅକ୍ଷର*bb,
				  unsigned width, unsigned part, unsigned off);

ବାହ୍ୟ ଶୂନ୍ୟ delete_udp_symbols(ଶୂନ୍ୟ);

ବାହ୍ୟ ଶୂନ୍ୟ compile_class_start(ଅକ୍ଷର*lab, ଅକ୍ଷର*nam, unsigned nprop);
ବାହ୍ୟ ଶୂନ୍ୟ compile_class_property(unsigned idx, ଅକ୍ଷର*nam, ଅକ୍ଷର*typ, uint64_t array_size);
ବାହ୍ୟ ଶୂନ୍ୟ compile_class_done(ଶୂନ୍ୟ);

#endif /* IVL_compile_H */
