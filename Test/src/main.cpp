#include "../../src/Security_Plugin_API.h"
#include <iostream>
// This is deliberately not cryptography. Secrets are redacted even in diagnostic mode.
extern "C" void Constructor(){std::cout<<"Constructor\n";}
extern "C" void Destructor(){std::cout<<"Destructor\n";}
extern "C" void Debug(bool enabled){std::cout<<"Debug "<<enabled<<'\n';}
extern "C" void Set_Paths(std::string root){std::cout<<"Set_Paths "<<root<<'\n';}
extern "C" std::string Generate_Key(){std::cout<<"Generate_Key\n";return "TEST_KEY_NOT_FOR_SECURITY";}
extern "C" std::string Encrypt(std::string,std::string){std::cout<<"Encrypt <redacted> <redacted>\n";return "TEST_CIPHERTEXT_NOT_ENCRYPTED";}
extern "C" std::string Decrypt(std::string,std::string){std::cout<<"Decrypt <redacted> <redacted>\n";return "TEST_PLAINTEXT";}
extern "C" std::string Encrypt_With_Password(std::string,std::string){std::cout<<"Encrypt_With_Password <redacted> <redacted>\n";return "TEST_CIPHERTEXT_NOT_ENCRYPTED";}
extern "C" std::string Decrypt_With_Password(std::string,std::string){std::cout<<"Decrypt_With_Password <redacted> <redacted>\n";return "TEST_PLAINTEXT";}
extern "C" bool Encrypt_File(std::string source,std::string destination,std::string){std::cout<<"Encrypt_File "<<source<<' '<<destination<<" <redacted>\n";return true;}
extern "C" bool Decrypt_File(std::string source,std::string destination,std::string){std::cout<<"Decrypt_File "<<source<<' '<<destination<<" <redacted>\n";return true;}
extern "C" bool Encrypt_File_With_Password(std::string source,std::string destination,std::string){std::cout<<"Encrypt_File_With_Password "<<source<<' '<<destination<<" <redacted>\n";return true;}
extern "C" bool Decrypt_File_With_Password(std::string source,std::string destination,std::string){std::cout<<"Decrypt_File_With_Password "<<source<<' '<<destination<<" <redacted>\n";return true;}
extern "C" std::string Last_Error(){std::cout<<"Last_Error\n";return "";}
extern "C" std::string Backend_Version(){std::cout<<"Backend_Version\n";return "Test (no cryptography)";}
