// EnergyPlus, Copyright (c) 1996-2020, The Board of Trustees of the University of Illinois,
// The Regents of the University of California, through Lawrence Berkeley National Laboratory
// (subject to receipt of any required approvals from the U.S. Dept. of Energy), Oak Ridge
// National Laboratory, managed by UT-Battelle, Alliance for Sustainable Energy, LLC, and other
// contributors. All rights reserved.
//
// NOTICE: This Software was developed under funding from the U.S. Department of Energy and the
// U.S. Government consequently retains certain rights. As such, the U.S. Government has been
// granted for itself and others acting on its behalf a paid-up, nonexclusive, irrevocable,
// worldwide license in the Software to reproduce, distribute copies to the public, prepare
// derivative works, and perform publicly and display publicly, and to permit others to do so.
//
// Redistribution and use in source and binary forms, with or without modification, are permitted
// provided that the following conditions are met:
//
// (1) Redistributions of source code must retain the above copyright notice, this list of
//     conditions and the following disclaimer.
//
// (2) Redistributions in binary form must reproduce the above copyright notice, this list of
//     conditions and the following disclaimer in the documentation and/or other materials
//     provided with the distribution.
//
// (3) Neither the name of the University of California, Lawrence Berkeley National Laboratory,
//     the University of Illinois, U.S. Dept. of Energy nor the names of its contributors may be
//     used to endorse or promote products derived from this software without specific prior
//     written permission.
//
// (4) Use of EnergyPlus(TM) Name. If Licensee (i) distributes the software in stand-alone form
//     without changes from the version obtained under this License, or (ii) Licensee makes a
//     reference solely to the software portion of its product, Licensee must refer to the
//     software as "EnergyPlus version X" software, where "X" is the version number Licensee
//     obtained under this License and may not use a different name for the software. Except as
//     specifically required in this Section (4), Licensee shall not use in a company name, a
//     product name, in advertising, publicity, or other promotional activities any name, trade
//     name, trademark, logo, or other designation of "EnergyPlus", "E+", "e+" or confusingly
//     similar designation, without the U.S. Department of Energy's prior written consent.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR
// IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY
// AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR
// CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
// CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
// SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
// THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
// OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
// POSSIBILITY OF SUCH DAMAGE.

// Google Test Headers
#include <gtest/gtest.h>

// EnergyPlus Headers
#include <EnergyPlus/OutputFiles.hh>
#include <EnergyPlus/SimulationManager.hh>

#include "Fixtures/EnergyPlusFixture.hh"

using namespace EnergyPlus;
using namespace ObjexxFCL;

TEST_F(EnergyPlusFixture, CheckThreading)
{
    std::string const idf_objects = delimited_string({
        "ProgramControl,",
        "	1;",

    });

    EXPECT_FALSE(process_idf(idf_objects, false));

    std::string const error_string = delimited_string({
        "   ** Severe  ** Line: 1 Index: 14 - \"ProgramControl\" is not a valid Object Type.",
    });

    EXPECT_TRUE(compare_err_stream(error_string, true));
}

TEST_F(EnergyPlusFixture, Test_PerformancePrecisionTradeoffs)
{
    std::string const idf_objects = delimited_string({
        "  Version,9.3;",

        "  SimulationControl,",
        "    No,                      !- Do Zone Sizing Calculation",
        "    No,                      !- Do System Sizing Calculation",
        "    No,                      !- Do Plant Sizing Calculation",
        "    No,                      !- Run Simulation for Sizing Periods",
        "    Yes;                     !- Run Simulation for Weather File Run Periods",

        "  PerformancePrecisionTradeoffs,",
        "    No;       ! - Use Coil Direct Solutions",
    });

    EXPECT_TRUE(process_idf(idf_objects));

    SimulationManager::GetProjectData(OutputFiles::getSingleton());

    // no error message from PerformancePrecisionTradeoffs objects
    EXPECT_TRUE(compare_err_stream("", true));
  
}

TEST_F(EnergyPlusFixture, Test_PerformancePrecisionTradeoffs_DirectSolution_Message)
{
    // issue 7646
    std::string const idf_objects = delimited_string({
        "  Version,9.3;",
        "  PerformancePrecisionTradeoffs,",
        "     Yes; ! - Use Coil Direct Solutions",

    });

    EXPECT_TRUE(process_idf(idf_objects, false));

    SimulationManager::GetProjectData(OutputFiles::getSingleton());

    std::string const error_string = delimited_string({
        "   ** Warning ** PerformancePrecisionTradeoffs: Coil Direct Solution simulation is selected.",
    });

    EXPECT_TRUE(compare_err_stream(error_string, true));
}

TEST_F(EnergyPlusFixture, Simulationmanager_bool_to_string)
{
    EXPECT_EQ(SimulationManager::bool_to_string(true), "True");
    EXPECT_EQ(SimulationManager::bool_to_string(false), "False");
}

TEST_F(EnergyPlusFixture, Simulationmanager_writeIntialPerfLogValues)
{
    DataStringGlobals::outputPerfLogFileName = "eplusout_perflog.csv";

    // start with no file 
    std::remove(DataStringGlobals::outputPerfLogFileName.c_str());

    // make sure the static variables are cleared
    UtilityRoutines::appendPerfLog("RESET", "RESET");

    DataStringGlobals::VerString = "EnergyPlus, Version 0.0.0-xxxx, August 14 1945";

    // call the function to test
    SimulationManager::writeIntialPerfLogValues("MODE193");

    // force the file to be written
    UtilityRoutines::appendPerfLog("lastHeader", "lastValue", true);

    std::ifstream perfLogFile;
    std::stringstream perfLogStrSteam;

    perfLogFile.open(DataStringGlobals::outputPerfLogFileName);
    perfLogStrSteam << perfLogFile.rdbuf();
    perfLogFile.close();
    std::string perfLogContents = perfLogStrSteam.str();

    std::string expectedContents = "Program, Version, TimeStamp,Use Coil Direct Solution,Zone Radiant Exchange Algorithm,"
        "Override Mode,Number of Timesteps per Hour,Minimum Number of Warmup Days,SuppressAllBeginEnvironmentResets,MaxZoneTempDiff,lastHeader,\n"
        "EnergyPlus, Version 0.0.0-xxxx, August 14 1945,False,ScriptF,MODE193,0,1,False,0.30,lastValue,\n";

    EXPECT_EQ(perfLogContents, expectedContents);

    // clean up the file
    std::remove(DataStringGlobals::outputPerfLogFileName.c_str());

}
