#اگر_غیر_تعریف_شدہ IVL_compile_H
#تعریف_کرو IVL_compile_H
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
خارجی std::سمتیہ<ثابت حرف*> file_نام;

/*
 * The functions described here are the compile time support
 * functions. Various bits of the compile process are taken care of
 * here. What is called when is mostly controlled by the parser.
 *
 * Before compilation takes place, the compile_init function must be
 * called once to set stuff up.
 */

خارجی خالی compile_init(خالی);

خارجی خالی compile_cleanup(خالی);

خارجی bool verbose_flag;

/*
 * If this file opened, then write debug information to this
 * file. This is used for debugging the VVP runtime itself.
 */
خارجی std::ofstream debug_file;

/*
 *  Connect a list of symbols to a contiguous set of ipoints.
 *  Constants C<?> are handled by setting the ival of the ipoint.
 */
خارجی خالی inputs_connect(vvp_net_t*fdx, غیرعلامتی argc, ساخت symb_s*argv);
خارجی خالی داخلہ_connect(vvp_net_t*fdx, غیرعلامتی پورٹ, حرف*label);

/*
 * This function is an expansion of the inputs_connect function. It
 * uses the inputs_connect function, but it creates vvp_wide_fun_t
 * nodes to handle arbitrary width nodes, and connects those nodes to
 * the vvp_wide_fun_core object passed in.
 */
خارجی خالی wide_inputs_connect(vvp_wide_fun_core*core,
				غیرعلامتی argc, ساخت symb_s*argv);

خارجی vvp_net_t* vvp_net_lookup(ثابت حرف*label);
خارجی vpiHandle vvp_lookup_handle(ثابت حرف*label);

/*
 *  Add a functor to the symbol table
 */
خارجی خالی define_functor_علامت(ثابت حرف*label, vvp_net_t*ipt);


/*
 * This is a count of errors encountered during compilation. If this
 * is non-zero, then simulation is not recommended.
 */
خارجی غیرعلامتی compile_errors;

خارجی ثابت حرف* module_راستہ;
خارجی خالی compile_بھرو_vpi_module(حرف*name);

خارجی خالی compile_vpi_وقت_precision(طویل pre);

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
خارجی خالی compile_functor(حرف*label, حرف*type, غیرعلامتی چوڑائی,
			    غیرعلامتی ostr0, غیرعلامتی ostr1,
			    غیرعلامتی argc, ساخت symb_s*argv);


/*
 * This is called by the parser to make a resolver. This is a special
 * kind of functor; a strength aware functor. It has up to 4 inputs
 * that are blended to make a resolved output. The type string selects
 * a resolution algorithm.
 */
خارجی خالی compile_resolver(حرف*label, حرف*type,
			     غیرعلامتی argc, ساخت symb_s*argv);

خارجی خالی compile_concat(حرف*label, غیرعلامتی w0, غیرعلامتی w1,
			   غیرعلامتی w2, غیرعلامتی w3,
			   غیرعلامتی argc, ساخت symb_s*argv);

خارجی خالی compile_concat8(حرف*label, غیرعلامتی w0, غیرعلامتی w1,
			    غیرعلامتی w2, غیرعلامتی w3,
			    غیرعلامتی argc, ساخت symb_s*argv);

خارجی خالی compile_substitute(حرف*label, غیرعلامتی چوڑائی,
			       غیرعلامتی soff, غیرعلامتی swidth,
			       غیرعلامتی argc, ساخت symb_s*argv);

/*
 * Arrange for the system task/function call to have its compiletf
 * function called.
 */
خارجی خالی compile_compiletf(ساخت __vpiSysTaskCall*);

/*
 * Compile delay nodes of various form.
 */
خارجی خالی compile_delay(حرف*label, غیرعلامتی چوڑائی,
                          vvp_delay_t*del, ساخت symb_s داخلہ);
خارجی خالی compile_delay(حرف*label, غیرعلامتی چوڑائی,
                          غیرعلامتی argc, ساخت symb_s*argv,
                          bool ignore_decay);

