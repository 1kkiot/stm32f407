/*
 * File:        alloc.c
 * Purpose:     generic malloc() and free() engine
 *
 * Notes:       99% of this code stolen/borrowed from the K&R C
 *              examples.
 *
 */
#include <stdarg.h>
#include <stdio.h>
#include "stdlib.h"

#include "stm32f4xx.h"
#include "wujique_log.h"
#include "alloc.h"

/*
ʹ�ñ���������Ķ���Ϊ�ڴ�أ�ע�⣬ֱ��ʹ��malloc�������C��ĺ�����ʹ�õľ��Ƕѣ�
Ҫ��ֹ��ͻ
*/
//#define ALLOC_USE_HEAP	//�ö����ڴ��
#define ALLOC_USE_ARRAY		//�ö�����������ڴ��

#ifdef ALLOC_USE_HEAP
#pragma section = "HEAP"
#endif

#ifdef ALLOC_USE_ARRAY
#define AllocArraySize (70*1024)

__align(4)//��֤�ڴ�����ֽڶ���
char AllocArray[AllocArraySize];
#endif

/********************************************************************/

/*
 * This struct forms the minimum block size which is allocated, and
 * also forms the linked list for the memory space used with alloc()
 * and free().  It is padded so that on a 32-bit machine, all malloc'ed
 * pointers are 16-byte aligned.
 */
 /*
	���ڴ���䷽��������Ľṹ������������ṹ�塣
	��ÿһ������ڴ��ͷ������һ����
	����ṹ��size��¼�˱����ڴ�Ĵ�С,
	ptr�����ӵ���һ������ڴ档
	�����ڴ�ʱ����һ������ڴ��и���Ҫ���ڴ��ȥ��

*/
typedef struct ALLOC_HDR
{
  struct
  {
    struct ALLOC_HDR *ptr;
    unsigned int size;/*�����ڴ�����*/
  } s;
  unsigned int align;
  unsigned int pad;
} ALLOC_HDR;

static ALLOC_HDR base;/*�����ڴ�����ͷ���*/
static ALLOC_HDR *freep = NULL;

/********************************************************************/
void wjq_free_t( void *ap )
{
  ALLOC_HDR *bp, *p;

	/* ��õ��ж��ǲ���Ӧ���ж��ڶѷ�Χ�ڣ�*/
	if(ap == NULL)
		return;

	/* ���������ap�ǿ�ʹ���ڴ��ָ�룬��ǰ��һ���ṹ��λ�ã�
		Ҳ���������bp�����Ǽ�¼�ڴ���Ϣ��λ��*/
  bp = (ALLOC_HDR *) ap - 1; /* point to block header */

  /*
  	�ҵ���Ҫ�ͷŵ��ڴ��ǰ����п�
  	��ʵ���ǱȽ��ڴ��λ�õĴ�С
  */
  for ( p = freep; !( ( bp > p ) && ( bp < p->s.ptr ) ); p = p->s.ptr )
  {
    if ( ( p >= p->s.ptr ) && ( ( bp > p ) || ( bp < p->s.ptr ) ) )
    {
      break; /* freed block at start or end of arena */
    }
  }

	/*�ж��Ƿ��ܸ�һ����ϲ����ܺϲ��ͺϲ������ܺϲ���������������*/
  if ( ( bp + bp->s.size ) == p->s.ptr )
  {
    bp->s.size += p->s.ptr->s.size;
    bp->s.ptr = p->s.ptr->s.ptr;
  }
  else
  {
    bp->s.ptr = p->s.ptr;
  }
	/*ͬ���������һ��Ĺ�ϵ*/
  if ( ( p + p->s.size ) == bp )
  {
    p->s.size += bp->s.size;
    p->s.ptr = bp->s.ptr;
  }
  else
  {
    p->s.ptr = bp;
  }

  freep = p;

}

/********************************************************************/
void* wjq_malloc_t( unsigned nbytes )
{
  /* Get addresses for the HEAP start and end */
#ifdef ALLOC_USE_HEAP
  char* __HEAP_START = __section_begin("HEAP");
  char* __HEAP_END = __section_end("HEAP");
#endif

#ifdef ALLOC_USE_ARRAY
  char* __HEAP_START = AllocArray;
  char* __HEAP_END = __HEAP_START+AllocArraySize;
#endif

  ALLOC_HDR *p, *prevp;
  unsigned nunits;

	/*����Ҫ������ڴ����*/
  nunits = ( ( nbytes + sizeof(ALLOC_HDR) - 1 ) / sizeof(ALLOC_HDR) ) + 1;

/*��һ��ʹ��malloc���ڴ�����û�н���
  ��ʼ������*/
  if ( ( prevp = freep ) == NULL )
  {
    p = (ALLOC_HDR *) __HEAP_START;
    p->s.size = ( ( (uint32_t) __HEAP_END - (uint32_t) __HEAP_START )
      / sizeof(ALLOC_HDR) );
    p->s.ptr = &base;
    base.s.ptr = p;
    base.s.size = 0;
    prevp = freep = &base;
	/*������ʼ����ֻ��һ����п�*/
  }

	/*��ѯ�������Һ��ʿ�*/
  for ( p = prevp->s.ptr;; prevp = p, p = p->s.ptr )
  {
    if ( p->s.size >= nunits )
    {
      if ( p->s.size == nunits )
      {
        prevp->s.ptr = p->s.ptr;
      }
      else
      {
        p->s.size -= nunits;
        p += p->s.size;
        p->s.size = nunits;
      }
      freep = prevp;
	  /*���ؿ����ڴ�ָ����û���
	  �����ڴ�Ҫ��ȥ�ڴ�����ṹ��*/
      return (void *) ( p + 1 );
    }

	/*����ʧ��*/
    if ( p == freep )
    {
			#if 1
    	while(1)
    	{
    		/*����Ƕ��ʽ��˵��û�л��������ڴ棬��ˣ��������ڴ����ʧ��*/
    		wjq_log(LOG_ERR, "wujique malloc err!!\r\n");
    	}
			#else
      return NULL;
			#endif
    }
	
  }
}


void *wjq_malloc_m(unsigned nbytes)
{   
    void * p;
    
    p = wjq_malloc_t(nbytes);

    return p;
}

void wjq_free_m (void *ap)
{

    if(ap == NULL)
        return;
    wjq_free_t(ap);
}

/*
	������δ����
*/
void *wjq_calloc(size_t n, size_t size)
{
	void *p;
	p = wjq_malloc_m(n*size);
	if(p != NULL)
	{
		memset((char*)p, 0, n*size);
	}

	return p;
}


void wjq_malloc_test(void)
{
	char* p;
	
	p = (char *)wjq_malloc(1024);
	/*��ӡָ�룬�����ǲ���4�ֽڶ���*/
	wjq_log(LOG_FUN, "pointer :%08x\r\n", p);
	
	memset(p, 0xf0, 1024);
	wjq_log(LOG_FUN, "data:%02x\r\n", *(p+1023));
	wjq_log(LOG_FUN, "data:%02x\r\n", *(p+1024));
	
	wjq_free(p);
	wjq_log(LOG_FUN, "alloc free ok\r\n");
	
	while(1);
}


/***************************** end ***************************************/

