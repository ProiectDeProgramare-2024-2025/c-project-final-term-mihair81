#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ncurses.h>
#include <unistd.h>
#include <ctype.h>

#define NUM_OPTIONS 4
#define MAX_CONTACTS 100
#define FILENAME "contacts.txt"
#define BLOCKED_FILENAME "blocked_contacts.txt"
#define MAX_LINE 100

typedef struct {
    char name[50];
    char phone[20];
} Contact;

Contact contacts[MAX_CONTACTS];
int contactCount = 0;

Contact blockedContacts[MAX_CONTACTS];
int blockedContactCount = 0;

// Funcții utilitare
void readLine(char *buffer, int size) {
    int ch, i = 0;
    while (1) {
        ch = getch();
        if (ch == '\n' || ch == '\r') {
            break;
        } else if (ch == KEY_BACKSPACE || ch == 127 || ch == 8) {
            if (i > 0) {
                i--;
                int y, x;
                getyx(stdscr, y, x);
                mvwaddch(stdscr, y, x - 1, ' ');
                move(y, x - 1);
                refresh();
            }
        } else if (isprint(ch) && i < size - 1) {
            buffer[i++] = ch;
            addch(ch);
            refresh();
        }
    }
    buffer[i] = '\0';
}

int isValidPhone(const char *phone) {
    for (int i = 0; phone[i]; i++) {
        if (!isdigit(phone[i]) && phone[i] != '+') return 0;
    }
    return 1;
}

// Fișier contacte
void loadContacts() {
    FILE *file = fopen(FILENAME, "r");
    if (file) {
        contactCount = 0;
        while (fgets(contacts[contactCount].name, 50, file) &&
               fgets(contacts[contactCount].phone, 20, file)) {
            strtok(contacts[contactCount].name, "\n");
            strtok(contacts[contactCount].phone, "\n");
            contactCount++;
        }
        fclose(file);
    }
}

void saveContacts() {
    FILE *file = fopen(FILENAME, "w");
    if (file) {
        for (int i = 0; i < contactCount; i++) {
            fprintf(file, "%s\n%s\n", contacts[i].name, contacts[i].phone);
        }
        fclose(file);
    }
}

// Contacte blocate
void loadBlockedContacts() {
    FILE *file = fopen(BLOCKED_FILENAME, "r");
    if (file) {
        blockedContactCount = 0;
        while (fgets(blockedContacts[blockedContactCount].name, 50, file) &&
               fgets(blockedContacts[blockedContactCount].phone, 20, file)) {
            strtok(blockedContacts[blockedContactCount].name, "\n");
            strtok(blockedContacts[blockedContactCount].phone, "\n");
            blockedContactCount++;
        }
        fclose(file);
    }
}

void saveBlockedContacts() {
    FILE *file = fopen(BLOCKED_FILENAME, "w");
    if (file) {
        for (int i = 0; i < blockedContactCount; i++) {
            fprintf(file, "%s\n%s\n", blockedContacts[i].name, blockedContacts[i].phone);
        }
        fclose(file);
    }
}

void addContact() {
    if (contactCount < MAX_CONTACTS) {
        clear();
        printw("Adauga contact\n---------------------------\n");
        printw(" Nume complet (ex: Ion Popescu): ");
        refresh();
        char name[50];
        readLine(name, 50);

        clear();
        printw("Adauga contact\n---------------------------\n");
        printw(" Telefon (doar cifre sau +): ");
        refresh();
        char phone[20];
        while (1) {
            readLine(phone, 20);
            if (isValidPhone(phone)) break;
            printw(" Telefon invalid!\n Reintrodu telefon: ");
            refresh();
        }

        strcpy(contacts[contactCount].name, name);
        strcpy(contacts[contactCount].phone, phone);
        contactCount++;
        saveContacts();

        printw("\nContact adaugat cu succes!\nApasa orice tasta pentru a reveni.\n");
        refresh();
        getch();
    } else {
        printw("Lista de contacte este plina!\nApasa orice tasta pentru a reveni.\n");
        refresh();
        getch();
    }
}

