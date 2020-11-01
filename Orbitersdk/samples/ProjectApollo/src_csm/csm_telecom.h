/***************************************************************************
  This file is part of Project Apollo - NASSP
  Copyright 2004-2005

  Telecommunications Subssytems
  The implementation is in satsystems.cpp

  Project Apollo is free software; you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation; either version 2 of the License, or
  (at your option) any later version.

  Project Apollo is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with Project Apollo; if not, write to the Free Software
  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA

  See http://nassp.sourceforge.net/license/ for more details.

  **************************************************************************/

/* PCM DOWN-TELEMETRY

	HBR FRAME:
	8 bit words, 128 words per frame.
			
	WORD	ALL			B			C			D			E			F
	1		51DP1A	
	2		51DP1B
	3		51DP1C
	4		51DP1D
	5		22A1
	6		22A2
	7		22A3
	8		22A4
	9					11A1		11A37		11A73		11A109		11A145
	10					11A2		11A38		11A74		11A110		11A146
	11					11A3		11A39		11A75		11A111		11A147
	12					11A4		11A40		11A76		11A112		11A148
	13		12A1
	14		12A2
	15		12A3
	16		12A4
	17					11A5		11A41		11A77		11A113		11A149
	18		22DP1
	19		22DP2
	20					10DP1		SRC 0		SRC 1		... and subsequent up to 50 ...
	21		12A5
	22		12A6
	23		12A7
	24		12A8
	25					11A6		11A42		11A78		11A114		11A150
	26					11A7		11A43		11A79		11A115		11A151
	27					11A8		11A44		11A80		11A116		11A152
	28					11A9		11A45		11A81		11A117		11A153
	29		51A1
	30		51A2
	31		51A3
	32		51DS1A
	33		51DS1B
	34		51DS1C
	35		51DS1D
	36		51DS1E
	37		22A1
	38		22A2
	39		22A3
	40		22A4
	41					11A10		11A46		11A82		11A118		11A154
	42					11A11		11A47		11A83		11A119		11A155
	43					11A12		11A48		11A84		11A120		11A156
	44					11A13		11A49		11A85		11A121		11A157
	45		12A9
	46		12A10
	47		12A11
	48		12A12
	49					11A14		11A50		11A86		11A122		11A158
	50		22DP1
	51		22DP2
	52					10A1		10A4		10A7		... and subsequent up to 10A148 at frame 50 ...
	53		12A13
	54		12A14
	55		12A15
	56		12A16
	57					11A15		11A51		11A87		11A123		11A159
	58					11A16		11A52		11A88		11A124		11A160
	59					11A17		11A53		11A89		11A125		11A161
	60					11A18		11A54		11A90		11A126		11A162
	61		51A4
	62		51A5
	63		51A6
	64		51A7
	65					11DP2A		11DP6		11DP13		11DP20		11DP27
	66					11DP2B		11DP7		11DP14		11DP21		11DP28
	67					11DP2C		11DP8		11DP15		11DP22		11DP29
	68					11DP2D		11DP9		11DP16		11DP23		11DP30
	69		22A1
	70		22A2
	71		22A3	
	72		22A4
	73					11A19		11A55		11A91		11A127		11A163
	74					11A20		11A56		11A92		11A128		11A164
	75					11A21		11A57		11A93		11A129		11A165
	76					11A22		11A58		11A94		11A130		11A166
	77		12A1
	78		12A2
	79		12A3
	80		12A4
	81					11A23		11A59		11A95		11A131		11A167
	82		22DP1
	83		22DP2
	84					10A2		10A5		10A8		... and subsequent up to 10A149 at frame 50 ...
	85		12A5
	86		12A6
	87		12A7
	88		12A8
	89					11A24		11A60		11A96		11A132		11A168
	90					11A25		11A61		11A97		11A133		11A169
	91					11A26		11A62		11A98		11A134		11A170
	92					11A27		11A63		11A99		11A135		11A171
	93		51A8
	94		51A9
	95		51A10
	96		51A11
	97					11DP3		11DP10		11DP17		11DP24		11DP31
	98					11DP4		11DP11		11DP18		11DP25		11DP32
	99					11DP5		11DP12		11DP19		11DP26		11DP33
	100		51DP2
	101		22A1	
	102		22A2
	103		22A3
	104		22A4
	105					11A28		11A64		11A100		11A136		11A172
	106					11A29		11A65		11A101		11A137		11A173
	107					11A30		11A66		11A102		11A138		11A174
	108					11A31		11A67		11A103		11A139		11A175
	109		12A9
	110		12A10
	111		12A11
	112		12A12
	113					11A32		11A68		11A104		11A140		11A176
	114		22DP1
	115		22DP2
	116					10A3		10A6		10A9		... and subsequent up to 10A150 at frame 50 ...
	117		12A13
	118		12A14
	119		12A15
	120		12A16
	121					11A33		11A69		11A105		11A141		11A177
	122					11A34		11A70		11A106		11A142		11A178
	123					11A35		11A71		11A107		11A143		11A179
	124					11A36		11A72		11A108		11A144		11A180
	125		51A12
	126		51A13
	127		51A14
	128		51A15

	LBR FRAME:

	MEASUREMENTS:
	ID		WHAT						VALUE OR NOTES
	51DP1A	SYNC 1						00000101   (Octal 5)
	51DP1B	SYNC 2						01111001   (Octal 171)
	51DP1C	SYNC 3						10110111   (Octal 267)
	51DP1D	SYNC 4 & FRAME COUNT		(1 thru 50?)

	*/

