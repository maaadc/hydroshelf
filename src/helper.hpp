#ifndef HYDROSHELF_HELPER_HPP
#define HYDROSHELF_HELPER_HPP

namespace hydroshelf
{
template <typename T>
T clip(T f_low, T f_value, T f_high)
{
    assert(f_low <= f_high);

    if (f_value < f_low)
    {
        return f_low;
    }
    if (f_value > f_high)
    {
        return f_high;
    }
    return f_value;
}
}  // namespace hydroshelf

#endif  // HYDROSHELF_HELPER_HPP