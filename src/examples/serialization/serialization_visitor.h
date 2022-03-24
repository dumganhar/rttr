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

#ifndef SERIALIZATION_VISITOR_H_
#define SERIALIZATION_VISITOR_H_



#include <rttr/visitor.h>

#include <bitsery/bitsery.h>
#include <bitsery/bitsery.h>
#include <bitsery/adapter/buffer.h>
//to use brief syntax always include this header
#include <bitsery/brief_syntax.h>
//we also need additional traits to work with container types,
//instead of including <bitsery/traits/vector.h> for vector traits, now we also need traits to work with brief_syntax types.
//so include everything from <bitsery/brief_syntax/...> instead of <bitsery/traits/...>
//otherwise we'll get static assert error, saying to define serialize function.
#include <bitsery/brief_syntax/vector.h>




//some helper types
using Buffer = std::vector<uint8_t>;
using OutputAdapter = bitsery::OutputBufferAdapter<Buffer>;
using InputAdapter = bitsery::InputBufferAdapter<Buffer>;

#define CC_SERIALIZATION_VISITOR_PROPERTIES(OutputAdapterType, InputAdapterType) \
    bitsery::Serializer<OutputAdapterType>* _serializer_##OutputAdapterType{}; \
    bitsery::Deserializer<InputAdapterType>* _deserializer_##InputAdapterType{};


#define CC_SERIALIZATION_VISITOR_PROPERTIES_HANDLER(OutputAdapterType, InputAdapterType) \
    switch (_serializeType) { \
        case bitsery::SerializeType::SERIALIZE: \
            (*_serializer_##OutputAdapterType)(property); \
            break; \
        case bitsery::SerializeType::DESERIALIZE: \
            (*_deserializer_##InputAdapterType)(property); \
            break; \
        default: \
            break; \
    }

template<typename S>
class serialization_visitor : public rttr::visitor
{
    S* _s{};
    void* _instance{};
//    bitsery::SerializeType _serializeType{bitsery::SerializeType::NONE};
public:
////    CC_SERIALIZATION_VISITOR_PROPERTIES(OutputAdapter, InputAdapter)
//
//    bitsery::Serializer<OutputAdapter>* _serializer_OutputAdapter{}; \
//    bitsery::Deserializer<InputAdapter>* _deserializer_InputAdapter{};

//    serialization_visitor(void* instance, bitsery::SerializeType stype)
//    : _instance(instance)
//    , _serializeType(stype)
//    {
//
//    }
    serialization_visitor(void* instance, S* s)
    : _instance(instance)
//    , _serializeType(stype)
    , _s(s)
    {

    }

    /////////////////////////////////////////////////////////////////////////////////////

    template<typename Derived>
    void iterate_base_classes()
    {
    }

    template<typename Derived, typename Base_Class, typename...Base_Classes>
    void iterate_base_classes()
    {
        // m_chai.add(chaiscript::base_class<Base_Class, Derived>());
        iterate_base_classes<Derived, Base_Classes...>();
    }

    /////////////////////////////////////////////////////////////////////////////////////

    template<typename T, typename...Base_Classes>
    void visit_type_begin(const type_info<T>& info)
    {
        using declaring_type_t = typename type_info<T>::declaring_type;
        iterate_base_classes<declaring_type_t, Base_Classes...>();
    }

    /////////////////////////////////////////////////////////////////////////////////////

    template<typename T, typename...Ctor_Args>
    void visit_constructor(const constructor_info<T>& info)
    {
        using declaring_type_t = typename constructor_info<T>::declaring_type;
        // m_chai.add(chaiscript::constructor<declaring_type_t(Ctor_Args...)>(), get_type_name<declaring_type_t>());
    }

    /////////////////////////////////////////////////////////////////////////////////////

    template<typename T>
    void visit_global_method(const method_info<T>& info)
    {
        // m_chai.add(chaiscript::fun(info.function_ptr), info.method_item.get_name().to_string());
    }

    /////////////////////////////////////////////////////////////////////////////////////

    template<typename T>
    void visit_method(const method_info<T>& info)
    {
        // m_chai.add(chaiscript::fun(info.function_ptr), info.method_item.get_name().to_string());
    }

    /////////////////////////////////////////////////////////////////////////////////////
    template<typename T>
    void visit_property(const property_info<T>& info)
    {
        using InstanceType = typename property_info<T>::declaring_type;
        auto* instance = static_cast<InstanceType*>(_instance);

        auto& property = instance->*info.property_accessor;

//        CC_SERIALIZATION_VISITOR_PROPERTIES_HANDLER(OutputAdapter, InputAdapter)

//        switch (_serializeType) {
//            case bitsery::SerializeType::SERIALIZE:
//                (*_serializer_OutputAdapter)(property);
//                break;
//            case bitsery::SerializeType::DESERIALIZE:
//                (*_deserializer_InputAdapter)(property);
//                break;
//            default:
//                break;
//        }
        (*_s)(property);
    }

    template<typename T>
    void visit_getter_setter_property(const property_getter_setter_info<T>& info)
    {
        // m_chai.add(chaiscript::fun(info.property_getter), std::string("get_") + info.property_item.get_name().to_string());
        // m_chai.add(chaiscript::fun(info.property_setter), std::string("set_") + info.property_item.get_name().to_string());
    }

    template<typename T>
    void visit_readonly_property(const property_info<T>& info)
    {
        // m_chai.add(chaiscript::fun(info.property_accessor), info.property_item.get_name().to_string());
    }

    /////////////////////////////////////////////////////////////////////////////////////

private:
    template<typename T>
    static std::string get_type_name()
    {
        return rttr::type::template get<T>().get_name().to_string();
    }

private:

    RTTR_ENABLE(visitor) // Important!! Otherwise the object instance cannot be casted from "visitor" to "chai_script_binding_visitor"
};

using my_serialization_visitor = serialization_visitor<bitsery::Serializer<OutputAdapter>>;
using my_deserialization_visitor = serialization_visitor<bitsery::Deserializer<InputAdapter>>;

RTTR_REGISTER_VISITOR(my_serialization_visitor); // Important!!
RTTR_REGISTER_VISITOR(my_deserialization_visitor); // Important!!
                                           // In order to make the visitor available during registration




#endif  // SERIALIZATION_VISITOR_H_
