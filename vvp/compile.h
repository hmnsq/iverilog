#جيڪڏهن_وصف_نه_ٿيل IVL_compile_H
#وصف_ڪريو IVL_compile_H
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
خارجي std::ويڪٽر<ثابت حرف*> file_نالا;

/*
 * The functions described here are the compile time support
 * functions. Various bits of the compile process are taken care of
 * here. What is called when is mostly controlled by the parser.
 *
 * Before compilation takes place, the compile_init function must be
 * called once to set stuff up.
 */

خارجي فراغ compile_init(فراغ);

خارجي فراغ compile_cleanup(فراغ);

خارجي bool verbose_flag;

/*
 * If this file opened, then write debug information to this
 * file. This is used for debugging the VVP runtime itself.
 */
خارجي std::ofstream debug_file;

/*
 *  Connect a list of symbols to a contiguous set of ipoints.
 *  Constants C<?> are handled by setting the ival of the ipoint.
 */
خارجي فراغ inputs_connect(vvp_net_t*fdx, بنانشان argc, بنية symb_s*argv);
خارجي فراغ داخلا_connect(vvp_net_t*fdx, بنانشان پورٽ, حرف*label);

/*
 * This function is an expansion of the inputs_connect function. It
 * uses the inputs_connect function, but it creates vvp_wide_fun_t
 * nodes to handle arbitrary width nodes, and connects those nodes to
 * the vvp_wide_fun_core object passed in.
 */
خارجي فراغ wide_inputs_connect(vvp_wide_fun_core*core,
				بنانشان argc, بنية symb_s*argv);

خارجي vvp_net_t* vvp_net_lookup(ثابت حرف*label);
خارجي vpiHandle vvp_lookup_handle(ثابت حرف*label);

/*
 *  Add a functor to the symbol table
 */
خارجي فراغ define_functor_نشان(ثابت حرف*label, vvp_net_t*ipt);


/*
 * This is a count of errors encountered during compilation. If this
 * is non-zero, then simulation is not recommended.
 */
خارجي بنانشان compile_errors;

خارجي ثابت حرف* module_رستو;
خارجي فراغ compile_ڀريو_vpi_module(حرف*name);

خارجي فراغ compile_vpi_وقت_precision(ڊگهو pre);

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
خارجي فراغ compile_functor(حرف*label, حرف*type, بنانشان ويڪر,
			    بنانشان ostr0, بنانشان ostr1,
			    بنانشان argc, بنية symb_s*argv);


/*
 * This is called by the parser to make a resolver. This is a special
 * kind of functor; a strength aware functor. It has up to 4 inputs
 * that are blended to make a resolved output. The type string selects
 * a resolution algorithm.
 */
خارجي فراغ compile_resolver(حرف*label, حرف*type,
			     بنانشان argc, بنية symb_s*argv);

خارجي فراغ compile_concat(حرف*label, بنانشان w0, بنانشان w1,
			   بنانشان w2, بنانشان w3,
			   بنانشان argc, بنية symb_s*argv);

خارجي فراغ compile_concat8(حرف*label, بنانشان w0, بنانشان w1,
			    بنانشان w2, بنانشان w3,
			    بنانشان argc, بنية symb_s*argv);

خارجي فراغ compile_substitute(حرف*label, بنانشان ويڪر,
			       بنانشان soff, بنانشان swidth,
			       بنانشان argc, بنية symb_s*argv);

/*
 * Arrange for the system task/function call to have its compiletf
 * function called.
 */
خارجي فراغ compile_compiletf(بنية __vpiSysTaskCall*);

/*
 * Compile delay nodes of various form.
 */
خارجي فراغ compile_delay(حرف*label, بنانشان ويڪر,
                          vvp_delay_t*del, بنية symb_s داخلا);
خارجي فراغ compile_delay(حرف*label, بنانشان ويڪر,
                          بنانشان argc, بنية symb_s*argv,
                          bool ignore_decay);

