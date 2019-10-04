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

#ifndef WaterThermalTanks_hh_INCLUDED
#define WaterThermalTanks_hh_INCLUDED

// ObjexxFCL Headers
#include <ObjexxFCL/Array1D.fwd.hh>
#include <ObjexxFCL/Optional.fwd.hh>

// EnergyPlus Headers
#include <EnergyPlus.hh>
#include <VariableSpeedCoils.hh>

namespace EnergyPlus {

namespace WaterThermalTanks {

    extern int const HeatMode;  // heating source is on, source will not turn off until setpoint temp is reached
    extern int const FloatMode; // heating source is off, source will not turn on until cutin temp is reached
    extern int const VentMode;  // tank temp is above maximum temperature and water is venting
    extern int const CoolMode;  // cooling source is on, source will not turn off until setpoint temp is reached

    extern int const AmbientTempSchedule;   // ambient temperature around tank (or HPWH inlet air) is scheduled
    extern int const AmbientTempZone;       // tank is located in a zone or HPWH inlet air is zone air only
    extern int const AmbientTempOutsideAir; // tank is located outdoors or HPWH inlet air is outdoor air only
    extern int const AmbientTempZoneAndOA;  // applicable to HPWH only, inlet air is mixture of OA and zone air

    extern int const CrankcaseTempSchedule; // temperature controlling compressor crankcase heater is scheduled
    extern int const CrankcaseTempZone;     // temperature controlling compressor crankcase heater is zone air
    extern int const CrankcaseTempExterior; // temperature controlling compressor crankcase heater is outdoor air

    extern int const ControlTypeCycle;    // water heater only, cycling heating source control
    extern int const ControlTypeModulate; // water heater only, modulating heating source control

    extern int const TankShapeVertCylinder;  // tank shape is a vertical cylinder
    extern int const TankShapeHorizCylinder; // tank shape is a horizontal cylinder
    extern int const TankShapeOther;         // tank shape has an arbitrary perimeter shape

    extern int const PriorityMasterSlave;  // water heater only, master-slave priority control of heater elements
    extern int const PrioritySimultaneous; // water heater only, simultaneous control of heater elements

    extern int const InletModeFixed;   // water heater only, inlet water always enters at the user-specified height
    extern int const InletModeSeeking; // water heater only, inlet water seeks out the node with the closest temperature

    // reclaim heat object types for Coil:WaterHeating:Desuperheater object
    extern int const COMPRESSORRACK_REFRIGERATEDCASE; // reclaim heating source is refrigerated case compressor rack
    extern int const COIL_DX_COOLING;                 // reclaim heating source is DX cooling coil
    extern int const COIL_DX_MULTISPEED;              // reclaim heating source is DX multispeed coil
    extern int const COIL_DX_MULTIMODE;               // reclaim heating source is DX multimode coil
    extern int const CONDENSER_REFRIGERATION;         // reclaim heating source is detailed refrigeration system condenser
    extern int const COIL_DX_VARIABLE_COOLING;        // reclaim heating source is Variable Speed DX cooling coil
    extern int const COIL_AIR_WATER_HEATPUMP_EQ;      // reclaim heating source is Water to air heat pump cooling coil

    extern int const UseSide;    // Indicates Use side of water heater
    extern int const SourceSide; // Indicates Source side of water heater

    extern int const SizeNotSet;
    extern int const SizePeakDraw;
    extern int const SizeResidentialMin;
    extern int const SizePerPerson;
    extern int const SizePerFloorArea;
    extern int const SizePerUnit;
    extern int const SizePerSolarColArea;

    extern int const SourceSideStorageTank;
    extern int const SourceSideIndirectHeatPrimarySetpoint;
    extern int const SourceSideIndirectHeatAltSetpoint;

    // MODULE VARIABLE DECLARATIONS:
    extern int NumChilledWaterMixed;        // number of mixed chilled water tanks
    extern int NumChilledWaterStratified;   // number of stratified chilled water tanks
    extern int NumWaterHeaterMixed;         // number of mixed water heaters
    extern int NumWaterHeaterStratified;    // number of stratified water heaters
    extern int NumWaterThermalTank;         // total number of water thermal tanks, hot and cold (MIXED + STRATIFIED)
    extern int NumWaterHeaterDesuperheater; // number of desuperheater heating coils
    extern int NumHeatPumpWaterHeater;      // number of heat pump water heaters

    extern Real64 modHPPartLoadRatio;            // part load ratio of HPWH
    extern bool modGetWaterThermalTankInputFlag; // Calls to Water Heater from multiple places in code
    extern Real64 modMixerInletAirSchedule;      // output of inlet air mixer node schedule
    extern Real64 modMdotAir;                    // mass flow rate of evaporator air, kg/s
    extern int NumWaterHeaterSizing;          // Number of sizing/design objects for water heaters.
    extern Array1D_bool AlreadyRated;         // control so we don't repeat again

    struct StratifiedNodeData
    {
        // Members
        Real64 Mass; // All nodes have the same mass (kg)
        Real64 OnCycLossCoeff;
        Real64 OffCycLossCoeff;
        Real64 Temp;
        Real64 SavedTemp;
        Real64 NewTemp;
        Real64 TempSum;
        Real64 TempAvg; // Average node temperature over the time step (C)
        Real64 CondCoeffUp;
        Real64 CondCoeffDn;
        Real64 OffCycParaLoad; // Heat delivered to the tank from off-cycle parasitic sources
        Real64 OnCycParaLoad;
        Real64 UseMassFlowRate;
        Real64 SourceMassFlowRate;
        Real64 MassFlowFromUpper; // Mass flow rate into this node from node above
        Real64 MassFlowFromLower; // Mass flow rate into this node from node below
        Real64 MassFlowToUpper;   // Mass flow rate from this node to node above
        Real64 MassFlowToLower;   // Mass flow rate from this node to node below
        // Report Variables
        Real64 Volume;
        Real64 Height;      // Node height from top to bottom (like a thickness)
        Real64 MaxCapacity; // For reporting
        int Inlets;
        int Outlets;
        Real64 HPWHWrappedCondenserHeatingFrac; // fraction of the heat from a wrapped condenser that enters into this node, should add up to 1.

        // Default Constructor
        StratifiedNodeData()
            : Mass(0.0), OnCycLossCoeff(0.0), OffCycLossCoeff(0.0), Temp(0.0), SavedTemp(0.0), NewTemp(0.0), TempSum(0.0), TempAvg(0.0),
              CondCoeffUp(0.0), CondCoeffDn(0.0), OffCycParaLoad(0.0), OnCycParaLoad(0.0), UseMassFlowRate(0.0), SourceMassFlowRate(0.0),
              MassFlowFromUpper(0.0), MassFlowFromLower(0.0), MassFlowToUpper(0.0), MassFlowToLower(0.0), Volume(0.0), Height(0.0), MaxCapacity(0.0),
              Inlets(0), Outlets(0), HPWHWrappedCondenserHeatingFrac(0.0)
        {
        }
    };

