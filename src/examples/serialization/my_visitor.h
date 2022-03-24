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

#ifndef MY_VISITOR_H_
#define MY_VISITOR_H_

#include <rttr/visitor.h>


class my_visitor : public rttr::visitor
{
    void* _instance{};
public:

    my_visitor(void* instance)
    : _instance(instance)
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

    }

    /////////////////////////////////////////////////////////////////////////////////////

    template<typename T, typename...Base_Classes>
    void visit_type_begin(const type_info<T>& info)
    {

    }

    /////////////////////////////////////////////////////////////////////////////////////

    template<typename T, typename...Ctor_Args>
    void visit_constructor(const constructor_info<T>& info)
    {

    }

    /////////////////////////////////////////////////////////////////////////////////////

    template<typename T>
    void visit_global_method(const method_info<T>& info)
    {

    }

    /////////////////////////////////////////////////////////////////////////////////////

    template<typename T>
    void visit_method(const method_info<T>& info)
    {

    }

    /////////////////////////////////////////////////////////////////////////////////////
    template<typename T>
    void visit_property(const property_info<T>& info)
    {
        using InstanceType = typename property_info<T>::declaring_type;
        auto* instance = static_cast<InstanceType*>(_instance);

        auto& property = instance->*info.property_accessor;


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


RTTR_REGISTER_VISITOR(my_visitor); // Important!!
                                           // In order to make the visitor available during registration

#endif
