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
बाह्य std::vector<स्थिरांक अक्षर*> file_names;

/*
 * The functions described here are the compile time support
 * functions. Various bits of the compile process are taken care of
 * here. What is called when is mostly controlled by the parser.
 *
 * Before compilation takes place, the compile_init function must be
 * called once to set stuff up.
 */

बाह्य शून्य compile_init(शून्य);

बाह्य शून्य compile_cleanup(शून्य);

बाह्य bool verbose_flag;

/*
 * If this file opened, then write debug information to this
 * file. This is used for debugging the VVP runtime itself.
 */
बाह्य std::ofstream debug_file;

/*
 *  Connect a list of symbols to a contiguous set of ipoints.
 *  Constants C<?> are handled by setting the ival of the ipoint.
 */
बाह्य शून्य inputs_connect(vvp_net_t*fdx, अचिन्हित argc, संरचना symb_s*argv);
बाह्य शून्य आदान_connect(vvp_net_t*fdx, अचिन्हित port, अक्षर*label);

/*
 * This function is an expansion of the inputs_connect function. It
 * uses the inputs_connect function, but it creates vvp_wide_fun_t
 * nodes to handle arbitrary width nodes, and connects those nodes to
 * the vvp_wide_fun_core object passed in.
 */
बाह्य शून्य wide_inputs_connect(vvp_wide_fun_core*core,
				अचिन्हित argc, संरचना symb_s*argv);

बाह्य vvp_net_t* vvp_net_lookup(स्थिरांक अक्षर*label);
बाह्य vpiHandle vvp_lookup_handle(स्थिरांक अक्षर*label);

/*
 *  Add a functor to the symbol table
 */
बाह्य शून्य define_functor_चिन्ह(स्थिरांक अक्षर*label, vvp_net_t*ipt);


/*
 * This is a count of errors encountered during compilation. If this
 * is non-zero, then simulation is not recommended.
 */
बाह्य अचिन्हित compile_errors;

बाह्य स्थिरांक अक्षर* module_path;
बाह्य शून्य compile_load_vpi_module(अक्षर*name);

बाह्य शून्य compile_vpi_time_precision(दीर्घ pre);

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
बाह्य शून्य compile_functor(अक्षर*label, अक्षर*type, अचिन्हित width,
			    अचिन्हित ostr0, अचिन्हित ostr1,
			    अचिन्हित argc, संरचना symb_s*argv);


/*
 * This is called by the parser to make a resolver. This is a special
 * kind of functor; a strength aware functor. It has up to 4 inputs
 * that are blended to make a resolved output. The type string selects
 * a resolution algorithm.
 */
बाह्य शून्य compile_resolver(अक्षर*label, अक्षर*type,
			     अचिन्हित argc, संरचना symb_s*argv);

बाह्य शून्य compile_concat(अक्षर*label, अचिन्हित w0, अचिन्हित w1,
			   अचिन्हित w2, अचिन्हित w3,
			   अचिन्हित argc, संरचना symb_s*argv);

बाह्य शून्य compile_concat8(अक्षर*label, अचिन्हित w0, अचिन्हित w1,
			    अचिन्हित w2, अचिन्हित w3,
			    अचिन्हित argc, संरचना symb_s*argv);

बाह्य शून्य compile_substitute(अक्षर*label, अचिन्हित width,
			       अचिन्हित soff, अचिन्हित swidth,
			       अचिन्हित argc, संरचना symb_s*argv);

/*
 * Arrange for the system task/function call to have its compiletf
 * function called.
 */
बाह्य शून्य compile_compiletf(संरचना __vpiSysTaskCall*);

/*
 * Compile delay nodes of various form.
 */
बाह्य शून्य compile_delay(अक्षर*label, अचिन्हित width,
                          vvp_delay_t*del, संरचना symb_s आदान);
बाह्य शून्य compile_delay(अक्षर*label, अचिन्हित width,
                          अचिन्हित argc, संरचना symb_s*argv,
                          bool ignore_decay);

/*
 * This is called by the parser to create a part select node.
 * See the PART SELECT STATEMENT section in the README.txt
 */
बाह्य शून्य compile_part_select(अक्षर*label, अक्षर*src,
				अचिन्हित base, अचिन्हित wid);
