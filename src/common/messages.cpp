/**
 * @file messages.cpp
 * @brief Implementation of the messages to be displayed by the Client or the
 * Server.
 */
#include "messages.hpp"

using namespace std;

void Message::UserLoginSuccess() {
    cout << "User sucessfully logged in" << endl;
}

void Message::WrongPassword() {
    cout << "Wrong password" << endl;
}

void Message::WrongUsernameOrPassword() {
    cout << "Username does not exist or wrong password" << endl;
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

void Message::UserLogoutSucess() {
    cout << "User sucessfully logged out" << endl;
}

void Message::UserNotRegistered() {
    cout << "User is not registered" << endl;
}

void Message::UserUnregisterSucess() {
    cout << "User sucessfully unregistered" << endl;
}

void Message::UserIsLoggedIn() {
    cout << "User is logged in" << endl;
}

void Message::FileTooBig() {
    cout << "File too big" << endl;
}

void Message::AuctionCreated(string aid) {
    cout << "Auction successfully created." << endl << "AID: " << aid << endl;
}

void Message::AuctionNotCreated() {
    cout << "Auction was not able to be created" << endl;
}

void Message::AuctionClosedSucessfully(string aid) {
    cout << "Auction " << aid << " closed successfully" << endl;
}

void Message::AuctionNotExists(string aid) {
    cout << "Auction " << aid << " does not exist" << endl;
}

void Message::UserNotOwner() {
    cout << "You need to be the owner of the auction to delete it" << endl;
}

void Message::AuctionAlreadyEnded(string aid) {
    cout << "Auction " << aid << " has already ended" << endl;
}

void Message::UserNotMadeAuction() {
    cout << "User has not made an auction" << endl;
}

void Message::ShowUser(string uid) {
    cout << "User " << uid;
}

void Message::ListAuctionsAndState(const map<string, string> &list) {
    for (auto auction : list) {  // For each auction in the list
        string state;
        if (auction.second == "0") {  // If the auction is not active
            state = " NOT ACTIVE";
        } else {  // If the auction is active
            state = " ACTIVE";
        }
        cout << "Auction " << auction.first << "-" << state << endl;
    }
}

void Message::UserHasNoBids() {
    cout << "User doesn't have a bid in any auction" << endl;
}

void Message::NoAuctionStarted() {
    cout << "No auctions has been started" << endl;
}

void Message::ErrorDownloadAsset() {
    cout << "There was an error downloading the requested asset" << endl;
}

void Message::DownloadAsset(string name, int size) {
    cout << "Downloaded file " << name << " (" << size << " bytes)" << endl;
}

void Message::UserSucessfullyBid(string value, string aid) {
    cout << "Successfully bid " << value << " on auction " << aid << endl;
}

void Message::HigherValueBid() {
    cout << "Someone has already bid a higher ammount on this auction" << endl;
}

void Message::BidOwnAuctions() {
    cout << "You can't bid on your own auction" << endl;
}

void Message::AuctionRecordEnded(time_t endDateTime, int endSecTime) {
    // The header of the record end data
    cout << "                                    AUCTION HAS ENDED             "
            "                       "
         << endl;
    cout << "End Date and Time: " << DateTimeToString(endDateTime);
    cout << "\t\t\tTime Elapsed (seconds): " << endSecTime << endl;
    cout << "------------------------------------------------------------------"
            "-----------------------"
         << endl;
}

void Message::ShowRecordHeader(string aid, string host, string auctionName,
                               string assetFname, int startValue,
                               time_t startDateTime, int timeActive) {
    cout << "------------------------------------------------------------------"
            "-----------------------"
         << endl;
    cout << "                                      AUCTION RECORD              "
            "                       "
         << endl;
    cout << "------------------------------------------------------------------"
            "-----------------------"
         << endl;
    // The details of the auction
    cout << "Auction ID: \t\t\t" << aid << endl;
    cout << "Host ID: \t\t\t" << host << endl;
    cout << "Auction Name: \t\t\t" << auctionName << endl;
    cout << "Asset File Name: \t\t" << assetFname << endl;
    cout << "Start Value: \t\t\t" << startValue << endl;
    cout << "Start Date and Time: \t\t" << DateTimeToString(startDateTime)
         << endl;
    cout << "Time of activity (seconds): \t" << timeActive << endl;
    cout << "------------------------------------------------------------------"
            "-----------------------"
         << endl;
    cout << "                                            BIDS                  "
            "                       "
         << endl;
    cout << "------------------------------------------------------------------"
            "-----------------------"
         << endl;
}

void Message::ShowRecordBids(vector<string> bidderUids, vector<int> bidValues,
                             vector<time_t> bidDateTime,
                             vector<int> bidSecTimes) {
    long unsigned int size = bidderUids.size();
    // The header of the bids
    cout << "Bidder ID\tBid Value\tBid Date and Time\tTime Elapsed (seconds)"
         << endl;
    for (long unsigned int i = 0; i < size; i++) {  // For each bid
        cout << " " << bidderUids[i] << "\t\t  " << bidValues[i] << "\t      "
             << DateTimeToString(bidDateTime[i]) << "\t\t" << bidSecTimes[i]
             << endl;
    }
    cout << "------------------------------------------------------------------"
            "-----------------------"
         << endl;
}

std::string Message::ServerRequestDetails(std::string uid,
                                          std::string requestType,
                                          std::string resultType) {
    // For commands which send both the UID and the Password of the user
    return "From: " + uid + " Request: " + requestType +
           " Result: " + resultType;
}

std::string Message::ServerRequestDetails(std::string requestType,
                                          std::string resultType) {
    // For commands which send only the Password of the user
    return "Request: " + requestType + " Result: " + resultType;
}

std::string Message::ServerConnectionDetails(std::string ip, std::string port,
                                             std::string protocol) {
    // Shows the details of the Client connected to the Server
    return "Request received from: " + ip + ":" + port + " (" + protocol + ")";
}