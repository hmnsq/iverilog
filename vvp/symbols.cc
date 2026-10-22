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
संरचना किल्ली_strings {
      संरचना किल्ली_strings*next;
      वर्ण data[64*1024 - परिमाण(संरचना किल्ली_strings*)];
};

वर्ण*चिन्ह_table_s::किल्ली_strdup(ध्रुव वर्ण*str)
{
      unsigned len = strlen(str);
      प्रतिज्ञा( (len+1) <= परिमाण str_chunk->data );

      यदि ( (len+1) > (परिमाण str_chunk->data - str_used) ) {
	    किल्ली_strings*tmp = नूतन किल्ली_strings;
	    tmp->next = str_chunk;
	    str_chunk = tmp;
	    str_used = 0;
      }

      वर्ण*res = str_chunk->data + str_used;
      str_used += len + 1;
      strcpy(res, str);
      प्रत्यागम res;
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

ध्रुव unsigned leaf_width = 254;
ध्रुव unsigned गाठ_width = 508;

संरचना वृक्ष_data {
      वर्ण*किल्ली;
      चिन्ह_मूल्य_t val;
};

संरचना वृक्ष_गाठ {
      तार्किक leaf_flag;
      unsigned मोजणी;
      संरचना वृक्ष_गाठ*parent;

      सङ्घ {
	    संरचना वृक्ष_data leaf[leaf_width];
	    संरचना वृक्ष_गाठ *child[गाठ_width];
      };

};

स्थिर अन्तःस्थ वर्ण* गाठ_last_किल्ली(संरचना वृक्ष_गाठ*गाठ)
{
      यावत् (गाठ->leaf_flag == असत्य)
	    गाठ = गाठ->child[गाठ->मोजणी-1];

      प्रत्यागम गाठ->leaf[गाठ->मोजणी-1].किल्ली;
}

/*
 * Allocate a new symbol table means creating the table structure and
 * a root node, and initializing the pointers and members of the root
 * node.
 */
चिन्ह_table_s::चिन्ह_table_s()
{
      मूळ = नूतन संरचना वृक्ष_गाठ;
      मूळ->leaf_flag = असत्य;
      मूळ->मोजणी = 0;
      मूळ->parent = 0;

      str_chunk = नूतन किल्ली_strings;
      str_chunk->next = 0;
      str_used = 0;
}

स्थिर शून्य delete_चिन्ह_गाठ(संरचना वृक्ष_गाठ*cur)
{
      यदि (! cur->leaf_flag) {
	    प्रति (unsigned idx = 0 ;  idx < cur->मोजणी ;  idx += 1)
		  delete_चिन्ह_गाठ(cur->child[idx]);
      }

      delete cur;
}

/* Do as split_leaf_ does, but for nodes. */
स्थिर शून्य split_गाठ(संरचना वृक्ष_गाठ*cur)
{
      प्रतिज्ञा(!cur->leaf_flag);
      यदि (cur->parent) प्रतिज्ञा(! cur->parent->leaf_flag);

      यावत् (cur->मोजणी == गाठ_width) {
	      /* Create a new node to hold half the data from cur. */
	    संरचना वृक्ष_गाठ *new_गाठ = नूतन संरचना वृक्ष_गाठ;
	    new_गाठ->leaf_flag = असत्य;
	    new_गाठ->मोजणी = cur->मोजणी / 2;
	      /* cur is not root; new_node becomes sibling. */
	    यदि (cur->parent) new_गाठ->parent = cur->parent;

	      /* Move the last half of the data from the end of the old node
	       * to the beginning of the new node. At the same time, reduce
	       * the size of the old node. */
	    unsigned idx1 = new_गाठ->मोजणी;
	    unsigned idx2 = cur->मोजणी;
	    यावत् (idx1 > 0) {
		  idx1 -= 1;
		  idx2 -= 1;
		  new_गाठ->child[idx1] = cur->child[idx2];
		  new_गाठ->child[idx1]->parent = new_गाठ;
		  cur->मोजणी -= 1;
	    }

	    प्रतिज्ञा(new_गाठ->मोजणी > 0);
	    प्रतिज्ञा(cur->मोजणी > 0);

	    यदि (cur->parent == 0) {
		    /* cur is root. Move first half of children to another
		     * new node, and put the two new nodes in cur. The plan
		     * here is to make cur into the new root and split its
		     * contents into 2 children. */

		  new_गाठ->parent = cur;
		  संरचना वृक्ष_गाठ*new2_गाठ = नूतन संरचना वृक्ष_गाठ;
		  new2_गाठ->leaf_flag = असत्य;
		  new2_गाठ->मोजणी = cur->मोजणी;
		  new2_गाठ->parent = cur;
		  प्रति (unsigned idx = 0; idx < cur->मोजणी; idx += 1) {
			new2_गाठ->child[idx] = cur->child[idx];
			new2_गाठ->child[idx]->parent = new2_गाठ;
		  }
		  cur->child[0] = new2_गाठ;
		  cur->child[1] = new_गाठ;
		  cur->मोजणी = 2;
		    /* no more ancestors, stop the while loop */
		  भङ्ग;
	    }

	      /* cur is not root. hook new_node to cur->parent. */
	    unsigned idx = 0;
	    यावत् (cur->parent->child[idx] != cur) {
		  प्रतिज्ञा(idx < cur->parent->मोजणी);
		  idx += 1;
	    }
	    idx += 1;

	    प्रति (unsigned tmp = cur->parent->मोजणी ;  tmp > idx ;  tmp -= 1) {
		  cur->parent->child[tmp] = cur->parent->child[tmp-1];
	    }

	    cur->parent->child[idx] = new_गाठ;
	    cur->parent->मोजणी += 1;

	      /* check the ancestor */
	    cur = cur->parent;
      }
}

/*
 * This function takes a leaf node and splits in into two. Move half
 * the leaf keys into the new node, and add the new leaf into the
 * parent node.
 */
स्थिर संरचना वृक्ष_गाठ* split_leaf_(संरचना वृक्ष_गाठ*cur)
{
      प्रतिज्ञा(cur->leaf_flag);
      प्रतिज्ञा(cur->parent);
      प्रतिज्ञा(! cur->parent->leaf_flag);

	/* Create a new leaf to hold half the data from the old leaf. */
      संरचना वृक्ष_गाठ*new_leaf = नूतन संरचना वृक्ष_गाठ;
      new_leaf->leaf_flag = सत्य;
      new_leaf->मोजणी = cur->मोजणी / 2;
      new_leaf->parent = cur->parent;

	/* Move the last half of the data from the end of the old leaf
	   to the beginning of the new leaf. At the same time, reduce
	   the size of the old leaf. */
      unsigned idx1 = new_leaf->मोजणी;
      unsigned idx2 = cur->मोजणी;
      यावत् (idx1 > 0) {
	    idx1 -= 1;
	    idx2 -= 1;
	    new_leaf->leaf[idx1] = cur->leaf[idx2];
	    cur->मोजणी -= 1;
      }

      प्रतिज्ञा(new_leaf->मोजणी > 0);
      प्रतिज्ञा(cur->मोजणी > 0);

      unsigned idx = 0;
      यावत् (cur->parent->child[idx] != cur) {
	    प्रतिज्ञा(idx < cur->parent->मोजणी);
	    idx += 1;
      }

      idx += 1;

      प्रति (unsigned tmp = cur->parent->मोजणी ;  tmp > idx ;  tmp -= 1)
	    cur->parent->child[tmp] = cur->parent->child[tmp-1];

      cur->parent->child[idx] = new_leaf;
      cur->parent->मोजणी += 1;

      यदि (cur->parent->मोजणी == गाठ_width)
	    split_गाठ(cur->parent);

      प्रत्यागम new_leaf;
}


/*
 * This function searches tree recursively for the key. If the value
 * is not found (and we are at a leaf) then set the key with the given
 * value. If the key is found, set the value only if the force_flag is
 * true.
 */

चिन्ह_मूल्य_t चिन्ह_table_s::find_मूल्य(संरचना वृक्ष_गाठ*cur,
					   ध्रुव वर्ण*किल्ली, चिन्ह_मूल्य_t val,
					   तार्किक force_flag)
{
      यदि (cur->leaf_flag) {

	    unsigned idx = 0;
	    प्रति (;;) {
		    /* If we run out of keys in the leaf, then add
		       this at the end of the leaf. */
		  यदि (idx == cur->मोजणी) {
			cur->leaf[idx].किल्ली = किल्ली_strdup(किल्ली);
			cur->leaf[idx].val = val;
			cur->मोजणी += 1;
			यदि (cur->मोजणी == leaf_width)
			      split_leaf_(cur);

			प्रत्यागम val;
		  }

		  पूर्णाङ्क rc = strcmp(किल्ली, cur->leaf[idx].किल्ली);

		    /* If we found the key already in the table, then
		       set the new value and break. */
		  यदि (rc == 0) {
			यदि (force_flag)
			      cur->leaf[idx].val = val;
			प्रत्यागम cur->leaf[idx].val;
		  }

		    /* If this key goes before the current key, then
		       push all the following entries back and add
		       this key here. */
		  यदि (rc < 0) {
			प्रति (unsigned tmp = cur->मोजणी; tmp > idx; tmp -= 1)
			      cur->leaf[tmp] = cur->leaf[tmp-1];

			cur->leaf[idx].किल्ली = किल्ली_strdup(किल्ली);
			cur->leaf[idx].val = val;
			cur->मोजणी += 1;
			यदि (cur->मोजणी == leaf_width)
			      split_leaf_(cur);

			प्रत्यागम val;
		  }

		  idx += 1;
	    }

      } अन्यथा {
	      /* Do a binary search within the inner node. */
	    unsigned min = 0;
	    unsigned max = cur->मोजणी;
	    unsigned idx = max/2;
	    प्रति (;;) {
		  पूर्णाङ्क rc = strcmp(किल्ली, गाठ_last_किल्ली(cur->child[idx]));
		  यदि (rc == 0) {
			प्रत्यागम find_मूल्य(cur->child[idx],
					   किल्ली, val, force_flag);
		  }

		  यदि (rc > 0) {
			min = idx + 1;
			यदि (min == cur->मोजणी)
			      प्रत्यागम find_मूल्य(cur->child[idx],
						 किल्ली, val, force_flag);
			यदि (min == max)
			      प्रत्यागम find_मूल्य(cur->child[max],
						 किल्ली, val, force_flag);

			idx = min + (max-min)/2;

		  } अन्यथा  {
			max = idx;
			यदि (idx == min)
			      प्रत्यागम find_मूल्य(cur->child[idx],
						 किल्ली, val, force_flag);
			idx = min + (max-min)/2;
		  }
	    }
      }

      प्रतिज्ञा(0);
      { चिन्ह_मूल्य_t tmp;
        tmp.ptr = 0;
        प्रत्यागम tmp;
      }
}

शून्य चिन्ह_table_s::sym_set_मूल्य(ध्रुव वर्ण*किल्ली, चिन्ह_मूल्य_t val)
{
      यदि (मूळ->मोजणी == 0) {
	      /* Handle the special case that this is the very first
		 value in the symbol table. Create the first leaf node
		 and initialize the pointers. */
	    संरचना वृक्ष_गाठ*cur = नूतन संरचना वृक्ष_गाठ;
	    cur->leaf_flag = सत्य;
	    cur->parent = मूळ;
	    cur->मोजणी = 1;
	    cur->leaf[0].किल्ली = किल्ली_strdup(किल्ली);
	    cur->leaf[0].val = val;

	    मूळ->मोजणी = 1;
	    मूळ->child[0] = cur;
      } अन्यथा {
	    find_मूल्य(मूळ, किल्ली, val, सत्य);
      }
}

चिन्ह_मूल्य_t चिन्ह_table_s::sym_get_मूल्य(ध्रुव वर्ण*किल्ली)
{
      चिन्ह_मूल्य_t def;
      def.ptr = 0;

      यदि (मूळ->मोजणी == 0) {
	      /* Handle the special case that this is the very first
		 value in the symbol table. Create the first leaf node
		 and initialize the pointers. */
	    संरचना वृक्ष_गाठ*cur = नूतन संरचना वृक्ष_गाठ;
	    cur->leaf_flag = सत्य;
	    cur->parent = मूळ;
	    cur->मोजणी = 1;
	    cur->leaf[0].किल्ली = किल्ली_strdup(किल्ली);
	    cur->leaf[0].val = def;

	    मूळ->मोजणी = 1;
	    मूळ->child[0] = cur;
	    प्रत्यागम cur->leaf[0].val;
      } अन्यथा {
	    प्रत्यागम find_मूल्य(मूळ, किल्ली, def, असत्य);
      }
}

चिन्ह_table_s::~चिन्ह_table_s()
{
      delete_चिन्ह_गाठ(मूळ);
      यावत् (str_chunk) {
	    किल्ली_strings*tmp = str_chunk;
	    str_chunk = tmp->next;
	    delete tmp;
      }
}