/*
 * This is called by the parser to create a part select node.
 * See the PART SELECT STATEMENT section in the README.txt
 */
خارجي فراغ compile_part_select(حرف*label, حرف*src,
				بنانشان بنياد, بنانشان wid);
خارجي فراغ compile_part_select_pv(حرف*label, حرف*src,
				   بنانشان بنياد, بنانشان wid,
				   بنانشان vec_wid);
خارجي فراغ compile_part_select_var(حرف*label, حرف*src,
                                    حرف*var, بنانشان wid, bool is_signed);

/*
 * This is called by the parser to make the various arithmetic and
 * comparison functors.
 */
خارجي فراغ compile_arith_pow(حرف*label, ڊگهو ويڪر, bool signed_flag,
			      بنانشان argc, بنية symb_s*argv);
خارجي فراغ compile_arith_abs(حرف*label,
			      بنانشان argc, بنية symb_s*argv);
خارجي فراغ compile_arith_cast_int(حرف*label, ڊگهو ويڪر,
			           بنانشان argc, بنية symb_s*argv);
خارجي فراغ compile_arith_cast_real(حرف*label, bool signed_flag,
			            بنانشان argc, بنية symb_s*argv);
خارجي فراغ compile_arith_cast_vec2(حرف*label, ڊگهو ويڪر,
				    بنانشان argc, بنية symb_s*argv);
خارجي فراغ compile_arith_div(حرف*label, ڊگهو ويڪر, bool signed_flag,
			      بنانشان argc, بنية symb_s*argv);
خارجي فراغ compile_arith_mod(حرف*label, ڊگهو ويڪر, bool signed_flag,
			      بنانشان argc, بنية symb_s*argv);
خارجي فراغ compile_arith_mult(حرف*label, ڊگهو ويڪر,
			       بنانشان argc, بنية symb_s*argv);
خارجي فراغ compile_arith_جوڙ(حرف*label, ڊگهو ويڪر,
			      بنانشان argc, بنية symb_s*argv);
خارجي فراغ compile_arith_sub(حرف*label, ڊگهو ويڪر,
			      بنانشان argc, بنية symb_s*argv);
خارجي فراغ compile_cmp_eeq(حرف*label, ڊگهو ويڪر,
			   بنانشان argc, بنية symb_s*argv);
خارجي فراغ compile_cmp_nee(حرف*label, ڊگهو ويڪر,
			   بنانشان argc, بنية symb_s*argv);
خارجي فراغ compile_cmp_eq(حرف*label, ڊگهو ويڪر,
			   بنانشان argc, بنية symb_s*argv);
خارجي فراغ compile_cmp_eqx(حرف*label, ڊگهو ويڪر,
			    بنانشان argc, بنية symb_s*argv);
خارجي فراغ compile_cmp_eqz(حرف*label, ڊگهو ويڪر,
			    بنانشان argc, بنية symb_s*argv);
خارجي فراغ compile_cmp_ne(حرف*label, ڊگهو ويڪر,
			   بنانشان argc, بنية symb_s*argv);
خارجي فراغ compile_cmp_ge(حرف*label, ڊگهو ويڪر, bool signed_flag,
			   بنانشان argc, بنية symb_s*argv);
خارجي فراغ compile_cmp_gt(حرف*label, ڊگهو ويڪر, bool signed_flag,
			   بنانشان argc, بنية symb_s*argv);
خارجي فراغ compile_cmp_weq(حرف*label, ڊگهو ويڪر,
			   بنانشان argc, بنية symb_s*argv);
خارجي فراغ compile_cmp_wne(حرف*label, ڊگهو ويڪر,
			   بنانشان argc, بنية symb_s*argv);

خارجي فراغ compile_arith_mult_r(حرف*label, بنانشان argc,
                                 بنية symb_s*argv);
