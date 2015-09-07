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

#include "SynchronousReleaser.h"
#include "Shorteners.h"
#include "RawObject.h"
#include "AppMacros.h"
#include "FenneXMacros.h"

NS_FENNEX_BEGIN
// singleton stuff
static SynchronousReleaser *s_SharedReleaser = NULL;

SynchronousReleaser* SynchronousReleaser::sharedReleaser(void)
{
    if (!s_SharedReleaser)
    {
        s_SharedReleaser = new SynchronousReleaser();
        s_SharedReleaser->init();
    }
    
    return s_SharedReleaser;
}

void SynchronousReleaser::init()
{
    releasePool.reserve(32);
}

SynchronousReleaser::~SynchronousReleaser()
{
    releasePool.clear();
    s_SharedReleaser = NULL;
}


void SynchronousReleaser::emptyReleasePool()
{
#if VERBOSE_DEALLOC
    for(Ref* obj : releasePool)
    {
        if(obj->getReferenceCount() != 1)
        {
            const char* name = isKindOfClass(obj, RawObject) ? ((RawObject*)obj)->getName() : "Unknown";
            CCLOG("!!Warning!! before releasing from ReleasePool, obj %s have retainCount %d", name, obj->getReferenceCount());
        }
    }
#endif
    releasePool.clear();
}

void SynchronousReleaser::addObjectToReleasePool(Ref* obj)
{
    if(obj != NULL && !releasePool.contains(obj))
    {
        releasePool.pushBack(obj);
    }
}
NS_FENNEX_END
