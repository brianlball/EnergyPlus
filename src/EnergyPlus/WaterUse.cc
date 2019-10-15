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

// C++ Headers
#include <cmath>

// ObjexxFCL Headers
#include <ObjexxFCL/Array.functions.hh>
#include <ObjexxFCL/Fmath.hh>

// EnergyPlus Headers
#include <EnergyPlus/BranchNodeConnections.hh>
#include <EnergyPlus/DataEnvironment.hh>
#include <EnergyPlus/DataHVACGlobals.hh>
#include <EnergyPlus/DataHeatBalFanSys.hh>
#include <EnergyPlus/DataHeatBalance.hh>
#include <EnergyPlus/DataIPShortCuts.hh>
#include <EnergyPlus/DataLoopNode.hh>
#include <EnergyPlus/DataPlant.hh>
#include <EnergyPlus/DataPrecisionGlobals.hh>
#include <EnergyPlus/DataWater.hh>
#include <EnergyPlus/General.hh>
#include <EnergyPlus/HeatBalanceInternalHeatGains.hh>
#include <EnergyPlus/InputProcessing/InputProcessor.hh>
#include <EnergyPlus/NodeInputManager.hh>
#include <EnergyPlus/OutputProcessor.hh>
#include <EnergyPlus/PlantUtilities.hh>
#include <EnergyPlus/Psychrometrics.hh>
#include <EnergyPlus/ScheduleManager.hh>
#include <EnergyPlus/UtilityRoutines.hh>
#include <EnergyPlus/WaterManager.hh>
#include <EnergyPlus/WaterUse.hh>

namespace EnergyPlus {

namespace WaterUse {

    // MODULE INFORMATION:
    //       AUTHOR         Peter Graham Ellis
    //       DATE WRITTEN   August 2006
    //       MODIFIED       Brent Griffith, plant upgrade
    //       RE-ENGINEERED  na

    // MODULE PARAMETER DEFINITIONS:
    int const HeatRecoveryHXIdeal(1);
    int const HeatRecoveryHXCounterFlow(2);
    int const HeatRecoveryHXCrossFlow(3);

    int const HeatRecoveryConfigPlant(1);
    int const HeatRecoveryConfigEquipment(2);
    int const HeatRecoveryConfigPlantAndEquip(3);

    static std::string const BlankString;

    // MODULE VARIABLE DECLARATIONS:
    int modNumWaterEquipment(0);
    int modNumWaterConnections(0);
    bool GetWaterUseInputFlag(true);

    Array1D_bool CheckEquipName;
    Array1D_bool CheckPlantLoop;

    // Object Data
    Array1D<WaterEquipmentType> WaterEquipment;
    Array1D<WaterConnectionsType> WaterConnections;

    void clear_state()
    {
        modNumWaterEquipment = 0;
        modNumWaterConnections = 0;
        GetWaterUseInputFlag = true;
        CheckEquipName.deallocate();
        CheckPlantLoop.deallocate();
        WaterEquipment.deallocate();
        WaterConnections.deallocate();
    }

    void SimulateWaterUse(bool FirstHVACIteration)
    {

        // SUBROUTINE INFORMATION:
        //       AUTHOR         Peter Graham Ellis
        //       DATE WRITTEN   August 2006
        //       MODIFIED       Brent Griffith, March 2010, seperated plant connected to different sim routine
        //       RE-ENGINEERED  na

        // PURPOSE OF THIS SUBROUTINE:
        // This routine is called from non zone equipment manager and serves to call
        // water use and connections that are not connected to a full plant loop

        // Locals
        // SUBROUTINE PARAMETER DEFINITIONS:
        int const MaxIterations(100);
        Real64 const Tolerance(0.1); // Make input?

        // SUBROUTINE LOCAL VARIABLE DECLARATIONS:
        int WaterEquipNum;
        int WaterConnNum;
        int NumIteration;
        int MaxIterationsErrorCount;
        bool MyEnvrnFlag(true);

        // FLOW:
        if (GetWaterUseInputFlag) {
            GetWaterUseInput();
            GetWaterUseInputFlag = false;
        }

        if (DataGlobals::BeginEnvrnFlag && MyEnvrnFlag) {
            MaxIterationsErrorCount = 0;
            if (modNumWaterEquipment > 0) {
                for (auto &e : WaterEquipment) {
                    e.SensibleRate = 0.0;
                    e.SensibleEnergy = 0.0;
                    e.LatentRate = 0.0;
                    e.LatentEnergy = 0.0;
                    e.MixedTemp = 0.0;
                    e.TotalMassFlowRate = 0.0;
                    e.DrainTemp = 0.0;
                }
            }

            if (modNumWaterConnections > 0) {
                for (auto &e : WaterConnections)
                    e.TotalMassFlowRate = 0.0;
            }

            MyEnvrnFlag = false;
        }

        if (!DataGlobals::BeginEnvrnFlag) MyEnvrnFlag = true;

        // Simulate all unconnected WATER USE EQUIPMENT objects
        for (WaterEquipNum = 1; WaterEquipNum <= modNumWaterEquipment; ++WaterEquipNum) {
            if (WaterEquipment(WaterEquipNum).Connections == 0) {
                CalcEquipmentFlowRates(WaterEquipNum);
                CalcEquipmentDrainTemp(WaterEquipNum);
            }
        } // WaterEquipNum

        ReportStandAloneWaterUse();

        // Simulate WATER USE CONNECTIONS objects and connected WATER USE EQUIPMENT objects
        for (WaterConnNum = 1; WaterConnNum <= modNumWaterConnections; ++WaterConnNum) {

            if (!WaterConnections(WaterConnNum).StandAlone) continue; // only model non plant connections here

            InitConnections(WaterConnNum);

            NumIteration = 0;

            while (true) {
                ++NumIteration;

                CalcConnectionsFlowRates(WaterConnNum, FirstHVACIteration);
                CalcConnectionsDrainTemp(WaterConnNum);
                CalcConnectionsHeatRecovery(WaterConnNum);

                if (WaterConnections(WaterConnNum).TempError < Tolerance) {
                    break;
                } else if (NumIteration > MaxIterations) {
                    if (!DataGlobals::WarmupFlag) {
                        if (WaterConnections(WaterConnNum).MaxIterationsErrorIndex == 0) {
                            ShowWarningError("WaterUse:Connections = " + WaterConnections(WaterConnNum).Name +
                                             ":  Heat recovery temperature did not converge");
                            ShowContinueErrorTimeStamp("");
                        }
                        ShowRecurringWarningErrorAtEnd("WaterUse:Connections = " + WaterConnections(WaterConnNum).Name +
                                                           ":  Heat recovery temperature did not converge",
                                                       WaterConnections(WaterConnNum).MaxIterationsErrorIndex);
                    }
                    break;
                }

            } // WHILE

            UpdateWaterConnections(WaterConnNum);

            ReportWaterUse(WaterConnNum);

        } // WaterConnNum
    }

    void SimulateWaterUseConnection(
        int EP_UNUSED(EquipTypeNum), std::string &CompName, int &CompIndex, bool InitLoopEquip, bool FirstHVACIteration)
    {

        // SUBROUTINE INFORMATION:
        //       AUTHOR         Brent Griffith March 2010, Demand Side Update
        //       DATE WRITTEN   August 2006
        //       MODIFIED
        //       RE-ENGINEERED  na

        // PURPOSE OF THIS SUBROUTINE:
        // Plant sim call for plant loop connected water use and connections
        // (based on SimulateWaterUse by P. Ellis)

        // Locals
        // SUBROUTINE PARAMETER DEFINITIONS:
        int const MaxIterations(100);
        Real64 const Tolerance(0.1); // Make input?

        // SUBROUTINE LOCAL VARIABLE DECLARATIONS:
        //  INTEGER :: WaterEquipNum
        int WaterConnNum;
        int NumIteration;
        int MaxIterationsErrorCount;
        bool MyEnvrnFlag(true);

        // FLOW:
        if (GetWaterUseInputFlag) {
            GetWaterUseInput();
            GetWaterUseInputFlag = false;
        }

        if (CompIndex == 0) {
            WaterConnNum = UtilityRoutines::FindItemInList(CompName, WaterConnections);
            if (WaterConnNum == 0) {
                ShowFatalError("SimulateWaterUseConnection: Unit not found=" + CompName);
            }
            CompIndex = WaterConnNum;
        } else {
            WaterConnNum = CompIndex;
            if (WaterConnNum > modNumWaterConnections || WaterConnNum < 1) {
                ShowFatalError("SimulateWaterUseConnection: Invalid CompIndex passed=" + General::TrimSigDigits(WaterConnNum) +
                               ", Number of Units=" + General::TrimSigDigits(modNumWaterConnections) + ", Entered Unit name=" + CompName);
            }
            if (CheckEquipName(WaterConnNum)) {
                if (CompName != WaterConnections(WaterConnNum).Name) {
                    ShowFatalError("SimulateWaterUseConnection: Invalid CompIndex passed=" + General::TrimSigDigits(WaterConnNum) + ", Unit name=" + CompName +
                                   ", stored Unit Name for that index=" + WaterConnections(WaterConnNum).Name);
                }
                CheckEquipName(WaterConnNum) = false;
            }
        }

        if (InitLoopEquip) {
            return;
        }

        if (DataGlobals::BeginEnvrnFlag && MyEnvrnFlag) {
            MaxIterationsErrorCount = 0;
            if (modNumWaterEquipment > 0) {
                for (int i = WaterEquipment.l(), e = WaterEquipment.u(); i <= e; ++i) {
                    WaterEquipment(i).reset();
                }
            }

            if (modNumWaterConnections > 0) {
                for (auto &e : WaterConnections)
                    e.TotalMassFlowRate = 0.0;
            }

            MyEnvrnFlag = false;
        }

        if (!DataGlobals::BeginEnvrnFlag) MyEnvrnFlag = true;

        InitConnections(WaterConnNum);

        NumIteration = 0;

        while (true) {
            ++NumIteration;

            CalcConnectionsFlowRates(WaterConnNum, FirstHVACIteration);
            CalcConnectionsDrainTemp(WaterConnNum);
            CalcConnectionsHeatRecovery(WaterConnNum);

            if (WaterConnections(WaterConnNum).TempError < Tolerance) {
                break;
            } else if (NumIteration > MaxIterations) {
                if (!DataGlobals::WarmupFlag) {
                    if (WaterConnections(WaterConnNum).MaxIterationsErrorIndex == 0) {
                        ShowWarningError("WaterUse:Connections = " + WaterConnections(WaterConnNum).Name +
                                         ":  Heat recovery temperature did not converge");
                        ShowContinueErrorTimeStamp("");
                    }
                    ShowRecurringWarningErrorAtEnd("WaterUse:Connections = " + WaterConnections(WaterConnNum).Name +
                                                       ":  Heat recovery temperature did not converge",
                                                   WaterConnections(WaterConnNum).MaxIterationsErrorIndex);
                }
                break;
            }

        } // WHILE

        UpdateWaterConnections(WaterConnNum);

        ReportWaterUse(WaterConnNum);
    }