/*
 * This is called by the parser to create a part select node.
 * See the PART SELECT STATEMENT section in the README.txt
 */
خارجی خالی compile_part_select(حرف*label, حرف*src,
				غیرعلامتی بنیاد, غیرعلامتی wid);
خارجی خالی compile_part_select_pv(حرف*label, حرف*src,
				   غیرعلامتی بنیاد, غیرعلامتی wid,
				   غیرعلامتی vec_wid);
خارجی خالی compile_part_select_var(حرف*label, حرف*src,
                                    حرف*var, غیرعلامتی wid, bool is_signed);

/*
 * This is called by the parser to make the various arithmetic and
 * comparison functors.
 */
خارجی خالی compile_arith_pow(حرف*label, طویل چوڑائی, bool signed_flag,
			      غیرعلامتی argc, ساخت symb_s*argv);
خارجی خالی compile_arith_abs(حرف*label,
			      غیرعلامتی argc, ساخت symb_s*argv);
خارجی خالی compile_arith_cast_int(حرف*label, طویل چوڑائی,
			           غیرعلامتی argc, ساخت symb_s*argv);
خارجی خالی compile_arith_cast_real(حرف*label, bool signed_flag,
			            غیرعلامتی argc, ساخت symb_s*argv);
خارجی خالی compile_arith_cast_vec2(حرف*label, طویل چوڑائی,
				    غیرعلامتی argc, ساخت symb_s*argv);
خارجی خالی compile_arith_div(حرف*label, طویل چوڑائی, bool signed_flag,
			      غیرعلامتی argc, ساخت symb_s*argv);
خارجی خالی compile_arith_mod(حرف*label, طویل چوڑائی, bool signed_flag,
			      غیرعلامتی argc, ساخت symb_s*argv);
خارجی خالی compile_arith_mult(حرف*label, طویل چوڑائی,
			       غیرعلامتی argc, ساخت symb_s*argv);
خارجی خالی compile_arith_مجموع(حرف*label, طویل چوڑائی,
			      غیرعلامتی argc, ساخت symb_s*argv);
خارجی خالی compile_arith_sub(حرف*label, طویل چوڑائی,
			      غیرعلامتی argc, ساخت symb_s*argv);
خارجی خالی compile_cmp_eeq(حرف*label, طویل چوڑائی,
			   غیرعلامتی argc, ساخت symb_s*argv);
خارجی خالی compile_cmp_nee(حرف*label, طویل چوڑائی,
			   غیرعلامتی argc, ساخت symb_s*argv);
خارجی خالی compile_cmp_eq(حرف*label, طویل چوڑائی,
			   غیرعلامتی argc, ساخت symb_s*argv);
خارجی خالی compile_cmp_eqx(حرف*label, طویل چوڑائی,
			    غیرعلامتی argc, ساخت symb_s*argv);
خارجی خالی compile_cmp_eqz(حرف*label, طویل چوڑائی,
			    غیرعلامتی argc, ساخت symb_s*argv);
خارجی خالی compile_cmp_ne(حرف*label, طویل چوڑائی,
			   غیرعلامتی argc, ساخت symb_s*argv);
خارجی خالی compile_cmp_ge(حرف*label, طویل چوڑائی, bool signed_flag,
			   غیرعلامتی argc, ساخت symb_s*argv);
خارجی خالی compile_cmp_gt(حرف*label, طویل چوڑائی, bool signed_flag,
			   غیرعلامتی argc, ساخت symb_s*argv);
خارجی خالی compile_cmp_weq(حرف*label, طویل چوڑائی,
			   غیرعلامتی argc, ساخت symb_s*argv);
خارجی خالی compile_cmp_wne(حرف*label, طویل چوڑائی,
			   غیرعلامتی argc, ساخت symb_s*argv);

خارجی خالی compile_arith_mult_r(حرف*label, غیرعلامتی argc,
                                 ساخت symb_s*argv);
