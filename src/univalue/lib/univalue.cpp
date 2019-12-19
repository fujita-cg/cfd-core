// Copyright 2014 BitPay Inc.
// Copyright 2015 Bitcoin Core Developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or https://opensource.org/licenses/mit-license.php.

#include <stdint.h>
#include <iomanip>
#include <sstream>
#include <stdlib.h>

#include "univalue.h"

const UniValue NullUniValue;

UniValue::UniValue() { typ = VNULL; }
UniValue::UniValue(UniValue::VType initialType, const std::string& initialStr) {
    typ = initialType;
    val = initialStr;
}
UniValue::UniValue(uint64_t val_) {
    setInt(val_);
}
UniValue::UniValue(int64_t val_) {
    setInt(val_);
}
UniValue::UniValue(bool val_) {
    setBool(val_);
}
UniValue::UniValue(int val_) {
    setInt(val_);
}
UniValue::UniValue(double val_) {
    setFloat(val_);
}
UniValue::UniValue(const std::string& val_) {
    setStr(val_);
}
UniValue::UniValue(const char *val_) {
    std::string s(val_);
    setStr(s);
}
UniValue::~UniValue() {}

bool UniValue::setInt(int val_) { return setInt((int64_t)val_); }

enum UniValue::VType UniValue::getType() const { return typ; }
const std::string& UniValue::getValStr() const { return val; }
bool UniValue::empty() const { return (values.size() == 0); }

size_t UniValue::size() const { return values.size(); }

bool UniValue::getBool() const { return isTrue(); }

bool UniValue::exists(const std::string& key) const { size_t i; return findKey(key, i); }

bool UniValue::isNull() const { return (typ == VNULL); }
bool UniValue::isTrue() const { return (typ == VBOOL) && (val == "1"); }
bool UniValue::isFalse() const { return (typ == VBOOL) && (val != "1"); }
bool UniValue::isBool() const { return (typ == VBOOL); }
bool UniValue::isStr() const { return (typ == VSTR); }
bool UniValue::isNum() const { return (typ == VNUM); }
bool UniValue::isArray() const { return (typ == VARR); }
bool UniValue::isObject() const { return (typ == VOBJ); }
bool UniValue::push_back(const std::string& val_) {
    UniValue tmpVal(VSTR, val_);
    return push_back(tmpVal);
}
bool UniValue::push_back(const char *val_) {
    std::string s(val_);
    return push_back(s);
}
bool UniValue::push_back(uint64_t val_) {
    UniValue tmpVal(val_);
    return push_back(tmpVal);
}
bool UniValue::push_back(int64_t val_) {
    UniValue tmpVal(val_);
    return push_back(tmpVal);
}
bool UniValue::push_back(int val_) {
    UniValue tmpVal(val_);
    return push_back(tmpVal);
}
bool UniValue::push_back(double val_) {
    UniValue tmpVal(val_);
    return push_back(tmpVal);
}
bool UniValue::pushKV(const std::string& key, const std::string& val_) {
    UniValue tmpVal(VSTR, val_);
    return pushKV(key, tmpVal);
}
bool UniValue::pushKV(const std::string& key, const char *val_) {
    std::string _val(val_);
    return pushKV(key, _val);
}
bool UniValue::pushKV(const std::string& key, int64_t val_) {
    UniValue tmpVal(val_);
    return pushKV(key, tmpVal);
}
bool UniValue::pushKV(const std::string& key, uint64_t val_) {
    UniValue tmpVal(val_);
    return pushKV(key, tmpVal);
}
bool UniValue::pushKV(const std::string& key, bool val_) {
    UniValue tmpVal((bool)val_);
    return pushKV(key, tmpVal);
}
bool UniValue::pushKV(const std::string& key, int val_) {
    UniValue tmpVal((int64_t)val_);
    return pushKV(key, tmpVal);
}
bool UniValue::pushKV(const std::string& key, double val_) {
    UniValue tmpVal(val_);
    return pushKV(key, tmpVal);
}
bool UniValue::read(const char *raw) { return read(raw, strlen(raw)); }
bool UniValue::read(const std::string& rawStr) {
    return read(rawStr.data(), rawStr.size());
}
enum UniValue::VType UniValue::type() const { return getType(); }
bool UniValue::push_back(std::pair<std::string,UniValue> pear) {
    return pushKV(pear.first, pear.second);
}

void UniValue::clear()
{
    typ = VNULL;
    val.clear();
    keys.clear();
    values.clear();
}

