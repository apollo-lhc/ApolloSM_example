
#include <ApolloSM/ApolloSM.hh>
#include <ApolloSM/ApolloSM_Exceptions.hh>
#include <vector>
#include <string>

#include <iostream>



int main(int argc, char** argv) { 
  if(argc < 2){
    printf("%s connection file\n",argv[0]);
    return 1;
  }

  ApolloSM * SM = NULL;

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
    //           (ip >> 16) & 0xFF,
    //           (ip >> 8) & 0xFF,
    //           ip & 0xFF);
 
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
    uint32_t k_mem;
    SM->WriteRegister("K_C2C_INTF.C2C_1.DRP.ES_SDATA_MASK1",0x0);
    k_mem = SM->ReadRegister("K_C2C_INTF.C2C_1.DRP.ES_SDATA_MASK1");
    printf("K_C2C_INTF.C2C_1.DRP.ES_SDATA_MASK1 = 0x%08X\n",k_mem);
    SM->WriteRegister("K_C2C_INTF.C2C_1.DRP.ES_SDATA_MASK1",0xFF);
    k_mem = SM->ReadRegister("K_C2C_INTF.C2C_1.DRP.ES_SDATA_MASK1");
    printf("K_C2C_INTF.C2C_1.DRP.ES_SDATA_MASK1 = 0x%08X\n",k_mem);

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

    /*
    SM->GenerateStatusDisplay(1,std::cout,std::string("CM"));
    std::cout << "Turning on CM_1 power" << std::endl;
    SM->PowerUpCM(1,10);
    SM->GenerateStatusDisplay(1,std::cout,std::string("CM"));
    SM->GenerateStatusDisplay(3,std::cout,std::string("C2C"));

    std::cout << "Turning off CM_1 power" << std::endl;
    SM->PowerDownCM(1,10);
    SM->GenerateStatusDisplay(1,std::cout,std::string("CM"));
    */

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
