#ifndef __MESSAGES_HPP__
#define __MESSAGES_HPP__

#include <iostream>

class Message {
  public:
    static void UserLoginSuccess();
    static void WrongPassword();
    static void UserRegisterSuccess();
    static void UserNotLoggedIn();
    static void UserAlreadyLoggedIn();
};

#endif