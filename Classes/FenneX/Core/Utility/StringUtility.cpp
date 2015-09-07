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

#include "Logs.h"
#include "StringUtility.h"
#include "Shorteners.h"
#if (CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
#include <codecvt>
#endif

NS_FENNEX_BEGIN
bool arrayContainsString(CCArray* list, CCString* string)
{
    Ref* obj;
    CCARRAY_FOREACH(list, obj)
    {
        if(string->isEqual(obj))
            return true;
    }
    return false;
}

int arrayGetStringIndex(CCArray* list, CCString* string)
{
    for(int i = 0; i < list->count(); i++)
    {
        Ref* obj = list->objectAtIndex(i);
        if(string->isEqual(obj))
            return i;
    }
    return -1;
}

void arrayRemoveString(CCArray* list, CCString* string)
{
    
    Ref* obj;
    CCARRAY_FOREACH(list, obj)
    {
        if(string->isEqual(obj))
        {
            list->removeObject(obj);
            return;
        }
    }
}

void arrayRemoveStringFromOther(CCArray* list, CCArray* other)
{
    
    Ref* obj;
    CCArray* objectsToRemove = Acreate();
    CCARRAY_FOREACH(list, obj)
    {
        if(isKindOfClass(obj, CCString) && arrayContainsString(other, (CCString*)obj))
        {
            objectsToRemove->addObject(obj);
        }
    }
    list->removeObjectsInArray(objectsToRemove);
}

const char* changeFirstLetterCase(const char* text, bool lower)
{
    return changeFirstLetterCase(Screate(text), lower)->getCString();
}

CCString* changeFirstLetterCase(CCString* text, bool lower)
{
    if(text->length() > 0)
    {
        char firstChar = lower ? tolower(text->getCString()[0]) : toupper(text->getCString()[0]);
        std::string textString = std::string(text->getCString());
        std::string subString = textString.substr(1);
        const char* rest = subString.c_str();
        //reconstruct the char with first character lower cased
        text = ScreateF("%c%s", firstChar, rest);
    }
    return text;
}

CCString* upperCaseFirstLetter(CCString* text)
{
    return changeFirstLetterCase(text, false);
}

const char* upperCaseFirstLetter(const char* text)
{
    return changeFirstLetterCase(text, false);
}

CCString* lowerCaseFirstLetter(CCString* text)
{
    return changeFirstLetterCase(text, true);
}

const char* lowerCaseFirstLetter(const char* text)
{
    return changeFirstLetterCase(text, true);
}

std::vector<std::pair<std::string, std::string>> getConversions()
{
    std::vector<std::pair<std::string, std::string>> result;
    char upperCase[20];
    char separator[20];
    char lowerCase[20];
#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)
    ssize_t bufferSize = 0;
    //Load file from apk
    const char* charbuffer = reinterpret_cast<const char*>(FileUtils::getInstance()->getFileData("letters_conversion.txt", "r", &bufferSize));
    if (charbuffer) {
        int index = 0;
        while (sscanf(&charbuffer[index], "%20s %20s %20s", upperCase, separator, lowerCase) !=EOF)
        {
            result.push_back(std::pair<std::string, std::string>(upperCase, lowerCase));
            //3 => 2 spaces + backspace
            index += strlen(upperCase) + strlen(lowerCase) + strlen(separator) + 3;
        }
    }
#else
    FILE* file = fopen(getResourcesPath("letters_conversion.txt").c_str(), "r");
    if (file) {
        while (fscanf(file, "%s %s %s", upperCase, separator, lowerCase)!=EOF)
        {
            result.push_back(std::pair<std::string, std::string>(upperCase, lowerCase));
        }
        fclose(file);
    }
#endif
    return result;
}

CCString* upperCaseString(CCString* text)
{
    timeval startTime;
    gettimeofday(&startTime, NULL);
    //TODO : speed up by sorting the vector and doing a better search
    static std::vector<std::pair<std::string, std::string>> conversions = getConversions();
    if(conversions.size() == 0)
    {
        CCLOG("Warning: missing file letters_conversion.txt, required for upperCaseString, string %s not converted", text->getCString());
        return text;
    }
    std::string from = text->getCString();
    std::string to;
    for(int i = 0; i < from.length(); i+= utf8_chsize(&from[i]))
    {
        long charLength = utf8_chsize(&from[i]);
        std::string charString = from.substr(i, charLength);
        int conversionIndex = 0;
        while(conversionIndex < conversions.size() && conversions[conversionIndex].second != charString)
        {
            conversionIndex++;
        }
        if(conversionIndex < conversions.size())
        {
            to += conversions[conversionIndex].first;
        }
        else
        {
            to += charString;
        }
    }
    timeval endTime;
    gettimeofday(&endTime, NULL);
    return Screate(to.c_str());
}

const char* upperCaseString(const char* text)
{
    return upperCaseString(Screate(text))->getCString();
}

bool stringEndsWith(const char *str, const char *suffix)
{
    if (!str || !suffix)
        return 0;
    long lenstr = strlen(str);
    long lensuffix = strlen(suffix);
    if (lensuffix >  lenstr)
        return 0;
    return strncmp(str + lenstr - lensuffix, suffix, lensuffix) == 0;
}

//comes from http://www.zedwood.com/article/cpp-utf-8-mb_substr-function
std::string utf8_substr(const std::string& str, long start, long leng)
{
    if (leng==0) { return ""; }
    long c, i, ix, q, min=std::string::npos, max=std::string::npos;
    for (q=0, i=0, ix=str.length(); i < ix; i++, q++)
    {
        if (q==start){ min=i; }
        if (q<=start+leng || leng==std::string::npos){ max=i; }
        
        c = (unsigned char) str[i];
        if      (c<=127) i+=0;
        else if ((c & 0xE0) == 0xC0) i+=1;
        else if ((c & 0xF0) == 0xE0) i+=2;
        else if ((c & 0xF8) == 0xF0) i+=3;
        //else if (($c & 0xFC) == 0xF8) i+=4; // 111110bb //byte 5, unnecessary in 4 byte UTF-8
        //else if (($c & 0xFE) == 0xFC) i+=5; // 1111110b //byte 6, unnecessary in 4 byte UTF-8
        else return "";//invalid utf8
    }
    if (q<=start+leng || leng==std::string::npos){ max=i; }
    if (min==std::string::npos || max==std::string::npos) { return ""; }
    return str.substr(min,max);
}

//comes from strutil 1.5 https://code.google.com/p/strutil/
long utf8_chsize( const char* source )
{
    const unsigned ch = (unsigned char)*source;
    if ( ch < 192 )
        return 1;
    else if ( ch < 224 )
        return 2;
    else if ( ch < 240 )
        return 3;
    else if ( ch < 248 )
        return 4;
    else if ( ch < 252 )
        return 5;
    else
        return 6;
}

long utf8_len( const std::string& s )
{
    const char* it = s.c_str();
    long n = 0;
    
    while ( *it )
    {
        ++n;
        it += utf8_chsize( it );
    }
    return n;
}
NS_FENNEX_END