خارجی خالی compile_arith_pow_r(حرف*label, غیرعلامتی argc, ساخت symb_s*argv);
خارجی خالی compile_arith_div_r(حرف*label, غیرعلامتی argc, ساخت symb_s*argv);
خارجی خالی compile_arith_mod_r(حرف*label, غیرعلامتی argc, ساخت symb_s*argv);
خارجی خالی compile_arith_مجموع_r(حرف*label, غیرعلامتی argc, ساخت symb_s*argv);
خارجی خالی compile_arith_sub_r(حرف*label, غیرعلامتی argc, ساخت symb_s*argv);
خارجی خالی compile_cmp_eq_r(حرف*label, غیرعلامتی argc, ساخت symb_s*argv);
خارجی خالی compile_cmp_ne_r(حرف*label, غیرعلامتی argc, ساخت symb_s*argv);
خارجی خالی compile_cmp_ge_r(حرف*label, غیرعلامتی argc, ساخت symb_s*argv);
خارجی خالی compile_cmp_gt_r(حرف*label, غیرعلامتی argc, ساخت symb_s*argv);

خارجی خالی compile_dff(حرف*label, غیرعلامتی چوڑائی, bool negedge,
			ساخت symb_s arg_d,
			ساخت symb_s arg_c,
			ساخت symb_s arg_e);

خارجی خالی compile_dff_aclr(حرف*label, غیرعلامتی چوڑائی, bool negedge,
			     ساخت symb_s arg_d,
			     ساخت symb_s arg_c,
			     ساخت symb_s arg_e,
			     ساخت symb_s arg_a);
خارجی خالی compile_dff_aset(حرف*label, غیرعلامتی چوڑائی, bool negedge,
			     ساخت symb_s arg_d,
			     ساخت symb_s arg_c,
			     ساخت symb_s arg_e,
			     ساخت symb_s arg_a,
			     حرف*asc_قدر);

خارجی خالی compile_latch(حرف*label, غیرعلامتی چوڑائی,
			  ساخت symb_s arg_d,
			  ساخت symb_s arg_e);

خارجی خالی compile_enum2_type(حرف*label, طویل چوڑائی, bool signed_flag,
			      std::list<ساخت enum_name_s>*نام);
خارجی خالی compile_enum4_type(حرف*label, طویل چوڑائی, bool signed_flag,
			      std::list<ساخت enum_name_s>*نام);

ساخت __vpiModPath;
خارجی __vpiModPath* compile_modpath(حرف*label,
                                     غیرعلامتی چوڑائی,
				     ساخت symb_s drv,
				     ساخت symb_s dest);
خارجی خالی compile_modpath_src(__vpiModPath*dst,
				حرف کنارہ_c,
				ثابت ساخت symb_s&src,
				ساخت numbv_s&vals,
				ثابت ساخت symb_s&condit_src,
				ثابت ساخت symb_s&راستہ_رکن_اندر);
خارجی خالی compile_modpath_src(__vpiModPath*dst,
				حرف کنارہ_c,
				ثابت ساخت symb_s&src,
				ساخت numbv_s&vals,
				صحیح condit_src, /* match with '0' */
				ثابت ساخت symb_s&راستہ_رکن_اندر,
				bool ifnone);

خارجی خالی compile_reduce_and(حرف*label, ثابت ساخت symb_s&arg);
خارجی خالی compile_reduce_or(حرف*label, ثابت ساخت symb_s&arg);
خارجی خالی compile_reduce_xor(حرف*label, ثابت ساخت symb_s&arg);
خارجی خالی compile_reduce_nand(حرف*label, ثابت ساخت symb_s&arg);
خارجی خالی compile_reduce_nor(حرف*label, ثابت ساخت symb_s&arg);
خارجی خالی compile_reduce_xnor(حرف*label, ثابت ساخت symb_s&arg);

خارجی خالی compile_extend_signed(حرف*label, طویل چوڑائی, ساخت symb_s arg);

خارجی خالی compile_sfunc(حرف*label, حرف*name, حرف*format_string,
			  طویل file_idx, طویل lineno,
			  غیرعلامتی argc, ساخت symb_s*argv,
                          حرف*trigger_label);

