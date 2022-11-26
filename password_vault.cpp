#include <iostream>
#include <vector>
#include "base64.h"
#include <fstream>

#include "C:/Program Files/MySQL/MySQL Server 8.0/include/mysql.h"

using namespace std;

class PASSWORD{
    public:
        string url="";
        string username="";
        string password="";
        bool password_is_set = false;
        BASE64 base64;

    string get_a(bool decode=false){
        if(!password_is_set){
            return "Set a Password First !";
        }
        else{
            if(decode){
                return this->url+" | "+this->username+" | "+this->base64.base64_decode(this->password);
            }
            else{
                return this->url+" | "+this->username+" | "+this->password;
            }
        }
    }

    void set_a(string site, string user, string pass){
        this->url = site;
        this->username = user;
        this->password = this->base64.base64_encode(pass);
        this->password_is_set = true;
    }

    bool validate_password(string pass){
        return base64.base64_encode(pass) == this->password;
    }
};

void printVect(vector<string> v){
    cout<<"< ";
    for(auto s : v){
        cout<<s<<" ";
    }
    cout<<" >"<<endl;
}

int main(){
    MYSQL * conn;
    MYSQL_ROW row;
    MYSQL_RES *res;
    int qstate1, qstate2, qstate4;
    conn = mysql_init(0);
    conn = mysql_real_connect(conn, "localhost", "root", "password", "encrypt", 3306, NULL, 0);
    if(conn){
        cout<<"DONE !!"<<endl;
        // string query1 = "SELECT * FROM passwords";
        // string query2 = "INSERT INTO passwords (SITENAME, USERNAME, PASSWORD_B64) VALUES ('test3.com', 'test3', 'something64==');";
        // const char* q1 = query1.c_str();
        // const char* q2 = query2.c_str();
        // qstate1 = mysql_query(conn, q1);
        // qstate2 = mysql_query(conn, q2);
        // if(!qstate1){
        //     res=mysql_store_result(conn);
        //     while( (row = mysql_fetch_row(res)) ){
        //         printf("SITE: %s | USER: %s | VALUE: %s\n", row[0], row[1], row[2]);
        //     }
        // }
        // else{
        //     cout<<"Query Failed"<<endl;
        // }
    }
    else{
        cout<<"NOT DONE !!"<<endl;
    }

    PASSWORD pass;
    vector<PASSWORD> passwords;

    // cout<<pass.get_a()<<endl;
    // pass.set_a("https://www.test.com", "test", "abcdefg");
    // cout<<pass.get_a()<<endl;
    // cout<<pass.get_a(true)<<endl;
    // cout<<pass.validate_password("abcdefg")<<endl;
    // cout<<pass.validate_password("acdfg")<<endl;

    while(true){
        cout<<"What would you like to do ?"<<endl;
        cout<<"1. Set Password"<<endl;
        cout<<"2. View all Passwords"<<endl;
        cout<<"3. Exit"<<endl;
        cout<<"4. Clear Test"<<endl;

        int choice=0;
        cin>>choice;

        if(choice==1){
            string query1 = "INSERT INTO passwords (SITENAME, USERNAME, PASSWORD_B64) VALUES ('test3.com', 'test3', 'something64==');";
            const char* q1 = query1.c_str();
            qstate1 = mysql_query(conn, q1);
            if(!qstate1){
                cout<<"Inserted Succsessfully"<<endl;
            }
            else{
                cout<<"Insert failed"<<endl;
            }
            cout<<""<<endl;
        }
        if(choice==2){
            string query2 = "SELECT * FROM passwords";
            const char* q2 = query2.c_str();
            qstate2 = mysql_query(conn, q2);
            if(!qstate2){
                res=mysql_store_result(conn);
                while( (row = mysql_fetch_row(res)) ){
                    printf("SITE: %s | USER: %s | VALUE: %s\n", row[0], row[1], row[2]);
                }
            }
            else{
                cout<<"Query Failed"<<endl;
            }
            cout<<""<<endl;
        }
        if(choice==3){
            cout<<"Exiting"<<endl;
            break;
        }
        if(choice==4){
            string query4 = "DELETE FROM passwords where USERNAME='test3'";
            const char* q4 = query4.c_str();
            qstate4 = mysql_query(conn, q4);
            if(!qstate4){
                cout<<"Deleted !"<<endl;
            }
            else{
                cout<<"Query Failed"<<endl;
            }
            cout<<""<<endl;
        }
    }
    cout<<"BYE !"<<endl;

    return 0;
}