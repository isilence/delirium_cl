/*
 *  https://gist.github.com/ISilence/52b41a53fc0f0cbb538f
 */

__kernel void gramF_slow(
        __global const float * __restrict const        vs,
        __global float * __restrict const            output,
        //const int                                     n,
        const int                                     vecSz)
{
    const int idy = get_global_id(1);
    const int idx = get_global_id(0);
    const int n = get_global_size(0);

    __global const float * const xs = &vs[idx * vecSz];
    __global const float * const ys = &vs[idy * vecSz];

    float res = 0.0f;
    for (int i=0; i<vecSz; ++i)
        res += xs[i] * ys[i];
    output[idy * n + idx] = res;
}

// ================================================

// preferred device arithmetic vector width
#if !defined(DLM_ARCH_VECSZ_CHAR)
    #define DLM_ARCH_VECSZ_CHAR 4
#endif
#if !defined(DLM_ARCH_VECSZ_SHORT)
    #define DLM_ARCH_VECSZ_SHORT 1
#endif
#if !defined(DLM_ARCH_VECSZ_INT)
    #define DLM_ARCH_VECSZ_INT 1
#endif
#if !defined(DLM_ARCH_VECSZ_LONG)
    #define DLM_ARCH_VECSZ_LONG 1
#endif
#if !defined(DLM_ARCH_VECSZ_FLOAT)
    #define DLM_ARCH_VECSZ_FLOAT 1
#endif
#if !defined(DLM_ARCH_VECSZ_DOUBLE)
    #define DLM_ARCH_VECSZ_DOUBLE 1
#endif

// useful macroses
#define DLM_QUOTE_(str) #str
#define DLM_QUOTE(str) DLM_QUOTE_(str)
#define DLM_PRIMITIVE_CONCAT(str1, str2) str1 ## str2
#define DLM_CONCAT(str1, str2) DLM_PRIMITIVE_CONCAT(str1, str2)

// compiler hints
#if  __OPENCL_VERSION__ >= 210
    #define DLM_UNROLL __attribute__((opencl_unroll_hint))
    #define DLM_UNROLLN(n) __attribute__((opencl_unroll_hint( n )))
    #define DLM_NOSVM __attribute__((nosvm))
#else
    #define DLM_UNROLL
    #define DLM_UNROLLN(n)
    #define DLM_NOSVM
#endif

#if defined(DLM_COMPILE_WITHOUT_SVM)
    #define DLM_STAT_SVM DLM_NO_SVM
#else
    #define DLM_STAT_SVM
#endif


// datatype generalization
typedef char char1;
typedef uchar uchar1;
typedef short short1;
typedef ushort ushort1;
typedef int int1;
typedef uint uint1;
typedef long long1;
typedef ulong ulong1;
typedef float float1;
#if defined(cl_khr_fp64)
    typedef double double1;
#endif


// memory operations generalizaiton
#define vload1(offset, ptr) ((ptr)[offset])
#define vloadN(N, offset, ptr) DLM_CONCAT(vload, N)((offset), (ptr))
#define vloadSN(N, ptr) vloadN(N, 0, ptr)

#define vstore1(data, offset, ptr) (ptr)[offset] = (data)
#define vstoreN(N, data, offset, ptr) DLM_CONCAT(vstore, N)((data), (offset), (ptr))
#define vstoreSN(N, data, ptr) vstoreN(N, data, 0, ptr)

#define vtransferN(N, loadOffset, loadPtr, storeOffset, storePtr) \
    vstoreN(N, \
            vloadN(N, (loadOffset), (loadPtr)), \
            (storeOffset), (storePtr) \
    )
#define vtransferSN(N, loadPtr, storePtr) vtransferN(N, 0, loadPtr, 0, storePtr)

// work-group foos
#if (__OPENCL_VERSION__ - 0 >= 210)
	inline size_t dlm_loc_lin_id(void) {
        return get_local_linear_id();
    }
    inline size_t dlm_glob_lin_id(void) {
        return get_global_linear_id();
    }
#else
    inline size_t dlm_loc_lin_id(void) {
        return get_local_id(1) * get_local_size(0) + get_local_id(0);
    }
    inline size_t dlm_glob_lin_id(void) {
        return get_global_id(1) * get_global_size(0) + get_global_id(0);
    }
#endif
inline size_t dlm_group_offset(const uint dimindx) {
    return get_group_id(dimindx) * get_local_size(dimindx);
}

// vector type folding
inline float sumVec1(const float v) {
    return v;
}
inline float sumVec2(const float2 v) {
    return v.x + v.y;
}
inline float sumVec4(const float4 v) {
    return dot(v, (float4)1.0f);
}
inline float sumVec8(const float8 v) {
    return sumVec4(v.hi + v.lo);
}
inline float sumVec16(const float16 v) {
    return sumVec8(v.hi + v.lo);
}
#define sumVecN(N, v) DLM_CONCAT(sumVec, N)((v))



// ================================================
//        gram preferences

#if !defined(BLOCK_SIDEX)
    #define BLOCK_SIDEX 8
#endif
#if !defined(BLOCK_SIDEY)
    #define BLOCK_SIDEY 8
