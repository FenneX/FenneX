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

#ifndef DelegatingRecognizer_h
#define DelegatingRecognizer_h

#include "GenericRecognizer.h"

NS_FENNEX_BEGIN

/**
 * This class handle what's necessary about having delegates for a recognizer
 **/
template <class T>
class DelegatingRecognizer : public GenericRecognizer
{
public:
    void addDelegate(T* delegate) {if(std::find(delegates.begin(), delegates.end(), delegate) == delegates.end()) delegates.push_back(delegate);}
    void removeDelegate(T* delegate) {delegates.erase(std::remove(delegates.begin(), delegates.end(), delegate), delegates.end());}
protected:
    std::vector<T*> delegates;
};

NS_FENNEX_END

#endif /* DelegatingRecognizer_h */
