#include <stdio.h>

int main()
{
    char c = 0b00001111; // Example input
    char result = 0;

    for (int i = 0; i < 8; i++)
    {
        if (i % 2 == 1)
        {
            result |= (1 << i);
        }
        // else
        // {
        //     result |= (c & (1 << i));
        // }
    }
    char bin_str[33]; // holds up to 32-bit binary string + terminating null character

    // Convert num to binary string
    for (int i = 31; i >= 0; i--)
    {
        bin_str[31 - i] = ((result >> i) & 1) ? '0' : '1';
    }
    bin_str[32] = '\0'; // terminate string

    printf("Binary representation of %d is %s\n", result, bin_str);

    printf("Input: %d\nResult: %d\n", c, result);
    return 0;
}
