#pragma once

#define WIN32_LEAN_AND_MEAN 

#include <stdio.h>
#include <tchar.h>

#include <stdlib.h>
#include <string.h>
#include <string>
#include <vector>
#include <set>
#include <iostream>
#include <fstream>
#include <sstream>
#include <unordered_set>
#include <functional>
#include <thread>
#include <algorithm>
#include <queue>
#include <regex>
#include <map>
#include <unordered_map>
#include <array>
#include <fstream> 

#define mySleep(a) std::this_thread::sleep_for(std::chrono::milliseconds(a));
#include <SFML/Network.hpp>
/*
let me think

inefficent proof of concept
a SERVER will host a tcp listener on some static port
a CLIENT will use tcp client to connect to it

Client send a handshake
Server responds to handshake
-- they now know they are on compatible network apis, this to be verified
Client send its initial data like a nickname of the player
Server responds assigning an unique ID which will be uint32 always increasing
--
every 'net frame'
server sends to all clients:
    concatenated big blob with explict type PLAYERS, in count of all players, every element to be:
        player id, data length, and here some unique data like x y z position whatever
    a big global state structure explict  type GLOBAL containing oter useful stuff like cards positions, points
client receives the data, parses every element and finding the player updates the data and calls some on Net Frame call handled by the app
--
if server needs to do something on the clinet (like, make explosion? spawn a car?) 
    server sends to one client a structure like
    explict type COMMAND its uint ID, command length, and then some data
    and client parses on its own (callback on ID, app does idebntification and args extraction)
    with mechanism like
    readInt32(int offset) -> return int32 size == 4

*/