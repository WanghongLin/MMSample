//
//  Copyright (c) 2018 mutter
//
//  This file is part of MMSample.
//
//  MMSample is free software: you can redistribute it and/or modify
//  it under the terms of the GNU General Public License as published by
//  the Free Software Foundation, either version 3 of the License, or
//  (at your option) any later version.
//
//  MMSample is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//  GNU General Public License for more details.
//
//  You should have received a copy of the GNU General Public License
//  along with MMSample.  If not, see <http://www.gnu.org/licenses/>.
//  
//
// Created by mutter on 4/11/18.
//

/**
 *
 * You are given a string, s, and a list of words, words, that are all of the same length. Find all starting indices of substring(s) in s that is a concatenation of each word in words exactly once and without any intervening characters.
 * For example, given:
 * s: "barfoothefoobarman"
 * words: ["foo", "bar"]
 *
 * You should return the indices: [0,9].
 * (order does not matter).
 */

#include <iostream>
#include <vector>

using namespace std;

vector<string> words_permutation(vector<string>& from, int m)
{
    if (m == 1) {
        auto vs = vector<string>(from);
        return vs;
    }

    vector<string> vs;

    for (auto it = from.begin(); it != from.end(); it++) {

        string take = *it;
        auto smaller_from = vector<string>(from);
        for (auto smaller_it = smaller_from.begin(); smaller_it != smaller_from.end(); smaller_it++) {
            if (*smaller_it == take) {
                smaller_from.erase(smaller_it);
                break;
            }
        }

        auto smaller_result = words_permutation(smaller_from, m-1);
        for (auto smaller_result_item : smaller_result) {
            smaller_result_item.append(take);
            vs.push_back(smaller_result_item);
        }
    }

    return vs;
}

ostream& operator<<(ostream& os, vector<string>& vs)
{
    for (const auto& s : vs) {
        os << s << ' ';
    }
    os << '\n';
    return os;
}

ostream& operator<<(ostream& os, vector<int>& vi)
{
    os << '[';
    for (auto it = vi.begin(); it != vi.end(); it++) {
        os << *it;

        if (it == vi.end()-1) {
            os << ']';
        } else {
            os << ", ";
        }
    }
    os << '\n';
    return os;
}

int main(int argc, char *argv[])
{
    if (argc < 3) {
        std::cerr << "Usage: " << argv[0] << " <given-string> <words>...\n";
        std::exit(0);
    }

    string search = argv[1];

    vector<string> vs;
    for (int i = 2; i < argc; i++) {
        vs.emplace_back(argv[i]);
    }

    vector<string> result = words_permutation(vs, static_cast<int>(vs.size()));
    cout << "words concatenations\n";
    cout << result;

    vector<int> result_index;

    for (auto& s : result) {
        auto index = search.find(s);
        if (index != string::npos) {
            result_index.emplace_back(index);
        }
    }

    if (!result_index.empty()) {
        std::cout << "found index\n";
        cout << result_index;
    } else {
        std::cout << "not found";
    }

    return 0;
}

