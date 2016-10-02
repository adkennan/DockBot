
#include "prefs.h"

#include <clib/utility_protos.h>

struct TagItem *make_tag_list(ULONG data, ...)
{
    struct TagItem *tags = (struct TagItem *)&data;

    return CloneTagItems(tags);
}