خارجي فراغ compile_arith_pow_r(حرف*label, بنانشان argc, بنية symb_s*argv);
خارجي فراغ compile_arith_div_r(حرف*label, بنانشان argc, بنية symb_s*argv);
خارجي فراغ compile_arith_mod_r(حرف*label, بنانشان argc, بنية symb_s*argv);
خارجي فراغ compile_arith_جوڙ_r(حرف*label, بنانشان argc, بنية symb_s*argv);
خارجي فراغ compile_arith_sub_r(حرف*label, بنانشان argc, بنية symb_s*argv);
خارجي فراغ compile_cmp_eq_r(حرف*label, بنانشان argc, بنية symb_s*argv);
خارجي فراغ compile_cmp_ne_r(حرف*label, بنانشان argc, بنية symb_s*argv);
خارجي فراغ compile_cmp_ge_r(حرف*label, بنانشان argc, بنية symb_s*argv);
خارجي فراغ compile_cmp_gt_r(حرف*label, بنانشان argc, بنية symb_s*argv);

خارجي فراغ compile_dff(حرف*label, بنانشان ويڪر, bool negedge,
			بنية symb_s arg_d,
			بنية symb_s arg_c,
			بنية symb_s arg_e);

خارجي فراغ compile_dff_aclr(حرف*label, بنانشان ويڪر, bool negedge,
			     بنية symb_s arg_d,
			     بنية symb_s arg_c,
			     بنية symb_s arg_e,
			     بنية symb_s arg_a);
خارجي فراغ compile_dff_aset(حرف*label, بنانشان ويڪر, bool negedge,
			     بنية symb_s arg_d,
			     بنية symb_s arg_c,
			     بنية symb_s arg_e,
			     بنية symb_s arg_a,
			     حرف*asc_قدر);

خارجي فراغ compile_latch(حرف*label, بنانشان ويڪر,
			  بنية symb_s arg_d,
			  بنية symb_s arg_e);

خارجي فراغ compile_enum2_type(حرف*label, ڊگهو ويڪر, bool signed_flag,
			      std::list<بنية enum_name_s>*نالا);
خارجي فراغ compile_enum4_type(حرف*label, ڊگهو ويڪر, bool signed_flag,
			      std::list<بنية enum_name_s>*نالا);

بنية __vpiModPath;
خارجي __vpiModPath* compile_modpath(حرف*label,
                                     بنانشان ويڪر,
				     بنية symb_s drv,
				     بنية symb_s dest);
خارجي فراغ compile_modpath_src(__vpiModPath*dst,
				حرف ڪنارو_c,
				ثابت بنية symb_s&src,
				بنية numbv_s&vals,
				ثابت بنية symb_s&condit_src,
				ثابت بنية symb_s&رستو_رڪن_اندريون);
خارجي فراغ compile_modpath_src(__vpiModPath*dst,
				حرف ڪنارو_c,
				ثابت بنية symb_s&src,
				بنية numbv_s&vals,
				صحيح condit_src, /* match with '0' */
				ثابت بنية symb_s&رستو_رڪن_اندريون,
				bool ifnone);

خارجي فراغ compile_reduce_and(حرف*label, ثابت بنية symb_s&arg);
خارجي فراغ compile_reduce_or(حرف*label, ثابت بنية symb_s&arg);
خارجي فراغ compile_reduce_xor(حرف*label, ثابت بنية symb_s&arg);
خارجي فراغ compile_reduce_nand(حرف*label, ثابت بنية symb_s&arg);
خارجي فراغ compile_reduce_nor(حرف*label, ثابت بنية symb_s&arg);
خارجي فراغ compile_reduce_xnor(حرف*label, ثابت بنية symb_s&arg);

خارجي فراغ compile_extend_signed(حرف*label, ڊگهو ويڪر, بنية symb_s arg);

خارجي فراغ compile_sfunc(حرف*label, حرف*name, حرف*format_string,
			  ڊگهو file_idx, ڊگهو lineno,
			  بنانشان argc, بنية symb_s*argv,
                          حرف*trigger_label);

خارجي فراغ compile_repeat(حرف*label, ڊگهو ويڪر, ڊگهو repeat,
			   بنية symb_s arg);

