#నిర్వచితం_కాకపోతే IVL_vvp_darray_H
#నిర్వచించు IVL_vvp_darray_H
/*
 * Copyright (c) 2012-2025 Stephen Williams (steve@icarus.com)
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

# include  "vvp_object.h"
# include  "vvp_net.h"
# include  <deque>
# include  <string>
# include  <vector>

class vvp_darray : బహిరంగ vvp_object {

    బహిరంగ:
      అంతఃవరుస vvp_darray() { }
      మాయా ~vvp_darray() override;

      మాయా size_t get_కొలత(శూన్య) స్థిరరాశి =0;

      మాయా శూన్య సమితి_మాట(గుర్తులేని adr, స్థిరరాశి vvp_vector4_t&విలువ);
      మాయా శూన్య get_మాట(గుర్తులేని adr, vvp_vector4_t&విలువ);

      మాయా శూన్య సమితి_మాట(గుర్తులేని adr, ద్విగుణ విలువ);
      మాయా శూన్య get_మాట(గుర్తులేని adr, ద్విగుణ&విలువ);

      మాయా శూన్య సమితి_మాట(గుర్తులేని adr, స్థిరరాశి std::string&విలువ);
      మాయా శూన్య get_మాట(గుర్తులేని adr, std::string&విలువ);

      మాయా శూన్య సమితి_మాట(గుర్తులేని adr, స్థిరరాశి vvp_object_t&విలువ);
      మాయా శూన్య get_మాట(గుర్తులేని adr, vvp_object_t&విలువ);

      మాయా vvp_vector4_t get_bitstream(bool as_vec4);

	/* In-place element order reversal (dynamic arrays and queues). */
      మాయా శూన్య తిరగేయి_elems(శూన్య);

	/* sort: ascending if true, descending if false; shuffle randomizes. */
      మాయా శూన్య క్రమీకరించు_elems(bool ascending);
      మాయా శూన్య shuffle_elems(శూన్య);
};

మూస <class TYPE> class vvp_darray_atom : బహిరంగ vvp_darray {

    బహిరంగ:
      explicit అంతఃవరుస vvp_darray_atom(size_t siz) : array_(siz) { }
      ~vvp_darray_atom() override;

      size_t get_కొలత(శూన్య) స్థిరరాశి override;
      శూన్య తిరగేయి_elems(శూన్య) override;
      శూన్య క్రమీకరించు_elems(bool ascending) override;
      శూన్య shuffle_elems(శూన్య) override;
      శూన్య సమితి_మాట(గుర్తులేని adr, స్థిరరాశి vvp_vector4_t&విలువ) override;
      శూన్య get_మాట(గుర్తులేని adr, vvp_vector4_t&విలువ) override;
      శూన్య shallow_copy(స్థిరరాశి vvp_object*obj) override;
      vvp_object* duplicate(శూన్య) స్థిరరాశి override;
      vvp_vector4_t get_bitstream(bool as_vec4) override;

    అంతరంగ:
      std::సదిశ<TYPE> array_;
};

class vvp_darray_vec4 : బహిరంగ vvp_darray {

    బహిరంగ:
      అంతఃవరుస vvp_darray_vec4(size_t siz, గుర్తులేని మాట_wid) :
                             array_(siz), మాట_wid_2(మాట_wid) { }
      ~vvp_darray_vec4() override;

      size_t get_కొలత(శూన్య) స్థిరరాశి override;
      శూన్య సమితి_మాట(గుర్తులేని adr, స్థిరరాశి vvp_vector4_t&విలువ) override;
      శూన్య get_మాట(గుర్తులేని adr, vvp_vector4_t&విలువ) override;
      శూన్య shallow_copy(స్థిరరాశి vvp_object*obj) override;
      vvp_object* duplicate(శూన్య) స్థిరరాశి override;
      vvp_vector4_t get_bitstream(bool as_vec4) override;
      శూన్య తిరగేయి_elems(శూన్య) override;
      శూన్య క్రమీకరించు_elems(bool ascending) override;
      శూన్య shuffle_elems(శూన్య) override;