#endif
#if (BLOCK_SIDEY == BLOCK_SIDEX)
    #define BLOCK_SIDE BLOCK_SIDEX
#endif

#define N DLM_ARCH_VECSZ_FLOAT
#define M 1
#define BLOCK_SIZE (BLOCK_SIDEX * BLOCK_SIDEY)
#define DATA_WIDTH (M * BLOCK_SIZE)

typedef DLM_CONCAT(float, N) floatN;
#define STRIDE (DATA_WIDTH + 1)

// Gram matrix computation block algorithm
// group size: (BLOCK_SIDEX, BLOCK_SIDEY, 1)
// global size: should match with gram matrix size (n * n)
//
// params:
// n - implicit argument, equal get_global_id(i), i = {1,2}.
//     number of vectors in input array & size of gram matrix
// vs - array of input vectors (n * vecSz). Vectors stored in memory sequentially
// output - gram matrix (n * n)
// vecSz - length of each input vector
//
//    algorithm basics
// 1. each work-item computes appropriate element of gram matrix,
// 2. each group computes [BLOCK_SIDEX * BLOCK_SIDEY] block of gram matrix,
//    starts with group_global_offset (see dlm_group_offset) gram's element in each dimension
// 3. Outer loop:
//    work-item computes dot product of [DATA_WIDTH] elements each iteration
// 4. First inner loop:
//    whole group reads [DATA_WIDTH] chunk of vector each iteration.
//    After we have [BLOCK_SIDEX] & [BLOCK_SIDEY] chunks of vectors in local memory
//
__attribute__((vec_type_hint(floatN)))
__attribute__((work_group_size_hint(BLOCK_SIDEX, BLOCK_SIDEY, 1)))
__kernel void computeGramMrx2(
    DLM_STAT_SVM __global const float * __restrict const vs,
    DLM_STAT_SVM __global float * __restrict const output,
    const int vecSz)
{
    __local float xss[BLOCK_SIDEX][DATA_WIDTH + 1]; // avoid bank conflicts!
    __local float yss[BLOCK_SIDEY][DATA_WIDTH + 1];

    const uint localId = dlm_loc_lin_id() * M;
    const uint xBlockShift = dlm_group_offset(0) * vecSz + localId;
    const uint yBlockShift = dlm_group_offset(1) * vecSz + localId;

    floatN s = (floatN) 0.0f;
	for (uint layer=0; layer < vecSz; layer += DATA_WIDTH) {

		for (uint i=0, chunkShift = layer; i < BLOCK_SIDEX; ++i, chunkShift += vecSz) {
            vtransferSN( M, &vs[chunkShift + xBlockShift], &xss[i][localId] );
			vtransferSN( M, &vs[chunkShift + yBlockShift], &yss[i][localId] );
        }
        barrier(CLK_LOCAL_MEM_FENCE);

		for (uint i=0; i < DATA_WIDTH; i += vec_step(s)) {
            const floatN x = vloadSN( N, &xss[get_local_id(0)][i] );
            const floatN y = vloadSN( N, &yss[get_local_id(1)][i] );
            s += x * y; //mad(x,y,s);
        }
        barrier(CLK_LOCAL_MEM_FENCE);
    }
    output[dlm_glob_lin_id()] = sumVecN(N, s);
}


__attribute__((vec_type_hint(floatN)))
__attribute__((work_group_size_hint(BLOCK_SIDEX, BLOCK_SIDEY, 1)))
__kernel void computeGramMrx(
    DLM_STAT_SVM __global const float * __restrict const vs,
    DLM_STAT_SVM __global float * __restrict const output,
    const int vecSz)
{
    __local float xss[BLOCK_SIDEX][DATA_WIDTH + 1]; // avoid bank conflicts!
    __local float yss[BLOCK_SIDEY][DATA_WIDTH + 1];

    const uint localId = dlm_loc_lin_id() * M;
    const uint xBlockShift = dlm_group_offset(0) * vecSz;
    const uint yBlockShift = dlm_group_offset(1) * vecSz;
    const __global float* const xBlockPtr = &vs[xBlockShift];
    const __global float* const yBlockPtr = &vs[yBlockShift];


    floatN s = (floatN) 0.0f;
    for (uint layer=0; layer < vecSz; layer += DATA_WIDTH) {

        for (uint i=0, chunkShift = layer; i < BLOCK_SIDEX; ++i, chunkShift += vecSz) {
            vtransferSN( M, &xBlockPtr[chunkShift + localId], &xss[i][localId] );
            vtransferSN( M, &yBlockPtr[chunkShift + localId], &yss[i][localId] );
        }
        barrier(CLK_LOCAL_MEM_FENCE);

        for (uint i=0; i < DATA_WIDTH; i += vec_step(s)) {
            const floatN x = vloadSN( N, &xss[get_local_id(0)][i] );
            const floatN y = vloadSN( N, &yss[get_local_id(1)][i] );
            s += x * y; //mad(x,y,s);
        }
        barrier(CLK_LOCAL_MEM_FENCE);
    }
    output[dlm_glob_lin_id()] = sumVecN(N, s);
}
