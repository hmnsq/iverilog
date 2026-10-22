/*
 * Copyright (C) 2011-2025 Cary R. (cygcary@yahoo.com)
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License along
 *  with this program; if not, write to the Free Software Foundation, Inc.,
 *  51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 */

# include "compile.h"
# include "vpi_priv.h"

ଶ୍ରେଣୀ __vpiFileLine : ସର୍ବସାଧାରଣ __vpiHandle {
    ସର୍ବସାଧାରଣ:
      explicit __vpiFileLine(ଅଚଳ ଅକ୍ଷର*desc_, long file_idx_, long lineno_);
      ପୂର୍ଣ୍ଣସଂଖ୍ୟା get_type_code(ଶୂନ୍ୟ) ଅଚଳ override;
      ପୂର୍ଣ୍ଣସଂଖ୍ୟା vpi_get(ପୂର୍ଣ୍ଣସଂଖ୍ୟା code) override;
      ଅକ୍ଷର* vpi_get_str(ପୂର୍ଣ୍ଣସଂଖ୍ୟା code) override;

      ଅଚଳ ଅକ୍ଷର* get_description() ଅଚଳ { ଫେରନ୍ତୁ description; };
      unsigned get_file_idx() ଅଚଳ { ଫେରନ୍ତୁ file_idx; };
      unsigned get_lineno() ଅଚଳ { ଫେରନ୍ତୁ lineno; };

    ବ୍ୟକ୍ତିଗତ:
      ଅଚଳ ଅକ୍ଷର *description;
      unsigned file_idx;
      unsigned lineno;
};

ତାର୍କିକ show_file_ଧାଡ଼ି = ମିଥ୍ୟା;
ତାର୍କିକ code_is_instrumented = ମିଥ୍ୟା;

ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା file_ଧାଡ଼ି_get(ପୂର୍ଣ୍ଣସଂଖ୍ୟା type, vpiHandle ref)
{
      __vpiFileLine*rfp = dynamic_cast<__vpiFileLine*>(ref);
      ପ୍ରତିଜ୍ଞା(rfp);

      ବାଛ (type) {
	ସ୍ଥିତି vpi_ଧାଡ଼ି_No:
	    ଫେରନ୍ତୁ rfp->get_lineno();
	ପୂର୍ବନିର୍ଦ୍ଧାରିତ:
	    ଫେରନ୍ତୁ vpiUndefined;
      }
}

ସ୍ଥିର ଅକ୍ଷର *file_ଧାଡ଼ି_get_str(ପୂର୍ଣ୍ଣସଂଖ୍ୟା type, vpiHandle ref)
{
      __vpiFileLine*rfp = dynamic_cast<__vpiFileLine*>(ref);
      ପ୍ରତିଜ୍ଞା(rfp);

      ବାଛ (type) {
	ସ୍ଥିତି vpiFile:
	    ପ୍ରତିଜ୍ଞା(rfp->get_file_idx() < file_names.size());
	    ଫେରନ୍ତୁ simple_set_rbuf_str(file_names[rfp->get_file_idx()]);
	ସ୍ଥିତି _vpiDescription:
	    ଯଦି (rfp->get_description()) {
		  ଫେରନ୍ତୁ simple_set_rbuf_str(rfp->get_description());
	    } ଅନ୍ୟଥା ଫେରନ୍ତୁ simple_set_rbuf_str("Procedural tracing.");
	ପୂର୍ବନିର୍ଦ୍ଧାରିତ:
	    ଫେରନ୍ତୁ 0;
      }
}

ଧାଡ଼ିଭିତରେ __vpiFileLine::__vpiFileLine(ଅଚଳ ଅକ୍ଷର*desc_, long file_idx_,
                                    long lineno_)
{
      ଯଦି (desc_) description = vpip_name_string(desc_);
      ଅନ୍ୟଥା description = 0;
      file_idx = (unsigned) file_idx_;
      lineno = (unsigned) lineno_;
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା __vpiFileLine::get_type_code(ଶୂନ୍ୟ) ଅଚଳ
{ ଫେରନ୍ତୁ vpi_File_ଧାଡ଼ି; }

ପୂର୍ଣ୍ଣସଂଖ୍ୟା __vpiFileLine::vpi_get(ପୂର୍ଣ୍ଣସଂଖ୍ୟା code)
{ ଫେରନ୍ତୁ file_ଧାଡ଼ି_get(code, ଏହି); }

ଅକ୍ଷର* __vpiFileLine::vpi_get_str(ପୂର୍ଣ୍ଣସଂଖ୍ୟା code)
{ ଫେରନ୍ତୁ file_ଧାଡ଼ି_get_str(code, ଏହି); }


vpiHandle vpip_build_file_ଧାଡ଼ି(ଅଚଳ ଅକ୍ଷର*description, long file_idx, long lineno)
{
      __vpiFileLine*obj = ନୂତନ __vpiFileLine(description, file_idx, lineno);

	/* You can turn on the diagnostic output if we find a %file_line. */
      code_is_instrumented = ସତ୍ୟ;

      ଫେରନ୍ତୁ obj;
}
