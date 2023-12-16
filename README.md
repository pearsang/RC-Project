# RC-Project

The goal of this project is to implement a simple auction platform. Users can open 
(host) an auction to sell some asset, and close it, as well list ongoing auctions and make bids.

The development of the project requires implementing an Auction Server (AS) and 
a User Application (User). The AS and multiple User application instances are intended 
to operate simultaneously on different machines connected to the Internet.

The AS will be running on a machine with known IP address and port.

This project consists on a Auction Server (AS) system, composed by an user application 
and a server application, both developed using the C++ language, given that it was 
decided that the main data structure would be classes. Every cpp file is coupled 
with the respective header file. It was also developed a Makefile in the project 
root directory to facilitate the compilation of both applications.

## How to run:

In order to compile the user application, run the command: make user
To run the application, run the command: ./user
The following flags can be used:
    -n : to set the AS hostname
    -p : to set the AS port
    -h : to show the help menu

In order to compile the AS, run the command: make AS
To run the server, run the command: ./AS
The following flags can be used:
    -p : to set the port number the server will be listening on
    -v : to activate the verbose mode, where the AS prints log messages 
during its execution.

If the flags to set the hostname (-n) or the port (-p) are not used,
the default values will ben taken into consideration. You can change these
values and a lot more on utils/constants.hpp.

## The Code:

The program is divided into 3 main directories:

    ./client - all the main functions needed to run the user app.

    ./server - all the main functions needed to run the AS.

    ./utils - set of functions that are used by both the user app and the AS,
including the "protocol.cpp" (with the respective header file), that includes
all the functions to write/read from the UDP and TCP sockets, which depending
on the commands, is how both endpoints will communicate.

## Usage:

Any user can execute the following 12 commands:

    > login <userID password> 
        - It allows the user "userID" to login into the AS.

    > logout
        - It allows a logged in user to logout.

    > unregister
        - Deletes the account of a logged in user.

    > exit 
        - Closes the user app.

    > open <name asset_fname start_value timeactive>
        - The user opens an auction with the description <name>, starting at 
    a price of <start_value> and lasting <timeactive> seconds. An image of path
    <asset_name> is also sent to the AS.

    > close <auction_id>
        - Closes an auction of id <auction_id>. Only the user that hosted the auction
    is allowed to close said auction.

    > my_auctions (ma)
        - The user is shown a list of the auctions hosted by he/she hosts.

    > my_bids (mb)
        - The user is shown a list of the auctions he/she has bidded on.

    > list (l)
        - Shows a list of all auctions that were opened in the AS, as well as their 
    current state (Active or Non Active).

    > show_asset (sa) <auction_id>
        - The user downloads the asset image of the auction <auction_id> into 
        their environment.

    > bid (b) <auction_id value>
        - It allows a logged in user to place a bid on the auction <auction_id>
    of value <value>.

    > show_record (sr) <auction_id>
        - Shows the details for the auction <auction_id>, including a list of its top
    50 largest bids placed until the present moment and information if said auction has 
    already been closed. 


To learn more about the commands please read the [instructions](instructions.pdf).


### Relevant Notes:

Upon running the server for the first time, a data base will be created, in a directory 
under the name AS-DB. In there, there will be 2 directories, the AUCTIONS (all data relative 
to auctions and bids) and the USERS (passwords and loggin state of each user), as well as
a text file used to determine the ID of the next auction to be opened. 
