#include"Json.h"

struct JsonStatic {
    static const int int_static;
    static const double double_static;
    static const bool bool_static;
    static const std::string string_static;
    static const Json::array array_static;
    static const Json::object object_static;
    static Json json_static;
};
const int JsonStatic::int_static = 0;
const double JsonStatic::double_static = 0;
const bool JsonStatic::bool_static = false;
const std::string JsonStatic::string_static;
const Json::array JsonStatic::array_static;
const Json::object JsonStatic::object_static;
Json JsonStatic::json_static;

int JsonItem::int_value() const { return JsonStatic::int_static; }
double JsonItem::double_value() const { return JsonStatic::double_static; }
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
    bool equal(const JsonItem *rhs) const override { return true; }
};

class JsonInt : public JsonItem {
public:
    JsonInt(int val) :m_value(val) {}
    Json::Type type() const override { return Json::Type::NUMBER; }
    std::string dump() const override { return std::to_string(m_value); }
    int int_value() const { return m_value; }
    bool equal(const JsonItem *rhs) const override { return m_value == rhs->int_value(); }
private:
    int m_value;
};

class JsonDouble : public JsonItem {
public:
    JsonDouble(double val) :m_value(val) {}
    Json::Type type() const override { return Json::Type::NUMBER; }
    std::string dump() const override { return std::to_string(m_value); }
    double double_value() const { return m_value; }
    bool equal(const JsonItem *rhs) const override { return m_value == rhs->double_value(); }
private:
    double m_value;
};

class JsonBool : public JsonItem {
public:
    JsonBool(bool val) :m_value(val) {}
    Json::Type type() const override { return Json::Type::BOOL; }
    std::string dump() const override { return m_value ? "true" : "false"; }
    bool bool_value() const { return m_value; }
    bool equal(const JsonItem *rhs) const override { return m_value == rhs->bool_value(); }
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
    bool equal(const JsonItem *rhs) const override { return m_value == rhs->string_value(); }
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
    bool equal(const JsonItem *rhs) const override { return m_value == rhs->array_value(); }
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
    return res;
}

class JsonObject : public JsonItem {
public:
    JsonObject(const Json::object &val) :m_value(val) {}
    Json::Type type() const override { return Json::Type::OBJECT; }
    std::string dump() const override;
    const Json::object &object_value() const override { return m_value; }
    const Json &operator[](const std::string &key) const override;
    bool equal(const JsonItem *rhs) const override { return m_value == rhs->object_value(); }
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
    return res;
}

const Json &JsonObject::operator[](const std::string &key) const {
    auto it = m_value.find(key);
    if (it == m_value.end()) return JsonStatic::json_static;
    return it->second;
}

Json::Json() :m_json(new JsonNull) {}
Json::Json(int val) : m_json(new JsonInt(val)) {}
Json::Json(double val) : m_json(new JsonDouble(val)) {}
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
double Json::double_value() const { return m_json->double_value(); }
bool Json::bool_value() const { return m_json->bool_value(); }
const std::string &Json::string_value() const { return m_json->string_value(); }
const Json::array &Json::array_items() const { return m_json->array_value(); }
const Json::object &Json::object_item() const { return m_json->object_value(); }

const Json &Json::operator[](const std::string &key) const { return m_json->operator[](key); }
const Json &Json::operator[](int pos) const { return m_json->operator[](pos); }

bool Json::operator==(const Json &rhs) const {
    if (m_json == rhs.m_json) return true;
    if (type() != rhs.type()) return false;
    return m_json->equal(rhs.m_json.get());
}

std::string Json::dump() const { return m_json->dump(); }

char JsonParser::next() {
    while (m_st < m_str.size() && isspace(m_str[m_st])) {
        m_st++;
    }
    if (m_st == m_str.size()) return '\0';
    return m_str[m_st++];
}

Json JsonParser::parse() {
    if (m_str.size() == 0) return Json{};
    char c = next();
    if (c == 'n') {
        m_st--;
        return parseNull();
    } else if (c == 't' || c == 'f') {
        m_st--;
        return parseBool();
    } else if (c == '-' || isdigit(c)) {
        m_st--;
        return parseNumber();
    } else if (c == '"') {
        return parseString();
    } else if (c == '[') {
        Json::array ans;
        while (true) {
            if (m_str[m_st] == ']') { m_st++; break; }
            ans.push_back(parse());
            char c = next();
            if (c == ',') continue;
            else if (c == ']') break;
        }
        return ans;
    } else if (c == '{') {
        Json::object ans;
        while (true) {
            if (m_str[m_st] == '}') { m_st++; break; }
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

Json JsonParser::parseNumber() {
    char c = next();
    int ans = 0;
    bool neg = false;
    if (c == '-') neg = true;
    else ans = c - '0';
    while (isdigit(c = next())) {
        ans = ans * 10 + c - '0';
    }
    bool is_double = false;
    int dans = 0;
    double dcnt = 1;
    if (c == '.') {
        is_double = true;
        while (isdigit(c = next())) {
            dans = dans * 10 + c - '0';
            dcnt *= 0.1;
        }
    }
    m_st--;
    if (neg) ans *= -1, dans *= -1;
    if (is_double) return ans + dans * dcnt;
    else return ans;
}

Json JsonParser::parseString() {
    std::string ans;
    char ch;
    while ((ch = m_str[m_st++]) != '"') {
        ans.push_back(ch);
    }
    return ans;
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