// DS20070108 Telemetry measurement types
#define TLM_A	1
#define TLM_DP	2
#define TLM_DS	3
#define TLM_E	4
#define TLM_SRC 5

// DS20060326 Telecommunications system objects
class Saturn;

//
// Data storage equipment. Aka data recorder.
//
// Note that Apollo 15 and later used upgraded recorders which ran at half the speed with double the data density.
//

/// High-bit-rate chunk holds 160 bytes.
const unsigned int dseChunkSizeHBR = 160;

/// Low-bit-rate chunk holds 80 bytes.
const unsigned int dseChunkSizeLBR = 80;

///
/// Data storage chunk. Represents 1.5 inches of tape.
///
class DSEChunk
{

enum DSEChunkType
{
	DSEEMPTY,		/// No data, erased or not written
	DSEHBR,			/// High bit-rate chunk (51200bps)
	DSELBR			/// Low bit-rate chunk (1600bps)
};

public:
	DSEChunk();
	virtual ~DSEChunk();
	void Erase( const DSEChunkType dataType );

private:

	void deleteData();

	DSEChunkType chunkType;			/// What type of chunk is this?
	unsigned char *chunkData;		/// Pointer to chunk data.
	unsigned int chunkSize;			/// Size of chunk.
	unsigned int chunkValidBytes;	/// Number of valid bytes in the chunk.
};

const unsigned int tapeSize = 18000;

///
/// DSE holds 27,000 inches of tape, or 18,000 chunks.
///
class DSE : public e_object
{
enum DSEState
{
	STOPPED,			/// Tape is stopped
	STARTING_PLAY,		/// Tape is accelerating to play speed
	STARTING_RECORD,	/// Tape is accelerating to play speed
	SLOWING_RECORD,		/// Tape is slowing to record speed
	PLAYING,			/// Tape is playing
	RECORDING,			/// Tape is recording
	STOPPING,			/// Tape is stopping
};

public:
	DSE();
	virtual ~DSE();

	void Init(Saturn *vessel);	       // Initialization

	///
	/// \brief Tape motion indicator.
	///
	bool TapeMotion(); 

	///
	/// \brief Start tape playing.
	///
	void Play();

	///
	/// \brief Stop tape playing.
	///
	void Stop();

	///
	/// \brief Start tape recording.
	///
	void Record( bool hbr );

	///
	/// \brief Timestep processing.
	///
	void TimeStep( double simt, double simdt );

