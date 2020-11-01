#include "ARCore.h"

#include "soundlib.h"
#include "apolloguidance.h"
#include "csmcomputer.h"
#include "IMU.h"
#include "saturn.h"
#include "saturnv.h"
#include "iu.h"
#include "LVDC.h"
#include "LEM.h"
#include "sivb.h"
#include "mcc.h"
#include "TLMCC.h"
#include "rtcc.h"

static WSADATA wsaData;
static SOCKET m_socket;
static sockaddr_in clientService;
static SOCKET close_Socket = INVALID_SOCKET;
static char debugString[100];
static char debugStringBuffer[100];
static char debugWinsock[100];

static DWORD WINAPI RTCCMFD_Trampoline(LPVOID ptr) {
	ARCore *core = (ARCore *)ptr;
	return(core->subThread());
}

AR_GCore::AR_GCore(VESSEL* v)
{
	rtcc = new RTCC();

	MissionPlanningActive = false;
	pCSMnumber = -1;
	pLMnumber = -1;
	pCSM = pLM = NULL;
	mptInitError = 0;

	mission = 0;
	rtcc->BZLSDISP.lat[RTCC_LMPOS_BEST] = 0.0;
	rtcc->BZLSDISP.lng[RTCC_LMPOS_BEST] = 0.0;
	t_Land = 0.0;
	rtcc->PZLTRT.DT_Ins_TPI = rtcc->PZLTRT.DT_Ins_TPI_NOM = 40.0*60.0;
	rtcc->GZGENCSN.TIPhaseAngle = 0.0;

	if (strcmp(v->GetName(), "AS-205") == 0)
	{
		mission = 7;
	}
	else if (strcmp(v->GetName(), "AS-503") == 0)
	{
		mission = 8;
	}
	else if (strcmp(v->GetName(), "AS-504") == 0 || strcmp(v->GetName(), "Spider") == 0)
	{
		mission = 9;
	}
	else if (strcmp(v->GetName(), "AS-505") == 0 || strcmp(v->GetName(), "Charlie-Brown") == 0 || strcmp(v->GetName(), "Snoopy") == 0)
	{
		mission = 10;
	}
	else if (strcmp(v->GetName(), "AS-506") == 0 || strcmp(v->GetName(), "Columbia") == 0 || strcmp(v->GetName(), "Eagle") == 0)
	{
		mission = 11;
	}
	else if (strcmp(v->GetName(), "Yankee-Clipper") == 0 || strcmp(v->GetName(), "Intrepid") == 0)
	{
		mission = 12;
	}
	else if (strcmp(v->GetName(), "Odyssey") == 0 || strcmp(v->GetName(), "Aquarius") == 0)
	{
		mission = 13;
	}
	else if (strcmp(v->GetName(), "Kitty-Hawk") == 0 || strcmp(v->GetName(), "Antares") == 0)
	{
		mission = 14;
	}
	else if (strcmp(v->GetName(), "Endeavour") == 0 || strcmp(v->GetName(), "Falcon") == 0)
	{
		mission = 15;
	}
	else if (strcmp(v->GetName(), "Casper") == 0 || strcmp(v->GetName(), "Orion") == 0)
	{
		mission = 16;
	}
	else if (strcmp(v->GetName(), "America") == 0 || strcmp(v->GetName(), "Challenger") == 0)
	{
		mission = 17;
	}

	SetMissionSpecificParameters();
}

AR_GCore::~AR_GCore()
{
	delete rtcc;
}

void AR_GCore::SetMissionSpecificParameters()
{
	rtcc->AGCEpoch = 40221.525;
	if (mission == 7)
	{
		rtcc->MCLSDA = sin(28.5217969*RAD);
		rtcc->MCLCDA = cos(28.5217969*RAD);
		rtcc->MCLGRA = -80.5612465*RAD;

		rtcc->GMGMED("P80,1,CSM,10,11,1968;");
		rtcc->GMGMED("P10,CSM,15:02:45;");
		rtcc->GMGMED("P12,CSM,15:02:45,72.0;");
	}
	else if (mission == 8)
	{
		rtcc->BZLSDISP.lat[RTCC_LMPOS_BEST] = 2.6317*RAD;
		rtcc->BZLSDISP.lng[RTCC_LMPOS_BEST] = 34.0253*RAD;
		rtcc->MCSMLR = OrbMech::R_Moon - 0.82*1852.0;
		rtcc->med_k18.psi_DS = -78.0 + 360.0;
		rtcc->med_k40.HP_LLS = 60.0;
		t_Land = OrbMech::HHMMSSToSS(82.0, 8.0, 26.0);
		rtcc->PZREAP.RRBIAS = 1350.0;

		rtcc->PZMCCPLN.LOPC_M = 1;
		rtcc->PZMCCPLN.LOPC_N = 0;
		rtcc->PZMCCPLN.AZ_min = rtcc->PZMCCPLN.AZ_max = -78.0*RAD;
		rtcc->PZMCCPLN.REVS2 = rtcc->med_k40.REVS2 = 4;

		rtcc->PZSFPTAB.blocks[0].GMT_pc1 = rtcc->PZSFPTAB.blocks[0].GMT_pc2 = rtcc->PZSFPTAB.blocks[0].GMT_nd = OrbMech::HHMMSSToSS(82, 0, 0);
		rtcc->PZSFPTAB.blocks[0].lat_pc1 = rtcc->PZSFPTAB.blocks[0].lat_pc2 = rtcc->PZSFPTAB.blocks[0].lat_nd = -5.67822*RAD;
		rtcc->PZSFPTAB.blocks[0].lng_pc1 = rtcc->PZSFPTAB.blocks[0].lng_pc2 = rtcc->PZSFPTAB.blocks[0].lng_nd = 180.0*RAD;
		rtcc->PZSFPTAB.blocks[0].h_pc1 = rtcc->PZSFPTAB.blocks[0].h_pc2 = rtcc->PZSFPTAB.blocks[0].h_nd = 60.0*1852.0;
		rtcc->PZSFPTAB.blocks[0].GET_TLI = OrbMech::HHMMSSToSS(2, 50, 31);
		rtcc->PZSFPTAB.blocks[0].dpsi_loi = 2.4*RAD;
		rtcc->PZSFPTAB.blocks[0].gamma_loi = 0.0;
		rtcc->PZSFPTAB.blocks[0].T_lo = OrbMech::HHMMSSToSS(20, 3, 30);
		rtcc->PZSFPTAB.blocks[0].dt_lls = OrbMech::HHMMSSToSS(12, 59, 26);
		rtcc->PZSFPTAB.blocks[0].psi_lls = -78.0*RAD;
		rtcc->PZSFPTAB.blocks[0].lat_lls = 2.6317*RAD;
		rtcc->PZSFPTAB.blocks[0].lng_lls = 34.0253*RAD;
		rtcc->PZSFPTAB.blocks[0].rad_lls = OrbMech::R_Moon - 0.82*1852.0;
		rtcc->PZSFPTAB.blocks[0].dpsi_tei = -2.3*RAD;
		rtcc->PZSFPTAB.blocks[0].dv_tei = 3531.7*0.3048;
		rtcc->PZSFPTAB.blocks[0].T_te = OrbMech::HHMMSSToSS(57, 30, 35);
		rtcc->PZSFPTAB.blocks[0].incl_fr = 33.73*RAD;

		rtcc->GMGMED("P80,1,CSM,12,21,1968;");
		rtcc->GMGMED("P10,CSM,12:51:0;");
		rtcc->GMGMED("P12,CSM,12:51:0,72.0;");
	}
	else if (mission == 9)
	{
		rtcc->GMGMED("P80,1,CSM,3,3,1969;");
		rtcc->GMGMED("P10,CSM,16:00:00;");
		rtcc->GMGMED("P12,CSM,16:00:00,72.0;");

		rtcc->MCCLEX = 3431;
		rtcc->MCCLRF = 1735;
	}
	else if (mission == 10)
	{
		rtcc->BZLSDISP.lat[RTCC_LMPOS_BEST] = 0.732*RAD;
		rtcc->BZLSDISP.lng[RTCC_LMPOS_BEST] = 23.647*RAD;
		rtcc->MCSMLR = OrbMech::R_Moon -1.66*1852.0;
		rtcc->med_k18.psi_DS = -91.0 + 360.0;
		rtcc->med_k40.HP_LLS = 60.0;
		t_Land = OrbMech::HHMMSSToSS(100.0, 46.0, 19.0);
		rtcc->PZREAP.RRBIAS = 1285.0;

		rtcc->PZMCCPLN.AZ_min = rtcc->PZMCCPLN.AZ_max = -91.0*RAD;

		rtcc->PZSFPTAB.blocks[0].GMT_pc1 = rtcc->PZSFPTAB.blocks[0].GMT_pc2 = rtcc->PZSFPTAB.blocks[0].GMT_nd = OrbMech::HHMMSSToSS(92, 39, 0);
		rtcc->PZSFPTAB.blocks[0].lat_pc1 = rtcc->PZSFPTAB.blocks[0].lat_pc2 = rtcc->PZSFPTAB.blocks[0].lat_nd = -4.933294*RAD;
		rtcc->PZSFPTAB.blocks[0].lng_pc1 = rtcc->PZSFPTAB.blocks[0].lng_pc2 = rtcc->PZSFPTAB.blocks[0].lng_nd = 180.0*RAD;
		rtcc->PZSFPTAB.blocks[0].h_pc1 = rtcc->PZSFPTAB.blocks[0].h_pc2 = rtcc->PZSFPTAB.blocks[0].h_nd = 60.0*1852.0;
		rtcc->PZSFPTAB.blocks[0].GET_TLI = OrbMech::HHMMSSToSS(2, 33, 26);
		rtcc->PZSFPTAB.blocks[0].dpsi_loi = -6.1*RAD;
		rtcc->PZSFPTAB.blocks[0].gamma_loi = 0.0;
		rtcc->PZSFPTAB.blocks[0].T_lo = OrbMech::HHMMSSToSS(61, 31, 0);
		rtcc->PZSFPTAB.blocks[0].dt_lls = OrbMech::HHMMSSToSS(24, 40, 45);
		rtcc->PZSFPTAB.blocks[0].psi_lls = -91.0*RAD;
		rtcc->PZSFPTAB.blocks[0].lat_lls = 0.732*RAD;
		rtcc->PZSFPTAB.blocks[0].lng_lls = 23.647*RAD;
		rtcc->PZSFPTAB.blocks[0].rad_lls = OrbMech::R_Moon - 1.66*1852.0;
		rtcc->PZSFPTAB.blocks[0].dpsi_tei = -0.2*RAD;
		rtcc->PZSFPTAB.blocks[0].dv_tei = 3622.5*0.3048;
		rtcc->PZSFPTAB.blocks[0].T_te = OrbMech::HHMMSSToSS(54, 29, 32);
		rtcc->PZSFPTAB.blocks[0].incl_fr = 40.0*RAD;

		rtcc->PZMCCPLN.LOPC_M = 0;
		rtcc->PZMCCPLN.LOPC_N = 0;
		rtcc->PZMCCPLN.REVS2 = rtcc->med_k40.REVS2 = 10;

		rtcc->GMGMED("P80,1,CSM,5,18,1969;");
		rtcc->GMGMED("P10,CSM,16:49:00;");
		rtcc->GMGMED("P12,CSM,16:49:00,72.0;");

		rtcc->MCLSDA = sin(28.608202*RAD);
		rtcc->MCLCDA = cos(28.608202*RAD);
		rtcc->MCLGRA = -80.602087222*RAD;
		rtcc->MCCLEX = 3431;
	}
	else if (mission == 11) // July 16th Launch
	{
		rtcc->AGCEpoch = 40586.767239;

		rtcc->BZLSDISP.lat[RTCC_LMPOS_BEST] = 0.71388888*RAD;
		rtcc->BZLSDISP.lng[RTCC_LMPOS_BEST] = 23.7077777*RAD;
		rtcc->MCSMLR = OrbMech::R_Moon -3073.263;
		rtcc->med_k18.psi_DS = -91.0 + 360.0;
		rtcc->med_k40.HP_LLS = 60.0;
		t_Land = OrbMech::HHMMSSToSS(102.0, 47.0, 11.0);
		rtcc->PZREAP.RRBIAS = 1285.0;

		rtcc->PZMCCPLN.AZ_min = rtcc->PZMCCPLN.AZ_max = -91.0*RAD;

		rtcc->PZSFPTAB.blocks[0].GMT_pc1 = rtcc->PZSFPTAB.blocks[0].GMT_pc2 = rtcc->PZSFPTAB.blocks[0].GMT_nd = OrbMech::HHMMSSToSS(89, 25, 35);
		rtcc->PZSFPTAB.blocks[0].lat_pc1 = rtcc->PZSFPTAB.blocks[0].lat_pc2 = rtcc->PZSFPTAB.blocks[0].lat_nd = 0.26206227*RAD;
		rtcc->PZSFPTAB.blocks[0].lng_pc1 = rtcc->PZSFPTAB.blocks[0].lng_pc2 = rtcc->PZSFPTAB.blocks[0].lng_nd = 179.81579*RAD;
		rtcc->PZSFPTAB.blocks[0].h_pc1 = rtcc->PZSFPTAB.blocks[0].h_pc2 = rtcc->PZSFPTAB.blocks[0].h_nd = 60.0*1852.0;
		rtcc->PZSFPTAB.blocks[0].GET_TLI = OrbMech::HHMMSSToSS(2, 44, 18);
		rtcc->PZSFPTAB.blocks[0].dpsi_loi = -4.4393618*RAD;
		rtcc->PZSFPTAB.blocks[0].gamma_loi = 0.0;
		rtcc->PZSFPTAB.blocks[0].T_lo = OrbMech::HHMMSSToSS(59, 30, 5);
		rtcc->PZSFPTAB.blocks[0].dt_lls = OrbMech::HHMMSSToSS(26, 40, 45);
		rtcc->PZSFPTAB.blocks[0].psi_lls = -91.0*RAD;
		rtcc->PZSFPTAB.blocks[0].lat_lls = 0.71388888*RAD;
		rtcc->PZSFPTAB.blocks[0].lng_lls = 23.7077777*RAD;
		rtcc->PZSFPTAB.blocks[0].rad_lls = OrbMech::R_Moon - 3073.263;
		rtcc->PZSFPTAB.blocks[0].dpsi_tei = 4.8067843*RAD;
		rtcc->PZSFPTAB.blocks[0].dv_tei = 3155.9987*0.3048;
		rtcc->PZSFPTAB.blocks[0].T_te = OrbMech::HHMMSSToSS(63, 30, 0);
		rtcc->PZSFPTAB.blocks[0].incl_fr = 40.0*RAD;

		rtcc->GMGMED("P80,1,CSM,7,16,1969;");
		rtcc->GMGMED("P10,CSM,13:32:00;");
		rtcc->GMGMED("P12,CSM,13:32:00,72.0;");
	}
	/*else if (mission == 11) // July 18th Launch
	{
		rtcc->AGCEpoch = 40586.767239;
		rtcc->BZLSDISP.lat[RTCC_LMPOS_BEST] = 0.35277778*RAD;
		rtcc->BZLSDISP.lng[RTCC_LMPOS_BEST] = -01.29916667*RAD;
		rtcc->MCSMLR = OrbMech::R_Moon - 1.01*1852.0;
		rtcc->med_k18.psi_DS = -89.0 + 360.0;
		rtcc->med_k40.HP_LLS = 60.0;
		t_Land = OrbMech::HHMMSSToSS(100.0, 43.0, 0.0);
		rtcc->PZREAP.RRBIAS = 1285.0;

		rtcc->PZMCCPLN.AZ_min = rtcc->PZMCCPLN.AZ_max = -89.0*RAD;

		rtcc->PZSFPTAB.blocks[0].GMT_pc1 = rtcc->PZSFPTAB.blocks[0].GMT_pc2 = rtcc->PZSFPTAB.blocks[0].GMT_nd = OrbMech::HHMMSSToSS(89, 17, 42);
		rtcc->PZSFPTAB.blocks[0].lat_pc1 = rtcc->PZSFPTAB.blocks[0].lat_pc2 = rtcc->PZSFPTAB.blocks[0].lat_nd = 2.3710876*RAD;
		rtcc->PZSFPTAB.blocks[0].lng_pc1 = rtcc->PZSFPTAB.blocks[0].lng_pc2 = rtcc->PZSFPTAB.blocks[0].lng_nd = -179.68688*RAD;
		rtcc->PZSFPTAB.blocks[0].h_pc1 = rtcc->PZSFPTAB.blocks[0].h_pc2 = rtcc->PZSFPTAB.blocks[0].h_nd = 60.0*1852.0;
		rtcc->PZSFPTAB.blocks[0].GET_TLI = OrbMech::HHMMSSToSS(2, 43, 10);
		rtcc->PZSFPTAB.blocks[0].dpsi_loi = -4.6862913*RAD;
		rtcc->PZSFPTAB.blocks[0].gamma_loi = 0.0;
		rtcc->PZSFPTAB.blocks[0].T_lo = OrbMech::HHMMSSToSS(55, 26, 12);
		rtcc->PZSFPTAB.blocks[0].dt_lls = OrbMech::HHMMSSToSS(24, 53, 48);
		rtcc->PZSFPTAB.blocks[0].psi_lls = -89.0*RAD;
		rtcc->PZSFPTAB.blocks[0].lat_lls = 0.35277778*RAD;
		rtcc->PZSFPTAB.blocks[0].lng_lls = -01.29916667*RAD;
		rtcc->PZSFPTAB.blocks[0].rad_lls = OrbMech::R_Moon - 1.01*1852.0;
		rtcc->PZSFPTAB.blocks[0].dpsi_tei = 5.9618956*RAD;
		rtcc->PZSFPTAB.blocks[0].dv_tei = 3095.6632*0.3048;
		rtcc->PZSFPTAB.blocks[0].T_te = OrbMech::HHMMSSToSS(65, 27, 01);
		rtcc->PZSFPTAB.blocks[0].incl_fr = 40.0*RAD;

		rtcc->GMGMED("P80,1,CSM,7,18,1969;");
		rtcc->GMGMED("P10,CSM,15:32:00;");
	}
	else if (mission == 11) // July 21st Launch
	{
		rtcc->AGCEpoch = 40586.767239;
		rtcc->BZLSDISP.lat[RTCC_LMPOS_BEST] = 1.67805556*RAD;
		rtcc->BZLSDISP.lng[RTCC_LMPOS_BEST] = -41.89916667*RAD;
		rtcc->MCSMLR = OrbMech::R_Moon - 1.25*1852.0;
		rtcc->med_k18.psi_DS = -86.0 + 360.0;
		rtcc->med_k40.HP_LLS = 60.0;
		t_Land = OrbMech::HHMMSSToSS(103.0, 46.0, 0.0);
		rtcc->PZREAP.RRBIAS = 1285.0;

		rtcc->PZMCCPLN.AZ_min = rtcc->PZMCCPLN.AZ_max = -86.0*RAD;

		rtcc->PZSFPTAB.blocks[0].GMT_pc1 = OrbMech::HHMMSSToSS(92, 58, 38);
		rtcc->PZSFPTAB.blocks[0].lat_pc1 = -0.48832588*RAD;
		rtcc->PZSFPTAB.blocks[0].lng_pc1 = 179.84695*RAD;
		rtcc->PZSFPTAB.blocks[0].h_pc1 = 799.9*1852.0;

		rtcc->PZSFPTAB.blocks[0].GMT_pc2 = rtcc->PZSFPTAB.blocks[0].GMT_nd = OrbMech::HHMMSSToSS(92, 58, 38);
		rtcc->PZSFPTAB.blocks[0].lat_pc2 = rtcc->PZSFPTAB.blocks[0].lat_nd = -0.48832588*RAD;
		rtcc->PZSFPTAB.blocks[0].lng_pc2 = rtcc->PZSFPTAB.blocks[0].lng_nd = 180.0*RAD;
		rtcc->PZSFPTAB.blocks[0].h_pc2 = rtcc->PZSFPTAB.blocks[0].h_nd = 60.0*1852.0;

		rtcc->PZSFPTAB.blocks[0].GET_TLI = OrbMech::HHMMSSToSS(2, 53, 11);
		rtcc->PZSFPTAB.blocks[0].dpsi_loi = -2.8424*RAD;
		rtcc->PZSFPTAB.blocks[0].gamma_loi = 0.0;
		rtcc->PZSFPTAB.blocks[0].T_lo = OrbMech::HHMMSSToSS(55, 21, 55);
		rtcc->PZSFPTAB.blocks[0].dt_lls = OrbMech::HHMMSSToSS(25, 04, 06);
		rtcc->PZSFPTAB.blocks[0].psi_lls = -86.0*RAD;
		rtcc->PZSFPTAB.blocks[0].lat_lls = 1.67805556*RAD;
		rtcc->PZSFPTAB.blocks[0].lng_lls = -41.89916667*RAD;
		rtcc->PZSFPTAB.blocks[0].rad_lls = OrbMech::R_Moon - 1.25*1852.0;
		rtcc->PZSFPTAB.blocks[0].dpsi_tei = 2.6518*RAD;
		rtcc->PZSFPTAB.blocks[0].dv_tei = 3060.6*0.3048;
		rtcc->PZSFPTAB.blocks[0].T_te = OrbMech::HHMMSSToSS(65, 52, 02);
		rtcc->PZSFPTAB.blocks[0].incl_fr = 40.0*RAD;

		rtcc->GMGMED("P80,1,CSM,7,21,1969;");
		rtcc->GMGMED("P10,CSM,16:09:00;");
	}*/
	else if (mission == 12)
	{
		rtcc->AGCEpoch = 40586.767239;

		rtcc->BZLSDISP.lat[RTCC_LMPOS_BEST] = -2.9425*RAD;
		rtcc->BZLSDISP.lng[RTCC_LMPOS_BEST] = -23.44333*RAD;
		rtcc->MCSMLR = OrbMech::R_Moon -1.19*1852.0;
		rtcc->med_k18.psi_DS = -75.0 + 360.0;
		rtcc->med_k40.HP_LLS = 60.0;
		t_Land = OrbMech::HHMMSSToSS(110.0, 31.0, 19.0);
		rtcc->PZREAP.RRBIAS = 1250.0;

		rtcc->PZMCCPLN.AZ_min = rtcc->PZMCCPLN.AZ_max = -75.0*RAD;

		rtcc->PZSFPTAB.blocks[0].GMT_pc1 = OrbMech::HHMMSSToSS(99, 47, 51);
		rtcc->PZSFPTAB.blocks[0].lat_pc1 = -1.963*RAD;
		rtcc->PZSFPTAB.blocks[0].lng_pc1 = 178.047*RAD;
		rtcc->PZSFPTAB.blocks[0].h_pc1 = 1851.7*1852.0;

		rtcc->PZSFPTAB.blocks[0].GMT_pc2 = rtcc->PZSFPTAB.blocks[0].GMT_nd = OrbMech::HHMMSSToSS(99, 50, 39);
		rtcc->PZSFPTAB.blocks[0].lat_pc2 = rtcc->PZSFPTAB.blocks[0].lat_nd = 3.618*RAD;
		rtcc->PZSFPTAB.blocks[0].lng_pc2 = rtcc->PZSFPTAB.blocks[0].lng_nd = 157.6*RAD;
		rtcc->PZSFPTAB.blocks[0].h_pc2 = rtcc->PZSFPTAB.blocks[0].h_nd = 59.9*1852.0;

		rtcc->PZSFPTAB.blocks[0].GET_TLI = OrbMech::HHMMSSToSS(2, 47, 22);
		rtcc->PZSFPTAB.blocks[0].dpsi_loi = 6.7*RAD;
		rtcc->PZSFPTAB.blocks[0].gamma_loi = 2.0*RAD;
		rtcc->PZSFPTAB.blocks[0].T_lo = OrbMech::HHMMSSToSS(90, 57, 0);
		rtcc->PZSFPTAB.blocks[0].dt_lls = OrbMech::HHMMSSToSS(27, 02, 19);
		rtcc->PZSFPTAB.blocks[0].psi_lls = -75.0*RAD;
		rtcc->PZSFPTAB.blocks[0].lat_lls = -2.9425*RAD;
		rtcc->PZSFPTAB.blocks[0].lng_lls = -23.44333*RAD;
		rtcc->PZSFPTAB.blocks[0].rad_lls = OrbMech::R_Moon - 1.19*1852.0;
		rtcc->PZSFPTAB.blocks[0].dpsi_tei = 2.0*RAD;
		rtcc->PZSFPTAB.blocks[0].dv_tei = 3113.0*0.3048;
		rtcc->PZSFPTAB.blocks[0].T_te = OrbMech::HHMMSSToSS(70, 0, 0);
		rtcc->PZSFPTAB.blocks[0].incl_fr = 40.0*RAD;

		rtcc->GMGMED("P80,1,CSM,11,14,1969;");
		rtcc->GMGMED("P10,CSM,16:22:00;");
		rtcc->GMGMED("P12,CSM,16:22:00,72.0;");

		rtcc->RTCCPDIIgnitionTargets.v_IGG = 5551.1299*0.3048;
		rtcc->RTCCPDIIgnitionTargets.r_IGXG = -133067.52*0.3048;
		rtcc->RTCCPDIIgnitionTargets.r_IGZG = -1437887.4*0.3048;
		rtcc->RTCCPDIIgnitionTargets.K_X = 0.331;
		rtcc->RTCCPDIIgnitionTargets.K_Y = 5.8694e-7 / 0.3048;
		rtcc->RTCCPDIIgnitionTargets.K_V = 438.0;

		rtcc->RTCCDescentTargets.RBRFG = _V(-3.56205e3, 0.0, -1.370571e4)*0.3048;
		rtcc->RTCCDescentTargets.VBRFG = _V(-1.8690305e2, 0.0, -9.873819e1)*0.3048;
		rtcc->RTCCDescentTargets.ABRFG = _V(-4.502495e-1, 0.0, -9.5150975)*0.3048;
		rtcc->RTCCDescentTargets.JBRFGZ = -1.4742736e-2*0.3048;
		rtcc->RTCCDescentTargets.RARFG = _V(8.29275e1, 0.0, -2.01605e1)*0.3048;
		rtcc->RTCCDescentTargets.VARFG = _V(-3.19e-1, 0.0, 3.1233e-1)*0.3048;
		rtcc->RTCCDescentTargets.AARFG = _V(2.9982e-1, 0.0, -4.0165e-1)*0.3048;
		rtcc->RTCCDescentTargets.JARFGZ = 3.76954e-2*0.3048;
	}
	else if (mission == 13)
	{
		rtcc->AGCEpoch = 40586.767239;

		rtcc->BZLSDISP.lat[RTCC_LMPOS_BEST] = -3.6686*RAD;
		rtcc->BZLSDISP.lng[RTCC_LMPOS_BEST] = -17.4842*RAD;
		rtcc->MCSMLR = OrbMech::R_Moon -0.76*1852.0;
		rtcc->med_k18.psi_DS = -93.88 + 360.0;
		t_Land = OrbMech::HHMMSSToSS(103.0, 42.0, 02.0);
		rtcc->med_k17.DwellOrbits = 11;
		rtcc->med_k40.REVS1 = rtcc->PZMCCPLN.REVS1 = 1.96;
		rtcc->med_k40.eta_1 = 14.4;
		rtcc->PZREAP.RRBIAS = 1250.0;

		rtcc->PZMCCPLN.LOPC_M = 5;
		rtcc->PZMCCPLN.LOPC_N = 12;
		rtcc->PZMCCPLN.H_P_LPO1 = 59.0*1852.0;
		rtcc->PZMCCPLN.ETA1 = 14.4*RAD;

		rtcc->PZSFPTAB.blocks[0].GMT_pc1 = OrbMech::HHMMSSToSS(96, 53, 22);
		rtcc->PZSFPTAB.blocks[0].lat_pc1 = -0.058*RAD;
		rtcc->PZSFPTAB.blocks[0].lng_pc1 = 180.155*RAD;
		rtcc->PZSFPTAB.blocks[0].h_pc1 = 211.7*1852.0;
		rtcc->PZSFPTAB.blocks[0].GMT_pc2 = rtcc->PZSFPTAB.blocks[0].GMT_nd = OrbMech::HHMMSSToSS(96, 41, 39);
		rtcc->PZSFPTAB.blocks[0].lat_pc2 = rtcc->PZSFPTAB.blocks[0].lat_nd = -0.343*RAD;
		rtcc->PZSFPTAB.blocks[0].lng_pc2 = rtcc->PZSFPTAB.blocks[0].lng_nd = 177.996*RAD;
		rtcc->PZSFPTAB.blocks[0].h_pc2 = rtcc->PZSFPTAB.blocks[0].h_nd = 57.0*1852.0;
		rtcc->PZSFPTAB.blocks[0].GET_TLI = OrbMech::HHMMSSToSS(2, 35, 24);
		rtcc->PZSFPTAB.blocks[0].dpsi_loi = -0.7*RAD;
		rtcc->PZSFPTAB.blocks[0].gamma_loi = 2.0*RAD;
		rtcc->PZSFPTAB.blocks[0].T_lo = OrbMech::HHMMSSToSS(90, 0, 0);
		rtcc->PZSFPTAB.blocks[0].dt_lls = OrbMech::HHMMSSToSS(26, 13, 35);
		rtcc->PZSFPTAB.blocks[0].psi_lls = -93.88*RAD;
		rtcc->PZSFPTAB.blocks[0].lat_lls = -3.6686*RAD;
		rtcc->PZSFPTAB.blocks[0].lng_lls = -17.4842*RAD;
		rtcc->PZSFPTAB.blocks[0].rad_lls = OrbMech::R_Moon - 0.76*1852.0;
		rtcc->PZSFPTAB.blocks[0].dpsi_tei = 11.0*RAD;
		rtcc->PZSFPTAB.blocks[0].dv_tei = 3449.5*0.3048;
		rtcc->PZSFPTAB.blocks[0].T_te = OrbMech::HHMMSSToSS(72, 0, 0);
		rtcc->PZSFPTAB.blocks[0].incl_fr = 40.0*RAD;

		rtcc->GMGMED("P80,1,CSM,4,11,1970;");
		rtcc->GMGMED("P10,CSM,19:13:00;");
		rtcc->GMGMED("P12,CSM,19:13:00,72.0;");

		rtcc->RTCCPDIIgnitionTargets.v_IGG = 5546.4472*0.3048;
		rtcc->RTCCPDIIgnitionTargets.r_IGXG = -140345.73*0.3048;
		rtcc->RTCCPDIIgnitionTargets.r_IGZG = -1464980.0*0.3048;
		rtcc->RTCCPDIIgnitionTargets.K_X = 0.419;
		rtcc->RTCCPDIIgnitionTargets.K_Y = 9.05e-7 / 0.3048;
		rtcc->RTCCPDIIgnitionTargets.K_V = 470.0;

		rtcc->RTCCDescentTargets.RBRFG = _V(-3.56205e3, 0.0, -1.370571e4)*0.3048;
		rtcc->RTCCDescentTargets.VBRFG = _V(-1.8690305e2, 0.0, -9.873819e1)*0.3048;
		rtcc->RTCCDescentTargets.ABRFG = _V(-4.502495e-1, 0.0, -9.5150975)*0.3048;
		rtcc->RTCCDescentTargets.JBRFGZ = -1.4742736e-2*0.3048;
		rtcc->RTCCDescentTargets.RARFG = _V(8.29275e1, 0.0, -2.01605e1)*0.3048;
		rtcc->RTCCDescentTargets.VARFG = _V(-3.19e-1, 0.0, 3.1233e-1)*0.3048;
		rtcc->RTCCDescentTargets.AARFG = _V(2.9982e-1, 0.0, -4.0165e-1)*0.3048;
		rtcc->RTCCDescentTargets.JARFGZ = 3.769542e-2*0.3048;
	}
	else if (mission == 14)
	{
		rtcc->AGCEpoch = 40952.009432;

		rtcc->MCCCRF = 1733;
		rtcc->MCCCRF_DL = 01733;
		rtcc->MCCCXS = 306;
		rtcc->MCCLRF = 1731;
		rtcc->MCCLRF_DL = 01731;
		rtcc->MCCLXS = 3606;

		rtcc->BZLSDISP.lat[RTCC_LMPOS_BEST] = -3.672*RAD;
		rtcc->BZLSDISP.lng[RTCC_LMPOS_BEST] = -17.463*RAD;
		rtcc->MCSMLR = OrbMech::R_Moon -0.76*1852.0;
		rtcc->med_k18.psi_DS = -76.31 + 360.0;
		rtcc->med_k18.HPLOI1 = 59.6;
		t_Land = OrbMech::HHMMSSToSS(108.0, 53.0, 32.6);
		rtcc->med_k17.DwellOrbits = 11;
		rtcc->med_k40.REVS1 = rtcc->PZMCCPLN.REVS1 = 1.94;
		rtcc->med_k40.eta_1 = 21.6;
		rtcc->PZREAP.RRBIAS = 1250.0;
		rtcc->PZLTRT.DT_Ins_TPI = rtcc->PZLTRT.DT_Ins_TPI_NOM = 38.0*60.0;

		rtcc->PZMCCPLN.ETA1 = 21.6*RAD;

		rtcc->PZSFPTAB.blocks[0].GMT_pc1 = OrbMech::HHMMSSToSS(103, 2, 52);
		rtcc->PZSFPTAB.blocks[0].lat_pc1 = -0.058*RAD;
		rtcc->PZSFPTAB.blocks[0].lng_pc1 = 180.710*RAD;
		rtcc->PZSFPTAB.blocks[0].h_pc1 = 2030.9*1852.0;
		rtcc->PZSFPTAB.blocks[0].GMT_pc2 = rtcc->PZSFPTAB.blocks[0].GMT_nd = OrbMech::HHMMSSToSS(103, 3, 36);
		rtcc->PZSFPTAB.blocks[0].lat_pc2 = rtcc->PZSFPTAB.blocks[0].lat_nd = -4.572*RAD;
		rtcc->PZSFPTAB.blocks[0].lng_pc2 = rtcc->PZSFPTAB.blocks[0].lng_nd = 161.158*RAD;
		rtcc->PZSFPTAB.blocks[0].h_pc2 = rtcc->PZSFPTAB.blocks[0].h_nd = 57.0*1852.0;
		rtcc->PZSFPTAB.blocks[0].GET_TLI = OrbMech::HHMMSSToSS(2, 30, 38);
		rtcc->PZSFPTAB.blocks[0].dpsi_loi = -11.2*RAD;
		rtcc->PZSFPTAB.blocks[0].gamma_loi = 0.0*RAD;
		rtcc->PZSFPTAB.blocks[0].T_lo = OrbMech::HHMMSSToSS(66, 37, 36);
		rtcc->PZSFPTAB.blocks[0].dt_lls = OrbMech::HHMMSSToSS(26, 13, 47);
		rtcc->PZSFPTAB.blocks[0].psi_lls = -76.31*RAD;
		rtcc->PZSFPTAB.blocks[0].lat_lls = -3.672*RAD;
		rtcc->PZSFPTAB.blocks[0].lng_lls = -17.463*RAD;
		rtcc->PZSFPTAB.blocks[0].rad_lls = OrbMech::R_Moon - 0.76*1852.0;
		rtcc->PZSFPTAB.blocks[0].dpsi_tei = 11.0*RAD;
		rtcc->PZSFPTAB.blocks[0].dv_tei = 3449.5*0.3048;
		rtcc->PZSFPTAB.blocks[0].T_te = OrbMech::HHMMSSToSS(67, 10, 59);
		rtcc->PZSFPTAB.blocks[0].incl_fr = 40.0*RAD;

		rtcc->GMGMED("P80,1,CSM,1,31,1971;");
		rtcc->GMGMED("P10,CSM,20:23:00;");
		rtcc->GMGMED("P12,CSM,20:23:00,72.0;");

		rtcc->RTCCPDIIgnitionTargets.v_IGG = 5546.4472*0.3048;
		rtcc->RTCCPDIIgnitionTargets.r_IGXG = -140345.73*0.3048;
		rtcc->RTCCPDIIgnitionTargets.r_IGZG = -1464980.0*0.3048;
		rtcc->RTCCPDIIgnitionTargets.K_X = 0.419;
		rtcc->RTCCPDIIgnitionTargets.K_Y = 9.05e-7 / 0.3048;
		rtcc->RTCCPDIIgnitionTargets.K_V = 470.0;

		rtcc->RTCCDescentTargets.RBRFG = _V(-1773.725, 0.0, -14488.027)*0.3048;
		rtcc->RTCCDescentTargets.VBRFG = _V(-168.10646, 0.0, -77.614367)*0.3048;
		rtcc->RTCCDescentTargets.ABRFG = _V(-0.64723603, 0.0, -8.4143819)*0.3048;
		rtcc->RTCCDescentTargets.JBRFGZ = 8.2572949e-3*0.3048;
		rtcc->RTCCDescentTargets.RARFG = _V(94.9191, 0.0, -15.7208)*0.3048;
		rtcc->RTCCDescentTargets.VARFG = _V(2.08358, 0.0, 0.83031879)*0.3048;
		rtcc->RTCCDescentTargets.AARFG = _V(0.540285, 0.0, -0.235423)*0.3048;
		rtcc->RTCCDescentTargets.JARFGZ = 0.045092421*0.3048;
	}
	else if (mission == 15)
	{
		rtcc->BZLSDISP.lat[RTCC_LMPOS_BEST] = 26.0739*RAD;
		rtcc->BZLSDISP.lng[RTCC_LMPOS_BEST] = 3.6539*RAD;
		rtcc->MCSMLR = OrbMech::R_Moon -1.92*1852.0;
		rtcc->med_k18.psi_DS = -91.0 + 360.0;
		t_Land = OrbMech::HHMMSSToSS(104.0, 40.0, 57.0);
		rtcc->med_k17.DescentFlightArc = 16.0*RAD;
		rtcc->med_k17.DwellOrbits = 11;
		rtcc->med_k40.REVS1 = rtcc->PZMCCPLN.REVS1 = 1.9;
		rtcc->med_k40.eta_1 = 36.0;
		rtcc->PZREAP.IRMAX = 40.0;
		rtcc->PZREAP.RRBIAS = 1190.0;
		rtcc->PZLTRT.DT_Ins_TPI = rtcc->PZLTRT.DT_Ins_TPI_NOM = 45.0*60.0;

		rtcc->PZMCCPLN.ETA1 = 36.0*RAD;

		rtcc->PZSFPTAB.blocks[0].GMT_pc1 = rtcc->PZSFPTAB.blocks[0].GMT_pc2 = rtcc->PZSFPTAB.blocks[0].GMT_nd = OrbMech::HHMMSSToSS(92, 9, 1);
		rtcc->PZSFPTAB.blocks[0].lat_pc1 = rtcc->PZSFPTAB.blocks[0].lat_pc2 = rtcc->PZSFPTAB.blocks[0].lat_nd = -16.905*RAD;
		rtcc->PZSFPTAB.blocks[0].lng_pc1 = rtcc->PZSFPTAB.blocks[0].lng_pc2 = rtcc->PZSFPTAB.blocks[0].lng_nd = 175.625*RAD;
		rtcc->PZSFPTAB.blocks[0].h_pc1 = rtcc->PZSFPTAB.blocks[0].h_pc2 = rtcc->PZSFPTAB.blocks[0].h_nd = 67.88*1852.0;
		rtcc->PZSFPTAB.blocks[0].GET_TLI = OrbMech::HHMMSSToSS(2, 49, 58);
		rtcc->PZSFPTAB.blocks[0].dpsi_loi = -7.9*RAD;
		rtcc->PZSFPTAB.blocks[0].gamma_loi = 0.0;
		rtcc->PZSFPTAB.blocks[0].T_lo = OrbMech::HHMMSSToSS(145, 14, 51);
		rtcc->PZSFPTAB.blocks[0].dt_lls = OrbMech::HHMMSSToSS(26, 7, 40);
		rtcc->PZSFPTAB.blocks[0].psi_lls = -91.0*RAD;
		rtcc->PZSFPTAB.blocks[0].lat_lls = 26.0739*RAD;
		rtcc->PZSFPTAB.blocks[0].lng_lls = 3.6539*RAD;
		rtcc->PZSFPTAB.blocks[0].rad_lls = OrbMech::R_Moon - 1.92*1852.0;
		rtcc->PZSFPTAB.blocks[0].dpsi_tei = 5.6*RAD;
		rtcc->PZSFPTAB.blocks[0].dv_tei = 3046.7*0.3048;
		rtcc->PZSFPTAB.blocks[0].T_te = OrbMech::HHMMSSToSS(71, 13, 20);
		rtcc->PZSFPTAB.blocks[0].incl_fr = 75.0*RAD;

		rtcc->GMGMED("P80,1,CSM,7,26,1971;");
		rtcc->GMGMED("P10,CSM,13:34:00;");
		rtcc->GMGMED("P12,CSM,13:34:00,80.088;");

		rtcc->RTCCPDIIgnitionTargets.v_IGG = 5548.14101*0.3048;
		rtcc->RTCCPDIIgnitionTargets.r_IGXG = -162539.6686*0.3048;
		rtcc->RTCCPDIIgnitionTargets.r_IGZG = -1547120.997*0.3048;
		rtcc->RTCCPDIIgnitionTargets.K_X = 0.334;
		rtcc->RTCCPDIIgnitionTargets.K_Y = 2.207e-7 / 0.3048;
		rtcc->RTCCPDIIgnitionTargets.K_V = 498.0;
	}
	else if (mission == 16)
	{
		rtcc->BZLSDISP.lat[RTCC_LMPOS_BEST] = -9.00028*RAD;
		rtcc->BZLSDISP.lng[RTCC_LMPOS_BEST] = 15.51639*RAD;
		rtcc->MCSMLR = OrbMech::R_Moon -0.1405*1852.0;
		rtcc->med_k18.psi_DS = -90.0 + 360.0;
		t_Land = OrbMech::HHMMSSToSS(98.0, 46.0, 42.4);
		rtcc->med_k17.DescentFlightArc = 16.0*RAD;
		rtcc->med_k17.DwellOrbits = rtcc->med_k40.REVS2 = rtcc->PZMCCPLN.REVS2 = 10;
		rtcc->med_k17.DescIgnHeight = 52500.0*0.3048;
		rtcc->med_k40.DW = -16.0;
		rtcc->med_k40.REVS1 = rtcc->PZMCCPLN.REVS1 = 1.87;
		rtcc->med_k40.eta_1 = 46.8;
		rtcc->PZREAP.IRMAX = 80.0;
		rtcc->PZREAP.RRBIAS = 1190.0;
		rtcc->PZLTRT.DT_Ins_TPI = rtcc->PZLTRT.DT_Ins_TPI_NOM = 47.0*60.0;

		rtcc->PZMCCPLN.SITEROT = -16.0*RAD;
		rtcc->PZMCCPLN.ETA1 = 46.8*RAD;

		rtcc->PZSFPTAB.blocks[0].GMT_pc1 = rtcc->PZSFPTAB.blocks[0].GMT_pc2 = rtcc->PZSFPTAB.blocks[0].GMT_nd = OrbMech::HHMMSSToSS(92, 26, 13);
		rtcc->PZSFPTAB.blocks[0].lat_pc1 = rtcc->PZSFPTAB.blocks[0].lat_pc2 = rtcc->PZSFPTAB.blocks[0].lat_nd = 5.529*RAD;
		rtcc->PZSFPTAB.blocks[0].lng_pc1 = rtcc->PZSFPTAB.blocks[0].lng_pc2 = rtcc->PZSFPTAB.blocks[0].lng_nd = 168.758*RAD;
		rtcc->PZSFPTAB.blocks[0].h_pc1 = rtcc->PZSFPTAB.blocks[0].h_pc2 = rtcc->PZSFPTAB.blocks[0].h_nd = 72.76*1852.0;
		rtcc->PZSFPTAB.blocks[0].GET_TLI = OrbMech::HHMMSSToSS(2, 33, 15);
		rtcc->PZSFPTAB.blocks[0].dpsi_loi = -0.9*RAD;
		rtcc->PZSFPTAB.blocks[0].gamma_loi = 0.0;
		rtcc->PZSFPTAB.blocks[0].T_lo = OrbMech::HHMMSSToSS(147, 52, 0);
		rtcc->PZSFPTAB.blocks[0].dt_lls = OrbMech::HHMMSSToSS(24, 15, 0);
		rtcc->PZSFPTAB.blocks[0].psi_lls = -90.0*RAD;
		rtcc->PZSFPTAB.blocks[0].lat_lls = -9.00028*RAD;
		rtcc->PZSFPTAB.blocks[0].lng_lls = 15.51639*RAD;
		rtcc->PZSFPTAB.blocks[0].rad_lls = OrbMech::R_Moon - 0.1405*1852.0;
		rtcc->PZSFPTAB.blocks[0].dpsi_tei = -2.1*RAD;
		rtcc->PZSFPTAB.blocks[0].dv_tei = 3212.2*0.3048;
		rtcc->PZSFPTAB.blocks[0].T_te = OrbMech::HHMMSSToSS(68, 0, 0);
		rtcc->PZSFPTAB.blocks[0].incl_fr = 40.0*RAD;

		rtcc->GMGMED("P80,1,CSM,4,16,1972;");
		rtcc->GMGMED("P10,CSM,17:54:00;");
		rtcc->GMGMED("P12,CSM,17:54:00,72.0;");

		rtcc->RTCCPDIIgnitionTargets.v_IGG = 5543.4605*0.3048;
		rtcc->RTCCPDIIgnitionTargets.r_IGXG = -159548.72*0.3048;
		rtcc->RTCCPDIIgnitionTargets.r_IGZG = -1547623.3*0.3048;
		rtcc->RTCCPDIIgnitionTargets.K_X = 0.334;
		rtcc->RTCCPDIIgnitionTargets.K_Y = 2.207e-7 / 0.3048;
		rtcc->RTCCPDIIgnitionTargets.K_V = 498.0;
	}
	else if (mission == 17)
	{
		rtcc->BZLSDISP.lat[RTCC_LMPOS_BEST] = 20.164*RAD;
		rtcc->BZLSDISP.lng[RTCC_LMPOS_BEST] = 30.750*RAD;
		rtcc->MCSMLR = OrbMech::R_Moon -1.95*1852.0;
		rtcc->med_k18.psi_DS = -90.0 + 360.0;
		rtcc->med_k18.HPLOI1 = 52.8;
		t_Land = OrbMech::HHMMSSToSS(113.0, 01.0, 38.4);
		rtcc->med_k17.DescentFlightArc = -10.0*RAD;
		rtcc->med_k40.DW = 20.0;
		rtcc->med_k40.HP_LLS = 13.17;
		rtcc->med_k40.REVS2 = rtcc->PZMCCPLN.REVS2 = 10;
		rtcc->med_k17.DescIgnHeight = 40000.0*0.3048;
		rtcc->PZREAP.IRMAX = 80.0;
		rtcc->PZREAP.RRBIAS = 1190.0;
		rtcc->PZLTRT.DT_Ins_TPI = rtcc->PZLTRT.DT_Ins_TPI_NOM = 47.0*60.0;

		rtcc->PZMCCPLN.SITEROT = 20.0*RAD;
		rtcc->PZMCCPLN.H_P_LPO1 = 51.3*1852.0;
		rtcc->PZMCCPLN.H_P_LPO2 = 13.17*1852.0;

		rtcc->PZSFPTAB.blocks[0].GMT_pc1 = rtcc->PZSFPTAB.blocks[0].GMT_pc2 = rtcc->PZSFPTAB.blocks[0].GMT_nd = OrbMech::HHMMSSToSS(91, 52, 23);
		rtcc->PZSFPTAB.blocks[0].lat_pc1 = rtcc->PZSFPTAB.blocks[0].lat_pc2 = rtcc->PZSFPTAB.blocks[0].lat_nd = -9.75*RAD;
		rtcc->PZSFPTAB.blocks[0].lng_pc1 = rtcc->PZSFPTAB.blocks[0].lng_pc2 = rtcc->PZSFPTAB.blocks[0].lng_nd = 161.779*RAD;
		rtcc->PZSFPTAB.blocks[0].h_pc1 = rtcc->PZSFPTAB.blocks[0].h_pc2 = rtcc->PZSFPTAB.blocks[0].h_nd = 51.3*1852.0;
		rtcc->PZSFPTAB.blocks[0].GET_TLI = OrbMech::HHMMSSToSS(3, 20, 58);
		rtcc->PZSFPTAB.blocks[0].dpsi_loi = 11.3*RAD;
		rtcc->PZSFPTAB.blocks[0].gamma_loi = 0.0;
		rtcc->PZSFPTAB.blocks[0].T_lo = OrbMech::HHMMSSToSS(147, 41, 0);
		rtcc->PZSFPTAB.blocks[0].dt_lls = OrbMech::HHMMSSToSS(23, 59, 39);
		rtcc->PZSFPTAB.blocks[0].psi_lls = -90.0*RAD;
		rtcc->PZSFPTAB.blocks[0].lat_lls = 20.164*RAD;
		rtcc->PZSFPTAB.blocks[0].lng_lls = 30.750*RAD;
		rtcc->PZSFPTAB.blocks[0].rad_lls = OrbMech::R_Moon - 1.95*1852.0;
		rtcc->PZSFPTAB.blocks[0].dpsi_tei = 0.9*RAD;
		rtcc->PZSFPTAB.blocks[0].dv_tei = 3045.7*0.3048;
		rtcc->PZSFPTAB.blocks[0].T_te = OrbMech::HHMMSSToSS(67, 37, 2);
		rtcc->PZSFPTAB.blocks[0].incl_fr = 75.0*RAD;

		rtcc->GMGMED("P80,1,CSM,12,7,1972;");
		rtcc->GMGMED("P10,CSM,02:53:00;");
		rtcc->GMGMED("P12,CSM,02:53:00,72.0;");

		rtcc->RTCCPDIIgnitionTargets.v_IGG = 5542.8976*0.3048;
		rtcc->RTCCPDIIgnitionTargets.r_IGXG = -156145.03*0.3048;
		rtcc->RTCCPDIIgnitionTargets.r_IGZG = -1541941.8*0.3048;
		rtcc->RTCCPDIIgnitionTargets.K_X = 0.334;
		rtcc->RTCCPDIIgnitionTargets.K_Y = 2.207e-7 / 0.3048;
		rtcc->RTCCPDIIgnitionTargets.K_V = 498.0;
	}

	if (mission == 15 || mission == 16 || mission == 17)
	{
		rtcc->AGCEpoch = 41317.251625;
		rtcc->MCCCRF = 1733;
		rtcc->MCCCRF_DL = 01733;
		rtcc->MCCCXS = 306;
		rtcc->MCCLRF = 1731;
		rtcc->MCCLRF_DL = 01731;
		rtcc->MCCLXS = 3606;
		rtcc->RTCCDescentTargets.RBRFG = _V(-3.1183588e3, 0.0, -1.1741441e4)*0.3048;
		rtcc->RTCCDescentTargets.VBRFG = _V(-1.9646916e2, 0.0, -1.6675997e2)*0.3048;
		rtcc->RTCCDescentTargets.ABRFG = _V(-7.1824814e-1, 0.0, -8.3024503)*0.3048;
		rtcc->RTCCDescentTargets.JBRFGZ = -1.5123659e-2*0.3048;
		rtcc->RTCCDescentTargets.RARFG = _V(1.585e2, 0.0, -2.73554e1)*0.3048;
		rtcc->RTCCDescentTargets.VARFG = _V(-3.53476, 0.0, 2.49505e-2)*0.3048;
		rtcc->RTCCDescentTargets.AARFG = _V(7.71783e-2, 0.0, -5.89627e-1)*0.3048;
		rtcc->RTCCDescentTargets.JARFGZ = 4.31736e-2*0.3048;
	}

	rtcc->med_k18.psi_MX = rtcc->med_k18.psi_DS + 1.0;
	rtcc->med_k18.psi_MN = rtcc->med_k18.psi_DS - 1.0;
}

