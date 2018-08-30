/**Memory allocation (3a)*/

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "stdio.h"
#include "stdlib.h"
#define MAPSIZE 100

// 链表结点
struct map
{
    int m_addr;
    int m_size;
};

struct map map[MAPSIZE];

/*BF 最佳适应法分配函数*/
int BF_malloc(struct map* mp, int size)
{
    register int a, s;
    register struct map* bp, * bpp;
    for (bp = mp; bp->m_size; bp++)
    {
        // 找到第一个满足条件的 map 结点
        if (bp->m_size >= size)
        {
            a = bp->m_addr;
            s = bp->m_size;
            // 最佳适应法: 从前一个结点开始，找能满足分配的 bpp->m_size >= size，且 size 最小的 map 结点
            for (bpp = bp; bpp->m_size; bpp++)
            {
                // 最佳适应法
                if (bpp->m_size >= size && bpp->m_size < s)
                {
                    a = bpp->m_addr;
                    s = bpp->m_size;
                    bp = bpp;
                }
            }

            // 找到后分配，修改分配后的 bp 块的起始地址
            bp->m_addr += size;
            // 该块 bp 已分配完，无可用空间，将他指向下一 bp 所指内容
            if ((bp->m_size -= size) == 0)
                do
                {
                    bp++;
                    (bp - 1)->m_addr = bp->m_addr;
                } while((bp - 1)->m_size = bp->m_size);
            return a;   // 返回分配空间的地址
        }
    }
    return -1;
}

int WF_malloc(struct map* mp, int size)
{
    register int a, s;
    register struct map* bp, * bpp;
    for (bp = mp; bp->m_size; bp++)
    {
        // 找到第一个满足条件的 map 结点
        if (bp->m_size >= size)
        {
            a = bp->m_addr;
            s = bp->m_size;
            // 最坏适应法: 从前一个结点开始，找能满足分配的 bpp->m_size >= size，且 size 最大的 map 结点
            for (bpp = bp; bpp->m_size; bpp++)
            {
                // 最坏适应法
                if (bpp->m_size > s)
                {
                    a = bpp->m_addr;
                    s = bpp->m_size;
                    bp = bpp;
                }
            }

            // 找到后分配，修改分配后的 bp 块的起始地址
            bp->m_addr += size;
            // 该块 bp 已分配完，无可用空间，将他指向下一 bp 所指内容
            if ((bp->m_size -= size) == 0)
                do
                {
                    bp++;
                    (bp - 1)->m_addr = bp->m_addr;
                } while((bp - 1)->m_size = bp->m_size);
            return a;   // 返回分配空间的地址
        }
    }
    return -1;
}

/*存储释放*/
void mfree(struct map* mp, int aa, int size)
{
    register struct map* bp;
    register int t;
    register int a;

    a = aa;

    for (bp = mp; bp->m_addr <= a && bp->m_size != 0; bp++);    // 找到第一个 size 大于 0 且地址在待释放地址之前的 bp 块 ?
    if (bp > mp && (bp - 1)->m_addr + (bp - 1)->m_size == a)
    {
        // 与前合并
        (bp - 1)->m_size += size;
        if (a + size == bp->m_addr) // 前后可以连接在一起
        {
            // 前后合并
            (bp - 1)->m_size += bp->m_size;
            while (bp->m_size)
            {
                // 后向前移
                bp++;
                (bp - 1)->m_addr = bp->m_addr;
                (bp - 1)->m_size = bp->m_size;
            }
        }
    }
    else
    {
        if (a + size == bp->m_addr && bp->m_size)
        {
            // 不能与前合并，但能与后合并
            bp->m_addr -= size;
            bp->m_size += size;
        }
        else if (size)
            // 不能与前或后合并
            do
            {
                t = bp->m_addr;
                bp->m_addr = a;
                a = t;
                t = bp->m_size;
                bp->m_size = size;
                bp++;
            } while (size = t);
    }
}

void init()
{
    struct map* bp;
    int addr, size;
    int i = 0;
    bp = map;
    printf("Please input starting addr and total size: ");
    scanf("%d,%d", &addr, &size);
    bp->m_addr = addr;
    bp->m_size = size;
    (++bp)->m_size = 0; // 表尾以 size 为 0 标记
}

void show_map()
{
    int i = 0;
    system("clear");
    struct map* bp;
    bp = map;
    printf("\n Current memory map...\n");
    printf("Address\t\tSize\n");
    while (bp->m_size != 0)
    {
        printf("<%d\t\t%d>\n", bp->m_addr, bp->m_size);
        bp++;
    }
    printf("\n");
}

int main(int argc, char** argv)
{
    int a, s;
    int c, i;
    init();
    printf("Please input, b for BF, w for WF: ");
    scanf("%c", &c);
    do
    {
        show_map();
        printf("Please input, 1 for request, 2 for release, 0 for exit:");
        scanf("%d", &c);
        switch(c)
        {
            case 1:
                printf("Please input size: ");
                scanf("%d", &s);
                if (c == 'b')
                    a = BF_malloc(map, s);
                else
                    a = BF_malloc(map, s);
                if (a == -1)
                    printf("request cannot be satisfied\n");
                else
                    printf("alloc memory at address: %d, size:%d", a, s);
                break;
            case 2:
                printf("Please input addr and size: ");
                scanf("%d,%d", &a, &s);
                mfree(map, a, s);
                break;
            case 0:
                exit(0);
        }
    } while(1);
}
