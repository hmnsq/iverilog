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

ਵਰਗ __vpiFileLine : ਜਨਤਕ __vpiHandle {
    ਜਨਤਕ:
      ਸਪਸ਼ਟ __vpiFileLine(ਅਚਲ ਅੱਖਰ*desc_, ਲੰਮਾ file_idx_, ਲੰਮਾ lineno_);
      ਪੂਰਨ get_type_ਕੋਡ(ਖਾਲੀ) ਅਚਲ override;
      ਪੂਰਨ vpi_get(ਪੂਰਨ code) override;
      ਅੱਖਰ* vpi_get_str(ਪੂਰਨ code) override;

      ਅਚਲ ਅੱਖਰ* get_description() ਅਚਲ { ਵਾਪਸ description; };
      ਬਿਨਾਨਿਸ਼ਾਨੀ get_file_idx() ਅਚਲ { ਵਾਪਸ file_idx; };
      ਬਿਨਾਨਿਸ਼ਾਨੀ get_lineno() ਅਚਲ { ਵਾਪਸ lineno; };

    ਨਿਜੀ:
      ਅਚਲ ਅੱਖਰ *description;
      ਬਿਨਾਨਿਸ਼ਾਨੀ file_idx;
      ਬਿਨਾਨਿਸ਼ਾਨੀ lineno;
};

ਤਾਰਕਿਕ show_file_ਸਤਰ = ਝੂਠ;
ਤਾਰਕਿਕ ਕੋਡ_is_instrumented = ਝੂਠ;

ਸਥਿਰ ਪੂਰਨ file_ਸਤਰ_get(ਪੂਰਨ type, vpiHandle ref)
{
      __vpiFileLine*rfp = dynamic_cast<__vpiFileLine*>(ref);
      ਪ੍ਰਤਿਜ੍ਞਾ(rfp);

      ਚੋਣ (type) {
	ਹਾਲਤ vpi_ਸਤਰ_No:
	    ਵਾਪਸ rfp->get_lineno();
	ਮੂਲ:
	    ਵਾਪਸ vpiUndefined;
      }
}

ਸਥਿਰ ਅੱਖਰ *file_ਸਤਰ_get_str(ਪੂਰਨ type, vpiHandle ref)
{
      __vpiFileLine*rfp = dynamic_cast<__vpiFileLine*>(ref);
      ਪ੍ਰਤਿਜ੍ਞਾ(rfp);

      ਚੋਣ (type) {
	ਹਾਲਤ vpiFile:
	    ਪ੍ਰਤਿਜ੍ਞਾ(rfp->get_file_idx() < file_ਨਾਂ.size());
	    ਵਾਪਸ simple_ਸੈੱਟ_rbuf_str(file_ਨਾਂ[rfp->get_file_idx()]);
	ਹਾਲਤ _vpiDescription:
	    ਜੇ (rfp->get_description()) {
		  ਵਾਪਸ simple_ਸੈੱਟ_rbuf_str(rfp->get_description());
	    } ਨਹੀਂਤਾਂ ਵਾਪਸ simple_ਸੈੱਟ_rbuf_str("Procedural tracing.");
	ਮੂਲ:
	    ਵਾਪਸ 0;
      }
}

ਸਤਰਅੰਦਰ __vpiFileLine::__vpiFileLine(ਅਚਲ ਅੱਖਰ*desc_, ਲੰਮਾ file_idx_,
                                    ਲੰਮਾ lineno_)
{
      ਜੇ (desc_) description = vpip_name_string(desc_);
      ਨਹੀਂਤਾਂ description = 0;
      file_idx = (ਬਿਨਾਨਿਸ਼ਾਨੀ) file_idx_;
      lineno = (ਬਿਨਾਨਿਸ਼ਾਨੀ) lineno_;
}

ਪੂਰਨ __vpiFileLine::get_type_ਕੋਡ(ਖਾਲੀ) ਅਚਲ
{ ਵਾਪਸ vpi_File_ਸਤਰ; }

ਪੂਰਨ __vpiFileLine::vpi_get(ਪੂਰਨ code)
{ ਵਾਪਸ file_ਸਤਰ_get(code, ਇਹ); }

ਅੱਖਰ* __vpiFileLine::vpi_get_str(ਪੂਰਨ code)
{ ਵਾਪਸ file_ਸਤਰ_get_str(code, ਇਹ); }


vpiHandle vpip_build_file_ਸਤਰ(ਅਚਲ ਅੱਖਰ*description, ਲੰਮਾ file_idx, ਲੰਮਾ lineno)
{
      __vpiFileLine*obj = ਨਵਾਂ __vpiFileLine(description, file_idx, lineno);

	/* You can turn on the diagnostic output if we find a %file_line. */
      ਕੋਡ_is_instrumented = ਸੱਚ;

      ਵਾਪਸ obj;
}