int AR_GCore::MPTTrajectoryUpdate()
{
	VESSEL *ves;
	if (rtcc->med_m50.Table == RTCC_MPT_CSM)
	{
		ves = pCSM;
	}
	else
	{
		ves = pLM;
	}

	if (ves == NULL) return 1;

	bool landed = ves->GroundContact();
	EphemerisData sv2;

	if (landed)
	{
		double lat, lng, rad;
		ves->GetEquPos(lng, lat, rad);

		rtcc->BZLSDISP.lat[RTCC_LMPOS_BEST] = lat;
		rtcc->BZLSDISP.lng[RTCC_LMPOS_BEST] = lng;
		rtcc->MCSMLR = rad;
	}
	else
	{
		MPTSV sv = rtcc->StateVectorCalc(ves);

		sv2.R = sv.R;
		sv2.V = sv.V;
		sv2.GMT = OrbMech::GETfromMJD(sv.MJD, rtcc->GetGMTBase());
		if (sv.gravref == oapiGetObjectByName("Earth"))
		{
			sv2.RBI = BODY_EARTH;
		}
		else
		{
			sv2.RBI = BODY_MOON;
		}
	}
	rtcc->PMSVCT(4, rtcc->med_m50.Table, &sv2, landed);

	return 0;
}

void AR_GCore::MPTMassUpdate()
{
	//Mass Update
	VESSEL *vessel = NULL;
	int vesseltype = 0;
	std::string cfg;
	double cmmass, lmmass, sivb_mass, lm_ascent_mass;
	cmmass = lmmass = sivb_mass = lm_ascent_mass = 0.0;

	if (rtcc->med_m50.Table == RTCC_MPT_LM)
	{
		vessel = pLM;
	}
	else
	{
		vessel = pCSM;
	}

	if (vessel == NULL) return;

	char Buffer[100];

	sprintf_s(Buffer, vessel->GetClassNameA());

	if (!stricmp(Buffer, "ProjectApollo\\Saturn5") ||
		!stricmp(Buffer, "ProjectApollo/Saturn5") ||
		!stricmp(Buffer, "ProjectApollo\\Saturn1b") ||
		!stricmp(Buffer, "ProjectApollo/Saturn1b"))
	{
		vesseltype = 0;
	}
	else if (!stricmp(vessel->GetClassName(), "ProjectApollo\\LEM") ||
		!stricmp(vessel->GetClassName(), "ProjectApollo/LEM"))
	{
		vesseltype = 1;
	}
	else if (!stricmp(vessel->GetClassName(), "ProjectApollo\\sat5stg3") ||
		!stricmp(vessel->GetClassName(), "ProjectApollo/sat5stg3") ||
		!stricmp(vessel->GetClassName(), "ProjectApollo\\nsat1stg2") ||
		!stricmp(vessel->GetClassName(), "ProjectApollo/nsat1stg2"))
	{
		vesseltype = 2;
	}
	else
	{
		vesseltype = 3;
	}

	if (vesseltype == 3) return;

	if (vesseltype == 0)
	{
		Saturn *sat = (Saturn *)vessel;
		if (sat->GetStage() < CSM_LEM_STAGE)
		{
			cmmass = sat->SM_FuelMass + sat->SM_EmptyMass + sat->CM_FuelMass + sat->CM_EmptyMass + 4.0*152.5 + 2.0*55.5;
			if (sat->SIVBPayload == PAYLOAD_LEM)
			{
				cfg = "CSL";
				lmmass = sat->LMDescentFuelMassKg + sat->LMAscentFuelMassKg + sat->LMDescentEmptyMassKg + sat->LMAscentEmptyMassKg + 2.0*133.084001;
				lm_ascent_mass = sat->LMAscentFuelMassKg + sat->LMAscentEmptyMassKg + 2.0*133.084001;

			}
			else
			{
				cfg = "CS";
			}
			sivb_mass = vessel->GetMass() - cmmass - lmmass;
		}
		else
		{
			cmmass = vessel->GetMass();
			if (lmmass = rtcc->GetDockedVesselMass(vessel))
			{
				DOCKHANDLE dock;
				OBJHANDLE hLM;
				VESSEL *lm;
				dock = vessel->GetDockHandle(0);
				hLM = vessel->GetDockStatus(dock);
				lm = oapiGetVesselInterface(hLM);

				LEM *lem = (LEM *)lm;
				lm_ascent_mass = lem->GetAscentStageMass();

				//TBD: Make this better
				if (lem->GetStage() < 2)
				{
					cfg = "CL";
				}
				else
				{
					cfg = "CA";
				}

				
			}
			else
			{
				cfg = "C";
			}
		}
	}
	else if (vesseltype == 1)
	{
		LEM *lem = (LEM *)vessel;

		lmmass = vessel->GetMass();
		lm_ascent_mass = lem->GetAscentStageMass();

		if (lem->GetStage() < 2)
		{
			if (cmmass = rtcc->GetDockedVesselMass(vessel))
			{
				cfg = "CL";
			}
			else
			{
				cfg = "L";
			}
		}
		else
		{
			if (cmmass = rtcc->GetDockedVesselMass(vessel))
			{
				cfg = "CA";
			}
			else
			{
				cfg = "A";
			}
		}
	}
	else
	{
		cfg = "S";
		sivb_mass = vessel->GetMass();
	}

	rtcc->med_m55.ConfigCode = cfg;

	rtcc->med_m50.CSMWT = cmmass;
	rtcc->med_m50.LMWT = lmmass;
	rtcc->med_m50.LMASCWT = lm_ascent_mass;
	rtcc->med_m50.SIVBWT = sivb_mass;
}