خارجي فراغ compile_shiftl(حرف*label, ڊگهو ويڪر,
			   بنانشان argc, بنية symb_s*argv);
خارجي فراغ compile_shiftr(حرف*label, ڊگهو ويڪر, bool signed_flag,
			   بنانشان argc, بنية symb_s*argv);

خارجي فراغ compile_timescale(ڊگهو units, ڊگهو precision);

خارجي فراغ compile_vpi_نشان(ثابت حرف*label, vpiHandle obj);
خارجي فراغ compile_vpi_lookup(vpiHandle *objref, حرف*label);

خارجي فراغ compile_param_string(حرف*label, ثابت حرف*name, حرف*قدر,
                                 bool local_flag,
                                 ڊگهو file_idx, ڊگهو lineno);
خارجي فراغ compile_param_logic(حرف*label, ثابت حرف*name, حرف*قدر,
				bool signed_flag, bool local_flag,
                                ڊگهو file_idx, ڊگهو lineno);
خارجي فراغ compile_param_real(حرف*label, ثابت حرف*name, حرف*قدر,
                               bool local_flag,
                               ڊگهو file_idx, ڊگهو lineno);

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

    عام:
      explicit resolv_list_s(حرف*lab) : label_(lab) {
	    ايندڙ_2 = عدم;
      }
      مجازي ~resolv_list_s();

      resolv_list_s(ثابت resolv_list_s&) = delete;
      resolv_list_s& عامل=(ثابت resolv_list_s&) = delete;

      مجازي bool resolve(bool mes = false) = 0;

    محمي:
      ثابت حرف*label() ثابت { أرجع label_; }

    خاص:
      دوست فراغ resolv_submit(class resolv_list_s*cur);
      دوست فراغ compile_cleanup(فراغ);

      حرف*label_;
      class resolv_list_s*ايندڙ_2;
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
خارجي فراغ functor_ref_lookup(vvp_net_t**ref, حرف*lab);

خارجي vpiHandle vpip_make_prop_قطار_ref(حرف* class_label,
                                          بنانشان prop_idx,
                                          بنانشان is_قطار_flag);

/*
 * This function schedules a lookup of the labeled instruction. The
 * code points to a code structure that points to the instruction
 * field that receives the result, and the label is the name to
 * lookup. The lookup will free the label text when it is done.
 *
 * The cptr2 flag tells the lookup to write the code pointer into the
 * cptr2 member of the code, instead of the cptr member.
 */
خارجي فراغ ڪوڊ_label_lookup(بنية vvp_ڪوڊ_s *ڪوڊ, حرف *label, bool cptr2);

/*
 * The `compile_udp_def' function creates a UDP.  The `table' is a
 * NULL terminated array of char*, as assembled by `compile_udp_table'.
 * `compile_udp_table' is called with `table'==NULL to create a new
 * table, or with an existing table to append to.
 *
 * `compile_udp_functor' creates a mode-3 functor referring to the
 * labeled UDP.
 */

خارجي فراغ compile_udp_def(صحيح sequ, حرف*label, حرف *name,
			    بنانشان nin, بنانشان init, حرف **جدول);

خارجي فراغ compile_udp_functor(حرف*label, حرف*type,
				بنانشان argc, بنية symb_s*argv);

خارجي حرف **compile_udp_جدول(حرف **جدول, حرف *صف);

/*
 * Memory Instances, Ports, and Initialization
 */

خارجي فراغ compile_var_array(حرف*label, حرف*name,
			      صحيح آخري, صحيح پهريون,
			      صحيح msb, صحيح lsb, حرف signed_flag);
خارجي فراغ compile_var2_array(حرف*label, حرف*name,
			      صحيح آخري, صحيح پهريون,
			      صحيح msb, صحيح lsb, bool signed_flag);
خارجي فراغ compile_real_array(حرف*label, حرف*name,
			       صحيح آخري, صحيح پهريون);
خارجي فراغ compile_string_array(حرف*label, حرف*name,
				 صحيح آخري, صحيح پهريون);
