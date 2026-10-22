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
संरचना कुञ्चिका_strings {
      संरचना कुञ्चिका_strings*next;
      वर्ण data[64*1024 - परिमाण(संरचना कुञ्चिका_strings*)];
};

वर्ण*चिह्नम्_table_s::कुञ्चिका_strdup(ध्रुव वर्ण*str)
{
      unsigned len = strlen(str);
      प्रतिज्ञा( (len+1) <= परिमाण str_chunk->data );

      जइ ( (len+1) > (परिमाण str_chunk->data - str_used) ) {
	    कुञ्चिका_strings*tmp = नूतन कुञ्चिका_strings;
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
ध्रुव unsigned ग्रन्थिः_width = 508;

संरचना वृक्षः_data {
      वर्ण*कुञ्चिका;
      चिह्नम्_मानम्_t val;
};

संरचना वृक्ष_ग्रन्थिः {
      तार्किक leaf_flag;
      unsigned गणनम्;
      संरचना वृक्ष_ग्रन्थिः*parent;

      सङ्घ {
	    संरचना वृक्षः_data leaf[leaf_width];
	    संरचना वृक्ष_ग्रन्थिः *child[ग्रन्थिः_width];
      };

};

स्थिर अन्तःस्थ वर्ण* ग्रन्थिः_last_कुञ्चिका(संरचना वृक्ष_ग्रन्थिः*ग्रन्थिः)
{
      जाव (ग्रन्थिः->leaf_flag == मिच्छ)
	    ग्रन्थिः = ग्रन्थिः->child[ग्रन्थिः->गणनम्-1];

      प्रत्यागम ग्रन्थिः->leaf[ग्रन्थिः->गणनम्-1].कुञ्चिका;
}

/*
 * Allocate a new symbol table means creating the table structure and
 * a root node, and initializing the pointers and members of the root
 * node.
 */
चिह्नम्_table_s::चिह्नम्_table_s()
{
      मूलम् = नूतन संरचना वृक्ष_ग्रन्थिः;
      मूलम्->leaf_flag = मिच्छ;
      मूलम्->गणनम् = 0;
      मूलम्->parent = 0;

      str_chunk = नूतन कुञ्चिका_strings;
      str_chunk->next = 0;
      str_used = 0;
}

स्थिर शून्य delete_चिह्नम्_ग्रन्थिः(संरचना वृक्ष_ग्रन्थिः*cur)
{
      जइ (! cur->leaf_flag) {
	    प्रति (unsigned idx = 0 ;  idx < cur->गणनम् ;  idx += 1)
		  delete_चिह्नम्_ग्रन्थिः(cur->child[idx]);
      }

      delete cur;
}

/* Do as split_leaf_ does, but for nodes. */
स्थिर शून्य split_ग्रन्थिः(संरचना वृक्ष_ग्रन्थिः*cur)
{
      प्रतिज्ञा(!cur->leaf_flag);
      जइ (cur->parent) प्रतिज्ञा(! cur->parent->leaf_flag);

      जाव (cur->गणनम् == ग्रन्थिः_width) {
	      /* Create a new node to hold half the data from cur. */
	    संरचना वृक्ष_ग्रन्थिः *new_ग्रन्थिः = नूतन संरचना वृक्ष_ग्रन्थिः;
	    new_ग्रन्थिः->leaf_flag = मिच्छ;
	    new_ग्रन्थिः->गणनम् = cur->गणनम् / 2;
	      /* cur is not root; new_node becomes sibling. */
	    जइ (cur->parent) new_ग्रन्थिः->parent = cur->parent;

	      /* Move the last half of the data from the end of the old node
	       * to the beginning of the new node. At the same time, reduce
	       * the size of the old node. */
	    unsigned idx1 = new_ग्रन्थिः->गणनम्;
	    unsigned idx2 = cur->गणनम्;
	    जाव (idx1 > 0) {
		  idx1 -= 1;
		  idx2 -= 1;
		  new_ग्रन्थिः->child[idx1] = cur->child[idx2];
		  new_ग्रन्थिः->child[idx1]->parent = new_ग्रन्थिः;
		  cur->गणनम् -= 1;
	    }

	    प्रतिज्ञा(new_ग्रन्थिः->गणनम् > 0);
	    प्रतिज्ञा(cur->गणनम् > 0);

	    जइ (cur->parent == 0) {
		    /* cur is root. Move first half of children to another
		     * new node, and put the two new nodes in cur. The plan
		     * here is to make cur into the new root and split its
		     * contents into 2 children. */

		  new_ग्रन्थिः->parent = cur;
		  संरचना वृक्ष_ग्रन्थिः*new2_ग्रन्थिः = नूतन संरचना वृक्ष_ग्रन्थिः;
		  new2_ग्रन्थिः->leaf_flag = मिच्छ;
		  new2_ग्रन्थिः->गणनम् = cur->गणनम्;
		  new2_ग्रन्थिः->parent = cur;
		  प्रति (unsigned idx = 0; idx < cur->गणनम्; idx += 1) {
			new2_ग्रन्थिः->child[idx] = cur->child[idx];
			new2_ग्रन्थिः->child[idx]->parent = new2_ग्रन्थिः;
		  }
		  cur->child[0] = new2_ग्रन्थिः;
		  cur->child[1] = new_ग्रन्थिः;
		  cur->गणनम् = 2;
		    /* no more ancestors, stop the while loop */
		  भङ्ग;
	    }

	      /* cur is not root. hook new_node to cur->parent. */
	    unsigned idx = 0;
	    जाव (cur->parent->child[idx] != cur) {
		  प्रतिज्ञा(idx < cur->parent->गणनम्);
		  idx += 1;
	    }
	    idx += 1;

	    प्रति (unsigned tmp = cur->parent->गणनम् ;  tmp > idx ;  tmp -= 1) {
		  cur->parent->child[tmp] = cur->parent->child[tmp-1];
	    }

	    cur->parent->child[idx] = new_ग्रन्थिः;
	    cur->parent->गणनम् += 1;

	      /* check the ancestor */
	    cur = cur->parent;
      }
}

/*
 * This function takes a leaf node and splits in into two. Move half
 * the leaf keys into the new node, and add the new leaf into the
 * parent node.
 */
स्थिर संरचना वृक्ष_ग्रन्थिः* split_leaf_(संरचना वृक्ष_ग्रन्थिः*cur)
{
      प्रतिज्ञा(cur->leaf_flag);
      प्रतिज्ञा(cur->parent);
      प्रतिज्ञा(! cur->parent->leaf_flag);

	/* Create a new leaf to hold half the data from the old leaf. */
      संरचना वृक्ष_ग्रन्थिः*new_leaf = नूतन संरचना वृक्ष_ग्रन्थिः;
      new_leaf->leaf_flag = सच्च;
      new_leaf->गणनम् = cur->गणनम् / 2;
      new_leaf->parent = cur->parent;

	/* Move the last half of the data from the end of the old leaf
	   to the beginning of the new leaf. At the same time, reduce
	   the size of the old leaf. */
      unsigned idx1 = new_leaf->गणनम्;
      unsigned idx2 = cur->गणनम्;
      जाव (idx1 > 0) {
	    idx1 -= 1;
	    idx2 -= 1;
	    new_leaf->leaf[idx1] = cur->leaf[idx2];
	    cur->गणनम् -= 1;
      }

      प्रतिज्ञा(new_leaf->गणनम् > 0);
      प्रतिज्ञा(cur->गणनम् > 0);

      unsigned idx = 0;
      जाव (cur->parent->child[idx] != cur) {
	    प्रतिज्ञा(idx < cur->parent->गणनम्);
	    idx += 1;
      }

      idx += 1;

      प्रति (unsigned tmp = cur->parent->गणनम् ;  tmp > idx ;  tmp -= 1)
	    cur->parent->child[tmp] = cur->parent->child[tmp-1];

      cur->parent->child[idx] = new_leaf;
      cur->parent->गणनम् += 1;

      जइ (cur->parent->गणनम् == ग्रन्थिः_width)
	    split_ग्रन्थिः(cur->parent);

      प्रत्यागम new_leaf;
}


/*
 * This function searches tree recursively for the key. If the value
 * is not found (and we are at a leaf) then set the key with the given
 * value. If the key is found, set the value only if the force_flag is
 * true.
 */

चिह्नम्_मानम्_t चिह्नम्_table_s::find_मानम्(संरचना वृक्ष_ग्रन्थिः*cur,
					   ध्रुव वर्ण*कुञ्चिका, चिह्नम्_मानम्_t val,
					   तार्किक force_flag)
{
      जइ (cur->leaf_flag) {

	    unsigned idx = 0;
	    प्रति (;;) {
		    /* If we run out of keys in the leaf, then add
		       this at the end of the leaf. */
		  जइ (idx == cur->गणनम्) {
			cur->leaf[idx].कुञ्चिका = कुञ्चिका_strdup(कुञ्चिका);
			cur->leaf[idx].val = val;
			cur->गणनम् += 1;
			जइ (cur->गणनम् == leaf_width)
			      split_leaf_(cur);

			प्रत्यागम val;
		  }

		  पूर्णाङ्क rc = strcmp(कुञ्चिका, cur->leaf[idx].कुञ्चिका);

		    /* If we found the key already in the table, then
		       set the new value and break. */
		  जइ (rc == 0) {
			जइ (force_flag)
			      cur->leaf[idx].val = val;
			प्रत्यागम cur->leaf[idx].val;
		  }

		    /* If this key goes before the current key, then
		       push all the following entries back and add
		       this key here. */
		  जइ (rc < 0) {
			प्रति (unsigned tmp = cur->गणनम्; tmp > idx; tmp -= 1)
			      cur->leaf[tmp] = cur->leaf[tmp-1];

			cur->leaf[idx].कुञ्चिका = कुञ्चिका_strdup(कुञ्चिका);
			cur->leaf[idx].val = val;
			cur->गणनम् += 1;
			जइ (cur->गणनम् == leaf_width)
			      split_leaf_(cur);

			प्रत्यागम val;
		  }

		  idx += 1;
	    }

      } अण्णहा {
	      /* Do a binary search within the inner node. */
	    unsigned min = 0;
	    unsigned max = cur->गणनम्;
	    unsigned idx = max/2;
	    प्रति (;;) {
		  पूर्णाङ्क rc = strcmp(कुञ्चिका, ग्रन्थिः_last_कुञ्चिका(cur->child[idx]));
		  जइ (rc == 0) {
			प्रत्यागम find_मानम्(cur->child[idx],
					   कुञ्चिका, val, force_flag);
		  }

		  जइ (rc > 0) {
			min = idx + 1;
			जइ (min == cur->गणनम्)
			      प्रत्यागम find_मानम्(cur->child[idx],
						 कुञ्चिका, val, force_flag);
			जइ (min == max)
			      प्रत्यागम find_मानम्(cur->child[max],
						 कुञ्चिका, val, force_flag);

			idx = min + (max-min)/2;

		  } अण्णहा  {
			max = idx;
			जइ (idx == min)
			      प्रत्यागम find_मानम्(cur->child[idx],
						 कुञ्चिका, val, force_flag);
			idx = min + (max-min)/2;
		  }
	    }
      }

      प्रतिज्ञा(0);
      { चिह्नम्_मानम्_t tmp;
        tmp.ptr = 0;
        प्रत्यागम tmp;
      }
}

शून्य चिह्नम्_table_s::sym_set_मानम्(ध्रुव वर्ण*कुञ्चिका, चिह्नम्_मानम्_t val)
{
      जइ (मूलम्->गणनम् == 0) {
	      /* Handle the special case that this is the very first
		 value in the symbol table. Create the first leaf node
		 and initialize the pointers. */
	    संरचना वृक्ष_ग्रन्थिः*cur = नूतन संरचना वृक्ष_ग्रन्थिः;
	    cur->leaf_flag = सच्च;
	    cur->parent = मूलम्;
	    cur->गणनम् = 1;
	    cur->leaf[0].कुञ्चिका = कुञ्चिका_strdup(कुञ्चिका);
	    cur->leaf[0].val = val;

	    मूलम्->गणनम् = 1;
	    मूलम्->child[0] = cur;
      } अण्णहा {
	    find_मानम्(मूलम्, कुञ्चिका, val, सच्च);
      }
}

चिह्नम्_मानम्_t चिह्नम्_table_s::sym_get_मानम्(ध्रुव वर्ण*कुञ्चिका)
{
      चिह्नम्_मानम्_t def;
      def.ptr = 0;

      जइ (मूलम्->गणनम् == 0) {
	      /* Handle the special case that this is the very first
		 value in the symbol table. Create the first leaf node
		 and initialize the pointers. */
	    संरचना वृक्ष_ग्रन्थिः*cur = नूतन संरचना वृक्ष_ग्रन्थिः;
	    cur->leaf_flag = सच्च;
	    cur->parent = मूलम्;
	    cur->गणनम् = 1;
	    cur->leaf[0].कुञ्चिका = कुञ्चिका_strdup(कुञ्चिका);
	    cur->leaf[0].val = def;

	    मूलम्->गणनम् = 1;
	    मूलम्->child[0] = cur;
	    प्रत्यागम cur->leaf[0].val;
      } अण्णहा {
	    प्रत्यागम find_मानम्(मूलम्, कुञ्चिका, def, मिच्छ);
      }
}

चिह्नम्_table_s::~चिह्नम्_table_s()
{
      delete_चिह्नम्_ग्रन्थिः(मूलम्);
      जाव (str_chunk) {
	    कुञ्चिका_strings*tmp = str_chunk;
	    str_chunk = tmp->next;
	    delete tmp;
      }
}