ARCore::ARCore(VESSEL* v, AR_GCore* gcin)
{
	GC = gcin;

	SPQMode = 0;
	CSItime = 0.0;
	CDHtime = 0.0;
	SPQTIG = 0.0;
	CDHtimemode = 0;
	this->vessel = v;
	t_TPI = 0.0;

	SPQDeltaV = _V(0, 0, 0);
	target = NULL;
	//screen = 0;
	targetnumber = -1;
	AGCEphemTEphemZero = 40038.0;
	REFSMMATTime = 0.0;
	REFSMMATopt = 4;
	REFSMMATcur = 4;
	manpadopt = 0;
	vesseltype = 0;
	lemdescentstage = true;

	if (strcmp(v->GetName(), "Spider") == 0)
	{
		vesseltype = 2;
	}
	else if (strcmp(v->GetName(), "Snoopy") == 0)
	{
		vesseltype = 2;
	}
	else if (strcmp(v->GetName(), "AS-506") == 0 || strcmp(v->GetName(), "Columbia") == 0)
	{
		AGCEphemTEphemZero = 40403.0;
	}
	else if (strcmp(v->GetName(), "Eagle") == 0)
	{
		vesseltype = 2;
		AGCEphemTEphemZero = 40403.0;
	}
	else if (strcmp(v->GetName(), "Yankee-Clipper") == 0)
	{
		AGCEphemTEphemZero = 40403.0;
	}
	else if (strcmp(v->GetName(), "Intrepid") == 0)
	{
		vesseltype = 2;
		AGCEphemTEphemZero = 40403.0;
	}
	else if (strcmp(v->GetName(), "Odyssey") == 0)
	{
		AGCEphemTEphemZero = 40403.0;
	}
	else if (strcmp(v->GetName(), "Aquarius") == 0)
	{
		vesseltype = 2;
		AGCEphemTEphemZero = 40403.0;
	}
	else if (strcmp(v->GetName(), "Kitty-Hawk") == 0)
	{
		AGCEphemTEphemZero = 40768.0;
	}
	else if (strcmp(v->GetName(), "Antares") == 0)
	{
		vesseltype = 2;
		AGCEphemTEphemZero = 40768.0;
	}
	else if (strcmp(v->GetName(), "Endeavour") == 0)
	{
		AGCEphemTEphemZero = 41133.0;
	}
	else if (strcmp(v->GetName(), "Falcon") == 0)
	{
		vesseltype = 2;
		AGCEphemTEphemZero = 41133.0;
	}
	else if (strcmp(v->GetName(), "Casper") == 0)
	{
		AGCEphemTEphemZero = 41133.0;
	}
	else if (strcmp(v->GetName(), "Orion") == 0)
	{
		vesseltype = 2;
		AGCEphemTEphemZero = 41133.0;
	}
	else if (strcmp(v->GetName(), "America") == 0)
	{
		AGCEphemTEphemZero = 41133.0;
	}
	else if (strcmp(v->GetName(), "Challenger") == 0)
	{
		vesseltype = 2;
		AGCEphemTEphemZero = 41133.0;
	}

	if (vessel->DockingStatus(0) == 1)
	{
		vesseltype++;
	}

	REFSMMATHeadsUp = true;

	GMPManeuverCode = 0;
	GMPManeuverPoint = 0;
	GMPManeuverType = 0;
	OrbAdjAltRef = true;
	OrbAdjDVX = _V(0, 0, 0);
	SPSGET = 0.0;
	GPM_TIG = 0.0;
	GMPApogeeHeight = 0;
	GMPPerigeeHeight = 0;
	GMPWedgeAngle = 0.0;
	GMPManeuverLongitude = 0.0;
	GMPManeuverHeight = 0.0;
	GMPHeightChange = 0.0;
	GMPNodeShiftAngle = 0.0;
	GMPDeltaVInput = 0.0;
	GMPPitch = 0.0;
	GMPYaw = 0.0;
	GMPRevs = 0;
	GMPApseLineRotAngle = 0.0;
	GMPResults.A = 0.0;
	GMPResults.Del_G = 0.0;
	GMPResults.E = 0.0;
	GMPResults.GET_A = 0.0;
	GMPResults.GET_P = 0.0;
	GMPResults.HA = 0.0;
	GMPResults.HP = 0.0;
	GMPResults.H_Man = 0.0;
	GMPResults.I = 0.0;
	GMPResults.lat_A = 0.0;
	GMPResults.lat_Man = 0.0;
	GMPResults.lat_P = 0.0;
	GMPResults.long_A = 0.0;
	GMPResults.long_Man = 0.0;
	GMPResults.long_P = 0.0;
	GMPResults.Node_Ang = 0.0;
	GMPResults.Pitch_Man = 0.0;
	GMPResults.Yaw_Man = 0.0;

	g_Data.uplinkBufferSimt = 0;
	g_Data.connStatus = 0;
	g_Data.uplinkState = 0;
	if (vesseltype < 2)
	{
		g_Data.uplinkLEM = 0;
	}
	else
	{
		g_Data.uplinkLEM = 1;

		if (!stricmp(vessel->GetClassName(), "ProjectApollo\\LEM") ||
			!stricmp(vessel->GetClassName(), "ProjectApollo/LEM")) {
			LEM *lem = (LEM *)vessel;
			if (lem->GetStage() < 2)
			{
				lemdescentstage = true;
			}
			else
			{
				lemdescentstage = false;
			}
		}
	}
	for (int i = 0; i < 24; i++)
	{
		g_Data.emem[i] = 0;
	}
	ZeroMemory(&g_Data.burnData, sizeof(IMFD_BURN_DATA));
	g_Data.isRequesting = false;
	g_Data.progVessel = (Saturn *)vessel;
	int iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
	if (iResult != NO_ERROR) {
		sprintf(debugWinsock, "ERROR AT WSAStartup()");
	}
	else {
		sprintf(debugWinsock, "DISCONNECTED");
	}
	P30TIG = 0;
	dV_LVLH = _V(0.0, 0.0, 0.0);
	for (int i = 0;i < 016;i++)
	{
		RetrofireEXDVOctals[i] = 0;
	}

	EntryTIG = 0.0;
	EntryLat = 0.0;
	EntryLng = 0.0;
	EntryTIGcor = 0.0;
	EntryLatcor = 0.0;
	EntryLngcor = 0.0;
	EntryAng = 0.0;
	EntryAngcor = 0.0;
	Entry_DV = _V(0.0, 0.0, 0.0);
	entrycritical = 1;
	RTEReentryTime = 0.0;
	entrynominal = 1;
	entryrange = 0.0;
	EntryRTGO = 0.0;
	FlybyPeriAlt = 0.0;
	EntryDesiredInclination = 0.0;
	RTECalcMode = 1;
	RTEReturnInclination = 0.0;
	RTETradeoffMode = 0;

	SVSlot = true; //true = CSM; false = Other
	SVDesiredGET = 0.0;
	manpad.Trun = 0.0;
	manpad.Shaft = 0.0;
	manpad.Star = 0;
	manpad.BSSStar = 0;
	manpad.SPA = 0.0;
	manpad.SXP = 0.0;
	manpad.Att = _V(0, 0, 0);
	manpad.GDCangles = _V(0, 0, 0);
	//GDCset = 0;
	manpad.SetStars[0] = 0;
	manpad.SetStars[1] = 0;
	HeadsUp = false;
	manpad.HA = 0.0;
	manpad.HP = 0.0;
	manpad.Weight = 0.0;
	manpad.burntime = 0.0;
	manpad.Vt = 0.0;
	manpad.Vc = 0.0;
	manpad.pTrim = 0.0;
	manpad.yTrim = 0.0;
	manpad.LMWeight = 0.0;
	lmmanpad.Att = _V(0, 0, 0);
	lmmanpad.BSSStar = 0;
	lmmanpad.burntime = 0.0;
	lmmanpad.CSMWeight = 0.0;
	lmmanpad.dV = _V(0, 0, 0);
	lmmanpad.dVR = 0.0;
	lmmanpad.dV_AGS = _V(0, 0, 0);
	lmmanpad.GETI = 0.0;
	lmmanpad.HA = 0.0;
	lmmanpad.HP = 0.0;
	lmmanpad.LMWeight = 0.0;
	lmmanpad.SPA = 0.0;
	lmmanpad.SXP = 0.0;
	sprintf(lmmanpad.remarks, "");
	entrypadopt = 0;
	manpadenginetype = RTCC_ENGINETYPE_CSMSPS;
	deorbitenginetype = RTCC_ENGINETYPE_CSMSPS;
	TPIPAD_AZ = 0.0;
	TPIPAD_dH = 0.0;
	TPIPAD_dV_LOS = _V(0.0, 0.0, 0.0);
	TPIPAD_ELmin5 = 0.0;
	TPIPAD_R = 0.0;
	TPIPAD_Rdot = 0.0;
	TPIPAD_ddH = 0.0;
	TPIPAD_BT = _V(0.0, 0.0, 0.0);
	sxtstardtime = 0.0;
	EntryRRT = 0.0;
	EntryRET05G = 0.0;

	mapupdate.LOSGET = 0.0;
	mapupdate.AOSGET = 0.0;
	mapupdate.SSGET = 0.0;
	mapupdate.SRGET = 0.0;
	mapupdate.PMGET = 0.0;
	mappage = 1;
	mapgs = 0;
	mapUpdateGET = 0.0;
	GSAOSGET = 0.0;
	GSLOSGET = 0.0;
	inhibUplLOS = false;
	PADSolGood = true;
	svtarget = NULL;
	svtargetnumber = -1;
	TLCCSolGood = true;

	for (int i = 0;i < 021;i++)
	{
		SVOctals[i] = 0;
	}
	RLSUplink = _V(0, 0, 0);
	for (int i = 0;i < 010;i++)
	{
		RLSOctals[i] = 0;
	}
	for (int i = 0;i < 5;i++)
	{
		TLANDOctals[i] = 0;
	}

	entrylongmanual = true;
	landingzone = 0;
	entryprecision = -1;

	TLImaneuver = 0;
	TLCCmaneuver = 1;
	GC->rtcc->PZMCCPLN.MidcourseGET = 0.0;
	TLCC_dV_LVLH = _V(0.0, 0.0, 0.0);
	TLCC_TIG = 0.0;
	
	tlipad.TB6P = 0.0;
	tlipad.BurnTime = 0.0;
	tlipad.dVC = 0.0;
	tlipad.VI = 0.0;
	tlipad.SepATT = _V(0.0, 0.0, 0.0);
	tlipad.IgnATT = _V(0.0, 0.0, 0.0);
	tlipad.ExtATT = _V(0.0, 0.0, 0.0);
	R_TLI = _V(0, 0, 0);
	V_TLI = _V(0, 0, 0);

	pdipad.Att = _V(0, 0, 0);
	pdipad.CR = 0.0;
	pdipad.DEDA231 = 0.0;
	pdipad.GETI = 0.0;
	pdipad.t_go = 0.0;

	subThreadMode = 0;
	subThreadStatus = 0;

	LmkLat = 0;
	LmkLng = 0;
	LmkTime = 0;
	landmarkpad.T1[0] = 0;
	landmarkpad.T2[0] = 0;
	landmarkpad.CRDist[0] = 0;
	landmarkpad.Alt[0] = 0;
	landmarkpad.Lat[0] = 0;
	landmarkpad.Lng05[0] = 0;

	VECoption = 0;
	VECdirection = 0;
	VECbody = NULL;
	VECangles = _V(0, 0, 0);

	DOI_dV_LVLH = _V(0, 0, 0);

	AGSKFactor = 90.0*3600.0;

	DKI_Profile = 0;
	DKI_TPI_Mode = 0;
	DKI_Radial_DV = false;
	DKI_TIG = 0.0;
	DKI_DV = _V(0, 0, 0);
	DKI_Maneuver_Line = true;
	DKI_dt_TPI_sunrise = 16.0*60.0;
	DKI_N_HC = 1;
	DKI_N_PB = 1;
	DKI_dt_PBH = DKI_dt_BHAM = DKI_dt_HAMH = 3600.0;
	dkiresult.DV_Phasing = _V(0, 0, 0);
	dkiresult.t_CDH = 0.0;
	dkiresult.dv_CSI = 0.0;
	dkiresult.t_CSI = 0.0;
	dkiresult.t_TPI = 0.0;
	dkiresult.DV_CDH = _V(0, 0, 0);
	dkiresult.t_Boost = 0.0;
	dkiresult.dv_Boost = 0.0;
	dkiresult.t_HAM = 0.0;

	PDAPEngine = 0;
	PDAPTwoSegment = false;
	PDAPABTCOF[0] = 0.0;
	PDAPABTCOF[1] = 0.0;
	PDAPABTCOF[2] = 0.0;
	PDAPABTCOF[3] = 0.0;
	PDAPABTCOF[4] = 0.0;
	PDAPABTCOF[5] = 0.0;
	PDAPABTCOF[6] = 0.0;
	PDAPABTCOF[7] = 0.0;
	DEDA224 = 0.0;
	DEDA225 = 0.0;
	DEDA226 = 0.0;
	DEDA227 = 0;
	PDAP_J1 = 0.0;
	PDAP_J2 = 0.0;
	PDAP_K1 = 0.0;
	PDAP_K2 = 0.0;
	PDAP_Theta_LIM = 0.0;
	PDAP_R_amin = 0.0;

	if (GC->mission == 9)
	{
		AGSKFactor = 40.0*3600.0;
	}
	else if (GC->mission == 12)
	{
		AGSKFactor = 100.0*3600.0;
		//For PTC REFSMMAT
		REFSMMATTime = OrbMech::HHMMSSToSS(183, 0, 30);

		PDAP_J1 = 6.0325675e6*0.3048;
		PDAP_K1 = -6.2726125e5*0.3048;
		PDAP_J2 = 6.03047e6*0.3048;
		PDAP_K2 = -3.1835146e5*0.3048;
		PDAP_Theta_LIM = 8.384852304*RAD;
		PDAP_R_amin = 5.8768997e6*0.3048;
	}
	else if (GC->mission == 13)
	{
		//For PTC REFSMMAT
		REFSMMATTime = OrbMech::HHMMSSToSS(178, 30, 0);
	}
	else if (GC->mission == 14)
	{
		AGSKFactor = 100.0*3600.0;
		//For PTC REFSMMAT
		REFSMMATTime = OrbMech::HHMMSSToSS(166, 10, 30);
	}
	else if (GC->mission == 15)
	{
		AGSKFactor = 100.0*3600.0;
		//For PTC REFSMMAT
		REFSMMATTime = OrbMech::HHMMSSToSS(230, 9, 0);
	}
	else if (GC->mission == 16)
	{
		//For PTC REFSMMAT
		REFSMMATTime = OrbMech::HHMMSSToSS(166, 2, 50);
	}
	else if (GC->mission == 17)
	{
		AGSKFactor = 110.0*3600.0;
		//For PTC REFSMMAT
		REFSMMATTime = OrbMech::HHMMSSToSS(241, 29, 30);
	}

	Skylabmaneuver = 0;
	SkylabTPIGuess = 0.0;
	Skylab_n_C = 1.5;
	SkylabDH1 = 20.0*1852.0;
	SkylabDH2 = 10.0*1852.0;
	Skylab_E_L = 27.0*RAD;
	SkylabSolGood = true;
	Skylab_dV_NSR = Skylab_dV_NCC = _V(0, 0, 0);
	Skylab_dH_NC2 = Skylab_dv_NC2 = Skylab_t_NC1 = Skylab_t_NC2 = Skylab_dv_NCC = Skylab_t_NCC = Skylab_t_NSR = Skylab_dt_TPM = 0.0;
	Skylab_NPCOption = Skylab_PCManeuver = false;

	TMLat = 0.0;
	TMLng = 0.0;
	TMAzi = 0.0;
	TMDistance = 600000.0*0.3048;
	TMStepSize = 100.0*0.3048;
	TMAlt = 0.0;

	t_LunarLiftoff = 0.0;
	t_TPIguess = 0.0;

	EMPUplinkType = 0;
	EMPUplinkNumber = 0;

	LVDCLaunchAzimuth = 0.0;

	AGCEphemOption = 0;
	AGCEphemBRCSEpoch = GC->rtcc->AGCEpoch;
	AGCEphemTIMEM0 = floor(GC->rtcc->CalcGETBase()) + 6.75;
	AGCEphemTEPHEM = GC->rtcc->CalcGETBase();
	AGCEphemTLAND = GC->t_Land;
	AGCEphemMission = GC->mission;
	AGCEphemIsCMC = vesseltype < 2;

	earthentrypad.Att400K[0] = _V(0, 0, 0);
	earthentrypad.BankAN[0] = 0;
	earthentrypad.DRE[0] = 0;
	earthentrypad.dVTO[0] = 0;
	earthentrypad.Lat[0] = 0;
	earthentrypad.Lng[0] = 0;
	earthentrypad.PB_BankAN[0] = 0;
	earthentrypad.PB_DRE[0] = 0;
	earthentrypad.PB_R400K[0] = 0;
	earthentrypad.PB_Ret05[0] = 0;
	earthentrypad.PB_Ret2[0] = 0;
	earthentrypad.PB_RetBBO[0] = 0;
	earthentrypad.PB_RetDrog[0] = 0;
	earthentrypad.PB_RetEBO[0] = 0;
	earthentrypad.PB_RetRB[0] = 0;
	earthentrypad.PB_RTGO[0] = 0;
	earthentrypad.PB_VIO[0] = 0;
	earthentrypad.Ret05[0] = 0;
	earthentrypad.Ret2[0] = 0;
	earthentrypad.RetBBO[0] = 0;
	earthentrypad.RetDrog[0] = 0;
	earthentrypad.RetEBO[0] = 0;
	earthentrypad.RetRB[0] = 0;
	earthentrypad.RTGO[0] = 0;
	earthentrypad.VIO[0] = 0;

	lunarentrypad.Att05[0] = _V(0, 0, 0);
	lunarentrypad.BSS[0] = 0;
	lunarentrypad.DO[0] = 0.0;
	lunarentrypad.Gamma400K[0] = 0.0;
	lunarentrypad.GETHorCheck[0] = 0.0;
	lunarentrypad.Lat[0] = 0.0;
	lunarentrypad.LiftVector[0][0] = 0;
	lunarentrypad.Lng[0] = 0.0;
	lunarentrypad.MaxG[0] = 0.0;
	lunarentrypad.PitchHorCheck[0] = 0.0;
	lunarentrypad.RET05[0] = 0.0;
	lunarentrypad.RRT[0] = 0.0;
	lunarentrypad.RTGO[0] = 0.0;
	lunarentrypad.SFT[0] = 0.0;
	lunarentrypad.SPA[0] = 0.0;
	lunarentrypad.SXP[0] = 0.0;
	lunarentrypad.SXTS[0] = 0;
	lunarentrypad.TRN[0] = 0;
	lunarentrypad.V400K[0] = 0.0;
	lunarentrypad.VIO[0] = 0.0;

	navcheckpad.alt[0] = 0.0;
	navcheckpad.lat[0] = 0.0;
	navcheckpad.lng[0] = 0.0;
	navcheckpad.NavChk[0] = 0.0;

	agssvpad.DEDA240 = 0.0;
	agssvpad.DEDA241 = 0.0;
	agssvpad.DEDA242 = 0.0;
	agssvpad.DEDA244 = 0.0;
	agssvpad.DEDA245 = 0.0;
	agssvpad.DEDA246 = 0.0;
	agssvpad.DEDA254 = 0.0;
	agssvpad.DEDA260 = 0.0;
	agssvpad.DEDA261 = 0.0;
	agssvpad.DEDA262 = 0.0;
	agssvpad.DEDA264 = 0.0;
	agssvpad.DEDA265 = 0.0;
	agssvpad.DEDA266 = 0.0;
	agssvpad.DEDA272 = 0.0;

	DAP_PAD.OtherVehicleWeight = 0.0;
	DAP_PAD.PitchTrim = 0.0;
	DAP_PAD.ThisVehicleWeight = 0.0;
	DAP_PAD.YawTrim = 0.0;

	lmascentpad.CR = 0.0;
	lmascentpad.DEDA047 = 0;
	lmascentpad.DEDA053 = 0;
	lmascentpad.DEDA225_226 = 0.0;
	lmascentpad.DEDA231 = 0.0;
	sprintf(lmascentpad.remarks, "");
	lmascentpad.TIG = 0.0;
	lmascentpad.V_hor = 0.0;
	lmascentpad.V_vert = 0.0;

	NodeConvOpt = true;
	NodeConvLat = 0.0;
	NodeConvLng = 0.0;
	NodeConvGET = 0.0;
	NodeConvResLat = 0.0;
	NodeConvResLng = 0.0;
}

ARCore::~ARCore()
{
}

void ARCore::MinorCycle(double SimT, double SimDT, double mjd)
{
	if (g_Data.connStatus > 0 && g_Data.uplinkBuffer.size() > 0) {
		if (SimT > g_Data.uplinkBufferSimt + 0.1) {
			unsigned char data = g_Data.uplinkBuffer.front();
			send(m_socket, (char *)&data, 1, 0);
			g_Data.uplinkBuffer.pop();
			g_Data.uplinkBufferSimt = SimT;
		}
	}
	else if (g_Data.connStatus > 0 && g_Data.uplinkBuffer.size() == 0) {
		if (g_Data.connStatus == 1)	{
			sprintf(debugWinsock, "DISCONNECTED");
			//g_Data.uplinkDataReady = 0;
			//g_Data.updateClockReady = 0;
			g_Data.connStatus = 0;
			closesocket(m_socket);
		}
		//else if (g_Data.connStatus == 2 && g_Data.updateClockReady == 2) {
		//	UpdateClock();
		//}
	}
	if (g_Data.isRequesting && g_Data.progVessel->GetIMFDClient()->IsBurnDataValid()) {
		g_Data.burnData = g_Data.progVessel->GetIMFDClient()->GetBurnData();
		StopIMFDRequest();
		if (g_Data.burnData.p30mode && !g_Data.burnData.impulsive) {
			//g_Data.errorMessage = "IMFD not in Off-Axis, P30 Mode";
			//g_Data.progState = PROGSTATE_TLI_ERROR;
			dV_LVLH = g_Data.burnData._dV_LVLH;
			P30TIG = (g_Data.burnData.IgnMJD - GC->rtcc->CalcGETBase())*24.0*3600.0;

			double *EarthPos;
			EarthPos = new double[12];
			VECTOR3 EarthVec, EarthVecVel;
			CELBODY *cEarth;
			OBJHANDLE hEarth = oapiGetObjectByName("Earth");

			cEarth = oapiGetCelbodyInterface(hEarth);
			cEarth->clbkEphemeris(g_Data.burnData.IgnMJD + g_Data.burnData.BT / 24.0 / 3600.0, EPHEM_BARYPOS | EPHEM_BARYVEL, EarthPos);

			EarthVec = _V(EarthPos[0], EarthPos[1], EarthPos[2]);
			EarthVecVel = _V(EarthPos[3], EarthPos[4], EarthPos[5]);

			R_TLI = g_Data.burnData._RCut - EarthVec;
			R_TLI = _V(R_TLI.x, R_TLI.z, R_TLI.y);
			V_TLI = g_Data.burnData._VCut - EarthVecVel;
			V_TLI = _V(V_TLI.x, V_TLI.z, V_TLI.y);
		}
		else {
			//g_Data.progState = PROGSTATE_TLI_WAITING;
		}
	}
}

void ARCore::LmkCalc()
{
	startSubthread(33);
}

void ARCore::LOICalc()
{
	startSubthread(5);
}

void ARCore::LDPPalc()
{
	startSubthread(10);
}

void ARCore::SkylabCalc()
{
	startSubthread(12);
}

void ARCore::LunarLaunchTargetingCalc()
{
	startSubthread(13);
}

void ARCore::LunarLiftoffCalc()
{
	startSubthread(15);
}

void ARCore::EntryUpdateCalc()
{
	SV sv0;
	EntryResults res;

	sv0 = GC->rtcc->StateVectorCalc(vessel);
	GC->rtcc->EntryUpdateCalc(sv0, GC->rtcc->CalcGETBase(), entryrange, true, &res);

	EntryLatcor = res.latitude;
	EntryLngcor = res.longitude;
	EntryRTGO = res.RTGO;
}

void ARCore::EntryCalc()
{
	startSubthread(7);
}

void ARCore::DeorbitCalc()
{
	startSubthread(17);
}

void ARCore::MoonRTECalc()
{
	startSubthread(11);
}

void ARCore::SPQcalc()
{
	startSubthread(2);
}

void ARCore::lambertcalc()
{
	startSubthread(1);
}

void ARCore::GPMPCalc()
{
	startSubthread(3);
}

void ARCore::REFSMMATCalc()
{
	startSubthread(4);
}

void ARCore::TLI_PAD()
{
	startSubthread(8);
}

void ARCore::TLCCCalc()
{
	startSubthread(14);
}

void ARCore::PDI_PAD()
{
	startSubthread(16);
}

void ARCore::DKICalc()
{
	startSubthread(19);
}

void ARCore::LAPCalc()
{
	startSubthread(20);
}

void ARCore::AscentPADCalc()
{
	startSubthread(21);
}

void ARCore::PDAPCalc()
{
	startSubthread(22);
}

void ARCore::CycleFIDOOrbitDigitals1()
{
	if (subThreadStatus == 0)
	{
		double GET = OrbMech::GETfromMJD(oapiGetSimMJD(), GC->rtcc->CalcGETBase());
		if (GET > GC->rtcc->EZSAVCSM.GET + 12.0)
		{
			startSubthread(24);
		}
	}
}

void ARCore::CycleFIDOOrbitDigitals2()
{
	if (subThreadStatus == 0)
	{
		double GET = OrbMech::GETfromMJD(oapiGetSimMJD(), GC->rtcc->CalcGETBase());
		if (GET > GC->rtcc->EZSAVLEM.GET + 12.0)
		{
			startSubthread(26);
		}
	}
}

void ARCore::CycleSpaceDigitals()
{
	if (subThreadStatus == 0)
	{
		double GET = OrbMech::GETfromMJD(oapiGetSimMJD(), GC->rtcc->CalcGETBase());
		if (GET > GC->rtcc->EZSPACE.GET + 12.0)
		{
			startSubthread(29);
		}
	}
}

void ARCore::SpaceDigitalsMSKRequest()
{
	if (subThreadStatus == 0)
	{
		startSubthread(30);
	}
}

void ARCore::CycleNextStationContactsDisplay()
{
	if (subThreadStatus == 0)
	{
		double GET = OrbMech::GETfromMJD(oapiGetSimMJD(), GC->rtcc->CalcGETBase());
		if (GET > GC->rtcc->NextStationContactsBuffer.GET + 12.0)
		{
			startSubthread(36);
		}
	}
}

void ARCore::SLVNavigationUpdateCalc()
{
	startSubthread(27);
}

void ARCore::SLVNavigationUpdateUplink()
{
	startSubthread(28);
}

void ARCore::RTETradeoffDisplayCalc()
{
	startSubthread(52);
}

void ARCore::GeneralMEDRequest()
{
	startSubthread(53);
}

void ARCore::TransferTIToMPT()
{
	startSubthread(38);
}

void ARCore::TransferSPQToMPT()
{
	startSubthread(39);
}

void ARCore::TransferDKIToMPT()
{
	startSubthread(40);
}

void ARCore::MPTDirectInputCalc()
{
	startSubthread(41);
}

void ARCore::TransferDescentPlanToMPT()
{
	startSubthread(42);
}

