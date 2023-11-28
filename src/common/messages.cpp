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
    cout << "Auction successfully created. AID: " << aid << endl;
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
    for (auto auction : list) {
        string state;
        if (auction.second == "0") {
            state = " Status: Not active";
        } else {
            state = " Status: Active";
        }
        cout << "Auction ID: " << auction.first << state << endl;
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
    cout << "This auction has ended" << endl;
    cout << "End Date and Time: " << DateTimeToString(endDateTime);
    cout << " Number of seconds the auction was opened: " << endSecTime << endl;
}

void Message::ShowRecordHeader(string aid, string host, string auctionName,
                               string assetFname, int startValue,
                               time_t startDateTime, int timeActive) {
    cout << "Auction ID: " << aid << endl;
    cout << "Host ID: " << host << endl;
    cout << "Auction Name: " << auctionName << endl;
    cout << "Asset File Name: " << assetFname << endl;
    cout << "Start Value: " << startValue << endl;
    cout << "Start Date and Time: " << DateTimeToString(startDateTime) << endl;
    cout << "Time of activity: " << timeActive << endl;
    cout << "---------------------" << endl;
    cout << "Bids: " << endl;
    cout << "---------------------" << endl;
}

void Message::ShowRecordBids(vector<string> bidderUids, vector<int> bidValues,
                             vector<time_t> bidDateTime,
                             vector<int> bidSecTimes) {
    long unsigned int size = bidderUids.size();
    cout << "Bidder ID\tBid Value\tBid Date and Time\tBid Time "
            "after the opening of the auction"
         << endl;
    for (long unsigned int i = 0; i < size; i++) {
        cout << bidderUids[i] << "            ";
        cout << bidValues[i] << "          ";
        cout << DateTimeToString(bidDateTime[i])
             << "                          ";
        cout << bidSecTimes[i] << endl;
    }
    cout << "---------------------" << endl;
}