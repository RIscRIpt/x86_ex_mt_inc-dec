#include <Windows.h>
#include <stdio.h>
#include <math.h>

int number;

const int NbOfTests      = 1000;
volatile int NbOfChanges = 10000;

DWORD WINAPI incer(LPVOID param) {
    for(int i = 0; i < NbOfChanges; i++) {
        number++;
        //__asm inc number
        //__asm lock inc number
    }
}

DWORD WINAPI decer(LPVOID param) {
    for(int i = 0; i < NbOfChanges; i++) {
        number--;
        //__asm dec number
        //__asm lock dec number
    }
}

int main(void) {
    HANDLE hCurrProc;
    DWORD_PTR pam, sam;

    HANDLE hThreads[2];

    // Make process running on a single core
    if(1) {
        hCurrProc = GetCurrentProcess();
        GetProcessAffinityMask(hCurrProc, &pam, &sam);
        SetProcessAffinityMask(hCurrProc, sam & -sam);
        // X & -X isolates least significant bit which is set (1).
    }

    int count = 0;
    for(int i = 0; i < NbOfTests; i++) {
        number = 0;
        hThreads[0] = CreateThread(0, 0, incer, NbOfChanges, 0, 0);
        hThreads[1] = CreateThread(0, 0, decer, NbOfChanges, 0, 0);
        WaitForMultipleObjects(ARRAYSIZE(hThreads), hThreads, TRUE, INFINITE);
        CloseHandle(hThreads[0]);
        CloseHandle(hThreads[1]);
        if(number) {
            count++;
            printf("%-5d : %d\t", i, number);
            if(count % 4 == 0)
                printf("\n");
        }
    }
    printf("\ncount = %d\n", count);

    getch();
    return 0;
}
