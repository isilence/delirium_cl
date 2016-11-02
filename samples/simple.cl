__kernel void squareArray(__global const float * __restrict const in, __global float * __restrict const out)
{
    const int idy = get_global_id(1);
    const int idx = get_global_id(0);
    const int n = get_global_size(0);
    const int offset = idy * n + idx;

    out[offset] = in[offset] * in[offset];
}