    void GetWaterUseInput()
    {

        // SUBROUTINE INFORMATION:
        //       AUTHOR         Peter Graham Ellis
        //       DATE WRITTEN   August 2006
        //       MODIFIED       na
        //       RE-ENGINEERED  na

        // METHODOLOGY EMPLOYED: Standard EnergyPlus methodology.

        // SUBROUTINE LOCAL VARIABLE DECLARATIONS:
        bool ErrorsFound(false); // Set to true if errors in input, fatal at end of routine
        int IOStatus;                   // Used in GetObjectItem
        int NumAlphas;                  // Number of Alphas for each GetObjectItem call
        int NumNumbers;                 // Number of Numbers for each GetObjectItem call
        // unused1208  INTEGER                        :: NumArgs
        int WaterEquipNum;
        int WaterConnNum;
        int AlphaNum;
        int thisWaterEquipNum;

        // FLOW:

        DataIPShortCuts::cCurrentModuleObject = "WaterUse:Equipment";
        modNumWaterEquipment = inputProcessor->getNumObjectsFound(DataIPShortCuts::cCurrentModuleObject);

        if (modNumWaterEquipment > 0) {
            WaterEquipment.allocate(modNumWaterEquipment);

            for (WaterEquipNum = 1; WaterEquipNum <= modNumWaterEquipment; ++WaterEquipNum) {
                inputProcessor->getObjectItem(DataIPShortCuts::cCurrentModuleObject,
                                              WaterEquipNum,
                                              DataIPShortCuts::cAlphaArgs,
                                              NumAlphas,
                                              DataIPShortCuts::rNumericArgs,
                                              NumNumbers,
                                              IOStatus,
                                              _,
                                              DataIPShortCuts::lAlphaFieldBlanks,
                                              DataIPShortCuts::cAlphaFieldNames,
                                              DataIPShortCuts::cNumericFieldNames);
                UtilityRoutines::IsNameEmpty(DataIPShortCuts::cAlphaArgs(1), DataIPShortCuts::cCurrentModuleObject, ErrorsFound);
                WaterEquipment(WaterEquipNum).Name = DataIPShortCuts::cAlphaArgs(1);

                WaterEquipment(WaterEquipNum).EndUseSubcatName = DataIPShortCuts::cAlphaArgs(2);

                WaterEquipment(WaterEquipNum).PeakVolFlowRate = DataIPShortCuts::rNumericArgs(1);

                if ((NumAlphas > 2) && (!DataIPShortCuts::lAlphaFieldBlanks(3))) {
                    WaterEquipment(WaterEquipNum).FlowRateFracSchedule = ScheduleManager::GetScheduleIndex(DataIPShortCuts::cAlphaArgs(3));
                    // If no FlowRateFracSchedule, fraction defaults to 1.0

                    if (WaterEquipment(WaterEquipNum).FlowRateFracSchedule == 0) {
                        ShowSevereError("Invalid " + DataIPShortCuts::cAlphaFieldNames(3) + '=' + DataIPShortCuts::cAlphaArgs(3));
                        ShowContinueError("Entered in " + DataIPShortCuts::cCurrentModuleObject + '=' + DataIPShortCuts::cAlphaArgs(1));
                        ErrorsFound = true;
                    }
                }

                if ((NumAlphas > 3) && (!DataIPShortCuts::lAlphaFieldBlanks(4))) {
                    WaterEquipment(WaterEquipNum).TargetTempSchedule = ScheduleManager::GetScheduleIndex(DataIPShortCuts::cAlphaArgs(4));

                    if (WaterEquipment(WaterEquipNum).TargetTempSchedule == 0) {
                        ShowSevereError("Invalid " + DataIPShortCuts::cAlphaFieldNames(4) + '=' + DataIPShortCuts::cAlphaArgs(4));
                        ShowContinueError("Entered in " + DataIPShortCuts::cCurrentModuleObject + '=' + DataIPShortCuts::cAlphaArgs(1));
                        ErrorsFound = true;
                    }
                }

                if ((NumAlphas > 4) && (!DataIPShortCuts::lAlphaFieldBlanks(5))) {
                    WaterEquipment(WaterEquipNum).HotTempSchedule = ScheduleManager::GetScheduleIndex(DataIPShortCuts::cAlphaArgs(5));
                    // If no HotTempSchedule, there is no hot water.
                    // HotTempSchedule is ignored if connected to a plant loop via WATER USE CONNECTIONS

                    if (WaterEquipment(WaterEquipNum).HotTempSchedule == 0) {
                        ShowSevereError("Invalid " + DataIPShortCuts::cAlphaFieldNames(5) + '=' + DataIPShortCuts::cAlphaArgs(5));
                        ShowContinueError("Entered in " + DataIPShortCuts::cCurrentModuleObject + '=' + DataIPShortCuts::cAlphaArgs(1));
                        ErrorsFound = true;
                    }
                }

                if ((NumAlphas > 5) && (!DataIPShortCuts::lAlphaFieldBlanks(6))) {
                    WaterEquipment(WaterEquipNum).ColdTempSchedule = ScheduleManager::GetScheduleIndex(DataIPShortCuts::cAlphaArgs(6));
                    // If no ColdTempSchedule, temperatures will be calculated by WATER MAINS TEMPERATURES object

                    if (WaterEquipment(WaterEquipNum).ColdTempSchedule == 0) {
                        ShowSevereError("Invalid " + DataIPShortCuts::cAlphaFieldNames(6) + '=' + DataIPShortCuts::cAlphaArgs(6));
                        ShowContinueError("Entered in " + DataIPShortCuts::cCurrentModuleObject + '=' + DataIPShortCuts::cAlphaArgs(1));
                        ErrorsFound = true;
                    }
                }

                if ((NumAlphas > 6) && (!DataIPShortCuts::lAlphaFieldBlanks(7))) {
                    WaterEquipment(WaterEquipNum).Zone = UtilityRoutines::FindItemInList(DataIPShortCuts::cAlphaArgs(7), DataHeatBalance::Zone);

                    if (WaterEquipment(WaterEquipNum).Zone == 0) {
                        ShowSevereError("Invalid " + DataIPShortCuts::cAlphaFieldNames(7) + '=' + DataIPShortCuts::cAlphaArgs(7));
                        ShowContinueError("Entered in " + DataIPShortCuts::cCurrentModuleObject + '=' + DataIPShortCuts::cAlphaArgs(1));
                        ErrorsFound = true;
                    }
                }

                if ((NumAlphas > 7) && (!DataIPShortCuts::lAlphaFieldBlanks(8))) {
                    WaterEquipment(WaterEquipNum).SensibleFracSchedule = ScheduleManager::GetScheduleIndex(DataIPShortCuts::cAlphaArgs(8));

                    if (WaterEquipment(WaterEquipNum).SensibleFracSchedule == 0) {
                        ShowSevereError("Invalid " + DataIPShortCuts::cAlphaFieldNames(8) + '=' + DataIPShortCuts::cAlphaArgs(8));
                        ShowContinueError("Entered in " + DataIPShortCuts::cCurrentModuleObject + '=' + DataIPShortCuts::cAlphaArgs(1));
                        ErrorsFound = true;
                    }
                }

                if ((NumAlphas > 8) && (!DataIPShortCuts::lAlphaFieldBlanks(9))) {
                    WaterEquipment(WaterEquipNum).LatentFracSchedule = ScheduleManager::GetScheduleIndex(DataIPShortCuts::cAlphaArgs(9));

                    if (WaterEquipment(WaterEquipNum).LatentFracSchedule == 0) {
                        ShowSevereError("Invalid " + DataIPShortCuts::cAlphaFieldNames(9) + '=' + DataIPShortCuts::cAlphaArgs(9));
                        ShowContinueError("Entered in " + DataIPShortCuts::cCurrentModuleObject + '=' + DataIPShortCuts::cAlphaArgs(1));
                        ErrorsFound = true;
                    }
                }

            } // WaterEquipNum

            if (ErrorsFound) ShowFatalError("Errors found in processing input for " + DataIPShortCuts::cCurrentModuleObject);
        }

        DataIPShortCuts::cCurrentModuleObject = "WaterUse:Connections";
        modNumWaterConnections = inputProcessor->getNumObjectsFound(DataIPShortCuts::cCurrentModuleObject);

        if (modNumWaterConnections > 0) {
            WaterConnections.allocate(modNumWaterConnections);

            for (WaterConnNum = 1; WaterConnNum <= modNumWaterConnections; ++WaterConnNum) {
                inputProcessor->getObjectItem(DataIPShortCuts::cCurrentModuleObject,
                                              WaterConnNum,
                                              DataIPShortCuts::cAlphaArgs,
                                              NumAlphas,
                                              DataIPShortCuts::rNumericArgs,
                                              NumNumbers,
                                              IOStatus,
                                              _,
                                              DataIPShortCuts::lAlphaFieldBlanks,
                                              DataIPShortCuts::cAlphaFieldNames,
                                              DataIPShortCuts::cNumericFieldNames);
                UtilityRoutines::IsNameEmpty(DataIPShortCuts::cAlphaArgs(1), DataIPShortCuts::cCurrentModuleObject, ErrorsFound);
                WaterConnections(WaterConnNum).Name = DataIPShortCuts::cAlphaArgs(1);

                if ((!DataIPShortCuts::lAlphaFieldBlanks(2)) || (!DataIPShortCuts::lAlphaFieldBlanks(3))) {
                    WaterConnections(WaterConnNum).InletNode = NodeInputManager::GetOnlySingleNode(DataIPShortCuts::cAlphaArgs(2),
                                                                                 ErrorsFound,
                                                                                 DataIPShortCuts::cCurrentModuleObject,
                                                                                 DataIPShortCuts::cAlphaArgs(1),
                                                                                 DataLoopNode::NodeType_Water,
                                                                                 DataLoopNode::NodeConnectionType_Inlet,
                                                                                 1,
                                                                                 DataLoopNode::ObjectIsNotParent);
                    WaterConnections(WaterConnNum).OutletNode = NodeInputManager::GetOnlySingleNode(DataIPShortCuts::cAlphaArgs(3),
                                                                                  ErrorsFound,
                                                                                  DataIPShortCuts::cCurrentModuleObject,
                                                                                  DataIPShortCuts::cAlphaArgs(1),
                                                                                  DataLoopNode::NodeType_Water,
                                                                                  DataLoopNode::NodeConnectionType_Outlet,
                                                                                  1,
                                                                                  DataLoopNode::ObjectIsNotParent);

                    // Check plant connections
                    BranchNodeConnections::TestCompSet(DataIPShortCuts::cCurrentModuleObject,
                                DataIPShortCuts::cAlphaArgs(1),
                                DataIPShortCuts::cAlphaArgs(2),
                                DataIPShortCuts::cAlphaArgs(3),
                                "DHW Nodes");
                } else {
                    // If no plant nodes are connected, simulate in stand-alone mode.
                    WaterConnections(WaterConnNum).StandAlone = true;
                }

                if (!DataIPShortCuts::lAlphaFieldBlanks(4)) {
                    WaterManager::SetupTankDemandComponent(WaterConnections(WaterConnNum).Name,
                                             DataIPShortCuts::cCurrentModuleObject,
                                             DataIPShortCuts::cAlphaArgs(4),
                                             ErrorsFound,
                                             WaterConnections(WaterConnNum).SupplyTankNum,
                                             WaterConnections(WaterConnNum).TankDemandID);
                }

                if (!DataIPShortCuts::lAlphaFieldBlanks(5)) {
                    WaterManager::SetupTankSupplyComponent(WaterConnections(WaterConnNum).Name,
                                             DataIPShortCuts::cCurrentModuleObject,
                                             DataIPShortCuts::cAlphaArgs(5),
                                             ErrorsFound,
                                             WaterConnections(WaterConnNum).RecoveryTankNum,
                                             WaterConnections(WaterConnNum).TankSupplyID);
                }

                if (!DataIPShortCuts::lAlphaFieldBlanks(6)) {
                    WaterConnections(WaterConnNum).HotTempSchedule = ScheduleManager::GetScheduleIndex(DataIPShortCuts::cAlphaArgs(6));
                    // If no HotTempSchedule, there is no hot water.
                    // HotTempSchedule is ignored if connected to a plant loop via WATER USE CONNECTIONS

                    if (WaterConnections(WaterConnNum).HotTempSchedule == 0) {
                        ShowSevereError("Invalid " + DataIPShortCuts::cAlphaFieldNames(6) + '=' + DataIPShortCuts::cAlphaArgs(6));
                        ShowContinueError("Entered in " + DataIPShortCuts::cCurrentModuleObject + '=' + DataIPShortCuts::cAlphaArgs(1));
                        ErrorsFound = true;
                    }
                }

                if (!DataIPShortCuts::lAlphaFieldBlanks(7)) {
                    WaterConnections(WaterConnNum).ColdTempSchedule = ScheduleManager::GetScheduleIndex(DataIPShortCuts::cAlphaArgs(7));
                    // If no ColdTempSchedule, temperatures will be calculated by WATER MAINS TEMPERATURES object

                    if (WaterConnections(WaterConnNum).ColdTempSchedule == 0) {
                        ShowSevereError("Invalid " + DataIPShortCuts::cAlphaFieldNames(7) + '=' + DataIPShortCuts::cAlphaArgs(7));
                        ShowContinueError("Entered in " + DataIPShortCuts::cCurrentModuleObject + '=' + DataIPShortCuts::cAlphaArgs(1));
                        ErrorsFound = true;
                    }
                }

                if ((!DataIPShortCuts::lAlphaFieldBlanks(8)) && (DataIPShortCuts::cAlphaArgs(8) != "NONE")) {
                    WaterConnections(WaterConnNum).HeatRecovery = true;

                    {
                        auto const SELECT_CASE_var(DataIPShortCuts::cAlphaArgs(8));
                        if (SELECT_CASE_var == "IDEAL") {
                            WaterConnections(WaterConnNum).HeatRecoveryHX = HeatRecoveryHXIdeal;
                        } else if (SELECT_CASE_var == "COUNTERFLOW") {
                            WaterConnections(WaterConnNum).HeatRecoveryHX = HeatRecoveryHXCounterFlow;
                        } else if (SELECT_CASE_var == "CROSSFLOW") {
                            WaterConnections(WaterConnNum).HeatRecoveryHX = HeatRecoveryHXCrossFlow;
                        } else {
                            ShowSevereError("Invalid " + DataIPShortCuts::cAlphaFieldNames(8) + '=' + DataIPShortCuts::cAlphaArgs(8));
                            ShowContinueError("Entered in " + DataIPShortCuts::cCurrentModuleObject + '=' + DataIPShortCuts::cAlphaArgs(1));
                            ErrorsFound = true;
                        }
                    }

                    {
                        auto const SELECT_CASE_var(DataIPShortCuts::cAlphaArgs(9));
                        if (SELECT_CASE_var == "PLANT") {
                            WaterConnections(WaterConnNum).HeatRecoveryConfig = HeatRecoveryConfigPlant;
                        } else if (SELECT_CASE_var == "EQUIPMENT") {
                            WaterConnections(WaterConnNum).HeatRecoveryConfig = HeatRecoveryConfigEquipment;
                        } else if (SELECT_CASE_var == "PLANTANDEQUIPMENT") {
                            WaterConnections(WaterConnNum).HeatRecoveryConfig = HeatRecoveryConfigPlantAndEquip;
                        } else {
                            ShowSevereError("Invalid " + DataIPShortCuts::cAlphaFieldNames(9) + '=' + DataIPShortCuts::cAlphaArgs(9));
                            ShowContinueError("Entered in " + DataIPShortCuts::cCurrentModuleObject + '=' + DataIPShortCuts::cAlphaArgs(1));
                            ErrorsFound = true;
                        }
                    }
                }

                WaterConnections(WaterConnNum).HXUA = DataIPShortCuts::rNumericArgs(1);

                WaterConnections(WaterConnNum).WaterEquipment.allocate(NumAlphas - 9);

                for (AlphaNum = 10; AlphaNum <= NumAlphas; ++AlphaNum) {
                    WaterEquipNum = UtilityRoutines::FindItemInList(DataIPShortCuts::cAlphaArgs(AlphaNum), WaterEquipment);

                    if (WaterEquipNum == 0) {
                        ShowSevereError("Invalid " + DataIPShortCuts::cAlphaFieldNames(AlphaNum) + '=' + DataIPShortCuts::cAlphaArgs(AlphaNum));
                        ShowContinueError("Entered in " + DataIPShortCuts::cCurrentModuleObject + '=' + DataIPShortCuts::cAlphaArgs(1));
                        ErrorsFound = true;
                    } else {
                        if (WaterEquipment(WaterEquipNum).Connections > 0) {
                            ShowSevereError(DataIPShortCuts::cCurrentModuleObject + " = " + DataIPShortCuts::cAlphaArgs(1) +
                                            ":  WaterUse:Equipment = " +
                                            DataIPShortCuts::cAlphaArgs(AlphaNum) +
                                            " is already referenced by another object.");
                            ErrorsFound = true;
                        } else {
                            WaterEquipment(WaterEquipNum).Connections = WaterConnNum;

                            ++WaterConnections(WaterConnNum).NumWaterEquipment;
                            WaterConnections(WaterConnNum).WaterEquipment(WaterConnections(WaterConnNum).NumWaterEquipment) = WaterEquipNum;

                            WaterConnections(WaterConnNum).PeakVolFlowRate +=
                                WaterEquipment(WaterEquipNum).PeakVolFlowRate; // this does not include possible multipliers
                        }
                    }
                }

            } // WaterConnNum

            if (ErrorsFound) ShowFatalError("Errors found in processing input for " + DataIPShortCuts::cCurrentModuleObject);

            if (modNumWaterConnections > 0) {
                CheckEquipName.allocate(modNumWaterConnections);
                CheckPlantLoop.allocate(modNumWaterConnections);
                CheckEquipName = true;
                CheckPlantLoop = true;
            }
        }

        // determine connection's peak mass flow rates.
        if (modNumWaterConnections > 0) {
            for (WaterConnNum = 1; WaterConnNum <= modNumWaterConnections; ++WaterConnNum) {
                WaterConnections(WaterConnNum).PeakMassFlowRate = 0.0;
                for (WaterEquipNum = 1; WaterEquipNum <= WaterConnections(WaterConnNum).NumWaterEquipment; ++WaterEquipNum) {
                    thisWaterEquipNum = WaterConnections(WaterConnNum).WaterEquipment(WaterEquipNum);
                    if (WaterEquipment(thisWaterEquipNum).Zone > 0) {
                        WaterConnections(WaterConnNum).PeakMassFlowRate +=
                            WaterEquipment(thisWaterEquipNum).PeakVolFlowRate * Psychrometrics::RhoH2O(DataGlobals::InitConvTemp) *
                            DataHeatBalance::Zone(WaterEquipment(thisWaterEquipNum).Zone).Multiplier * DataHeatBalance::Zone(WaterEquipment(thisWaterEquipNum).Zone).ListMultiplier;
                    } else { // can't have multipliers
                        WaterConnections(WaterConnNum).PeakMassFlowRate +=
                            WaterEquipment(thisWaterEquipNum).PeakVolFlowRate * Psychrometrics::RhoH2O(DataGlobals::InitConvTemp);
                    }
                }
                PlantUtilities::RegisterPlantCompDesignFlow(WaterConnections(WaterConnNum).InletNode,
                                            WaterConnections(WaterConnNum).PeakMassFlowRate / Psychrometrics::RhoH2O(DataGlobals::InitConvTemp));
            }
        }

        // Setup EQUIPMENT report variables (now that connections have been established)
        // CurrentModuleObject='WaterUse:Equipment'
        for (WaterEquipNum = 1; WaterEquipNum <= modNumWaterEquipment; ++WaterEquipNum) {

            SetupOutputVariable("Water Use Equipment Hot Water Mass Flow Rate",
                                OutputProcessor::Unit::kg_s,
                                WaterEquipment(WaterEquipNum).HotMassFlowRate,
                                "System",
                                "Average",
                                WaterEquipment(WaterEquipNum).Name);

            SetupOutputVariable("Water Use Equipment Cold Water Mass Flow Rate",
                                OutputProcessor::Unit::kg_s,
                                WaterEquipment(WaterEquipNum).ColdMassFlowRate,
                                "System",
                                "Average",
                                WaterEquipment(WaterEquipNum).Name);

            SetupOutputVariable("Water Use Equipment Total Mass Flow Rate",
                                OutputProcessor::Unit::kg_s,
                                WaterEquipment(WaterEquipNum).TotalMassFlowRate,
                                "System",
                                "Average",
                                WaterEquipment(WaterEquipNum).Name);

            SetupOutputVariable("Water Use Equipment Hot Water Volume Flow Rate",
                                OutputProcessor::Unit::m3_s,
                                WaterEquipment(WaterEquipNum).HotVolFlowRate,
                                "System",
                                "Average",
                                WaterEquipment(WaterEquipNum).Name);

            SetupOutputVariable("Water Use Equipment Cold Water Volume Flow Rate",
                                OutputProcessor::Unit::m3_s,
                                WaterEquipment(WaterEquipNum).ColdVolFlowRate,
                                "System",
                                "Average",
                                WaterEquipment(WaterEquipNum).Name);

            SetupOutputVariable("Water Use Equipment Total Volume Flow Rate",
                                OutputProcessor::Unit::m3_s,
                                WaterEquipment(WaterEquipNum).TotalVolFlowRate,
                                "System",
                                "Average",
                                WaterEquipment(WaterEquipNum).Name);

            SetupOutputVariable("Water Use Equipment Hot Water Volume",
                                OutputProcessor::Unit::m3,
                                WaterEquipment(WaterEquipNum).HotVolume,
                                "System",
                                "Sum",
                                WaterEquipment(WaterEquipNum).Name);

            SetupOutputVariable("Water Use Equipment Cold Water Volume",
                                OutputProcessor::Unit::m3,
                                WaterEquipment(WaterEquipNum).ColdVolume,
                                "System",
                                "Sum",
                                WaterEquipment(WaterEquipNum).Name);

            SetupOutputVariable("Water Use Equipment Total Volume",
                                OutputProcessor::Unit::m3,
                                WaterEquipment(WaterEquipNum).TotalVolume,
                                "System",
                                "Sum",
                                WaterEquipment(WaterEquipNum).Name,
                                _,
                                "Water",
                                "WATERSYSTEMS",
                                WaterEquipment(WaterEquipNum).EndUseSubcatName,
                                "Plant");
            SetupOutputVariable("Water Use Equipment Mains Water Volume",
                                OutputProcessor::Unit::m3,
                                WaterEquipment(WaterEquipNum).TotalVolume,
                                "System",
                                "Sum",
                                WaterEquipment(WaterEquipNum).Name,
                                _,
                                "MainsWater",
                                "WATERSYSTEMS",
                                WaterEquipment(WaterEquipNum).EndUseSubcatName,
                                "Plant");

            SetupOutputVariable("Water Use Equipment Hot Water Temperature",
                                OutputProcessor::Unit::C,
                                WaterEquipment(WaterEquipNum).HotTemp,
                                "System",
                                "Average",
                                WaterEquipment(WaterEquipNum).Name);

            SetupOutputVariable("Water Use Equipment Cold Water Temperature",
                                OutputProcessor::Unit::C,
                                WaterEquipment(WaterEquipNum).ColdTemp,
                                "System",
                                "Average",
                                WaterEquipment(WaterEquipNum).Name);

            SetupOutputVariable("Water Use Equipment Target Water Temperature",
                                OutputProcessor::Unit::C,
                                WaterEquipment(WaterEquipNum).TargetTemp,
                                "System",
                                "Average",
                                WaterEquipment(WaterEquipNum).Name);

            SetupOutputVariable("Water Use Equipment Mixed Water Temperature",
                                OutputProcessor::Unit::C,
                                WaterEquipment(WaterEquipNum).MixedTemp,
                                "System",
                                "Average",
                                WaterEquipment(WaterEquipNum).Name);

            SetupOutputVariable("Water Use Equipment Drain Water Temperature",
                                OutputProcessor::Unit::C,
                                WaterEquipment(WaterEquipNum).DrainTemp,
                                "System",
                                "Average",
                                WaterEquipment(WaterEquipNum).Name);

            SetupOutputVariable("Water Use Equipment Heating Rate",
                                OutputProcessor::Unit::W,
                                WaterEquipment(WaterEquipNum).Power,
                                "System",
                                "Average",
                                WaterEquipment(WaterEquipNum).Name);

            if (WaterEquipment(WaterEquipNum).Connections == 0) {
                SetupOutputVariable("Water Use Equipment Heating Energy",
                                    OutputProcessor::Unit::J,
                                    WaterEquipment(WaterEquipNum).Energy,
                                    "System",
                                    "Sum",
                                    WaterEquipment(WaterEquipNum).Name,
                                    _,
                                    "DISTRICTHEATING",
                                    "WATERSYSTEMS",
                                    WaterEquipment(WaterEquipNum).EndUseSubcatName,
                                    "Plant");

            } else if (WaterConnections(WaterEquipment(WaterEquipNum).Connections).StandAlone) {
                SetupOutputVariable("Water Use Equipment Heating Energy",
                                    OutputProcessor::Unit::J,
                                    WaterEquipment(WaterEquipNum).Energy,
                                    "System",
                                    "Sum",
                                    WaterEquipment(WaterEquipNum).Name,
                                    _,
                                    "DISTRICTHEATING",
                                    "WATERSYSTEMS",
                                    WaterEquipment(WaterEquipNum).EndUseSubcatName,
                                    "Plant");

            } else { // The EQUIPMENT is coupled to a plant loop via a CONNECTIONS object
                SetupOutputVariable("Water Use Equipment Heating Energy",
                                    OutputProcessor::Unit::J,
                                    WaterEquipment(WaterEquipNum).Energy,
                                    "System",
                                    "Sum",
                                    WaterEquipment(WaterEquipNum).Name,
                                    _,
                                    "ENERGYTRANSFER",
                                    "WATERSYSTEMS",
                                    WaterEquipment(WaterEquipNum).EndUseSubcatName,
                                    "Plant");
            }

            if (WaterEquipment(WaterEquipNum).Zone > 0) {
                SetupOutputVariable("Water Use Equipment Zone Sensible Heat Gain Rate",
                                    OutputProcessor::Unit::W,
                                    WaterEquipment(WaterEquipNum).SensibleRate,
                                    "System",
                                    "Average",
                                    WaterEquipment(WaterEquipNum).Name);
                SetupOutputVariable("Water Use Equipment Zone Sensible Heat Gain Energy",
                                    OutputProcessor::Unit::J,
                                    WaterEquipment(WaterEquipNum).SensibleEnergy,
                                    "System",
                                    "Sum",
                                    WaterEquipment(WaterEquipNum).Name);

                SetupOutputVariable("Water Use Equipment Zone Latent Gain Rate",
                                    OutputProcessor::Unit::W,
                                    WaterEquipment(WaterEquipNum).LatentRate,
                                    "System",
                                    "Average",
                                    WaterEquipment(WaterEquipNum).Name);
                SetupOutputVariable("Water Use Equipment Zone Latent Gain Energy",
                                    OutputProcessor::Unit::J,
                                    WaterEquipment(WaterEquipNum).LatentEnergy,
                                    "System",
                                    "Sum",
                                    WaterEquipment(WaterEquipNum).Name);

                SetupOutputVariable("Water Use Equipment Zone Moisture Gain Mass Flow Rate",
                                    OutputProcessor::Unit::kg_s,
                                    WaterEquipment(WaterEquipNum).MoistureRate,
                                    "System",
                                    "Average",
                                    WaterEquipment(WaterEquipNum).Name);
                SetupOutputVariable("Water Use Equipment Zone Moisture Gain Mass",
                                    OutputProcessor::Unit::kg,
                                    WaterEquipment(WaterEquipNum).MoistureMass,
                                    "System",
                                    "Sum",
                                    WaterEquipment(WaterEquipNum).Name);

                SetupZoneInternalGain(WaterEquipment(WaterEquipNum).Zone,
                                      "WaterUse:Equipment",
                                      WaterEquipment(WaterEquipNum).Name,
                                      DataHeatBalance::IntGainTypeOf_WaterUseEquipment,
                                      WaterEquipment(WaterEquipNum).SensibleRateNoMultiplier,
                                      _,
                                      _,
                                      WaterEquipment(WaterEquipNum).LatentRateNoMultiplier);
            }

        } // WaterEquipNum

        // Setup CONNECTIONS report variables (don't put any on meters; they are metered at WATER USE EQUIPMENT level)
        // CurrentModuleObject='WaterUse:Connections'
        for (WaterConnNum = 1; WaterConnNum <= modNumWaterConnections; ++WaterConnNum) {

            SetupOutputVariable("Water Use Connections Hot Water Mass Flow Rate",
                                OutputProcessor::Unit::kg_s,
                                WaterConnections(WaterConnNum).HotMassFlowRate,
                                "System",
                                "Average",
                                WaterConnections(WaterConnNum).Name);

            SetupOutputVariable("Water Use Connections Cold Water Mass Flow Rate",
                                OutputProcessor::Unit::kg_s,
                                WaterConnections(WaterConnNum).ColdMassFlowRate,
                                "System",
                                "Average",
                                WaterConnections(WaterConnNum).Name);

            SetupOutputVariable("Water Use Connections Total Mass Flow Rate",
                                OutputProcessor::Unit::kg_s,
                                WaterConnections(WaterConnNum).TotalMassFlowRate,
                                "System",
                                "Average",
                                WaterConnections(WaterConnNum).Name);

            SetupOutputVariable("Water Use Connections Drain Water Mass Flow Rate",
                                OutputProcessor::Unit::kg_s,
                                WaterConnections(WaterConnNum).DrainMassFlowRate,
                                "System",
                                "Average",
                                WaterConnections(WaterConnNum).Name);

            SetupOutputVariable("Water Use Connections Heat Recovery Mass Flow Rate",
                                OutputProcessor::Unit::kg_s,
                                WaterConnections(WaterConnNum).RecoveryMassFlowRate,
                                "System",
                                "Average",
                                WaterConnections(WaterConnNum).Name);

            SetupOutputVariable("Water Use Connections Hot Water Volume Flow Rate",
                                OutputProcessor::Unit::m3_s,
                                WaterConnections(WaterConnNum).HotVolFlowRate,
                                "System",
                                "Average",
                                WaterConnections(WaterConnNum).Name);

            SetupOutputVariable("Water Use Connections Cold Water Volume Flow Rate",
                                OutputProcessor::Unit::m3_s,
                                WaterConnections(WaterConnNum).ColdVolFlowRate,
                                "System",
                                "Average",
                                WaterConnections(WaterConnNum).Name);

            SetupOutputVariable("Water Use Connections Total Volume Flow Rate",
                                OutputProcessor::Unit::m3_s,
                                WaterConnections(WaterConnNum).TotalVolFlowRate,
                                "System",
                                "Average",
                                WaterConnections(WaterConnNum).Name);

            SetupOutputVariable("Water Use Connections Hot Water Volume",
                                OutputProcessor::Unit::m3,
                                WaterConnections(WaterConnNum).HotVolume,
                                "System",
                                "Sum",
                                WaterConnections(WaterConnNum).Name);

            SetupOutputVariable("Water Use Connections Cold Water Volume",
                                OutputProcessor::Unit::m3,
                                WaterConnections(WaterConnNum).ColdVolume,
                                "System",
                                "Sum",
                                WaterConnections(WaterConnNum).Name);

            SetupOutputVariable("Water Use Connections Total Volume",
                                OutputProcessor::Unit::m3,
                                WaterConnections(WaterConnNum).TotalVolume,
                                "System",
                                "Sum",
                                WaterConnections(WaterConnNum).Name); //, &
            // ResourceTypeKey='Water', EndUseKey='DHW', EndUseSubKey=EndUseSubcategoryName, GroupKey='Plant')
            // tHIS WAS double counting

            SetupOutputVariable("Water Use Connections Hot Water Temperature",
                                OutputProcessor::Unit::C,
                                WaterConnections(WaterConnNum).HotTemp,
                                "System",
                                "Average",
                                WaterConnections(WaterConnNum).Name);

            SetupOutputVariable("Water Use Connections Cold Water Temperature",
                                OutputProcessor::Unit::C,
                                WaterConnections(WaterConnNum).ColdTemp,
                                "System",
                                "Average",
                                WaterConnections(WaterConnNum).Name);

            SetupOutputVariable("Water Use Connections Drain Water Temperature",
                                OutputProcessor::Unit::C,
                                WaterConnections(WaterConnNum).DrainTemp,
                                "System",
                                "Average",
                                WaterConnections(WaterConnNum).Name);

            SetupOutputVariable("Water Use Connections Return Water Temperature",
                                OutputProcessor::Unit::C,
                                WaterConnections(WaterConnNum).ReturnTemp,
                                "System",
                                "Average",
                                WaterConnections(WaterConnNum).Name);

            SetupOutputVariable("Water Use Connections Waste Water Temperature",
                                OutputProcessor::Unit::C,
                                WaterConnections(WaterConnNum).WasteTemp,
                                "System",
                                "Average",
                                WaterConnections(WaterConnNum).Name);

            SetupOutputVariable("Water Use Connections Heat Recovery Water Temperature",
                                OutputProcessor::Unit::C,
                                WaterConnections(WaterConnNum).RecoveryTemp,
                                "System",
                                "Average",
                                WaterConnections(WaterConnNum).Name);

            SetupOutputVariable("Water Use Connections Heat Recovery Effectiveness",
                                OutputProcessor::Unit::None,
                                WaterConnections(WaterConnNum).Effectiveness,
                                "System",
                                "Average",
                                WaterConnections(WaterConnNum).Name);

            SetupOutputVariable("Water Use Connections Heat Recovery Rate",
                                OutputProcessor::Unit::W,
                                WaterConnections(WaterConnNum).RecoveryRate,
                                "System",
                                "Average",
                                WaterConnections(WaterConnNum).Name);
            SetupOutputVariable("Water Use Connections Heat Recovery Energy",
                                OutputProcessor::Unit::J,
                                WaterConnections(WaterConnNum).RecoveryEnergy,
                                "System",
                                "Sum",
                                WaterConnections(WaterConnNum).Name);
            // Does this go on a meter?

            // To do:  Add report variable for starved flow when tank can't deliver?

            if (!WaterConnections(WaterConnNum).StandAlone) {
                SetupOutputVariable("Water Use Connections Plant Hot Water Energy",
                                    OutputProcessor::Unit::J,
                                    WaterConnections(WaterConnNum).Energy,
                                    "System",
                                    "Sum",
                                    WaterConnections(WaterConnNum).Name,
                                    _,
                                    "PLANTLOOPHEATINGDEMAND",
                                    "WATERSYSTEMS",
                                    _,
                                    "Plant");
            }

        } // WaterConnNum
    }

