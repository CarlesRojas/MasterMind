#include <iostream>
#include <vector>
#include <random>
#include <set>
using namespace std;





// Initial conditions
int numPos;
int numCol;

// Structs for the replies and codes
struct Reply{
	int blacks;
	int whites;
	Reply(){
		blacks = 0;
		whites = 0;
	}
	Reply(int b, int w){
		blacks = b;
		whites = w;
	}
};

struct Code{
	vector<int> code;
	vector<bool> checked;

	// Constructors
	Code(){
		code = vector<int>(0,0);
		checked = vector<bool>(numPos, false);
	}
	Code(vector<int>& codeNums){
		code = codeNums;
		checked = vector<bool>(numPos, false);
	}

	// Per ordenar-los
	bool operator <(const Code& c) const
    {
		for(int i = 0; i < (int)code.size(); ++i){
			if(code[i] != c.code[i]) return code[i] < c.code[i];
		}
    }

	bool operator >(const Code& c) const
    {
		for(int i = 0; i < (int)code.size(); ++i){
			if(code[i] != c.code[i]) return code[i] > c.code[i];
		}
    }
};

// All codes and replies possible
set<Code> codesLeft;
set<Code> notGuessedCodes;
vector<Reply> possibleReplies;

// Final code (Answer)
Code result;
Code guess;
Reply reply;






// Generates the vector of possible replies according to the number of positions chosen
void generatePossibleReplies(){
	possibleReplies.clear();
	for	(int b = 0; b <= numPos; ++b){
		for(int w = 0; w + b <= numPos; ++w){
			Reply r = Reply(b, w); 
			if (b != numPos - 1 || w != 1) possibleReplies.push_back(r);
		}
	}

	/*
	for(int i = 0; i < (int)possibleReplies.size(); ++i) 
		cout << "Blacks: " << possibleReplies[i].blacks << "  Whites: " << possibleReplies[i].whites << endl;
	*/
}

//  Recursive function to generate all combinations
void combinations(vector<int> v, int pos){
	if (pos < 0) { codesLeft.insert(Code(v)); notGuessedCodes.insert(Code(v)); }
	else {
		for (int i = 1; i <= numCol; ++i) {
			v[pos] = i;
			combinations(v, pos - 1);
		}
    }
}

// Generates all combinations of codes and puts them on the "codesLeft" and "notGuessedCodes" vector
void generateAllCombinations(){
	codesLeft.clear();
	notGuessedCodes.clear();
	combinations(vector<int>(numPos,-1), numPos - 1);

	/*
	for(set<Code>::iterator i = codesLeft.begin(); i != codesLeft.end(); ++i){
		for(int j = 0; j < (*i).code.size(); ++j) cout << (*i).code[j] << " ";
		cout << endl;
	}
	*/
}

// Generates a random code
Code generateRandomCode(){
	
	// Random Generator
	random_device rd;
	mt19937 generator(rd());    
	uniform_int_distribution<int> uni(1, numCol);

	vector<int> randomCode;
	for(int i = 0; i < numPos; ++i) randomCode.push_back(uni(generator));
	
	/*
	cout << "Result Code: ";
	for(int i = 0; i < (int)randomCode.size(); ++i) cout << randomCode[i] << " ";
	cout << endl;
	*/

	return randomCode;
}

// Prints the code c
void printTitle(){
    
    cout << endl <<
    "\033[1;31m         __  ___              __               __  ___ _             __" << endl <<
    "        /  |/  /____ _ _____ / /_ ___   _____ /  |/  /(_)____   ____/ /"           << endl <<
    "       / /|_/ // __ '// ___// __// _ \\ / ___// /|_/ // // __ \\ / __  / "         << endl <<
    "      / /  / // /_/ /(__  )/ /_ /  __// /   / /  / // // / / // /_/ /  "           << endl <<
    "     /_/  /_/ \\____//____/ \\__/ \\___//_/   /_/  /_//_//_/ /_/ \\____/   \033[0m"<< endl << endl << endl << endl;
}

// Prints the code c
void printCode(Code c){
	for(int i = 0; i < c.code.size(); ++i) cout << c.code[i] << " ";
}

// Prints the reply r
void printReply(Reply r){
	cout << "     B: " << r.blacks << "   W: " << r.whites;
}

// Generate first Guess
Code generateFirstGuess(){
	vector<int> codeAux (numPos);
	for(int i = 0; i < numPos; ++i){
		if(i < numPos/2) codeAux[i] = 1;
		else codeAux[i] = 2;
	}
	return Code(codeAux); 
}