void ARCore::TransferPoweredDescentToMPT()
{
	startSubthread(43);
}

void ARCore::TransferPoweredAscentToMPT()
{
	startSubthread(44);
}

void ARCore::TransferGPMToMPT()
{
	startSubthread(45);
}

void ARCore::MPTTLIDirectInput()
{
	startSubthread(46);
}

void ARCore::TransferLOIorMCCtoMPT()
{
	startSubthread(48);
}

void ARCore::TransferRTEToMPT()
{
	startSubthread(49);
}

void ARCore::DAPPADCalc()
{
	if (vesseltype < 2)
	{
		GC->rtcc->CSMDAPUpdate(vessel, DAP_PAD);
	}
	else
	{
		GC->rtcc->LMDAPUpdate(vessel, DAP_PAD, lemdescentstage == false);
	}
}

void ARCore::GenerateAGCEphemeris()
{
	AGCEphemeris(AGCEphemTIMEM0, AGCEphemBRCSEpoch, AGCEphemTEphemZero);
}

void ARCore::GenerateAGCCorrectionVectors()
{
	AGCCorrectionVectors(AGCEphemTEPHEM, AGCEphemTLAND, AGCEphemMission, AGCEphemIsCMC);
}

void ARCore::EntryPAD()
{
	startSubthread(31);
}

void ARCore::ManeuverPAD()
{
	startSubthread(9);
}

void ARCore::TPIPAD()
{
	startSubthread(6);
}

void ARCore::MapUpdate()
{
	startSubthread(32);
}

void ARCore::CalculateTPITime()
{
	startSubthread(23);
}

void ARCore::VectorCompareDisplayCalc()
{
	startSubthread(25);
}

void ARCore::GetStateVectorFromAGC(bool csm)
{
	agc_t* vagc;

	if (vesseltype < 2)
	{
		Saturn *saturn = (Saturn *)vessel;

		vagc = &saturn->agc.vagc;
	}
	else
	{
		LEM *lem = (LEM *)vessel;

		vagc = &lem->agc.vagc;
	}

	unsigned short SVoct[16];
	int SVadd, MoonBit;
	
	if (csm)
	{
		SVadd = 01554;
		MoonBit = 11;
	}
	else
	{
		SVadd = 01626;
		MoonBit = 10;
	}
	
	bool MoonFlag;

	for (int i = 0;i < 14;i++)
	{
		SVoct[i] = vagc->Erasable[0][SVadd + i];
	}
	SVoct[14] = vagc->Erasable[0][SVadd + 38];
	SVoct[15] = vagc->Erasable[0][SVadd + 39];

	MoonFlag = (vagc->Erasable[0][0104] & (1 << MoonBit));

	VECTOR3 R, V;
	double GET;

	R.x = OrbMech::DecToDouble(SVoct[0], SVoct[1]);
	R.y = OrbMech::DecToDouble(SVoct[2], SVoct[3]);
	R.z = OrbMech::DecToDouble(SVoct[4], SVoct[5]);
	V.x = OrbMech::DecToDouble(SVoct[6], SVoct[7])*100.0;
	V.y = OrbMech::DecToDouble(SVoct[8], SVoct[9])*100.0;
	V.z = OrbMech::DecToDouble(SVoct[10], SVoct[11])*100.0;
	GET = (OrbMech::DecToDouble(SVoct[12], SVoct[13]) - OrbMech::DecToDouble(SVoct[14], SVoct[15])) / 100.0*pow(2, 28);

	if (MoonFlag)
	{
		R.x *= pow(2, 27);
		R.y *= pow(2, 27);
		R.z *= pow(2, 27);
		V.x *= pow(2, 5);
		V.y *= pow(2, 5);
		V.z *= pow(2, 5);
	}
	else
	{
		R.x *= pow(2, 29);
		R.y *= pow(2, 29);
		R.z *= pow(2, 29);
		V.x *= pow(2, 7);
		V.y *= pow(2, 7);
		V.z *= pow(2, 7);
	}

	EphemerisData sv;
	sv.R = R;
	sv.V = V;
	sv.GMT = GC->rtcc->GMTfromGET(GET);
	if (MoonFlag)
	{
		sv.RBI = BODY_MOON;
	}
	else
	{
		sv.RBI = BODY_EARTH;
	}

	if (csm)
	{
		if (vesseltype < 2)
		{
			GC->rtcc->BZEVLVEC.Vectors[0] = sv;
			GC->rtcc->BZEVLVEC.ID[0] = 1;
		}
		else
		{
			GC->rtcc->BZEVLVEC.Vectors[1] = sv;
			GC->rtcc->BZEVLVEC.ID[1] = 1;
		}
	}
	else
	{
		if (vesseltype < 2)
		{
			GC->rtcc->BZEVLVEC.Vectors[6] = sv;
			GC->rtcc->BZEVLVEC.ID[6] = 1;
		}
		else
		{
			GC->rtcc->BZEVLVEC.Vectors[7] = sv;
			GC->rtcc->BZEVLVEC.ID[7] = 1;
		}
	}
}

void ARCore::NavCheckPAD()
{
	SV sv;

	sv = GC->rtcc->StateVectorCalc(vessel);

	GC->rtcc->NavCheckPAD(sv, navcheckpad, GC->rtcc->CalcGETBase(), navcheckpad.NavChk[0]);
}

void ARCore::LandingSiteUpdate()
{
	double lat, lng, rad;
	svtarget->GetEquPos(lng, lat, rad);

	GC->rtcc->BZLSDISP.lat[RTCC_LMPOS_BEST] = lat;
	GC->rtcc->BZLSDISP.lng[RTCC_LMPOS_BEST] = lng;
	GC->rtcc->MCSMLR = rad;
}

void ARCore::LSUplinkCalc()
{
	VECTOR3 R_P;

	R_P = OrbMech::r_from_latlong(GC->rtcc->BZLSDISP.lat[RTCC_LMPOS_BEST], GC->rtcc->BZLSDISP.lng[RTCC_LMPOS_BEST], GC->rtcc->MCSMLR);

	RLSUplink = R_P;

	RLSOctals[0] = 10;

	if (vesseltype < 2)
	{
		RLSOctals[1] = 2025;
	}
	else
	{
		if (GC->mission < 14)
		{
			RLSOctals[1] = 2022;
		}
		else
		{
			RLSOctals[1] = 2020;
		}
	}

	RLSOctals[2] = OrbMech::DoubleToBuffer(RLSUplink.x, 27, 1);
	RLSOctals[3] = OrbMech::DoubleToBuffer(RLSUplink.x, 27, 0);
	RLSOctals[4] = OrbMech::DoubleToBuffer(RLSUplink.y, 27, 1);
	RLSOctals[5] = OrbMech::DoubleToBuffer(RLSUplink.y, 27, 0);
	RLSOctals[6] = OrbMech::DoubleToBuffer(RLSUplink.z, 27, 1);
	RLSOctals[7] = OrbMech::DoubleToBuffer(RLSUplink.z, 27, 0);
}

void ARCore::LandingSiteUplink()
{
	for (int i = 0;i < 010;i++)
	{
		g_Data.emem[i] = RLSOctals[i];
	}

	UplinkData();
}

void ARCore::StateVectorCalc()
{
	SV sv0, sv1;
	MATRIX3 Rot;
	VECTOR3 pos, vel;
	double get;
	OBJHANDLE hMoon = oapiGetGbodyByName("Moon");
	OBJHANDLE hEarth = oapiGetGbodyByName("Earth");

	if (GC->MissionPlanningActive)
	{
		int mptveh;

		if (SVSlot)
		{
			mptveh = RTCC_MPT_CSM;
		}
		else
		{
			mptveh = RTCC_MPT_LM;
		}
		double GMT = GC->rtcc->GMTfromGET(SVDesiredGET);
		EphemerisData EPHEM;
		if (GC->rtcc->ELFECH(GMT, mptveh, EPHEM))
		{
			return;
		}

		sv1.R = EPHEM.R;
		sv1.V = EPHEM.V;
		sv1.MJD = OrbMech::MJDfromGET(EPHEM.GMT, GC->rtcc->GetGMTBase());
		sv1.gravref = GC->rtcc->GetGravref(EPHEM.RBI);
	}
	else
	{
		sv0 = GC->rtcc->StateVectorCalc(svtarget);
		sv1 = GC->rtcc->coast(sv0, SVDesiredGET - OrbMech::GETfromMJD(sv0.MJD, GC->rtcc->CalcGETBase()));
	}

	UplinkSV = sv1;

	Rot = OrbMech::J2000EclToBRCS(GC->rtcc->AGCEpoch);

	UplinkSV.R = mul(Rot, UplinkSV.R);
	UplinkSV.V = mul(Rot, UplinkSV.V);
	UplinkSV.MJD = OrbMech::GETfromMJD(UplinkSV.MJD, GC->rtcc->CalcGETBase());

	pos = UplinkSV.R;
	vel = UplinkSV.V*0.01;
	get = UplinkSV.MJD;

	SVOctals[0] = 21;
	SVOctals[1] = 1501;

	if (sv1.gravref == hMoon)
	{
		if (SVSlot)
		{
			SVOctals[2] = 2;
		}
		else
		{
			SVOctals[2] = 77775;	// Octal coded decimal
		}

		SVOctals[3] = OrbMech::DoubleToBuffer(pos.x, 27, 1);
		SVOctals[4] = OrbMech::DoubleToBuffer(pos.x, 27, 0);
		SVOctals[5] = OrbMech::DoubleToBuffer(pos.y, 27, 1);
		SVOctals[6] = OrbMech::DoubleToBuffer(pos.y, 27, 0);
		SVOctals[7] = OrbMech::DoubleToBuffer(pos.z, 27, 1);
		SVOctals[8] = OrbMech::DoubleToBuffer(pos.z, 27, 0);
		SVOctals[9] = OrbMech::DoubleToBuffer(vel.x, 5, 1);
		SVOctals[10] = OrbMech::DoubleToBuffer(vel.x, 5, 0);
		SVOctals[11] = OrbMech::DoubleToBuffer(vel.y, 5, 1);
		SVOctals[12] = OrbMech::DoubleToBuffer(vel.y, 5, 0);
		SVOctals[13] = OrbMech::DoubleToBuffer(vel.z, 5, 1);
		SVOctals[14] = OrbMech::DoubleToBuffer(vel.z, 5, 0);
		SVOctals[15] = OrbMech::DoubleToBuffer(get*100.0, 28, 1);
		SVOctals[16] = OrbMech::DoubleToBuffer(get*100.0, 28, 0);
		return;
	}

	if (sv1.gravref == hEarth)
	{
		if (SVSlot)
		{
			SVOctals[2] = 1;
		}
		else
		{
			SVOctals[2] = 77776;	// Octal coded decimal
		}

		SVOctals[3] = OrbMech::DoubleToBuffer(pos.x, 29, 1);
		SVOctals[4] = OrbMech::DoubleToBuffer(pos.x, 29, 0);
		SVOctals[5] = OrbMech::DoubleToBuffer(pos.y, 29, 1);
		SVOctals[6] = OrbMech::DoubleToBuffer(pos.y, 29, 0);
		SVOctals[7] = OrbMech::DoubleToBuffer(pos.z, 29, 1);
		SVOctals[8] = OrbMech::DoubleToBuffer(pos.z, 29, 0);
		SVOctals[9] = OrbMech::DoubleToBuffer(vel.x, 7, 1);
		SVOctals[10] = OrbMech::DoubleToBuffer(vel.x, 7, 0);
		SVOctals[11] = OrbMech::DoubleToBuffer(vel.y, 7, 1);
		SVOctals[12] = OrbMech::DoubleToBuffer(vel.y, 7, 0);
		SVOctals[13] = OrbMech::DoubleToBuffer(vel.z, 7, 1);
		SVOctals[14] = OrbMech::DoubleToBuffer(vel.z, 7, 0);
		SVOctals[15] = OrbMech::DoubleToBuffer(get*100.0, 28, 1);
		SVOctals[16] = OrbMech::DoubleToBuffer(get*100.0, 28, 0);
		return;
	}
}

void ARCore::AGSStateVectorCalc()
{
	AGSSVOpt opt;
	SV sv;

	sv = GC->rtcc->StateVectorCalc(svtarget);

	opt.AGSbase = AGSKFactor;
	opt.csm = SVSlot;
	opt.GETbase = GC->rtcc->CalcGETBase();
	opt.REFSMMAT = GC->rtcc->EZJGMTX3.data[0].REFSMMAT;
	opt.sv = sv;

	GC->rtcc->AGSStateVectorPAD(&opt, agssvpad);
}

void ARCore::StateVectorUplink()
{
	for (int i = 0;i < 021;i++)
	{
		g_Data.emem[i] = SVOctals[i];
	}

	UplinkData();
}


void ARCore::send_agc_key(char key)	{

	int bytesXmit = SOCKET_ERROR;
	unsigned char cmdbuf[4];

	if (vesseltype > 1){
		cmdbuf[0] = 031; // VA,SA for LEM
	}
	else{
		cmdbuf[0] = 043; // VA,SA for CM
	}

	switch (key) {
	case 'V': // 11-000-101 11-010-001										
		cmdbuf[1] = 0305;
		cmdbuf[2] = 0321;
		break;
	case 'N': // 11-111-100 00-011-111
		cmdbuf[1] = 0374;
		cmdbuf[2] = 0037;
		break;
	case 'E': // 11-110-000 01-111-100
		cmdbuf[1] = 0360;
		cmdbuf[2] = 0174;
		break;
	case 'R': // 11-001-001 10-110-010
		cmdbuf[1] = 0311;
		cmdbuf[2] = 0262;
		break;
	case 'C': // 11-111-000 00-111-110
		cmdbuf[1] = 0370;
		cmdbuf[2] = 0076;
		break;
	case 'K': // 11-100-100 11-011-001
		cmdbuf[1] = 0344;
		cmdbuf[2] = 0331;
		break;
	case '+': // 11-101-000 10-111-010
		cmdbuf[1] = 0350;
		cmdbuf[2] = 0272;
		break;
	case '-': // 11-101-100 10-011-011
		cmdbuf[1] = 0354;
		cmdbuf[2] = 0233;
		break;
	case '1': // 10-000-111 11-000-001
		cmdbuf[1] = 0207;
		cmdbuf[2] = 0301;
		break;
	case '2': // 10-001-011 10-100-010
		cmdbuf[1] = 0213;
		cmdbuf[2] = 0242;
		break;
	case '3': // 10-001-111 10-000-011
		cmdbuf[1] = 0217;
		cmdbuf[2] = 0203;
		break;
	case '4': // 10-010-011 01-100-100
		cmdbuf[1] = 0223;
		cmdbuf[2] = 0144;
		break;
	case '5': // 10-010-111 01-000-101
		cmdbuf[1] = 0227;
		cmdbuf[2] = 0105;
		break;
	case '6': // 10-011-011 00-100-110
		cmdbuf[1] = 0233;
		cmdbuf[2] = 0046;
		break;
	case '7': // 10-011-111 00-000-111
		cmdbuf[1] = 0237;
		cmdbuf[2] = 0007;
		break;
	case '8': // 10-100-010 11-101-000
		cmdbuf[1] = 0242;
		cmdbuf[2] = 0350;
		break;
	case '9': // 10-100-110 11-001-001
		cmdbuf[1] = 0246;
		cmdbuf[2] = 0311;
		break;
	case '0': // 11-000-001 11-110-000
		cmdbuf[1] = 0301;
		cmdbuf[2] = 0360;
		break;
	case 'S': // 11-001-101 10-010-011 (code 23)
		cmdbuf[1] = 0315;
		cmdbuf[2] = 0223;
		break;
	case 'T': // 11-010-001 01-110-100 (code 24)
		cmdbuf[1] = 0321;
		cmdbuf[2] = 0164;
		break;
	}
	for (int i = 0; i < 3; i++) {
		g_Data.uplinkBuffer.push(cmdbuf[i]);
	}
}

void ARCore::REFSMMATUplink(void)
{
	for (int i = 0; i < 20; i++)
	{
		if (vesseltype < 2)
		{
			g_Data.emem[i] = GC->rtcc->CZREFMAT.Block[0].Octals[i];
		}
		else
		{
			g_Data.emem[i] = GC->rtcc->CZREFMAT.Block[1].Octals[i];
		}
	}
	UplinkData();
}

void ARCore::P30UplinkCalc()
{
	if (vesseltype < 2)
	{
		GC->rtcc->CMMAXTDV(P30TIG, dV_LVLH);
	}
	else
	{
		GC->rtcc->CMMLXTDV(P30TIG, dV_LVLH);
	}
}

void ARCore::P30Uplink()
{
	int *P30Octals;
	if (vesseltype < 2)
	{
		P30Octals = GC->rtcc->CZAXTRDV.Octals;
	}
	else
	{
		P30Octals = GC->rtcc->CZLXTRDV.Octals;
	}
	for (int i = 0;i < 012;i++)
	{
		g_Data.emem[i] = P30Octals[i];
	}

	UplinkData();
}

void ARCore::RetrofireEXDVUplinkCalc()
{
	double getign = P30TIG;

	RetrofireEXDVOctals[0] = 16;
	RetrofireEXDVOctals[1] = 3400;
	RetrofireEXDVOctals[2] = OrbMech::DoubleToBuffer(EntryLatcor / PI2, 0, 1);
	RetrofireEXDVOctals[3] = OrbMech::DoubleToBuffer(EntryLatcor / PI2, 0, 0);
	RetrofireEXDVOctals[4] = OrbMech::DoubleToBuffer(EntryLngcor / PI2, 0, 1);
	RetrofireEXDVOctals[5] = OrbMech::DoubleToBuffer(EntryLngcor / PI2, 0, 0);
	RetrofireEXDVOctals[6] = OrbMech::DoubleToBuffer(dV_LVLH.x / 100.0, 7, 1);
	RetrofireEXDVOctals[7] = OrbMech::DoubleToBuffer(dV_LVLH.x / 100.0, 7, 0);
	RetrofireEXDVOctals[8] = OrbMech::DoubleToBuffer(dV_LVLH.y / 100.0, 7, 1);
	RetrofireEXDVOctals[9] = OrbMech::DoubleToBuffer(dV_LVLH.y / 100.0, 7, 0);
	RetrofireEXDVOctals[10] = OrbMech::DoubleToBuffer(dV_LVLH.z / 100.0, 7, 1);
	RetrofireEXDVOctals[11] = OrbMech::DoubleToBuffer(dV_LVLH.z / 100.0, 7, 0);
	RetrofireEXDVOctals[12] = OrbMech::DoubleToBuffer(getign*100.0, 28, 1);
	RetrofireEXDVOctals[13] = OrbMech::DoubleToBuffer(getign*100.0, 28, 0);
}

void ARCore::RetrofireEXDVUplink()
{
	for (int i = 0;i < 016;i++)
	{
		g_Data.emem[i] = RetrofireEXDVOctals[i];
	}

	UplinkData();
}

void ARCore::EntryUpdateUplink(void)
{
	g_Data.emem[0] = 06;
	g_Data.emem[1] = 3400;
	g_Data.emem[2] = OrbMech::DoubleToBuffer(EntryLatcor / PI2, 0, 1);
	g_Data.emem[3] = OrbMech::DoubleToBuffer(EntryLatcor / PI2, 0, 0);
	g_Data.emem[4] = OrbMech::DoubleToBuffer(EntryLngcor / PI2, 0, 1);
	g_Data.emem[5] = OrbMech::DoubleToBuffer(EntryLngcor / PI2, 0, 0);

	//g_Data.uplinkDataReady = 2;
	UplinkData(); // Go for uplink
}

void ARCore::TLANDUplinkCalc(void)
{
	if (vesseltype > 1)
	{
		TLANDOctals[0] = 5;

		if (GC->mission < 14)
		{
			TLANDOctals[1] = 2400;
			TLANDOctals[3] = 2401;
		}
		else
		{
			TLANDOctals[1] = 2026;
			g_Data.emem[3] = 2027;
		}
		TLANDOctals[2] = OrbMech::DoubleToBuffer(GC->t_Land*100.0, 28, 1);
		TLANDOctals[4] = OrbMech::DoubleToBuffer(GC->t_Land*100.0, 28, 0);
	}
}

void ARCore::TLANDUplink(void)
{
	if (vesseltype > 1)
	{
		for (int i = 0;i < 5;i++)
		{
			g_Data.emem[i] = TLANDOctals[i];
		}

		UplinkData2(); // Go for uplink
	}
}

void ARCore::EMPP99Uplink(int i)
{
	if (vesseltype > 1)
	{
		if (i == 0)
		{
			g_Data.emem[0] = 24;
			g_Data.emem[1] = 3404;
			g_Data.emem[2] = 1450;
			g_Data.emem[3] = 12324;
			g_Data.emem[4] = 5520;
			g_Data.emem[5] = 161;
			g_Data.emem[6] = 1400;
			g_Data.emem[7] = 12150;
			g_Data.emem[8] = 5656;
			g_Data.emem[9] = 3667;
			g_Data.emem[10] = 74066;
			g_Data.emem[11] = 12404;
			g_Data.emem[12] = 12433;
			g_Data.emem[13] = 1406;
			g_Data.emem[14] = 5313;
			g_Data.emem[15] = 143;
			g_Data.emem[16] = 36266;
			g_Data.emem[17] = 54333;
			g_Data.emem[18] = 6060;
			g_Data.emem[19] = 77634;

			UplinkData(); // Go for uplink
		}
		else if(i == 1)
		{
			g_Data.emem[0] = 12;
			g_Data.emem[1] = 3734;
			g_Data.emem[2] = 26;
			g_Data.emem[3] = 30605;
			g_Data.emem[4] = 151;
			g_Data.emem[5] = 5214;
			g_Data.emem[6] = 0;
			g_Data.emem[7] = 0;
			g_Data.emem[8] = 15400;
			g_Data.emem[9] = 0;

			UplinkData(); // Go for uplink
		}
		else if (i == 2)
		{
			g_Data.emem[0] = 17;
			g_Data.emem[1] = 3400;
			g_Data.emem[2] = 5520;
			g_Data.emem[3] = 3401;
			g_Data.emem[4] = 312;
			g_Data.emem[5] = 3402;
			g_Data.emem[6] = 5263;
			g_Data.emem[7] = 3426;
			g_Data.emem[8] = 10636;
			g_Data.emem[9] = 3427;
			g_Data.emem[10] = 56246;
			g_Data.emem[11] = 3430;
			g_Data.emem[12] = 77650;
			g_Data.emem[13] = 3431;
			g_Data.emem[14] = 75202;

			UplinkData2(); // Go for uplink
		}
		else if (i == 3)
		{
			g_Data.emem[0] = 15;
			g_Data.emem[1] = 3455;
			g_Data.emem[2] = 1404;
			g_Data.emem[3] = 1250;
			g_Data.emem[4] = 0;
			g_Data.emem[5] = 3515;
			g_Data.emem[6] = 4;
			g_Data.emem[7] = 2371;
			g_Data.emem[8] = 13001;
			g_Data.emem[9] = 2372;
			g_Data.emem[10] = 1420;
			g_Data.emem[11] = 2373;
			g_Data.emem[12] = 12067;

			UplinkData2(); // Go for uplink
		}
	}
}

void ARCore::AP11AbortCoefUplink()
{
	g_Data.emem[0] = 22;
	g_Data.emem[1] = 2550;
	g_Data.emem[2] = OrbMech::DoubleToBuffer(PDAPABTCOF[0] * pow(100.0, -4)*pow(2, 44), 0, 1);
	g_Data.emem[3] = OrbMech::DoubleToBuffer(PDAPABTCOF[0] * pow(100.0, -4)*pow(2, 44), 0, 0);
	g_Data.emem[4] = OrbMech::DoubleToBuffer(PDAPABTCOF[1] * pow(100.0, -3)*pow(2, 27), 0, 1);
	g_Data.emem[5] = OrbMech::DoubleToBuffer(PDAPABTCOF[1] * pow(100.0, -3)*pow(2, 27), 0, 0);
	g_Data.emem[6] = OrbMech::DoubleToBuffer(PDAPABTCOF[2] * pow(100.0, -2)*pow(2, 10), 0, 1);
	g_Data.emem[7] = OrbMech::DoubleToBuffer(PDAPABTCOF[2] * pow(100.0, -2)*pow(2, 10), 0, 0);
	g_Data.emem[8] = OrbMech::DoubleToBuffer(PDAPABTCOF[3] * pow(100.0, -1), 7, 1);
	g_Data.emem[9] = OrbMech::DoubleToBuffer(PDAPABTCOF[3] * pow(100.0, -1), 7, 0);
	g_Data.emem[10] = OrbMech::DoubleToBuffer(PDAPABTCOF[4] * pow(100.0, -4)*pow(2, 44), 0, 1);
	g_Data.emem[11] = OrbMech::DoubleToBuffer(PDAPABTCOF[4] * pow(100.0, -4)*pow(2, 44), 0, 0);
	g_Data.emem[12] = OrbMech::DoubleToBuffer(PDAPABTCOF[5] * pow(100.0, -3)*pow(2, 27), 0, 1);
	g_Data.emem[13] = OrbMech::DoubleToBuffer(PDAPABTCOF[5] * pow(100.0, -3)*pow(2, 27), 0, 0);
	g_Data.emem[14] = OrbMech::DoubleToBuffer(PDAPABTCOF[6] * pow(100.0, -2)*pow(2, 10), 0, 1);
	g_Data.emem[15] = OrbMech::DoubleToBuffer(PDAPABTCOF[6] * pow(100.0, -2)*pow(2, 10), 0, 0);
	g_Data.emem[16] = OrbMech::DoubleToBuffer(PDAPABTCOF[7] * pow(100.0, -1), 7, 1);
	g_Data.emem[17] = OrbMech::DoubleToBuffer(PDAPABTCOF[7] * pow(100.0, -1), 7, 0);

	UplinkData(); // Go for uplink
}

void ARCore::AP12AbortCoefUplink()
{
	g_Data.emem[0] = 16;
	if (GC->mission <= 13)
	{
		g_Data.emem[1] = 2550;
	}
	else
	{
		g_Data.emem[1] = 2545;
	}
	g_Data.emem[2] = OrbMech::DoubleToBuffer(PDAP_J1, 23, 1);
	g_Data.emem[3] = OrbMech::DoubleToBuffer(PDAP_J1, 23, 0);
	g_Data.emem[4] = OrbMech::DoubleToBuffer(PDAP_K1*PI2, 23, 1);
	g_Data.emem[5] = OrbMech::DoubleToBuffer(PDAP_K1*PI2, 23, 0);
	g_Data.emem[6] = OrbMech::DoubleToBuffer(PDAP_J2, 23, 1);
	g_Data.emem[7] = OrbMech::DoubleToBuffer(PDAP_J2, 23, 0);
	g_Data.emem[8] = OrbMech::DoubleToBuffer(PDAP_K2*PI2, 23, 1);
	g_Data.emem[9] = OrbMech::DoubleToBuffer(PDAP_K2*PI2, 23, 0);
	g_Data.emem[10] = OrbMech::DoubleToBuffer(PDAP_Theta_LIM / PI2, 0, 1);
	g_Data.emem[11] = OrbMech::DoubleToBuffer(PDAP_Theta_LIM / PI2, 0, 0);
	g_Data.emem[12] = OrbMech::DoubleToBuffer(PDAP_R_amin, 24, 1);
	g_Data.emem[13] = OrbMech::DoubleToBuffer(PDAP_R_amin, 24, 0);

	UplinkData(); // Go for uplink
}