    void CalcEquipmentFlowRates(int WaterEquipNum)
    {

        // SUBROUTINE INFORMATION:
        //       AUTHOR         Peter Graham Ellis
        //       DATE WRITTEN   August 2006
        //       MODIFIED       na
        //       RE-ENGINEERED  na

        // PURPOSE OF THIS SUBROUTINE:
        // Calculate desired hot and cold water flow rates


        // SUBROUTINE LOCAL VARIABLE DECLARATIONS:
        int WaterConnNum;

        // FLOW:
        WaterConnNum = WaterEquipment(WaterEquipNum).Connections;

        if (WaterConnNum > 0) {
            // Get water temperature conditions from the CONNECTIONS object
            WaterEquipment(WaterEquipNum).ColdTemp = WaterConnections(WaterConnNum).ColdTemp;
            WaterEquipment(WaterEquipNum).HotTemp = WaterConnections(WaterConnNum).HotTemp;

        } else {
            // Get water temperature conditions from the WATER USE EQUIPMENT schedules
            if (WaterEquipment(WaterEquipNum).ColdTempSchedule > 0) {
                WaterEquipment(WaterEquipNum).ColdTemp = ScheduleManager::GetCurrentScheduleValue(WaterEquipment(WaterEquipNum).ColdTempSchedule);
            } else { // If no ColdTempSchedule, use the mains temperature
                WaterEquipment(WaterEquipNum).ColdTemp = DataEnvironment::WaterMainsTemp;
            }

            if (WaterEquipment(WaterEquipNum).HotTempSchedule > 0) {
                WaterEquipment(WaterEquipNum).HotTemp = ScheduleManager::GetCurrentScheduleValue(WaterEquipment(WaterEquipNum).HotTempSchedule);
            } else { // If no HotTempSchedule, use all cold water
                WaterEquipment(WaterEquipNum).HotTemp = WaterEquipment(WaterEquipNum).ColdTemp;
            }
        }

        if (WaterEquipment(WaterEquipNum).TargetTempSchedule > 0) {
            WaterEquipment(WaterEquipNum).TargetTemp = ScheduleManager::GetCurrentScheduleValue(WaterEquipment(WaterEquipNum).TargetTempSchedule);
        } else { // If no TargetTempSchedule, use all hot water
            WaterEquipment(WaterEquipNum).TargetTemp = WaterEquipment(WaterEquipNum).HotTemp;
        }

        // Get the requested total flow rate
        // 11-17-2006 BG Added multipliers in next block
        if (WaterEquipment(WaterEquipNum).Zone > 0) {
            if (WaterEquipment(WaterEquipNum).FlowRateFracSchedule > 0) {
                WaterEquipment(WaterEquipNum).TotalVolFlowRate =
                    WaterEquipment(WaterEquipNum).PeakVolFlowRate *
                    ScheduleManager::GetCurrentScheduleValue(WaterEquipment(WaterEquipNum).FlowRateFracSchedule) *
                    DataHeatBalance::Zone(WaterEquipment(WaterEquipNum).Zone).Multiplier * DataHeatBalance::Zone(WaterEquipment(WaterEquipNum).Zone).ListMultiplier;
            } else {
                WaterEquipment(WaterEquipNum).TotalVolFlowRate = WaterEquipment(WaterEquipNum).PeakVolFlowRate *
                                                                 DataHeatBalance::Zone(WaterEquipment(WaterEquipNum).Zone).Multiplier *
                                                                 DataHeatBalance::Zone(WaterEquipment(WaterEquipNum).Zone).ListMultiplier;
            }
        } else {
            if (WaterEquipment(WaterEquipNum).FlowRateFracSchedule > 0) {
                WaterEquipment(WaterEquipNum).TotalVolFlowRate =
                    WaterEquipment(WaterEquipNum).PeakVolFlowRate *
                    ScheduleManager::GetCurrentScheduleValue(WaterEquipment(WaterEquipNum).FlowRateFracSchedule);
            } else {
                WaterEquipment(WaterEquipNum).TotalVolFlowRate = WaterEquipment(WaterEquipNum).PeakVolFlowRate;
            }
        }

        WaterEquipment(WaterEquipNum).TotalMassFlowRate =
            WaterEquipment(WaterEquipNum).TotalVolFlowRate * Psychrometrics::RhoH2O(DataGlobals::InitConvTemp);

        // Calculate hot and cold water mixing at the tap
        if (WaterEquipment(WaterEquipNum).TotalMassFlowRate > 0.0) {
            // Calculate the flow rates needed to meet the target temperature
            if (WaterEquipment(WaterEquipNum).HotTemp == WaterEquipment(WaterEquipNum).ColdTemp) { // Avoid divide by zero
                // There is no hot water
                WaterEquipment(WaterEquipNum).HotMassFlowRate = 0.0;

                // Need a special case for HotTemp < ColdTemp, due to bad user input  (but could happen in a plant loop accidentally)

            } else if (WaterEquipment(WaterEquipNum).TargetTemp > WaterEquipment(WaterEquipNum).HotTemp) {
                WaterEquipment(WaterEquipNum).HotMassFlowRate = WaterEquipment(WaterEquipNum).TotalMassFlowRate;

            } else {
                WaterEquipment(WaterEquipNum).HotMassFlowRate = WaterEquipment(WaterEquipNum).TotalMassFlowRate *
                                                                (WaterEquipment(WaterEquipNum).TargetTemp - WaterEquipment(WaterEquipNum).ColdTemp) /
                                                                (WaterEquipment(WaterEquipNum).HotTemp - WaterEquipment(WaterEquipNum).ColdTemp);
            }

            if (WaterEquipment(WaterEquipNum).HotMassFlowRate < 0.0) {
                // Target temp is colder than the cold water temp; don't allow colder
                WaterEquipment(WaterEquipNum).HotMassFlowRate = 0.0;
            }

            WaterEquipment(WaterEquipNum).ColdMassFlowRate =
                WaterEquipment(WaterEquipNum).TotalMassFlowRate - WaterEquipment(WaterEquipNum).HotMassFlowRate;

            if (WaterEquipment(WaterEquipNum).ColdMassFlowRate < 0.0) WaterEquipment(WaterEquipNum).ColdMassFlowRate = 0.0;

            WaterEquipment(WaterEquipNum).MixedTemp = (WaterEquipment(WaterEquipNum).ColdMassFlowRate * WaterEquipment(WaterEquipNum).ColdTemp +
                                                       WaterEquipment(WaterEquipNum).HotMassFlowRate * WaterEquipment(WaterEquipNum).HotTemp) /
                                                      WaterEquipment(WaterEquipNum).TotalMassFlowRate;
        } else {
            WaterEquipment(WaterEquipNum).HotMassFlowRate = 0.0;
            WaterEquipment(WaterEquipNum).ColdMassFlowRate = 0.0;
            WaterEquipment(WaterEquipNum).MixedTemp = WaterEquipment(WaterEquipNum).TargetTemp;
        }
    }

