#include <iostream>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <fstream>

using namespace std;

int main() {
	char user[80], other[80];
	const char *TomToJerry = "./my-fifo-a"; // FIFO의 이름 경로명
	const char *jerryToTom = "./my-fifo-b"; // FIFO의 이름 경로명

	mkfifo(TomToJerry, 0666); // ./my-fifo-a 이름의 FIFO 생성
	mkfifo(jerryToTom, 0666); // ./my-fifo-b 이름의 FIFO 생성

    ofstream out(jerryToTom);
    ifstream in(TomToJerry);

    if(fork()>0){
        while(1){
            cin >> user;
        cout << ":";
        cout << user << endl;
        out << user << endl;
        }
    }
    else{
        while(1){
            in >> other;
            cout << other << endl;
        }
    }
	
	in.close();
    out.close();
	return 0;
}
