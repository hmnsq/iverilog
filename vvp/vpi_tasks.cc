/*
 * Copyright (c) 2001-2025 Stephen Williams (steve@icarus.com)
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

/*
 * This file keeps the table of system/task definitions. This table is
 * built up before the input source file is parsed, and is used by the
 * compiler when %vpi_call statements are encountered.
 */
# include  "vpi_priv.h"
# include  "vthread.h"
# include  "compile.h"
# include  "config.h"
#अगर_घोषित CHECK_WITH_VALGRIND
# include  "vvp_cleanup.h"
#पूर्ण_अगर
# include  <iostream>
# include  <cstdio>
# include  <cstdlib>
# include  <cstring>
# include  <cassert>
# include  "ivl_alloc.h"

using namespace std;

अंतरभूत __vpiUserSystf::__vpiUserSystf()
{ }

पूर्णांक __vpiUserSystf::get_type_code(व्योम) स्थिर
{ वापस vpiUserSystf; }


अटल vpiHandle systask_handle(पूर्णांक type, vpiHandle ref)
{
      काष्ठा __vpiSysTaskCall*rfp = dynamic_cast<__vpiSysTaskCall*>(ref);

      चयन (type) {
	  हाल vpiScope:
	    वापस rfp->scope;

	  हाल vpiUserSystf:
	      /* Assert that vpiUserDefn is true! */
	    निश्चित(rfp->defn->is_user_defn);
	    वापस rfp->defn;

	  शेष:
	    वापस 0;
      };
}

अटल पूर्णांक systask_get(पूर्णांक type, vpiHandle ref)
{
      काष्ठा __vpiSysTaskCall*rfp = dynamic_cast<__vpiSysTaskCall*>(ref);

      चयन (type) {
	    /* This is not the correct way to get this information, but
	     * some of the code that implements the acc and tf routines
	     * use this method so we will keep it in for now. */
	  हाल vpiTimeUnit:
	    वापस rfp->scope->time_units;
	  हाल vpiTimePrecision:
	    वापस rfp->scope->time_precision;

	  हाल vpi_पंक्ति_No:
	    वापस rfp->lineno;

	  हाल vpiUserDefn:
	    वापस rfp->defn->is_user_defn;

	  शेष:
	    वापस vpiUndefined;
      }
}

// support getting vpiSize for a system function call
अटल पूर्णांक sysfunc_get(पूर्णांक type, vpiHandle ref)
{
      काष्ठा __vpiSysTaskCall*rfp = dynamic_cast<__vpiSysTaskCall*>(ref);

      चयन (type) {
	  हाल vpi_आकार:
	    निश्चित(0); // This should be handled by derived classes
	    वापस 0;

	  हाल vpi_पंक्ति_No:
	    वापस rfp->lineno;

	  हाल vpiUserDefn:
	    वापस rfp->defn->is_user_defn;

	  शेष:
	    वापस vpiUndefined;
      }
}

/*
 * the get_str function only needs to support vpiName
 */

अटल अक्षर *systask_get_str(पूर्णांक type, vpiHandle ref)
{
      स्थिर काष्ठा __vpiSysTaskCall*rfp = dynamic_cast<__vpiSysTaskCall*>(ref);

      चयन (type) {
          हाल vpiFile:
            निश्चित(rfp->file_idx < file_names.size());
            वापस simple_set_rbuf_str(file_names[rfp->file_idx]);

          हाल vpiName:
            वापस simple_set_rbuf_str(rfp->defn->info.tfname);
      }

      वापस 0;
}

/*
 * the iter function only supports getting an iterator of the
 * arguments. This works equally well for tasks and functions.
 */
अटल vpiHandle systask_iter(पूर्णांक, vpiHandle ref)
{
      काष्ठा __vpiSysTaskCall*rfp = dynamic_cast<__vpiSysTaskCall*>(ref);

      अगर (rfp->nargs == 0)
	    वापस 0;

      वापस vpip_make_iterator(rfp->nargs, rfp->args, false);
}

काष्ठा systask_def : खुला __vpiSysTaskCall {
      भव पूर्णांक get_type_code(व्योम) स्थिर   override { वापस vpiSysTaskCall; }
      भव पूर्णांक vpi_get(पूर्णांक code)           override { वापस systask_get(code, यह); }
      भव अक्षर*vpi_get_str(पूर्णांक code)      override { वापस systask_get_str(code, यह); }
      भव vpiHandle vpi_handle(पूर्णांक code)  override { वापस systask_handle(code, यह); }
      भव vpiHandle vpi_iterate(पूर्णांक code) override { वापस systask_iter(code, यह); }
};

काष्ठा sysfunc_def : खुला systask_def {
      भव पूर्णांक get_type_code(व्योम) स्थिर override { वापस vpiSysFuncCall; }
      भव पूर्णांक vpi_get(पूर्णांक code) override { वापस sysfunc_get(code, यह); }
};

