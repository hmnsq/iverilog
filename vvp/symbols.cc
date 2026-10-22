/*
 * Copyright (c) 2001-2013 Stephen Williams (steve@icarus.com)
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

# include  "symbols.h"
# include  <cstring>
# include  <cstdlib>
# include  <cassert>

/*
 * The keys of the symbol table are null terminated strings. Keep them
 * in a string buffer, with the strings separated by a single null,
 * for compact use of memory. This also makes it easy to delete the
 * entire lot of keys, simply by deleting the heaps.
 *
 * The key_strdup() function below allocates the strings from this
 * buffer, possibly making a new buffer if needed.
 */
بنية کلی_مزي_ګانې {
      بنية کلی_مزي_ګانې*بل_2;
      حرف معلومات[64*1024 - حجم(بنية کلی_مزي_ګانې*)];
};

حرف*نښه_جدول_s::کلی_strdup(ثابت حرف*str)
{
      بینښه len = مزي_اوږدوالی(str);
      أكد( (len+1) <= حجم str_chunk->معلومات );

      إذا ( (len+1) > (حجم str_chunk->معلومات - str_کارول_شوی) ) {
	    کلی_مزي_ګانې*tmp = جديد کلی_مزي_ګانې;
	    tmp->بل_2 = str_chunk;
	    str_chunk = tmp;
	    str_کارول_شوی = 0;
      }

      حرف*res = str_chunk->معلومات + str_کارول_شوی;
      str_کارول_شوی += len + 1;
      strcpy(res, str);
      أرجع res;
}

/*
 * This is a B-Tree data structure, where there are nodes and
 * leaves.
 *
 * Nodes have a bunch of pointers to children. Each child pointer has
 * associated with it a key that is the largest key referenced by that
 * child. So, if the key being searched for has a value <= the first
 * key of a node, then the value is in the first child.
 *
 * leaves have a sorted table of key-value pairs. The search can use a
 * simple binary search to find an item. Each key represents an item.
 */

ثابت بینښه ونې_پاڼه_پلنوالی = 254;
ثابت بینښه غوټه_پلنوالی = 508;

بنية ونه_معلومات {
      حرف*کلی;
      نښه_ارزښت_t val;
};

بنية ونې_غوټه {
      منطقي ونې_پاڼه_flag;
      بینښه شمېر;
      بنية ونې_غوټه*پلار;

      اتحاد {
	    بنية ونه_معلومات ونې_پاڼه[ونې_پاڼه_پلنوالی];
	    بنية ونې_غوټه *ماشوم[غوټه_پلنوالی];
      };

};

ساكن مضمن حرف* غوټه_وروستی_کلی(بنية ونې_غوټه*غوټه)
{
      طالما (غوټه->ونې_پاڼه_flag == خطأ)
	    غوټه = غوټه->ماشوم[غوټه->شمېر-1];

      أرجع غوټه->ونې_پاڼه[غوټه->شمېر-1].کلی;
}

/*
 * Allocate a new symbol table means creating the table structure and
 * a root node, and initializing the pointers and members of the root
 * node.
 */
نښه_جدول_s::نښه_جدول_s()
{
      ریښه = جديد بنية ونې_غوټه;
      ریښه->ونې_پاڼه_flag = خطأ;
      ریښه->شمېر = 0;
      ریښه->پلار = 0;

      str_chunk = جديد کلی_مزي_ګانې;
      str_chunk->بل_2 = 0;
      str_کارول_شوی = 0;
}

ساكن فراغ delete_نښه_غوټه(بنية ونې_غوټه*cur)
{
      إذا (! cur->ونې_پاڼه_flag) {
	    لكل (بینښه idx = 0 ;  idx < cur->شمېر ;  idx += 1)
		  delete_نښه_غوټه(cur->ماشوم[idx]);
      }

      ړنګول cur;
}