    void CalcEquipmentDrainTemp(int WaterEquipNum)
    {

        // SUBROUTINE INFORMATION:
        //       AUTHOR         Peter Graham Ellis
        //       DATE WRITTEN   August 2006
        //       MODIFIED       na
        //       RE-ENGINEERED  na

        // PURPOSE OF THIS SUBROUTINE:
        // Calculate drainwater temperature and heat and moisture gains to zone.


        // Locals
        // SUBROUTINE ARGUMENT DEFINITIONS:

        // SUBROUTINE LOCAL VARIABLE DECLARATIONS:
        int ZoneNum;
        Real64 ZoneMAT;
        Real64 ZoneHumRat;
        Real64 ZoneHumRatSat;
        Real64 RhoAirDry;
        Real64 ZoneMassMax;
        Real64 FlowMassMax;
        Real64 MoistureMassMax;

        static std::string const RoutineName("CalcEquipmentDrainTemp");

        // FLOW:

        WaterEquipment(WaterEquipNum).SensibleRate = 0.0;
        WaterEquipment(WaterEquipNum).SensibleEnergy = 0.0;
        WaterEquipment(WaterEquipNum).LatentRate = 0.0;
        WaterEquipment(WaterEquipNum).LatentEnergy = 0.0;

        if ((WaterEquipment(WaterEquipNum).Zone == 0) || (WaterEquipment(WaterEquipNum).TotalMassFlowRate == 0.0)) {
            WaterEquipment(WaterEquipNum).DrainTemp = WaterEquipment(WaterEquipNum).MixedTemp;
            WaterEquipment(WaterEquipNum).DrainMassFlowRate = WaterEquipment(WaterEquipNum).TotalMassFlowRate;

        } else {
            ZoneNum = WaterEquipment(WaterEquipNum).Zone;
            ZoneMAT = DataHeatBalFanSys::MAT(ZoneNum);

            if (WaterEquipment(WaterEquipNum).SensibleFracSchedule == 0) {
                WaterEquipment(WaterEquipNum).SensibleRate = 0.0;
                WaterEquipment(WaterEquipNum).SensibleEnergy = 0.0;
            } else {
                WaterEquipment(WaterEquipNum).SensibleRate =
                    ScheduleManager::GetCurrentScheduleValue(WaterEquipment(WaterEquipNum).SensibleFracSchedule) *
                    WaterEquipment(WaterEquipNum).TotalMassFlowRate *
                    Psychrometrics::CPHW(DataGlobals::InitConvTemp) *
                                                             (WaterEquipment(WaterEquipNum).MixedTemp - ZoneMAT);
                WaterEquipment(WaterEquipNum).SensibleEnergy =
                    WaterEquipment(WaterEquipNum).SensibleRate * DataHVACGlobals::TimeStepSys * DataGlobals::SecInHour;
            }

            if (WaterEquipment(WaterEquipNum).LatentFracSchedule == 0) {
                WaterEquipment(WaterEquipNum).LatentRate = 0.0;
                WaterEquipment(WaterEquipNum).LatentEnergy = 0.0;
            } else {
                ZoneHumRat = DataHeatBalFanSys::ZoneAirHumRat(ZoneNum);
                ZoneHumRatSat =
                    Psychrometrics::PsyWFnTdbRhPb(ZoneMAT, 1.0, DataEnvironment::OutBaroPress, RoutineName); // Humidratio at 100% relative humidity
                RhoAirDry = Psychrometrics::PsyRhoAirFnPbTdbW(DataEnvironment::OutBaroPress, ZoneMAT, 0.0);

                ZoneMassMax =
                    (ZoneHumRatSat - ZoneHumRat) * RhoAirDry * DataHeatBalance::Zone(ZoneNum).Volume;    // Max water that can be evaporated to zone
                FlowMassMax =
                    WaterEquipment(WaterEquipNum).TotalMassFlowRate * DataHVACGlobals::TimeStepSys * DataGlobals::SecInHour; // Max water in flow
                MoistureMassMax = min(ZoneMassMax, FlowMassMax);

                WaterEquipment(WaterEquipNum).MoistureMass =
                    ScheduleManager::GetCurrentScheduleValue(WaterEquipment(WaterEquipNum).LatentFracSchedule) * MoistureMassMax;
                WaterEquipment(WaterEquipNum).MoistureRate =
                    WaterEquipment(WaterEquipNum).MoistureMass / (DataHVACGlobals::TimeStepSys * DataGlobals::SecInHour);

                WaterEquipment(WaterEquipNum).LatentRate =
                    WaterEquipment(WaterEquipNum).MoistureRate * Psychrometrics::PsyHfgAirFnWTdb(ZoneHumRat, ZoneMAT);
                WaterEquipment(WaterEquipNum).LatentEnergy =
                    WaterEquipment(WaterEquipNum).LatentRate * DataHVACGlobals::TimeStepSys * DataGlobals::SecInHour;
            }

            WaterEquipment(WaterEquipNum).DrainMassFlowRate =
                WaterEquipment(WaterEquipNum).TotalMassFlowRate - WaterEquipment(WaterEquipNum).MoistureRate;

            if (WaterEquipment(WaterEquipNum).DrainMassFlowRate == 0.0) {
                WaterEquipment(WaterEquipNum).DrainTemp = WaterEquipment(WaterEquipNum).MixedTemp;
            } else {
                WaterEquipment(WaterEquipNum).DrainTemp =
                    (WaterEquipment(WaterEquipNum).TotalMassFlowRate * Psychrometrics::CPHW(DataGlobals::InitConvTemp) *
                         WaterEquipment(WaterEquipNum).MixedTemp -
                     WaterEquipment(WaterEquipNum).SensibleRate - WaterEquipment(WaterEquipNum).LatentRate) /
                    (WaterEquipment(WaterEquipNum).DrainMassFlowRate * Psychrometrics::CPHW(DataGlobals::InitConvTemp));
            }
        }
    }

