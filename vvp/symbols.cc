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
संरचना कुंजी_strings {
      संरचना कुंजी_strings*next;
      वर्ण data[64*1024 - परिमाण(संरचना कुंजी_strings*)];
};

वर्ण*प्रतीक_table_s::कुंजी_strdup(ध्रुव वर्ण*str)
{
      अचिन्हित len = म_माप(str);
      प्रतिज्ञा( (len+1) <= परिमाण str_chunk->data );

      यदि ( (len+1) > (परिमाण str_chunk->data - str_used) ) {
	    कुंजी_strings*tmp = नूतन कुंजी_strings;
	    tmp->next = str_chunk;
	    str_chunk = tmp;
	    str_used = 0;
      }

      वर्ण*res = str_chunk->data + str_used;
      str_used += len + 1;
      म_नकल(res, str);
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

ध्रुव अचिन्हित leaf_width = 254;
ध्रुव अचिन्हित गाँठ_width = 508;

संरचना वृक्ष_data {
      वर्ण*कुंजी;
      प्रतीक_मूल्य_t val;
};

संरचना वृक्ष_गाँठ {
      तार्किक leaf_flag;
      अचिन्हित गिनती;
      संरचना वृक्ष_गाँठ*parent;

      सङ्घ {
	    संरचना वृक्ष_data leaf[leaf_width];
	    संरचना वृक्ष_गाँठ *child[गाँठ_width];
      };

};

स्थिर अन्तःस्थ वर्ण* गाँठ_last_कुंजी(संरचना वृक्ष_गाँठ*गाँठ)
{
      यावत् (गाँठ->leaf_flag == असत्य)
	    गाँठ = गाँठ->child[गाँठ->गिनती-1];

      प्रत्यागम गाँठ->leaf[गाँठ->गिनती-1].कुंजी;
}

/*
 * Allocate a new symbol table means creating the table structure and
 * a root node, and initializing the pointers and members of the root
 * node.
 */
प्रतीक_table_s::प्रतीक_table_s()
{
      मूल = नूतन संरचना वृक्ष_गाँठ;
      मूल->leaf_flag = असत्य;
      मूल->गिनती = 0;
      मूल->parent = 0;

      str_chunk = नूतन कुंजी_strings;
      str_chunk->next = 0;
      str_used = 0;
}

स्थिर शून्य delete_प्रतीक_गाँठ(संरचना वृक्ष_गाँठ*cur)
{
      यदि (! cur->leaf_flag) {
	    प्रति (अचिन्हित idx = 0 ;  idx < cur->गिनती ;  idx += 1)
		  delete_प्रतीक_गाँठ(cur->child[idx]);
      }

      delete cur;
}

/* Do as split_leaf_ does, but for nodes. */
स्थिर शून्य split_गाँठ(संरचना वृक्ष_गाँठ*cur)
{
      प्रतिज्ञा(!cur->leaf_flag);
      यदि (cur->parent) प्रतिज्ञा(! cur->parent->leaf_flag);

      यावत् (cur->गिनती == गाँठ_width) {
	      /* Create a new node to hold half the data from cur. */
	    संरचना वृक्ष_गाँठ *new_गाँठ = नूतन संरचना वृक्ष_गाँठ;
	    new_गाँठ->leaf_flag = असत्य;
	    new_गाँठ->गिनती = cur->गिनती / 2;
	      /* cur is not root; new_node becomes sibling. */
	    यदि (cur->parent) new_गाँठ->parent = cur->parent;

	      /* Move the last half of the data from the end of the old node
	       * to the beginning of the new node. At the same time, reduce
	       * the size of the old node. */
	    अचिन्हित idx1 = new_गाँठ->गिनती;
	    अचिन्हित idx2 = cur->गिनती;
	    यावत् (idx1 > 0) {
		  idx1 -= 1;
		  idx2 -= 1;
		  new_गाँठ->child[idx1] = cur->child[idx2];
		  new_गाँठ->child[idx1]->parent = new_गाँठ;
		  cur->गिनती -= 1;
	    }

	    प्रतिज्ञा(new_गाँठ->गिनती > 0);
	    प्रतिज्ञा(cur->गिनती > 0);

	    यदि (cur->parent == 0) {
		    /* cur is root. Move first half of children to another
		     * new node, and put the two new nodes in cur. The plan
		     * here is to make cur into the new root and split its
		     * contents into 2 children. */

		  new_गाँठ->parent = cur;
		  संरचना वृक्ष_गाँठ*new2_गाँठ = नूतन संरचना वृक्ष_गाँठ;
		  new2_गाँठ->leaf_flag = असत्य;
		  new2_गाँठ->गिनती = cur->गिनती;
		  new2_गाँठ->parent = cur;
		  प्रति (अचिन्हित idx = 0; idx < cur->गिनती; idx += 1) {
			new2_गाँठ->child[idx] = cur->child[idx];
			new2_गाँठ->child[idx]->parent = new2_गाँठ;
		  }
		  cur->child[0] = new2_गाँठ;
		  cur->child[1] = new_गाँठ;
		  cur->गिनती = 2;
		    /* no more ancestors, stop the while loop */
		  भङ्ग;
	    }

	      /* cur is not root. hook new_node to cur->parent. */
	    अचिन्हित idx = 0;
	    यावत् (cur->parent->child[idx] != cur) {
		  प्रतिज्ञा(idx < cur->parent->गिनती);
		  idx += 1;
	    }
	    idx += 1;

	    प्रति (अचिन्हित tmp = cur->parent->गिनती ;  tmp > idx ;  tmp -= 1) {
		  cur->parent->child[tmp] = cur->parent->child[tmp-1];
	    }

	    cur->parent->child[idx] = new_गाँठ;
	    cur->parent->गिनती += 1;

	      /* check the ancestor */
	    cur = cur->parent;
      }
}

/*
 * This function takes a leaf node and splits in into two. Move half
 * the leaf keys into the new node, and add the new leaf into the
 * parent node.
 */
स्थिर संरचना वृक्ष_गाँठ* split_leaf_(संरचना वृक्ष_गाँठ*cur)
{
      प्रतिज्ञा(cur->leaf_flag);
      प्रतिज्ञा(cur->parent);
      प्रतिज्ञा(! cur->parent->leaf_flag);

	/* Create a new leaf to hold half the data from the old leaf. */
      संरचना वृक्ष_गाँठ*new_leaf = नूतन संरचना वृक्ष_गाँठ;
      new_leaf->leaf_flag = सत्य;
      new_leaf->गिनती = cur->गिनती / 2;
      new_leaf->parent = cur->parent;

	/* Move the last half of the data from the end of the old leaf
	   to the beginning of the new leaf. At the same time, reduce
	   the size of the old leaf. */
      अचिन्हित idx1 = new_leaf->गिनती;
      अचिन्हित idx2 = cur->गिनती;
      यावत् (idx1 > 0) {
	    idx1 -= 1;
	    idx2 -= 1;
	    new_leaf->leaf[idx1] = cur->leaf[idx2];
	    cur->गिनती -= 1;
      }

      प्रतिज्ञा(new_leaf->गिनती > 0);
      प्रतिज्ञा(cur->गिनती > 0);

      अचिन्हित idx = 0;
      यावत् (cur->parent->child[idx] != cur) {
	    प्रतिज्ञा(idx < cur->parent->गिनती);
	    idx += 1;
      }

      idx += 1;

      प्रति (अचिन्हित tmp = cur->parent->गिनती ;  tmp > idx ;  tmp -= 1)
	    cur->parent->child[tmp] = cur->parent->child[tmp-1];

      cur->parent->child[idx] = new_leaf;
      cur->parent->गिनती += 1;

      यदि (cur->parent->गिनती == गाँठ_width)
	    split_गाँठ(cur->parent);

      प्रत्यागम new_leaf;
}


/*
 * This function searches tree recursively for the key. If the value
 * is not found (and we are at a leaf) then set the key with the given
 * value. If the key is found, set the value only if the force_flag is
 * true.
 */

प्रतीक_मूल्य_t प्रतीक_table_s::find_मूल्य(संरचना वृक्ष_गाँठ*cur,
					   ध्रुव वर्ण*कुंजी, प्रतीक_मूल्य_t val,
					   तार्किक force_flag)
{
      यदि (cur->leaf_flag) {

	    अचिन्हित idx = 0;
	    प्रति (;;) {
		    /* If we run out of keys in the leaf, then add
		       this at the end of the leaf. */
		  यदि (idx == cur->गिनती) {
			cur->leaf[idx].कुंजी = कुंजी_strdup(कुंजी);
			cur->leaf[idx].val = val;
			cur->गिनती += 1;
			यदि (cur->गिनती == leaf_width)
			      split_leaf_(cur);

			प्रत्यागम val;
		  }

		  पूर्णाङ्क rc = म_भेद(कुंजी, cur->leaf[idx].कुंजी);

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
			प्रति (अचिन्हित tmp = cur->गिनती; tmp > idx; tmp -= 1)
			      cur->leaf[tmp] = cur->leaf[tmp-1];

			cur->leaf[idx].कुंजी = कुंजी_strdup(कुंजी);
			cur->leaf[idx].val = val;
			cur->गिनती += 1;
			यदि (cur->गिनती == leaf_width)
			      split_leaf_(cur);

			प्रत्यागम val;
		  }

		  idx += 1;
	    }

      } अन्यथा {
	      /* Do a binary search within the inner node. */
	    अचिन्हित min = 0;
	    अचिन्हित max = cur->गिनती;
	    अचिन्हित idx = max/2;
	    प्रति (;;) {
		  पूर्णाङ्क rc = म_भेद(कुंजी, गाँठ_last_कुंजी(cur->child[idx]));
		  यदि (rc == 0) {
			प्रत्यागम find_मूल्य(cur->child[idx],
					   कुंजी, val, force_flag);
		  }

		  यदि (rc > 0) {
			min = idx + 1;
			यदि (min == cur->गिनती)
			      प्रत्यागम find_मूल्य(cur->child[idx],
						 कुंजी, val, force_flag);
			यदि (min == max)
			      प्रत्यागम find_मूल्य(cur->child[max],
						 कुंजी, val, force_flag);

			idx = min + (max-min)/2;

		  } अन्यथा  {
			max = idx;
			यदि (idx == min)
			      प्रत्यागम find_मूल्य(cur->child[idx],
						 कुंजी, val, force_flag);
			idx = min + (max-min)/2;
		  }
	    }
      }

      प्रतिज्ञा(0);
      { प्रतीक_मूल्य_t tmp;
        tmp.ptr = 0;
        प्रत्यागम tmp;
      }
}