/* Do as split_leaf_ does, but for nodes. */
ساكن فراغ وېشل_غوټه(بنية ونې_غوټه*cur)
{
      أكد(!cur->ونې_پاڼه_flag);
      إذا (cur->پلار) أكد(! cur->پلار->ونې_پاڼه_flag);

      طالما (cur->شمېر == غوټه_پلنوالی) {
	      /* Create a new node to hold half the data from cur. */
	    بنية ونې_غوټه *new_غوټه = جديد بنية ونې_غوټه;
	    new_غوټه->ونې_پاڼه_flag = خطأ;
	    new_غوټه->شمېر = cur->شمېر / 2;
	      /* cur is not root; new_node becomes sibling. */
	    إذا (cur->پلار) new_غوټه->پلار = cur->پلار;

	      /* Move the last half of the data from the end of the old node
	       * to the beginning of the new node. At the same time, reduce
	       * the size of the old node. */
	    بینښه idx1 = new_غوټه->شمېر;
	    بینښه idx2 = cur->شمېر;
	    طالما (idx1 > 0) {
		  idx1 -= 1;
		  idx2 -= 1;
		  new_غوټه->ماشوم[idx1] = cur->ماشوم[idx2];
		  new_غوټه->ماشوم[idx1]->پلار = new_غوټه;
		  cur->شمېر -= 1;
	    }

	    أكد(new_غوټه->شمېر > 0);
	    أكد(cur->شمېر > 0);

	    إذا (cur->پلار == 0) {
		    /* cur is root. Move first half of children to another
		     * new node, and put the two new nodes in cur. The plan
		     * here is to make cur into the new root and split its
		     * contents into 2 children. */

		  new_غوټه->پلار = cur;
		  بنية ونې_غوټه*new2_غوټه = جديد بنية ونې_غوټه;
		  new2_غوټه->ونې_پاڼه_flag = خطأ;
		  new2_غوټه->شمېر = cur->شمېر;
		  new2_غوټه->پلار = cur;
		  لكل (بینښه idx = 0; idx < cur->شمېر; idx += 1) {
			new2_غوټه->ماشوم[idx] = cur->ماشوم[idx];
			new2_غوټه->ماشوم[idx]->پلار = new2_غوټه;
		  }
		  cur->ماشوم[0] = new2_غوټه;
		  cur->ماشوم[1] = new_غوټه;
		  cur->شمېر = 2;
		    /* no more ancestors, stop the while loop */
		  اكسر;
	    }

	      /* cur is not root. hook new_node to cur->parent. */
	    بینښه idx = 0;
	    طالما (cur->پلار->ماشوم[idx] != cur) {
		  أكد(idx < cur->پلار->شمېر);
		  idx += 1;
	    }
	    idx += 1;

	    لكل (بینښه tmp = cur->پلار->شمېر ;  tmp > idx ;  tmp -= 1) {
		  cur->پلار->ماشوم[tmp] = cur->پلار->ماشوم[tmp-1];
	    }

	    cur->پلار->ماشوم[idx] = new_غوټه;
	    cur->پلار->شمېر += 1;

	      /* check the ancestor */
	    cur = cur->پلار;
      }
}

/*
 * This function takes a leaf node and splits in into two. Move half
 * the leaf keys into the new node, and add the new leaf into the
 * parent node.
 */
ساكن بنية ونې_غوټه* وېشل_ونې_پاڼه(بنية ونې_غوټه*cur)
{
      أكد(cur->ونې_پاڼه_flag);
      أكد(cur->پلار);
      أكد(! cur->پلار->ونې_پاڼه_flag);

	/* Create a new leaf to hold half the data from the old leaf. */
      بنية ونې_غوټه*new_ونې_پاڼه = جديد بنية ونې_غوټه;
      new_ونې_پاڼه->ونې_پاڼه_flag = ریښتیا;
      new_ونې_پاڼه->شمېر = cur->شمېر / 2;
      new_ونې_پاڼه->پلار = cur->پلار;

	/* Move the last half of the data from the end of the old leaf
	   to the beginning of the new leaf. At the same time, reduce
	   the size of the old leaf. */
      بینښه idx1 = new_ونې_پاڼه->شمېر;
      بینښه idx2 = cur->شمېر;
      طالما (idx1 > 0) {
	    idx1 -= 1;
	    idx2 -= 1;
	    new_ونې_پاڼه->ونې_پاڼه[idx1] = cur->ونې_پاڼه[idx2];
	    cur->شمېر -= 1;
      }

      أكد(new_ونې_پاڼه->شمېر > 0);
      أكد(cur->شمېر > 0);

      بینښه idx = 0;
      طالما (cur->پلار->ماشوم[idx] != cur) {
	    أكد(idx < cur->پلار->شمېر);
	    idx += 1;
      }

      idx += 1;

      لكل (بینښه tmp = cur->پلار->شمېر ;  tmp > idx ;  tmp -= 1)
	    cur->پلار->ماشوم[tmp] = cur->پلار->ماشوم[tmp-1];

      cur->پلار->ماشوم[idx] = new_ونې_پاڼه;
      cur->پلار->شمېر += 1;

      إذا (cur->پلار->شمېر == غوټه_پلنوالی)
	    وېشل_غوټه(cur->پلار);

      أرجع new_ونې_پاڼه;
}


/*
 * This function searches tree recursively for the key. If the value
 * is not found (and we are at a leaf) then set the key with the given
 * value. If the key is found, set the value only if the force_flag is
 * true.
 */

