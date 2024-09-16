#include <ncurses.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <ctype.h>

// functie care afiseaza meniul
void get_menu(int *play)
{
    mvprintw(3, 30, "MENU\n");
    refresh();
    // am creat fereastra pentru meniu
    WINDOW *menu = newwin(7, 60, 4, 4);
    // i-am creat o margine pentru a o face vizibila
    box(menu, 0, 0);
    wrefresh(menu);
    // instructiune necesara pentru ca utilizatorii sa poata interactiona
    // cu tastatura
    keypad(menu, true);
    char menuoptions[2][100] = {"Start a new game", "Exit"};
    int i, chosen = 0, option = 0;
    // perechea de culori pe care am folosit-o pentru evidentierea optiunii
    // selectate
    // variabila chosen va indica daca optiunea a fost sau nu selectata
    // variabila option retine tasta apasata de utlizator
    init_pair(4, COLOR_BLACK, COLOR_RED);
    // cat timp nu este apasata tasta enter, decizia nu este definitiva,
    // iar utilizatorul poate selecta orice optiune
    while (option != '\n')
    {
        for (i = 0; i < 2; i++)
        {
            // cazul in care optiunea a fost selectata
            if (i == chosen)
            {
                wattron(menu, COLOR_PAIR(4));
            }
            mvwprintw(menu, 2 + i, 6, "%s", menuoptions[i]);
            // afisarea meniului
            wattroff(menu, COLOR_PAIR(4));
        }
        // se verifica ce tasta a fost apasata de utilizator si optiunea
        // aleasa este evidentiata
        option = wgetch(menu);
        if (option == KEY_UP)
        {
            chosen--;
            if (chosen < 0)
                chosen = 0;
        }
        else if (option == KEY_DOWN)
        {
            chosen++;
            if (chosen > 1)
                chosen = 1;
        }
        // daca utilizatorul apasa tasta enter, se iese din bucla, deoarece
        // a fost luata o decizie
        if (option == '\n')
        {
            break;
        }
    }
    // daca utilizatorul a ales sa joace un nou joc, parametrul play devine 1
    // ceea ce indica faptul ca trebuie incarcata o noua sesiune a jocului
    if (chosen == 0)
    {
        *(play) = 1;
    }
    // daca utilizatorul a ales sa iasa din joc, parametrul play devine 0
    // ceea ce indica faptul ca trebuie sa se iasa din joc
    if (chosen == 1)
    {
        *(play) = 0;
    }
}

// functie care alege un cuvant aleatoriu pentru joc
void pick_a_word(char words[][100], char chosen[])
{
    int random;
    srand(time(NULL));
    random = rand() % 27;
    strcpy(chosen, words[random]);
}

// declararea patratelor ce alcatuiesc tabla de joc
WINDOW *square[30];

// functie care creeaza tabla de joc
void table(void)
{
    int i, y = 4, x, j, k = 0;
    // sunt create 6 randuri de cate 5 ferestre
    for (i = 0; i < 6; i++)
    {
        x = 1;
        for (j = 0; j < 5; j++)
        {
            square[k] = newwin(4, 6, y, x);
            box(square[k], 0, 0);
            wrefresh(square[k]);
            k++;
            x = x + 6;
        }
        y = y + 4;
    }
}

// functie pentru adaugarea cuvintelor introduse de utilizator
void add_word(WINDOW *square[], int *length, int *nr, int *b, char chosen[],
              int *(found))
{
    int i, ok, j;
    // parametrul found verifica daca a fost ghicit cuvantul
    *(found) = 0;
    // in sirul cuv se pastreaza cuvantul introdus de utilizator
    char cuv[100];
    char c;
    c = getch();
    // cat timp nu este apasata tasta enter, se alcatuieste cuvantul, litera
    // cu litera
    while (c != '\n')
    {
        // instructiune care asigura ca literele cuvintelor vor fi mari
        c = toupper(c);
        cuv[*(length)] = c;
        // cuvantul este afisat pe masura ce este format
        mvprintw(3, *(length), "%c", c);
        c = getch();
        // in cazul in care a fost apasata tasta backspace, cat timp
        // acesta continua sa fie apasata, caracterele sirului cuv sunt
        // inlocuite cu spatii, iar lungimea initiala a sirului scade
        while (c == 127)
        {
            mvprintw(3, *(length), " ");
            *(length) = *(length)-1;
            c = getch();
        }
        *(length) = *(length) + 1;
    }
    // se elimina cuvantul afisat
    move(3, 0);
    clrtoeol();
    // daca lungimea cuvantului introdus de utilizator este mai mare decat 5
    // se afiseaza un mesaj care semnaleaza acest lucru, iar utilizatorul este
    // rugat sa apese pe enter pentru a introduce un nou cuvant
    if (*(length) != 5)
    {
        mvprintw(3, 0,
                 "Please enter a five-letter word! Press enter to try again.");
        move(3, 0);
        c = getch();
        // in cazul in care utilizatorul a apasat tasta enter, mesajul dispare
        if (c == '\n')
        {
            clrtoeol();
        }
    }
    // cazul in care cuvantul introdus are lungimea corecta
    else
    {
        // variabila nr retine numarul de incercari care i-au fost necesare
        // utilizatorului pentru a ghici cuvantul
        *(nr) = *(nr) + 1;
        // perechile de culori pentru litere
        init_pair(1, COLOR_BLACK, COLOR_GREEN);
        init_pair(2, COLOR_BLACK, COLOR_YELLOW);
        init_pair(3, COLOR_WHITE, COLOR_BLACK);
        for (i = 0; i < 5; i++)
        {
            // daca litera cuvantului introdus este pe pozitia corecta
            // aceasta este adaugata in fereastra corespunzatoare cu
            // cu prima pereche de culori
            if (cuv[i] == chosen[i])
            {
                wattron(square[*(b) + i], COLOR_PAIR(1));
                mvwaddch(square[*(b) + i], 1, 3, cuv[i]);
                wrefresh(square[*(b) + i]);
                wattroff(square[*(b) + i], COLOR_PAIR(1));
                // valoarea lui found creste, indicand faptul ca a fost gasita
                // o litera a cuvantului
                *(found) = *(found) + 1;
            }
            else
            {
                // se cauta daca litera face parte din cuvantul ce trebuie
                // ghicit
                ok = 0;
                for (j = 1; j < 5; j++)
                {
                    if (chosen[j] == cuv[i])
                    {
                        ok++;
                    }
                }
                // daca aceasta nu este gasita, litera este adaugata cu
                // perechea de culori corespunzatoare
                if (ok == 0)
                {
                    wattron(square[*(b) + i], COLOR_PAIR(3));
                    mvwaddch(square[*(b) + i], 1, 3, cuv[i]);
                    wrefresh(square[*(b) + i]);
                    wattroff(square[*(b) + i], COLOR_PAIR(3));
                }
                // daca aceasta este gasita, litera este adaugata cu
                // perechea de culori corespunzatoare
                else
                {
                    wattron(square[*(b) + i], COLOR_PAIR(2));
                    mvwaddch(square[*(b) + i], 1, 3, cuv[i]);
                    wrefresh(square[*(b) + i]);
                    wattroff(square[*(b) + i], COLOR_PAIR(2));
                }
            }
        }
        // b creste cu 5, trecandu-se la urmatorul rand de ferestre
        *(b) = *(b) + 5;
    }

    *(length) = 0;
}

