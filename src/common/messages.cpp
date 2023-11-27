#include "messages.hpp"

using namespace std;

void Message::UserLoginSuccess() {
    cout << "User sucessfully logged in" << endl;
}

void Message::WrongPassword() {
    cout << "Wrong password" << endl;
}

void Message::UserRegisterSuccess() {
    cout << "User successfully registered" << endl;
}

void Message::UserNotLoggedIn() {
    cout << "User is not logged in" << endl;
}

void Message::UserAlreadyLoggedIn() {
    cout << "User already logged in" << endl;
}