/*
    
    Copyright (c) 2026 Aleksandr Sokolov

    This file is part of the cm_dumper project, which is licensed under a custom license.
    Please see the LICENSE file for details.
    
    If you have any questions, please contact me at one of the folowing;
		Telegram: @Konekomimi
        E-Mail: mashiro@airmail.cc

*/

#include "pch.h"
#include "il2cpp_resolver/IL2CPP_Resolver.hpp"
#include <Windows.h>
//#include <stdio.h> // not needed
#include <fstream>
#include <iomanip>
#include <vector>
#include <filesystem>
#include <shellapi.h>
#define IL2CPP_MAIN_MODULE "project.dll"
using namespace IL2CPP;


typedef struct Il2CppType Il2CppType; 
void i_obfExports() {HMODULE gameAssembly = GetModuleHandleA("project.dll"); // !! DO NOT EDIT THESE UNLESS YOU KNOW WHAT YOU ARE DOING !!
    if (gameAssembly == NULL) {MessageBoxA(0,"project.dll not loaded","error", MB_ICONERROR);return;}
    //core
    IL2CPP::Functions.m_DomainGet=GetProcAddress(gameAssembly,"ApzldXylDWR");
    IL2CPP::Functions.m_DomainGetAssemblies=GetProcAddress(gameAssembly,"gobBtMyApTj");
    //class
    IL2CPP::Functions.m_ClassFromName=GetProcAddress(gameAssembly,"etIgYqMGBTj");
    IL2CPP::Functions.m_ClassFromIl2cppType=GetProcAddress(gameAssembly,"nAwDcisvKeb");
    IL2CPP::Functions.m_ClassGetMethods=GetProcAddress(gameAssembly,"wwlPDayBOUV");
    IL2CPP::Functions.m_ClassGetMethodFromName=GetProcAddress(gameAssembly,"BfmNBPOFdWH");
    IL2CPP::Functions.m_ClassGetFields=GetProcAddress(gameAssembly,"dvnHzpFiKhe");
    IL2CPP::Functions.m_ClassGetFieldFromName=GetProcAddress(gameAssembly,"bljLN_VuatI");
    IL2CPP::Functions.m_ClassGetType=GetProcAddress(gameAssembly,"rBBtHuCQjPL");
    IL2CPP::Functions.m_ClassGetPropertyFromName=GetProcAddress(gameAssembly,"YCfHljSKetv");
    //image
    IL2CPP::Functions.m_ImageGetClass=GetProcAddress(gameAssembly,"sEfnFqjmWJN");
    IL2CPP::Functions.m_ImageGetClassCount=GetProcAddress(gameAssembly,"eTLADjuMyXv");
    //type
    IL2CPP::Functions.m_TypeGetName=GetProcAddress(gameAssembly,"ySfCbKcrsLP");
    IL2CPP::Functions.m_TypeGetObject=GetProcAddress(gameAssembly,"OGdxlYqyamx");
    //string/memory
    IL2CPP::Functions.m_StringNew=GetProcAddress(gameAssembly,"HVsgcDrnuqH");
    IL2CPP::Functions.m_Free=GetProcAddress(gameAssembly,"pWXIggAgkKK");
    //thread
    IL2CPP::Functions.m_ThreadAttach=GetProcAddress(gameAssembly,"ZxbKYsbhTwf");
    IL2CPP::Functions.m_ThreadDetach=GetProcAddress(gameAssembly,"OoiRpAhKmoS");
    //properties
    IL2CPP::Functions.m_ClassGetProperties=GetProcAddress(gameAssembly,"siluMEtjbbJ");
    //misc.
    IL2CPP::Functions.m_ResolveFunction=GetProcAddress(gameAssembly,"BvOkqJhjjkb");
    IL2CPP::Functions.m_MethodGetParamName=GetProcAddress(gameAssembly,"PfpABMJMPMv");
    //il2cpp_init
    IL2CPP::Globals.m_GameAssembly = gameAssembly;
    auto il2cpp_init=GetProcAddress(gameAssembly,"emXPmLUuoPR");if(il2cpp_init){((void(*)())il2cpp_init)();}}

