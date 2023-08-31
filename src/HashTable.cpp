//============================================================================
// Name        : HashTable.cpp
// Author      : Corey Nance
// Version     : 1.0
// Copyright   : Copyright © 2017 SNHU COCE
// Description : Hello World in C++, Ansi-style
// Date        : 2/7/2021
// Course      : CS 260
// Project #   : 5-2 Programming Activity: Hash Tables and Chaining
//============================================================================


// Include statements to include C++ libraries to be used in this program.
#include <algorithm>
#include <climits>
#include <iostream>
#include <string> //
#include <time.h>

#include "CSVparser.hpp"

using namespace std;


// Global definitions visible to all methods and classes
const unsigned int DEFAULT_SIZE = 179;

// forward declarations
double strToDouble(string str, char ch);

// define a structure to hold bid information
struct Bid {
    string bidId; // unique identifier
    string title;
    string fund;
    double amount;
    Bid() {
        amount = 0.0;
    }
};


// Hash Table class definition to implement a hash table with chaining.
class HashTable {

private:
    //structures to hold bids
	struct Node {
		Bid bid;
		unsigned key;
		Node* next;

		// default constructor
		Node() {
			key = UINT_MAX;
			next = nullptr;
		}

		// initialize with a bid
		Node(Bid aBid) : Node() {
			bid = aBid;
		}

		// initialize with a bid and a key
		Node(Bid aBid, unsigned aKey) : Node(aBid) {

			key = aKey;
		}

	};

    // vector to hold nodes
	vector<Node> nodes;

	// a statement to set table size to defaultsize.
	unsigned tableSize = DEFAULT_SIZE;



	// hash key creation
    unsigned int hash(int key);


// public access functions to be allow access to class variables.
public:
    HashTable();
    HashTable(unsigned size);
    virtual ~HashTable();
    void Insert(Bid bid);
    void PrintAll();
    void Remove(string bidId);
    Bid Search(string bidId);
};


 // constructors  for the Hashtable class
HashTable::HashTable() {

	nodes.resize(tableSize);
}

HashTable::HashTable(unsigned size) {

	this->tableSize = size;
	nodes.resize(tableSize);
}


// Destructor to free up storage when class is destroyed
HashTable::~HashTable() {

	nodes.erase(nodes.begin());

}

//logic to calculate a hash value
unsigned int HashTable::hash(int key) {


	return key % tableSize;
}

// logic to insert a bid
void HashTable::Insert(Bid bid) {


	//calculate the key for this bid
	unsigned key = hash(atoi(bid.bidId.c_str()));

	// try and retrieve node using the key
	Node* oldNode = &(nodes.at(key));

	//if no entry found for this key
	if (oldNode == nullptr) {
		Node* newNode = new Node(bid, key);
		nodes.insert(nodes.begin() + key, (*newNode));
	}
	else {
		//node was found
		if (oldNode->key == UINT_MAX) {
			oldNode->key = key;
			oldNode->bid = bid;
			oldNode->next = nullptr;
		}
		else {
			// find the next open node (last one)
			while (oldNode->next != nullptr) {
				oldNode = oldNode->next;
			}
			oldNode->next = new Node(bid, key);
		}


	}
}

// logic to print all bids
void HashTable::PrintAll() {


	/* A for loop to iterate through the hash table then access the bucket
	 * values and then output all the data.
	 */
	for (auto iter = nodes.begin(); iter != nodes.end(); ++iter) {
	        if (iter->key != UINT_MAX) {
	            cout << "Key " << iter->key << ": " << iter->bid.bidId << " | "
	                    << iter->bid.title << " | " << iter->bid.amount << " | "
	                    << iter->bid.fund << endl;
	            Node* node = iter->next;
	            while (node != nullptr) {
	                cout << "    " << node->key << ": " << node->bid.bidId
	                        << " | " << node->bid.title << " | " << node->bid.amount
	                        << " | " << node->bid.fund << endl;
	                node = node->next;
	            }
	        }
	    }




}


// logic to remove a bid
void HashTable::Remove(string bidId) {


	unsigned key = hash(atoi(bidId.c_str()));
	nodes.erase(nodes.begin() + key);
}

