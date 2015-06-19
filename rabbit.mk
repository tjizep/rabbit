##
## Auto Generated makefile by CodeLite IDE
## any manual changes will be erased      
##
## Release
ProjectName            :=rabbit
ConfigurationName      :=Release
WorkspacePath          := "C:\dev\cpp_all\rabbit"
ProjectPath            := "C:\dev\cpp_all\rabbit"
IntermediateDirectory  :=./Release
OutDir                 := $(IntermediateDirectory)
CurrentFileName        :=
CurrentFilePath        :=
CurrentFileFullPath    :=
User                   :=christiaan
Date                   :=06/19/15
CodeLitePath           :="C:\Program Files\CodeLite"
LinkerName             :=C:/TDM-GCC-64/bin/g++.exe 
SharedObjectLinkerName :=C:/TDM-GCC-64/bin/g++.exe -shared -fPIC
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
OutputFile             :=$(IntermediateDirectory)/$(ProjectName)
Preprocessors          :=$(PreprocessorSwitch)NDEBUG 
ObjectSwitch           :=-o 
ArchiveOutputSwitch    := 
PreprocessOnlySwitch   :=-E
ObjectsFileList        :="rabbit.txt"
PCHCompileFlags        :=
MakeDirCommand         :=makedir
RcCmpOptions           := 
RcCompilerName         :=C:/TDM-GCC-64/bin/windres.exe 
LinkOptions            :=  
IncludePath            :=  $(IncludeSwitch). $(IncludeSwitch). $(IncludeSwitch)C:/dev/cpp_all/repo/sparsehash-2.0.2/src/windows $(IncludeSwitch)C:/dev/cpp_all/repo/sparsehash-2.0.2/src 
IncludePCH             := 
RcIncludePath          := 
Libs                   := $(LibrarySwitch)psapi 
ArLibs                 :=  "psapi" 
LibPath                := $(LibraryPathSwitch). 

##
## Common variables
## AR, CXX, CC, AS, CXXFLAGS and CFLAGS can be overriden using an environment variables
##
AR       := C:/TDM-GCC-64/bin/ar.exe rcu
CXX      := C:/TDM-GCC-64/bin/g++.exe 
CC       := C:/TDM-GCC-64/bin/gcc.exe 
CXXFLAGS :=  -O3 -fexpensive-optimizations -std=c++11 -Wall $(Preprocessors)
CFLAGS   :=  -O2 -Wall $(Preprocessors)
ASFLAGS  := 
AS       := C:/TDM-GCC-64/bin/as.exe 


##
## User defined environment variables
##
CodeLiteDir:=C:\Program Files\CodeLite
Objects0=$(IntermediateDirectory)/tests_main.cpp$(ObjectSuffix) $(IntermediateDirectory)/time_hash_maps.cpp$(ObjectSuffix) 



Objects=$(Objects0) 

##
## Main Build Targets 
##
.PHONY: all clean PreBuild PrePreBuild PostBuild
all: $(OutputFile)

$(OutputFile): $(IntermediateDirectory)/.d $(Objects) 
	@$(MakeDirCommand) $(@D)
	@echo "" > $(IntermediateDirectory)/.d
	@echo $(Objects0)  > $(ObjectsFileList)
	$(LinkerName) $(OutputSwitch)$(OutputFile) @$(ObjectsFileList) $(LibPath) $(Libs) $(LinkOptions)

$(IntermediateDirectory)/.d:
	@$(MakeDirCommand) "./Release"

PreBuild:


##
## Objects
##
$(IntermediateDirectory)/tests_main.cpp$(ObjectSuffix): tests/main.cpp $(IntermediateDirectory)/tests_main.cpp$(DependSuffix)
	$(CXX) $(IncludePCH) $(SourceSwitch) "C:/dev/cpp_all/rabbit/tests/main.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/tests_main.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/tests_main.cpp$(DependSuffix): tests/main.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/tests_main.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/tests_main.cpp$(DependSuffix) -MM "tests/main.cpp"

$(IntermediateDirectory)/tests_main.cpp$(PreprocessSuffix): tests/main.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/tests_main.cpp$(PreprocessSuffix) "tests/main.cpp"

$(IntermediateDirectory)/time_hash_maps.cpp$(ObjectSuffix): time_hash_maps.cpp $(IntermediateDirectory)/time_hash_maps.cpp$(DependSuffix)
	$(CXX) $(IncludePCH) $(SourceSwitch) "C:/dev/cpp_all/rabbit/time_hash_maps.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/time_hash_maps.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/time_hash_maps.cpp$(DependSuffix): time_hash_maps.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/time_hash_maps.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/time_hash_maps.cpp$(DependSuffix) -MM "time_hash_maps.cpp"

$(IntermediateDirectory)/time_hash_maps.cpp$(PreprocessSuffix): time_hash_maps.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/time_hash_maps.cpp$(PreprocessSuffix) "time_hash_maps.cpp"


-include $(IntermediateDirectory)/*$(DependSuffix)
##
## Clean
##
clean:
	$(RM) -r ./Release/


