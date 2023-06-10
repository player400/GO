#define _CRT_SECURE_NO_WARNINGS

#include <stdio.h>
#include "conio2.h"
#include <iostream>

#define INIT_VECTOR_CAP 2 //POJEMNOŚĆ NOWO UTWORZONEGO WEKTORA (WEKTOR JEST ZWIĘKSZANY DWUKROTNIE ZA KAŻDYM RAZEM GDY POJEMNOŚC JEST PRZEKRACZANA)
#define NUMBER_SYSTEM_FOR_INPUT 10 //SYSTEM LICZBOWY, WEDŁUG KTÓREGO INTERPRETOWANE SĄ WPISANE PRZEZ UŻYTKOWNIKA LICZBY (10 - DZIESIĘTNY, 2-BINARNY)

#define EMPTY 0
#define WHITE_STONE 1
#define BLACK_STONE 2

#define MENU_POSITION_X 1
#define MENU_POSITION_Y 1
#define MENU_PROMPT_POSITION_Y 20

#define MAX_FULL_BOARD_DISPLAY_SIZE 25 //MAKSYMALNY ROZMIAR PLANSZY, PRZY KTÓRYM JEST WYŚWIETLANA CAŁA PLANSZA
#define HANDICAP_POINTS_PER_STONE 1; //LICZBA PUNKTÓW, KTÓRE BIAŁY OTRZYMUJE ZA KAŻDY DODATKOWY KAMIEŃ OTRZYMANY PRZEZ CZARNEGO NA POCZĄTKU GRY
#define BOARD_POSITION_X 80
#define BOARD_POSITION_Y 1
#define CURSOR_RELATIVE_STARTING_POSITION_X 0 //POZYCJA XY KURSORA NA PLANSZY W NOWEJ GRZE
#define CURSOR_RELATIVE_STARTING_POSITION_Y 0
#define DEF_SIZE_1 9 //PREDEFINIOWANY ROZMIAR MAŁEJ PLANSZY
#define DEF_SIZE_2 13 //PREDEFINIOWANY ROZMIAR ŚREDNIEJ PLANSZY
#define DEF_SIZE_3 19 //PREDEFINIOWANY ROZMIAR DUŻEJ PLANSZY
#define CURSOR 'O' 
#define STONE '#'
#define INTERSECTION '+'

#define FILE_EXTENSION ".go" //DOMYŚLNE ROZSZERZENIE PLIKÓW, KTÓRYCH GRA UŻYWA DO ZAPISYWANIA

//KLAWISZE SPECJALNE CONIO2:
#define UP 80
#define DOWN 72
#define LEFT 75
#define RIGHT 77
#define ESC 27
#define ENTER '\r'

template<typename T>
class Vector
{
private:
    T* buffer=nullptr;
    int capacity;

public:
    int size;

    //FUNKCJA ZWRACA WSKAŹNIK DO ZAWARTOŚCI WEKTORA, ABY MOŻNA BYŁO ODNIEŚĆ SIĘ DO NIEGO POZA KLASĄ
    T* get_ptr()
    {
        return buffer;
    }

    T get(int index)
    {
        return buffer[index];
    }

    //FUNKCJA SPRAWDZA CZY JEST JESZCZE MIEJSCE W WEKTORZE, JEŚLI NIE DO POWIĘKSZA GO DWUKROTNIE. NASTĘPNIE ZAPISUJE ELEMENT NA KOŃCU WEKTORA
    void push_back(T new_item)
    {
        if (size < capacity)
        {
            buffer[size] = new_item;
        }
        else
        {
            T* temp_buffer = (T*)malloc(sizeof(T) * capacity * 2);
            memcpy(temp_buffer, buffer, capacity*sizeof(T));
            temp_buffer[size] = new_item;
            capacity = capacity * 2;
            free(buffer);
            buffer = (T*)malloc(sizeof(T) * capacity);
            memcpy(buffer, temp_buffer, capacity * sizeof(T));
            free(temp_buffer);
        }
        size++;
    }

    //FUNKCJA EFEKTYWNIE USUWA OSTATNI ELEMENT WEKTORA (POD WARUNKIEM, ŻE WEKTOR MA ROZMIAR WIĘKSZY OD 0)
    void pop_back()
    {
        if (size > 0)
        {
            size--;
        }
    }

    Vector()
    {
        size = 0;
        capacity = INIT_VECTOR_CAP;
        buffer = (T*)malloc(sizeof(T) * INIT_VECTOR_CAP);
    }

    ~Vector()
    {
        free(buffer);
    }
};

struct Pos
{
    int x = 1;
    int y = 1;
};

//FUNKCJA WYPISUJE TEKST I PRZESUWA KURSOR O 1 W DÓŁ
void write_a_line(const char* text, Pos& current_cursor_pos)
{
    gotoxy(current_cursor_pos.x, current_cursor_pos.y);
    cputs(text);
    current_cursor_pos.y++;
}

