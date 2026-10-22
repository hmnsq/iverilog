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
काष्ठा कुंजी_strings {
      काष्ठा कुंजी_strings*अग्र_2;
      अक्षर तथ्य[64*1024 - माप(काष्ठा कुंजी_strings*)];
};

अक्षर*प्रतीक_table_s::कुंजी_strdup(स्थिर अक्षर*str)
{
      अचिन्हित len = म_माप(str);
      निश्चित( (len+1) <= माप str_chunk->तथ्य );

      अगर ( (len+1) > (माप str_chunk->तथ्य - str_used) ) {
	    कुंजी_strings*tmp = नया कुंजी_strings;
	    tmp->अग्र_2 = str_chunk;
	    str_chunk = tmp;
	    str_used = 0;
      }

      अक्षर*res = str_chunk->तथ्य + str_used;
      str_used += len + 1;
      म_नकल(res, str);
      वापस res;
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

स्थिर अचिन्हित leaf_width = 254;
स्थिर अचिन्हित गाँठ_width = 508;

काष्ठा वृक्ष_तथ्य {
      अक्षर*कुंजी;
      प्रतीक_मूल्य_t val;
};

काष्ठा वृक्ष_गाँठ {
      bool leaf_flag;
      अचिन्हित गिनती;
      काष्ठा वृक्ष_गाँठ*parent;

      जोड़ {
	    काष्ठा वृक्ष_तथ्य leaf[leaf_width];
	    काष्ठा वृक्ष_गाँठ *child[गाँठ_width];
      };

};

अटल अंतरभूत अक्षर* गाँठ_last_कुंजी(काष्ठा वृक्ष_गाँठ*गाँठ)
{
      जबतक (गाँठ->leaf_flag == false)
	    गाँठ = गाँठ->child[गाँठ->गिनती-1];

      वापस गाँठ->leaf[गाँठ->गिनती-1].कुंजी;
}

/*
 * Allocate a new symbol table means creating the table structure and
 * a root node, and initializing the pointers and members of the root
 * node.
 */
प्रतीक_table_s::प्रतीक_table_s()
{
      मूल = नया काष्ठा वृक्ष_गाँठ;
      मूल->leaf_flag = false;
      मूल->गिनती = 0;
      मूल->parent = 0;

      str_chunk = नया कुंजी_strings;
      str_chunk->अग्र_2 = 0;
      str_used = 0;
}

अटल व्योम delete_प्रतीक_गाँठ(काष्ठा वृक्ष_गाँठ*cur)
{
      अगर (! cur->leaf_flag) {
	    क्रम (अचिन्हित idx = 0 ;  idx < cur->गिनती ;  idx += 1)
		  delete_प्रतीक_गाँठ(cur->child[idx]);
      }

      मिटाओ cur;
}

/* Do as split_leaf_ does, but for nodes. */
अटल व्योम split_गाँठ(काष्ठा वृक्ष_गाँठ*cur)
{
      निश्चित(!cur->leaf_flag);
      अगर (cur->parent) निश्चित(! cur->parent->leaf_flag);

      जबतक (cur->गिनती == गाँठ_width) {
	      /* Create a new node to hold half the data from cur. */
	    काष्ठा वृक्ष_गाँठ *new_गाँठ = नया काष्ठा वृक्ष_गाँठ;
	    new_गाँठ->leaf_flag = false;
	    new_गाँठ->गिनती = cur->गिनती / 2;
	      /* cur is not root; new_node becomes sibling. */
	    अगर (cur->parent) new_गाँठ->parent = cur->parent;

	      /* Move the last half of the data from the end of the old node
	       * to the beginning of the new node. At the same time, reduce
	       * the size of the old node. */
	    अचिन्हित idx1 = new_गाँठ->गिनती;
	    अचिन्हित idx2 = cur->गिनती;
	    जबतक (idx1 > 0) {
		  idx1 -= 1;
		  idx2 -= 1;
		  new_गाँठ->child[idx1] = cur->child[idx2];
		  new_गाँठ->child[idx1]->parent = new_गाँठ;
		  cur->गिनती -= 1;
	    }

	    निश्चित(new_गाँठ->गिनती > 0);
	    निश्चित(cur->गिनती > 0);

	    अगर (cur->parent == 0) {
		    /* cur is root. Move first half of children to another
		     * new node, and put the two new nodes in cur. The plan
		     * here is to make cur into the new root and split its
		     * contents into 2 children. */

		  new_गाँठ->parent = cur;
		  काष्ठा वृक्ष_गाँठ*new2_गाँठ = नया काष्ठा वृक्ष_गाँठ;
		  new2_गाँठ->leaf_flag = false;
		  new2_गाँठ->गिनती = cur->गिनती;
		  new2_गाँठ->parent = cur;
		  क्रम (अचिन्हित idx = 0; idx < cur->गिनती; idx += 1) {
			new2_गाँठ->child[idx] = cur->child[idx];
			new2_गाँठ->child[idx]->parent = new2_गाँठ;
		  }
		  cur->child[0] = new2_गाँठ;
		  cur->child[1] = new_गाँठ;
		  cur->गिनती = 2;
		    /* no more ancestors, stop the while loop */
		  अवरोध;
	    }

	      /* cur is not root. hook new_node to cur->parent. */
	    अचिन्हित idx = 0;
	    जबतक (cur->parent->child[idx] != cur) {
		  निश्चित(idx < cur->parent->गिनती);
		  idx += 1;
	    }
	    idx += 1;

	    क्रम (अचिन्हित tmp = cur->parent->गिनती ;  tmp > idx ;  tmp -= 1) {
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
अटल काष्ठा वृक्ष_गाँठ* split_leaf_(काष्ठा वृक्ष_गाँठ*cur)
{
      निश्चित(cur->leaf_flag);
      निश्चित(cur->parent);
      निश्चित(! cur->parent->leaf_flag);

	/* Create a new leaf to hold half the data from the old leaf. */
      काष्ठा वृक्ष_गाँठ*new_leaf = नया काष्ठा वृक्ष_गाँठ;
      new_leaf->leaf_flag = true;
      new_leaf->गिनती = cur->गिनती / 2;
      new_leaf->parent = cur->parent;

	/* Move the last half of the data from the end of the old leaf
	   to the beginning of the new leaf. At the same time, reduce
	   the size of the old leaf. */
      अचिन्हित idx1 = new_leaf->गिनती;
      अचिन्हित idx2 = cur->गिनती;
      जबतक (idx1 > 0) {
	    idx1 -= 1;
	    idx2 -= 1;
	    new_leaf->leaf[idx1] = cur->leaf[idx2];
	    cur->गिनती -= 1;
      }

      निश्चित(new_leaf->गिनती > 0);
      निश्चित(cur->गिनती > 0);

      अचिन्हित idx = 0;
      जबतक (cur->parent->child[idx] != cur) {
	    निश्चित(idx < cur->parent->गिनती);
	    idx += 1;
      }

      idx += 1;

      क्रम (अचिन्हित tmp = cur->parent->गिनती ;  tmp > idx ;  tmp -= 1)
	    cur->parent->child[tmp] = cur->parent->child[tmp-1];

      cur->parent->child[idx] = new_leaf;
      cur->parent->गिनती += 1;

      अगर (cur->parent->गिनती == गाँठ_width)
	    split_गाँठ(cur->parent);

      वापस new_leaf;
}


/*
 * This function searches tree recursively for the key. If the value
 * is not found (and we are at a leaf) then set the key with the given
 * value. If the key is found, set the value only if the force_flag is
 * true.
 */

प्रतीक_मूल्य_t प्रतीक_table_s::find_मूल्य(काष्ठा वृक्ष_गाँठ*cur,
					   स्थिर अक्षर*कुंजी, प्रतीक_मूल्य_t val,
					   bool force_flag)
{
      अगर (cur->leaf_flag) {

	    अचिन्हित idx = 0;
	    क्रम (;;) {
		    /* If we run out of keys in the leaf, then add
		       this at the end of the leaf. */
		  अगर (idx == cur->गिनती) {
			cur->leaf[idx].कुंजी = कुंजी_strdup(कुंजी);
			cur->leaf[idx].val = val;
			cur->गिनती += 1;
			अगर (cur->गिनती == leaf_width)
			      split_leaf_(cur);

			वापस val;
		  }

		  पूर्णांक rc = म_भेद(कुंजी, cur->leaf[idx].कुंजी);

		    /* If we found the key already in the table, then
		       set the new value and break. */
		  अगर (rc == 0) {
			अगर (force_flag)
			      cur->leaf[idx].val = val;
			वापस cur->leaf[idx].val;
		  }

		    /* If this key goes before the current key, then
		       push all the following entries back and add
		       this key here. */
		  अगर (rc < 0) {
			क्रम (अचिन्हित tmp = cur->गिनती; tmp > idx; tmp -= 1)
			      cur->leaf[tmp] = cur->leaf[tmp-1];

			cur->leaf[idx].कुंजी = कुंजी_strdup(कुंजी);
			cur->leaf[idx].val = val;
			cur->गिनती += 1;
			अगर (cur->गिनती == leaf_width)
			      split_leaf_(cur);

			वापस val;
		  }

		  idx += 1;
	    }

      } अन्यथा {
	      /* Do a binary search within the inner node. */
	    अचिन्हित min = 0;
	    अचिन्हित max = cur->गिनती;
	    अचिन्हित idx = max/2;
	    क्रम (;;) {
		  पूर्णांक rc = म_भेद(कुंजी, गाँठ_last_कुंजी(cur->child[idx]));
		  अगर (rc == 0) {
			वापस find_मूल्य(cur->child[idx],
					   कुंजी, val, force_flag);
		  }

		  अगर (rc > 0) {
			min = idx + 1;
			अगर (min == cur->गिनती)
			      वापस find_मूल्य(cur->child[idx],
						 कुंजी, val, force_flag);
			अगर (min == max)
			      वापस find_मूल्य(cur->child[max],
						 कुंजी, val, force_flag);

			idx = min + (max-min)/2;

		  } अन्यथा  {
			max = idx;
			अगर (idx == min)
			      वापस find_मूल्य(cur->child[idx],
						 कुंजी, val, force_flag);
			idx = min + (max-min)/2;
		  }
	    }
      }

      निश्चित(0);
      { प्रतीक_मूल्य_t tmp;
        tmp.ptr = 0;
        वापस tmp;
      }
}

व्योम प्रतीक_table_s::sym_set_मूल्य(स्थिर अक्षर*कुंजी, प्रतीक_मूल्य_t val)
{
      अगर (मूल->गिनती == 0) {
	      /* Handle the special case that this is the very first
		 value in the symbol table. Create the first leaf node
		 and initialize the pointers. */
	    काष्ठा वृक्ष_गाँठ*cur = नया काष्ठा वृक्ष_गाँठ;
	    cur->leaf_flag = true;
	    cur->parent = मूल;
	    cur->गिनती = 1;
	    cur->leaf[0].कुंजी = कुंजी_strdup(कुंजी);
	    cur->leaf[0].val = val;

	    मूल->गिनती = 1;
	    मूल->child[0] = cur;
      } अन्यथा {
	    find_मूल्य(मूल, कुंजी, val, true);
      }
}

प्रतीक_मूल्य_t प्रतीक_table_s::sym_get_मूल्य(स्थिर अक्षर*कुंजी)
{
      प्रतीक_मूल्य_t def;
      def.ptr = 0;

      अगर (मूल->गिनती == 0) {
	      /* Handle the special case that this is the very first
		 value in the symbol table. Create the first leaf node
		 and initialize the pointers. */
	    काष्ठा वृक्ष_गाँठ*cur = नया काष्ठा वृक्ष_गाँठ;
	    cur->leaf_flag = true;
	    cur->parent = मूल;
	    cur->गिनती = 1;
	    cur->leaf[0].कुंजी = कुंजी_strdup(कुंजी);
	    cur->leaf[0].val = def;

	    मूल->गिनती = 1;
	    मूल->child[0] = cur;
	    वापस cur->leaf[0].val;
      } अन्यथा {
	    वापस find_मूल्य(मूल, कुंजी, def, false);
      }
}

प्रतीक_table_s::~प्रतीक_table_s()
{
      delete_प्रतीक_गाँठ(मूल);
      जबतक (str_chunk) {
	    कुंजी_strings*tmp = str_chunk;
	    str_chunk = tmp->अग्र_2;
	    मिटाओ tmp;
      }
}
