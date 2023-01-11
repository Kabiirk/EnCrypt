#include <iostream>
#include <vector>
#include "base64.h"
#include <curses.h>
#include "menu.h"

// Connecting to MySQL
// Windows
// #include "C:/Program Files/MySQL/MySQL Server 8.0/include/mysql.h"

// Linux
#include "mysql_connection.h"
#include <cppconn/driver.h>
#include <cppconn/exception.h>
#include <cppconn/resultset.h>
#include <cppconn/statement.h>
#include <cppconn/prepared_statement.h>


#define EXAMPLE_HOST "localhost"
#define EXAMPLE_USER "root"
#define EXAMPLE_PASS "password"
#define EXAMPLE_DB "encrypt"

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

std::string take_input(WINDOW *win, int y, int x){
    wmove(win,y,x);
    std::string s;
    char a = wgetch(win);
    while(1){
        if(a=='\n'){
            break;
        }
        s.push_back(a);
        a = wgetch(win);
    }
    return s;
}

int main(){
    // Initialize TUI
    initscr();
    noecho();
    cbreak();
    keypad(stdscr, TRUE);
    curs_set(0);

    // Initialize MySQL Connection
    const string url=EXAMPLE_HOST;
    const string user=EXAMPLE_USER;
    const string pass=EXAMPLE_PASS;
    const string database=EXAMPLE_DB;
    sql::Driver* driver = get_driver_instance();
    sql::Statement* stmt;
    sql::ResultSet* res;
    unique_ptr<sql::Connection> con(driver->connect(url, user, pass));
    con->setSchema(database);
    stmt = con->createStatement();


    int yMax, xMax;
    getmaxyx(stdscr, yMax, xMax);

    WINDOW *win = newwin(3*yMax/4, (3*xMax/4)-20, yMax/8, (xMax/8)+20);
    WINDOW *win2 = newwin((3*yMax/8)+1, 20, yMax/8, xMax/8);
    WINDOW *win3 = newwin(3*yMax/8, 20, yMax/2, xMax/8);

    // scrollable pad
    // WINDOW *pad = newpad((3*yMax/4) + 1, xMax);


    box(win, 0, 0);
    box(win2, 0, 0);
    box(win3, 0, 0);

    wrefresh(win);
    wrefresh(win2);
    wrefresh(win3);


    scrollok(win, TRUE);

    // Initialize Menubars
    Menu menus[5] = {
        Menu("Add", 'a'),
        Menu("Delete", 'd'),
        Menu("View", 'v'),
        Menu("Settings", 's'),
        Menu("[X]", 'x'),
    };

    Menubar menubar = Menubar(win, menus, 5, true);
    Menubar menubar2 = Menubar(win3, menus, 5, false);
    menubar.draw();
    menubar2.draw();


    char ch='a';
    while ( (ch!='x') )
    {
        // Menubar : Check user input for triggers
        ch = wgetch(win);
        if(ch=='x'){
            break;
        }
        // menubar.handleTriggers(ch);

        if(ch=='\n'){
            menubar.clearScreen();
            std::string ets = menubar.menus[menubar.selected_menu].text+" was pressed !";
            wmove(menubar.win,2,2);
            if(menubar.menus[menubar.selected_menu].text == "Add"){
                std::string request = "Please enter Site URL:";
                menubar.draw();
                menubar2.draw();
                mvwprintw(win, 2,2, request.c_str());
                echo();
                std::string s1 = take_input(win, 3, 2);
                
                request = "Please Enter Username:";
                menubar.draw();
                mvwprintw(win, 4,2, request.c_str());
                std::string s2 = take_input(win, 5, 2); 

                request = "Please enter your password:";
                menubar.draw();
                mvwprintw(win, 6,2, request.c_str());
                std::string s3 = take_input(win, 7, 2);
                noecho();

                stmt->execute("INSERT INTO passwords (SITENAME, USERNAME, PASSWORD_B64) VALUES ('"+s1+"', '"+s2+"', '"+s3+"');");
                
                request = "DONE ! Press Enter to return to Home screen";
                mvwprintw(win, 9,2, request.c_str());
                std::string s4 = take_input(win, 7, 2);
                menubar.clearScreen();
            }
            else if(menubar.menus[menubar.selected_menu].text == "Delete"){
                std::string request = "Please enter username to delete:";
                menubar.draw();
                mvwprintw(win, 2,2, request.c_str());
                echo();
                std::string s1 = take_input(win, 3, 2);
                noecho();

                stmt->execute("DELETE FROM passwords where USERNAME='"+s1+"';");
    
                string done = "Password Deleted ! Press Enter to return to Home screen";
                mvwprintw(win, 5,2, done.c_str());
                std::string s4 = take_input(win, 7, 2);
                menubar.clearScreen();
            }
            else if(menubar.menus[menubar.selected_menu].text == "View"){
                menubar.draw();

                // Scrolling window WIP
                res = stmt->executeQuery("SELECT * FROM passwords");
                std::string s = "URL, Username, Password";
                wattron(win, A_BOLD);
                mvwprintw(win, 2,2, s.c_str());
                s = "-----------------------";
                mvwprintw(win, 3,2, s.c_str());
                wattroff(win, A_BOLD);
                int i = 4;
                while (res->next()) {
                    std::string result = res->getString(1)+", "+res->getString(2)+", "+res->getString(3);
                    mvwprintw(win, i,2, result.c_str());
                    i++;
                }
                cout<<endl;
            }
            else{
                wmove(win,2,2);
                // wclrtoeol(menubar.win);
                mvwprintw(win, 2,2, ets.c_str());
            }
            // box(menubar.win, 0, 0);
        }
        for(int i = 0; i<menubar.num_menus; i++){
            if(ch == menubar.menus[i].trigger){
                menubar.selected_menu = i;
                menubar2.selected_menu = i;
            }
        }


        menubar.draw();
        menubar2.draw();
    }

	endwin();

    return 0;
}