    struct WaterHeaterSizingData
    {
        // Members
        // input data
        int DesignMode;                           // what sizing method to use
        Real64 TankDrawTime;                      // in hours, time storage can meet peak demand
        Real64 RecoveryTime;                      // time for tank to recover
        Real64 NominalVolForSizingDemandSideFlow; // nominal tank size to use in sizing demand side connections
        int NumberOfBedrooms;
        Real64 NumberOfBathrooms;
        Real64 TankCapacityPerPerson;
        Real64 RecoveryCapacityPerPerson;
        Real64 TankCapacityPerArea;
        Real64 RecoveryCapacityPerArea;
        Real64 NumberOfUnits;
        Real64 TankCapacityPerUnit;
        Real64 RecoveryCapacityPerUnit;
        Real64 TankCapacityPerCollectorArea;
        Real64 HeightAspectRatio;
        // data from elsewhere in E+
        Real64 PeakDemand;
        Real64 PeakNumberOfPeople;
        Real64 TotalFloorArea;
        Real64 TotalSolarCollectorArea;

        // Default Constructor
        WaterHeaterSizingData()
            : DesignMode(SizeNotSet), TankDrawTime(0.0), RecoveryTime(0.0), NominalVolForSizingDemandSideFlow(0.0), NumberOfBedrooms(0),
              NumberOfBathrooms(0.0), TankCapacityPerPerson(0.0), RecoveryCapacityPerPerson(0.0), TankCapacityPerArea(0.0),
              RecoveryCapacityPerArea(0.0), NumberOfUnits(0.0), TankCapacityPerUnit(0.0), RecoveryCapacityPerUnit(0.0),
              TankCapacityPerCollectorArea(0.0), HeightAspectRatio(0.0), PeakDemand(0.0), PeakNumberOfPeople(0.0), TotalFloorArea(0.0),
              TotalSolarCollectorArea(0.0)
        {
        }
    };

