/*
 * Copyright (c) 2011-2014 zhangjianlin ’≈ºˆ¡÷
 * eamil:jonas88@sina.com
 * addr: china
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. The name of the author may not be used to endorse or promote products
 *    derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
 * OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
 * IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT
 * NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
 * THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include <stdlib.h>
#include <ctype.h>
#include <string.h>

static unsigned char hexchars[] = "0123456789ABCDEF";  
  
static int php_htoi(char *s)  
{  
    int c = *((unsigned char *)s);
    int value;  
    if (isupper(c))  
        c = tolower(c);  
    value = (c >= '0' && c <= '9' ? c - '0' : c - 'a' + 10) * 16;  
  
    c = ((unsigned char *)s)[1];  
    if (isupper(c))  
        c = tolower(c);  
    value += c >= '0' && c <= '9' ? c - '0' : c - 'a' + 10;  
  
    return (value);  
}  
  
  
char *iop_url_encode(char const *s, int len, int *new_length)  
{  
    register unsigned char c;  
    unsigned char *to, *start;  
    unsigned char const *from, *end;  
      
    from = (unsigned char *)s;  
    end  = (unsigned char *)s + len;  
    start = to = (unsigned char *) calloc(1, 3*len+1);  
  
    while (from < end)   
    {  
        c = *from++;  
  
        if (c == ' ')   
        {  
            *to++ = '+';  
        }   
        else if ((c < '0' && c != '-' && c != '.') ||  
                 (c < 'A' && c > '9') ||  
                 (c > 'Z' && c < 'a' && c != '_') ||  
                 (c > 'z'))   
        {  
            to[0] = '%';  
            to[1] = hexchars[c >> 4];  
            to[2] = hexchars[c & 15];  
            to += 3;  
        }  
        else   
        {  
            *to++ = c;  
        }  
    }  
    *to = 0;  
    if (new_length)   
    {  
        *new_length = to - start;  
    }  
    return (char *) start;  
}  
  
  
int iop_url_decode(char *str, int len)  
{  
    char *dest = str;  
    char *data = str;  
  
    while (len--)   
    {  
        if (*data == '+')   
        {  
            *dest = ' ';  
        }  
        else if (*data == '%' && len >= 2 && isxdigit((int) *(data + 1)) && isxdigit((int) *(data + 2)))   
        {  
            *dest = (char) php_htoi(data + 1);  
            data += 2;  
            len -= 2;  
        }   
        else   
        {  
            *dest = *data;  
        }  
        data++;  
        dest++;  
    }  
    *dest = 0;
    return dest - str;  
}