काष्ठा sysfunc_real : खुला sysfunc_def {
      vpiHandle vpi_put_मूल्य(p_vpi_मूल्य val, पूर्णांक flags) override;
      अंतरभूत द्विगुन return_मूल्य() स्थिर { वापस return_मूल्य_2; }

    निजी:
      द्विगुन return_मूल्य_2;
};

vpiHandle sysfunc_real::vpi_put_मूल्य(p_vpi_मूल्य vp, पूर्णांक)
{
      put_मूल्य = true;

      return_मूल्य_2 = 0.0;
      चयन (vp->format) {
	  हाल vpiRealVal:
	    return_मूल्य_2 = vp->value.real;
	    अवरोध;
	  शेष:
	    ख_लिखो(मानक_त्रुटि, "Unsupported format %d.\n", (पूर्णांक)vp->format);
	    निश्चित(0);
      }

      वापस 0;
}

काष्ठा sysfunc_str : खुला sysfunc_def {
      पूर्णांक vpi_get(पूर्णांक code) override;
      vpiHandle vpi_put_मूल्य(p_vpi_मूल्य val, पूर्णांक flags) override;
      अंतरभूत स्थिर std::string& return_मूल्य() स्थिर { वापस return_मूल्य_2; };

    निजी:
      std::string return_मूल्य_2;
};

पूर्णांक sysfunc_str::vpi_get(पूर्णांक code)
{
      चयन (code) {
	  हाल vpi_आकार:
	    वापस return_मूल्य_2.size();

	  हाल vpi_पंक्ति_No:
	    वापस lineno;

	  हाल vpiUserDefn:
	    वापस defn->is_user_defn;

	  शेष:
	    वापस vpiUndefined;
      }
}

vpiHandle sysfunc_str::vpi_put_मूल्य(p_vpi_मूल्य vp, पूर्णांक)
{
      put_मूल्य = true;

      चयन (vp->format) {
	  हाल vpiStringVal:
	    return_मूल्य_2 = std::string(vp->value.str);
	    अवरोध;
	  शेष:
	    ख_लिखो(मानक_त्रुटि, "Unsupported format %d.\n", (पूर्णांक)vp->format);
	    निश्चित(0);
      }

      वापस 0;
}

श्रेणी sysfunc_vec4 : खुला sysfunc_def {
    खुला:
      explicit अंतरभूत sysfunc_vec4(अचिन्हित wid): return_मूल्य_2(wid, BIT4_X) { }
      पूर्णांक vpi_get(पूर्णांक code) override;
      vpiHandle vpi_put_मूल्य(p_vpi_मूल्य val, पूर्णांक flags) override;
      अंतरभूत स्थिर vvp_vector4_t& return_मूल्य() स्थिर { वापस return_मूल्य_2; }

    निजी:
      vpiHandle put_मूल्य_scalar(p_vpi_मूल्य vp);
      vpiHandle put_मूल्य_int(p_vpi_मूल्य vp);
      vpiHandle put_मूल्य_string(p_vpi_मूल्य vp);
      vpiHandle put_मूल्य_vector(p_vpi_मूल्य vp);
      vpiHandle put_मूल्य_time(p_vpi_मूल्य vp);

      vvp_vector4_t return_मूल्य_2;
};

पूर्णांक sysfunc_vec4::vpi_get(पूर्णांक code)
{
      चयन (code) {
	  हाल vpi_आकार:
	    वापस return_मूल्य_2.size();

	  हाल vpi_पंक्ति_No:
	    वापस lineno;

	  हाल vpiUserDefn:
	    वापस defn->is_user_defn;

	  शेष:
	    वापस vpiUndefined;
      }
}

vpiHandle sysfunc_vec4::put_मूल्य_scalar(स्थिर p_vpi_मूल्य vp)
{
      चयन (vp->value.scalar) {
	  हाल vpi0:
	    return_मूल्य_2.set_bit(0, BIT4_0);
	    अवरोध;
	  हाल vpi1:
	    return_मूल्य_2.set_bit(0, BIT4_1);
	    अवरोध;
	  हाल vpiX:
	    return_मूल्य_2.set_bit(0, BIT4_X);
	    अवरोध;
	  हाल vpiZ:
	    return_मूल्य_2.set_bit(0, BIT4_Z);
	    अवरोध;
	  शेष:
	    ख_लिखो(मानक_त्रुटि, "Unsupported value %d.\n", (पूर्णांक)vp->value.scalar);
	    निश्चित(0);
      }
      वापस 0;
}

