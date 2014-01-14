

#include <iostream>
#include <string>
#include <fstream>
#include <cstring>
#include <cstdlib>

#pragma GCC diagnostic ignored "-Wwrite-strings"

class TableEntry {
private:
    std::string key;
    int value;
public:
    TableEntry(std::string key, int value) {
        this->key = key;
        this->value = value;
    }
    
    std::string getKey() { return key; }
    int getValue() { return value; }
    
};


// #define TABLE_SIZE 100 // Change as necessary

class Hasher {
private:
    TableEntry** table;

    int TABLE_SIZE; //size of the table //careful
		
		int num_elements;     //number of element in the hash-table
		
		char probeType;  //type of probe for hash-table
		                       //  - 'd' for double
													 //  - 'q' for quadratic
		
		char hashType;   //type of hashcode for hash-table
		                       //  - 'g' for good
													 //  - 'b' for poor
		
		double load_factor; //for stat printing purpose

		int temp_probe;        //temp counter for # of probes
		int total_probe;       //total # of probes
		int total_insert;      //total # of inserts
		int total_elems;       //total # of elements

		bool is_full;   // flag: T indicate a failed insertion
		                       //       a table is considered to be full if there's a failed insertion
												   //       according to the assignment description	

		//generate a good hashcode
		//Pre-condition:  correct string key is passed in
		//Post-condition: a good hashcode is returned back
		int goodHash(std::string key)
		{
			unsigned long hash = 5381;
			for (int i = 0; i < key.length(); i++)
			{
				hash = ((hash << 5) + hash) + key.at(i); //bitshift+add up
			}
			hash = hash % TABLE_SIZE; //fit it to the TABLE_SIZE
			return (int) hash;
		}

		//generate a poor hashcode
		//Pre-condition: correct string key is passed in
		//Post-condition: a poor hashcode is returned back
		int poorHash (std::string key)
		{
			int hash = 0;
			for (int i = 0; i < key.length(); i++)
			{
				hash = hash + key.at(i);  //add up the ASCII chars
			}
			return hash % TABLE_SIZE;  //fit it to the TABLE_SIZE
		}

		//generate a hashcode for Double Hash
		//Pre-condition: correct string key is passed in
		//Post-condition: a hashcode (hopefully different from the other two) is returned
		int secondHash (std::string key)
		{
			int hash = 0;
			for (int i = 0; i < key.length(); i++)
			{
				hash = hash + key.at(i) / (i + 1);  //make hash different
			}
			return 997 - ( hash % 997); // method suggested in class slides
		}

		// search for a key
		// Pre-condition: given the key to find
		//                given a subscript by reference
		//                given the type of search desired as a char, either
		//                        - 's' for simple lookups            or
		//                        - 'i' for insertion
		// Post-condition: true is returned if key is found
		//                 false is returned if key is not found
		//                 for type 's': 
		//                     subscript is set to position of the key or the first NULL encountered
		//                 for type 'i':
		//                     subscript is set to position of the key or the first tombstone encountered
		// Note: the search function has been modified to take three arguments instead of two
		//       as instructed in the assignment description
    bool search(std::string key, int& subscript, char type)
		{
			if (probeType == 'd')
				return searchD(key, subscript, type); //helper for double hash
			else 
				return searchQ(key, subscript, type); //helper for quad hash
		}

	  // search function for double probing
		// See the private search function for pre- and post-conditions
		bool searchD(std::string key, int& subscript, char type)
		{
	    // initialize and set hashcode for key base on hashType
			int hash;    
			if (hashType == 'g') 
				hash = goodHash(key);
			else
			  hash = poorHash(key);

			int increment = secondHash(key);  // set increment to second Hashcode

			int pos = hash;         //set current pos to hash
			subscript = pos;

			temp_probe = 1;

			if (type == 's') // lookup
			{
				while(table[pos] != NULL) 
				{
					if (key.compare(table[pos]->getKey()) == 0) //found
			    {
						return true;
					}
					if (temp_probe >= TABLE_SIZE) // should not need to probe more the table_size times
					{
						is_full = true; 
						return false;
					}
					
					// check next spot
					pos = (pos + increment) %  TABLE_SIZE;
					subscript = pos;
					temp_probe++;
					
				}
				return false;
			}
			else // insert case
			{
				while(table[pos] != NULL && ((table[pos]->getValue()) != -1)) // a value of -1 is 
				                                                                    // the tombstone
		    {
					if (key.compare(table[pos]->getKey()) == 0) //found
					{
						return true;
					}
					if (temp_probe >= TABLE_SIZE) // should not need to probe more the table_size times
					{
						is_full = true;  
						return false;
					}
					
					// check next spot
					pos = (pos + increment) %  TABLE_SIZE;
					subscript = pos;
					temp_probe++;
				}
				return false;
			}
		}

