#include <iostream>
#include <windows.h>

using namespace std;

int board[4][4] = {  //��l�ƿW 
    {0, 0, 1, 0},
    {0, 4, 0, 0},
    {0, 0, 2, 0},
    {0, 1, 0, 0},
};

bool editable[4][4];
bool row_invalid[4] = {false};  // �аO�C��O�_�����ƼƦr
bool col_invalid[4] = {false};  // �аO�C�C�O�_�����ƼƦr
bool block_invalid[2][2] = {false};  // �аO�C�� 2x2 �j��O�_�����ƼƦr

bool row_complete[4] = {false};  // �аO�C��O�_���駹��
bool col_complete[4] = {false};  // �аO�C�C�O�_���駹��
bool block_complete[2][2] = {false};  // �аO�C�� 2x2 �j��O�_���駹��

int cur_r = 0, cur_c = 0;

void check_horizontal(int i) {//�� 
    bool seen[5] = {false};
    row_invalid[i] = false;
    row_complete[i] = true;
    for (int j = 0; j < 4; ++j) {
        if (board[i][j] == 0) {
            row_complete[i] = false;
            continue;
        }
        if (seen[board[i][j]]) {
            row_invalid[i] = true;
        }
        seen[board[i][j]] = true;
    }
}

void check_vertical(int j) {//�C 
    bool seen[5] = {false};
    col_invalid[j] = false;
    col_complete[j] = true;
    for (int i = 0; i < 4; ++i) {
        if (board[i][j] == 0) {
            col_complete[j] = false;
            continue;
        }
        if (seen[board[i][j]]) {
            col_invalid[j] = true;
        }
        seen[board[i][j]] = true;
    }
}

void check_block(int block_row, int block_col) {//�j�� 
    bool seen[5] = {false};
    block_invalid[block_row][block_col] = false;
    block_complete[block_row][block_col] = true;
    for (int i = block_row * 2; i < (block_row + 1) * 2; ++i) {
        for (int j = block_col * 2; j < (block_col + 1) * 2; ++j) {
            if (board[i][j] == 0) {
                block_complete[block_row][block_col] = false;
                continue;
            }
            if (seen[board[i][j]]) {
                block_invalid[block_row][block_col] = true;
            }
            seen[board[i][j]] = true;
        }
    }
}

string get_styled_text(string text, string style) {
    string color = "", font = "";
    for (char c : style) {
        if (c == 'R') color = "31";  // Red
        if (c == 'G') color = "32";  // Green
        if (c == 'E') color = "41";  // Red background
        if (c == 'C') color = "106"; // Cyan background
        if (c == 'B') font = ";1";   // Bold
    }
    return "\x1b[" + color + font + "m" + text + "\x1b[0m";
}

void print_board() {
    // Flush the screen
    cout << "\x1b[2J\x1b[1;1H";

    // Print usage hint
    cout << get_styled_text("W/A/S/D: ", "B") << "move cursor" << endl;
    cout << get_styled_text("    1-4: ", "B") << "fill in number" << endl;
    cout << get_styled_text("      0: ", "B") << "clear the cell" << endl;
    cout << get_styled_text("      Q: ", "B") << "quit" << endl;
    cout << endl;

    // Print the board
    for (int i = 0; i < 4; ++i) {
        // Print horizontal divider
        if (i && i % 2 == 0)
            cout << "---------------" << endl;

        // Print vertical divider for each row
        cout << "|";
        for (int j = 0; j < 4; ++j) {
            // Set the style for the cell
            string style = "";


            // Cyan for the cursor position
            if (cur_r == i && cur_c == j) {
                style = "C";
            }

            // Red for invalid cells
            else if (row_invalid[i] || col_invalid[j] || block_invalid[i / 2][j / 2]) {
                style = "R";
            }
           
            // Green if the row, column, or block is complete
            else if (row_complete[i] || col_complete[j] || block_complete[i / 2][j / 2]) {
                style = "G";
            }

            // Bold for immutable cells
            if (!editable[i][j]) {
                style += "B";
            }

            // Print the cell content
            if (board[i][j] == 0)
                cout << get_styled_text(" �P ", style);
            else
                cout << get_styled_text(" " + to_string(board[i][j]) + " ", style);

            // Print vertical divider for each block
            if ((j + 1) % 2 == 0)
                cout << "|";
        }
        cout << endl;
    }
}

