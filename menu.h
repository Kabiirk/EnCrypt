#include<curses.h>
#include<string>

#ifndef _MENU_H_
#define _MENU_H_

class Menu{
    public:
        Menu(std::string text, char trigger){
            this->text = text;
            this->trigger = trigger;
        }
        int start_x;
        std::string text;
        char trigger;
};

class Menubar{
    public:
        Menubar(WINDOW *win, Menu* menus, int num_menus){
            this->win = win;
            this->menus = menus;
            this->num_menus = num_menus;
            this->selected_menu = -1;

            int current_pos = 2;

            for(int i = 0; i<num_menus; i++){
                this->menus[i].start_x = current_pos;
                current_pos += this->menus[i].text.length()+2;
            }
        }
        WINDOW *win;
        Menu *menus;
        int num_menus;
        int selected_menu;

    void draw(){
        for(int i = 0; i<num_menus; i++){
            int start_x = this->menus[i].start_x;
            std::string text = this->menus[i].text;
            if(selected_menu==i){
                wattron(win, A_STANDOUT);
            }
            mvwprintw(win, 0, start_x, text.c_str());
            wattroff(win, A_STANDOUT);
        }
    }

    void handleTriggers(char trigger){
        if(trigger=='\n'){
            std::string et = this->menus[selected_menu].text+" was pressed !";
            wmove(this->win,2,2);
            wclrtoeol(this->win);
            if(this->menus[selected_menu].text == "File"){
                std::string et[4] = {
                    "A",
                    "B",
                    "C"
                };
                int i = 2;
                for(auto e : et){
                    mvwprintw(win, i,2, e.c_str());
                    i++;
                }
                i = 2;
            }
            box(this->win, 0, 0);
        }
        for(int i = 0; i<num_menus; i++){
            if(trigger == this->menus[i].trigger){
                selected_menu = i;
            }
        }
    }

};

#endif