शून्य प्रतीक_table_s::sym_set_मूल्य(ध्रुव वर्ण*कुंजी, प्रतीक_मूल्य_t val)
{
      यदि (मूल->गिनती == 0) {
	      /* Handle the special case that this is the very first
		 value in the symbol table. Create the first leaf node
		 and initialize the pointers. */
	    संरचना वृक्ष_गाँठ*cur = नूतन संरचना वृक्ष_गाँठ;
	    cur->leaf_flag = सत्य;
	    cur->parent = मूल;
	    cur->गिनती = 1;
	    cur->leaf[0].कुंजी = कुंजी_strdup(कुंजी);
	    cur->leaf[0].val = val;

	    मूल->गिनती = 1;
	    मूल->child[0] = cur;
      } अन्यथा {
	    find_मूल्य(मूल, कुंजी, val, सत्य);
      }
}

प्रतीक_मूल्य_t प्रतीक_table_s::sym_get_मूल्य(ध्रुव वर्ण*कुंजी)
{
      प्रतीक_मूल्य_t def;
      def.ptr = 0;

      यदि (मूल->गिनती == 0) {
	      /* Handle the special case that this is the very first
		 value in the symbol table. Create the first leaf node
		 and initialize the pointers. */
	    संरचना वृक्ष_गाँठ*cur = नूतन संरचना वृक्ष_गाँठ;
	    cur->leaf_flag = सत्य;
	    cur->parent = मूल;
	    cur->गिनती = 1;
	    cur->leaf[0].कुंजी = कुंजी_strdup(कुंजी);
	    cur->leaf[0].val = def;

	    मूल->गिनती = 1;
	    मूल->child[0] = cur;
	    प्रत्यागम cur->leaf[0].val;
      } अन्यथा {
	    प्रत्यागम find_मूल्य(मूल, कुंजी, def, असत्य);
      }
}

प्रतीक_table_s::~प्रतीक_table_s()
{
      delete_प्रतीक_गाँठ(मूल);
      यावत् (str_chunk) {
	    कुंजी_strings*tmp = str_chunk;
	    str_chunk = tmp->next;
	    delete tmp;
      }
}
