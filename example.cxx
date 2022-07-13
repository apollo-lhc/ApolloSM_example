
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