बाह्य शून्य compile_part_select_pv(अक्षर*label, अक्षर*src,
				   अचिन्हित base, अचिन्हित wid,
				   अचिन्हित vec_wid);
बाह्य शून्य compile_part_select_var(अक्षर*label, अक्षर*src,
                                    अक्षर*var, अचिन्हित wid, bool is_signed);

/*
 * This is called by the parser to make the various arithmetic and
 * comparison functors.
 */
बाह्य शून्य compile_arith_pow(अक्षर*label, दीर्घ width, bool signed_flag,
			      अचिन्हित argc, संरचना symb_s*argv);
बाह्य शून्य compile_arith_abs(अक्षर*label,
			      अचिन्हित argc, संरचना symb_s*argv);
बाह्य शून्य compile_arith_cast_int(अक्षर*label, दीर्घ width,
			           अचिन्हित argc, संरचना symb_s*argv);
बाह्य शून्य compile_arith_cast_real(अक्षर*label, bool signed_flag,
			            अचिन्हित argc, संरचना symb_s*argv);
बाह्य शून्य compile_arith_cast_vec2(अक्षर*label, दीर्घ width,
				    अचिन्हित argc, संरचना symb_s*argv);
बाह्य शून्य compile_arith_div(अक्षर*label, दीर्घ width, bool signed_flag,
			      अचिन्हित argc, संरचना symb_s*argv);
बाह्य शून्य compile_arith_mod(अक्षर*label, दीर्घ width, bool signed_flag,
			      अचिन्हित argc, संरचना symb_s*argv);
बाह्य शून्य compile_arith_mult(अक्षर*label, दीर्घ width,
			       अचिन्हित argc, संरचना symb_s*argv);
बाह्य शून्य compile_arith_sum(अक्षर*label, दीर्घ width,
			      अचिन्हित argc, संरचना symb_s*argv);
बाह्य शून्य compile_arith_sub(अक्षर*label, दीर्घ width,
			      अचिन्हित argc, संरचना symb_s*argv);
बाह्य शून्य compile_cmp_eeq(अक्षर*label, दीर्घ width,
			   अचिन्हित argc, संरचना symb_s*argv);
बाह्य शून्य compile_cmp_nee(अक्षर*label, दीर्घ width,
			   अचिन्हित argc, संरचना symb_s*argv);
बाह्य शून्य compile_cmp_eq(अक्षर*label, दीर्घ width,
			   अचिन्हित argc, संरचना symb_s*argv);
बाह्य शून्य compile_cmp_eqx(अक्षर*label, दीर्घ width,
			    अचिन्हित argc, संरचना symb_s*argv);
बाह्य शून्य compile_cmp_eqz(अक्षर*label, दीर्घ width,
			    अचिन्हित argc, संरचना symb_s*argv);
बाह्य शून्य compile_cmp_ne(अक्षर*label, दीर्घ width,
			   अचिन्हित argc, संरचना symb_s*argv);
बाह्य शून्य compile_cmp_ge(अक्षर*label, दीर्घ width, bool signed_flag,
			   अचिन्हित argc, संरचना symb_s*argv);
बाह्य शून्य compile_cmp_gt(अक्षर*label, दीर्घ width, bool signed_flag,
			   अचिन्हित argc, संरचना symb_s*argv);
बाह्य शून्य compile_cmp_weq(अक्षर*label, दीर्घ width,
			   अचिन्हित argc, संरचना symb_s*argv);
बाह्य शून्य compile_cmp_wne(अक्षर*label, दीर्घ width,
			   अचिन्हित argc, संरचना symb_s*argv);

बाह्य शून्य compile_arith_mult_r(अक्षर*label, अचिन्हित argc,
                                 संरचना symb_s*argv);