		// Determine whether a number is prime (brute-force no optimization)
		// Pre-condition: postive integer passed in
		// Post-condtion: true if n is prime; false otherwise
		bool isPrime(int n)
		{
			for (int i = 2; i < n; i++)
			{
				if (n % i == 0) return false;
			}
			return true;
		}

	  // search function for quadratic probing
		// See the private search function for pre- and post-conditions
		bool searchQ(std::string key, int& subscript, char type)
		{
	    // initialize and set hashcode for key base on hashType
			int hash;    
			if (hashType == 'g') 
				hash = goodHash(key);
			else
			  hash = poorHash(key);

			int pos = hash;         //set current pos to hash
			subscript = pos;

			temp_probe = 1;

			if (type == 's') // lookup
			{
				while(table[pos] != NULL) 
				{
					if (key.compare(table[pos]->getKey()) == 0) //found
					{
						return true;
					}
					if (temp_probe >= TABLE_SIZE) // should not need to probe more the table_size times
					{
						is_full = true;  
						return false;
					}
					
					// check next spot
					pos = (hash + temp_probe * temp_probe) %  TABLE_SIZE;
					subscript = pos;
					temp_probe++;
					
				}
				return false;
			}
			else // insert case
			{
				while((table[pos] != NULL) && ((table[pos]->getValue()) != -1)) // a value of -1 is 
				                                                                    // the tombstone
		    {
					if (key.compare(table[pos]->getKey()) == 0) //found
					{
						return true;
					}
					if (temp_probe >= TABLE_SIZE) // should not need to probe more the table_size times
					{
						is_full = true;  
						return false;
					}
					
					// check next spot
					pos = (hash + temp_probe * temp_probe) %  TABLE_SIZE;
					subscript = pos;
					temp_probe++;
				}
				return false;
			}
		}



				
    
    // Define any other necessary helper functions that are not part of the public interface:
    
public:
    // Define the necessary constructors (be sure to initialize your table entries to NULL):
    
		// Constructor
		// Pre-condition: type ('g' or 'b') and crp ('d' or 'q') is provided
    // Post-condition: An empty hash-table with type and crp of size 100 is constructed
    Hasher(char type, char crp)
		{
			this->TABLE_SIZE = 100;
			hashType = type;
			probeType = crp;
			total_insert = 0;
			total_probe = 0;
			temp_probe = 0;
			total_elems = 0;
			is_full = false;
			load_factor = -1; // indicate load_factor does not apply
			table = (TableEntry**) malloc(this->TABLE_SIZE * sizeof(TableEntry*)); // ERROR-SOURCE
			for (int i = 0; i < TABLE_SIZE; i++)
		  {
				table[i] = NULL;
			}
		}
    
		// Constructor
		// Pre-condition: type ('g' or 'b') and crp ('d' or 'q') is provided
		//                filename is the location of a file that comply with the desired format
		// Post-condition: An hash-table with type and crp of size 100 and as many TableEntry
		//                 inserted as possible;
    Hasher(char type, char crp, char* filename)  
    {
			Hasher(type, crp);

			std::ifstream infile(filename);
			std::string line;

			while (getline(infile, line))
			{
				std::string key = line.substr(0, 8);
				std::string num = line.substr(8, line.size());
				int value = atoi(num.c_str());
				insert(key, value);
				if (is_full) break;   // stop insertion if table is full
			}
		}

		// Constructor
		// Pre-condition: type ('g' or 'b') and crp ('d' or 'q') and loadFactor is provided
		//                filename is the location of a file that comply with the desired format
		// Post-condition: An hash-table with type and crp of size determined by capacity
		//                 and as many TableEntry inserted as possible;
    Hasher(char type, char crp, double loadFactor, char* filename)  
    {
						
			hashType = type;
			probeType = crp;
			load_factor = loadFactor;
			total_insert = 0;
			total_probe = 0;
			temp_probe = 0;
			is_full = false;
			total_elems = 0;
			
			int numLines = 0;
			std::ifstream infile(filename);
			std::string line;
			while(getline(infile, line)) 
			{
				numLines++;
			}
			infile.close();
			int capacity = numLines/load_factor;
			while (!isPrime(capacity))
			{
				capacity++;
			}

			TABLE_SIZE = capacity;
			table = (TableEntry**) malloc(this->TABLE_SIZE * sizeof(TableEntry*)); // ERROR-SOURCE
			for (int i = 0; i < TABLE_SIZE; i++)
		  {
				table[i] = NULL;
			}

			infile.open(filename);

			while (getline(infile, line))
			{
				std::string key = line.substr(0, 8);
				std::string num = line.substr(8, line.size());
				int value = atoi(num.c_str());
				insert(key, value);
				if (is_full) break;
			}
		}

