#define APPEAL_SECURITY_BRIDGE
#include "Security_Plugin_API.h"
#include "Plugin.h"
#include "Data_Source.h"
#include "Module_Data.h"
#include "Named_List.h"
#include <stdexcept>
namespace {
Plugin Backend;
Named_List<Module_Data> *Globals=nullptr;
#define SECURITY_FUNCTIONS(X) \
 X(Debug,void,(bool)) X(Set_Paths,void,(std::string)) X(Generate_Key,std::string,()) \
 X(Encrypt,std::string,(std::string,std::string)) X(Decrypt,std::string,(std::string,std::string)) \
 X(Encrypt_With_Password,std::string,(std::string,std::string)) X(Decrypt_With_Password,std::string,(std::string,std::string)) \
 X(Encrypt_File,bool,(std::string,std::string,std::string)) X(Decrypt_File,bool,(std::string,std::string,std::string)) \
 X(Encrypt_File_With_Password,bool,(std::string,std::string,std::string)) X(Decrypt_File_With_Password,bool,(std::string,std::string,std::string)) \
 X(Last_Error,std::string,()) X(Backend_Version,std::string,())
#define DECLARE(Name,Type,Arguments) Type (*P_##Name) Arguments=nullptr;
SECURITY_FUNCTIONS(DECLARE)
#undef DECLARE
bool Loaded=false;
void Publish(std::string name,std::string type,std::string value){
    if(!Globals)return;
    Module_Data data{"Security",name,type,value};const auto key="Security."+name;
    if(Globals->Find(key))Globals->Current->Value=data;else Globals->Add_Node(key,data);
}
void Status(){const auto error=P_Last_Error();Publish("Success","bool",error.empty()?"1":"0");Publish("Last_Error","string",error);}
bool Boolean(const std::string &value){
    if(value=="1" || value=="True" || value=="true" || value=="On")return true;
    if(value=="0" || value=="False" || value=="false" || value=="Off")return false;
    throw std::runtime_error("Expected a boolean value");
}
}
extern "C" void Shutdown(){Backend.Unload();Loaded=false;Globals=nullptr;
#define CLEAR(Name,Type,Arguments) P_##Name=nullptr;
SECURITY_FUNCTIONS(CLEAR)
#undef CLEAR
}
extern "C" void Init_Globals(std::string file,Named_List<Module_Data> *globals){
    if(Loaded)Shutdown();
    Globals=globals;Backend.Load(file);
    try {
#define ASSIGN(Name,Type,Arguments) Backend.Assign(#Name,P_##Name);if(!P_##Name)throw std::runtime_error("Security plugin missing " #Name);
        SECURITY_FUNCTIONS(ASSIGN)
#undef ASSIGN
        Loaded=true;Publish("Success","bool","0");Publish("Last_Error","string","");Publish("Backend_Version","string","");
    }catch(...){Shutdown();throw;}
}
extern "C" void Init(std::string file){Init_Globals(file,nullptr);}
extern "C" void Interpreter(Data_Source *data){
    if(!Loaded)throw std::runtime_error("Security plugin is not loaded");
    std::string command;*data>>command;
    if(command=="Generate_Key"){auto result=P_Generate_Key();Status();data->Return_Value(std::move(result));}
    else if(command=="Encrypt" || command=="Decrypt" || command=="Encrypt_With_Password" || command=="Decrypt_With_Password"){
        std::string input,secret;*data>>input;*data>>secret;
        auto function=command=="Encrypt"?P_Encrypt:command=="Decrypt"?P_Decrypt:command=="Encrypt_With_Password"?P_Encrypt_With_Password:P_Decrypt_With_Password;
        auto result=function(std::move(input),std::move(secret));Status();data->Return_Value(std::move(result));
    }
    else if(command=="Encrypt_File" || command=="Decrypt_File" || command=="Encrypt_File_With_Password" || command=="Decrypt_File_With_Password"){
        std::string source,destination,secret;*data>>source;*data>>destination;*data>>secret;
        auto function=command=="Encrypt_File"?P_Encrypt_File:command=="Decrypt_File"?P_Decrypt_File:command=="Encrypt_File_With_Password"?P_Encrypt_File_With_Password:P_Decrypt_File_With_Password;
        bool success=function(std::move(source),std::move(destination),std::move(secret));Status();data->Return_Value(success?"1":"0");
    }
    else if(command=="Last_Error")data->Return_Value(P_Last_Error());
    else if(command=="Backend_Version"){
        const auto version=P_Backend_Version();Publish("Backend_Version","string",version);data->Return_Value(version);
    }
    else if(command=="Set_Paths"){std::string root;*data>>root;P_Set_Paths(std::move(root));Status();}
    else if(command=="Debug"){std::string enabled;*data>>enabled;P_Debug(Boolean(enabled));Status();}
    else throw std::runtime_error("Unknown Security command: "+command);
}