vpiHandle sysfunc_vec4::put_मूल्य_int(p_vpi_मूल्य vp)
{
      दीर्घ tmp = vp->value.integer;
      अचिन्हित width = return_मूल्य_2.size();
      क्रम (अचिन्हित idx = 0 ;  idx < width ;  idx += 1) {
	    return_मूल्य_2.set_bit(idx, (tmp&1)? BIT4_1 : BIT4_0);
	    tmp >>= 1;
      }

      वापस 0;
}

vpiHandle sysfunc_vec4::put_मूल्य_string(p_vpi_मूल्य vp)
{
      माप_प्रकार slen = म_माप(vp->value.str);
      अचिन्हित wid = return_मूल्य_2.size();

      क्रम (अचिन्हित idx = 0 ; idx < wid ; idx += 1) {
	    अचिन्हित cidx = idx / 8;
	      // If wider then the string, then pad with zero.
	    अगर (cidx >= slen) {
		  return_मूल्य_2.set_bit(idx, BIT4_0);
		  जारी;
	    }

	      // Put the end of the string in the LSB of the vector
	    अक्षर use_char = vp->value.str[slen - 1 - cidx];

	    अगर ((use_char >> (idx % 8)) & 1)
		  return_मूल्य_2.set_bit(idx, BIT4_1);
	    अन्यथा
		  return_मूल्य_2.set_bit(idx, BIT4_0);
      }
      वापस 0;
}

vpiHandle sysfunc_vec4::put_मूल्य_vector(p_vpi_मूल्य vp)
{
      अचिन्हित width = return_मूल्य_2.size();
      क्रम (अचिन्हित idx = 0 ; idx < width ; idx += 1) {
	    अचिन्हित word = idx / 32;
	    अचिन्हित bidx = idx % 32;

	    अचिन्हित दीर्घ aval = vp->value.vector[word].aval >> bidx;
	    अचिन्हित दीर्घ bval = vp->value.vector[word].bval >> bidx;

	    पूर्णांक bit = (aval&1) | ((bval<<1)&2);
	    vvp_bit4_t bit4;

	    चयन (bit) {
		हाल 0:
		  bit4 = BIT4_0;
		  अवरोध;
		हाल 1:
		  bit4 = BIT4_1;
		  अवरोध;
		हाल 2:
		  bit4 = BIT4_Z;
		  अवरोध;
		हाल 3:
		  bit4 = BIT4_X;
		  अवरोध;
		शेष:
		  निश्चित(0);
		  bit4 = BIT4_X;
		  अवरोध;
	    }
	    return_मूल्य_2.set_bit(idx, bit4);
      }
      वापस 0;
}

vpiHandle sysfunc_vec4::put_मूल्य_time(p_vpi_मूल्य vp)
{
      अचिन्हित width = return_मूल्य_2.size();
      दीर्घ tmp = 0;
      क्रम (अचिन्हित idx = 0 ; idx < width ; idx += 1) {
	    अगर (idx == 0)
		  tmp = vp->value.समय->निचला;
	    अन्यथा अगर (idx == 32)
		  tmp = vp->value.समय->ऊपरी;
	    अन्यथा अगर (idx == 64)
		  tmp = 0;

	    return_मूल्य_2.set_bit(idx, (tmp&1)? BIT4_1 : BIT4_0);
	    tmp >>= 1;
      }
      वापस 0;
}

vpiHandle sysfunc_vec4::vpi_put_मूल्य(p_vpi_मूल्य vp, पूर्णांक)
{
      put_मूल्य = true;

      चयन (vp->format) {
	  हाल vpiScalarVal:
	    वापस put_मूल्य_scalar(vp);
	  हाल vpiIntVal:
	    वापस put_मूल्य_int(vp);
	  हाल vpiStringVal:
	    वापस put_मूल्य_string(vp);
	  हाल vpiVectorVal:
	    वापस put_मूल्य_vector(vp);
	  हाल vpiTimeVal:
	    वापस put_मूल्य_time(vp);
	  शेष:
	    ख_लिखो(मानक_त्रुटि, "Unsupported format %d setting sysfunc vec4 value.\n", (पूर्णांक)vp->format);
	    निश्चित(0);
      }

      वापस 0;
}

काष्ठा sysfunc_4net : खुला sysfunc_def {
      explicit अंतरभूत sysfunc_4net(अचिन्हित wid) : vwid_(wid) { }
      पूर्णांक vpi_get(पूर्णांक code) override;
      vpiHandle vpi_put_मूल्य(p_vpi_मूल्य val, पूर्णांक flags) override;

    निजी:
      अचिन्हित vwid_;
};

// support getting vpiSize for a system function call
पूर्णांक sysfunc_4net::vpi_get(पूर्णांक code)
{
      चयन (code) {
	  हाल vpi_आकार:
	    वापस vwid_;

	  हाल vpi_पंक्ति_No:
	    वापस lineno;

	  हाल vpiUserDefn:
	    वापस defn->is_user_defn;

	  शेष:
	    वापस vpiUndefined;
      }
}