void ARCore::UplinkData()
{
	if (g_Data.connStatus == 0) {
		int bytesRecv = SOCKET_ERROR;
		char addr[256];
		char buffer[8];
		m_socket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
		if (m_socket == INVALID_SOCKET) {
			//g_Data.uplinkDataReady = 0;
			sprintf(debugWinsock, "ERROR AT SOCKET(): %ld", WSAGetLastError());
			closesocket(m_socket);
			return;
		}
		sprintf(addr, "127.0.0.1");
		clientService.sin_family = AF_INET;
		clientService.sin_addr.s_addr = inet_addr(addr);
		if (g_Data.uplinkLEM > 0){ clientService.sin_port = htons(14243); }
		else{ clientService.sin_port = htons(14242); }
		if (connect(m_socket, (SOCKADDR*)&clientService, sizeof(clientService)) == SOCKET_ERROR) {
			//g_Data.uplinkDataReady = 0;
			sprintf(debugWinsock, "FAILED TO CONNECT, ERROR %ld", WSAGetLastError());
			closesocket(m_socket);
			return;
		}
		sprintf(debugWinsock, "CONNECTED");
		g_Data.uplinkState = 0;
		send_agc_key('V');
		send_agc_key('7');
		send_agc_key('1');
		send_agc_key('E');

		int cnt2 = (g_Data.emem[0] / 10);
		int cnt = (g_Data.emem[0] - (cnt2 * 10)) + cnt2 * 8;

		while (g_Data.uplinkState < cnt && cnt <= 20 && cnt >= 3)
			{
				sprintf(buffer, "%ld", g_Data.emem[g_Data.uplinkState]);
				uplink_word(buffer);
				g_Data.uplinkState++;
			}
		send_agc_key('V');
		send_agc_key('3');
		send_agc_key('3');
		send_agc_key('E');
		g_Data.connStatus = 1;
		g_Data.uplinkState = 0;
	}
}

void ARCore::UplinkData2()
{
	if (g_Data.connStatus == 0) {
		int bytesRecv = SOCKET_ERROR;
		char addr[256];
		char buffer[8];
		m_socket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
		if (m_socket == INVALID_SOCKET) {
			//g_Data.uplinkDataReady = 0;
			sprintf(debugWinsock, "ERROR AT SOCKET(): %ld", WSAGetLastError());
			closesocket(m_socket);
			return;
		}
		sprintf(addr, "127.0.0.1");
		clientService.sin_family = AF_INET;
		clientService.sin_addr.s_addr = inet_addr(addr);
		if (g_Data.uplinkLEM > 0){ clientService.sin_port = htons(14243); }
		else{ clientService.sin_port = htons(14242); }
		if (connect(m_socket, (SOCKADDR*)&clientService, sizeof(clientService)) == SOCKET_ERROR) {
			//g_Data.uplinkDataReady = 0;
			sprintf(debugWinsock, "FAILED TO CONNECT, ERROR %ld", WSAGetLastError());
			closesocket(m_socket);
			return;
		}
		sprintf(debugWinsock, "CONNECTED");
		g_Data.uplinkState = 0;
		send_agc_key('V');
		send_agc_key('7');
		send_agc_key('2');
		send_agc_key('E');

		int cnt2 = (g_Data.emem[0] / 10);
		int cnt = (g_Data.emem[0] - (cnt2 * 10)) + cnt2 * 8;

		while (g_Data.uplinkState < cnt && cnt <= 20 && cnt >= 3)
		{
			sprintf(buffer, "%ld", g_Data.emem[g_Data.uplinkState]);
			uplink_word(buffer);
			g_Data.uplinkState++;
		}
		send_agc_key('V');
		send_agc_key('3');
		send_agc_key('3');
		send_agc_key('E');
		g_Data.connStatus = 1;
		g_Data.uplinkState = 0;
	}
}

void ARCore::uplink_word(char *data)
{
	int i;
	for (i = 5; i > (int)strlen(data); i--) {
		send_agc_key('0');
	}
	for (i = 0; i < (int)strlen(data); i++) {
		send_agc_key(data[i]);
	}
	send_agc_key('E');
}

bool ARCore::vesselinLOS()
{
	VECTOR3 R, V;
	double MJD;
	OBJHANDLE gravref = GC->rtcc->AGCGravityRef(vessel);
	vessel->GetRelativePos(gravref, R);
	vessel->GetRelativeVel(gravref, V);
	MJD = oapiGetSimMJD();

	return OrbMech::vesselinLOS(R, V, MJD);
}

void ARCore::VecPointCalc()
{
	if (VECoption == 0)
	{
		if (VECbody == NULL) return;

		VECTOR3 vPos, pPos, relvec, UX, UY, UZ, loc;
		MATRIX3 M, M_R;
		double p_T, y_T;
		OBJHANDLE gravref = GC->rtcc->AGCGravityRef(vessel);

		p_T = 0;
		y_T = 0;

		if (VECdirection == 1)
		{
			p_T = PI;
			y_T = 0;
		}
		else if (VECdirection == 2)
		{
			p_T = 0;
			y_T = PI05;
		}
		else if (VECdirection == 3)
		{
			p_T = 0;
			y_T = -PI05;
		}
		else if (VECdirection == 4)
		{
			p_T = -PI05;
			y_T = 0;
		}
		else if (VECdirection == 5)
		{
			p_T = PI05;
			y_T = 0;
		}

		vessel->GetGlobalPos(vPos);
		oapiGetGlobalPos(VECbody, &pPos);
		vessel->GetRelativePos(gravref, loc);

		relvec = unit(pPos - vPos);
		relvec = _V(relvec.x, relvec.z, relvec.y);
		loc = _V(loc.x, loc.z, loc.y);

		UX = relvec;
		UY = unit(crossp(UX, -loc));
		UZ = unit(crossp(UX, crossp(UX, -loc)));

		M_R = _M(UX.x, UX.y, UX.z, UY.x, UY.y, UY.z, UZ.x, UZ.y, UZ.z);
		M = _M(cos(y_T)*cos(p_T), sin(y_T), -cos(y_T)*sin(p_T), -sin(y_T)*cos(p_T), cos(y_T), sin(y_T)*sin(p_T), sin(p_T), 0.0, cos(p_T));

		VECangles = OrbMech::CALCGAR(GC->rtcc->EZJGMTX1.data[0].REFSMMAT, mul(OrbMech::tmat(M), M_R));
	}
	else if (VECoption == 1)
	{
		VECangles = GC->rtcc->HatchOpenThermalControl(vessel, GC->rtcc->EZJGMTX1.data[0].REFSMMAT);
	}
	else
	{
		SV sv;

		GC->rtcc->PointAOTWithCSM(GC->rtcc->EZJGMTX1.data[0].REFSMMAT, sv, 2, 1, 0.0);
	}
}

void ARCore::TerrainModelCalc()
{
	MATRIX3 Rot3, Rot4;
	VECTOR3 R_P, UX10, UY10, UZ10, axis, R_loc;
	double ang, r_0, anginc, dist, lat, lng, alt;
	OBJHANDLE hMoon;

	hMoon = oapiGetObjectByName("Moon");
	ang = 0.0;
	dist = 0.0;

	R_P = unit(_V(cos(TMLng)*cos(TMLat), sin(TMLng)*cos(TMLat), sin(TMLat)));

	TMAlt = oapiSurfaceElevation(hMoon, TMLng, TMLat);
	r_0 = TMAlt + oapiGetSize(hMoon);
	anginc = TMStepSize / r_0;

	UX10 = R_P;
	UY10 = unit(crossp(_V(0.0, 0.0, 1.0), UX10));
	UZ10 = crossp(UX10, UY10);

	Rot3 = _M(UX10.x, UX10.y, UX10.z, UY10.x, UY10.y, UY10.z, UZ10.x, UZ10.y, UZ10.z);
	Rot4 = _M(1.0, 0.0, 0.0, 0.0, cos(TMAzi), -sin(TMAzi), 0.0, sin(TMAzi), cos(TMAzi));

	axis = mul(OrbMech::tmat(Rot3), mul(Rot4, _V(0.0, 1.0, 0.0)));


	FILE *file = fopen("TerrainModel.txt", "w");

	fprintf(file, "%f;%f\n", -dist, 0.0);

	while (dist < TMDistance)
	{
		ang += anginc;
		dist += TMStepSize;

		R_loc = OrbMech::RotateVector(axis, -ang, R_P);
		R_loc = unit(R_loc);

		lat = atan2(R_loc.z, sqrt(R_loc.x*R_loc.x + R_loc.y*R_loc.y));
		lng = atan2(R_loc.y, R_loc.x);

		alt = oapiSurfaceElevation(hMoon, lng, lat);

		fprintf(file, "%f;%f\n", -dist, alt - TMAlt);
	}

	if (file) fclose(file);
}

void ARCore::NodeConvCalc()
{
	VECTOR3 R_EMP, R_selen, R;

	double MJD = GC->rtcc->CalcGETBase() + NodeConvGET / 24.0 / 3600.0;
	MATRIX3 M_EMP = OrbMech::EMPMatrix(MJD);
	MATRIX3 Rot = OrbMech::GetRotationMatrix(BODY_MOON, MJD);

	if (NodeConvOpt)
	{
		R_selen = OrbMech::r_from_latlong(NodeConvLat, NodeConvLng);
		R = rhmul(Rot, R_selen);
		R_EMP = mul(M_EMP, R);
		OrbMech::latlong_from_r(R_EMP, NodeConvResLat, NodeConvResLng);
	}
	else
	{
		R_EMP = OrbMech::r_from_latlong(NodeConvLat, NodeConvLng);
		R = tmul(M_EMP, R_EMP);
		R_selen = rhtmul(Rot, R);
		OrbMech::latlong_from_r(R_selen, NodeConvResLat, NodeConvResLng);
	}
	if (NodeConvResLng < 0)
	{
		NodeConvResLng += PI2;
	}
}

void ARCore::SendNodeToSFP()
{
	GC->rtcc->PZSFPTAB.blocks[1].GMT_nd = GC->rtcc->GMTfromGET(NodeConvGET);
	GC->rtcc->PZSFPTAB.blocks[1].lat_nd = NodeConvResLat;
	GC->rtcc->PZSFPTAB.blocks[1].lng_nd = NodeConvResLng;
	GC->rtcc->PZSFPTAB.blocks[1].h_nd = NodeConvHeight;
}

int ARCore::startSubthread(int fcn) {
	if (subThreadStatus < 1) {
		// Punt thread
		subThreadMode = fcn;
		subThreadStatus = 1; // Busy
		DWORD id = 0;
		hThread = CreateThread(NULL, 0, RTCCMFD_Trampoline, this, 0, &id);
	}
	else {
		//Kill thread
		DWORD exitcode = 0;
		if (TerminateThread(hThread, exitcode))
		{
			subThreadStatus = 0;
			if (hThread != NULL) { CloseHandle(hThread); }
		}
		return(-1);
	}
	return(0);
}

