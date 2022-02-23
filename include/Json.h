#ifndef JSON_H
#define JSON_H

#include<string>
#include<cstring>
#include<vector>
#include<map>
#include<memory>
#include<initializer_list>

class JsonItem;

class Json {
public:
    enum class Type {
        NUL, NUMBER, BOOL, STRING, ARRAY, OBJECT
    };

    using ptr = std::shared_ptr <Json>;
    using array = std::vector<Json>;
    using object = std::map<std::string, Json>;

    // null
    Json();
    // number
    Json(int val);
    // bool
    Json(bool val);
    // string
    Json(const std::string &val);
    Json(std::string &&val);
    Json(const char *val);
    // array
    Json(const array &val);
    // object
    Json(const object &val);

    Type type() const;
    
    bool is_null()      const { return type() == Type::NUL; }
    bool is_number()    const { return type() == Type::NUMBER; }
    bool is_bool()      const { return type() == Type::BOOL; }
    bool is_string()    const { return type() == Type::STRING; }
    bool is_array()     const { return type() == Type::ARRAY; }
    bool is_object()    const { return type() == Type::OBJECT; }

    int int_value() const;
    bool bool_value() const;
    const std::string &string_value() const;
    const array &array_items() const;
    const object &object_item() const;

    const Json &operator[](const std::string &key) const;
    const Json &operator[](int pos) const;

    std::string dump() const;

private:
    std::shared_ptr<JsonItem> m_json;
};

class JsonItem {
public:
    using ptr = std::shared_ptr<JsonItem>;
    virtual Json::Type type() const = 0;
    virtual std::string dump() const = 0;

    virtual int int_value() const;
    virtual bool bool_value() const;
    virtual const std::string &string_value() const;
    virtual const Json::array &array_value() const;
    virtual const Json::object &object_value() const;

    virtual const Json &operator[](const std::string &key) const;
    virtual const Json &operator[](int pos) const;

    virtual ~JsonItem() {};
};


class JsonParser {
public:
    using ptr = std::shared_ptr<JsonParser>;
    JsonParser(const std::string &str) :m_str(str), m_st(0) {}
    void set(const std::string &str) { m_str = str; }
    Json parse();
private:
    char next();
    Json parseNull();
    Json parseInt();
    Json parseBool();
    Json parseString();
private:
    int m_st;
    std::string m_str;
};

#endif
