
#include <ApolloSM/ApolloSM.hh>
#include <ApolloSM/ApolloSM_Exceptions.hh>
#include <vector>
#include <string>

#include <iostream>
#include <chrono>
#include <random>

ApolloSM * SM = NULL;

int loop_back_test(std::string node, uint32_t loops);

int main(int argc, char** argv) { 
  if(argc < 2){
    printf("%s connection file\n",argv[0]);
    return 1;
  }

  //  ApolloSM * SM = NULL;

  try{
    // ==================================
    // Initialize ApolloSM
    std::vector<std::string> arg;
    arg.push_back(argv[1]);
    SM = new ApolloSM(arg);
    if(NULL == SM){
      printf("Failed to create new ApolloSM\n");
      exit(EXIT_FAILURE);
    }else{
      printf("Created new ApolloSM\n");      
    }

    uint32_t val = SM->ReadRegister("SM_INFO.BUILD_DATE");
    printf("0x%08X\n",val);

    //    SM->GenerateStatusDisplay(1,std::cout,std::string("CM"));

    std::cout << "Shelf ID = " << SM->GetShelfID() << std::endl;
    std::cout << "Serial Number = " << SM->GetSerialNumber() << std::endl;
    std::cout << "Revision Number = " << SM->GetRevNumber() << std::endl;
    std::cout << "Zynq IP = " << SM->GetZynqIP() << std::endl;
    std::cout << "IPMC IP = " << std::hex << SM->GetIPMCIP() << std::endl;
    uint32_t ip;
    ip = SM->GetZynqIP();
    printf("Zynq IP = %i.%i.%i.%i\n",
	   ip & 0xFF,
	   (ip >> 8) & 0xFF,
	   (ip >> 16) & 0xFF,
           (ip >> 24) & 0xFF);
 
    ip = SM->GetIPMCIP();    
    printf("IPMC IP = %i.%i.%i.%i\n",
           ip & 0xFF,
           (ip >> 8) & 0xFF,
           (ip >> 16) & 0xFF,
           (ip >> 24) & 0xFF);
  
    uint32_t pl_mem;
    SM->WriteRegister("PL_MEM.SCRATCH.WORD_00",0x0);
    pl_mem = SM->ReadRegister("PL_MEM.SCRATCH.WORD_00");
    printf("PL_MEM.SCRATCH.WORD_00 = 0x%08X\n",pl_mem);
    SM->WriteRegister("PL_MEM.SCRATCH.WORD_00",0xFF);
    pl_mem = SM->ReadRegister("PL_MEM.SCRATCH.WORD_00");
    printf("PL_MEM.SCRATCH.WORD_00 = 0x%08X\n",pl_mem);

    SM->unblockAXI();

    // Read out the CM S/N from the DS28CM00 device
    std::cout << std::endl << "Read out the DS28CM00 device on the MPI CM" << std::endl; 
    std::string cm_i2c_temp;
    std::string cm_sn; 
    // set starting address
    cm_i2c_temp = SM->UART_CMD("/dev/ttyUL1","i2c 4 0x50 0x4 0x00",'>');
    cm_i2c_temp = SM->UART_CMD("/dev/ttyUL1","i2c 4 0x50 0x3 1",'>');    
    std::cout << "Device Family Code = 0x" << cm_i2c_temp.substr(cm_i2c_temp.length()-3,2) << std::endl;
    cm_i2c_temp =  SM->UART_CMD("/dev/ttyUL1","i2c 4 0x50 0x3 1",'>');
    cm_sn = cm_i2c_temp.substr(cm_i2c_temp.length()-3,2) + cm_sn;
    cm_i2c_temp =  SM->UART_CMD("/dev/ttyUL1","i2c 4 0x50 0x3 1",'>');
    cm_sn = cm_i2c_temp.substr(cm_i2c_temp.length()-3,2) + cm_sn;
    cm_i2c_temp =  SM->UART_CMD("/dev/ttyUL1","i2c 4 0x50 0x3 1",'>');
    cm_sn = cm_i2c_temp.substr(cm_i2c_temp.length()-3,2) + cm_sn;
    cm_i2c_temp =  SM->UART_CMD("/dev/ttyUL1","i2c 4 0x50 0x3 1",'>');
    cm_sn = cm_i2c_temp.substr(cm_i2c_temp.length()-3,2) + cm_sn;
    cm_i2c_temp =  SM->UART_CMD("/dev/ttyUL1","i2c 4 0x50 0x3 1",'>');
    cm_sn = cm_i2c_temp.substr(cm_i2c_temp.length()-3,2) + cm_sn;
    cm_i2c_temp =  SM->UART_CMD("/dev/ttyUL1","i2c 4 0x50 0x3 1",'>');
    cm_sn = cm_i2c_temp.substr(cm_i2c_temp.length()-3,2) + cm_sn;
    std::cout << "CM addreess = 0x" << cm_sn << std::endl;
    cm_i2c_temp =  SM->UART_CMD("/dev/ttyUL1","i2c 4 0x50 0x3 1",'>');
    std::cout << "CRC of Family Code and 48-bit Serial Number = 0x" 
	      << cm_i2c_temp.substr(cm_i2c_temp.length()-3,2) << std::endl;

    uint32_t loops = 1000000;
    //    std::string node = "KINTEX_BRAM.MEM";
    std::string node = "PL_MEM.SCRATCH.WORD_00";
    loop_back_test(node,loops);

  }catch(BUException::exBase const & e){
    printf("Caught BUException: %s\n   Info: %s\n",e.what(),e.Description());          
  }catch(std::exception const & e){
    printf("Caught std::exception: %s\n",e.what());          
  }

  //Clean up
  if(NULL != SM) {
    delete SM;
  }
  
  return 0;
}