// Reply from trying code "gue" (guess) against code "res" (result)
Reply getReply(Code res, Code gue){
	Reply r;
	res.checked = vector<bool>(numPos, false);
	gue.checked = vector<bool>(numPos, false);

	// Check for blacks
	for(int i = 0; i < (int)res.code.size(); ++i){
		if(res.code[i] == gue.code[i]){
			r.blacks++;
			res.checked[i] = true;
			gue.checked[i] = true;
		}
	}

	// Check for whites
	for(int i = 0; i < (int)res.code.size(); ++i){
		if(!res.checked[i]){
			for(int j = 0; j < (int)gue.code.size(); ++j){
				if(!gue.checked[j]){
					/*cout << "Checking result[" << i << "] whith guess[" << j << "]" << endl;*/
					if(res.code[i] == gue.code[j]){
						r.whites++;
						res.checked[i] = true;
						gue.checked[j] = true;
						break;
					}
				}
			}
		}
	}

	return r;
}

// Delete or count eliminated elements from codesLeft when checked with the Code "c" and Reply "r"
int deleteOrCountCombinations(Code c, Reply r, bool del){
	int count = 0;
	
	// For each comination in the set of codesLeft...
	for(set<Code>::iterator i = codesLeft.begin(); i != codesLeft.end(); ++i){
		// ...if checked against the "c" doesn't reply the exact same black/white score we have on "r"...
		Reply currReply = getReply(c, *i);
		if(r.blacks != currReply.blacks || r.whites != currReply.whites){
			// ...delete or count that code from codesLeft.
			if (del) { codesLeft.erase(i); --i; }
			else count++;
		}
	}

	return count;
}

// Obtains the next Guess
Code getNextGuess(){
	int max = -1;
	Code bestCode;

	// For each combination still in notGuessedCodes...
	for(set<Code>::iterator i = notGuessedCodes.begin(); i != notGuessedCodes.end(); ++i){
		
		// ...for each possible reply of that combination...
		int min = INT32_MAX;
		for(int j = 0; j < (int)possibleReplies.size(); ++j){
			// ...we check how many elements would be eliminated from codesLeft (using that combination with that reply)...
			int elimNum = deleteOrCountCombinations(*i, possibleReplies[j], false);
			// ...and we keep the minimum of all the possible replies for this combination.
			if (elimNum < min) min = elimNum;
		}

		// Here we keep combination that eliminates the max in its worst reply (In case of a tie, we keep the one thats in codesLeft, if both are, the lowest)
		if (min == max){
			set<Code>::iterator oldCode = codesLeft.find(bestCode);
			set<Code>::iterator newCode = codesLeft.find(*i);
			if (oldCode != codesLeft.end() && newCode != codesLeft.end()){
				if(*i > bestCode) bestCode = *i;
			}
			else if (newCode != codesLeft.end()) bestCode = *i;
		}
		else if (min > max) { max = min; bestCode = *i; }
	}
	return bestCode;
}

// Play
int main(){
    cout << "\033[2J\033[1;1H";
	printTitle();

	// Choose difficulty
	cout << "                       Choose the code lenght: ";
	cin >> numPos;
	cout << "                       Choose the number of colors: ";
	cin >> numCol; cout << endl;
    cout << "\033[2J\033[1;1H";
    

	// Start a game
	bool playAgain = true;
	while(playAgain) {
        printTitle();

		// Generate all possible replies and combinations
		generatePossibleReplies();
		generateAllCombinations();

		// Generate result
		result = generateRandomCode();
		cout << "\033[1;33m                   Code:      \033[0m"; printCode(result); cout << endl << endl;

		// First Guess
		int steps = 1;
		guess = generateFirstGuess();
		reply = getReply(result, guess);
		notGuessedCodes.erase(notGuessedCodes.find(guess));

		// Loop for each guess
		while(reply.blacks != numPos){
			
			// Print Guess
			cout << "\033[1m                   Guess " << steps << ":   \033[0m"; printCode(guess);
			printReply(reply); cout << endl;

			// Delete from allCombations 
			deleteOrCountCombinations(guess, reply, true);
			
			// Get next guess with it's reply
			guess = getNextGuess();
			reply = getReply(result, guess);
			notGuessedCodes.erase(notGuessedCodes.find(guess));

			steps++;
		}

		// Print Last Guess
		cout << "\033[1m                   Guess " << steps << ":   \033[0m"; printCode(guess);
		printReply(reply); cout << endl << endl << endl << "\033[5;1m                              *** DONE *** \033[0m" << endl << endl << endl;
		
		// Play Again?
		cout << "                           Play again? (\033[32;1my\033[0m,\033[31;1mn\033[0m) ";
		char cAux; cin >> cAux;
        cout << "\033[2J\033[1;1H";
		if(cAux == 'y') playAgain = true;
		else playAgain = false;
	}
}
