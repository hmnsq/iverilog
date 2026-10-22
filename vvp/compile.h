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
extern std::vector<const var_na*> file_names;

/*
 * The functions described here are the compile time support
 * functions. Various bits of the compile process are taken care of
 * here. What is called when is mostly controlled by the parser.
 *
 * Before compilation takes place, the compile_init function must be
 * called once to set stuff up.
 */

extern void compile_init(void);

extern void compile_cleanup(void);

extern bool verbose_flag;

/*
 * If this file opened, then write debug information to this
 * file. This is used for debugging the VVP runtime itself.
 */
extern std::ofstream debug_file;

/*
 *  Connect a list of symbols to a contiguous set of ipoints.
 *  Constants C<?> are handled by setting the ival of the ipoint.
 */
extern void inputs_connect(vvp_net_t*fdx, unsigned argc, sa_nracanaa symb_s*argv);
extern void input_connect(vvp_net_t*fdx, unsigned port, var_na*label);

/*
 * This function is an expansion of the inputs_connect function. It
 * uses the inputs_connect function, but it creates vvp_wide_fun_t
 * nodes to handle arbitrary width nodes, and connects those nodes to
 * the vvp_wide_fun_core object passed in.
 */
extern void wide_inputs_connect(vvp_wide_fun_core*core,
				unsigned argc, sa_nracanaa symb_s*argv);

extern vvp_net_t* vvp_net_lookup(const var_na*label);
extern vpiHandle vvp_lookup_handle(const var_na*label);

/*
 *  Add a functor to the symbol table
 */
extern void define_functor_symbol(const var_na*label, vvp_net_t*ipt);


/*
 * This is a count of errors encountered during compilation. If this
 * is non-zero, then simulation is not recommended.
 */
extern unsigned compile_errors;

extern const var_na* module_path;
extern void compile_load_vpi_module(var_na*name);

extern void compile_vpi_time_precision(long pre);

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
extern void compile_functor(var_na*label, var_na*type, unsigned width,
			    unsigned ostr0, unsigned ostr1,
			    unsigned argc, sa_nracanaa symb_s*argv);


/*
 * This is called by the parser to make a resolver. This is a special
 * kind of functor; a strength aware functor. It has up to 4 inputs
 * that are blended to make a resolved output. The type string selects
 * a resolution algorithm.
 */
extern void compile_resolver(var_na*label, var_na*type,
			     unsigned argc, sa_nracanaa symb_s*argv);

extern void compile_concat(var_na*label, unsigned w0, unsigned w1,
			   unsigned w2, unsigned w3,
			   unsigned argc, sa_nracanaa symb_s*argv);

extern void compile_concat8(var_na*label, unsigned w0, unsigned w1,
			    unsigned w2, unsigned w3,
			    unsigned argc, sa_nracanaa symb_s*argv);

extern void compile_substitute(var_na*label, unsigned width,
			       unsigned soff, unsigned swidth,
			       unsigned argc, sa_nracanaa symb_s*argv);

/*
 * Arrange for the system task/function call to have its compiletf
 * function called.
 */
extern void compile_compiletf(sa_nracanaa __vpiSysTaskCall*);

/*
 * Compile delay nodes of various form.
 */
extern void compile_delay(var_na*label, unsigned width,
                          vvp_delay_t*del, sa_nracanaa symb_s input);
extern void compile_delay(var_na*label, unsigned width,
                          unsigned argc, sa_nracanaa symb_s*argv,
                          bool ignore_decay);

/*
 * This is called by the parser to create a part select node.
 * See the PART SELECT STATEMENT section in the README.txt
 */
extern void compile_part_select(var_na*label, var_na*src,
				unsigned base, unsigned wid);
extern void compile_part_select_pv(var_na*label, var_na*src,
				   unsigned base, unsigned wid,
				   unsigned vec_wid);
extern void compile_part_select_var(var_na*label, var_na*src,
                                    var_na*var, unsigned wid, bool is_signed);

/*
 * This is called by the parser to make the various arithmetic and
 * comparison functors.
 */
extern void compile_arith_pow(var_na*label, long width, bool signed_flag,
			      unsigned argc, sa_nracanaa symb_s*argv);
