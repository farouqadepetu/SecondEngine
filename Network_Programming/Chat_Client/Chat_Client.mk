##
## Auto Generated makefile by CodeLite IDE
## any manual changes will be erased      
##
## Debug
ProjectName            :=Chat_Client
ConfigurationName      :=Debug
WorkspaceConfiguration :=Debug
WorkspacePath          :=/home/farouq/Documents/SecondEngine/Network_Programming
ProjectPath            :=/home/farouq/Documents/SecondEngine/Network_Programming/Chat_Client
IntermediateDirectory  :=../build-$(WorkspaceConfiguration)/Chat_Client
OutDir                 :=$(IntermediateDirectory)
CurrentFileName        :=
CurrentFilePath        :=
CurrentFileFullPath    :=
User                   :=farouq
Date                   :=22/08/25
CodeLitePath           :=/home/farouq/.codelite
MakeDirCommand         :=mkdir -p
LinkerName             :=/usr/bin/g++-13
SharedObjectLinkerName :=/usr/bin/g++-13 -shared -fPIC
ObjectSuffix           :=.o
DependSuffix           :=.o.d
PreprocessSuffix       :=.i
DebugSwitch            :=-g 
IncludeSwitch          :=-I
LibrarySwitch          :=-l
OutputSwitch           :=-o 
LibraryPathSwitch      :=-L
PreprocessorSwitch     :=-D
SourceSwitch           :=-c 
OutputDirectory        :=/home/farouq/Documents/SecondEngine/Network_Programming/build-$(WorkspaceConfiguration)/bin
OutputFile             :=../build-$(WorkspaceConfiguration)/bin/$(ProjectName)
Preprocessors          :=
ObjectSwitch           :=-o 
ArchiveOutputSwitch    := 
PreprocessOnlySwitch   :=-E
ObjectsFileList        :=$(IntermediateDirectory)/ObjectsList.txt
PCHCompileFlags        :=
LinkOptions            :=  
IncludePath            :=  $(IncludeSwitch). $(IncludeSwitch). 
IncludePCH             := 
RcIncludePath          := 
Libs                   := 
ArLibs                 :=  
LibPath                := $(LibraryPathSwitch). 

##
## Common variables
## AR, CXX, CC, AS, CXXFLAGS and CFLAGS can be overridden using an environment variable
##
AR       := /usr/bin/ar rcu
CXX      := /usr/bin/g++-13
CC       := /usr/bin/gcc-13
CXXFLAGS :=  -gdwarf-2 -O0 -Wall $(Preprocessors)
CFLAGS   :=  -gdwarf-2 -O0 -Wall $(Preprocessors)
ASFLAGS  := 
AS       := /usr/bin/as


##
## User defined environment variables
##
CodeLiteDir:=/usr/share/codelite
Objects0=$(IntermediateDirectory)/up_Chat_ChatString.cpp$(ObjectSuffix) $(IntermediateDirectory)/main.cpp$(ObjectSuffix) $(IntermediateDirectory)/up_Chat_ChatPacket.cpp$(ObjectSuffix) $(IntermediateDirectory)/up_Network_Network.cpp$(ObjectSuffix) $(IntermediateDirectory)/up_Thread_Thread.cpp$(ObjectSuffix) 



Objects=$(Objects0) 

##
## Main Build Targets 
##
.PHONY: all clean PreBuild PrePreBuild PostBuild MakeIntermediateDirs
all: MakeIntermediateDirs $(OutputFile)

$(OutputFile): $(IntermediateDirectory)/.d $(Objects) 
	@$(MakeDirCommand) "$(IntermediateDirectory)"
	@echo "" > $(IntermediateDirectory)/.d
	@echo $(Objects0)  > $(ObjectsFileList)
	$(LinkerName) $(OutputSwitch)$(OutputFile) @$(ObjectsFileList) $(LibPath) $(Libs) $(LinkOptions)

MakeIntermediateDirs:
	@$(MakeDirCommand) "$(IntermediateDirectory)"
	@$(MakeDirCommand) "$(OutputDirectory)"

$(IntermediateDirectory)/.d:
	@$(MakeDirCommand) "$(IntermediateDirectory)"

PreBuild:


