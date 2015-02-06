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
#include "Logs.h"
#include "AppMacros.h"
#include "NativeUtility.h"
#include "StringUtility.h"
#include "FenneXMacros.h"

NS_FENNEX_BEGIN
CCString* Localization::currentLanguage = NULL;
CCDictionary* Localization::infos = NULL;
CCArray* Localization::availableTranslations = NULL;


//TODO : refactor : tell which translations should be tried

bool Localization::willTranslate()
{
#if !(USE_TRANSLATION)
    return false;
#endif
    loadAvailableTranslations();
    CCString* language = Screate(getLocalLanguage());
    return arrayContainsString(availableTranslations, language);
}

CCString* Localization::getLocalizedString(CCString* string) {
#if !(USE_TRANSLATION)
    return string;
#endif
#if VERBOSE_LOCALIZATION
    CCLOG("Getting localized string ...");
#endif
    CCString* language = Screate(getLocalLanguage());
    if (!loadAvailableTranslations() || !arrayContainsString(availableTranslations, language))
    {
#if VERBOSE_LOCALIZATION
        CCLOG("language not supported, returning same string");
#endif
        return string;
    }
    if (currentLanguage == NULL || !currentLanguage->isEqual(language)) {
#if VERBOSE_LOCALIZATION
        CCLOG("language : %s, loading infos", language->getCString());
#endif
        if(currentLanguage != NULL)
        {
            currentLanguage->release();
        }
        currentLanguage = language;
        currentLanguage->retain();
        if (infos != NULL) {
            infos->release();
        }
        loadInfos();
    }
#if VERBOSE_LOCALIZATION
    if(infos != NULL && infos->objectForKey(string->getCString()) == NULL)
    {
        CCLOG("Warning : the string %s doesn't have any match, check your translation file", string->getCString());
    }
#endif
    return infos != NULL && infos->objectForKey(string->getCString()) != NULL ?
    (CCString*) infos->objectForKey(string->getCString()) : string;
}

const char* Localization::getLocalizedString(const char* string) {
    return getLocalizedString(Screate(string))->getCString();
}

const std::string Localization::getLocalizedString(const std::string& string){
    return getLocalizedString(Screate(string.c_str()))->getCString();
}

void Localization::loadAdditionalTranslations(std::function<std::string(std::string)> resolveLanguageFile)
{
    //Ensure Localization is loaded
    getLocalizedString("");
    if(willTranslate())
    {
        CCDictionary* additionalTranslations = (CCDictionary*)loadObjectFromFile(resolveLanguageFile(currentLanguage->getCString()).c_str(), true);
        if(additionalTranslations != NULL)
        {
            CCArray* keys = additionalTranslations->allKeys();
            for(int i = 0; i < keys->count(); i++)
            {
                std::string key = TOCSTRING(keys->objectAtIndex(i));
                if(infos->objectForKey(key) == NULL)
                {
                    infos->setObject(additionalTranslations->objectForKey(key), key);
                }
                else
                {
                    CCLOG("Warning, translations already contain key %s", key.c_str());
                }
            }
        }
    }
}

bool Localization::loadAvailableTranslations()
{
    if(availableTranslations == NULL)
    {
        availableTranslations = (CCArray*)loadObjectFromFile("Available_translations.plist", true);
        
        if(availableTranslations == NULL)
        {
            availableTranslations = new CCArray();
        }
        else
        {
            availableTranslations->retain();
        }
    }
    if(availableTranslations->count() == 0)
    {
        return false;
    }
    return true;
}

void Localization::loadInfos()
{
    infos = (CCDictionary*) loadObjectFromFile(ScreateF("Strings_%s.plist", currentLanguage->getCString())->getCString(), true);
    
    if (infos == NULL)
    {
#if VERBOSE_LOCALIZATION
        CCLOG("Warning, language not supported : %d", currentLanguage);
#endif
    }
    else
    {
#if VERBOSE_LOCALIZATION
        CCLOG("infos loaded, returning string ...");
#endif
        infos->retain();
        CCArray* keys = infos->allKeys();
        //replace occurences of (p) by %
        for(int i = 0; i < keys->count(); i++)
        {
            CCString* key = (CCString*)keys->objectAtIndex(i);
            CCString* value = (CCString*)infos->objectForKey(key->getCString());
            bool valueChanged = false;
            std::string stdKey = key->getCString();
            std::string stdValue = value->getCString();
            std::size_t position;
            std::string toReplace = "(p)";
            while((position = stdValue.find(toReplace)) != std::string::npos)
            {
                valueChanged = true;
                stdValue = stdValue.replace(position, toReplace.size(), "%");
            }
            if(stdKey.find(toReplace) != std::string::npos)
            {
                infos->removeObjectForKey(stdKey);
                while((position = stdKey.find(toReplace)) != std::string::npos)
                {
                    stdKey = stdKey.replace(position, toReplace.size(), "%");
                }
                infos->setObject(Screate(stdValue.c_str()), stdKey);
            }
            else if(valueChanged)
            {
                infos->setObject(Screate(stdValue.c_str()), stdKey);
            }
        }
    }
}
NS_FENNEX_END