बाह्य शून्य compile_arith_pow_r(अक्षर*label, अचिन्हित argc, संरचना symb_s*argv);
बाह्य शून्य compile_arith_div_r(अक्षर*label, अचिन्हित argc, संरचना symb_s*argv);
बाह्य शून्य compile_arith_mod_r(अक्षर*label, अचिन्हित argc, संरचना symb_s*argv);
बाह्य शून्य compile_arith_sum_r(अक्षर*label, अचिन्हित argc, संरचना symb_s*argv);
बाह्य शून्य compile_arith_sub_r(अक्षर*label, अचिन्हित argc, संरचना symb_s*argv);
बाह्य शून्य compile_cmp_eq_r(अक्षर*label, अचिन्हित argc, संरचना symb_s*argv);
बाह्य शून्य compile_cmp_ne_r(अक्षर*label, अचिन्हित argc, संरचना symb_s*argv);
बाह्य शून्य compile_cmp_ge_r(अक्षर*label, अचिन्हित argc, संरचना symb_s*argv);
बाह्य शून्य compile_cmp_gt_r(अक्षर*label, अचिन्हित argc, संरचना symb_s*argv);

बाह्य शून्य compile_dff(अक्षर*label, अचिन्हित width, bool negedge,
			संरचना symb_s arg_d,
			संरचना symb_s arg_c,
			संरचना symb_s arg_e);

बाह्य शून्य compile_dff_aclr(अक्षर*label, अचिन्हित width, bool negedge,
			     संरचना symb_s arg_d,
			     संरचना symb_s arg_c,
			     संरचना symb_s arg_e,
			     संरचना symb_s arg_a);
बाह्य शून्य compile_dff_aset(अक्षर*label, अचिन्हित width, bool negedge,
			     संरचना symb_s arg_d,
			     संरचना symb_s arg_c,
			     संरचना symb_s arg_e,
			     संरचना symb_s arg_a,
			     अक्षर*asc_मूल्य);

बाह्य शून्य compile_latch(अक्षर*label, अचिन्हित width,
			  संरचना symb_s arg_d,
			  संरचना symb_s arg_e);

बाह्य शून्य compile_enum2_type(अक्षर*label, दीर्घ width, bool signed_flag,
			      std::list<संरचना enum_name_s>*names);
बाह्य शून्य compile_enum4_type(अक्षर*label, दीर्घ width, bool signed_flag,
			      std::list<संरचना enum_name_s>*names);

संरचना __vpiModPath;
बाह्य __vpiModPath* compile_modpath(अक्षर*label,
                                     अचिन्हित width,
				     संरचना symb_s drv,
				     संरचना symb_s dest);
बाह्य शून्य compile_modpath_src(__vpiModPath*dst,
				अक्षर edge_c,
				स्थिरांक संरचना symb_s&src,
				संरचना numbv_s&vals,
				स्थिरांक संरचना symb_s&condit_src,
				स्थिरांक संरचना symb_s&path_term_in);
बाह्य शून्य compile_modpath_src(__vpiModPath*dst,
				अक्षर edge_c,
				स्थिरांक संरचना symb_s&src,
				संरचना numbv_s&vals,
				पूर्णांक condit_src, /* match with '0' */
				स्थिरांक संरचना symb_s&path_term_in,
				bool ifnone);

बाह्य शून्य compile_reduce_and(अक्षर*label, स्थिरांक संरचना symb_s&arg);
बाह्य शून्य compile_reduce_or(अक्षर*label, स्थिरांक संरचना symb_s&arg);
बाह्य शून्य compile_reduce_xor(अक्षर*label, स्थिरांक संरचना symb_s&arg);
बाह्य शून्य compile_reduce_nand(अक्षर*label, स्थिरांक संरचना symb_s&arg);
बाह्य शून्य compile_reduce_nor(अक्षर*label, स्थिरांक संरचना symb_s&arg);
बाह्य शून्य compile_reduce_xnor(अक्षर*label, स्थिरांक संरचना symb_s&arg);

बाह्य शून्य compile_extend_signed(अक्षर*label, दीर्घ width, संरचना symb_s arg);

बाह्य शून्य compile_sfunc(अक्षर*label, अक्षर*name, अक्षर*format_string,
			  दीर्घ file_idx, दीर्घ lineno,
			  अचिन्हित argc, संरचना symb_s*argv,
                          अक्षर*trigger_label);

बाह्य शून्य compile_repeat(अक्षर*label, दीर्घ width, दीर्घ repeat,
			   संरचना symb_s arg);

बाह्य शून्य compile_shiftl(अक्षर*label, दीर्घ width,
			   अचिन्हित argc, संरचना symb_s*argv);
