#include <iostream>
#include <string>
#include <sstream>
#include <fstream>
#include <vector>
#include <algorithm>

using namespace std;
/*
 * struct Instruction keeps the data of instruction name, source registers and
 * the destination register as strings in the struct's fields.
 */
struct Instruction{
    string name;
    string rs1;
    string rs2;
    string rd;
};
string input_path;
string output_path;
ofstream writer;
bool isNotAlnum(char c) {
    return isalnum(c) == 0;
}
/*
 * Main function of the program.
 * Parameters: input file path, output file path
 * How to compile:  g++ main.cpp -o main
 * How to run:      ./main <input_path> <output_path>
 */
int main(int argc, char *argv[]) {
    vector<Instruction> instructions;
    /// Get input and output paths
    input_path = argv[1];
    output_path = argv[2];
    ifstream inst_file(input_path);

    string inst, rs1, rs2,rd,name;
    /*
     * Input file is read line by line.
     * Each line is separated into instruction name and corresponding registers.
     * A new Instruction object is created and pushed back to the instructions vector.
     */
    if(inst_file.is_open()){
        while(getline(inst_file, inst)){
            stringstream lineSplit(inst);
            getline(lineSplit, name, ' ');
            if(name == "add"||name == "sub"||name == "and"||name == "or"){
                getline(lineSplit, rd, ',');
                getline(lineSplit, rs1, ',');
                getline(lineSplit, rs2);
            }
            else if(name == "ld"||name=="sd"){
                getline(lineSplit, rd, ',');
                string str1,str2;
                getline(lineSplit, str1,'(');
                getline(lineSplit,rs1,')');
                rs2="";
            }
            else if(name=="beq"){
                getline(lineSplit, rs1, ',');
                getline(lineSplit, rs2,',');
                getline(lineSplit, rd);
            }
            else{
//                cout<<"Give a proper assembly file"<<endl;
                exit(0);
            }
            Instruction instruc;
            instruc.name = name;
            instruc.rd = rd;
            instruc.rs1 = rs1;
            instruc.rs2 = rs2;
            instructions.push_back(instruc);
        }
    }
    /*
     * If the input file could not be opened, a message to the user is printed.
     */
    else{
        cout<<"provide a proper file path"<<endl;
    }
    /*
     * int stall_counter: for printing the number of total stalls added to the output file
     * vector<Instruction> instructions_with_stall: stalls inserted version of the instructions
     * vector<Instruction> reasons_for_stall: for printing the instructions causing the stalls
     */
    int stall_counter=0;
    vector<Instruction> instructions_with_stall;
    vector<Instruction> reasons_for_stall;
    /*
     * This for-loop iterates over the instructions vector, while keeping
     * two pointers at each iteration: itr and next.
     * Instructions pointed by itr and next are compared and adequate number of
     * stalls are inserted into the vectors: reasons_for_stall and instructions_with_stall
     *
     * In our program, a stall (bubble) is an instruction with the following fields:
     *      name= "stall"; rs1, rs2, rd = "";
     *
     *  If stalls are added, stall_counter is incremented while loop iteration.
     */
    for(auto itr = instructions.begin(); itr != instructions.end(); ++itr){
        auto next = itr+1;
        instructions_with_stall.push_back(*itr);
        /*
         * If the itr points to a load instruction,
         * and one of the following conditions are held, 1 stall is inserted:
         * itr's destination register == next's 1st source register
         * itr's destination register == next's 2nd source register
         */
        if(itr->name == "ld"){
            if(itr->rd == next->rs1 || itr->rd == next->rs2){
                Instruction stall;
                stall.name = "stall";
                stall.rs1 = "";
                stall.rs2 = "";
                stall.rd = "";
                reasons_for_stall.push_back(*itr);
                instructions_with_stall.push_back(stall);
                stall_counter++;
            }
        }
            /*
             * If the itr points to a branch if equal instruction,
             *
             */
        else if(itr->name == "beq"){
            reasons_for_stall.push_back(*itr);
            if(itr->rs1 == itr->rs2){
                string x=itr->rd;
                for(int i = 0;i<stoi(x)-1;i++){
                    itr++;
                }
            }
            Instruction stall;
            stall.name = "stall";
            stall.rs1 = "";
            stall.rs2 = "";
            stall.rd = "";
            instructions_with_stall.push_back(stall);
            instructions_with_stall.push_back(stall);
            stall_counter +=2;
        }
    }
    writer.open(output_path, ios::app);
    double clocks,cpi;
    /*
     * number of clock cycles is calculated by adding 4 to the instruction number and stalls,
     * since the first instruction takes 5 cycles and the others add only 1.
     * */
    clocks = instructions_with_stall.size()+4;
    /*
     * clock per instruction. It is calculated bby dividing the number of cycles to the
     * number of instructions will be calculated */
    cpi = clocks/(instructions_with_stall.size()-stall_counter);
    writer<<"Welcome to the simulator!\nTotal number of clocks:"<<clocks<<"\nCPI:"<<cpi<<"\nNumber of stalls:"<<stall_counter<<endl;
    if (stall_counter){
        writer<<"Instructions causing stalls and the corresponding number of stalls:"<<endl;
    }

    for(auto & instruction:reasons_for_stall){
        if(instruction.name=="beq"){
            writer<<instruction.name<< " "<<instruction.rs1<<","<<instruction.rs2<<","<<instruction.rd<<", 2 stalls"<<endl;
        }
        else {
            writer<<instruction.name<< " "<<instruction.rd<<","<<instruction.rs1<<","<<instruction.rs2<<" 1 stall"<<endl;
        }
    }
    writer<<"----------------------------------------------------------------------------------------------------"<<endl;
    int i=1;
    /*
     * write the instruction names to file as they are simulating the clock cycle
     */
    for(auto & instruction : instructions_with_stall){
        for(int j = 0; j<i-1;j++){
            writer<<"\t\t\t\t";
        }
        for(int k = 0; k<5;k++){
            if(instruction.name=="stall"){
                writer<<"stall\t\t\t";
            }
            else if(instruction.name=="beq"){
                writer<<instruction.name<< " "<<instruction.rs1<<","<<instruction.rs2<<","<<instruction.rd<<"\t\t";
            }
            else if(instruction.name == "ld"||instruction.name=="sd"){
                writer<<instruction.name<< " "<<instruction.rd<<","<<instruction.rs1<<","<<instruction.rs2<<"\t\t";
            }
            else{
                writer<<instruction.name<< " "<<instruction.rd<<","<<instruction.rs1<<","<<instruction.rs2<<"\t";
            }
        }
        writer << "\n";
        i++;
    }
    writer.close();

    return 0;
}
