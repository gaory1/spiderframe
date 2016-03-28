#ifndef _LINK_MODULE_
#define _LINK_MODULE_

extern void*  SingleLinkAppend(void **Root, void **Tail, void *Item);
extern void*  SingleLinkNext(void **Root, void **Tail);

extern void*  DualLinkPush(void **Root, void *Item);
extern void*  DualLinkAppend(void **Root, void *Item);
extern void*  DualLinkRemove(void **Root, void *Item);

typedef struct _tagSingleLink {
    struct _tagSingleLink *next;
} t_single_link;
typedef struct _tagDualLink {
    struct _tagDualLink *prev, *next;
} t_dual_link;

#endif
