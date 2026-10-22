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
संरचना साँचो_strings {
      संरचना साँचो_strings*next;
      वर्ण data[64*1024 - परिमाण(संरचना साँचो_strings*)];
};

वर्ण*चिह्न_table_s::साँचो_strdup(अचर वर्ण*str)
{
      अचिन्हित len = म_माप(str);
      प्रतिज्ञा( (len+1) <= परिमाण str_chunk->data );

      यदि ( (len+1) > (परिमाण str_chunk->data - str_used) ) {
	    साँचो_strings*tmp = नूतन साँचो_strings;
	    tmp->next = str_chunk;
	    str_chunk = tmp;
	    str_used = 0;
      }

      वर्ण*res = str_chunk->data + str_used;
      str_used += len + 1;
      म_नकल(res, str);
      फर्काऊ res;
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

अचर अचिन्हित leaf_width = 254;
अचर अचिन्हित गाँठो_width = 508;

संरचना रूख_data {
      वर्ण*साँचो;
      चिह्न_मान_t val;
};

संरचना रूख_गाँठो {
      तार्किक leaf_flag;
      अचिन्हित गन्ती;
      संरचना रूख_गाँठो*parent;

      सङ्घ {
	    संरचना रूख_data leaf[leaf_width];
	    संरचना रूख_गाँठो *child[गाँठो_width];
      };

};

स्थिर अन्तःस्थ वर्ण* गाँठो_last_साँचो(संरचना रूख_गाँठो*गाँठो)
{
      जबसम्म (गाँठो->leaf_flag == असत्य)
	    गाँठो = गाँठो->child[गाँठो->गन्ती-1];

      फर्काऊ गाँठो->leaf[गाँठो->गन्ती-1].साँचो;
}

/*
 * Allocate a new symbol table means creating the table structure and
 * a root node, and initializing the pointers and members of the root
 * node.
 */
चिह्न_table_s::चिह्न_table_s()
{
      जरा = नूतन संरचना रूख_गाँठो;
      जरा->leaf_flag = असत्य;
      जरा->गन्ती = 0;
      जरा->parent = 0;

      str_chunk = नूतन साँचो_strings;
      str_chunk->next = 0;
      str_used = 0;
}

स्थिर शून्य delete_चिह्न_गाँठो(संरचना रूख_गाँठो*cur)
{
      यदि (! cur->leaf_flag) {
	    लागि (अचिन्हित idx = 0 ;  idx < cur->गन्ती ;  idx += 1)
		  delete_चिह्न_गाँठो(cur->child[idx]);
      }

      delete cur;
}

/* Do as split_leaf_ does, but for nodes. */
स्थिर शून्य split_गाँठो(संरचना रूख_गाँठो*cur)
{
      प्रतिज्ञा(!cur->leaf_flag);
      यदि (cur->parent) प्रतिज्ञा(! cur->parent->leaf_flag);

      जबसम्म (cur->गन्ती == गाँठो_width) {
	      /* Create a new node to hold half the data from cur. */
	    संरचना रूख_गाँठो *new_गाँठो = नूतन संरचना रूख_गाँठो;
	    new_गाँठो->leaf_flag = असत्य;
	    new_गाँठो->गन्ती = cur->गन्ती / 2;
	      /* cur is not root; new_node becomes sibling. */
	    यदि (cur->parent) new_गाँठो->parent = cur->parent;

	      /* Move the last half of the data from the end of the old node
	       * to the beginning of the new node. At the same time, reduce
	       * the size of the old node. */
	    अचिन्हित idx1 = new_गाँठो->गन्ती;
	    अचिन्हित idx2 = cur->गन्ती;
	    जबसम्म (idx1 > 0) {
		  idx1 -= 1;
		  idx2 -= 1;
		  new_गाँठो->child[idx1] = cur->child[idx2];
		  new_गाँठो->child[idx1]->parent = new_गाँठो;
		  cur->गन्ती -= 1;
	    }

	    प्रतिज्ञा(new_गाँठो->गन्ती > 0);
	    प्रतिज्ञा(cur->गन्ती > 0);

	    यदि (cur->parent == 0) {
		    /* cur is root. Move first half of children to another
		     * new node, and put the two new nodes in cur. The plan
		     * here is to make cur into the new root and split its
		     * contents into 2 children. */

		  new_गाँठो->parent = cur;
		  संरचना रूख_गाँठो*new2_गाँठो = नूतन संरचना रूख_गाँठो;
		  new2_गाँठो->leaf_flag = असत्य;
		  new2_गाँठो->गन्ती = cur->गन्ती;
		  new2_गाँठो->parent = cur;
		  लागि (अचिन्हित idx = 0; idx < cur->गन्ती; idx += 1) {
			new2_गाँठो->child[idx] = cur->child[idx];
			new2_गाँठो->child[idx]->parent = new2_गाँठो;
		  }
		  cur->child[0] = new2_गाँठो;
		  cur->child[1] = new_गाँठो;
		  cur->गन्ती = 2;
		    /* no more ancestors, stop the while loop */
		  रोक;
	    }

	      /* cur is not root. hook new_node to cur->parent. */
	    अचिन्हित idx = 0;
	    जबसम्म (cur->parent->child[idx] != cur) {
		  प्रतिज्ञा(idx < cur->parent->गन्ती);
		  idx += 1;
	    }
	    idx += 1;

	    लागि (अचिन्हित tmp = cur->parent->गन्ती ;  tmp > idx ;  tmp -= 1) {
		  cur->parent->child[tmp] = cur->parent->child[tmp-1];
	    }

	    cur->parent->child[idx] = new_गाँठो;
	    cur->parent->गन्ती += 1;

	      /* check the ancestor */
	    cur = cur->parent;
      }
}

/*
 * This function takes a leaf node and splits in into two. Move half
 * the leaf keys into the new node, and add the new leaf into the
 * parent node.
 */
स्थिर संरचना रूख_गाँठो* split_leaf_(संरचना रूख_गाँठो*cur)
{
      प्रतिज्ञा(cur->leaf_flag);
      प्रतिज्ञा(cur->parent);
      प्रतिज्ञा(! cur->parent->leaf_flag);

	/* Create a new leaf to hold half the data from the old leaf. */
      संरचना रूख_गाँठो*new_leaf = नूतन संरचना रूख_गाँठो;
      new_leaf->leaf_flag = सत्य;
      new_leaf->गन्ती = cur->गन्ती / 2;
      new_leaf->parent = cur->parent;

	/* Move the last half of the data from the end of the old leaf
	   to the beginning of the new leaf. At the same time, reduce
	   the size of the old leaf. */
      अचिन्हित idx1 = new_leaf->गन्ती;
      अचिन्हित idx2 = cur->गन्ती;
      जबसम्म (idx1 > 0) {
	    idx1 -= 1;
	    idx2 -= 1;
	    new_leaf->leaf[idx1] = cur->leaf[idx2];
	    cur->गन्ती -= 1;
      }

      प्रतिज्ञा(new_leaf->गन्ती > 0);
      प्रतिज्ञा(cur->गन्ती > 0);

      अचिन्हित idx = 0;
      जबसम्म (cur->parent->child[idx] != cur) {
	    प्रतिज्ञा(idx < cur->parent->गन्ती);
	    idx += 1;
      }

      idx += 1;

      लागि (अचिन्हित tmp = cur->parent->गन्ती ;  tmp > idx ;  tmp -= 1)
	    cur->parent->child[tmp] = cur->parent->child[tmp-1];

      cur->parent->child[idx] = new_leaf;
      cur->parent->गन्ती += 1;

      यदि (cur->parent->गन्ती == गाँठो_width)
	    split_गाँठो(cur->parent);

      फर्काऊ new_leaf;
}


/*
 * This function searches tree recursively for the key. If the value
 * is not found (and we are at a leaf) then set the key with the given
 * value. If the key is found, set the value only if the force_flag is
 * true.
 */

चिह्न_मान_t चिह्न_table_s::find_मान(संरचना रूख_गाँठो*cur,
					   अचर वर्ण*साँचो, चिह्न_मान_t val,
					   तार्किक force_flag)
{
      यदि (cur->leaf_flag) {

	    अचिन्हित idx = 0;
	    लागि (;;) {
		    /* If we run out of keys in the leaf, then add
		       this at the end of the leaf. */
		  यदि (idx == cur->गन्ती) {
			cur->leaf[idx].साँचो = साँचो_strdup(साँचो);
			cur->leaf[idx].val = val;
			cur->गन्ती += 1;
			यदि (cur->गन्ती == leaf_width)
			      split_leaf_(cur);

			फर्काऊ val;
		  }

		  पूर्णाङ्क rc = म_भेद(साँचो, cur->leaf[idx].साँचो);

		    /* If we found the key already in the table, then
		       set the new value and break. */
		  यदि (rc == 0) {
			यदि (force_flag)
			      cur->leaf[idx].val = val;
			फर्काऊ cur->leaf[idx].val;
		  }

		    /* If this key goes before the current key, then
		       push all the following entries back and add
		       this key here. */
		  यदि (rc < 0) {
			लागि (अचिन्हित tmp = cur->गन्ती; tmp > idx; tmp -= 1)
			      cur->leaf[tmp] = cur->leaf[tmp-1];

			cur->leaf[idx].साँचो = साँचो_strdup(साँचो);
			cur->leaf[idx].val = val;
			cur->गन्ती += 1;
			यदि (cur->गन्ती == leaf_width)
			      split_leaf_(cur);

			फर्काऊ val;
		  }

		  idx += 1;
	    }

      } अन्यथा {
	      /* Do a binary search within the inner node. */
	    अचिन्हित min = 0;
	    अचिन्हित max = cur->गन्ती;
	    अचिन्हित idx = max/2;
	    लागि (;;) {
		  पूर्णाङ्क rc = म_भेद(साँचो, गाँठो_last_साँचो(cur->child[idx]));
		  यदि (rc == 0) {
			फर्काऊ find_मान(cur->child[idx],
					   साँचो, val, force_flag);
		  }

		  यदि (rc > 0) {
			min = idx + 1;
			यदि (min == cur->गन्ती)
			      फर्काऊ find_मान(cur->child[idx],
						 साँचो, val, force_flag);
			यदि (min == max)
			      फर्काऊ find_मान(cur->child[max],
						 साँचो, val, force_flag);

			idx = min + (max-min)/2;

		  } अन्यथा  {
			max = idx;
			यदि (idx == min)
			      फर्काऊ find_मान(cur->child[idx],
						 साँचो, val, force_flag);
			idx = min + (max-min)/2;
		  }
	    }
      }

      प्रतिज्ञा(0);
      { चिह्न_मान_t tmp;
        tmp.ptr = 0;
        फर्काऊ tmp;
      }
}

शून्य चिह्न_table_s::sym_set_मान(अचर वर्ण*साँचो, चिह्न_मान_t val)
{
      यदि (जरा->गन्ती == 0) {
	      /* Handle the special case that this is the very first
		 value in the symbol table. Create the first leaf node
		 and initialize the pointers. */
	    संरचना रूख_गाँठो*cur = नूतन संरचना रूख_गाँठो;
	    cur->leaf_flag = सत्य;
	    cur->parent = जरा;
	    cur->गन्ती = 1;
	    cur->leaf[0].साँचो = साँचो_strdup(साँचो);
	    cur->leaf[0].val = val;

	    जरा->गन्ती = 1;
	    जरा->child[0] = cur;
      } अन्यथा {
	    find_मान(जरा, साँचो, val, सत्य);
      }
}

चिह्न_मान_t चिह्न_table_s::sym_get_मान(अचर वर्ण*साँचो)
{
      चिह्न_मान_t def;
      def.ptr = 0;

      यदि (जरा->गन्ती == 0) {
	      /* Handle the special case that this is the very first
		 value in the symbol table. Create the first leaf node
		 and initialize the pointers. */
	    संरचना रूख_गाँठो*cur = नूतन संरचना रूख_गाँठो;
	    cur->leaf_flag = सत्य;
	    cur->parent = जरा;
	    cur->गन्ती = 1;
	    cur->leaf[0].साँचो = साँचो_strdup(साँचो);
	    cur->leaf[0].val = def;

	    जरा->गन्ती = 1;
	    जरा->child[0] = cur;
	    फर्काऊ cur->leaf[0].val;
      } अन्यथा {
	    फर्काऊ find_मान(जरा, साँचो, def, असत्य);
      }
}

चिह्न_table_s::~चिह्न_table_s()
{
      delete_चिह्न_गाँठो(जरा);
      जबसम्म (str_chunk) {
	    साँचो_strings*tmp = str_chunk;
	    str_chunk = tmp->next;
	    delete tmp;
      }
}