int ARCore::subThread()
{
	int Result = 0;

	int mptveh, docked, mptotherveh;

	if (GC->MissionPlanningActive)
	{
		if (vesseltype < 2)
		{
			mptveh = RTCC_MPT_CSM;
			mptotherveh = RTCC_MPT_LM;
		}
		else
		{
			mptveh = RTCC_MPT_LM;
			mptotherveh = RTCC_MPT_CSM;
		}

		//Queue control
		GC->rtcc->PMSEXE(RTCC_MPT_CSM, GC->rtcc->RTCCPresentTimeGMT());
		GC->rtcc->PMSEXE(RTCC_MPT_LM, GC->rtcc->RTCCPresentTimeGMT());
	}

	if (vesseltype == 1 || vesseltype == 3)
	{
		docked = true;
	}
	else
	{
		docked = false;
	}

	//Generate ephemeris table, if necessary
	OrbMech::GenerateSunMoonEphemeris(oapiGetSimMJD(), GC->rtcc->pzefem);

	subThreadStatus = 2; // Running
	switch (subThreadMode) {
	case 0: // Test
		Sleep(5000); // Waste 5 seconds
		Result = 0;  // Success (negative = error)
		break;
	case 1: //Lambert Targeting
	{
		TwoImpulseOpt opt;
		TwoImpulseResuls res;
		EphemerisData sv_A, sv_P;

		if (GC->MissionPlanningActive)
		{
			EphemerisData EPHEM;

			double GMT;
			
			if (GC->rtcc->med_k30.ChaserVectorTime > 0)
			{
				GMT = GC->rtcc->GMTfromGET(GC->rtcc->med_k30.ChaserVectorTime);
			}
			else
			{
				GMT = GC->rtcc->RTCCPresentTimeGMT();
			}

			if (GC->rtcc->ELFECH(GMT, GC->rtcc->med_k30.Vehicle, EPHEM))
			{
				Result = 0;
				break;
			}

			sv_A = EPHEM;

			if (GC->rtcc->med_k30.TargetVectorTime > 0)
			{
				GMT = GC->rtcc->GMTfromGET(GC->rtcc->med_k30.TargetVectorTime);
			}
			else
			{
				GMT = GC->rtcc->RTCCPresentTimeGMT();
			}

			if (GC->rtcc->ELFECH(GMT, 4 - GC->rtcc->med_k30.Vehicle, EPHEM))
			{
				Result = 0;
				break;
			}
			sv_P = EPHEM;
		}
		else
		{
			sv_A = GC->rtcc->StateVectorCalcEphem(vessel);
			sv_P = GC->rtcc->StateVectorCalcEphem(target);
		}

		opt.mode = 2;

		if (GC->rtcc->med_k30.StartTime < 0)
		{
			opt.T1 = -1;
		}
		else
		{
			opt.T1 = GC->rtcc->GMTfromGET(GC->rtcc->med_k30.StartTime);
		}
		if (GC->rtcc->med_k30.EndTime < 0)
		{
			opt.T2 = -1;
		}
		else
		{
			opt.T2 = GC->rtcc->GMTfromGET(GC->rtcc->med_k30.EndTime);
		}
		
		opt.TimeStep = GC->rtcc->med_k30.TimeStep;
		opt.TimeRange = GC->rtcc->med_k30.TimeRange;
		opt.sv_A = sv_A;
		opt.sv_P = sv_P;
		opt.IVFLAG = GC->rtcc->med_k30.IVFlag;
		opt.ChaserVehicle = GC->rtcc->med_k30.Vehicle;

		GC->rtcc->PMSTICN(opt, res);

		Result = 0;
	}
	break;
	case 2:	//Concentric Rendezvous Processor
	{
		SPQOpt opt;
		SPQResults res;
		MPTSV sv_A, sv_P, sv_pre, sv_post;

		if (GC->MissionPlanningActive)
		{
			int err;
			double GMT_C, GMT_T;

			if (GC->rtcc->med_k01.ChaserThresholdGET < 0)
			{
				GMT_C = GC->rtcc->RTCCPresentTimeGMT();
			}
			else
			{
				GMT_C = GC->rtcc->GMTfromGET(GC->rtcc->med_k01.ChaserThresholdGET);
			}
			if (GC->rtcc->med_k01.TargetThresholdGET < 0)
			{
				GMT_T = GC->rtcc->RTCCPresentTimeGMT();
			}
			else
			{
				GMT_T = GC->rtcc->GMTfromGET(GC->rtcc->med_k01.TargetThresholdGET);
			}

			EphemerisData EPHEM;
			err = GC->rtcc->ELFECH(GMT_C, GC->rtcc->med_k01.ChaserVehicle, EPHEM);
			if (err)
			{
				Result = 0;
				break;
			}
			sv_A.R = EPHEM.R;
			sv_A.V = EPHEM.V;
			sv_A.MJD = OrbMech::MJDfromGET(EPHEM.GMT, GC->rtcc->GetGMTBase());
			sv_A.gravref = GC->rtcc->GetGravref(EPHEM.RBI);

			err = GC->rtcc->ELFECH(GMT_T, 4 - GC->rtcc->med_k01.ChaserVehicle, EPHEM);
			if (err)
			{
				Result = 0;
				break;
			}
			sv_P.R = EPHEM.R;
			sv_P.V = EPHEM.V;
			sv_P.MJD = OrbMech::MJDfromGET(EPHEM.GMT, GC->rtcc->GetGMTBase());
			sv_P.gravref = GC->rtcc->GetGravref(EPHEM.RBI);
		}
		else
		{
			if (target == NULL)
			{
				Result = 0;
				break;
			}

			sv_A = GC->rtcc->StateVectorCalc(vessel);
			sv_P = GC->rtcc->StateVectorCalc(target);
		}

		opt.DH = GC->rtcc->GZGENCSN.SPQDeltaH;
		opt.E = GC->rtcc->GZGENCSN.SPQElevationAngle;
		opt.GETbase = GC->rtcc->CalcGETBase();
		opt.sv_A = sv_A;
		opt.sv_P = sv_P;
		opt.WT = GC->rtcc->GZGENCSN.SPQTerminalPhaseAngle;
		if (GC->MissionPlanningActive)
		{
			opt.ChaserID = GC->rtcc->med_k01.ChaserVehicle;
		}
		else
		{
			if (vesseltype < 2)
			{
				opt.ChaserID = 1;
			}
			else
			{
				opt.ChaserID = 3;
			}
		}
		if (SPQMode != 1)
		{
			opt.t_CSI = CSItime;
			
			if (SPQMode == 2)
			{
				opt.K_CDH = 1;
				opt.OptimumCSI = true;
			}
			else
			{
				opt.K_CDH = CDHtimemode;
				opt.OptimumCSI = false;
			}
		}
		else
		{
			opt.t_CSI = -1;
			if (CDHtimemode == 0)
			{
				opt.t_CDH = CDHtime;
			}
			else
			{
				opt.t_CDH = GC->rtcc->FindDH(sv_A, sv_P, GC->rtcc->CalcGETBase(), CDHtime, GC->rtcc->GZGENCSN.SPQDeltaH);
			}
		}
		opt.t_TPI = GC->rtcc->GZGENCSN.TPIDefinitionValue;
		opt.I_CDH = GC->rtcc->med_k01.I_CDH;
		opt.DU_D = GC->rtcc->med_k01.CDH_Angle;

		GC->rtcc->PMMDKI(opt, res);

		if (SPQMode != 1)
		{
			SPQTIG = res.t_CSI;
		}
		else
		{
			SPQTIG = res.t_CDH;
		}

		if (SPQMode != 1)
		{
			CDHtime = res.t_CDH;
			SPQDeltaV = res.dV_CSI;
		}
		else
		{
			SPQDeltaV = res.dV_CDH;
		}

		Result = 0;
	}
	break;
	case 3:	//Orbital Adjustment Targeting
	{
		GMPOpt opt;
		SV sv0, sv_pre, sv_post;

		if (GC->MissionPlanningActive)
		{
			double GMT = GC->rtcc->GMTfromGET(SPSGET);
			EphemerisData EPHEM;
			if (GC->rtcc->ELFECH(GMT, GC->rtcc->med_k20.Vehicle, EPHEM))
			{
				Result = 0;
				break;
			}
			sv0.R = EPHEM.R;
			sv0.V = EPHEM.V;
			sv0.MJD = OrbMech::MJDfromGET(EPHEM.GMT, GC->rtcc->GetGMTBase());
			sv0.gravref = GC->rtcc->GetGravref(EPHEM.RBI);
		}
		else
		{
			sv0 = GC->rtcc->StateVectorCalc(vessel);
		}

		opt.ManeuverCode = GMPManeuverCode;
		opt.GETbase = GC->rtcc->CalcGETBase();
		opt.H_A = GMPApogeeHeight;
		opt.H_P = GMPPerigeeHeight;
		opt.dH_D = GMPHeightChange;
		opt.TIG_GET = SPSGET;
		opt.AltRef = OrbAdjAltRef;
		opt.dLAN = GMPNodeShiftAngle;
		opt.R_LLS = GC->rtcc->MCSMLR;
		opt.dW = GMPWedgeAngle;
		opt.long_D = GMPManeuverLongitude;
		opt.H_D = GMPManeuverHeight;
		opt.dV = GMPDeltaVInput;
		opt.Pitch = GMPPitch;
		opt.Yaw = GMPYaw;
		opt.dLOA = GMPApseLineRotAngle;
		opt.N = GMPRevs;
		opt.RV_MCC = sv0;

		GC->rtcc->GeneralManeuverProcessor(&opt, OrbAdjDVX, GPM_TIG, GMPResults);
		SV GPM_SV = GC->rtcc->coast(opt.RV_MCC, GPM_TIG - OrbMech::GETfromMJD(opt.RV_MCC.MJD, GC->rtcc->CalcGETBase()));
		OrbAdjDVX = mul(OrbMech::LVLH_Matrix(GPM_SV.R, GPM_SV.V), OrbAdjDVX);

		Result = 0;
	}
	break;
	case 4:	//REFSMMAT Calculation
	{
		REFSMMATOpt opt;

		opt.dV_LVLH = dV_LVLH;
		opt.GETbase = GC->rtcc->CalcGETBase();
		opt.LSAzi = GC->rtcc->med_k18.psi_DS*RAD;
		opt.LSLat = GC->rtcc->BZLSDISP.lat[RTCC_LMPOS_BEST];
		opt.LSLng = GC->rtcc->BZLSDISP.lng[RTCC_LMPOS_BEST];
		opt.mission = GC->mission;
		opt.REFSMMATopt = REFSMMATopt;

		if (REFSMMATopt == 0 || REFSMMATopt == 1)
		{
			opt.REFSMMATTime = P30TIG;
		}
		else if (REFSMMATopt == 5 || REFSMMATopt == 8)
		{
			opt.REFSMMATTime = GC->t_Land;
		}
		else
		{
			opt.REFSMMATTime = REFSMMATTime;
		}

		opt.vessel = vessel;
		opt.vesseltype = vesseltype;
		opt.HeadsUp = REFSMMATHeadsUp;
		if (vesseltype < 2)
		{
			opt.PresentREFSMMAT = GC->rtcc->EZJGMTX1.data[0].REFSMMAT;
		}
		else
		{
			opt.PresentREFSMMAT = GC->rtcc->EZJGMTX3.data[0].REFSMMAT;
		}

		opt.IMUAngles = VECangles;
		opt.csmlmdocked = !GC->MissionPlanningActive && docked;

		if (GC->MissionPlanningActive && GC->rtcc->MPTHasManeuvers(mptveh))
		{
			opt.useSV = true;

			if (REFSMMATopt == 0 || REFSMMATopt == 1 || REFSMMATopt == 2 || REFSMMATopt == 5)
			{
				//SV at specified time
				double GMT = GC->rtcc->GMTfromGET(opt.REFSMMATTime);
				EphemerisData EPHEM;
				if (GC->rtcc->ELFECH(GMT, mptveh, EPHEM))
				{
					Result = 0;
					break;
				}
				opt.RV_MCC.R = EPHEM.R;
				opt.RV_MCC.V = EPHEM.V;
				opt.RV_MCC.MJD = OrbMech::MJDfromGET(EPHEM.GMT, GC->rtcc->GetGMTBase());
				opt.RV_MCC.gravref = GC->rtcc->GetGravref(EPHEM.RBI);

				GC->rtcc->PLAWDT(mptveh, GMT, opt.RV_MCC.mass);
			}
			else if (REFSMMATopt == 3)
			{
				//Last SV in the table
				MissionPlanTable *tab;
				if (mptveh == RTCC_MPT_CSM)
				{
					tab = &GC->rtcc->PZMPTCSM;
				}
				else
				{
					tab = &GC->rtcc->PZMPTLEM;
				}

				opt.RV_MCC.R = tab->mantable.back().R_BO;
				opt.RV_MCC.V = tab->mantable.back().V_BO;
				opt.RV_MCC.MJD = OrbMech::MJDfromGET(tab->mantable.back().GMT_BO, GC->rtcc->GetGMTBase());
				opt.RV_MCC.gravref = GC->rtcc->GetGravref(tab->mantable.back().RefBodyInd);
				GC->rtcc->PLAWDT(mptveh, tab->mantable.back().GMT_BO, opt.RV_MCC.mass);
			}
			else
			{
				opt.useSV = false;
			}
		}
		else
		{
			opt.useSV = false;
		}

		MATRIX3 REFSMMAT = GC->rtcc->REFSMMATCalc(&opt);
		if (vesseltype < 2)
		{
			GC->rtcc->EMGSTSTM(1, REFSMMAT, RTCC_REFSMMAT_TYPE_CUR, GC->rtcc->RTCCPresentTimeGMT());
		}
		else
		{
			GC->rtcc->EMGSTSTM(3, REFSMMAT, RTCC_REFSMMAT_TYPE_CUR, GC->rtcc->RTCCPresentTimeGMT());
		}

		//sprintf(oapiDebugString(), "%f, %f, %f, %f, %f, %f, %f, %f, %f", REFSMMAT.m11, REFSMMAT.m12, REFSMMAT.m13, REFSMMAT.m21, REFSMMAT.m22, REFSMMAT.m23, REFSMMAT.m31, REFSMMAT.m32, REFSMMAT.m33);

		REFSMMATcur = REFSMMATopt;

		Result = 0;
	}
	break;
	case 5: //LOI Targeting
	{
		EphemerisData sv0;

		if (GC->MissionPlanningActive)
		{
			double gmt;
			if (GC->rtcc->med_k18.VectorTime != 0.0)
			{
				gmt = GC->rtcc->GMTfromGET(GC->rtcc->med_k18.VectorTime);
			}
			else
			{
				gmt = GC->rtcc->RTCCPresentTimeGMT();
				GC->rtcc->med_k18.VectorTime = GC->rtcc->GETfromGMT(gmt);
			}

			if (GC->rtcc->ELFECH(gmt, RTCC_MPT_CSM, sv0))
			{
				Result = 0;
				break;
			}
		}
		else
		{
			SV sv0_apo = GC->rtcc->StateVectorCalc(vessel);
			sv0.R = sv0_apo.R;
			sv0.V = sv0_apo.V;
			sv0.GMT = OrbMech::GETfromMJD(sv0_apo.MJD, GC->rtcc->GetGMTBase());
			if (sv0_apo.gravref == oapiGetObjectByName("Earth"))
			{
				sv0.RBI = BODY_EARTH;
			}
			else
			{
				sv0.RBI = BODY_MOON;
			}
		}

		GC->rtcc->PMMLRBTI(sv0);

		Result = 0;
	}
	break;
	case 6: //TPI PAD
	{
		AP7TPIPADOpt opt;
		AP7TPI pad;

		opt.dV_LVLH = dV_LVLH;
		opt.GETbase = GC->rtcc->CalcGETBase();
		opt.TIG = P30TIG;
		opt.sv_A = GC->rtcc->StateVectorCalc(vessel);
		opt.sv_P = GC->rtcc->StateVectorCalc(target);

		GC->rtcc->AP7TPIPAD(opt, pad);

		TPIPAD_AZ = pad.AZ;
		TPIPAD_BT = pad.Backup_bT;
		TPIPAD_ddH = pad.dH_Max;
		TPIPAD_dH = pad.dH_TPI;
		TPIPAD_dV_LOS = pad.Backup_dV;
		TPIPAD_ELmin5 = pad.EL;
		TPIPAD_R = pad.R;
		TPIPAD_Rdot = pad.Rdot;

		Result = 0;
	}
	break;
	case 7:	//Entry Targeting
	{
		EntryResults res;
		EntryOpt opt;
		double csmmass, lmascmass, lmdscmass, cfg_weight, sivbmass;
		std::bitset<4> cfg;

		if (GC->MissionPlanningActive)
		{
			if (GC->rtcc->NewMPTTrajectory(mptveh, opt.RV_MCC))
			{
				Result = 0;
				break;
			}
			if (GC->rtcc->PLAWDT(RTCC_MPT_CSM, GC->rtcc->GMTfromGET(EntryTIG), cfg, cfg_weight, csmmass, lmascmass, lmdscmass, sivbmass))
			{
				Result = 0;
				break;
			}
		}
		else
		{
			opt.RV_MCC = GC->rtcc->StateVectorCalc(vessel);

			if (vesseltype == 0)
			{
				csmmass = vessel->GetMass();
				lmascmass = lmdscmass = 0.0;
			}
			else if (vesseltype == 1)
			{
				csmmass = vessel->GetMass();
				double lmmass;
				if (lmmass = GC->rtcc->GetDockedVesselMass(vessel))
				{
					LEM *l = (LEM *)oapiGetVesselInterface(vessel->GetDockStatus(vessel->GetDockHandle(0)));
					lmascmass = l->GetAscentStageMass();
					lmdscmass = lmmass - lmascmass;
				}
			}
			else if (vesseltype == 2)
			{
				Result = 0;
				break;
			}
			else
			{
				double lmmass;
				LEM *l = (LEM *)vessel;
				lmmass = vessel->GetMass();
				lmascmass = l->GetAscentStageMass();
				lmdscmass = lmmass - lmascmass;

				if (csmmass = GC->rtcc->GetDockedVesselMass(vessel))
				{

				}
				else
				{
					Result = 0;
					break;
				}
			}
		}

		if (entrylongmanual)
		{
			opt.lng = EntryLng;
		}
		else
		{
			opt.lng = (double)landingzone;
		}

		opt.GETbase = GC->rtcc->CalcGETBase();

		if (GC->rtcc->RTEManeuverCodeLogic(GC->rtcc->PZREAP.RTEManeuverCode, csmmass, lmascmass, lmdscmass, opt.enginetype, opt.RV_MCC.mass))
		{
			Result = 0;
			break;
		}

		opt.csmlmdocked = false;
		opt.entrylongmanual = entrylongmanual;
		opt.ReA = EntryAng;
		opt.TIGguess = EntryTIG;
		opt.vessel = vessel;
		opt.type = entrycritical;
		opt.r_rbias = GC->rtcc->PZREAP.RRBIAS;
		opt.csmlmdocked = false;

		GC->rtcc->EntryTargeting(&opt, &res);

		Entry_DV = res.dV_LVLH;
		EntryTIGcor = res.P30TIG;
		EntryLatcor = res.latitude;
		EntryLngcor = res.longitude;
		EntryRRT = res.GET400K;
		EntryRET05G = res.GET05G;
		EntryRTGO = res.RTGO;
		EntryAngcor = res.ReA;
		P30TIG = EntryTIGcor;
		dV_LVLH = Entry_DV;
		entryprecision = res.precision;

		Result = 0;
	}
	break;
	case 8: //TLI PAD
	{
		SaturnV *SatV = (SaturnV*)g_Data.progVessel;
		LVDCSV *lvdc = (LVDCSV*)SatV->iu->GetLVDC();

		TLIPADOpt opt;

		SV sv_A, sv_IG, sv_TLI;
		sv_A = GC->rtcc->StateVectorCalc(vessel);

		if (lvdc->TU)
		{
			opt.dV_LVLH = dV_LVLH;
			opt.GETbase = GC->rtcc->CalcGETBase();
			opt.REFSMMAT = GC->rtcc->EZJGMTX1.data[0].REFSMMAT;
			opt.TIG = P30TIG;
			opt.vessel = vessel;
			opt.uselvdc = false;
			opt.SeparationAttitude = _V(PI, 120.0*RAD, 0.0);
		}
		else
		{
			LVDCTLIparam tliparam;

			tliparam.alpha_TS = lvdc->alpha_TS;
			tliparam.Azimuth = lvdc->Azimuth;
			tliparam.beta = lvdc->beta;
			tliparam.cos_sigma = lvdc->cos_sigma;
			tliparam.C_3 = lvdc->C_3;
			tliparam.DEC = lvdc->DEC;
			tliparam.e_N = lvdc->e_N;
			tliparam.f = lvdc->f;
			tliparam.mu = lvdc->mu;
			tliparam.omega_E = lvdc->omega_E;
			tliparam.phi_L = lvdc->PHI;
			tliparam.RA = lvdc->RAS;
			tliparam.R_N = lvdc->R_N;
			tliparam.T_2R = lvdc->T_2R;
			tliparam.T4C = lvdc->TB5 - lvdc->TB1;
			tliparam.theta_EO = lvdc->theta_EO;
			tliparam.t_D = lvdc->t_D;
			tliparam.T_L = lvdc->T_L;
			tliparam.T_LO = lvdc->T_LO + 17.0;
			tliparam.T_RG = lvdc->T_RG;
			tliparam.T_ST = lvdc->T_ST;
			tliparam.Tt_3R = lvdc->Tt_3R;
			tliparam.t_clock = lvdc->t_clock;

			double m0 = vessel->GetEmptyMass();
			GC->rtcc->LVDCTLIPredict(tliparam, m0, sv_A, GC->rtcc->CalcGETBase(), dV_LVLH, P30TIG, sv_IG, sv_TLI);

			R_TLI = sv_TLI.R;
			V_TLI = sv_TLI.V;

			opt.dV_LVLH = dV_LVLH;
			opt.GETbase = GC->rtcc->CalcGETBase();
			opt.REFSMMAT = GC->rtcc->EZJGMTX1.data[0].REFSMMAT;
			opt.TIG = P30TIG;
			opt.vessel = vessel;
			opt.SeparationAttitude = lvdc->XLunarAttitude;
			opt.TLI = OrbMech::GETfromMJD(sv_TLI.MJD, GC->rtcc->CalcGETBase());
			opt.R_TLI = R_TLI;
			opt.V_TLI = V_TLI;
			opt.uselvdc = true;
		}

		opt.sv0 = sv_A;

		GC->rtcc->TLI_PAD(&opt, tlipad);

		Result = 0;
	}
	break;
	case 9: //Maneuver PAD
	{
		SV sv_A;

		if (GC->MissionPlanningActive)
		{
			double GMT = GC->rtcc->GMTfromGET(P30TIG);
			EphemerisData EPHEM;
			if (GC->rtcc->ELFECH(GMT, mptveh, EPHEM))
			{
				Result = 0;
				break;
			}
			sv_A.R = EPHEM.R;
			sv_A.V = EPHEM.V;
			sv_A.MJD = OrbMech::MJDfromGET(EPHEM.GMT, GC->rtcc->GetGMTBase());
			sv_A.gravref = GC->rtcc->GetGravref(EPHEM.RBI);

			std::bitset<4> cfg;
			double cfg_weight, csm_weight, sivb_weight, lma_weight, lmd_weight;

			if (GC->rtcc->PLAWDT(mptveh, GMT, cfg, cfg_weight, csm_weight, lma_weight, lmd_weight, sivb_weight))
			{
				Result = 0;
				break;
			}

			if (vesseltype < 2)
			{
				sv_A.mass = csm_weight;
			}
			else
			{
				sv_A.mass = lma_weight + lmd_weight;
			}
		}
		else
		{
			sv_A = GC->rtcc->StateVectorCalc(vessel);
		}

		if (vesseltype < 2)
		{
			AP11ManPADOpt opt;

			opt.dV_LVLH = dV_LVLH;
			opt.enginetype = manpadenginetype;
			opt.GETbase = GC->rtcc->CalcGETBase();
			opt.HeadsUp = HeadsUp;
			opt.REFSMMAT = GC->rtcc->EZJGMTX1.data[0].REFSMMAT;
			opt.sxtstardtime = sxtstardtime;
			opt.TIG = P30TIG;
			opt.vessel = vessel;
			opt.vesseltype = vesseltype;
			opt.R_LLS = GC->rtcc->MCSMLR;
			opt.useSV = true;
			opt.RV_MCC = sv_A;

			GC->rtcc->AP11ManeuverPAD(&opt, manpad);
		}
		else
		{
			AP11LMManPADOpt opt;

			opt.dV_LVLH = dV_LVLH;
			opt.enginetype = manpadenginetype;
			opt.GETbase = GC->rtcc->CalcGETBase();
			opt.HeadsUp = HeadsUp;
			opt.REFSMMAT = GC->rtcc->EZJGMTX3.data[0].REFSMMAT;
			opt.sxtstardtime = sxtstardtime;
			opt.TIG = P30TIG;
			opt.vessel = vessel;
			opt.csmlmdocked = !GC->MissionPlanningActive && docked;
			opt.R_LLS = GC->rtcc->MCSMLR;
			opt.useSV = true;
			opt.RV_MCC = sv_A;

GC->rtcc->AP11LMManeuverPAD(&opt, lmmanpad);
		}

		Result = 0;
	}
	break;
	case 10: //Lunar Descent Planning Processor
	{
		SV sv;

		if (GC->MissionPlanningActive)
		{
			double gmt;

			if (GC->rtcc->med_k16.VectorTime != 0.0)
			{
				gmt = GC->rtcc->GMTfromGET(GC->rtcc->med_k16.VectorTime);
			}
			else
			{
				gmt = GC->rtcc->RTCCPresentTimeGMT();
			}

			EphemerisData EPHEM;
			if (GC->rtcc->ELFECH(gmt, GC->rtcc->med_k16.Vehicle, EPHEM))
			{
				Result = 0;
				break;
			}

			sv.R = EPHEM.R;
			sv.V = EPHEM.V;
			sv.MJD = OrbMech::MJDfromGET(EPHEM.GMT, GC->rtcc->GetGMTBase());
			sv.gravref = GC->rtcc->GetGravref(EPHEM.RBI);
		}
		else
		{
			sv = GC->rtcc->StateVectorCalc(vessel);
		}

		if (!GC->rtcc->LunarDescentPlanningProcessor(sv, GC->rtcc->CalcGETBase(), GC->rtcc->BZLSDISP.lat[RTCC_LMPOS_BEST], GC->rtcc->BZLSDISP.lng[RTCC_LMPOS_BEST], GC->rtcc->MCSMLR, GC->descplantable))
		{
			if (GC->rtcc->med_k16.Mode != 7)
			{
				GC->t_Land = GC->descplantable.PD_GETTD;
			}
		}

		Result = 0;
	}
	break;
	case 11: //TEI Targeting
	{
		RTEMoonOpt opt;
		EntryResults res;
		double csmmass, lmascmass, lmdscmass, cfg_weight, sivbmass;
		std::bitset<4> cfg;

		if (GC->MissionPlanningActive)
		{
			if (GC->rtcc->NewMPTTrajectory(RTCC_MPT_CSM, opt.RV_MCC))
			{
				Result = 0;
				break;
			}
			if (GC->rtcc->PLAWDT(RTCC_MPT_CSM, GC->rtcc->GMTfromGET(EntryTIG), cfg, cfg_weight, csmmass, lmascmass, lmdscmass, sivbmass))
			{
				Result = 0;
				break;
			}
		}
		else
		{
			opt.RV_MCC = GC->rtcc->StateVectorCalc(vessel);

			if (vesseltype == 0)
			{
				csmmass = vessel->GetMass();
				lmascmass = lmdscmass = 0.0;
			}
			else if (vesseltype == 1)
			{
				csmmass = vessel->GetMass();
				double lmmass;
				if (lmmass = GC->rtcc->GetDockedVesselMass(vessel))
				{
					LEM *l = (LEM *)oapiGetVesselInterface(vessel->GetDockStatus(vessel->GetDockHandle(0)));
					lmascmass = l->GetAscentStageMass();
					lmdscmass = lmmass - lmascmass;
				}
			}
			else if (vesseltype == 2)
			{
				Result = 0;
				break;
			}
			else
			{
				double lmmass;
				LEM *l = (LEM *)vessel;
				lmmass = vessel->GetMass();
				lmascmass = l->GetAscentStageMass();
				lmdscmass = lmmass - lmascmass;

				if (csmmass = GC->rtcc->GetDockedVesselMass(vessel))
				{

				}
				else
				{
					Result = 0;
					break;
				}
			}
		}

		entryprecision = 1;
		
		opt.EntryLng = EntryLng;
		opt.ATPLine = landingzone;

		if (RTECalcMode == 1)
		{
			opt.SMODE = 34;
		}
		else if (RTECalcMode == 2)
		{
			opt.SMODE = 14;
		}
		else if (RTECalcMode == 3)
		{
			opt.SMODE = 36;
		}
		else
		{
			opt.SMODE = 16;
		}

		opt.GETbase = GC->rtcc->CalcGETBase();
		opt.RevsTillTEI = 0;
		opt.vessel = vessel;
		opt.entrylongmanual = entrylongmanual;
		opt.TIGguess = EntryTIG;
		opt.Inclination = EntryDesiredInclination;
		opt.t_zmin = RTEReentryTime;

		if (GC->rtcc->RTEManeuverCodeLogic(GC->rtcc->PZREAP.RTEManeuverCode, csmmass, lmascmass, lmdscmass, opt.enginetype, opt.RV_MCC.mass))
		{
			Result = 0;
			break;
		}

		opt.csmlmdocked = false;

		GC->rtcc->RTEMoonTargeting(&opt, &res);
		TLCCSolGood = res.solutionfound;

		if (TLCCSolGood == false)
		{

		}
		else
		{
			Entry_DV = res.dV_LVLH;
			EntryTIGcor = res.P30TIG;
			EntryLatcor = res.latitude;
			EntryLngcor = res.longitude;
			EntryRRT = res.GET400K;
			EntryRET05G = res.GET05G;
			EntryRTGO = res.RTGO;
			EntryAngcor = res.ReA;
			P30TIG = EntryTIGcor;
			dV_LVLH = Entry_DV;
			entryprecision = res.precision;
			RTEReturnInclination = res.Incl;
			FlybyPeriAlt = res.FlybyAlt;
		}
		
		Result = 0;
	}
	break;
	case 12: //Skylab Rendezvous Targeting
	{
		SkyRendOpt opt;
		SkylabRendezvousResults res;

		opt.E_L = Skylab_E_L;
		opt.GETbase = GC->rtcc->CalcGETBase();
		opt.man = Skylabmaneuver;
		opt.DH1 = SkylabDH1;
		opt.DH2 = SkylabDH2;
		opt.n_C = Skylab_n_C;
		opt.target = target;
		opt.t_TPI = t_TPI;
		opt.PCManeuver = Skylab_PCManeuver;
		opt.NPCOption = Skylab_NPCOption;
		opt.vessel = vessel;

		if (Skylabmaneuver == 0)
		{
			opt.TPIGuess = SkylabTPIGuess;
		}
		else if (Skylabmaneuver == 1)
		{
			opt.t_C = Skylab_t_NC1;
		}
		else if (Skylabmaneuver == 2)
		{
			opt.t_C = Skylab_t_NC2;
		}
		else if (Skylabmaneuver == 3)
		{
			opt.t_C = Skylab_t_NCC;
		}
		else if (Skylabmaneuver == 4)
		{
			opt.t_C = Skylab_t_NSR;
		}
		else if (Skylabmaneuver == 5)
		{
			opt.t_C = t_TPI;
		}
		else if (Skylabmaneuver == 6)
		{
			opt.t_C = t_TPI + Skylab_dt_TPM;
		}
		else if (Skylabmaneuver == 7)
		{
			if (Skylab_PCManeuver == 0)
			{
				opt.t_NC = Skylab_t_NC1;
			}
			else
			{
				opt.t_NC = Skylab_t_NC2;
			}
		}

		SkylabSolGood = GC->rtcc->SkylabRendezvous(&opt, &res);

		if (SkylabSolGood)
		{
			if (Skylabmaneuver == 0)
			{
				t_TPI = res.t_TPI;
			}
			else
			{
				dV_LVLH = res.dV_LVLH;
				P30TIG = res.P30TIG;
			}
			
			if (Skylabmaneuver == 1)
			{
				Skylab_dH_NC2 = res.dH_NC2;
				Skylab_dv_NC2 = res.dv_NC2;
				Skylab_dv_NCC = res.dv_NCC;
				Skylab_dV_NSR = res.dV_NSR;
				Skylab_t_NC2 = res.t_NC2;
				Skylab_t_NCC = res.t_NCC;
				Skylab_t_NSR = res.t_NSR;
			}
			else if (Skylabmaneuver == 2)
			{
				Skylab_dv_NCC = res.dv_NCC;
				Skylab_dV_NSR = res.dV_NSR;
				Skylab_t_NCC = res.t_NCC;
				Skylab_t_NSR = res.t_NSR;
			}
			else if (Skylabmaneuver == 3)
			{
				Skylab_dV_NSR = res.dV_NSR;
				Skylab_t_NSR = res.t_NSR;
			}
		}

		Result = 0;
	}
	break;
	case 13: //Lunar Launch Targeting Processor (Short Rendezvous Profile)
	{
		LLTPOpt opt;
		EphemerisData sv_CSM;

		if (GC->MissionPlanningActive)
		{
			if (GC->rtcc->ELFECH(GC->rtcc->GMTfromGET(GC->rtcc->med_k50.GETV), RTCC_MPT_CSM, sv_CSM))
			{
				Result = 0;
				break;
			}
		}
		else
		{
			sv_CSM = GC->rtcc->StateVectorCalcEphem(target);
		}

		opt.sv_CSM = sv_CSM;
		opt.Y_S = GC->rtcc->PZLTRT.YawSteerCap;
		opt.V_Z_NOM = 32.0*0.3048;
		opt.T_TH = GC->rtcc->GMTfromGET(GC->rtcc->med_k50.GETTH);
		opt.R_LS = GC->rtcc->MCSMLR;
		opt.lat_LS = GC->rtcc->BZLSDISP.lat[0];
		opt.lng_LS = GC->rtcc->BZLSDISP.lng[0];
		opt.alpha_PF = GC->rtcc->PZLTRT.PoweredFlightArc;
		opt.dt_PF = GC->rtcc->PZLTRT.PoweredFlightTime;
		opt.dt_INS_TPI = GC->rtcc->PZLTRT.DT_Ins_TPI;
		opt.h_INS = GC->rtcc->PZLTRT.InsertionHeight;
		opt.DH_TPI = GC->rtcc->PZLTRT.DT_DH;
		opt.dTheta_TPI = GC->rtcc->PZLTRT.DT_Theta_i;
		opt.WT = GC->rtcc->PZLTRT.TerminalPhaseTravelAngle;

		if (GC->rtcc->LunarLiftoffTimePredictionDT(opt, GC->rtcc->PZLLTT))
		{
			t_LunarLiftoff = GC->rtcc->PZLLTT.GETLOR;
			GC->rtcc->PZLTRT.InsertionHorizontalVelocity = GC->rtcc->PZLLTT.VH;
		}

		Result = 0;
	}
	break;
	case 14: //MCC Targeting
	{
		SV sv0;
		double CSMmass, LMmass;

		if (GC->MissionPlanningActive)
		{
			std::bitset<4> cfg;
			double GMT = GC->rtcc->GMTfromGET(GC->rtcc->PZMCCPLN.VectorGET);
			EphemerisData EPHEM;
			if (GC->rtcc->ELFECH(GMT, RTCC_MPT_CSM, EPHEM))
			{
				Result = 0;
				break;
			}

			double lm_asc_weight, lm_dsc_weight, sivb_weight;

			sv0.R = EPHEM.R;
			sv0.V = EPHEM.V;
			sv0.MJD = OrbMech::MJDfromGET(EPHEM.GMT, GC->rtcc->GetGMTBase());
			sv0.gravref = GC->rtcc->GetGravref(EPHEM.RBI);
			GC->rtcc->PLAWDT(RTCC_MPT_CSM, GMT, cfg, sv0.mass, CSMmass, lm_asc_weight, lm_dsc_weight, sivb_weight);
			LMmass = lm_asc_weight + lm_dsc_weight;
		}
		else
		{
			sv0 = GC->rtcc->StateVectorCalc(vessel);

			CSMmass = sv0.mass;
			if (GC->rtcc->PZMCCPLN.Config)
			{
				LMmass = GC->rtcc->GetDockedVesselMass(vessel);
			}
			else
			{
				LMmass = 0.0;
			}
		}

		GC->rtcc->PZMCCPLN.Mode = TLCCmaneuver;
		GC->rtcc->TranslunarMidcourseCorrectionProcessor(sv0, CSMmass, LMmass);

		Result = 0;
	}
	break;
	case 15:	//Lunar Launch Window Processor
	{
		LunarLiftoffTimeOpt opt;
		SV sv_CSM;

		if (GC->MissionPlanningActive)
		{
			double GMT = GC->rtcc->GMTfromGET(GC->rtcc->med_k15.CSMVectorTime);
			EphemerisData EPHEM;
			if (GC->rtcc->ELFECH(GMT, RTCC_MPT_CSM, EPHEM))
			{
				Result = 0;
				break;
			}
			sv_CSM.R = EPHEM.R;
			sv_CSM.V = EPHEM.V;
			sv_CSM.MJD = OrbMech::MJDfromGET(EPHEM.GMT, GC->rtcc->GetGMTBase());
			sv_CSM.gravref = GC->rtcc->GetGravref(EPHEM.RBI);
		}
		else
		{
			sv_CSM = GC->rtcc->StateVectorCalc(target);
		}

		if (GC->rtcc->med_k15.CSI_Flag == 0)
		{
			opt.I_BURN = 0;
		}
		else if (GC->rtcc->med_k15.CSI_Flag < 0)
		{
			opt.I_BURN = 2;
		}
		else
		{
			opt.I_BURN = 1;
			opt.DT_B = GC->rtcc->med_k15.CSI_Flag;
		}
		opt.I_TPI = GC->rtcc->med_k15.TPIDefinition;
		opt.I_CDH = GC->rtcc->med_k15.CDH_Flag;
		opt.t_BASE = GC->rtcc->PZLTRT.dt_bias;
		if (GC->rtcc->med_k15.DeltaHTFlag < 0)
		{
			opt.I_SRCH = 0;
		}
		else
		{
			opt.I_SRCH = 1;
			opt.N_CURV = GC->rtcc->med_k15.DeltaHTFlag;
		}
		opt.L_DH = 3;
		opt.t_max = GC->rtcc->PZLTRT.MaxAscLifetime;
		opt.H_S = GC->rtcc->PZLTRT.MinSafeHeight;
		opt.DV_MAX[0] = GC->rtcc->PZLTRT.CSMMaxDeltaV;
		opt.DV_MAX[1] = GC->rtcc->PZLTRT.LMMaxDeltaV;
		opt.DH[0] = GC->rtcc->med_k15.DH1;
		opt.DH[1] = GC->rtcc->med_k15.DH2;
		opt.DH[2] = GC->rtcc->med_k15.DH3;
		opt.theta_1 = GC->rtcc->PZLTRT.PoweredFlightArc;
		opt.dt_1 = GC->rtcc->PZLTRT.PoweredFlightTime;
		opt.v_LH = GC->rtcc->PZLTRT.InsertionHorizontalVelocity;
		opt.v_LV = GC->rtcc->PZLTRT.InsertionRadialVelocity;
		opt.h_BO = GC->rtcc->PZLTRT.InsertionHeight;
		opt.Y_S = GC->rtcc->PZLTRT.YawSteerCap;
		opt.DH_SRCH = GC->rtcc->PZLTRT.Height_Diff_Begin;
		opt.DH_STEP = GC->rtcc->PZLTRT.Height_Diff_Incr;
		opt.theta_F = GC->rtcc->PZLTRT.TerminalPhaseTravelAngle;
		opt.E = GC->rtcc->PZLTRT.ElevationAngle;
		opt.DH_OFF = GC->rtcc->PZLTRT.TPF_Height_Offset;
		opt.dTheta_OFF = GC->rtcc->PZLTRT.TPF_Phase_Offset;
		opt.t_hole = GC->rtcc->GMTfromGET(GC->rtcc->med_k15.ThresholdTime);
		opt.lat = GC->rtcc->BZLSDISP.lat[RTCC_LMPOS_BEST];
		opt.lng = GC->rtcc->BZLSDISP.lng[RTCC_LMPOS_BEST];
		opt.R_LLS = GC->rtcc->MCSMLR;
		opt.lng_TPI = GC->rtcc->med_k15.TPIValue;
		opt.sv_CSM = sv_CSM;
		if (GC->rtcc->med_k15.Chaser == 1)
		{
			opt.M = 1;
			opt.P = 2;
		}
		else
		{
			opt.M = 2;
			opt.P = 1;
		}

		GC->rtcc->LunarLaunchWindowProcessor(opt);

		Result = 0;
	}
	break;
	case 16: //PDI PAD
	{
		PDIPADOpt opt;
		AP11PDIPAD temppdipad;

		if (GC->MissionPlanningActive)
		{
			if (GC->rtcc->NewMPTTrajectory(RTCC_MPT_LM, opt.sv0))
			{
				Result = 0;
				break;
			}
		}
		else
		{
			opt.sv0 = GC->rtcc->StateVectorCalc(vessel);
		}

		opt.direct = true;
		opt.GETbase = GC->rtcc->CalcGETBase();
		opt.HeadsUp = HeadsUp;
		opt.REFSMMAT = GC->rtcc->EZJGMTX3.data[0].REFSMMAT;
		opt.R_LS = OrbMech::r_from_latlong(GC->rtcc->BZLSDISP.lat[RTCC_LMPOS_BEST], GC->rtcc->BZLSDISP.lng[RTCC_LMPOS_BEST], GC->rtcc->MCSMLR);
		opt.t_land = GC->t_Land;
		opt.vessel = vessel;

		PADSolGood = GC->rtcc->PDI_PAD(&opt, temppdipad);

		if (PADSolGood)
		{
			pdipad = temppdipad;
		}

		Result = 0;
	}
	break;
	case 17: //Deorbit Maneuver
	{
		EntryResults res;
		EarthEntryOpt opt;

		if (GC->MissionPlanningActive)
		{
			if (GC->rtcc->NewMPTTrajectory(mptveh, opt.RV_MCC))
			{
				Result = 0;
				break;
			}
		}
		else
		{
			opt.RV_MCC = GC->rtcc->StateVectorCalc(vessel);
		}

		if (entrylongmanual)
		{
			opt.lng = EntryLng;
		}
		else
		{
			opt.lng = (double)landingzone;
		}
		
		opt.GETbase = GC->rtcc->CalcGETBase();
		opt.enginetype = deorbitenginetype;
		opt.entrylongmanual = entrylongmanual;
		opt.ReA = EntryAng;
		opt.TIGguess = EntryTIG;
		opt.vessel = vessel;
		opt.nominal = entrynominal;
		opt.useSV = true;

		GC->rtcc->BlockDataProcessor(&opt, &res);

		Entry_DV = res.dV_LVLH;
		EntryTIGcor = res.P30TIG;
		EntryLatcor = res.latitude;
		EntryLngcor = res.longitude;
		EntryRRT = res.GET400K;
		EntryRET05G = res.GET05G;
		EntryRTGO = res.RTGO;
		EntryAngcor = res.ReA;
		P30TIG = EntryTIGcor;
		dV_LVLH = Entry_DV;
		entryprecision = res.precision;

		Result = 0;
	}
	break;
	case 19: //Docking Initiation Processor
	{
		DKIOpt opt;
		SV sv_A, sv_P, sv_pre, sv_post;

		if (GC->MissionPlanningActive)
		{
			EphemerisData EPHEM;
			double GMT;

			if (GC->rtcc->med_k10.MLDTime < 0)
			{
				GMT = GC->rtcc->RTCCPresentTimeGMT();
			}
			else
			{
				GMT = GC->rtcc->GMTfromGET(GC->rtcc->med_k10.MLDTime);
			}

			int err = GC->rtcc->ELFECH(GMT, GC->rtcc->med_k00.ChaserVehicle, EPHEM);
			if (err)
			{
				Result = 0;
				break;
			}
			sv_A.R = EPHEM.R;
			sv_A.V = EPHEM.V;
			sv_A.MJD = OrbMech::MJDfromGET(EPHEM.GMT, GC->rtcc->GetGMTBase());
			sv_A.gravref = GC->rtcc->GetGravref(EPHEM.RBI);

			err = GC->rtcc->ELFECH(GMT, 4 - GC->rtcc->med_k00.ChaserVehicle, EPHEM);
			if (err)
			{
				Result = 0;
				break;
			}
			sv_P.R = EPHEM.R;
			sv_P.V = EPHEM.V;
			sv_P.MJD = OrbMech::MJDfromGET(EPHEM.GMT, GC->rtcc->GetGMTBase());
			sv_P.gravref = GC->rtcc->GetGravref(EPHEM.RBI);
		}
		else
		{
			if (target == NULL)
			{
				Result = 0;
				break;
			}

			sv_A = GC->rtcc->StateVectorCalc(vessel);
			sv_P = GC->rtcc->StateVectorCalc(target);
		}		

		opt.DH = GC->rtcc->GZGENCSN.DKIDeltaH;
		opt.E = GC->rtcc->GZGENCSN.DKIElevationAngle;
		opt.GETbase = GC->rtcc->CalcGETBase();
		opt.maneuverline = DKI_Maneuver_Line;
		opt.N_HC = DKI_N_HC;
		opt.N_PB = DKI_N_PB;
		opt.plan = DKI_Profile;
		opt.radial_dv = DKI_Radial_DV;
		opt.tpimode = DKI_TPI_Mode;
		opt.dt_TPI_sunrise = DKI_dt_TPI_sunrise;
		opt.sv_A = sv_A;
		opt.sv_P = sv_P;
		opt.t_TIG = DKI_TIG;
		opt.t_TPI_guess = t_TPIguess;
		opt.DeltaT_BHAM = DKI_dt_BHAM;
		opt.DeltaT_PBH = DKI_dt_PBH;
		opt.Delta_HAMH = DKI_dt_HAMH;

		GC->rtcc->DockingInitiationProcessor(opt, dkiresult);

		if (DKI_Profile != 3)
		{
			DKI_DV = dkiresult.DV_Phasing;
		}

		t_TPI = dkiresult.t_TPI;

		Result = 0;
	}
	break;
	case 20: //Lunar Ascent Processor
	{
		SV sv_CSM, sv_Ins, sv_IG;
		VECTOR3 R_LS;
		double theta, dt, m0, dv;

		if (GC->MissionPlanningActive)
		{
			double GMT = GC->rtcc->GMTfromGET(t_LunarLiftoff);
			EphemerisData EPHEM;
			if (GC->rtcc->ELFECH(GMT, RTCC_MPT_CSM, EPHEM))
			{
				Result = 0;
				break;
			}

			sv_CSM.R = EPHEM.R;
			sv_CSM.V = EPHEM.V;
			sv_CSM.MJD = OrbMech::MJDfromGET(EPHEM.GMT, GC->rtcc->GetGMTBase());
			sv_CSM.gravref = GC->rtcc->GetGravref(EPHEM.RBI);

			std::bitset<4> cfg;
			double cfg_weight, csm_weight, lma_weight, lmd_weight, sivb_weight;
			if (GC->rtcc->PLAWDT(RTCC_MPT_LM, GMT, cfg, cfg_weight, csm_weight, lma_weight, lmd_weight, sivb_weight))
			{
				Result = 0;
				break;
			}
			m0 = lma_weight;
		}
		else
		{
			if (target == NULL)
			{
				Result = 0;
				break;
			}
			sv_CSM = GC->rtcc->StateVectorCalc(target);
			LEM *l = (LEM *)vessel;
			m0 = l->GetAscentStageMass();
		}

		R_LS = OrbMech::r_from_latlong(GC->rtcc->BZLSDISP.lat[RTCC_LMPOS_BEST], GC->rtcc->BZLSDISP.lng[RTCC_LMPOS_BEST], GC->rtcc->MCSMLR);

		GC->rtcc->LunarAscentProcessor(R_LS, m0, sv_CSM, GC->rtcc->CalcGETBase(), t_LunarLiftoff, GC->rtcc->PZLTRT.InsertionHorizontalVelocity, GC->rtcc->PZLTRT.InsertionRadialVelocity, theta, dt, dv, sv_IG, sv_Ins);

		GC->rtcc->PZLTRT.PoweredFlightArc = theta;
		GC->rtcc->PZLTRT.PoweredFlightTime = dt;

		GC->rtcc->JZLAI.t_launch = t_LunarLiftoff;
		GC->rtcc->JZLAI.R_D = 60000.0*0.3048;
		GC->rtcc->JZLAI.Y_D = 0.0;
		GC->rtcc->JZLAI.R_D_dot = GC->rtcc->PZLTRT.InsertionRadialVelocity;
		GC->rtcc->JZLAI.Y_D_dot = 0.0;
		GC->rtcc->JZLAI.Z_D_dot = GC->rtcc->PZLTRT.InsertionHorizontalVelocity;
		GC->rtcc->JZLAI.sv_CSM = sv_CSM;
		GC->rtcc->JZLAI.sv_Insertion = sv_Ins;

		Result = 0;
	}
	break;
	case 21: //LM Ascent PAD
	{
		ASCPADOpt opt;
		SV sv_CSM;
		MATRIX3 Rot, Rot2;

		sv_CSM = GC->rtcc->StateVectorCalc(target);
		vessel->GetRotationMatrix(Rot);
		oapiGetRotationMatrix(sv_CSM.gravref, &Rot2);

		opt.GETbase = GC->rtcc->CalcGETBase();
		opt.Rot_VL = OrbMech::GetVesselToLocalRotMatrix(Rot, Rot2);
		opt.R_LS = OrbMech::r_from_latlong(GC->rtcc->BZLSDISP.lat[RTCC_LMPOS_BEST], GC->rtcc->BZLSDISP.lng[RTCC_LMPOS_BEST], GC->rtcc->MCSMLR);
		opt.sv_CSM = sv_CSM;
		opt.TIG = t_LunarLiftoff;
		opt.v_LH = GC->rtcc->PZLTRT.InsertionHorizontalVelocity;
		opt.v_LV = GC->rtcc->PZLTRT.InsertionRadialVelocity;

		GC->rtcc->LunarAscentPAD(opt, lmascentpad);

		Result = 0;
	}
	break;
	case 22: //Powered Descent Abort Program
	{
		PDAPOpt opt;
		PDAPResults res;
		SV sv_LM, sv_CSM;

		LEM *l = (LEM *)vessel;


		if (GC->MissionPlanningActive)
		{
			if (GC->rtcc->NewMPTTrajectory(RTCC_MPT_LM, sv_LM))
			{
				Result = 0;
				break;
			}
			if (GC->rtcc->NewMPTTrajectory(RTCC_MPT_CSM, sv_CSM))
			{
				Result = 0;
				break;
			}

			opt.W_TAPS = 0.0;
			opt.W_TDRY = 0.0;
		}
		else
		{
			if (vesseltype < 2 || target == NULL)
			{
				Result = 0;
				break;
			}

			opt.W_TAPS = l->GetAscentStageMass();
			opt.W_TDRY = opt.sv_A.mass - vessel->GetPropellantMass(vessel->GetPropellantHandleByIndex(0));

			sv_LM = GC->rtcc->StateVectorCalc(vessel);
			sv_CSM = GC->rtcc->StateVectorCalc(target);
		}

		if (PDAPEngine == 0)
		{
			opt.dt_stage = 999999.9;
		}
		else
		{
			opt.dt_stage = 0.0;

		}

		opt.GETbase = GC->rtcc->CalcGETBase();
		opt.IsTwoSegment = GC->mission > 11;
		opt.REFSMMAT = GC->rtcc->EZJGMTX3.data[0].REFSMMAT;
		opt.R_LS = OrbMech::r_from_latlong(GC->rtcc->BZLSDISP.lat[RTCC_LMPOS_BEST], GC->rtcc->BZLSDISP.lng[RTCC_LMPOS_BEST], GC->rtcc->MCSMLR);
		opt.sv_A = sv_LM;
		opt.sv_P = sv_CSM;
		opt.TLAND = GC->t_Land;
		opt.t_TPI = t_TPI;
		if (opt.IsTwoSegment)
		{
			opt.dt_step = 20.0;
		}
		else
		{
			opt.dt_step = 120.0;
		}

		if (PADSolGood = GC->rtcc->PoweredDescentAbortProgram(opt, res))
		{
			if (opt.IsTwoSegment == false)
			{
				if (PDAPEngine == 0)
				{
					PDAPABTCOF[0] = res.ABTCOF1;
					PDAPABTCOF[1] = res.ABTCOF2;
					PDAPABTCOF[2] = res.ABTCOF3;
					PDAPABTCOF[3] = res.ABTCOF4;
				}
				else
				{
					PDAPABTCOF[4] = res.ABTCOF1;
					PDAPABTCOF[5] = res.ABTCOF2;
					PDAPABTCOF[6] = res.ABTCOF3;
					PDAPABTCOF[7] = res.ABTCOF4;
				}
			}
			else
			{
				PDAP_J1 = res.J1;
				PDAP_J2 = res.J2;
				PDAP_K1 = res.K1;
				PDAP_K2 = res.K2;
				PDAP_Theta_LIM = res.Theta_LIM;
				PDAP_R_amin = res.R_amin;
			}

			DEDA224 = res.DEDA224;
			DEDA225 = res.DEDA225;
			DEDA226 = res.DEDA226;
			DEDA227 = OrbMech::DoubleToDEDA(res.DEDA227 / 0.3048*pow(2, -20), 14);
		}

		Result = 0;
	}
	break;
	case 23: //Calculate TPI times
	{
		SV sv0 = GC->rtcc->StateVectorCalc(target);
		t_TPI = GC->rtcc->CalculateTPITimes(sv0, DKI_TPI_Mode, t_TPIguess, DKI_dt_TPI_sunrise);

		Result = 0;
	}
	break;
	case 24: //FIDO Orbit Digitals No 1 Cycle
	{
		GC->rtcc->EMMDYNMC(1, 1);
		Result = 0;
	}
	break;
	case 25: //Vector Compare Display
	{
		GetStateVectorFromAGC(true);
		GetStateVectorFromAGC(false);

		if (vesseltype < 2)
		{
			GC->rtcc->BZEVLVEC.Vectors[4] = GC->rtcc->StateVectorCalcEphem(vessel);
			GC->rtcc->BZEVLVEC.ID[4] = 1;
		}
		else
		{
			GC->rtcc->BZEVLVEC.Vectors[10] = GC->rtcc->StateVectorCalcEphem(vessel);
			GC->rtcc->BZEVLVEC.ID[10] = 1;
		}

		GC->rtcc->BMSVEC();
		Result = 0;
	}
	break;
	case 26: //FIDO Orbit Digitals No 1 Cycle
	{
		GC->rtcc->EMMDYNMC(2, 1);
		Result = 0;
	}
	break;
	case 27: //SLV Navigation Update Calculation
	{
		if (svtarget == NULL)
		{
			Result = 0;
			break;
		}

		IU *iu;
		bool isSaturnV;

		if (!stricmp(svtarget->GetClassName(), "ProjectApollo\\Saturn5") ||
			!stricmp(svtarget->GetClassName(), "ProjectApollo/Saturn5"))
		{
			Saturn *iuv = (Saturn *)svtarget;
			iu = iuv->GetIU();
			isSaturnV = true;
		}
		else if (!stricmp(svtarget->GetClassName(), "ProjectApollo\\Saturn1b") ||
			!stricmp(svtarget->GetClassName(), "ProjectApollo/Saturn1b"))
		{
			Saturn *iuv = (Saturn *)svtarget;
			iu = iuv->GetIU();
			isSaturnV = false;
		}
		else if (!stricmp(svtarget->GetClassName(), "ProjectApollo\\sat5stg3") ||
			!stricmp(svtarget->GetClassName(), "ProjectApollo/sat5stg3"))
		{
			SIVB *iuv = (SIVB *)svtarget;
			iu = iuv->GetIU();
			isSaturnV = true;
		}
		else if (!stricmp(svtarget->GetClassName(), "ProjectApollo\\nsat1stg2") ||
			!stricmp(svtarget->GetClassName(), "ProjectApollo/nsat1stg2"))
		{
			SIVB *iuv = (SIVB *)svtarget;
			iu = iuv->GetIU();
			isSaturnV = false;
		}
		else
		{
			Result = 0;
			break;
		}

		double A_Z, T_GRR;

		if (isSaturnV)
		{
			LVDCSV* lvdc = (LVDCSV*)iu->GetLVDC();

			GC->rtcc->MDVSTP.PHIL = lvdc->PHI;
			A_Z = lvdc->Azimuth;
			T_GRR = lvdc->T_L;
		}
		else
		{
			LVDC1B* lvdc = (LVDC1B*)iu->GetLVDC();

			GC->rtcc->MDVSTP.PHIL = lvdc->PHI;
			A_Z = lvdc->Azimuth;
			T_GRR = lvdc->T_GRR;
		}

		char Buffer[64], Buffer2[64];
		OrbMech::format_time_HHMMSS(Buffer, T_GRR);
		sprintf_s(Buffer2, "P12,IU1,%s,%.4lf;", Buffer, A_Z*DEG);
		GC->rtcc->GMGMED(Buffer2);

		EphemerisData sv = GC->rtcc->StateVectorCalcEphem(svtarget);
		EphemerisData sv2 = GC->rtcc->coast(sv,  GC->rtcc->GMTfromGET(SVDesiredGET) - sv.GMT);

		GC->rtcc->CMMSLVNAV(sv2.R, sv2.V, sv2.GMT);

		Result = 0;
	}
	break;
	case 28: //SLV Navigation Update Uplink
	{
		if (GC->rtcc->CZNAVSLV.NUPTIM == 0.0)
		{
			Result = 0;
			break;
		}

		IU *iu;

		if (!stricmp(svtarget->GetClassName(), "ProjectApollo\\Saturn5") ||
			!stricmp(svtarget->GetClassName(), "ProjectApollo/Saturn5"))
		{
			Saturn *iuv = (Saturn *)svtarget;
			iu = iuv->GetIU();
		}
		else if (!stricmp(svtarget->GetClassName(), "ProjectApollo\\Saturn1b") ||
			!stricmp(svtarget->GetClassName(), "ProjectApollo/Saturn1b"))
		{
			Saturn *iuv = (Saturn *)svtarget;
			iu = iuv->GetIU();
		}
		else if (!stricmp(svtarget->GetClassName(), "ProjectApollo\\sat5stg3") ||
			!stricmp(svtarget->GetClassName(), "ProjectApollo/sat5stg3"))
		{
			SIVB *iuv = (SIVB *)svtarget;
			iu = iuv->GetIU();
		}
		else if (!stricmp(svtarget->GetClassName(), "ProjectApollo\\nsat1stg2") ||
			!stricmp(svtarget->GetClassName(), "ProjectApollo/nsat1stg2"))
		{
			SIVB *iuv = (SIVB *)svtarget;
			iu = iuv->GetIU();
		}
		else
		{
			Result = 0;
			break;
		}

		void *uplink = NULL;
		DCSSLVNAVUPDATE upl;

		upl.PosS = GC->rtcc->CZNAVSLV.PosS;
		upl.DotS = GC->rtcc->CZNAVSLV.DotS;
		upl.NUPTIM = GC->rtcc->CZNAVSLV.NUPTIM;

		uplink = &upl;
		bool uplinkaccepted = iu->DCSUplink(DCSUPLINK_SLV_NAVIGATION_UPDATE, uplink);

		Result = 0;
	}
	break;
	case 29: //FIDO Space Digitals Cycle
	{
		GC->rtcc->EMDSPACE(1);
		Result = 0;
	}
	break;
	case 30: //FIDO Space Digitals MSK Request
	{
		GC->rtcc->EMDSPACE(6);
		Result = 0;
	}
	break;
	case 31: //Entry PAD
	{
		OBJHANDLE hEarth;
		double mu;

		hEarth = oapiGetObjectByName("Earth");
		mu = GGRAV * oapiGetMass(hEarth);

		if (entrypadopt == 0)
		{
			EarthEntryPADOpt opt;

			opt.dV_LVLH = Entry_DV;
			opt.GETbase = GC->rtcc->CalcGETBase();
			opt.P30TIG = EntryTIGcor;
			opt.REFSMMAT = GC->rtcc->EZJGMTX1.data[0].REFSMMAT;
			opt.sv0 = GC->rtcc->StateVectorCalc(vessel);

			if (EntryLatcor == 0)
			{
				opt.lat = 0;
				opt.lng = 0;
			}
			else
			{
				opt.lat = EntryLatcor;
				opt.lng = EntryLngcor;
			}

			VECTOR3 R, V;
			double apo, peri;
			OBJHANDLE gravref = GC->rtcc->AGCGravityRef(vessel);
			vessel->GetRelativePos(gravref, R);
			vessel->GetRelativeVel(gravref, V);
			OrbMech::periapo(R, V, mu, apo, peri);
			if (peri < oapiGetSize(gravref) + 50 * 1852.0)
			{
				opt.preburn = false;
				GC->rtcc->EarthOrbitEntry(opt, earthentrypad);
			}
			else
			{
				opt.preburn = true;
				GC->rtcc->EarthOrbitEntry(opt, earthentrypad);
			}
		}
		else
		{
			LunarEntryPADOpt opt;

			opt.dV_LVLH = dV_LVLH;
			opt.GETbase = GC->rtcc->CalcGETBase();

			if (EntryLatcor == 0)
			{
				//EntryPADLat = entry->EntryLatPred;
				//EntryPADLng = entry->EntryLngPred;
			}
			else
			{
				if (GC->MissionPlanningActive)
				{
					if (!GC->rtcc->NewMPTTrajectory(mptveh, opt.sv0))
					{
						opt.sv0 = GC->rtcc->StateVectorCalc(vessel);
					}
				}
				else
				{
					opt.sv0 = GC->rtcc->StateVectorCalc(vessel);
				}

				//EntryPADLat = EntryLatcor;
				//EntryPADLng = EntryLngcor;
				opt.lat = EntryLatcor;
				opt.lng = EntryLngcor;
				opt.P30TIG = P30TIG;
				opt.REFSMMAT = GC->rtcc->EZJGMTX1.data[0].REFSMMAT;

				GC->rtcc->LunarEntryPAD(&opt, lunarentrypad);
			}
		}

		Result = 0;
	}
	break;
	case 32: //Map Update
	{
		SV sv0;

		if (GC->MissionPlanningActive && GC->rtcc->MPTHasManeuvers(mptveh))
		{
			if (mapUpdateGET <= 0.0)
			{
				GC->rtcc->NewMPTTrajectory(mptveh, sv0);
			}
			else
			{
				EphemerisData sv;
				if (GC->rtcc->ELFECH(GC->rtcc->GMTfromGET(mapUpdateGET), mptveh, sv))
				{
					Result = 0;
					break;
				}
				sv0.R = sv.R;
				sv0.V = sv.V;
				sv0.MJD = OrbMech::MJDfromGET(sv.GMT, GC->rtcc->GetGMTBase());
				sv0.gravref = GC->rtcc->GetGravref(sv.RBI);
			}
		}
		else
		{
			sv0 = GC->rtcc->StateVectorCalc(vessel);
		}

		if (mappage == 0)
		{
			int gstat;
			double ttoGSAOS, ttoGSLOS;

			gstat = OrbMech::findNextAOS(sv0.R, sv0.V, sv0.MJD, sv0.gravref);

			OrbMech::groundstation(sv0.R, sv0.V, sv0.MJD, sv0.gravref, groundstations[gstat][0], groundstations[gstat][1], 1, ttoGSAOS);
			OrbMech::groundstation(sv0.R, sv0.V, sv0.MJD, sv0.gravref, groundstations[gstat][0], groundstations[gstat][1], 0, ttoGSLOS);
			GSAOSGET = (sv0.MJD - GC->rtcc->CalcGETBase())*24.0*3600.0 + ttoGSAOS;
			GSLOSGET = (sv0.MJD - GC->rtcc->CalcGETBase())*24.0*3600.0 + ttoGSLOS;
			mapgs = gstat;
		}
		else
		{
			GC->rtcc->LunarOrbitMapUpdate(sv0, GC->rtcc->CalcGETBase(), mapupdate);
		}

		Result = 0;
	}
	break;
	case 33: //Landmark Tracking PAD
	{
		LMARKTRKPADOpt opt;
		SV sv0;

		if (GC->MissionPlanningActive && GC->rtcc->MPTHasManeuvers(mptveh))
		{
			if (LmkTime <= 0.0)
			{
				GC->rtcc->NewMPTTrajectory(mptveh, sv0);
			}
			else
			{
				EphemerisData sv;
				if (GC->rtcc->ELFECH(GC->rtcc->GMTfromGET(LmkTime), mptveh, sv))
				{
					Result = 0;
					break;
				}
				sv0.R = sv.R;
				sv0.V = sv.V;
				sv0.MJD = OrbMech::MJDfromGET(sv.GMT, GC->rtcc->GetGMTBase());
				sv0.gravref = GC->rtcc->GetGravref(sv.RBI);
			}
		}
		else
		{
			sv0 = GC->rtcc->StateVectorCalc(vessel);
		}

		opt.GETbase = GC->rtcc->CalcGETBase();
		opt.lat[0] = LmkLat;
		opt.LmkTime[0] = LmkTime;
		opt.lng[0] = LmkLng;
		opt.sv0 = sv0;
		opt.entries = 1;

		GC->rtcc->LandmarkTrackingPAD(&opt, landmarkpad);

		Result = 0;
	}
	break;
	case 34: //Spare
	{
		Result = 0;
	}
	break;
	case 35: //Spare
	{
		Result = 0;
	}
	break;
	case 36: //Next Station Contacts Display
	{
		double GET = OrbMech::GETfromMJD(oapiGetSimMJD(), GC->rtcc->CalcGETBase());
		GC->rtcc->EMDSTAC();

		Result = 0;
	}
	break;
	case 37: //Spare
	{
		Result = 0;
	}
	break;
	case 38: //Transfer Two-Impulse Solution to MPT
	{
		TwoImpulseOpt opt;
		TwoImpulseResuls res;

		if (GC->MissionPlanningActive)
		{
			opt.mode = 4;
			opt.SingSolNum = GC->rtcc->med_m72.Plan;
			opt.SingSolTable = GC->rtcc->med_m72.Table;
			GC->rtcc->PMSTICN(opt, res);
		}
		else
		{
			if (GC->rtcc->med_m72.Table == 1)
			{
				if (GC->rtcc->med_m72.Plan > GC->rtcc->PZTIPREG.Solutions)
				{
					Result = 0;
					break;
				}
				opt.sv_A = GC->rtcc->PZMYSAVE.SV_mult[0];
				opt.sv_P = GC->rtcc->PZMYSAVE.SV_mult[1];
				opt.DH = GC->rtcc->GZGENCSN.TIDeltaH;
				opt.PhaseAngle = GC->rtcc->GZGENCSN.TIPhaseAngle;
				opt.T1 = GC->rtcc->PZTIPREG.data[GC->rtcc->med_m72.Plan - 1].Time1;
				opt.T2 = GC->rtcc->PZTIPREG.data[GC->rtcc->med_m72.Plan - 1].Time2;
			}
			else
			{
				//TBD
				Result = 0;
				break;
			}

			opt.mode = 5;
			GC->rtcc->PMSTICN(opt, res);

			double attachedMass = 0.0;
			SV sv_now = GC->rtcc->StateVectorCalc(vessel);

			if (docked)
			{
				attachedMass = GC->rtcc->GetDockedVesselMass(vessel);
			}
			else
			{
				attachedMass = 0.0;
			}

			GC->rtcc->PoweredFlightProcessor(sv_now, GC->rtcc->CalcGETBase(), res.T1, GC->rtcc->med_m72.Thruster, 0.0, res.dV, false, P30TIG, dV_LVLH);
		}

		Result = 0;
	}
	break;
	case 39: //Transfer SPQ to MPT
	{
		if (GC->MissionPlanningActive)
		{
			std::vector<std::string> str;
			GC->rtcc->PMMMED("70", str);
		}
		else
		{
			SV sv_pre, sv_post, sv_tig;
			double attachedMass = 0.0;

			SV sv_now = GC->rtcc->StateVectorCalc(vessel);
			sv_tig = GC->rtcc->coast(sv_now, SPQTIG - OrbMech::GETfromMJD(sv_now.MJD, GC->rtcc->CalcGETBase()));

			if (docked)
			{
				attachedMass = GC->rtcc->GetDockedVesselMass(vessel);
			}
			else
			{
				attachedMass = 0.0;
			}
			GC->rtcc->PoweredFlightProcessor(sv_tig, GC->rtcc->CalcGETBase(), SPQTIG, GC->rtcc->med_m70.Thruster, 0.0, SPQDeltaV, true, P30TIG, dV_LVLH, sv_pre, sv_post);
		}

		Result = 0;
	}
	break;
	case 40: //Transfer DKI to MPT
	{
		if (GC->MissionPlanningActive)
		{
			std::vector<std::string> str;
			GC->rtcc->PMMMED("70", str);
		}
		else
		{
			SV sv_pre, sv_post, sv_tig;
			double attachedMass = 0.0;

			SV sv_now = GC->rtcc->StateVectorCalc(vessel);
			sv_tig = GC->rtcc->coast(sv_now, DKI_TIG - OrbMech::GETfromMJD(sv_now.MJD, GC->rtcc->CalcGETBase()));

			if (docked)
			{
				attachedMass = GC->rtcc->GetDockedVesselMass(vessel);
			}
			else
			{
				attachedMass = 0.0;
			}

			GC->rtcc->PoweredFlightProcessor(sv_tig, GC->rtcc->CalcGETBase(), DKI_TIG, GC->rtcc->med_m70.Thruster, 0.0, DKI_DV, true, P30TIG, dV_LVLH, sv_pre, sv_post);
		}

		Result = 0;
	}
	break;
	case 41: //Direct Input to the MPT
	{
		//Dummy data
		std::vector<std::string> str;
		GC->rtcc->PMMMED("66", str);

		Result = 0;
	}
	break;
	case 42: //Transfer Descent Plan to MPT
	{
		if (GC->MissionPlanningActive)
		{
			std::vector<std::string> str;
			GC->rtcc->PMMMED("70", str);
		}
		else
		{
			SV sv_pre, sv_post, sv_tig;
			double attachedMass = 0.0;

			SV sv_now = GC->rtcc->StateVectorCalc(vessel);
			sv_tig = GC->rtcc->coast(sv_now, GC->descplantable.GETIG[0] - OrbMech::GETfromMJD(sv_now.MJD, GC->rtcc->CalcGETBase()));

			if (docked)
			{
				attachedMass = GC->rtcc->GetDockedVesselMass(vessel);
			}

			GC->rtcc->PoweredFlightProcessor(sv_tig, GC->rtcc->CalcGETBase(), GC->descplantable.GETIG[0], GC->rtcc->med_m70.Thruster, attachedMass, GC->descplantable.DVVector[0] * 0.3048, true, P30TIG, dV_LVLH, sv_pre, sv_post);
		}

		Result = 0;
	}
	break;
	case 43: //Direct Input of Lunar Descent Maneuver
	{
		if (GC->MissionPlanningActive)
		{
			//Temporary
			GC->rtcc->med_m86.Time = GC->t_Land;

			std::vector<std::string> str;
			GC->rtcc->PMMMED("86", str);
		}

		Result = 0;
	}
	break;
	case 44: //Transfer ascent maneuver to MPT from lunar targeting
	{
		if (GC->MissionPlanningActive)
		{
			std::vector<std::string> str;
			GC->rtcc->PMMMED("85", str);
		}

		Result = 0;
	}
	break;
	case 45: //Transfer GPM to the MPT
	{
		if (GC->MissionPlanningActive)
		{
			std::vector<std::string> str;
			GC->rtcc->PMMMED("65", str);
		}
		else
		{
			SV sv_pre, sv_post, sv_tig;
			double attachedMass = 0.0;
			SV sv_now = GC->rtcc->StateVectorCalc(vessel);
			sv_tig = GC->rtcc->coast(sv_now, GPM_TIG - OrbMech::GETfromMJD(sv_now.MJD, GC->rtcc->CalcGETBase()));

			if (docked)
			{
				attachedMass = GC->rtcc->GetDockedVesselMass(vessel);
			}
			else
			{
				attachedMass = 0.0;
			}
			GC->rtcc->PoweredFlightProcessor(sv_tig, GC->rtcc->CalcGETBase(), GPM_TIG, GC->rtcc->med_m65.Thruster, attachedMass, OrbAdjDVX, true, P30TIG, dV_LVLH, sv_pre, sv_post);
		}

		Result = 0;
	}
	break;
	case 46: //TLI Direct Input
	{
		if (!GC->MissionPlanningActive)
		{
			Result = 0;
			break;
		}
		if (g_Data.progVessel->GetStage() >= CSM_LEM_STAGE)
		{
			Result = 0;
			break;
		}

		SaturnV *SatV = (SaturnV*)g_Data.progVessel;
		LVDCSV *lvdc = (LVDCSV*)SatV->iu->GetLVDC();

		GC->rtcc->MDVSTP.T4IG = lvdc->t_3i - 17.0;
		GC->rtcc->MDVSTP.T4C = lvdc->TB5 - 17.0;
		GC->rtcc->MDVSTP.DT4N = lvdc->T_4N;
		GC->rtcc->MDVSTP.KP1 = lvdc->K_P1;
		GC->rtcc->MDVSTP.KP2 = lvdc->K_P2;
		GC->rtcc->MDVSTP.KY1 = lvdc->K_Y1;
		GC->rtcc->MDVSTP.KY2 = lvdc->K_Y2;
		GC->rtcc->MDVSTP.PHIL = lvdc->PHI;
		GC->rtcc->MDVSTP.t_D0 = lvdc->t_D0;
		GC->rtcc->MDVSTP.t_D1 = lvdc->t_D1; 
		GC->rtcc->MDVSTP.t_D2 = lvdc->t_D2; 
		GC->rtcc->MDVSTP.t_D3 = lvdc->t_D3;
		GC->rtcc->MDVSTP.t_DS0 = lvdc->t_DS0;
		GC->rtcc->MDVSTP.t_DS1 = lvdc->t_DS1;
		GC->rtcc->MDVSTP.t_DS2 = lvdc->t_DS2;
		GC->rtcc->MDVSTP.t_DS3 = lvdc->t_DS3;
		GC->rtcc->MDVSTP.t_SD1 = lvdc->t_SD1;
		GC->rtcc->MDVSTP.t_SD2 = lvdc->t_SD2;
		GC->rtcc->MDVSTP.t_SD3 = lvdc->t_SD3;

		int i, j;
		for (i = 0;i < 3;i++)
		{
			for (j = 0;j < 5;j++)
			{
				GC->rtcc->MDVSTP.hx[i][j] = lvdc->hx[i][j];
			}
		}
		for (i = 0;i < 7;i++)
		{
			GC->rtcc->MDVSTP.fx[i] = lvdc->fx[i];
			GC->rtcc->MDVSTP.gx[i] = lvdc->gx[i];
		}

		GC->rtcc->PZSTARGP.T_LO = lvdc->T_LO;
		GC->rtcc->PZSTARGP.theta_EO = lvdc->theta_EO;
		GC->rtcc->PZSTARGP.omega_E = lvdc->omega_E;
		GC->rtcc->PZSTARGP.K_T3 = lvdc->K_T3;

		for (i = 0;i < 2;i++)
		{
			GC->rtcc->PZSTARGP.T_ST[i] = lvdc->TABLE15[i].T_ST;
			GC->rtcc->PZSTARGP.beta[i] = lvdc->TABLE15[i].beta*RAD;
			GC->rtcc->PZSTARGP.alpha_TS[i] = lvdc->TABLE15[i].alphaS_TS*RAD;
			GC->rtcc->PZSTARGP.f[i] = lvdc->TABLE15[i].f*RAD;
			GC->rtcc->PZSTARGP.R_N[i] = lvdc->TABLE15[i].R_N;
			GC->rtcc->PZSTARGP.T3_apo[i] = lvdc->TABLE15[i].T3PR;
			GC->rtcc->PZSTARGP.tau3R[i] = lvdc->TABLE15[i].TAU3R;
			GC->rtcc->PZSTARGP.T2[i] = lvdc->TABLE15[i].T2IR;
			GC->rtcc->PZSTARGP.DV_BR[i] = lvdc->TABLE15[i].dV_BR;

			//TBD: LVDC needs to have separate values for these for the two restart opportunities
			GC->rtcc->PZSTARGP.Vex2[i] = lvdc->V_ex2R;
			GC->rtcc->PZSTARGP.Mdot2[i] = lvdc->dotM_2R;
			GC->rtcc->PZSTARGP.tau2N[i] = lvdc->tau2N;
			GC->rtcc->PZSTARGP.KP0[i] = 0.0;
			GC->rtcc->PZSTARGP.KY0[i] = 0.0;

			for (j = 0;j < 15;j++)
			{
				GC->rtcc->PZSTARGP.cos_sigma[i][j] = lvdc->TABLE15[i].target[j].cos_sigma;
				GC->rtcc->PZSTARGP.C_3[i][j] = lvdc->TABLE15[i].target[j].C_3;
				GC->rtcc->PZSTARGP.e_N[i][j] = lvdc->TABLE15[i].target[j].e_N;
				GC->rtcc->PZSTARGP.RA[i][j] = lvdc->TABLE15[i].target[j].RAS*RAD;
				GC->rtcc->PZSTARGP.DEC[i][j] = lvdc->TABLE15[i].target[j].DEC*RAD;
				GC->rtcc->PZSTARGP.t_D[i][j] = lvdc->TABLE15[i].target[j].t_D;
			}
		}

		//MED string was previously saved
		GC->rtcc->GMGMED(GC->rtcc->RTCCMEDBUFFER);

		Result = 0;
	}
	break;
	case 47: //Spare
	{
		Result = 0;
	}
	break;
	case 48: //LOI and MCC Transfer
	{
		if (GC->MissionPlanningActive)
		{
			std::vector<std::string> data;
			GC->rtcc->PMMMED("78", data);
		}
		else
		{
			VECTOR3 dv;
			double tig;

			if (GC->rtcc->med_m78.Type)
			{
				if (GC->rtcc->med_m78.ManeuverNumber < 1 || GC->rtcc->med_m78.ManeuverNumber > 8)
				{
					Result = 0;
					break;
				}
				tig = GC->rtcc->GETfromGMT(GC->rtcc->PZLRBELM.sv_man_bef[GC->rtcc->med_m78.ManeuverNumber - 1].GMT);
				dv = GC->rtcc->PZLRBELM.V_man_after[GC->rtcc->med_m78.ManeuverNumber - 1] - GC->rtcc->PZLRBELM.sv_man_bef[GC->rtcc->med_m78.ManeuverNumber - 1].V;
			}
			else
			{
				if (GC->rtcc->med_m78.ManeuverNumber < 1 || GC->rtcc->med_m78.ManeuverNumber > 4)
				{
					Result = 0;
					break;
				}
				tig = GC->rtcc->GETfromGMT(GC->rtcc->PZMCCXFR.sv_man_bef[GC->rtcc->med_m78.ManeuverNumber - 1].GMT);
				dv = GC->rtcc->PZMCCXFR.V_man_after[GC->rtcc->med_m78.ManeuverNumber - 1] - GC->rtcc->PZMCCXFR.sv_man_bef[GC->rtcc->med_m78.ManeuverNumber - 1].V;
			}

			SV sv_pre, sv_post, sv_tig;
			double attachedMass = 0.0;
			SV sv_now = GC->rtcc->StateVectorCalc(vessel);
			sv_tig = GC->rtcc->coast(sv_now, tig - OrbMech::GETfromMJD(sv_now.MJD, GC->rtcc->CalcGETBase()));

			if (docked)
			{
				attachedMass = GC->rtcc->GetDockedVesselMass(vessel);
			}
			else
			{
				attachedMass = 0.0;
			}
			GC->rtcc->PoweredFlightProcessor(sv_tig, GC->rtcc->CalcGETBase(), tig, GC->rtcc->med_m78.Thruster, attachedMass, dv, false, P30TIG, dV_LVLH, sv_pre, sv_post);
		}

		Result = 0;
	}
	break;
	case 49: //Transfer Maneuver to MPT from TTF, SCS, RTE
	{
		GC->rtcc->GMGMED(GC->rtcc->RTCCMEDBUFFER);

		Result = 0;
	}
	break;
	case 50: //Spare
	{
		Result = 0;
	}
	break;

	case 51: //Moonrise/Moonset Display
	{
		Result = 0;
	}
	break;
	case 52: //RTE Tradeoff Display
	{
		std::string mode;

		if (RTETradeoffMode == 0)
		{
			mode = "70";
		}
		else
		{
			mode = "71";
		}

		GC->rtcc->PMQAFMED(mode);

		Result = 0;
	}
	break;
	case 53: //Central Manual Entry Device Decoder
	{
		GC->rtcc->GMGMED(GC->rtcc->RTCCMEDBUFFER);

		Result = 0;
	}
	break;
	}

	subThreadStatus = Result;
	if (hThread != NULL) { CloseHandle(hThread); }

	return(0);
}