    struct WaterThermalTankData
    {
        // Members
        std::string Name;                  // Name of water heater
        std::string Type;                  // Type of water heater (MIXED or STRATIFIED)
        int TypeNum;                       // integer parameter for water heater(if part of an HPWH,then=HPWH)
        bool IsChilledWaterTank;           // logical flag, true if for chilled water, false if for hot water
        std::string EndUseSubcategoryName; // User-defined end-use subcategory name
        bool Init;                         // Flag for initialization:  TRUE means do the init
        bool StandAlone;                   // Flag for operation with no plant connections (no source or use)
        Real64 Volume;                     // Tank volume (m3)
        bool VolumeWasAutoSized;           // true if tank volume was autosize on input
        Real64 Mass;                       // Total mass of fluid in the tank (kg)
        Real64 TimeElapsed;                // Fraction of the current hour that has elapsed (h)
        // Saved in order to identify the beginning of a new system time
        int AmbientTempIndicator;              // Indicator for ambient tank losses (SCHEDULE, ZONE, EXTERIOR)
        int AmbientTempSchedule;               // Schedule index pointer
        int AmbientTempZone;                   // Number of ambient zone around tank
        int AmbientTempOutsideAirNode;         // Number of outside air node
        Real64 AmbientTemp;                    // Ambient temperature around tank (C)
        Real64 AmbientZoneGain;                // Internal gain to zone from tank losses (W)
        Real64 LossCoeff;                      // Overall tank heat loss coefficient, UA (W/K)
        Real64 OffCycLossCoeff;                // Off-cycle overall tank heat loss coefficient, UA (W/K)
        Real64 OffCycLossFracToZone;           // Fraction of off-cycle losses added to zone
        Real64 OnCycLossCoeff;                 // On-cycle overall tank heat loss coefficient, UA (W/K)
        Real64 OnCycLossFracToZone;            // Fraction of on-cycle losses added to zone
        int Mode;                              // Indicator for current operating mode
        int SavedMode;                         // Mode indicator saved from previous time step
        int ControlType;                       // Indicator for control type
        std::string FuelType;                  // Fuel type
        Real64 MaxCapacity;                    // Maximum capacity of auxiliary heater 1 (W)
        bool MaxCapacityWasAutoSized;          // true if heater 1 capacity was autosized on input
        Real64 MinCapacity;                    // Minimum capacity of auxiliary heater 1 (W)
        Real64 Efficiency;                     // Thermal efficiency of auxiliary heater 1 ()
        int PLFCurve;                          // Part load factor curve as a function of part load ratio
        int SetPointTempSchedule;              // Schedule index pointer
        Real64 SetPointTemp;                   // Setpoint temperature of auxiliary heater 1 (C)
        Real64 DeadBandDeltaTemp;              // Deadband temperature difference of auxiliary heater 1 (deltaC)
        Real64 TankTempLimit;                  // Maximum tank temperature limit before venting (C)
        Real64 IgnitionDelay;                  // Time delay before heater is allowed to turn on (s)
        Real64 OffCycParaLoad;                 // Rate for off-cycle parasitic load (W)
        std::string OffCycParaFuelType;        // Fuel type for off-cycle parasitic load
        Real64 OffCycParaFracToTank;           // Fraction of off-cycle parasitic energy ending up in tank (W)
        Real64 OnCycParaLoad;                  // Rate for on-cycle parasitic load (W)
        std::string OnCycParaFuelType;         // Fuel type for on-cycle parasitic load
        Real64 OnCycParaFracToTank;            // Fraction of on-cycle parasitic energy ending up in tank (W)
        int UseCurrentFlowLock;                // current flow lock setting on use side
        int UseInletNode;                      // Inlet node on the use side; colder water returning to a hottank
        Real64 UseInletTemp;                   // Use side inlet temperature (C)
        int UseOutletNode;                     // Outlet node on the use side; hot tank water
        Real64 UseOutletTemp;                  // Use side outlet temperature (C)
        Real64 UseMassFlowRate;                // Mass flow rate on the use side (kg/s)
        Real64 UseEffectiveness;               // Heat transfer effectiveness on use side ()
        Real64 PlantUseMassFlowRateMax;        // Plant demand-side max flow request on use side (kg/s)
        Real64 SavedUseOutletTemp;             // Use side outlet temp saved for demand-side flow control (C)
        Real64 UseDesignVolFlowRate;           // Use side plant volume flow rate (input data, autosizable) m3/s
        bool UseDesignVolFlowRateWasAutoSized; // true if use flow rate was autosize on input
        int UseBranchControlType;              // Use side plant branch control type e.g active, passive, bypass
        int UseSidePlantSizNum;                // index in plant sizing that the use side is on
        bool UseSideSeries;
        int UseSideAvailSchedNum;                 // use side availability schedule
        Real64 UseSideLoadRequested;              // hold MyLoad request from plant management.
        int UseSidePlantLoopNum;                  // if not zero, then this use side is on plant loop #
        int UseSidePlantLoopSide;                 // use side is on loop side index
        int UseSidePlantBranchNum;                // use side branch num in plant topology
        int UseSidePlantCompNum;                  // use side component num in plant topology
        int SourceCurrentFlowLock;                // current flow lock setting on source side
        int SourceInletNode;                      // Inlet node for the source side; hot water from supply
        Real64 SourceInletTemp;                   // Source side inlet temperature (C)
        int SourceOutletNode;                     // Outlet node for the source side; colder tank water
        Real64 SourceOutletTemp;                  // Source side outlet temperature (C)
        Real64 SourceMassFlowRate;                // Mass flow rate on the source side (kg/s)
        Real64 SourceEffectiveness;               // Heat transfer effectiveness on source side ()
        Real64 PlantSourceMassFlowRateMax;        // Plant demand-side max flow request on source side (kg/s)
        Real64 SavedSourceOutletTemp;             // Source side outlet temp saved for demand-side flow control (C)
        Real64 SourceDesignVolFlowRate;           // Source side plant volume flow rate (input, autosizable) m3/s
        bool SourceDesignVolFlowRateWasAutoSized; // true if source flow rate was autosize on input
        int SourceBranchControlType;              // source side plant branch control type e.g active, passive, bypass
        int SourceSidePlantSizNum;                // index in plant sizing that the source side is on
        bool SourceSideSeries;
        int SourceSideAvailSchedNum;       // source side availability schedule.
        int SourceSidePlantLoopNum;        // if not zero, then this use side is on plant loop #
        int SourceSidePlantLoopSide;       // loop side that Source side is one eg. supply or demand
        int SourceSidePlantBranchNum;      // Source side branch num in plant topology
        int SourceSidePlantCompNum;        // Source side component num in plant topology
        int SourceSideControlMode;         // flag for how source side flow is controlled
        int SourceSideAltSetpointSchedNum; // schedule of alternate temperature setpoint values
        Real64 SizingRecoveryTime;         // sizing parameter for autosizing indirect water heaters (hr)
        Real64 MassFlowRateMax;            // Maximum flow rate for scheduled DHW (kg/s)
        Real64 VolFlowRateMin;             // Minimum flow rate for heater ignition (kg/s)
        Real64 MassFlowRateMin;            // Minimum mass flow rate for heater ignition (kg/s)
        int FlowRateSchedule;              // Schedule index pointer
        int UseInletTempSchedule;          // Cold water supply temperature schedule index pointer
        Real64 TankTemp;                   // Temperature of tank fluid (average, if stratified) (C)
        Real64 SavedTankTemp;              // Tank temp that is carried from time step to time step (C)
        Real64 TankTempAvg;                // Average tank temperature over the time step (C)
        // Stratified variables (in addition to the above)
        Real64 Height;           // Height of tank (m)
        bool HeightWasAutoSized; // true if the height of tank was autosize on input
        Real64 Perimeter;        // Perimeter of tank (m), only used for OTHER shape
        int Shape;               // Tank shape:  VERTICAL CYLINDER, HORIZONTAL CYLINDER, or OTHER
        Real64 HeaterHeight1;
        int HeaterNode1;
        bool HeaterOn1;
        bool SavedHeaterOn1;
        Real64 HeaterHeight2;
        int HeaterNode2;
        bool HeaterOn2;
        bool SavedHeaterOn2;
        Real64 AdditionalCond; // Additional destratification conductivity (W/m K)
        Real64 SetPointTemp2;  // Setpoint temperature of auxiliary heater 2 (C)
        int SetPointTempSchedule2;
        Real64 DeadBandDeltaTemp2;
        Real64 MaxCapacity2;
        Real64 OffCycParaHeight;
        Real64 OnCycParaHeight;
        Real64 SkinLossCoeff;
        Real64 SkinLossFracToZone;
        Real64 OffCycFlueLossCoeff;
        Real64 OffCycFlueLossFracToZone;
        Real64 UseInletHeight;              // Height of use side inlet (m)
        Real64 UseOutletHeight;             // Height of use side outlet (m)
        bool UseOutletHeightWasAutoSized;   // true if use outlet height was autosize on input
        Real64 SourceInletHeight;           // Height of source side inlet (m)
        bool SourceInletHeightWasAutoSized; // true if source inlet height was autosize on input
        Real64 SourceOutletHeight;          // Height of source side outlet (m)
        int UseInletStratNode;              // Use-side inlet node number
        int UseOutletStratNode;             // Use-side outlet node number
        int SourceInletStratNode;           // Source-side inlet node number
        int SourceOutletStratNode;          // Source-side outlet node number
        int InletMode;                      // Inlet position mode:  1 = FIXED; 2 = SEEKING
        Real64 InversionMixingRate;
        Array1D<Real64> AdditionalLossCoeff; // Loss coefficient added to the skin loss coefficient (W/m2-K)
        int Nodes;                           // Number of nodes
        Array1D<StratifiedNodeData> Node;    // Array of node data
        // Report variables
        Real64 VolFlowRate;            // Scheduled DHW demand (m3/s)
        Real64 VolumeConsumed;         // Volume of DHW consumed (m3)
        Real64 UnmetRate;              // Energy demand to heat tank water to setpoint (W)
        Real64 LossRate;               // Energy demand to support heat losses due to ambient temp (W)
        Real64 FlueLossRate;           // Heat loss rate to flue (W)
        Real64 UseRate;                // Energy demand to heat the Use Side water to tank temp (W)
        Real64 TotalDemandRate;        // Total demand rate (sum of all above rates) (W)
        Real64 SourceRate;             // Energy supplied by the source side to help heat the tank (W)
        Real64 HeaterRate;             // The energy the water heater burner puts into the water (W)
        Real64 HeaterRate1;            // The energy heater 1 puts into the water (W)
        Real64 HeaterRate2;            // The energy heater 2 puts into the water (W)
        Real64 FuelRate;               // The fuel consumption rate for the water heater burner (W)
        Real64 FuelRate1;              // The fuel consumption rate for heater 1 (W)
        Real64 FuelRate2;              // The fuel consumption rate for heater 2 (W)
        Real64 VentRate;               // Heat recovery energy lost due to setpoint temp (W)
        Real64 OffCycParaFuelRate;     // Fuel consumption rate for off-cycle parasitic load (W)
        Real64 OffCycParaRateToTank;   // Heat rate to tank for off-cycle parasitic load (W)
        Real64 OnCycParaFuelRate;      // Fuel consumption rate for on-cycle parasitic load (W)
        Real64 OnCycParaRateToTank;    // Heat rate to tank for on-cycle parasitic load (W)
        Real64 NetHeatTransferRate;    // Net heat transfer rate to/from tank (W)
        int CycleOnCount;              // Number of times heater cycles on in the current time step
        int CycleOnCount1;             // Number of times heater 1 cycles on in the current time step
        int CycleOnCount2;             // Number of times heater 2 cycles on in the current time step
        Real64 RuntimeFraction;        // Runtime fraction, fraction of timestep that any  heater is running
        Real64 RuntimeFraction1;       // Runtime fraction, fraction of timestep that heater 1 is running
        Real64 RuntimeFraction2;       // Runtime fraction, fraction of timestep that heater 2 is running
        Real64 PartLoadRatio;          // Part load ratio, fraction of maximum heater capacity
        Real64 UnmetEnergy;            // Energy to heat tank water to setpoint (J)
        Real64 LossEnergy;             // Energy to support heat losses due to ambient temp (J)
        Real64 FlueLossEnergy;         // Energy to support heat losses to the flue (J)
        Real64 UseEnergy;              // Energy to heat the use side water to tank temp (J)
        Real64 TotalDemandEnergy;      // Total energy demand (sum of all above energies) (J)
        Real64 SourceEnergy;           // Energy supplied by the source side to help heat the tank (J)
        Real64 HeaterEnergy;           // The energy the water heater burner puts into the water (J)
        Real64 HeaterEnergy1;          // The energy heater 1 puts into the water (J)
        Real64 HeaterEnergy2;          // The energy heater 2 puts into the water (J)
        Real64 FuelEnergy;             // The fuel consumption energy for the water heater burner (J)
        Real64 FuelEnergy1;            // The fuel consumption energy for heater 1 (J)
        Real64 FuelEnergy2;            // The fuel consumption energy for heater 2 (J)
        Real64 VentEnergy;             // Heat recovery energy lost due to setpoint temp (J)
        Real64 OffCycParaFuelEnergy;   // Fuel consumption energy for off-cycle parasitic load (J)
        Real64 OffCycParaEnergyToTank; // Energy to tank for off-cycle parasitic load (J)
        Real64 OnCycParaFuelEnergy;    // Fuel consumption energy for on-cycle parasitic load (J)
        Real64 OnCycParaEnergyToTank;  // Energy to tank for on-cycle parasitic load (J)
        Real64 NetHeatTransferEnergy;  // Net heat transfer energy to/from tank (J)
        bool FirstRecoveryDone;        // Flag to indicate when first recovery to the setpoint is done
        Real64 FirstRecoveryFuel;      // Fuel energy needed for first recovery to the setpoint (J)
        int HeatPumpNum;               // Index to heat pump water heater
        int DesuperheaterNum;          // Index to desuperheating coil
        bool ShowSetPointWarning;      // Warn when set point is greater than max tank temp limit
        int MaxCycleErrorIndex;        // recurring error index
        int FreezingErrorIndex;        // recurring error index for freeze conditions
        WaterHeaterSizingData Sizing;  // ancillary data for autosizing
        int FluidIndex;                // fluid properties index
        bool MyOneTimeFlagWH; // first pass log
        bool MyTwoTimeFlagWH; // second pass do input check
        bool MyEnvrnFlag;
        bool WarmupFlag;
        bool SetLoopIndexFlag;
        bool MyOneTimeSetupFlag;
        bool AlreadyReported;
        bool AlreadyRated;
        bool MyHPSizeFlag;
        bool CheckWTTEquipName;

