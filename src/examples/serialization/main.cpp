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
#include "my_visitor.h"

#include <rttr/registration>

#include <rttr/detail/visitor/visitor_iterator.h>



//#define CC_SERIALIZE_FUNCTION(classType) \
//template<typename AdapterType> \
//void serialize(bitsery::Serializer<AdapterType> & s, classType& obj) { \
//    my_serialization_visitor vi(&obj, s.getSerializeType()); \
//    vi._serializer_OutputAdapter = &s; \
//    vi.visit(rttr::type::get<classType>()); \
//} \
//\
//template<typename AdapterType> \
//void serialize(bitsery::Deserializer<AdapterType> & s, classType& obj) { \
//    my_serialization_visitor vi(&obj, s.getSerializeType()); \
//    vi._deserializer_InputAdapter = &s; \
//    vi.visit(rttr::type::get<classType>()); \
//}

#define CC_SERIALIZE_ACCESS(classType, outputAdapterType, inputAdapterType) \
void serialize(bitsery::Serializer<outputAdapterType> & s) { \
    serialization_visitor vi(this, s.getSerializeType()); \
    vi._serializer_##outputAdapterType = &s; \
    vi.visit(rttr::type::get<classType>()); \
}\
void serialize(bitsery::Deserializer<inputAdapterType> & s) { \
    serialization_visitor vi(this, s.getSerializeType()); \
    vi._deserializer_##inputAdapterType = &s; \
    vi.visit(rttr::type::get<classType>()); \
}

enum class MyEnum:uint16_t { V1,V2,V3 };

struct MyStruct {
    uint32_t i;
    MyEnum e;
    std::vector<float> fs;

//    CC_SERIALIZE_ACCESS(MyStruct, OutputAdapter, InputAdapter)
};

//struct MyDummyVisitor : public rttr::visitor {};

//namespace rttr { namespace detail {
//
//template<>
//struct visitor_iterator<
//            serialization_visitor<bitsery::Serializer<bitsery::OutputBufferAdapter<std::vector<unsigned char>>>>,
//            serialization_visitor<bitsery::Deserializer<bitsery::InputBufferAdapter<std::vector<unsigned char>>>>> {
//
//    static void visit(visitor& visitor, const rttr::detail::type_visitor_invoker<rttr::type_list<MyStruct, rttr::type_list<>>>& invoker)
//    {
//    }
//};
//
//}}

//CC_SERIALIZE_FUNCTION(MyStruct)

template<typename S>
void serialize(S& s, MyStruct& obj) {
    serialization_visitor<S> vi(&obj, &s);
    vi.visit(rttr::type::get<MyStruct>());
}


//template<typename OutputAdapterType>
//void serialize(bitsery::Serializer<OutputAdapterType> & s, MyStruct& obj) {
//    {
//        serialization_visitor vi(&obj, s.getSerializeType());
//        vi._serializer_OutputAdapter = &s;
//        vi.visit(rttr::type::get<MyStruct>());
//    }
//
//    {
//        my_visitor vi(&obj);
//        vi.visit(rttr::type::get<MyStruct>());
//    }
//}
//
//template<typename InputAdapterType>
//void serialize(bitsery::Deserializer<InputAdapterType> & s, MyStruct& obj) {
//    {
//        serialization_visitor vi(&obj, s.getSerializeType());
//        vi._deserializer_InputAdapter = &s;
//        vi.visit(rttr::type::get<MyStruct>());
//    }
//
//    {
//        my_visitor vi(&obj);
//        vi.visit(rttr::type::get<MyStruct>());
//    }
//}




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

    assert(state.first == bitsery::ReaderError::NoError && state.second);
    assert(data.fs == res.fs && data.i == res.i && data.e == res.e);

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