//helpers (minified)
std::string CleanTypeName(const std::string&raw){if(raw=="System.Single")return"float";if(raw=="System.Int32")return"int";if(raw=="System.UInt32")return"unsigned int";if(raw=="System.Int64")return"int64_t";if(raw=="System.Boolean")return"bool";if(raw=="System.Double")return"double";if(raw=="System.String")return"System::String*";if(raw=="System.Void")return"void";if (raw.find("UnityEngine.Vector3")!=std::string::npos)return"Vector3";if(raw.find("UnityEngine.Quaternion")!=std::string::npos)return"Quaternion";std::string result=raw;size_t pos;while ((pos = result.find("System."))!=std::string::npos)result.replace(pos,7,"");if(result.back()=='&')result.pop_back();return result+"*";}
std::string GetMethodReturnType(Unity::il2cppMethodInfo* method){if(!method)return"void";const Il2CppType*type=nullptr;if(method->m_pReturnType)type=(const Il2CppType*)method->m_pReturnType;if(!type)return"void";const char*typeName=((const char*(*)(const Il2CppType*))IL2CPP::Functions.m_TypeGetName)(type);if(!typeName)return"void";return CleanTypeName(typeName);}
std::string GetFieldTypeName(Unity::il2cppFieldInfo*field){if(!field)return"void*";const Il2CppType*type=*(const Il2CppType**)((uintptr_t)field+0x8);if(!type)return"void*";const char*typeName=((const char*(*)(const Il2CppType*))IL2CPP::Functions.m_TypeGetName)(type);if(!typeName)return"unknown";return CleanTypeName(typeName);}
std::string GetRVA(uintptr_t address){if(!address)return"0x0";uintptr_t baseAddress=(uintptr_t)IL2CPP::Globals.m_GameAssembly;uintptr_t rva=address-baseAddress;std::stringstream ss;ss<<"0x"<<std::hex<<rva;return ss.str();}