// logic to search for and return a bid
Bid HashTable::Search(string bidId) {
    Bid bid;



    // calculate the key for this bid
    unsigned key = hash(atoi(bidId.c_str()));

	// try and retrieve node using the key
	Node* node = &(nodes.at(key));

	//if no entry found
	if (node == nullptr || node->key == UINT_MAX) {
		return bid;

	}

	//if node found that matches key
	if (node != nullptr && node->key != UINT_MAX
			&& node->bid.bidId.compare(bidId) == 0) {
			return node->bid;

	}


	// walk the linked list to find the match
	while(node != nullptr) {

		if (node->key != UINT_MAX && node->bid.bidId.compare(bidId) == 0) {
			return node->bid;

		}
		node = node->next;
	}


    return bid;
}


// Function to Display the bid information to the console (std::out)
void displayBid(Bid bid) {
    cout << bid.bidId << ": " << bid.title << " | " << bid.amount << " | "
            << bid.fund << endl;
    return;
}


// Function to Load a CSV file containing bids into a container
void loadBids(string csvPath, HashTable* hashTable) {
    cout << "Loading CSV file " << csvPath << endl;

    // initialize the CSV Parser using the given path
    csv::Parser file = csv::Parser(csvPath);

    // read and display header row - optional
    vector<string> header = file.getHeader();
    for (auto const& c : header) {
        cout << c << " | ";
    }
    cout << "" << endl;

    try {
        // loop to read rows of a CSV file
        for (unsigned int i = 0; i < file.rowCount(); i++) {

            // Create a data structure and add to the collection of bids
            Bid bid;
            bid.bidId = file[i][1];
            bid.title = file[i][0];
            bid.fund = file[i][8];
            bid.amount = strToDouble(file[i][4], '$');


            // push this bid to the end
            hashTable->Insert(bid);
        }
    } catch (csv::Error &e) {
        std::cerr << e.what() << std::endl;
    }
}

/**
 * Simple C function to convert a string to a double
 * after stripping out unwanted char
 *
 * credit: http://stackoverflow.com/a/24875936
 *
 * @param ch The character to strip out
 */
double strToDouble(string str, char ch) {
    str.erase(remove(str.begin(), str.end(), ch), str.end());
    return atof(str.c_str());
}

// main() method
int main(int argc, char* argv[]) {

    // process command line arguments
    string csvPath, searhValue;
    switch (argc) {
    case 2:
        csvPath = argv[1];
        searhValue = "98109";
        break;
    case 3:
        csvPath = argv[1];
        searhValue = argv[2];
        break;
    default:
        csvPath = "eBid_Monthly_Sales_Dec_2016.csv";
        searhValue = "98109";
    }

    // Define a timer variable
    clock_t ticks;

    // Define a hash table to hold all the bids
    HashTable* bidTable;

    Bid bid;

    int choice = 0;
    while (choice != 9) {
        cout << "Menu:" << endl;
        cout << "  1. Load Bids" << endl;
        cout << "  2. Display All Bids" << endl;
        cout << "  3. Find Bid" << endl;
        cout << "  4. Remove Bid" << endl;
        cout << "  9. Exit" << endl;
        cout << "Enter choice: ";
        cin >> choice;

        switch (choice) {

        case 1:
            bidTable = new HashTable();

            // Initialize a timer variable before loading bids
            ticks = clock();

            // Complete the method call to load the bids
            loadBids(csvPath, bidTable);

            // Calculate elapsed time and display result
            ticks = clock() - ticks; // current clock ticks minus starting clock ticks
            cout << "time: " << ticks << " clock ticks" << endl;
            cout << "time: " << ticks * 1.0 / CLOCKS_PER_SEC << " seconds" << endl;
            break;

        case 2:
            bidTable->PrintAll();
            break;

        case 3:
            ticks = clock();

            bid = bidTable->Search(searhValue);

            ticks = clock() - ticks; // current clock ticks minus starting clock ticks

            if (!bid.bidId.empty()) {
                displayBid(bid);
            } else {
                cout << "Bid Id " << searhValue << " not found." << endl;
            }

            cout << "time: " << ticks << " clock ticks" << endl;
            cout << "time: " << ticks * 1.0 / CLOCKS_PER_SEC << " seconds" << endl;
            break;

        case 4:
            bidTable->Remove(searhValue);
            break;
        }
    }

    cout << "Good bye." << endl;

    return 0;
}