    void InitConnections(int WaterConnNum)
    {

        // SUBROUTINE INFORMATION:
        //       AUTHOR         Peter Graham Ellis
        //       DATE WRITTEN   August 2006
        //       MODIFIED       Brent Griffith 2010, demand side update
        //       RE-ENGINEERED  na


        // SUBROUTINE LOCAL VARIABLE DECLARATIONS:
        int InletNode;
        int OutletNode;
        bool MyOneTimeFlag(true);      // one time flag                    !DSU
        Array1D_bool SetLoopIndexFlag; // get loop number flag             !DSU
        bool errFlag;

        if (MyOneTimeFlag) {                                       // DSU
            SetLoopIndexFlag.dimension(modNumWaterConnections, true); // DSU
            MyOneTimeFlag = false;                                 // DSU
        }                                                          // DSU

        if (SetLoopIndexFlag(WaterConnNum)) {                                         // DSU
            if (allocated(DataPlant::PlantLoop) && !WaterConnections(WaterConnNum).StandAlone) { // DSU
                errFlag = false;
                PlantUtilities::ScanPlantLoopsForObject(WaterConnections(WaterConnNum).Name,
                                        DataPlant::TypeOf_WaterUseConnection,
                                        WaterConnections(WaterConnNum).PlantLoopNum,
                                        WaterConnections(WaterConnNum).PlantLoopSide,
                                        WaterConnections(WaterConnNum).PlantLoopBranchNum,
                                        WaterConnections(WaterConnNum).PlantLoopCompNum,
                                        errFlag,
                                        _,
                                        _,
                                        _,
                                        _,
                                        _);                                                        // DSU | DSU | DSU | DSU | DSU | DSU | DSU
                if (errFlag) {                                                                           // DSU
                    ShowFatalError("InitConnections: Program terminated due to previous condition(s)."); // DSU
                }                                                                                        // DSU
                SetLoopIndexFlag(WaterConnNum) = false;                                                  // DSU
            }                                                                                            // DSU
            if (WaterConnections(WaterConnNum).StandAlone) SetLoopIndexFlag(WaterConnNum) = false;
        }

        // Set the cold water temperature
        if (WaterConnections(WaterConnNum).SupplyTankNum > 0) {
            WaterConnections(WaterConnNum).ColdSupplyTemp = DataWater::WaterStorage(WaterConnections(WaterConnNum).SupplyTankNum).Twater;

        } else if (WaterConnections(WaterConnNum).ColdTempSchedule > 0) {
            WaterConnections(WaterConnNum).ColdSupplyTemp = ScheduleManager::GetCurrentScheduleValue(WaterConnections(WaterConnNum).ColdTempSchedule);

        } else {
            WaterConnections(WaterConnNum).ColdSupplyTemp = DataEnvironment::WaterMainsTemp;
        }

        // Initially set ColdTemp to the ColdSupplyTemp; with heat recovery, ColdTemp will change during iteration
        WaterConnections(WaterConnNum).ColdTemp = WaterConnections(WaterConnNum).ColdSupplyTemp;

        // Set the hot water temperature
        if (WaterConnections(WaterConnNum).StandAlone) {
            if (WaterConnections(WaterConnNum).HotTempSchedule > 0) {
                WaterConnections(WaterConnNum).HotTemp = ScheduleManager::GetCurrentScheduleValue(WaterConnections(WaterConnNum).HotTempSchedule);
            } else {
                // If no HotTempSchedule, use all cold water
                WaterConnections(WaterConnNum).HotTemp = WaterConnections(WaterConnNum).ColdTemp;
            }

        } else {
            InletNode = WaterConnections(WaterConnNum).InletNode;
            OutletNode = WaterConnections(WaterConnNum).OutletNode;

            if (DataGlobals::BeginEnvrnFlag && WaterConnections(WaterConnNum).Init) {
                // Clear node initial conditions
                if (InletNode > 0 && OutletNode > 0) {
                    PlantUtilities::InitComponentNodes(0.0,
                                       WaterConnections(WaterConnNum).PeakMassFlowRate,
                                       InletNode,
                                       OutletNode,
                                       WaterConnections(WaterConnNum).PlantLoopNum,
                                       WaterConnections(WaterConnNum).PlantLoopSide,
                                       WaterConnections(WaterConnNum).PlantLoopBranchNum,
                                       WaterConnections(WaterConnNum).PlantLoopCompNum);

                    WaterConnections(WaterConnNum).ReturnTemp = DataLoopNode::Node(InletNode).Temp;
                }

                WaterConnections(WaterConnNum).Init = false;
            }

            if (!DataGlobals::BeginEnvrnFlag) WaterConnections(WaterConnNum).Init = true;

            if (InletNode > 0) {
                if (!DataGlobals::DoingSizing) {
                    WaterConnections(WaterConnNum).HotTemp = DataLoopNode::Node(InletNode).Temp;
                } else {
                    // plant loop will not be running so need a value here.
                    // should change to use tank setpoint but water use connections don't have knowledge of the tank they are fed by
                    WaterConnections(WaterConnNum).HotTemp = 60.0;
                }
            }
        }
    }