خارجي فراغ compile_object_array(حرف*label, حرف*name,
				 صحيح آخري, صحيح پهريون);
خارجي فراغ compile_net_array(حرف*label, حرف*name,
			      صحيح آخري, صحيح پهريون);
خارجي فراغ compile_array_alias(حرف*label, حرف*name, حرف*src);

  /* Index is a net. */
خارجي فراغ compile_array_پورٽ(حرف*label, حرف*name, حرف*addr);
  /* Index is a constant address */
خارجي فراغ compile_array_پورٽ(حرف*label, حرف*name, ڊگهو addr);

خارجي فراغ compile_array_cleanup(فراغ);

/*
 * Compile the .ufunc statement.
 */
خارجي فراغ compile_ufunc_real(حرف*label, حرف*ڪوڊ, بنانشان wid,
			  بنانشان argc, بنية symb_s*argv,
			  بنانشان portc, بنية symb_s*portv,
			  حرف*scope_label, حرف*trigger_label);
خارجي فراغ compile_ufunc_vec4(حرف*label, حرف*ڪوڊ, بنانشان wid,
			  بنانشان argc, بنية symb_s*argv,
			  بنانشان portc, بنية symb_s*portv,
			  حرف*scope_label, حرف*trigger_label);

/*
 * The compile_event function takes the parts of the event statement
 * and makes the various objects needed to simulate it. This includes
 * the functor that receives the signals and the event_t that holds
 * the threads.
 */
خارجي فراغ compile_event(حرف*label, حرف*type,
			  بنانشان argc, بنية symb_s*argv);
خارجي فراغ compile_named_event(حرف*label, حرف*type, bool local_flag=false);


/*
 * A code statement is a label, an opcode and up to 3 operands. There
 * are a few lexical types that the parser recognizes of the operands,
 * given by the ltype_e enumeration. The compile_code function takes
 * the label, mnemonic and parsed operands and writes a properly
 * formed instruction into the code space. The label is set into the
 * symbol table with the address of the instruction.
 */

#وصف_ڪريو عمليو_MAX 3
تعداد ltype_e { L_NUMB, L_SYMB, L_STRING };

بنية comp_operands_s {
      بنانشان argc;
      بنية {
	    تعداد ltype_e ltype;
	    اتحاد {
		  بنانشان ڊگهو numb;
		  بنية symb_s symb;
		  ثابت حرف   *متن;
	    };
      } argv[عمليو_MAX];
};

تعريفنوع بنية comp_operands_s*comp_operands_t;

خارجي فراغ compile_ڪوڊ(حرف*label, حرف*mnem, comp_operands_t opa);

خارجي فراغ compile_file_سٽ(حرف*label, ڊگهو file_idx, ڊگهو lineno,
                              حرف*description);

خارجي فراغ compile_vpi_call(حرف*label, حرف*name,
			     bool func_as_task_err, bool func_as_task_warn,
			     ڊگهو file_idx, ڊگهو lineno,
			     بنانشان argc, vpiHandle*argv,
			     بنانشان vec4_ڍير,
			     بنانشان real_ڍير,
			     بنانشان string_ڍير);

/* Compile a function call. The vbit and vwid encode the return
   type. If the vwid >0, the return type is a vector. If the vwid is
   <0, the return type is -vpiRealConst or some other constant subtype
   code that represents the function type. */
خارجي فراغ compile_vpi_func_call(حرف*label, حرف*name,
				  صحيح val_type, بنانشان val_wid,
				  ڊگهو file_idx, ڊگهو lineno,
				  بنانشان argc, vpiHandle*argv,
				  بنانشان vec4_ڍير,
				  بنانشان real_ڍير,
				  بنانشان string_ڍير);
خارجي فراغ print_vpi_call_errors();

خارجي فراغ compile_codelabel(حرف*label);

/*
 * The parser uses these functions to compile .scope statements.
 * The implementations of these live in the vpi_scope.cc file.
 */
