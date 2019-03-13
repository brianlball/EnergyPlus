// EnergyPlus, Copyright (c) 1996-2019, The Board of Trustees of the University of Illinois,
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

#include "EnergyPlusPgm.hh"
#include <iostream>
#include <FileSystem.hh>

void message_callback_handler(std::string const &message)
{
    std::cout << "EnergyPlusStateTest (message): " << message << std::endl;
}

void progress_callback_handler(int const progress)
{
    std::cout << "EnergyPlusStateTest (progress): " << progress << std::endl;
}

void run_sims(std::string const directory, std::string const modelName)
{
    using namespace EnergyPlus::FileSystem;
    int status(EXIT_FAILURE);
    //make tst_ModelName test directory
    std::string tstDir(directory + std::string("/tst_") + modelName);
    makeDirectory(tstDir);
    //make 3 run dirs
    std::string tstDirFull(tstDir + std::string("/full"));
    std::string tstDirFirst(tstDir + std::string("/first"));
    std::string tstDirLast(tstDir + std::string("/last"));
    makeDirectory(tstDirFull);
    makeDirectory(tstDirFirst);
    makeDirectory(tstDirLast);
    //std::cout << "COPYING: " << directory + std::string("/") + modelName + tstDirFull + std::string("/in.idf");
    //copy files to the 3 directories
    linkFile(directory + std::string("/") + modelName + std::string(".idf"), tstDirFull + std::string("/in.idf"));
    linkFile(directory + std::string("/in.epw") , tstDirFull + std::string("/in.epw"));
    linkFile(directory + std::string("/") + modelName + std::string(".idf"), tstDirFirst + std::string("/in.idf"));
    linkFile(directory + std::string("/in.epw"), tstDirFirst + std::string("/in.epw"));
    linkFile(directory + std::string("/") + modelName + std::string(".idf"), tstDirLast + std::string("/in.idf"));
    linkFile(directory + std::string("/in.epw"), tstDirLast + std::string("/in.epw"));
    //run full runperiod
    status = RunEnergyPlus(tstDirFull);

    if (!std::cin.good()) std::cin.clear();
    if (!std::cerr.good()) std::cerr.clear();
    if (!std::cout.good()) std::cout.clear();
    std::cerr << "Standard error is still available for use" << std::endl;
    std::cout << "Standard output is still available for use" << std::endl;
    //run first half TODO
    status = RunEnergyPlus(tstDirFirst);

    if (!std::cin.good()) std::cin.clear();
    if (!std::cerr.good()) std::cerr.clear();
    if (!std::cout.good()) std::cout.clear();
    std::cerr << "Standard error is still available for use" << std::endl;
    std::cout << "Standard output is still available for use" << std::endl;
    //run last half TODO
    status = RunEnergyPlus(tstDirLast);

    if (!std::cin.good()) std::cin.clear();
    if (!std::cerr.good()) std::cerr.clear();
    if (!std::cout.good()) std::cout.clear();
    std::cerr << "Standard error is still available for use" << std::endl;
    std::cout << "Standard output is still available for use" << std::endl;

}

int main(int argc, char *argv[])
{
    //using namespace EnergyPlus;
    using namespace EnergyPlus::FileSystem;

    std::cout << "Testing EnergyPlus reset states." << std::endl;
    StoreMessageCallback(message_callback_handler);
    StoreProgressCallback(progress_callback_handler);

    
    if (argc < 3) {
        std::cout << "Call this with 1: a path to EnergyPlus files, 2: name of file" << std::endl;
        return EXIT_FAILURE;
    }

    if (directoryExists(argv[1])) {
        std::string filePath(argv[1] + std::string("/") + argv[2] + std::string(".idf"));
        if (fileExists(filePath)) {
            run_sims(argv[1],argv[2]);
        } else {
          std::cout << "argument 2: 'name of file' is not good." << std::endl;
            return EXIT_FAILURE;
        }
    } else {
        std::cout << "argument 1: 'a path to EnergyPlus files' is not good." << std::endl;
        return EXIT_FAILURE;
    }
    
    if (!std::cin.good()) std::cin.clear();
    if (!std::cerr.good()) std::cerr.clear();
    if (!std::cout.good()) std::cout.clear();
    std::cerr << "Standard error is still available for use" << std::endl;
    std::cout << "Standard output is still available for use" << std::endl;
    //return status;
}