    void CalcConnectionsFlowRates(int WaterConnNum, bool FirstHVACIteration)
    {

        // SUBROUTINE INFORMATION:
        //       AUTHOR         Peter Graham Ellis
        //       DATE WRITTEN   August 2006
        //       MODIFIED       na
        //       RE-ENGINEERED  na

        // PURPOSE OF THIS SUBROUTINE:
        // Calculate summed values for WATER USE CONNECTIONS (to prepare to request flow from plant, and for reporting).


        // SUBROUTINE LOCAL VARIABLE DECLARATIONS:
        int WaterEquipNum;
        int Loop;
        int InletNode;
        int OutletNode;
        int LoopNum;
        int LoopSideNum;
        Real64 AvailableFraction;
        Real64 DesiredHotWaterMassFlow; // store original request

        // FLOW:
        WaterConnections(WaterConnNum).ColdMassFlowRate = 0.0;
        WaterConnections(WaterConnNum).HotMassFlowRate = 0.0;

        for (Loop = 1; Loop <= WaterConnections(WaterConnNum).NumWaterEquipment; ++Loop) {
            WaterEquipNum = WaterConnections(WaterConnNum).WaterEquipment(Loop);

            CalcEquipmentFlowRates(WaterEquipNum);

            WaterConnections(WaterConnNum).ColdMassFlowRate += WaterEquipment(WaterEquipNum).ColdMassFlowRate;
            WaterConnections(WaterConnNum).HotMassFlowRate += WaterEquipment(WaterEquipNum).HotMassFlowRate;
        } // Loop

        WaterConnections(WaterConnNum).TotalMassFlowRate =
            WaterConnections(WaterConnNum).ColdMassFlowRate + WaterConnections(WaterConnNum).HotMassFlowRate;

        if (!WaterConnections(WaterConnNum).StandAlone) { // Interact with the plant loop
            InletNode = WaterConnections(WaterConnNum).InletNode;
            OutletNode = WaterConnections(WaterConnNum).OutletNode;
            LoopNum = WaterConnections(WaterConnNum).PlantLoopNum;
            LoopSideNum = WaterConnections(WaterConnNum).PlantLoopSide;
            if (InletNode > 0) {
                if (FirstHVACIteration) {
                    // Request the mass flow rate from the demand side manager
                    //        Node(InletNode)%MassFlowRate = WaterConnections(WaterConnNum)%HotMassFlowRate
                    //        Node(InletNode)%MassFlowRateMaxAvail = WaterConnections(WaterConnNum)%PeakMassFlowRate
                    //        Node(InletNode)%MassFlowRateMinAvail = 0.0D0
                    PlantUtilities::SetComponentFlowRate(WaterConnections(WaterConnNum).HotMassFlowRate,
                                         InletNode,
                                         OutletNode,
                                         LoopNum,
                                         LoopSideNum,
                                         WaterConnections(WaterConnNum).PlantLoopBranchNum,
                                         WaterConnections(WaterConnNum).PlantLoopCompNum);

                } else {
                    DesiredHotWaterMassFlow = WaterConnections(WaterConnNum).HotMassFlowRate;
                    PlantUtilities::SetComponentFlowRate(DesiredHotWaterMassFlow,
                                         InletNode,
                                         OutletNode,
                                         LoopNum,
                                         LoopSideNum,
                                         WaterConnections(WaterConnNum).PlantLoopBranchNum,
                                         WaterConnections(WaterConnNum).PlantLoopCompNum);
                    // DSU3   Node(InletNode)%MassFlowRate = MIN(WaterConnections(WaterConnNum)%HotMassFlowRate, Node(InletNode)%MassFlowRateMaxAvail)
                    // DSU3   Node(InletNode)%MassFlowRate = MAX(WaterConnections(WaterConnNum)%HotMassFlowRate, Node(InletNode)%MassFlowRateMinAvail)
                    // readjust if more than actual available mass flow rate determined by the demand side manager
                    if ((WaterConnections(WaterConnNum).HotMassFlowRate != DesiredHotWaterMassFlow) &&
                        (WaterConnections(WaterConnNum).HotMassFlowRate > 0.0)) { // plant didn't give what was asked for

                        // DSU3   Node(InletNode)%MassFlowRate = Node(InletNode)%MassFlowRateMaxAvail

                        AvailableFraction = DesiredHotWaterMassFlow / WaterConnections(WaterConnNum).HotMassFlowRate;

                        // DSU3    WaterConnections(WaterConnNum)%HotMassFlowRate = Node(InletNode)%MassFlowRateMaxAvail
                        WaterConnections(WaterConnNum).ColdMassFlowRate =
                            WaterConnections(WaterConnNum).TotalMassFlowRate -
                            WaterConnections(WaterConnNum).HotMassFlowRate; // Preserve the total mass flow rate

                        // Proportionally reduce hot water and increase cold water for all WATER USE EQUIPMENT
                        for (Loop = 1; Loop <= WaterConnections(WaterConnNum).NumWaterEquipment; ++Loop) {
                            WaterEquipNum = WaterConnections(WaterConnNum).WaterEquipment(Loop);

                            // Recalculate flow rates for water equipment within connection
                            WaterEquipment(WaterEquipNum).HotMassFlowRate *= AvailableFraction;
                            WaterEquipment(WaterEquipNum).ColdMassFlowRate =
                                WaterEquipment(WaterEquipNum).TotalMassFlowRate - WaterEquipment(WaterEquipNum).HotMassFlowRate;

                            // Recalculate mixed water temperature
                            if (WaterEquipment(WaterEquipNum).TotalMassFlowRate > 0.0) {
                                WaterEquipment(WaterEquipNum).MixedTemp =
                                    (WaterEquipment(WaterEquipNum).ColdMassFlowRate * WaterEquipment(WaterEquipNum).ColdTemp +
                                     WaterEquipment(WaterEquipNum).HotMassFlowRate * WaterEquipment(WaterEquipNum).HotTemp) /
                                    WaterEquipment(WaterEquipNum).TotalMassFlowRate;
                            } else {
                                WaterEquipment(WaterEquipNum).MixedTemp = WaterEquipment(WaterEquipNum).TargetTemp;
                            }
                        } // Loop
                    }
                }
            }
        }

        if (WaterConnections(WaterConnNum).SupplyTankNum > 0) {
            // Set the demand request for supply water from water storage tank
            WaterConnections(WaterConnNum).ColdVolFlowRate =
                WaterConnections(WaterConnNum).ColdMassFlowRate / Psychrometrics::RhoH2O(DataGlobals::InitConvTemp);
            DataWater::WaterStorage(WaterConnections(WaterConnNum).SupplyTankNum).VdotRequestDemand(WaterConnections(WaterConnNum).TankDemandID) =
                WaterConnections(WaterConnNum).ColdVolFlowRate;

            // Check if cold flow rate should be starved by restricted flow from tank
            // Currently, the tank flow is not really starved--water continues to flow at the tank water temperature
            // But the user can see the error by comparing report variables for TankVolFlowRate < ColdVolFlowRate
            WaterConnections(WaterConnNum).TankVolFlowRate = DataWater::WaterStorage(WaterConnections(WaterConnNum).SupplyTankNum)
                                                                 .VdotAvailDemand(WaterConnections(WaterConnNum).TankDemandID);
            WaterConnections(WaterConnNum).TankMassFlowRate =
                WaterConnections(WaterConnNum).TankVolFlowRate * Psychrometrics::RhoH2O(DataGlobals::InitConvTemp);
        }
    }

