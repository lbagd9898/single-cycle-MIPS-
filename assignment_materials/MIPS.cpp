#include<iostream>
#include<string>
#include<vector>
#include<bitset>
#include<fstream>

using namespace std;

// Internal ALU control codes.
#define ADDU (1)
#define SUBU (3)
#define AND (4)
#define OR  (5)
#define NOR (7)

// Memory size.
// In reality, the memory size should be 2^32, but for this lab and space reasons,
// we keep it as this large number, but the memory is still 32-bit addressable.
#define MemSize (65536)


class RF
{
  public:
    bitset<32> ReadData1, ReadData2; 
    RF()
    { 
      Registers.resize(32);  
      Registers[0] = bitset<32> (0);  
    }

    void ReadWrite(bitset<5> RdReg1, bitset<5> RdReg2, bitset<5> WrtReg, bitset<32> WrtData, bitset<1> WrtEnable)
    {   

      /**
       * @brief Reads or writes data from/to the Register.
       *
       * This function is used to read or write data from/to the register, depending on the value of WrtEnable.
       * Put the read results to the ReadData1 and ReadData2.
       */
      // TODO: implement!

    }

    void OutputRF()
    {
      ofstream rfout;
      rfout.open("RFresult.txt",std::ios_base::app);
      if (rfout.is_open())
      {
        rfout<<"A state of RF:"<<endl;
        for (int j = 0; j<32; j++)
        {        
          rfout << Registers[j]<<endl;
        }

      }
      else cout<<"Unable to open file";
      rfout.close();

    }     
  private:
    vector<bitset<32> >Registers;
};


class ALU
{
  public:
    bitset<32> ALUresult;
    bitset<32> ALUOperation (bitset<3> ALUOP, bitset<32> oprand1, bitset<32> oprand2)
    {   
      //reset before new operators
      ALUresult.reset();
      if (ALUOP == 1) {
        unsigned long o1 = oprand1.to_ulong();
        unsigned long o2 = oprand2.to_ulong();
        unsigned long result = o1 + o2;
        ALUresult = bitset<32>(result);
      } else if (ALUOP == 3) {
        unsigned long o1 = oprand1.to_ulong();
        unsigned long o2 = oprand2.to_ulong();
        unsigned long result = o1 - o2;
        ALUresult = bitset<32>(result);
      } else if (ALUOP == 4) {       
        ALUresult = oprand1 & oprand2;
      } else if (ALUOP == 5) {
        ALUresult = oprand1 | oprand2;
      }  else if  (ALUOP == 7) {
        ALUresult = ~(oprand1 | oprand2);    
      } else {
        cout << "Warning: Invalid opcode" << ALUOP.to_ulong() <<"\n";
      }
      return ALUresult;
    }            
};


class INSMem
{
  public:
    bitset<32> Instruction;
    INSMem()
    {       IMem.resize(MemSize); 
      ifstream imem;
      string line;
      int i=0;
      imem.open("imem.txt");
      if (imem.is_open())
      {
        while (getline(imem,line))
        {      
          IMem[i] = bitset<8>(line);
          i++;
        }

      }
      else cout<<"Unable to open file";
      imem.close();

    }

    bitset<32> ReadMemory (bitset<32> ReadAddress) 
    {    
      // TODO: implement!
      /**
       * @brief Read Instruction Memory (IMem).
       *
       * Read the byte at the ReadAddress and the following three byte,
       * and return the read result. 
       */
      //convert to integer
      unsigned long address = ReadAddress.to_ulong();
      //find vector indexes of 4 bytes
      unsigned long index1 = address * 4;
      unsigned long index2 = (address + 1) * 4 - 1;
      //slice from index1 all the way through index2
      vector<bitset<8>> InstructionSlice(IMem.begin() + index1, IMem.begin() + index2 + 1);

      //print statements for testing
      cout << index1 << "\n";
      cout << index2 << "\n";
      for (const auto& byte : InstructionSlice) {
      std::cout << byte << "\n";  // Outputs: 00000000, 10101010, etc.
      }

      unsigned long IntInstruction = (InstructionSlice[0].to_ulong() << 24) | (InstructionSlice[1].to_ulong() << 16) | (InstructionSlice[2].to_ulong() << 8) | (InstructionSlice[3].to_ulong());

      bitset<32>Instruction(IntInstruction);

      //printing to test
      cout << Instruction << "\n"; 


      return Instruction;     
    }     

  private:
    vector<bitset<8> > IMem;

};

class DataMem    
{
  public:
    bitset<32> readdata;  
    DataMem()
    {
      DMem.resize(MemSize); 
      ifstream dmem;
      string line;
      int i=0;
      dmem.open("dmem.txt");
      if (dmem.is_open())
      {
        while (getline(dmem,line))
        {      
          DMem[i] = bitset<8>(line);
          i++;
        }
      }
      else cout<<"Unable to open file";
      dmem.close();

    }  
    bitset<32> MemoryAccess (bitset<32> Address, bitset<32> WriteData, bitset<1> readmem, bitset<1> writemem) 
    {    
      /**
       * @brief Reads/writes data from/to the Data Memory.
       *
       * This function is used to read/write data from/to the DataMem, depending on the readmem and writemem.
       * First, if writemem enabled, WriteData should be written to DMem, clear or ignore the return value readdata,
       * and note that 32-bit WriteData will occupy 4 continious Bytes in DMem. 
       * If readmem enabled, return the DMem read result as readdata.
       */
      // TODO: implement!
      return readdata;     
    }   

    void OutputDataMem()
    {
      ofstream dmemout;
      dmemout.open("dmemresult.txt");
      if (dmemout.is_open())
      {
        for (int j = 0; j< 1000; j++)
        {     
          dmemout << DMem[j]<<endl;
        }

      }
      else cout<<"Unable to open file";
      dmemout.close();

    }             

  private:
    vector<bitset<8> > DMem;

};  



int main()
{
  RF myRF;
  ALU myALU;
  INSMem myInsMem;
  DataMem myDataMem;
  bitset<32> PC(0); // 32-bit program counter, initialized to 0

  while (1)  // TODO: implement!
  {
    bitset<32> a(0);
    bitset<32> b(1);
    myInsMem.ReadMemory(a);
    myInsMem.ReadMemory(b);

    break;
    // Fetch: fetch an instruction from myInsMem.

    // If current instruction is "11111111111111111111111111111111", then break; (exit the while loop)

    // decode(Read RF): get opcode and other signals from instruction, decode instruction

    // Execute: after decoding, ALU may run and return result

    // Read/Write Mem: access data memory (myDataMem)

    // Write back to RF: some operations may write things to RF

    // Update PC


    /**** You don't need to modify the following lines. ****/

    myRF.OutputRF(); // dump RF;    
  }
  myDataMem.OutputDataMem(); // dump data mem

  return 0;
}