		// Constructor
		// Pre-condition: type ('g' or 'b') and crp ('d' or 'q') and loadFactor is provided
		//                filename is the location of a file that comply with the desired format
		// Post-condition: An hash-table with type and crp of size determined by capacity
		//                 and as many TableEntry inserted as possible;
    Hasher(char type, char crp, double loadFactor, std::string filename)  
    {
			Hasher(type, crp, loadFactor, filename.c_str());
		}

    
		// search for a key
		// Pre-condition: given the key to find
		//                given a subscript by reference
		// Post-condition: true is returned if key is found
		//                 false is returned if key is not found
		//                     subscript is set to position of the key or the first NULL encountered
		// Note: this search function is written to comply to the interface assumed by main
		bool search(std::string key, int& subscript)
		{
			search(key, subscript, 's');  // calls the private helper with 3 arguments
		}
    
		// insert a key val pair
		// Pre-condition: given appropriate key and corresponding value to be inserted
		// Post-condition: true is returned if the key-val pair is successfully inserted
		//                 false otherwise; prints out "Table full." upon failure
		//                 key-val pair is inserted if insertion is possible
    bool insert(std::string key, int value)
		{
			if (is_full)
			{
				std::cout << "Table full." << std::endl;
				return false;
			}
			int subscript = -1;
			search(key, subscript, 'i');
			if (!is_full)
			{
				table[subscript] = new TableEntry(key, value);
				total_probe += temp_probe;    //update counters
				temp_probe = 0;
				total_insert++;
				total_elems++;
				return true;
			}
			std::cout << "Table full." << std::endl;
			return false;
		}

		// remove a key
		// Pre-condition: given the key to remove
		// Post-condition: key removed (tombstone set) and true returned if key is present
		//                 otherwise false is returned
    bool remove(std::string key)
    {
		  int subscript = -1;
			if (search(key, subscript, 's'))
			{
				table[subscript] = new TableEntry("", -1); //set tombstone
				total_elems--;
				return true;
			}
			return false;
		}

		// whether table is full
		// Pre-condition: none
		// Post-condition: true returned to indicate a failed insertion has occurred
		//                 false returned otherwise
    bool isFull()
		{
			return is_full;
		}
    
		// returns the key stored at given subscript
		// Pre-condition: valid subscript given
		// Post-condition: key returned
		const std::string getKey(const int subscript)
		{
			return table[subscript]->getKey();
		}

		// returns the value stored at given subscript
		// Pre-condition: valid subscript given
		// Post-condition: value returned
		const int getValue(const int subscript)
		{
			return table[subscript]->getValue();
		}
	
		// return the number of probes
		// Pre-condition: none
		// Post-condition: number of probes returned
		const int getProbes()
		{
			return total_probe;
		}

		// return the hash table size
		// Pre-condition: none
		// Post-condition: size of the hash table is returned
		const int getCapacity()
		{
			return TABLE_SIZE;
		}

		// return the number of non-empty elements
		// Pre-condition: none
		// Post-condition: number of non-empty elements returned
		const int getSize()
		{
			return total_elems;
		}

		// 
    // Simply outputs the current contents of the table and the indices (you can write a loop
    // that just prints out the underlying array):
    // E.g.
    //    table->printTable() might generate:
    //        25  HBZEJKGA   1
    //        32  RHJMIVTA   2
    //
    void printTable()
		{
			for(int i = 0; i < TABLE_SIZE; i++){
		    if (table[i] != NULL)
				{
		      std::cout << "Position " << i << ": ";
		   		if(table[i]->getValue() == -1){
	        	std::cout << "Tombstone" << std::endl; 
				  }
				  else
				  {
	        std::cout << "Key: " << table[i]->getKey() << " Value: " << table[i]->getValue() << std::endl;
		   	  }
	    	}
	    }
	    return;
		}
    
		// Print stat
		// Pre-condition: none
		// Post-condition: stat of hash-table printed
		void printStat()
		{
			std::cout << "Stats: " << std::endl;
			std::cout << "load factor: " << load_factor << std::endl;
			std::cout << "hash type: " << hashType << std::endl;
			std::cout << "probe type: " << probeType << std::endl;
			std::cout << "probe/insertion: " << total_probe << "/" << total_insert << " = " << (double) total_probe/total_insert << std::endl; 
			std::cout << "table size: " << TABLE_SIZE << std::endl;
		}
    
    // Destructor-- do not alter.
    ~Hasher() 
    {
        for (int i = 0; i < TABLE_SIZE; i++)
            if (table[i] != NULL)
                delete table[i];
        delete[] table;
    }
    
};





