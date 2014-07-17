/* Reminder:
 * Do not use #ifndef header guards for template files.
 */

template <typename T> T clamp(const T& value, const T& low, const T& high)
{
    return value < low ? low : (value > high ? high : value);
}