##
## Objects
##
$(IntermediateDirectory)/up_Chat_ChatString.cpp$(ObjectSuffix): ../Chat/ChatString.cpp $(IntermediateDirectory)/up_Chat_ChatString.cpp$(DependSuffix)
	$(CXX) $(IncludePCH) $(SourceSwitch) "/home/farouq/Documents/SecondEngine/Network_Programming/Chat/ChatString.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/up_Chat_ChatString.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/up_Chat_ChatString.cpp$(DependSuffix): ../Chat/ChatString.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/up_Chat_ChatString.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/up_Chat_ChatString.cpp$(DependSuffix) -MM ../Chat/ChatString.cpp

$(IntermediateDirectory)/up_Chat_ChatString.cpp$(PreprocessSuffix): ../Chat/ChatString.cpp
	$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/up_Chat_ChatString.cpp$(PreprocessSuffix) ../Chat/ChatString.cpp

$(IntermediateDirectory)/main.cpp$(ObjectSuffix): main.cpp $(IntermediateDirectory)/main.cpp$(DependSuffix)
	$(CXX) $(IncludePCH) $(SourceSwitch) "/home/farouq/Documents/SecondEngine/Network_Programming/Chat_Client/main.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/main.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/main.cpp$(DependSuffix): main.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/main.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/main.cpp$(DependSuffix) -MM main.cpp

$(IntermediateDirectory)/main.cpp$(PreprocessSuffix): main.cpp
	$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/main.cpp$(PreprocessSuffix) main.cpp

$(IntermediateDirectory)/up_Chat_ChatPacket.cpp$(ObjectSuffix): ../Chat/ChatPacket.cpp $(IntermediateDirectory)/up_Chat_ChatPacket.cpp$(DependSuffix)
	$(CXX) $(IncludePCH) $(SourceSwitch) "/home/farouq/Documents/SecondEngine/Network_Programming/Chat/ChatPacket.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/up_Chat_ChatPacket.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/up_Chat_ChatPacket.cpp$(DependSuffix): ../Chat/ChatPacket.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/up_Chat_ChatPacket.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/up_Chat_ChatPacket.cpp$(DependSuffix) -MM ../Chat/ChatPacket.cpp

$(IntermediateDirectory)/up_Chat_ChatPacket.cpp$(PreprocessSuffix): ../Chat/ChatPacket.cpp
	$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/up_Chat_ChatPacket.cpp$(PreprocessSuffix) ../Chat/ChatPacket.cpp

$(IntermediateDirectory)/up_Network_Network.cpp$(ObjectSuffix): ../Network/Network.cpp $(IntermediateDirectory)/up_Network_Network.cpp$(DependSuffix)
	$(CXX) $(IncludePCH) $(SourceSwitch) "/home/farouq/Documents/SecondEngine/Network_Programming/Network/Network.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/up_Network_Network.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/up_Network_Network.cpp$(DependSuffix): ../Network/Network.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/up_Network_Network.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/up_Network_Network.cpp$(DependSuffix) -MM ../Network/Network.cpp

$(IntermediateDirectory)/up_Network_Network.cpp$(PreprocessSuffix): ../Network/Network.cpp
	$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/up_Network_Network.cpp$(PreprocessSuffix) ../Network/Network.cpp

$(IntermediateDirectory)/up_Thread_Thread.cpp$(ObjectSuffix): ../Thread/Thread.cpp $(IntermediateDirectory)/up_Thread_Thread.cpp$(DependSuffix)
	$(CXX) $(IncludePCH) $(SourceSwitch) "/home/farouq/Documents/SecondEngine/Network_Programming/Thread/Thread.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/up_Thread_Thread.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/up_Thread_Thread.cpp$(DependSuffix): ../Thread/Thread.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/up_Thread_Thread.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/up_Thread_Thread.cpp$(DependSuffix) -MM ../Thread/Thread.cpp

$(IntermediateDirectory)/up_Thread_Thread.cpp$(PreprocessSuffix): ../Thread/Thread.cpp
	$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/up_Thread_Thread.cpp$(PreprocessSuffix) ../Thread/Thread.cpp


-include $(IntermediateDirectory)/*$(DependSuffix)
##
## Clean
##
clean:
	$(RM) -r $(IntermediateDirectory)


