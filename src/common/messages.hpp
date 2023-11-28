#ifndef __MESSAGES_HPP__
#define __MESSAGES_HPP__

#include <ctime>
#include <iostream>
#include <unordered_map>
#include <map>
#include <vector>
#include "utils.hpp"

class Message {
  public:
    static void UserLoginSuccess();
    static void WrongPassword();
    static void UserRegisterSuccess();
    static void UserNotLoggedIn();
    static void UserAlreadyLoggedIn();
    static void UserLogoutSucess();
    static void UserNotRegistered();
    static void UserUnregisterSucess();
    static void UserIsLoggedIn();
    static void FileTooBig();
    static void AuctionCreated(std::string aid);
    static void AuctionNotCreated();
    static void AuctionClosedSucessfully(std::string aid);
    static void AuctionNotExists(std::string aid);
    static void UserNotOwner();
    static void AuctionAlreadyEnded(std::string aid);
    static void UserNotMadeAuction();
    static void ListAuctionsAndState(
        const std::map<std::string, std::string> &list);
    static void UserHasNoBids();
    static void ShowUser(std::string uid);
    static void NoAuctionStarted();
    static void ErrorDownloadAsset();
    static void DownloadAsset(std::string name, int size);
    static void UserSucessfullyBid(std::string value, std::string aid);
    static void HigherValueBid();
    static void BidOwnAuctions();
    static void AuctionRecordEnded(std::time_t endDateTime, int endSecTime);
    static void ShowRecordHeader(std::string aid, std::string host,
                                 std::string auctionName,
                                 std::string assetFname, int startValue,
                                 std::time_t startDateTime, int timeActive);
    static void ShowRecordBids(std::vector<std::string> bidderUids,
                               std::vector<int> bidValues,
                               std::vector<std::time_t> bidDateTime,
                               std::vector<int> bidSecTimes);
};

#endif