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
ਬਣਤਰ ਕੁੰਜੀ_ਲੜੀਆਂ {
      ਬਣਤਰ ਕੁੰਜੀ_ਲੜੀਆਂ*ਅਗਲਾ_2;
      ਅੱਖਰ ਅੰਕੜੇ[64*1024 - ਆਕਾਰ(ਬਣਤਰ ਕੁੰਜੀ_ਲੜੀਆਂ*)];
};

ਅੱਖਰ*ਚਿੰਨ੍ਹ_ਸਾਰਣੀ_s::ਕੁੰਜੀ_strdup(ਅਚਲ ਅੱਖਰ*str)
{
      ਬਿਨਾਨਿਸ਼ਾਨੀ len = ਲੜੀ_ਲੰਬਾਈ(str);
      ਪ੍ਰਤਿਜ੍ਞਾ( (len+1) <= ਆਕਾਰ str_chunk->ਅੰਕੜੇ );

      ਜੇ ( (len+1) > (ਆਕਾਰ str_chunk->ਅੰਕੜੇ - str_ਵਰਤਿਆ) ) {
	    ਕੁੰਜੀ_ਲੜੀਆਂ*tmp = ਨਵਾਂ ਕੁੰਜੀ_ਲੜੀਆਂ;
	    tmp->ਅਗਲਾ_2 = str_chunk;
	    str_chunk = tmp;
	    str_ਵਰਤਿਆ = 0;
      }

      ਅੱਖਰ*res = str_chunk->ਅੰਕੜੇ + str_ਵਰਤਿਆ;
      str_ਵਰਤਿਆ += len + 1;
      strcpy(res, str);
      ਵਾਪਸ res;
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

ਅਚਲ ਬਿਨਾਨਿਸ਼ਾਨੀ ਪੱਤੀ_ਚੌੜਾਈ = 254;
ਅਚਲ ਬਿਨਾਨਿਸ਼ਾਨੀ ਗੰਢ_ਚੌੜਾਈ = 508;

ਬਣਤਰ ਰੁੱਖ_ਅੰਕੜੇ {
      ਅੱਖਰ*ਕੁੰਜੀ;
      ਚਿੰਨ੍ਹ_ਮੁੱਲ_t val;
};

ਬਣਤਰ ਰੁੱਖ_ਗੰਢ {
      ਤਾਰਕਿਕ ਪੱਤੀ_flag;
      ਬਿਨਾਨਿਸ਼ਾਨੀ ਗਿਣਤੀ_2;
      ਬਣਤਰ ਰੁੱਖ_ਗੰਢ*ਮਾਪਾ;

      ਸੰਘ {
	    ਬਣਤਰ ਰੁੱਖ_ਅੰਕੜੇ ਪੱਤੀ[ਪੱਤੀ_ਚੌੜਾਈ];
	    ਬਣਤਰ ਰੁੱਖ_ਗੰਢ *ਬੱਚਾ[ਗੰਢ_ਚੌੜਾਈ];
      };

};

ਸਥਿਰ ਸਤਰਅੰਦਰ ਅੱਖਰ* ਗੰਢ_ਆਖਰੀ_ਕੁੰਜੀ(ਬਣਤਰ ਰੁੱਖ_ਗੰਢ*ਗੰਢ)
{
      ਜਦੋਂ (ਗੰਢ->ਪੱਤੀ_flag == ਝੂਠ)
	    ਗੰਢ = ਗੰਢ->ਬੱਚਾ[ਗੰਢ->ਗਿਣਤੀ_2-1];

      ਵਾਪਸ ਗੰਢ->ਪੱਤੀ[ਗੰਢ->ਗਿਣਤੀ_2-1].ਕੁੰਜੀ;
}

/*
 * Allocate a new symbol table means creating the table structure and
 * a root node, and initializing the pointers and members of the root
 * node.
 */
ਚਿੰਨ੍ਹ_ਸਾਰਣੀ_s::ਚਿੰਨ੍ਹ_ਸਾਰਣੀ_s()
{
      ਜੜ੍ਹ = ਨਵਾਂ ਬਣਤਰ ਰੁੱਖ_ਗੰਢ;
      ਜੜ੍ਹ->ਪੱਤੀ_flag = ਝੂਠ;
      ਜੜ੍ਹ->ਗਿਣਤੀ_2 = 0;
      ਜੜ੍ਹ->ਮਾਪਾ = 0;

      str_chunk = ਨਵਾਂ ਕੁੰਜੀ_ਲੜੀਆਂ;
      str_chunk->ਅਗਲਾ_2 = 0;
      str_ਵਰਤਿਆ = 0;
}

ਸਥਿਰ ਖਾਲੀ delete_ਚਿੰਨ੍ਹ_ਗੰਢ(ਬਣਤਰ ਰੁੱਖ_ਗੰਢ*cur)
{
      ਜੇ (! cur->ਪੱਤੀ_flag) {
	    ਲਈ (ਬਿਨਾਨਿਸ਼ਾਨੀ idx = 0 ;  idx < cur->ਗਿਣਤੀ_2 ;  idx += 1)
		  delete_ਚਿੰਨ੍ਹ_ਗੰਢ(cur->ਬੱਚਾ[idx]);
      }

      ਮਿਟਾਓ cur;
}

/* Do as split_leaf_ does, but for nodes. */
ਸਥਿਰ ਖਾਲੀ ਵੰਡੋ_ਗੰਢ(ਬਣਤਰ ਰੁੱਖ_ਗੰਢ*cur)
{
      ਪ੍ਰਤਿਜ੍ਞਾ(!cur->ਪੱਤੀ_flag);
      ਜੇ (cur->ਮਾਪਾ) ਪ੍ਰਤਿਜ੍ਞਾ(! cur->ਮਾਪਾ->ਪੱਤੀ_flag);

      ਜਦੋਂ (cur->ਗਿਣਤੀ_2 == ਗੰਢ_ਚੌੜਾਈ) {
	      /* Create a new node to hold half the data from cur. */
	    ਬਣਤਰ ਰੁੱਖ_ਗੰਢ *new_ਗੰਢ = ਨਵਾਂ ਬਣਤਰ ਰੁੱਖ_ਗੰਢ;
	    new_ਗੰਢ->ਪੱਤੀ_flag = ਝੂਠ;
	    new_ਗੰਢ->ਗਿਣਤੀ_2 = cur->ਗਿਣਤੀ_2 / 2;
	      /* cur is not root; new_node becomes sibling. */
	    ਜੇ (cur->ਮਾਪਾ) new_ਗੰਢ->ਮਾਪਾ = cur->ਮਾਪਾ;

	      /* Move the last half of the data from the end of the old node
	       * to the beginning of the new node. At the same time, reduce
	       * the size of the old node. */
	    ਬਿਨਾਨਿਸ਼ਾਨੀ idx1 = new_ਗੰਢ->ਗਿਣਤੀ_2;
	    ਬਿਨਾਨਿਸ਼ਾਨੀ idx2 = cur->ਗਿਣਤੀ_2;
	    ਜਦੋਂ (idx1 > 0) {
		  idx1 -= 1;
		  idx2 -= 1;
		  new_ਗੰਢ->ਬੱਚਾ[idx1] = cur->ਬੱਚਾ[idx2];
		  new_ਗੰਢ->ਬੱਚਾ[idx1]->ਮਾਪਾ = new_ਗੰਢ;
		  cur->ਗਿਣਤੀ_2 -= 1;
	    }

	    ਪ੍ਰਤਿਜ੍ਞਾ(new_ਗੰਢ->ਗਿਣਤੀ_2 > 0);
	    ਪ੍ਰਤਿਜ੍ਞਾ(cur->ਗਿਣਤੀ_2 > 0);

	    ਜੇ (cur->ਮਾਪਾ == 0) {
		    /* cur is root. Move first half of children to another
		     * new node, and put the two new nodes in cur. The plan
		     * here is to make cur into the new root and split its
		     * contents into 2 children. */

		  new_ਗੰਢ->ਮਾਪਾ = cur;
		  ਬਣਤਰ ਰੁੱਖ_ਗੰਢ*new2_ਗੰਢ = ਨਵਾਂ ਬਣਤਰ ਰੁੱਖ_ਗੰਢ;
		  new2_ਗੰਢ->ਪੱਤੀ_flag = ਝੂਠ;
		  new2_ਗੰਢ->ਗਿਣਤੀ_2 = cur->ਗਿਣਤੀ_2;
		  new2_ਗੰਢ->ਮਾਪਾ = cur;
		  ਲਈ (ਬਿਨਾਨਿਸ਼ਾਨੀ idx = 0; idx < cur->ਗਿਣਤੀ_2; idx += 1) {
			new2_ਗੰਢ->ਬੱਚਾ[idx] = cur->ਬੱਚਾ[idx];
			new2_ਗੰਢ->ਬੱਚਾ[idx]->ਮਾਪਾ = new2_ਗੰਢ;
		  }
		  cur->ਬੱਚਾ[0] = new2_ਗੰਢ;
		  cur->ਬੱਚਾ[1] = new_ਗੰਢ;
		  cur->ਗਿਣਤੀ_2 = 2;
		    /* no more ancestors, stop the while loop */
		  ਰੋਕੋ;
	    }

	      /* cur is not root. hook new_node to cur->parent. */
	    ਬਿਨਾਨਿਸ਼ਾਨੀ idx = 0;
	    ਜਦੋਂ (cur->ਮਾਪਾ->ਬੱਚਾ[idx] != cur) {
		  ਪ੍ਰਤਿਜ੍ਞਾ(idx < cur->ਮਾਪਾ->ਗਿਣਤੀ_2);
		  idx += 1;
	    }
	    idx += 1;

	    ਲਈ (ਬਿਨਾਨਿਸ਼ਾਨੀ tmp = cur->ਮਾਪਾ->ਗਿਣਤੀ_2 ;  tmp > idx ;  tmp -= 1) {
		  cur->ਮਾਪਾ->ਬੱਚਾ[tmp] = cur->ਮਾਪਾ->ਬੱਚਾ[tmp-1];
	    }

	    cur->ਮਾਪਾ->ਬੱਚਾ[idx] = new_ਗੰਢ;
	    cur->ਮਾਪਾ->ਗਿਣਤੀ_2 += 1;

	      /* check the ancestor */
	    cur = cur->ਮਾਪਾ;
      }
}

/*
 * This function takes a leaf node and splits in into two. Move half
 * the leaf keys into the new node, and add the new leaf into the
 * parent node.
 */
ਸਥਿਰ ਬਣਤਰ ਰੁੱਖ_ਗੰਢ* ਵੰਡੋ_ਪੱਤੀ(ਬਣਤਰ ਰੁੱਖ_ਗੰਢ*cur)
{
      ਪ੍ਰਤਿਜ੍ਞਾ(cur->ਪੱਤੀ_flag);
      ਪ੍ਰਤਿਜ੍ਞਾ(cur->ਮਾਪਾ);
      ਪ੍ਰਤਿਜ੍ਞਾ(! cur->ਮਾਪਾ->ਪੱਤੀ_flag);

	/* Create a new leaf to hold half the data from the old leaf. */
      ਬਣਤਰ ਰੁੱਖ_ਗੰਢ*new_ਪੱਤੀ = ਨਵਾਂ ਬਣਤਰ ਰੁੱਖ_ਗੰਢ;
      new_ਪੱਤੀ->ਪੱਤੀ_flag = ਸੱਚ;
      new_ਪੱਤੀ->ਗਿਣਤੀ_2 = cur->ਗਿਣਤੀ_2 / 2;
      new_ਪੱਤੀ->ਮਾਪਾ = cur->ਮਾਪਾ;

	/* Move the last half of the data from the end of the old leaf
	   to the beginning of the new leaf. At the same time, reduce
	   the size of the old leaf. */
      ਬਿਨਾਨਿਸ਼ਾਨੀ idx1 = new_ਪੱਤੀ->ਗਿਣਤੀ_2;
      ਬਿਨਾਨਿਸ਼ਾਨੀ idx2 = cur->ਗਿਣਤੀ_2;
      ਜਦੋਂ (idx1 > 0) {
	    idx1 -= 1;
	    idx2 -= 1;
	    new_ਪੱਤੀ->ਪੱਤੀ[idx1] = cur->ਪੱਤੀ[idx2];
	    cur->ਗਿਣਤੀ_2 -= 1;
      }

      ਪ੍ਰਤਿਜ੍ਞਾ(new_ਪੱਤੀ->ਗਿਣਤੀ_2 > 0);
      ਪ੍ਰਤਿਜ੍ਞਾ(cur->ਗਿਣਤੀ_2 > 0);

      ਬਿਨਾਨਿਸ਼ਾਨੀ idx = 0;
      ਜਦੋਂ (cur->ਮਾਪਾ->ਬੱਚਾ[idx] != cur) {
	    ਪ੍ਰਤਿਜ੍ਞਾ(idx < cur->ਮਾਪਾ->ਗਿਣਤੀ_2);
	    idx += 1;
      }

      idx += 1;

      ਲਈ (ਬਿਨਾਨਿਸ਼ਾਨੀ tmp = cur->ਮਾਪਾ->ਗਿਣਤੀ_2 ;  tmp > idx ;  tmp -= 1)
	    cur->ਮਾਪਾ->ਬੱਚਾ[tmp] = cur->ਮਾਪਾ->ਬੱਚਾ[tmp-1];

      cur->ਮਾਪਾ->ਬੱਚਾ[idx] = new_ਪੱਤੀ;
      cur->ਮਾਪਾ->ਗਿਣਤੀ_2 += 1;

      ਜੇ (cur->ਮਾਪਾ->ਗਿਣਤੀ_2 == ਗੰਢ_ਚੌੜਾਈ)
	    ਵੰਡੋ_ਗੰਢ(cur->ਮਾਪਾ);

      ਵਾਪਸ new_ਪੱਤੀ;
}


/*
 * This function searches tree recursively for the key. If the value
 * is not found (and we are at a leaf) then set the key with the given
 * value. If the key is found, set the value only if the force_flag is
 * true.
 */

ਚਿੰਨ੍ਹ_ਮੁੱਲ_t ਚਿੰਨ੍ਹ_ਸਾਰਣੀ_s::ਲੱਭੋ_ਮੁੱਲ(ਬਣਤਰ ਰੁੱਖ_ਗੰਢ*cur,
					   ਅਚਲ ਅੱਖਰ*ਕੁੰਜੀ, ਚਿੰਨ੍ਹ_ਮੁੱਲ_t val,
					   ਤਾਰਕਿਕ force_flag)
{
      ਜੇ (cur->ਪੱਤੀ_flag) {

	    ਬਿਨਾਨਿਸ਼ਾਨੀ idx = 0;
	    ਲਈ (;;) {
		    /* If we run out of keys in the leaf, then add
		       this at the end of the leaf. */
		  ਜੇ (idx == cur->ਗਿਣਤੀ_2) {
			cur->ਪੱਤੀ[idx].ਕੁੰਜੀ = ਕੁੰਜੀ_strdup(ਕੁੰਜੀ);
			cur->ਪੱਤੀ[idx].val = val;
			cur->ਗਿਣਤੀ_2 += 1;
			ਜੇ (cur->ਗਿਣਤੀ_2 == ਪੱਤੀ_ਚੌੜਾਈ)
			      ਵੰਡੋ_ਪੱਤੀ(cur);

			ਵਾਪਸ val;
		  }

		  ਪੂਰਨ rc = strcmp(ਕੁੰਜੀ, cur->ਪੱਤੀ[idx].ਕੁੰਜੀ);

		    /* If we found the key already in the table, then
		       set the new value and break. */
		  ਜੇ (rc == 0) {
			ਜੇ (force_flag)
			      cur->ਪੱਤੀ[idx].val = val;
			ਵਾਪਸ cur->ਪੱਤੀ[idx].val;
		  }

		    /* If this key goes before the current key, then
		       push all the following entries back and add
		       this key here. */
		  ਜੇ (rc < 0) {
			ਲਈ (ਬਿਨਾਨਿਸ਼ਾਨੀ tmp = cur->ਗਿਣਤੀ_2; tmp > idx; tmp -= 1)
			      cur->ਪੱਤੀ[tmp] = cur->ਪੱਤੀ[tmp-1];

			cur->ਪੱਤੀ[idx].ਕੁੰਜੀ = ਕੁੰਜੀ_strdup(ਕੁੰਜੀ);
			cur->ਪੱਤੀ[idx].val = val;
			cur->ਗਿਣਤੀ_2 += 1;
			ਜੇ (cur->ਗਿਣਤੀ_2 == ਪੱਤੀ_ਚੌੜਾਈ)
			      ਵੰਡੋ_ਪੱਤੀ(cur);

			ਵਾਪਸ val;
		  }

		  idx += 1;
	    }

      } ਨਹੀਂਤਾਂ {
	      /* Do a binary search within the inner node. */
	    ਬਿਨਾਨਿਸ਼ਾਨੀ min = 0;
	    ਬਿਨਾਨਿਸ਼ਾਨੀ max = cur->ਗਿਣਤੀ_2;
	    ਬਿਨਾਨਿਸ਼ਾਨੀ idx = max/2;
	    ਲਈ (;;) {
		  ਪੂਰਨ rc = strcmp(ਕੁੰਜੀ, ਗੰਢ_ਆਖਰੀ_ਕੁੰਜੀ(cur->ਬੱਚਾ[idx]));
		  ਜੇ (rc == 0) {
			ਵਾਪਸ ਲੱਭੋ_ਮੁੱਲ(cur->ਬੱਚਾ[idx],
					   ਕੁੰਜੀ, val, force_flag);
		  }

		  ਜੇ (rc > 0) {
			min = idx + 1;
			ਜੇ (min == cur->ਗਿਣਤੀ_2)
			      ਵਾਪਸ ਲੱਭੋ_ਮੁੱਲ(cur->ਬੱਚਾ[idx],
						 ਕੁੰਜੀ, val, force_flag);
			ਜੇ (min == max)
			      ਵਾਪਸ ਲੱਭੋ_ਮੁੱਲ(cur->ਬੱਚਾ[max],
						 ਕੁੰਜੀ, val, force_flag);

			idx = min + (max-min)/2;

		  } ਨਹੀਂਤਾਂ  {
			max = idx;
			ਜੇ (idx == min)
			      ਵਾਪਸ ਲੱਭੋ_ਮੁੱਲ(cur->ਬੱਚਾ[idx],
						 ਕੁੰਜੀ, val, force_flag);
			idx = min + (max-min)/2;
		  }
	    }
      }

      ਪ੍ਰਤਿਜ੍ਞਾ(0);
      { ਚਿੰਨ੍ਹ_ਮੁੱਲ_t tmp;
        tmp.ptr = 0;
        ਵਾਪਸ tmp;
      }
}

ਖਾਲੀ ਚਿੰਨ੍ਹ_ਸਾਰਣੀ_s::sym_ਸੈੱਟ_ਮੁੱਲ(ਅਚਲ ਅੱਖਰ*ਕੁੰਜੀ, ਚਿੰਨ੍ਹ_ਮੁੱਲ_t val)
{
      ਜੇ (ਜੜ੍ਹ->ਗਿਣਤੀ_2 == 0) {
	      /* Handle the special case that this is the very first
		 value in the symbol table. Create the first leaf node
		 and initialize the pointers. */
	    ਬਣਤਰ ਰੁੱਖ_ਗੰਢ*cur = ਨਵਾਂ ਬਣਤਰ ਰੁੱਖ_ਗੰਢ;
	    cur->ਪੱਤੀ_flag = ਸੱਚ;
	    cur->ਮਾਪਾ = ਜੜ੍ਹ;
	    cur->ਗਿਣਤੀ_2 = 1;
	    cur->ਪੱਤੀ[0].ਕੁੰਜੀ = ਕੁੰਜੀ_strdup(ਕੁੰਜੀ);
	    cur->ਪੱਤੀ[0].val = val;

	    ਜੜ੍ਹ->ਗਿਣਤੀ_2 = 1;
	    ਜੜ੍ਹ->ਬੱਚਾ[0] = cur;
      } ਨਹੀਂਤਾਂ {
	    ਲੱਭੋ_ਮੁੱਲ(ਜੜ੍ਹ, ਕੁੰਜੀ, val, ਸੱਚ);
      }
}

ਚਿੰਨ੍ਹ_ਮੁੱਲ_t ਚਿੰਨ੍ਹ_ਸਾਰਣੀ_s::sym_get_ਮੁੱਲ(ਅਚਲ ਅੱਖਰ*ਕੁੰਜੀ)
{
      ਚਿੰਨ੍ਹ_ਮੁੱਲ_t def;
      def.ptr = 0;

      ਜੇ (ਜੜ੍ਹ->ਗਿਣਤੀ_2 == 0) {
	      /* Handle the special case that this is the very first
		 value in the symbol table. Create the first leaf node
		 and initialize the pointers. */
	    ਬਣਤਰ ਰੁੱਖ_ਗੰਢ*cur = ਨਵਾਂ ਬਣਤਰ ਰੁੱਖ_ਗੰਢ;
	    cur->ਪੱਤੀ_flag = ਸੱਚ;
	    cur->ਮਾਪਾ = ਜੜ੍ਹ;
	    cur->ਗਿਣਤੀ_2 = 1;
	    cur->ਪੱਤੀ[0].ਕੁੰਜੀ = ਕੁੰਜੀ_strdup(ਕੁੰਜੀ);
	    cur->ਪੱਤੀ[0].val = def;

	    ਜੜ੍ਹ->ਗਿਣਤੀ_2 = 1;
	    ਜੜ੍ਹ->ਬੱਚਾ[0] = cur;
	    ਵਾਪਸ cur->ਪੱਤੀ[0].val;
      } ਨਹੀਂਤਾਂ {
	    ਵਾਪਸ ਲੱਭੋ_ਮੁੱਲ(ਜੜ੍ਹ, ਕੁੰਜੀ, def, ਝੂਠ);
      }
}

ਚਿੰਨ੍ਹ_ਸਾਰਣੀ_s::~ਚਿੰਨ੍ਹ_ਸਾਰਣੀ_s()
{
      delete_ਚਿੰਨ੍ਹ_ਗੰਢ(ਜੜ੍ਹ);
      ਜਦੋਂ (str_chunk) {
	    ਕੁੰਜੀ_ਲੜੀਆਂ*tmp = str_chunk;
	    str_chunk = tmp->ਅਗਲਾ_2;
	    ਮਿਟਾਓ tmp;
      }
}