char* number_to_string(int number)
{
    Vector<char>letters;
    if (number == 0)
    {
        letters.push_back('0');
    }
    while (number != 0) //PĘTLA USUWA CYFRY Z LICZBY ZACZYNAJĄC OD OSTATNIEJ 
    {
        int digit = number % 10;
        number = number-digit;
        number = number / 10;
        letters.push_back(digit + '0'); //USUNIĘTA CYFRA JEST ZAPISYWANA DO WEKTORA
    }
    char* array = letters.get_ptr();
    char* result = (char*)malloc(sizeof(char) * letters.size + 1);
    for (int i = 0; i < letters.size; i++) //LICZBA BYŁA ODCZYTYWANA OD KOŃCA, WIĘC KONIECZNE JEST ZAMIENIENIE CYFR MIEJSCAMI
    {
        result[i] = array[letters.size-1-i];
    }
    result[letters.size] = '\0';
    return result;
}

int string_to_number(const char* string)
{
    Vector<char>string_without_non_numbers;
    for (int i = 0; i<strlen(string); i++) //USUWANIE ZE STRINGA WSZYSTKICH ZNAKÓW NIE BĘDĄCYCH CYFRAMI
    {
        if (string[i] >= '0' && string[i] <= ('0'+NUMBER_SYSTEM_FOR_INPUT-1))
        {
            string_without_non_numbers.push_back(string[i]);
        }
    }
    int result = 0;
    int power = 1;
    for (int i = string_without_non_numbers.size - 1; i >= 0; i--) //ODCZYTYWANIE LICZBY
    {
        result = result + power * (string_without_non_numbers.get(i) - '0');
        power = power * NUMBER_SYSTEM_FOR_INPUT;
    }
    return result;
}

//FUNKCJA ZWRACA WARTOŚĆ BEZWZGLĘDNĄ 
int value(int number)
{
    return number >= 0 ? number : number * (-1);
}

char* add_strings(const char* string1, const char* string2)
{
    char* result = (char*)malloc(strlen(string1) + strlen(string2) + sizeof(char));
    int j = 0;
    for (int i = 0; i < strlen(string1); i++)
    {
        result[j] = string1[i];
        j++;
    }
    for (int i = 0; i < strlen(string2); i++)
    {
        result[j] = string2[i];
        j++;
    }
    result[j] = '\0';
    return result;
}

class Board
{
public:
    int** board=nullptr;
    int white_captures = 0;
    int black_captures = 0;

private:
    int board_size;
    
    //FUNKCJA SPRAWDZA CZY KAMIEŃ MA OPCJE, A JESLI NALEŻY DO GRUPY TO REKURENCYJNIE SPRAWDZA CZY CAŁA GRUPA MA OPCJE
    unsigned int how_many_options(int y, int x, int kind, Vector<Pos>*list_of_checked)
    {
        for (int k = 0; k < list_of_checked->size; k++)
        {
            if (list_of_checked->get(k).y == y && list_of_checked->get(k).x == x)
            {
                return 0;
            }
        }
        if (board[y][x] == EMPTY)
        {
            return 1;
        }
        if (board[y][x] == kind)
        {
            Pos temp;
            temp.y = y;
            temp.x = x;
            list_of_checked->push_back(temp);
            int options_count = 0;
            if (y > 0)
            {
                options_count = options_count + how_many_options(y - 1, x, kind, list_of_checked);
            }
            if (y + 1 < board_size)
            {
                options_count = options_count + how_many_options(y + 1, x, kind, list_of_checked);
            }
            if (x > 0)
            {
                options_count = options_count + how_many_options(y, x-1, kind, list_of_checked);
            }
            if (x + 1 < board_size)
            {
                options_count = options_count + how_many_options(y, x+1, kind, list_of_checked);
            }
            return options_count;
        }
        else
        {
            return 0;
        }
    }
    //FUNKCJA SPRAWDZA REKURENCYJNE CZY DANA KRATKA JEST OTOCZONA PRZEZ 1 RODZAJ KAMIENI
    bool is_surrounded(int y, int x, int kind, Vector<Pos>* group)
    {
        for (int i = 0; i < group->size; i++)
        {
            if (group->get(i).y == y && group->get(i).x == x)
            {
                return true;
            }
        }
        if (board[y][x] != EMPTY)
        {
            if (board[y][x] == kind)
            {
                return true;
            }
            else
            {
                return false;
            }
        }
        Pos this_crossing;
        this_crossing.x = x;
        this_crossing.y = y;
        group->push_back(this_crossing);
        if (y > 0)
        {
            if (is_surrounded(y - 1, x, kind, group) == false)
            {
                return false;
            }
        }
        if (x > 0)
        {
            if (is_surrounded(y, x - 1, kind, group) == false)
            {
                return false;
            }
        }
        if (y + 1 < board_size)
        {
            if (is_surrounded(y + 1, x, kind, group) == false)
            {
                return false;
            }
        }
        if (x + 1 < board_size)
        {
            if (is_surrounded(y, x+1, kind, group) == false)
            {
                return false;
            }
        }
        return true;
    }

public:
    //METODA USUWA WSZYSTKIE KAMIENIE RODZAJU KIND, DLA KTÓRYCH LICZBA OPCJI JEST MNIEJSZA NIŻ WARTOŚĆ barrier
    void check_for_options(int kind, unsigned int barrier)
    {
        int removed_stones_count = 0;
        for (int i = 0; i < board_size; i++)
        {
            for (int j = 0; j < board_size; j++)
            {
                if (board[i][j] == kind)
                {
                    Vector<Pos>* to_be_removed = new Vector<Pos>;
                    if (how_many_options(i, j, kind, to_be_removed) <= barrier)
                    {
                        removed_stones_count = removed_stones_count + to_be_removed->size;
                        for (int k = 0; k < to_be_removed->size; k++)
                        {
                            board[to_be_removed->get(k).y][to_be_removed->get(k).x] = EMPTY;
                        }
                    }
                    delete to_be_removed;
                }
            }
        }
        kind == WHITE_STONE ? black_captures += removed_stones_count : white_captures += removed_stones_count;
    }