vpiHandle sysfunc_4net::vpi_put_मूल्य(p_vpi_मूल्य vp, पूर्णांक)
{
      put_मूल्य = true;

      vvp_vector4_t val (vwid_);

      चयन (vp->format) {

          हाल vpiScalarVal: {
	        चयन(vp->value.scalar) {
		      हाल vpi0:
			val.set_bit(0, BIT4_0);
                        अवरोध;
		      हाल vpi1:
                        val.set_bit(0, BIT4_1);
                        अवरोध;
		      हाल vpiX:
                        val.set_bit(0, BIT4_X);
                        अवरोध;
		      हाल vpiZ:
                        val.set_bit(0, BIT4_Z);
                        अवरोध;
		      शेष:
                        ख_लिखो(मानक_त्रुटि, "Unsupported bit value %d.\n",
                                (पूर्णांक)vp->value.scalar);
                        निश्चित(0);
                }
		अवरोध;
          }

	  हाल vpiIntVal: {
		दीर्घ tmp = vp->value.integer;
		क्रम (अचिन्हित idx = 0 ;  idx < vwid_ ;  idx += 1) {
		      val.set_bit(idx, (tmp&1)? BIT4_1 : BIT4_0);
		      tmp >>= 1;
		}
		अवरोध;
	  }

          हाल vpiTimeVal: {
                अचिन्हित दीर्घ tmp = vp->value.समय->निचला;
                क्रम (अचिन्हित idx = 0 ;  idx < vwid_ ;  idx += 1) {
                      val.set_bit(idx, (tmp&1)? BIT4_1 : BIT4_0);

                      अगर (idx == 31)
                            tmp = vp->value.समय->ऊपरी;
                      अन्यथा
                            tmp >>= 1;
                }
                अवरोध;
          }

	  हाल vpiVectorVal:

	    क्रम (अचिन्हित wdx = 0 ;  wdx < vwid_ ;  wdx += 32) {
		  अचिन्हित word = wdx / 32;
		  अचिन्हित दीर्घ aval = vp->value.vector[word].aval;
		  अचिन्हित दीर्घ bval = vp->value.vector[word].bval;

		  क्रम (अचिन्हित idx = 0 ;  (wdx+idx) < vwid_ && idx < 32;
		       idx += 1) {
			पूर्णांक bit = (aval&1) | ((bval<<1)&2);
			vvp_bit4_t bit4;

			चयन (bit) {
			    हाल 0:
			      bit4 = BIT4_0;
			      अवरोध;
			    हाल 1:
			      bit4 = BIT4_1;
			      अवरोध;
			    हाल 2:
			      bit4 = BIT4_Z;
			      अवरोध;
			    हाल 3:
			      bit4 = BIT4_X;
			      अवरोध;
			    शेष:
			      bit4 = BIT4_X;
			      ख_लिखो(मानक_त्रुटि, "Unsupported bit value %d.\n",
			              bit);
			      निश्चित(0);
			}
			val.set_bit(wdx+idx, bit4);

			aval >>= 1;
			bval >>= 1;
		  }
	    }
	    अवरोध;

	  शेष:
	    ख_लिखो(मानक_त्रुटि, "XXXX format=%d, vwid_=%u\n", (पूर्णांक)vp->format, vwid_);
	    निश्चित(0);
      }

      fnet->send_vec4(val, vthread_get_wt_context());
      वापस 0;
}

काष्ठा sysfunc_rnet : खुला sysfunc_def {
      vpiHandle vpi_put_मूल्य(p_vpi_मूल्य val, पूर्णांक flags) override;
};

vpiHandle sysfunc_rnet::vpi_put_मूल्य(p_vpi_मूल्य vp, पूर्णांक)
{
      put_मूल्य = true;

      द्विगुन value;
      चयन (vp->format) {

	  हाल vpiRealVal:
	    value = vp->value.real;
	    अवरोध;

	  शेष:
	    value = 0.0;
	    ख_लिखो(मानक_त्रुटि, "Unsupported format %d.\n", (पूर्णांक)vp->format);
	    निश्चित(0);
      }

      fnet->send_real(value, vthread_get_wt_context());

      वापस 0;
}

काष्ठा sysfunc_no : खुला sysfunc_def {
      vpiHandle vpi_put_मूल्य(p_vpi_मूल्य val, पूर्णांक flags) override;
};

vpiHandle sysfunc_no::vpi_put_मूल्य(p_vpi_मूल्य, पूर्णांक)
{
      वापस 0;
}


  /* **** Manipulate the internal data structures. **** */