bool UniValue::setNull()
{
    clear();
    return true;
}

bool UniValue::setBool(bool val_)
{
    clear();
    typ = VBOOL;
    if (val_)
        val = "1";
    return true;
}

static bool validNumStr(const std::string& s)
{
    std::string tokenVal;
    unsigned int consumed;
    enum jtokentype tt = getJsonToken(tokenVal, consumed, s.data(), s.data() + s.size());
    return (tt == JTOK_NUMBER);
}

bool UniValue::setNumStr(const std::string& val_)
{
    if (!validNumStr(val_))
        return false;

    clear();
    typ = VNUM;
    val = val_;
    return true;
}

bool UniValue::setInt(uint64_t val_)
{
    std::ostringstream oss;

    oss << val_;

    return setNumStr(oss.str());
}

bool UniValue::setInt(int64_t val_)
{
    std::ostringstream oss;

    oss << val_;

    return setNumStr(oss.str());
}

bool UniValue::setFloat(double val_)
{
    std::ostringstream oss;

    oss << std::setprecision(16) << val_;

    bool ret = setNumStr(oss.str());
    typ = VNUM;
    return ret;
}

bool UniValue::setStr(const std::string& val_)
{
    clear();
    typ = VSTR;
    val = val_;
    return true;
}

bool UniValue::setArray()
{
    clear();
    typ = VARR;
    return true;
}

bool UniValue::setObject()
{
    clear();
    typ = VOBJ;
    return true;
}

bool UniValue::push_back(const UniValue& val_)
{
    if (typ != VARR)
        return false;

    values.push_back(val_);
    return true;
}

bool UniValue::push_backV(const std::vector<UniValue>& vec)
{
    if (typ != VARR)
        return false;

    values.insert(values.end(), vec.begin(), vec.end());

    return true;
}

void UniValue::__pushKV(const std::string& key, const UniValue& val_)
{
    keys.push_back(key);
    values.push_back(val_);
}

bool UniValue::pushKV(const std::string& key, const UniValue& val_)
{
    if (typ != VOBJ)
        return false;

    size_t idx;
    if (findKey(key, idx))
        values[idx] = val_;
    else
        __pushKV(key, val_);
    return true;
}

bool UniValue::pushKVs(const UniValue& obj)
{
    if (typ != VOBJ || obj.typ != VOBJ)
        return false;

    for (size_t i = 0; i < obj.keys.size(); i++)
        __pushKV(obj.keys[i], obj.values.at(i));

    return true;
}

void UniValue::getObjMap(std::map<std::string,UniValue>& kv) const
{
    if (typ != VOBJ)
        return;

    kv.clear();
    for (size_t i = 0; i < keys.size(); i++)
        kv[keys[i]] = values[i];
}

bool UniValue::findKey(const std::string& key, size_t& retIdx) const
{
    for (size_t i = 0; i < keys.size(); i++) {
        if (keys[i] == key) {
            retIdx = i;
            return true;
        }
    }

    return false;
}

bool UniValue::checkObject(const std::map<std::string,UniValue::VType>& t) const
{
    if (typ != VOBJ)
        return false;

    for (std::map<std::string,UniValue::VType>::const_iterator it = t.begin();
         it != t.end(); ++it) {
        size_t idx = 0;
        if (!findKey(it->first, idx))
            return false;

        if (values.at(idx).getType() != it->second)
            return false;
    }

    return true;
}

const UniValue& UniValue::operator[](const std::string& key) const
{
    if (typ != VOBJ)
        return NullUniValue;

    size_t index = 0;
    if (!findKey(key, index))
        return NullUniValue;

    return values.at(index);
}

const UniValue& UniValue::operator[](size_t index) const
{
    if (typ != VOBJ && typ != VARR)
        return NullUniValue;
    if (index >= values.size())
        return NullUniValue;

    return values.at(index);
}

const char *uvTypeName(UniValue::VType t)
{
    switch (t) {
    case UniValue::VNULL: return "null";
    case UniValue::VBOOL: return "bool";
    case UniValue::VOBJ: return "object";
    case UniValue::VARR: return "array";
    case UniValue::VSTR: return "string";
    case UniValue::VNUM: return "number";
    }

    // not reached
    return NULL;
}

const UniValue& find_value(const UniValue& obj, const std::string& name)
{
    for (unsigned int i = 0; i < obj.keys.size(); i++)
        if (obj.keys[i] == name)
            return obj.values.at(i);

    return NullUniValue;
}