void d_sdk(const std::string& o_dir) {std::filesystem::create_directories(o_dir); //start dump function
    size_t assemblyCount = 0;auto assemblies = IL2CPP::Domain::GetAssemblies(&assemblyCount);
    for (size_t i = 0; i < assemblyCount; ++i) {
        auto*assembly=assemblies[i];
        if(!assembly||!assembly->m_pImage){continue;}
        std::string i_name = assembly->m_pImage->m_pNameNoExt ? assembly->m_pImage->m_pNameNoExt : "unknown";
        std::ofstream file(o_dir + "/" + i_name + ".hpp");
        if (!file.is_open()){continue;}
        file << "// CombatMaster SDK Dumper by t.me/Konekomimi\n";
        file << "#pragma once\n\n";
        file << "namespace " << i_name << "\n{\n";
        std::vector<Unity::il2cppClass*> classes;
        IL2CPP::Class::FetchClasses(&classes, i_name.c_str(), nullptr);
        for (auto* klass : classes) {
            if (!klass) continue;
            std::string className = klass->m_pName ? klass->m_pName : "unknown";
            std::string ns = klass->m_pNamespace ? klass->m_pNamespace : "";
            file << "\n    // " << (ns.empty() ? "" : ns + "::") << className << " - class ptr: 0x" << std::hex 
            << (uintptr_t)klass << " - gameassembly = 0x" << std::hex << (uintptr_t)IL2CPP::Globals.m_GameAssembly << "\n";
            file << "    class " << className << "\n    {\n";
            //fields
            std::vector<Unity::il2cppFieldInfo*> fields;
            IL2CPP::Class::FetchFields(klass, &fields);
            for (auto* field : fields) {
                if (!field) continue;std::string fieldName = field->m_pName ? field->m_pName : "unknown";uintptr_t offset = field->m_iOffset;
				std::string typeStr = GetFieldTypeName(field);file << "        " << typeStr << " " << fieldName
                    << "; // 0x" << std::hex << offset << std::dec << "\n";}
            //properties
            std::vector<Unity::il2cppPropertyInfo*> properties;
            IL2CPP::Class::FetchProperties(klass, &properties);
            for (auto* prop : properties) {
                if (!prop) continue;
                std::string propName = prop->m_pName ? prop->m_pName : "unknown";
                file << "        // PROPERTY: " << propName << "\n";
                auto is_notBinary = [](const char* s) -> bool {
                    if (!s || (uintptr_t)s < 0x10000) return false;
                    size_t len = 0;
                    for (const char* p = s; *p; ++p, ++len) { if (len > 128) return false;
                        unsigned char c = (unsigned char)*p; if (c < 0x20 || c > 0x7E)/* fix range */ return false; }
                    return len > 0;};
                if (prop->m_pGet) {const char* gname = (is_notBinary(prop->m_pGet->m_pName)) ? prop->m_pGet->m_pName : "getter";
                    file << "        //   GET  0x" << std::hex << (uintptr_t)prop->m_pGet->m_pMethodPointer
                        << std::dec << " " << gname << "()\n";}if (prop->m_pSet) {
                    const char* sname = (is_notBinary(prop->m_pSet->m_pName)) ? prop->m_pSet->m_pName : "setter";
                    file << "        //   SET  0x" << std::hex << (uintptr_t)prop->m_pSet->m_pMethodPointer
                        << std::dec << " " << sname << "()\n";}}
            //methods
            std::vector<Unity::il2cppMethodInfo*> methods;
            IL2CPP::Class::FetchMethods(klass, &methods);
            for (auto* method : methods) {
                if (!method) continue; bool isAccessor = false;
                for (auto* prop : properties) {
                    if ((prop->m_pGet && prop->m_pGet == method) || (prop->m_pSet && prop->m_pSet == method)) 
                    { isAccessor = true; break; } }
                if (isAccessor) continue; auto is_notBinary = [](const char* s) -> bool {
                    if (!s || (uintptr_t)s < 0x10000) return false;size_t len = 0;
                    for (const char* p = s; *p; ++p, ++len) { if (len > 128) /* 128 as upper limit, since I don't have proper functions */ return false;
                        unsigned char c = (unsigned char)*p; if (c < 0x20 || c > 0x7E) /* fix range */ return false; } return len > 0; };
                const char* mname = (is_notBinary(method->m_pName)) ? method->m_pName : "unknown";
                if (strcmp(mname, "unknown") == 0 || (method->m_pName && method->m_pName[0] == '<'))
                    continue;
                file << "        // 0x" << std::hex << (uintptr_t)method->m_pMethodPointer
                    << std::dec << " " << mname << "()\n";
            } file << "    };\n";
        } file << "}\n";
        file.close();}ShellExecuteA(NULL, "open", "C:\\cm_sdk", NULL, NULL, SW_SHOWDEFAULT);}

DWORD WINAPI MainThread(LPVOID) {AllocConsole();
    FILE* fp;
    freopen_s(&fp, "CONOUT$", "w", stdout);
    SetConsoleTitleA("cm_sdk - t.me/Konekomimi");
    printf("I love kittens!!!\n");
    i_obfExports();
    while (true) { size_t count = 0; if (auto assemblies = IL2CPP::Domain::GetAssemblies(&count)) { if (count > 0)break; }Sleep(100);}
    d_sdk("C:\\cm_sdk");
    return 0;}

//dllmain (minified)
BOOL APIENTRY DllMain(HMODULE hModule, DWORD reason, LPVOID) { if (reason == DLL_PROCESS_ATTACH) { DisableThreadLibraryCalls(hModule);CreateThread(nullptr,0,MainThread,nullptr,0,nullptr);};return TRUE;}
