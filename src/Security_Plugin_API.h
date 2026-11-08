#pragma once
#include <string>
// Match the host C++ compiler/STL ABI, as with the other Appeal plugin SDKs.
#ifndef APPEAL_SECURITY_BRIDGE
extern "C" {
void Constructor();
void Destructor();
void Debug(bool Enabled);
void Set_Paths(std::string Resources);
std::string Generate_Key();
std::string Encrypt(std::string Plaintext,std::string Key);
std::string Decrypt(std::string Ciphertext,std::string Key);
std::string Encrypt_With_Password(std::string Plaintext,std::string Password);
std::string Decrypt_With_Password(std::string Ciphertext,std::string Password);
bool Encrypt_File(std::string Source,std::string Destination,std::string Key);
bool Decrypt_File(std::string Source,std::string Destination,std::string Key);
bool Encrypt_File_With_Password(std::string Source,std::string Destination,std::string Password);
bool Decrypt_File_With_Password(std::string Source,std::string Destination,std::string Password);
std::string Last_Error();
std::string Backend_Version();
}
#endif
