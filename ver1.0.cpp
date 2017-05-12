#include <conio.h>
#include <iostream>
#include <stdio.h>
#include <string>
#include <ctime>
#include <fstream> 
#include <windows.h>
#include <vector>

using namespace std;
typedef struct process {
	string name;
	int status;
	PROCESS_INFORMATION pi;
} PROCESS;

vector<PROCESS> process_list;
vector<string> path_list;
BOOL CtrlHandler(DWORD fdwCtrlType); 
string current_Dir();
void Mess_error(string c);
void Execute_process(string exe_file);
void Print_time_date(string type);
void ClearScreen();
void Print_list_process(); 
int is_Background_cmd(string c);
void Background_dealing(string c);
void Print_list_path();
int is_Addpath_cmd(string c);
void Add_path(string c);
void Create_new_cmd();
void Print_help();
int is_Cd_cmd(string c);
void Change_dir(string c);
void handling(string c);
int is_Batch_cmd(string c);
void Execute_batch(string c);
int is_Echo_cmd(string c);
void Print_echo(string c);
int main(int argc, char const *argv[]) {	
//	SetConsoleCtrlHandler(NULL, FALSE);
	SetConsoleCtrlHandler((PHANDLER_ROUTINE)CtrlHandler, TRUE);
	// SetConsoleCtrlHandler(null, true);
//    GenerateConsoleCtrlEvent(CtrlTypes.CTRL_C_EVENT, 0);
	string c;
	cout << current_Dir();
	while(1) {
		getline(cin, c);
		handling(c);
		cout << current_Dir();
	}
	return 0;
}
int is_Echo_cmd(string c) {
	if (c.length() < 6) return 0;
	if (c.substr(0, 4) == "echo") return 1;
	return 0;
}
void Print_echo(string c) {
	if (c == "echo")
		cout << "echo is always on.\n";
	else
		cout << c.substr(5, c.length() - 1) << "\n";
}
void Execute_batch(string c) {
	string line;
  	ifstream batch_file(c.c_str());
  	if (batch_file.is_open()) {
		int i = 0;
		int is_Echo_on = 1;
    	while (getline(batch_file, line)) {
    		if (line == "@echo off") {
    			is_Echo_on = 0;
    			continue;
    		} else if (line == "@echo on") {
    			is_Echo_on = 1;
    			continue;
    		} else if (line == ":START") {
    			continue;
    		}
      		if (is_Echo_on) {
      			cout << current_Dir();
      			cout << line << "\n";
      			handling(line);
      		} else if (!is_Echo_on) {
      			handling(line);
      		}
    	}
    	batch_file.close();
  	}
	else
		cout << "Unable to open file"; 
	return;
}
int is_Batch_cmd(string c) {
	if (c.length() < 5) return 0;
	if (c.substr(c.length() - 4, c.length()) == ".bat") return 1;
	return 0;
}
void handling(string c) {
	if (c == "time" || c == "date") 
		Print_time_date(c);
	else if (c == "")
		cout << "";
	else if (c == "cls")
		ClearScreen();
	else if (c == "dir")
		system("dir");
	else if (c == "list")
		Print_list_process();
	else if (c == "path")
		Print_list_path();
	else if (c == "exit")
		exit(0);
	else if (c == "help")
		Print_help();
	else if (c == "pause") {
		cout << "Press Enter to continue...\n";
		getch();
	}
	else if (is_Echo_cmd(c))
		Print_echo(c);
	else if (is_Batch_cmd(c))
		Execute_batch(c);
	else if (c == "cmd")
		Create_new_cmd();
	else if (is_Cd_cmd(c))
		Change_dir(c);
	else if (is_Addpath_cmd(c))
		Add_path(c);
	else if (is_Background_cmd(c))
		Background_dealing(c);
	else 
		Execute_process(c);
}
BOOL CtrlHandler( DWORD fdwCtrlType ) 
{ 
  switch( fdwCtrlType ) 
  { 
    // Handle the CTRL-C signal. 
    case CTRL_C_EVENT: 
      printf( "\^C\n" );
//      Beep( 750, 300 ); 
      return TRUE;
 
//    // CTRL-CLOSE: confirm that the user wants to exit. 
//    case CTRL_CLOSE_EVENT: 
//      Beep( 600, 200 ); 
//      printf( "Ctrl-Close event\n\n" );
//      return( TRUE ); 
// 
//    // Pass other signals to the next handler. 
//    case CTRL_BREAK_EVENT: 
//      Beep( 900, 200 ); 
//      printf( "Ctrl-Break event\n\n" );
//      return FALSE; 
// 
//    case CTRL_LOGOFF_EVENT: 
//      Beep( 1000, 200 ); 
//      printf( "Ctrl-Logoff event\n\n" );
//      return FALSE; 
// 
//    case CTRL_SHUTDOWN_EVENT: 
//      Beep( 750, 500 ); 
//      printf( "Ctrl-Shutdown event\n\n" );
//      return FALSE; 
// 
    default: 
      return FALSE; 
  } 
} 
string current_Dir() {
	char pBuf[256]; 
	size_t len = sizeof(pBuf);
	GetCurrentDirectory(len, pBuf);
	string dir = pBuf;
	dir += ">";
	return dir;
}
void Mess_error(string c) {
	cout << '\'' << c << '\'' << "is not recognized as an internal or external command, operable program or batch file.\n" ;
	return;
}
void Execute_process(string exe_file) {
	string exe_file_extend;
	int Tag;
	int status = 0;
	// 0: foreground
	// 1: background
	DWORD exitCode = 0;
	STARTUPINFO si;
	PROCESS_INFORMATION pi;
	ZeroMemory(&si, sizeof(si));
	si.cb = sizeof(si);
	
	//preprocess file_name
	if (exe_file[exe_file.length() - 1] == '&') {
		status = 1;
		int space = exe_file.find(' ');
		exe_file = exe_file.substr(0, space);
	}
	if (exe_file.length() < 5)
		exe_file_extend = exe_file + ".exe";
	else if (exe_file.substr(exe_file.length() - 4, exe_file.length() - 1) != ".exe")
		exe_file_extend = exe_file + ".exe";
	else if (exe_file.substr(exe_file.length() - 4, exe_file.length() - 1) == ".exe")
		exe_file_extend = exe_file;
	
	if (status == 0) {
		Tag = CreateProcess(exe_file_extend.c_str(),
							NULL,
							NULL,
							NULL,
							FALSE,
							0,
							NULL,
							NULL,
							&si,
							&pi);
		int i = 0;
		while(!Tag) {
			string exe_file_abs_extend = path_list[i] + '\\' + exe_file_extend;
			Tag = CreateProcess(exe_file_abs_extend.c_str(),
								NULL,
								NULL,
								NULL,
								FALSE,
								0,
								NULL,
								NULL,
								&si,
								&pi);
			i++;
			if (i >= path_list.size()) break;
		}
		if(!Tag)
			Mess_error(exe_file);
		else
			process_list.push_back({exe_file_extend, status, pi});
			WaitForSingleObject(pi.hProcess, INFINITE);
			CloseHandle(pi.hProcess);
			CloseHandle(pi.hThread);
//			for(int i = process_list.size() - 1; i >= 0; i--) {
//				if (exe_file_extend == process_list[i].name) {
//					process_list.erase(process_list.end() - i);
//					break;
//				}
//			}
	} else {
		Tag = CreateProcess(exe_file_extend.c_str(),
							NULL,
							NULL,
							NULL,
							FALSE,
							CREATE_NEW_CONSOLE,
							NULL,
							NULL,
							&si,
							&pi);
		int i = 0;
		while(!Tag) {
			string exe_file_abs_extend = path_list[i] + '\\' + exe_file_extend;
			Tag = CreateProcess(exe_file_abs_extend.c_str(),
								NULL,
								NULL,
								NULL,
								FALSE,
								CREATE_NEW_CONSOLE,
								NULL,
								NULL,
								&si,
								&pi);
			i++;
			if (i >= path_list.size()) break;
		}
		if(!Tag)
			Mess_error(exe_file);
		else {
			process_list.push_back({exe_file_extend, status, pi});
			// CloseHandle(pi.hProcess);
			// CloseHandle(pi.hThread);
		}
	}
	return;
}

