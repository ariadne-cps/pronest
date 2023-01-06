/***************************************************************************
 *            container.hpp
 *
 *  Copyright  2023  Luca Geretti
 *
 ****************************************************************************/

/*
 * This file is part of ProNest, under the MIT license.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is furnished
 * to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS
 * FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR
 * COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
 * IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION
 * WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 */

/*! \file container.hpp
 *  \brief
 */



#ifndef PRONEST_CONTAINER_HPP
#define PRONEST_CONTAINER_HPP

#include <vector>
#include <map>
#include <set>
#include "declarations.hpp"

namespace ProNest {

//! \brief List is actually a vector
template<class T> using List = std::vector<T>;

template<class T> OutputStream& operator<<(OutputStream& os, const List<T>& v) {
    bool first=true;
    for(auto x : v) {
        os << (first ? "[" : ",") << x;
        first = false;
    }
    if(first) { os << "["; }
    return os << "]";
}

template<class K, class T> class Map : public std::map<K,T> {
  public:
    typedef typename std::map<K,T>::iterator Iterator;
    typedef typename std::map<K,T>::const_iterator ConstIterator;
    using std::map<K,T>::map;
    using std::map<K,T>::insert;
    T& operator[](K k) { return this->std::map<K,T>::operator[](k); }
    const T& operator[](K k) const { auto iter=this->find(k); PRONEST_ASSERT(iter!=this->end()); return iter->second; }
    const T& get(const K& k) const { auto i=this->find(k);
        PRONEST_ASSERT(i!=this->end()); return i->second; }
    bool has_key(const K& k) const {
        return this->find(k)!=this->end(); }
    T& value(const K& k) {
        auto iter=this->find(k);
        PRONEST_ASSERT(iter!=this->end()); return iter->second; }
    const T& value(const K& k) const {
        auto iter=this->find(k);
        PRONEST_ASSERT(iter!=this->end()); return iter->second; }
    void insert(const std::pair<K,T>& kv) {
        this->std::map<K,T>::insert(kv); }
    void insert(const K& k, const T& v) {
        this->std::map<K,T>::insert(std::make_pair(k,v)); }
    void adjoin(const std::map<K,T>& m) {
        for(auto i=m.begin(); i!=m.end(); ++i) { this->insert(*i); } }
    List<T> values() const {
        List<T> res; for(auto iter=this->begin(); iter!=this->end(); ++iter) {
            res.push_back(iter->second); } return res; }
};

template<class K, class T> OutputStream& operator<<(OutputStream& os, const std::map<K,T>& m) {
    bool first=true;
    for(auto x : m) {
        os << (first ? "{ " : ", ") << x.first << ":" << x.second;
        first = false;
    }
    if(first) { os << "{"; }
    return os << " }";
}

template<class T> class Set : public std::set<T> {
  public:
    Set() : std::set<T>() { }
    Set(const InitializerList<T>& s) : std::set<T>(s.begin(),s.end()) { }
    Set(const List<T>& s) : std::set<T>(s.begin(),s.end()) { }
    bool contains(const T& t) const {
        return this->find(t)!=this->end(); }
    Set<T>& adjoin(const std::set<T>& s) {
        for(auto iter=s.begin(); iter!=s.end(); ++iter) { this->insert(*iter); } return *this; }
    Set<T>& remove(const T& t) {
        auto iter=this->find(t);
        if(iter!=this->end()) { this->erase(iter); }
        return *this; }
};

template<class T> OutputStream& operator<<(OutputStream& os, const Set<T>& v) {
    bool first=true;
    for(auto x : v) {
        os << (first ? "{" : ",") << x;
        first = false;
    }
    if(first) { os << "{"; }
    return os << "}";
}

} // namespace ProNest

#endif // PRONEST_CONTAINER_HPP
