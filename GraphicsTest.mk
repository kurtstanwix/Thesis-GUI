##
## Auto Generated makefile by CodeLite IDE
## any manual changes will be erased      
##
## Debug
ProjectName            :=GraphicsTest
ConfigurationName      :=Debug
WorkspacePath          :=/home/kurt/Desktop/METR4901/Workspaces
ProjectPath            :=/home/kurt/Desktop/METR4901/Workspaces/GraphicsTest
IntermediateDirectory  :=./Debug
OutDir                 := $(IntermediateDirectory)
CurrentFileName        :=
CurrentFilePath        :=
CurrentFileFullPath    :=
User                   :=Kurt Stanwix
Date                   :=12/30/20
CodeLitePath           :=/home/kurt/.codelite
LinkerName             :=/usr/bin/g++
SharedObjectLinkerName :=/usr/bin/g++ -shared -fPIC
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
Preprocessors          :=
ObjectSwitch           :=-o 
ArchiveOutputSwitch    := 
PreprocessOnlySwitch   :=-E
ObjectsFileList        :="GraphicsTest.txt"
PCHCompileFlags        :=
MakeDirCommand         :=mkdir -p
LinkOptions            :=  
IncludePath            :=  $(IncludeSwitch). $(IncludeSwitch). 
IncludePCH             := 
RcIncludePath          := $(IncludeSwitch)$(WorkspacePath) 
Libs                   := $(LibrarySwitch)sfml-graphics $(LibrarySwitch)sfml-window $(LibrarySwitch)sfml-system 
ArLibs                 :=  "sfml-graphics" "sfml-window" "sfml-system" 
LibPath                := $(LibraryPathSwitch). 

##
## Common variables
## AR, CXX, CC, AS, CXXFLAGS and CFLAGS can be overriden using an environment variables
##
AR       := /usr/bin/ar rcu
CXX      := /usr/bin/g++
CC       := /usr/bin/gcc
CXXFLAGS :=  -g -O0 -Wall $(Preprocessors)
CFLAGS   :=  -g -O0 -Wall $(Preprocessors)
ASFLAGS  := 
AS       := /usr/bin/as


##
## User defined environment variables
##
CodeLiteDir:=/usr/share/codelite
LD_LIBRARY_PATH:=$HOME/Desktop/madp/lib
Objects0=$(IntermediateDirectory)/Interface.cpp$(ObjectSuffix) $(IntermediateDirectory)/NetworkTopology.cpp$(ObjectSuffix) $(IntermediateDirectory)/NetworkWindow.cpp$(ObjectSuffix) $(IntermediateDirectory)/BezierCurve.cpp$(ObjectSuffix) $(IntermediateDirectory)/Tests.cpp$(ObjectSuffix) $(IntermediateDirectory)/main.cpp$(ObjectSuffix) 



Objects=$(Objects0) 

##
## Main Build Targets 
##
.PHONY: all clean PreBuild PrePreBuild PostBuild MakeIntermediateDirs
all: $(OutputFile)

$(OutputFile): $(IntermediateDirectory)/.d $(Objects) 
	@$(MakeDirCommand) $(@D)
	@echo "" > $(IntermediateDirectory)/.d
	@echo $(Objects0)  > $(ObjectsFileList)
	$(LinkerName) $(OutputSwitch)$(OutputFile) @$(ObjectsFileList) $(LibPath) $(Libs) $(LinkOptions)

MakeIntermediateDirs:
	@test -d ./Debug || $(MakeDirCommand) ./Debug


$(IntermediateDirectory)/.d:
	@test -d ./Debug || $(MakeDirCommand) ./Debug

PreBuild:


##
## Objects
##
$(IntermediateDirectory)/Interface.cpp$(ObjectSuffix): Interface.cpp $(IntermediateDirectory)/Interface.cpp$(DependSuffix)
	$(CXX) $(IncludePCH) $(SourceSwitch) "/home/kurt/Desktop/METR4901/Workspaces/GraphicsTest/Interface.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/Interface.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/Interface.cpp$(DependSuffix): Interface.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/Interface.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/Interface.cpp$(DependSuffix) -MM Interface.cpp