    void CalcConnectionsDrainTemp(int WaterConnNum)
    {

        // SUBROUTINE INFORMATION:
        //       AUTHOR         Peter Graham Ellis
        //       DATE WRITTEN   August 2006
        //       MODIFIED       na
        //       RE-ENGINEERED  na

        // PURPOSE OF THIS SUBROUTINE:
        // Calculate


        // SUBROUTINE LOCAL VARIABLE DECLARATIONS:
        int WaterEquipNum;
        int Loop;
        Real64 MassFlowTempSum;

        // FLOW:
        WaterConnections(WaterConnNum).DrainMassFlowRate = 0.0;
        MassFlowTempSum = 0.0;

        for (Loop = 1; Loop <= WaterConnections(WaterConnNum).NumWaterEquipment; ++Loop) {
            WaterEquipNum = WaterConnections(WaterConnNum).WaterEquipment(Loop);

            CalcEquipmentDrainTemp(WaterEquipNum);

            WaterConnections(WaterConnNum).DrainMassFlowRate += WaterEquipment(WaterEquipNum).DrainMassFlowRate;
            MassFlowTempSum += WaterEquipment(WaterEquipNum).DrainMassFlowRate * WaterEquipment(WaterEquipNum).DrainTemp;
        } // Loop

        if (WaterConnections(WaterConnNum).DrainMassFlowRate > 0.0) {
            WaterConnections(WaterConnNum).DrainTemp = MassFlowTempSum / WaterConnections(WaterConnNum).DrainMassFlowRate;
        } else {
            WaterConnections(WaterConnNum).DrainTemp = WaterConnections(WaterConnNum).HotTemp;
        }

        WaterConnections(WaterConnNum).DrainVolFlowRate =
            WaterConnections(WaterConnNum).DrainMassFlowRate * Psychrometrics::RhoH2O(DataGlobals::InitConvTemp);
    }

    void CalcConnectionsHeatRecovery(int WaterConnNum)
    {

        // SUBROUTINE INFORMATION:
        //       AUTHOR         Peter Graham Ellis
        //       DATE WRITTEN   August 2006
        //       MODIFIED       na
        //       RE-ENGINEERED  na

        // PURPOSE OF THIS SUBROUTINE:
        // Calculate drainwater heat recovery

        // SUBROUTINE LOCAL VARIABLE DECLARATIONS:
        Real64 CapacityRatio;
        Real64 NTU;
        Real64 ExpVal;
        Real64 HXCapacityRate;
        Real64 DrainCapacityRate;
        Real64 MinCapacityRate;

        // FLOW:
        if (!WaterConnections(WaterConnNum).HeatRecovery) {
            WaterConnections(WaterConnNum).RecoveryTemp = WaterConnections(WaterConnNum).ColdSupplyTemp;
            WaterConnections(WaterConnNum).ReturnTemp = WaterConnections(WaterConnNum).ColdSupplyTemp;
            WaterConnections(WaterConnNum).WasteTemp = WaterConnections(WaterConnNum).DrainTemp;

        } else if (WaterConnections(WaterConnNum).TotalMassFlowRate == 0.0) {
            WaterConnections(WaterConnNum).Effectiveness = 0.0;
            WaterConnections(WaterConnNum).RecoveryRate = 0.0;
            WaterConnections(WaterConnNum).RecoveryTemp = WaterConnections(WaterConnNum).ColdSupplyTemp;
            WaterConnections(WaterConnNum).ReturnTemp = WaterConnections(WaterConnNum).ColdSupplyTemp;
            WaterConnections(WaterConnNum).WasteTemp = WaterConnections(WaterConnNum).DrainTemp;

        } else { // WaterConnections(WaterConnNum)%TotalMassFlowRate > 0.0

            {
                auto const SELECT_CASE_var(WaterConnections(WaterConnNum).HeatRecoveryConfig);
                if (SELECT_CASE_var == HeatRecoveryConfigPlant) {
                    WaterConnections(WaterConnNum).RecoveryMassFlowRate = WaterConnections(WaterConnNum).HotMassFlowRate;
                } else if (SELECT_CASE_var == HeatRecoveryConfigEquipment) {
                    WaterConnections(WaterConnNum).RecoveryMassFlowRate = WaterConnections(WaterConnNum).ColdMassFlowRate;
                } else if (SELECT_CASE_var == HeatRecoveryConfigPlantAndEquip) {
                    WaterConnections(WaterConnNum).RecoveryMassFlowRate = WaterConnections(WaterConnNum).TotalMassFlowRate;
                }
            }

            HXCapacityRate = Psychrometrics::CPHW(DataGlobals::InitConvTemp) * WaterConnections(WaterConnNum).RecoveryMassFlowRate;
            DrainCapacityRate = Psychrometrics::CPHW(DataGlobals::InitConvTemp) * WaterConnections(WaterConnNum).DrainMassFlowRate;
            MinCapacityRate = min(DrainCapacityRate, HXCapacityRate);

            {
                auto const SELECT_CASE_var(WaterConnections(WaterConnNum).HeatRecoveryHX);
                if (SELECT_CASE_var == HeatRecoveryHXIdeal) {
                    WaterConnections(WaterConnNum).Effectiveness = 1.0;

                } else if (SELECT_CASE_var == HeatRecoveryHXCounterFlow) { // Unmixed
                    CapacityRatio = MinCapacityRate / max(DrainCapacityRate, HXCapacityRate);
                    NTU = WaterConnections(WaterConnNum).HXUA / MinCapacityRate;
                    if (CapacityRatio == 1.0) {
                        WaterConnections(WaterConnNum).Effectiveness = NTU / (1.0 + NTU);
                    } else {
                        ExpVal = std::exp(-NTU * (1.0 - CapacityRatio));
                        WaterConnections(WaterConnNum).Effectiveness = (1.0 - ExpVal) / (1.0 - CapacityRatio * ExpVal);
                    }

                } else if (SELECT_CASE_var == HeatRecoveryHXCrossFlow) { // Unmixed
                    CapacityRatio = MinCapacityRate / max(DrainCapacityRate, HXCapacityRate);
                    NTU = WaterConnections(WaterConnNum).HXUA / MinCapacityRate;
                    WaterConnections(WaterConnNum).Effectiveness =
                        1.0 - std::exp((std::pow(NTU, 0.22) / CapacityRatio) * (std::exp(-CapacityRatio * std::pow(NTU, 0.78)) - 1.0));
                }
            }

            WaterConnections(WaterConnNum).RecoveryRate = WaterConnections(WaterConnNum).Effectiveness * MinCapacityRate *
                                                          (WaterConnections(WaterConnNum).DrainTemp - WaterConnections(WaterConnNum).ColdSupplyTemp);

            WaterConnections(WaterConnNum).RecoveryTemp =
                WaterConnections(WaterConnNum).ColdSupplyTemp +
                WaterConnections(WaterConnNum).RecoveryRate /
                    (Psychrometrics::CPHW(DataGlobals::InitConvTemp) * WaterConnections(WaterConnNum).TotalMassFlowRate);

            WaterConnections(WaterConnNum).WasteTemp =
                WaterConnections(WaterConnNum).DrainTemp -
                WaterConnections(WaterConnNum).RecoveryRate /
                    (Psychrometrics::CPHW(DataGlobals::InitConvTemp) * WaterConnections(WaterConnNum).TotalMassFlowRate);

            if (WaterConnections(WaterConnNum).RecoveryTankNum > 0) {
                DataWater::WaterStorage(WaterConnections(WaterConnNum).RecoveryTankNum).VdotAvailSupply(WaterConnections(WaterConnNum).TankSupplyID) =
                    WaterConnections(WaterConnNum).DrainVolFlowRate;
                DataWater::WaterStorage(WaterConnections(WaterConnNum).RecoveryTankNum).TwaterSupply(WaterConnections(WaterConnNum).TankSupplyID) =
                    WaterConnections(WaterConnNum).WasteTemp;
            }

            {
                auto const SELECT_CASE_var(WaterConnections(WaterConnNum).HeatRecoveryConfig);
                if (SELECT_CASE_var == HeatRecoveryConfigPlant) {
                    WaterConnections(WaterConnNum).TempError = 0.0; // No feedback back to the cold supply
                    // WaterConnections(WaterConnNum)%ColdTemp = WaterConnections(WaterConnNum)%ColdSupplyTemp
                    WaterConnections(WaterConnNum).ReturnTemp = WaterConnections(WaterConnNum).RecoveryTemp;

                } else if (SELECT_CASE_var == HeatRecoveryConfigEquipment) {
                    WaterConnections(WaterConnNum).TempError =
                        std::abs(WaterConnections(WaterConnNum).ColdTemp - WaterConnections(WaterConnNum).RecoveryTemp);

                    WaterConnections(WaterConnNum).ColdTemp = WaterConnections(WaterConnNum).RecoveryTemp;
                    WaterConnections(WaterConnNum).ReturnTemp = WaterConnections(WaterConnNum).ColdSupplyTemp;

                } else if (SELECT_CASE_var == HeatRecoveryConfigPlantAndEquip) {
                    WaterConnections(WaterConnNum).TempError =
                        std::abs(WaterConnections(WaterConnNum).ColdTemp - WaterConnections(WaterConnNum).RecoveryTemp);

                    WaterConnections(WaterConnNum).ColdTemp = WaterConnections(WaterConnNum).RecoveryTemp;
                    WaterConnections(WaterConnNum).ReturnTemp = WaterConnections(WaterConnNum).RecoveryTemp;
                }
            }
        }
    }

