# Compiler
A compiler made in C++ that compiles a custom language created by me.

It currently compiles my language to [NASM](https://www.nasm.us/) which then, thanks to the `run.bat` script, goes through the NASM assembler and then through `GNU ld` linker, which creates an executable file.

The grammar of this custom language is a mix of Rust and C++.

## Example
This is a tris game made in my language:

https://github.com/Angelo13C/Compiler/assets/55251189/7fd8d280-3d1e-4476-a15f-112581ee0cb0

This is the code:
```c++
// THESE ARE GENERIC FUNCTIONS FOR MY LANGUAGE

fn int tern(int condition, int trueValue, int falseValue)
{
    if condition
    {
        return trueValue;
    }
    return falseValue;
}

fn int setChar(string str, int index, int char)
{
    asm!("
    mov rax, QWORD [rsp + 24]
    add rax, QWORD [rsp + 16]
    mov rbx, QWORD [rsp + 8]
    mov byte [rax], bl
    ");
    return 0;
}
fn int getChar(string str, int index)
{
    int char = 0;
    asm!("
    mov rax, QWORD [rsp + 24]
    add rax, QWORD [rsp + 16]
    mov rax, [rax]
    mov byte [rsp], al
    ");
    return char;
}

fn int itoa(string buffer, int numberToConvert)
{
    int charactersCount = 0;

    int digit0 = 48;

    if numberToConvert == 0
    {
        setChar(buffer, 0, digit0);
    }

    while numberToConvert != 0
    {
        int newNumber = numberToConvert / 10;
        int digit = numberToConvert - newNumber * 10;
        numberToConvert = newNumber;
        setChar(buffer, charactersCount, digit0 + digit);

        charactersCount = charactersCount + 1;
    }

    strReverse(buffer);

    return charactersCount;
}
fn string charToString(int char)
{
    string buffer = allocateOnTheHeap(2);
    setChar(buffer, 0, char);
    return buffer;
}
// Returns `1` if the `a` and `b` string contain the same content, otherwise returns `0`
fn int strAreEqual(string a, string b)
{
    int i = 0;
    while 1
    {
        int aChar = getChar(a, i);
        int bChar = getChar(b, i);
        if aChar
        {
            if bChar
            {
                if aChar != bChar
                {
                    return 0;
                }
            }
            else
            {
                return 0;
            }
        }
        else
        {
            if bChar
            {
                return 0;
            }
            else
            {
                return 1;
            }
        }
        i = i + 1;
    }
}

fn string allocateOnTheHeap(int bytesCount)
{
    string allocation;
    asm!("
    mov  rcx, [rel heapHandle]
    mov  rdx, 8; Allocation flags
    mov  r8, QWORD [rsp + 16]; Size to allocate

    push rcx; Shadow space
    push rcx; Shadow space
    push rcx; Shadow space
    push rcx; Shadow space
    call HeapAlloc
    pop rcx; Delete the shadow space
    pop rcx; Delete the shadow space
    pop rcx; Delete the shadow space
    pop rcx; Delete the shadow space

    mov QWORD [rsp], rax
    ");
    return allocation;
}
fn int deallocateFromTheHeap(string allocation)
{
    asm!("
    mov  rcx, [rel heapHandle]
    mov  rdx, 0; Deallocation flags
    mov  r8, QWORD [rsp + 8]; Memory to deallocate

    push rcx; Shadow space
    push rcx; Shadow space
    push rcx; Shadow space
    push rcx; Shadow space
    call HeapAlloc
    pop rcx; Delete the shadow space
    pop rcx; Delete the shadow space
    pop rcx; Delete the shadow space
    pop rcx; Delete the shadow space
    ");
    return 0;
}

// Returns the number of characters in the `str` string
fn int strLen(string str)
{
    asm!("
    mov rax, QWORD [rsp + 8]
    dec rax
.strLenLoop:  
    inc rax
    cmp byte [rax], 0
    jne .strLenLoop
    sub rax, QWORD [rsp + 8]
    mov QWORD [rsp + 8], rax"
    );
    return str;
}

fn int strReverse(string buffer)
{
    int length = strLen(buffer);
    int halfLength = length / 2;
    int i = 0;
    while i < halfLength
    {
        int leftChar = getChar(buffer, i);
        int rightChar = getChar(buffer, length - i - 1);
        setChar(buffer, i, rightChar);
        setChar(buffer, length - i - 1, leftChar);

        i = i + 1;
    }
    return length;
}

fn int print(string str)
{
    int length = strLen(str);
    asm!("
        mov  rcx, [rel stdout]
        mov  rdx, QWORD [rsp + 16]; Message to print
        mov  r8, QWORD [rsp]; Length of message to print
        mov  r9, bytesWritten
        push qword 0
        
        push rcx; Shadow space
        push rcx; Shadow space
        push rcx; Shadow space
        push rcx; Shadow space
        call WriteFile
        pop rcx; Delete the shadow space
        pop rcx; Delete the shadow space
        pop rcx; Delete the shadow space
        pop rcx; Delete the shadow space
        
        pop rax
    ");
    return length;
}
// Always returns 0! (differently from `scanWithoutNewLine`)
fn int scan(string bufferToWrite)
{
    int temp = 0;
    asm!("
        mov rcx, [rel stdin]
        mov rdx, QWORD [rsp + 16]
        mov r8, 50; Number of characters to read
        mov r9, QWORD [rsp]
        push qword 0; Add the optional parameter
        
        push rcx; Shadow space
        push rcx; Shadow space
        push rcx; Shadow space
        push rcx; Shadow space
        call ReadFile
        pop rcx; Delete the shadow space
        pop rcx; Delete the shadow space
        pop rcx; Delete the shadow space
        pop rcx; Delete the shadow space

        pop rcx; Remove the optional parameter
    ");
    return 0;
}
// Returns the number of bytes that have been read from the scan
fn int scanWithoutNewLine(string bufferToWrite)
{
    scan(bufferToWrite);

    // Remove the new line
    int length = strLen(bufferToWrite);
    setChar(bufferToWrite, length - 2, 0);

    return length - 2;
}

fn int strCopy(string into, string from)
{
    int fromLength = strLen(from);
    int i = 0;
    while i < fromLength
    {
        int c = getChar(from, i);
        setChar(into, i, c);
        i = i + 1;
    }
    return i;
}

fn int strRemoveFirstNCharacters(string str, int n)
{
    int i = 0;
    while i < n
    {
        setChar(str, i, 0);
        i = i + 1;
    }

    return 0;
}


// THESE ARE THE ACTUAL FUNCTIONS FOR THE GAME
playGame();
return 0;

fn int playGame()
{
    // Constants
    int EMPTY_CELL_SYMBOL = 32; // ' ' in ASCII
    int PLAYER_1_SYMBOL = 88; // 'X' in ASCII
    int PLAYER_2_SYMBOL = 79; // 'O' in ASCII
    int INVALID_COORDINATES = 99;
    int TRUE = 1;
    int FALSE = 0;
    int SCAN_INPUT_SIZE = 50;

    // There's no support for stack allocated arrays. That's why I am allocating on the heap
    string board = allocateOnTheHeap(3 * 3 + 1);
    string input = allocateOnTheHeap(SCAN_INPUT_SIZE);

    // The players can play multiple matches in a single run
    int isPlaying = TRUE;
    while isPlaying
    {
        print("\nStarting the match!\n");

        // Reset the board to blank
        strCopy(board, "         ");

        int playedTurnsCount = 0;
        int isMatchFinished = FALSE;
        int currentPlayer = 1;
        while isMatchFinished == FALSE
        {
            printBoard(board);

            // Get a valid coordinate input
            int coordinatesAsIndex = INVALID_COORDINATES;
            while coordinatesAsIndex == INVALID_COORDINATES
            {
                print("- Player '");
                print(charToString(getPlayerSymbol(currentPlayer, PLAYER_1_SYMBOL, PLAYER_2_SYMBOL)));
                print("', insert the cell location (valid inputs are: 'A1', 'A2', 'A3', 'B1', 'B2', 'B3', 'C1', 'C2', 'C3' without the quotes): ");

                scanWithoutNewLine(input);

                coordinatesAsIndex = coordinatesToIndex(input, INVALID_COORDINATES);
                if coordinatesAsIndex == INVALID_COORDINATES
                {
                    print("The inserted cell is invalid, retry\n");
                }
                else
                {
                    if isCellEmpty(board, coordinatesAsIndex, EMPTY_CELL_SYMBOL) == 0
                    {
                        print("The inserted cell is already occupied, retry\n");
                        coordinatesAsIndex = INVALID_COORDINATES;
                    }
                }

                // Clear the input buffer
                strRemoveFirstNCharacters(input, SCAN_INPUT_SIZE);
            }

            // Set the correct cell based on the input
            int currentPlayerSymbol = getPlayerSymbol(currentPlayer, PLAYER_1_SYMBOL, PLAYER_2_SYMBOL);
            setChar(board, coordinatesAsIndex, currentPlayerSymbol);

            // Check if someone has won
            if hasWon(board, currentPlayerSymbol)
            {
                printBoard(board);
                print("\n\nPlayer ");
                print(charToString(currentPlayerSymbol));
                print(" wins!");

                isMatchFinished = TRUE;
            }
            else
            {
                // Check if the match finished with a tie
                playedTurnsCount = playedTurnsCount + 1;
                if playedTurnsCount == 9
                {
                    isMatchFinished = TRUE;
                    printBoard(board);

                    print("\n\nTIE!");
                }
            }

            // Change the turn of the player
            currentPlayer = tern(currentPlayer == 1, 2, 1);
        }

        // Check if the players want to play again
        print("\n- Do you want to play again? (S/N): ");
        scanWithoutNewLine(input);

        if strAreEqual(input, "N")
        {
            isPlaying = FALSE;
        }
        if strAreEqual(input, "n")
        {
            isPlaying = FALSE;
        }

        // Clear the input buffer
        strRemoveFirstNCharacters(input, SCAN_INPUT_SIZE);
    }

    deallocateFromTheHeap(board);
    deallocateFromTheHeap(input);

    return 0;
}

fn int checkWinConditionInLine(string board, int playerSymbolToCheck, int xMultiplier, int yMultiplier)
{
    int y = 0;
    while y < 3
    {
        int x = 0;
        while x < 3
        {
            if getChar(board, y * yMultiplier + x * xMultiplier) != playerSymbolToCheck
            {
                x = 100;
            }
            else
            {
                x = x + 1;
            }
        }
        if x == 3
        {
            return 1;
        }

        y = y + 1;
    }
    return 0;
}
fn int checkWinConditionInDiagonal(string board, int playerSymbolToCheck, int pos1, int pos2, int pos3)
{
    if getChar(board, pos1) == playerSymbolToCheck
    {
        if getChar(board, pos2) == playerSymbolToCheck
        {
            if getChar(board, pos3) == playerSymbolToCheck
            {
                return 1;
            }
        }
    }
    return 0;
}

// Returns `1` if a player has won, otherwise `0`
fn int hasWon(string board, int playerSymbolToCheck)
{
    if checkWinConditionInLine(board, playerSymbolToCheck, 3, 1)
    {
        return 1;
    }
    if checkWinConditionInLine(board, playerSymbolToCheck, 1, 3)
    {
        return 1;
    }
    if checkWinConditionInDiagonal(board, playerSymbolToCheck, 0, 4, 8)
    {
        return 1;
    }
    if checkWinConditionInDiagonal(board, playerSymbolToCheck, 2, 4, 6)
    {
        return 1;
    }

    return 0;
}

fn int coordinatesToIndex(string coordinates, int invalidCoordinates)
{
    if strAreEqual(coordinates, "A1")
    {
        return 0;
    }
    if strAreEqual(coordinates, "B1")
    {
        return 1;
    }
    if strAreEqual(coordinates, "C1")
    {
        return 2;
    }
    if strAreEqual(coordinates, "A2")
    {
        return 3;
    }
    if strAreEqual(coordinates, "B2")
    {
        return 4;
    }
    if strAreEqual(coordinates, "C2")
    {
        return 5;
    }
    if strAreEqual(coordinates, "A3")
    {
        return 6;
    }
    if strAreEqual(coordinates, "B3")
    {
        return 7;
    }
    if strAreEqual(coordinates, "C3")
    {
        return 8;
    }

    return invalidCoordinates;
}
fn int getPlayerSymbol(int playerNumber, int player1Symbol, int player2Symbol)
{
    return tern(playerNumber == 1, player1Symbol, player2Symbol);
}
fn int printBoard(string boardToPrint)
{
    print("\n  A B C\n");
    int y = 0;
    while y < 3
    {
        if y != 0
        {
            print(" |-----|\n");
            if y == 1
            {
                print("2");
            }
            else
            {
                print("3");
            }
        }
        else
        {
            print(" -------\n1");
        }

        int x = 0;
        while x < 3
        {
            print("|");
            print(charToString(getChar(boardToPrint, y * 3 + x)));
            x = x + 1;
        }
        y = y + 1;
        print("|\n");
    }
    print(" -------\n");
    return 0;
}
fn int isCellEmpty(string boardToPrint, int placeIndex, int emptyCellSymbol)
{
    return getChar(boardToPrint, placeIndex) == emptyCellSymbol;
}
```
