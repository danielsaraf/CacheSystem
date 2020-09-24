//
// Created by daniel saraf on 24/11/2019.
//
#ifndef EX2__EX2_H_
#define EX2__EX2_H_
#include <iostream>
#include <string>
#include <unordered_map>
#include <list>
#include <fstream>
using namespace std;

template<typename T>
class CacheManager {
  int capacity;
  int numOfElements = 0;
  list<pair<string, T>> cacheL;
  unordered_map<string, typename list<pair<string, T>>::iterator> cacheM;
 public:
  CacheManager(int c) : capacity(c) {}

  void insert(string key, T obj) {
    // ---------------insert or update the file system----------------
    ofstream outFile;
    outFile.open(obj.class_name + "_" + key, ios::binary);
    if (!outFile.is_open()) {
      throw "error of creating file";
    }
    outFile.write((char *) &obj, sizeof(T));
    outFile.close();

    // --------- insert or update the cache -------------
    if (cacheM.find(key) != cacheM.end()) {
      // exists value, delete it and re-enter it after for update
      cacheL.erase(cacheM[key]);
      cacheM.erase(key);
    }
      // new value, enter it to the cache
    else {
      // there is a space in the cache, increment the number of elements counter
      if (numOfElements < capacity)
        numOfElements++;
        // there is no space in the cache, delete the last object.
      else {
        auto it = cacheL.end();
        it--;
        cacheM.erase(it->first);
        cacheL.erase(it);
      }
    }
    cacheL.push_front(make_pair(key, obj));
    cacheM[key] = cacheL.begin();
  }
  T get(string key) {
    if (numOfElements == 0) throw "an error";
    // check if the key exists in the map
    if (cacheM.find(key) == cacheM.end()) {
      // key is not in the cache, check if its in the file system
      T obj = readFromFile(key, &obj);
      insert(key, obj);
      return obj;
    }
    // if it do exits, update LRU
    typename list<pair<string, T>>::iterator p = cacheM[key];
    cacheL.erase(cacheM[key]);
    cacheM.erase(key);
    cacheL.push_front(make_pair(p->first, p->second));
    cacheM[key] = cacheL.begin();
    return cacheM[key]->second;
  }

  T readFromFile(string key, T *obj) {
    // try to open file read mode
    fstream inFile(T::class_name + "_" + key, ios::in | ios::binary);
    if (!inFile) {
      throw "an error";
    }
    inFile.read((char *) &*obj, sizeof(*obj));
    inFile.close();
    return *obj;
  }
  template<class Operation>
  void foreach(Operation o) {
    for (pair<string, T> p : cacheL)
      o(p.second);
  }
};
#endif //EX2__EX2_H_