बाह्य शून्य compile_shiftr(अक्षर*label, दीर्घ width, bool signed_flag,
			   अचिन्हित argc, संरचना symb_s*argv);

बाह्य शून्य compile_timescale(दीर्घ units, दीर्घ precision);

बाह्य शून्य compile_vpi_चिन्ह(स्थिरांक अक्षर*label, vpiHandle obj);
बाह्य शून्य compile_vpi_lookup(vpiHandle *objref, अक्षर*label);

बाह्य शून्य compile_param_string(अक्षर*label, स्थिरांक अक्षर*name, अक्षर*मूल्य,
                                 bool local_flag,
                                 दीर्घ file_idx, दीर्घ lineno);
बाह्य शून्य compile_param_logic(अक्षर*label, स्थिरांक अक्षर*name, अक्षर*मूल्य,
				bool signed_flag, bool local_flag,
                                दीर्घ file_idx, दीर्घ lineno);
बाह्य शून्य compile_param_real(अक्षर*label, स्थिरांक अक्षर*name, अक्षर*मूल्य,
                               bool local_flag,
                               दीर्घ file_idx, दीर्घ lineno);

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

    सार्वजनिक:
      explicit resolv_list_s(अक्षर*lab) : label_(lab) {
	    next = रिक्त;
      }
      आभासी ~resolv_list_s();

      resolv_list_s(स्थिरांक resolv_list_s&) = delete;
      resolv_list_s& चालक=(स्थिरांक resolv_list_s&) = delete;

      आभासी bool resolve(bool mes = false) = 0;

    संरक्षित:
      स्थिरांक अक्षर*label() स्थिरांक { परत label_; }

    खाजगी:
      मित्र शून्य resolv_submit(class resolv_list_s*cur);
      मित्र शून्य compile_cleanup(शून्य);

      अक्षर*label_;
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
बाह्य शून्य functor_ref_lookup(vvp_net_t**ref, अक्षर*lab);

बाह्य vpiHandle vpip_make_prop_queue_ref(अक्षर* class_label,
                                          अचिन्हित prop_idx,
                                          अचिन्हित is_queue_flag);

/*
 * This function schedules a lookup of the labeled instruction. The
 * code points to a code structure that points to the instruction
 * field that receives the result, and the label is the name to
 * lookup. The lookup will free the label text when it is done.
 *
 * The cptr2 flag tells the lookup to write the code pointer into the
 * cptr2 member of the code, instead of the cptr member.
 */
बाह्य शून्य code_label_lookup(संरचना vvp_code_s *code, अक्षर *label, bool cptr2);

/*
 * The `compile_udp_def' function creates a UDP.  The `table' is a
 * NULL terminated array of char*, as assembled by `compile_udp_table'.
 * `compile_udp_table' is called with `table'==NULL to create a new
 * table, or with an existing table to append to.
 *
 * `compile_udp_functor' creates a mode-3 functor referring to the
 * labeled UDP.
 */

बाह्य शून्य compile_udp_def(पूर्णांक sequ, अक्षर*label, अक्षर *name,
			    अचिन्हित nin, अचिन्हित init, अक्षर **table);

बाह्य शून्य compile_udp_functor(अक्षर*label, अक्षर*type,
				अचिन्हित argc, संरचना symb_s*argv);

बाह्य अक्षर **compile_udp_table(अक्षर **table, अक्षर *row);

/*
 * Memory Instances, Ports, and Initialization
 */

बाह्य शून्य compile_var_array(अक्षर*label, अक्षर*name,
			      पूर्णांक last, पूर्णांक first,
			      पूर्णांक msb, पूर्णांक lsb, अक्षर signed_flag);
बाह्य शून्य compile_var2_array(अक्षर*label, अक्षर*name,
			      पूर्णांक last, पूर्णांक first,
			      पूर्णांक msb, पूर्णांक lsb, bool signed_flag);
बाह्य शून्य compile_real_array(अक्षर*label, अक्षर*name,
			       पूर्णांक last, पूर्णांक first);
बाह्य शून्य compile_string_array(अक्षर*label, अक्षर*name,
				 पूर्णांक last, पूर्णांक first);
बाह्य शून्य compile_object_array(अक्षर*label, अक्षर*name,
				 पूर्णांक last, पूर्णांक first);
