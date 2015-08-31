#include <algorithm>
#include <cxxabi.h>
#include <iostream>
#include <string>
#ifdef USE_RE2
#include <re2/re2.h>
using Regex = RE2;
#else
#include <regex>
using Regex = std::regex;
#endif

using std::advance;
using std::string;
using std::stoi;
using std::cout;

bool demangle(string s, string& res) {
    int status;
    char* name = abi::__cxa_demangle(s.c_str(), nullptr, 0, &status);
    if (status != 0) {
        string err;
        switch(status) {
        case -1: res = "memory allocation error"; break;
        case -2: res = "invalid name given"; break;
        case -3: res = "internal error: __cxa_demangle: invalid argument"; break;
        default: res = "unknown error occured"; break;
        }
        return false;
    }
    res = name;
    free(name);
    return true;
}

int main(int argc, char** argv) {
    if (argc != 2) {
        std::cerr << "usage: " << argv[0] << " <name>\n";
        return 1;
    }
    string name;
    if (!demangle(argv[1], name)) {
        std::cerr << name << '\n';
        return 1;
    }
    size_t pos{}, tmp{};
    int ind{};
    // functional stuff
    auto drop = [&](size_t i) { return name.substr(pos+i); };
    auto chr = [&](size_t i) { return pos+i < name.length() ? name[pos+i] : '\0'; };
    auto startswith = [&](string prefix) -> bool {
        return name.length()-pos >= prefix.length() &&
            std::equal(prefix.begin(), prefix.end(), name.begin()+pos);
    };
    auto word = [&](string word) -> bool {
        bool r = name.length()-pos >= word.length()
                ? startswith(word) && !isalpha(chr(0))
                : startswith(word);
        if (r) tmp = word.length();
        return r;
    };
    auto ln = [&]() {
        cout << '\n';
        for (int i=0; i<ind; ++i) cout << "  ";
    };
    string rm; // regex match
    auto match = [&](const Regex& re) {
        #ifdef USE_RE2
        re2::StringPiece p{name.c_str()+pos};
        return RE2::PartialMatch(p, re, &rm);
        #else
        std::smatch sm;
        bool res = std::regex_search(name.cbegin()+pos, name.cend(), sm, re);
        if (res) rm = sm[1];
        return res;
        #endif
    };
    string
        c_reset{"\033[0m"},
        c_char{"\033[36m"},
        c_digit{"\033[34m"},
        c_kw{"\033[35m"},
        c_lambda{"\033[32m"},
        c_ptr{"\033[31m"},
        c_ns{"\033[33m"};
    Regex kw{"^(int|char|std|unsigned|long|short)"};
    while (pos < name.length()) {
        if (startswith("(char)")) {
            pos += 6;
            cout << c_char << '\'' << static_cast<char>(stoi(drop(0), &tmp))
                 << '\'';
            pos += tmp;
        } else if (isdigit(chr(0)) || (chr(0) == '-' && isdigit(chr(1)))) {
            cout << c_digit << stoi(drop(0), &tmp);
            pos += tmp;
            while (chr(0) == 'u' || chr(0) == 'l') {
                cout << chr(0);
                ++pos;
            }
        } else if (match(kw)) {
            cout << c_kw << rm;
            pos += rm.length();
        } else if (word("lambda")) {
            cout << c_lambda << "lambda";
            pos += 6;
        } else if (chr(0) == '*') {
            cout << c_ptr << '*';
            ++pos;
        } else if (startswith("::")) {
            cout << c_ns << "::";
            pos += 2;
        } else if (chr(0) == '<') {
            cout << '<';
            ++pos;
            ++ind;
            ln();
        } else if (chr(0) == '>') {
            cout << '>';
            --ind;
            if (chr(1) == ' ') {
                putchar(' ');
                ++pos;
            }
            ++pos;
            if (chr(0) == '>') while (chr(0) == '>') {
                cout << '>';
                --ind;
                ++pos;
                if (chr(0) == ' ') {
                    cout << ' ';
                    ++pos;
                }
            }
            if (chr(0) == ',') {
                cout << ',';
                ++pos;
                if (chr(0) == ' ') ++pos;;
            }
            ln();
        } else {
            cout << chr(0);
            ++pos;
        }
        cout << c_reset;
    }
}
