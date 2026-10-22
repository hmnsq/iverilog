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

తరగతి __vpiFileLine : బహిరంగ __vpiHandle {
    బహిరంగ:
      స్పష్టమైన __vpiFileLine(స్థిరరాశి అక్షరం*desc_, దీర్ఘ file_idx_, దీర్ఘ lineno_);
      పూర్ణాంకం get_type_సంకేతం(శూన్య) స్థిరరాశి override;
      పూర్ణాంకం vpi_get(పూర్ణాంకం code) override;
      అక్షరం* vpi_get_str(పూర్ణాంకం code) override;

      స్థిరరాశి అక్షరం* get_description() స్థిరరాశి { తిరిగి description; };
      గుర్తులేని get_file_idx() స్థిరరాశి { తిరిగి file_idx; };
      గుర్తులేని get_lineno() స్థిరరాశి { తిరిగి lineno; };

    అంతరంగ:
      స్థిరరాశి అక్షరం *description;
      గుర్తులేని file_idx;
      గుర్తులేని lineno;
};

తార్కిక show_file_పంక్తి = అబద్ధం;
తార్కిక సంకేతం_is_instrumented = అబద్ధం;

స్థిర పూర్ణాంకం file_పంక్తి_get(పూర్ణాంకం type, vpiHandle ref)
{
      __vpiFileLine*rfp = dynamic_cast<__vpiFileLine*>(ref);
      ప్రతిజ్ఞా(rfp);

      ఎంపిక (type) {
	స్థితి vpi_పంక్తి_No:
	    తిరిగి rfp->get_lineno();
	అప్రమేయ:
	    తిరిగి vpiUndefined;
      }
}

స్థిర అక్షరం *file_పంక్తి_get_str(పూర్ణాంకం type, vpiHandle ref)
{
      __vpiFileLine*rfp = dynamic_cast<__vpiFileLine*>(ref);
      ప్రతిజ్ఞా(rfp);

      ఎంపిక (type) {
	స్థితి vpiFile:
	    ప్రతిజ్ఞా(rfp->get_file_idx() < file_పేర్లు.size());
	    తిరిగి simple_సమితి_rbuf_str(file_పేర్లు[rfp->get_file_idx()]);
	స్థితి _vpiDescription:
	    ఒకవేళ (rfp->get_description()) {
		  తిరిగి simple_సమితి_rbuf_str(rfp->get_description());
	    } లేకపోతే తిరిగి simple_సమితి_rbuf_str("Procedural tracing.");
	అప్రమేయ:
	    తిరిగి 0;
      }
}

అంతఃవరుస __vpiFileLine::__vpiFileLine(స్థిరరాశి అక్షరం*desc_, దీర్ఘ file_idx_,
                                    దీర్ఘ lineno_)
{
      ఒకవేళ (desc_) description = vpip_name_string(desc_);
      లేకపోతే description = 0;
      file_idx = (గుర్తులేని) file_idx_;
      lineno = (గుర్తులేని) lineno_;
}

పూర్ణాంకం __vpiFileLine::get_type_సంకేతం(శూన్య) స్థిరరాశి
{ తిరిగి vpi_File_పంక్తి; }

పూర్ణాంకం __vpiFileLine::vpi_get(పూర్ణాంకం code)
{ తిరిగి file_పంక్తి_get(code, ఇది); }

అక్షరం* __vpiFileLine::vpi_get_str(పూర్ణాంకం code)
{ తిరిగి file_పంక్తి_get_str(code, ఇది); }


vpiHandle vpip_build_file_పంక్తి(స్థిరరాశి అక్షరం*description, దీర్ఘ file_idx, దీర్ఘ lineno)
{
      __vpiFileLine*obj = కొత్త __vpiFileLine(description, file_idx, lineno);

	/* You can turn on the diagnostic output if we find a %file_line. */
      సంకేతం_is_instrumented = నిజం;

      తిరిగి obj;
}