/*
 * We keep a table of all the __vpiUserSystf objects that are created
 * so that the user can iterate over them. The def_table is an array
 * of pointers to __vpiUserSystf objects. This table can be searched
 * by name using the vpi_find_systf function, and they can be
 * collected into an iterator using the vpip_make_systf_iterator function.
 */
अटल काष्ठा __vpiUserSystf**def_table = 0;
अटल अचिन्हित def_गिनती = 0;

अटल काष्ठा __vpiUserSystf* allocate_def(व्योम)
{
      अगर (def_table == 0) {
	    def_table = static_cast<काष्ठा __vpiUserSystf**>
	                (दो_स्मृति(माप(काष्ठा __vpiUserSystf*)));

	    def_table[0] = नया __vpiUserSystf;

	    def_गिनती = 1;
	    वापस def_table[0];
      }

      def_table = static_cast<काष्ठा __vpiUserSystf**>
                  (पुनः_स्मृति(def_table,
                   (def_गिनती+1)*माप(काष्ठा __vpiUserSystf*)));

      def_table[def_गिनती] = नया __vpiUserSystf;

      वापस def_table[def_गिनती++];
}

#अगर_घोषित CHECK_WITH_VALGRIND
व्योम def_table_delete(व्योम)
{
      क्रम (अचिन्हित idx = 0; idx < def_गिनती; idx += 1) {
	    मुक्त(const_cast<अक्षर *>(def_table[idx]->info.tfname));
	    मिटाओ def_table[idx];
      }
      मुक्त(def_table);
      def_table = 0;
      def_गिनती = 0;
}
#पूर्ण_अगर

श्रेणी __vpiSystfIterator : खुला __vpiHandle {
    खुला:
      explicit __vpiSystfIterator(अचिन्हित idx);
      पूर्णांक get_type_code(व्योम) स्थिर override;
      vpiHandle vpi_सूचकांक(पूर्णांक idx) override;
      free_object_fun_t free_object_fun(व्योम) override;

      अचिन्हित अग्र_2;
};

अटल vpiHandle systf_iterator_scan(vpiHandle ref, पूर्णांक)
{
      __vpiSystfIterator*obj = dynamic_cast<__vpiSystfIterator*>(ref);

      अगर (obj->अग्र_2 >= def_गिनती) {
	    vpi_free_object(ref);
	    वापस 0;
      }

      अचिन्हित use_सूचकांक = obj->अग्र_2;
      जबतक (!def_table[use_सूचकांक]->is_user_defn) {
	    obj->अग्र_2 += 1;
	    use_सूचकांक = obj->अग्र_2;
	    अगर (obj->अग्र_2 >= def_गिनती) {
		  vpi_free_object(ref);
		  वापस 0;
	    }
      }
      obj->अग्र_2 += 1;
      वापस def_table[use_सूचकांक];
}

अटल पूर्णांक systf_iterator_free_object(vpiHandle ref)
{
      __vpiSystfIterator*obj = dynamic_cast<__vpiSystfIterator*>(ref);
      मिटाओ obj;
      वापस 1;
}

अंतरभूत __vpiSystfIterator::__vpiSystfIterator(अचिन्हित idx)
{ अग्र_2 = idx; }

पूर्णांक __vpiSystfIterator::get_type_code(व्योम) स्थिर
{ वापस vpiIterator; }

vpiHandle __vpiSystfIterator::vpi_सूचकांक(पूर्णांक idx)
{ वापस systf_iterator_scan(यह, idx); }

__vpiHandle::free_object_fun_t __vpiSystfIterator::free_object_fun(व्योम)
{ वापस &systf_iterator_free_object; }

vpiHandle vpip_make_systf_iterator(व्योम)
{
	/* Check to see if there are any user defined functions. */
      bool have_user_defn = false;
      अचिन्हित idx;
      क्रम (idx = 0; idx < def_गिनती; idx += 1) {
	    अगर (def_table[idx]->is_user_defn) {
		  have_user_defn = true;
		  अवरोध;
	    }
      }
      अगर (!have_user_defn) वापस 0;

      __vpiSystfIterator*res = नया __vpiSystfIterator(idx);
      res->अग्र_2 = idx;
      वापस res;
}

काष्ठा __vpiUserSystf* vpip_find_systf(स्थिर अक्षर*name)
{
      क्रम (अचिन्हित idx = 0 ;  idx < def_गिनती ;  idx += 1)
	    अगर (म_भेद(def_table[idx]->info.tfname, name) == 0)
		  वापस def_table[idx];

      वापस 0;
}

व्योम vpip_make_systf_system_defined(vpiHandle ref)
{
      निश्चित(ref);
      काष्ठा __vpiUserSystf*obj = dynamic_cast<__vpiUserSystf*>(ref);
      निश्चित(obj);
      obj->is_user_defn = false;
}

/*
 * To get better error message we need to cache the vpi_call fail
 * information so that we can print the file name.
 */
