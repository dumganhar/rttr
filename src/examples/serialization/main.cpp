/************************************************************************************
*                                                                                   *
*   Copyright (c) 2014 - 2018 Axel Menzel <info@rttr.org>                           *
*                                                                                   *
*   This file is part of RTTR (Run Time Type Reflection)                            *
*   License: MIT License                                                            *
*                                                                                   *
*   Permission is hereby granted, free of charge, to any person obtaining           *
*   a copy of this software and associated documentation files (the "Software"),    *
*   to deal in the Software without restriction, including without limitation       *
*   the rights to use, copy, modify, merge, publish, distribute, sublicense,        *
*   and/or sell copies of the Software, and to permit persons to whom the           *
*   Software is furnished to do so, subject to the following conditions:            *
*                                                                                   *
*   The above copyright notice and this permission notice shall be included in      *
*   all copies or substantial portions of the Software.                             *
*                                                                                   *
*   THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR      *
*   IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,        *
*   FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE     *
*   AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER          *
*   LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,   *
*   OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE   *
*   SOFTWARE.                                                                       *
*                                                                                   *
*************************************************************************************/

#include <iostream>

#include <rttr/type>

#include "serialization_visitor.h"

#include <rttr/registration>

#include <bitsery/bitsery.h>
#include <bitsery/adapter/buffer.h>
//to use brief syntax always include this header
#include <bitsery/brief_syntax.h>
//we also need additional traits to work with container types,
//instead of including <bitsery/traits/vector.h> for vector traits, now we also need traits to work with brief_syntax types.
//so include everything from <bitsery/brief_syntax/...> instead of <bitsery/traits/...>
//otherwise we'll get static assert error, saying to define serialize function.
#include <bitsery/brief_syntax/vector.h>

enum class MyEnum:uint16_t { V1,V2,V3 };
struct MyStruct {
    uint32_t i;
    MyEnum e;
    std::vector<float> fs;
};



//define serialize function as usual
template <typename S>
void serialize(S& s, MyStruct& my) {
    //now we can use brief syntax with
//    s(my.i);
//    s(my.e);
//    s(my.fs);

    serialization_visitor vi(&my);

//    auto func = [=](auto& property){
//
//    };

    vi.visit(rttr::type::get<MyStruct>());
}


//some helper types
using Buffer = std::vector<uint8_t>;
using OutputAdapter = bitsery::OutputBufferAdapter<Buffer>;
using InputAdapter = bitsery::InputBufferAdapter<Buffer>;

using namespace rttr;

int main(int argc, char** argv)
{
    //set some random data
    MyStruct data{8941, MyEnum::V2, {15.0f, -8.5f, 0.045f}};
    MyStruct res{};

    //serialization, deserialization flow is unchanged as in basic usage
    Buffer buffer;
    auto writtenSize = bitsery::quickSerialization<OutputAdapter>(buffer, data);

    auto state = bitsery::quickDeserialization<InputAdapter>({buffer.begin(), writtenSize}, res);

//    assert(state.first == bitsery::ReaderError::NoError && state.second);
//    assert(data.fs == res.fs && data.i == res.i && data.e == res.e);

    return 0;
}

RTTR_REGISTRATION
{
    rttr::registration::class_<MyStruct>("MyStruct")
        .constructor<>()
        .property("i", &MyStruct::i)
        .property("e", &MyStruct::e)
        .property("fs", &MyStruct::fs)
    ;
}

