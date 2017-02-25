__kernel void squareArray(__global const float * __restrict const in, __global float * __restrict const out)
{
    const int offset = get_global_id(0);
    out[offset] = in[offset] * in[offset];
}