क्रमागत vpi_call_error_type {VPI_CALL_NO_DEF, VPI_CALL_TASK_AS_FUNC,
                          VPI_CALL_FUNC_AS_TASK, VPI_CALL_FUNC_AS_TASK_WARN};
प्रकार काष्ठा vpi_call_error {
      vpi_call_error_type type;
      अक्षर *name;
      दीर्घ file_idx;
      दीर्घ lineno;
} vpi_call_error_s, *vpi_call_error_p;

अटल vpi_call_error_p vpi_call_error_lst = शून्य;
अटल अचिन्हित vpi_call_error_अंक = 0;

अटल व्योम add_vpi_call_error(vpi_call_error_type type, स्थिर अक्षर *name,
                               दीर्घ file_idx, दीर्घ lineno)
{
      vpi_call_error_lst = static_cast<vpi_call_error_p>
                           (पुनः_स्मृति(vpi_call_error_lst,
                                    (vpi_call_error_अंक + 1) *
                                    माप(vpi_call_error_s)));
      vpi_call_error_lst[vpi_call_error_अंक].type = type;
      vpi_call_error_lst[vpi_call_error_अंक].name = strdup(name);
      vpi_call_error_lst[vpi_call_error_अंक].file_idx = file_idx;
      vpi_call_error_lst[vpi_call_error_अंक].lineno = lineno;
      vpi_call_error_अंक += 1;
}

व्योम print_vpi_call_errors()
{
      क्रम (अचिन्हित idx = 0; idx < vpi_call_error_अंक; idx += 1) {
	    चयन (vpi_call_error_lst[idx].type) {
		हाल VPI_CALL_NO_DEF:
		  ख_लिखो(मानक_त्रुटि, "%s:%d: Error: System task/function %s() is "
		                  "not defined by any module.\n",
		                  file_names[vpi_call_error_lst[idx].file_idx],
		                  (पूर्णांक)vpi_call_error_lst[idx].lineno,
		                  vpi_call_error_lst[idx].name);
		  अवरोध;
		हाल VPI_CALL_TASK_AS_FUNC:
		  ख_लिखो(मानक_त्रुटि, "%s:%d: Error: %s() is a system task, it "
			          "cannot be called as a function.\n",
		                  file_names[vpi_call_error_lst[idx].file_idx],
		                  (पूर्णांक)vpi_call_error_lst[idx].lineno,
		                  vpi_call_error_lst[idx].name);
		  अवरोध;
		हाल VPI_CALL_FUNC_AS_TASK:
		  ख_लिखो(मानक_त्रुटि, "%s:%d: Error: %s() is a system function, it "
			          "cannot be called as a task.\n",
		                  file_names[vpi_call_error_lst[idx].file_idx],
		                  (पूर्णांक)vpi_call_error_lst[idx].lineno,
		                  vpi_call_error_lst[idx].name);
		  अवरोध;
		हाल VPI_CALL_FUNC_AS_TASK_WARN:
                 ख_लिखो(मानक_त्रुटि, "%s:%d: Warning: Calling system function "
                                 "%s() as a task.\n",
		                  file_names[vpi_call_error_lst[idx].file_idx],
		                  (पूर्णांक)vpi_call_error_lst[idx].lineno,
		                  vpi_call_error_lst[idx].name);
                 ख_लिखो(मानक_त्रुटि, "%s:%d:          The functions return "
                                 "value will be ignored.\n",
                                 file_names[vpi_call_error_lst[idx].file_idx],
                                 (पूर्णांक)vpi_call_error_lst[idx].lineno);
		  अवरोध;
	    }
	    मुक्त(vpi_call_error_lst[idx].name);
      }
      मुक्त(vpi_call_error_lst);
      ख_साफ(मानक_त्रुटि);
}

#अगर_घोषित CHECK_WITH_VALGRIND
अटल व्योम cleanup_vpi_call_args(अचिन्हित argc, vpiHandle*argv)
{
      अगर (argc) {
	      /* Since this is just being used to cleanup the arguments a
	       * system task definition can be used. */
	    काष्ठा __vpiSysTaskCall*obj = नया systask_def;
	    obj->nargs = argc;
	    obj->args  = argv;
	    vpi_call_delete(obj);
      }
}
#पूर्ण_अगर

/*
 * A vpi_call is actually built up into a vpiSysTaskCall VPI object
 * that refers back to the vpiUserSystf VPI object that is the
 * definition. So this function is called by the compiler when a
 * %vpi_call statement is encountered. Create here a vpiHandle that
 * describes the call, and return it. The %vpi_call instruction will
 * store this handle for when it is executed.
 */