    int check_for_territory(int kind)
    {
        int result = 0;
        Vector<Pos>checked;
        for (int i = 0; i < board_size; i++)
        {
            for (int j = 0; j < board_size; j++)
            {
                if (board[i][j] != EMPTY)
                {
                    continue;
                }
                bool is_checked = false; //SPRAWDZANIE, CZY DANY KAWAŁEK TERYTORIUM ZOSTAŁ JUŻ SPRAWDZONY
                for (int k = 0; k < checked.size; k++)
                {
                    if (checked.get(k).y == i && checked.get(k).x == j)
                    {
                        is_checked = true;
                        break;
                    }
                }
                if (!is_checked) //USTALENIE CZY TERYTORIUM JEST OTOCZONE (TERYTORIUM JEST OTOCZONE JEŚLI MA STYCZNOŚĆ TYLKO Z 1 RODZAJEM KAMIENIA)
                {
                    Vector<Pos>group;
                    if (is_surrounded(i, j, kind, &group))
                    {
                        result += group.size;
                    }
                    for (int k = 0; k < group.size; k++)
                    {
                        checked.push_back(group.get(k)); //DODAWANIE CĄŁEJ GRUPY DO LISTY SPRAWDZONEGO TERYTORIUM
                    }
                }
            }
        }
        return result;
    }

    void place_stone(int y, int x, int kind)
    {
        board[y][x] = kind;
        if (kind == BLACK_STONE) //PIERWSZY ,,ZABIJA" WROGIE KAMIENIE GRACZ, KTÓRY WYKONAŁ RUCH
        {
            check_for_options(WHITE_STONE, 0);
            check_for_options(BLACK_STONE, 0);
        }
        else
        {
            check_for_options(BLACK_STONE, 0);
            check_for_options(WHITE_STONE, 0);
        }
    }

    int count_stones(int kind)
    {
        int result = 0;
        for (int i = 0; i < board_size; i++)
        {
            for (int j = 0; j < board_size; j++)
            {
                if (board[i][j] == kind)
                {
                    result++;
                }
            }
        }
        return result;
    }

    Board(int board_size_)
    {
        board_size = board_size_;
        board = (int**)malloc(sizeof(int*) * board_size);
        for (int i = 0; i < board_size; i++)
        {
            board[i] = (int*)malloc(sizeof(int) * board_size);
            for (int j = 0; j < board_size; j++)
            {
                board[i][j] = EMPTY;
            }
        }
    }

    ~Board()
    {
        for (int i = 0; i < board_size; i++)
        {
            free(board[i]);
        }
        free(board);
    }
};

class Game
{
private:
    int board_size;
    Board* past_board = nullptr; //KOPIA PLANSZY SPRZED OSTATNIEGO RUCHU
    Board* current_board = nullptr; //OBECNA PLANSZA
    int stone_to_be_put_next = BLACK_STONE;
    int white_handicap_points = 0;

    void write_horizontal_frame()
    {
        putch('#');
        for (int i = 0; i < (board_size <= MAX_FULL_BOARD_DISPLAY_SIZE ? board_size : MAX_FULL_BOARD_DISPLAY_SIZE); i++)
        {
            putch('=');
        }
        putch('#');
    }


    void copy_the_board(Board* source, Board** target)
    {
        *target = new Board(board_size);
        (*target)->black_captures = source->black_captures;
        (*target)->white_captures = source->white_captures;
        for (int i = 0; i < board_size; i++)
        {
            for (int j = 0; j < board_size; j++)
            {
                (*target)->board[j][i] = source->board[j][i];
            }
        }
    }

    bool compare_board(Board** first, Board** second) //FUNKCJA SPRAWDZA CZY UŁOŻENIE KAMIENI NA DWÓCH PLANSZACH JEST IDENTYCZNE
    {
        if (*first == nullptr || *second == nullptr)
        {
            return false;        //JEŚLI KTÓRAŚ Z PLANSZ NIE ISTNIEJE TO FUNKCJA ZWRACA false
        }
        for (int i = 0; i < board_size; i++)
        {
            for (int j = 0; j < board_size; j++)
            {
                if ((*first)->board[j][i] != (*second)->board[j][i])
                {
                    return false;
                }
            }
        }
        return true;
    }

public:
    Pos on_board_cursor_pos;

