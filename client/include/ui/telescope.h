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

#ifndef TELESCOPE_H
#define TELESCOPE_H

#include "function.h"
#include "ui.h"

using namespace ftxui;

extern ScreenInteractive* screen;
extern std::unique_ptr<Function> function;

enum { COMMON = 0, MYSELF, CHAT, FRIEND, GROUP };
enum { LIST = 0, INPUT };

class Vchat;
class Telescope;

class Common {
public:
  int selected;
  Common(Telescope*);
  Component content;
  std::unordered_map<int, Element> previews;
};

class Chats {
private:
  Telescope* telescope;
  std::pair<int, Component> addlist(FriendInfo);
 
public:
  std::vector<std::pair<int, Component>> list;
  void refresh();
  int selected;
  Chats(Telescope*);
  Component content;
  std::unordered_map<int, Element> previews;

};

class Friends {
private:
  Telescope* telescope;
  std::pair<int, Component> addlist(FriendInfo);
  int but_selected = 0;

public:
  std::vector<std::pair<int, Component>> list;
  void refresh();
  int friend_selected;
  int function_selected;
  int function1_selected;
  int function2_selected;
  int selected;
  Friends(Telescope*);
  Component content;
  std::unordered_map<int, Element> previews;
};

class Myself {
public:
  int function_selected;
  int selected;
  Component content;
  std::unordered_map<int, Element> previews;
  Myself(Telescope*);

private:
  Telescope* telescope;
  struct AddFriend {
    int selected;
    Component content;
    AddFriend(Myself*);
  }addfriend;
};

class Telescope {
public:
  Vchat *vchat;
  int *toggle;
  int selected = LIST;
  int list_selected = CHAT;
  std::string ss;
  Component input;
  Component content;
  Common common;
  Myself myself;
  Chats chats;
  Friends friends;
  Telescope(Vchat*, int*);
  Telescope(Telescope &&) = delete;
  Telescope(const Telescope &) = delete;
  ~Telescope();

private:
  void getinput();

};

#endif // TELESCOPE_H