vpiHandle vpip_build_vpi_call(स्थिर अक्षर*name, पूर्णांक val_code, अचिन्हित return_width,
			      vvp_net_t*fnet,
			      bool func_as_task_err, bool func_as_task_warn,
			      अचिन्हित argc, vpiHandle*argv,
			      अचिन्हित vec4_ढेर, अचिन्हित real_ढेर, अचिन्हित string_ढेर,
			      दीर्घ file_idx, दीर्घ lineno)
{
      निश्चित(!(func_as_task_err && func_as_task_warn));

      काष्ठा __vpiUserSystf*defn = vpip_find_systf(name);
      अगर (defn == 0) {
	    add_vpi_call_error(VPI_CALL_NO_DEF, name, file_idx, lineno);
#अगर_घोषित CHECK_WITH_VALGRIND
	    cleanup_vpi_call_args(argc, argv);
#पूर्ण_अगर
	    वापस 0;
      }

      चयन (defn->info.type) {
	  हाल vpiSysTask:
	    अगर (val_code != 0 || fnet != 0) {
		  add_vpi_call_error(VPI_CALL_TASK_AS_FUNC, name, file_idx,
		                     lineno);
#अगर_घोषित CHECK_WITH_VALGRIND
		  cleanup_vpi_call_args(argc, argv);
#पूर्ण_अगर
		  वापस 0;
	    }
	    अवरोध;

	  हाल vpiSysFunc:
	    अगर (val_code == 0 && fnet == 0) {
		  अगर (func_as_task_err) {
			add_vpi_call_error(VPI_CALL_FUNC_AS_TASK,
			                   name, file_idx, lineno);
#अगर_घोषित CHECK_WITH_VALGRIND
			cleanup_vpi_call_args(argc, argv);
#पूर्ण_अगर
			वापस 0;
		  } अन्यथा अगर (func_as_task_warn) {
			add_vpi_call_error(VPI_CALL_FUNC_AS_TASK_WARN,
			                   name, file_idx, lineno);
		  }
	    }
	    अवरोध;

	  शेष:
	    ख_लिखो(मानक_त्रुटि, "Unsupported vpi_call type %d.\n",
	                    (पूर्णांक)defn->info.type);
	    निश्चित(0);
      }

      काष्ठा __vpiSysTaskCall*obj = 0;

      चयन (defn->info.type) {
	  हाल vpiSysTask:
	    obj = नया systask_def;
	    अवरोध;

	  हाल vpiSysFunc:
	    अगर (fnet && val_code == -vpiRealVal) {
		  obj = नया sysfunc_rnet;

	    } अन्यथा अगर (fnet && val_code == -vpiVectorVal) {
		  obj = नया sysfunc_4net(return_width);

	    } अन्यथा अगर (val_code == -vpiRealVal) {
		  obj = नया sysfunc_real;

	    } अन्यथा अगर (val_code == -vpiVectorVal) {
		  obj = नया sysfunc_vec4(return_width);

	    } अन्यथा अगर (val_code == -vpiStringVal) {
		  obj = नया sysfunc_str;

           } अन्यथा अगर (val_code == 0 && fnet == 0) {
		  obj = नया sysfunc_no;

	    } अन्यथा {
		  ख_लिखो(मानक_त्रुटि, "XXXX fnet=%p, val_code=%d\n", fnet, val_code);
		  निश्चित(0);
	    }
	    अवरोध;
      }

      obj->scope = vpip_peek_current_scope();
      obj->defn  = defn;
      obj->nargs = argc;
      obj->args  = argv;
      obj->vec4_ढेर = vec4_ढेर;
      obj->real_ढेर = real_ढेर;
      obj->string_ढेर = string_ढेर;
      obj->fnet  = fnet;
      obj->file_idx  = (अचिन्हित) file_idx;
      obj->lineno   = (अचिन्हित) lineno;
      obj->userdata  = 0;
      obj->put_मूल्य = false;

      compile_compiletf(obj);

      वापस obj;
}

#अगर_घोषित CHECK_WITH_VALGRIND
व्योम vpi_call_delete(vpiHandle item)
{
      काष्ठा __vpiSysTaskCall*obj = dynamic_cast<__vpiSysTaskCall*>(item);
	/* The object can be NULL if there was an error. */
      अगर (!obj) वापस;
      क्रम (अचिन्हित arg = 0; arg < obj->nargs; arg += 1) {
	    चयन (obj->args[arg]->get_type_code()) {
		हाल vpiConstant:
		  चयन (vpi_get(_vpiFromThr, obj->args[arg])) {
		      हाल _vpiNoThr:
			constant_delete(obj->args[arg]);
			अवरोध;
		      हाल _vpiString:
			thread_string_delete(obj->args[arg]);
			अवरोध;
		      हाल _vpiVThr:
			thread_vthr_delete(obj->args[arg]);
			अवरोध;
		      हाल _vpiWord:
			thread_word_delete(obj->args[arg]);
			अवरोध;
		      शेष:
			निश्चित(0);
		  }
		  अवरोध;
		हाल vpiMemoryWord:
		  चयन (vpi_get(_vpiFromThr, obj->args[arg])) {
		      हाल _vpi_at_A:
			A_delete(obj->args[arg]);
			अवरोध;
		  }
		  अवरोध;
		हाल vpiPartSelect:
		  चयन (vpi_get(_vpiFromThr, obj->args[arg])) {
		      हाल _vpi_at_PV:
			PV_delete(obj->args[arg]);
			अवरोध;
		      हाल _vpi_at_APV:
			APV_delete(obj->args[arg]);
			अवरोध;
		  }
		  अवरोध;
	    }
      }
      मुक्त(obj->args);
      मिटाओ obj;
}
#पूर्ण_अगर

