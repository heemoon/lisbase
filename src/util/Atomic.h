//
// Created by 58 on 2016/4/18.
//

#ifndef __LISBASE_UTIL_ATOMIC_H__
#define __LISBASE_UTIL_ATOMIC_H__


/************************************************************************
 *  实现Linux2.6.18后版本中的同步原语,因为系统选择的是2.6.18版本,
 *  所以暂时先只对这个进行处理,如果之后有需要,再实现汇编版本
 ************************************************************************/

namespace lisbase {

typedef struct {
	volatile int atomic;
} atomic_t;

typedef struct {
	volatile long long atomic;
} atomic64_t;

#define atomic_read(x) ((x)->atomic)
#define atomic_set(x, val) ((x)->atomic = (val))
#define atomic_inc(x) ( __sync_fetch_and_add (&(x)->atomic, 1))
#define atomic_dec(x) ( __sync_fetch_and_sub  (&(x)->atomic, 1))
#define atomic_inc_and_test(x) (__sync_fetch_and_add (&(x)->atomic, 1) == 1)
#define atomic_dec_and_test(x) (__sync_fetch_and_sub (&(x)->atomic, 1) == 1)
#define atomic_add(x, v) ( __sync_add_and_fetch(&(x)->atomic, (v)))
#define atomic_sub(x, v) ( __sync_sub_and_fetch(&(x)->atomic, (v)))
#define atomic_cmpxchg(x, oldv, newv) __sync_val_compare_and_swap(&(x)->atomic, (oldv), (newv))
#define atomic_init(a,v) atomic_set(a,v)
#define atomic_cas(x, oldv, newv) __sync_bool_compare_and_swap(&(x)->atomic, (oldv), (newv))
#define CAS(x, oldv, newv) __sync_bool_compare_and_swap(x, (oldv), (newv))


//class AtomicBool {
//public:
//	AtomicBool();
//	~AtomicBool();
//public:
//	inline int get();
//	inline void set();
//	inline BOOL cas(int oldv, int newv);
//private:
//	atomic_t m_nVal;
//};
//
//class AtomicInteger {
//public:
//	AtomicInteger();
//	~AtomicInteger();
//public:
//	inline int get();
//	inline void set();
//	inline int inc();
//	inline int dec();
//	inline int add(int val);
//	inline int sub(int val);
//	inline BOOL cas(int oldv, int newv);
//private:
//	atomic_t m_nVal;
//};
//
//class AtomicLong {
//public:
//	AtomicLong();
//	~AtomicLong();
//public:
//	inline I64 get();
//	inline void set();
//	inline I64 inc();
//	inline I64 dec();
//	inline I64 add(I64 val);
//	inline I64 sub(I64 val);
//	inline BOOL cas(I64 oldv, I64 newv);
//private:
//	atomic64_t m_nVal;
//};
}

#endif //WREDISCLIENT_ATOMIC_H