int loop_back_test(std::string node, uint32_t loops) {

  uint32_t write_mem;
  uint32_t read_mem;

  double speed;

  std::random_device rd;  //Will be used to obtain a seed for the random number engine
  std::mt19937 gen(rd()); //Standard mersenne_twister_engine seeded with rd()
  std::uniform_int_distribution<unsigned int> distrib(0, 0xFFFFFFFF);

  auto begin = std::chrono::high_resolution_clock::now();
  auto end = std::chrono::high_resolution_clock::now();
  auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end-begin).count();
    
  std::cout << std::endl << "Loop test of AXI C2C data integrity and speed" << std::endl 
	    << std::dec << loops << " loops doing write-read of random 32-bit words to " << node 
	    << std::endl << std::endl; 

  for(uint32_t i = 0; i < loops; ++i) {

    write_mem = distrib(gen);
    //    write_mem = 0xDEADBEEF;
    SM->WriteRegister(node,write_mem);
    read_mem = SM->ReadRegister(node);

    if (write_mem != read_mem) {
      std::cout << "R/W error: loop " << i << ", write_mem = " << std::hex << write_mem 
		<< ", read_mem = " << read_mem << std::endl << std::endl;
      return -1;
    }

    if (i < 10) {
      std::cout << "write_mem = " << std::hex << write_mem << ", read_mem = " << read_mem << std::endl;
    }
       
    if (i%100000 == 0 && i != 0) {
      end = std::chrono::high_resolution_clock::now();
      duration = std::chrono::duration_cast<std::chrono::microseconds>(end-begin).count();
      speed = 2.*32.*i/duration;
      std::cout << std::dec << i << " reads done, speed = " << speed <<  " Mbps" << std::endl;
    }

  }
   
  end = std::chrono::high_resolution_clock::now();
  duration = std::chrono::duration_cast<std::chrono::microseconds>(end-begin).count();

  std::cout << std::endl << "Speed test: " << std::dec << loops << " write-reads of " << node << std::endl;
  std::cout << duration << " us total, average : " << duration / loops << " us." << std::endl;

  speed = 2.*32.*loops/duration;
  std::cout << "Speed = " << speed << " Mbps" << std::endl;

  return 0;
}