    అంతరంగ:
      std::సదిశ<vvp_vector4_t> array_;
      గుర్తులేని మాట_wid_2;
};

class vvp_darray_vec2 : బహిరంగ vvp_darray {

    బహిరంగ:
      అంతఃవరుస vvp_darray_vec2(size_t siz, గుర్తులేని మాట_wid) :
                             array_(siz), మాట_wid_2(మాట_wid) { }
      ~vvp_darray_vec2() override;

      size_t get_కొలత(శూన్య) స్థిరరాశి override;
      శూన్య తిరగేయి_elems(శూన్య) override;
      శూన్య క్రమీకరించు_elems(bool ascending) override;
      శూన్య shuffle_elems(శూన్య) override;
      శూన్య సమితి_మాట(గుర్తులేని adr, స్థిరరాశి vvp_vector4_t&విలువ) override;
      శూన్య get_మాట(గుర్తులేని adr, vvp_vector4_t&విలువ) override;
      శూన్య shallow_copy(స్థిరరాశి vvp_object*obj) override;
      vvp_vector4_t get_bitstream(bool as_vec4) override;

    అంతరంగ:
      std::సదిశ<vvp_vector2_t> array_;
      గుర్తులేని మాట_wid_2;
};

class vvp_darray_real : బహిరంగ vvp_darray {

    బహిరంగ:
      explicit అంతఃవరుస vvp_darray_real(size_t siz) : array_(siz) { }
      ~vvp_darray_real() override;

      size_t get_కొలత(శూన్య) స్థిరరాశి override;
      శూన్య సమితి_మాట(గుర్తులేని adr, ద్విగుణ విలువ) override;
      శూన్య get_మాట(గుర్తులేని adr, ద్విగుణ&విలువ) override;
      శూన్య shallow_copy(స్థిరరాశి vvp_object*obj) override;
      vvp_object* duplicate(శూన్య) స్థిరరాశి override;
      vvp_vector4_t get_bitstream(bool as_vec4) override;
      శూన్య తిరగేయి_elems(శూన్య) override;
      శూన్య క్రమీకరించు_elems(bool ascending) override;
      శూన్య shuffle_elems(శూన్య) override;

    అంతరంగ:
      std::సదిశ<ద్విగుణ> array_;
};

class vvp_darray_string : బహిరంగ vvp_darray {

    బహిరంగ:
      explicit అంతఃవరుస vvp_darray_string(size_t siz) : array_(siz) { }
      ~vvp_darray_string() override;

      size_t get_కొలత(శూన్య) స్థిరరాశి override;
      శూన్య సమితి_మాట(గుర్తులేని adr, స్థిరరాశి std::string&విలువ) override;
      శూన్య get_మాట(గుర్తులేని adr, std::string&విలువ) override;
      శూన్య shallow_copy(స్థిరరాశి vvp_object*obj) override;
      vvp_object* duplicate(శూన్య) స్థిరరాశి override;
      శూన్య తిరగేయి_elems(శూన్య) override;
      శూన్య క్రమీకరించు_elems(bool ascending) override;
      శూన్య shuffle_elems(శూన్య) override;

    అంతరంగ:
      std::సదిశ<std::string> array_;
};

class vvp_darray_object : బహిరంగ vvp_darray {

    బహిరంగ:
      explicit అంతఃవరుస vvp_darray_object(size_t siz) : array_(siz) { }
      ~vvp_darray_object() override;

      size_t get_కొలత(శూన్య) స్థిరరాశి override;
      శూన్య తిరగేయి_elems(శూన్య) override;
      శూన్య క్రమీకరించు_elems(bool ascending) override;
      శూన్య shuffle_elems(శూన్య) override;
      శూన్య సమితి_మాట(గుర్తులేని adr, స్థిరరాశి vvp_object_t&విలువ) override;
      శూన్య get_మాట(గుర్తులేని adr, vvp_object_t&విలువ) override;
      శూన్య shallow_copy(స్థిరరాశి vvp_object*obj) override;
      //virtual vvp_object* duplicate(void) const;