    struct Score {
        int black_score;
        int white_score;
    };

    void view_board(Pos& current_cursor_pos)
    {
        current_cursor_pos.x = BOARD_POSITION_X;
        current_cursor_pos.y = BOARD_POSITION_Y;
        gotoxy(current_cursor_pos.x, current_cursor_pos.y); //USTAWIENIE KURSORA W ODPOWIEDNIEJ POZYCJI
        write_horizontal_frame(); //WYŚWIETLENIE GÓRNEJ RAMKI PLANSZY
        current_cursor_pos.y++;
        current_cursor_pos.x = BOARD_POSITION_X;
        for (int i = 0; i < board_size; i++)
        {   
            if (board_size <= MAX_FULL_BOARD_DISPLAY_SIZE || value(on_board_cursor_pos.y - i) <= MAX_FULL_BOARD_DISPLAY_SIZE/2) //DLA PLANSZY O ROZMIARZE WIĘKSZYM NIŻ 25 WYŚWIETLANE SĄ TYLKO POLA W PROMIENIU 12 KRATEK OD KURSORA
            {
                gotoxy(current_cursor_pos.x, current_cursor_pos.y); 
                putch('|'); //WYŚWIETLENIE LEWEJ RAMKI PLANSZY
                textbackground(YELLOW);
                for (int j = 0; j < board_size; j++)
                {
                    if (board_size <= MAX_FULL_BOARD_DISPLAY_SIZE || value(on_board_cursor_pos.x - j) <= MAX_FULL_BOARD_DISPLAY_SIZE/2) //DLA PLANSZY O ROZMIARZE WIĘKSZYM NIŻ 25 WYŚWIETLANE SĄ TYLKO POLA W PROMIENIU 12 KRATEK OD KURSORA
                    {
                        if (i == on_board_cursor_pos.y && j == on_board_cursor_pos.x)
                        {
                            textcolor(RED);
                            putch(CURSOR);
                        }
                        else if (current_board->board[i][j] == EMPTY)
                        {
                            textcolor(BLACK);
                            putch(INTERSECTION);
                        }
                        else if (current_board->board[i][j] == WHITE_STONE)
                        {
                            textcolor(WHITE);
                            putch(STONE);
                        }
                        else if (current_board->board[i][j] == BLACK_STONE)
                        {
                            textcolor(BLACK);
                            putch(STONE);
                        }
                        textcolor(WHITE);
                    }
                }
                textbackground(BLACK);
                putch('|'); //WYŚWIETLANIE PRAWEJ RAMKI PLANSZY
                current_cursor_pos.y++;
                current_cursor_pos.x = BOARD_POSITION_X;
            }
        }
        gotoxy(current_cursor_pos.x, current_cursor_pos.y);
        write_horizontal_frame();
    }