void initialize() {
    // Set up styled text for Windows
    HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
    DWORD dwMode = 0;
    GetConsoleMode(hOut, &dwMode);
    dwMode |= ENABLE_VIRTUAL_TERMINAL_PROCESSING;
    SetConsoleMode(hOut, dwMode);

    // Mark editable cells
    for (int i = 0; i < 4; ++i)
        for (int j = 0; j < 4; ++j)
            editable[i][j] = !board[i][j];

    // Check for conflicts and completeness in rows, columns, and blocks
    for (int i = 0; i < 4; ++i) {
        check_horizontal(i);
        check_vertical(i);
    }
    for (int i = 0; i < 2; ++i) {
        for (int j = 0; j < 2; ++j) {
            check_block(i, j);
        }
    }

    // Print the initial board
    print_board();
}

void fill_number(char c) {
    if (editable[cur_r][cur_c]) {
        board[cur_r][cur_c] = c - '0'; // Convert char to integer and fill it in
    }
    else {
        cout << "!!! Cannot edit this cell !!!" << endl; // Cell is immutable
    }
}

void move_cursor(char c) {
   if (c == 'W' || c == 'w')
    {
        // �V�W���ʡA����J��i�s�誺��l�Ϊ̥X��
        for (int i = cur_r - 1; i >= 0; --i)
       
        {
            if (editable[i][cur_c])
            {
                cur_r = i;
                return;
            }
        }
    }
    else if (c == 'S' || c == 's')
    {
        // �V�U���ʡA����J��i�s�誺��l�Ϊ̥X��
        for (int i = cur_r + 1; i < 4; ++i)
        {
            if (editable[i][cur_c])
            {
                cur_r = i;
                return;
            }
        }
    }
    else if (c == 'A' || c == 'a')
    {
        // �V�����ʡA����J��i�s�誺��l�Ϊ̥X��
        for (int i = cur_c - 1; i >= 0; --i)
        {
            if (editable[cur_r][i])
            {
                cur_c = i;
                return;
            }
        }
    }
    else if (c == 'D' || c == 'd')
    {
        // �V�k���ʡA����J��i�s�誺��l�Ϊ̥X��
        for (int i = cur_c + 1; i < 4; ++i)
        {
            if (editable[cur_r][i])
            {
                cur_c = i;
                return;
            }
        }
    }
}

bool check_win() {
    // �ˬd�C��B�C�C�H�ΨC�� 2x2 �j��O�_���w�����A�B�L�Ĭ�
    for (int i = 0; i < 4; ++i) {
        if (!row_complete[i] || row_invalid[i]) {
            return false;
        }
        if (!col_complete[i] || col_invalid[i]) {
            return false;
        }
    }
    for (int i = 0; i < 2; ++i) {
        for (int j = 0; j < 2; ++j) {
            if (!block_complete[i][j] || block_invalid[i][j]) {
                return false;
            }
        }
    }

    // �Y��������A�h�C������
    cout << get_styled_text("YOU WIN!", "G") << endl;
    return true;
}


int main() {
    char c;
    bool action_ok;

    initialize();
    while (cin >> c) {
        action_ok = false;
        if (c == 'Q' || c == 'q')
            break;

        if (c >= '0' && c <= '4') {
            fill_number(c);
            action_ok = true;
        } else {
            move_cursor(c);
            action_ok = true;
        }

        // Recheck conflicts and completeness after filling
        for (int i = 0; i < 4; ++i) {
            check_horizontal(i);
            check_vertical(i);
        }
        for (int i = 0; i < 2; ++i) {
            for (int j = 0; j < 2; ++j) {
                check_block(i, j);
            }
        }

        print_board();

        // �ˬd�O�_�����C��
        if (check_win()) {
            break; // �����{��
        }

        if (!action_ok)
            cout << get_styled_text("!!! Invalid action !!!", "R");
    }
    return 0;
}
