//For this project, you will need to write a tool that can take a sentence, and squeeze the words that have the same ending as the beginning of the next word.
// WRITE YOUR CODE HERE
#include <iostream>
#include <sstream>
#include <vector>
#include <string>
#include <cctype>
using namespace std;

int NumberOfCharactersOverlap(const string& a, const string& b) {
    int maxOverlap = 0;
    int maxLen = min(a.size(), b.size());
    for (int len = 1; len <= maxLen; len++) {
        if (a.substr(a.size() - len) == b.substr(0, len)) {
            maxOverlap = len;
        }
    }
    return maxOverlap;
}

string ProcessLine(const string& line) {
    istringstream iss(line);
    vector<string> words;
    string w;
    while (iss >> w) {
        words.push_back(w);
    }

    if (words.empty()) return "";

    string result = words[0];
    for (size_t i = 1; i < words.size(); i++) {
        int overlap = NumberOfCharactersOverlap(result, words[i]);
        if (overlap > 0) {
            result += words[i].substr(overlap);
        } else {
            result += " " + words[i];
        }
    }
    return result;
}

int main() {
    string line;
    while (getline(cin, line)) {
        cout << ProcessLine(line) << endl;
    }
    return 0;
}
