#include <iostream>
#include <windows.h>

using namespace std;

int board[4][4] = {
    {0, 0, 1, 0},
    {0, 4, 0, 0},
    {0, 0, 2, 0},
    {0, 1, 0, 0},
};

bool editable[4][4];
bool row_invalid[4] = {false};  // 標記每行是否有重複數字
bool col_invalid[4] = {false};  // 標記每列是否有重複數字
bool block_invalid[2][2] = {false};  // 標記每個 2x2 大格是否有重複數字

int cur_r = 0, cur_c = 0;

void check_horizontal(int i) {
    bool seen[5] = {false};  // 範圍是 1-4
    for (int j = 0; j < 4; ++j) {
        if (board[i][j] != 0) {
            if (seen[board[i][j]]) {
                row_invalid[i] = true;
                return;
            }
            seen[board[i][j]] = true;
        }
    }
    // 如果該行無重複，則恢復該行為白色
    row_invalid[i] = false;
}

void check_vertical(int j) {
    bool seen[5] = {false};  // 範圍是 1-4
    for (int i = 0; i < 4; ++i) {
        if (board[i][j] != 0) {
            if (seen[board[i][j]]) {
                col_invalid[j] = true;
                return;
            }
            seen[board[i][j]] = true;
        }
    }
    // 如果該列無重複，則恢復該列為白色
    col_invalid[j] = false;
}

void check_block(int block_row, int block_col) {
    bool seen[5] = {false};  // 範圍是 1-4
    for (int i = block_row * 2; i < (block_row + 1) * 2; ++i) {
        for (int j = block_col * 2; j < (block_col + 1) * 2; ++j) {
            if (board[i][j] != 0) {
                if (seen[board[i][j]]) {
                    block_invalid[block_row][block_col] = true;
                    return;
                }
                seen[board[i][j]] = true;
            }
        }
    }
    // 如果該區塊無重複，則恢復該區塊為白色
    block_invalid[block_row][block_col] = false;
}

bool is_invalid(int i, int j) {
    return row_invalid[i] || col_invalid[j] || block_invalid[i / 2][j / 2];
}

bool is_done(int i, int j) {
    return board[i][j] != 0;
}

bool check_win() {
    for (int i = 0; i < 4; ++i)
        for (int j = 0; j < 4; ++j)
            if (board[i][j] == 0)
                return false;
    return true;
}

bool is_moving_action(char c) {
    return (c == 'W' || c == 'w' || c == 'S' || c == 's' ||
            c == 'A' || c == 'a' || c == 'D' || c == 'd');
}

bool is_filling_action(char c) {
    return (c >= '0' && c <= '4');
}

string get_styled_text(string text, string style) {
    string color = "", font = "";
    for (char c : style) {
        if (c == 'R') color = "31";  // Red for errors
        if (c == 'G') color = "32";  // Green for filled
        if (c == 'E') color = "41";  // Red background for invalid cells
        if (c == 'C') color = "106"; // Cyan background for cursor
        if (c == 'B') font = ";1";   // Bold for immutable cells
    }
    return "\x1b[" + color + font + "m" + text + "\x1b[0m";
}

void print_board() {
    // Clear the screen
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

            // Set style for the cursor position
            if (cur_r == i && cur_c == j)
                style = "C";
            // Set style for the cell in an invalid line
            else if (is_invalid(i, j))
                style = "E";  // Red background for invalid cells
            // Set style for a finished line
            else if (is_done(i, j))
                style = "G";  // Green for filled cells
            // Set style for immutable cells
            else if (!editable[i][j])
                style += "B";  // Blue for immutable cells
            // Initial cell color should be white
            else {
                style = "";  // No color for editable and valid cells, they are white by default
            }

            // Print the cell content
            if (board[i][j] == 0)
                cout << get_styled_text(" · ", style);
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

    // Check for conflicts in rows, columns, and blocks
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
    if (c == 'W' || c == 'w') {
        // Move up, until finding an editable cell or reaching boundary
        for (int i = cur_r - 1; i >= 0; --i) {
            if (editable[i][cur_c]) {
                cur_r = i;
                return;
            }
        }
    }
    else if (c == 'S' || c == 's') {
        // Move down, until finding an editable cell or reaching boundary
        for (int i = cur_r + 1; i < 4; ++i) {
            if (editable[i][cur_c]) {
                cur_r = i;
                return;
            }
        }
    }
    else if (c == 'A' || c == 'a') {
        // Move left, until finding an editable cell or reaching boundary
        for (int i = cur_c - 1; i >= 0; --i) {
            if (editable[cur_r][i]) {
                cur_c = i;
                return;
            }
        }
    }
    else if (c == 'D' || c == 'd') {
        // Move right, until finding an editable cell or reaching boundary
        for (int i = cur_c + 1; i < 4; ++i) {
            if (editable[cur_r][i]) {
                cur_c = i;
                return;
            }
        }
    }
}

int main() {
    char c;
    bool action_ok;

    initialize();
    while (cin >> c) {
        action_ok = false;
        if (is_moving_action(c)) {
            action_ok = true;
            move_cursor(c);
        }

        if (is_filling_action(c)) {
            action_ok = true;
            fill_number(c);
        }

        if (c == 'Q' || c == 'q')
            break;

        // Recheck conflicts after filling
        check_horizontal(cur_r);
        check_vertical(cur_c);
        check_block(cur_r / 2, cur_c / 2);

        print_board();

        if (check_win()) {
            cout << "YOU WIN!" << endl;
            break;
        }

        if (!action_ok)
            cout << get_styled_text("!!! Invalid action !!!", "R");
    }

    return 0;
}
