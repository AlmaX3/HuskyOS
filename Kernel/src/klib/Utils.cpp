#include <Utils.h>
#include <stdint.h>
#include <Math.h>

void unsigned_number_to_string(uint32_t number, int base, char *buffer, int precision)
{
    if (number == 0)
    {
        *buffer++ = '0';
        *buffer = 0;
        return;
    }

    char buff[65];
    for (int i = 0; i < 65; i++)
    {
        buff[i] = 0;
    }

    int curr = 0;

    while (number)
    {
        int digit = number % base;
        if (digit >= 10)
        {
            buff[curr++] = 'a' + (digit - 10);
        }
        else
        {
            buff[curr++] = '0' + digit;
        }

        number /= base;
    }

    if (precision != 0)
    {
        int to_reduce = precision;
        while (to_reduce)
        {
            buff[curr++] = '0';
            to_reduce--;
        }
    }

    for (int i = curr - 1; i != 0; i--)
    {
        *buffer++ = buff[i];
    }
    *buffer++ = buff[0];
    *buffer = 0;
}

void number_to_string(int32_t number, int base, char *buffer, int precision)
{
    if (number < 0)
    {
        *buffer++ = '-';
        number = -number;
    }

    unsigned_number_to_string(number, base, buffer, precision);
}


int fnumber_to_string(float floatNum, int base, char *buffer, int precision)
{
    
    int32_t number = (int32_t)floatNum;

    if (number < 0)
    {
        *buffer++ = '-';
        number = -number;
    }

    if (number == 0)
    {
        *buffer++ = '0';
        *buffer = 0;
    }

    char buff[65];
    for (int i = 0; i < 65; i++)
    {
        buff[i] = 0;
    }

    int curr = 0;

    if (precision != 0)
    {

        float floating = floatNum - (double)number;

        int floatPart = floating * pow(base, precision - 1);

        if (floatPart == 0)
        {
            int to_reduce = precision;
            while (to_reduce)
            {
                buff[curr++] = '0';
                to_reduce--;
            }
        }

        while (floatPart)
        {
            int digit = floatPart % base;

            if (digit >= 10)
            {
                buff[curr++] = 'a' + (digit - 10);
            }
            else
            {
                buff[curr++] = '0' + digit;
            }

            floatPart /= base;
        }

        buff[curr++] = '.';
    }

    while (number)
    {
        int digit = number % base;
        if (digit >= 10)
        {
            buff[curr++] = 'a' + (digit - 10);
        }
        else
        {
            buff[curr++] = '0' + digit;
        }

        number /= base;
    }

    for (int i = curr - 1; i != 0; i--)
    {
        *buffer++ = buff[i];
    }

    *buffer++ = buff[0];
    *buffer = 0;
    return 0;
}