    Score calculate_score()
    {
        Board* temporary_board = nullptr;
        copy_the_board(current_board, &temporary_board); //TWORZENIE TYMCZASOWEJ PLANSZY, NA KTÓREJ BĘDĄ WYKONYANE WSZYSTKIE OPERACJE
        Vector<Pos>added_stones;
        for (int i = 0; i < board_size; i++) //PĘTLA ZAPISUJE DO WEKTORA WSZYSTKIE PUSTE PRZECIĘCIA ZNAJDUJĄCE SIĘ OBOK KAMIENI
        {
            for (int j = 0; j < board_size; j++)
            {
                if (temporary_board->board[i][j] != EMPTY)
                {
                    if (i > 0)
                    {
                        if (temporary_board->board[i - 1][j] == EMPTY)
                        {
                            Pos stone;
                            stone.y = i - 1;
                            stone.x = j;
                            added_stones.push_back(stone);
                        }
                    }
                    if (i + 1 < board_size)
                    {
                        if (temporary_board->board[i + 1][j] == EMPTY)
                        {
                            Pos stone;
                            stone.y = i + 1;
                            stone.x = j;
                            added_stones.push_back(stone);
                        }
                    }
                    if (j > 0)
                    {
                        if (temporary_board->board[i][j - 1] == EMPTY)
                        {
                            Pos stone;
                            stone.y = i;
                            stone.x = j - 1;
                            added_stones.push_back(stone);
                        }
                    }
                    if (j + 1 < board_size)
                    {
                        if (temporary_board->board[i][j + 1] == EMPTY)
                        {
                            Pos stone;
                            stone.y = i;
                            stone.x = j + 1;
                            added_stones.push_back(stone);
                        }
                    }
                }
            }
        }
        int current_stone = stone_to_be_put_next;
        for (int i = 0; i < added_stones.size; i++) //STAWIANIE NA PRZEMIAN RÓŻNYCH RODZAJÓW KAMIENI NA PRZECIĘCIACH ZNALEZIONYCH PRZEZ POPRZEDNIĄ PĘTLĘ
        {
            temporary_board->board[added_stones.get(i).y][added_stones.get(i).x] = current_stone;
            current_stone = current_stone == WHITE_STONE ? BLACK_STONE : WHITE_STONE;
        }
        temporary_board->check_for_options(stone_to_be_put_next == WHITE_STONE ? BLACK_STONE : WHITE_STONE, 0); //USUWANIE KAMIENI BEZ ODDECHÓW (GRACZ, KTÓREGO JEST TURA MA KAMIENIE ,,ZABIJANE" PÓŹNIEJ NIŻ DRUGI GRACZ)
        temporary_board->check_for_options(stone_to_be_put_next, 0);
        for (int i = 0; i < added_stones.size; i++) //USUWANIE KAMIENI DODANYCH NA POTRZEBY TESTU
        {
            temporary_board->board[added_stones.get(i).y][added_stones.get(i).x] = EMPTY;
        }
        temporary_board->black_captures = current_board->black_captures + (current_board->count_stones(WHITE_STONE) - temporary_board->count_stones(WHITE_STONE)); //LICZBA ZABITYCH KAMIENI JEST OBLICZANA NA PODSTAWIE TEGO ILE KAMIENI ZOSTAŁO NA PLANSZY PO WYKONANIU WSZYSTKICH OPERACJI
        temporary_board->white_captures = current_board->white_captures + (current_board->count_stones(BLACK_STONE) - temporary_board->count_stones(BLACK_STONE));
        Score result;
        result.black_score = temporary_board->check_for_territory(BLACK_STONE) + temporary_board->black_captures;
        result.white_score = temporary_board->check_for_territory(WHITE_STONE) + temporary_board->white_captures + white_handicap_points;
        delete temporary_board;
        return result;
    }

    void place_black_stone()
    {
        if (past_board == nullptr)
        {
            current_board->place_stone(on_board_cursor_pos.y, on_board_cursor_pos.x, BLACK_STONE);
            white_handicap_points += HANDICAP_POINTS_PER_STONE;
        }
    }

    void place_stone()
    {
        if (current_board->board[on_board_cursor_pos.y][on_board_cursor_pos.x] == EMPTY)
        {
            Board* future_board = nullptr;
            copy_the_board(current_board, &future_board);
            future_board->place_stone(on_board_cursor_pos.y, on_board_cursor_pos.x, stone_to_be_put_next);  //KOPIOWANIE PLANSZY I STAWIANIE KAMIENIA NA KOPII
            if (!compare_board(&future_board, &past_board)) //SPRAWDZANIE CZY KOPIA JEST RÓŻNA OD PLANSZY SPRZED OSTATNIEGO RUCHU (JEŚLI NIE, TO ZŁAMANA JEST ZASADA KO I RUCHU NIE MOŻNA WYKONAĆ)
            {
                bool is_suicide = false;
                if (stone_to_be_put_next == BLACK_STONE && (future_board->white_captures > current_board->white_captures && future_board->black_captures == current_board->black_captures))  //SPRAWDZANIE CZY RUCH NIE JEST OCZYWISTYM SAMOBÓJSTWEM (JEŚLI PO DANYM RUCHU PRZECIWNIK MA WIĘCEJ JEŃCÓW, A WYKONUJĄCY RUCH NIE, TO ZNACZY ŻE RUCH JEST SAMOBÓJSTWEM)
                {
                    is_suicide = true;
                }
                else if (stone_to_be_put_next == WHITE_STONE && (future_board->black_captures > current_board->black_captures && future_board->white_captures == current_board->white_captures))
                {
                    is_suicide = true;
                }
                if (!is_suicide)
                {
                    delete past_board;
                    copy_the_board(current_board, &past_board); //ZAPISYWANIE PLANSZY JAKO past_board ABY MÓC SPRAWDZAĆ KOLEJNE RUCHY POD KĄTEM ZASADY KO
                    delete current_board;
                    copy_the_board(future_board, &current_board); //RUCH ZOSTAŁ WYKONANY NA TYMCZASOWEJ PLANSZY, WIĘC PO STWIEERDZENIU, ŻE JEST ON DOSWOLONY TYMCZASOWA PLANSZA JEST W CAŁOŚCI KOPIOWANA DO PLANSZY GŁÓWNEJ
                    if (stone_to_be_put_next == WHITE_STONE) //ZMIANA TUR (JESLI RUCH WYKONYWAŁ BIAŁY, TO KOLEJNY JEST CZARNY ITP)
                    {
                        stone_to_be_put_next = BLACK_STONE;
                    }
                    else
                    {
                        stone_to_be_put_next = WHITE_STONE;
                    }
                }
            }
            delete future_board;
        }
    }