        // Default Constructor
        WaterThermalTankData()
            : TypeNum(0), IsChilledWaterTank(false), Init(true), StandAlone(false), Volume(0.0), VolumeWasAutoSized(false), Mass(0.0),
              TimeElapsed(0.0), AmbientTempIndicator(0), AmbientTempSchedule(0), AmbientTempZone(0), AmbientTempOutsideAirNode(0), AmbientTemp(0.0),
              AmbientZoneGain(0.0), LossCoeff(0.0), OffCycLossCoeff(0.0), OffCycLossFracToZone(0.0), OnCycLossCoeff(0.0), OnCycLossFracToZone(0.0),
              Mode(0), SavedMode(0), ControlType(1), MaxCapacity(0.0), MaxCapacityWasAutoSized(false), MinCapacity(0.0), Efficiency(0.0), PLFCurve(0),
              SetPointTempSchedule(0), SetPointTemp(0.0), DeadBandDeltaTemp(0.0), TankTempLimit(0.0), IgnitionDelay(0.0), OffCycParaLoad(0.0),
              OffCycParaFracToTank(0.0), OnCycParaLoad(0.0), OnCycParaFracToTank(0.0), UseCurrentFlowLock(0), UseInletNode(0), UseInletTemp(0.0),
              UseOutletNode(0), UseOutletTemp(0.0), UseMassFlowRate(0.0), UseEffectiveness(0.0), PlantUseMassFlowRateMax(0.0),
              SavedUseOutletTemp(0.0), UseDesignVolFlowRate(0.0), UseDesignVolFlowRateWasAutoSized(false), UseBranchControlType(2),
              UseSidePlantSizNum(0), UseSideSeries(true), UseSideAvailSchedNum(0), UseSideLoadRequested(0.0), UseSidePlantLoopNum(0),
              UseSidePlantLoopSide(0), UseSidePlantBranchNum(0), UseSidePlantCompNum(0), SourceCurrentFlowLock(0), SourceInletNode(0),
              SourceInletTemp(0.0), SourceOutletNode(0), SourceOutletTemp(0.0), SourceMassFlowRate(0.0), SourceEffectiveness(0.0),
              PlantSourceMassFlowRateMax(0.0), SavedSourceOutletTemp(0.0), SourceDesignVolFlowRate(0.0), SourceDesignVolFlowRateWasAutoSized(false),
              SourceBranchControlType(2), SourceSidePlantSizNum(0), SourceSideSeries(true), SourceSideAvailSchedNum(0), SourceSidePlantLoopNum(0),
              SourceSidePlantLoopSide(0), SourceSidePlantBranchNum(0), SourceSidePlantCompNum(0), SourceSideControlMode(0),
              SourceSideAltSetpointSchedNum(0), SizingRecoveryTime(0.0), MassFlowRateMax(0.0), VolFlowRateMin(0.0), MassFlowRateMin(0.0),
              FlowRateSchedule(0), UseInletTempSchedule(0), TankTemp(0.0), SavedTankTemp(0.0), TankTempAvg(0.0), Height(0.0),
              HeightWasAutoSized(false), Perimeter(0.0), Shape(0), HeaterHeight1(0.0), HeaterNode1(0), HeaterOn1(false), SavedHeaterOn1(false),
              HeaterHeight2(0.0), HeaterNode2(0), HeaterOn2(false), SavedHeaterOn2(false), AdditionalCond(0.0), SetPointTemp2(0.0),
              SetPointTempSchedule2(0), DeadBandDeltaTemp2(0.0), MaxCapacity2(0.0), OffCycParaHeight(0.0), OnCycParaHeight(0.0), SkinLossCoeff(0.0),
              SkinLossFracToZone(0.0), OffCycFlueLossCoeff(0.0), OffCycFlueLossFracToZone(0.0), UseInletHeight(0.0), UseOutletHeight(0.0),
              UseOutletHeightWasAutoSized(false), SourceInletHeight(0.0), SourceInletHeightWasAutoSized(false), SourceOutletHeight(0.0),
              UseInletStratNode(0), UseOutletStratNode(0), SourceInletStratNode(0), SourceOutletStratNode(0), InletMode(1), InversionMixingRate(0.0),
              Nodes(0), VolFlowRate(0.0), VolumeConsumed(0.0), UnmetRate(0.0), LossRate(0.0), FlueLossRate(0.0), UseRate(0.0), TotalDemandRate(0.0),
              SourceRate(0.0), HeaterRate(0.0), HeaterRate1(0.0), HeaterRate2(0.0), FuelRate(0.0), FuelRate1(0.0), FuelRate2(0.0), VentRate(0.0),
              OffCycParaFuelRate(0.0), OffCycParaRateToTank(0.0), OnCycParaFuelRate(0.0), OnCycParaRateToTank(0.0), NetHeatTransferRate(0.0),
              CycleOnCount(0), CycleOnCount1(0), CycleOnCount2(0), RuntimeFraction(0.0), RuntimeFraction1(0.0), RuntimeFraction2(0.0),
              PartLoadRatio(0.0), UnmetEnergy(0.0), LossEnergy(0.0), FlueLossEnergy(0.0), UseEnergy(0.0), TotalDemandEnergy(0.0), SourceEnergy(0.0),
              HeaterEnergy(0.0), HeaterEnergy1(0.0), HeaterEnergy2(0.0), FuelEnergy(0.0), FuelEnergy1(0.0), FuelEnergy2(0.0), VentEnergy(0.0),
              OffCycParaFuelEnergy(0.0), OffCycParaEnergyToTank(0.0), OnCycParaFuelEnergy(0.0), OnCycParaEnergyToTank(0.0),
              NetHeatTransferEnergy(0.0), FirstRecoveryDone(false), FirstRecoveryFuel(0.0), HeatPumpNum(0), DesuperheaterNum(0),
              ShowSetPointWarning(true), MaxCycleErrorIndex(0), FreezingErrorIndex(0), FluidIndex(0), MyOneTimeFlagWH(true), MyTwoTimeFlagWH(true),
              MyEnvrnFlag(true), WarmupFlag(false), SetLoopIndexFlag(true), MyOneTimeSetupFlag(true), AlreadyReported(false), AlreadyRated(false),
              MyHPSizeFlag(true), CheckWTTEquipName(true)
        {
        }