// functia principala a jocului
void wordle(char words[][100], char chosen[], WINDOW *square[], int *win,
            int *try)
{
    // mai intai se alege un cuvant care trebuie ghicit
    pick_a_word(words, chosen);
    refresh();
    // se afiseaza tabla de joc
    table();
    int b = 0, found = 0, nr = 0, k = 0;
    // cat timp cuvantul nu este ghicit si numarul de incercari este mai mic
    // decat 6, se adauga cuvinte
    while (found != 5 && nr < 6)
    {
        add_word(square, &k, &nr, &b, chosen, &found);
        refresh();
    }
    // variabila win indica daca a fost gasit cuvantul
    // variabila try indica numarul de incercari
    *(win) = found;
    *(try) = nr;
}

int main()
{
    initscr();
    cbreak();
    noecho();
    start_color();
    mvprintw(0, 25, "WORDLE\n");
    refresh();
    printw("Press enter to start the game!\n");
    refresh();
    int x = 1, y = 4, i, j, w, t, play, title = 0;
    char words[100][100] = {
        "ARICI",
        "ATENT",
        "BAIAT",
        "CEATA",
        "DEBUT",
        "PESTE",
        "FIXAT",
        "HAMAC",
        "HARTA",
        "JALON",
        "JUCAM",
        "LACAT",
        "MAGIE",
        "NUFAR",
        "OASTE",
        "PERUS",
        "RIGLE",
        "ROMAN",
        "SANIE",
        "SCRIS",
        "SONDA",
        "TEXTE",
        "TIPAR",
        "TITAN",
        "ZEBRA",
        "VAPOR",
        "VATRA",
    };
    char chosen[6], cuv[100];
    char c = getch();
    // daca a fost apasata tasta enter, jocul incepe
    if (c == '\n')
    {
        // variabila play ia valoarea 1, cat timp jocul se desfasoara si
        // devine 0, atunci cand utilizatorul doreste sa paraseasca jocul
        play = 1;
        while (play == 1)
        {
            // in cazul in care nu este primul joc, se adauga iarasi titlul
            if (title == 1)
            {
                mvprintw(0, 25, "WORDLE\n");
                refresh();
                printw(
                    "Press enter to start the game or : to enter the menu!\n");
                refresh();
            }
            // jocul
            wordle(words, chosen, square, &w, &t);
            // cazul in care a fost gasit cuvantul
            if (w == 5)
            {
                clear();
                mvprintw(5, 20, "YOU WON!");
                mvprintw(6, 20, "Press : to enter the menu and play again."
                , chosen);
                c = getch();
                // apasarea tastei : deschide meniul
                if (c == ':')
                {
                    get_menu(&play);
                    // daca variabila play este 1, inseamna ca utilizatorul a
                    // ales sa joace din nou
                    if (play == 1)
                    {
                        title = 1;
                    }
                }
                clear();
            }
            // cazul in care nu a fost gasit cuvantul
            else if (t == 6)
            {
                clear();
                mvprintw(5, 20, "YOU LOST!");
                mvprintw(6, 20, "The word: %s!", chosen);
                mvprintw(6, 20, "Press : to enter the menu and try again"
                , chosen);
                c = getch();
                // apasarea tastei : deschide meniul
                if (c == ':')
                {
                    get_menu(&play);
                    // daca variabila play este 1, inseamna ca utilizatorul a
                    // ales sa joace din nou
                    if (play == 1)
                    {
                        title = 1;
                    }
                }
                clear();
            }
        }
        // daca variabila play este 0, inseamna ca utilizatorul a
        // ales sa iasa din joc
        if (play == 0)
        {
            endwin();
            refresh();
        }
    }
    return 0;
}
