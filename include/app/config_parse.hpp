#pragma once

#include "app/lua.hpp"
#include "core/pch.hpp"
#include "game/types.hpp"

enum class ParseErrorType {
    NoError,
    IOError,
    ParseError,
};

struct IOError final {
    std::string desc;
};

struct ParseError final {
    std::vector<std::string_view>
        nodePath;  // parse stack contain node path in config file
    std::string desc;

    enum class Type {
        Unknown,
        FieldNotFound,
        SyntaxError,
        TypeIncorrect,
        Custom,
    } type = Type::Unknown;

    static std::string_view Type2Str(Type type) {
#define CASE(e)   \
    case Type::e: \
        return #e;
        switch (type) {
            CASE(Unknown);
            CASE(FieldNotFound);
            CASE(SyntaxError);
            CASE(TypeIncorrect);
            CASE(Custom);
            default:
                return "Unknwon";
        }
#undef CASE
    }
};

#define MemberAssignmentFunc(classtype, member, instance, value)              \
    [&](auto& data) {                                                         \
        if constexpr (std::is_same_v<                                         \
                          std::remove_reference_t<decltype(data)>::pointer_t, \
                          decltype(&classtype::member)>) {                    \
            if (#member == data.Name()) instance.*(data.Pointer()) = value;   \
        }                                                                     \
    }

#define DeclareParseFunc(classtype)                   \
    std::optional<classtype> Parse ## classtype ## (const sol::table& root) { \
        classtype instance;                                 \
        auto classinfo = refl::GetClass<classtype>();

#define EndDeclareParseFunc() \
    return instance;          \
    }

#define Field(name, mtype)                                              \
    {                                                                   \
        auto field = root.get<std::optional<mtype>>(#name);             \
        if (!field.has_value()) return std::nullopt;                    \
        classinfo.VisitMembers(MemberAssignmentFunc(                    \
            decltype(classinfo)::type, name, instance, field.value())); \
    }

#define ArrayField(name, mtype, count)                                         \
    {                                                                          \
        auto field = root.get<std::optional<std::array<mtype, count>>>(#name); \
        if (!field.has_value()) return std::nullopt;                           \
        classinfo.VisitMembers(MemberAssignmentFunc(                           \
            decltype(classinfo)::type, name, instance, field.value()));        \
    }

#define DynArrayField(name, mtype)                                       \
    {                                                                    \
        auto field = root.get<std::optional<std::vector<mtype>>>(#name); \
        if (!field.has_value()) return std::nullopt;                     \
        classinfo.VisitMembers(MemberAssignmentFunc(                     \
            decltype(classinfo)::type, name, instance, field.value()));  \
    }

#define ObjField(name, mtype)                                            \
    {                                                                    \
        auto table = root.get<std::optional<sol::table>>(#name);         \
        if (!table.has_value()) return std::nullopt;                     \
        auto member = Parse##mtype##(table.value());                     \
        if (!member.has_value()) return std::nullopt;                    \
        classinfo.VisitMembers(MemberAssignmentFunc(                     \
            decltype(classinfo)::type, name, instance, member.value())); \
    }
