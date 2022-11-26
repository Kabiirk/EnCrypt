#include <iostream>
#include <string>
#include <bitset>
#include <vector>

static const std::string base64_chars = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";

class BASE64{
    public:
        BASE64(){};
        int pad = 0;

    static std::string base64_encode(const std::string &in) {
        std::string out;

        int val = 0, valb = -6;
        for (unsigned char c : in) {
            val = (val << 8) + c;
            valb += 8;
            while (valb >= 0) {
                out.push_back("ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/"[(val>>valb)&0x3F]);
                valb -= 6;
            }
        }
        if (valb>-6) out.push_back("ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/"[((val<<8)>>(valb+8))&0x3F]);
        while (out.size()%4) out.push_back('=');
        return out;
    }

    static std::string base64_decode(const std::string &in) {
        std::string out;

        std::vector<int> T(256,-1);
        for (int i=0; i<64; i++) T["ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/"[i]] = i;

        int val=0, valb=-8;
        for (unsigned char c : in) {
            if (T[c] == -1) break;
            val = (val << 6) + T[c];
            valb += 6;
            if (valb >= 0) {
                out.push_back(char((val>>valb)&0xFF));
                valb -= 8;
            }
        }
        return out;
    }

    /* POC Functions created to get a better understanding of how base64 encryption works*/
    std::string encode_str_to_base64_str(std::string password_string){
        std::string bin_string = string_to_bin_string(password_string);
        std::vector<std::string> sectors = divide_bin_string(bin_string);
        std::string encoded_string = divided_bin_string_to_base64_string(sectors);

        return encoded_string;
    }
    std::string decode_base64_str_to_str(std::string encoded_string){
        std::string password_string = "Decoded";
        return password_string;
    }
    std::string string_to_bin_string(std::string plaintext_pass){
        std::string bin_string_pass = "";
        for(auto i : plaintext_pass){
            bin_string_pass += std::bitset<8>(i).to_string();
        }
        return bin_string_pass;
    }
    std::vector<std::string> divide_bin_string(std::string binary_string, unsigned int gap=6, std::string pad_char="0"){
        int n = binary_string.size();
        int j=0;
        std::vector<std::string> result;
        std::string nth_substring = "";

        while(j<n){
            nth_substring += binary_string[j];
            if(nth_substring.size() == gap){
                result.push_back(nth_substring);
                nth_substring = "";
            }
            j++;
        }
        
        if(nth_substring != ""){
            while(nth_substring.size() < gap){
                nth_substring+=pad_char;
            }
            result.push_back(nth_substring);
        }

        if(result.size()%4 != 0){
            for(unsigned int i = 0; i< 4-(result.size()%4); i++){
                this->pad++;
            }
        }
        std::cout<<"Pad : "<<this->pad<<std::endl;

        return result;
    }

    std::string divided_bin_string_to_base64_string(std::vector<std::string> divided_bin_string){
        std::string base64_string = "";
        for(auto s : divided_bin_string){
            base64_string += base64_chars[stoi(s, 0, 2)];
        }
        for(int i = 0; i<this->pad; i++){
            base64_string +="=";
        }

        return base64_string;
    }

};