خارجی خالی compile_repeat(حرف*label, طویل چوڑائی, طویل repeat,
			   ساخت symb_s arg);

خارجی خالی compile_shiftl(حرف*label, طویل چوڑائی,
			   غیرعلامتی argc, ساخت symb_s*argv);
خارجی خالی compile_shiftr(حرف*label, طویل چوڑائی, bool signed_flag,
			   غیرعلامتی argc, ساخت symb_s*argv);

خارجی خالی compile_timescale(طویل units, طویل precision);

خارجی خالی compile_vpi_علامت(ثابت حرف*label, vpiHandle obj);
خارجی خالی compile_vpi_lookup(vpiHandle *objref, حرف*label);

خارجی خالی compile_param_string(حرف*label, ثابت حرف*name, حرف*قدر,
                                 bool local_flag,
                                 طویل file_idx, طویل lineno);
خارجی خالی compile_param_logic(حرف*label, ثابت حرف*name, حرف*قدر,
				bool signed_flag, bool local_flag,
                                طویل file_idx, طویل lineno);
خارجی خالی compile_param_real(حرف*label, ثابت حرف*name, حرف*قدر,
                               bool local_flag,
                               طویل file_idx, طویل lineno);

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

    عوامی:
      explicit resolv_list_s(حرف*lab) : label_(lab) {
	    اگلا_2 = تہی;
      }
      مجازی ~resolv_list_s();

      resolv_list_s(ثابت resolv_list_s&) = delete;
      resolv_list_s& عامل=(ثابت resolv_list_s&) = delete;

      مجازی bool resolve(bool mes = false) = 0;

    محفوظ:
      ثابت حرف*label() ثابت { واپس label_; }

    نجی:
      دوست خالی resolv_submit(class resolv_list_s*cur);
      دوست خالی compile_cleanup(خالی);

      حرف*label_;
      class resolv_list_s*اگلا_2;
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
خارجی خالی functor_ref_lookup(vvp_net_t**ref, حرف*lab);

خارجی vpiHandle vpip_make_prop_قطار_ref(حرف* class_label,
                                          غیرعلامتی prop_idx,
                                          غیرعلامتی is_قطار_flag);

/*
 * This function schedules a lookup of the labeled instruction. The
 * code points to a code structure that points to the instruction
 * field that receives the result, and the label is the name to
 * lookup. The lookup will free the label text when it is done.
 *
 * The cptr2 flag tells the lookup to write the code pointer into the
 * cptr2 member of the code, instead of the cptr member.
 */
خارجی خالی رمز_label_lookup(ساخت vvp_رمز_s *رمز, حرف *label, bool cptr2);

/*
 * The `compile_udp_def' function creates a UDP.  The `table' is a
 * NULL terminated array of char*, as assembled by `compile_udp_table'.
 * `compile_udp_table' is called with `table'==NULL to create a new
 * table, or with an existing table to append to.
 *
 * `compile_udp_functor' creates a mode-3 functor referring to the
 * labeled UDP.
 */

خارجی خالی compile_udp_def(صحیح sequ, حرف*label, حرف *name,
			    غیرعلامتی nin, غیرعلامتی init, حرف **جدول);

خارجی خالی compile_udp_functor(حرف*label, حرف*type,
				غیرعلامتی argc, ساخت symb_s*argv);

خارجی حرف **compile_udp_جدول(حرف **جدول, حرف *صف);

/*
 * Memory Instances, Ports, and Initialization
 */

خارجی خالی compile_var_array(حرف*label, حرف*name,
			      صحیح آخری, صحیح پہلا,
			      صحیح msb, صحیح lsb, حرف signed_flag);
خارجی خالی compile_var2_array(حرف*label, حرف*name,
			      صحیح آخری, صحیح پہلا,
			      صحیح msb, صحیح lsb, bool signed_flag);
خارجی خالی compile_real_array(حرف*label, حرف*name,
			       صحیح آخری, صحیح پہلا);