बाह्य शून्य compile_net_array(अक्षर*label, अक्षर*name,
			      पूर्णांक last, पूर्णांक first);
बाह्य शून्य compile_array_alias(अक्षर*label, अक्षर*name, अक्षर*src);

  /* Index is a net. */
बाह्य शून्य compile_array_port(अक्षर*label, अक्षर*name, अक्षर*addr);
  /* Index is a constant address */
बाह्य शून्य compile_array_port(अक्षर*label, अक्षर*name, दीर्घ addr);

बाह्य शून्य compile_array_cleanup(शून्य);

/*
 * Compile the .ufunc statement.
 */
बाह्य शून्य compile_ufunc_real(अक्षर*label, अक्षर*code, अचिन्हित wid,
			  अचिन्हित argc, संरचना symb_s*argv,
			  अचिन्हित portc, संरचना symb_s*portv,
			  अक्षर*scope_label, अक्षर*trigger_label);
बाह्य शून्य compile_ufunc_vec4(अक्षर*label, अक्षर*code, अचिन्हित wid,
			  अचिन्हित argc, संरचना symb_s*argv,
			  अचिन्हित portc, संरचना symb_s*portv,
			  अक्षर*scope_label, अक्षर*trigger_label);

/*
 * The compile_event function takes the parts of the event statement
 * and makes the various objects needed to simulate it. This includes
 * the functor that receives the signals and the event_t that holds
 * the threads.
 */
बाह्य शून्य compile_event(अक्षर*label, अक्षर*type,
			  अचिन्हित argc, संरचना symb_s*argv);
बाह्य शून्य compile_named_event(अक्षर*label, अक्षर*type, bool local_flag=false);


/*
 * A code statement is a label, an opcode and up to 3 operands. There
 * are a few lexical types that the parser recognizes of the operands,
 * given by the ltype_e enumeration. The compile_code function takes
 * the label, mnemonic and parsed operands and writes a properly
 * formed instruction into the code space. The label is set into the
 * symbol table with the address of the instruction.
 */

#define OPERAND_MAX 3
गणना ltype_e { L_NUMB, L_SYMB, L_STRING };

संरचना comp_operands_s {
      अचिन्हित argc;
      संरचना {
	    गणना ltype_e ltype;
	    संघ {
		  अचिन्हित दीर्घ numb;
		  संरचना symb_s symb;
		  स्थिरांक अक्षर   *text;
	    };
      } argv[OPERAND_MAX];
};

प्रकारव्याख्या संरचना comp_operands_s*comp_operands_t;

बाह्य शून्य compile_code(अक्षर*label, अक्षर*mnem, comp_operands_t opa);

बाह्य शून्य compile_file_ओळ(अक्षर*label, दीर्घ file_idx, दीर्घ lineno,
                              अक्षर*description);

बाह्य शून्य compile_vpi_call(अक्षर*label, अक्षर*name,
			     bool func_as_task_err, bool func_as_task_warn,
			     दीर्घ file_idx, दीर्घ lineno,
			     अचिन्हित argc, vpiHandle*argv,
			     अचिन्हित vec4_चळत,
			     अचिन्हित real_चळत,
			     अचिन्हित string_चळत);

/* Compile a function call. The vbit and vwid encode the return
   type. If the vwid >0, the return type is a vector. If the vwid is
   <0, the return type is -vpiRealConst or some other constant subtype
   code that represents the function type. */
बाह्य शून्य compile_vpi_func_call(अक्षर*label, अक्षर*name,
				  पूर्णांक val_type, अचिन्हित val_wid,
				  दीर्घ file_idx, दीर्घ lineno,
				  अचिन्हित argc, vpiHandle*argv,
				  अचिन्हित vec4_चळत,
				  अचिन्हित real_चळत,
				  अचिन्हित string_चळत);
बाह्य शून्य print_vpi_call_errors();

बाह्य शून्य compile_codelabel(अक्षर*label);

/*
 * The parser uses these functions to compile .scope statements.
 * The implementations of these live in the vpi_scope.cc file.
 */
