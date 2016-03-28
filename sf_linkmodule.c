#include "sf_include.h"




/***********************************************************************
* �������ƣ� SingleLinkAppend()
* ���������� ��ָ���ڵ㸽�ӵ����������β��
* ���ʵı�
* �޸ĵı�
* �������:     Item������Ľڵ�
                Tail�����������β��ַ
* ���������    Root������������׵�ַ
                Tail�����������β��ַ
* �� �� ֵ��    ����Ľڵ�
* ����˵����
* �޸�����    �汾��     �޸���      �޸�����
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
* �������ƣ� SingleLinkNext()
* ���������� ������������׽ڵ���������Ƴ�
* ���ʵı�
* �޸ĵı�
* �������:     Root������������׵�ַ
                Tail�����������β��ַ
* ���������    Root������������׵�ַ
                Tail�����������β��ַ
* �� �� ֵ��    �Ӷ����Ƴ����Ľڵ�
* ����˵����
* �޸�����    �汾��     �޸���      �޸�����
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
* �������ƣ� DualLinkPush()
* ���������� ��ָ���ڵ�ѹ��˫�������ɵĶ�ջ��
* ���ʵı�
* �޸ĵı�
* �������:     Root������������׵�ַ
                Item�����ӵĽڵ�
* ���������    Root������������׵�ַ
* �� �� ֵ��    ���뵽˫�������еĽڵ�
* ����˵����
* �޸�����    �汾��     �޸���      �޸�����
*************************************************************************/
void*  DualLinkPush(void **Root, void *Item)
{
    DualLinkAppend(Root, Item);

    *Root = Item; /* modified by sgt for 2007-08-08 for CTRCR00316548 */

    return (void* )Item;
}

/***********************************************************************
* �������ƣ� DualLinkAppend()
* ���������� ��ָ���ڵ㸽�ӵ�˫�������β��
* ���ʵı�
* �޸ĵı�
* �������:     Root������������׵�ַ
                Item������Ľڵ�
* ���������    Root������������׵�ַ
* �� �� ֵ��    ����Ľڵ�
* ����˵����
* �޸�����    �汾��     �޸���      �޸�����
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
* �������ƣ� DualLinkRemove()
* ���������� ��ָ���ڵ��˫���������Ƴ�
* ���ʵı�
* �޸ĵı�
* �������:     Root������������׵�ַ
                Item���Ƴ��Ľڵ�
* ���������    Root������������׵�ַ
* �� �� ֵ��    �Ӷ����Ƴ����Ľڵ�
* ����˵����
* �޸�����    �汾��     �޸���      �޸�����
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

