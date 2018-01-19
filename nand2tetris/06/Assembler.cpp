#include <bits/stdc++.h>
using namespace std;

map<string, int> mp;
map<string, string> mpc_dest;
map<string, string> mpc_comp;
map<string, string> mpc_jump;

int globle_counter = 16;
vector<string> file;
vector<string> global_res;

template<typename T>
std::string ToString(T x){
    std::stringstream sout;
    sout << x;
    return sout.str();
}

template<typename T>
T StringTo(const std::string &s){
    std::stringstream sout;
    sout << s;
    T tmp;
    sout >> tmp;
    return tmp;
}
// 这个函数啊！超级关键啊！
string remove_the_blanks_both_side(string t) {
    int b = 0;
    int e = t.size()-1;
    while (t[b]==' ') b++;
    while (t[e]==' ') e--;
    return string(t.begin()+b, t.begin()+e+1);
}

string get_binary(int x) {
    string ret;
    for (int i=0; i<15; i++) {
        if (x%2) ret += '1';
        else ret += '0';
        x = x/2;
    }
    reverse(ret.begin(), ret.end());
    return ret;
}

int get_var_number(string s) {

    if (mp.find(s) == mp.end()) {
        mp[s] = globle_counter++;
    }

    return mp[s];
}

bool all_blank(const string &s) {
    for (int i=0; i<s.size(); i++) {
        if (s[i] != ' ') return false;
    }
    return true;
}


void init_symbol_table() {
    for (int i=0; i<16; i++) {
        string tmp = "R" + ToString<int>(i);
        mp[tmp] = i;
    }
    mp["SCREEN"] = 16384;
    mp["KBD"] = 24576;
    mp["SP"] = 0;
    mp["LCL"] = 1;
    mp["ARG"] = 2;
    mp["THIS"] = 3;
    mp["THAT"] = 4;

    mpc_dest["null"] = "000";
    mpc_dest["M"] = "001";
    mpc_dest["D"] = "010";
    mpc_dest["MD"] = "011";
    mpc_dest["A"] = "100";
    mpc_dest["AM"] = "101";
    mpc_dest["AD"] = "110";
    mpc_dest["AMD"] = "111";

    mpc_jump["null"] = "000";
    mpc_jump["JGT"] = "001";
    mpc_jump["JEQ"] = "010";
    mpc_jump["JGE"] = "011";
    mpc_jump["JLT"] = "100";
    mpc_jump["JNE"] = "101";
    mpc_jump["JLE"] = "110";
    mpc_jump["JMP"] = "111";


    mpc_comp["0"] = "0101010";
    mpc_comp["1"] = "0111111";
    mpc_comp["-1"] = "0111010";
    mpc_comp["D"] = "0001100";
    mpc_comp["A"] = "0110000";

    mpc_comp["!D"] = "0001101";
    mpc_comp["!A"] = "00110001";
    mpc_comp["-D"] = "0001111";
    mpc_comp["-A"] = "0110011";
    mpc_comp["D+1"] = "0011111";

    mpc_comp["A+1"] = "0110111";
    mpc_comp["D-1"] = "0001110";
    mpc_comp["A-1"] = "0110010";
    mpc_comp["D+A"] = "0000010";
    mpc_comp["D-A"] = "0010011";

    mpc_comp["A-D"] = "0000111";
    mpc_comp["D&A"] = "0000000";
    mpc_comp["D|A"] = "0010101";


     mpc_comp["M"] = "1110000";
     mpc_comp["!M"] = "1110001";
     mpc_comp["-M"] = "1110011";
     mpc_comp["M+1"] = "1110111";
     mpc_comp["M-1"] = "1110010";

     mpc_comp["D+M"] = "1000010";
     mpc_comp["D-M"] = "1010011";
     mpc_comp["M-D"] = "1000111";
     mpc_comp["D&M"] = "1000000";
     mpc_comp["D|M"] = "1010101";
}

void add_labels() {
    int cnt = 0;
    vector<string> tmp;
    for (auto line: file) {
        if (line[0] == '(') {

            mp[line.substr(1, line.size()-2)] = cnt;
        } else {
            cnt ++;
            tmp.push_back(line);
        }
    }
    file = tmp;
}


void handle_A_instruction(string s) {
    //s = remove_the_blanks_both_side(s);
    int value;
    string tmp;
    if (s[1] <= '9' && s[1] >= '0') {
        value = StringTo<int>(s.substr(1));
    } else {
        tmp = s.substr(1);
        value = get_var_number(tmp);
    }
    string res = "0";
    res += get_binary(value);
    global_res.push_back(res);

}

void handle_C_instruction(string s) {
    //s = remove_the_blanks_both_side(s);
    string jump, comp, dest;
    int pos = s.find(';');
    if (pos < s.size()) {
        jump = s.substr(pos+1);
        s = string(s.begin(), s.begin()+pos);
    } else {
        jump = "null";
    }

    pos = s.find('=');
    if (pos < s.size()) {
        comp = s.substr(pos+1);
        dest = string(s.begin(), s.begin()+pos);
    } else {
        comp = s;
        dest = "null";
    }
    string res = "111";
    res +=( mpc_comp[comp] + mpc_dest[dest] + mpc_jump[jump]);

    global_res.push_back(res);
}


void process() {
    for (auto line: file) {
        if (line[0] == '@') {
            handle_A_instruction(line);
        } else {
            handle_C_instruction(line);
        }
    }
}


int main() {

    freopen("in.txt", "r", stdin);
    string line;
    while (getline(cin, line)) {
        int pos = line.find('/');
        if (pos < line.size())
            line = string(line.begin(), line.begin()+pos);
        if (all_blank(line)) continue;
        file.push_back(line);
    }
    init_symbol_table();
    add_labels();
    process();
    for (auto line: global_res) cout << line << endl ;
}
