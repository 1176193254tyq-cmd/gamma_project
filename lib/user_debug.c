
#include "user_debug.h"

int HexToAscii(unsigned char *pHexStr,unsigned char *pAscStr, int Len)
{
    char Nibble[3];
    unsigned char Buffer[2048];
    int i = 0;
    int j = 0;

    for (i = 0; i < Len; i++)
    {
        Nibble[0] = pHexStr[i] >> 4 & 0X0F;
        Nibble[1] = pHexStr[i] & 0x0F;
        for (j = 0; j < 2; j++)
        {
            if (Nibble[j] < 10)
            {
                Nibble[j] = Nibble[j] + 0x30;
            }
            else if (Nibble[j] < 16)
            {
                Nibble[j] = Nibble[j] - 10 + 'A';
            }
            else
            {
                return 0;
            }
        }
        Nibble[2]=' ';
        memcpy(Buffer + i * 3, Nibble, 3);
    }
    Buffer[3 * Len] = 0x00;
    memcpy(pAscStr, Buffer, 3 * Len);
    pAscStr[3 * Len] = 0x00;
    return 1;
}

char AsciiToHex(unsigned char *pAscii, unsigned char *pHex, int nLen)
{
    int nHexLen = nLen / 2;
    unsigned char Nibble[2] = {0};
    int i = 0;
    int j = 0;

    if (nLen % 2)
    {
        return 1;
    }

    for (i = 0; i < nHexLen; i++)
    {
        Nibble[0] = *pAscii++;
        Nibble[1] = *pAscii++;
        for (j = 0; j < 2; j++)
        {
            if (Nibble[j] <= 'F' && Nibble[j] >= 'A')
                Nibble[j] = Nibble[j] - 'A' + 10;
            else if (Nibble[j] <= 'f' && Nibble[j] >= 'a')
                Nibble[j] = Nibble[j] - 'a' + 10;
            else if (Nibble[j] >= '0' && Nibble[j] <= '9')
                Nibble[j] = Nibble[j] - '0';
            else
                return 1; // Nibble[j] = Nibble[j] - 'a' + 10;

        }                         // for (int j = ...)
        pHex[i] = Nibble[0] << 4; // Set the high nibble
        pHex[i] |= Nibble[1];     // Set the low nibble
    }                             // for (int i = ...)
    return 0;
}
