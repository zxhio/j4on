// File:    j4on.hh
// Author:  definezxh@163.com
// Date:    2019/07/11 23:17:55
// Desc:
//   Light weight json parser of C++ implementation.

#pragma once

#include <string_view>
#include <string>
#include <vector>
#include <any>
#include <map>
#include <memory>

namespace j4on {

enum ValueType {
    kNull,
    kFalse,
    kTrue,
    kNumber,
    kString,
    kArray,
    kObject,
    kUnknown
};

const char *typeToString(ValueType type);

// general Value structure
class Value {
  public:
    Value() : type_(kUnknown) {}
    Value(ValueType type, std::any value) : type_(type), value_(value) {}
    ValueType type() const { return type_; }
    std::any getAnyValue() const { return value_; }
    void setAnyValue(std::any value) { value_ = value; }

  private:
    ValueType type_;
    std::any value_;
};

class Literal {
  public:
    explicit Literal(const char *liteal, size_t n) : literal_(liteal, n) {}
    std::string_view getLiteral() { return literal_; };

  private:
    std::string_view literal_;
};

class Number {
  public:
    Number(double number) : number_(number) {}
    double getNumber() const { return number_; }

  private:
    double number_;
};

class String {
  public:
    String(const char *str, size_t n) : str_(str, n) {}
    std::string_view getString() { return str_; }

  private:
    std::string_view str_;
};

class Array {
  public:
    Array() {}
    Value getValueByIndex(size_t index) const { return values_[index]; }
    void add(Value v) { values_.push_back(v); }

  private:
    std::vector<Value> values_;
};

class Object {
  public:
    Object(){};
    Object(std::pair<std::string_view, Value> member) : members_{member} {};
    Value getValueByKey(std::string_view key) { return members_[key]; }

    void add(std::pair<std::string_view, Value> member) {
        members_.emplace(member);
    }

  private:
    // <key, value>
    std::map<std::string_view, Value> members_;
};

// JSON
//   - element
// element
//   - ws value ws
// elements
//   - element ',' elements
// value
//   - object
//   - array
//   - string
//   - number
//   - "true"
//   - "false"
//   - "null"
// array
//   - '[' ws ']'
//   - '[' elements ']'
// object
//   - '{' ws '}'
//   - '{ members '}'
// members
//   - member
//   - member ',' members
// member
//   - ws string ws ':' element

// string, number, true, false, null is leaf node.
// Parse JSON in the above order.
class J4onParser {
  public:
    J4onParser(const char *filename);

    template <typename T> void check(T expect, T actual);
    template <typename T> void check(bool t, T actual, const char *msg);

    char *beginParse() { return &(context_.get()[index_]); }
    uint32_t getTokenIndex() const { return index_; }
    char getCurrToken() const { return context_.get()[index_]; }
    char getNextToken();

    // Start parse JSON.
    void parse();

    // Parse Value
    // @return is value. if it's not root value, we need take it to array or
    // object. when return as reference or pointer, value's lifetime will be end
    // at function execuation completion.
    Value parseValue();
    Value parseLiteral(const char *literal, ValueType type, size_t n);
    Value parseNumber();
    Value parseString();
    // Value *parseArray();
    // Value *praseObject();

    // Intermediate Parse part.
    // Some parts will be deleted later.
    void parseWhitespace();
    Value parseElement();
    void parseElements(Array &array);
    std::pair<std::string_view, Value> parseMember();
    void parseMembers(Object &obj);

    Value getRootValue() const { return *rootValue_.get(); }

  private:
    uint32_t index_;
    uint32_t row_;
    uint32_t column_;
    uint32_t length_;
    std::unique_ptr<char[]> context_;
    std::unique_ptr<Value> rootValue_;
};

} // namespace j4on