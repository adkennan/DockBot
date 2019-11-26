/************************************
**
**  DockBot - A Dock For AmigaOS 3
**
**  © 2019 Andrew Kennan
**
************************************/

#include "prefs.h"

#include <clib/utility_protos.h>

struct TagItem *make_tag_list(ULONG data, ...)
{
    struct TagItem *tags = (struct TagItem *)&data;

    return CloneTagItems(tags);
}

ULONG count_tags(struct TagItem *list)
{
    ULONG c = 0;
    struct TagItem *tstate, *tag;

    if( list == NULL ) {
        return 0;
    }

    tstate = list;
    while( tag = NextTagItem(&tstate) ) {
        c++;
    }
    return c;
}

struct TagItem *copy_tags(struct TagItem *dest, struct TagItem *src)
{
    struct TagItem *tstate, *tag;

    tstate = src;
    while( tag = NextTagItem(&tstate) ) {
        dest->ti_Tag = tag->ti_Tag;
        dest->ti_Data = tag->ti_Data;
        dest++;
    }
    return dest;
}

struct TagItem *merge_tag_lists(struct TagItem *head, struct TagItem *middle, struct TagItem *tail)
{
    struct TagItem *newTags, *dest;
    ULONG tagCount = count_tags(head) + count_tags(middle) + count_tags(tail);

    if( newTags = AllocateTagItems(tagCount) ) {

        dest = newTags;
        dest = copy_tags(dest, head);
        if( middle != NULL ) {
            dest = copy_tags(dest, middle);
        }
        dest = copy_tags(dest, tail);

        dest->ti_Tag = TAG_END;
    }

    return newTags;
}