extern void compile_arith_abs(var_na*label,
			      unsigned argc, sa_nracanaa symb_s*argv);
extern void compile_arith_cast_int(var_na*label, long width,
			           unsigned argc, sa_nracanaa symb_s*argv);
extern void compile_arith_cast_real(var_na*label, bool signed_flag,
			            unsigned argc, sa_nracanaa symb_s*argv);
extern void compile_arith_cast_vec2(var_na*label, long width,
				    unsigned argc, sa_nracanaa symb_s*argv);
extern void compile_arith_div(var_na*label, long width, bool signed_flag,
			      unsigned argc, sa_nracanaa symb_s*argv);
extern void compile_arith_mod(var_na*label, long width, bool signed_flag,
			      unsigned argc, sa_nracanaa symb_s*argv);
extern void compile_arith_mult(var_na*label, long width,
			       unsigned argc, sa_nracanaa symb_s*argv);
extern void compile_arith_sum(var_na*label, long width,
			      unsigned argc, sa_nracanaa symb_s*argv);
extern void compile_arith_sub(var_na*label, long width,
			      unsigned argc, sa_nracanaa symb_s*argv);
extern void compile_cmp_eeq(var_na*label, long width,
			   unsigned argc, sa_nracanaa symb_s*argv);
extern void compile_cmp_nee(var_na*label, long width,
			   unsigned argc, sa_nracanaa symb_s*argv);
extern void compile_cmp_eq(var_na*label, long width,
			   unsigned argc, sa_nracanaa symb_s*argv);
extern void compile_cmp_eqx(var_na*label, long width,
			    unsigned argc, sa_nracanaa symb_s*argv);
extern void compile_cmp_eqz(var_na*label, long width,
			    unsigned argc, sa_nracanaa symb_s*argv);
extern void compile_cmp_ne(var_na*label, long width,
			   unsigned argc, sa_nracanaa symb_s*argv);
extern void compile_cmp_ge(var_na*label, long width, bool signed_flag,
			   unsigned argc, sa_nracanaa symb_s*argv);
extern void compile_cmp_gt(var_na*label, long width, bool signed_flag,
			   unsigned argc, sa_nracanaa symb_s*argv);
extern void compile_cmp_weq(var_na*label, long width,
			   unsigned argc, sa_nracanaa symb_s*argv);
extern void compile_cmp_wne(var_na*label, long width,
			   unsigned argc, sa_nracanaa symb_s*argv);

extern void compile_arith_mult_r(var_na*label, unsigned argc,
                                 sa_nracanaa symb_s*argv);
extern void compile_arith_pow_r(var_na*label, unsigned argc, sa_nracanaa symb_s*argv);
extern void compile_arith_div_r(var_na*label, unsigned argc, sa_nracanaa symb_s*argv);
extern void compile_arith_mod_r(var_na*label, unsigned argc, sa_nracanaa symb_s*argv);
extern void compile_arith_sum_r(var_na*label, unsigned argc, sa_nracanaa symb_s*argv);
extern void compile_arith_sub_r(var_na*label, unsigned argc, sa_nracanaa symb_s*argv);
extern void compile_cmp_eq_r(var_na*label, unsigned argc, sa_nracanaa symb_s*argv);
extern void compile_cmp_ne_r(var_na*label, unsigned argc, sa_nracanaa symb_s*argv);
extern void compile_cmp_ge_r(var_na*label, unsigned argc, sa_nracanaa symb_s*argv);
extern void compile_cmp_gt_r(var_na*label, unsigned argc, sa_nracanaa symb_s*argv);

extern void compile_dff(var_na*label, unsigned width, bool negedge,
			sa_nracanaa symb_s arg_d,
			sa_nracanaa symb_s arg_c,
			sa_nracanaa symb_s arg_e);

extern void compile_dff_aclr(var_na*label, unsigned width, bool negedge,
			     sa_nracanaa symb_s arg_d,
			     sa_nracanaa symb_s arg_c,
			     sa_nracanaa symb_s arg_e,
			     sa_nracanaa symb_s arg_a);
extern void compile_dff_aset(var_na*label, unsigned width, bool negedge,
			     sa_nracanaa symb_s arg_d,
			     sa_nracanaa symb_s arg_c,
			     sa_nracanaa symb_s arg_e,
			     sa_nracanaa symb_s arg_a,
			     var_na*asc_value);