	void LoadState(char *line);
	void SaveState(FILEHANDLE scn);

protected:
	Saturn *sat;					    /// Ship we're installed in
	DSEChunk tape[tapeSize];			/// Simulated tape.
	double tapeSpeedInchesPerSecond;	/// Tape speed in inches per second.
	double desiredTapeSpeed;			/// Desired tape speed in inches per second.
	double tapeMotion;					/// Tape motion from 0.0 to 1.0.
	DSEState state;						/// Tape state.

	double lastEventTime;				/// Last event time.
};

// PCM system
class PCM {
public:		
	PCM();                          // Cons
	void Init(Saturn *vessel);	    // Initialization
	void TimeStep(double simt);     // TimeStep
	void SystemTimestep(double simdt); // System Timestep (consume power)

	// Winsock2
	WSADATA wsaData;				// Winsock subsystem data
	SOCKET m_socket;				// TCP socket
	sockaddr_in service;			// SOCKADDR_IN
	SOCKET AcceptSocket;			// Accept Socket
	int conn_state;                 // Connection State
	int uplink_state;               // Uplink State
	void perform_io(double simt);   // Get data from here to there
	void handle_uplink();	// Handle incoming data
	void generate_stream_lbr();     // Generate LBR datastream
	void generate_stream_hbr();     // Same for HBR datastream
	unsigned char scale_data(double data, double low, double high); // Scale data for PCM transmission
	unsigned char measure(int channel, int type, int ccode);

	// Error control
	int wsk_error;                  // Winsock error
	char wsk_emsg[256];             // Winsock error message
	
	// PCM datastream management
	double last_update;				// simt of last update
	double last_rx;                 // simt of last uplink update
	int word_addr;                  // Word address of outgoing packet
	int frame_addr;                 // Frame address
	int frame_count;				// Frame counter
	int tx_size;                    // Number of words to send
	int tx_offset;                  // Offset to use
	int rx_offset;					// RX offset to use
	int mcc_offset;					// RX offset into MCC data block
	int mcc_size;					// Size of MCC data block
	int pcm_rate_override;          // Downtelemetry rate override
	unsigned char tx_data[1024];    // Characters to be transmitted
	unsigned char rx_data[1024];    // Characters recieved
	unsigned char mcc_data[2048];	// MCC-provided incoming data

	bool registerSocket(SOCKET sock);

	Saturn *sat;					// Ship we're installed in
	friend class MCC;				// Allow MCC to write directly to buffer
};

// Premodulation Processor
class PMP {
public:
	PMP();
	void Init(Saturn *vessel);	       // Initialization
	void TimeStep(double simt);        // TimeStep
	void SystemTimestep(double simdt); // System Timestep
	Saturn *sat;					   // Ship we're installed in
};

// Generic S-Band Antenna
class SBandAntenna
{
public:
	SBandAntenna() { SignalStrength = 0.0; }
	double GetSignalStrength() { return SignalStrength; }
	double dBm2SignalStrength(double dBm);
protected:
	double SignalStrength;						// Signal Strength (0-100)
};

// Unified S-Band system
// Two transponders, an FM transmitter, and a power supply
// The transponders are used for primary communications (data and voice), and only one works at a time.

class USB {
public:
	USB();
	void Init(Saturn *vessel);	       // Initialization
	void TimeStep(double simt);        // TimeStep
	void SystemTimestep(double simdt); // System Timestep
	void LoadState(char *line);
	void SaveState(FILEHANDLE scn);

	Saturn *sat;					   // Ship we're installed in
	int fm_ena;                        // FM transmitter enable relays from uptelemetry
	int xpdr_sel;                      // Transponder select
	bool fm_opr;                       // FM transmitter operating
	int pa_mode_1, pa_mode_2;          // Power amplifier mode
	double pa_timer_1, pa_timer_2;	   // Tube heater timer
	int pa_ovr_1, pa_ovr_2;				// PA mode override for uptelemetry channel
	double rcvr_agc_voltage;			//Receiver AGC Voltage
	SBandAntenna *ant;
};

// High Gain Antenna system

