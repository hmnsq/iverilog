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
ഘടന താക്കോൽ_strings {
      ഘടന താക്കോൽ_strings*next;
      അക്ഷരം data[64*1024 - വലുപ്പം(ഘടന താക്കോൽ_strings*)];
};

അക്ഷരം*ചിഹ്നം_table_s::താക്കോൽ_strdup(സ്ഥിരം അക്ഷരം*str)
{
      unsigned len = strlen(str);
      പ്രതിജ്ഞാ( (len+1) <= വലുപ്പം str_chunk->data );

      എങ്കിൽ ( (len+1) > (വലുപ്പം str_chunk->data - str_used) ) {
	    താക്കോൽ_strings*tmp = പുതിയ താക്കോൽ_strings;
	    tmp->next = str_chunk;
	    str_chunk = tmp;
	    str_used = 0;
      }

      അക്ഷരം*res = str_chunk->data + str_used;
      str_used += len + 1;
      strcpy(res, str);
      തിരികെ res;
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

സ്ഥിരം unsigned leaf_width = 254;
സ്ഥിരം unsigned കണ്ണി_width = 508;

ഘടന വൃക്ഷം_data {
      അക്ഷരം*താക്കോൽ;
      ചിഹ്നം_മൂല്യം_t val;
};

ഘടന വൃക്ഷ_കണ്ണി {
      തർക്കം leaf_flag;
      unsigned എണ്ണം;
      ഘടന വൃക്ഷ_കണ്ണി*parent;

      സംഘം {
	    ഘടന വൃക്ഷം_data leaf[leaf_width];
	    ഘടന വൃക്ഷ_കണ്ണി *child[കണ്ണി_width];
      };

};

സ്ഥിതം വരിയിൽ അക്ഷരം* കണ്ണി_last_താക്കോൽ(ഘടന വൃക്ഷ_കണ്ണി*കണ്ണി)
{
      ആകുമ്പോൾ (കണ്ണി->leaf_flag == അസത്യം)
	    കണ്ണി = കണ്ണി->child[കണ്ണി->എണ്ണം-1];

      തിരികെ കണ്ണി->leaf[കണ്ണി->എണ്ണം-1].താക്കോൽ;
}

/*
 * Allocate a new symbol table means creating the table structure and
 * a root node, and initializing the pointers and members of the root
 * node.
 */
ചിഹ്നം_table_s::ചിഹ്നം_table_s()
{
      വേര് = പുതിയ ഘടന വൃക്ഷ_കണ്ണി;
      വേര്->leaf_flag = അസത്യം;
      വേര്->എണ്ണം = 0;
      വേര്->parent = 0;

      str_chunk = പുതിയ താക്കോൽ_strings;
      str_chunk->next = 0;
      str_used = 0;
}

സ്ഥിതം ശൂന്യം delete_ചിഹ്നം_കണ്ണി(ഘടന വൃക്ഷ_കണ്ണി*cur)
{
      എങ്കിൽ (! cur->leaf_flag) {
	    വേണ്ടി (unsigned idx = 0 ;  idx < cur->എണ്ണം ;  idx += 1)
		  delete_ചിഹ്നം_കണ്ണി(cur->child[idx]);
      }

      delete cur;
}

/* Do as split_leaf_ does, but for nodes. */
സ്ഥിതം ശൂന്യം split_കണ്ണി(ഘടന വൃക്ഷ_കണ്ണി*cur)
{
      പ്രതിജ്ഞാ(!cur->leaf_flag);
      എങ്കിൽ (cur->parent) പ്രതിജ്ഞാ(! cur->parent->leaf_flag);

      ആകുമ്പോൾ (cur->എണ്ണം == കണ്ണി_width) {
	      /* Create a new node to hold half the data from cur. */
	    ഘടന വൃക്ഷ_കണ്ണി *new_കണ്ണി = പുതിയ ഘടന വൃക്ഷ_കണ്ണി;
	    new_കണ്ണി->leaf_flag = അസത്യം;
	    new_കണ്ണി->എണ്ണം = cur->എണ്ണം / 2;
	      /* cur is not root; new_node becomes sibling. */
	    എങ്കിൽ (cur->parent) new_കണ്ണി->parent = cur->parent;

	      /* Move the last half of the data from the end of the old node
	       * to the beginning of the new node. At the same time, reduce
	       * the size of the old node. */
	    unsigned idx1 = new_കണ്ണി->എണ്ണം;
	    unsigned idx2 = cur->എണ്ണം;
	    ആകുമ്പോൾ (idx1 > 0) {
		  idx1 -= 1;
		  idx2 -= 1;
		  new_കണ്ണി->child[idx1] = cur->child[idx2];
		  new_കണ്ണി->child[idx1]->parent = new_കണ്ണി;
		  cur->എണ്ണം -= 1;
	    }

	    പ്രതിജ്ഞാ(new_കണ്ണി->എണ്ണം > 0);
	    പ്രതിജ്ഞാ(cur->എണ്ണം > 0);

	    എങ്കിൽ (cur->parent == 0) {
		    /* cur is root. Move first half of children to another
		     * new node, and put the two new nodes in cur. The plan
		     * here is to make cur into the new root and split its
		     * contents into 2 children. */

		  new_കണ്ണി->parent = cur;
		  ഘടന വൃക്ഷ_കണ്ണി*new2_കണ്ണി = പുതിയ ഘടന വൃക്ഷ_കണ്ണി;
		  new2_കണ്ണി->leaf_flag = അസത്യം;
		  new2_കണ്ണി->എണ്ണം = cur->എണ്ണം;
		  new2_കണ്ണി->parent = cur;
		  വേണ്ടി (unsigned idx = 0; idx < cur->എണ്ണം; idx += 1) {
			new2_കണ്ണി->child[idx] = cur->child[idx];
			new2_കണ്ണി->child[idx]->parent = new2_കണ്ണി;
		  }
		  cur->child[0] = new2_കണ്ണി;
		  cur->child[1] = new_കണ്ണി;
		  cur->എണ്ണം = 2;
		    /* no more ancestors, stop the while loop */
		  നിർത്തുക;
	    }

	      /* cur is not root. hook new_node to cur->parent. */
	    unsigned idx = 0;
	    ആകുമ്പോൾ (cur->parent->child[idx] != cur) {
		  പ്രതിജ്ഞാ(idx < cur->parent->എണ്ണം);
		  idx += 1;
	    }
	    idx += 1;

	    വേണ്ടി (unsigned tmp = cur->parent->എണ്ണം ;  tmp > idx ;  tmp -= 1) {
		  cur->parent->child[tmp] = cur->parent->child[tmp-1];
	    }

	    cur->parent->child[idx] = new_കണ്ണി;
	    cur->parent->എണ്ണം += 1;

	      /* check the ancestor */
	    cur = cur->parent;
      }
}

/*
 * This function takes a leaf node and splits in into two. Move half
 * the leaf keys into the new node, and add the new leaf into the
 * parent node.
 */
സ്ഥിതം ഘടന വൃക്ഷ_കണ്ണി* split_leaf_(ഘടന വൃക്ഷ_കണ്ണി*cur)
{
      പ്രതിജ്ഞാ(cur->leaf_flag);
      പ്രതിജ്ഞാ(cur->parent);
      പ്രതിജ്ഞാ(! cur->parent->leaf_flag);

	/* Create a new leaf to hold half the data from the old leaf. */
      ഘടന വൃക്ഷ_കണ്ണി*new_leaf = പുതിയ ഘടന വൃക്ഷ_കണ്ണി;
      new_leaf->leaf_flag = സത്യം;
      new_leaf->എണ്ണം = cur->എണ്ണം / 2;
      new_leaf->parent = cur->parent;

	/* Move the last half of the data from the end of the old leaf
	   to the beginning of the new leaf. At the same time, reduce
	   the size of the old leaf. */
      unsigned idx1 = new_leaf->എണ്ണം;
      unsigned idx2 = cur->എണ്ണം;
      ആകുമ്പോൾ (idx1 > 0) {
	    idx1 -= 1;
	    idx2 -= 1;
	    new_leaf->leaf[idx1] = cur->leaf[idx2];
	    cur->എണ്ണം -= 1;
      }

      പ്രതിജ്ഞാ(new_leaf->എണ്ണം > 0);
      പ്രതിജ്ഞാ(cur->എണ്ണം > 0);

      unsigned idx = 0;
      ആകുമ്പോൾ (cur->parent->child[idx] != cur) {
	    പ്രതിജ്ഞാ(idx < cur->parent->എണ്ണം);
	    idx += 1;
      }

      idx += 1;

      വേണ്ടി (unsigned tmp = cur->parent->എണ്ണം ;  tmp > idx ;  tmp -= 1)
	    cur->parent->child[tmp] = cur->parent->child[tmp-1];

      cur->parent->child[idx] = new_leaf;
      cur->parent->എണ്ണം += 1;

      എങ്കിൽ (cur->parent->എണ്ണം == കണ്ണി_width)
	    split_കണ്ണി(cur->parent);

      തിരികെ new_leaf;
}


/*
 * This function searches tree recursively for the key. If the value
 * is not found (and we are at a leaf) then set the key with the given
 * value. If the key is found, set the value only if the force_flag is
 * true.
 */

ചിഹ്നം_മൂല്യം_t ചിഹ്നം_table_s::find_മൂല്യം(ഘടന വൃക്ഷ_കണ്ണി*cur,
					   സ്ഥിരം അക്ഷരം*താക്കോൽ, ചിഹ്നം_മൂല്യം_t val,
					   തർക്കം force_flag)
{
      എങ്കിൽ (cur->leaf_flag) {

	    unsigned idx = 0;
	    വേണ്ടി (;;) {
		    /* If we run out of keys in the leaf, then add
		       this at the end of the leaf. */
		  എങ്കിൽ (idx == cur->എണ്ണം) {
			cur->leaf[idx].താക്കോൽ = താക്കോൽ_strdup(താക്കോൽ);
			cur->leaf[idx].val = val;
			cur->എണ്ണം += 1;
			എങ്കിൽ (cur->എണ്ണം == leaf_width)
			      split_leaf_(cur);

			തിരികെ val;
		  }

		  പൂർണ്ണസംഖ്യ rc = strcmp(താക്കോൽ, cur->leaf[idx].താക്കോൽ);

		    /* If we found the key already in the table, then
		       set the new value and break. */
		  എങ്കിൽ (rc == 0) {
			എങ്കിൽ (force_flag)
			      cur->leaf[idx].val = val;
			തിരികെ cur->leaf[idx].val;
		  }

		    /* If this key goes before the current key, then
		       push all the following entries back and add
		       this key here. */
		  എങ്കിൽ (rc < 0) {
			വേണ്ടി (unsigned tmp = cur->എണ്ണം; tmp > idx; tmp -= 1)
			      cur->leaf[tmp] = cur->leaf[tmp-1];

			cur->leaf[idx].താക്കോൽ = താക്കോൽ_strdup(താക്കോൽ);
			cur->leaf[idx].val = val;
			cur->എണ്ണം += 1;
			എങ്കിൽ (cur->എണ്ണം == leaf_width)
			      split_leaf_(cur);

			തിരികെ val;
		  }

		  idx += 1;
	    }

      } അല്ലെങ്കിൽ {
	      /* Do a binary search within the inner node. */
	    unsigned min = 0;
	    unsigned max = cur->എണ്ണം;
	    unsigned idx = max/2;
	    വേണ്ടി (;;) {
		  പൂർണ്ണസംഖ്യ rc = strcmp(താക്കോൽ, കണ്ണി_last_താക്കോൽ(cur->child[idx]));
		  എങ്കിൽ (rc == 0) {
			തിരികെ find_മൂല്യം(cur->child[idx],
					   താക്കോൽ, val, force_flag);
		  }

		  എങ്കിൽ (rc > 0) {
			min = idx + 1;
			എങ്കിൽ (min == cur->എണ്ണം)
			      തിരികെ find_മൂല്യം(cur->child[idx],
						 താക്കോൽ, val, force_flag);
			എങ്കിൽ (min == max)
			      തിരികെ find_മൂല്യം(cur->child[max],
						 താക്കോൽ, val, force_flag);

			idx = min + (max-min)/2;

		  } അല്ലെങ്കിൽ  {
			max = idx;
			എങ്കിൽ (idx == min)
			      തിരികെ find_മൂല്യം(cur->child[idx],
						 താക്കോൽ, val, force_flag);
			idx = min + (max-min)/2;
		  }
	    }
      }

      പ്രതിജ്ഞാ(0);
      { ചിഹ്നം_മൂല്യം_t tmp;
        tmp.ptr = 0;
        തിരികെ tmp;
      }
}

ശൂന്യം ചിഹ്നം_table_s::sym_set_മൂല്യം(സ്ഥിരം അക്ഷരം*താക്കോൽ, ചിഹ്നം_മൂല്യം_t val)
{
      എങ്കിൽ (വേര്->എണ്ണം == 0) {
	      /* Handle the special case that this is the very first
		 value in the symbol table. Create the first leaf node
		 and initialize the pointers. */
	    ഘടന വൃക്ഷ_കണ്ണി*cur = പുതിയ ഘടന വൃക്ഷ_കണ്ണി;
	    cur->leaf_flag = സത്യം;
	    cur->parent = വേര്;
	    cur->എണ്ണം = 1;
	    cur->leaf[0].താക്കോൽ = താക്കോൽ_strdup(താക്കോൽ);
	    cur->leaf[0].val = val;

	    വേര്->എണ്ണം = 1;
	    വേര്->child[0] = cur;
      } അല്ലെങ്കിൽ {
	    find_മൂല്യം(വേര്, താക്കോൽ, val, സത്യം);
      }
}

ചിഹ്നം_മൂല്യം_t ചിഹ്നം_table_s::sym_get_മൂല്യം(സ്ഥിരം അക്ഷരം*താക്കോൽ)
{
      ചിഹ്നം_മൂല്യം_t def;
      def.ptr = 0;

      എങ്കിൽ (വേര്->എണ്ണം == 0) {
	      /* Handle the special case that this is the very first
		 value in the symbol table. Create the first leaf node
		 and initialize the pointers. */
	    ഘടന വൃക്ഷ_കണ്ണി*cur = പുതിയ ഘടന വൃക്ഷ_കണ്ണി;
	    cur->leaf_flag = സത്യം;
	    cur->parent = വേര്;
	    cur->എണ്ണം = 1;
	    cur->leaf[0].താക്കോൽ = താക്കോൽ_strdup(താക്കോൽ);
	    cur->leaf[0].val = def;

	    വേര്->എണ്ണം = 1;
	    വേര്->child[0] = cur;
	    തിരികെ cur->leaf[0].val;
      } അല്ലെങ്കിൽ {
	    തിരികെ find_മൂല്യം(വേര്, താക്കോൽ, def, അസത്യം);
      }
}

ചിഹ്നം_table_s::~ചിഹ്നം_table_s()
{
      delete_ചിഹ്നം_കണ്ണി(വേര്);
      ആകുമ്പോൾ (str_chunk) {
	    താക്കോൽ_strings*tmp = str_chunk;
	    str_chunk = tmp->next;
	    delete tmp;
      }
}
