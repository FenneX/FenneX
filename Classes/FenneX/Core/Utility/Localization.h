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

#ifndef __FenneX__Localization__
#define __FenneX__Localization__

#include "FenneXMacros.h"
#include "cocos2d.h"
USING_NS_CC;

/* important note : in the String_xx.plist file, every string formatter (example : %d) should have the "%" replaced by "(p)" so that the xml can be printed properly. \n must also be replaced by (n). Those will be replaced when loading the file
 TODO : try to use CDATA to counter that
 */
NS_FENNEX_BEGIN
class Localization
{
public:
    static bool willTranslate();
    static const std::string getLocalizedString(const std::string& string);
    
    static void loadAdditionalTranslations(std::function<std::string(std::string)> resolveLanguageFile);
private:
    static std::string currentLanguage;
    static std::map<std::string, std::string> translations;
    static std::vector<std::string> availableTranslations;
    static bool loadAvailableTranslations();
    static void loadTranslations();
};
NS_FENNEX_END

#endif /* defined(__FenneX__Localization__) */