        Real64 getDeadBandTemp();
    };

    struct HeatPumpWaterHeaterData
    {
        // Members
        std::string Name;                                   // Name of heat pump water heater
        std::string Type;                                   // Type of water heater (HEAT PUMP:WATER HEATER)
        int TypeNum;                                        // integer parameter for heat pump water heater
        std::string TankName;                               // Name of tank associated with heat pump water heater
        std::string TankType;                               // Type of water heater (MIXED or STRATIFIED) used with heat pump
        int TankTypeNum;                                    // Parameter for tank type (MIXED or STRATIFIED)
        bool StandAlone;                                    // Flag for operation with no plant connections (no use nodes)
        int AvailSchedPtr;                                  // Index to Availability Schedule curve index
        int SetPointTempSchedule;                           // Index to Setpoint Temperature Schedule curve
        Real64 DeadBandTempDiff;                            // Dead band temperature difference (cut-in temperature)
        Real64 Capacity;                                    // Heat Pump rated capacity (W)
        Real64 BackupElementCapacity;                       // Tank backup element capacity (W)
        Real64 BackupElementEfficiency;                     // Tank backup element efficiency
        Real64 WHOnCycParaLoad;                             // tank's on-cycle parasitic load (W), disable for rating
        Real64 WHOffCycParaLoad;                            // tank's off-cycle parasitic load (W), disable for rating
        Real64 WHOnCycParaFracToTank;                       // tank's on-cycle parasitic frac to tank, disable for rating
        Real64 WHOffCycParaFracToTank;                      // tank's off-cycle parasitic frac to tank, disable for rating
        int WHPLFCurve;                                     // tank part-load fraction curve index, used for rating procedure
        Real64 OperatingAirFlowRate;                        // Operating volumetric air flow rate (m3/s)
        Real64 OperatingAirMassFlowRate;                    // Operating air mass flow rate (kg/s)
        Real64 OperatingWaterFlowRate;                      // Operating volumetric water flow rate (m3/s)
        Real64 COP;                                         // Heat Pump coefficient of performance (W/W)
        Real64 SHR;                                         // Heat Pump air-side coil sensible heat ratio
        Real64 RatedInletDBTemp;                            // Rated evaporator inlet air dry-bulb temperature (C)
        Real64 RatedInletWBTemp;                            // Rated evaporator inlet air wet-bulb temperature (C)
        Real64 RatedInletWaterTemp;                         // Rated condenser inlet water temperature (C)
        bool FoundTank;                                     // Found storage tank flag associated with HP water heater
        int HeatPumpAirInletNode;                           // HP air inlet node (for zone, zone/outdoor or scheduled)
        int HeatPumpAirOutletNode;                          // HP air outlet node (for zone, zone/outdoor or scheduled)
        int OutsideAirNode;                                 // outdoor air node (for outdoor or zone/outdoor air unit only)
        int ExhaustAirNode;                                 // Exhaust air node (for outdoor or zone/outdoor air unit only)
        int CondWaterInletNode;                             // Condenser water inlet node
        int CondWaterOutletNode;                            // Condenser water outlet node
        int WHUseInletNode;                                 // Water heater tank use side inlet node
        int WHUseOutletNode;                                // Water heater tank use side outlet node
        int WHUseSidePlantLoopNum;                          // if not zero, then this water heater is on plant loop #
        std::string DXCoilType;                             // Type of DX coil (Coil:DX:HeatPumpWaterHeater)
        std::string DXCoilName;                             // Name of DX coil
        int DXCoilNum;                                      // Index of DX coil
        int DXCoilTypeNum;                                  // Type Number of DX coil
        int DXCoilAirInletNode;                             // Inlet air node number of DX coil
        int DXCoilPLFFPLR;                                  // Index to HPWH's DX Coil PLF as a function of PLR curve
        std::string FanType;                                // Type of Fan (Fan:OnOff)
        int FanType_Num;                                    // Integer type of fan (3 = Fan:OnOff)
        std::string FanName;                                // Name of Fan
        std::string FanInletNode_str;                       // Fan inlet node name
        std::string FanOutletNode_str;                      // Fan outlet node name
        int FanNum;                                         // Index of Fan
        int FanPlacement;                                   // Location of Fan
        int FanOutletNode;                                  // Outlet node of heat pump water heater fan
        int WaterHeaterTankNum;                             // Index of Water Heater Tank
        int OutletAirSplitterSchPtr;                        // Index to air-side outlet air splitter schedule
        int InletAirMixerSchPtr;                            // Index to air-side inlet air mixer schedule
        int Mode;                                           // HP mode (0 = float, 1 = heating [-1 = venting na for HP])
        int SaveMode;                                       // HP mode on first iteration
        int SaveWHMode;                                     // mode of water heater tank element (backup element)
        Real64 Power;                                       // HP power used for reporting
        Real64 Energy;                                      // HP energy used for reporting
        Real64 HeatingPLR;                                  // HP PLR used for reporting
        Real64 SetPointTemp;                                // set point or cut-out temperature [C]
        Real64 MinAirTempForHPOperation;                    // HP does not operate below this ambient temperature
        Real64 MaxAirTempForHPOperation;                    // HP does not operate above this ambient temperature
        int InletAirMixerNode;                              // Inlet air mixer node number of HP water heater
        int OutletAirSplitterNode;                          // Outlet air splitter node number of HP water heater
        Real64 SourceMassFlowRate;                          // Maximum mass flow rate on the source side (kg/s)
        int InletAirConfiguration;                          // Identifies source of HPWH inlet air
        int AmbientTempSchedule;                            // Schedule index pointer for ambient air temp at HPWH inlet
        int AmbientRHSchedule;                              // Schedule index pointer for ambient air RH at HPWH inlet
        int AmbientTempZone;                                // Index of ambient zone for ambient air at HPWH inlet
        int CrankcaseTempIndicator;                         // Indicator for HPWH compressor/crankcase heater location
        int CrankcaseTempSchedule;                          // Schedule index pointer where crankcase heater is located
        int CrankcaseTempZone;                              // Index of zone where compressor/crankcase heater is located
        Real64 OffCycParaLoad;                              // Rate for off-cycle parasitic load (W)
        Real64 OnCycParaLoad;                               // Rate for on-cycle parasitic load (W)
        int ParasiticTempIndicator;                         // Indicator for HPWH parasitic heat rejection location
        Real64 OffCycParaFuelRate;                          // Electric consumption rate for off-cycle parasitic load (W)
        Real64 OnCycParaFuelRate;                           // Electric consumption rate for on-cycle parasitic load (W)
        Real64 OffCycParaFuelEnergy;                        // Electric energy consumption for off-cycle parasitic load (J)
        Real64 OnCycParaFuelEnergy;                         // Electric energy consumption for on-cycle parasitic load (J)
        bool AirFlowRateAutoSized;                          // Used to report air flow autosize info in Init
        bool WaterFlowRateAutoSized;                        // Used to report water flow autosize info in Init
        int HPSetPointError;                                // Used when temperature SP's in tank and HP are reversed
        int HPSetPointErrIndex1;                            // Index to recurring error for tank/HP set point temp
        int IterLimitErrIndex1;                             // Index for recurring iteration limit warning messages
        int IterLimitExceededNum1;                          // Counter for recurring iteration limit warning messages
        int RegulaFalsiFailedIndex1;                        // Index for recurring RegulaFalsi failed warning messages
        int RegulaFalsiFailedNum1;                          // Counter for recurring RegulaFalsi failed warning messages
        int IterLimitErrIndex2;                             // Index for recurring iteration limit warning messages
        int IterLimitExceededNum2;                          // Counter for recurring iteration limit warning messages
        int RegulaFalsiFailedIndex2;                        // Index for recurring RegulaFalsi failed warning messages
        int RegulaFalsiFailedNum2;                          // Counter for recurring RegulaFalsi failed warning messages
        bool FirstTimeThroughFlag;                          // Flag for saving water heater status
        bool ShowSetPointWarning;                           // Warn when set point is greater than max tank temp limit
        Real64 HPWaterHeaterSensibleCapacity;               // sensible capacity delivered when HPWH is attached to a zone (W)
        Real64 HPWaterHeaterLatentCapacity;                 // latent capacity delivered when HPWH is attached to a zone (kg/s)
        Real64 WrappedCondenserBottomLocation;              // Location of the bottom of the wrapped condenser.
        Real64 WrappedCondenserTopLocation;                 // Location of the top of the wrapped condenser.
        Real64 ControlSensor1Height;                        // location from bottom of tank of control sensor 1
        int ControlSensor1Node;                             // Node number of control sensor 1
        Real64 ControlSensor1Weight;                        // weight of control sensor 1
        Real64 ControlSensor2Height;                        // location from bottom of tank of control sensor 2
        int ControlSensor2Node;                             // Node number of control sensor 2
        Real64 ControlSensor2Weight;                        // weight of control sensor 2
        Real64 ControlTempAvg;                              // Measured control temperature for the heat pump, average over timestep, for reporting
        Real64 ControlTempFinal;                            // Measured control temperature at the end of the timestep, for reporting
        bool AllowHeatingElementAndHeatPumpToRunAtSameTime; // if false, if the heating element kicks on, it will recover with that before turning the
                                                            // heat pump back on.
        // variables for variable-speed HPWH
        int NumofSpeed;                        // number of speeds for VS HPWH
        Array1D<Real64> HPWHAirVolFlowRate;    // air volume flow rate during heating operation
        Array1D<Real64> HPWHAirMassFlowRate;   // air mass flow rate during heating operation
        Array1D<Real64> HPWHWaterVolFlowRate;  // water volume flow rate during heating operation
        Array1D<Real64> HPWHWaterMassFlowRate; // water mass flow rate during heating operation
        Array1D<Real64> MSAirSpeedRatio;       // air speed ratio in heating mode
        Array1D<Real64> MSWaterSpeedRatio;     // water speed ratio in heating mode
        bool bIsIHP;                           // whether the HP is a part of Integrated Heat Pump
        bool MyOneTimeFlagHP; // first pass log
        bool MyTwoTimeFlagHP; // second pass do input check
        std::string CoilInletNode_str;         // Used to set up comp set
        std::string CoilOutletNode_str;        // Used to set up comp set
        bool CheckHPWHEquipName;