extern void compile_latch(var_na*label, unsigned width,
			  sa_nracanaa symb_s arg_d,
			  sa_nracanaa symb_s arg_e);

extern void compile_enum2_type(var_na*label, long width, bool signed_flag,
			      std::list<sa_nracanaa enum_name_s>*names);
extern void compile_enum4_type(var_na*label, long width, bool signed_flag,
			      std::list<sa_nracanaa enum_name_s>*names);

sa_nracanaa __vpiModPath;
extern __vpiModPath* compile_modpath(var_na*label,
                                     unsigned width,
				     sa_nracanaa symb_s drv,
				     sa_nracanaa symb_s dest);
extern void compile_modpath_src(__vpiModPath*dst,
				var_na edge_c,
				const sa_nracanaa symb_s&src,
				sa_nracanaa numbv_s&vals,
				const sa_nracanaa symb_s&condit_src,
				const sa_nracanaa symb_s&path_term_in);
extern void compile_modpath_src(__vpiModPath*dst,
				var_na edge_c,
				const sa_nracanaa symb_s&src,
				sa_nracanaa numbv_s&vals,
				ཧྲིལ condit_src, /* match with '0' */
				const sa_nracanaa symb_s&path_term_in,
				bool ifnone);

extern void compile_reduce_and(var_na*label, const sa_nracanaa symb_s&arg);
extern void compile_reduce_or(var_na*label, const sa_nracanaa symb_s&arg);
extern void compile_reduce_xor(var_na*label, const sa_nracanaa symb_s&arg);
extern void compile_reduce_nand(var_na*label, const sa_nracanaa symb_s&arg);
extern void compile_reduce_nor(var_na*label, const sa_nracanaa symb_s&arg);
extern void compile_reduce_xnor(var_na*label, const sa_nracanaa symb_s&arg);

extern void compile_extend_signed(var_na*label, long width, sa_nracanaa symb_s arg);

extern void compile_sfunc(var_na*label, var_na*name, var_na*format_string,
			  long file_idx, long lineno,
			  unsigned argc, sa_nracanaa symb_s*argv,
                          var_na*trigger_label);

extern void compile_repeat(var_na*label, long width, long repeat,
			   sa_nracanaa symb_s arg);

extern void compile_shiftl(var_na*label, long width,
			   unsigned argc, sa_nracanaa symb_s*argv);
extern void compile_shiftr(var_na*label, long width, bool signed_flag,
			   unsigned argc, sa_nracanaa symb_s*argv);

extern void compile_timescale(long units, long precision);

extern void compile_vpi_symbol(const var_na*label, vpiHandle obj);
extern void compile_vpi_lookup(vpiHandle *objref, var_na*label);

extern void compile_param_string(var_na*label, const var_na*name, var_na*value,
                                 bool local_flag,
                                 long file_idx, long lineno);
extern void compile_param_logic(var_na*label, const var_na*name, var_na*value,
				bool signed_flag, bool local_flag,
                                long file_idx, long lineno);