خارجی خالی compile_string_array(حرف*label, حرف*name,
				 صحیح آخری, صحیح پہلا);
خارجی خالی compile_object_array(حرف*label, حرف*name,
				 صحیح آخری, صحیح پہلا);
خارجی خالی compile_net_array(حرف*label, حرف*name,
			      صحیح آخری, صحیح پہلا);
خارجی خالی compile_array_alias(حرف*label, حرف*name, حرف*src);

  /* Index is a net. */
خارجی خالی compile_array_پورٹ(حرف*label, حرف*name, حرف*addr);
  /* Index is a constant address */
خارجی خالی compile_array_پورٹ(حرف*label, حرف*name, طویل addr);

خارجی خالی compile_array_cleanup(خالی);

/*
 * Compile the .ufunc statement.
 */
خارجی خالی compile_ufunc_real(حرف*label, حرف*رمز, غیرعلامتی wid,
			  غیرعلامتی argc, ساخت symb_s*argv,
			  غیرعلامتی portc, ساخت symb_s*portv,
			  حرف*scope_label, حرف*trigger_label);
خارجی خالی compile_ufunc_vec4(حرف*label, حرف*رمز, غیرعلامتی wid,
			  غیرعلامتی argc, ساخت symb_s*argv,
			  غیرعلامتی portc, ساخت symb_s*portv,
			  حرف*scope_label, حرف*trigger_label);

/*
 * The compile_event function takes the parts of the event statement
 * and makes the various objects needed to simulate it. This includes
 * the functor that receives the signals and the event_t that holds
 * the threads.
 */
خارجی خالی compile_event(حرف*label, حرف*type,
			  غیرعلامتی argc, ساخت symb_s*argv);
خارجی خالی compile_named_event(حرف*label, حرف*type, bool local_flag=false);


/*
 * A code statement is a label, an opcode and up to 3 operands. There
 * are a few lexical types that the parser recognizes of the operands,
 * given by the ltype_e enumeration. The compile_code function takes
 * the label, mnemonic and parsed operands and writes a properly
 * formed instruction into the code space. The label is set into the
 * symbol table with the address of the instruction.
 */

#تعریف_کرو عملیہ_MAX 3
شمار ltype_e { L_NUMB, L_SYMB, L_STRING };

ساخت comp_operands_s {
      غیرعلامتی argc;
      ساخت {
	    شمار ltype_e ltype;
	    اتحاد {
		  غیرعلامتی طویل numb;
		  ساخت symb_s symb;
		  ثابت حرف   *متن;
	    };
      } argv[عملیہ_MAX];
};

قسمتعریف ساخت comp_operands_s*comp_operands_t;

خارجی خالی compile_رمز(حرف*label, حرف*mnem, comp_operands_t opa);

خارجی خالی compile_file_سطر(حرف*label, طویل file_idx, طویل lineno,
                              حرف*description);

خارجی خالی compile_vpi_call(حرف*label, حرف*name,
			     bool func_as_task_err, bool func_as_task_warn,
			     طویل file_idx, طویل lineno,
			     غیرعلامتی argc, vpiHandle*argv,
			     غیرعلامتی vec4_ڈھیر,
			     غیرعلامتی real_ڈھیر,
			     غیرعلامتی string_ڈھیر);

/* Compile a function call. The vbit and vwid encode the return
   type. If the vwid >0, the return type is a vector. If the vwid is
   <0, the return type is -vpiRealConst or some other constant subtype
   code that represents the function type. */
خارجی خالی compile_vpi_func_call(حرف*label, حرف*name,
				  صحیح val_type, غیرعلامتی val_wid,
				  طویل file_idx, طویل lineno,
				  غیرعلامتی argc, vpiHandle*argv,
				  غیرعلامتی vec4_ڈھیر,
				  غیرعلامتی real_ڈھیر,
				  غیرعلامتی string_ڈھیر);
خارجی خالی print_vpi_call_errors();

خارجی خالی compile_codelabel(حرف*label);

/*
 * The parser uses these functions to compile .scope statements.
 * The implementations of these live in the vpi_scope.cc file.
 */