    అంతరంగ:
      std::సదిశ<vvp_object_t> array_;
};

class vvp_వరుస : బహిరంగ vvp_darray {

    బహిరంగ:
      అంతఃవరుస vvp_వరుస(శూన్య) { }
      ~vvp_వరుస() override;

      మాయా size_t get_కొలత(శూన్య) స్థిరరాశి override =0;
      మాయా శూన్య copy_elems(vvp_object_t src, గుర్తులేని max_కొలత);

      మాయా శూన్య సమితి_మాట_max(గుర్తులేని adr, స్థిరరాశి vvp_vector4_t&విలువ, గుర్తులేని max_కొలత);
      మాయా శూన్య చొప్పించు(గుర్తులేని idx, స్థిరరాశి vvp_vector4_t&విలువ, గుర్తులేని max_కొలత);
      మాయా శూన్య నెట్టు_వెనుక(స్థిరరాశి vvp_vector4_t&విలువ, గుర్తులేని max_కొలత);
      మాయా శూన్య నెట్టు_front(స్థిరరాశి vvp_vector4_t&విలువ, గుర్తులేని max_కొలత);

      మాయా శూన్య సమితి_మాట_max(గుర్తులేని adr, ద్విగుణ విలువ, గుర్తులేని max_కొలత);
      మాయా శూన్య చొప్పించు(గుర్తులేని idx, ద్విగుణ విలువ, గుర్తులేని max_కొలత);
      మాయా శూన్య నెట్టు_వెనుక(ద్విగుణ విలువ, గుర్తులేని max_కొలత);
      మాయా శూన్య నెట్టు_front(ద్విగుణ విలువ, గుర్తులేని max_కొలత);

      మాయా శూన్య సమితి_మాట_max(గుర్తులేని adr, స్థిరరాశి std::string&విలువ, గుర్తులేని max_కొలత);
      మాయా శూన్య చొప్పించు(గుర్తులేని idx, స్థిరరాశి std::string&విలువ, గుర్తులేని max_కొలత);
      మాయా శూన్య నెట్టు_వెనుక(స్థిరరాశి std::string&విలువ, గుర్తులేని max_కొలత);
      మాయా శూన్య నెట్టు_front(స్థిరరాశి std::string&విలువ, గుర్తులేని max_కొలత);

      మాయా శూన్య తీయి_వెనుక(శూన్య) =0;
      మాయా శూన్య తీయి_front(శూన్య)=0;
      మాయా శూన్య erase(గుర్తులేని idx)=0;
      మాయా శూన్య erase_తోక(గుర్తులేని idx)=0;
};

class vvp_వరుస_real : బహిరంగ vvp_వరుస {

    బహిరంగ:
      size_t get_కొలత(శూన్య) స్థిరరాశి override { తిరిగి వరుస.కొలత(); };
      శూన్య copy_elems(vvp_object_t src, గుర్తులేని max_కొలత) override;
      శూన్య సమితి_మాట_max(గుర్తులేని adr, ద్విగుణ విలువ, గుర్తులేని max_కొలత) override;
      శూన్య సమితి_మాట(గుర్తులేని adr, ద్విగుణ విలువ) override;
      శూన్య get_మాట(గుర్తులేని adr, ద్విగుణ&విలువ) override;
      శూన్య చొప్పించు(గుర్తులేని idx, ద్విగుణ విలువ, గుర్తులేని max_కొలత) override;
      శూన్య నెట్టు_వెనుక(ద్విగుణ విలువ, గుర్తులేని max_కొలత) override;
      శూన్య నెట్టు_front(ద్విగుణ విలువ, గుర్తులేని max_కొలత) override;
      శూన్య తీయి_వెనుక(శూన్య) override { వరుస.తీయి_వెనుక(); };
      శూన్య తీయి_front(శూన్య) override { వరుస.తీయి_front(); };
      శూన్య erase(గుర్తులేని idx) override;
      శూన్య erase_తోక(గుర్తులేని idx) override;
      శూన్య తిరగేయి_elems(శూన్య) override;
      శూన్య క్రమీకరించు_elems(bool ascending) override;
      శూన్య shuffle_elems(శూన్య) override;