نښه_ارزښت_t نښه_جدول_s::پیدا_کړه_ارزښت(بنية ونې_غوټه*cur,
					   ثابت حرف*کلی, نښه_ارزښت_t val,
					   منطقي force_flag)
{
      إذا (cur->ونې_پاڼه_flag) {

	    بینښه idx = 0;
	    لكل (;;) {
		    /* If we run out of keys in the leaf, then add
		       this at the end of the leaf. */
		  إذا (idx == cur->شمېر) {
			cur->ونې_پاڼه[idx].کلی = کلی_strdup(کلی);
			cur->ونې_پاڼه[idx].val = val;
			cur->شمېر += 1;
			إذا (cur->شمېر == ونې_پاڼه_پلنوالی)
			      وېشل_ونې_پاڼه(cur);

			أرجع val;
		  }

		  صحيح rc = strcmp(کلی, cur->ونې_پاڼه[idx].کلی);

		    /* If we found the key already in the table, then
		       set the new value and break. */
		  إذا (rc == 0) {
			إذا (force_flag)
			      cur->ونې_پاڼه[idx].val = val;
			أرجع cur->ونې_پاڼه[idx].val;
		  }

		    /* If this key goes before the current key, then
		       push all the following entries back and add
		       this key here. */
		  إذا (rc < 0) {
			لكل (بینښه tmp = cur->شمېر; tmp > idx; tmp -= 1)
			      cur->ونې_پاڼه[tmp] = cur->ونې_پاڼه[tmp-1];

			cur->ونې_پاڼه[idx].کلی = کلی_strdup(کلی);
			cur->ونې_پاڼه[idx].val = val;
			cur->شمېر += 1;
			إذا (cur->شمېر == ونې_پاڼه_پلنوالی)
			      وېشل_ونې_پاڼه(cur);

			أرجع val;
		  }

		  idx += 1;
	    }

      } وإلا {
	      /* Do a binary search within the inner node. */
	    بینښه min = 0;
	    بینښه max = cur->شمېر;
	    بینښه idx = max/2;
	    لكل (;;) {
		  صحيح rc = strcmp(کلی, غوټه_وروستی_کلی(cur->ماشوم[idx]));
		  إذا (rc == 0) {
			أرجع پیدا_کړه_ارزښت(cur->ماشوم[idx],
					   کلی, val, force_flag);
		  }

		  إذا (rc > 0) {
			min = idx + 1;
			إذا (min == cur->شمېر)
			      أرجع پیدا_کړه_ارزښت(cur->ماشوم[idx],
						 کلی, val, force_flag);
			إذا (min == max)
			      أرجع پیدا_کړه_ارزښت(cur->ماشوم[max],
						 کلی, val, force_flag);

			idx = min + (max-min)/2;

		  } وإلا  {
			max = idx;
			إذا (idx == min)
			      أرجع پیدا_کړه_ارزښت(cur->ماشوم[idx],
						 کلی, val, force_flag);
			idx = min + (max-min)/2;
		  }
	    }
      }

      أكد(0);
      { نښه_ارزښت_t tmp;
        tmp.ptr = 0;
        أرجع tmp;
      }
}

فراغ نښه_جدول_s::sym_سېټ_ارزښت(ثابت حرف*کلی, نښه_ارزښت_t val)
{
      إذا (ریښه->شمېر == 0) {
	      /* Handle the special case that this is the very first
		 value in the symbol table. Create the first leaf node
		 and initialize the pointers. */
	    بنية ونې_غوټه*cur = جديد بنية ونې_غوټه;
	    cur->ونې_پاڼه_flag = ریښتیا;
	    cur->پلار = ریښه;
	    cur->شمېر = 1;
	    cur->ونې_پاڼه[0].کلی = کلی_strdup(کلی);
	    cur->ونې_پاڼه[0].val = val;

	    ریښه->شمېر = 1;
	    ریښه->ماشوم[0] = cur;
      } وإلا {
	    پیدا_کړه_ارزښت(ریښه, کلی, val, ریښتیا);
      }
}

نښه_ارزښت_t نښه_جدول_s::sym_get_ارزښت(ثابت حرف*کلی)
{
      نښه_ارزښت_t def;
      def.ptr = 0;

      إذا (ریښه->شمېر == 0) {
	      /* Handle the special case that this is the very first
		 value in the symbol table. Create the first leaf node
		 and initialize the pointers. */
	    بنية ونې_غوټه*cur = جديد بنية ونې_غوټه;
	    cur->ونې_پاڼه_flag = ریښتیا;
	    cur->پلار = ریښه;
	    cur->شمېر = 1;
	    cur->ونې_پاڼه[0].کلی = کلی_strdup(کلی);
	    cur->ونې_پاڼه[0].val = def;

	    ریښه->شمېر = 1;
	    ریښه->ماشوم[0] = cur;
	    أرجع cur->ونې_پاڼه[0].val;
      } وإلا {
	    أرجع پیدا_کړه_ارزښت(ریښه, کلی, def, خطأ);
      }
}

نښه_جدول_s::~نښه_جدول_s()
{
      delete_نښه_غوټه(ریښه);
      طالما (str_chunk) {
	    کلی_مزي_ګانې*tmp = str_chunk;
	    str_chunk = tmp->بل_2;
	    ړنګول tmp;
      }
}
