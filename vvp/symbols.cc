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
ساخت کلید_لڑیاں {
      ساخت کلید_لڑیاں*اگلا_2;
      حرف ڈیٹا[64*1024 - حجم(ساخت کلید_لڑیاں*)];
};

حرف*علامت_جدول_s::کلید_strdup(ثابت حرف*str)
{
      غیرعلامتی len = لڑی_لمبائی(str);
      أكد( (len+1) <= حجم str_chunk->ڈیٹا );

      اگر ( (len+1) > (حجم str_chunk->ڈیٹا - str_مستعمل) ) {
	    کلید_لڑیاں*tmp = نیا کلید_لڑیاں;
	    tmp->اگلا_2 = str_chunk;
	    str_chunk = tmp;
	    str_مستعمل = 0;
      }

      حرف*res = str_chunk->ڈیٹا + str_مستعمل;
      str_مستعمل += len + 1;
      strcpy(res, str);
      واپس res;
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

ثابت غیرعلامتی برگ_چوڑائی = 254;
ثابت غیرعلامتی گرہ_چوڑائی = 508;

ساخت درخت_ڈیٹا {
      حرف*کلید;
      علامت_قدر_t val;
};

ساخت درخت_گرہ {
      منطقی برگ_flag;
      غیرعلامتی گنتی;
      ساخت درخت_گرہ*والد;

      اتحاد {
	    ساخت درخت_ڈیٹا برگ[برگ_چوڑائی];
	    ساخت درخت_گرہ *بچہ[گرہ_چوڑائی];
      };

};

ساکن اندرونی حرف* گرہ_آخری_کلید(ساخت درخت_گرہ*گرہ)
{
      جب (گرہ->برگ_flag == جھوٹ)
	    گرہ = گرہ->بچہ[گرہ->گنتی-1];

      واپس گرہ->برگ[گرہ->گنتی-1].کلید;
}

/*
 * Allocate a new symbol table means creating the table structure and
 * a root node, and initializing the pointers and members of the root
 * node.
 */
علامت_جدول_s::علامت_جدول_s()
{
      جڑ = نیا ساخت درخت_گرہ;
      جڑ->برگ_flag = جھوٹ;
      جڑ->گنتی = 0;
      جڑ->والد = 0;

      str_chunk = نیا کلید_لڑیاں;
      str_chunk->اگلا_2 = 0;
      str_مستعمل = 0;
}

ساکن خالی delete_علامت_گرہ(ساخت درخت_گرہ*cur)
{
      اگر (! cur->برگ_flag) {
	    برائے (غیرعلامتی idx = 0 ;  idx < cur->گنتی ;  idx += 1)
		  delete_علامت_گرہ(cur->بچہ[idx]);
      }

      حذف cur;
}

/* Do as split_leaf_ does, but for nodes. */
ساکن خالی بانٹو_گرہ(ساخت درخت_گرہ*cur)
{
      أكد(!cur->برگ_flag);
      اگر (cur->والد) أكد(! cur->والد->برگ_flag);

      جب (cur->گنتی == گرہ_چوڑائی) {
	      /* Create a new node to hold half the data from cur. */
	    ساخت درخت_گرہ *new_گرہ = نیا ساخت درخت_گرہ;
	    new_گرہ->برگ_flag = جھوٹ;
	    new_گرہ->گنتی = cur->گنتی / 2;
	      /* cur is not root; new_node becomes sibling. */
	    اگر (cur->والد) new_گرہ->والد = cur->والد;

	      /* Move the last half of the data from the end of the old node
	       * to the beginning of the new node. At the same time, reduce
	       * the size of the old node. */
	    غیرعلامتی idx1 = new_گرہ->گنتی;
	    غیرعلامتی idx2 = cur->گنتی;
	    جب (idx1 > 0) {
		  idx1 -= 1;
		  idx2 -= 1;
		  new_گرہ->بچہ[idx1] = cur->بچہ[idx2];
		  new_گرہ->بچہ[idx1]->والد = new_گرہ;
		  cur->گنتی -= 1;
	    }

	    أكد(new_گرہ->گنتی > 0);
	    أكد(cur->گنتی > 0);

	    اگر (cur->والد == 0) {
		    /* cur is root. Move first half of children to another
		     * new node, and put the two new nodes in cur. The plan
		     * here is to make cur into the new root and split its
		     * contents into 2 children. */

		  new_گرہ->والد = cur;
		  ساخت درخت_گرہ*new2_گرہ = نیا ساخت درخت_گرہ;
		  new2_گرہ->برگ_flag = جھوٹ;
		  new2_گرہ->گنتی = cur->گنتی;
		  new2_گرہ->والد = cur;
		  برائے (غیرعلامتی idx = 0; idx < cur->گنتی; idx += 1) {
			new2_گرہ->بچہ[idx] = cur->بچہ[idx];
			new2_گرہ->بچہ[idx]->والد = new2_گرہ;
		  }
		  cur->بچہ[0] = new2_گرہ;
		  cur->بچہ[1] = new_گرہ;
		  cur->گنتی = 2;
		    /* no more ancestors, stop the while loop */
		  توڑو;
	    }

	      /* cur is not root. hook new_node to cur->parent. */
	    غیرعلامتی idx = 0;
	    جب (cur->والد->بچہ[idx] != cur) {
		  أكد(idx < cur->والد->گنتی);
		  idx += 1;
	    }
	    idx += 1;

	    برائے (غیرعلامتی tmp = cur->والد->گنتی ;  tmp > idx ;  tmp -= 1) {
		  cur->والد->بچہ[tmp] = cur->والد->بچہ[tmp-1];
	    }

	    cur->والد->بچہ[idx] = new_گرہ;
	    cur->والد->گنتی += 1;

	      /* check the ancestor */
	    cur = cur->والد;
      }
}

/*
 * This function takes a leaf node and splits in into two. Move half
 * the leaf keys into the new node, and add the new leaf into the
 * parent node.
 */
ساکن ساخت درخت_گرہ* بانٹو_برگ(ساخت درخت_گرہ*cur)
{
      أكد(cur->برگ_flag);
      أكد(cur->والد);
      أكد(! cur->والد->برگ_flag);

	/* Create a new leaf to hold half the data from the old leaf. */
      ساخت درخت_گرہ*new_برگ = نیا ساخت درخت_گرہ;
      new_برگ->برگ_flag = سچ;
      new_برگ->گنتی = cur->گنتی / 2;
      new_برگ->والد = cur->والد;

	/* Move the last half of the data from the end of the old leaf
	   to the beginning of the new leaf. At the same time, reduce
	   the size of the old leaf. */
      غیرعلامتی idx1 = new_برگ->گنتی;
      غیرعلامتی idx2 = cur->گنتی;
      جب (idx1 > 0) {
	    idx1 -= 1;
	    idx2 -= 1;
	    new_برگ->برگ[idx1] = cur->برگ[idx2];
	    cur->گنتی -= 1;
      }

      أكد(new_برگ->گنتی > 0);
      أكد(cur->گنتی > 0);

      غیرعلامتی idx = 0;
      جب (cur->والد->بچہ[idx] != cur) {
	    أكد(idx < cur->والد->گنتی);
	    idx += 1;
      }

      idx += 1;

      برائے (غیرعلامتی tmp = cur->والد->گنتی ;  tmp > idx ;  tmp -= 1)
	    cur->والد->بچہ[tmp] = cur->والد->بچہ[tmp-1];

      cur->والد->بچہ[idx] = new_برگ;
      cur->والد->گنتی += 1;

      اگر (cur->والد->گنتی == گرہ_چوڑائی)
	    بانٹو_گرہ(cur->والد);

      واپس new_برگ;
}


/*
 * This function searches tree recursively for the key. If the value
 * is not found (and we are at a leaf) then set the key with the given
 * value. If the key is found, set the value only if the force_flag is
 * true.
 */

علامت_قدر_t علامت_جدول_s::ڈھونڈو_قدر(ساخت درخت_گرہ*cur,
					   ثابت حرف*کلید, علامت_قدر_t val,
					   منطقی force_flag)
{
      اگر (cur->برگ_flag) {

	    غیرعلامتی idx = 0;
	    برائے (;;) {
		    /* If we run out of keys in the leaf, then add
		       this at the end of the leaf. */
		  اگر (idx == cur->گنتی) {
			cur->برگ[idx].کلید = کلید_strdup(کلید);
			cur->برگ[idx].val = val;
			cur->گنتی += 1;
			اگر (cur->گنتی == برگ_چوڑائی)
			      بانٹو_برگ(cur);

			واپس val;
		  }

		  صحیح rc = strcmp(کلید, cur->برگ[idx].کلید);

		    /* If we found the key already in the table, then
		       set the new value and break. */
		  اگر (rc == 0) {
			اگر (force_flag)
			      cur->برگ[idx].val = val;
			واپس cur->برگ[idx].val;
		  }

		    /* If this key goes before the current key, then
		       push all the following entries back and add
		       this key here. */
		  اگر (rc < 0) {
			برائے (غیرعلامتی tmp = cur->گنتی; tmp > idx; tmp -= 1)
			      cur->برگ[tmp] = cur->برگ[tmp-1];

			cur->برگ[idx].کلید = کلید_strdup(کلید);
			cur->برگ[idx].val = val;
			cur->گنتی += 1;
			اگر (cur->گنتی == برگ_چوڑائی)
			      بانٹو_برگ(cur);

			واپس val;
		  }

		  idx += 1;
	    }

      } ورنہ {
	      /* Do a binary search within the inner node. */
	    غیرعلامتی min = 0;
	    غیرعلامتی max = cur->گنتی;
	    غیرعلامتی idx = max/2;
	    برائے (;;) {
		  صحیح rc = strcmp(کلید, گرہ_آخری_کلید(cur->بچہ[idx]));
		  اگر (rc == 0) {
			واپس ڈھونڈو_قدر(cur->بچہ[idx],
					   کلید, val, force_flag);
		  }

		  اگر (rc > 0) {
			min = idx + 1;
			اگر (min == cur->گنتی)
			      واپس ڈھونڈو_قدر(cur->بچہ[idx],
						 کلید, val, force_flag);
			اگر (min == max)
			      واپس ڈھونڈو_قدر(cur->بچہ[max],
						 کلید, val, force_flag);

			idx = min + (max-min)/2;

		  } ورنہ  {
			max = idx;
			اگر (idx == min)
			      واپس ڈھونڈو_قدر(cur->بچہ[idx],
						 کلید, val, force_flag);
			idx = min + (max-min)/2;
		  }
	    }
      }

      أكد(0);
      { علامت_قدر_t tmp;
        tmp.ptr = 0;
        واپس tmp;
      }
}

خالی علامت_جدول_s::sym_سیٹ_قدر(ثابت حرف*کلید, علامت_قدر_t val)
{
      اگر (جڑ->گنتی == 0) {
	      /* Handle the special case that this is the very first
		 value in the symbol table. Create the first leaf node
		 and initialize the pointers. */
	    ساخت درخت_گرہ*cur = نیا ساخت درخت_گرہ;
	    cur->برگ_flag = سچ;
	    cur->والد = جڑ;
	    cur->گنتی = 1;
	    cur->برگ[0].کلید = کلید_strdup(کلید);
	    cur->برگ[0].val = val;

	    جڑ->گنتی = 1;
	    جڑ->بچہ[0] = cur;
      } ورنہ {
	    ڈھونڈو_قدر(جڑ, کلید, val, سچ);
      }
}

علامت_قدر_t علامت_جدول_s::sym_get_قدر(ثابت حرف*کلید)
{
      علامت_قدر_t def;
      def.ptr = 0;

      اگر (جڑ->گنتی == 0) {
	      /* Handle the special case that this is the very first
		 value in the symbol table. Create the first leaf node
		 and initialize the pointers. */
	    ساخت درخت_گرہ*cur = نیا ساخت درخت_گرہ;
	    cur->برگ_flag = سچ;
	    cur->والد = جڑ;
	    cur->گنتی = 1;
	    cur->برگ[0].کلید = کلید_strdup(کلید);
	    cur->برگ[0].val = def;

	    جڑ->گنتی = 1;
	    جڑ->بچہ[0] = cur;
	    واپس cur->برگ[0].val;
      } ورنہ {
	    واپس ڈھونڈو_قدر(جڑ, کلید, def, جھوٹ);
      }
}

علامت_جدول_s::~علامت_جدول_s()
{
      delete_علامت_گرہ(جڑ);
      جب (str_chunk) {
	    کلید_لڑیاں*tmp = str_chunk;
	    str_chunk = tmp->اگلا_2;
	    حذف tmp;
      }
}