// **Sample** main function/driver-- THIS IS NOT A COMPLETE TEST SUITE
// YOU MUST WRITE YOUR OWN TESTS
// See assignment description.
int main( int argc, char* argv[])
{
    // Generate empty hash tables:
    Hasher* goodHashRP1 = new Hasher('g', 'd');
    Hasher* goodHashQP1 = new Hasher('g', 'q');
    Hasher* badHashRP1 = new Hasher('b', 'd');
    Hasher* badHashQP1 = new Hasher('b', 'q');
    
    // Generate hash tables that are systematically loaded from file.
    // Note that if you cannot fit an element you should stop inserting elements
    // and set a flag to full.
    Hasher* goodHashRPa = new Hasher('g', 'd', 0.25, "4000record.txt");
		Hasher* goodHashRPb = new Hasher('g', 'd', 0.50, "4000record.txt");
		Hasher* goodHashRPc = new Hasher('g', 'd', 0.75, "4000record.txt");
    Hasher* goodHashQPa = new Hasher('g', 'q', 0.25, "4000record.txt");
    Hasher* goodHashQPb = new Hasher('g', 'q', 0.50, "4000record.txt");
    Hasher* goodHashQPc = new Hasher('g', 'q', 0.75, "4000record.txt");
    Hasher* poorHashRPa = new Hasher('b', 'd', 0.25, "4000record.txt");
    Hasher* poorHashRPb = new Hasher('b', 'd', 0.50, "4000record.txt");
    Hasher* poorHashRPc = new Hasher('b', 'd', 0.75, "4000record.txt");
    Hasher* poorHashQPa = new Hasher('b', 'q', 0.25, "4000record.txt");
    Hasher* poorHashQPb = new Hasher('b', 'q', 0.50, "4000record.txt");
    Hasher* poorHashQPc = new Hasher('b', 'q', 0.75, "4000record.txt");
	  goodHashRPa->printStat();
		goodHashRPb->printStat();
		goodHashRPc->printStat();
		goodHashQPa->printStat();
		goodHashQPb->printStat();
		goodHashQPc->printStat();
		poorHashRPa->printStat();
		poorHashRPb->printStat();
		poorHashRPc->printStat();
		poorHashQPa->printStat();
		poorHashQPb->printStat();
		poorHashQPc->printStat();

    // Sample use case:
		std::cout << "Insert MUZEJKGA 10" << std::endl;
    std::string key = "MUZEJKGA";
    int value = 10;
    if(goodHashRP1->insert(key, value)) 
        std::cout << "Inserted" << std::endl;
    else
        std::cout << "Failed to insert" << std::endl;
    
    goodHashRP1->printTable();

    int subscript = -1; 
    std::cout << "search for inserted" << std::endl;
    if(goodHashRP1->search(key, subscript)) 
        std::cout << "Found at " << subscript << std::endl;
    else
        std::cout << "Failed to find" << std::endl;
    
    goodHashRP1->printTable();
    std::cout << "remove that one" << std::endl;
		if(goodHashRP1->remove(key)) 
        std::cout << "Removed" << std::endl;
    else
        std::cout << "Not deleted/not found" << std::endl;
    
		goodHashRP1->printTable();
    
		std::cout << "remove once more" << std::endl;
    if(goodHashRP1->remove(key)) 
        std::cout << "Removed" << std::endl;
    else
        std::cout << "Not deleted/not found" << std::endl;
    goodHashRP1->printTable();

		std::cout << "insert again" << std::endl;
    if(goodHashRP1->insert(key, value)) 
        std::cout << "Inserted" << std::endl;
    else
        std::cout << "Failed to insert" << std::endl;
    goodHashRP1->printTable();
		std::cout << "search for it" << std::endl;
    if(goodHashRP1->search(key, subscript)) 
        std::cout << "Found at " << subscript << std::endl;
    else
        std::cout << "Failed to find" << std::endl;
    goodHashRP1->printTable();
		value=3;
		std::cout << "insert with same key diff val" << std::endl;
    if(goodHashRP1->insert(key, value)) 
        std::cout << "Inserted" << std::endl;
    else
        std::cout << "Failed to insert" << std::endl;
    goodHashRP1->printTable();
		std::cout << "find it" << std::endl;
    if(goodHashRP1->search(key, subscript)) 
        std::cout << "Found at " << subscript << std::endl;
    else
        std::cout << "Failed to find" << std::endl;
    goodHashRP1->printTable();
		std::cout << "remove " << std::endl;
    if(goodHashRP1->remove(key)) 
        std::cout << "Removed" << std::endl;
    else
        std::cout << "Not deleted/not found" << std::endl;
    goodHashRP1->printTable();
		std::cout << "insert again" << std::endl;
    if(goodHashRP1->insert(key, value)) 
        std::cout << "Inserted" << std::endl;
    else
        std::cout << "Failed to insert" << std::endl;
    goodHashRP1->printTable();
    return 0;
}
