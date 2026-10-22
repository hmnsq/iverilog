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
بنية کلید_رشتهها {
      بنية کلید_رشتهها*بعدی_2;
      حرف داده[64*1024 - حجم(بنية کلید_رشتهها*)];
};

حرف*نماد_جدول_s::کلید_strdup(ثابت حرف*str)
{
      بیعلامت len = طول_رشته(str);
      أكد( (len+1) <= حجم str_chunk->داده );

      إذا ( (len+1) > (حجم str_chunk->داده - str_استفاده_شده) ) {
	    کلید_رشتهها*tmp = جديد کلید_رشتهها;
	    tmp->بعدی_2 = str_chunk;
	    str_chunk = tmp;
	    str_استفاده_شده = 0;
      }

      حرف*res = str_chunk->داده + str_استفاده_شده;
      str_استفاده_شده += len + 1;
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

ثابت بیعلامت برگ_عرض = 254;
ثابت بیعلامت گره_عرض = 508;

بنية درخت_داده {
      حرف*کلید;
      نماد_مقدار_t val;
};

بنية گره_درخت {
      منطقي برگ_flag;
      بیعلامت شمار;
      بنية گره_درخت*والد;

      اتحاد {
	    بنية درخت_داده برگ[برگ_عرض];
	    بنية گره_درخت *فرزند[گره_عرض];
      };

};

ساكن مضمن حرف* گره_آخرین_کلید(بنية گره_درخت*گره)
{
      طالما (گره->برگ_flag == خطأ)
	    گره = گره->فرزند[گره->شمار-1];

      أرجع گره->برگ[گره->شمار-1].کلید;
}

/*
 * Allocate a new symbol table means creating the table structure and
 * a root node, and initializing the pointers and members of the root
 * node.
 */
نماد_جدول_s::نماد_جدول_s()
{
      ریشه = جديد بنية گره_درخت;
      ریشه->برگ_flag = خطأ;
      ریشه->شمار = 0;
      ریشه->والد = 0;

      str_chunk = جديد کلید_رشتهها;
      str_chunk->بعدی_2 = 0;
      str_استفاده_شده = 0;
}

ساكن فراغ delete_نماد_گره(بنية گره_درخت*cur)
{
      إذا (! cur->برگ_flag) {
	    لكل (بیعلامت idx = 0 ;  idx < cur->شمار ;  idx += 1)
		  delete_نماد_گره(cur->فرزند[idx]);
      }

      حذف cur;
}

/* Do as split_leaf_ does, but for nodes. */
ساكن فراغ تقسیم_گره(بنية گره_درخت*cur)
{
      أكد(!cur->برگ_flag);
      إذا (cur->والد) أكد(! cur->والد->برگ_flag);

      طالما (cur->شمار == گره_عرض) {
	      /* Create a new node to hold half the data from cur. */
	    بنية گره_درخت *new_گره = جديد بنية گره_درخت;
	    new_گره->برگ_flag = خطأ;
	    new_گره->شمار = cur->شمار / 2;
	      /* cur is not root; new_node becomes sibling. */
	    إذا (cur->والد) new_گره->والد = cur->والد;

	      /* Move the last half of the data from the end of the old node
	       * to the beginning of the new node. At the same time, reduce
	       * the size of the old node. */
	    بیعلامت idx1 = new_گره->شمار;
	    بیعلامت idx2 = cur->شمار;
	    طالما (idx1 > 0) {
		  idx1 -= 1;
		  idx2 -= 1;
		  new_گره->فرزند[idx1] = cur->فرزند[idx2];
		  new_گره->فرزند[idx1]->والد = new_گره;
		  cur->شمار -= 1;
	    }

	    أكد(new_گره->شمار > 0);
	    أكد(cur->شمار > 0);

	    إذا (cur->والد == 0) {
		    /* cur is root. Move first half of children to another
		     * new node, and put the two new nodes in cur. The plan
		     * here is to make cur into the new root and split its
		     * contents into 2 children. */

		  new_گره->والد = cur;
		  بنية گره_درخت*new2_گره = جديد بنية گره_درخت;
		  new2_گره->برگ_flag = خطأ;
		  new2_گره->شمار = cur->شمار;
		  new2_گره->والد = cur;
		  لكل (بیعلامت idx = 0; idx < cur->شمار; idx += 1) {
			new2_گره->فرزند[idx] = cur->فرزند[idx];
			new2_گره->فرزند[idx]->والد = new2_گره;
		  }
		  cur->فرزند[0] = new2_گره;
		  cur->فرزند[1] = new_گره;
		  cur->شمار = 2;
		    /* no more ancestors, stop the while loop */
		  اكسر;
	    }

	      /* cur is not root. hook new_node to cur->parent. */
	    بیعلامت idx = 0;
	    طالما (cur->والد->فرزند[idx] != cur) {
		  أكد(idx < cur->والد->شمار);
		  idx += 1;
	    }
	    idx += 1;

	    لكل (بیعلامت tmp = cur->والد->شمار ;  tmp > idx ;  tmp -= 1) {
		  cur->والد->فرزند[tmp] = cur->والد->فرزند[tmp-1];
	    }

	    cur->والد->فرزند[idx] = new_گره;
	    cur->والد->شمار += 1;

	      /* check the ancestor */
	    cur = cur->والد;
      }
}

/*
 * This function takes a leaf node and splits in into two. Move half
 * the leaf keys into the new node, and add the new leaf into the
 * parent node.
 */
ساكن بنية گره_درخت* تقسیم_برگ(بنية گره_درخت*cur)
{
      أكد(cur->برگ_flag);
      أكد(cur->والد);
      أكد(! cur->والد->برگ_flag);

	/* Create a new leaf to hold half the data from the old leaf. */
      بنية گره_درخت*new_برگ = جديد بنية گره_درخت;
      new_برگ->برگ_flag = درست;
      new_برگ->شمار = cur->شمار / 2;
      new_برگ->والد = cur->والد;

	/* Move the last half of the data from the end of the old leaf
	   to the beginning of the new leaf. At the same time, reduce
	   the size of the old leaf. */
      بیعلامت idx1 = new_برگ->شمار;
      بیعلامت idx2 = cur->شمار;
      طالما (idx1 > 0) {
	    idx1 -= 1;
	    idx2 -= 1;
	    new_برگ->برگ[idx1] = cur->برگ[idx2];
	    cur->شمار -= 1;
      }

      أكد(new_برگ->شمار > 0);
      أكد(cur->شمار > 0);

      بیعلامت idx = 0;
      طالما (cur->والد->فرزند[idx] != cur) {
	    أكد(idx < cur->والد->شمار);
	    idx += 1;
      }

      idx += 1;

      لكل (بیعلامت tmp = cur->والد->شمار ;  tmp > idx ;  tmp -= 1)
	    cur->والد->فرزند[tmp] = cur->والد->فرزند[tmp-1];

      cur->والد->فرزند[idx] = new_برگ;
      cur->والد->شمار += 1;

      إذا (cur->والد->شمار == گره_عرض)
	    تقسیم_گره(cur->والد);

      أرجع new_برگ;
}


/*
 * This function searches tree recursively for the key. If the value
 * is not found (and we are at a leaf) then set the key with the given
 * value. If the key is found, set the value only if the force_flag is
 * true.
 */

نماد_مقدار_t نماد_جدول_s::پیدا_کن_مقدار(بنية گره_درخت*cur,
					   ثابت حرف*کلید, نماد_مقدار_t val,
					   منطقي force_flag)
{
      إذا (cur->برگ_flag) {

	    بیعلامت idx = 0;
	    لكل (;;) {
		    /* If we run out of keys in the leaf, then add
		       this at the end of the leaf. */
		  إذا (idx == cur->شمار) {
			cur->برگ[idx].کلید = کلید_strdup(کلید);
			cur->برگ[idx].val = val;
			cur->شمار += 1;
			إذا (cur->شمار == برگ_عرض)
			      تقسیم_برگ(cur);

			أرجع val;
		  }

		  صحيح rc = strcmp(کلید, cur->برگ[idx].کلید);

		    /* If we found the key already in the table, then
		       set the new value and break. */
		  إذا (rc == 0) {
			إذا (force_flag)
			      cur->برگ[idx].val = val;
			أرجع cur->برگ[idx].val;
		  }

		    /* If this key goes before the current key, then
		       push all the following entries back and add
		       this key here. */
		  إذا (rc < 0) {
			لكل (بیعلامت tmp = cur->شمار; tmp > idx; tmp -= 1)
			      cur->برگ[tmp] = cur->برگ[tmp-1];

			cur->برگ[idx].کلید = کلید_strdup(کلید);
			cur->برگ[idx].val = val;
			cur->شمار += 1;
			إذا (cur->شمار == برگ_عرض)
			      تقسیم_برگ(cur);

			أرجع val;
		  }

		  idx += 1;
	    }

      } وإلا {
	      /* Do a binary search within the inner node. */
	    بیعلامت min = 0;
	    بیعلامت max = cur->شمار;
	    بیعلامت idx = max/2;
	    لكل (;;) {
		  صحيح rc = strcmp(کلید, گره_آخرین_کلید(cur->فرزند[idx]));
		  إذا (rc == 0) {
			أرجع پیدا_کن_مقدار(cur->فرزند[idx],
					   کلید, val, force_flag);
		  }

		  إذا (rc > 0) {
			min = idx + 1;
			إذا (min == cur->شمار)
			      أرجع پیدا_کن_مقدار(cur->فرزند[idx],
						 کلید, val, force_flag);
			إذا (min == max)
			      أرجع پیدا_کن_مقدار(cur->فرزند[max],
						 کلید, val, force_flag);

			idx = min + (max-min)/2;

		  } وإلا  {
			max = idx;
			إذا (idx == min)
			      أرجع پیدا_کن_مقدار(cur->فرزند[idx],
						 کلید, val, force_flag);
			idx = min + (max-min)/2;
		  }
	    }
      }

      أكد(0);
      { نماد_مقدار_t tmp;
        tmp.ptr = 0;
        أرجع tmp;
      }
}

فراغ نماد_جدول_s::sym_مجموعه_مقدار(ثابت حرف*کلید, نماد_مقدار_t val)
{
      إذا (ریشه->شمار == 0) {
	      /* Handle the special case that this is the very first
		 value in the symbol table. Create the first leaf node
		 and initialize the pointers. */
	    بنية گره_درخت*cur = جديد بنية گره_درخت;
	    cur->برگ_flag = درست;
	    cur->والد = ریشه;
	    cur->شمار = 1;
	    cur->برگ[0].کلید = کلید_strdup(کلید);
	    cur->برگ[0].val = val;

	    ریشه->شمار = 1;
	    ریشه->فرزند[0] = cur;
      } وإلا {
	    پیدا_کن_مقدار(ریشه, کلید, val, درست);
      }
}

نماد_مقدار_t نماد_جدول_s::sym_get_مقدار(ثابت حرف*کلید)
{
      نماد_مقدار_t def;
      def.ptr = 0;

      إذا (ریشه->شمار == 0) {
	      /* Handle the special case that this is the very first
		 value in the symbol table. Create the first leaf node
		 and initialize the pointers. */
	    بنية گره_درخت*cur = جديد بنية گره_درخت;
	    cur->برگ_flag = درست;
	    cur->والد = ریشه;
	    cur->شمار = 1;
	    cur->برگ[0].کلید = کلید_strdup(کلید);
	    cur->برگ[0].val = def;

	    ریشه->شمار = 1;
	    ریشه->فرزند[0] = cur;
	    أرجع cur->برگ[0].val;
      } وإلا {
	    أرجع پیدا_کن_مقدار(ریشه, کلید, def, خطأ);
      }
}

نماد_جدول_s::~نماد_جدول_s()
{
      delete_نماد_گره(ریشه);
      طالما (str_chunk) {
	    کلید_رشتهها*tmp = str_chunk;
	    str_chunk = tmp->بعدی_2;
	    حذف tmp;
      }
}
