#include<iostream>
#include<vector>
#include<map>
#include<set>
#include<cmath>
#include<iomanip>
#include <sys/types.h>
#include <sys/stat.h>
using namespace std;

//stores the names of files for a particular inode number
map<int, vector<string> > files_list;

//stores inode number for a file
map<string,int> inode_number;

//stores the names of already used filenames in a set
set<string> used_file_names;

//stores the content of a file
map<int, string > file_data;


//-------------------------Function declaration----------------------------------
bool create_files(string filename , string file_content);
bool delete_files(string filename);
void rename_files(string filename1 , string filename2);
void print_file_content(string filename);
void display_filenames();
void delete_all_files();

//-----------------------------------main-----------------------------------------
int main(){
	string command;
	const char *s="root";
        mkdir(s,S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);
	while(true){
		
		//taking command as input from console
		cout<<"\nEnter command (To exit , enter \"EXIT\" without quotes): ";
		getline(cin,command);
		
		//exit form program
		if(command=="EXIT") break;
		int l = command.size();
		
		//no command is of size less than 2
		if(l<2) cout<<"\nINVALID COMMAND!\n";
		
		//Create file command
		if(command[0]=='m' && command[1]=='f'){
			int i;
			
			//create file command size cannot be less than 6
			if(l<6 || command[2]!=' '){
				cout<<"INVALID COMMAND!\n";
				continue;
			}
			
			//checking if file content has been provided or not
			for(i = 3 ; i < l ; i++)
				if(command[i]==' ') break;
			if(i==l){
				cout<<"INVALID COMMAND!\n";
				continue;
			}
			
			//getting filename from command
			int file_name_size = i-3;
			string filename = command.substr(3,file_name_size);
			
			//getting file content
			string file_content = command.substr(i+1);
			if(file_content.size()==0) {
				cout<<"INVALID COMMAND!\n";
				continue;
			}
			
			//calling create file function
			bool cr = create_files(filename, file_content);
			if(cr) cout<<"File Created!\n";		
		}
		
		//delete dile command
		else if(command[0]=='d' && command[1]=='f'){
			//command size cannot be less than 4
			if(l<4 || command[2]!=' '){
				cout<<"INVALID COMMAND!\n";
				continue;
			}
			
			//getting filename from command
			string filename = command.substr(3);
			if(filename.size()==0){
				cout<<"INVALID COMMAND!\n";
				continue;
			}
			
			//callinf delete file function
			int dl = delete_files(filename);
			if(dl) cout<<"File deleted!\n";
		}
		
		//rename file 
		else if(command[0]=='r' && command[1]=='f'){
			//command size cannot be less than 6
			if(l<6 || command[2]!=' '){
				cout<<"INVALID COMMAND!\n";
				continue;
			}
			
			//checking filenames
			int i;
			for(i = 3 ; i < l ; i++)
				if(command[i]==' ') break;
			if(i==l){
				cout<<"INVALID COMMAND!\n";
				continue;
			}
			int file_name_size = i-3;
			if(file_name_size<=0){
				cout<<"INVALID COMMAND!\n";
				continue;
			}
			
			//getting the old and new filenames
			string filename1 = command.substr(3,file_name_size);
			if(i>=l-1){
				cout<<"INVALID COMMAND!\n";
				continue;
			}
			string filename2 = command.substr(i+1);
			
			//calling the rename file function
			rename_files(filename1, filename2);
		}
		
		//print file function
		//prints the contents of the required file
		else if(command[0]=='p' && command[1]=='f'){
			if(l<4 || command[2]!=' '){
				cout<<"INVALID COMMAND!\n";
				continue;
			}
			string filename = command.substr(3);
			print_file_content(filename);
		}
		
		//ls command - lists all the files and their inode numbers
		else if(command[0]=='l' && command[1]=='s'){
			if(l>2){
				cout<<"INVALID COMMAND!\n";
				continue;
			}
			display_filenames();
		}
		else{
			cout<<"INVALID COMMAND!\n";
		}
	}
	
	//delete all files in the end
	delete_all_files();
	return 0;
}

//----------------------------Function Definition--------------------------------