class HGA:public SBandAntenna {
public:
	HGA();
	void Init(Saturn *vessel);					// Initialization
	void DefineAnimations(UINT idx);
	void DeleteAnimations();
	void TimeStep(double simt, double simdt);   // TimeStep
	void SystemTimestep(double simdt);			// System Timestep
	void LoadState(char *line);
	void SaveState(FILEHANDLE scn);
	bool ScanLimitWarning();
	bool IsPowered();
	void clbkPostCreation();
	double ModeSwitchTimer;
	int RcvBeamWidthSelect = 0; // 0 = none, 1 = Wide, 2 = Med, 3 = Narrow
	int XmtBeamWidthSelect = 0; // 0 = none, 1 = Wide, 2 = Med, 3 = Narrow
	bool AutoTrackingMode;
	double HGAWavelength;
	double HGAFrequency;
	double Gain85ft;
	double Power85ft;

	double GetResolvedPitch() { return PitchRes * DEG; }
	double GetResolvedYaw() { return YawRes * DEG; }
	double GetAlpha() { return Alpha; }
	double GetBeta() { return Beta; }
	double GetGamma() { return Gamma; }

private:
	VECTOR3 PitchYawToBodyVector(double pit, double ya);
	void BodyVectorToPitchYaw(VECTOR3 U_R, double &pitch, double &yaw);
	void BodyToAC(VECTOR3 U_R, double &alpha, double &gamma);
	VECTOR3 ABCAndVectorToBody(double alpha, double beta, double gamma, VECTOR3 U_R);
	void ServoDrive(double &Angle, double AngleCmd, double RateLimit, double simdt);

	Saturn *sat;								// Ship we're installed in
	double Alpha;								// Antenna alpha
	double Beta;								// Antenna beta
	double Gamma;								// Antenna gamma
	double AAxisCmd;
	double BAxisCmd;
	double CAxisCmd;
	double PitchRes;
	double YawRes;
	bool scanlimit;
	bool scanlimitwarn;
	double HornSignalStrength[4];

	VECTOR3 U_Horn[4];

	// Animations
	UINT anim_HGAalpha, anim_HGAbeta, anim_HGAgamma;
	double	hga_proc[3];
	double	hga_proc_last[3];
};

//S-Band Omnidirectional Antenna system

class OMNI:public SBandAntenna {
public:
	OMNI(VECTOR3 dir);
	void Init(Saturn *vessel);	// Initialization
	void TimeStep();			// TimeStep
	double OMNIWavelength;
	double OMNIFrequency;
	double Gain30ft;
	double Power30ft;
	double OMNI_Gain;
protected:
	Saturn *sat;				// Ship we're installed in
	VECTOR3 direction;
	double hpbw_factor;			//Beamwidth factor
	OBJHANDLE hMoon;
	OBJHANDLE hEarth;
};

class VHFAntenna
{
public:
	VHFAntenna(VECTOR3 dir);
};

class VHFAMTransceiver
{
public:
	VHFAMTransceiver();
	void Timestep();
	void Init(ThreePosSwitch *vhfASw, ThreePosSwitch *vhfBSw, ThreePosSwitch *rcvSw, CircuitBrakerSwitch *ctrpowcb);
	void LoadState(char *line);
	void SaveState(FILEHANDLE scn);
	bool IsVHFRangingConfig() { return (receiveA && !receiveB && !transmitA && transmitB); }
protected:
	bool K1;
	bool K2;

	bool receiveA;
	bool receiveB;
	bool transmitA;
	bool transmitB;

	ThreePosSwitch *vhfASwitch;
	ThreePosSwitch *vhfBSwitch;
	ThreePosSwitch *rcvSwitch;
	CircuitBrakerSwitch *ctrPowerCB;
};

class LEM;

class VHFRangingSystem
{
public:
	VHFRangingSystem();
	void Init(Saturn *vessel, CircuitBrakerSwitch *cb, ToggleSwitch *powersw, ToggleSwitch *resetsw, VHFAMTransceiver *transc);
	void TimeStep(double simdt);
	void SystemTimestep(double simdt);
	bool IsPowered();
	void LoadState(char *line);
	void SaveState(FILEHANDLE scn);

