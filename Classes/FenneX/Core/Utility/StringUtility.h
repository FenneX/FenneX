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

#ifndef __FenneX__StringUtility__
#define __FenneX__StringUtility__

#include "cocos2d.h"
#include "FenneXMacros.h"
USING_NS_CC;

NS_FENNEX_BEGIN
//Perform a real string comparison, whereas the default CCArray::containsObject just do a pointer comparison

//Replacement suggestion assume std::vector<std::string> list; and std::string string;

/*
 Replace by 
 std::find(list.begin(), list.end(), string) != list.end();
 */
CC_DEPRECATED_ATTRIBUTE bool arrayContainsString(CCArray* list, CCString* string);

/* 
 Replace by
 std::vector<std::string>::iterator iter = std::find_if(list.begin(), list.end(), string);
 if(iter != list.end()) size_t index = std::distance(list.begin(), iter);
 */
CC_DEPRECATED_ATTRIBUTE int arrayGetStringIndex(CCArray* list, CCString* string);

/*
 Replace by
 list.erase(std::remove(list.begin(), list.end(), string), list.end())
 */
CC_DEPRECATED_ATTRIBUTE void arrayRemoveString(CCArray* list, CCString* string);

/*
 Replace by
 list.erase(std::remove_if(list.begin(), list.end(), [other](const std::string& string) {
    std::find(other.begin(), other.end(), string) != other.end()
 }), list.end())
 */
CC_DEPRECATED_ATTRIBUTE void arrayRemoveStringFromOther(CCArray* list, CCArray* other);

//Those methods are UTF-8 aware and require letters_conversion.txt resource to work (add it to project on iOS)
std::string upperCase(std::string text);
std::string lowerCase(std::string text);

bool stringEndsWith(std::string str, std::string suffix);

//Use Percent-encoding as defined in https://en.wikipedia.org/wiki/Percent-encoding
//Useful to submit forms, since libcurl curl_easy_escape is hard to use
//Not utf-8 aware, but it should work with utf-8 characters as well
std::string urlEncode(const std::string& str);

//Do a substring with utf8 aware code
std::string utf8_substr(const std::string& str, long start, long leng);
//Get the size of a single character
long utf8_chsize( const char* source );
//Get the size of an utf8 string
long utf8_len( const std::string& s );

//Split the string s to a vector using separator separator
std::vector<std::string> split(const std::string& s, char seperator);

NS_FENNEX_END

#endif /* defined(__FenneX__StringUtility__) */
