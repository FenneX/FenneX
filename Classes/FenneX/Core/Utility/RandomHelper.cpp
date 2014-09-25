/****************************************************************************
Copyright (c) 2013-2014 Auticiel SAS

http://www.fennex.org

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
THE SOFTWARE.
****************************************************************************///

#include "RandomHelper.h"

NS_FENNEX_BEGIN
bool containsNumber(int* array, int number, int count)
{
    for(int j = 0; j < count; j++)
    {
        if(number == array[j])
        {
            return true;
        }
    }
    return false;
}

bool containsNumber(CCArray* array, int number)
{
    for(int j = 0; j < array->count(); j++)
    {
        if(number == TOINT(array->objectAtIndex(j)))
            return true;
    }
    return false;
}

void randomWithoutRetake(int* array, int count, int max)
{
    int actualCount = max != -1 ? (count > max ? max : count) : count;
    if(max == -1)
    {
        max = count;
    }
    for(int i = 0; i < actualCount; i++)
    {
        int r = arc4random() % max;
        while(containsNumber(array, r, i))
        {
            r++;
        }
        //insert r so that taken is in increasing order
        /*int j = 0;
         while(j < i && r > array[j]) j++;
         for(int k = i - 1; k >= j; k--)
         {
         array[k+1] = array[k];
         }
         array[j] = r;*/
        array[i] = r;
		max--;
	}
	//sanity test
	for(int i = 0; i < actualCount; i++)
	{
		for(int j = i + 1; j < actualCount; j++)
		{
			if(array[i] == array[j])
			{
				CCLOG("Warning : number %d picked twice during question choice", array[i]);
			}
		}
	}
}

void randomWithoutRetake(CCArray* array, int count, int max)
{
	int actualCount = max != -1 ? (count > max ? max : count) : count;
    if(max == -1)
    {
        max = array->count();
    }
    CCAssert(max >= actualCount, "Can't do a random without a valid max (>= count)");
	for(int i = 0; i < actualCount; i++)
	{
		int r = arc4random() % max;
        while(containsNumber(array, r))
        {
            r++;
        }
        array->addObject(Icreate(r));
		max--;
	}
	//sanity test
	for(int i = 0; i < actualCount; i++)
	{
		for(int j = i + 1; j < actualCount; j++)
		{
			if(TOINT(array->objectAtIndex(i)) == TOINT(array->objectAtIndex(j)))
			{
				CCLOG("Warning : number %d picked twice during question choice", TOINT(array->objectAtIndex(i)));
			}
		}
	}
}
NS_FENNEX_END