void ARCore::StartIMFDRequest() {

	g_Data.isRequesting = true;
	if (!g_Data.progVessel->GetIMFDClient()->IsBurnDataRequesting())
		g_Data.progVessel->GetIMFDClient()->StartBurnDataRequests();
}

void ARCore::StopIMFDRequest() {

	g_Data.isRequesting = false;
		g_Data.progVessel->GetIMFDClient()->StopBurnDataRequests();
}

void ARCore::DetermineGMPCode()
{
	int code = 0;

	if (GMPManeuverType == 0)
	{
		if (GMPManeuverPoint == 1)
		{
			//PCE: Plane Change at equatorial crossing
			code = 1;
		}
		else if (GMPManeuverPoint == 3)
		{
			//PCL: Plane change at a specified longitude
			code = 2;
		}
		else if (GMPManeuverPoint == 4)
		{
			//PCH: Plane change at a height
			code = 49;
		}
		else if (GMPManeuverPoint == 5)
		{
			//PCT: Plane change at a specified time
			code = 3;
		}
	}
	else if (GMPManeuverType == 1)
	{
		if (GMPManeuverPoint == 0)
		{
			//CRA: Circularization maneuver at apogee
			code = 41;
		}
		else if (GMPManeuverPoint == 2)
		{
			//CRA: Circularization maneuver at perigee
			code = 42;
		}
		else if (GMPManeuverPoint == 3)
		{
			//CRL: Circularization maneuver at a specified longitude
			code = 4;
		}
		else if (GMPManeuverPoint == 4)
		{
			//CRH: Circularization maneuver at a specified height
			code = 5;
		}
		else if (GMPManeuverPoint == 5)
		{
			//CRT: Circularization maneuver at a specified time
			code = 40;
		}
	}
	else if (GMPManeuverType == 2)
	{
		if (GMPManeuverPoint == 0)
		{
			//HAO: Height maneuver at apogee
			code = 8;
		}
		else if (GMPManeuverPoint == 2)
		{
			//HPO: Height maneuver at perigee
			code = 9;
		}
		else if (GMPManeuverPoint == 3)
		{
			//HOL: Height maneuver at a specified longitude
			code = 6;
		}
		else if (GMPManeuverPoint == 4)
		{
			//HOH: Height maneuver at a height
			code = 52;
		}
		else if (GMPManeuverPoint == 5)
		{
			//HOL: Height maneuver at a specified time
			code = 7;
		}
	}
	else if (GMPManeuverType == 3)
	{
		if (GMPManeuverPoint == 4)
		{
			//NSH: Node shift maneuver at a height
			code = 50;
		}
		else if (GMPManeuverPoint == 3)
		{
			//NSL: Node shift maneuver at a longitude
			code = 51;
		}
		else if (GMPManeuverPoint == 5)
		{
			//NST: Node shift maneuver at a specified time
			code = 10;
		}
		else if (GMPManeuverPoint == 6)
		{
			//NSO: Optimum node shift maneuver
			code = 11;
		}
	}
	else if (GMPManeuverType == 4)
	{
		if (GMPManeuverPoint == 3)
		{
			//HBL: Maneuver to change both apogee and perigee at a specified longitude
			code = 33;
		}
		else if (GMPManeuverPoint == 4)
		{
			//HBH: Maneuver to change both apogee and perigee at a specified height
			code = 13;
		}
		else if (GMPManeuverPoint == 5)
		{
			//HBT: Maneuver to change both apogee and perigee at a specified time
			code = 12;
		}
		else if (GMPManeuverPoint == 6)
		{
			//HBO: Optimum maneuver to change both apogee and perigee
			code = 14;
		}
	}
	else if (GMPManeuverType == 5)
	{
		if (GMPManeuverPoint == 0)
		{
			//FCA: Input maneuver at an apogee
			code = 18;
		}
		else if (GMPManeuverPoint == 1)
		{
			//FCE: Input maneuver at an equatorial crossing
			code = 20;
		}
		else if (GMPManeuverPoint == 2)
		{
			//FCP: Input maneuver at an perigee
			code = 19;
		}
		else if (GMPManeuverPoint == 3)
		{
			//FCL: Input maneuver at a specified longitude
			code = 16;
		}
		else if (GMPManeuverPoint == 4)
		{
			//FCH: Input maneuver at a specified height
			code = 17;
		}
		else if (GMPManeuverPoint == 5)
		{
			//FCT: Input maneuver at a specified time
			code = 15;
		}
	}
	else if (GMPManeuverType == 6)
	{
		if (GMPManeuverPoint == 3)
		{
			//NHL: Combination maneuver to change both apogee and perigee and shift the node at a specified longitude
			code = 22;
		}
		else if (GMPManeuverPoint == 5)
		{
			//NHT: Combination maneuver to change both apogee and perigee and shift the node at a specified time
			code = 21;
		}
	}
	else if (GMPManeuverType == 7)
	{
		if (GMPManeuverPoint == 0)
		{
			code = RTCC_GMP_SAA;
		}
		else if (GMPManeuverPoint == 3)
		{
			code = RTCC_GMP_SAL;
		}
		else if (GMPManeuverPoint == 5)
		{
			//SAT: Maneuver to shift line-of-apsides some angle at a specified time
			code = 31;
		}
		else if (GMPManeuverPoint == 6)
		{
			//SAO: Maneuver to shift line-of-apsides some angle and keep the same apogee and perigee altitudes
			code = 32;
		}
	}
	else if (GMPManeuverType == 8)
	{
		if (GMPManeuverPoint == 0)
		{
			//PHA: Combination height maneuver and a plane change at an apogee
			code = 27;
		}
		else if (GMPManeuverPoint == 2)
		{
			//PHP: Combination height maneuver and a plane change at an perigee
			code = 28;
		}
		else if (GMPManeuverPoint == 3)
		{
			//PHL: Combination height maneuver and a plane change at a specified longitude
			code = 25;
		}
		else if (GMPManeuverPoint == 5)
		{
			//PHT: Combination height maneuver and a plane change at a specified time
			code = 26;
		}
	}
	else if (GMPManeuverType == 9)
	{
		if (GMPManeuverPoint == 0)
		{
			//CPA: Combination circularization maneuver and a plane change at apogee
			code = 44;
		}
		else if (GMPManeuverPoint == 2)
		{
			//CPP: Combination circularization maneuver and a plane change at perigee
			code = 45;
		}
		else if (GMPManeuverPoint == 3)
		{
			//CPL: Combination circularization maneuver and a plane change at a specified longitude
			code = 29;
		}
		else if (GMPManeuverPoint == 4)
		{
			//CPH: Combination circularization maneuver and a plane change at a specified altitude
			code = 30;
		}
		else if (GMPManeuverPoint == 5)
		{
			//CPT: Combination circularization maneuver and a plane change at a specified time
			code = 43;
		}
	}
	else if (GMPManeuverType == 10)
	{
		if (GMPManeuverPoint == 0)
		{
			//CNA: Circularization and node shift at apogee
			code = 47;
		}
		else if (GMPManeuverPoint == 2)
		{
			//CNP: Circularization and node shift at perigee
			code = 48;
		}
		else if (GMPManeuverPoint == 3)
		{
			//CNL: Circularization and node shift at a specified longitude
			code = 34;
		}
		else if (GMPManeuverPoint == 4)
		{
			//CNH: Circularization and node shift at a specified height
			code = 35;
		}
		else if (GMPManeuverPoint == 5)
		{
			//CNT: Circularization and node shift maneuver at a specified time
			code = 46;
		}
	}
	else if (GMPManeuverType == 11)
	{
		if (GMPManeuverPoint == 0)
		{
			//HNA: Height maneuver and node shift at apogee
			code = 38;
		}
		else if (GMPManeuverPoint == 2)
		{
			//HNP: Height maneuver and node shift at perigee
			code = 39;
		}
		else if (GMPManeuverPoint == 3)
		{
			//HNL: Height maneuver and node shift at a specified longitude
			code = 36;
		}
		else if (GMPManeuverPoint == 5)
		{
			//HNT: Height maneuver and node shift at a specified time
			code = 37;
		}
	}
	else if (GMPManeuverType == 12)
	{
		if (GMPManeuverPoint == 6)
		{
			//HAS
			code = RTCC_GMP_HAS;
		}
	}

	GMPManeuverCode = code;
}