خارجي فراغ compile_scope_decl(حرف*typ, حرف*lab, حرف*nam, حرف*tnam,
                               حرف*par, ڊگهو file_idx, ڊگهو lineno,
                               ڊگهو def_file_idx, ڊگهو def_lineno,
                               ڊگهو is_cell);
خارجي فراغ compile_scope_recall(حرف*sym);

/*
 * The parser uses this function to declare a thread. The start_sym is
 * the start instruction, and must already be defined.
 */
خارجي فراغ compile_thread(حرف*شروعات_sym, حرف*flag);

/*
 * This function is called to create a var vector with the given name.
 *
 * The vpi_type_code argument of compile_variable() is one of the vpi
 * object codes that identify the type: vpiReg, vpiIntegerVar,
 * vpiIntVar, etc.
 */
خارجي فراغ compile_variable(حرف*label, حرف*name,
			     صحيح msb, صحيح lsb, صحيح vpi_type_ڪوڊ,
			     bool signed_flag, bool local_flag);

خارجي فراغ compile_var_real(حرف*label, حرف*name, bool local_flag);
خارجي فراغ compile_var_string(حرف*label, حرف*name);
خارجي فراغ compile_var_darray(حرف*label, حرف*name, بنانشان سائيز);
خارجي فراغ compile_var_cobject(حرف*label, حرف*name);
خارجي فراغ compile_var_قطار(حرف*label, حرف*name, بنانشان سائيز);

/*
 * This function is used to create a scope port
 * Current ONLY module ports are supported and ports exist purely
 * as meta-data for VPI queries (i.e. there is NO corresponding net etc)
 * as elaboration internally eliminates port-nets by directly connecting
 * nets connected through module ports.
 */

خارجي فراغ compile_پورٽ_info( بنانشان اشاريو, صحيح vpi_پورٽ_type, بنانشان ويڪر, ثابت حرف *name, حرف* بفر );


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
خارجي فراغ compile_net(حرف*label, حرف*name, صحيح msb, صحيح lsb,
			صحيح vpi_type_ڪوڊ, bool signed_flag, bool local_flag,
			بنانشان argc, بنية symb_s*argv);

خارجي فراغ compile_net_real(حرف*label, حرف*name,
			     صحيح msb, صحيح lsb, bool local_flag,
			     بنانشان argc, بنية symb_s*argv);

خارجي فراغ compile_netw(حرف*label, حرف*array_label, بنانشان ڊگهو array_addr,
			 صحيح msb, صحيح lsb, صحيح vpi_type_ڪوڊ, bool signed_flag,
			 بنانشان argc, بنية symb_s*argv);

خارجي فراغ compile_netw_real(حرف*label, حرف*array_نشان,
			      بنانشان ڊگهو array_addr,
			      صحيح msb, صحيح lsb,
			      بنانشان argc, بنية symb_s*argv);


خارجي فراغ compile_island(حرف*label, حرف*type);
خارجي فراغ compile_island_پورٽ(حرف*label, حرف*island, حرف*src);
خارجي فراغ compile_island_import(حرف*label, حرف*island, حرف*src);
خارجي فراغ compile_island_export(حرف*label, حرف*island);
خارجي فراغ compile_island_cleanup(فراغ);

خارجي فراغ compile_island_tran(حرف*label);
خارجي فراغ compile_island_tranif(صحيح sense, حرف*island,
				  حرف*ba, حرف*bb, حرف*src, bool resistive);
خارجي فراغ compile_island_tranvp(حرف*island, حرف*ba, حرف*bb,
				  بنانشان ويڪر, بنانشان part, بنانشان off);

خارجي فراغ delete_udp_symbols(فراغ);

خارجي فراغ compile_class_شروعات(حرف*lab, حرف*nam, بنانشان nprop);
خارجي فراغ compile_class_property(بنانشان idx, حرف*nam, حرف*typ, uint64_t array_سائيز_2);
خارجي فراغ compile_class_پورو(فراغ);

#جيڪڏهن_پورو /* IVL_compile_H */
