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

#include "Localization.h"
#include "PListPersist.h"
#include "Shorteners.h"
#include "AppMacros.h"
#include "NativeUtility.h"
#include "StringUtility.h"
#include "FenneXMacros.h"
#include "ValueConversion.h"

NS_FENNEX_BEGIN

std::string Localization::currentLanguage;
std::map<std::string, std::string> Localization::translations;
std::vector<std::string> Localization::availableTranslations;

//TODO : refactor : tell which translations should be tried

bool Localization::willTranslate()
{
#if !(USE_TRANSLATION)
    return false;
#endif
    loadAvailableTranslations();
    return std::find(availableTranslations.begin(), availableTranslations.end(), getLocalLanguage()) != availableTranslations.end();
}

CCString* Localization::getLocalizedString(CCString* string) {
    return Screate(getLocalizedString(string->getCString()));
}

const std::string Localization::getLocalizedString(const std::string& string){
#if !(USE_TRANSLATION)
    return string;
#endif
#if VERBOSE_LOCALIZATION
    log("Getting localized string ...");
#endif
    std::string language = getLocalLanguage();
    if (!loadAvailableTranslations() || !willTranslate())
    {
#if VERBOSE_LOCALIZATION
        log("language not supported, returning same string");
#endif
        return string;
    }
    if (currentLanguage != language) {
#if VERBOSE_LOCALIZATION
        log("language : %s, loading translations", language.c_str());
#endif
        currentLanguage = language;
        translations.clear();
        loadTranslations();
    }
#if VERBOSE_LOCALIZATION
    if(infos != NULL && infos.find(string->getCString()) == infos.end())
    {
        log("Warning : the string %s doesn't have any match, check your translation file", string->getCString());
    }
#endif
    return translations.find(string) != translations.end() ? translations[string] : string;
}

void Localization::loadAdditionalTranslations(std::function<std::string(std::string)> resolveLanguageFile)
{
    //Ensure Localization is loaded
    getLocalizedString("");
    if(willTranslate())
    {
        std::map<std::string, std::string> additionalTranslations = ValueConversion::toMapStringString(loadValueFromFile(resolveLanguageFile(currentLanguage), true));
        for(auto iter = additionalTranslations.begin(); iter != additionalTranslations.end(); iter++)
        {
            if(translations.find(iter->first) == translations.end())
            {
                translations[iter->first] = iter->second;
            }
            else
            {
                log("Warning, translations already contain key %s", iter->first.c_str());
            }
        }
    }
}

bool Localization::loadAvailableTranslations()
{
    if(availableTranslations.empty())
    {
        availableTranslations = ValueConversion::toVectorString(loadValueFromFile("Available_translations.plist", true));
    }
    if(availableTranslations.size() == 0)
    {
        //Put in a dummy value to avoid reloading every time
        availableTranslations.push_back("noTranslation");
        return false;
    }
    return true;
}

void Localization::loadTranslations()
{
    translations = ValueConversion::toMapStringString(loadValueFromFile(ScreateF("Strings_%s.plist", currentLanguage.c_str())->getCString(), true));
    
    if (translations.empty())
    {
#if VERBOSE_LOCALIZATION
        log("Warning, language not supported : %s", currentLanguage.c_str());
#endif
    }
    else
    {
#if VERBOSE_LOCALIZATION
        log("infos loaded, returning string ...");
#endif
        //Avoid erasing during iteration
        std::vector<std::string> toErase;
        for(auto iter = translations.begin(); iter != translations.end(); iter++)
        {
            bool valueChanged = false;
            std::string key = iter->first;
            std::string value = iter->second;
            std::size_t position;
            std::string toReplace = "(p)";
            while((position = value.find(toReplace)) != std::string::npos)
            {
                valueChanged = true;
                value = value.replace(position, toReplace.size(), "%");
            }
            if(key.find(toReplace) != std::string::npos)
            {
                toErase.push_back(key);
                while((position = key.find(toReplace)) != std::string::npos)
                {
                    key = key.replace(position, toReplace.size(), "%");
                }
                translations.at(key) = value;
            }
            else if(valueChanged)
            {
                translations.at(key) = value;
            }
            
        }
        for(std::string key : toErase)
        {
            translations.erase(key);
        }
    }
}
NS_FENNEX_END
