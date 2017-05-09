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

#include "StringUtility.h"
#include "Shorteners.h"
#if (CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
#include <codecvt>
#endif

#include <sstream>
#include <iomanip>

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

std::vector<std::pair<std::string, std::string>>* getConversions()
{
    //Conversions is heavy, avoid copying it. There are 15695 pairs
    static std::vector<std::pair<std::string, std::string>>* result = NULL;
    if(result != NULL)
    {
        return result;
    }
    result = new std::vector<std::pair<std::string, std::string>>();
    char* upperCase = new char[20];
    char* separator = new char[20];
    char* lowerCase = new char[20];
#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)
    ssize_t bufferSize = 0;
    //Load file from apk
    const char* charbuffer = reinterpret_cast<const char*>(FileUtils::getInstance()->getFileData("letters_conversion.txt", "r", &bufferSize));
    if (charbuffer) {
        int index = 0;
        while (sscanf(&charbuffer[index], "%20s %20s %20s", upperCase, separator, lowerCase) !=EOF)
        {
            result->push_back(std::pair<std::string, std::string>(upperCase, lowerCase));
            //3 => 2 spaces + backspace
            index += strlen(upperCase) + strlen(lowerCase) + strlen(separator) + 3;
        }
    }
#else
    FILE* file = fopen(getResourcesPath("letters_conversion.txt").c_str(), "r");
    if (file) {
        while (fscanf(file, "%s %s %s", upperCase, separator, lowerCase)!=EOF)
        {
            result->push_back(std::pair<std::string, std::string>(upperCase, lowerCase));
        }
        fclose(file);
    }
#endif
    delete[] upperCase;
    delete[] separator;
    delete[] lowerCase;
    return result;
}

std::string upperCase(std::string text)
{
    //TODO : speed up by sorting the vector and doing a better search
    std::vector<std::pair<std::string, std::string>>* conversions = getConversions();
    if(conversions->size() == 0)
    {
        log("Warning: missing file letters_conversion.txt, required for upperCase, string %s not converted", text.c_str());
        return text;
    }
    std::string to;
    for(int i = 0; i < text.length(); i+= utf8_chsize(&text[i]))
    {
        long charLength = utf8_chsize(&text[i]);
        std::string charString = text.substr(i, charLength);
        int conversionIndex = 0;
        while(conversionIndex < conversions->size() && conversions->at(conversionIndex).second != charString)
        {
            conversionIndex++;
        }
        if(conversionIndex < conversions->size())
        {
            to += conversions->at(conversionIndex).first;
        }
        else
        {
            to += charString;
        }
    }
    return to;
}

std::string lowerCase(std::string text)
{
    //TODO : speed up by sorting the vector and doing a better search
    std::vector<std::pair<std::string, std::string>>* conversions = getConversions();
    if(conversions->size() == 0)
    {
        log("Warning: missing file letters_conversion.txt, required for upperCase, string %s not converted", text.c_str());
        return text;
    }
    std::string to;
    for(int i = 0; i < text.length(); i+= utf8_chsize(&text[i]))
    {
        long charLength = utf8_chsize(&text[i]);
        std::string charString = text.substr(i, charLength);
        int conversionIndex = 0;
        while(conversionIndex < conversions->size() && conversions->at(conversionIndex).first != charString)
        {
            conversionIndex++;
        }
        if(conversionIndex < conversions->size())
        {
            to += conversions->at(conversionIndex).second;
        }
        else
        {
            to += charString;
        }
    }
    return to;
}

bool stringEndsWith(std::string str, std::string suffix)
{
    if (suffix.length() >  str.length())
        return false;
    return str.substr(str.length() - suffix.length()) == suffix;
}

//Copied from http://stackoverflow.com/questions/154536/encode-decode-urls-in-c
std::string urlEncode(const std::string& value)
{
    std::ostringstream escaped;
    escaped.fill('0');
    escaped << std::hex;
    
    for (std::string::const_iterator i = value.begin(), n = value.end(); i != n; ++i)
    {
        std::string::value_type c = (*i);
        
        // Keep alphanumeric and other accepted characters intact, according https://tools.ietf.org/html/rfc3986#section-2.3
        if (isalnum(c) || c == '-' || c == '_' || c == '.' || c == '~')
        {
            escaped << c;
            continue;
        }
        else
        {
            // Any other characters are percent-encoded
            // If a urlDecode is needed, don't forget that ' ' (space character) can be encoded either as "%20" or "+" (shortcode since it's used a lot)
            escaped << std::uppercase;
            escaped << '%' << std::setw(2) << int((unsigned char) c);
            escaped << std::nouppercase;
        }
    }
    
    return escaped.str();
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
