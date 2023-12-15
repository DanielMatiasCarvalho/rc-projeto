
#ifndef __MESSAGES_HPP__
#define __MESSAGES_HPP__

#include <ctime>
#include <iostream>
#include <map>
#include <unordered_map>
#include <vector>
#include "utils.hpp"

/**
 * @brief The Message class contains static methods for displaying various messages.
 */
class Message {
  public:
    /**
     * @brief Displays a message for successful user login.
     */
    static void UserLoginSuccess();

    /**
     * @brief Displays a message for wrong password entered during login.
     */
    static void WrongPassword();

    /**
     * @brief Displays a message for wrong username or password entered during login.
     */
    static void WrongUsernameOrPassword();
    /**
     * @brief Displays a message for successful user registration.
     */
    static void UserRegisterSuccess();

    /**
     * @brief Displays a message for a user not being logged in.
     */
    static void UserNotLoggedIn();

    /**
     * @brief Displays a message for a user already being logged in.
     */
    static void UserAlreadyLoggedIn();

    /**
     * @brief Displays a message for successful user logout.
     */
    static void UserLogoutSucess();

    /**
     * @brief Displays a message for a user not being registered.
     */
    static void UserNotRegistered();

    /**
     * @brief Displays a message for successful user unregistration.
     */
    static void UserUnregisterSucess();

    /**
     * @brief Displays a message for a user being already logged in.
     */
    static void UserIsLoggedIn();

    /**
     * @brief Displays a message for a file being too big.
     */
    static void FileTooBig();

    /**
     * @brief Displays a message for successful auction creation.
     * @param aid The ID of the created auction.
     */
    static void AuctionCreated(std::string aid);

    /**
     * @brief Displays a message for unsuccessful auction creation.
     */
    static void AuctionNotCreated();

    /**
     * @brief Displays a message for successful auction closure.
     * @param aid The ID of the closed auction.
     */
    static void AuctionClosedSucessfully(std::string aid);

    /**
     * @brief Displays a message for a non-existing auction.
     * @param aid The ID of the non-existing auction.
     */
    static void AuctionNotExists(std::string aid);

    /**
     * @brief Displays a message for a user not being the owner of an auction.
     */
    static void UserNotOwner();

    /**
     * @brief Displays a message for an auction already being ended.
     * @param aid The ID of the ended auction.
     */
    static void AuctionAlreadyEnded(std::string aid);

    /**
     * @brief Displays a message for a user not having made any auctions.
     */
    static void UserNotMadeAuction();

    /**
     * @brief Displays a message with a list of auctions and their states.
     * @param list A map containing the IDs and states of the auctions.
     */
    static void ListAuctionsAndState(
        const std::map<std::string, std::string> &list);

    /**
     * @brief Displays a message for a user not having any bids.
     */
    static void UserHasNoBids();

    /**
     * @brief Displays a message showing user ID.
     * @param uid The ID of the user.
     */
    static void ShowUser(std::string uid);

    /**
     * @brief Displays a message for no auction being started.
     */
    static void NoAuctionStarted();

    /**
     * @brief Displays a message for an error during asset download.
     */
    static void ErrorDownloadAsset();

    /**
     * @brief Displays a message for asset download.
     * @param name The name of the asset.
     * @param size The size of the asset.
     */
    static void DownloadAsset(std::string name, int size);

    /**
     * @brief Displays a message for a user successfully bidding on an auction.
     * @param value The value of the bid.
     * @param aid The ID of the auction.
     */
    static void UserSucessfullyBid(std::string value, std::string aid);

    /**
     * @brief Displays a message for a higher value bid already existing.
     */
    static void HigherValueBid();

    /**
     * @brief Displays a message for a user trying to bid on their own auctions.
     */
    static void BidOwnAuctions();

    /**
     * @brief Displays a message for an auction record being ended.
     * @param endDateTime The end date and time of the auction.
     * @param endSecTime The duration of the auction in seconds.
     */
    static void AuctionRecordEnded(std::time_t endDateTime, int endSecTime);

    /**
     * @brief Displays a message showing the header of an auction record.
     * @param aid The ID of the auction.
     * @param host The host of the auction.
     * @param auctionName The name of the auction.
     * @param assetFname The filename of the auction's asset.
     * @param startValue The starting value of the auction.
     * @param startDateTime The start date and time of the auction.
     * @param timeActive The active time of the auction in seconds.
     */
    static void ShowRecordHeader(std::string aid, std::string host,
                                 std::string auctionName,
                                 std::string assetFname, int startValue,
                                 std::time_t startDateTime, int timeActive);

    /**
     * @brief Displays a message showing the bids of an auction record.
     * @param bidderUids The IDs of the bidders.
     * @param bidValues The values of the bids.
     * @param bidDateTime The date and time of the bids.
     * @param bidSecTimes The time in seconds of the bids, since the beginning of the auction.
     */
    static void ShowRecordBids(std::vector<std::string> bidderUids,
                               std::vector<int> bidValues,
                               std::vector<std::time_t> bidDateTime,
                               std::vector<int> bidSecTimes);

    static std::string ServerRequestDetails(std::string uid,
                                            std::string requestType,
                                            std::string resultType);
    static std::string ServerRequestDetails(std::string requestType,
                                            std::string resultType);

    static std::string ServerConnectionDetails(std::string ip, std::string port,
                                               std::string protocol);
};

#endif