        // end of variables for variable-speed HPWH

        // Default Constructor
        HeatPumpWaterHeaterData()
                : TypeNum(0), TankTypeNum(0), StandAlone(false), AvailSchedPtr(0), SetPointTempSchedule(0), DeadBandTempDiff(0.0), Capacity(0.0),
                  BackupElementCapacity(0.0), BackupElementEfficiency(0.0), WHOnCycParaLoad(0.0), WHOffCycParaLoad(0.0), WHOnCycParaFracToTank(0.0),
                  WHOffCycParaFracToTank(0.0), WHPLFCurve(0), OperatingAirFlowRate(0.0), OperatingAirMassFlowRate(0.0), OperatingWaterFlowRate(0.0), COP(0.0), SHR(0.0),
                  RatedInletDBTemp(0.0), RatedInletWBTemp(0.0), RatedInletWaterTemp(0.0), FoundTank(false), HeatPumpAirInletNode(0), HeatPumpAirOutletNode(0),
                  OutsideAirNode(0), ExhaustAirNode(0), CondWaterInletNode(0), CondWaterOutletNode(0), WHUseInletNode(0), WHUseOutletNode(0),
                  WHUseSidePlantLoopNum(0), DXCoilNum(0), DXCoilTypeNum(0), DXCoilAirInletNode(0), DXCoilPLFFPLR(0), FanType_Num(0), FanNum(0),
                  FanPlacement(0), FanOutletNode(0), WaterHeaterTankNum(0), OutletAirSplitterSchPtr(0), InletAirMixerSchPtr(0), Mode(0), SaveMode(0),
                  SaveWHMode(0), Power(0.0), Energy(0.0), HeatingPLR(0.0), SetPointTemp(0.0), MinAirTempForHPOperation(5.0),
                  MaxAirTempForHPOperation(48.8888888889), InletAirMixerNode(0), OutletAirSplitterNode(0), SourceMassFlowRate(0.0), InletAirConfiguration(0),
                  AmbientTempSchedule(0), AmbientRHSchedule(0), AmbientTempZone(0), CrankcaseTempIndicator(0), CrankcaseTempSchedule(0), CrankcaseTempZone(0),
                  OffCycParaLoad(0.0), OnCycParaLoad(0.0), ParasiticTempIndicator(0), OffCycParaFuelRate(0.0), OnCycParaFuelRate(0.0),
                  OffCycParaFuelEnergy(0.0), OnCycParaFuelEnergy(0.0), AirFlowRateAutoSized(false), WaterFlowRateAutoSized(false), HPSetPointError(0),
                  HPSetPointErrIndex1(0), IterLimitErrIndex1(0), IterLimitExceededNum1(0), RegulaFalsiFailedIndex1(0), RegulaFalsiFailedNum1(0),
                  IterLimitErrIndex2(0), IterLimitExceededNum2(0), RegulaFalsiFailedIndex2(0), RegulaFalsiFailedNum2(0), FirstTimeThroughFlag(true),
                  ShowSetPointWarning(true), HPWaterHeaterSensibleCapacity(0.0), HPWaterHeaterLatentCapacity(0.0), WrappedCondenserBottomLocation(0.0),
                  WrappedCondenserTopLocation(0.0), ControlSensor1Height(-1.0), ControlSensor1Node(1), ControlSensor1Weight(1.0), ControlSensor2Height(-1.0),
                  ControlSensor2Node(2), ControlSensor2Weight(0.0), ControlTempAvg(0.0), ControlTempFinal(0.0),
                  AllowHeatingElementAndHeatPumpToRunAtSameTime(true), NumofSpeed(0), HPWHAirVolFlowRate(VariableSpeedCoils::MaxSpedLevels, 0.0),
                  HPWHAirMassFlowRate(VariableSpeedCoils::MaxSpedLevels, 0.0), HPWHWaterVolFlowRate(VariableSpeedCoils::MaxSpedLevels, 0.0), HPWHWaterMassFlowRate(VariableSpeedCoils::MaxSpedLevels, 0.0),
                  MSAirSpeedRatio(VariableSpeedCoils::MaxSpedLevels, 0.0), MSWaterSpeedRatio(VariableSpeedCoils::MaxSpedLevels, 0.0), bIsIHP(false),
                  MyOneTimeFlagHP(true), MyTwoTimeFlagHP(true), CheckHPWHEquipName(true)
        {
        }
    };