    void change_on_board_cursor_pos(int add_to_y, int add_to_x)
    {
        if (on_board_cursor_pos.x + add_to_x < board_size && on_board_cursor_pos.x + add_to_x >= 0)
        {
            on_board_cursor_pos.x = on_board_cursor_pos.x + add_to_x;
        }
        if (on_board_cursor_pos.y + add_to_y < board_size && on_board_cursor_pos.y + add_to_y >= 0)
        {
            on_board_cursor_pos.y = on_board_cursor_pos.y + add_to_y;
        }
    }

    int get_captures(int kind)
    {
        if (kind == WHITE_STONE)
        {
            return current_board->white_captures;
        }
        if (kind == BLACK_STONE)
        {
            return current_board->black_captures;
        }
        return 0;
    }
/*
FORMAT PLIKU DO ZAPISU STANU GRY:

NAGŁÓWEK PLIKU (INFORMACJE NA TEMAT GRY):
n - (INT) ROZMIAR PLANSZY
l - (INT) GRACZ, KTÓRY WYKONUJE NASTĘPNY RUCH (black_stone LUB white_stone)
m - (INT, INT) POZYCJA KURSORA NA PLANSZY
h - (INT) PUNKTY HANDICAPU (DOD. PUNKTY DLA BIAŁEGO
p - (BOOL) INFORMACJA CZY ZOSTAŁA ZAPISANA KOPIA MAPY SPRZED OSTATNIEGO RUCHU

INFORMACJE NA TEMAT PLANSZY:
b (int) - LICZBA KAMIENI ZABITYCH PRZEZ CZARNEGO GRACZA
w (int) - LICZBA KAMIENI ZABITYCH PRZEZ BIĄŁEGO GRACZA
1 (1(int), 2(int), ..., n(int))
2 (1(int), 2(int), ..., n(int))
3 (1(int), 2(int), ..., n(int))
.
.
.
n (1(int), 2(int), ..., n(int)) - DANE NA TEMAT WSZYSTKICH PÓL PLANSZY PODZIELONE NA WEKTORY (WIERSZE)

JEŚLI p==true W TYM MIEJSCU ZNAJDUJĄ SIĘ INFORMACJE NA TEMAT PLANSZY SPRZED OSTATNIEGO RUCHU, W PRZECIWNYM WYPADKU JEST TO KONIEC PLIKU
*/

    void save_to_file(const char* name)
    {
        bool past_board_exits = past_board == nullptr ? false : true; //SPRAWDZANIE, CZY ISTNIEJE KOPIA PLANSZY SPRZED 1 RUCHU, INFORMACJA TA ZOSTANIE ZAPISANA I WYKORZYSTANA PRZY ODCZYTYWANIU PLIKU
        FILE* file = fopen(name, "w");
        if (file == nullptr)
        {
            return;
        }
        fwrite(&board_size, sizeof(int), 1, file);
        fwrite(&stone_to_be_put_next, sizeof(int), 1, file);
        fwrite(&on_board_cursor_pos, sizeof(Pos), 1, file);
        fwrite(&white_handicap_points, sizeof(int), 1, file);
        fwrite(&past_board_exits, sizeof(bool), 1, file);
        fwrite(&current_board->black_captures, sizeof(int), 1, file);
        fwrite(&current_board->white_captures, sizeof(int), 1, file);
        for (int i = 0; i < board_size; i++) //PĘTLA, KTÓRA PRZEKAZUJE KOLEJNO WSKAŹNIKI DO WSZYSTKICH WEKTORÓW SKŁADAJĄCYCH SIĘ NA DWUWYMIAROWĄ TABLICĘ board
        {
            fwrite(current_board->board[i], sizeof(int), board_size, file);
        }
        if (past_board_exits) //JEŻELI KOPIA PLANSZY SPRZED 1 RUCHU ISTNIEJE, TO ZOSTANIE ONA ZAPISANA DO PLIKU
        {
            fwrite(&past_board->black_captures, sizeof(int), 1, file);
            fwrite(&past_board->white_captures, sizeof(int), 1, file);
            for (int i = 0; i < board_size; i++)
            {
                fwrite(past_board->board[i], sizeof(int), board_size, file);
            }
        }
        fclose(file);
    }