बाह्य शून्य compile_scope_decl(अक्षर*typ, अक्षर*lab, अक्षर*nam, अक्षर*tnam,
                               अक्षर*par, दीर्घ file_idx, दीर्घ lineno,
                               दीर्घ def_file_idx, दीर्घ def_lineno,
                               दीर्घ is_cell);
बाह्य शून्य compile_scope_recall(अक्षर*sym);

/*
 * The parser uses this function to declare a thread. The start_sym is
 * the start instruction, and must already be defined.
 */
बाह्य शून्य compile_thread(अक्षर*start_sym, अक्षर*flag);

/*
 * This function is called to create a var vector with the given name.
 *
 * The vpi_type_code argument of compile_variable() is one of the vpi
 * object codes that identify the type: vpiReg, vpiIntegerVar,
 * vpiIntVar, etc.
 */
बाह्य शून्य compile_variable(अक्षर*label, अक्षर*name,
			     पूर्णांक msb, पूर्णांक lsb, पूर्णांक vpi_type_code,
			     bool signed_flag, bool local_flag);

बाह्य शून्य compile_var_real(अक्षर*label, अक्षर*name, bool local_flag);
बाह्य शून्य compile_var_string(अक्षर*label, अक्षर*name);
बाह्य शून्य compile_var_darray(अक्षर*label, अक्षर*name, अचिन्हित size);
बाह्य शून्य compile_var_cobject(अक्षर*label, अक्षर*name);
बाह्य शून्य compile_var_queue(अक्षर*label, अक्षर*name, अचिन्हित size);

/*
 * This function is used to create a scope port
 * Current ONLY module ports are supported and ports exist purely
 * as meta-data for VPI queries (i.e. there is NO corresponding net etc)
 * as elaboration internally eliminates port-nets by directly connecting
 * nets connected through module ports.
 */

बाह्य शून्य compile_port_info( अचिन्हित index, पूर्णांक vpi_port_type, अचिन्हित width, स्थिरांक अक्षर *name, अक्षर* buffer );


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
बाह्य शून्य compile_net(अक्षर*label, अक्षर*name, पूर्णांक msb, पूर्णांक lsb,
			पूर्णांक vpi_type_code, bool signed_flag, bool local_flag,
			अचिन्हित argc, संरचना symb_s*argv);

बाह्य शून्य compile_net_real(अक्षर*label, अक्षर*name,
			     पूर्णांक msb, पूर्णांक lsb, bool local_flag,
			     अचिन्हित argc, संरचना symb_s*argv);

बाह्य शून्य compile_netw(अक्षर*label, अक्षर*array_label, अचिन्हित दीर्घ array_addr,
			 पूर्णांक msb, पूर्णांक lsb, पूर्णांक vpi_type_code, bool signed_flag,
			 अचिन्हित argc, संरचना symb_s*argv);

बाह्य शून्य compile_netw_real(अक्षर*label, अक्षर*array_चिन्ह,
			      अचिन्हित दीर्घ array_addr,
			      पूर्णांक msb, पूर्णांक lsb,
			      अचिन्हित argc, संरचना symb_s*argv);


बाह्य शून्य compile_island(अक्षर*label, अक्षर*type);
बाह्य शून्य compile_island_port(अक्षर*label, अक्षर*island, अक्षर*src);
बाह्य शून्य compile_island_import(अक्षर*label, अक्षर*island, अक्षर*src);
बाह्य शून्य compile_island_export(अक्षर*label, अक्षर*island);
बाह्य शून्य compile_island_cleanup(शून्य);

बाह्य शून्य compile_island_tran(अक्षर*label);
बाह्य शून्य compile_island_tranif(पूर्णांक sense, अक्षर*island,
				  अक्षर*ba, अक्षर*bb, अक्षर*src, bool resistive);
बाह्य शून्य compile_island_tranvp(अक्षर*island, अक्षर*ba, अक्षर*bb,
				  अचिन्हित width, अचिन्हित part, अचिन्हित off);

बाह्य शून्य delete_udp_symbols(शून्य);

बाह्य शून्य compile_class_start(अक्षर*lab, अक्षर*nam, अचिन्हित nprop);
बाह्य शून्य compile_class_property(अचिन्हित idx, अक्षर*nam, अक्षर*typ, uint64_t array_size);
बाह्य शून्य compile_class_done(शून्य);

#endif /* IVL_compile_H */
