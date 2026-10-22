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
संरचना कुञ्चिका_वाक्यानि {
      संरचना कुञ्चिका_वाक्यानि*अग्र_2;
      वर्ण दत्त[64*1024 - परिमाण(संरचना कुञ्चिका_वाक्यानि*)];
};

वर्ण*चिह्नम्_सारणी_s::कुञ्चिका_strdup(ध्रुव वर्ण*str)
{
      अचिह्नित len = वाक्यदैर्घ्यम्(str);
      प्रतिज्ञा( (len+1) <= परिमाण str_chunk->दत्त );

      यदि ( (len+1) > (परिमाण str_chunk->दत्त - str_प्रयुक्तम्) ) {
	    कुञ्चिका_वाक्यानि*tmp = नूतन कुञ्चिका_वाक्यानि;
	    tmp->अग्र_2 = str_chunk;
	    str_chunk = tmp;
	    str_प्रयुक्तम् = 0;
      }

      वर्ण*res = str_chunk->दत्त + str_प्रयुक्तम्;
      str_प्रयुक्तम् += len + 1;
      वाक्यं_प्रतिलिख(res, str);
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

ध्रुव अचिह्नित पर्णम्_विस्तारः = 254;
ध्रुव अचिह्नित ग्रन्थिः_विस्तारः = 508;

संरचना वृक्षः_दत्त {
      वर्ण*कुञ्चिका;
      चिह्नम्_मानम्_t val;
};

संरचना वृक्ष_ग्रन्थिः {
      तार्किक पर्णम्_flag;
      अचिह्नित गणनम्;
      संरचना वृक्ष_ग्रन्थिः*parent;

      सङ्घ {
	    संरचना वृक्षः_दत्त पर्णम्[पर्णम्_विस्तारः];
	    संरचना वृक्ष_ग्रन्थिः *child[ग्रन्थिः_विस्तारः];
      };

};

स्थिर अन्तःस्थ वर्ण* ग्रन्थिः_चरमः_कुञ्चिका(संरचना वृक्ष_ग्रन्थिः*ग्रन्थिः)
{
      यावत् (ग्रन्थिः->पर्णम्_flag == असत्य)
	    ग्रन्थिः = ग्रन्थिः->child[ग्रन्थिः->गणनम्-1];

      प्रत्यागम ग्रन्थिः->पर्णम्[ग्रन्थिः->गणनम्-1].कुञ्चिका;
}

/*
 * Allocate a new symbol table means creating the table structure and
 * a root node, and initializing the pointers and members of the root
 * node.
 */
चिह्नम्_सारणी_s::चिह्नम्_सारणी_s()
{
      मूलम् = नूतन संरचना वृक्ष_ग्रन्थिः;
      मूलम्->पर्णम्_flag = असत्य;
      मूलम्->गणनम् = 0;
      मूलम्->parent = 0;

      str_chunk = नूतन कुञ्चिका_वाक्यानि;
      str_chunk->अग्र_2 = 0;
      str_प्रयुक्तम् = 0;
}

स्थिर शून्य delete_चिह्नम्_ग्रन्थिः(संरचना वृक्ष_ग्रन्थिः*cur)
{
      यदि (! cur->पर्णम्_flag) {
	    प्रति (अचिह्नित idx = 0 ;  idx < cur->गणनम् ;  idx += 1)
		  delete_चिह्नम्_ग्रन्थिः(cur->child[idx]);
      }

      विलोपय cur;
}

/* Do as split_leaf_ does, but for nodes. */
स्थिर शून्य विभज_ग्रन्थिः(संरचना वृक्ष_ग्रन्थिः*cur)
{
      प्रतिज्ञा(!cur->पर्णम्_flag);
      यदि (cur->parent) प्रतिज्ञा(! cur->parent->पर्णम्_flag);

      यावत् (cur->गणनम् == ग्रन्थिः_विस्तारः) {
	      /* Create a new node to hold half the data from cur. */
	    संरचना वृक्ष_ग्रन्थिः *new_ग्रन्थिः = नूतन संरचना वृक्ष_ग्रन्थिः;
	    new_ग्रन्थिः->पर्णम्_flag = असत्य;
	    new_ग्रन्थिः->गणनम् = cur->गणनम् / 2;
	      /* cur is not root; new_node becomes sibling. */
	    यदि (cur->parent) new_ग्रन्थिः->parent = cur->parent;

	      /* Move the last half of the data from the end of the old node
	       * to the beginning of the new node. At the same time, reduce
	       * the size of the old node. */
	    अचिह्नित idx1 = new_ग्रन्थिः->गणनम्;
	    अचिह्नित idx2 = cur->गणनम्;
	    यावत् (idx1 > 0) {
		  idx1 -= 1;
		  idx2 -= 1;
		  new_ग्रन्थिः->child[idx1] = cur->child[idx2];
		  new_ग्रन्थिः->child[idx1]->parent = new_ग्रन्थिः;
		  cur->गणनम् -= 1;
	    }

	    प्रतिज्ञा(new_ग्रन्थिः->गणनम् > 0);
	    प्रतिज्ञा(cur->गणनम् > 0);

	    यदि (cur->parent == 0) {
		    /* cur is root. Move first half of children to another
		     * new node, and put the two new nodes in cur. The plan
		     * here is to make cur into the new root and split its
		     * contents into 2 children. */

		  new_ग्रन्थिः->parent = cur;
		  संरचना वृक्ष_ग्रन्थिः*new2_ग्रन्थिः = नूतन संरचना वृक्ष_ग्रन्थिः;
		  new2_ग्रन्थिः->पर्णम्_flag = असत्य;
		  new2_ग्रन्थिः->गणनम् = cur->गणनम्;
		  new2_ग्रन्थिः->parent = cur;
		  प्रति (अचिह्नित idx = 0; idx < cur->गणनम्; idx += 1) {
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
	    अचिह्नित idx = 0;
	    यावत् (cur->parent->child[idx] != cur) {
		  प्रतिज्ञा(idx < cur->parent->गणनम्);
		  idx += 1;
	    }
	    idx += 1;

	    प्रति (अचिह्नित tmp = cur->parent->गणनम् ;  tmp > idx ;  tmp -= 1) {
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
स्थिर संरचना वृक्ष_ग्रन्थिः* विभज_पर्णम्(संरचना वृक्ष_ग्रन्थिः*cur)
{
      प्रतिज्ञा(cur->पर्णम्_flag);
      प्रतिज्ञा(cur->parent);
      प्रतिज्ञा(! cur->parent->पर्णम्_flag);

	/* Create a new leaf to hold half the data from the old leaf. */
      संरचना वृक्ष_ग्रन्थिः*new_पर्णम् = नूतन संरचना वृक्ष_ग्रन्थिः;
      new_पर्णम्->पर्णम्_flag = सत्य;
      new_पर्णम्->गणनम् = cur->गणनम् / 2;
      new_पर्णम्->parent = cur->parent;

	/* Move the last half of the data from the end of the old leaf
	   to the beginning of the new leaf. At the same time, reduce
	   the size of the old leaf. */
      अचिह्नित idx1 = new_पर्णम्->गणनम्;
      अचिह्नित idx2 = cur->गणनम्;
      यावत् (idx1 > 0) {
	    idx1 -= 1;
	    idx2 -= 1;
	    new_पर्णम्->पर्णम्[idx1] = cur->पर्णम्[idx2];
	    cur->गणनम् -= 1;
      }

      प्रतिज्ञा(new_पर्णम्->गणनम् > 0);
      प्रतिज्ञा(cur->गणनम् > 0);

      अचिह्नित idx = 0;
      यावत् (cur->parent->child[idx] != cur) {
	    प्रतिज्ञा(idx < cur->parent->गणनम्);
	    idx += 1;
      }

      idx += 1;

      प्रति (अचिह्नित tmp = cur->parent->गणनम् ;  tmp > idx ;  tmp -= 1)
	    cur->parent->child[tmp] = cur->parent->child[tmp-1];

      cur->parent->child[idx] = new_पर्णम्;
      cur->parent->गणनम् += 1;

      यदि (cur->parent->गणनम् == ग्रन्थिः_विस्तारः)
	    विभज_ग्रन्थिः(cur->parent);

      प्रत्यागम new_पर्णम्;
}


/*
 * This function searches tree recursively for the key. If the value
 * is not found (and we are at a leaf) then set the key with the given
 * value. If the key is found, set the value only if the force_flag is
 * true.
 */

चिह्नम्_मानम्_t चिह्नम्_सारणी_s::find_मानम्(संरचना वृक्ष_ग्रन्थिः*cur,
					   ध्रुव वर्ण*कुञ्चिका, चिह्नम्_मानम्_t val,
					   तार्किक force_flag)
{
      यदि (cur->पर्णम्_flag) {

	    अचिह्नित idx = 0;
	    प्रति (;;) {
		    /* If we run out of keys in the leaf, then add
		       this at the end of the leaf. */
		  यदि (idx == cur->गणनम्) {
			cur->पर्णम्[idx].कुञ्चिका = कुञ्चिका_strdup(कुञ्चिका);
			cur->पर्णम्[idx].val = val;
			cur->गणनम् += 1;
			यदि (cur->गणनम् == पर्णम्_विस्तारः)
			      विभज_पर्णम्(cur);

			प्रत्यागम val;
		  }

		  पूर्णाङ्क rc = वाक्यं_तुलय(कुञ्चिका, cur->पर्णम्[idx].कुञ्चिका);

		    /* If we found the key already in the table, then
		       set the new value and break. */
		  यदि (rc == 0) {
			यदि (force_flag)
			      cur->पर्णम्[idx].val = val;
			प्रत्यागम cur->पर्णम्[idx].val;
		  }

		    /* If this key goes before the current key, then
		       push all the following entries back and add
		       this key here. */
		  यदि (rc < 0) {
			प्रति (अचिह्नित tmp = cur->गणनम्; tmp > idx; tmp -= 1)
			      cur->पर्णम्[tmp] = cur->पर्णम्[tmp-1];

			cur->पर्णम्[idx].कुञ्चिका = कुञ्चिका_strdup(कुञ्चिका);
			cur->पर्णम्[idx].val = val;
			cur->गणनम् += 1;
			यदि (cur->गणनम् == पर्णम्_विस्तारः)
			      विभज_पर्णम्(cur);

			प्रत्यागम val;
		  }

		  idx += 1;
	    }

      } अन्यथा {
	      /* Do a binary search within the inner node. */
	    अचिह्नित min = 0;
	    अचिह्नित max = cur->गणनम्;
	    अचिह्नित idx = max/2;
	    प्रति (;;) {
		  पूर्णाङ्क rc = वाक्यं_तुलय(कुञ्चिका, ग्रन्थिः_चरमः_कुञ्चिका(cur->child[idx]));
		  यदि (rc == 0) {
			प्रत्यागम find_मानम्(cur->child[idx],
					   कुञ्चिका, val, force_flag);
		  }

		  यदि (rc > 0) {
			min = idx + 1;
			यदि (min == cur->गणनम्)
			      प्रत्यागम find_मानम्(cur->child[idx],
						 कुञ्चिका, val, force_flag);
			यदि (min == max)
			      प्रत्यागम find_मानम्(cur->child[max],
						 कुञ्चिका, val, force_flag);

			idx = min + (max-min)/2;

		  } अन्यथा  {
			max = idx;
			यदि (idx == min)
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

शून्य चिह्नम्_सारणी_s::sym_समुच्चयः_मानम्(ध्रुव वर्ण*कुञ्चिका, चिह्नम्_मानम्_t val)
{
      यदि (मूलम्->गणनम् == 0) {
	      /* Handle the special case that this is the very first
		 value in the symbol table. Create the first leaf node
		 and initialize the pointers. */
	    संरचना वृक्ष_ग्रन्थिः*cur = नूतन संरचना वृक्ष_ग्रन्थिः;
	    cur->पर्णम्_flag = सत्य;
	    cur->parent = मूलम्;
	    cur->गणनम् = 1;
	    cur->पर्णम्[0].कुञ्चिका = कुञ्चिका_strdup(कुञ्चिका);
	    cur->पर्णम्[0].val = val;

	    मूलम्->गणनम् = 1;
	    मूलम्->child[0] = cur;
      } अन्यथा {
	    find_मानम्(मूलम्, कुञ्चिका, val, सत्य);
      }
}

चिह्नम्_मानम्_t चिह्नम्_सारणी_s::sym_get_मानम्(ध्रुव वर्ण*कुञ्चिका)
{
      चिह्नम्_मानम्_t def;
      def.ptr = 0;

      यदि (मूलम्->गणनम् == 0) {
	      /* Handle the special case that this is the very first
		 value in the symbol table. Create the first leaf node
		 and initialize the pointers. */
	    संरचना वृक्ष_ग्रन्थिः*cur = नूतन संरचना वृक्ष_ग्रन्थिः;
	    cur->पर्णम्_flag = सत्य;
	    cur->parent = मूलम्;
	    cur->गणनम् = 1;
	    cur->पर्णम्[0].कुञ्चिका = कुञ्चिका_strdup(कुञ्चिका);
	    cur->पर्णम्[0].val = def;

	    मूलम्->गणनम् = 1;
	    मूलम्->child[0] = cur;
	    प्रत्यागम cur->पर्णम्[0].val;
      } अन्यथा {
	    प्रत्यागम find_मानम्(मूलम्, कुञ्चिका, def, असत्य);
      }
}

चिह्नम्_सारणी_s::~चिह्नम्_सारणी_s()
{
      delete_चिह्नम्_ग्रन्थिः(मूलम्);
      यावत् (str_chunk) {
	    कुञ्चिका_वाक्यानि*tmp = str_chunk;
	    str_chunk = tmp->अग्र_2;
	    विलोपय tmp;
      }
}