    void UpdateWaterConnections(int WaterConnNum)
    {

        // SUBROUTINE INFORMATION:
        //       AUTHOR         Peter Graham Ellis
        //       DATE WRITTEN   August 2006
        //       MODIFIED       na
        //       RE-ENGINEERED  na

        // PURPOSE OF THIS SUBROUTINE:
        // Updates the node variables with local variables.

        // SUBROUTINE LOCAL VARIABLE DECLARATIONS:
        int InletNode;
        int OutletNode;
        int LoopNum;

        // FLOW:
        InletNode = WaterConnections(WaterConnNum).InletNode;
        OutletNode = WaterConnections(WaterConnNum).OutletNode;
        LoopNum = WaterConnections(WaterConnNum).PlantLoopNum;

        if (InletNode > 0 && OutletNode > 0) {
            // Pass all variables from inlet to outlet node
            PlantUtilities::SafeCopyPlantNode(InletNode, OutletNode, LoopNum);
            // DSU3 Node(OutletNode) = Node(InletNode)

            // Set outlet node variables that are possibly changed
            DataLoopNode::Node(OutletNode).Temp = WaterConnections(WaterConnNum).ReturnTemp;
            // should add enthalpy update to return?
        }
    }

    void ReportStandAloneWaterUse()
    {

        // SUBROUTINE INFORMATION:
        //       AUTHOR         B. Griffith, Peter Graham Ellis
        //       DATE WRITTEN   Nov. 2011
        //       MODIFIED       Brent Griffith, March 2010 added argument
        //       RE-ENGINEERED  na

        // PURPOSE OF THIS SUBROUTINE:
        // Calculates report variables for stand alone water use


        // Locals
        // SUBROUTINE ARGUMENT DEFINITIONS:

        int WaterEquipNum;

        // FLOW:
        for (WaterEquipNum = 1; WaterEquipNum <= modNumWaterEquipment; ++WaterEquipNum) {
            WaterEquipment(WaterEquipNum).ColdVolFlowRate =
                WaterEquipment(WaterEquipNum).ColdMassFlowRate / Psychrometrics::RhoH2O(DataGlobals::InitConvTemp);
            WaterEquipment(WaterEquipNum).HotVolFlowRate =
                WaterEquipment(WaterEquipNum).HotMassFlowRate / Psychrometrics::RhoH2O(DataGlobals::InitConvTemp);
            WaterEquipment(WaterEquipNum).TotalVolFlowRate =
                WaterEquipment(WaterEquipNum).ColdVolFlowRate + WaterEquipment(WaterEquipNum).HotVolFlowRate;

            WaterEquipment(WaterEquipNum).ColdVolume =
                WaterEquipment(WaterEquipNum).ColdVolFlowRate * DataHVACGlobals::TimeStepSys * DataGlobals::SecInHour;
            WaterEquipment(WaterEquipNum).HotVolume =
                WaterEquipment(WaterEquipNum).HotVolFlowRate * DataHVACGlobals::TimeStepSys * DataGlobals::SecInHour;
            WaterEquipment(WaterEquipNum).TotalVolume =
                WaterEquipment(WaterEquipNum).TotalVolFlowRate * DataHVACGlobals::TimeStepSys * DataGlobals::SecInHour;

            if (WaterEquipment(WaterEquipNum).Connections == 0) {
                WaterEquipment(WaterEquipNum).Power = WaterEquipment(WaterEquipNum).HotMassFlowRate *
                                                      Psychrometrics::CPHW(DataGlobals::InitConvTemp) *
                                                      (WaterEquipment(WaterEquipNum).HotTemp - WaterEquipment(WaterEquipNum).ColdTemp);
            } else {
                WaterEquipment(WaterEquipNum).Power =
                    WaterEquipment(WaterEquipNum).HotMassFlowRate * Psychrometrics::CPHW(DataGlobals::InitConvTemp) *
                    (WaterEquipment(WaterEquipNum).HotTemp - WaterConnections(WaterEquipment(WaterEquipNum).Connections).ReturnTemp);
            }

            WaterEquipment(WaterEquipNum).Energy = WaterEquipment(WaterEquipNum).Power * DataHVACGlobals::TimeStepSys * DataGlobals::SecInHour;
        }
    }

    void ReportWaterUse(int WaterConnNum)
    {

        // SUBROUTINE INFORMATION:
        //       AUTHOR         Peter Graham Ellis
        //       DATE WRITTEN   August 2006
        //       MODIFIED       Brent Griffith, March 2010 added argument
        //       RE-ENGINEERED  na

        // PURPOSE OF THIS SUBROUTINE:
        // Calculates report variables.

        // Locals
        // SUBROUTINE ARGUMENT DEFINITIONS:

        int Loop;
        int WaterEquipNum;

        // FLOW:
        for (Loop = 1; Loop <= WaterConnections(WaterConnNum).NumWaterEquipment; ++Loop) {
            WaterEquipNum = WaterConnections(WaterConnNum).WaterEquipment(Loop);
            WaterEquipment(WaterEquipNum).ColdVolFlowRate =
                WaterEquipment(WaterEquipNum).ColdMassFlowRate / Psychrometrics::RhoH2O(DataGlobals::InitConvTemp);
            WaterEquipment(WaterEquipNum).HotVolFlowRate =
                WaterEquipment(WaterEquipNum).HotMassFlowRate / Psychrometrics::RhoH2O(DataGlobals::InitConvTemp);
            WaterEquipment(WaterEquipNum).TotalVolFlowRate =
                WaterEquipment(WaterEquipNum).ColdVolFlowRate + WaterEquipment(WaterEquipNum).HotVolFlowRate;

            WaterEquipment(WaterEquipNum).ColdVolume =
                WaterEquipment(WaterEquipNum).ColdVolFlowRate * DataHVACGlobals::TimeStepSys * DataGlobals::SecInHour;
            WaterEquipment(WaterEquipNum).HotVolume =
                WaterEquipment(WaterEquipNum).HotVolFlowRate * DataHVACGlobals::TimeStepSys * DataGlobals::SecInHour;
            WaterEquipment(WaterEquipNum).TotalVolume =
                WaterEquipment(WaterEquipNum).TotalVolFlowRate * DataHVACGlobals::TimeStepSys * DataGlobals::SecInHour;

            if (WaterEquipment(WaterEquipNum).Connections == 0) {
                WaterEquipment(WaterEquipNum).Power = WaterEquipment(WaterEquipNum).HotMassFlowRate *
                                                      Psychrometrics::CPHW(DataGlobals::InitConvTemp) *
                                                      (WaterEquipment(WaterEquipNum).HotTemp - WaterEquipment(WaterEquipNum).ColdTemp);
            } else {
                WaterEquipment(WaterEquipNum).Power =
                    WaterEquipment(WaterEquipNum).HotMassFlowRate * Psychrometrics::CPHW(DataGlobals::InitConvTemp) *
                    (WaterEquipment(WaterEquipNum).HotTemp - WaterConnections(WaterEquipment(WaterEquipNum).Connections).ReturnTemp);
            }

            WaterEquipment(WaterEquipNum).Energy = WaterEquipment(WaterEquipNum).Power * DataHVACGlobals::TimeStepSys * DataGlobals::SecInHour;
        }

        WaterConnections(WaterConnNum).ColdVolFlowRate =
            WaterConnections(WaterConnNum).ColdMassFlowRate / Psychrometrics::RhoH2O(DataGlobals::InitConvTemp);
        WaterConnections(WaterConnNum).HotVolFlowRate =
            WaterConnections(WaterConnNum).HotMassFlowRate / Psychrometrics::RhoH2O(DataGlobals::InitConvTemp);
        WaterConnections(WaterConnNum).TotalVolFlowRate =
            WaterConnections(WaterConnNum).ColdVolFlowRate + WaterConnections(WaterConnNum).HotVolFlowRate;

        WaterConnections(WaterConnNum).ColdVolume =
            WaterConnections(WaterConnNum).ColdVolFlowRate * DataHVACGlobals::TimeStepSys * DataGlobals::SecInHour;
        WaterConnections(WaterConnNum).HotVolume =
            WaterConnections(WaterConnNum).HotVolFlowRate * DataHVACGlobals::TimeStepSys * DataGlobals::SecInHour;
        WaterConnections(WaterConnNum).TotalVolume =
            WaterConnections(WaterConnNum).TotalVolFlowRate * DataHVACGlobals::TimeStepSys * DataGlobals::SecInHour;

        WaterConnections(WaterConnNum).Power = WaterConnections(WaterConnNum).HotMassFlowRate * Psychrometrics::CPHW(DataGlobals::InitConvTemp) *
                                               (WaterConnections(WaterConnNum).HotTemp - WaterConnections(WaterConnNum).ReturnTemp);
        WaterConnections(WaterConnNum).Energy = WaterConnections(WaterConnNum).Power * DataHVACGlobals::TimeStepSys * DataGlobals::SecInHour;

        WaterConnections(WaterConnNum).RecoveryEnergy =
            WaterConnections(WaterConnNum).RecoveryRate * DataHVACGlobals::TimeStepSys * DataGlobals::SecInHour;
    }

    void CalcWaterUseZoneGains()
    {

        // SUBROUTINE INFORMATION:
        //       AUTHOR         Peter Graham Ellis
        //       DATE WRITTEN   August 2006
        //       MODIFIED
        //       RE-ENGINEERED  na

        // PURPOSE OF THIS SUBROUTINE:
        // Calculates the zone internal gains due to water use sensible and latent loads.

        // Locals
        // SUBROUTINE LOCAL VARIABLE DECLARATIONS:
        int WaterEquipNum;
        int ZoneNum;
        bool MyEnvrnFlag(true);

        // FLOW:
        if (modNumWaterEquipment == 0) return;

        if (DataGlobals::BeginEnvrnFlag && MyEnvrnFlag) {
            for (auto &e : WaterEquipment) {
                e.SensibleRate = 0.0;
                e.SensibleEnergy = 0.0;
                e.SensibleRateNoMultiplier = 0.0;
                e.LatentRate = 0.0;
                e.LatentEnergy = 0.0;
                e.LatentRateNoMultiplier = 0.0;
                e.MixedTemp = 0.0;
                e.TotalMassFlowRate = 0.0;
                e.DrainTemp = 0.0;
                e.ColdVolFlowRate = 0.0;
                e.HotVolFlowRate = 0.0;
                e.TotalVolFlowRate = 0.0;
                e.ColdMassFlowRate = 0.0;
                e.HotMassFlowRate = 0.0;
            }
            MyEnvrnFlag = false;
        }

        if (!DataGlobals::BeginEnvrnFlag) MyEnvrnFlag = true;

        for (WaterEquipNum = 1; WaterEquipNum <= modNumWaterEquipment; ++WaterEquipNum) {
            if (WaterEquipment(WaterEquipNum).Zone == 0) continue;
            ZoneNum = WaterEquipment(WaterEquipNum).Zone;
            WaterEquipment(WaterEquipNum).SensibleRateNoMultiplier =
                WaterEquipment(WaterEquipNum).SensibleRate /
                (DataHeatBalance::Zone(ZoneNum).Multiplier * DataHeatBalance::Zone(ZoneNum).ListMultiplier); // CR7401, back out multipliers
            WaterEquipment(WaterEquipNum).LatentRateNoMultiplier =
                WaterEquipment(WaterEquipNum).LatentRate /
                (DataHeatBalance::Zone(ZoneNum).Multiplier * DataHeatBalance::Zone(ZoneNum).ListMultiplier); // CR7401, back out multipliers
        }

      
    }

} // namespace WaterUse

} // namespace EnergyPlus
