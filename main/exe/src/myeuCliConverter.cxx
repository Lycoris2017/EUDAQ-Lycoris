#include "eudaq/OptionParser.hh"
#include "eudaq/DataConverter.hh"
#include "eudaq/FileWriter.hh"
#include "eudaq/FileReader.hh"
#include <iostream>

int main(int /*argc*/, const char **argv) {
  eudaq::OptionParser op("EUDAQ Command Line DataConverter", "2.0", "The Data Converter launcher of EUDAQ");
  eudaq::Option<std::string> file_input(op, "i", "input", "", "string",
					"input file");
  eudaq::Option<std::string> file_output(op, "o", "output", "", "string",
					 "output file");
  eudaq::OptionFlag iprint(op, "ip", "iprint", "enable print of input Event");

  try{
    op.Parse(argv);
  }
  catch (...) {
    return op.HandleMainException();
  }
  
  std::string infile_path = file_input.Value();
  if(infile_path.empty()){
    std::cout<<"option --help to get help"<<std::endl;
    return 1;
  }
  
  std::string outfile_path = file_output.Value();
  std::string type_in = infile_path.substr(infile_path.find_last_of(".")+1);
  std::string type_out = outfile_path.substr(outfile_path.find_last_of(".")+1);
  bool print_ev_in = iprint.Value();

  std::cout<<"outfile_path = "<<outfile_path<<"\n"
	   <<"type_in = "<< type_in <<"\n"
    	   <<"type_out = "<< type_out <<"\n"
	   <<std::endl;
  
  if(type_in=="raw")
    type_in = "native";
  if(type_out=="raw")
    type_out = "native";
  
  eudaq::FileReaderUP reader;
  eudaq::FileWriterUP writer;
  
  std::cout<<"I am a debugger --1\n";
  reader = eudaq::Factory<eudaq::FileReader>::MakeUnique(eudaq::str2hash(type_in), infile_path);
  std::cout<<"I am a debugger --2\n";
  
  if(!type_out.empty()){

    std::cout<<"[output file not empty] @0@!"<<std::endl;
    writer = eudaq::Factory<eudaq::FileWriter>::MakeUnique(eudaq::str2hash(type_out), outfile_path);
    
  }

  int evtCounting = 0;
  while(1){

    /* 
     * ATTENTION! once empty evt in the middle, no evt to read after the EMPTY one!
     */

    auto ev = reader->GetNextEvent(); //--> when ev is empty, an error catched from throw;
    
    if(!ev) {
      /* 
       * NAIVE protection to check if any evt existed after the empty one TAT;
       */
      auto ev2 = reader->GetNextEvent();
      if (!ev2) break;
      else ev = ev2;
    }
    
    if(print_ev_in)
      ev->Print(std::cout);
    
    if(writer)
      writer->WriteEvent(ev);
    evtCounting++;    
  }
  
  std::cout<<"[Converter:info] # of Evt counting ==> "
	   << evtCounting <<"\n";
  return 0;
}
