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
بنية کلید_لڑیاں {
      بنية کلید_لڑیاں*اگلا_2;
      حرف انکڑے[64*1024 - حجم(بنية کلید_لڑیاں*)];
};

حرف*علامت_جدول_s::کلید_strdup(ثابت حرف*str)
{
      بنانشانی len = لڑی_لمبائی(str);
      أكد( (len+1) <= حجم str_chunk->انکڑے );

      إذا ( (len+1) > (حجم str_chunk->انکڑے - str_ورتیا) ) {
	    کلید_لڑیاں*tmp = جديد کلید_لڑیاں;
	    tmp->اگلا_2 = str_chunk;
	    str_chunk = tmp;
	    str_ورتیا = 0;
      }

      حرف*res = str_chunk->انکڑے + str_ورتیا;
      str_ورتیا += len + 1;
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

ثابت بنانشانی پتی_چوڑائی = 254;
ثابت بنانشانی گرہ_چوڑائی = 508;

بنية درخت_انکڑے {
      حرف*کلید;
      علامت_قدر_t val;
};

بنية درخت_گرہ {
      منطقي پتی_flag;
      بنانشانی گنتی;
      بنية درخت_گرہ*ماپا;

      اتحاد {
	    بنية درخت_انکڑے پتی[پتی_چوڑائی];
	    بنية درخت_گرہ *بچہ[گرہ_چوڑائی];
      };

};

ساكن مضمن حرف* گرہ_آخری_کلید(بنية درخت_گرہ*گرہ)
{
      طالما (گرہ->پتی_flag == خطأ)
	    گرہ = گرہ->بچہ[گرہ->گنتی-1];

      أرجع گرہ->پتی[گرہ->گنتی-1].کلید;
}

/*
 * Allocate a new symbol table means creating the table structure and
 * a root node, and initializing the pointers and members of the root
 * node.
 */
علامت_جدول_s::علامت_جدول_s()
{
      جڑ = جديد بنية درخت_گرہ;
      جڑ->پتی_flag = خطأ;
      جڑ->گنتی = 0;
      جڑ->ماپا = 0;

      str_chunk = جديد کلید_لڑیاں;
      str_chunk->اگلا_2 = 0;
      str_ورتیا = 0;
}

ساكن فراغ delete_علامت_گرہ(بنية درخت_گرہ*cur)
{
      إذا (! cur->پتی_flag) {
	    لكل (بنانشانی idx = 0 ;  idx < cur->گنتی ;  idx += 1)
		  delete_علامت_گرہ(cur->بچہ[idx]);
      }

      مٹاؤ cur;
}

/* Do as split_leaf_ does, but for nodes. */
ساكن فراغ ونڈو_گرہ(بنية درخت_گرہ*cur)
{
      أكد(!cur->پتی_flag);
      إذا (cur->ماپا) أكد(! cur->ماپا->پتی_flag);

      طالما (cur->گنتی == گرہ_چوڑائی) {
	      /* Create a new node to hold half the data from cur. */
	    بنية درخت_گرہ *new_گرہ = جديد بنية درخت_گرہ;
	    new_گرہ->پتی_flag = خطأ;
	    new_گرہ->گنتی = cur->گنتی / 2;
	      /* cur is not root; new_node becomes sibling. */
	    إذا (cur->ماپا) new_گرہ->ماپا = cur->ماپا;

	      /* Move the last half of the data from the end of the old node
	       * to the beginning of the new node. At the same time, reduce
	       * the size of the old node. */
	    بنانشانی idx1 = new_گرہ->گنتی;
	    بنانشانی idx2 = cur->گنتی;
	    طالما (idx1 > 0) {
		  idx1 -= 1;
		  idx2 -= 1;
		  new_گرہ->بچہ[idx1] = cur->بچہ[idx2];
		  new_گرہ->بچہ[idx1]->ماپا = new_گرہ;
		  cur->گنتی -= 1;
	    }

	    أكد(new_گرہ->گنتی > 0);
	    أكد(cur->گنتی > 0);

	    إذا (cur->ماپا == 0) {
		    /* cur is root. Move first half of children to another
		     * new node, and put the two new nodes in cur. The plan
		     * here is to make cur into the new root and split its
		     * contents into 2 children. */

		  new_گرہ->ماپا = cur;
		  بنية درخت_گرہ*new2_گرہ = جديد بنية درخت_گرہ;
		  new2_گرہ->پتی_flag = خطأ;
		  new2_گرہ->گنتی = cur->گنتی;
		  new2_گرہ->ماپا = cur;
		  لكل (بنانشانی idx = 0; idx < cur->گنتی; idx += 1) {
			new2_گرہ->بچہ[idx] = cur->بچہ[idx];
			new2_گرہ->بچہ[idx]->ماپا = new2_گرہ;
		  }
		  cur->بچہ[0] = new2_گرہ;
		  cur->بچہ[1] = new_گرہ;
		  cur->گنتی = 2;
		    /* no more ancestors, stop the while loop */
		  اكسر;
	    }

	      /* cur is not root. hook new_node to cur->parent. */
	    بنانشانی idx = 0;
	    طالما (cur->ماپا->بچہ[idx] != cur) {
		  أكد(idx < cur->ماپا->گنتی);
		  idx += 1;
	    }
	    idx += 1;

	    لكل (بنانشانی tmp = cur->ماپا->گنتی ;  tmp > idx ;  tmp -= 1) {
		  cur->ماپا->بچہ[tmp] = cur->ماپا->بچہ[tmp-1];
	    }

	    cur->ماپا->بچہ[idx] = new_گرہ;
	    cur->ماپا->گنتی += 1;

	      /* check the ancestor */
	    cur = cur->ماپا;
      }
}

/*
 * This function takes a leaf node and splits in into two. Move half
 * the leaf keys into the new node, and add the new leaf into the
 * parent node.
 */
ساكن بنية درخت_گرہ* ونڈو_پتی(بنية درخت_گرہ*cur)
{
      أكد(cur->پتی_flag);
      أكد(cur->ماپا);
      أكد(! cur->ماپا->پتی_flag);

	/* Create a new leaf to hold half the data from the old leaf. */
      بنية درخت_گرہ*new_پتی = جديد بنية درخت_گرہ;
      new_پتی->پتی_flag = سچ;
      new_پتی->گنتی = cur->گنتی / 2;
      new_پتی->ماپا = cur->ماپا;

	/* Move the last half of the data from the end of the old leaf
	   to the beginning of the new leaf. At the same time, reduce
	   the size of the old leaf. */
      بنانشانی idx1 = new_پتی->گنتی;
      بنانشانی idx2 = cur->گنتی;
      طالما (idx1 > 0) {
	    idx1 -= 1;
	    idx2 -= 1;
	    new_پتی->پتی[idx1] = cur->پتی[idx2];
	    cur->گنتی -= 1;
      }

      أكد(new_پتی->گنتی > 0);
      أكد(cur->گنتی > 0);

      بنانشانی idx = 0;
      طالما (cur->ماپا->بچہ[idx] != cur) {
	    أكد(idx < cur->ماپا->گنتی);
	    idx += 1;
      }

      idx += 1;

      لكل (بنانشانی tmp = cur->ماپا->گنتی ;  tmp > idx ;  tmp -= 1)
	    cur->ماپا->بچہ[tmp] = cur->ماپا->بچہ[tmp-1];

      cur->ماپا->بچہ[idx] = new_پتی;
      cur->ماپا->گنتی += 1;

      إذا (cur->ماپا->گنتی == گرہ_چوڑائی)
	    ونڈو_گرہ(cur->ماپا);

      أرجع new_پتی;
}


/*
 * This function searches tree recursively for the key. If the value
 * is not found (and we are at a leaf) then set the key with the given
 * value. If the key is found, set the value only if the force_flag is
 * true.
 */

علامت_قدر_t علامت_جدول_s::لبھو_قدر(بنية درخت_گرہ*cur,
					   ثابت حرف*کلید, علامت_قدر_t val,
					   منطقي force_flag)
{
      إذا (cur->پتی_flag) {

	    بنانشانی idx = 0;
	    لكل (;;) {
		    /* If we run out of keys in the leaf, then add
		       this at the end of the leaf. */
		  إذا (idx == cur->گنتی) {
			cur->پتی[idx].کلید = کلید_strdup(کلید);
			cur->پتی[idx].val = val;
			cur->گنتی += 1;
			إذا (cur->گنتی == پتی_چوڑائی)
			      ونڈو_پتی(cur);

			أرجع val;
		  }

		  صحيح rc = strcmp(کلید, cur->پتی[idx].کلید);

		    /* If we found the key already in the table, then
		       set the new value and break. */
		  إذا (rc == 0) {
			إذا (force_flag)
			      cur->پتی[idx].val = val;
			أرجع cur->پتی[idx].val;
		  }

		    /* If this key goes before the current key, then
		       push all the following entries back and add
		       this key here. */
		  إذا (rc < 0) {
			لكل (بنانشانی tmp = cur->گنتی; tmp > idx; tmp -= 1)
			      cur->پتی[tmp] = cur->پتی[tmp-1];

			cur->پتی[idx].کلید = کلید_strdup(کلید);
			cur->پتی[idx].val = val;
			cur->گنتی += 1;
			إذا (cur->گنتی == پتی_چوڑائی)
			      ونڈو_پتی(cur);

			أرجع val;
		  }

		  idx += 1;
	    }

      } وإلا {
	      /* Do a binary search within the inner node. */
	    بنانشانی min = 0;
	    بنانشانی max = cur->گنتی;
	    بنانشانی idx = max/2;
	    لكل (;;) {
		  صحيح rc = strcmp(کلید, گرہ_آخری_کلید(cur->بچہ[idx]));
		  إذا (rc == 0) {
			أرجع لبھو_قدر(cur->بچہ[idx],
					   کلید, val, force_flag);
		  }

		  إذا (rc > 0) {
			min = idx + 1;
			إذا (min == cur->گنتی)
			      أرجع لبھو_قدر(cur->بچہ[idx],
						 کلید, val, force_flag);
			إذا (min == max)
			      أرجع لبھو_قدر(cur->بچہ[max],
						 کلید, val, force_flag);

			idx = min + (max-min)/2;

		  } وإلا  {
			max = idx;
			إذا (idx == min)
			      أرجع لبھو_قدر(cur->بچہ[idx],
						 کلید, val, force_flag);
			idx = min + (max-min)/2;
		  }
	    }
      }

      أكد(0);
      { علامت_قدر_t tmp;
        tmp.ptr = 0;
        أرجع tmp;
      }
}

فراغ علامت_جدول_s::sym_سیٹ_قدر(ثابت حرف*کلید, علامت_قدر_t val)
{
      إذا (جڑ->گنتی == 0) {
	      /* Handle the special case that this is the very first
		 value in the symbol table. Create the first leaf node
		 and initialize the pointers. */
	    بنية درخت_گرہ*cur = جديد بنية درخت_گرہ;
	    cur->پتی_flag = سچ;
	    cur->ماپا = جڑ;
	    cur->گنتی = 1;
	    cur->پتی[0].کلید = کلید_strdup(کلید);
	    cur->پتی[0].val = val;

	    جڑ->گنتی = 1;
	    جڑ->بچہ[0] = cur;
      } وإلا {
	    لبھو_قدر(جڑ, کلید, val, سچ);
      }
}

علامت_قدر_t علامت_جدول_s::sym_get_قدر(ثابت حرف*کلید)
{
      علامت_قدر_t def;
      def.ptr = 0;

      إذا (جڑ->گنتی == 0) {
	      /* Handle the special case that this is the very first
		 value in the symbol table. Create the first leaf node
		 and initialize the pointers. */
	    بنية درخت_گرہ*cur = جديد بنية درخت_گرہ;
	    cur->پتی_flag = سچ;
	    cur->ماپا = جڑ;
	    cur->گنتی = 1;
	    cur->پتی[0].کلید = کلید_strdup(کلید);
	    cur->پتی[0].val = def;

	    جڑ->گنتی = 1;
	    جڑ->بچہ[0] = cur;
	    أرجع cur->پتی[0].val;
      } وإلا {
	    أرجع لبھو_قدر(جڑ, کلید, def, خطأ);
      }
}

علامت_جدول_s::~علامت_جدول_s()
{
      delete_علامت_گرہ(جڑ);
      طالما (str_chunk) {
	    کلید_لڑیاں*tmp = str_chunk;
	    str_chunk = tmp->اگلا_2;
	    مٹاؤ tmp;
      }
}