    struct WaterHeaterDesuperheaterData
    {
        // Members
        std::string Name;              // Name of heat pump water heater desuperheater
        std::string Type;              // Type of water heater desuperheating coil
        int InsuffTemperatureWarn;     // Used for recurring error count on low source temperature
        int AvailSchedPtr;             // Index to Availability Schedule curve index
        int SetPointTempSchedule;      // Index to Setpoint Temperature Schedule curve
        Real64 DeadBandTempDiff;       // Dead band temperature difference (cut-in temperature)
        Real64 HeatReclaimRecoveryEff; // recovery efficiency of desuperheater (0.3 max)
        int WaterInletNode;            // Desuperheater water inlet node
        int WaterOutletNode;           // Desuperheater water outlet node
        Real64 RatedInletWaterTemp;    // Inlet water temp at rated heat reclaim recovery eff (C)
        Real64 RatedOutdoorAirTemp;    // Outdoor air temp at rated heat reclaim recovery eff (C)
        Real64 MaxInletWaterTemp;      // Max water temp for heat reclaim recovery (C)
        std::string TankType;          // Type of water heater (MIXED or STRATIFIED)
        int TankTypeNum;               // Parameter for tank type (MIXED or STRATIFIED)
        std::string TankName;          // Name of tank associated with desuperheater
        bool StandAlone;               // Flag for operation with no plant connections (no use nodes)
        std::string HeatingSourceType;        // Type of heating source (DX coil or refrigerated rack)
        std::string HeatingSourceName;        // Name of heating source
        Real64 HeaterRate;                    // Report variable for desuperheater heating rate [W]
        Real64 HeaterEnergy;                  // Report variable for desuperheater heating energy [J]
        Real64 PumpPower;                     // Report variable for water circulation pump power [W]
        Real64 PumpEnergy;                    // Report variable for water circulation pump energy [J]
        Real64 PumpElecPower;                 // Nominal power input to the water circulation pump [W]
        Real64 PumpFracToWater;               // Nominal power fraction to water for the water circulation pump
        Real64 OperatingWaterFlowRate;        // Operating volumetric water flow rate (m3/s)
        int HEffFTemp;                        // Heating capacity as a function of temperature curve index
        Real64 HEffFTempOutput;               // report variable for HEffFTemp curve
        Real64 SetPointTemp;                  // set point or cut-out temperature [C]
        int WaterHeaterTankNum;               // Index of Water Heater Tank
        Real64 DesuperheaterPLR;              // part load ratio of desuperheater
        Real64 OnCycParaLoad;                 // Rate for on-cycle parasitic load (W)
        Real64 OffCycParaLoad;                // Rate for off-cycle parasitic load (W)
        Real64 OnCycParaFuelEnergy;           // Electric energy consumption for on-cycle parasitic load (J)
        Real64 OnCycParaFuelRate;             // Electric consumption rate for on-cycle parasitic load (W)
        Real64 OffCycParaFuelEnergy;          // Electric energy consumption for off-cycle parasitic load (J)
        Real64 OffCycParaFuelRate;            // Electric consumption rate for off-cycle parasitic load (W)
        int Mode;                             // mode (0 = float, 1 = heating [-1=venting na for desuperheater])
        int SaveMode;                         // desuperheater mode on first iteration
        int SaveWHMode;                       // mode of water heater tank element (backup element)
        Real64 BackupElementCapacity;         // Tank backup element capacity (W)
        Real64 DXSysPLR;                      // runtime fraction of desuperheater heating coil
        int ReclaimHeatingSourceIndexNum;     // Index to reclaim heating source (condenser) of a specific type
        int ReclaimHeatingSource;             // The source for the Desuperheater Heating Coil
        int SetPointError;           // Used when temp SP in tank and desuperheater are reversed
        int SetPointErrIndex1;       // Index to recurring error for tank/desuperheater set point temp
        int IterLimitErrIndex1;      // Index for recurring iteration limit warning messages
        int IterLimitExceededNum1;   // Counter for recurring iteration limit warning messages
        int RegulaFalsiFailedIndex1; // Index for recurring RegulaFalsi failed warning messages
        int RegulaFalsiFailedNum1;   // Counter for recurring RegulaFalsi failed warning messages
        int IterLimitErrIndex2;      // Index for recurring iteration limit warning messages
        int IterLimitExceededNum2;   // Counter for recurring iteration limit warning messages
        int RegulaFalsiFailedIndex2; // Index for recurring RegulaFalsi failed warning messages
        int RegulaFalsiFailedNum2;   // Counter for recurring RegulaFalsi failed warning messages
        bool FirstTimeThroughFlag;   // Flag for saving water heater status
        bool ValidSourceType;

        // Default Constructor
        WaterHeaterDesuperheaterData()
            : InsuffTemperatureWarn(0), AvailSchedPtr(0), SetPointTempSchedule(0), DeadBandTempDiff(0.0), HeatReclaimRecoveryEff(0.0),
              WaterInletNode(0), WaterOutletNode(0), RatedInletWaterTemp(0.0), RatedOutdoorAirTemp(0.0), MaxInletWaterTemp(0.0), TankTypeNum(0),
              StandAlone(false), HeaterRate(0.0), HeaterEnergy(0.0), PumpPower(0.0), PumpEnergy(0.0), PumpElecPower(0.0), PumpFracToWater(0.0),
              OperatingWaterFlowRate(0.0), HEffFTemp(0), HEffFTempOutput(0.0), SetPointTemp(0.0), WaterHeaterTankNum(0), DesuperheaterPLR(0.0),
              OnCycParaLoad(0.0), OffCycParaLoad(0.0), OnCycParaFuelEnergy(0.0), OnCycParaFuelRate(0.0), OffCycParaFuelEnergy(0.0),
              OffCycParaFuelRate(0.0), Mode(0), SaveMode(0), SaveWHMode(0), BackupElementCapacity(0.0), DXSysPLR(0.0),
              ReclaimHeatingSourceIndexNum(0), ReclaimHeatingSource(0), SetPointError(0), SetPointErrIndex1(0), IterLimitErrIndex1(0),
              IterLimitExceededNum1(0), RegulaFalsiFailedIndex1(0), RegulaFalsiFailedNum1(0), IterLimitErrIndex2(0), IterLimitExceededNum2(0),
              RegulaFalsiFailedIndex2(0), RegulaFalsiFailedNum2(0), FirstTimeThroughFlag(true), ValidSourceType(false)
        {
        }
    };

    // Object Data
    extern Array1D<WaterThermalTankData> WaterThermalTank;
    extern Array1D<HeatPumpWaterHeaterData> HPWaterHeater;
    extern Array1D<WaterHeaterDesuperheaterData> WaterHeaterDesuperheater;

    // Functions

    void SimWaterThermalTank(int CompType,
                             std::string const &CompName,
                             int &CompIndex,
                             bool RunFlag, // unused1208
                             bool InitLoopEquip,
                             Real64 &MyLoad,
                             Real64 &MaxCap,
                             Real64 &MinCap,
                             Real64 &OptCap,
                             bool FirstHVACIteration, // TRUE if First iteration of simulation
                             Optional_int_const LoopNum = _,
                             Optional_int_const LoopSideNum = _);

    void SimulateWaterHeaterStandAlone(int WaterHeaterNum, bool FirstHVACIteration);

