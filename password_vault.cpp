#include <iostream>
#include <vector>
#include "base64.h"
#include <curses.h>
#include "menu.h"
#include <string>

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

// Sleep
#include <chrono>
#include <thread>


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

void debug_print(WINDOW * win, vector< std::pair<int, int> > coords){
    std::string str = "*";
    for (auto coord : coords){
        mvwprintw(win, coord.second, coord.first, str.c_str());
    }
}

void window_refresh_print(WINDOW * win, vector<std::string> result_array, int viewport_begin, int viewport_height,int active_row){
    for(int i = viewport_begin; i<viewport_begin+viewport_height; i++){
        if(i == active_row){
            wattron(win, A_STANDOUT);
        }
        mvwprintw(win, viewport_begin, 1, result_array[i].c_str());
        wattroff(win, A_STANDOUT);
    }
    wrefresh(win);
}

int main(){
    // Initialize TUI
    initscr();
    noecho();
    cbreak();
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

    int height = 3*yMax/4;
    int width = (3*xMax/4);

    WINDOW *pad = newpad(height, width);
    keypad(pad, TRUE);

    // scrollable pad
    // WINDOW *pad = newpad((3*yMax/4) + 1, xMax);


    box(win, 0, 0);
    box(win2, 0, 0);
    box(win3, 0, 0);
    box(pad, 0, 0);

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
            // Temporary fix to view pad
            // menubar.clearScreen();
            std::string ets = menubar.menus[menubar.selected_menu].text+" was pressed !";
            wmove(menubar.win,2,2);
            if(menubar.menus[menubar.selected_menu].text == "Add"){
                menubar.clearScreen();
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
                menubar2.draw();

                // Scrolling window WIP
                res = stmt->executeQuery("SELECT * FROM passwords");
                // TODO : After assignment, last password will stay in memory, need to clear this
                //        Variable after every view activity.
                std::string result = "";
                vector<std::string> result_array;
                std::string s = "URL, Username, Password";
                wattron(win, A_BOLD);
                mvwprintw(win, 2,3, s.c_str());
                s = "-----------------------";
                mvwprintw(win, 3,3, s.c_str());
                wattroff(win, A_BOLD);
                wrefresh(win); // so that above text is visible
                int i = 0;
                while (res->next()) {
                    result = res->getString(1)+", "+res->getString(2)+", "+res->getString(3);
                    result_array.push_back(result);
                    mvwprintw(pad, i,2, result.c_str());
                    i++;
                }

                /*APPROACH 1 : PAD*/

                int posy = (yMax/8)+1;
                int posx = (xMax/8)+21;
                int init_posy = 0;
                // Keep track of visible result array
                int init_active_row = -1;
                // visible window would be from 0 -> y_win_max-5
                int y_win_beg = 0;
                int x_win_end, y_win_end;
                getmaxyx(win, y_win_end, x_win_end);// check this

                // Scroll viewport implementation 2
                int width2, height2;
                getmaxyx(win, height2, width2);
                int x_view_beg = (xMax/8)+21;//1;
                int y_view_beg = (yMax/8)+1;//4;
                int x_view_end = x_view_beg+width2-3;
                int y_view_end = y_view_beg+height2-6;

                prefresh(pad, init_posy,0,posy+3,posx,height,width);
                int scroll_key = 0;
                while(scroll_key!='x'){
                    scroll_key = wgetch(pad);
                    if(scroll_key=='x'){
                        continue;
                    }
                    if(scroll_key==KEY_UP){
                        // if(y_win_beg == 0){continue;}
                        // init_posy--;
                        // Viewport
                        init_active_row--;
                        // y_win_beg--;
                        // y_win_end--;

                        // Emphasis
                        for(int i = 0; i<result_array.size(); i++){
                            if(i == init_active_row){
                                wattron(pad, A_STANDOUT);
                            }
                            mvwprintw(pad, i,2, result_array[i].c_str());
                            wattroff(pad, A_STANDOUT);
                        }
                    }
                    if(scroll_key==KEY_DOWN){
                        if(init_posy == result_array.size()){continue;}
                        // init_posy++;
                        // // Viewport
                        init_active_row++;
                        // y_win_beg++;
                        // y_win_end++;

                        // Emphasis
                        for(int i = 0; i<result_array.size(); i++){
                            if(i == init_active_row){
                                wattron(pad, A_STANDOUT);
                            }
                            mvwprintw(pad, i,2, result_array[i].c_str());
                            wattroff(pad, A_STANDOUT);
                        }
                    }
                    if(scroll_key=='\n'){
                        mvwprintw(win, 1,1, result_array[init_active_row].c_str());
                        wrefresh(win);
                    }
                    prefresh(pad, init_posy,0,posy+3,posx,height,width);
                    menubar.draw();
                    menubar2.draw();

                    // Debugging visible password window
                    // mvwprintw(win2, 1,1, std::to_string(result_array.size()).c_str());
                    // mvwprintw(win2, 2,1, std::to_string(y_view_beg).c_str());
                    // mvwprintw(win2, 3,1, std::to_string(y_view_end).c_str());
                    mvwprintw(win2, 4,1, std::to_string(init_active_row).c_str());
                    // mvwprintw(win2, 5,1, std::to_string(yMax).c_str());
                    // mvwprintw(win2, 6,1, std::to_string(xMax).c_str());

                    wrefresh(win2);
                }

            }
            else if(menubar.menus[menubar.selected_menu].text == "Settings"){
                menubar.clearScreen();
                int width, height;
                getmaxyx(win, height, width);
                int x_view_beg = 1;
                int y_view_beg = 4;
                int x_view_end = x_view_beg+width-3;
                int y_view_end = y_view_beg+height-6;


                vector<std::pair<int, int>> v_view = {
                    std::make_pair(x_view_beg, y_view_beg),// view top left
                    std::make_pair(x_view_end, y_view_end),// view bottom right
                    std::make_pair(x_view_end, y_view_beg),
                    std::make_pair(x_view_beg, y_view_end),
                };

                debug_print(win, v_view);
                wrefresh(win);                
            }
            else{
                menubar.clearScreen();
                wmove(win,2,2);
                // wclrtoeol(menubar.win);
                mvwprintw(win, 2,3, ets.c_str());
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