void deleteContact() {
    if (contactCount == 0) {
        printw("Nu exista contacte.\n");
        getch(); return;
    }
    int selected = 0, key;
    while (1) {
        clear();
        printw("Sterge contact\n---------------------------\n");
        for (int i = 0; i < contactCount; i++) {
            printw("%s%d. ", (selected == i ? "> " : "  "), i + 1);
            attron(COLOR_PAIR(1));
            printw("%s", contacts[i].name);
            attroff(COLOR_PAIR(1));
            printw(" - ");
            attron(COLOR_PAIR(2));
            printw("%s\n", contacts[i].phone);
            attroff(COLOR_PAIR(2));
        }
        printw("%sBack\n", selected == contactCount ? "> " : "  ");
        refresh();

        key = getch();
        if (key == KEY_UP) selected = (selected - 1 + contactCount + 1) % (contactCount + 1);
        else if (key == KEY_DOWN) selected = (selected + 1) % (contactCount + 1);
        else if (key == 10) {
            if (selected == contactCount) return;
            for (int j = selected; j < contactCount - 1; j++)
                contacts[j] = contacts[j + 1];
            contactCount--;
            saveContacts();
            if (selected >= contactCount) selected = contactCount - 1;
        }
    }
}


void blockedContactsMenu() {
    loadBlockedContacts();
    int selected = 0, key;
    while (1) {
        clear();
        printw("Contacte blocate\n---------------------------\n");
        if (blockedContactCount == 0) printw("Nu exista contacte blocate.\n");
        for (int i = 0; i < blockedContactCount; i++) {
            printw("%s%d. ", selected == i ? "> " : "  ");
            attron(COLOR_PAIR(1));
            printw("%s\n", blockedContacts[i].name);
            attroff(COLOR_PAIR(1));
            printw(" - ");
            attron(COLOR_PAIR(2));
            printw("%s\n", blockedContacts[i].phone);
            attroff(COLOR_PAIR(2));
        }
        printw("%sBack\n", selected == blockedContactCount ? "> " : "  ");
        refresh();

        key = getch();
        if (key == KEY_UP) selected = (selected - 1 + blockedContactCount + 1) % (blockedContactCount + 1);
        else if (key == KEY_DOWN) selected = (selected + 1) % (blockedContactCount + 1);
        else if (key == 10) {
            if (selected == blockedContactCount) return;
            printw("\nDeblochezi acest contact? Apasa 'd' pentru da.\n");
            int c = getch();
            if (c == 'd' || c == 'D') {
                for (int j = selected; j < blockedContactCount - 1; j++)
                    blockedContacts[j] = blockedContacts[j + 1];
                blockedContactCount--;
                saveBlockedContacts();
            }
        }
    }
}

void searchContact() {
    clear();
    printw("Cautare\n");
    printw("Introdu nume (partial): ");
    refresh();

    echo();
    char searchName[50];
    getnstr(searchName, 49);
    noecho();

    int found = 0;
    clear();
    printw("Rezultate cautare:\n");
    printw("---------------------------\n");
    for (int i = 0; i < contactCount; i++) {
        if (strstr(contacts[i].name, searchName)) {
            printw("%d. ", i + 1);
            attron(COLOR_PAIR(1));
            printw("%s", contacts[i].name);
            attroff(COLOR_PAIR(1));
            printw(" - ");
            attron(COLOR_PAIR(2));
            printw("%s\n", contacts[i].phone);
            attroff(COLOR_PAIR(2));
            found = 1;
        }
    }

    if (!found) {
        printw("Niciun contact gasit.\n");
    }

    printw("\nApasa orice tasta pentru a reveni.\n");
    refresh();
    getch();
}


void displayMainMenu(int selectedOption) {
    clear();
    printw("Gestionare contacte\n---------------------------\nMeniu principal\n---------------------------\n");
    printw("%sAdauga contact\n", selectedOption == 0 ? "> " : "  ");
    printw("%sStergere contact\n", selectedOption == 1 ? "> " : "  ");
    printw("%sContacte blocate\n", selectedOption == 2 ? "> " : "  ");
    printw("%sCautare\n", selectedOption == 3 ? "> " : "  ");
    refresh();
}

void handleMainMenu() {
    int selectedOption = 0, key;
    initscr();
    cbreak();
    noecho();
    keypad(stdscr, TRUE);
    curs_set(0);
    start_color();
    init_pair(1, COLOR_CYAN, COLOR_BLACK); // nume
    init_pair(2, COLOR_YELLOW, COLOR_BLACK); // telefon

    loadContacts();

    while (1) {
        displayMainMenu(selectedOption);
        key = getch();
        if (key == KEY_UP) selectedOption = (selectedOption - 1 + NUM_OPTIONS) % NUM_OPTIONS;
        else if (key == KEY_DOWN) selectedOption = (selectedOption + 1) % NUM_OPTIONS;
        else if (key == 10) {
            switch (selectedOption) {
                case 0: addContact(); break;
                case 1: deleteContact(); break;
                case 2: blockedContactsMenu(); break;
                case 3: searchContact(); break;
            }
        }
    }
    endwin();
}

int main() {
    handleMainMenu();
    return 0;
}