/*
 * This function is used by the %vpi_call instruction to actually
 * place the call to the system task/function. For now, only support
 * calls to system tasks.
 */

vthread_t vpip_current_vthread;

व्योम vpip_execute_vpi_call(vthread_t thr, vpiHandle ref)
{
      vpip_current_vthread = thr;

      vpip_cur_task = dynamic_cast<__vpiSysTaskCall*>(ref);

      अगर (vpip_cur_task->defn->info.calltf) {
	    निश्चित(vpi_mode_flag == VPI_MODE_NONE);
	    vpi_mode_flag = VPI_MODE_CALLTF;
	    vpip_cur_task->put_मूल्य = false;
	    vpip_cur_task->defn->info.calltf(vpip_cur_task->defn->info.user_तथ्य);
	    vpi_mode_flag = VPI_MODE_NONE;
	      /* If the function call did not set a value then put a
	       * default value (0). */
	    अगर (ref->get_type_code() == vpiSysFuncCall &&
	        !vpip_cur_task->put_मूल्य) {
		  s_vpi_मूल्य val;
		  val.format = vpiIntVal;
		  val.value.integer = 0;
		  vpi_put_मूल्य(ref, &val, 0, vpiNoDelay);
	    }
      }
      अगर (vpip_cur_task->vec4_ढेर > 0)
	    vthread_निकालो_vec4(thr, vpip_cur_task->vec4_ढेर);
      अगर (vpip_cur_task->real_ढेर > 0)
	    vthread_निकालो_real(thr, vpip_cur_task->real_ढेर);
      अगर (vpip_cur_task->string_ढेर > 0)
	    vthread_निकालो_str(thr, vpip_cur_task->string_ढेर);

	/* If the function returns a value, then push the value
	   to the appropriate thread stack. */
      अगर (स्थिर sysfunc_real*func_real = dynamic_cast<sysfunc_real*>(ref)) {
	    vthread_धकेलो(thr, func_real->return_मूल्य());
      }
      अन्यथा अगर (स्थिर sysfunc_str*func_string = dynamic_cast<sysfunc_str*>(ref)) {
	    vthread_धकेलो(thr, func_string->return_मूल्य());
      }
      अन्यथा अगर (स्थिर sysfunc_vec4*func_vec4 = dynamic_cast<sysfunc_vec4*>(ref)) {
	    vthread_धकेलो(thr, func_vec4->return_मूल्य());
      }
      vpip_cur_task = 0;
}

/*
 * This is the entry function that a VPI module uses to hook a new
 * task/function into the simulator. The function creates a new
 * __vpi_userSystf to represent the definition for the calls that come
 * to pass later.
 */
vpiHandle vpi_register_systf(स्थिर काष्ठा t_vpi_systf_तथ्य*ss)
{
      काष्ठा __vpiUserSystf*cur = allocate_def();
      निश्चित(ss);
      चयन (ss->type) {
	  हाल vpiSysTask:
	  हाल vpiSysFunc:
	    अवरोध;
	  शेष:
	    ख_लिखो(मानक_त्रुटि, "Unsupported type %d.\n", (पूर्णांक)ss->type);
	    निश्चित(0);
      }

      cur->info = *ss;
      cur->info.tfname = strdup(ss->tfname);
      cur->is_user_defn = true;

      वापस cur;
}

PLI_INT32 vpi_put_userdata(vpiHandle ref, व्योम*तथ्य)
{
      काष्ठा __vpiSysTaskCall*rfp = dynamic_cast<__vpiSysTaskCall*>(ref);
      अगर (rfp == 0)
	    वापस 0;

      rfp->userdata = तथ्य;
      वापस 1;
}

व्योम* vpi_get_userdata(vpiHandle ref)
{
      काष्ठा __vpiSysTaskCall*rfp = dynamic_cast<__vpiSysTaskCall*>(ref);
      निश्चित(rfp);

      वापस rfp->userdata;
}
