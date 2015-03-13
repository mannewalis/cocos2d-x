
/****************************************************************************
 Copyright (c) 2013-2014 Chukong Technologies Inc.
 
 http://www.cocos2d-x.org
 
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
 ****************************************************************************/

#ifndef _CC_PAL_HANDLES_H_
#define _CC_PAL_HANDLES_H_

#include <vector>
#include "CCPALMacros.h"

NS_PRIVATE_BEGIN

class Handles
{
public:
    
    explicit Handles(size_t initialHandleCount)
    {
        grow(initialHandleCount);
    }
    
    handle allocate(const intptr_t& reference)
    {
        if (_free.empty())
            grow(_handles.size());
        auto h = _free.back();
        _free.pop_back();
        PAL_ASSERT(h >= 0 && h < _handles.size());
        _handles[h] = reference;
        return h;
    }
    
    intptr_t free(handle h)
    {
        PAL_ASSERT(h >= 0 && h < _handles.size());
        auto reference = _handles[h];
        _free.push_back(h);
        return reference;
    }
    
    void set(handle h, const intptr_t& reference)
    {
        PAL_ASSERT(h >= 0 && h < _handles.size());
        _handles[h] = reference;
    }
    
    template <class C>
    C* getPtr(handle h)
    {
        PAL_ASSERT(h >= 0 && h < _handles.size());
        return static_cast<C*>((void*)_handles[h]);
    }

    template <class T>
    const T& getVal(handle h)
    {
        PAL_ASSERT(h >= 0 && h < _handles.size());
        return (T&)_handles[h];
    }

protected:
    
    void grow(size_t amount)
    {
        auto start = _handles.size();
        auto size = start + amount;

        _handles.reserve(size);
        _free.reserve(size);
        
        for (auto i = size; i > start; --i)
        {
            _handles.push_back(0);
            _free.push_back(i);
        }
    }
    
protected:
    
    std::vector<intptr_t> _handles;
    std::vector<ssize_t> _free;
};

#define HANDLE_CREATE(_h, ptr) (_h).allocate((intptr_t)ptr)
#define HANDLE_TOVAL(_h, h, c) (_h).getVal<c>(h)
#define HANDLE_TOPTR(_h, h, c) (_h).getPtr<c>(h)
#define HANDLE_DESTROY(_h, h)  (_h).free(h)

NS_PRIVATE_END

#endif//_CC_PAL_HANDLES_H_