    అంతరంగ:
      std::deque<ద్విగుణ> వరుస;
};

class vvp_వరుస_string : బహిరంగ vvp_వరుస {

    బహిరంగ:
      size_t get_కొలత(శూన్య) స్థిరరాశి override { తిరిగి వరుస.కొలత(); };
      శూన్య copy_elems(vvp_object_t src, గుర్తులేని max_కొలత) override;
      శూన్య సమితి_మాట_max(గుర్తులేని adr, స్థిరరాశి std::string&విలువ, గుర్తులేని max_కొలత) override;
      శూన్య సమితి_మాట(గుర్తులేని adr, స్థిరరాశి std::string&విలువ) override;
      శూన్య get_మాట(గుర్తులేని adr, std::string&విలువ) override;
      శూన్య చొప్పించు(గుర్తులేని idx, స్థిరరాశి std::string&విలువ, గుర్తులేని max_కొలత) override;
      శూన్య నెట్టు_వెనుక(స్థిరరాశి std::string&విలువ, గుర్తులేని max_కొలత) override;
      శూన్య నెట్టు_front(స్థిరరాశి std::string&విలువ, గుర్తులేని max_కొలత) override;
      శూన్య తీయి_వెనుక(శూన్య) override { వరుస.తీయి_వెనుక(); };
      శూన్య తీయి_front(శూన్య) override { వరుస.తీయి_front(); };
      శూన్య erase(గుర్తులేని idx) override;
      శూన్య erase_తోక(గుర్తులేని idx) override;
      శూన్య తిరగేయి_elems(శూన్య) override;
      శూన్య క్రమీకరించు_elems(bool ascending) override;
      శూన్య shuffle_elems(శూన్య) override;

    అంతరంగ:
      std::deque<std::string> వరుస;
};

class vvp_వరుస_vec4 : బహిరంగ vvp_వరుస {

    బహిరంగ:
      size_t get_కొలత(శూన్య) స్థిరరాశి override { తిరిగి వరుస.కొలత(); };
      శూన్య copy_elems(vvp_object_t src, గుర్తులేని max_కొలత) override;
      శూన్య సమితి_మాట_max(గుర్తులేని adr, స్థిరరాశి vvp_vector4_t&విలువ, గుర్తులేని max_కొలత) override;
      శూన్య సమితి_మాట(గుర్తులేని adr, స్థిరరాశి vvp_vector4_t&విలువ) override;
      శూన్య get_మాట(గుర్తులేని adr, vvp_vector4_t&విలువ) override;
      శూన్య చొప్పించు(గుర్తులేని idx, స్థిరరాశి vvp_vector4_t&విలువ, గుర్తులేని max_కొలత) override;
      శూన్య నెట్టు_వెనుక(స్థిరరాశి vvp_vector4_t&విలువ, గుర్తులేని max_కొలత) override;
      శూన్య నెట్టు_front(స్థిరరాశి vvp_vector4_t&విలువ, గుర్తులేని max_కొలత) override;
      శూన్య తీయి_వెనుక(శూన్య) override { వరుస.తీయి_వెనుక(); };
      శూన్య తీయి_front(శూన్య) override { వరుస.తీయి_front(); };
      శూన్య erase(గుర్తులేని idx) override;
      శూన్య erase_తోక(గుర్తులేని idx) override;
      శూన్య తిరగేయి_elems(శూన్య) override;
      శూన్య క్రమీకరించు_elems(bool ascending) override;
      శూన్య shuffle_elems(శూన్య) override;

    అంతరంగ:
      std::deque<vvp_vector4_t> వరుస;
};

బాహ్య std::string get_fileline();

#అయితే_ముగింపు /* IVL_vvp_darray_H */
