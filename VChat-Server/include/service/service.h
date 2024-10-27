/*
 * Copyright (c) 2024 Hunter 执着
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

#ifndef SERVICE_H
#define SERVICE_H

#include "common.h"
#include "connection.h"
#include "package.h"
#include "store.h"

class Connection;

class Service {
private:
  Service();
  static Service* service;
  static struct Cache {
    std::map<int, Connection*> online;
    std::map<int, std::vector<Json::Value>> addfriendapply;
  } cache;
  // serve
  void login(Json::Value, Connection*);
  //void logout(Json::Value, Connection*);
  void signin(Json::Value, Connection*);
  //void signout(Json::Value, Connection*);
  void sendmsg(Json::Value);
  void find(Json::Value, Connection*);
  void addfriend(Json::Value, Connection*);
  void addfriend(int, Json::Value, Connection*);
  //void deletefriend(Json::Value, Connection*);

public:
  static void getinstance();
  Service(const Service&) = delete;
  Service& operator=(const Service&) = delete;
  static void serve(int, Json::Value, Connection*);

};

#endif // SERVICE_H