void Print_time_date(string type) {
	time_t rawtime;
  	struct tm * timeinfo;
  	char buffer[80];
  	time (&rawtime);
  	timeinfo = localtime(&rawtime);
  	if (type == "date") {
	  	strftime(buffer,sizeof(buffer),"The current date is %a %d-%m-%Y\n", timeinfo);
	  	cout << buffer;	  	
	} else if (type == "time"){
		strftime(buffer,sizeof(buffer),"The current time is %I:%M:%S\n", timeinfo);
  		cout << buffer;
  	}
	return;
}

void ClearScreen() {
	//source code from: https://msdn.microsoft.com/en-us/library/windows/desktop/ms682022(v=vs.85).aspx
	CONSOLE_SCREEN_BUFFER_INFO csbi;
	DWORD                      count;
	DWORD                      cellCount;
	COORD                      homeCoords = { 0, 0 };

	HANDLE hStdOut = GetStdHandle(STD_OUTPUT_HANDLE);
	if (hStdOut == INVALID_HANDLE_VALUE) return;

	/* Get the number of cells in the current buffer */
	if (!GetConsoleScreenBufferInfo( hStdOut, &csbi )) return;
	cellCount = csbi.dwSize.X *csbi.dwSize.Y;

	/* Fill the entire buffer with spaces */
	if (!FillConsoleOutputCharacter(
	hStdOut,
    (TCHAR) ' ',
    cellCount,
    homeCoords,
    &count
    )) return;

  /* Fill the entire buffer with the current colors and attributes */
  if (!FillConsoleOutputAttribute(
    hStdOut,
    csbi.wAttributes,
    cellCount,
    homeCoords,
    &count
    )) return;

  /* Move the cursor home */
  SetConsoleCursorPosition( hStdOut, homeCoords );
  }