//creates files with name filename and content file_content
//the content is divided into strings of four characters and put into separate files
bool create_files(string filename , string file_content){
	
	//checking if the filename already exits
	//if it does then return from the funtion
	if(used_file_names.find(filename)!=used_file_names.end()){
		cout<<"A file with this name already exists!\n";
		return false;
	}
	
	//generating a random inode number for the file
	int inode_num = rand()%1000;
	
	//checking if the generated inode number already exists
	while(files_list.find(inode_num)!=files_list.end())
		inode_num = rand()%1000+1;
		
	//inserting this new filename to the used_file_names set
	used_file_names.insert(filename);
	inode_number[filename] = inode_num;
	file_data[inode_num] = file_content;
	int l = file_content.size();
	
	//No. of files to be created
	int num_files = ceil((float)l/4);
	
	int j = 0;
	//creating files and putting 4 bytes of data in each file
	for(int i = 0 ; i < num_files ; i++){
		string file ="root/" + filename +"_"+ to_string(i)+".txt";
		files_list[inode_num].push_back(file);
		FILE* fp = fopen(file.c_str(),"w");
		for(int k = 0 ; k < 4 && j<l ;k++,j++)
			fprintf(fp,"%c" , file_content[j]);
		fclose(fp);
	}
	return true;
}

//deletes file with name filename(if it exists)
bool delete_files(string filename){
	
	//cheking if the file with name filename exists or not
	//if it doesnot then return from function
	if(used_file_names.find(filename)==used_file_names.end()){
		cout<<"This file doesnot exists\n";
		return false;
	}
	
	//getting the inode number of file to be deleted	
	int inode_num = inode_number[filename];
	
	//getting the list of files to be deleted
	vector<string> to_delete = files_list[inode_num];
	int l = to_delete.size();
	
	//deleting the files
	for(int i = 0 ; i < l ; i++){
		remove(to_delete[i].c_str());
	}
	
	//removing the file info from maps and set
	files_list.erase(inode_num);
	file_data.erase(inode_num);
	used_file_names.erase(filename);
	inode_number.erase(filename);
	return true;
}

//renames filename1 to filename2
void rename_files(string filename1 , string filename2){
	//cheking if the file with name filename1 exists or not
	//if it doesnot then return from function
	if(used_file_names.find(filename1)==used_file_names.end()){
		cout<<"The file to be named doesnot exist";
		return;
	}
	
	//checking if the filename2 already exits
	//if it does then return from the funtion
	if(used_file_names.find(filename2)!=used_file_names.end()){
		cout<<"A file with new name already exists";
		return;
	}
	
	//Removig filename1 from  set and inserting filename2
	used_file_names.erase(filename1);
	used_file_names.insert(filename2);
	
	//getting inode number for filename1 and the list of files
	int inode_num = inode_number[filename1];
	vector<string> files = files_list[inode_num];
	int l = files_list[inode_num].size();
	
	//renaming files and updating the files_list map
	for(int i = 0 ; i < l ; i++){
		string file = "root/" + filename2 +"_"+ to_string(i)+".txt"; 
		//string old = files[i];
		string old = "root/" + filename1 +"_"+ to_string(i)+".txt"; 
		rename(old.c_str(),file.c_str());
		files[i] = file;
	}
	files_list[inode_num] = files;
	
	//inserting filename2 in inode_number map
	inode_number[filename2] = inode_num;
	inode_number.erase(filename1);
	cout<<filename1<<" has been renamed to "<<filename2<<endl;
}
void print_file_content(string filename){
	//checking if the requested file exixtx or not
	if(used_file_names.find(filename)==used_file_names.end()){
		cout<<"This file doesnot exist\n";
		return;
	}
	
	//getting the inode number of the requested file 
	int inode_num = inode_number[filename];
	
	//printing the contents of that file
	cout<<file_data[inode_num];
}

//Display all the filenames in the directory with their inode numbers
void display_filenames(){
	cout<<"\n\n";
	cout<<setw(20)<<"FILENAME"<<setw(15)<<"INODE NUMBER\n";
	cout<<"========================================\n";
	for(auto it : inode_number){
		cout<<setw(18)<<it.first<<setw(10)<<it.second<<endl;
		cout<<"________________________________________\n";
	}
	cout<<endl;	
}

//deleting all files after execution of the program
void delete_all_files(){
	for(auto it : files_list)
	{
		vector<string> to_delete = it.second;
		int l = to_delete.size();
		for(int i = 0 ; i < l ; i++)
			remove(to_delete[i].c_str());
	}
}