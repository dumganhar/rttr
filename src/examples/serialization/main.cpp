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
#include <string>

#include <rttr/type>

#include "serialization_visitor.h"
#include "my_visitor.h"

#include <rttr/registration>

#include <rttr/registration_friend> // important!

#include <bitsery/brief_syntax/string.h>

enum class MyEnum:uint16_t { V1,V2,V3 };

#define CC_CLASS(classType, ...) \
public: \
    template<typename S> \
    void serialize(S& s) { \
        serialization_visitor<S> vi(this, &s); \
        vi.visit_for_serialization(rttr::type::get<classType>()); \
    } \
    RTTR_ENABLE(__VA_ARGS__) \
    RTTR_REGISTRATION_FRIEND \
private:


#define CC_STRUCT(classType) \
    template<typename S> \
    void serialize(S& s) { \
        serialization_visitor<S> vi(this, &s); \
        vi.visit_for_serialization(rttr::type::get<classType>()); \
    }



struct MyStructBase {
    CC_CLASS(MyStructBase)
public:

    void fooBase() {
        printf("MyStructBase::fooBase\n");
    }

    int myint{};
};

struct MyStruct {
    CC_CLASS(MyStruct)
public:
    MyStruct() {}
    MyStruct(uint32_t ai, MyEnum ae, const std::vector<float>& afs)
    : i(ai)
    , e(ae)
    , fs(afs)
    {}

    uint32_t get_i() const { return i; }
    MyEnum get_e() const { return e; }
    const std::vector<float> &get_fs() const { return fs; }
    int jjj{};
private:
    uint32_t i{};
    MyEnum e{};
    std::vector<float> fs;


//public:
//    template<typename S>
//    void serialize(S& s) {
//        serialization_visitor<S> vi(this, &s);
//        vi.visit(rttr::type::get<MyStruct>());
//    }
//
//    RTTR_ENABLE()
//    RTTR_REGISTRATION_FRIEND
};

struct MyStruct3 {
    CC_STRUCT(MyStruct3)
    std::string m_haha;
};

struct MySubStruct : public MyStruct, public MyStructBase {
    CC_CLASS(MySubStruct, MyStruct, MyStructBase)
public:
    MySubStruct() {}
    MySubStruct(uint32_t ai, MyEnum ae, const std::vector<float>& afs, const std::string& hello) : MyStruct(ai, ae, afs) {
        m_hello = hello;
    }

    const std::string& get_hello() const { return m_hello; }
    MyStruct3 m_mystruct;
private:
    std::string m_hello;

//public:
//    template<typename S>
//    void serialize(S& s) {
//        serialization_visitor<S> vi(this, &s);
//        vi.visit(rttr::type::get<MySubStruct>());
//    }
//
//
//    RTTR_ENABLE(MyStruct)
//    RTTR_REGISTRATION_FRIEND
};



//template<typename S>
//void serialize(S& s, MySubStruct& obj) {
//    serialization_visitor<S> vi(&obj, &s);
//    vi.visit(rttr::type::get<MySubStruct>());
//}


using namespace rttr;

int main(int argc, char** argv)
{
    type mystruct = rttr::type::get<MySubStruct>();
    auto inst = mystruct.create();

    auto prop_i = mystruct.get_property("i");
    assert(prop_i.is_valid());
    prop_i.set_value(inst, (uint32_t)123);

    auto prop_myint = mystruct.get_property("myint");
    assert(prop_myint.is_valid());
    prop_myint.set_value(inst, (int)10101);

    auto method_get_i = mystruct.get_method("get_i");
    assert(method_get_i.is_valid());
    variant myi = method_get_i.invoke(inst);
    assert(myi.is_valid());
    assert(myi.to_uint32() == 123);

    auto method_fooBase = mystruct.get_method("fooBase");
    assert(method_fooBase.is_valid());
    method_fooBase.invoke(inst);

    mystruct.destroy(inst);

    //set some random data
//    MyStruct data{8941, MyEnum::V2, {15.0f, -8.5f, 0.045f}};
//    MyStruct res{};

    MySubStruct data{8941, MyEnum::V2, {15.0f, -8.5f, 0.045f}, "world"};
    data.m_mystruct.m_haha = "hahaha";
    data.myint = 1234;
    MySubStruct res{};

    //serialization, deserialization flow is unchanged as in basic usage
    Buffer buffer;
    auto writtenSize = bitsery::quickSerialization<OutputAdapter>(buffer, data);

    auto state = bitsery::quickDeserialization<InputAdapter>({buffer.begin(), writtenSize}, res);

    assert(state.first == bitsery::ReaderError::NoError && state.second);
    assert(data.get_fs() == res.get_fs() && data.get_i() == res.get_i() && data.get_e() == res.get_e());
    assert(data.get_hello() == res.get_hello());
    assert(data.m_mystruct.m_haha == res.m_mystruct.m_haha);
    assert(data.myint == res.myint);



    return 0;
}

RTTR_REGISTRATION
{
    rttr::registration::class_<MyStructBase>("MyStructBase")
        .constructor<>()
        .property("myint", &MyStructBase::myint, registration::serialize_access)
        .method("fooBase", &MyStructBase::fooBase)
    ;

    rttr::registration::class_<MyStruct>("MyStruct")
        .constructor<>()
        .property("i", &MyStruct::i, registration::serialize_access)
        .method("get_i", &MyStruct::get_i)
        .property("e", &MyStruct::e, registration::serialize_access)
        .property("fs", &MyStruct::fs, registration::serialize_access)
        .property("jjj", &MyStruct::jjj)
    ;

    rttr::registration::class_<MyStruct3>("MyStruct3")
        .constructor<>()
        .property("m_haha", &MyStruct3::m_haha, registration::serialize_access)
    ;

    rttr::registration::class_<MySubStruct>("MySubStruct")
        .constructor<>()
        .constructor<uint32_t, MyEnum, std::vector<float>&& , std::string&& >()
        .property("m_hello", &MySubStruct::m_hello, registration::serialize_access)
        .property("m_mystruct", &MySubStruct::m_mystruct, registration::serialize_access)
    ;

}