خارجی خالی compile_scope_decl(حرف*typ, حرف*lab, حرف*nam, حرف*tnam,
                               حرف*par, طویل file_idx, طویل lineno,
                               طویل def_file_idx, طویل def_lineno,
                               طویل is_cell);
خارجی خالی compile_scope_recall(حرف*sym);

/*
 * The parser uses this function to declare a thread. The start_sym is
 * the start instruction, and must already be defined.
 */
خارجی خالی compile_thread(حرف*آغاز_sym, حرف*flag);

/*
 * This function is called to create a var vector with the given name.
 *
 * The vpi_type_code argument of compile_variable() is one of the vpi
 * object codes that identify the type: vpiReg, vpiIntegerVar,
 * vpiIntVar, etc.
 */
خارجی خالی compile_variable(حرف*label, حرف*name,
			     صحیح msb, صحیح lsb, صحیح vpi_type_رمز,
			     bool signed_flag, bool local_flag);

خارجی خالی compile_var_real(حرف*label, حرف*name, bool local_flag);
خارجی خالی compile_var_string(حرف*label, حرف*name);
خارجی خالی compile_var_darray(حرف*label, حرف*name, غیرعلامتی سائز);
خارجی خالی compile_var_cobject(حرف*label, حرف*name);
خارجی خالی compile_var_قطار(حرف*label, حرف*name, غیرعلامتی سائز);

/*
 * This function is used to create a scope port
 * Current ONLY module ports are supported and ports exist purely
 * as meta-data for VPI queries (i.e. there is NO corresponding net etc)
 * as elaboration internally eliminates port-nets by directly connecting
 * nets connected through module ports.
 */

خارجی خالی compile_پورٹ_info( غیرعلامتی اشاریہ, صحیح vpi_پورٹ_type, غیرعلامتی چوڑائی, ثابت حرف *name, حرف* بفر );


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
خارجی خالی compile_net(حرف*label, حرف*name, صحیح msb, صحیح lsb,
			صحیح vpi_type_رمز, bool signed_flag, bool local_flag,
			غیرعلامتی argc, ساخت symb_s*argv);

خارجی خالی compile_net_real(حرف*label, حرف*name,
			     صحیح msb, صحیح lsb, bool local_flag,
			     غیرعلامتی argc, ساخت symb_s*argv);

خارجی خالی compile_netw(حرف*label, حرف*array_label, غیرعلامتی طویل array_addr,
			 صحیح msb, صحیح lsb, صحیح vpi_type_رمز, bool signed_flag,
			 غیرعلامتی argc, ساخت symb_s*argv);

خارجی خالی compile_netw_real(حرف*label, حرف*array_علامت,
			      غیرعلامتی طویل array_addr,
			      صحیح msb, صحیح lsb,
			      غیرعلامتی argc, ساخت symb_s*argv);


خارجی خالی compile_island(حرف*label, حرف*type);
خارجی خالی compile_island_پورٹ(حرف*label, حرف*island, حرف*src);
خارجی خالی compile_island_import(حرف*label, حرف*island, حرف*src);
خارجی خالی compile_island_export(حرف*label, حرف*island);
خارجی خالی compile_island_cleanup(خالی);

خارجی خالی compile_island_tran(حرف*label);
خارجی خالی compile_island_tranif(صحیح sense, حرف*island,
				  حرف*ba, حرف*bb, حرف*src, bool resistive);
خارجی خالی compile_island_tranvp(حرف*island, حرف*ba, حرف*bb,
				  غیرعلامتی چوڑائی, غیرعلامتی part, غیرعلامتی off);

خارجی خالی delete_udp_symbols(خالی);

خارجی خالی compile_class_آغاز(حرف*lab, حرف*nam, غیرعلامتی nprop);
خارجی خالی compile_class_property(غیرعلامتی idx, حرف*nam, حرف*typ, uint64_t array_سائز_2);
خارجی خالی compile_class_مکمل(خالی);

#اگر_ختم /* IVL_compile_H */