    void SimHeatPumpWaterHeater(std::string const &CompName,
                                bool FirstHVACIteration,
                                Real64 &SensLoadMet, // sensible load met by this equipment and sent to zone, W
                                Real64 &LatLoadMet,  // net latent load met and sent to zone (kg/s), dehumid = negative
                                int &CompIndex);

    void CalcWaterThermalTankZoneGains();

    bool GetWaterThermalTankInput();

    void ValidatePLFCurve(int CurveIndex, bool &IsValid);

    void SetupStratifiedNodes(int WaterThermalTankNum); // Water Heater being simulated

    void InitWaterThermalTank(int WaterThermalTankNum,
                              bool FirstHVACIteration,
                              Optional_int_const LoopNum = _,
                              Optional_int_const LoopSideNum = _);

    void CalcWaterThermalTankMixed(int WaterThermalTankNum); // Water Heater being simulated

    void CalcMixedTankSourceSideHeatTransferRate(Real64 HPWHCondenserDeltaT, // input, The temperature difference (C) across the heat pump, zero if
                                                                             // there is no heat pump or if the heat pump is off
                                                 Real64 SourceInletTemp,     // input, Source inlet temperature (C)
                                                 Real64 Cp,                  // Specific heat of fluid (J/kg deltaC)
                                                 Real64 SetPointTemp,        // input, Mixed tank set point temperature
                                                 Real64 &SourceMassFlowRate, // source mass flow rate (kg/s)
                                                 Real64 &Qheatpump,          // heat transfer rate from heat pump
                                                 Real64 &Qsource             // steady state heat transfer rate from a constant source side flow
    );

    Real64 CalcTimeNeeded(Real64 Ti, // Initial tank temperature (C)
                          Real64 Tf, // Final tank temperature (C)
                          Real64 Ta, // Ambient environment temperature (C)
                          Real64 T1, // Temperature of flow 1 (C)
                          Real64 T2, // Temperature of flow 2 (C)
                          Real64 m,  // Mass of tank fluid (kg)
                          Real64 Cp, // Specific heat of fluid (J/kg deltaC)
                          Real64 m1, // Mass flow rate 1 (kg/s)
                          Real64 m2, // Mass flow rate 2 (kg/s)
                          Real64 UA, // Heat loss coefficient to ambient environment (W/deltaC)
                          Real64 Q   // Net heating rate for non-temp dependent sources, i.e. heater and parasitics (W)
    );

    Real64 CalcTankTemp(Real64 Ti, // Initial tank temperature (C)
                        Real64 Ta, // Ambient environment temperature (C)
                        Real64 T1, // Temperature of flow 1 (C)
                        Real64 T2, // Temperature of flow 2 (C)
                        Real64 m,  // Mass of tank fluid (kg)
                        Real64 Cp, // Specific heat of fluid (J/kg deltaC)
                        Real64 m1, // Mass flow rate 1 (kg/s)
                        Real64 m2, // Mass flow rate 2 (kg/s)
                        Real64 UA, // Heat loss coefficient to ambient environment (W/deltaC)
                        Real64 Q,  // Net heating rate for non-temp dependent sources, i.e. heater and parasitics (W)
                        Real64 t   // Time elapsed from Ti to Tf (s)
    );

    Real64 CalcTempIntegral(Real64 Ti, // Initial tank temperature (C)
                            Real64 Tf, // Final tank temperature (C)
                            Real64 Ta, // Ambient environment temperature (C)
                            Real64 T1, // Temperature of flow 1 (C)
                            Real64 T2, // Temperature of flow 2 (C)
                            Real64 m,  // Mass of tank fluid (kg)
                            Real64 Cp, // Specific heat of fluid (J/kg deltaC)
                            Real64 m1, // Mass flow rate 1 (kg/s)
                            Real64 m2, // Mass flow rate 2 (kg/s)
                            Real64 UA, // Heat loss coefficient to ambient environment (W/deltaC)
                            Real64 Q,  // Net heating rate for non-temp dependent sources, i.e. heater and parasitics (W)
                            Real64 t   // Time elapsed from Ti to Tf (s)
    );

    Real64 PartLoadFactor(int WaterThermalTankNum, Real64 PartLoadRatio);

    void CalcWaterThermalTankStratified(int WaterThermalTankNum // Water Heater being simulated
    );

    void CalcNodeMassFlows(int WaterThermalTankNum, // Water Heater being simulated
                           int InletMode            // InletModeFixed or InletModeSeeking
    );

    void CalcDesuperheaterWaterHeater(int WaterThermalTankNum, // Water Heater being simulated
                                      bool FirstHVACIteration  // TRUE if First iteration of simulation
    );

    void CalcHeatPumpWaterHeater(int WaterThermalTankNum, // Water Heater tank being simulated
                                 bool FirstHVACIteration  // TRUE if First iteration of simulation
    );

    void CalcWaterThermalTank(int WaterThermalTankNum);

    Real64 GetHPWHSensedTankTemp(WaterThermalTankData const &Tank);

    void ConvergeSingleSpeedHPWHCoilAndTank(int WaterThermalTankNum, Real64 PartLoadRatio);

    Real64 PLRResidualIterSpeed(Real64 SpeedRatio,  // speed ratio between two speed levels
                                Array1<Real64> const &Par //
    );

    Real64 PLRResidualWaterThermalTank(Real64 HPPartLoadRatio, // compressor cycling ratio (1.0 is continuous, 0.0 is off)
                                Array1<Real64> const &Par     // par(1) = HP set point temperature [C]
    );

    Real64 PLRResidualHPWH(Real64 HPPartLoadRatio, Array1<Real64> const &Par);

    bool SourceHeatNeed(WaterThermalTankData WaterThermalTank,
                        Real64 OutletTemp,
                        Real64 DeadBandTemp,
                        Real64 SetPointTemp);

    Real64 PlantMassFlowRatesFunc(int WaterThermalTankNum,
                                  int InNodeNum,
                                  bool FirstHVACIteration,
                                  int WaterThermalTankSide,
                                  int PlantLoopSide,
                                  bool PlumbedInSeries, // !unused1208
                                  int BranchControlType,
                                  Real64 OutletTemp,
                                  Real64 DeadBandTemp,
                                  Real64 SetPointTemp);

    void MinePlantStructForInfo(int WaterThermalTankNum);

    void SizeSupplySidePlantConnections(int WaterThermalTankNum, Optional_int_const LoopNum = _, Optional_int_const LoopSideNum = _);

    void SizeTankForDemandSide(int WaterThermalTankNum);

    void SizeTankForSupplySide(int WaterThermalTankNum);

    void SizeDemandSidePlantConnections(int WaterThermalTankNum);

    void SizeStandAloneWaterHeater(int WaterThermalTankNum);

    void UpdateWaterThermalTank(int WaterThermalTankNum);

    void ReportWaterThermalTank(int WaterThermalTankNum);

    void CalcStandardRatings(int WaterThermalTankNum);

    void ReportCWTankInits(int WaterThermalTankNum);

    Real64 FindStratifiedTankSensedTemp(WaterThermalTankData const &Tank, bool UseAverage = false);

    void SetVSHPWHFlowRates(int WaterThermalTankNum, // Water Heater tank being simulated
                            int HPNum,
                            int SpeedNum,
                            Real64 SpeedRatio,
                            Real64 WaterDens,
                            Real64 &MdotWater,            // water flow rate
                            bool FirstHVACIteration // TRUE if First iteration of simulation
    );

    void clear_state();

} // namespace WaterThermalTanks

} // namespace EnergyPlus

#endif