void Print_list_process() {
	printf("%10s | %20s | %10s\n", "pId", "Name", "Status");
	for (int i = 0; i < process_list.size(); i++) {
		printf("%10d | %20s | %10d\n", process_list[i].pi.dwProcessId, process_list[i].name.c_str(), process_list[i].status);
	}
}  


int is_Background_cmd(string c) {
	int first_space = c.find(' ');
	string action = c.substr(0, first_space);
	if (action == "kill" || action == "resume" || action == "stop") return 1;
	return 0;
}

void Background_dealing(string c) {
	int found_flag = 0;
	int first_space = c.find(' ');
	string action = c.substr(0, first_space);
	string file = c.substr(first_space + 1, c.length());
	if (file.length() < 5)
		file += ".exe";
	else if (file.substr(file.length() - 4, file.length() - 1) != ".exe")
		file += ".exe";
	for(int i = process_list.size() - 1; i >= 0; i--) {
		if (process_list[i].name == file && process_list[i].status == 1) {
			found_flag = 1;
			if (action == "kill") {
				TerminateProcess(process_list[i].pi.hProcess, 0);
				process_list.erase(process_list.begin() + i);
			}
			else if (action == "stop")
				SuspendThread(process_list[i].pi.hThread);
			else if (action == "resume")
				ResumeThread(process_list[i].pi.hThread);
			break;
		}
	}
	if (!found_flag) 
		cout << file << " 's not found";
	return;
}
void Print_list_path() {
	if (path_list.size() == 0) {
		cout << "Your list of path is empty \n";
		return;
	}
	cout << "PATH=";
	for (int i = 0; i < path_list.size() - 1; i++) {
		cout << path_list[i] << "; ";
	}
	cout << path_list[path_list.size() - 1] << "\n";
	return;
}
int is_Addpath_cmd(string c) {
	int first_space = c.find(' ');
	string action = c.substr(0, first_space);
	if (action == "addpath") return 1;
	return 0;
}
void Add_path(string c) {
	int first_space = c.find(' ');
	string path = c.substr(first_space + 1, c.length());
	path_list.push_back(path);
	return;
}
void Create_new_cmd() {
	STARTUPINFO si;
	PROCESS_INFORMATION pi;
	ZeroMemory(&si, sizeof(si));
	si.cb = sizeof(si);
	CreateProcess("try.exe",
					NULL,
					NULL,
					NULL,
					FALSE,
					CREATE_NEW_CONSOLE,
					NULL,
					NULL,
					&si,
					&pi);
}
void Print_help() {
	cout << "For more information on a specific command, type help command-name\n";
	printf("%-10s%s\n", "cls", "Clears the screen.");
	printf("%-10s%s\n", "date", "Displays the date.");
	printf("%-10s%s\n", "time", "Displays the system time.");
	printf("%-10s%s\n", "exit", "Quits the Myshell.EXE program.");
	printf("%-10s%s\n", "dir", "Displays a list of files and subdirectories in a directory.");
	printf("%-10s%s\n", "list", "Displays a list of executed or executing process.");
	printf("%-10s%s\n", "kill", "Terminates an executing process on background mode. ");
	printf("%-10s%s\n", "stop", "Suspends an executing process on background mode.");
	printf("%-10s%s\n", "resume", "Resumes an suspending process on background mode");
	printf("%-10s%s\n", "path", "Displays a search path for executable files.");
	printf("%-10s%s\n", "addpath", "Sets a search path for executable files.");
	// printf("%-10s%s\n", "", "");
	// printf("%-10s%s\n", "", "");
	// printf("%-10s%s\n", "", "");
	// printf("%-10s%s\n", "", "");
	cout << "For more information on tools contact me via email: dungmin97@gmail.com.\n";
}
int is_Cd_cmd(string c) {
	int first_space = c.find(' ');
	string action = c.substr(0, first_space);
	if (action == "cd") return 1;
	return 0;
}
void Change_dir(string c) {
	int first_space = c.find(' ');
	string new_dir = c.substr(first_space + 1, c.length());
	if(!SetCurrentDirectory(new_dir.c_str())) {
		char pBuf[256]; 
		size_t len = sizeof(pBuf);
		GetCurrentDirectory(len, pBuf);
		string current_dir = pBuf;
		if (new_dir == "..") {
			for(int i = current_dir.length() - 1; i >= 0; i--) {
				if(current_dir[i] == '\\') {
					string new_path = current_dir.substr(0, i);
					SetCurrentDirectory(new_path.c_str());
					return;
				}
			}
		}
		string new_path = current_dir + '\\' + new_dir;
		if(!SetCurrentDirectory(new_path.c_str())) 
			cout << "The system cannot find the path specified.\n";
	}
}