void ARCore::AGCCorrectionVectors(double mjd_launch, double t_land, int mission, bool isCMC)
{
	OBJHANDLE hEarth, hMoon;
	MATRIX3 R, Rot2, J2000, R2, R3, M, M_AGC;
	VECTOR3 l;
	double mjd_mid, brcsmjd, w_E, t0, B_0, Omega_I0, F_0, B_dot, Omega_I_dot, F_dot, cosI, sinI;
	double A_Z, A_Z0, A_X, minA_Y, mjd_land;
	int mem;
	char AGC[64];

	mjd_mid = mjd_launch + 7.0;
	mjd_land = mjd_launch + t_land / 24.0 / 3600.0;
	hEarth = oapiGetObjectByName("Earth");
	hMoon = oapiGetObjectByName("Moon");

	Rot2 = _M(1., 0., 0., 0., 0., 1., 0., 1., 0.);
	R = OrbMech::GetRotationMatrix(BODY_EARTH, mjd_mid);

	if (isCMC)
	{
		if (mission < 11)
		{
			brcsmjd = 40221.525;    //Nearest Besselian Year 1969
			w_E = 7.29211515e-5;
			B_0 = 0.409164173;              
			Omega_I0 = -6.03249419;
			F_0 = 2.61379488;
			B_dot = -7.19756666e-14;
			Omega_I_dot = -1.07047016e-8;
			F_dot = 2.67240019e-6;
			cosI = 0.99964115;
			sinI = 0.02678760;
			t0 = 40038;
			if (mission < 9)
			{
				sprintf(AGC, "Colossus237");
			}
			else if (mission < 10)
			{
				sprintf(AGC, "Colossus249");
			}
			else
			{
				sprintf(AGC, "Comanche044");
			}
		}
		else if (mission < 14)
		{
			brcsmjd = 40586.767239; //Nearest Besselian Year 1970
			w_E = 7.29211494e-5;    //Comanche 055 (Apollo 11 CM AGC)
			B_0 = 0.40916190299;
			Omega_I0 = 6.19653663041;
			F_0 = 5.20932947829;
			B_dot = -7.19757301e-14;
			Omega_I_dot = -1.07047011e-8;
			F_dot = 2.67240410e-6;
			cosI = 0.99964173;
			sinI = 0.02676579;
			t0 = 40403;
			sprintf(AGC, "Comanche055");
		}
		else if (mission < 15)
		{
			brcsmjd = 40952.009432; //Nearest Besselian Year 1971
			w_E = 7.292115147e-5;	//Comanche 108 (Apollo 14 CM AGC)
			B_0 = 0.40915963316;
			Omega_I0 = 5.859196887;
			F_0 = 1.5216749598;
			B_dot = -7.1975797907e-14;
			Omega_I_dot = -1.070470151e-8;
			F_dot = 2.6724042552e-6;
			cosI = 0.999641732;
			sinI = 0.0267657905;
			t0 = 40768;
			sprintf(AGC, "Comanche108");
		}
		else
		{
			brcsmjd = 41317.251625; //Nearest Besselian Year 1972
			w_E = 7.29211514667e-5; //Artemis 072 (Apollo 15 CM AGC)
			B_0 = 0.409157363336;
			Omega_I0 = 5.52185714700;
			F_0 = 4.11720655556;
			B_dot = -7.19758599677e-14;
			Omega_I_dot = -1.07047013100e-8;
			F_dot = 2.67240425480e-6;
			cosI = 0.999641732;
			sinI = 0.0267657905;
			t0 = 41133;
			sprintf(AGC, "Artemis072");
		}
	}
	else
	{
		if (mission < 11)
		{
			brcsmjd = 40221.525;    //Nearest Besselian Year 1969
			w_E = 7.29211515e-5;    //Luminary 069 (Apollo 10 LM AGC)
			B_0 = 0.409164173;
			Omega_I0 = -6.03249419;
			F_0 = 2.61379488;
			B_dot = -7.19756666e-14;
			Omega_I_dot = -1.07047016e-8;
			F_dot = 2.67240019e-6;
			cosI = 0.99964115;
			sinI = 0.02678760;
			t0 = 40038;
			sprintf(AGC, "Luminary069");
		}
		else if (mission < 12)
		{
			brcsmjd = 40586.767239;		//Nearest Besselian Year 1970
			w_E = 7.29211319606104e-5;  //Luminary 099 (Apollo 11 LM AGC)
			B_0 = 0.40916190299;
			Omega_I0 = 6.1965366255107;
			F_0 = 5.20932947411685;
			B_dot = -7.19757301e-14;
			Omega_I_dot = -1.07047011e-8;
			F_dot = 2.67240410e-6;
			cosI = 0.99964173;
			sinI = 0.02676579;
			t0 = 40403;
			sprintf(AGC, "Luminary099");
		}
		else if (mission < 13)
		{
			brcsmjd = 40586.767239; //Nearest Besselian Year 1970
			w_E = 7.29211494e-5;    //Luminary 116 (Apollo 12 LM AGC)
			B_0 = 0.4091619030;
			Omega_I0 = 6.196536640;
			F_0 = 5.209327056;
			B_dot = -7.197573418e-14;
			Omega_I_dot = -1.070470170e-8;
			F_dot = 2.672404256e-6;
			cosI = 0.9996417320;
			sinI = 0.02676579050;
			t0 = 40403;
			sprintf(AGC, "Luminary116");
		}
		else if (mission < 14)
		{
			brcsmjd = 40586.767239;		//Nearest Besselian Year 1970
			w_E = 7.292115145489943e-05;//Luminary 131 (Apollo 13 LM AGC)
			B_0 = 0.4091619030;
			Omega_I0 = 6.196536640;
			F_0 = 5.209327056;
			B_dot = -7.197573418e-14;
			Omega_I_dot = -1.070470170e-8;
			F_dot = 2.672404256e-6;
			cosI = 0.9996417320;
			sinI = 0.02676579050;
			t0 = 40403;
			sprintf(AGC, "Luminary131");
		}
		else if (mission < 15)
		{
			brcsmjd = 40952.009432; //Nearest Besselian Year 1971
			w_E = 7.292115147e-5;	//Luminary 178 (Apollo 14 LM AGC)
			B_0 = 0.40915963316;
			Omega_I0 = 5.859196887;
			F_0 = 1.5216749598;
			B_dot = -7.1975797907e-14;
			Omega_I_dot = -1.070470151e-8;
			F_dot = 2.6724042552e-6;
			cosI = 0.999641732;
			sinI = 0.0267657905;
			t0 = 40768;
			sprintf(AGC, "Luminary178");
		}
		else
		{
			brcsmjd = 41317.251625; //Nearest Besselian Year 1972
			w_E = 7.29211514667e-5; //Luminary 210 (Apollo 15 LM AGC)
			B_0 = 0.409157363336;
			Omega_I0 = 5.52185714700;
			F_0 = 4.11720655556;
			B_dot = -7.19758599677e-14;
			Omega_I_dot = -1.07047013100e-8;
			F_dot = 2.67240425480e-6;
			cosI = 0.999641732;
			sinI = 0.0267657905;
			t0 = 41133;
			sprintf(AGC, "Luminary210");
		}
	}

	//EARTH ROTATIONS
	J2000 = OrbMech::J2000EclToBRCS(brcsmjd);
	R2 = mul(OrbMech::tmat(Rot2), mul(R, Rot2));
	R3 = mul(J2000, R2);

	A_Z = atan2(R3.m21, R3.m11);
	if (mission < 14)
	{
		A_Z0 = fmod((A_Z - w_E * (mjd_mid - t0) * 24.0 * 3600.0), PI2);  //AZ0 for mission
		if (A_Z0 < 0) A_Z0 += PI2;
	}
	else if (mission < 15)
	{
		A_Z0 = 4.8631512705;   //Hardcoded in Comanche 108 and Luminary 178
	}
	else
	{
		A_Z0 = 4.85898502016;  //Hardcoded in Artemis 072 and Luminary 210
	}

	M = _M(cos(A_Z), sin(A_Z), 0., -sin(A_Z), cos(A_Z), 0., 0., 0., 1.);
	M_AGC = mul(R3, M);
	A_X = atan2(M_AGC.m32, M_AGC.m33);
	minA_Y = -atan2(-M_AGC.m31, sqrt(M_AGC.m32*M_AGC.m32 + M_AGC.m33*M_AGC.m33));
	l = _V(A_X, -minA_Y, 0.);

	mem = 01711;

	//TBD: Print stuff here
	FILE *file = fopen("PrecessionData.txt", "w");
	fprintf(file, "------- AGC Correction Vectors for Apollo %d using %s -------\n", mission, AGC);
	fprintf(file, "Epoch   = %6.6f (MJD) Epoch of Basic Reference Coordinate System\n", brcsmjd);
	fprintf(file, "TEphem0 = %6.6f (MJD) Ephemeris Time Zero\n", t0);
	fprintf(file, "TEPHEM  = %6.6f (MJD) Mission launch time\n", mjd_launch);
	fprintf(file, "MJDLAND = %6.6f (MJD) Mission lunar landing time\n", mjd_land);
	fprintf(file, "TIMEM0  = %6.6f (MJD) Mission mid-range time\n\n", mjd_mid);
	fprintf(file, "------- Earth Orientation -------\n");
	if (mission < 14)
	{
		fprintf(file, "AZO %.10f DEG\n", A_Z0*DEG);
	}
	fprintf(file, "-AYO %.10f DEG\n", minA_Y*DEG);
	fprintf(file, "AXO %.10f DEG\n", A_X*DEG);
	if (mission < 14)
	{
		fprintf(file, "EMEM%o %d\n", mem, OrbMech::DoubleToBuffer(A_Z0 / PI2, 0, 1)); mem++;
		fprintf(file, "EMEM%o %d\n", mem, OrbMech::DoubleToBuffer(A_Z0 / PI2, 0, 0)); mem++;
	}	
	fprintf(file, "EMEM%o %d\n", mem, OrbMech::DoubleToBuffer(minA_Y / PI2, 0, 1)); mem++;
	fprintf(file, "EMEM%o %d\n", mem, OrbMech::DoubleToBuffer(minA_Y / PI2, 0, 0)); mem++;
	fprintf(file, "EMEM%o %d\n", mem, OrbMech::DoubleToBuffer(A_X / PI2, 0, 1)); mem++;
	fprintf(file, "EMEM%o %d\n", mem, OrbMech::DoubleToBuffer(A_X / PI2, 0, 0)); mem++;

	//MOON ROTATIONS
	MATRIX3 M1, M2, M3, M4, MM, M_AGC_M, RM, R2M, R3M;
	VECTOR3 lm;
	double t_M, B, Omega_I, F;

	if (isCMC)
	{
		t_M = (mjd_mid - t0) * 24.0 * 3600.0;
		RM = OrbMech::GetRotationMatrix(BODY_MOON, mjd_mid);
	}
	else
	{
		t_M = (mjd_land - t0) * 24.0 * 3600.0;
		RM = OrbMech::GetRotationMatrix(BODY_MOON, mjd_land);
	}

	B = B_0 + B_dot * t_M;
	Omega_I = Omega_I0 + Omega_I_dot * t_M;
	F = F_0 + F_dot * t_M;
	M1 = _M(1., 0., 0., 0., cos(B), sin(B), 0., -sin(B), cos(B));
	M2 = _M(cos(Omega_I), sin(Omega_I), 0., -sin(Omega_I), cos(Omega_I), 0., 0., 0., 1.);
	M3 = _M(1., 0., 0., 0., cosI, -sinI, 0., sinI, cosI);
	M4 = _M(-cos(F), -sin(F), 0., sin(F), -cos(F), 0., 0., 0., 1.);
	MM = mul(M4, mul(M3, mul(M2, M1)));
	R2M = mul(OrbMech::tmat(Rot2), mul(RM, Rot2));
	R3M = mul(J2000, R2M);
	M_AGC_M = mul(MM, R3M);

	lm.x = atan2(M_AGC_M.m32, M_AGC_M.m33);
	lm.y = atan2(-M_AGC_M.m31, sqrt(M_AGC_M.m32*M_AGC_M.m32 + M_AGC_M.m33*M_AGC_M.m33));
	lm.z = atan2(M_AGC_M.m21, M_AGC_M.m11);

	if (isCMC)
	{
		mem = 02011;
	}
	else
	{
		mem = 02012;
	}
	fprintf(file, "------- Moon Orientation -------\n");
	fprintf(file, "504LM %.10f DEG\n", lm.x*DEG);
	fprintf(file, "504LM+2 %.10f DEG\n", lm.y*DEG);
	fprintf(file, "504LM+4 %.10f DEG\n", lm.z*DEG);
	fprintf(file, "EMEM%o %d\n", mem, OrbMech::DoubleToBuffer(lm.x, 0, 1)); mem++;
	fprintf(file, "EMEM%o %d\n", mem, OrbMech::DoubleToBuffer(lm.x, 0, 0)); mem++;
	fprintf(file, "EMEM%o %d\n", mem, OrbMech::DoubleToBuffer(lm.y, 0, 1)); mem++;
	fprintf(file, "EMEM%o %d\n", mem, OrbMech::DoubleToBuffer(lm.y, 0, 0)); mem++;
	fprintf(file, "EMEM%o %d\n", mem, OrbMech::DoubleToBuffer(lm.z, 0, 1)); mem++;
	fprintf(file, "EMEM%o %d\n", mem, OrbMech::DoubleToBuffer(lm.z, 0, 0)); mem++;

	if (file) fclose(file);
}