    //FUNKCJA TWORZY OBIEKT KLASY game, USTAWIA JEGO ZAWARTOŚĆ NA DANE Z PLIKU I ZWRACA WSKAŹNIK DO NIEGO
    void load_from_file(const char* name)
    {
        FILE* file = fopen(name, "r");
        if (file == nullptr)
        {
            return;
        }
        bool past_board_exists;
        fread(&board_size, sizeof(int), 1, file);
        delete current_board;
        current_board = new Board(board_size);
        fread(&stone_to_be_put_next, sizeof(int), 1, file);
        fread(&on_board_cursor_pos, sizeof(Pos), 1, file);
        fread(&white_handicap_points, sizeof(int), 1, file);
        fread(&past_board_exists, sizeof(bool), 1, file); //ODCZYTANIE ZAWARTEJ W PLIKU INFORMACJI, CZY ZOSTAŁA W NIM ZAPISANA KOPIA PLANSZY SPRZED 1 RUCHU
        fread(&current_board->black_captures, sizeof(int), 1, file);
        fread(&current_board->white_captures, sizeof(int), 1, file);
        for (int i = 0; i < board_size; i++)
        {
            fread(current_board->board[i], sizeof(int), board_size, file);
        }
        if (past_board_exists) //JEŚLI KOPIA PLANSZY SPRZED 1 RUCHU ISTNIEJE, TO ZOSTAJE ODCZYTANA
        {
            delete past_board;
            copy_the_board(current_board, &past_board); //W NOWO UTWORZONYM OBIEKCIE KLASY Game KOPIA PLANSZY SPRZED 1 RUCHU DOMYŚLNIE NIE ISTNIEJE, DLATEGO NALEŻY JĄ UTWORZYĆ ZA POMOCĄ METODY copy_the_board 
            fread(&past_board->black_captures, sizeof(int), 1, file);
            fread(&past_board->white_captures, sizeof(int), 1, file);
            for (int i = 0; i < board_size; i++)
            {
                fread(past_board->board[i], sizeof(int), board_size, file);
            }
        }
        fclose(file);
    }

    Game(int board_size_)
    {
        board_size = board_size_;
        current_board = new Board(board_size);
        on_board_cursor_pos.x = CURSOR_RELATIVE_STARTING_POSITION_X;
        on_board_cursor_pos.y = CURSOR_RELATIVE_STARTING_POSITION_Y;
    }

    ~Game()
    {
        delete current_board;
        delete past_board;
    }
};

void display_prompt(const char* text, Pos& current_cursor_pos)
{
    current_cursor_pos.x = MENU_POSITION_X;
    current_cursor_pos.y = MENU_PROMPT_POSITION_Y;
    textcolor(RED);
    textbackground(WHITE);
    write_a_line(text, current_cursor_pos);
    textbackground(BLACK);
    textcolor(WHITE);
}

Vector<char>* take_input(Pos& current_cursor_pos)
{
    Vector<char>* result = new Vector<char>;
    Pos input_cursor_pos = current_cursor_pos;
    textbackground(BLUE);
    while (true)
    {
        int character = getch();
        if (character == ESC) //JEŚLI WCIŚNIĘTO KLAWISZ ESC FUNKCJA ZWRACA nullptr
        {
            delete result;
            textbackground(BLACK);
            return nullptr;
        }
        else if (character == ENTER)
        {
            break;
        }
        else if (character == 8)
        {
            result->pop_back();
        }
        result->push_back(character);
        result->push_back('\0'); //PRZED WYŚWIETLENIEM KONIECZNE JEST DODANIE DO WEKTORA ZNAKU KOŃCA TEKSTU, KTÓRY POTEM ZOSTANIE USUNIĘTY
        current_cursor_pos = input_cursor_pos; //FUNKCJA write_a_line PRZESUWA KURSOR O 1 POZYCJĘ W DÓŁ, WIĘC PRZED JEJ UŻYCIEM POZYCJA KURSORA JEST RESETOWANA, ABY WYŚWIETLANY TEKST POZOSTAŁ W MIEJSCU
        write_a_line(result->get_ptr(), current_cursor_pos);
        result->pop_back(); //USUNIĘCIE ZNAKU KOŃCA TEKSTU PO WYŚWIETLENIU
    }
    textbackground(BLACK);
    result->push_back('\0'); //NA KOŃCU DO WEKTORA DODAWANY JEST ZNAK \0, ABY ZASYGNALIZOWAĆ KONIEC TEKSTU
    return result;
}

