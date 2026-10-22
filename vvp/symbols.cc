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
بنية ڪنجي_لڙيون {
      بنية ڪنجي_لڙيون*ايندڙ_2;
      حرف معلومات[64*1024 - حجم(بنية ڪنجي_لڙيون*)];
};

حرف*نشان_جدول_s::ڪنجي_strdup(ثابت حرف*str)
{
      بنانشان len = لڙي_ڊيگهه(str);
      أكد( (len+1) <= حجم str_chunk->معلومات );

      إذا ( (len+1) > (حجم str_chunk->معلومات - str_استعمال_ٿيل) ) {
	    ڪنجي_لڙيون*tmp = جديد ڪنجي_لڙيون;
	    tmp->ايندڙ_2 = str_chunk;
	    str_chunk = tmp;
	    str_استعمال_ٿيل = 0;
      }

      حرف*res = str_chunk->معلومات + str_استعمال_ٿيل;
      str_استعمال_ٿيل += len + 1;
      strcpy(res, str);
      أرجع res;
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

ثابت بنانشان پن_ويڪر = 254;
ثابت بنانشان ڳنڍ_ويڪر = 508;

بنية وڻ_معلومات {
      حرف*ڪنجي;
      نشان_قدر_t val;
};

بنية وڻ_ڳنڍ {
      منطقي پن_flag;
      بنانشان ڳڻپ;
      بنية وڻ_ڳنڍ*والد;

      اتحاد {
	    بنية وڻ_معلومات پن[پن_ويڪر];
	    بنية وڻ_ڳنڍ *ٻار[ڳنڍ_ويڪر];
      };

};

ساكن مضمن حرف* ڳنڍ_آخري_ڪنجي(بنية وڻ_ڳنڍ*ڳنڍ)
{
      طالما (ڳنڍ->پن_flag == خطأ)
	    ڳنڍ = ڳنڍ->ٻار[ڳنڍ->ڳڻپ-1];

      أرجع ڳنڍ->پن[ڳنڍ->ڳڻپ-1].ڪنجي;
}

/*
 * Allocate a new symbol table means creating the table structure and
 * a root node, and initializing the pointers and members of the root
 * node.
 */
نشان_جدول_s::نشان_جدول_s()
{
      پاڙ = جديد بنية وڻ_ڳنڍ;
      پاڙ->پن_flag = خطأ;
      پاڙ->ڳڻپ = 0;
      پاڙ->والد = 0;

      str_chunk = جديد ڪنجي_لڙيون;
      str_chunk->ايندڙ_2 = 0;
      str_استعمال_ٿيل = 0;
}

ساكن فراغ delete_نشان_ڳنڍ(بنية وڻ_ڳنڍ*cur)
{
      إذا (! cur->پن_flag) {
	    لكل (بنانشان idx = 0 ;  idx < cur->ڳڻپ ;  idx += 1)
		  delete_نشان_ڳنڍ(cur->ٻار[idx]);
      }

      ميٽيو cur;
}

/* Do as split_leaf_ does, but for nodes. */
ساكن فراغ ورهايو_ڳنڍ(بنية وڻ_ڳنڍ*cur)
{
      أكد(!cur->پن_flag);
      إذا (cur->والد) أكد(! cur->والد->پن_flag);

      طالما (cur->ڳڻپ == ڳنڍ_ويڪر) {
	      /* Create a new node to hold half the data from cur. */
	    بنية وڻ_ڳنڍ *new_ڳنڍ = جديد بنية وڻ_ڳنڍ;
	    new_ڳنڍ->پن_flag = خطأ;
	    new_ڳنڍ->ڳڻپ = cur->ڳڻپ / 2;
	      /* cur is not root; new_node becomes sibling. */
	    إذا (cur->والد) new_ڳنڍ->والد = cur->والد;

	      /* Move the last half of the data from the end of the old node
	       * to the beginning of the new node. At the same time, reduce
	       * the size of the old node. */
	    بنانشان idx1 = new_ڳنڍ->ڳڻپ;
	    بنانشان idx2 = cur->ڳڻپ;
	    طالما (idx1 > 0) {
		  idx1 -= 1;
		  idx2 -= 1;
		  new_ڳنڍ->ٻار[idx1] = cur->ٻار[idx2];
		  new_ڳنڍ->ٻار[idx1]->والد = new_ڳنڍ;
		  cur->ڳڻپ -= 1;
	    }

	    أكد(new_ڳنڍ->ڳڻپ > 0);
	    أكد(cur->ڳڻپ > 0);

	    إذا (cur->والد == 0) {
		    /* cur is root. Move first half of children to another
		     * new node, and put the two new nodes in cur. The plan
		     * here is to make cur into the new root and split its
		     * contents into 2 children. */

		  new_ڳنڍ->والد = cur;
		  بنية وڻ_ڳنڍ*new2_ڳنڍ = جديد بنية وڻ_ڳنڍ;
		  new2_ڳنڍ->پن_flag = خطأ;
		  new2_ڳنڍ->ڳڻپ = cur->ڳڻپ;
		  new2_ڳنڍ->والد = cur;
		  لكل (بنانشان idx = 0; idx < cur->ڳڻپ; idx += 1) {
			new2_ڳنڍ->ٻار[idx] = cur->ٻار[idx];
			new2_ڳنڍ->ٻار[idx]->والد = new2_ڳنڍ;
		  }
		  cur->ٻار[0] = new2_ڳنڍ;
		  cur->ٻار[1] = new_ڳنڍ;
		  cur->ڳڻپ = 2;
		    /* no more ancestors, stop the while loop */
		  اكسر;
	    }

	      /* cur is not root. hook new_node to cur->parent. */
	    بنانشان idx = 0;
	    طالما (cur->والد->ٻار[idx] != cur) {
		  أكد(idx < cur->والد->ڳڻپ);
		  idx += 1;
	    }
	    idx += 1;

	    لكل (بنانشان tmp = cur->والد->ڳڻپ ;  tmp > idx ;  tmp -= 1) {
		  cur->والد->ٻار[tmp] = cur->والد->ٻار[tmp-1];
	    }

	    cur->والد->ٻار[idx] = new_ڳنڍ;
	    cur->والد->ڳڻپ += 1;

	      /* check the ancestor */
	    cur = cur->والد;
      }
}

/*
 * This function takes a leaf node and splits in into two. Move half
 * the leaf keys into the new node, and add the new leaf into the
 * parent node.
 */
ساكن بنية وڻ_ڳنڍ* ورهايو_پن(بنية وڻ_ڳنڍ*cur)
{
      أكد(cur->پن_flag);
      أكد(cur->والد);
      أكد(! cur->والد->پن_flag);

	/* Create a new leaf to hold half the data from the old leaf. */
      بنية وڻ_ڳنڍ*new_پن = جديد بنية وڻ_ڳنڍ;
      new_پن->پن_flag = سچ;
      new_پن->ڳڻپ = cur->ڳڻپ / 2;
      new_پن->والد = cur->والد;

	/* Move the last half of the data from the end of the old leaf
	   to the beginning of the new leaf. At the same time, reduce
	   the size of the old leaf. */
      بنانشان idx1 = new_پن->ڳڻپ;
      بنانشان idx2 = cur->ڳڻپ;
      طالما (idx1 > 0) {
	    idx1 -= 1;
	    idx2 -= 1;
	    new_پن->پن[idx1] = cur->پن[idx2];
	    cur->ڳڻپ -= 1;
      }

      أكد(new_پن->ڳڻپ > 0);
      أكد(cur->ڳڻپ > 0);

      بنانشان idx = 0;
      طالما (cur->والد->ٻار[idx] != cur) {
	    أكد(idx < cur->والد->ڳڻپ);
	    idx += 1;
      }

      idx += 1;

      لكل (بنانشان tmp = cur->والد->ڳڻپ ;  tmp > idx ;  tmp -= 1)
	    cur->والد->ٻار[tmp] = cur->والد->ٻار[tmp-1];

      cur->والد->ٻار[idx] = new_پن;
      cur->والد->ڳڻپ += 1;

      إذا (cur->والد->ڳڻپ == ڳنڍ_ويڪر)
	    ورهايو_ڳنڍ(cur->والد);

      أرجع new_پن;
}


/*
 * This function searches tree recursively for the key. If the value
 * is not found (and we are at a leaf) then set the key with the given
 * value. If the key is found, set the value only if the force_flag is
 * true.
 */

نشان_قدر_t نشان_جدول_s::ڳوليو_قدر(بنية وڻ_ڳنڍ*cur,
					   ثابت حرف*ڪنجي, نشان_قدر_t val,
					   منطقي force_flag)
{
      إذا (cur->پن_flag) {

	    بنانشان idx = 0;
	    لكل (;;) {
		    /* If we run out of keys in the leaf, then add
		       this at the end of the leaf. */
		  إذا (idx == cur->ڳڻپ) {
			cur->پن[idx].ڪنجي = ڪنجي_strdup(ڪنجي);
			cur->پن[idx].val = val;
			cur->ڳڻپ += 1;
			إذا (cur->ڳڻپ == پن_ويڪر)
			      ورهايو_پن(cur);

			أرجع val;
		  }

		  صحيح rc = strcmp(ڪنجي, cur->پن[idx].ڪنجي);

		    /* If we found the key already in the table, then
		       set the new value and break. */
		  إذا (rc == 0) {
			إذا (force_flag)
			      cur->پن[idx].val = val;
			أرجع cur->پن[idx].val;
		  }

		    /* If this key goes before the current key, then
		       push all the following entries back and add
		       this key here. */
		  إذا (rc < 0) {
			لكل (بنانشان tmp = cur->ڳڻپ; tmp > idx; tmp -= 1)
			      cur->پن[tmp] = cur->پن[tmp-1];

			cur->پن[idx].ڪنجي = ڪنجي_strdup(ڪنجي);
			cur->پن[idx].val = val;
			cur->ڳڻپ += 1;
			إذا (cur->ڳڻپ == پن_ويڪر)
			      ورهايو_پن(cur);

			أرجع val;
		  }

		  idx += 1;
	    }

      } وإلا {
	      /* Do a binary search within the inner node. */
	    بنانشان min = 0;
	    بنانشان max = cur->ڳڻپ;
	    بنانشان idx = max/2;
	    لكل (;;) {
		  صحيح rc = strcmp(ڪنجي, ڳنڍ_آخري_ڪنجي(cur->ٻار[idx]));
		  إذا (rc == 0) {
			أرجع ڳوليو_قدر(cur->ٻار[idx],
					   ڪنجي, val, force_flag);
		  }

		  إذا (rc > 0) {
			min = idx + 1;
			إذا (min == cur->ڳڻپ)
			      أرجع ڳوليو_قدر(cur->ٻار[idx],
						 ڪنجي, val, force_flag);
			إذا (min == max)
			      أرجع ڳوليو_قدر(cur->ٻار[max],
						 ڪنجي, val, force_flag);

			idx = min + (max-min)/2;

		  } وإلا  {
			max = idx;
			إذا (idx == min)
			      أرجع ڳوليو_قدر(cur->ٻار[idx],
						 ڪنجي, val, force_flag);
			idx = min + (max-min)/2;
		  }
	    }
      }

      أكد(0);
      { نشان_قدر_t tmp;
        tmp.ptr = 0;
        أرجع tmp;
      }
}

فراغ نشان_جدول_s::sym_سيٽ_قدر(ثابت حرف*ڪنجي, نشان_قدر_t val)
{
      إذا (پاڙ->ڳڻپ == 0) {
	      /* Handle the special case that this is the very first
		 value in the symbol table. Create the first leaf node
		 and initialize the pointers. */
	    بنية وڻ_ڳنڍ*cur = جديد بنية وڻ_ڳنڍ;
	    cur->پن_flag = سچ;
	    cur->والد = پاڙ;
	    cur->ڳڻپ = 1;
	    cur->پن[0].ڪنجي = ڪنجي_strdup(ڪنجي);
	    cur->پن[0].val = val;

	    پاڙ->ڳڻپ = 1;
	    پاڙ->ٻار[0] = cur;
      } وإلا {
	    ڳوليو_قدر(پاڙ, ڪنجي, val, سچ);
      }
}

نشان_قدر_t نشان_جدول_s::sym_get_قدر(ثابت حرف*ڪنجي)
{
      نشان_قدر_t def;
      def.ptr = 0;

      إذا (پاڙ->ڳڻپ == 0) {
	      /* Handle the special case that this is the very first
		 value in the symbol table. Create the first leaf node
		 and initialize the pointers. */
	    بنية وڻ_ڳنڍ*cur = جديد بنية وڻ_ڳنڍ;
	    cur->پن_flag = سچ;
	    cur->والد = پاڙ;
	    cur->ڳڻپ = 1;
	    cur->پن[0].ڪنجي = ڪنجي_strdup(ڪنجي);
	    cur->پن[0].val = def;

	    پاڙ->ڳڻپ = 1;
	    پاڙ->ٻار[0] = cur;
	    أرجع cur->پن[0].val;
      } وإلا {
	    أرجع ڳوليو_قدر(پاڙ, ڪنجي, def, خطأ);
      }
}

نشان_جدول_s::~نشان_جدول_s()
{
      delete_نشان_ڳنڍ(پاڙ);
      طالما (str_chunk) {
	    ڪنجي_لڙيون*tmp = str_chunk;
	    str_chunk = tmp->ايندڙ_2;
	    ميٽيو tmp;
      }
}
