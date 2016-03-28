#include "sf_include.h"




/***********************************************************************
* 函数名称： SingleLinkAppend()
* 功能描述： 将指定节点附加到单向链表的尾部
* 访问的表：
* 修改的表：
* 输入参数:     Item：插入的节点
                Tail：单向链表的尾地址
* 输出参数：    Root：单向链表的首地址
                Tail：单向链表的尾地址
* 返 回 值：    插入的节点
* 其它说明：
* 修改日期    版本号     修改人      修改内容
*************************************************************************/
void* SingleLinkAppend(void **Root, void **Tail, void *Item)
{
    t_single_link *item     = (t_single_link *)Item;
    t_single_link **root    = (t_single_link **)Root;
    t_single_link **tail	= (t_single_link **)Tail;

    item->next = 0;

    if ((*tail))
    {
        (*tail)->next = item;
        *tail         = item;
    }
    else
    {
        *root = *tail = item;
    }

    return (void*)item;
}

/***********************************************************************
* 函数名称： SingleLinkNext()
* 功能描述： 将单向链表的首节点从链表中移出
* 访问的表：
* 修改的表：
* 输入参数:     Root：单向链表的首地址
                Tail：单向链表的尾地址
* 输出参数：    Root：单向链表的首地址
                Tail：单向链表的尾地址
* 返 回 值：    从队列移出来的节点
* 其它说明：
* 修改日期    版本号     修改人      修改内容
*************************************************************************/
void*  SingleLinkNext(void **Root, void **Tail)
{
    t_single_link *item;
    t_single_link **root = (t_single_link **)Root;
    t_single_link **tail = (t_single_link **)Tail;

    item = *root;

    if (item)
    {
        *root = item->next;
        if(! (*root))
        {
            *tail = 0;
        }
    }

    return (void* )item;
}

/***********************************************************************
* 函数名称： DualLinkPush()
* 功能描述： 将指定节点压入双向链表构成的堆栈中
* 访问的表：
* 修改的表：
* 输入参数:     Root：单向链表的首地址
                Item：增加的节点
* 输出参数：    Root：单向链表的首地址
* 返 回 值：    加入到双向链表中的节点
* 其它说明：
* 修改日期    版本号     修改人      修改内容
*************************************************************************/
void*  DualLinkPush(void **Root, void *Item)
{
    DualLinkAppend(Root, Item);

    *Root = Item; /* modified by sgt for 2007-08-08 for CTRCR00316548 */

    return (void* )Item;
}

/***********************************************************************
* 函数名称： DualLinkAppend()
* 功能描述： 将指定节点附加到双向链表的尾部
* 访问的表：
* 修改的表：
* 输入参数:     Root：单向链表的首地址
                Item：插入的节点
* 输出参数：    Root：单向链表的首地址
* 返 回 值：    插入的节点
* 其它说明：
* 修改日期    版本号     修改人      修改内容
*************************************************************************/
void*  DualLinkAppend(void **Root, void *Item)
{
    t_dual_link *item   = (t_dual_link *)Item;
    t_dual_link **root  = (t_dual_link **)Root;

    if (*root)
    {
        item->next = *root;
        item->prev = (*root)->prev;

        (*root)->prev->next = item;
        (*root)->prev       = item;
    }
    else
    {
        item->next  = item;
        item->prev  = item;
        *root       = item;
    }

    return (void* )item;
}

/***********************************************************************
* 函数名称： DualLinkRemove()
* 功能描述： 将指定节点从双向链表中移出
* 访问的表：
* 修改的表：
* 输入参数:     Root：单向链表的首地址
                Item：移出的节点
* 输出参数：    Root：单向链表的首地址
* 返 回 值：    从队列移出来的节点
* 其它说明：
* 修改日期    版本号     修改人      修改内容
*************************************************************************/
void* DualLinkRemove(void **Root, void *Item)
{
    t_dual_link *item   = (t_dual_link *)Item;
    t_dual_link **root  = (t_dual_link **)Root;

    item->prev->next = item->next;
    item->next->prev = item->prev;

    if (*root == item)
    {
        *root = item->next;
        if (*root == item)
        {
            *root = 0;
        }
    }

    return (void* )item;
}

