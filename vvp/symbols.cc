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
بنية مفتاح_strings {
      بنية مفتاح_strings*next;
      حرف data[64*1024 - حجم(بنية مفتاح_strings*)];
};

حرف*رمز_table_s::مفتاح_strdup(ثابت حرف*str)
{
      unsigned len = strlen(str);
      أكد( (len+1) <= حجم str_chunk->data );

      إذا ( (len+1) > (حجم str_chunk->data - str_used) ) {
	    مفتاح_strings*tmp = جديد مفتاح_strings;
	    tmp->next = str_chunk;
	    str_chunk = tmp;
	    str_used = 0;
      }

      حرف*res = str_chunk->data + str_used;
      str_used += len + 1;
      strcpy(res, str);
      إرجاع res;
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

ثابت unsigned leaf_width = 254;
ثابت unsigned عقدة_width = 508;

بنية شجرة_data {
      حرف*مفتاح;
      رمز_قيمة_t val;
};

بنية عقدة_الشجرة {
      منطقي leaf_flag;
      unsigned تعداد_2;
      بنية عقدة_الشجرة*parent;

      اتحاد {
	    بنية شجرة_data leaf[leaf_width];
	    بنية عقدة_الشجرة *child[عقدة_width];
      };

};

ساكن مضمن حرف* عقدة_last_مفتاح(بنية عقدة_الشجرة*عقدة)
{
      بينما (عقدة->leaf_flag == خطأ)
	    عقدة = عقدة->child[عقدة->تعداد_2-1];

      إرجاع عقدة->leaf[عقدة->تعداد_2-1].مفتاح;
}

/*
 * Allocate a new symbol table means creating the table structure and
 * a root node, and initializing the pointers and members of the root
 * node.
 */
رمز_table_s::رمز_table_s()
{
      جذر = جديد بنية عقدة_الشجرة;
      جذر->leaf_flag = خطأ;
      جذر->تعداد_2 = 0;
      جذر->parent = 0;

      str_chunk = جديد مفتاح_strings;
      str_chunk->next = 0;
      str_used = 0;
}

ساكن فراغ delete_رمز_عقدة(بنية عقدة_الشجرة*cur)
{
      إذا (! cur->leaf_flag) {
	    لكل (unsigned idx = 0 ;  idx < cur->تعداد_2 ;  idx += 1)
		  delete_رمز_عقدة(cur->child[idx]);
      }

      delete cur;
}

/* Do as split_leaf_ does, but for nodes. */
ساكن فراغ split_عقدة(بنية عقدة_الشجرة*cur)
{
      أكد(!cur->leaf_flag);
      إذا (cur->parent) أكد(! cur->parent->leaf_flag);

      بينما (cur->تعداد_2 == عقدة_width) {
	      /* Create a new node to hold half the data from cur. */
	    بنية عقدة_الشجرة *new_عقدة = جديد بنية عقدة_الشجرة;
	    new_عقدة->leaf_flag = خطأ;
	    new_عقدة->تعداد_2 = cur->تعداد_2 / 2;
	      /* cur is not root; new_node becomes sibling. */
	    إذا (cur->parent) new_عقدة->parent = cur->parent;

	      /* Move the last half of the data from the end of the old node
	       * to the beginning of the new node. At the same time, reduce
	       * the size of the old node. */
	    unsigned idx1 = new_عقدة->تعداد_2;
	    unsigned idx2 = cur->تعداد_2;
	    بينما (idx1 > 0) {
		  idx1 -= 1;
		  idx2 -= 1;
		  new_عقدة->child[idx1] = cur->child[idx2];
		  new_عقدة->child[idx1]->parent = new_عقدة;
		  cur->تعداد_2 -= 1;
	    }

	    أكد(new_عقدة->تعداد_2 > 0);
	    أكد(cur->تعداد_2 > 0);

	    إذا (cur->parent == 0) {
		    /* cur is root. Move first half of children to another
		     * new node, and put the two new nodes in cur. The plan
		     * here is to make cur into the new root and split its
		     * contents into 2 children. */

		  new_عقدة->parent = cur;
		  بنية عقدة_الشجرة*new2_عقدة = جديد بنية عقدة_الشجرة;
		  new2_عقدة->leaf_flag = خطأ;
		  new2_عقدة->تعداد_2 = cur->تعداد_2;
		  new2_عقدة->parent = cur;
		  لكل (unsigned idx = 0; idx < cur->تعداد_2; idx += 1) {
			new2_عقدة->child[idx] = cur->child[idx];
			new2_عقدة->child[idx]->parent = new2_عقدة;
		  }
		  cur->child[0] = new2_عقدة;
		  cur->child[1] = new_عقدة;
		  cur->تعداد_2 = 2;
		    /* no more ancestors, stop the while loop */
		  اكسر;
	    }

	      /* cur is not root. hook new_node to cur->parent. */
	    unsigned idx = 0;
	    بينما (cur->parent->child[idx] != cur) {
		  أكد(idx < cur->parent->تعداد_2);
		  idx += 1;
	    }
	    idx += 1;

	    لكل (unsigned tmp = cur->parent->تعداد_2 ;  tmp > idx ;  tmp -= 1) {
		  cur->parent->child[tmp] = cur->parent->child[tmp-1];
	    }

	    cur->parent->child[idx] = new_عقدة;
	    cur->parent->تعداد_2 += 1;

	      /* check the ancestor */
	    cur = cur->parent;
      }
}

/*
 * This function takes a leaf node and splits in into two. Move half
 * the leaf keys into the new node, and add the new leaf into the
 * parent node.
 */
ساكن بنية عقدة_الشجرة* split_leaf_(بنية عقدة_الشجرة*cur)
{
      أكد(cur->leaf_flag);
      أكد(cur->parent);
      أكد(! cur->parent->leaf_flag);

	/* Create a new leaf to hold half the data from the old leaf. */
      بنية عقدة_الشجرة*new_leaf = جديد بنية عقدة_الشجرة;
      new_leaf->leaf_flag = true;
      new_leaf->تعداد_2 = cur->تعداد_2 / 2;
      new_leaf->parent = cur->parent;

	/* Move the last half of the data from the end of the old leaf
	   to the beginning of the new leaf. At the same time, reduce
	   the size of the old leaf. */
      unsigned idx1 = new_leaf->تعداد_2;
      unsigned idx2 = cur->تعداد_2;
      بينما (idx1 > 0) {
	    idx1 -= 1;
	    idx2 -= 1;
	    new_leaf->leaf[idx1] = cur->leaf[idx2];
	    cur->تعداد_2 -= 1;
      }

      أكد(new_leaf->تعداد_2 > 0);
      أكد(cur->تعداد_2 > 0);

      unsigned idx = 0;
      بينما (cur->parent->child[idx] != cur) {
	    أكد(idx < cur->parent->تعداد_2);
	    idx += 1;
      }

      idx += 1;

      لكل (unsigned tmp = cur->parent->تعداد_2 ;  tmp > idx ;  tmp -= 1)
	    cur->parent->child[tmp] = cur->parent->child[tmp-1];

      cur->parent->child[idx] = new_leaf;
      cur->parent->تعداد_2 += 1;

      إذا (cur->parent->تعداد_2 == عقدة_width)
	    split_عقدة(cur->parent);

      إرجاع new_leaf;
}


/*
 * This function searches tree recursively for the key. If the value
 * is not found (and we are at a leaf) then set the key with the given
 * value. If the key is found, set the value only if the force_flag is
 * true.
 */

رمز_قيمة_t رمز_table_s::find_قيمة(بنية عقدة_الشجرة*cur,
					   ثابت حرف*مفتاح, رمز_قيمة_t val,
					   منطقي force_flag)
{
      إذا (cur->leaf_flag) {

	    unsigned idx = 0;
	    لكل (;;) {
		    /* If we run out of keys in the leaf, then add
		       this at the end of the leaf. */
		  إذا (idx == cur->تعداد_2) {
			cur->leaf[idx].مفتاح = مفتاح_strdup(مفتاح);
			cur->leaf[idx].val = val;
			cur->تعداد_2 += 1;
			إذا (cur->تعداد_2 == leaf_width)
			      split_leaf_(cur);

			إرجاع val;
		  }

		  صحيح rc = strcmp(مفتاح, cur->leaf[idx].مفتاح);

		    /* If we found the key already in the table, then
		       set the new value and break. */
		  إذا (rc == 0) {
			إذا (force_flag)
			      cur->leaf[idx].val = val;
			إرجاع cur->leaf[idx].val;
		  }

		    /* If this key goes before the current key, then
		       push all the following entries back and add
		       this key here. */
		  إذا (rc < 0) {
			لكل (unsigned tmp = cur->تعداد_2; tmp > idx; tmp -= 1)
			      cur->leaf[tmp] = cur->leaf[tmp-1];

			cur->leaf[idx].مفتاح = مفتاح_strdup(مفتاح);
			cur->leaf[idx].val = val;
			cur->تعداد_2 += 1;
			إذا (cur->تعداد_2 == leaf_width)
			      split_leaf_(cur);

			إرجاع val;
		  }

		  idx += 1;
	    }

      } وإلا {
	      /* Do a binary search within the inner node. */
	    unsigned min = 0;
	    unsigned max = cur->تعداد_2;
	    unsigned idx = max/2;
	    لكل (;;) {
		  صحيح rc = strcmp(مفتاح, عقدة_last_مفتاح(cur->child[idx]));
		  إذا (rc == 0) {
			إرجاع find_قيمة(cur->child[idx],
					   مفتاح, val, force_flag);
		  }

		  إذا (rc > 0) {
			min = idx + 1;
			إذا (min == cur->تعداد_2)
			      إرجاع find_قيمة(cur->child[idx],
						 مفتاح, val, force_flag);
			إذا (min == max)
			      إرجاع find_قيمة(cur->child[max],
						 مفتاح, val, force_flag);

			idx = min + (max-min)/2;

		  } وإلا  {
			max = idx;
			إذا (idx == min)
			      إرجاع find_قيمة(cur->child[idx],
						 مفتاح, val, force_flag);
			idx = min + (max-min)/2;
		  }
	    }
      }

      أكد(0);
      { رمز_قيمة_t tmp;
        tmp.ptr = 0;
        إرجاع tmp;
      }
}

فراغ رمز_table_s::sym_set_قيمة(ثابت حرف*مفتاح, رمز_قيمة_t val)
{
      إذا (جذر->تعداد_2 == 0) {
	      /* Handle the special case that this is the very first
		 value in the symbol table. Create the first leaf node
		 and initialize the pointers. */
	    بنية عقدة_الشجرة*cur = جديد بنية عقدة_الشجرة;
	    cur->leaf_flag = true;
	    cur->parent = جذر;
	    cur->تعداد_2 = 1;
	    cur->leaf[0].مفتاح = مفتاح_strdup(مفتاح);
	    cur->leaf[0].val = val;

	    جذر->تعداد_2 = 1;
	    جذر->child[0] = cur;
      } وإلا {
	    find_قيمة(جذر, مفتاح, val, true);
      }
}

رمز_قيمة_t رمز_table_s::sym_get_قيمة(ثابت حرف*مفتاح)
{
      رمز_قيمة_t def;
      def.ptr = 0;

      إذا (جذر->تعداد_2 == 0) {
	      /* Handle the special case that this is the very first
		 value in the symbol table. Create the first leaf node
		 and initialize the pointers. */
	    بنية عقدة_الشجرة*cur = جديد بنية عقدة_الشجرة;
	    cur->leaf_flag = true;
	    cur->parent = جذر;
	    cur->تعداد_2 = 1;
	    cur->leaf[0].مفتاح = مفتاح_strdup(مفتاح);
	    cur->leaf[0].val = def;

	    جذر->تعداد_2 = 1;
	    جذر->child[0] = cur;
	    إرجاع cur->leaf[0].val;
      } وإلا {
	    إرجاع find_قيمة(جذر, مفتاح, def, خطأ);
      }
}

رمز_table_s::~رمز_table_s()
{
      delete_رمز_عقدة(جذر);
      بينما (str_chunk) {
	    مفتاح_strings*tmp = str_chunk;
	    str_chunk = tmp->next;
	    delete tmp;
      }
}