$(IntermediateDirectory)/Interface.cpp$(PreprocessSuffix): Interface.cpp
	$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/Interface.cpp$(PreprocessSuffix) Interface.cpp

$(IntermediateDirectory)/NetworkTopology.cpp$(ObjectSuffix): NetworkTopology.cpp $(IntermediateDirectory)/NetworkTopology.cpp$(DependSuffix)
	$(CXX) $(IncludePCH) $(SourceSwitch) "/home/kurt/Desktop/METR4901/Workspaces/GraphicsTest/NetworkTopology.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/NetworkTopology.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/NetworkTopology.cpp$(DependSuffix): NetworkTopology.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/NetworkTopology.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/NetworkTopology.cpp$(DependSuffix) -MM NetworkTopology.cpp

$(IntermediateDirectory)/NetworkTopology.cpp$(PreprocessSuffix): NetworkTopology.cpp
	$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/NetworkTopology.cpp$(PreprocessSuffix) NetworkTopology.cpp

$(IntermediateDirectory)/NetworkWindow.cpp$(ObjectSuffix): NetworkWindow.cpp $(IntermediateDirectory)/NetworkWindow.cpp$(DependSuffix)
	$(CXX) $(IncludePCH) $(SourceSwitch) "/home/kurt/Desktop/METR4901/Workspaces/GraphicsTest/NetworkWindow.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/NetworkWindow.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/NetworkWindow.cpp$(DependSuffix): NetworkWindow.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/NetworkWindow.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/NetworkWindow.cpp$(DependSuffix) -MM NetworkWindow.cpp

$(IntermediateDirectory)/NetworkWindow.cpp$(PreprocessSuffix): NetworkWindow.cpp
	$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/NetworkWindow.cpp$(PreprocessSuffix) NetworkWindow.cpp

$(IntermediateDirectory)/BezierCurve.cpp$(ObjectSuffix): BezierCurve.cpp $(IntermediateDirectory)/BezierCurve.cpp$(DependSuffix)
	$(CXX) $(IncludePCH) $(SourceSwitch) "/home/kurt/Desktop/METR4901/Workspaces/GraphicsTest/BezierCurve.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/BezierCurve.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/BezierCurve.cpp$(DependSuffix): BezierCurve.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/BezierCurve.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/BezierCurve.cpp$(DependSuffix) -MM BezierCurve.cpp

$(IntermediateDirectory)/BezierCurve.cpp$(PreprocessSuffix): BezierCurve.cpp
	$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/BezierCurve.cpp$(PreprocessSuffix) BezierCurve.cpp

$(IntermediateDirectory)/Tests.cpp$(ObjectSuffix): Tests.cpp $(IntermediateDirectory)/Tests.cpp$(DependSuffix)
	$(CXX) $(IncludePCH) $(SourceSwitch) "/home/kurt/Desktop/METR4901/Workspaces/GraphicsTest/Tests.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/Tests.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/Tests.cpp$(DependSuffix): Tests.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/Tests.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/Tests.cpp$(DependSuffix) -MM Tests.cpp

$(IntermediateDirectory)/Tests.cpp$(PreprocessSuffix): Tests.cpp
	$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/Tests.cpp$(PreprocessSuffix) Tests.cpp

$(IntermediateDirectory)/main.cpp$(ObjectSuffix): main.cpp $(IntermediateDirectory)/main.cpp$(DependSuffix)
	$(CXX) $(IncludePCH) $(SourceSwitch) "/home/kurt/Desktop/METR4901/Workspaces/GraphicsTest/main.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/main.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/main.cpp$(DependSuffix): main.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/main.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/main.cpp$(DependSuffix) -MM main.cpp

$(IntermediateDirectory)/main.cpp$(PreprocessSuffix): main.cpp
	$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/main.cpp$(PreprocessSuffix) main.cpp


-include $(IntermediateDirectory)/*$(DependSuffix)
##
## Clean
##
clean:
	$(RM) -r ./Debug/