bool take_controls(Game** game, Pos& current_cursor_pos)
{
    int menu_char = getch();
    bool quit = false;
    switch (menu_char)
    {
    case 'q': {
        return true;
    }; break;
    case 'n': {
        int new_board_size=0;
        display_prompt("Choose the size of the board:\n1. small \n2. medium\n3. large \n4. custom", current_cursor_pos);
        current_cursor_pos.y = current_cursor_pos.y + 5;
        while (true)
        {
            int menu_char2 = getch();
            switch (menu_char2) //WYBÓR JEDNEJ Z PREDEFINIOWANYCN OPCJI
            {
            case '1': {
                new_board_size = DEF_SIZE_1;
            }; break;
            case '2': {
                new_board_size = DEF_SIZE_2;
            }; break;
            case '3': {
                new_board_size = DEF_SIZE_3;
            }; break;
            case '4': {
                write_a_line("Write down the new board size", current_cursor_pos); //WYBÓR WŁASNEGO ROZMIARU PLANSZY
                Vector<char>* text_input = take_input(current_cursor_pos);
                if(text_input==nullptr)
                {
                    return false;
                }
                new_board_size = string_to_number(text_input->get_ptr());
                delete text_input;
            }; break;
            case ESC: {
                clrscr();
                return false;
            }
            }
            if (new_board_size)
            {
                break;
            }
        }
        delete (* game);
        *game = new Game(new_board_size);
        clrscr();
    }; break;
    case 'i': {
        (*game)->place_stone();
    }; break;
    case 'j': {
        (*game)->place_black_stone();
    } break;
    case 's': {
        display_prompt("Give the name of the save.", current_cursor_pos);
        Vector<char>* save_name = take_input(current_cursor_pos);
        if (save_name == nullptr)
        {
            return false;
        }
        char* file_name = add_strings(save_name->get_ptr(), FILE_EXTENSION);
        (*game)->save_to_file(file_name);
        delete save_name;
        free(file_name);
        clrscr();
    }; break;
    case 'l': {
        display_prompt("Give the name of the save.", current_cursor_pos);
        Vector<char>* save_name = take_input(current_cursor_pos);
        if (save_name == nullptr)
        {
            return false;
        }
        char* file_name = add_strings(save_name->get_ptr(), FILE_EXTENSION);
        (*game)->load_from_file(file_name);
        delete save_name;
        free(file_name);
        clrscr();
    }; break;
    case 'f': {
        display_prompt("Score of the game:", current_cursor_pos);
        Game::Score score = (*game)->calculate_score();
        char* number = number_to_string(score.white_score);
        char* text = add_strings("White: ", number);
        write_a_line(text, current_cursor_pos);
        free(number);
        free(text);
        number = number_to_string(score.black_score);
        text = add_strings("Black: ", number);
        write_a_line(text, current_cursor_pos);
        free(text);
        free(number);
        while (true)
        {
            char choice_character = getch();
            if (choice_character == ENTER)
            {
                delete (*game);
                (*game) = new Game(DEF_SIZE_2);
                break;
            }
            if (choice_character == ESC)
            {
                return false;
            }
        }
    }; break;
    case 0: {
        int zn2 = getch();
        switch (zn2)
        {
        case UP: {
            (*game)->change_on_board_cursor_pos(1, 0);
        }; break;
        case DOWN: {
            (*game)->change_on_board_cursor_pos(-1, 0);
        }; break;
        case LEFT: {
            (*game)->change_on_board_cursor_pos(0, -1);
        }; break;
        case RIGHT: {
            (*game)->change_on_board_cursor_pos(0, 1);
        }; break;
        }
    }
    }
    return false;
}

void display_menu(Game** game, Pos& current_cursor_pos)
{
    current_cursor_pos.x = MENU_POSITION_X;
    current_cursor_pos.y = MENU_POSITION_Y;
    textbackground(RED);
    write_a_line("Go game. a, b, c, d, e, f, g, h, i, j, k ,l", current_cursor_pos);
    textbackground(BLACK);
    write_a_line("__________________________________________________", current_cursor_pos);
    write_a_line("arrows - moving across the board", current_cursor_pos);
    write_a_line("q - close the program", current_cursor_pos);
    write_a_line("n - new game", current_cursor_pos);
    write_a_line("enter - confirm", current_cursor_pos);
    write_a_line("esc - cancel", current_cursor_pos);
    write_a_line("i - place a stone on the board", current_cursor_pos);
    write_a_line("j - place black stone on board (only at the beginning of the game)", current_cursor_pos);
    write_a_line("s - save the game", current_cursor_pos);
    write_a_line("l - load a saved game", current_cursor_pos);
    write_a_line("f - finish the game and display results", current_cursor_pos);
    write_a_line("__________________________________________________", current_cursor_pos);
    char* number;
    char* text;
    number = number_to_string((*game)->get_captures(WHITE_STONE));
    text = add_strings("Number of stones captured by white: ", number);
    write_a_line(text, current_cursor_pos);
    free(number);
    free(text);
    number = number_to_string((*game)->get_captures(BLACK_STONE));
    text = add_strings("Number of stones captured by black: ", number);
    write_a_line(text, current_cursor_pos);
    free(text);
    free(number);
    write_a_line("Position of the cursor:", current_cursor_pos);
    number = number_to_string((*game)->on_board_cursor_pos.x);
    write_a_line(number, current_cursor_pos);
    free(number);
    number = number_to_string((*game)->on_board_cursor_pos.y);
    write_a_line(number, current_cursor_pos);
    free(number);
}

int main()
{
    //JEDNORAZOWE ,,URUCHOMIENIE" BIBLIOTEKI CONIO
    Conio2_Init();
    settitle("Mateusz Nurczynski, 193053");
    _setcursortype(_NOCURSOR);
    Game* game = new Game(DEF_SIZE_2);
    Pos current_cursor_pos;
    while (true)
    {
        display_menu(&game, current_cursor_pos);
        game->view_board(current_cursor_pos);
        if (take_controls(&game, current_cursor_pos)) //FUNKCJA take_controls WCZYTUJE ZNAK Z KLAWIATURY I AKTYWUJE ODPOWIEDNIĄ OPCJĘ, JEŚLI JEST TO q  FUNKCJA ZWRÓCI true
        {
            break;
        }
        clrscr();
    }
    delete game;
    _setcursortype(_NORMALCURSOR);
    return 0;
}


