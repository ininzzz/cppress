#include"Json.h"

struct JsonStatic {
    static const int int_static = 0;
    static const bool bool_static = false;
    static const std::string string_static;
    static const Json::array array_static;
    static const Json::object object_static;
    static Json json_static;
};
const std::string JsonStatic::string_static;
const Json::array JsonStatic::array_static;
const Json::object JsonStatic::object_static;
Json JsonStatic::json_static;

int JsonItem::int_value() const { return JsonStatic::int_static; }
bool JsonItem::bool_value() const { return JsonStatic::bool_static; }
const std::string &JsonItem::string_value() const { return JsonStatic::string_static; }
const Json::array &JsonItem::array_value() const { return JsonStatic::array_static; }
const Json::object &JsonItem::object_value() const { return JsonStatic::object_static; }
const Json &JsonItem::operator[](const std::string &key) const { return JsonStatic::json_static; }
const Json &JsonItem::JsonItem::operator[](int pos) const { return JsonStatic::json_static; }

class JsonNull : public JsonItem {
public:
    Json::Type type() const override { return Json::Type::NUL; }
    std::string dump() const override { return "null"; }
};

class JsonInt : public JsonItem {
public:
    JsonInt(int val) :m_value(val) {}
    Json::Type type() const override { return Json::Type::NUMBER; }
    std::string dump() const override { return std::to_string(m_value); }
    int int_value() const { return m_value; }
private:
    int m_value;
};

class JsonBool : public JsonItem {
public:
    JsonBool(bool val) :m_value(val) {}
    Json::Type type() const override { return Json::Type::BOOL; }
    std::string dump() const override { return m_value ? "true" : "false"; }
    bool bool_value() const { return m_value; }
private:
    bool m_value;
};

class JsonString : public JsonItem {
public:
    JsonString(const std::string &val) :m_value(std::move(val)) {}
    JsonString(std::string &&val) :m_value(val) {}
    Json::Type type() const override { return Json::Type::STRING; }
    std::string dump() const override { return "\"" + m_value + "\""; }
    const std::string &string_value() const override { return m_value; }
private:
    std::string m_value;
};

class JsonArray : public JsonItem {
public:
    JsonArray(const Json::array &val) :m_value(val) {}
    Json::Type type() const override { return Json::Type::ARRAY; }
    std::string dump() const override;
    const Json::array &array_value() const override { return m_value; }
    const Json &operator[](int pos) const override { return m_value[pos]; }
private:
    Json::array m_value;
};

std::string JsonArray::dump() const {
    std::string res;
    bool first = true;
    res += '[';
    for (auto &val : m_value) {
        if (first) first = false;
        else res += ", ";
        res += val.dump();
    }
    res += ']';
    return std::move(res);
}

class JsonObject : public JsonItem {
public:
    JsonObject(const Json::object &val) :m_value(val) {}
    Json::Type type() const override { return Json::Type::OBJECT; }
    std::string dump() const override;
    const Json::object &object_value() const override { return m_value; }
    const Json &operator[](const std::string &key) const override;
private:
    Json::object m_value;
};

std::string JsonObject::dump() const {
    std::string res;
    bool first = true;
    res += '{';
    for (auto &val : m_value) {
        if (first) first = false;
        else res += ", ";
        res += ("\"" + val.first + "\"");
        res += ": ";
        res += val.second.dump();
    }
    res += '}';
    return std::move(res);
}

const Json &JsonObject::operator[](const std::string &key) const {
    auto it = m_value.find(key);
    if (it == m_value.end()) return JsonStatic::json_static;
    return it->second;
}

Json::Json() :m_json(new JsonNull) {}
Json::Json(int val) : m_json(new JsonInt(val)) {}
Json::Json(bool val) : m_json(new JsonBool(val)) {}
Json::Json(const std::string &val) :m_json(new JsonString(val)) {}
Json::Json(std::string &&val) :m_json(new JsonString(val)) {}
Json::Json(const char *val) :m_json(new JsonString(val)) {}
// array
Json::Json(const array &val) :m_json(new JsonArray(val)) {}
// object
Json::Json(const object &val) :m_json(new JsonObject(val)) {}
Json::Type Json::type() const { return m_json->type(); }

int Json::int_value() const { return m_json->int_value(); }
bool Json::bool_value() const { return m_json->bool_value(); }
const std::string &Json::string_value() const { return m_json->string_value(); }
const Json::array &Json::array_items() const { return m_json->array_value(); }
const Json::object &Json::object_item() const { return m_json->object_value(); }

const Json &Json::operator[](const std::string &key) const { return m_json->operator[](key); }
const Json &Json::operator[](int pos) const { return m_json->operator[](pos); }

std::string Json::dump() const { return m_json->dump(); }

char JsonParser::next() {
    while (m_st < m_str.size() && isspace(m_str[m_st])) {
        m_st++;
    }
    if (m_st == m_str.size()) throw std::runtime_error("get next error\n");
    return m_str[m_st++];
}

Json JsonParser::parse() {
    char c = next();
    if (c == 'n') {
        m_st--;
        return parseNull();
    } else if (c == 't' || c == 'f') {
        m_st--;
        return parseBool();
    } else if (c == '-' || isdigit(c)) {
        m_st--;
        return parseInt();
    } else if (c == '"') {
        return parseString();
    } else if (c == '[') {
        Json::array ans;
        while (1) {
            ans.push_back(parse());
            char c = next();
            if (c == ',') continue;
            else if (c == ']') break;
        }
        return ans;
    } else if (c == '{') {
        Json::object ans;
        while (1) {
            char c = next();
            if (c != '"') throw std::runtime_error("expect key with \"");
            std::string key = std::move(parseString().string_value());
            c = next();
            if (c != ':') throw std::runtime_error("expect ':'");
            ans.insert({ key,parse() });
            c = next();
            if (c == ',') continue;
            else if (c == '}') break;
        }
        return ans;
    }
    throw std::runtime_error("unknown type");
}

Json JsonParser::parseInt() {
    char c = next();
    int ans = 0;
    bool neg = false;
    if (c == '-') neg = true;
    else ans = c - '0';
    while (isdigit(c = next())) {
        ans = ans * 10 + c - '0';
    }
    m_st--;
    if (neg) ans *= -1;
    return ans;
}

Json JsonParser::parseString() {
    std::string ans;
    char ch;
    while ((ch = m_str[m_st++]) != '"') {
        ans.push_back(ch);
    }
    return std::move(ans);
}

Json JsonParser::parseBool() {
    if (strncasecmp(m_str.c_str() + m_st, "true", 4) == 0) {
        m_st += 4;
        return true;
    } else if (strncasecmp(m_str.c_str() + m_st, "false", 5) == 0) {
        m_st += 5;
        return false;
    }
    throw std::runtime_error("expect true or false");
}

Json JsonParser::parseNull() {
    if (strncasecmp(m_str.c_str() + m_st, "null", 4) == 0) {
        m_st += 4;
        return Json();
    }
    throw std::runtime_error("expect null");
}

