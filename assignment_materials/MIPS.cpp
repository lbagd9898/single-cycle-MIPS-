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

      //r0 needs to always be 0 
      /**
       * @brief Reads or writes data from/to the Register.
       *
       * This function is used to read or write data from/to the register, depending on the value of WrtEnable.
       * Put the read results to the ReadData1 and ReadData2.
       */
      // TODO: implement!
      ReadData1 = Registers[RdReg1.to_ulong()];
      ReadData2 = Registers[RdReg2.to_ulong()];
      
      if(WrtReg.to_ulong() != 0 && WrtEnable.test(0)) {
        Registers[WrtReg.to_ulong()] = WrtData;
      }
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
      unsigned long index1 = address;
      unsigned long index2 = address + 3;
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
      //checks if it is mod 4 by checking if last two values are 0s
      if ( Address[0] || Address[1]) {
        cout << "invalid address" << "\n";
        return 0;
      }  
      unsigned long add1 = Address.to_ulong();
      unsigned long add2 = add1 + 3;
      //if readmem is true
      if (readmem[0]) {
        vector<bitset<8>> addSlice(DMem.begin() + add1, DMem.begin() + add2 + 1);

        //print for verification
        cout << add1 << "\n";
        cout << add2 << "\n";
        for (const auto& byte : addSlice) {
        std::cout << byte << "\n"; 

         // Outputs: 00000000, 10101010, etc.
        }

        unsigned long IntDataMem = (addSlice[0].to_ulong() << 24) | (addSlice[1].to_ulong() << 16) | (addSlice[2].to_ulong() << 8) | (addSlice[3].to_ulong());

        cout << IntDataMem << "\n"; 
        bitset<32>readdata(IntDataMem);

        cout << readdata << "\n";

      }
      if (writemem[0]) {
        int shift = 24;
        

        for (int i = add1; i <= add2; i++) {
            // Explicitly convert shifted bitset to 8 bits
            DMem[i] = std::bitset<8>((WriteData >> shift).to_ulong());
            shift -= 8;
        }

        // --- PRINT DMEM TO VERIFY UPDATES ---
        std::cout << "--- Updated DMem ---" << std::endl;
        for (size_t i = 0; i < 16; i++) {
            std::cout << "DMem[" << i << "]: " << DMem[i] << std::endl;
        }
        std::cout << "--------------------" << std::endl;


        return 1;
    }
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

    // bitset<32> Address(4);
    // bitset<32> writeData(2870129782UL);
    // bitset<1> writeMem(1);
    // bitset<1> readMem(0);
    
    // cout << "--- Before MemoryAccess ---" << endl;
    // cout << "writeData (Binary) : " << writeData << endl;
    // cout << "writeData (Decimal): " << writeData.to_ulong() << endl;
    // cout << "---------------------------" << endl;

    // myDataMem.MemoryAccess(Address, writeData, readMem, writeMem);


    break;
    // Fetch: fetch an instruction from myInsMem.
    bitset<32> fetch_ins = myInsMem.ReadMemory(PC);

    // If current instruction is "11111111111111111111111111111111", then break; (exit the while loop)
    //is this going to work - if it is a string
    if(fetch_ins == bitset<32>(string(32, '1'))) {
      break;
    };
    unsigned long int_ins = fetch_ins.to_ulong();
    bitset<6> opcode(int_ins >> 26);

    if (opcode == 0) {
      
    } else if (opcode == 2 || opcode == 3) {
      //j instruction
      } else {
      bitset<16> imm(int_ins & 0xFFFF);
      bitset<5> rt((int_ins >> 16) & 0x1F);
      bitset<5> rs((int_ins >> 21) & 0x1F);
      //functionality for beq
      if ( opcode == 4) {
        unsigned long newPC;
        myRF.ReadWrite(rs, rt, bitset<5>(0), bitset<32>(0), bitset<1>(0));
        bitset<32> rs_data(myRF.ReadData1);
        bitset<32> rt_data(myRF.ReadData2);
        if (rs_data == rt_data) {
          bool bit15 = imm[15];
          bitset<32> signExtendedImm(0);
          if (bit15) {
            bitset<16> ones("1111111111111111");
            signExtendedImm = ((ones.to_ulong() << 16) | imm.to_ulong()); 
          } else {
            bitset<16> zeros(0);
            signExtendedImm = ((zeros.to_ulong() << 16) | imm.to_ulong());
          };
          newPC = PC.to_ulong() + 4 + (signExtendedImm.to_ulong() << 2);
        } else {
          newPC = PC.to_ulong() + 4;
        };
        PC = bitset<32>(newPC);
        continue;
        //next is opcode for addiu
      } else if (opcode == 9) {
        myRF.ReadWrite(rs, bitset<5>(0), bitset<5>(0), bitset<32>(0), bitset<1>(0));
        bitset<32> rs_data(myRF.ReadData1);
        //sign extension
        bool bit15 = imm[15];
        bitset<32> signExtendedImm(0);
        if (bit15) {
          bitset<16> ones("1111111111111111");
          signExtendedImm = ((ones.to_ulong() << 16) | imm.to_ulong()); 
        } else {
          bitset<16> zeros(0);
          signExtendedImm = ((zeros.to_ulong() << 16) | imm.to_ulong());
        };
        bitset<32> result(myALU.ALUOperation(bitset<3>(1), rs_data, signExtendedImm));
        myRF.ReadWrite(rs, bitset<5>(0), rt, result, bitset<1>(1));
        PC = bitset<32>(PC.to_ulong() + 4); 
        continue;
        //next is lw
      } else if (opcode == 35) {
        myRF.ReadWrite(rs, bitset<5>(0), bitset<5>(0), bitset<32>(0), bitset<1>(0));
        bitset<32> rs_data(myRF.ReadData1);
        //sign extension
        bool bit15 = imm[15];
        bitset<32> signExtendedImm(0);
        if (bit15) {
          bitset<16> ones("1111111111111111");
          signExtendedImm = ((ones.to_ulong() << 16) | imm.to_ulong()); 
        } else {
          bitset<16> zeros(0);
          signExtendedImm = ((zeros.to_ulong() << 16) | imm.to_ulong());
        };
        bitset<32> result(myALU.ALUOperation(bitset<3>(1), rs_data, signExtendedImm));
        bitset<32> memData = myDataMem.MemoryAccess(result, bitset<32>(0), bitset<1>(1), bitset<1>(0)); 
        myRF.ReadWrite(bitset<5>(0), bitset<5>(0), rt, memData, bitset<1>(1));
        PC = bitset<32>(PC.to_ulong() + 4); 
        continue;
        //next is sw
      } else if(opcode == 43) {
        myRF.ReadWrite(rs, rt, bitset<5>(0), bitset<32>(0), bitset<1>(0));
        bitset<32> rs_data(myRF.ReadData1);
        bitset<32> rt_data(myRF.ReadData2);
        bool bit15 = imm[15];
        bitset<32> signExtendedImm(0);
        if (bit15) {
          bitset<16> ones("1111111111111111");
          signExtendedImm = ((ones.to_ulong() << 16) | imm.to_ulong()); 
        } else {
          bitset<16> zeros(0);
          signExtendedImm = ((zeros.to_ulong() << 16) | imm.to_ulong());
        };
        bitset<32> result(myALU.ALUOperation(bitset<3>(1), rs_data, signExtendedImm));
        bitset<32> memData = myDataMem.MemoryAccess(result, bitset<32>(0), bitset<1>(1), bitset<1>(0));

      }


      }
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