	double GetRange() { return range / 185.20; }
	void RangingReturnSignal();
protected:

	bool dataGood;
	double internalrange;
	double range;
	bool isRanging;
	double phaseLockTimer;
	int hasLock;

	Saturn *sat;
	LEM *lem;
	VHFAMTransceiver *transceiver;
	CircuitBrakerSwitch *powercb;
	ToggleSwitch *powerswitch;
	ToggleSwitch *resetswitch;
};

class CSM_RRTto_LM_RRConnector;

class RNDZXPDRSystem
{
public:
	RNDZXPDRSystem();
	~RNDZXPDRSystem();
	void Init(Saturn *vessel, CircuitBrakerSwitch *PowerCB, ToggleSwitch *RNDZXPDRSwitch, ThreePosSwitch *Panel100RNDZXPDRSwitch, RotationalSwitch *LeftSystemTestRotarySwitch, RotationalSwitch *RightSystemTestRotarySwitch);
	void TimeStep(double simdt);
	void SystemTimestep(double simdt);
	void LoadState(char *line);
	void SaveState(FILEHANDLE scn);
	void SetRCVDrfProp(double freq, double pow, double gain, double phase) { RCVDfreq = freq; RCVDpow = pow; RCVDgain = gain; RCVDPhase = phase; };

	//these values are for the LEB101 test meter gauge
	unsigned char GetScaledRFPower(); //RF power, converts a -122 to -18 dBm signal to a 2.1V to 5V output 
	unsigned char GetScaledAGCPower(); //Automatic Gain Control, converts an - (-18) to - (-122) dBm signal to 0 to 4.5V output(AGV voltage down up when RF power received goes down)
	unsigned char GetScaledFreqLock(); //frequency lock, output 4.5V when locked (use lockTimer as input to simulate freq locking)

protected:
	double GetCSMGain(double theta, double phi); //returns the gain of the csm RRT system for returned power calculations
	void SendRF();

	Saturn *sat;
	LEM *lem;

	double XMITpower;
	double RCVDfreq;
	double RCVDpow;
	double RCVDgain;
	double RCVDPhase;
	double RCVDPowerdB;
	double RNDZXPDRGain;
	double RadarDist; //distance from CSM to LEM;
	double theta, phi;
	

	double lockTimer;

	enum FreqLock {
		UNLOCKED,
		LOCKED,
	};

	FreqLock haslock;
	bool XPDRon;
	bool XPDRheaterOn;
	bool XPDRtest;

	VECTOR3 lemPos;
	VECTOR3 csmPos;
	VECTOR3 R;
	VECTOR3 U_R;
	VECTOR3 U_R_RR;
	MATRIX3 LMRot, CSMRot;

	//not implimented yet
	//RRT_RFpowerXDUCER RFpowerXDUCER;
	//RRT_AGC_XDUCER AGC_XDUCER;
	//RRT_FREQLOCK_XDUCER FREQLOCK_XDUCER;

	CircuitBrakerSwitch *RRT_FLTBusCB;
	ToggleSwitch *TestOperateSwitch; //test operate switch
	ThreePosSwitch *HeaterPowerSwitch; //heater/power switch
	RotationalSwitch *RRT_LeftSystemTestRotarySwitch;
	RotationalSwitch *RRT_RightSystemTestRotarySwitch;
};

//not implimented yet
//class RRT_RFpowerXDUCER : public Transducer //RF power tranducer, converts a -122 to -18 dBm signal to a 2.1V to 5V output
//{
//	
//};
//
//class RRT_AGC_XDUCER : public Transducer//Automatic Gain Control transducer, converts an -(-18) to -(-122) dBm signal to 0 to 4.5V output (AGV voltage down up when RF power received goes down)
//{
//
//};
//
//class RRT_FREQLOCK_XDUCER : public Transducer //frequency lock transducer, output 4.5V when locked (use lockTimer as input to simulate freq locking)
//{
//
//};