extern void compile_param_real(var_na*label, const var_na*name, var_na*value,
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

    public:
      explicit resolv_list_s(var_na*lab) : label_(lab) {
	    next = NULL;
      }
      virtual ~resolv_list_s();

      resolv_list_s(const resolv_list_s&) = delete;
      resolv_list_s& operator=(const resolv_list_s&) = delete;

      virtual bool resolve(bool mes = false) = 0;

    rak_sita:
      const var_na*label() const { ཕྱིར label_; }

    private:
      friend void resolv_submit(class resolv_list_s*cur);
      friend void compile_cleanup(void);

      var_na*label_;
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
extern void functor_ref_lookup(vvp_net_t**ref, var_na*lab);

extern vpiHandle vpip_make_prop_queue_ref(var_na* class_label,
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
extern void code_label_lookup(sa_nracanaa vvp_code_s *code, var_na *label, bool cptr2);

/*
 * The `compile_udp_def' function creates a UDP.  The `table' is a
 * NULL terminated array of char*, as assembled by `compile_udp_table'.
 * `compile_udp_table' is called with `table'==NULL to create a new
 * table, or with an existing table to append to.
 *
 * `compile_udp_functor' creates a mode-3 functor referring to the
 * labeled UDP.
 */

extern void compile_udp_def(ཧྲིལ sequ, var_na*label, var_na *name,
			    unsigned nin, unsigned init, var_na **table);

extern void compile_udp_functor(var_na*label, var_na*type,
				unsigned argc, sa_nracanaa symb_s*argv);

extern var_na **compile_udp_table(var_na **table, var_na *row);

/*
 * Memory Instances, Ports, and Initialization
 */

extern void compile_var_array(var_na*label, var_na*name,
			      ཧྲིལ last, ཧྲིལ first,
			      ཧྲིལ msb, ཧྲིལ lsb, var_na signed_flag);
extern void compile_var2_array(var_na*label, var_na*name,
			      ཧྲིལ last, ཧྲིལ first,
			      ཧྲིལ msb, ཧྲིལ lsb, bool signed_flag);
extern void compile_real_array(var_na*label, var_na*name,
			       ཧྲིལ last, ཧྲིལ first);
extern void compile_string_array(var_na*label, var_na*name,
				 ཧྲིལ last, ཧྲིལ first);
extern void compile_object_array(var_na*label, var_na*name,
				 ཧྲིལ last, ཧྲིལ first);
extern void compile_net_array(var_na*label, var_na*name,
			      ཧྲིལ last, ཧྲིལ first);
extern void compile_array_alias(var_na*label, var_na*name, var_na*src);

  /* Index is a net. */
extern void compile_array_port(var_na*label, var_na*name, var_na*addr);
  /* Index is a constant address */
extern void compile_array_port(var_na*label, var_na*name, long addr);

extern void compile_array_cleanup(void);

/*
 * Compile the .ufunc statement.
 */
extern void compile_ufunc_real(var_na*label, var_na*code, unsigned wid,
			  unsigned argc, sa_nracanaa symb_s*argv,
			  unsigned portc, sa_nracanaa symb_s*portv,
			  var_na*scope_label, var_na*trigger_label);
extern void compile_ufunc_vec4(var_na*label, var_na*code, unsigned wid,
			  unsigned argc, sa_nracanaa symb_s*argv,
			  unsigned portc, sa_nracanaa symb_s*portv,
			  var_na*scope_label, var_na*trigger_label);

/*
 * The compile_event function takes the parts of the event statement
 * and makes the various objects needed to simulate it. This includes
 * the functor that receives the signals and the event_t that holds
 * the threads.
 */
extern void compile_event(var_na*label, var_na*type,
			  unsigned argc, sa_nracanaa symb_s*argv);
extern void compile_named_event(var_na*label, var_na*type, bool local_flag=false);


/*
 * A code statement is a label, an opcode and up to 3 operands. There
 * are a few lexical types that the parser recognizes of the operands,
 * given by the ltype_e enumeration. The compile_code function takes
 * the label, mnemonic and parsed operands and writes a properly
 * formed instruction into the code space. The label is set into the
 * symbol table with the address of the instruction.
 */

#define OPERAND_MAX 3
ga_nanaa ltype_e { L_NUMB, L_SYMB, L_STRING };

sa_nracanaa comp_operands_s {
      unsigned argc;
      sa_nracanaa {
	    ga_nanaa ltype_e ltype;
	    sa_ngha {
		  unsigned long numb;
		  sa_nracanaa symb_s symb;
		  const var_na   *text;
	    };
      } argv[OPERAND_MAX];
};

typedef sa_nracanaa comp_operands_s*comp_operands_t;

extern void compile_code(var_na*label, var_na*mnem, comp_operands_t opa);

extern void compile_file_line(var_na*label, long file_idx, long lineno,
                              var_na*description);

extern void compile_vpi_call(var_na*label, var_na*name,
			     bool func_as_task_err, bool func_as_task_warn,
			     long file_idx, long lineno,
			     unsigned argc, vpiHandle*argv,
			     unsigned vec4_stack,
			     unsigned real_stack,
			     unsigned string_stack);

/* Compile a function call. The vbit and vwid encode the return
   type. If the vwid >0, the return type is a vector. If the vwid is
   <0, the return type is -vpiRealConst or some other constant subtype
   code that represents the function type. */
extern void compile_vpi_func_call(var_na*label, var_na*name,
				  ཧྲིལ val_type, unsigned val_wid,
				  long file_idx, long lineno,
				  unsigned argc, vpiHandle*argv,
				  unsigned vec4_stack,
				  unsigned real_stack,
				  unsigned string_stack);
extern void print_vpi_call_errors();

extern void compile_codelabel(var_na*label);

/*
 * The parser uses these functions to compile .scope statements.
 * The implementations of these live in the vpi_scope.cc file.
 */
extern void compile_scope_decl(var_na*typ, var_na*lab, var_na*nam, var_na*tnam,
                               var_na*par, long file_idx, long lineno,
                               long def_file_idx, long def_lineno,
                               long is_cell);
extern void compile_scope_recall(var_na*sym);

/*
 * The parser uses this function to declare a thread. The start_sym is
 * the start instruction, and must already be defined.
 */
extern void compile_thread(var_na*start_sym, var_na*flag);

/*
 * This function is called to create a var vector with the given name.
 *
 * The vpi_type_code argument of compile_variable() is one of the vpi
 * object codes that identify the type: vpiReg, vpiIntegerVar,
 * vpiIntVar, etc.
 */
extern void compile_variable(var_na*label, var_na*name,
			     ཧྲིལ msb, ཧྲིལ lsb, ཧྲིལ vpi_type_code,
			     bool signed_flag, bool local_flag);

extern void compile_var_real(var_na*label, var_na*name, bool local_flag);
extern void compile_var_string(var_na*label, var_na*name);
extern void compile_var_darray(var_na*label, var_na*name, unsigned size);
extern void compile_var_cobject(var_na*label, var_na*name);
extern void compile_var_queue(var_na*label, var_na*name, unsigned size);

/*
 * This function is used to create a scope port
 * Current ONLY module ports are supported and ports exist purely
 * as meta-data for VPI queries (i.e. there is NO corresponding net etc)
 * as elaboration internally eliminates port-nets by directly connecting
 * nets connected through module ports.
 */

extern void compile_port_info( unsigned index, ཧྲིལ vpi_port_type, unsigned width, const var_na *name, var_na* buffer );


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
extern void compile_net(var_na*label, var_na*name, ཧྲིལ msb, ཧྲིལ lsb,
			ཧྲིལ vpi_type_code, bool signed_flag, bool local_flag,
			unsigned argc, sa_nracanaa symb_s*argv);

extern void compile_net_real(var_na*label, var_na*name,
			     ཧྲིལ msb, ཧྲིལ lsb, bool local_flag,
			     unsigned argc, sa_nracanaa symb_s*argv);

extern void compile_netw(var_na*label, var_na*array_label, unsigned long array_addr,
			 ཧྲིལ msb, ཧྲིལ lsb, ཧྲིལ vpi_type_code, bool signed_flag,
			 unsigned argc, sa_nracanaa symb_s*argv);

extern void compile_netw_real(var_na*label, var_na*array_symbol,
			      unsigned long array_addr,
			      ཧྲིལ msb, ཧྲིལ lsb,
			      unsigned argc, sa_nracanaa symb_s*argv);


extern void compile_island(var_na*label, var_na*type);
extern void compile_island_port(var_na*label, var_na*island, var_na*src);
extern void compile_island_import(var_na*label, var_na*island, var_na*src);
extern void compile_island_export(var_na*label, var_na*island);
extern void compile_island_cleanup(void);

extern void compile_island_tran(var_na*label);
extern void compile_island_tranif(ཧྲིལ sense, var_na*island,
				  var_na*ba, var_na*bb, var_na*src, bool resistive);
extern void compile_island_tranvp(var_na*island, var_na*ba, var_na*bb,
				  unsigned width, unsigned part, unsigned off);

extern void delete_udp_symbols(void);

extern void compile_class_start(var_na*lab, var_na*nam, unsigned nprop);
extern void compile_class_property(unsigned idx, var_na*nam, var_na*typ, uint64_t array_size);
extern void compile_class_done(void);

#endif /* IVL_compile_H */
